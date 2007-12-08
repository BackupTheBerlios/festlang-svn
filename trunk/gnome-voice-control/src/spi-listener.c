/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * spi-listener.c:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <libspi/Accessibility.h>
#include <libspi/accessible.h>
#include <libspi/application.h>
#include <cspi/spi.h>

#include "gstsphinxsink.h"

#include "spi-listener.h"

enum
{
  SIGNAL_CHANGED,
  LAST_SIGNAL
};

static guint control_spi_listener_signals[LAST_SIGNAL] = { 0 };

GstElement *voice_control_pipeline;
gboolean is_keypressed;

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
#if DEBUG
	    g_message ("Object %s", item->name);
#endif
	}
}

static gboolean
is_worth_searching (Accessible *accessible)
{
	gboolean worthiness = TRUE;
	gboolean bmenu = FALSE;
	AccessibleStateSet* states = NULL;

	switch (Accessible_getRole (accessible)) 
	{
		case SPI_ROLE_MENU:
		case SPI_ROLE_MENU_BAR:
		case SPI_ROLE_MENU_ITEM:
		case SPI_ROLE_CHECK_MENU_ITEM:
		case SPI_ROLE_RADIO_MENU_ITEM:
			bmenu = TRUE;
			break;
		default:
			bmenu = FALSE;
	}
	states = Accessible_getStateSet(accessible);
	if (states != NULL)
	{
		if (!bmenu && !AccessibleStateSet_contains(states, SPI_STATE_SHOWING))
		{
			worthiness = FALSE;
		}
		AccessibleStateSet_unref(states);
	}
	return worthiness;
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

#define SEARCH_DEPTH 7
#define SEARCH_BREADTH 18

static void
control_spi_listener_build_actions_list (ControlSpiListener *listener, Accessible *parent, int depth)
{
	Accessible* child;
	AccessibleRole role;
	
	int i, child_count;

	if (depth > SEARCH_DEPTH)
		return;

	child_count = Accessible_getChildCount (parent);
	
	child_count = MIN (child_count, SEARCH_BREADTH);

	for (i = 0; i < child_count; ++i) {
		char *name;
		char *normalized_name;
		
		child = Accessible_getChildAtIndex (parent, i);
		
		if (child == parent)
			continue;

		name = Accessible_getName (child);

#if DEBUG
		{
		        gchar *role_name;		
			
			role_name = Accessible_getRoleName (child);
			g_message ("Looking at %s %s", role_name, name);
		}
#endif
		
		if (name) {
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
		}
		    
		if (is_worth_searching (child))
			control_spi_listener_build_actions_list (listener, child, depth+1);

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
	control_spi_listener_build_actions_list (listener, listener->root, 0);
	control_spi_listener_dump_actions_list (listener);

        g_signal_emit (listener,
	        control_spi_listener_signals[SIGNAL_CHANGED], 0, NULL);
	        
	listener->idle_id = 0;
	
	return FALSE;
}

static void
control_spi_listener_window_listener_cb (const AccessibleEvent *event,
	    		         void *user_data)
{
	ControlSpiListener *listener = CONTROL_SPI_LISTENER (user_data);
	AccessibleEvent_ref (event);
	g_queue_push_tail(listener->event_queue, (gpointer)event);

	if (!listener->idle_id)
    		listener->idle_id = g_idle_add (control_spi_listener_process_event, listener);
}

static void
control_spi_listener_showing_listener_cb (const AccessibleEvent *event,
	    		          void *user_data)
{
	ControlSpiListener *listener = CONTROL_SPI_LISTENER (user_data);
	
	AccessibleEvent_ref (event);
	g_queue_push_tail(listener->event_queue, (gpointer)event);
	
	if (!listener->idle_id)
    		listener->idle_id = g_idle_add (control_spi_listener_process_event, listener);
}

control_spi_set_voice_control_pipeline( GstElement *pipeline ){

	voice_control_pipeline = pipeline;
	is_keypressed = FALSE;

}

static void
control_spi_listener_keys_listener_cb (const AccessibleEvent *event,
	    		          void *user_data)
{
	ControlSpiListener *listener = CONTROL_SPI_LISTENER (user_data);

	if( !is_keypressed == TRUE ){
        	gst_element_set_state (voice_control_pipeline, GST_STATE_PLAYING);
		is_keypressed = TRUE;
		g_message ("Key was pressed");	
	}
	else{
	        gst_element_set_state (voice_control_pipeline, GST_STATE_PAUSED);
		is_keypressed = FALSE;
		g_message ("Key was released");	
	}
	
}

static void
control_spi_listener_init (ControlSpiListener *listener)
{
	listener->event_queue = g_queue_new ();
        listener->window_listener = SPI_createAccessibleEventListener (control_spi_listener_window_listener_cb, listener);
        listener->showing_listener = SPI_createAccessibleEventListener (control_spi_listener_showing_listener_cb, listener);
        listener->keys_listener = SPI_createAccessibleEventListener (control_spi_listener_keys_listener_cb, listener);
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
	SPI_registerGlobalEventListener (listener->keys_listener, "keyboard:modifiers");
}

void 
control_spi_listener_stop (ControlSpiListener *listener)
{
	SPI_deregisterGlobalEventListenerAll(listener->window_listener);
	SPI_deregisterGlobalEventListenerAll(listener->showing_listener);
	SPI_deregisterGlobalEventListenerAll(listener->keys_listener);
	
	if (listener->idle_id)
		g_source_remove (listener->idle_id);
}

GSList*
control_spi_listener_get_object_list (ControlSpiListener *listener)
{
	return listener->actions;
}
		
