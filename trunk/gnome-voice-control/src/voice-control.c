/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2010  Consorcio Fernando de los Rios - Junta de Andalucia
 * Developed by Intelligent Dialogue Systems S.L. <info@indisys.es>
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
#include <locale.h>
#include <libgnome/libgnome.h>
#include <libgnomeui/libgnomeui.h>

#include "action.h"
#include "voicecontrol.h"

#include "asr.h"

static gboolean o_window = WINDOWS_MODE;

static GOptionEntry applet_control_options[] =
{
        { "window", 0, 0, G_OPTION_ARG_NONE, &o_window, "Window mode", NULL },
        { NULL }
};

static void
display_help_dialog (BonoboUIComponent   *uic,
		     VoiceControlApplet  *voice_control,
		     const char          *verbname);

static void
display_about_dialog (BonoboUIComponent  *uic,
		      VoiceControlApplet *voice_control,
		      const char         *verbname);

static void
control_start (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname);

static void
keypress_control_start (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname);

static void
change_language (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname);

static void
control_stop (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname);

static const BonoboUIVerb voice_control_menu_verbs [] = {
	BONOBO_UI_UNSAFE_VERB ("VoiceControlStart",         control_start),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlStop",          control_stop),
//	BONOBO_UI_UNSAFE_VERB ("KeypressVoiceControlStart", keypress_control_start),
        BONOBO_UI_UNSAFE_VERB ("ChangeLanguage",          change_language),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlHelp",          display_help_dialog),
	BONOBO_UI_UNSAFE_VERB ("VoiceControlAbout",         display_about_dialog),
        BONOBO_UI_VERB_END
};

static const gchar* voice_control_get_menu_xml();

G_DEFINE_TYPE (VoiceControlApplet, voice_control_applet, PANEL_TYPE_APPLET)

//////////////////////////////////
// Panel Controls
//////////////////////////////////

