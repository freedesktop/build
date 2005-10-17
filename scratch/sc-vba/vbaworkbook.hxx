#ifndef SC_VBA_WORKBOOK_HXX
#define SC_VBA_WORKBOOK_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/frame/XModel.hpp>
#include <org/openoffice/vba/XWorkbook.hpp>
#include "vbahelper.hxx"

class ScModelObj;

typedef ::cppu::WeakImplHelper1< oo::vba::XWorkbook > ScVbaWorkbook_BASE;

class ScVbaWorkbook : public ScVbaWorkbook_BASE
{
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	css::uno::Reference< css::frame::XModel > mxModel;

public:
	ScVbaWorkbook( 	css::uno::Reference< css::uno::XComponentContext >& xContext,
			css::uno::Reference< css::frame::XModel > xModel ) :
 			mxModel( xModel ), m_xContext( xContext ){}
	virtual ~ScVbaWorkbook() {}

    // Attributes
	virtual ::rtl::OUString SAL_CALL getName() throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getPath() throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getFullName() throw (css::uno::RuntimeException);
	virtual ::sal_Bool SAL_CALL getProtectStructure() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< oo::vba::XWorksheet > SAL_CALL getActiveSheet() throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getSaved() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setSaved( sal_Bool bSave ) throw (css::uno::RuntimeException);

	// Methods
	virtual css::uno::Any Worksheets( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Close( const css::uno::Any &bSaveChanges,
								 const css::uno::Any &aFileName,
								 const css::uno::Any &bRouteWorkbook ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Protect( const css::uno::Any & aPassword ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Unprotect( const css::uno::Any &aPassword ) throw (css::uno::RuntimeException);
	virtual void SAL_CALL Save() throw (css::uno::RuntimeException);
	virtual void SAL_CALL Activate() throw (css::uno::RuntimeException);
};

#endif /* SC_VBA_WORKBOOK_HXX */
