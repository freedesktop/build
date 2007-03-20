#include "vbaradiobutton.hxx"
#include <vector>

using namespace com::sun::star;
using namespace org::openoffice;


const static rtl::OUString LABEL( RTL_CONSTASCII_USTRINGPARAM("Label") );
const static rtl::OUString STATE( RTL_CONSTASCII_USTRINGPARAM("State") );
ScVbaRadioButton::ScVbaRadioButton( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< css::drawing::XControlShape >& xControlShape ) : RadioButtonImpl_BASE( xContext, xControlShape )
{
}

// Attributes
rtl::OUString SAL_CALL 
ScVbaRadioButton::getCaption() throw (css::uno::RuntimeException)
{
    rtl::OUString Label;
    m_xProps->getPropertyValue( LABEL ) >>= Label;
    return Label;
}

void SAL_CALL 
ScVbaRadioButton::setCaption( const rtl::OUString& _caption ) throw (::com::sun::star::uno::RuntimeException)
{
    m_xProps->setPropertyValue( LABEL, uno::makeAny( _caption ) );
}

sal_Bool SAL_CALL 
ScVbaRadioButton::getValue() throw (css::uno::RuntimeException)
{
    sal_Bool bValue = sal_False;
    sal_Int16 nValue;
    m_xProps->getPropertyValue( STATE ) >>= nValue;
    if( nValue != 0 )
        bValue = sal_True;
    return bValue;
}

void SAL_CALL 
ScVbaRadioButton::setValue( sal_Bool _value ) throw (css::uno::RuntimeException)
{
    sal_Int16 nValue = 0;
    if( _value )
        nValue = 1;
    m_xProps->setPropertyValue( STATE, uno::makeAny( nValue ) );
}
