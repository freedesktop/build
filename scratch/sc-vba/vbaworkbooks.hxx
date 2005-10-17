#ifndef SC_VBA_WORKBOOKS_HXX
#define SC_VBA_WORKBOOKS_HXX

#include <cppuhelper/implbase2.hxx>

#include <org/openoffice/vba/XGlobals.hpp>
#include <org/openoffice/vba/XWorkbooks.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper2< oo::vba::XWorkbooks,
        css::container::XEnumerationAccess > ScVbaWorkbooks_BASE;

class ScVbaWorkbooks : public ScVbaWorkbooks_BASE
{
private:
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	css::uno::Reference< oo::vba::XGlobals > getGlobals() throw (css::uno::RuntimeException);
	
public:
	ScVbaWorkbooks( css::uno::Reference< css::uno::XComponentContext >& xContext ): m_xContext( xContext ) {}
	virtual ~ScVbaWorkbooks() {}

	// XEnumerationAccess
	virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL hasElements() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

	// XCollection
    virtual ::sal_Int32 SAL_CALL getCount() throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL getParent() throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL getCreator() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< oo::vba::XApplication > SAL_CALL getApplication() throw (css::uno::RuntimeException);
    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);

	// XWorkbooks
    virtual css::uno::Any SAL_CALL Add() throw (css::uno::RuntimeException);
    virtual void SAL_CALL Close(  ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL Open(const css::uno::Any &aFileName) throw (css::uno::RuntimeException);
private:
        css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);

        css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException);
};

#endif /* SC_VBA_WORKBOOKS_HXX */
