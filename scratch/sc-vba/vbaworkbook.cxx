#include <tools/urlobj.hxx>

#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/frame/XFrame.hpp>

#include "vbaworksheet.hxx"
#include "vbaworksheets.hxx"
#include "vbaworkbook.hxx"
#include "vbahelper.hxx"

#include <stdio.h>

// Much of the impl. for the equivalend UNO module is
// sc/source/ui/unoobj/docuno.cxx, viewuno.cxx

using namespace ::org::openoffice;
using namespace ::com::sun::star;

class ActiveSheet : public ScVbaWorksheet
{
protected:
	virtual uno::Reference< frame::XModel > getModel()
	{ 	
		return getCurrentDocument(); 
	}
	virtual uno::Reference< sheet::XSpreadsheet > getSheet()
	{ 
		uno::Reference< frame::XModel > xModel = getModel();
		uno::Reference< sheet::XSpreadsheet > xSheet;
		if ( xModel.is() )
		{
			uno::Reference< sheet::XSpreadsheetView > xSpreadsheet(
                        	xModel->getCurrentController(), uno::UNO_QUERY );
			if ( xSpreadsheet.is() )
				xSheet = xSpreadsheet->getActiveSheet(); 
		}
		return xSheet;
	}
public:
	ActiveSheet( uno::Reference< uno::XComponentContext >& xContext ) : ScVbaWorksheet( xContext ) {}
		
};


::rtl::OUString
ScVbaWorkbook::getName() throw (uno::RuntimeException)
{
	INetURLObject aURL( getModel()->getURL() );
	return aURL.GetLastName();
}
::rtl::OUString
ScVbaWorkbook::getPath() throw (uno::RuntimeException)
{
	INetURLObject aURL( getModel()->getURL() );
	aURL.CutLastName();
	return aURL.GetURLPath();
}

::rtl::OUString
ScVbaWorkbook::getFullName() throw (uno::RuntimeException)
{
	INetURLObject aURL( getModel()->getURL() );
	return aURL.GetURLPath();
}
uno::Reference< vba::XWorksheet >
ScVbaWorkbook::getActiveSheet() throw (uno::RuntimeException)
{
	return new ActiveSheet( m_xContext );
}
uno::Any SAL_CALL
ScVbaWorkbook::Worksheets( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	uno::Reference< vba::XWorksheets > xWorkSheets( new ScVbaWorksheets(m_xContext,
		getModel()) );
	if (  aIndex.getValueTypeClass() == uno::TypeClass_VOID )
	{
		return uno::Any( xWorkSheets );	
	}
	return uno::Any( xWorkSheets->Item( aIndex ) );
}

void
ScVbaWorkbook::Close( const uno::Any &rSaveArg, const uno::Any &rFileArg,
					  const uno::Any &rRouteArg ) throw (uno::RuntimeException)
{
	sal_Bool bSaveChanges = sal_False;
	rtl::OUString aFileName;
	sal_Bool bRouteWorkbook = sal_True;

	rSaveArg >>= bSaveChanges;
	sal_Bool bFileName =  ( rFileArg >>= aFileName );
	rRouteArg >>= bRouteWorkbook;
	uno::Reference< frame::XStorable > xStorable( getModel(), uno::UNO_QUERY_THROW );
	uno::Reference< util::XModifiable > xModifiable( getModel(), uno::UNO_QUERY_THROW );

	if( bSaveChanges )
	{
		if( xStorable->isReadonly() )
		{	
			throw uno::RuntimeException(::rtl::OUString( 
				RTL_CONSTASCII_USTRINGPARAM( "Unable to save to a read only file ") ),
                        	uno::Reference< XInterface >() );
		}
		if( bFileName )
			xStorable->storeAsURL( aFileName, uno::Sequence< beans::PropertyValue >(0) ); 
		else
			xStorable->store();
	}	
	else
		xModifiable->setModified( false );		
	
	rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:CloseDoc"));
	uno::Reference< frame::XModel > xModel = getModel();
	dispatchRequests(xModel,url);
}

void
ScVbaWorkbook::Protect( const uno::Any &aPassword ) throw (uno::RuntimeException)
{
	rtl::OUString rPassword;
	uno::Reference< util::XProtectable > xProt( getModel(), uno::UNO_QUERY_THROW );
	SC_VBA_FIXME(("Workbook::Protect stub"));
	if(  aPassword >>= rPassword )
		xProt->protect( rPassword );
	else
		xProt->protect( rtl::OUString() );
}

void 
ScVbaWorkbook::Unprotect( const uno::Any &aPassword ) throw (uno::RuntimeException)
{
	rtl::OUString rPassword;
	uno::Reference< util::XProtectable > xProt( getModel(), uno::UNO_QUERY_THROW );
	if( !getProtectStructure() )
		throw uno::RuntimeException(::rtl::OUString(
			RTL_CONSTASCII_USTRINGPARAM( "File is already unprotected" ) ),
			uno::Reference< XInterface >() );
	else
	{
		if( aPassword >>= rPassword )
			xProt->unprotect( rPassword );
		else
			xProt->unprotect( rtl::OUString() );
	}
}
		
::sal_Bool
ScVbaWorkbook::getProtectStructure() throw (uno::RuntimeException)
{
	uno::Reference< util::XProtectable > xProt( getModel(), uno::UNO_QUERY_THROW );
	return xProt->isProtected();
}

void 
ScVbaWorkbook::setSaved( sal_Bool bSave ) throw (uno::RuntimeException)
{
	uno::Reference< util::XModifiable > xModifiable( getModel(), uno::UNO_QUERY_THROW );
	xModifiable->setModified( bSave );
}

sal_Bool
ScVbaWorkbook::getSaved() throw (uno::RuntimeException)
{
	uno::Reference< util::XModifiable > xModifiable( getModel(), uno::UNO_QUERY_THROW );
	return xModifiable->isModified();
}

void
ScVbaWorkbook::Save() throw (uno::RuntimeException)
{
	rtl::OUString url = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(".uno:Save"));
	uno::Reference< frame::XModel > xModel = getModel();
	dispatchRequests(xModel,url);
}

void 
ScVbaWorkbook::Activate() throw (uno::RuntimeException)
{
	uno::Reference< frame::XFrame > xFrame( getModel()->getCurrentController()->getFrame(), uno::UNO_QUERY_THROW );
	xFrame->activate();
}	

