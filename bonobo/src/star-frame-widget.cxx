#include <stdio.h>
#include "star-frame-widget.h"

#include <libgnome/gnome-macros.h>

#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/XSystemChildFactory.hpp>
#include <com/sun/star/lang/SystemDependent.hpp>

#include "services.h"

using rtl::OUString;
using namespace com::sun::star::awt;
using namespace com::sun::star::frame;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;

#define CLASS_BOILERPLATE(type, type_as_function,			\
						  parent_type, parent_type_macro)		\
	BONOBO_BOILERPLATE(type, type_as_function, type,		\
					   parent_type, parent_type_macro,		\
					   REGISTER_TYPE)

#define REGISTER_TYPE(type, type_as_function, corba_type,		\
					  parent_type, parent_type_macro)		\
	g_type_register_static (parent_type_macro, #type, &object_info, \
							(GTypeFlags)0)


CLASS_BOILERPLATE( StarFrameWidget, star_frame_widget,
				   GtkSocket, GTK_TYPE_SOCKET );

struct _StarFrameWidgetPrivate {
    Reference< XWindowPeer > x_window_peer;
	Reference< XFrame > x_frame;
};

static void
star_frame_widget_create_window_peer( StarFrameWidget *sfw )
{
	g_assert( sfw->service_manager.is() );

	Reference< XSystemChildFactory > xChildFactory(
		sfw->service_manager->createInstance( SERVICENAME_VCLTOOLKIT ),
		UNO_QUERY );
	g_assert( xChildFactory.is() );

	sfw->priv->x_window_peer.set(
		xChildFactory->createSystemChild(
			makeAny( (const sal_Int32) gtk_socket_get_id( GTK_SOCKET( sfw ) ) ),
			Sequence< sal_Int8 >(),
			SystemDependent::SYSTEM_XWINDOW ) );
	g_assert( sfw->priv->x_window_peer.is() );
}

Reference< XFrame >
star_frame_widget_get_frame( StarFrameWidget *sfw )
{
	return sfw->priv->x_frame;
}

static void
star_frame_widget_create_frame( StarFrameWidget *sfw )
{
	g_assert( sfw->priv->x_window_peer.is() );
	g_assert( sfw->service_manager.is() );

	sfw->priv->x_frame.set(
		sfw->service_manager->createInstance( SERVICENAME_FRAME ),
		UNO_QUERY );

    sfw->priv->x_frame->setName( OUString::createFromAscii( "A Frame in GTK+" ) );

	Reference< XWindow > xWindow( sfw->priv->x_window_peer, UNO_QUERY );
	sfw->priv->x_frame->initialize( xWindow );
	
    Reference< XFramesSupplier > xTreeRoot(
		sfw->service_manager->createInstance( SERVICENAME_DESKTOP ),
		UNO_QUERY );
    g_assert( xTreeRoot.is() );

    Reference< XFrames > xChildContainer = xTreeRoot->getFrames();
    xChildContainer->append( sfw->priv->x_frame );

    xWindow->setVisible( sal_True ); // FIXME
}

static void
star_frame_widget_realize( GtkWidget *widget )
{
	BONOBO_CALL_PARENT( GTK_WIDGET_CLASS, realize, ( widget ) );

	StarFrameWidget *sfw = STAR_FRAME_WIDGET( widget );

	star_frame_widget_create_window_peer( sfw );
	star_frame_widget_create_frame( sfw );
}

static void
star_frame_widget_finalize( GObject *object )
{
	delete STAR_FRAME_WIDGET( object )->priv;
	BONOBO_CALL_PARENT( G_OBJECT_CLASS, finalize, ( object ) );
}

static void
star_frame_widget_class_init( StarFrameWidgetClass *klass )
{
	GObjectClass *object_class = G_OBJECT_CLASS( klass );

	object_class->finalize = star_frame_widget_finalize;

	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS( klass );

	widget_class->realize = star_frame_widget_realize;
}

static void
star_frame_widget_instance_init( StarFrameWidget *sfw )
{
	sfw->priv = new StarFrameWidgetPrivate;
}

GtkWidget *
star_frame_widget_new ( Reference< XComponentContext > component_context )
{
    StarFrameWidget *swf = (StarFrameWidget *)g_object_new( TYPE_STAR_FRAME_WIDGET, NULL );
	swf->service_manager = Reference< XMultiServiceFactory >::query( component_context->getServiceManager() );
	return GTK_WIDGET( swf );
}
