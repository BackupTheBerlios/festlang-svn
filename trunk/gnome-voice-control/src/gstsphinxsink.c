/* GStreamer
 * Copyright (C)  2007 Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gstsphinxsink.h"

#include <locale.h>
#include <string.h>


static char *sphinx_command = 
"voice-control "
"-live TRUE "
"-verbose 0 "
"-langwt 6.5 "
"-fwdflatlw 8.5 -rescorelw 9.5 -ugwt 0.5 -fillpen 1e-10 -silpen 0.005 "
"-inspen 0.65 -top 1 -topsenfrm 3 -topsenthresh -70000 -beam 2e-06 "
"-npbeam 2e-06 -lpbeam 2e-05 -lponlybeam 0.0005 -nwbeam 0.0005 -fwdflat FALSE "
"-fwdflatbeam 1e-08 -fwdflatnwbeam 0.0003 -bestpath TRUE "
"-8bsen TRUE "
"-sendumpfn " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k/sendump "
"-ndictfn " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k/noisedict "
"-phnfn " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k/phone "
"-mapfn " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k/map "
"-hmmdir " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k "
"-hmmdirlist " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k "
"-cbdir " SPHINX2_PREFIX "/share/sphinx2/model/hmm/6k "
"-fsgfn " GNOMEDATADIR "/gnome-voice-control/desktop-control.fsg "
"-dictfn " GNOMEDATADIR "/gnome-voice-control/desktop-control.dict ";

static void
gst_sphinx_decoder_init (void)
{
    char **argv;
    int argc;

    /* FIXME: required for LM reading */
    setlocale (LC_ALL, "C");
    
    g_shell_parse_argv (sphinx_command, &argc, &argv, NULL);
    
    fbs_init (argc, argv);
    
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
gst_sphinx_sink_class_init (GstSphinxSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSinkClass *gstbase_sink_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstbase_sink_class = GST_BASE_SINK_CLASS (klass);

  gstbase_sink_class->start = GST_DEBUG_FUNCPTR (gst_sphinx_sink_start);
  gstbase_sink_class->stop = GST_DEBUG_FUNCPTR (gst_sphinx_sink_stop);
  gstbase_sink_class->render = GST_DEBUG_FUNCPTR (gst_sphinx_sink_render);

  gst_sphinx_sink_signals[SIGNAL_INITIALIZATION] =
      g_signal_new ("initialization", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, initialization), NULL, NULL,
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
}

int32 
gst_sphinx_sink_ad_read(ad_rec_t *ad, int16 *buf, int32 max)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (((GstSphinxSinkAd *)ad)->self);
  int len;

  len = max > (GST_BUFFER_SIZE (sphinxsink->buffer) / 2) ? (GST_BUFFER_SIZE (sphinxsink->buffer) / 2) : max;
  
  memcpy ((void *)buf, GST_BUFFER_DATA (sphinxsink->buffer), len * 2);
  
  return len;
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


static GstFlowReturn gst_sphinx_sink_render (GstBaseSink * asink, GstBuffer * buffer)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);

  int length = GST_BUFFER_SIZE (buffer);
  
    if (!sphinxsink->ad.initialized) {
          g_signal_emit (sphinxsink,
	        gst_sphinx_sink_signals[SIGNAL_INITIALIZATION], 0, NULL);
	  gst_sphinx_decoder_init ();
	  sphinxsink->ad.initialized = TRUE;
  }

  if (!sphinxsink->ad.calibrated) {
        int result;

        if (!sphinxsink->ad.calibrate_started) {
    	    g_signal_emit (sphinxsink,
		    gst_sphinx_sink_signals[SIGNAL_CALIBRATION], 0, NULL);
	    sphinxsink->ad.calibrate_started = TRUE;
	}

	result = cont_ad_calib_loop (sphinxsink->cont, (int16 *)GST_BUFFER_DATA (buffer), length / 2);
	
	if (result == 0) {
	    sphinxsink->ad.calibrated = TRUE;
    	    sphinxsink->ad.listening = 0;
    	    
	    g_signal_emit (sphinxsink,
	        gst_sphinx_sink_signals[SIGNAL_READY], 0, NULL);
	}
  } else {
	int32 k;
	int16 adbuf[4096];
	
	sphinxsink->buffer = buffer;
	k = cont_ad_read (sphinxsink->cont, adbuf, 4096);
	
	if (k == 0 && sphinxsink->last_ts == 0) {
	      return GST_FLOW_OK;
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

#if DUMPRAW 
  FILE *f;
  f = fopen ("dump.raw", "ab");
  fwrite (data, 1, length, f);
  fclose (f);
#endif

  return GST_FLOW_OK;
}

int
gst_sphinx_construct_trans_list (GSList *words, s2_fsg_trans_t **trans_list)
{
	GSList *l;
	s2_fsg_trans_t *transitions;
	int n, i, j;
	
	n = g_slist_length (words);
	
	transitions = g_new0(s2_fsg_trans_t, 2 * n);
	
	for (l = words, i = 0, j = 1; l; l = l->next, i+=2, j++)
	  {
		transitions[i].from_state = 0;
		transitions[i].to_state = j;
		transitions[i].prob = 1.0 / n;
		transitions[i].word = (gchar *)l->data;
		transitions[i].next = transitions + i + 1;

		transitions[i+1].from_state = j;
		transitions[i+1].to_state = n + 1;
		transitions[i+1].prob = 1.0;
		transitions[i+1].word = NULL;
		transitions[i+1].next = transitions + i + 2;
	  }
	transitions[2 * n - 1].next = NULL;
	*trans_list = transitions;
	
	return n + 2;
}

void gst_sphinx_sink_set_fsg (GstSphinxSink *sink, GSList *words)
{		
	s2_fsg_t fsg;
	s2_fsg_trans_t *trans_list;
	
	if (!sink->ad.calibrated)
		    return;
	
	fsg.name = "desktop-control";	
	fsg.n_state = gst_sphinx_construct_trans_list (words, &trans_list);
	fsg.start_state = 0;
	fsg.final_state = fsg.n_state - 1;
	fsg.trans_list = trans_list;
	
	uttproc_del_fsg ("desktop-control");
	
	uttproc_load_fsg (&fsg, 1, 1, 0.009, 0.0, 1.0);
	
	uttproc_set_fsg ("desktop-control");
	
	g_message ("New fsg is set");
}