static void
display_help_dialog (BonoboUIComponent   *uic,
		     VoiceControlApplet  *voice_control,
		     const char          *verbname)
{
	GError *error = NULL;

        gnome_help_display_desktop_on_screen (
		NULL, 
                "gnome-voice-control",
                "gnome-voice-control",
                NULL, // "voice-control-settings"
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
display_about_dialog (BonoboUIComponent  *uic,
		      VoiceControlApplet *voice_control,
		      const char         *verbname)
{
	char        *authors[6]; 	
	char        *descr;


	const char  *documenters [] = {
		"Nickolay V. Shmyrev <nshmyrev@yandex.ru>",
	        "Felipe Ram\u00F3n Fabresse <info@indisys.es>",
  	        "Diego Mart\u00EDnez Fontiveros <info@indisys.es>",
	   	NULL
		};

  	if (voice_control->about_dialog) {
		gtk_window_set_screen (GTK_WINDOW (voice_control->about_dialog),
				       gtk_widget_get_screen (GTK_WIDGET (voice_control)));
		gtk_window_present (GTK_WINDOW (voice_control->about_dialog));
		return;
	}

	authors [0] = _("Nickolay V. Shmyrev <nshmyrev@yandex.ru>");
	authors [1] = _("Raphael Nunes da Motta <raphael.nunes.enc04@gmail.com>");
  	authors [2] = _("Felipe Ram\u00F3n Fabresse <info@indisys.es>");
	authors [3] = _("Diego Mart\u00EDnez Fontiveros <info@indisys.es>");
	authors [4] = _("Guillermo P\u00E9rez Garc\u00EDa <info@indisys.es>");
	authors [5] = NULL;

	descr = _("Voice Control Applet used to manage desktop with speech");

	voice_control->about_dialog = gtk_about_dialog_new ();
	g_object_set (voice_control->about_dialog,
		      "name", _("VoiceControl"),
		      "version", VERSION,
		      "copyright", "Copyright \xc2\xa9 1998-2002 Free Software Foundation, Inc.\nCopyright \xc2\xa9 2010 Guadalinfo.es.\nCopyright \xc2\xa9 2010 Consorcio Fernando de los R\u00EDos.",
		      "comments", descr,
		      "authors", (const char **) authors,
		      "documenters", documenters,
		      "translator-credits", "Mauricio Henriquez <buhochileno@gmail.com>\nFelipe Ram\u00F3n Fabresse <info@indisys.es>",
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
control_start (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
	asr_start(voice_control);

	control_spi_listener_start (voice_control->spi_listener);
}

static void
keypress_control_start (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
    control_spi_listener_start (voice_control->spi_listener);
}

static void
change_language (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
    PanelApplet *applet = PANEL_APPLET (voice_control);

    const gchar* voice_control_menu_xml;
    gchar* lang = NULL;
    gchar* short_lang = NULL;
    gchar ** splits = NULL;

    splits = g_strsplit(voice_control->lang, "_", 2);
    lang = splits[0];

#ifdef ENABLE_DEBUG_DUMP
    g_message("Changing language from [%s] to ", voice_control->lang);
#endif

    if(strcmp(lang, "es") == 0)
    {
        lang = setlocale(LC_ALL, "en_US.utf8");
    }else
    {
        lang = setlocale(LC_ALL, "es_ES.utf8");
    }

    if(lang != NULL)
    {
        splits = g_strsplit(lang, "_", 2);
        short_lang = g_strdup(splits[0]);

        voice_control_menu_xml = voice_control_get_menu_xml();

        panel_applet_setup_menu (
                    applet, voice_control_menu_xml, voice_control_menu_verbs, voice_control);

        voice_control_set_state(voice_control, voice_control_get_state(voice_control), NULL);

        asr_set_language(voice_control, short_lang);

        g_free(voice_control->lang);
        voice_control->lang = g_strdup(lang);
    }

#ifdef ENABLE_DEBUG_DUMP
    g_message("[%s]\n", voice_control->lang);
#endif

    if(splits != NULL)
      g_strfreev(splits);
    g_free(voice_control_menu_xml);
}

static void
control_stop (BonoboUIComponent  *uic,
	       VoiceControlApplet *voice_control,
	       const char         *verbname)
{
        asr_stop(voice_control);
        voice_control_set_state(voice_control, STOP, NULL);
	control_spi_listener_stop (voice_control->spi_listener);
}

/////////////////////////////////////////////////////
// Process accesible actions (Not default actions)
/////////////////////////////////////////////////////

gboolean 
voice_control_process_action (VoiceControlApplet *voice_control, const gchar *message)
{
	GSList *l, *nodes;

	nodes = control_spi_listener_get_object_list (voice_control->spi_listener);

	for (l = nodes; l; l = l->next) {
		AccessibleItem *item = (AccessibleItem *)l->data;
		if (g_strrstr(message, item->name)) {
			AccessibleAction *action;

#ifdef ENABLE_DEBUG_DUMP
			g_message ("Running action '%s' accessible %p", item->name, item->accessible);
#endif
			
			action = Accessible_getAction (item->accessible);
			AccessibleAction_doAction (action, 0);
			break;
		}
	}
	return FALSE;
}

///////////////////////////////
// Get new accesible actions
///////////////////////////////

static void
voice_control_ui_changed (ControlSpiListener *listener, gpointer data)
{
	VoiceControlApplet *voice_control = VOICE_CONTROL_APPLET (data);
	GSList *l, *nodes, *commands;
	
	if (!asr_running(voice_control))
		return;
	
	nodes = control_spi_listener_get_object_list (listener);
	commands = NULL;

	for (l = nodes; l; l = l->next) {
		AccessibleItem *item = (AccessibleItem *)l->data;
		commands = g_slist_append (commands, item->name);
	}
	
	commands = voice_control_action_append_commands (commands);
	
	asr_set_grammar(voice_control, commands);

	g_slist_free (commands);
}

/////////////////////////////
// Auxiliary functions
/////////////////////////////

void
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
	GtkWidget *hbox = gtk_hbox_new (FALSE, 6);

	gtk_container_add (GTK_CONTAINER (widget), hbox);

	voice_control->frame = gtk_image_new_from_stock(GTK_STOCK_NO, GTK_ICON_SIZE_MENU);

	gtk_container_add (GTK_CONTAINER (hbox), voice_control->frame);
	
	voice_control->state_label = gtk_label_new ("");

	gtk_container_add (GTK_CONTAINER (hbox), voice_control->state_label);

	voice_control_set_state(voice_control, STOP, NULL);

	gtk_widget_show_all (widget);
}

static gboolean
voice_control_applet_fill (VoiceControlApplet *voice_control)
{
	PanelApplet *applet = PANEL_APPLET (voice_control);
        const gchar* voice_control_menu_xml = voice_control_get_menu_xml();

	setup_voice_control_widget (voice_control);

	panel_applet_setup_menu (
		applet, voice_control_menu_xml, voice_control_menu_verbs, voice_control);

        g_free(voice_control_menu_xml);

	return TRUE;
}

static gboolean
voice_control_applet_factory (PanelApplet *applet,
			      const char  *iid,
			      gpointer     data)
{
	gboolean retval = FALSE;

	if (iid == NULL || !strcmp (iid, "OAFIID:GNOME_VoiceControlApplet"))
		retval = voice_control_applet_fill (VOICE_CONTROL_APPLET (applet));

	return retval;
}

static void
voice_control_applet_destroy (GtkObject *object)
{
	VoiceControlApplet *voice_control = (VoiceControlApplet *) object;

	if (voice_control->about_dialog)
		gtk_widget_destroy (voice_control->about_dialog);
	voice_control->about_dialog = NULL;

	if (voice_control->tooltips)
		g_object_unref (voice_control->tooltips);
	voice_control->tooltips = NULL;

        asr_destroy(voice_control);

	GTK_OBJECT_CLASS (voice_control_applet_parent_class)->destroy (object);
}

static void
voice_control_applet_init (VoiceControlApplet      *voice_control)
{
	int i;
	
	voice_control->about_dialog = NULL;
        voice_control->tooltip = NULL;

	for (i = 0; i < N_VOICE_CONTROL_LISTENERS; i++)
		voice_control->listeners [i] = 0;

	panel_applet_set_flags (PANEL_APPLET (voice_control),
				PANEL_APPLET_EXPAND_MINOR);

        voice_control->lang = g_strdup(setlocale(LC_ALL, NULL));
        voice_control->asr = NULL;

        asr_init(voice_control);

	voice_control->spi_listener = g_object_new (CONTROL_SPI_LISTENER_TYPE, NULL);
	g_signal_connect_object (voice_control->spi_listener, "changed", 
				 G_CALLBACK (voice_control_ui_changed), voice_control, 0);

}
          
static void
voice_control_applet_class_init (VoiceControlAppletClass *klass)
{
	PanelAppletClass *applet_class     = (PanelAppletClass *) klass;
	GtkObjectClass   *gtk_object_class = (GtkObjectClass *) klass;

	voice_control_applet_parent_class = g_type_class_peek_parent (klass);
	gtk_object_class->destroy = voice_control_applet_destroy;
}

static const gchar*
voice_control_get_menu_xml()
{
    const gchar* voice_control_menu_xml = g_strdup_printf(
        "<popup name=\"button3\">\n"
        "   <menuitem name=\"Start Control\" verb=\"VoiceControlStart\" label=\"%s\"\n"
    	"	    pixtype=\"stock\" pixname=\"gtk-media-play\"/>\n"
//	"   <menuitem name=\"Start Keypress Control\" verb=\"KeypressVoiceControlStart\" label=\"%s\"\n"
//	"	    pixtype=\"stock\" pixname=\"gtk-media-play\"/>\n"
        "   <menuitem name=\"Stop Control\" verb=\"VoiceControlStop\" label=\"%s\"\n"
    	"	    pixtype=\"stock\" pixname=\"gtk-media-pause\"/>\n"
        "   <menuitem name=\"Change Language\" verb=\"ChangeLanguage\" label=\"%s\"\n"
    	"	    pixtype=\"stock\" pixname=\"gtk-preferences\"/>\n"
        "   <separator/>\n"
        "   <menuitem name=\"Clock Help Item\" verb=\"VoiceControlHelp\" label=\"%s\"\n"
    	"	    pixtype=\"stock\" pixname=\"gtk-help\"/>\n"
        "   <menuitem name=\"Voice Control About Item\" verb=\"VoiceControlAbout\" label=\"%s\"\n"
    	"	    pixtype=\"stock\" pixname=\"gtk-about\"/>\n"
        "</popup>\n",
        _("_Start Control"),
//        _("_Start Keypress Control"),
        _("_Stop Control"),
        _("_Change Language"),
        _("_Help"),
        _("_About"));

    return voice_control_menu_xml;
}

void  voice_control_set_state (VoiceControlApplet *voice_control, gint state, gchar* text)
{
    switch(state)
    {
        case INIT:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_STOP, GTK_ICON_SIZE_MENU);
            voice_control_set_text (voice_control, _("Init"), _("Loading acoustic and language model, wait a bit"));
            voice_control->tooltip = NULL;
            voice_control->state = INIT;
            break;

        case ERROR:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
            if(text)
            {
                voice_control_set_text (voice_control, _("Error"), _(text));
                voice_control->tooltip = g_strdup(text);
            }
            else if (voice_control->state == ERROR)
            {
                if (voice_control->tooltip != NULL)
                {
                    voice_control_set_text (voice_control, _("Error"), _(voice_control->tooltip));
                }
                else
                    voice_control_set_text (voice_control, _("Error"), NULL);
            }
            else
            {
                voice_control_set_text (voice_control, _("Error"), _("Application error, try again later"));
                voice_control->tooltip = g_strdup("Application error, try again later");
            }

            voice_control->state = ERROR;
            
            break;

        case STOP:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
            voice_control_set_text (voice_control, _("Idle"), _("Choose Start Control to start"));
            voice_control->tooltip = NULL;
            voice_control->state = STOP;
            break;

        case READY:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
            voice_control_set_text (voice_control, _("Ready"), _("Ready for input"));
            voice_control->tooltip = NULL;
            voice_control->state = READY;
            break;

        case PAUSE:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_MENU);
            voice_control_set_text (voice_control, _("Paused"), _("Recognizer paused"));
            voice_control->tooltip = NULL;
            voice_control->state = PAUSE;
            break;

        case LOAD:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_STOP, GTK_ICON_SIZE_MENU);
            voice_control_set_text (voice_control, _("Calibration"), _("Tuning up acoustic parameters"));
            voice_control->tooltip = NULL;
            voice_control->state = LOAD;
            break;

        case LISTEN:
            gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
            voice_control_set_text (voice_control, _("Listening"), _("Processing speech"));
            voice_control->tooltip = NULL;
            voice_control->state = LISTEN;
            break;

        case TEXT:
            if(text != NULL)
            {
                gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
                voice_control_set_text (voice_control, text, NULL);
                voice_control->tooltip = g_strdup(text);
            }
            else if (voice_control->tooltip != NULL)
            {
                gtk_image_set_from_stock(GTK_IMAGE(voice_control->frame), GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
                voice_control_set_text (voice_control, voice_control->tooltip, NULL);
            }
            break;

            voice_control->state = TEXT;
            
        default:
            return;
    }
}

gint  voice_control_get_state (VoiceControlApplet *voice_control)
{
    return voice_control->state;
}

int main (int argc, char *argv [])
{				
	int           retval;
 	static GtkWidget *window, *applet;
 	GError *error = NULL;
 	GOptionContext *context;
        GnomeProgram *program;
		
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

 	g_thread_init (NULL);
	
 	context = g_option_context_new (_("- voice control applet"));
 	g_option_context_add_main_entries (context, applet_control_options, GETTEXT_PACKAGE);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	
	gtk_init (&argc, &argv);						
	if (!bonobo_init (&argc, argv)) {					
		g_printerr ("Cannot initialize bonobo.n");
		g_option_context_free (context);
		return 1;							
	}									

        asr_setup (&argc, &argv, context);

        if (!g_option_context_parse (context, &argc, &argv, &error)) {
           g_error (_("Failed to parse option: %s\n"), error->message);
	   exit (1);
        }

	gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (),
					   APPNAME_DATA_DIR G_DIR_SEPARATOR_S 
		    			   "icons");

        program = gnome_program_init ("voice-control-applet", VERSION,
				      LIBGNOMEUI_MODULE,
				      argc, argv,
				      GNOME_CLIENT_PARAM_SM_CONNECT, FALSE,
				      GNOME_PARAM_NONE);

	SPI_init ();

	if (o_window) {
		window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window), _("Voice Control"));
		g_signal_connect (G_OBJECT (window), "destroy",
		      G_CALLBACK (gtk_main_quit), NULL);

		applet = g_object_new(voice_control_applet_get_type(), NULL);
		
		gtk_widget_set_size_request (GTK_WIDGET (window), 250, 25);
		
		retval = voice_control_applet_factory(PANEL_APPLET (applet), NULL, NULL);
	
		gtk_widget_reparent(applet, window);

		gtk_widget_show_all(window);
		gtk_main();
	} else {
        	retval = panel_applet_factory_main ("OAFIID:GNOME_VoiceControlApplet_Factory",
			     voice_control_applet_get_type (),
			     voice_control_applet_factory,
			     NULL);
	}

        g_object_unref (program);
	SPI_exit ();

	g_option_context_free (context);

	return retval;
}
