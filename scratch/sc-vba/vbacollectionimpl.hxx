/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
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
#ifndef SC_VBA_COLLECTION_IMPL_HXX
#define SC_VBA_COLLECTION_IMPL_HXX

#include <org/openoffice/vba/XCollection.hpp>
//#include <org/openoffice/vba/XCollectionTest.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/XDefaultMethod.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>

#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase1.hxx>

#include "vbahelper.hxx"
#include "vbahelperinterface.hxx"
#include "vbaglobals.hxx"


typedef ::cppu::WeakImplHelper1< css::container::XEnumeration > EnumerationHelper_BASE;

class EnumerationHelperImpl : public EnumerationHelper_BASE
{
protected:
	css::uno::Reference< css::uno::XComponentContext > m_xContext;
	css::uno::Reference< css::container::XEnumeration > m_xEnumeration;
public:

	EnumerationHelperImpl( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XEnumeration >& xEnumeration ) throw ( css::uno::RuntimeException ) : m_xContext( xContext ),  m_xEnumeration( xEnumeration ) { }
	virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (css::uno::RuntimeException) { return m_xEnumeration->hasMoreElements(); }
};


// including a HelperInterface implementation
template< typename Ifc1 >
class ScVbaCollectionBase : public InheritedHelperInterfaceImpl< Ifc1 >
{
typedef InheritedHelperInterfaceImpl< Ifc1 > BaseColBase;
protected:
	css::uno::Reference< css::container::XIndexAccess > m_xIndexAccess;
	css::uno::Reference< css::container::XNameAccess > m_xNameAccess;

	virtual css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);

	virtual css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException);

public:
	ScVbaCollectionBase( const css::uno::Reference< oo::vba::XHelperInterface >& xParent,   const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess ) : BaseColBase( xParent, xContext ), m_xIndexAccess( xIndexAccess ){ m_xNameAccess.set(m_xIndexAccess, css::uno::UNO_QUERY); }
	//XCollection
	virtual ::sal_Int32 SAL_CALL getCount() throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL Item( const css::uno::Any& Index ) throw (css::uno::RuntimeException);
	// XDefaultMethod
	::rtl::OUString SAL_CALL getDefaultMethodName(  ) throw (css::uno::RuntimeException);

	// XEnumerationAccess
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException) = 0;	

	// XElementAccess
	virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException) = 0;

	virtual ::sal_Bool SAL_CALL hasElements() throw (css::uno::RuntimeException);

	virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource ) = 0;
};

template <typename Ifc1>
css::uno::Any
ScVbaCollectionBase<Ifc1>::getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException)
{
	if ( !m_xNameAccess.is() )
		throw css::uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ScVbaCollectionBase string index access not supported by this object") ), css::uno::Reference< css::uno::XInterface >() );

	return createCollectionObject( m_xNameAccess->getByName( sIndex ) );	
}

template <typename Ifc1>
css::uno::Any 
ScVbaCollectionBase<Ifc1>::getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException)
{
	if ( !m_xIndexAccess.is() )
		throw css::uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ScVbaCollectionBase numeric index access not supported by this object") ), css::uno::Reference< css::uno::XInterface >() );
	if ( nIndex <= 0 )
	{
		throw  css::lang::IndexOutOfBoundsException( 
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( 
			"index is 0 or negative" ) ), 
			css::uno::Reference< css::uno::XInterface >() );
	}
	// need to adjust for vba index ( for which first element is 1 )
	return createCollectionObject( m_xIndexAccess->getByIndex( nIndex - 1 ) );	
}

template <typename Ifc1>
::sal_Int32 SAL_CALL 
ScVbaCollectionBase<Ifc1>::getCount() throw (css::uno::RuntimeException)
{
	return m_xIndexAccess->getCount();
}

template <typename Ifc1>
css::uno::Any SAL_CALL ScVbaCollectionBase<Ifc1>::Item( const css::uno::Any& aIndex ) throw (css::uno::RuntimeException)
{
	if ( aIndex.getValueTypeClass() != css::uno::TypeClass_STRING )
	{
		sal_Int32 nIndex = 0;

		if ( ( aIndex >>= nIndex ) != sal_True )
		{
			rtl::OUString message;
			message = rtl::OUString::createFromAscii(
				"Couldn't convert index to Int32");
			throw  css::lang::IndexOutOfBoundsException( message,
				css::uno::Reference< css::uno::XInterface >() );
		}
		return 	getItemByIntIndex( nIndex );
	}
	rtl::OUString aStringSheet;

	aIndex >>= aStringSheet;
	return getItemByStringIndex( aStringSheet );	
}

template <typename Ifc1>
::rtl::OUString SAL_CALL 
ScVbaCollectionBase<Ifc1>::getDefaultMethodName(  ) throw (css::uno::RuntimeException)
{
	const static rtl::OUString sName( RTL_CONSTASCII_USTRINGPARAM("Item") );
	return sName;
}

	// XElementAccess

template <typename Ifc1>
::sal_Bool SAL_CALL ScVbaCollectionBase<Ifc1>::hasElements() throw (css::uno::RuntimeException)
{
	return ( m_xIndexAccess->getCount() > 0 );
}



typedef ::cppu::WeakImplHelper1<oo::vba::XCollection> XCollection_InterfacesBASE;

typedef ScVbaCollectionBase< XCollection_InterfacesBASE > CollImplBase1;
// compatible with the old collections ( pre XHelperInterface base class ) ( some internal objects still use this )
class ScVbaCollectionBaseImpl : public CollImplBase1
{
public:
	ScVbaCollectionBaseImpl( const css::uno::Reference< oo::vba::XHelperInterface > xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess ) throw( css::uno::RuntimeException ) : CollImplBase1( xParent, xContext, xIndexAccess){}

};

template <typename Ifc> // where Ifc must implement XCollectionTest
class CollTestImplHelper :  public ScVbaCollectionBase< ::cppu::WeakImplHelper1< Ifc > >
{
typedef ScVbaCollectionBase< ::cppu::WeakImplHelper1< Ifc >  > ImplBase1;

public:
	CollTestImplHelper( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,  const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess ) throw( css::uno::RuntimeException ) : ImplBase1( xParent, xContext, xIndexAccess ) {}
};


#endif //SC_VBA_COLLECTION_IMPL_HXX
