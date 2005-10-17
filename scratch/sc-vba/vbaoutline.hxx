#ifndef SC_VBA_OUTLINE_HXX
#define SC_VBA_OUTLINE_HXX

#include <com/sun/star/sheet/XSheetOutline.hpp>
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/vba/XOutline.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include "vbahelper.hxx"

typedef ::cppu::WeakImplHelper1< oo::vba::XOutline > ScVbaOutline_BASE;

class ScVbaOutline :  public ScVbaOutline_BASE 
{
	css::uno::Reference< css::sheet::XSheetOutline > mxOutline;
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
public:
	ScVbaOutline( css::uno::Reference< css::uno::XComponentContext >& xContext,
		 css::uno::Reference<css::sheet::XSheetOutline> outline):m_xContext(xContext),mxOutline(outline)
	{}
	virtual ~ScVbaOutline(){}
	
	virtual void SAL_CALL ShowLevels( const css::uno::Any& RowLevels, const css::uno::Any& ColumnLevels ) throw (css::uno::RuntimeException) ;
};
#endif
