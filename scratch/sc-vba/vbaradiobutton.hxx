#ifndef SC_VBA_RADIOBUTTON_HXX
#define SC_VBA_RADIOBUTTON_HXX
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/msforms/XRadioButton.hpp>

#include "vbacontrol.hxx"
#include "vbahelper.hxx"

typedef cppu::ImplInheritanceHelper1< ScVbaControl, oo::msforms::XRadioButton > RadioButtonImpl_BASE;

class ScVbaRadioButton : public RadioButtonImpl_BASE
{
public:
    ScVbaRadioButton( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
   // Attributes
    virtual rtl::OUString SAL_CALL getCaption() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setCaption( const rtl::OUString& _caption ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getValue() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setValue( sal_Bool _value ) throw (css::uno::RuntimeException);

};
#endif //SC_VBA_RADIOBUTTON_HXX
