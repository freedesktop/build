#include <vbacollectionimpl.hxx>
#include "vbaglobals.hxx"
using namespace ::com::sun::star;
using namespace ::org::openoffice;


ScVbaCollectionBaseImpl::ScVbaCollectionBaseImpl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< container::XIndexAccess >& xIndexAccess ) throw (uno::RuntimeException) : m_xContext( xContext ), m_xIndexAccess( xIndexAccess )
{
	m_xNameAccess.set( xIndexAccess, uno::UNO_QUERY );
}

uno::Any
ScVbaCollectionBaseImpl::getItemByStringIndex( const rtl::OUString& sIndex ) throw (::uno::RuntimeException)
{
	if ( !m_xNameAccess.is() )
		throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ScVbaCollectionBaseImpl string index access not supported by this object") ), uno::Reference< uno::XInterface >() );

	return createCollectionObject( m_xNameAccess->getByName( sIndex ) );	
}

uno::Any 
ScVbaCollectionBaseImpl::getItemByIntIndex( const sal_Int32 nIndex ) throw (uno::RuntimeException)
{
	if ( !m_xIndexAccess.is() )
		throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ScVbaCollectionBaseImpl numeric index access not supported by this object") ), uno::Reference< uno::XInterface >() );
	if ( nIndex <= 0 )
	{
		throw  lang::IndexOutOfBoundsException( 
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			"index is 0 or negative" ) ), 
			uno::Reference< uno::XInterface >() );
	}
	// need to adjust for vba index ( for which first element is 1 )
	return createCollectionObject( m_xIndexAccess->getByIndex( nIndex - 1 ) );	
}

::sal_Int32 SAL_CALL 
ScVbaCollectionBaseImpl::getCount() throw (uno::RuntimeException)
{
	return m_xIndexAccess->getCount();
}

uno::Any SAL_CALL 
ScVbaCollectionBaseImpl::getParent() throw (uno::RuntimeException)
{
	// #TODO #FIXME investigate whether this makes sense
	uno::Reference< vba::XApplication > xApplication =
		ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
	uno::Reference< vba::XWorkbook > xWorkbook;
	if ( xApplication.is() )
	{
		xWorkbook = xApplication->getActiveWorkbook();
	}
	return uno::Any( xWorkbook );
}

::sal_Int32 SAL_CALL ScVbaCollectionBaseImpl::getCreator() throw (uno::RuntimeException)
{
	// #TODO #FIXE some sort of implementation
	//SC_VBA_STUB();
	return 0;
}
uno::Reference< vba::XApplication > SAL_CALL ScVbaCollectionBaseImpl::getApplication() throw (uno::RuntimeException)
{
	// #TODO #FIXME investigate whether this makes sense
	uno::Reference< vba::XApplication > xApplication =
		ScVbaGlobals::getGlobalsImpl( m_xContext )->getApplication();
	return  xApplication;
}

uno::Any SAL_CALL ScVbaCollectionBaseImpl::Item( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
	if ( aIndex.getValueTypeClass() != uno::TypeClass_STRING )
	{
		sal_Int32 nIndex = 0;

		if ( ( aIndex >>= nIndex ) != sal_True )
		{
			rtl::OUString message;
			message = rtl::OUString::createFromAscii(
				"Couldn't convert index to Int32");
			throw  lang::IndexOutOfBoundsException( message,
				uno::Reference< uno::XInterface >() );
		}
		return 	getItemByIntIndex( nIndex );
	}
	rtl::OUString aStringSheet;

	aIndex >>= aStringSheet;
	return getItemByStringIndex( aStringSheet );	
}

::rtl::OUString SAL_CALL 
ScVbaCollectionBaseImpl::getDefaultMethodName(  ) throw (css::uno::RuntimeException)
{
	const static rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM("Item") );
	return sName;
}

	// XElementAccess

::sal_Bool SAL_CALL ScVbaCollectionBaseImpl::hasElements() throw (uno::RuntimeException)
{
	return ( m_xIndexAccess->getCount() > 0 );
}


