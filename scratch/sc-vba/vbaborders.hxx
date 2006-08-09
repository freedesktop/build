#ifndef SC_VBA_BORDERS_HXX
#define SC_VBA_BORDERS_HXX

#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XBorders.hpp>

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/table/XCellRange.hpp>

 
#include "vbahelper.hxx"
#include "vbacollectionimpl.hxx"


typedef ::cppu::ImplInheritanceHelper1< ScVbaCollectionBaseImpl, oo::vba::XBorders > ScVbaBorders_BASE;

class ScVbaPalette;
class ScVbaBorders : public ScVbaBorders_BASE
{
protected:
	// XEnumerationAccess
	virtual css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException);

public:
	ScVbaBorders( const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::table::XCellRange >& xRange, ScVbaPalette& rPalette );
	virtual ~ScVbaBorders() {}

	// XEnumerationAccess
	virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);


	// XBorders

	// ScVbaCollectionBaseImpl	
	virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource ); 
	
	virtual css::uno::Any SAL_CALL getColor() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setColor( const css::uno::Any& _color ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getColorIndex() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setColorIndex( const css::uno::Any& _colorindex ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getLineStyle() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setLineStyle( const css::uno::Any& _linestyle ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getWeight() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setWeight( const  css::uno::Any& ) throw (css::uno::RuntimeException);
};

#endif //SC_VBA_BORDERS_HXX

