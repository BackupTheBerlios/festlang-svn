/* This file is a part of GStreamer
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * gstsphinxsink.c:
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
/**
 * SECTION:element-sphinxsink
 * @short_description: sink for speech recognition
 *
 * Simple sink for voice recognition
 */

#include <config.h>

#include "gstsphinxsink.h"

#include <locale.h>
#include <string.h>

static char *sphinx_command = 
"voice-control "
"-fwdflat no "
"-bestpath yes "
"-hmm " POCKETSPHINX_PREFIX "/share/pocketsphinx/model/hmm/wsj1 "
"-fsg " GNOMEDATADIR "/gnome-voice-control/desktop-control.fsg "
"-dict " GNOMEDATADIR "/gnome-voice-control/desktop-control.dict ";

static void
gst_sphinx_decoder_init (void)
{
    char **argv;
    int argc;

    g_shell_parse_argv (sphinx_command, &argc, &argv, NULL);

    setlocale (LC_ALL, "C");
    fbs_init (argc, argv);    
    setlocale (LC_ALL, "");

    g_strfreev (argv);
}

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "endianness = (int) LITTLE_ENDIAN, "
        "signed = (boolean) TRUE, "
        "width = (int) 16, "
        "depth = (int) 16, "
        "rate = (int) 16000, "
        "channels = (int) 1")
);

static const GstElementDetails gst_sphinx_sink_details =
GST_ELEMENT_DETAILS ("Sphinx Sink",
    "Sink",
    "Sink to recognize audio",
    "Nickolay V. Shmyrev <nshmyrev@yandex.ru>");


/* SphinxSink signals and args */
enum
{
  SIGNAL_INITIALIZATION,
  SIGNAL_AFTER_INITIALIZATION,
  SIGNAL_CALIBRATION,
  SIGNAL_LISTENING,
  SIGNAL_READY,
  SIGNAL_MESSAGE,
  LAST_SIGNAL
};

static void
_do_init (GType filesink_type)
{	
    return;
}

GST_BOILERPLATE_FULL (GstSphinxSink, gst_sphinx_sink, GstBaseSink,
    GST_TYPE_BASE_SINK,
    _do_init);

static gboolean gst_sphinx_sink_start(GstBaseSink * asink);
static gboolean gst_sphinx_sink_stop (GstBaseSink * asink);
static GstFlowReturn gst_sphinx_sink_render (GstBaseSink * sink, GstBuffer * buffer);

static guint gst_sphinx_sink_signals[LAST_SIGNAL] = { 0 };

static void
gst_sphinx_sink_base_init (gpointer g_class)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sinktemplate));
  gst_element_class_set_details (gstelement_class, &gst_sphinx_sink_details);
}


static void
gst_sphinx_sink_finalize (GObject * gobject)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (gobject);

  gst_adapter_clear (sphinxsink->adapter);
  g_object_unref (sphinxsink->adapter);

  GST_CALL_PARENT (G_OBJECT_CLASS, finalize, (gobject));
}

static void
gst_sphinx_sink_class_init (GstSphinxSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbase_sink_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstbase_sink_class = GST_BASE_SINK_CLASS (klass);

  gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_sphinx_sink_finalize);
  
  gstbase_sink_class->start = GST_DEBUG_FUNCPTR (gst_sphinx_sink_start);
  gstbase_sink_class->stop = GST_DEBUG_FUNCPTR (gst_sphinx_sink_stop);
  gstbase_sink_class->render = GST_DEBUG_FUNCPTR (gst_sphinx_sink_render);

  gst_sphinx_sink_signals[SIGNAL_INITIALIZATION] =
      g_signal_new ("initialization", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, initialization), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, G_TYPE_NONE);

  gst_sphinx_sink_signals[SIGNAL_AFTER_INITIALIZATION] =
      g_signal_new ("after_initialization", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, after_initialization), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, G_TYPE_NONE);

  gst_sphinx_sink_signals[SIGNAL_CALIBRATION] =
      g_signal_new ("calibration", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, calibration), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, G_TYPE_NONE);

  gst_sphinx_sink_signals[SIGNAL_LISTENING] =
      g_signal_new ("listening", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, listening), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, G_TYPE_NONE);

  gst_sphinx_sink_signals[SIGNAL_READY] =
      g_signal_new ("ready", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, calibration), NULL, NULL,
      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, G_TYPE_NONE);

  gst_sphinx_sink_signals[SIGNAL_MESSAGE] =
      g_signal_new ("message", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, message), NULL, NULL,
      g_cclosure_marshal_VOID__STRING, G_TYPE_NONE, 1, G_TYPE_STRING);
}

