#include "vbapivottable.hxx"
#include "vbapivotcache.hxx"


using namespace ::com::sun::star;
using namespace ::org::openoffice;

ScVbaPivotTable::ScVbaPivotTable( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< sheet::XDataPilotTable >& xTable ) : m_xContext( xContext ), m_xTable( xTable )
{
}

uno::Reference< vba::XPivotCache >
ScVbaPivotTable::PivotCache() throw (uno::RuntimeException)
{
	return new ScVbaPivotCache( m_xContext, m_xTable );
}
