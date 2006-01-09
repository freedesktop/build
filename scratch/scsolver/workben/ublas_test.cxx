#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <exception>

using boost::numeric::ublas::matrix;
namespace bnu = ::boost::numeric::ublas;
using namespace ::std;

typedef bnu::matrix<double> Mx;
typedef bnu::identity_matrix<double> MxIdentity;
typedef bnu::matrix_range< Mx > MxRange;
typedef bnu::matrix_slice< Mx > MxSlice;
		
class NonSquareMatrix : public ::std::exception {};

template<typename Data>
void printElements( const std::vector<Data>& cn, const char* sep = "" )
{
	copy( cn.begin(), cn.end(), ostream_iterator<Data>( cout, sep ) );
	cout << endl;
}

/** Read the content of a file and stores it as an array of lines 
	as strings. */
std::vector<string> readlines( const char* filename )
{
	ifstream file( filename );
	if ( !file )
	{
		cerr << "can't open file " << filename << endl;
		exit( EXIT_FAILURE );
	}
	
	char c;
	string s = "";
	std::vector<string> vs;
	while( file.get(c) )
	{
		if ( c == '\n' )
		{
			vs.push_back( s );
			s.clear();
		}
		else
			s += c;
	}
	
	if ( s.length() > 0 )
		vs.push_back( s );
		
	return vs;
}

/** Function object for converting strings into a matrix */
class ToMatrix : public unary_function< std::vector<double>, void >
{
public:
	ToMatrix() : m_nRow( 0 ), m_pMx( NULL ) {}
	~ToMatrix() throw() {}

	void setMatrixRef( matrix<double>* p )
	{
		m_nRow = 0;
		m_pMx = p;
	}
	
	void operator()( const std::vector<double>& cn )
	{
		std::vector<double>::const_iterator it, itBeg = cn.begin(), itEnd = cn.end();
		for ( it = itBeg; it != itEnd; ++it )
			m_pMx->operator()( m_nRow, distance( itBeg, it ) ) = *it;
		++m_nRow;
	}
	
private:
	size_t m_nRow;
	matrix<double>* m_pMx;
};

matrix<double> parseMatrix( const std::vector<string>& cnLines )
{
	using namespace boost::spirit;

	std::vector< std::vector<double> > cnRows;
	size_t nMaxSize = 0;
	std::vector<string>::const_iterator it, itBeg = cnLines.begin(), itEnd = cnLines.end();
	for ( it = itBeg; it != itEnd; ++it )
	{
		std::vector<double> cnRow;
		bool bSuccess = boost::spirit::parse( (*it).c_str(),
			(
				*space_p >> real_p[push_back_a(cnRow)] >> 
					*( +space_p >> real_p[push_back_a(cnRow)] ) >> *space_p
			),
			nothing_p
		).full;
		
		if ( !bSuccess )
		{
			cout << "parse failed" << endl;
			break;
		}

		cnRows.push_back( cnRow );
		nMaxSize = nMaxSize < cnRow.size() ? cnRow.size() : nMaxSize;
	}
	
	std::auto_ptr< matrix<double> > mx( new matrix<double>( cnRows.size(), nMaxSize ) );
	mx->clear();
	ToMatrix objFunc;
	objFunc.setMatrixRef( mx.get() );
	for_each( cnRows.begin(), cnRows.end(), objFunc );

	return *mx;
}

void swapRows( Mx& A, size_t nRow1, size_t nRow2 )
{
	bnu::matrix_row< Mx > row1( A, nRow1 );
	bnu::matrix_row< Mx > row2( A, nRow2 );
	row1.swap( row2 );
}

