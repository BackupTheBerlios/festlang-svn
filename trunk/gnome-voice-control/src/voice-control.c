/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * voice-control.c:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <config.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <panel-applet.h>
#include <glib/gi18n.h>
#include <gst/gstelement.h>
#include <gst/gstplugin.h>

#include <libnotify/notify.h>

#include "gstsphinxsink.h"
#include "action.h"

#define VOICE_CONTROL_APPLET(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), \
				 voice_control_applet_get_type(),          \
				 VoiceControlApplet))
#define VOICE_CONTROL_IS_APPLET(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), \
				   VOICE_CONTROL_TYPE_APPLET))

#define VOICE_CONTROL_ICON "gnome-grecord"

#define N_VOICE_CONTROL_LISTENERS 4

#include "spi-listener.h"

typedef struct {
	PanelApplet        applet;

	guint              listeners [N_VOICE_CONTROL_LISTENERS];
	
	GtkWidget*	   about_dialog;
	GtkWidget*	   frame;
	GtkWidget*         state_label;
	
	GtkTooltips* 	   tooltips;
	NotifyNotification *notif;
	
	GstElement*        pipeline;	
	GstElement* 	   sink;
	
	ControlSpiListener*	   spi_listener;
	gchar* last_message;
} VoiceControlApplet;

typedef struct {
	PanelAppletClass klass;
} VoiceControlAppletClass;

static void  voice_control_applet_init (VoiceControlApplet      *voice_control);
static void  voice_control_applet_class_init (VoiceControlAppletClass *klass);
static void  voice_control_set_text (VoiceControlApplet *voice_control, gchar *message, gchar *tooltip);

G_DEFINE_TYPE (VoiceControlApplet, voice_control_applet, PANEL_TYPE_APPLET)

static void
display_help_dialog (BonoboUIComponent   *uic,
		     VoiceControlApplet  *voice_control,
		     const char          *verbname)
{
	GError *error = NULL;

	gnome_help_display_desktop_on_screen (
		NULL, "gnome-voice-control", "gnome-voice-control", "voice-control-settings",
		gtk_widget_get_screen (GTK_WIDGET (voice_control)),
		&error);

	if (error) {
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new (
				NULL,
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("There was an error displaying help: %s"),
				error->message);

		g_signal_connect (dialog, "response",
				  G_CALLBACK (gtk_widget_destroy),
				  NULL);

		gtk_window_set_icon_name (GTK_WINDOW (dialog), VOICE_CONTROL_ICON);
		gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
		gtk_window_set_screen (GTK_WINDOW (dialog),
				       gtk_widget_get_screen (GTK_WIDGET (voice_control)));
		gtk_widget_show (dialog);
		g_error_free (error);
	}
}

static void
control_start (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
	gst_element_set_state (voice_control->pipeline, GST_STATE_PLAYING);

	control_spi_listener_start (voice_control->spi_listener);
}

static void
keypress_control_start (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{

	control_spi_listener_start (voice_control->spi_listener);

	control_spi_set_voice_control_pipeline(voice_control->pipeline);

}


static void
control_stop (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
        gst_element_set_state (voice_control->pipeline, GST_STATE_NULL);
	gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
	voice_control_set_text (voice_control, _("Idle"), _("Choose Start Control to start"));
	control_spi_listener_stop (voice_control->spi_listener);
}

static void
on_sink_initialization (GObject *sink, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);

	gdk_threads_enter ();
	gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_STOP, GTK_ICON_SIZE_MENU);
	voice_control_set_text (voice_control, _("Init"), _("Loading acoustic and language model, wait a bit"));
	gdk_threads_leave ();
}

static void
on_sink_after_initialization (GObject *sink, gpointer data)
{
	GSList *commands;
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);

	commands = voice_control_action_append_commands (NULL);	
	gst_sphinx_sink_set_fsg (GST_SPHINX_SINK(voice_control->sink), commands);
	g_slist_free (commands);

}

static void
on_sink_calibration (GObject *sink, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);

	gdk_threads_enter ();
	gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_STOP, GTK_ICON_SIZE_MENU);
	voice_control_set_text (voice_control, _("Calibration"), _("Tuning up acoustic parameters"));
	gdk_threads_leave ();
}

