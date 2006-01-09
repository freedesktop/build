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
#include <global.hxx>
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <exception>
#include <iterator>

#warning "No idea what's going on with noalias"
#undef noalias
#define noalias(a) (a)

#warning "We should re-write this to use OUString:: methods I think."
#if 0
#  include <boost/algorithm/string/replace.hpp>
using namespace ::boost::algorithm;
#else
//#  include <boost/mpl/distance.hpp>
//using namespace ::boost::mpl;
#endif
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>

namespace bnu = ::boost::numeric::ublas;

using std::cout;
using std::endl;

namespace scsolver { namespace numeric {

const char* MatrixSizeMismatch::what() const throw()
{
	return "Matrix size mismatch";
}

const char* MatrixNotDiagonal::what() const throw()
{
	return "Matrix not diagonal";
}

const char* OperationOnEmptyMatrix::what() const throw()
{
	return "Operation on empty matrix";
}

const char* SingularMatrix::what() const throw()
{
	return "Singular matrix encountered";
}

const char* NonSquareMatrix::what() const throw()
{
	return "Matrix not square where a square matrix is required";
}

class MatrixImpl
{
public:
	MatrixImpl();
	MatrixImpl( size_t, size_t, bool = false );
	MatrixImpl( const MatrixImpl& );
	MatrixImpl( const MatrixImpl* );
	MatrixImpl( ::bnu::matrix<double> );
	~MatrixImpl() throw();
	
	void setResizable( bool b ) { m_bResizable = b; }
	bool getResizable() const { return m_bResizable; }
	void swap( MatrixImpl& ) throw();
	void clear();
	void copy( const MatrixImpl& );

	bnu::matrix< std::string > getDisplayElements( size_t, size_t, bool ) const;	
	void print( size_t = 2, size_t = 1 ) const;
	
	const double& getValue( size_t, size_t ) const;
	double& getValue( size_t, size_t );
	void setValue( size_t, size_t, double );
	
	MatrixImpl getColumn( size_t );
	MatrixImpl getRow( size_t );
	void deleteColumn( size_t );
	void deleteColumns( const std::vector<size_t>& );
	void deleteRow( size_t );
	void deleteRows( const std::vector<size_t>& );
	
	const MatrixImpl adj() const;
	double cofactor( size_t, size_t ) const;
	double det() const;
	const MatrixImpl inverse() const;
	const MatrixImpl trans() const;
	double minors( size_t, size_t ) const;

	size_t rows() const { return m_aArray.size1(); }
	size_t cols() const { return m_aArray.size2(); }
	
	bool empty() const;
	bool isRowEmpty( size_t ) const;
	bool isColumnEmpty( size_t ) const;
	bool isSameSize( const MatrixImpl& ) const;
	bool isSquare() const;
	
	// Overloaded Operators
	
	MatrixImpl& operator=( const MatrixImpl& );
	const MatrixImpl operator+( const MatrixImpl& ) const;
	const MatrixImpl operator-( const MatrixImpl& ) const;
	const MatrixImpl operator*( double ) const;
	const MatrixImpl operator*( const MatrixImpl& ) const;
	const MatrixImpl operator/( double ) const;
	MatrixImpl& operator+=( const MatrixImpl& );
	MatrixImpl& operator+=( double );
	MatrixImpl& operator-=( const MatrixImpl& );
	MatrixImpl& operator*=( double );
	MatrixImpl& operator/=( double );
	const double& operator()( size_t i, size_t j ) const { return getValue( i, j ); }
	double& operator()( size_t, size_t );
	
	bool operator==( const MatrixImpl& ) const;
	bool operator!=( const MatrixImpl& ) const;
	

private:
	bool m_bResizable;
	bnu::matrix<double, bnu::row_major, std::vector<double> > m_aArray;

