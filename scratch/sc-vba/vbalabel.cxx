#include "vbalabel.hxx"
#include <vector>

using namespace com::sun::star;
using namespace org::openoffice;


const static rtl::OUString LABEL( RTL_CONSTASCII_USTRINGPARAM("Label") );
ScVbaLabel::ScVbaLabel( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< css::drawing::XControlShape >& xControlShape ) : LabelImpl_BASE( xContext, xControlShape )
{
}

// Attributes
rtl::OUString SAL_CALL 
ScVbaLabel::getCaption() throw (css::uno::RuntimeException)
{
    rtl::OUString Label;
    m_xProps->getPropertyValue( LABEL ) >>= Label;
    return Label;
}

void SAL_CALL 
ScVbaLabel::setCaption( const rtl::OUString& _caption ) throw (::com::sun::star::uno::RuntimeException)
{
    m_xProps->setPropertyValue( LABEL, uno::makeAny( _caption ) );
}
