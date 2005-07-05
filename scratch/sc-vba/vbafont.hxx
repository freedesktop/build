#ifndef SC_VBA_FONT_HXX
#define SC_VBA_FONT_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XFont.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include "vbahelper.hxx"

class ScTableSheetsObj;

typedef  ::cppu::WeakImplHelper1< org::openoffice::vba::XFont > ScVbaFont_BASE;

class ScVbaFont : public org::openoffice::vba::XFont
{
    uno::Reference< beans::XPropertySet > mxFont;
public:
	ScVbaFont( uno::Reference< beans::XPropertySet > xPropertySet ) :
            mxFont( xPropertySet ){}
	virtual ~ScVbaFont() {}

	// Attributes
	virtual void SAL_CALL setBold( sal_Bool bValue ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getBold() throw (uno::RuntimeException);
	virtual void SAL_CALL setUnderline( sal_Bool bValue ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getUnderline() throw (uno::RuntimeException);
	virtual void SAL_CALL setStrikethrough( sal_Bool bValue ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getStrikethrough() throw (uno::RuntimeException);
	virtual void SAL_CALL setShadow( sal_Bool bValue ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getShadow() throw (uno::RuntimeException);
	virtual void SAL_CALL setItalic( sal_Bool bValue ) throw (uno::RuntimeException);
	virtual sal_Bool SAL_CALL getItalic() throw (uno::RuntimeException);
	virtual void SAL_CALL setName( const ::rtl::OUString &rString ) throw (uno::RuntimeException);
	virtual ::rtl::OUString getName() throw (uno::RuntimeException);
};

#endif /* SC_VBA_FONT_HXX */