static void
gst_sphinx_sink_init (GstSphinxSink * sphinxsink, GstSphinxSinkClass * g_class)
{
  GST_BASE_SINK (sphinxsink)->sync = FALSE;
  sphinxsink->adapter = gst_adapter_new ();
}

#define REQUIRED_FRAME_SAMPLES 1024
#define REQUIRED_FRAME_BYTES  REQUIRED_FRAME_SAMPLES * 2

int32 
gst_sphinx_sink_ad_read(ad_rec_t *ad, int16 *buf, int32 max)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (((GstSphinxSinkAd *)ad)->self);

  memcpy ((void *)buf, gst_adapter_peek (sphinxsink->adapter, REQUIRED_FRAME_BYTES), REQUIRED_FRAME_BYTES);
  
  return REQUIRED_FRAME_SAMPLES;
}

static gboolean
gst_sphinx_sink_start (GstBaseSink * asink)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);

  sphinxsink->ad.self = sphinxsink;
  sphinxsink->ad.sps = 16000;
  sphinxsink->ad.self = sphinxsink;
  sphinxsink->ad.bps = sizeof(int16);
  sphinxsink->ad.calibrated = FALSE;
  sphinxsink->ad.calibrate_started = FALSE;
  
  sphinxsink->cont = cont_ad_init ((ad_rec_t*)&sphinxsink->ad, gst_sphinx_sink_ad_read);
  
  return TRUE;
}

static gboolean
gst_sphinx_sink_stop (GstBaseSink * asink)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);
  
  cont_ad_close (sphinxsink->cont);
  
  return TRUE;
}

static void gst_sphinx_sink_calibrate_chunk (GstSphinxSink *sphinxsink)
{
        int result;
	char *adbuf;

        if (!sphinxsink->ad.calibrate_started) {
    	    g_signal_emit (sphinxsink,
		    gst_sphinx_sink_signals[SIGNAL_CALIBRATION], 0, NULL);
	    sphinxsink->ad.calibrate_started = TRUE;
	}

	result = cont_ad_calib_loop (sphinxsink->cont, 
				    (int16 *)gst_adapter_peek(sphinxsink->adapter, REQUIRED_FRAME_BYTES),
				    REQUIRED_FRAME_SAMPLES);
	
	if (result == 0) {
	    sphinxsink->ad.calibrated = TRUE;
    	    sphinxsink->ad.listening = 0;
    	    
	    g_signal_emit (sphinxsink,
	        gst_sphinx_sink_signals[SIGNAL_READY], 0, NULL);
	}
}

static void gst_sphinx_sink_process_chunk (GstSphinxSink *sphinxsink)
{
	int32 k;
	int16 adbuf[REQUIRED_FRAME_SAMPLES];
	
	k = cont_ad_read (sphinxsink->cont, adbuf, REQUIRED_FRAME_SAMPLES);
	
	if (k == 0 && sphinxsink->last_ts == 0) {
	      return;
	} else if (k == 0 && sphinxsink->cont->read_ts - sphinxsink->last_ts > 
			    DEFAULT_SAMPLES_PER_SEC) {
	      int32 fr;
	      char *hyp = NULL;
  
	      uttproc_end_utt ();
   	      if (uttproc_result (&fr, &hyp, 1) < 0) {
		      g_warning ("uttproc_result failed");
	      } else {
		      if (hyp != NULL)
		        g_signal_emit (sphinxsink,
			               gst_sphinx_sink_signals[SIGNAL_MESSAGE], 
			               0, hyp);
	      }

	      sphinxsink->last_ts = 0;
	      sphinxsink->ad.listening = 0;
	      g_signal_emit (sphinxsink,
	          gst_sphinx_sink_signals[SIGNAL_READY], 0, NULL);

	} else if (k != 0) {
	     if (sphinxsink->ad.listening == 0) {
	    	    uttproc_begin_utt (NULL);
	    	    sphinxsink->ad.listening = 1;
	     }
	
	     uttproc_rawdata (adbuf, k, 1);
	     sphinxsink->last_ts = sphinxsink->cont->read_ts;

	     g_signal_emit (sphinxsink,
	          gst_sphinx_sink_signals[SIGNAL_LISTENING], 0, NULL);
	}
}

