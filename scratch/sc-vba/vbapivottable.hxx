#ifndef SC_VBA_PIVOTTABLE_HXX
#define SC_VBA_PIVOTTABLE_HXX
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/sheet/XDataPilotTable.hpp>
#include <org/openoffice/vba/XPivotTable.hpp>
#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1<oo::vba::XPivotTable > PivotTableImpl_BASE;

class ScVbaPivotTable : public PivotTableImpl_BASE
{		
	css::uno::Reference< css::sheet::XDataPilotTable > m_xTable;
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
public:
	ScVbaPivotTable( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::sheet::XDataPilotTable >& xTable );
	virtual css::uno::Reference< oo::vba::XPivotCache > SAL_CALL PivotCache(  ) throw (css::uno::RuntimeException);
};

#endif //SC_VBA_PIVOTTABLE_HXX
