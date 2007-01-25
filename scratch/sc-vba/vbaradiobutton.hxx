#ifndef SC_VBA_RADIOBUTTON_HXX
#define SC_VBA_RADIOBUTTON_HXX
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XRadioButton.hpp>
#include <comphelper/propertycontainer.hxx>

#include "vbacontrol.hxx"
#include "vbahelper.hxx"

typedef cppu::ImplInheritanceHelper1< ScVbaControl, oo::vba::XRadioButton > RadioButtonImpl_BASE;

class ScVbaRadioButton : public RadioButtonImpl_BASE
    ,public ::comphelper::OPropertyContainer
{
public:
    ScVbaRadioButton( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
   // Attributes
    virtual rtl::OUString SAL_CALL getCaption() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setCaption( const rtl::OUString& _caption ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getValue() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setValue( sal_Bool _value ) throw (css::uno::RuntimeException);

    // XPropertySet
    virtual css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (css::uno::RuntimeException);

    // XInterface
    DECLARE_XINTERFACE()

    // XTypeProvider
    DECLARE_XTYPEPROVIDER()

protected:
    // OPropertySetHelper
    virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();

    virtual ::cppu::IPropertyArrayHelper* createArrayHelper() const;

};
#endif //SC_VBA_RADIOBUTTON_HXX
