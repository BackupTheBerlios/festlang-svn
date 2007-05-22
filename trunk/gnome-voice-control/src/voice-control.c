/*
 * Copyright (C) 2007  Nickolay V. Shmyrev
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
#include <panel-applet.h>
#include <glib/gi18n.h>
#include <gst/gstelement.h>
#include <gst/gstplugin.h>

#include "gstsphinxsink.h"

#define VOICE_CONTROL_APPLET(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), \
				 voice_control_applet_get_type(),          \
				 VoiceControlApplet))
#define VOICE_CONTROL_IS_APPLET(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), \
				   VOICE_CONTROL_TYPE_APPLET))

#define VOICE_CONTROL_ICON "gnome-grecord"

#define N_VOICE_CONTROL_LISTENERS 4

typedef struct {
	PanelApplet        applet;

	guint              listeners [N_VOICE_CONTROL_LISTENERS];
	
	GtkWidget*	   about_dialog;
	GtkWidget*	   frame;
	
	GtkTooltips* 	   tooltips;
	
	GstElement*        pipeline;	
} VoiceControlApplet;

typedef struct {
	PanelAppletClass klass;
} VoiceControlAppletClass;

static void  voice_control_applet_init (VoiceControlApplet      *voice_control);
static void  voice_control_applet_class_init (VoiceControlAppletClass *klass);

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
    g_message ("Running");
    gst_element_set_state (voice_control->pipeline, GST_STATE_PLAYING);
}

static void
control_stop (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
    g_message ("Waiting");
    gst_element_set_state (voice_control->pipeline, GST_STATE_NULL);
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
		      "version", "0.1.0",
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
set_tooltip (VoiceControlApplet *voice_control)
{
        if (!voice_control->tooltips) {
                voice_control->tooltips = gtk_tooltips_new ();
                g_object_ref (voice_control->tooltips);
                gtk_object_sink (GTK_OBJECT (voice_control->tooltips));
        }
        gtk_tooltips_set_tip (voice_control->tooltips, GTK_WIDGET (voice_control), _("Voice Control Applet"), NULL);
}

static void
setup_voice_control_widget (VoiceControlApplet *voice_control)
{
	GtkWidget *widget = (GtkWidget *) voice_control;
	GtkWidget *label;

	voice_control->frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (voice_control->frame), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (widget), voice_control->frame);
	
	label = gtk_label_new ("Voice control");
	gtk_container_add (GTK_CONTAINER (voice_control->frame), label);

	set_tooltip (voice_control);

	gtk_widget_show_all (widget);
}

static const BonoboUIVerb voice_control_menu_verbs [] = {
	BONOBO_UI_UNSAFE_VERB ("VoiceControlStart",       control_start),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlStop",        control_stop),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlHelp",        display_help_dialog),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlAbout",       display_about_dialog),
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

      voice_control->pipeline = gst_parse_launch ("gconfaudiosrc ! sphinxsink", &error);
      
      if (error != NULL)
    	    g_warning ("Can't create pipeline: %s", error->message);
    	    
      return;
}

static void
voice_control_applet_init (VoiceControlApplet      *voice_control)
{
	int i;
	
	gst_init (0, NULL);
		
	voice_control->about_dialog = NULL;

	for (i = 0; i < N_VOICE_CONTROL_LISTENERS; i++)
		voice_control->listeners [i] = 0;

	panel_applet_set_flags (PANEL_APPLET (voice_control),
				PANEL_APPLET_EXPAND_MINOR);
				
	voice_control_applet_create_pipeline (voice_control);
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
	GOptionContext *context;
	int           retval;
	
	
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	
	
	context = g_option_context_new ("");
	program = gnome_program_init ("voice-control-applet", VERSION,
				      LIBGNOMEUI_MODULE,
				      argc, argv,
				      GNOME_PARAM_GOPTION_CONTEXT, context,
				      GNOME_CLIENT_PARAM_SM_CONNECT, FALSE,
				      GNOME_PARAM_NONE);

	gst_init (&argc, &argv);
	
        retval = panel_applet_factory_main ("OAFIID:GNOME_VoiceControlApplet_Factory",
			     voice_control_applet_get_type (),
			     voice_control_applet_factory,
			     NULL);		
	g_object_unref (program);
	return retval;
}



