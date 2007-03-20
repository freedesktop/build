#ifndef SC_VBA_LABEL_HXX
#define SC_VBA_LABEL_HXX
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/msforms/XLabel.hpp>

#include "vbacontrol.hxx"
#include "vbahelper.hxx"

typedef cppu::ImplInheritanceHelper1< ScVbaControl, oo::msforms::XLabel > LabelImpl_BASE;

class ScVbaLabel : public LabelImpl_BASE
{
public:
    ScVbaLabel( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
   // Attributes
    virtual rtl::OUString SAL_CALL getCaption() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setCaption( const rtl::OUString& _caption ) throw (css::uno::RuntimeException);
};
#endif //SC_VBA_LABEL_HXX