static GstFlowReturn gst_sphinx_sink_render (GstBaseSink * asink, GstBuffer * buffer)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);

#ifdef ENABLE_DEBUG_DUMP
  FILE *dump_fd;
  
  dump_fd = fopen ("dump.raw", "a+");
  fwrite (buffer->data, 1, buffer->size, dump_fd);
  fclose (dump_fd);
#endif

  if (!sphinxsink->ad.initialized) {
          g_signal_emit (sphinxsink,
	        gst_sphinx_sink_signals[SIGNAL_INITIALIZATION], 0, NULL);
	  gst_sphinx_decoder_init ();
          g_signal_emit (sphinxsink,
	        gst_sphinx_sink_signals[SIGNAL_AFTER_INITIALIZATION], 0, NULL);
	  sphinxsink->ad.initialized = TRUE;
  }

  gst_buffer_ref (buffer);
  gst_adapter_push (sphinxsink->adapter, buffer);
  
  while (gst_adapter_available (sphinxsink->adapter) >= REQUIRED_FRAME_BYTES) {
	if (sphinxsink->ad.calibrated) {
    		gst_sphinx_sink_process_chunk (sphinxsink);
    	} else {
    		gst_sphinx_sink_calibrate_chunk (sphinxsink);
    	}
        gst_adapter_flush (sphinxsink->adapter, REQUIRED_FRAME_BYTES);
  }

  return GST_FLOW_OK;
}

static int
gst_sphinx_construct_trans_list (GSList *phrases, s2_fsg_trans_t **trans_list)
{
	GSList *l, *word_list;
	gchar **words;
	s2_fsg_trans_t *transitions;
	int n_states, n_transitions, i, j;
	
	n_states = 2; /* Final and initial state */
	word_list = NULL;
	for (l = phrases; l; l = l->next) {
		words = g_strsplit (l->data, " ", 0);
		word_list = g_slist_append (word_list, words);
		n_states += g_strv_length (words);
	}
	n_transitions = n_states - 2;
	
	transitions = g_new0(s2_fsg_trans_t, n_transitions);
	
	for (i = 0, l = word_list; l; l = l->next) {
		words = l->data;
		for (j = 0; words[j] != NULL; j++, i++) {
			transitions[i].from_state = 
				(j == 0) ? 0 : i + 1;
    			transitions[i].to_state = 
    				(words[j+1] == NULL) ? n_states - 1 : i + 2;
	    		transitions[i].prob = 1.0;
			transitions[i].word = g_strdup (words[j]);
			transitions[i].next = transitions + i + 1;

#if DEBUG			
			g_message ("transition number %d from %d to %d word %s",
				    i, 
				    transitions[i].from_state,
				    transitions[i].to_state,
				    transitions[i].word);
#endif
		}
	}
	transitions[n_transitions-1].next = NULL;
	*trans_list = transitions;
	
	for (l = word_list; l; l = l->next)
		g_strfreev (l->data);
	g_slist_free (word_list);
	
	return n_states;
}

void gst_sphinx_sink_set_fsg (GstSphinxSink *sink, GSList *words)
{		
	s2_fsg_t fsg;
	s2_fsg_trans_t *trans_list;
	
	fsg.name = "desktop-control";	
	fsg.n_state = gst_sphinx_construct_trans_list (words, &trans_list);
	fsg.start_state = 0;
	fsg.final_state = fsg.n_state - 1;
	fsg.trans_list = trans_list;
	
	uttproc_del_fsg ("desktop-control");
	
	uttproc_load_fsg (&fsg, 1, 1, 0.009, 0.0, 1.0);
	
	uttproc_set_fsg ("desktop-control");
	
#if DEBUG
	g_message ("New fsg is set");
#endif
}

