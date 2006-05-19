#include "vbacomments.hxx"

#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/sheet/XSheetAnnotation.hpp>

#include "vbaglobals.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

uno::Any AnnotationToComment( const uno::Any& aSource, uno::Reference< uno::XComponentContext > & xContext )
{
	uno::Reference< sheet::XSheetAnnotation > xAnno( aSource, uno::UNO_QUERY_THROW );
	uno::Reference< container::XChild > xChild( xAnno, uno::UNO_QUERY_THROW );
	uno::Reference< table::XCellRange > xCellRange( xChild->getParent(), uno::UNO_QUERY_THROW );

	return uno::makeAny( uno::Reference< vba::XComment > ( new ScVbaComment( xContext, xCellRange ) ) );
}

class CommentEnumeration : public EnumerationHelperImpl
{
public:
	CommentEnumeration( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< container::XEnumeration >& xEnumeration ) throw ( uno::RuntimeException ) : EnumerationHelperImpl( xContext, xEnumeration ) {}

	virtual uno::Any SAL_CALL nextElement() throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
	{ 
		return AnnotationToComment( m_xEnumeration->nextElement(),  m_xContext );
	}

};

ScVbaComments::ScVbaComments( const uno::Reference< uno::XComponentContext > & xContext, const uno::Reference< container::XIndexAccess >& xIndexAccess  )
: ScVbaComments_BASE( xContext, xIndexAccess )
{
}

// public helper functions

uno::Reference< container::XEnumeration >
ScVbaComments::createEnumeration() throw (uno::RuntimeException)
{
	uno::Reference< container::XEnumerationAccess > xEnumAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );

	return new CommentEnumeration( m_xContext, xEnumAccess->createEnumeration() );
}

uno::Any
ScVbaComments::createCollectionObject( const css::uno::Any& aSource )
{
	return AnnotationToComment( aSource,  m_xContext );
}

uno::Type 
ScVbaComments::getElementType() throw (uno::RuntimeException)
{
	return vba::XComments::static_type(0);
}