static void
on_sink_listening (GObject *sink, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);
	GError *err = NULL;

	gdk_threads_enter ();
	notify_notification_close(voice_control->notif, &err);
	if (err) {
		g_warning("Failed to show notifcation: %s\n", err->message);
		g_error_free(err);
	}

	gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
	voice_control_set_text (voice_control, _("Listening"), _("Processing speech"));
	gdk_threads_leave ();
}

static void
on_sink_ready (GObject *sink, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);

	gdk_threads_enter ();
	gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
	voice_control_set_text (voice_control, _("Ready"), _("Ready for input"));
	gdk_threads_leave ();
}

static gboolean 
process_action (gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);
	GSList *l, *nodes;
	GError *err = NULL;
	notify_notification_update(voice_control->notif,
				   "Speech Input",
				   voice_control->last_message, NULL);
	notify_notification_set_timeout(voice_control->notif,
					NOTIFY_EXPIRES_DEFAULT);
	notify_notification_show(voice_control->notif, &err);
	if (err) {
		g_warning("Failed to show notifcation: %s\n", err->message);
		g_error_free(err);
	}

	nodes = control_spi_listener_get_object_list (voice_control->spi_listener);

	for (l = nodes; l; l = l->next) {
		AccessibleItem *item = (AccessibleItem *)l->data;
		if (g_strrstr(voice_control->last_message, item->name)) {
			AccessibleAction *action;
			
			g_message ("Running action '%s' accessible %p", item->name, item->accessible);
			
			action = Accessible_getAction (item->accessible);
			AccessibleAction_doAction (action, 0);
			break;
		}
	}
	return FALSE;
}

static gboolean
show_notification(gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);
	GError *err = NULL;

	notify_notification_update(voice_control->notif,
				   "Speech Input",
				   voice_control->last_message, NULL);
	notify_notification_set_timeout(voice_control->notif,
					NOTIFY_EXPIRES_DEFAULT);
	notify_notification_show(voice_control->notif, &err);
	if (err) {
		g_warning("Failed to show notifcation: %s\n", err->message);
		g_error_free(err);
	}
}

/**  
 * Handle messages
 *
 * Checks a pre-defined list of global actions if still not found 
 * searches in items available for the current window
 * 
 * @param *sink gst sink
 * @param *message current message
 * @param data voice_control instance
 * @return void  
 */
static void
on_sink_message (GObject *sink, gchar *message, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);

	if (voice_control->last_message)
		g_free (voice_control->last_message);
	voice_control->last_message = g_strdup (message);
	
	g_idle_add (show_notification, voice_control);
	
	if (voice_control_action_process_result (message))
		return;

	g_idle_add (process_action, data);		
	
	return;
}

static void
voice_control_ui_changed (ControlSpiListener *listener, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);
	GSList *l, *nodes, *commands;
	
	nodes = control_spi_listener_get_object_list (listener);
	commands = NULL;

	for (l = nodes; l; l = l->next) {
		AccessibleItem *item = (AccessibleItem *)l->data;
		commands = g_slist_append (commands, item->name);
	}
	
	commands = voice_control_action_append_commands (commands);
	
	gst_sphinx_sink_set_fsg (GST_SPHINX_SINK(voice_control->sink), commands);

	g_slist_free (commands);
}