void factorize( const Mx& mxA, Mx& mxL, Mx& mxU, Mx& mxP )
{
	if ( mxA.size1() != mxA.size2() )
	{
		cerr << "non-square matrix" << endl;
		return;
	}

	size_t n = mxA.size1();
	Mx A( mxA );
	MxIdentity I( A.size1(), A.size2() );
	Mx P( I );

	for ( unsigned int k = 0; k < n; ++k )
	{
		// Partial pivoting ( get column k and scan row(s) k:n )
		if ( n - k > 1 )
		{
			bnu::matrix_column< Mx > cl( A, k );
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
		{
			cerr << "pivot " << k << " is zero" << endl;
			return;
		}

		MxRange mr1( A, bnu::range( k + 1, n ), bnu::range( k, k + 1 ) );
		mr1 /= fPivot;
		MxRange mr2( A, bnu::range( k + 1, n ), bnu::range( k, k + 1 ) );
		MxRange mr3( A, bnu::range( k, k + 1 ), bnu::range( k + 1, n ) );
		MxRange mr4( A, bnu::range( k + 1, n ), bnu::range( k + 1, n ) );
		mr4 -= prod( mr2, mr3 );
	}
	
	// Transfer elements from A into L and U
	Mx L( I ), U( n, n );
	for ( unsigned int i = 0; i < n; ++i )
		for ( unsigned int j = 0; j < n; ++j )
		{
			if ( i > j )
				L( i, j ) = A( i, j );
			else
				U( i, j ) = A( i, j );
		}

	noalias( mxU ) = U;
	noalias( mxL ) = L;
	noalias( mxP ) = P;
}

void inverseU( const Mx& mxU, Mx& mxUInv )
{
	size_t nSize = mxU.size1();
	if ( nSize != mxU.size2() )
		throw NonSquareMatrix();

	Mx Combo( nSize, nSize*2 );
	MxRange U( Combo, bnu::range( 0, nSize ), bnu::range( 0, nSize ) );
	MxRange UInv( Combo, bnu::range( 0, nSize ), bnu::range( nSize, nSize*2 ) );
	MxIdentity I( nSize );
	U = mxU;
	UInv = I;
	
	// back substitution
	size_t nRow = nSize;
	do
	{
		// normalize the current row first
		double fPivot = U( --nRow, nRow );
		bnu::matrix_row< Mx > mrCur( Combo, nRow );
		mrCur /= fPivot;
		U( nRow, nRow ) = 1.0; // maybe redundant because it should already be 1.0
		
		// substitute upward
		if ( nRow > 0 )
		{
			size_t nSubRow = nRow;
			do
			{
				bnu::matrix_row< Mx > mrSub( Combo, --nSubRow );
				mrSub -= mrCur*U( nSubRow, nRow );
			}
			while ( nSubRow != 0 );
		}	
	}
	while ( nRow != 0 );
	
	noalias( mxUInv ) = UInv;
}

void inverseL( const Mx& mxL, Mx& mxLInv )
{
	size_t nSize = mxL.size1();
	if ( nSize != mxL.size2() )
		throw NonSquareMatrix();

	Mx Combo( nSize, nSize*2 );
	MxRange L( Combo, bnu::range( 0, nSize ), bnu::range( 0, nSize ) );
	MxRange LInv( Combo, bnu::range( 0, nSize ), bnu::range( nSize, nSize*2 ) );
	MxIdentity I( nSize );
	L = mxL;
	LInv = I;

	// forward substitution
	size_t nRow = 0;
	do
	{
		bnu::matrix_row< Mx > mrCur( Combo, nRow );
		
		// substitute downward
		if ( nRow < nSize - 1 )
		{
			size_t nSubRow = nRow + 1;
			do
			{
				bnu::matrix_row< Mx > mrSub( Combo, nSubRow );
				mrSub -= mrCur*L( nSubRow, nRow );
			}
			while ( ++nSubRow < nSize );
		}
	}
	while ( ++nRow < nSize );
	
	noalias( mxLInv ) = LInv;
}

void inverse( const matrix<double>& mxA, matrix<double>& mxInv )
{
	size_t nRow = mxA.size1(), nCol = mxA.size2();
	if ( nRow != nCol )
		throw NonSquareMatrix();
		
	size_t nSize = nRow;
		
	Mx mxL( nSize, nSize );
	Mx mxU( nSize, nSize );
	Mx mxP( nSize, nSize );
	factorize( mxA, mxL, mxU, mxP );
	cout << "U = " << mxU << endl;
	cout << "L = " << mxL << endl;
	cout << "LU = " << prod( mxL, mxU ) << endl;
	cout << "PA = " << prod( mxP, mxA ) << endl;
	
	Mx mxUInv( nSize, nSize );
	inverseU( mxU, mxUInv );
	cout << "U-1 = " << mxUInv << endl;
	
	Mx mxLInv( nSize, nSize );
	inverseL( mxL, mxLInv );
	cout << "L-1 = " << mxLInv << endl;
	
	Mx mxTemp = prod( mxUInv, mxLInv );
	mxTemp = prod( mxTemp, mxP );
	noalias( mxInv ) = mxTemp;
}

int main( int argc, char** argv )
{
	std::vector<string> cnLines;
	if ( argc < 2 )
		return EXIT_FAILURE;
	
	cnLines = readlines( argv[1] );
	matrix<double> mx1 = parseMatrix( cnLines );	
	cout << mx1 << endl;

	matrix<double> mxInv( mx1.size1(), mx1.size2() );
	inverse( mx1, mxInv );
	
	cout << "A-1 = " << mxInv << endl;
	cout << prod( mx1, mxInv ) << endl;
	
	return EXIT_SUCCESS;
}


