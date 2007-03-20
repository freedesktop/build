#include <com/sun/star/text/XTextRange.hpp>

#include "vbatextbox.hxx"
#include <vector>

using namespace com::sun::star;
using namespace org::openoffice;



ScVbaTextBox::ScVbaTextBox( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< css::drawing::XControlShape >& xControlShape ) : TextBoxImpl_BASE( xContext, xControlShape )
{
}

// Attributes
//getString() will cause some imfo lose.
rtl::OUString SAL_CALL 
ScVbaTextBox::getText() throw (css::uno::RuntimeException)
{
    uno::Any aValue;
    aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ) ) );
    rtl::OUString sString;
    aValue >>= sString;
    return sString;
}

void SAL_CALL 
ScVbaTextBox::setText( const rtl::OUString& _text ) throw (css::uno::RuntimeException)
{
    uno::Reference< text::XTextRange > xTextRange( m_xProps, uno::UNO_QUERY_THROW );
    xTextRange->setString( _text );
}

sal_Int32 SAL_CALL 
ScVbaTextBox::getMaxLength() throw (css::uno::RuntimeException)
{
    uno::Any aValue;
    aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MaxTextLen" ) ) );
    sal_Int32 nMaxLength = 0;
    aValue >>= nMaxLength;
    return nMaxLength;
}

void SAL_CALL 
ScVbaTextBox::setMaxLength( sal_Int32 _maxlength ) throw (css::uno::RuntimeException)
{
    uno::Any aValue( _maxlength );
    m_xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MaxTextLen" ) ), aValue);
}

sal_Bool SAL_CALL 
ScVbaTextBox::getMultiline() throw (css::uno::RuntimeException)
{
    uno::Any aValue;
    aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MultiLine" ) ) );
    sal_Bool bRet = false;
    aValue >>= bRet;
    return bRet;
}

void SAL_CALL 
ScVbaTextBox::setMultiline( sal_Bool _multiline ) throw (css::uno::RuntimeException)
{
    uno::Any aValue( _multiline );
    m_xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MultiLine" ) ), aValue);
}
