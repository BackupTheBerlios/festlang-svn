/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2010  Consorcio Fernando de los Rios - Junta de Andalucia
 * Developed by Intelligent Dialogue Systems S.L. <info@indisys.es>
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * asrsphinx.c:
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

#include <gst/gstelement.h>
#include <gst/gstplugin.h>
#include <glib-2.0/glib/goption.h>

#include "voicecontrol.h"
#include "spi-listener.h"

#include "asr.h"
#include "gstsphinxsink.h"

typedef struct {
    GstElement*        pipeline;
    GstElement* 	   sink;
} ASRSphinx;

//////////////////////////
// Pipeline Actions
//////////////////////////

static gboolean
on_sink_stop (gpointer data)
{
        VoiceControlApplet * voice_control = VOICE_CONTROL_APPLET (data);

	gst_element_set_state (((ASRSphinx *) voice_control->asr)->pipeline, GST_STATE_NULL);
	voice_control_set_state(voice_control, STOP, NULL);
	control_spi_listener_stop (voice_control->spi_listener);

        return TRUE;
}

static void
on_sink_initialization (VoiceControlApplet *voice_control)
{
	voice_control_set_state(voice_control, INIT, NULL);
}

static void
on_sink_after_initialization (VoiceControlApplet *voice_control)
{
	GSList *commands;

	// Set this in main thread as well, sphinxbase doesn't do that automatically unfortunately
	// for all the threads. Only main thread which called ps_init dumps to stderr
	err_set_logfp(stderr); 

	//commands = voice_control_action_append_commands (NULL);
	//gst_sphinx_sink_set_grammar (GST_SINK(((ASRSphinx *) voice_control->asr)->sink), commands);
	//g_slist_free (commands);
}

static void
on_sink_calibration (VoiceControlApplet *voice_control)
{
	voice_control_set_state(voice_control, LOAD, NULL);
}

static void
on_sink_listening (VoiceControlApplet *voice_control)
{
	voice_control_set_state(voice_control, LISTEN, NULL);
}

static gboolean
on_sink_ready (gpointer data)
{
	VoiceControlApplet * voice_control = VOICE_CONTROL_APPLET (data);
	voice_control_set_state(voice_control, READY, NULL);

	return FALSE;
}

