#ifndef SC_VBA_XFILLFORMAT_HXX
#define SC_VBA_XFILLFORMAT_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <org/openoffice/msforms/XFillFormat.hpp>
#include "vbahelperinterface.hxx"

typedef InheritedHelperInterfaceImpl1< oo::msforms::XFillFormat > ScVbaFillFormat_BASE;

class ScVbaFillFormat : public ScVbaFillFormat_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
    css::uno::Reference< css::beans::XPropertySet > m_xPropertySet;
    css::uno::Reference< oo::msforms::XColorFormat > m_xColorFormat;
    css::drawing::FillStyle m_nFillStyle;
    sal_Int32 m_nForeColor;
    sal_Int32 m_nBackColor;
    sal_Int16 m_nGradientAngle;
private:
    void setFillStyle( css::drawing::FillStyle nFillStyle ) throw (css::uno::RuntimeException);
protected:
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

public:
    ScVbaFillFormat( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape );

    void setForeColorAndInternalStyle( sal_Int32 nForeColor ) throw (css::uno::RuntimeException);
    // Attributes
    virtual sal_Bool SAL_CALL getVisible() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setVisible( sal_Bool _visible ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getTransparency() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTransparency( double _transparency ) throw (css::uno::RuntimeException);

    // Methods
    virtual void SAL_CALL Solid() throw (css::uno::RuntimeException);
    virtual void SAL_CALL TwoColorGradient( sal_Int32 style, sal_Int32 variant ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< oo::msforms::XColorFormat > SAL_CALL BackColor() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< oo::msforms::XColorFormat > SAL_CALL ForeColor() throw (css::uno::RuntimeException);

};

#endif//SC_VBA_XFILLFORMAT_HXX
