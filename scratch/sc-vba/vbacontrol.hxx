#ifndef SC_VBA_CONTROL_HXX
#define SC_VBA_CONTROL_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <org/openoffice/msforms/XControl.hpp>

#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< oo::msforms::XControl > ControlImpl_BASE;

class ScVbaControl : public ControlImpl_BASE
{
/* this will cause error when the sheet is invisible.
private:
    css::uno::Reference< css::awt::XWindowPeer > m_xWindowPeer;
*/
private:
    css::uno::Reference< css::awt::XWindowPeer > getWindowPeer( const css::uno::Reference< css::drawing::XControlShape >& xControlShape ) throw (css::uno::RuntimeException);
    com::sun::star::uno::Reference< com::sun::star::lang::XEventListener > m_xEventListener;
protected:
    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::beans::XPropertySet > m_xProps;
    css::uno::Reference< css::drawing::XControlShape > m_xControlShape;
protected:
    ScVbaControl(){ m_xContext = 0; m_xProps = 0; m_xControlShape = 0; }
    void SetControl( const css::uno::Reference< css::uno::XComponentContext > xContext, 
                const css::uno::Reference< css::drawing::XControlShape > xControlShape );
    void SetControl( const css::uno::Reference< css::uno::XComponentContext > xContext, 
                const css::uno::Reference< css::beans::XPropertySet > xProps,
                const css::uno::Reference< css::drawing::XControlShape > xControlShape );
public:
    ScVbaControl( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
    ScVbaControl( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::beans::XPropertySet >& xProps,
                    const css::uno::Reference< css::drawing::XControlShape > xControlShape );
    virtual ~ScVbaControl();
    // XControl
    virtual sal_Bool SAL_CALL getEnabled() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setEnabled( sal_Bool _enabled ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getVisible() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setVisible( sal_Bool _visible ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getHeight() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setHeight( double _height ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getWidth() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setWidth( double _width ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getLeft() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setLeft( double _left ) throw (css::uno::RuntimeException);
    virtual double SAL_CALL getTop() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setTop( double _top ) throw (css::uno::RuntimeException);

    //remove resouce because org.openoffice.excel.XControl is a wrapper of com.sun.star.drawing.XControlShape
    virtual void removeResouce() throw( css::uno::RuntimeException );
};


class ScVbaControlFactory
{
public:
    ScVbaControlFactory( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
    ScVbaControlFactory( const css::uno::Reference< css::uno::XComponentContext >& xContext, 
                    const css::uno::Reference< css::beans::XPropertySet >& xProps,
                    const css::uno::Reference< css::drawing::XControlShape > xControlShape );
    ScVbaControl* createControl( const sal_Int16 nClassID )  throw ( css::uno::RuntimeException );
    ScVbaControl* createControl()  throw ( css::uno::RuntimeException );
private:
    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::beans::XPropertySet > m_xProps;
    css::uno::Reference< css::drawing::XControlShape > m_xControlShape;
};

#endif//SC_VBA_CONTROL_HXX
