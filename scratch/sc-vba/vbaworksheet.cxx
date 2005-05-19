#include <cppuhelper/queryinterface.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include "vbarange.hxx"
#include "vbaworksheet.hxx"

// XInterface implementation
uno::Any ScVbaWorksheet::queryInterface( const uno::Type & rType ) throw (uno::RuntimeException)
{
    return cppu::queryInterface
			(rType, 
			 static_cast< uno::XInterface * >( static_cast< vba::XWorksheet * >( this ) ),
			 static_cast< vba::XWorksheet * >( this ) );
}


::rtl::OUString
ScVbaWorksheet::getName() throw (uno::RuntimeException)
{
	uno::Reference< container::XNamed > xNamed( mxSheet, uno::UNO_QUERY_THROW );
	return xNamed->getName();
}

void
ScVbaWorksheet::setName(const ::rtl::OUString &rName ) throw (uno::RuntimeException)
{
	uno::Reference< container::XNamed > xNamed( mxSheet, uno::UNO_QUERY_THROW );
	xNamed->setName( rName );
}

sal_Bool
ScVbaWorksheet::getVisible() throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( mxSheet, uno::UNO_QUERY_THROW );
	uno::Any aValue = xProps->getPropertyValue
			(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Visible" ) ) );
	sal_Bool bRet;
	aValue >>= bRet;
	return bRet;
}

void
ScVbaWorksheet::setVisible( sal_Bool bVisible ) throw (uno::RuntimeException)
{
	uno::Reference< beans::XPropertySet > xProps( mxSheet, uno::UNO_QUERY_THROW );
	uno::Any aValue( bVisible );
	xProps->setPropertyValue
			(rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Visible" ) ), aValue);
}

void
ScVbaWorksheet::Activate() throw (uno::RuntimeException)
{
	// FIXME:
/*	ThisComponent.CurrentController.setActiveSheet( this ); */
}

uno::Reference< vba::XRange >
ScVbaWorksheet::Range( const ::rtl::OUString &rRange ) throw (uno::RuntimeException)
{
	uno::Reference< table::XCellRange > xRanges( mxSheet, uno::UNO_QUERY_THROW );
	return uno::Reference< vba::XRange >( new ScVbaRange( xRanges->getCellRangeByName( rRange ) ) );
}
