#ifndef SC_VBA_WORKSHEET_HXX
#define SC_VBA_WORKSHEET_HXX

#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <org/openoffice/vba/XWorksheet.hpp>
#include <org/openoffice/vba/XRange.hpp>
#include "vbahelper.hxx"

class ScModelObj;

class ScVbaWorksheet : public org::openoffice::vba::XWorksheet
{
	SC_VBA_UNO_HELPER_MEMBERS;
	uno::Reference< sheet::XSpreadsheet > mxSheet;

public:
	ScVbaWorksheet( uno::Reference< sheet::XSpreadsheet > xSheet ) :
 			mxSheet( xSheet ), mnRefCount( 1 ) {}
	virtual ~ScVbaWorksheet() {}

	SC_VBA_UNO_HELPER_XINTERFACE;

    // Attributes
	virtual ::rtl::OUString SAL_CALL getName() throw (uno::RuntimeException);
	virtual void SAL_CALL setName( const ::rtl::OUString &rName ) throw (uno::RuntimeException);
	virtual sal_Bool getVisible() throw (uno::RuntimeException);
	virtual void setVisible( sal_Bool bVisible ) throw (uno::RuntimeException);

    // Methods
	virtual void SAL_CALL Activate() throw (uno::RuntimeException);
	virtual uno::Reference< vba::XRange > SAL_CALL Range( const ::uno::Any &rRange ) throw (uno::RuntimeException);
};

#endif /* SC_VBA_WORKSHEET_HXX */

