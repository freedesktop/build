#ifndef SC_VBA_OLEOBJECTS_HXX
#define SC_VBA_OLEOBJECTS_HXX

#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/excel/XOLEObjects.hpp>

#include "vbacollectionimpl.hxx"
#include "vbahelper.hxx"

typedef CollTestImplHelper< oo::excel::XOLEObjects > OLEObjectsImpl_BASE;

class ScVbaOLEObjects : public OLEObjectsImpl_BASE
{
protected:
    virtual css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
public:
    ScVbaOLEObjects( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess );

    // XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // ScVbaCollectionBaseImpl
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    
};
#endif //SC_VBA_OLEOBJECTS_HXX

