/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

 
#include "solvemodel.hxx"
#include "solver.hxx"
#include "global.hxx"
#include "lpbuilder.hxx"
#include "dialog.hxx"
#include "xcalc.hxx"
#include "numeric/lpmodel.hxx"
#include "numeric/type.hxx"
#include "numeric/lpbase.hxx"
#include "numeric/lpsimplex.hxx"

#ifdef ENABLE_SCSOLVER_UNO_ALGORITHM
#include "numeric/lpuno.hxx"
#endif

#include <memory>
#include <exception>

using namespace std;
using namespace scsolver::numeric::opres;
using scsolver::numeric::Matrix;
using com::sun::star::table::CellAddress;

namespace scsolver {

//---------------------------------------------------------------------------
// SolveModelImpl

class SolveModelImpl
{
public:

	class CellGeometriesDiffer : public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return "Cell geometries differ (internal)";
		}
	};

	SolveModelImpl( SolverImpl* );
	~SolveModelImpl() throw();
	void solve();
	bool isSolved() const;

private:
	SolverImpl* m_pSolverImpl;
	std::auto_ptr<LpModelBuilder> m_pBuilder;
	bool m_bSolved;
	Matrix m_mxSolution;
	
	SolverImpl* getSolverImpl() const { return m_pSolverImpl; }

	auto_ptr<lp::BaseAlgorithm> getLpAlgorithm() const;

	void parseConstraints();
	void resolveConstraintAddress();
	void resolveDecisionVarAddress();
	void resolveObjectiveFuncAddress();	

	void updateCells();

};

SolveModelImpl::SolveModelImpl( SolverImpl* p ) :
		m_pSolverImpl( p ), m_pBuilder( new LpModelBuilder() ), m_bSolved( false )
{
}

SolveModelImpl::~SolveModelImpl() throw()
{
}

/**
 * This method returns an algorithm object to use to solve a
 * given LP model.  While all this method does currently is
 * simply return the pre-selected algorithm, in future this
 * method may be used to pick a user-selected algorithm based on
 * his/her settings.
 * 
 * @return auto_ptr<lp::BaseAlgorithm>
 */
auto_ptr<lp::BaseAlgorithm> SolveModelImpl::getLpAlgorithm() const
{
#ifdef ENABLE_SCSOLVER_UNO_ALGORITHM
	auto_ptr<lp::BaseAlgorithm> algorithm( new lp::UnoAlgorithm(
		ascii("org.openoffice.sc.solver.LpSolve"), getSolverImpl()->getCalcInterface() ) );
#else
	auto_ptr<lp::BaseAlgorithm> algorithm( new lp::RevisedSimplex );
#endif

	return algorithm;
}

/** This is THE method which takes model parameters from the dialog,
	constructs an internal representation of a model, chooses an algorithm,
	and solves it. */
void SolveModelImpl::solve()
{
	using namespace numeric::opres;

	Goal eGoal = m_pSolverImpl->getMainDialog()->getGoal();
	SolverDialog* pMainDlg = getSolverImpl()->getMainDialog();
	if ( eGoal == GOAL_UNKNOWN )
	{
		Debug( "goal is not set" );
		pMainDlg->showSolveError( ascii( "Goal is not set" ) );
		return;
	}

	m_pBuilder->setGoal( eGoal );

	resolveObjectiveFuncAddress();
	resolveDecisionVarAddress();
	resolveConstraintAddress();
	parseConstraints();

	lp::Model aModel = m_pBuilder->getModel();
	aModel.print(); // prints model to stdout
	aModel.setPrecision( 2 );
	auto_ptr<lp::BaseAlgorithm> algorithm = getLpAlgorithm();

	aModel.setVerbose( true );
	m_bSolved = false;
	try
	{
		algorithm->setModel( &aModel );
		algorithm->solve();
		m_bSolved = true;
		m_mxSolution = algorithm->getSolution();
		updateCells();
		pMainDlg->showSolutionFound();
	}
	catch( const lp::ModelInfeasible& e )
	{
		Debug( "model infeasible" );
		pMainDlg->showSolutionInfeasible();
	}
	catch( const std::exception& e )
	{
		cout << "standard exception: " << e.what() << endl;
		pMainDlg->showSolveError( ascii( e.what() ) );
	}
}

bool SolveModelImpl::isSolved() const
{
	return m_bSolved;
}