static void
on_sink_error (VoiceControlApplet * voice_control)
{
	voice_control_set_state(voice_control, ERROR, "");
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
on_sink_message (VoiceControlApplet *voice_control, const gchar *message)
{
        voice_control_set_state(voice_control, TEXT, (gchar*) message);

        // Look for a predefined voice control command
	if (voice_control_action_process_result (message))
		return;

        // Look for other possible event
	voice_control_process_action (voice_control, message);

	return;
}

/////////////////////////////////
// Auxiliary functions
/////////////////////////////////

static gboolean
sphinx_pipeline_bus_callback (GstBus *bus,
				     GstMessage *message,
				     gpointer data)
{
	VoiceControlApplet *applet = (VoiceControlApplet *) data;

	switch (GST_MESSAGE_TYPE (message)) {
		case GST_MESSAGE_ELEMENT: {
		    const GstStructure *str = gst_message_get_structure (message);
		    const gchar *name = gst_structure_get_name (str);

		    if (applet->ready_timeout > 0) {
			    g_source_remove (applet->ready_timeout);
			    applet->ready_timeout = 0;
		    }

		    if (strcmp (name, "initialization") == 0) {	
                        on_sink_initialization (applet);            
		    } else if (strcmp (name, "after_initialization") == 0) {
			    on_sink_after_initialization (applet);
		    } else if (strcmp (name, "calibration") == 0) {
			    on_sink_calibration (applet);
		    } else if (strcmp (name, "listening") == 0) {
			    on_sink_listening (applet);
		    } else if (strcmp (name, "ready") == 0) {
			    on_sink_ready (applet);
		    } else if (strcmp (name, "message") == 0) {
			    on_sink_message (applet, gst_structure_get_string (str, "text"));
			    applet->ready_timeout = g_timeout_add_seconds (10, on_sink_ready, applet);
		    } else if (strcmp (name, "error") == 0) {
			    on_sink_error (applet);
                            applet->ready_timeout = g_timeout_add_seconds (5, on_sink_stop, applet);
		    }
		}
	}
	return TRUE;
}

static void
sphinx_create_pipeline (VoiceControlApplet* applet)
{
	GError *error = NULL;
	GstBus *bus;

	((ASRSphinx *) applet->asr)->pipeline = gst_parse_launch ("gconfaudiosrc ! sphinxsink name=sink", &error);

	if (error != NULL) {
		g_warning ("Can't create pipeline: %s", error->message);
		return;
	}

	((ASRSphinx *) applet->asr)->sink = gst_bin_get_by_name(GST_BIN (((ASRSphinx *) applet->asr)->pipeline),
	                                          "sink");
	if (!((ASRSphinx *) applet->asr)->sink) {
    		g_warning ("Pipeline has no sink");
		return;
	}

	bus = gst_pipeline_get_bus (GST_PIPELINE (((ASRSphinx *) applet->asr)->pipeline));
	gst_bus_add_watch (bus, sphinx_pipeline_bus_callback, applet);
	gst_object_unref (bus);

    	return;
}

static gboolean
sphinx_plugin_init (GstPlugin * plugin)
{
  gboolean res = TRUE;

  res &= gst_element_register (plugin, "sphinxsink", GST_RANK_PRIMARY, GST_TYPE_RECOG_SINK);
  return res;
}

GST_PLUGIN_DEFINE_STATIC (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "sphinxplugin",
    "Sphinx recognizer plugin",
    sphinx_plugin_init, VERSION, GST_LICENSE_UNKNOWN, PACKAGE_NAME, PACKAGE_NAME);

///////////////////////////////
// ASR Interface
///////////////////////////////

void
asr_setup(int* argc, char **argv [], gpointer data)
{
    g_option_context_add_group ((GOptionContext *) data, gst_init_get_option_group());
    
    gst_init (argc, argv);
}

void
asr_init(VoiceControlApplet* applet)
{
    applet->asr = g_malloc(sizeof(ASRSphinx));

    sphinx_create_pipeline(applet);
}

void
asr_destroy(VoiceControlApplet* applet)
{
    if (((ASRSphinx * ) applet->asr)->pipeline)
	g_object_unref (((ASRSphinx * ) applet->asr)->pipeline);
    ((ASRSphinx * ) applet->asr)->pipeline = NULL;
    g_free(applet->asr);
}

void
asr_start(VoiceControlApplet* voice_control)
{
    gchar* lang = NULL;
    gchar ** splits = NULL;

    splits = g_strsplit(voice_control->lang, "_", 2);
    lang = splits[0];

    if(strcmp(lang, "en") == 0)
    {
      gst_element_set_state (((ASRSphinx *)voice_control->asr)->pipeline, GST_STATE_PLAYING);
    }
    else
    {
      #ifdef ENABLE_DEBUG_DUMP
        g_message("Language %s not supported for Sphinx", lang);
      #endif
    }

   if(splits != NULL)
      g_strfreev(splits);
}

void
asr_pause(VoiceControlApplet* voice_control)
{    
    gst_element_set_state (((ASRSphinx *) voice_control->asr)->pipeline, GST_STATE_PAUSED);
    voice_control_set_state(voice_control, PAUSE, NULL);
}

void
asr_stop(VoiceControlApplet* applet)
{
    gst_element_set_state (((ASRSphinx *) applet->asr)->pipeline, GST_STATE_NULL);
}

gboolean
asr_running(VoiceControlApplet* applet)
{
    return gst_sphinx_sink_running(GST_SINK(((ASRSphinx *) applet->asr)->sink));
}

void
asr_set_grammar(VoiceControlApplet* applet, GSList* commands)
{
    // This peace of code don't work
    // gst_sphinx_sink_set_grammar(GST_SINK(((ASRSphinx *)applet->asr)->sink), commands);
}

void
asr_set_language(VoiceControlApplet* applet, gchar* lang)
{
    // Not implemented yet
}