static void
display_about_dialog (BonoboUIComponent  *uic,
		      VoiceControlApplet *voice_control,
		      const char         *verbname)
{
	const char  *documenters [] = {
		"Nickolay V. Shmyrev <nshmyrev@yandex.ru>",
          	NULL
	};

	char        *authors[2];
	char        *descr;

	if (voice_control->about_dialog) {
		gtk_window_set_screen (GTK_WINDOW (voice_control->about_dialog),
				       gtk_widget_get_screen (GTK_WIDGET (voice_control)));
		gtk_window_present (GTK_WINDOW (voice_control->about_dialog));
		return;
	}

	authors [0] = _("Nickolay V. Shmyrev <nshmyrev@yandex.ru>");
	authors [1] = _("Raphael Nunes da Motta <raphael.nunes.enc04@gmail.com>");
	authors [2] = NULL;

	descr = _("Voice Control Applet used to manage desktop with speech");
		
	voice_control->about_dialog = gtk_about_dialog_new ();
	g_object_set (voice_control->about_dialog,
		      "name", _("VoiceControl"),
		      "version", "0.2.0",
		      "copyright", "Copyright \xc2\xa9 1998-2002 Free Software Foundation, Inc.",
		      "comments", descr,
		      "authors", (const char **) authors,
		      "documenters", documenters,
		      "translator-credits", _("translator-credits"),
		      "logo-icon-name", VOICE_CONTROL_ICON,
		      NULL);

	gtk_window_set_icon_name (GTK_WINDOW (voice_control->about_dialog), VOICE_CONTROL_ICON);
	gtk_window_set_wmclass (GTK_WINDOW (voice_control->about_dialog), "fish", "VoiceControl");
	gtk_window_set_screen (GTK_WINDOW (voice_control->about_dialog),
			       gtk_widget_get_screen (GTK_WIDGET (voice_control)));

	g_signal_connect (voice_control->about_dialog, "destroy",
			  G_CALLBACK (gtk_widget_destroyed),
			  &voice_control->about_dialog);

	g_signal_connect (voice_control->about_dialog, "response",
			  G_CALLBACK (gtk_widget_destroy),
			  NULL);

	gtk_widget_show (voice_control->about_dialog);
}

static void
voice_control_set_text (VoiceControlApplet *voice_control, gchar *message, gchar *tooltip)
{
	gtk_label_set_text (GTK_LABEL (voice_control->state_label), message);
        if (!voice_control->tooltips) {
                voice_control->tooltips = gtk_tooltips_new ();
                g_object_ref (voice_control->tooltips);
                gtk_object_sink (GTK_OBJECT (voice_control->tooltips));
        }
        gtk_tooltips_set_tip (voice_control->tooltips, GTK_WIDGET (voice_control), tooltip, NULL);
}

static void
setup_voice_control_widget (VoiceControlApplet *voice_control)
{
	GtkWidget *widget = (GtkWidget *) voice_control;

	voice_control->frame = gtk_image_new_from_stock(GTK_STOCK_NO, GTK_ICON_SIZE_MENU);

	gtk_container_add (GTK_CONTAINER (widget), voice_control->frame);
	
	voice_control->state_label = gtk_label_new ("");

	voice_control_set_text (voice_control, _("Idle"), _("Choose Start Control to start"));

	gtk_widget_show_all (widget);
}

static const BonoboUIVerb voice_control_menu_verbs [] = {
	BONOBO_UI_UNSAFE_VERB ("VoiceControlStart",         control_start),
	BONOBO_UI_UNSAFE_VERB ("KeypressVoiceControlStart", keypress_control_start),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlStop",          control_stop),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlHelp",          display_help_dialog),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlAbout",         display_about_dialog),
        BONOBO_UI_VERB_END
};

static gboolean
voice_control_applet_fill (VoiceControlApplet *voice_control)
{
	PanelApplet *applet = PANEL_APPLET (voice_control);

	setup_voice_control_widget (voice_control);

	panel_applet_setup_menu_from_file (
		applet, GNOMEDATADIR, "GNOME_VoiceControlApplet.xml",
		NULL, voice_control_menu_verbs, voice_control);

	return TRUE;
}

static gboolean
voice_control_applet_factory (PanelApplet *applet,
			      const char  *iid,
			      gpointer     data)
{
	gboolean retval = FALSE;

	if (!strcmp (iid, "OAFIID:GNOME_VoiceControlApplet"))
		retval = voice_control_applet_fill (VOICE_CONTROL_APPLET (applet));

	return retval;
}

static void
voice_control_applet_destroy (GtkObject *object)
{
	VoiceControlApplet *voice_control = (VoiceControlApplet *) object;
	int         i;

	if (voice_control->about_dialog)
		gtk_widget_destroy (voice_control->about_dialog);
	voice_control->about_dialog = NULL;

	if (voice_control->tooltips)
		g_object_unref (voice_control->tooltips);
	voice_control->tooltips = NULL;

	if (voice_control->pipeline)
		g_object_unref (voice_control->pipeline);
	voice_control->pipeline = NULL;

	GTK_OBJECT_CLASS (voice_control_applet_parent_class)->destroy (object);
}

