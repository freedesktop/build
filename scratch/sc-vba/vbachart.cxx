#include "vbachart.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>


using namespace ::com::sun::star;
using namespace ::org::openoffice;

const rtl::OUString CHART_NAME( RTL_CONSTASCII_USTRINGPARAM("Name") );
::rtl::OUString SAL_CALL 
ScVbaChart::getName() throw (css::uno::RuntimeException)
{
	rtl::OUString sName;
	uno::Reference< beans::XPropertySet > xProps( m_xChartDoc, uno::UNO_QUERY_THROW );
	try
	{
		xProps->getPropertyValue( CHART_NAME ) >>= sName;
	}
	catch( uno::Exception e ) // swallow exceptions
	{
	}
	return sName;
}



