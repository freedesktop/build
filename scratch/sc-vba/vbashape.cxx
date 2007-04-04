#include<org/openoffice/office/MsoZOrderCmd.hpp>
#include<org/openoffice/office/MsoScaleFrom.hpp>
#include<com/sun/star/container/XNamed.hpp>

#include "vbashape.hxx"
#include "vbatextframe.hxx"
#include "vbalineformat.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShape::ScVbaShape( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape, const uno::Reference< drawing::XShapes > xShapes, sal_Int32 nType ) throw( lang::IllegalArgumentException ) : ScVbaShape_BASE( xParent, xContext ), m_xShape( xShape ), m_xShapes( xShapes ), m_nType( nType )
{
    m_xPropertySet.set( m_xShape, uno::UNO_QUERY_THROW );
}

ScVbaShape::ScVbaShape( const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< drawing::XShape > xShape ) throw( lang::IllegalArgumentException ) : ScVbaShape_BASE( uno::Reference< vba::XHelperInterface >(), xContext ), m_xShape( xShape )
{
}

sal_Int32 
ScVbaShape::getType( const css::uno::Reference< drawing::XShape > xShape ) throw (uno::RuntimeException)
{
    rtl::OUString sShapeType;
    uno::Reference< drawing::XShapeDescriptor > xShapeDescriptor( xShape, uno::UNO_QUERY_THROW );
    sShapeType = xShapeDescriptor->getShapeType();
    // office::MsoShapeType::msoDiagram to "com.sun.star.drawing.GroupShape"
    if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.GroupShape" ) ) )
        return office::MsoShapeType::msoGroup;
    else if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.GraphicObjectShape" ) ) )
        return office::MsoShapeType::msoPicture;
    else if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.ControlShape" ) ) )
        return office::MsoShapeType::msoOLEControlObject;
    // OOo don't support office::MsoShapeType::msoComment as a Shape.
    else if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.OLE2Shape" ) ) )
        return office::MsoShapeType::msoChart;
    // Art characters office::MsoShapeType::msoTextEffect, in OOo corresponding to "com.sun.star.drawing.CustomShape"
    else if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.ConnectorShape" ) ) )
        return office::MsoShapeType::msoLine;
    else if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.LineShape" ) ) )
        return office::MsoShapeType::msoLine;
    else if( sShapeType.equals( rtl::OUString::createFromAscii( "com.sun.star.drawing.CustomShape" ) ) )
        return office::MsoShapeType::msoAutoShape;
    else
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "the shape type do not be surppored: " ) + sShapeType, uno::Reference< uno::XInterface >() );
}

// Attributes
rtl::OUString SAL_CALL 
ScVbaShape::getName() throw (uno::RuntimeException)
{
    rtl::OUString sName;
    uno::Reference< container::XNamed > xNamed( m_xShape, uno::UNO_QUERY_THROW );
    sName = xNamed->getName();
    return sName;
}

void SAL_CALL 
ScVbaShape::setName( const rtl::OUString& _name ) throw (uno::RuntimeException)
{
    uno::Reference< container::XNamed > xNamed( m_xShape, uno::UNO_QUERY_THROW );
    xNamed->setName( _name );
}

double SAL_CALL 
ScVbaShape::getHeight() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getSize().Height );
}

void SAL_CALL 
ScVbaShape::setHeight( double _height ) throw (uno::RuntimeException)
{
    awt::Size aSize( m_xShape->getSize() );
    aSize.Height = pointsToHundredthsMillimeter( _height );
    m_xShape->setSize( aSize );
}

double SAL_CALL 
ScVbaShape::getWidth() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getSize().Width );
}

void SAL_CALL 
ScVbaShape::setWidth( double _width ) throw (uno::RuntimeException)
{
    awt::Size aSize( m_xShape->getSize() );
    aSize.Width = pointsToHundredthsMillimeter( _width );
    m_xShape->setSize( aSize );
}

