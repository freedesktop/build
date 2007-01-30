#include "vbapropvalue.hxx"

using namespace com::sun::star;
 
ScVbaPropValue::ScVbaPropValue( PropListener* pListener ) : m_pListener( pListener )
{
}

css::uno::Any SAL_CALL 
ScVbaPropValue::getValue() throw (css::uno::RuntimeException)
{
    return m_pListener->getValueEvent();
}

void SAL_CALL 
ScVbaPropValue::setValue( const css::uno::Any& _value ) throw (css::uno::RuntimeException)
{
    m_pListener->setValueEvent( _value );
}