static void
voice_control_applet_create_pipeline (VoiceControlApplet *voice_control)
{
      GError *error = NULL;

      voice_control->pipeline = gst_parse_launch ("gconfaudiosrc ! sphinxsink name=sink", &error);
      
      if (error != NULL) {
    	    g_warning ("Can't create pipeline: %s", error->message);
    	    return;
      }

      voice_control->sink = gst_bin_get_by_name(GST_BIN (voice_control->pipeline),
    						"sink");
      if (!voice_control->sink) {
    	    g_warning ("Pipeline has no sink");
    	    return;
      }
    	
      g_signal_connect (voice_control->sink, "initialization",
    		        G_CALLBACK(on_sink_initialization), voice_control);
      g_signal_connect (voice_control->sink, "after_initialization",
    		        G_CALLBACK(on_sink_after_initialization), voice_control);
      g_signal_connect (voice_control->sink, "calibration",
    		        G_CALLBACK(on_sink_calibration), voice_control);
      g_signal_connect (voice_control->sink, "listening",
    			G_CALLBACK(on_sink_listening), voice_control);
      g_signal_connect (voice_control->sink, "ready",
    			G_CALLBACK(on_sink_ready), voice_control);
      g_signal_connect (voice_control->sink, "message",
    			G_CALLBACK(on_sink_message), voice_control);
    
      return;
}

static void
voice_control_applet_init (VoiceControlApplet      *voice_control)
{
	int i;
	
	voice_control->about_dialog = NULL;

	for (i = 0; i < N_VOICE_CONTROL_LISTENERS; i++)
		voice_control->listeners [i] = 0;

	panel_applet_set_flags (PANEL_APPLET (voice_control),
				PANEL_APPLET_EXPAND_MINOR);
				
	voice_control_applet_create_pipeline (voice_control);

	voice_control->spi_listener = g_object_new (CONTROL_SPI_LISTENER_TYPE, NULL);
	g_signal_connect_object (voice_control->spi_listener, "changed", 
				 G_CALLBACK (voice_control_ui_changed), voice_control, 0);

	voice_control->notif = notify_notification_new("Speech Input",
						       NULL,
						       NULL,
						       GTK_WIDGET(voice_control));

}

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean res = TRUE;

  res &= gst_element_register (plugin, "sphinxsink", GST_RANK_PRIMARY, GST_TYPE_SPHINX_SINK);
  return res;
}

GST_PLUGIN_DEFINE_STATIC (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "sphinxplugin",
    "Sphinx Recognition plugin",
    plugin_init, VERSION, GST_LICENSE_UNKNOWN, PACKAGE_NAME, PACKAGE_NAME);
                
static void
voice_control_applet_class_init (VoiceControlAppletClass *klass)
{
	PanelAppletClass *applet_class     = (PanelAppletClass *) klass;
	GtkObjectClass   *gtk_object_class = (GtkObjectClass *) klass;

	voice_control_applet_parent_class = g_type_class_peek_parent (klass);
	gtk_object_class->destroy = voice_control_applet_destroy;
}

int main (int argc, char *argv [])
{				
	GnomeProgram *program;	
	int           retval;
		
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	
	g_thread_init (NULL);
	gst_init (&argc, &argv);
	gdk_threads_init ();

	if (!notify_init("voice-control-applet")) {
		g_warning("Failed to initialize libnotify\n");
	}
	
	program = gnome_program_init ("voice-control-applet", VERSION,
				      LIBGNOMEUI_MODULE,
				      argc, argv,
				      GNOME_CLIENT_PARAM_SM_CONNECT, FALSE,
				      GNOME_PARAM_NONE);

	gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
						APPNAME_DATA_DIR G_DIR_SEPARATOR_S 
						"icons");

	SPI_init ();
	
        retval = panel_applet_factory_main ("OAFIID:GNOME_VoiceControlApplet_Factory",
			     voice_control_applet_get_type (),
			     voice_control_applet_factory,
			     NULL);		
	g_object_unref (program);
	SPI_exit ();

	notify_uninit();

	return retval;
}
