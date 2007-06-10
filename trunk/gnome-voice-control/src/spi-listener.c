
#include <string.h>
#include <libspi/Accessibility.h>
#include <libspi/accessible.h>
#include <libspi/application.h>
#include <cspi/spi.h>

#include "spi-listener.h"

enum
{
  SIGNAL_CHANGED,
  LAST_SIGNAL
};

static guint control_spi_listener_signals[LAST_SIGNAL] = { 0 };

static void
control_spi_listener_free_actions_list (ControlSpiListener *listener)
{
	GSList *l;
	AccessibleItem *item;
	
	for (l = listener->actions; l; l = l->next) {
	    item = (AccessibleItem *)l->data;
	    Accessible_unref (item->accessible);
	    g_free (item->name);
	    g_free (item);
	}
	g_slist_free (listener->actions);
	listener->actions = NULL;
}

static void
control_spi_listener_dump_actions_list (ControlSpiListener *listener)
{
	GSList *l;
	AccessibleItem *item;
	
	for (l = listener->actions; l; l = l->next) {
	    item = (AccessibleItem *)l->data;
	    g_message ("Object %s", item->name);
	}
}

static gboolean
is_visible (Accessible *accessible)
{
	gboolean retval = FALSE;
	if (accessible) {
		AccessibleStateSet *states = Accessible_getStateSet (accessible);
		if (states && AccessibleStateSet_contains (states, SPI_STATE_VISIBLE) &&
		    AccessibleStateSet_contains (states, SPI_STATE_SHOWING))
			retval = TRUE;
		if (states)
			AccessibleStateSet_unref (states);
	}
	return retval;
}

static gboolean
is_actionable (Accessible *accessible)
{
	AccessibleStateSet *states;
	
	if (!accessible)
		return FALSE;
	
	states =  Accessible_getStateSet (accessible);
	
	if (!states)
		return FALSE;
		
	if ((AccessibleStateSet_contains (states, SPI_STATE_SELECTABLE) ||
	     Accessible_isAction (accessible)) &&
	     AccessibleStateSet_contains (states, SPI_STATE_VISIBLE) &&
	     AccessibleStateSet_contains (states, SPI_STATE_SHOWING) &&
	     AccessibleStateSet_contains (states, SPI_STATE_SENSITIVE)) {
	
		AccessibleStateSet_unref (states);
		return TRUE;
	}

        AccessibleStateSet_unref (states);

	return FALSE;
}

/* Leave only ASCII in uppercase */

static char*
control_spi_listener_normalize (char *name)
{
	int word_start;
	char *result, *src, *dst;
	
	result = g_strdup (name);

	src = name; 
	dst = result; 
	word_start = 2;
	
	while (*src != 0) {
		if ((*src >= 'A' && *src <='Z') ||
		    (*src >= 'a' && *src <='z')) {
		        if (word_start == 1) 
		    	    *dst++ = ' ';
			*dst++ = toupper (*src);
			word_start = 0;
		} else {
			if (word_start == 0)
				word_start = 1;
		}
		src = g_utf8_next_char (src);
	}
	*dst = 0;
	return result;
}

static void
control_spi_listener_build_actions_list (ControlSpiListener *listener, Accessible *parent)
{
	Accessible* child;
	AccessibleRole role;
	
	int i, child_count;

	child_count = Accessible_getChildCount (parent);

	for (i = 0; i < child_count; ++i) {
		char *name;
		char *normalized_name;
		
		child = Accessible_getChildAtIndex (parent, i);
		
		if (child == parent)
			continue;
		
		name = Accessible_getName (child);
		
		normalized_name = control_spi_listener_normalize (name);
		if (normalized_name && strlen(normalized_name) > 0 && is_actionable (child)) {
		        AccessibleItem *item;	    

		        item = g_new0(AccessibleItem, 1);
		        Accessible_ref (child);
    			item->accessible = child;
			item->name = g_strdup (normalized_name);
		
			listener->actions = g_slist_append (listener->actions, item);
		}
		SPI_freeString (name);
		g_free (normalized_name);
		    
		if (is_visible (child))
			control_spi_listener_build_actions_list (listener, child);

		Accessible_unref (child); 		
	}
}

static gboolean
control_spi_listener_process_event (gpointer data)
{
	AccessibleEvent *event;
	ControlSpiListener *listener = CONTROL_SPI_LISTENER (data);
	
	while (g_queue_is_empty(listener->event_queue) == FALSE) {
			event = (AccessibleEvent *)g_queue_pop_head(listener->event_queue);

			if (strcmp (event->type, "window:activate") == 0) {
		    		if (listener->root)
					Accessible_unref (listener->root);
				Accessible_ref (event->source);
				listener->root = event->source;
			}

			AccessibleEvent_unref (event);
	}

	control_spi_listener_free_actions_list (listener);
	control_spi_listener_build_actions_list (listener, listener->root);
	control_spi_listener_dump_actions_list (listener);

        g_signal_emit (listener,
	        control_spi_listener_signals[SIGNAL_CHANGED], 0, NULL);
	
	return FALSE;
}

static void
control_spi_listener_window_listener_cb (const AccessibleEvent *event,
	    		         void *user_data)
{
	ControlSpiListener *listener = CONTROL_SPI_LISTENER (user_data);
	AccessibleEvent_ref (event);
	g_queue_push_tail(listener->event_queue, (gpointer)event);
	g_idle_add (control_spi_listener_process_event, listener);
}

static void
control_spi_listener_showing_listener_cb (const AccessibleEvent *event,
	    		          void *user_data)
{
	ControlSpiListener *listener = CONTROL_SPI_LISTENER (user_data);
	
	AccessibleEvent_ref (event);
	g_queue_push_tail(listener->event_queue, (gpointer)event);
	g_idle_add (control_spi_listener_process_event, listener);
}

static void
control_spi_listener_init (ControlSpiListener *listener)
{
	listener->event_queue = g_queue_new ();
        listener->window_listener = SPI_createAccessibleEventListener (control_spi_listener_window_listener_cb, listener);
        listener->showing_listener = SPI_createAccessibleEventListener (control_spi_listener_showing_listener_cb, listener);
}

static void
control_spi_listener_class_init (ControlSpiListenerClass *klass)
{
	control_spi_listener_signals[SIGNAL_CHANGED] =
	g_signal_new ("changed", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
	G_STRUCT_OFFSET (ControlSpiListenerClass, changed), NULL, NULL,
	g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, G_TYPE_NONE);
	
	return;
}

G_DEFINE_TYPE(ControlSpiListener, control_spi_listener, G_TYPE_OBJECT);

void 
control_spi_listener_start (ControlSpiListener *listener)
{
        SPI_registerGlobalEventListener (listener->window_listener, "window:activate");
        SPI_registerGlobalEventListener (listener->showing_listener, "object:state-changed:showing");
}

void 
control_spi_listener_stop (ControlSpiListener *listener)
{
	SPI_deregisterGlobalEventListenerAll(listener->window_listener);
	SPI_deregisterGlobalEventListenerAll(listener->showing_listener);
}

GSList*
control_spi_listener_get_object_list (ControlSpiListener *listener)
{
	return listener->actions;
}

