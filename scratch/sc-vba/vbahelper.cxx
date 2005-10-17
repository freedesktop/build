#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include <sfx2/objsh.hxx>
#include <sfx2/app.hxx>

#include <basic/sbx.hxx>

#include "vbahelper.hxx"
#include "tabvwsh.hxx"
#include "transobj.hxx"

using namespace ::com::sun::star;
using namespace ::org::openoffice;
void unoToSbxValue( SbxVariable* pVar, const uno::Any& aValue );
uno::Any sbxToUnoValue( SbxVariable* pVar );

void
org::openoffice::dispatchRequests (uno::Reference< frame::XModel>& xModel,rtl::OUString & aUrl) 
{

	util::URL  url ;
	url.Complete = aUrl;
	rtl::OUString emptyString = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "" ));
	uno::Reference<frame::XController> xController = xModel->getCurrentController();
	uno::Reference<frame::XFrame> xFrame = xController->getFrame();
	uno::Reference<frame::XDispatchProvider> xDispatchProvider (xFrame,uno::UNO_QUERY_THROW);
	try
	{
		uno::Reference<uno::XComponentContext > xContext(  ::cppu::defaultBootstrap_InitialComponentContext());
		if ( !xContext.is() )
		{
			return ;
		}

		uno::Reference<lang::XMultiComponentFactory > xServiceManager(
				xContext->getServiceManager() );
		if ( !xServiceManager.is() )
		{
			return ;
		}
		uno::Reference<util::XURLTransformer> xParser( xServiceManager->createInstanceWithContext(     rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.util.URLTransformer" ) )
			,xContext), uno::UNO_QUERY_THROW );
		if (!xParser.is())
			return;
		xParser->parseStrict (url);
	}
	catch ( ::cppu::BootstrapException & e )
	{
		return ;
	}
	catch ( uno::Exception & e )
	{
		return ;
	}

	uno::Reference<frame::XDispatch> xDispatcher = xDispatchProvider->queryDispatch(url,emptyString,0);
	uno::Sequence<beans::PropertyValue> sProps ;
	sProps.realloc(2);
        beans::PropertyValue * aArray = sProps.getArray();
	aArray[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Silent" ));
	uno::Any aAny;
	sal_Bool isTrue = true;
	aAny <<= isTrue;

	aArray[0].Value = aAny;
	//aArray[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "StatusIndicator" ));
	//aArray[0].Value = true;

	if (xDispatcher.is())
		//xDispatcher->dispatch( url,uno::Sequence< beans::PropertyValue >(0) );
		xDispatcher->dispatch( url,sProps );
}


void
org::openoffice::implnPaste()
{
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	pViewShell->PasteFromSystem();
	pViewShell->CellContentChanged();
}


void
org::openoffice::implnCopy()
{
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	pViewShell->CopyToClip(NULL,false,false,true);
}

void 
org::openoffice::implnCut()
{
	ScTabViewShell* pViewShell = ScTabViewShell::GetActiveViewShell();
	pViewShell->CutToClip( NULL, TRUE );
}

void org::openoffice::implnPasteSpecial(USHORT nFlags,USHORT nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose)
{
	sal_Bool bAsLink(sal_False), bOtherDoc(sal_False);
	InsCellCmd eMoveMode = INS_NONE;

	ScTabViewShell* pTabViewShell = ScTabViewShell::GetActiveViewShell();
	Window* pWin = pTabViewShell->GetViewData()->GetActiveWin();
	ScDocument* pDoc = pTabViewShell->GetViewData()->GetDocument();
	ScTransferObj* pOwnClip = ScTransferObj::GetOwnClipboard( pWin );

	if ( bAsLink && bOtherDoc )
		pTabViewShell->PasteFromSystem(0);//SOT_FORMATSTR_ID_LINK
	else {
		pTabViewShell->PasteFromClip( nFlags, pOwnClip->GetDocument(),
		nFunction, bSkipEmpty, bTranspose, bAsLink,
		eMoveMode, IDF_NONE, TRUE );
		pTabViewShell->CellContentChanged();
		}
}

bool
org::openoffice::isRangeShortCut( const ::rtl::OUString& sParam )
{
        // for a ShortCutRange param, I'd expect the first letter to be
        //[A-Z] and the last letter to be a digit 0-9 e.g A10, [A1:A10] etc.
        ::rtl::OString sStr = rtl::OUStringToOString( sParam,
                RTL_TEXTENCODING_UTF8 );
        const sal_Char* pFirst = (const sal_Char*)sStr;
        const sal_Char* pLast = pFirst + ( sStr.getLength() - 1 );
        if ( (( *pFirst >= 'A' && *pFirst <= 'Z' ) || ( *pFirst >= 'a' && *pFirst <= 'Z' ) )  )
                if ( ( *pLast >= '0' ) && ( *pLast <= '9' ) )
                        return true;

        return false;
}

 uno::Reference< frame::XModel > 
org::openoffice::getCurrentDocument() throw (uno::RuntimeException)
{
	uno::Reference< frame::XModel > xModel;
	SbxObject* pBasic = reinterpret_cast< SbxObject* > ( SFX_APP()->GetBasic() );
	SbxObject* basicChosen =  pBasic ;
	if ( basicChosen == NULL)
	{
		OSL_TRACE("getModelFromBasic() StarBASIC* is NULL" );
		return xModel;
	}    
    SbxObject* p = pBasic;
    SbxObject* pParent = p->GetParent();
    SbxObject* pParentParent = pParent ? pParent->GetParent() : NULL;

    if( pParentParent )
    {
        basicChosen = pParentParent;
    }
    else if( pParent )
    {
        basicChosen = pParent;
    }


    uno::Any aModel; 
    SbxVariable *pCompVar = basicChosen->Find(  UniString(RTL_CONSTASCII_USTRINGPARAM("ThisComponent")), SbxCLASS_OBJECT );

	if ( pCompVar )
	{
		aModel = sbxToUnoValue( pCompVar );
		if ( sal_False == ( aModel >>= xModel ) ||
			!xModel.is() )
		{
			OSL_TRACE("Failed to extract model from thisComponent ");
			return xModel;
		}
		else
		{
			OSL_TRACE("Have model ThisComponent points to url %s",
			::rtl::OUStringToOString( xModel->getURL(),
				RTL_TEXTENCODING_ASCII_US ).pData->buffer );
		}
	}
	else
	{
		OSL_TRACE("Failed to get ThisComponent");
	}
	return xModel;
}

