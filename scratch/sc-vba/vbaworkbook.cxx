#include <cppuhelper/queryinterface.hxx>
#include <tools/urlobj.hxx>

#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>

#include "vbaworksheet.hxx"
#include "vbaworkbook.hxx"

// Much of the impl. for the equivalend UNO module is
// sc/source/ui/unoobj/docuno.cxx, viewuno.cxx

using namespace ::org::openoffice;
using namespace ::com::sun::star;

// XInterface implementation
uno::Any ScVbaWorkbook::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XWorkbook * >( this ) ),
			 static_cast< vba::XWorkbook * >( this ) );
}

::rtl::OUString
ScVbaWorkbook::getName() throw (uno::RuntimeException)
{
	INetURLObject aURL( mxModel->getURL() );
	return aURL.GetLastName();
}
::rtl::OUString
ScVbaWorkbook::getPath() throw (uno::RuntimeException)
{
	INetURLObject aURL( mxModel->getURL() );
	return aURL.GetURLPath();
}

::rtl::OUString
ScVbaWorkbook::getFullName() throw (uno::RuntimeException)
{
	// FIXME: what is the difference here ?
	return getName();
}

uno::Reference< vba::XWorksheet >
ScVbaWorkbook::getActiveSheet() throw (uno::RuntimeException)
{
	uno::Reference< sheet::XSpreadsheetView > xSpreadsheet(
			mxModel->getCurrentController(), uno::UNO_QUERY_THROW );

	return uno::Reference< vba::XWorksheet >( new ScVbaWorksheet( xSpreadsheet->getActiveSheet() ) );
}

void
ScVbaWorkbook::close( const uno::Any &rSaveArg, const uno::Any &rFileArg,
					  const uno::Any &rRouteArg ) throw (uno::RuntimeException)
{
	sal_Bool bSaveChanges = sal_False;
	rtl::OUString aFileName( RTL_CONSTASCII_USTRINGPARAM( "BeforeDefault" ) );
	sal_Bool bRouteWorkbook = sal_True;

	rSaveArg >>= bSaveChanges;
	rFileArg >>= aFileName;
	rRouteArg >>= bRouteWorkbook;

	rtl::OString aDbgType = rtl::OUStringToOString( rFileArg.getValueTypeName(),
													RTL_TEXTENCODING_UTF8 );
	rtl::OString aDbgFileName = rtl::OUStringToOString( aFileName, RTL_TEXTENCODING_UTF8 );
	fprintf( stderr, "Close '%d' '%s'('%s') '%d' \n", bSaveChanges,
			 (const sal_Char *)aDbgFileName, (const sal_Char *)aDbgType,
			 bRouteWorkbook );
			 

//	uno::Reference< util::XCloseable > xCloseable( mxModel, uno::UNO_QUERY_THROW );
	// FIXME: ignores 3 arguments.
//	xCloseable->close( FALSE );
}

void
ScVbaWorkbook::protect() throw (uno::RuntimeException)
{
	uno::Reference< util::XProtectable > xProt( mxModel, uno::UNO_QUERY_THROW );
	// FIXME: password ?
	xProt->protect( rtl::OUString() );
}

::sal_Bool
ScVbaWorkbook::getProtectStructure() throw (uno::RuntimeException)
{
	uno::Reference< util::XProtectable > xProt( mxModel, uno::UNO_QUERY_THROW );
	return xProt->isProtected();
}
