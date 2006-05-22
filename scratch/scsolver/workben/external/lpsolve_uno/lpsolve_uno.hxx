/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2006 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/


#ifndef _LPSOLVE_UNO_HXX_
#define _LPSOLVE_UNO_HXX_

#include "cppuhelper/implbase5.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/frame/XNotifyingDispatch.hpp"
#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"

#include "org/openoffice/sc/solver/XLpAlgorithm.hpp"

#include <memory>

#define	SERVICE_NAME		"org.openoffice.sc.solver.LpSolve"
#define	IMPLEMENTATION_NAME	"scsolver.LpSolveImpl"

using namespace ::com::sun::star;

namespace org { namespace openoffice { namespace sc { namespace solver {
	class XLpModel;
}}}}

namespace scsolver {

struct LpSolveImplData;

class LpSolveImpl : public ::cppu::WeakImplHelper5<
	lang::XInitialization,
	lang::XServiceInfo,
	frame::XDispatchProvider,
	frame::XNotifyingDispatch,
	::org::openoffice::sc::solver::XLpAlgorithm >
{
public:

	LpSolveImpl( const uno::Reference<uno::XComponentContext>& );
	virtual ~LpSolveImpl() throw();

	//--------------------------------------------------------------------------
	// UNO Component Interface Methods

	// XInitialization
	virtual void SAL_CALL initialize( const uno::Sequence<uno::Any>& )
		throw ( uno::Exception );

	// XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName()
		throw ( uno::RuntimeException );
	virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& )
		throw ( uno::RuntimeException );
	virtual uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames()
		throw ( uno::RuntimeException );

	// XDispatchProvider
	virtual uno::Reference<frame::XDispatch> SAL_CALL queryDispatch( 
		const util::URL&, const rtl::OUString &, sal_Int32 )
		throw ( uno::RuntimeException );
        
	virtual uno::Sequence< uno::Reference<frame::XDispatch> > SAL_CALL queryDispatches(
		const uno::Sequence<frame::DispatchDescriptor>& )
		throw ( uno::RuntimeException );
        
	// XDispatch
	virtual void SAL_CALL dispatch( const util::URL&, const uno::Sequence<beans::PropertyValue>& )
		throw( uno::RuntimeException );
        
	virtual void SAL_CALL addStatusListener( const uno::Reference<frame::XStatusListener>&, 
		const util::URL& ) 
		throw ( uno::RuntimeException );
        
	virtual void SAL_CALL removeStatusListener( const uno::Reference< frame::XStatusListener >&, const util::URL& )
		throw ( uno::RuntimeException );

	// XNotifyingDispatch
	virtual void SAL_CALL dispatchWithNotification( 
		const util::URL&, 
		const uno::Sequence< beans::PropertyValue >&, 
		const uno::Reference< frame::XDispatchResultListener >& )
		throw ( uno::RuntimeException );

	// XLpAlgorithm
	virtual void SAL_CALL setModel( 
		const uno::Reference<org::openoffice::sc::solver::XLpModel>& ) 
		throw ( uno::RuntimeException );
	virtual SAL_CALL uno::Reference<org::openoffice::sc::solver::XLpModel> getModel()
		throw ( uno::RuntimeException );

	virtual void SAL_CALL solve() throw ( uno::RuntimeException );
	virtual double SAL_CALL getVar( sal_Int32 ) throw ( uno::RuntimeException );
	virtual uno::Sequence<double> SAL_CALL getVars() throw ( uno::RuntimeException );

private:
	::std::auto_ptr<LpSolveImplData> m_pData;
};

}

#endif
