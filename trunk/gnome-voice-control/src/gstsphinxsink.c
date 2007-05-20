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
#  include <config.h>
#endif

#include "gstsphinxsink.h"

#include <locale.h>
#include <s2types.h>
#include <fbs.h>

static char *sphinx_command = 
"-live TRUE -ctloffset 0 "
"-ctlcount 100000000 "
"-langwt 6.5 "
"-fwdflatlw 8.5 -rescorelw 9.5 -ugwt 0.5 -fillpen 1e-10 -silpen 0.005 "
"-inspen 0.65 -top 1 -topsenfrm 3 -topsenthresh -70000 -beam 2e-06 "
"-npbeam 2e-06 -lpbeam 2e-05 -lponlybeam 0.0005 -nwbeam 0.0005 -fwdflat FALSE "
"-fwdflatbeam 1e-08 -fwdflatnwbeam 0.0003 -bestpath TRUE "
"-8bsen TRUE "
"-cepdir /home/shmyrev/local/share/sphinx2/model/lm/turtle/ctl "
"-datadir /home/shmyrev/local/share/sphinx2/model/lm/turtle/ctl "
"-kbdumpdir /home/shmyrev/local/share/sphinx2 "
"-dictfn /home/shmyrev/local/share/sphinx2/model/lm/turtle/turtle.dic "
"-ndictfn /home/shmyrev/local/share/sphinx2/model/hmm/6k/noisedict "
"-lmfn /home/shmyrev/local/share/sphinx2/model/lm/turtle/turtle.lm "
"-phnfn /home/shmyrev/local/share/sphinx2/model/hmm/6k/phone "
"-mapfn /home/shmyrev/local/share/sphinx2/model/hmm/6k/map "
"-hmmdir /home/shmyrev/local/share/sphinx2/model/hmm/6k "
"-hmmdirlist /home/shmyrev/local/share/sphinx2/model/hmm/6k "
"-sendumpfn /home/shmyrev/local/share/sphinx2/model/hmm/6k/sendump "
"-cbdir /home/shmyrev/local/share/sphinx2/model/hmm/6k ";

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
    GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (gst_sphinx_sink_debug);

static const GstElementDetails gst_sphinx_sink_details =
GST_ELEMENT_DETAILS ("Sphinx Sink",
    "Sink",
    "Sink to recognize audio",
    "Nickolay V. Shmyrev <nshmyrev@yandex.ru>");


/* SphinxSink signals and args */
enum
{
  DECODED_SIGNAL,
  LAST_SIGNAL
};

#define DEFAULT_DUMP FALSE

enum
{
  PROP_0,
  PROP_DUMP,
};

static void gst_sphinx_sink_init_interfaces (GType type)
{
    return;
}

GST_BOILERPLATE_FULL (GstSphinxSink, gst_sphinx_sink, GstAudioSink,
    GST_TYPE_AUDIO_SINK, 
    gst_sphinx_sink_init_interfaces);

static void gst_sphinx_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_sphinx_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_sphinx_sink_prepare (GstAudioSink * asink, GstRingBufferSpec * spec);
static gboolean gst_sphinx_sink_unprepare (GstAudioSink * asink);
static guint	gst_sphinx_sink_write (GstAudioSink * asink, gpointer data, guint length);

static guint gst_sphinx_sink_signals[LAST_SIGNAL] = { 0 };

static void
gst_sphinx_sink_base_init (gpointer g_class)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);

  gst_sphinx_decoder_init ();

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sinktemplate));
  gst_element_class_set_details (gstelement_class, &gst_sphinx_sink_details);
}

static void
gst_sphinx_sink_class_init (GstSphinxSinkClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstAudioSinkClass *gstaudio_sink_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstaudio_sink_class = GST_AUDIO_SINK_CLASS (klass);

  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_sphinx_sink_set_property);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_sphinx_sink_get_property);

  g_object_class_install_property (gobject_class, PROP_DUMP,
      g_param_spec_boolean ("dump", "Dump", "Dump buffer contents to stdout",
          DEFAULT_DUMP, G_PARAM_READWRITE));

  gstaudio_sink_class->prepare = GST_DEBUG_FUNCPTR (gst_sphinx_sink_prepare);
  gstaudio_sink_class->unprepare = GST_DEBUG_FUNCPTR (gst_sphinx_sink_unprepare);
  gstaudio_sink_class->write = GST_DEBUG_FUNCPTR (gst_sphinx_sink_write);
}

static void
gst_sphinx_sink_init (GstSphinxSink * sphinxsink, GstSphinxSinkClass * g_class)
{
  sphinxsink->dump = DEFAULT_DUMP;
  GST_BASE_SINK (sphinxsink)->sync = TRUE;
}

static void
gst_sphinx_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstSphinxSink *sink;

  sink = GST_SPHINX_SINK (object);

  switch (prop_id) {
    case PROP_DUMP:
      sink->dump = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_sphinx_sink_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstSphinxSink *sink;

  sink = GST_SPHINX_SINK (object);

  switch (prop_id) {
    case PROP_DUMP:
      g_value_set_boolean (value, sink->dump);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_sphinx_sink_prepare (GstAudioSink * asink, GstRingBufferSpec * spec)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);

  uttproc_begin_utt (NULL);

  return TRUE;
}

static gboolean
gst_sphinx_sink_unprepare (GstAudioSink * asink)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);
  
  int32 fr;
  char *hyp;

  uttproc_end_utt ();
  if (uttproc_result (&fr, &hyp, 1) < 0)
      g_warning ("uttproc_result failed");
  else
      g_message ("%d: %s", fr, hyp);

  return TRUE;
}

static guint
gst_sphinx_sink_write (GstAudioSink * asink, gpointer data, guint length)
{
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (asink);
  
  uttproc_rawdata (data, length, 0);
  
  return length;
}

