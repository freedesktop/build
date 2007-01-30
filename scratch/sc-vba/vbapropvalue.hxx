#ifndef SC_VBA_PROPVALULE_HXX
#define SC_VBA_PROPVALULE_HXX
#include <org/openoffice/vba/XPropValue.hpp>
#include <cppuhelper/implbase1.hxx>

#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< oo::vba::XPropValue > PropValueImpl_BASE;

class PropListener 
{
public:
    virtual void setValueEvent( const css::uno::Any& value ) = 0;
    virtual css::uno::Any getValueEvent() = 0; 
};


class ScVbaPropValue : public PropValueImpl_BASE 
{
    PropListener* m_pListener;
public:
    ScVbaPropValue( PropListener* pListener );

    // Attributes
    virtual css::uno::Any SAL_CALL getValue() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setValue( const css::uno::Any& _value ) throw (css::uno::RuntimeException);

    rtl::OUString SAL_CALL getDefaultPropertyName() throw (css::uno::RuntimeException) { return ::rtl::OUString::createFromAscii("Value"); }

};
#endif //SC_VBA_PROPVALULE_HXX