	void expandAsNeeded( size_t, size_t );
	void throwIfEmpty() const;

}; // class MatrixImpl


//---------------------------------------------------------------------------
// Local helper functions

namespace mxhelper {

typedef bnu::matrix_range< bnu::matrix<double> >  MxRange;
typedef bnu::matrix_row< bnu::matrix<double> >    MxRow;
typedef bnu::matrix_column< bnu::matrix<double> > MxColumn;

bool isRowEmpty( const bnu::matrix<double>& A, size_t nRowId )
{
	if ( nRowId >= A.size1() )
		throw MatrixSizeMismatch();

	size_t nColSize = A.size2();
	for ( size_t i = 0; i < nColSize; ++i )
		if ( A( nRowId, i ) )
			return false;

	return true;
}

bool isColumnEmpty( const bnu::matrix<double>& A, size_t nColId )
{
	if ( nColId >= A.size2() )
		throw MatrixSizeMismatch();

	size_t nRowSize = A.size1();
	for ( size_t i = 0; i < nRowSize; ++i )
		if ( A( i, nColId ) )
			return false;

	return true;
}

void swapRows( bnu::matrix<double>& A, size_t nRow1, size_t nRow2 )
{
	MxRow row1( A, nRow1 );
	MxRow row2( A, nRow2 );
	row1.swap( row2 );
}

void factorize( const bnu::matrix<double>& mxA, bnu::matrix<double>& mxL, 
		bnu::matrix<double>& mxU, bnu::matrix<double>& mxP )
{
	if ( mxA.size1() != mxA.size2() )
		throw NonSquareMatrix();

#if 0
	size_t n = mxA.size1();
	bnu::matrix<double> A( mxA );
	bnu::identity_matrix<double> I( A.size1(), A.size2() );
	bnu::matrix<double> P( I );

	for ( unsigned int k = 0; k < n; ++k )
	{
		// Partial pivoting ( get column k and scan row(s) k:n )
		if ( n - k > 1 )
		{
			MxColumn cl( A, k );
			for ( unsigned int i = k + 1; i < n; ++i )
			{
				size_t nSwapRowId = k;
				double fVal = cl( k );
				for ( size_t nRowId = k; nRowId < n; ++nRowId )
				{
					double fTemp = cl( nRowId );
					if ( fTemp > fVal )
					{
						nSwapRowId = nRowId;
						fVal = fTemp;
					}
				}

				if ( nSwapRowId != k )
				{
					swapRows( A, k, nSwapRowId );
					swapRows( P, k, nSwapRowId );
				}	
			}
		}

		double fPivot = A( k, k );
		if ( fPivot == 0.0 )
			throw SingularMatrix();

		MxRange mr1( A, bnu::range( k + 1, n ), bnu::range( k, k + 1 ) );
		mr1 /= fPivot;
		MxRange mr2( A, bnu::range( k + 1, n ), bnu::range( k, k + 1 ) );
		MxRange mr3( A, bnu::range( k, k + 1 ), bnu::range( k + 1, n ) );
		MxRange mr4( A, bnu::range( k + 1, n ), bnu::range( k + 1, n ) );
		mr4 -= prod( mr2, mr3 );
	}
	
	// Transfer elements from A into L and U
	bnu::matrix<double> L( I ), U( n, n );
	for ( unsigned int i = 0; i < n; ++i )
		for ( unsigned int j = 0; j < n; ++j )
		{
			if ( i > j )
				L( i, j ) = A( i, j );
			else
				U( i, j ) = A( i, j );
		}
#endif

	size_t n = mxA.size1();
	bnu::matrix<double> A( mxA );

	::std::vector<size_t> cnP;
	cnP.reserve( n );
	for ( size_t i = 0; i < n; ++i )
		cnP.push_back( i );
	cout << n << " " << cnP.size() << endl;
	// for each column k...
	for ( size_t k = 0; k < n - 1; ++k )
	{
		double fMax = 0.0;
		size_t nSwapRowId = k;
		for ( size_t nRowId = k; nRowId < n; ++nRowId )
		{
			double fVal = fabs( A( nRowId, k ) );
			if ( fVal > fMax )
			{
				fMax = fVal;
				nSwapRowId = nRowId;
			}
		}
		
		if ( fMax == 0.0 )
			throw SingularMatrix();
			
		::std::swap( cnP.at( k ), cnP.at( nSwapRowId ) );
		swapRows( A, k, nSwapRowId );
		
		for ( size_t i = k + 1; i < n; ++i )
		{
			A( i, k ) /= A( k, k );
			for ( size_t j = k + 1; j < n; ++j )
				A( i, j ) -= A( i, k )*A( k, j );
		}
	}

	// Transfer elements from A into L and U
	bnu::identity_matrix<double> I( n, n );
	bnu::zero_matrix<double> N( n, n );
	bnu::matrix<double> L( I ), U( N ), P( N );

	for ( unsigned int i = 0; i < n; ++i )
		for ( unsigned int j = 0; j < n; ++j )
		{
			if ( i > j )
				L( i, j ) = A( i, j );
			else
				U( i, j ) = A( i, j );
		}

	::std::vector<size_t>::const_iterator it, itBeg = cnP.begin(), itEnd = cnP.end();
	for ( it = itBeg; it != itEnd; ++it )
	{
		size_t nIdx = std::distance( itBeg, it );
		cout << nIdx << endl;
		P( std::distance( itBeg, it ), *it ) = 1.0;
	}

	noalias( mxU ) = U;
	noalias( mxL ) = L;
	noalias( mxP ) = P;
}

void inverseU( const bnu::matrix<double>& mxU, bnu::matrix<double>& mxUInv )
{
	size_t nSize = mxU.size1();
	if ( nSize != mxU.size2() )
		throw NonSquareMatrix();

	bnu::matrix<double> Combo( nSize, nSize*2 );
	MxRange U( Combo, bnu::range( 0, nSize ), bnu::range( 0, nSize ) );
	MxRange UInv( Combo, bnu::range( 0, nSize ), bnu::range( nSize, nSize*2 ) );
	bnu::identity_matrix<double> I( nSize );
	U = mxU;
	UInv = I;
	
	// back substitution
	size_t nRow = nSize;
	do
	{
		// normalize the current row first
		--nRow;
		double fPivot = U( nRow, nRow );
		MxRow mrCur( Combo, nRow );
		mrCur /= fPivot;
		U( nRow, nRow ) = 1.0; // maybe redundant because it should already be 1.0
		
		// substitute upward
		if ( nRow > 0 )
		{
			size_t nSubRow = nRow;
			do
			{
				MxRow mrSub( Combo, --nSubRow );
				mrSub -= mrCur*U( nSubRow, nRow );
			}
			while ( nSubRow != 0 );
		}	
	}
	while ( nRow != 0 );
	
	noalias( mxUInv ) = UInv;
}

void inverseL( const bnu::matrix<double>& mxL, bnu::matrix<double>& mxLInv )
{
	size_t nSize = mxL.size1();
	if ( nSize != mxL.size2() )
		throw NonSquareMatrix();

	bnu::matrix<double> Combo( nSize, nSize*2 );
	MxRange L( Combo, bnu::range( 0, nSize ), bnu::range( 0, nSize ) );
	MxRange LInv( Combo, bnu::range( 0, nSize ), bnu::range( nSize, nSize*2 ) );
	bnu::identity_matrix<double> I( nSize );
	L = mxL;
	LInv = I;

	// forward substitution
	size_t nRow = 0;
	do
	{
		MxRow mrCur( Combo, nRow );
		
		// substitute downward
		if ( nRow < nSize - 1 )
		{
			size_t nSubRow = nRow + 1;
			do
			{
				MxRow mrSub( Combo, nSubRow );
				mrSub -= mrCur*L( nSubRow, nRow );
			}
			while ( ++nSubRow < nSize );
		}
	}
	while ( ++nRow < nSize );
	
	noalias( mxLInv ) = LInv;
}

/** Computes an inverse matrix of an arbitrary matrix.

	@param mxA original matrix
	@param mxInv inverse matrix ( reference variable )
 */
void inverse( const bnu::matrix<double>& mxA, bnu::matrix<double>& mxInv )
{
	size_t nRow = mxA.size1(), nCol = mxA.size2();
	if ( nRow != nCol )
		throw NonSquareMatrix();
		
	size_t nSize = nRow;
		
	bnu::matrix<double> mxL( nSize, nSize );
	bnu::matrix<double> mxU( nSize, nSize );
	bnu::matrix<double> mxP( nSize, nSize );
	factorize( mxA, mxL, mxU, mxP );
	
	bnu::matrix<double> mxUInv( nSize, nSize );
	inverseU( mxU, mxUInv );
	
	bnu::matrix<double> mxLInv( nSize, nSize );
	inverseL( mxL, mxLInv );
	
	bnu::matrix<double> mxTemp = prod( mxUInv, mxLInv );
	mxTemp = prod( mxTemp, mxP );
	noalias( mxInv ) = mxTemp;
}

} // end of namespace mxhelper

//---------------------------------------------------------------------------

MatrixImpl::MatrixImpl() : m_bResizable( true )
{
	bnu::matrix<double> m( 0, 0 );
	m_aArray = m;
}

MatrixImpl::MatrixImpl( size_t nRow, size_t nCol, bool bIdentMatrixImpl ) :
	m_bResizable( true )
{	
	bnu::matrix<double> m( nRow, nCol );
	for ( unsigned int i = 0; i < m.size1(); ++i )
		for ( unsigned int j = 0; j < m.size2(); ++j )
		{
			if ( bIdentMatrixImpl && i == j )
				m( i, j ) = 1.0;
			else
				m( i, j ) = 0.0;
		}
	m_aArray = m;
}

MatrixImpl::MatrixImpl( const MatrixImpl& other ) :
	m_bResizable( other.m_bResizable ),
	m_aArray( other.m_aArray )
{
}

MatrixImpl::MatrixImpl( const MatrixImpl* p ) :
	m_bResizable( p->m_bResizable ),
	m_aArray( p->m_aArray )
{
}

MatrixImpl::MatrixImpl( bnu::matrix<double> m ) : m_bResizable( true )
{
	m_aArray = m;
}

MatrixImpl::~MatrixImpl() throw()
{
}

void MatrixImpl::swap( MatrixImpl& other ) throw()
{
	std::swap( m_aArray, other.m_aArray );
	std::swap( m_bResizable, other.m_bResizable );
}

void MatrixImpl::clear()
{
	m_aArray.resize( 0, 0 );
}

void MatrixImpl::copy( const MatrixImpl& other )
{
	MatrixImpl tmp( other );
	swap( tmp );
}

/** Returns a matrix containing a set of text elements intended for 
	output of an arbitrary matrix with a desired precision and inter-column
	margin.  It is to be used inside the print() method. */
bnu::matrix< std::string > MatrixImpl::getDisplayElements( 
		size_t nPrec, size_t nColSpace, bool bFormula ) const
{
	using std::string;
	using std::ostringstream;

	// Set all column widths to 0.
	std::vector<unsigned int> aColLen;
	for ( unsigned int j = 0; j < m_aArray.size2(); ++j )
		aColLen.push_back( 0 );
	
	// Get string matrix.
	bnu::matrix<std::string> mxElements( m_aArray.size1(), m_aArray.size2() );
	for ( unsigned int i = 0; i < m_aArray.size1(); ++i )
		for ( unsigned int j = 0; j < m_aArray.size2(); ++j )
		{
			ostringstream osElem;
			double fVal = m_aArray( i, j );
			for ( unsigned int k = 0; k < nColSpace; ++k )
				osElem << " ";
			if ( nPrec > 0 )
				osElem << std::showpoint;
			osElem << std::fixed << std::setprecision( nPrec ) << fVal;
			mxElements( i, j ) = osElem.str();
			if ( aColLen.at( j ) < osElem.str().size() )
				aColLen[j] = osElem.str().size();
		}

	// Adjust column widths
	for ( unsigned int i = 0; i < mxElements.size1(); ++i )
		for ( unsigned int j = 0; j < mxElements.size2(); ++j )
		{
			unsigned int nLen = mxElements( i, j ).size();
			if ( aColLen.at( j ) > nLen )
			{
				std::string sSpace;
				for ( unsigned int k = 0; k < (aColLen.at( j ) - nLen); ++k )
					sSpace += " ";
				mxElements( i, j ) = sSpace + mxElements( i, j );
			}
		}
	
	if ( !bFormula )
		return mxElements;
	
	// Process elements for formula display.
	for ( unsigned int i = 0; i < mxElements.size1(); ++i )
	{
		for ( unsigned int j = 0; j < mxElements.size2(); ++j )
		{
			double fVal = m_aArray( i, j );
#warning Foo Baa here - update string code ..
#if 0
			if ( fVal == 1.0 )
				replace_first( mxElements( i, j ), "1", " " );
			else if ( fVal == -1.0 )
				replace_first( mxElements( i, j ), "-1", " -" );
			
			if ( j != 0 )
			{
				string sElem = mxElements( i, j );
				ostringstream os;
				os << repeatString( " ", nColSpace );
				if ( fVal >= 0 )
					os << "+" << sElem.substr(1);
				else
				{
					replace_first( sElem, "-", " " );
					os << "-" << sElem.substr(1);
				}
				mxElements( i, j ) = os.str();
			}
#endif
		}
	}
	return mxElements;
}

void MatrixImpl::print( size_t nPrec, size_t nColSpace ) const
{
	using std::string;
	using std::ostringstream;

	// Print to stdout
	ostringstream os;
	const bnu::matrix< string > mElements = getDisplayElements( nPrec, nColSpace, false );
	for ( unsigned int i = 0; i < mElements.size1(); ++i )
	{
		os << "|";
		for ( unsigned int j = 0; j < mElements.size2(); ++j )
		{
			std::string s = mElements( i, j );
			os << s;
		}
		os << repeatString( " ", nColSpace ) << "|" << endl;
	}
	cout << os.str();
}

const double& MatrixImpl::getValue( size_t nRowId, size_t nColId ) const
{
	return m_aArray( nRowId, nColId );
}

double& MatrixImpl::getValue( size_t nRowId, size_t nColId )
{
	return m_aArray( nRowId, nColId );
}

void MatrixImpl::setValue( size_t nRowId, size_t nColId, double fVal )
{
	expandAsNeeded( nRowId, nColId );
	m_aArray( nRowId, nColId ) = fVal;
}

MatrixImpl MatrixImpl::getColumn( size_t nColId )
{
	size_t nRows = rows();
	MatrixImpl m( nRows, 1 );
	m.setResizable( false );
	for ( size_t i = 0; i < nRows; ++i )
		m( i, 0 ) = getValue( i, nColId );
	return m;
}

MatrixImpl MatrixImpl::getRow( size_t nRowId )
{
	size_t nCols = cols();
	MatrixImpl mx( 1, nCols );
	mx.setResizable( false );
	for ( size_t i = 0; i < nCols; ++i )
		mx( 0, i ) = getValue( nRowId, i );
	return mx;
}

void MatrixImpl::deleteColumn( size_t nColId )
{
	if ( nColId >= m_aArray.size2() )
	{
		Debug( "deleteColumn" );
		throw MatrixSizeMismatch();
	}

	bnu::matrix<double> m( m_aArray.size1(), m_aArray.size2()-1 );
	for ( size_t i = 0; i < m_aArray.size1(); ++i )
		for ( size_t j = 0; j < m_aArray.size2(); ++j )
		{
			if ( j == nColId )
				continue;
			size_t j2 = j > nColId ? j - 1 : j;
			m( i, j2 ) = m_aArray( i, j );
		}
	m_aArray = m;
}

void MatrixImpl::deleteColumns( const std::vector<size_t>& cnColIds )
{
	// reverse sorted set
	typedef std::set<size_t,std::greater<size_t> > uIntSet;
	
	uIntSet aSortedIds;
	std::vector<size_t>::const_iterator pos;
	for ( pos = cnColIds.begin(); pos != cnColIds.end(); ++pos )
		aSortedIds.insert( *pos );

	uIntSet::iterator pos2;
	for ( pos2 = aSortedIds.begin(); pos2 != aSortedIds.end(); ++pos2 )
		deleteColumn( *pos2 );
}

void MatrixImpl::deleteRow( size_t nRowId )
{
	cout << "Delete Row: " << nRowId << endl;

	if ( nRowId >= m_aArray.size1() )
	{
		Debug( "deleteRow" );
		throw MatrixSizeMismatch();
	}

	bnu::matrix<double> m( m_aArray.size1()-1, m_aArray.size2() );
	for ( size_t i = 0; i < m_aArray.size1(); ++i )
		if ( i != nRowId )
		{
			size_t i2 = i > nRowId ? i - 1 : i;
			for ( size_t j = 0; j < m_aArray.size2(); ++j )
				m( i2, j ) = m_aArray( i, j );
		}
	m_aArray = m;
}

void MatrixImpl::deleteRows( const std::vector<size_t>& cnRowIds )
{
	std::vector<size_t> cnRowIds2( cnRowIds.begin(), cnRowIds.end() );
	std::sort( cnRowIds2.begin(), cnRowIds2.end() );

	std::vector<size_t>::reverse_iterator it,
		itBeg = cnRowIds2.rbegin(), itEnd = cnRowIds2.rend();
	for ( it = itBeg; it != itEnd; ++it )
		deleteRow( *it );
}

const MatrixImpl MatrixImpl::adj() const
{
	throwIfEmpty();

	MatrixImpl adj( cols(), rows() ); // transposed

	for ( size_t i = 0; i < adj.rows(); ++i )
		for ( size_t j = 0; j < adj.cols(); ++j )
			adj.setValue( i, j, cofactor( j, i ) );

	return adj;
}

double MatrixImpl::cofactor( size_t i, size_t j ) const
{
	throwIfEmpty();

	double fVal = minors( i, j );
	if ( (i+j)%2 )
		fVal *= -1;

	return fVal;
}

/** This method calculates the determinant of a square matrix.  It raises
	a MatrixSizeMismatch exception if the given matrix is not square.
 */
double MatrixImpl::det() const
{
	throwIfEmpty();
	
	if ( !isSquare() )
	{
		Debug( "is not square" );
		throw MatrixSizeMismatch();
	}

	if ( cols() == 1 )
		return getValue( 0, 0 );
	else if ( cols() == 2 )
		return getValue( 0, 0 )*getValue( 1, 1 ) - getValue( 0, 1 )*getValue( 1, 0 );
	else if ( cols() == 3 )
		return getValue( 0, 0 )*getValue( 1, 1 )*getValue( 2, 2 ) - 
			getValue( 0, 0 )*getValue( 1, 2 )*getValue( 2, 1 ) - 
			getValue( 1, 0 )*getValue( 0, 1 )*getValue( 2, 2 ) +
			getValue( 1, 0 )*getValue( 0, 2 )*getValue( 2, 1 ) +
			getValue( 2, 0 )*getValue( 0, 1 )*getValue( 1, 2 ) -
			getValue( 2, 0 )*getValue( 0, 2 )*getValue( 1, 1 );

	double fSum = 0.0;
	
	for ( size_t j = 0; j < cols(); ++j )
	{
		double fHead = getValue( 0, j );
		if ( fHead )
		{
			if ( j%2 )
				fHead *= -1.0;
			fHead *= minors( 0, j );
			fSum += fHead;
		}
	}
	return fSum;
}

const MatrixImpl MatrixImpl::inverse() const
{
	throwIfEmpty();
	if ( !isSquare() )
	{
		cout << "matrix size = ( " << rows() << ", " << cols() << " )" << endl;
		Debug( "inversion requires a square matrix" );
		throw MatrixSizeMismatch();
	}

	bnu::matrix<double> mxAInv( m_aArray.size1(), m_aArray.size2() );
	mxhelper::inverse( m_aArray, mxAInv );
	MatrixImpl mxInv( mxAInv );
	mxInv.setResizable( getResizable() );

	return mxInv;
}

const MatrixImpl MatrixImpl::trans() const
{
	throwIfEmpty();
	MatrixImpl m( ::boost::numeric::ublas::trans( m_aArray ) );
	m.m_bResizable = m_bResizable;
	return m;
}

double MatrixImpl::minors( size_t iSkip, size_t jSkip ) const
{
	throwIfEmpty();
	MatrixImpl m( rows() - 1, cols() - 1 );
	m.setResizable( false );
	for ( size_t i = 0; i < m.cols(); ++i )
	{
		size_t iOffset = i >= iSkip ? 1 : 0;
		for ( size_t j = 0; j < m.rows(); ++j )
		{
			size_t jOffset = j >= jSkip ? 1 : 0;
			double fVal = getValue( i + iOffset, j + jOffset );
			m.setValue( i, j, fVal );
		}
	}
	m.setResizable( true );
	return m.det();
}

bool MatrixImpl::empty() const
{
	return ( rows() == 0 && cols() == 0 );
}

bool MatrixImpl::isRowEmpty( size_t nRow ) const
{
	return mxhelper::isRowEmpty( m_aArray, nRow );
}

bool MatrixImpl::isColumnEmpty( size_t nCol ) const
{
	return mxhelper::isColumnEmpty( m_aArray, nCol );
}

bool MatrixImpl::isSameSize( const MatrixImpl& r ) const
{
	return !( rows() != r.rows() || cols() != r.cols() );
}

bool MatrixImpl::isSquare() const
{
	return rows() == cols();
}

//----------------------------------------------------------------------------
// Overloaded Operators

MatrixImpl& MatrixImpl::operator=( const MatrixImpl& r )
{
	// check for assignment to self
	if ( this == &r )
		return *this;

	MatrixImpl temp( r );
	swap( temp );
	return *this;
}

const MatrixImpl MatrixImpl::operator+( const MatrixImpl& r ) const
{
	MatrixImpl m( this );
	if ( !m.isSameSize( r ) )
	{
		Debug( "addition of mis-matched matrices" );
		throw MatrixSizeMismatch();
	}
	
	for ( size_t i = 0; i < m.rows(); ++i )
		for ( size_t j = 0; j < m.cols(); ++j )
			m.setValue( i, j, m.getValue( i, j ) + r.getValue( i, j ) );
	return m;
}

const MatrixImpl MatrixImpl::operator-( const MatrixImpl& r ) const
{
	MatrixImpl m( this );
	if ( !m.isSameSize( r ) )
	{
		Debug( "subtraction of mis-matched matrices" );
		throw MatrixSizeMismatch();
	}
	
	for ( size_t i = 0; i < m.rows(); ++i )
		for ( size_t j = 0; j < m.cols(); ++j )
			m.setValue( i, j, m.getValue( i, j ) - r.getValue( i, j ) );
	return m;
}

const MatrixImpl MatrixImpl::operator*( double fMul ) const
{
	MatrixImpl m( this );
	for ( size_t i = 0; i < m.rows(); ++i )
		for ( size_t j = 0; j < m.cols(); ++j )
			m.setValue( i, j, m.getValue( i, j )*fMul );
	return m;
}

const MatrixImpl MatrixImpl::operator*( const MatrixImpl& r ) const
{
	if ( cols() != r.rows() )
	{
		Debug( "illegal multiplication" );
		throw MatrixSizeMismatch();
	}
	
	MatrixImpl m( prod( m_aArray, r.m_aArray ) );
	return m;
}

const MatrixImpl MatrixImpl::operator/( double fDiv ) const
{
	MatrixImpl m( this );
	for ( size_t i = 0; i < m.rows(); ++i )
		for ( size_t j = 0; j < m.cols(); ++j )
			m.setValue( i, j, m.getValue( i, j )/fDiv );
	return m;
}

MatrixImpl& MatrixImpl::operator+=( const MatrixImpl& r )
{
	*this = *this + r;
	return *this;
}

MatrixImpl& MatrixImpl::operator+=( double f )
{
	MatrixImpl mx( this );
	for ( size_t i = 0; i < mx.rows(); ++i )
		for ( size_t j = 0; j < mx.cols(); ++j )
			mx.setValue( i, j, mx.getValue( i, j ) + f );
	swap( mx );
	return *this;
}

MatrixImpl& MatrixImpl::operator-=( const MatrixImpl& r )
{
	*this = *this - r;
	return *this;
}

MatrixImpl& MatrixImpl::operator*=( double f )
{
	*this = *this * f;
	return *this;
}

MatrixImpl& MatrixImpl::operator/=( double f )
{
	*this = *this / f;
	return *this;
}

double& MatrixImpl::operator()( size_t nRowId, size_t nColId )
{
	expandAsNeeded( nRowId, nColId );
	return getValue( nRowId, nColId );
}

bool MatrixImpl::operator==( const MatrixImpl& other ) const
{
	return !operator!=( other );
}

bool MatrixImpl::operator!=( const MatrixImpl& other ) const
{
	if ( cols() != other.cols() || rows() != other.rows() )
		return true;

	for ( size_t i = 0; i < rows(); ++i )
		for ( size_t j = 0; j < cols(); ++j )
			if ( getValue( i, j ) != other( i, j ) )
				return true;
	return false;
}

void MatrixImpl::expandAsNeeded( size_t nRowId, size_t nColId )
{
	if ( m_bResizable )
	{
		size_t nRowSize = m_aArray.size1();
		size_t nColSize = m_aArray.size2();
		if ( nRowId >= nRowSize || nColId >= nColSize )
		{
			size_t nNewRowSize = nRowId + 1 > nRowSize ? nRowId + 1 : nRowSize;
			size_t nNewColSize = nColId + 1 > nColSize ? nColId + 1 : nColSize;
			m_aArray.resize( nNewRowSize, nNewColSize ); // is this exception-safe?
		}
	}
}

void MatrixImpl::throwIfEmpty() const
{
	if ( empty() )
		throw OperationOnEmptyMatrix();
}

//---------------------------------------------------------------------------
// Non-member functions

const MatrixImpl operator+( const MatrixImpl& left, double right )
{
	MatrixImpl mx( left );
	mx += right;
	return mx;
}


//---------------------------------------------------------------------------
// Matrix (envelope class)

Matrix::Matrix() : m_pImpl( new MatrixImpl )
{
}

Matrix::Matrix( size_t nRow, size_t nCol, bool bIdentMatrixImpl ) : 
	m_pImpl( new MatrixImpl( nRow, nCol, bIdentMatrixImpl ) )
{
}

Matrix::Matrix( const Matrix& other ) : m_pImpl( new MatrixImpl( *other.m_pImpl.get() ) )
{
}

Matrix::Matrix( const Matrix* p ) : m_pImpl( new MatrixImpl( p->m_pImpl.get() ) )
{
}

Matrix::Matrix( const MatrixImpl& impl ) : m_pImpl( new MatrixImpl( impl ) )
{
}

Matrix::Matrix( bnu::matrix<double> m ) : m_pImpl( new MatrixImpl( m ) )
{
}

Matrix::~Matrix() throw()
{
}

void Matrix::setResizable( bool b )
{
	m_pImpl->setResizable( b );
}

void Matrix::swap( Matrix& other ) throw()
{
	m_pImpl->swap( *other.m_pImpl );
}

void Matrix::clear()
{
	m_pImpl->clear();
}

void Matrix::copy( const Matrix& other )
{
	m_pImpl->copy( *other.m_pImpl.get() );
}

Matrix Matrix::clone() const
{
	Matrix mxCloned( this );
	return mxCloned;
}

bnu::matrix< std::string > Matrix::getDisplayElements( 
		size_t nPrec, size_t nColSpace, bool bFormula ) const
{
	return m_pImpl->getDisplayElements( nPrec, nColSpace, bFormula );
}

void Matrix::print( size_t nPrec, size_t nColSpace ) const
{
	m_pImpl->print( nPrec, nColSpace );
}

const double& Matrix::getValue( size_t nRowId, size_t nColId ) const
{
	return m_pImpl->getValue( nRowId, nColId );
}

double& Matrix::getValue( size_t nRowId, size_t nColId )
{
	return m_pImpl->getValue( nRowId, nColId );
}

void Matrix::setValue( size_t nRowId, size_t nColId, double fVal )
{
	m_pImpl->setValue( nRowId, nColId, fVal );
}


Matrix Matrix::getColumn( size_t nColId )
{
	return Matrix( m_pImpl->getColumn( nColId ) );
}

void Matrix::deleteColumn( size_t nColId )
{
	m_pImpl->deleteColumn( nColId );
}
 
void Matrix::deleteColumns( const std::vector<size_t>& cnColIds )
{
	m_pImpl->deleteColumns( cnColIds );
}

void Matrix::deleteRow( size_t nRowId )
{
	m_pImpl->deleteRow( nRowId );
}

void Matrix::deleteRows( const std::vector<size_t>& cnRowIds )
{
	m_pImpl->deleteRows( cnRowIds );
}

const Matrix Matrix::adj() const
{
	return Matrix( m_pImpl->adj() );
}

double Matrix::cofactor( size_t i, size_t j ) const
{
	return m_pImpl->cofactor( i, j );
} 

double Matrix::det() const
{
	return m_pImpl->det();
}

const Matrix Matrix::inverse() const
{
	return Matrix( m_pImpl->inverse() );
}

const Matrix Matrix::trans() const
{
	return Matrix( m_pImpl->trans() );
}

double Matrix::minors( size_t iSkip, size_t jSkip ) const
{
	return m_pImpl->minors( iSkip, jSkip );
}

size_t Matrix::rows() const
{
	return m_pImpl->rows();
}

size_t Matrix::cols() const
{
	return m_pImpl->cols();
}

bool Matrix::empty() const
{
	return m_pImpl->empty();
}

bool Matrix::isRowEmpty( size_t nRow ) const
{
	return m_pImpl->isRowEmpty( nRow );
}

bool Matrix::isColumnEmpty( size_t nCol ) const
{
	return m_pImpl->isColumnEmpty( nCol );
}

bool Matrix::isSameSize( const Matrix& r ) const
{
	return m_pImpl->isSameSize( *r.m_pImpl.get() );
}

bool Matrix::isSquare() const
{
	return m_pImpl->isSquare();
}

//----------------------------------------------------------------------------
// Overloaded Operators

Matrix& Matrix::operator=( const Matrix& r )
{
	m_pImpl.reset( new MatrixImpl( *r.m_pImpl.get() ) );
	return *this;
}

const Matrix Matrix::operator+( const Matrix& r ) const
{
	return Matrix( m_pImpl->operator+( *r.m_pImpl.get() ) );
}

const Matrix Matrix::operator-( const Matrix& r ) const
{
	return Matrix( m_pImpl->operator-( *r.m_pImpl.get() ) );
}

const Matrix Matrix::operator*( double fMul ) const
{
	return Matrix( m_pImpl->operator*( fMul ) );
}

const Matrix Matrix::operator*( const Matrix& r ) const
{
	return Matrix( m_pImpl->operator*( *r.m_pImpl.get() ) );
}

const Matrix Matrix::operator/( double fDiv ) const
{
	return Matrix( m_pImpl->operator/( fDiv ) );
}

Matrix& Matrix::operator+=( const Matrix& r )
{
	*this = *this + r;
	return *this;
}

Matrix& Matrix::operator+=( double f )
{
	m_pImpl->operator+=( f );
	return *this;
}

Matrix& Matrix::operator-=( const Matrix& r )
{
	*this = *this - r;
	return *this;
}

Matrix& Matrix::operator*=( double f )
{
	*this = *this * f;
	return *this;
}

Matrix& Matrix::operator/=( double f )
{
	*this = *this / f;
	return *this;
}

double& Matrix::operator()( size_t nRowId, size_t nColId )
{
	return m_pImpl->operator()( nRowId, nColId );
}

bool Matrix::operator==( const Matrix& other ) const
{
	return m_pImpl->operator==( *other.m_pImpl.get() );
}

bool Matrix::operator!=( const Matrix& other ) const
{
	return m_pImpl->operator!=( *other.m_pImpl.get() );
}


const Matrix operator+( const Matrix& mx, double f )
{
	Matrix mxAdded( mx );
	mxAdded += f;
	return mxAdded;
}

const Matrix operator+( double f, const Matrix& mx )
{
	return mx + f;
}

}}

