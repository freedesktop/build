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

#include <numeric/nlpnewton.hxx>
#include <numeric/diff.hxx>
#include <numeric/nlpmodel.hxx>
#include <numeric/funcobj.hxx>
#include <boost/shared_ptr.hpp>
#include <global.hxx>
#include <memory>
#include <iostream>
#include <iomanip>
#include <cmath>

using scsolver::numeric::Matrix;
using scsolver::numeric::Differentiate;
using boost::shared_ptr;
using namespace std;

namespace scsolver { namespace numeric { namespace opres { namespace nlp {

//---------------------------------------------------------------------------
// IMPLEMENTATION

class QuasiNewtonImpl
{
	static double norm( const Matrix& );
	static double evalF( const BaseFuncObj&, const Matrix&, vector<double>& );
	static double evalF( const BaseFuncObj&, const Matrix& );

public:
	QuasiNewtonImpl( QuasiNewton* );
	~QuasiNewtonImpl() throw() {}

	void solve();

private:
	QuasiNewton* m_pSelf;
	boost::shared_ptr<Differentiate> m_pDiff;

	// These data members are expected to be initialized at start of 'void solve()' call.
	unsigned long m_nIter;
	Model* m_pModel;
	Matrix m_mxVars;
	Matrix m_mxdVars;
	Matrix m_mxVarsOld;
	Matrix m_mxdF;
	Matrix m_mxdFOld;
	Matrix m_mxD;
	Matrix m_mxDOld;
	double m_fF;
	double m_fFOld;
	double m_fNorm;
	double m_fTolerance;

	boost::shared_ptr<BaseFuncObj> m_pFuncObj;