/** Takes a raw formula string, and transform it into a starndard form of
	linear constraint. */
void SolveModelImpl::parseConstraints()
{
	// Create a cost vector from the decision variables.
	
	vector< CellAddress > aAddrs = m_pBuilder->getAllDecisionVarAddresses();
	vector< double > aOrigVal;
	vector< CellAddress >::iterator pos;
	vector< CellAddress >::iterator aAddrsBegin = aAddrs.begin(), aAddrsEnd = aAddrs.end();
	CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
	for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
	{
		// Run through the decision variable cells, store their originla formulas,
		// and set "=0" to each of these cells.
		table::CellAddress aAddr = *pos;
		rtl::OUString sFormula = pCalc->getCellFormula( aAddr );
		m_pBuilder->setTempCellFormula( aAddr, sFormula );
		pCalc->setCellFormula( aAddr, ascii( "=0" ) );
	}
	
	// Set "=1" to each of the decision variable cells one at a time, and 
	// interpret the formula result.  Also set the constraint matrix size
	// so that it won't get resized at later time (resize is expensive).
	
	CellAddress aObjAddr = m_pBuilder->getObjectiveFormulaAddress();
	vector< ConstraintAddress > aConstAddrs = m_pBuilder->getAllConstraintAddresses();
	m_pBuilder->setConstraintMatrixSize( aConstAddrs.size(), aAddrs.size() );
	
	for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
	{
		table::CellAddress aAddr = *pos;
		pCalc->setCellFormula( aAddr, ascii( "=1" ) );
		
		double fVal = pCalc->getCellValue( aObjAddr );
		m_pBuilder->setCostVector( aAddr, fVal );
		
		// Go through the constraints to construct constraint matrix
		// as well as the RHS vector.
		
		vector< ConstraintAddress >::iterator posCA;
		for ( posCA = aConstAddrs.begin(); posCA != aConstAddrs.end(); ++posCA )
		{
			double fValL, fValR;
			ConstraintAddress aConstAddr = *posCA;
			fValL = pCalc->getCellValue( aConstAddr.Left );
			fValR = pCalc->getCellValue( aConstAddr.Right );
			m_pBuilder->setConstraintCoefficient( aAddr, aConstAddr, fValL, fValR );
		}		
		
		pCalc->setCellFormula( aAddr, ascii( "=0" ) );
	}
	
	// Restore the original formulas to the decision variable cells.
	for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
	{
		table::CellAddress aAddr = *pos;
		rtl::OUString sOrigFormula = m_pBuilder->getTempCellFormula( aAddr );
		pCalc->setCellFormula( aAddr, sOrigFormula );
	}

#ifdef DEBUG
	// Check to ensure the model has the right cost vector.
	for ( pos = aAddrsBegin; pos != aAddrsEnd; ++pos )
	{
		table::CellAddress aAddr = *pos;
		double f = m_pBuilder->getCostVector( aAddr );
		cout << aAddr.Column << ", " << aAddr.Row << " = " << f << endl;
	}
#endif
}

/** Take from the main dialog the constraint data which are given as cell 
	references. */
void SolveModelImpl::resolveConstraintAddress()
{
	vector< ConstraintString > aConstraint = m_pSolverImpl->getMainDialog()->getAllConstraints();
	vector< ConstraintString >::iterator pos = aConstraint.begin(), posEnd = aConstraint.end();
	CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
	m_pBuilder->clearConstraintAddresses();
	while ( pos != posEnd )
	{
		ConstraintString aConstStr = *pos;

		// Expand a cell range into multiple constraints.

		table::CellRangeAddress aRangeAddrL = pCalc->getCellRangeAddress( aConstStr.Left );
		table::CellRangeAddress aRangeAddrR = pCalc->getCellRangeAddress( aConstStr.Right );
		sal_Int16 nLSheet = aRangeAddrL.Sheet;
		sal_Int32 nLColS  = aRangeAddrL.StartColumn;
		sal_Int32 nLColE  = aRangeAddrL.EndColumn;
		sal_Int32 nLRowS  = aRangeAddrL.StartRow;
		sal_Int32 nLRowE  = aRangeAddrL.EndRow;
		sal_Int16 nRSheet = aRangeAddrR.Sheet;
		sal_Int32 nRColS  = aRangeAddrR.StartColumn;
		sal_Int32 nRColE  = aRangeAddrR.EndColumn;
		sal_Int32 nRRowS  = aRangeAddrR.StartRow;
		sal_Int32 nRRowE  = aRangeAddrR.EndRow;
		
		if ( nLColE - nLColS != nRColE - nRColS || nLRowE - nLRowS != nRRowE - nRRowS )
			throw CellGeometriesDiffer(); // This should not happen !
			
		for ( sal_Int32 i = 0; i <= nLColE - nLColS; ++i )
			for ( sal_Int32 j = 0; j <= nLRowE - nLRowS; ++j )
			{
				ConstraintAddress aConstAddr;
				table::CellAddress aAddrL, aAddrR;
				aAddrL.Sheet  = nLSheet;
				aAddrL.Row    = nLRowS + j;
				aAddrL.Column = nLColS + i;

				aAddrR.Sheet  = nRSheet;
				aAddrR.Row    = nRRowS + j;
				aAddrR.Column = nRColS + i;

				aConstAddr.Left  = aAddrL;
				aConstAddr.Right = aAddrR;
				aConstAddr.Equal = aConstStr.Equal;
				
				m_pBuilder->setConstraintAddress( aConstAddr );
			}
		
		++pos;
	}
}

