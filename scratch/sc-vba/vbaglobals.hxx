#ifndef SC_VBA_GLOBALS
#define SC_VBA_GLOBALS

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <org/openoffice/vba/XGlobals.hpp>

#include <cppuhelper/implbase2.hxx>
#include "vbahelper.hxx"

    // =============================================================================
    // class ScVbaGlobals
    // =============================================================================

    typedef ::cppu::WeakImplHelper2<    
        oo::vba::XGlobals,
        css::lang::XServiceInfo > ScVbaGlobals_BASE;


    class ScVbaGlobals : public ScVbaGlobals_BASE
    {
    private:
            css::uno::Reference< css::uno::XComponentContext > m_xContext;
            css::uno::Reference< oo::vba::XApplication > mxApplication;
    public:
	
        ScVbaGlobals(
            const css::uno::Reference< css::uno::XComponentContext >& rxContext );
        virtual ~ScVbaGlobals();

		static 	css::uno::Reference< oo::vba::XGlobals > getGlobalsImpl(css::uno::Reference< css::uno::XComponentContext >& ) throw (css::uno::RuntimeException);

        // XServiceInfo
        virtual ::rtl::OUString SAL_CALL getImplementationName(  ) 
            throw (css::uno::RuntimeException);
        virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) 
            throw (css::uno::RuntimeException);
        virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) 
            throw (css::uno::RuntimeException);

        // XGlobals
        virtual css::uno::Reference<
                        oo::vba::XApplication > SAL_CALL getApplication()
                        throw (css::uno::RuntimeException);
        virtual css::uno::Reference< oo::vba::XWorkbook > SAL_CALL getActiveWorkbook() throw (css::uno::RuntimeException);
        virtual css::uno::Reference< oo::vba::XWorksheet > SAL_CALL getActiveSheet() throw (css::uno::RuntimeException);
        virtual css::uno::Any SAL_CALL WorkSheets(const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);
        virtual css::uno::Any SAL_CALL WorkBooks(const css::uno::Any& aIndex ) throw (css::uno::RuntimeException);

	virtual css::uno::Sequence< css::uno::Any > SAL_CALL getGlobals(  ) throw (css::uno::RuntimeException);


    };
#endif // 
