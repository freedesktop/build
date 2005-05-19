#ifndef SC_VBA_HELPER_HXX
#define SC_VBA_HELPER_HXX

using namespace ::com::sun::star;
using namespace ::org::openoffice;

#ifdef DEBUG
#  define SC_VBA_FIXME(a) printf a
#else
#  define SC_VBA_FIXME(a)
#endif

#define SC_VBA_UNO_HELPER_MEMBERS \
		sal_Int32 mnRefCount

#define SC_VBA_UNO_HELPER_XINTERFACE \
    virtual void acquire() throw () { ++mnRefCount; } \
    virtual void release() throw () { if (! --mnRefCount) delete this; } \
    virtual com::sun::star::uno::Any queryInterface( \
			const com::sun::star::uno::Type & rType ) \
				throw (com::sun::star::uno::RuntimeException)


#endif