double SAL_CALL 
ScVbaShape::getLeft() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getPosition().X );
}

void SAL_CALL 
ScVbaShape::setLeft( double _left ) throw (uno::RuntimeException)
{
    awt::Point oldPosition;
    oldPosition = m_xShape->getPosition();
    oldPosition.X = pointsToHundredthsMillimeter( _left );
    m_xShape->setPosition( oldPosition );
}

double SAL_CALL 
ScVbaShape::getTop() throw (uno::RuntimeException)
{
    return hundredthsMillimeterToPoints( m_xShape->getPosition().Y );
}

void SAL_CALL 
ScVbaShape::setTop( double _top ) throw (uno::RuntimeException)
{
    awt::Point oldPosition;
    oldPosition = m_xShape->getPosition();
    oldPosition.Y = pointsToHundredthsMillimeter( _top );
    m_xShape->setPosition( oldPosition );
}

sal_Bool SAL_CALL 
ScVbaShape::getVisible() throw (uno::RuntimeException)
{
    //UNO Shapes are always visible
    return sal_True;
}

void SAL_CALL 
ScVbaShape::setVisible( sal_Bool _visible ) throw (uno::RuntimeException)
{
    //UNO Shapes are always visible
}

sal_Int32 SAL_CALL 
ScVbaShape::getZOrderPosition() throw (uno::RuntimeException)
{
    sal_Int32 nZOrderPosition;
    uno::Any aZOrderPosition =  m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ) );
    aZOrderPosition >>= nZOrderPosition;
    return nZOrderPosition + 1;
}

sal_Int32 SAL_CALL 
ScVbaShape::getType() throw (uno::RuntimeException)
{
    return m_nType;
}

double SAL_CALL 
ScVbaShape::getRotation() throw (uno::RuntimeException)
{
    double dRotation;
    sal_Int32 nRotation;
    m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "RotateAngle" ) ) >>= nRotation;
    dRotation = static_cast< double >( ( 36000 - nRotation )/100 );
    return dRotation;
}

void SAL_CALL 
ScVbaShape::setRotation( double _rotation ) throw (uno::RuntimeException)
{
    sal_Int32 nRotation = static_cast < sal_Int32 > ( _rotation );
    nRotation = nRotation % 360;
    m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "RotateAngle" ), uno::makeAny( 36000 - nRotation * 100 ) );
}

// Methods
uno::Reference< excel::XTextFrame > SAL_CALL 
ScVbaShape::TextFrame() throw (uno::RuntimeException)
{
    return uno::Reference< excel::XTextFrame >(new ScVbaTextFrame( getParent(), mxContext, m_xShape ) );
}

void SAL_CALL 
ScVbaShape::Delete() throw (uno::RuntimeException)
{
    m_xShapes->remove( m_xShape );
}

void SAL_CALL 
ScVbaShape::ZOrder( sal_Int32 ZOrderCmd ) throw (uno::RuntimeException)
{
    sal_Int32 nOrderPositon;
    uno::Any aOrderPostion = m_xPropertySet->getPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ) );
    aOrderPostion >>= nOrderPositon;
    switch( ZOrderCmd )
    {
    case office::MsoZOrderCmd::msoBringToFront:
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( SAL_MAX_INT32 ) );
        break;
    case office::MsoZOrderCmd::msoSendToBack:
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( (sal_Int32)0 ) );
        break;
    case office::MsoZOrderCmd::msoBringForward:
        nOrderPositon += 1;
        m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( nOrderPositon ) );
        break;
    case office::MsoZOrderCmd::msoSendBackward:
        if( nOrderPositon > 0 )
        {
            nOrderPositon -= 1;
            m_xPropertySet->setPropertyValue( rtl::OUString::createFromAscii( "ZOrder" ), uno::makeAny( nOrderPositon ) );
        }
        break;
    // below two commands use with Writer for text and image object.
    case office::MsoZOrderCmd::msoBringInFrontOfText:
    case office::MsoZOrderCmd::msoSendBehindText:
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "This ZOrderCmd is not implemented, it is use with writer." ), uno::Reference< uno::XInterface >() );
        break;
    default:
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "Invalid Parameter." ), uno::Reference< uno::XInterface >() );
    }
}

