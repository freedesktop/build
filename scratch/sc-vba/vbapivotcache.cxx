#include "vbapivotcache.hxx"


using namespace ::com::sun::star;
using namespace ::org::openoffice;

ScVbaPivotCache::ScVbaPivotCache( const uno::Reference< uno::XComponentContext >& xContext,  const uno::Reference< sheet::XDataPilotTable >& xTable ) : m_xContext( xContext ), m_xTable( xTable )
{
}

void SAL_CALL 
ScVbaPivotCache::Refresh() throw (css::uno::RuntimeException)
{
	m_xTable->refresh();
}

