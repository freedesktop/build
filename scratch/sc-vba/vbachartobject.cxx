#include "vbachart.hxx"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/document/XEmbeddedObjectSupplier.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include "vbachartobject.hxx"

using namespace ::com::sun::star;
using namespace ::org::openoffice;

const rtl::OUString CHART_NAME( RTL_CONSTASCII_USTRINGPARAM("Name") );

::rtl::OUString SAL_CALL 
ScVbaChartObject::getName() throw (css::uno::RuntimeException)
{
	rtl::OUString sName;
	uno::Reference< container::XNamed > xNamed( m_xTableChart, uno::UNO_QUERY );
	if ( xNamed.is() )
		sName = xNamed->getName();
	return sName;
}



uno::Reference< vba::XChart > SAL_CALL 
ScVbaChartObject::getChart() throw (css::uno::RuntimeException)
{
	uno::Reference< document::XEmbeddedObjectSupplier > xEmbed( m_xTableChart, uno::UNO_QUERY_THROW );
	uno::Reference< chart::XChartDocument > xChart( xEmbed->getEmbeddedObject(), uno::UNO_QUERY_THROW );

	// #TODO check with vba to see whether its valid to return a 
	// null object for the Chart property. atm, we throw ( i.e. if in
	// doubt... throw ) 
	
	return new ScVbaChart( m_xContext, xChart );
}

void SAL_CALL
ScVbaChartObject::test()throw (css::uno::RuntimeException)
{
}