void SAL_CALL 
ScVbaShape::IncrementRotation( double Increment ) throw (uno::RuntimeException)
{
    double nCurrentRotation = getRotation();
    nCurrentRotation += Increment;
    setRotation(nCurrentRotation);
}

void SAL_CALL 
ScVbaShape::IncrementLeft( double Increment ) throw (uno::RuntimeException)
{
    double nCurrentLeft = getLeft();
    nCurrentLeft += Increment;
    setLeft(nCurrentLeft);
}

void SAL_CALL 
ScVbaShape::IncrementTop( double Increment ) throw (uno::RuntimeException)
{
    double nCurrentTop = getTop();
    nCurrentTop += Increment;
    setTop(nCurrentTop);
}

void SAL_CALL 
ScVbaShape::ScaleHeight( double Factor, sal_Bool RelativeToOriginalSize, sal_Int32 Scale ) throw (uno::RuntimeException)
{
    double nHeight = getHeight();
    double nNewHeight = nHeight * Factor;
    if( Scale == office::MsoScaleFrom::msoScaleFromTopLeft )
    {
        setHeight(nNewHeight);
    }
    else if( Scale == office::MsoScaleFrom::msoScaleFromBottomRight )
    {
        double nDeltaHeight = nNewHeight - nHeight;
        double nNewTop = getTop() - nDeltaHeight;
        setTop(nNewTop);
        setHeight(nNewHeight);
    }
    else if( Scale == office::MsoScaleFrom::msoScaleFromMiddle )
    {
        double nDeltaHeight = (nNewHeight - nHeight) / 2;
        double nNewTop = getTop() - nDeltaHeight;
        setTop(nNewTop);
        setHeight(nNewHeight);
    }
    else
    {
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "ScaleHeight.Scale wrong value is given." ) , uno::Reference< uno::XInterface >() );
    }
}

void SAL_CALL 
ScVbaShape::ScaleWidth( double Factor, sal_Bool RelativeToOriginalSize, sal_Int32 Scale ) throw (uno::RuntimeException)
{
    double nWidth = getWidth();
    double nNewWidth = nWidth * Factor;
    if( Scale == office::MsoScaleFrom::msoScaleFromTopLeft )
    {
        setWidth(nNewWidth);
    }
    else if( Scale == office::MsoScaleFrom::msoScaleFromBottomRight )
    {
        double nDeltaWidth = nNewWidth - nWidth;
        double nNewLeft = getLeft() - nDeltaWidth;
        setLeft(nNewLeft);
        setWidth(nNewWidth);
    }
    else if( Scale == office::MsoScaleFrom::msoScaleFromMiddle )
    {
        double nDeltaWidth = (nNewWidth - nWidth) / 2;
        double nNewLeft = getLeft() - nDeltaWidth;
        setLeft(nNewLeft);
        setWidth(nNewWidth);
    }
    else
    {
        throw uno::RuntimeException( rtl::OUString::createFromAscii( "ScaleHeight.Scale wrong value is given." ) , uno::Reference< uno::XInterface >() );
    }
}

void SAL_CALL
ScVbaShape::Select( const uno::Any& Replace ) throw ( uno::RuntimeException )
{
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XSelectionSupplier > xSelectSupp( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    xSelectSupp->select( uno::makeAny( m_xShape ) );
}

uno::Reference< msform::XLineFormat > SAL_CALL 
ScVbaShape::getLine() throw (uno::RuntimeException)
{
    return uno::Reference< msform::XLineFormat >( new ScVbaLineFormat( this, mxContext, m_xShape ) );
}

rtl::OUString& 
ScVbaShape::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaShape") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaShape::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.Shape" ) );
	}
	return aServiceNames;
}