/** Convert a 3D cell reference (e.g. $'Sheet Name'.$B$5:$C$7) into an array of
	individual cell addresses.  These addresses represent a series of decision
	variables in sequential order. */
void SolveModelImpl::resolveDecisionVarAddress()
{
	using com::sun::star::table::CellRangeAddress;

	rtl::OUString sAddr = m_pSolverImpl->getMainDialog()->getVarCellAddress();
	if ( sAddr.getLength() > 0 )
	{
		CellRangeAddress aRangeAddr = m_pSolverImpl->getCalcInterface()->getCellRangeAddress( sAddr );
		
		// Convert address range into an array of individual cell coordinates.
		sal_Int16 nSheetId = aRangeAddr.Sheet;
		sal_Int32 nSCol = aRangeAddr.StartColumn, nSRow = aRangeAddr.StartRow;
		sal_Int32 nECol = aRangeAddr.EndColumn, nERow = aRangeAddr.EndRow;
		
		m_pBuilder->clearDecisionVarAddresses();
		for ( sal_Int32 nCol = nSCol; nCol <= nECol; ++nCol )
		{
			for ( sal_Int32 nRow = nSRow; nRow <= nERow; ++nRow )
			{
				CellAddress aCellAddr;
				aCellAddr.Sheet = nSheetId;
				aCellAddr.Column = nCol;
				aCellAddr.Row = nRow;
				m_pBuilder->setDecisionVarAddress( aCellAddr );
			}
		}
	}
	else
		Debug( "Decision variable cells empty" );
}

/** Takes the string form of an objective function cell address (e.g. $'Sheet1'.$A$2),
	converts it into a 3D address, and set it into the builder class.
 */
void SolveModelImpl::resolveObjectiveFuncAddress()
{
	rtl::OUString sTargetCellAddr = m_pSolverImpl->getMainDialog()->getTargetCellAddress();
	if ( sTargetCellAddr.getLength() > 0 )
	{
		CellAddress aAddr = m_pSolverImpl->getCalcInterface()->getCellAddress(
				sTargetCellAddr );
		
		m_pBuilder->setObjectiveFormulaAddress( aAddr );
	}
	else
		Debug( "Target cell address empty" );
}

void SolveModelImpl::updateCells()
{
	vector<CellAddress> cnAddrs = m_pBuilder->getAllDecisionVarAddresses();
	CalcInterface* pCalc = m_pSolverImpl->getCalcInterface();
	OSL_ASSERT( m_mxSolution.rows() == cnAddrs.size() );
	vector<CellAddress>::iterator it, itEnd = cnAddrs.end();
	size_t nIdx = 0;
	for ( it = cnAddrs.begin(); it != itEnd; ++it )
		pCalc->setCellValue( *it, m_mxSolution( nIdx++, 0 ) );
}

//---------------------------------------------------------------------------
// SolveModel

SolveModel::SolveModel( SolverImpl* p ) :
		m_pImpl( new SolveModelImpl( p ) )
{
}

SolveModel::~SolveModel() throw()
{
}

void SolveModel::solve()
{
	m_pImpl->solve();
}

bool SolveModel::isSolved() const
{
	return m_pImpl->isSolved();
}


}

		



