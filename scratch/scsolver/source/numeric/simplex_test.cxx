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


#include <numeric/type.hxx>
#include <numeric/lpbase.hxx>
#include <numeric/lpmodel.hxx>
#include <numeric/lpsimplex.hxx>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;
using namespace ::scsolver::numeric;
using namespace ::scsolver::numeric::opres;
using namespace ::scsolver::numeric::opres::lp;

void printTitle( const char* title )
{
	const char* line = "----------------------------------------------------------------------";
	cout << line << endl;
	cout << title << endl;
	cout << line << endl;
}

void testCase1()
{
	printTitle( "Revised Simplex 1" );

	auto_ptr<Model> p( new Model() );
	p->setCostVectorElement( 0, 12 );
	p->setCostVectorElement( 1, 9 );
	p->setGoal( GOAL_MAXIMIZE );
	vector<double> aConst;
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 1750 );
	aConst.clear();
	aConst.push_back( 4 );
	aConst.push_back( 2 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 4800 );
	aConst.clear();
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 1000 );
	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 1500 );
	
	p->setVerbose( true );
	p->print();
	p->setPrecision( 2 );
	
	std::auto_ptr<BaseAlgorithm> algorithm( new RevisedSimplex );
	p->solve( algorithm );
}

void testCase2()
{
	printTitle( "Revised Simplex 2" );

	auto_ptr<Model> p( new Model() );
	p->setCostVectorElement( 0, 12 );
	p->setCostVectorElement( 1, 9 );
	p->setCostVectorElement( 2, -10 );
	p->setGoal( GOAL_MAXIMIZE );
	vector<double> aConst;
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 1750 );
	aConst.clear();
	aConst.push_back( 4 );
	aConst.push_back( 2 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 4800 );
	aConst.clear();
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 1000 );
	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 1500 );	
	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 2 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, GREATER_THAN_EQUAL, 100 );
	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, GREATER_THAN_EQUAL, 500 );
	
	p->setVerbose( true );
	p->print();
	p->setPrecision( 2 );
	
	std::auto_ptr<BaseAlgorithm> algorithm( new RevisedSimplex );
	p->solve( algorithm );
}

void testCase3()
{
	printTitle( "Bounded Revised Simplex" );

	auto_ptr<Model> p( new Model() );
	p->setCostVectorElement( 0, 7 );
	p->setCostVectorElement( 1, 12 );
	p->setCostVectorElement( 2, 5 );
	p->setCostVectorElement( 3, 14 );
	vector<double> aConst;
	aConst.push_back(  300 );
	aConst.push_back(  600 );
	aConst.push_back(  500 );
	aConst.push_back( 1600 );
	p->addConstraint( aConst, GREATER_THAN_EQUAL, 700 );
	aConst.clear();
	aConst.push_back( 10 );
	aConst.push_back( 5 );
	aConst.push_back( 20 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 2000 );

	for ( unsigned long i = 0; i < 4; ++i )
	{
		if ( i%2 == 0 )
		{
		p->setVarBound( i, BOUND_LOWER, -10 );
		p->setVarBound( i, BOUND_UPPER,  10 );
		}
	}
	p->setGoal( GOAL_MINIMIZE );
	p->setVerbose( true );
	p->print();
	p->setPrecision( 2 );
	
	std::auto_ptr<BaseAlgorithm> algorithm( new BoundedRevisedSimplex );
	p->solve( algorithm );
}

void testModel1()
{
	printTitle( "LP Model Test" );

	auto_ptr<Model> p1( new Model() );
	p1->setCostVectorElement( 0, 1 );
	p1->setCostVectorElement( 1, 2 );
	p1->setCostVectorElement( 2, 20 );
	vector<double> aConst;
	aConst.push_back( 10 );
	aConst.push_back( -20 );
	p1->addConstraint( aConst, GREATER_THAN_EQUAL, 200 );
	p1->setGoal( GOAL_MINIMIZE );
	p1->setVarBound( 0, BOUND_LOWER, 0 );
	p1->setVarBound( 1, BOUND_LOWER, 0 );
	p1->setVarBound( 1, BOUND_UPPER, 10 );
	p1->setVerbose( true );
	p1->print();
	
	// Copy model and modify it.
	auto_ptr<Model> p2( new Model( *p1.get() ) );
	p2->setGoal( GOAL_MAXIMIZE );
	p2->setVarBound( 0, BOUND_LOWER, 10 );
	p2->setVarBound( 2, BOUND_UPPER, 100 );
	p2->setCostVectorElement( 0, 100 );
	p2->print();
	
	// Show not show any modification
	p1->print();
}

/** Test case provided by Niklas Nebel.  The earlier algorithm failed to solve
	this due to an error in the lambda calculation routine.
 */
void testCaseNiklas()
{
	printTitle( "Test case from Niklas Nebel" );

	auto_ptr<Model> p( new Model() );
	for ( int i = 0; i < 10; ++i )
		p->setCostVectorElement( i, i+1 );

	vector<double> aConst;
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 100 );

	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 100 );

	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 100 );

	aConst.clear();
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 0 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	aConst.push_back( 1 );
	p->addConstraint( aConst, LESS_THAN_EQUAL, 100 );

	p->setGoal( GOAL_MAXIMIZE );
	p->setVerbose( true );
	p->print();
	p->setPrecision( 2 );
	
	auto_ptr<BaseAlgorithm> algo1( new RevisedSimplex );
	auto_ptr<BaseAlgorithm> algo2( new BoundedRevisedSimplex );
	p->solve( algo1 );
	p->solve( algo2 );
}

void testCaseLudovic()
{
	printTitle( "Test Case from Ludovic" );
	auto_ptr<Model> pModel( new Model );
    //Objective: 0.3715x_1 + 0.929x_2 + 0.011x_3 + 77.5328x_4 + 74.9694x_5 + 72.1143x_6 + 81.0941x_7 + 81.0941x_8 + 70.5025x_9 + 20.0049x_10 + 74.7238x_11 - 0.625x_12 + 0.27875x_15 + 0.187x_16 + 0.187x_17 + 0.00825x_18 + 0.011x_19 + 0.02563x_20 + 0.09x_21 + 0.05345x_22  (min)
	double fCosts[] = {
		0.3715, 0.929, 0.011, 77.5328, 74.9694, 72.1143, 81.0941, 81.0941, 70.5025, 20.0049,
		74.7238, 0.625, 0, 0, 0.27875, 0.187, 0.187, 0.00825, 0.011, 0.02563,
		0.09, 0.05345 
	};
	//pModel->setCostVector( fCosts );
}

int main( int argc, char** argv )
{
	//testCase1();
	//testCase2();
	testCase3();
	//testCaseNiklas();

	//testModel1();
	//testCaseLudovic();


	return EXIT_SUCCESS;
}