	bool evaluateFunc();
	bool calcDefMatrix();
	double runLinearSearch();
	bool runIteration();
};

/** Calculate the norm of an array of variables.

	@param mxX one-column matrix representing delta f(x) vector

	@return calculate norm
 */
double QuasiNewtonImpl::norm( const Matrix& mxX )
{
	double fNorm = 0.0;
	for ( size_t i = 0; i < mxX.rows(); ++i )
	{
		double f = mxX( i, 0 );
		fNorm += f*f;
	}
	return std::sqrt( fNorm );
}

double QuasiNewtonImpl::evalF( const BaseFuncObj& oF, const Matrix& mxVars, vector<double>& fVars )
{
	size_t nRows = mxVars.rows();
	fVars.reserve( nRows );
	for ( size_t i = 0; i < nRows; ++i )
		fVars.push_back( mxVars( i, 0 ) );

	return oF.eval( fVars );
}

double QuasiNewtonImpl::evalF( const BaseFuncObj& oF, const Matrix& mxVars )
{
	vector<double> fVars;
	return evalF( oF, mxVars, fVars );
}

QuasiNewtonImpl::QuasiNewtonImpl( QuasiNewton* p ) :
	m_pSelf( p ),
	m_pDiff( new Differentiate() ),
	m_nIter( 0 ),
	m_pModel( NULL ),
	m_mxVars( 0, 0 ),
	m_mxdVars( 0, 0 ),
	m_mxVarsOld( 0, 0 ),
	m_mxdF( 0, 0 ),
	m_mxdFOld( 0, 0 ),
	m_mxD( 0, 0 ),
	m_mxDOld( 0, 0 ),
	m_fF( 0.0 ),
	m_fFOld( 0.0 ),
	m_fNorm( 0.0 ),
	m_fTolerance( 0.0 )
{
}

void QuasiNewtonImpl::solve()
{
	// Initialize relevant data members.
	m_pModel = m_pSelf->getModel();
	vector<double> cnVars = m_pModel->getVariable();
	
	vector<double>::const_iterator it, itBeg = cnVars.begin(), itEnd = cnVars.end();
	m_mxVars.clear();
	for ( it = itBeg; it != itEnd; ++it )
		m_mxVars( distance( itBeg, it ), 0 ) = *it;

	m_mxdVars.clear();
	m_mxVarsOld.clear();
	m_mxdF.clear();
	m_mxdFOld.clear();
	m_mxD.clear();
	m_mxDOld.clear();
	m_fTolerance = 0.1;
	for ( unsigned long i = 0; i < m_pModel->getPrecision(); ++i )
		m_fTolerance *= 0.1;

	m_fF = 0.0;
	m_fFOld = m_fTolerance * 100.0;
	m_fNorm = m_fTolerance * 100.0;

	m_pFuncObj = m_pModel->getFuncObject();
	m_pDiff->setFuncObject( m_pFuncObj );

	m_nIter = 0;
	bool bIterDone = false;
	cout << setprecision( m_pModel->getPrecision() );
	while ( !bIterDone )
		bIterDone = runIteration();

	cout << "f(x) = " << m_fF << endl;
}

bool QuasiNewtonImpl::evaluateFunc()
{
	using namespace std;

	// Solve f(x) given the x vector

	vector<double> fVars;
	m_fF = QuasiNewtonImpl::evalF( *m_pFuncObj, m_mxVars, fVars );
	size_t nRows = fVars.size();

	// Calculate df(x) gradient array
	m_pDiff->setVariables( fVars );
	for ( size_t i = 0; i < nRows; ++i )
	{
		m_pDiff->setVarIndex( i );
		m_mxdF( i, 0 ) = m_pDiff->run();
	}
	m_mxdF.print();
	m_fNorm = QuasiNewtonImpl::norm( m_mxdF );

	if ( fabs( m_fF - m_fFOld ) < m_fTolerance )
	{
		if ( m_pModel->getVerbose() )
			cout << "desired precision has been reached" << endl;
		return true;
	}

	if ( m_fNorm < m_fTolerance )
	{
		if ( m_pModel->getVerbose() )
			cout << "norm is below specified tolerance limit" << endl;
		return true;
	}

	return false;
}

/** Calculate or initialize deflection matrix (D).

	@return true if the iteration should stop, or false otherwise
 */
bool QuasiNewtonImpl::calcDefMatrix()
{
	if ( m_mxD.empty() )
	{
		// Deflection matrix is empty.  Initialize it.
		size_t nX = m_mxVars.rows();
		Matrix mxTemp( nX, nX, true );
		m_mxD.swap( mxTemp );
		if ( m_pModel->getGoal() == GOAL_MAXIMIZE )
			m_mxD *= -1.0;
	}
	else
	{
		// Both mxD and mxG are columnar matrix.
		Matrix mxD = m_mxVars - m_mxVarsOld;
		Matrix mxG = m_mxdF - m_mxdFOld;
		double fDG = ( mxD.trans()*mxG )( 0, 0 );
		if ( fDG == 0.0 )
		{
			if ( m_pModel->getVerbose() )
				cout << "dg is zero!" << endl;
			return true;
		}

		// Calculate new deflection matrix via BFGS formula
		Matrix mxA = mxG.trans() * m_mxDOld * mxG;
		double fA = 1.0 + mxA( 0, 0 ) / fDG;
		Matrix mxB = mxD * mxD.trans() / fDG;
		Matrix mxC = ( m_mxDOld * mxG * mxD.trans() + mxD * mxG.trans() * m_mxDOld ) / fDG;
		m_mxD = m_mxDOld + mxB*fA - mxC;
	}

	m_mxdVars = m_mxD * m_mxdF*(-1.0);

	if ( m_pModel->getVerbose() )
	{
		unsigned long nPrec = m_pModel->getPrecision();
		cout << setprecision( nPrec );
		cout << "f(x) = " << m_fF << endl;
		cout << "df(x) = ";
		m_mxdF.trans().print( nPrec );
		cout << "|| df(x) || = " << m_fNorm << endl;
		cout << "D:";
		m_mxD.print( nPrec );
		cout << "dx = ";
		m_mxdVars.trans().print( nPrec );
	}

	return false;
}

/** One-dimensional search (needs improvement)
 */
double QuasiNewtonImpl::runLinearSearch()
{
	// m_mxVars  : Original X
	// m_mxdVars : dX
	// m_fF      : Original f(x)

	double fInterval = 0.1;
	double fLambda = 0.0;
// 	double fTolerance = m_fTolerance;

	Matrix mxVarsOriginal = m_mxVars.clone();

	double fF = m_fF;

	while ( true )
	{
		// Inspect two points
		Matrix mxVars = mxVarsOriginal + m_mxdVars * ( fLambda + fInterval );
		double fF1 = QuasiNewtonImpl::evalF( *m_pFuncObj, mxVars );
		mxVars = mxVarsOriginal + m_mxdVars * ( fLambda + fInterval/2.0 );
		double fF2 = QuasiNewtonImpl::evalF( *m_pFuncObj, mxVars );

		double fSign = m_pModel->getGoal() == GOAL_MAXIMIZE ? 1.0 : -1.0;
		if ( fF2*fSign >= fF*fSign )
			if ( fF1*fSign >= fF2*fSign )
			{
				fLambda += fInterval;
				fF = fF1;
			}
			else
			{
				fLambda += fInterval/2.0;
				return fLambda;
			}
		else
			fInterval /= 2.0;
	}
	assert( !"logic error" );
}

bool QuasiNewtonImpl::runIteration()
{
	Debug( "runIteration" );

	if ( m_pModel->getVerbose() )
	{
		cout << repeatString( "-", 70 ) << endl;
		cout <<	"Iteration " << m_nIter << endl;
		cout << repeatString( "-", 70 ) << endl;
		cout << "x = ";
		m_mxVars.print( m_pModel->getPrecision() );
	}

	if ( evaluateFunc() )
		return true;

	if ( calcDefMatrix() )
		return true;

	double fLambda = runLinearSearch();
	if ( m_pModel->getVerbose() )
		cout << "lambda = " << fLambda << endl;

	// Update parameters for next iteration

	m_mxVarsOld = m_mxVars;
	m_mxVars += m_mxdVars * fLambda;
	m_mxdFOld = m_mxdF;
	m_mxDOld = m_mxD;
	m_fFOld = m_fF;
	++m_nIter;

	return false;
}


//---------------------------------------------------------------------------
// ENVELOPE

QuasiNewton::QuasiNewton() : m_pImpl( new QuasiNewtonImpl( this ) )
{
}

QuasiNewton::~QuasiNewton() throw()
{
}

void QuasiNewton::solve()
{
	m_pImpl->solve();

	Matrix mxSolution( 0, 0 );

	setSolution( mxSolution );
}

}}}}

