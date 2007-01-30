#include <com/sun/star/form/FormComponentType.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/view/XControlAccess.hpp>
#include <com/sun/star/container/XChild.hpp>

#include"vbacontrol.hxx"
#include"vbacombobox.hxx"
#include "vbabutton.hxx"
#include "vbalabel.hxx"
#include "vbatextbox.hxx"
#include "vbaradiobutton.hxx"
#include "vbalistbox.hxx"


using namespace com::sun::star;
using namespace org::openoffice;

//ScVbaControl

ScVbaControl::ScVbaControl( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< ::drawing::XControlShape >& xControlShape ) : m_xContext( xContext ), m_xControlShape( xControlShape )
{
    //init m_xProps
    uno::Reference< awt::XControlModel > xControlModel( xControlShape->getControl(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xControlModel, uno::UNO_QUERY_THROW );
    m_xProps.set( xProps, uno::UNO_QUERY_THROW );
    //init m_xWindowPeer
    uno::Reference< container::XChild > xChild( xControlModel, uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XControlAccess > xControlAccess( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    m_xWindowPeer = xControlAccess->getControl( xControlModel )->getPeer();
}

ScVbaControl::ScVbaControl( const uno::Reference< uno::XComponentContext >& xContext, 
        const uno::Reference< beans::XPropertySet >& xProps, 
        const uno::Reference< drawing::XControlShape > xControlShape ) : m_xContext( xContext ), 
        m_xProps( xProps ), m_xControlShape( xControlShape )
{
    // grab the default value property name
    uno::Reference< awt::XControlModel > xControlModel( m_xProps, uno::UNO_QUERY_THROW );
    uno::Reference< container::XChild > xChild( xControlModel, uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XControlAccess > xControlAccess( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    m_xWindowPeer = xControlAccess->getControl( xControlModel )->getPeer();

    // XShapeDescriptor
        
}

void ScVbaControl::SetControl( const uno::Reference< uno::XComponentContext > xContext, const uno::Reference< ::drawing::XControlShape > xControlShape )
{
    m_xContext.set( xContext, uno::UNO_QUERY_THROW );
    m_xControlShape.set( xControlShape, uno::UNO_QUERY_THROW );
    //init m_xProps
    uno::Reference< awt::XControlModel > xControlModel( xControlShape->getControl(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xControlModel, uno::UNO_QUERY_THROW );
    m_xProps = xProps;
    //init m_xWindowPeer
    uno::Reference< container::XChild > xChild( xControlModel, uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XControlAccess > xControlAccess( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    m_xWindowPeer = xControlAccess->getControl( xControlModel )->getPeer();
}

void ScVbaControl::SetControl( const uno::Reference< uno::XComponentContext > xContext, 
        const uno::Reference< beans::XPropertySet > xProps, 
        const uno::Reference< drawing::XControlShape > xControlShape )
{
    m_xContext.set( xContext, uno::UNO_QUERY_THROW );
    m_xProps.set( xProps, uno::UNO_QUERY_THROW );
    m_xControlShape.set( xControlShape, uno::UNO_QUERY_THROW );
    // grab the default value property name
    uno::Reference< awt::XControlModel > xControlModel( m_xProps, uno::UNO_QUERY_THROW );
    uno::Reference< container::XChild > xChild( xControlModel, uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    xChild.set( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( xChild->getParent(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XControlAccess > xControlAccess( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    m_xWindowPeer = xControlAccess->getControl( xControlModel )->getPeer();
}

/*
void
ScVbaControl::Init( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< beans::XPropertySet >& xProps )
{
    m_xContext = xContext;
    m_xProps = xProps;
}
*/
//In design model has different behavior
sal_Bool SAL_CALL ScVbaControl::getEnabled() throw (uno::RuntimeException)
{
    uno::Any aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enabled" ) ) );
    sal_Bool bRet = false;
    aValue >>= bRet;
    return bRet;
}

void SAL_CALL ScVbaControl::setEnabled( sal_Bool bVisible ) throw (uno::RuntimeException)
{
    uno::Any aValue( bVisible );
    m_xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Enabled" ) ), aValue);

}

sal_Bool SAL_CALL ScVbaControl::getVisible() throw (uno::RuntimeException)
{
    uno::Reference< awt::XWindow2 > xWindow2( m_xWindowPeer, uno::UNO_QUERY_THROW );
    return xWindow2->isVisible();
}

void SAL_CALL ScVbaControl::setVisible( sal_Bool bVisible ) throw (uno::RuntimeException)
{
    uno::Reference< awt::XWindow2 > xWindow2( m_xWindowPeer, uno::UNO_QUERY_THROW );
    xWindow2->setVisible( bVisible );
}
uno::Any SAL_CALL ScVbaControl::getSize() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    return uno::makeAny(  xShape->getSize() );
}
void SAL_CALL ScVbaControl::setSize( const uno::Any& _size ) throw (uno::RuntimeException)
{
    awt::Size aSize;
    _size >>= aSize;
    uno::Reference< drawing::XShape > xShape( m_xControlShape, uno::UNO_QUERY_THROW );
    xShape->setSize( aSize );
}


//ScVbaControlFactory

ScVbaControlFactory::ScVbaControlFactory( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XControlShape >& xControlShape ): m_xContext( xContext ), m_xControlShape( xControlShape )
{
    uno::Reference< awt::XControlModel > xControlModel( xControlShape->getControl(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xProps( xControlModel, uno::UNO_QUERY_THROW );
    m_xProps.set( xProps, uno::UNO_QUERY_THROW );
}

ScVbaControlFactory::ScVbaControlFactory( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< beans::XPropertySet >& xProps, const uno::Reference< drawing::XControlShape > xControlShape ): m_xContext( xContext ), m_xProps( xProps ), m_xControlShape( xControlShape )
{
}

ScVbaControl* ScVbaControlFactory::createControl( const sal_Int16 nClassId )  throw (uno::RuntimeException)
{
    switch( nClassId )
    {
        case form::FormComponentType::COMBOBOX:
            return new ScVbaComboBox( m_xContext, m_xProps, m_xControlShape );
        case form::FormComponentType::COMMANDBUTTON:
            return new ScVbaButton( m_xContext, m_xControlShape );
        case form::FormComponentType::FIXEDTEXT:
            return new ScVbaLabel( m_xContext, m_xControlShape );
        case form::FormComponentType::TEXTFIELD:
            return new ScVbaTextBox( m_xContext, m_xControlShape );
        case form::FormComponentType::RADIOBUTTON:
            return new ScVbaRadioButton( m_xContext, m_xControlShape );
        case form::FormComponentType::LISTBOX:
            return new ScVbaListBox( m_xContext, m_xControlShape );
        default:
            throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Donot surpport this Control Type." ), uno::Reference< uno::XInterface >() );
    }
}

ScVbaControl* ScVbaControlFactory::createControl()  throw (uno::RuntimeException)
{
    sal_Int32 nClassId = -1;
    const static rtl::OUString sClassId( RTL_CONSTASCII_USTRINGPARAM("ClassId") );
    m_xProps->getPropertyValue( sClassId ) >>= nClassId;
    switch( nClassId )
    {
        case form::FormComponentType::COMBOBOX:
            return new ScVbaComboBox( m_xContext, m_xControlShape );
        case form::FormComponentType::COMMANDBUTTON:
            return new ScVbaButton( m_xContext, m_xControlShape );
        case form::FormComponentType::FIXEDTEXT:
            return new ScVbaLabel( m_xContext, m_xControlShape );
        case form::FormComponentType::TEXTFIELD:
            return new ScVbaTextBox( m_xContext, m_xControlShape );
        case form::FormComponentType::RADIOBUTTON:
            return new ScVbaRadioButton( m_xContext, m_xControlShape );
        case form::FormComponentType::LISTBOX:
            return new ScVbaListBox( m_xContext, m_xControlShape );
        default:
            throw uno::RuntimeException( rtl::OUString::createFromAscii(
                    "Donot surpport this Control Type." ), uno::Reference< uno::XInterface >() );
    }
}
