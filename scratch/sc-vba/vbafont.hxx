#ifndef SC_VBA_FONT_HXX
#define SC_VBA_FONT_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XFont.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include "vbahelper.hxx"

class ScTableSheetsObj;

typedef  ::cppu::WeakImplHelper1< oo::vba::XFont > ScVbaFont_BASE;

class ScVbaFont : public ScVbaFont_BASE 
{
    css::uno::Reference< css::beans::XPropertySet > mxFont;
public:
	ScVbaFont( css::uno::Reference< css::beans::XPropertySet > xPropertySet ) throw ( css::uno::RuntimeException ) :
            mxFont( xPropertySet, css::uno::UNO_QUERY_THROW ){}
	virtual ~ScVbaFont() {}

	// Attributes
	virtual void SAL_CALL setBold( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getBold() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setUnderline( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getUnderline() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setStrikethrough( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getStrikethrough() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setShadow( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getShadow() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setItalic( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getItalic() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setName( const ::rtl::OUString &rString ) throw (css::uno::RuntimeException);
	virtual ::rtl::OUString getName() throw (css::uno::RuntimeException);
};

#endif /* SC_VBA_FONT_HXX */

