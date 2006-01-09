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


#include <numeric/matrix.hxx>
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace ::scsolver::numeric;
using ::boost::shared_ptr;

template<typename T>
void output( T t )
{
	std::cout << t << std::endl;
}

void hr()
{
	output( "----------------------------------" );
}

template<typename T>
void title( T t )
{
	output( "" );
	hr();
	output( t );
	hr();
}

void swapTest()
{
	title( "Swap Test" );
	Matrix m1( 3, 3, true );
	Matrix m2( 6, 6, true );
	output( "m1" );
	m1.print(0);
	output( "m2" );
	m2.print(0);
	
	m1.swap( m2 );

	output( "m1" );
	m1.print(0);
	output( "m2" );
	m2.print(0);
}

void copyTest()
{
	title( "Copy Test" );
	Matrix m1( 3, 3, true );
	m1( 2, 1 ) = 34.532;
	output( "m1" );
	m1.print();
	Matrix m2( 0, 0 );
	m2.copy( m1 );
	output( "m2 copied" );
	m2.print();
	Matrix m3 = m1.clone();
	output( "m3 cloned" );
	m3.print();
	m1( 1, 2 ) = 4534.234;
	output( "m1 again" );
	m1.print();
	output( "m2 again" );
	m2.print();
	output( "m3 again" );
	m3.print();
}

void smokeTest()
{
	title( "Smoke Test" );

	Matrix mx;
	double aVal[121] = {
	 9,2,1,5,1,2,9,4,9,2,1,
	 1,2,1,5,1,6,7,4,5,2,1,
	 2,2,2,2,2,5,6,4,4,3,1,
	 3,2,1,5,1,4,5,4,3,4,1,
	 4,2,3,9,2,3,4,4,2,5,1,
	 5,2,9,5,1,2,3,4,1,6,1,
	 6,2,1,5,4,1,2,4,9,7,1,
	 1,2,6,1,1,2,6,9,9,2,1,
	 5,2,1,5,5,2,3,4,8,2,1,
	 2,2,3,5,1,2,3,4,9,4,1,
	 1,2,2,2,1,9,3,4,9,2,10 };
 	
 	for ( size_t i = 0; i < 11; ++i )
 		for ( size_t j = 0; j < 11; ++j )
 			mx( i, j ) = aVal[ j + i*11 ];

	mx.print( 0 );
// 	Matrix mx2 = mx + 10.0;
// 	mx2.print( 0 );
// 	(mx*mx2).print( 0 );
	Matrix mxInv = mx.inverse();
	mxInv.print( 2 );
}

void baseTest()
{
	vector<size_t> aCols;
	aCols.push_back( 0 );
	aCols.push_back( 2 );
	aCols.push_back( 4 );

	shared_ptr<Matrix> p( new Matrix( 3, 3, true ) );
	for ( int i = 0; i < 8; ++i )
		p->setValue( 0, i, i );
	p->setValue( 2, 0, -4.56 );
	p->setValue( 2, 1, -2.3 );
	p->setValue( 3, 3, 2.34 );
	p->setValue( 5, 5, 567 );
	p->setValue( 5, 7, 13.4 );
	p->print();
	cout << endl;
	p->deleteColumns( aCols );
	p->print();
	cout << endl;
	Matrix m = (*p)/2.0;
	m.print();
	cout << endl;

	Matrix m1( 4, 3, true ), m2( 3, 3, true );
	m1.setValue( 2, 2, 10 );
	m2.setValue( 2, 2, 2.5 );
	m1.print();
	m2.print();
	
	try
	{
		m1 += m2;
		m1.print();
	}
	catch ( ::scsolver::numeric::MatrixSizeMismatch e )
	{
		cout << "Matrix sizes do not match" << endl;
		//return EXIT_FAILURE;
	}
	
	m1.clear();
	m1.setValue( 0, 0, 1.2 );
	m1.setValue( 0, 1, 3.4 );
	m1.setValue( 0, 2, 5.0 );
	m1.setValue( 2, 0, 23.0 );
	cout << "m1" << endl;
	m1.print();
	if ( m1.isSquare() )
		cout << "This is square matrix" << endl;
	cout << endl;
	
	m2.clear();
	m2.setValue( 0, 0, 2.0 );
	m2.setValue( 1, 0, 2.0 );
	m2.setValue( 2, 0, 3.0 );
	cout << "m2" << endl;
	m2.print();
	cout << endl;
	
	Matrix m3 = m1*m2;
	cout << "m3" << endl;
	m3.print();
	cout << endl;
	
	m1.clear();
	m1.setValue( 0, 0, 1.0 );
	m1.setValue( 1, 0, 2.3 );
	m1.setValue( 1, 1, 3.4 );
	m1.setValue( 2, 2, -23 );
	m1.setValue( 2, 0, 12.0 );
	m1.setValue( 0, 2, 2.9 );
	cout << "m1" << endl;
	m1.print();
	
	cout << "operator() " << m1( 1, 1 ) << endl;
	cout << "operator() assignment " << endl;
	m1( 1, 1 ) = 34.55555;
	m1.print();
	
	Matrix minv = m1.inverse();
	cout << endl << "m1 inverse" << endl;
	minv.print();
	if ( minv.isSquare() )
		cout << "This matrix is square" << endl;
	Matrix mtrans = minv.trans();
	mtrans.print();
	cout << "assignment to mtrans" << endl;
	double f = 324.3243;
	mtrans( 0, 0 ) = f;
	mtrans.print();
	
	cout << "operator=" << endl;
	m1 = m3;
	m1.print();
	Matrix mtest( m3 );
	mtest.print();
	mtest = m1 = m2;
	mtest.print();
	
	cout << endl << "const matrix test" << endl;
	const Matrix mconst( 2, 2, true );
	mconst.print();
}

int main( int argc, char** argv )
{
// 	baseTest();
// 	swapTest();
// 	copyTest();
// 	smokeTest();

	Matrix mx( 3, 3 );
	mx( 2, 0 ) = 3.4;
	mx( 0, 0 ) = 12.0;
	mx( 0, 1 ) = 23.4;
	mx( 1, 2 ) = 4.9;
	mx.print( 2 );
	mx.inverse().print( 2 );
	
	return EXIT_SUCCESS;
}
