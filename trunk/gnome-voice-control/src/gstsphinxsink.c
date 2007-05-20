/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2005 Wim Taymans <wim@fluendo.com>
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
 * @short_description: black hole for data
 * @see_also: #GstSphinxSrc
 *
 * Dummy sink that swallows everything.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gstsphinxsink.h"
#include <gst/gstmarshal.h>

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (gst_sphinx_sink_debug);
#define GST_CAT_DEFAULT gst_sphinx_sink_debug

static const GstElementDetails gst_sphinx_sink_details =
GST_ELEMENT_DETAILS ("Sphinx Sink",
    "Sink",
    "Black hole for data",
    "Erik Walthinsen <omega@cse.ogi.edu>, "
    "Wim Taymans <wim@fluendo.com>, "
    "Mr. 'frag-me-more' Vanderwingo <wingo@fluendo.com>");


/* SphinxSink signals and args */
enum
{
  /* FILL ME */
  SIGNAL_HANDOFF,
  SIGNAL_PREROLL_HANDOFF,
  LAST_SIGNAL
};

#define DEFAULT_SYNC FALSE

#define DEFAULT_STATE_ERROR SPHINX_SINK_STATE_ERROR_NONE
#define DEFAULT_SILENT FALSE
#define DEFAULT_DUMP FALSE
#define DEFAULT_SIGNAL_HANDOFFS FALSE
#define DEFAULT_LAST_MESSAGE NULL
#define DEFAULT_CAN_ACTIVATE_PUSH TRUE
#define DEFAULT_CAN_ACTIVATE_PULL FALSE

enum
{
  PROP_0,
  PROP_STATE_ERROR,
  PROP_SILENT,
  PROP_DUMP,
  PROP_SIGNAL_HANDOFFS,
  PROP_LAST_MESSAGE,
  PROP_CAN_ACTIVATE_PUSH,
  PROP_CAN_ACTIVATE_PULL
};

#define GST_TYPE_SPHINX_SINK_STATE_ERROR (gst_sphinx_sink_state_error_get_type())
static GType
gst_sphinx_sink_state_error_get_type (void)
{
  static GType sphinxsink_state_error_type = 0;
  static const GEnumValue sphinxsink_state_error[] = {
    {SPHINX_SINK_STATE_ERROR_NONE, "No state change errors", "none"},
    {SPHINX_SINK_STATE_ERROR_NULL_READY,
        "Fail state change from NULL to READY", "null-to-ready"},
    {SPHINX_SINK_STATE_ERROR_READY_PAUSED,
        "Fail state change from READY to PAUSED", "ready-to-paused"},
    {SPHINX_SINK_STATE_ERROR_PAUSED_PLAYING,
        "Fail state change from PAUSED to PLAYING", "paused-to-playing"},
    {SPHINX_SINK_STATE_ERROR_PLAYING_PAUSED,
        "Fail state change from PLAYING to PAUSED", "playing-to-paused"},
    {SPHINX_SINK_STATE_ERROR_PAUSED_READY,
        "Fail state change from PAUSED to READY", "paused-to-ready"},
    {SPHINX_SINK_STATE_ERROR_READY_NULL,
        "Fail state change from READY to NULL", "ready-to-null"},
    {0, NULL, NULL},
  };

  if (!sphinxsink_state_error_type) {
    sphinxsink_state_error_type =
        g_enum_register_static ("GstSphinxSinkStateError", sphinxsink_state_error);
  }
  return sphinxsink_state_error_type;
}

#define _do_init(bla) \
    GST_DEBUG_CATEGORY_INIT (gst_sphinx_sink_debug, "sphinxsink", 0, "sphinxsink element");

GST_BOILERPLATE_FULL (GstSphinxSink, gst_sphinx_sink, GstBaseSink,
    GST_TYPE_BASE_SINK, _do_init);

static void gst_sphinx_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_sphinx_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstStateChangeReturn gst_sphinx_sink_change_state (GstElement * element,
    GstStateChange transition);

static GstFlowReturn gst_sphinx_sink_preroll (GstBaseSink * bsink,
    GstBuffer * buffer);
static GstFlowReturn gst_sphinx_sink_render (GstBaseSink * bsink,
    GstBuffer * buffer);
static gboolean gst_sphinx_sink_event (GstBaseSink * bsink, GstEvent * event);

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

  gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_sphinx_sink_set_property);
  gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_sphinx_sink_get_property);

  g_object_class_install_property (gobject_class, PROP_STATE_ERROR,
      g_param_spec_enum ("state_error", "State Error",
          "Generate a state change error", GST_TYPE_SPHINX_SINK_STATE_ERROR,
          DEFAULT_STATE_ERROR, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_LAST_MESSAGE,
      g_param_spec_string ("last_message", "Last Message",
          "The message describing current status", DEFAULT_LAST_MESSAGE,
          G_PARAM_READABLE));
  g_object_class_install_property (gobject_class, PROP_SIGNAL_HANDOFFS,
      g_param_spec_boolean ("signal-handoffs", "Signal handoffs",
          "Send a signal before unreffing the buffer", DEFAULT_SIGNAL_HANDOFFS,
          G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent",
          "Don't produce last_message events", DEFAULT_SILENT,
          G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_DUMP,
      g_param_spec_boolean ("dump", "Dump", "Dump buffer contents to stdout",
          DEFAULT_DUMP, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
      PROP_CAN_ACTIVATE_PUSH,
      g_param_spec_boolean ("can-activate-push", "Can activate push",
          "Can activate in push mode", DEFAULT_CAN_ACTIVATE_PUSH,
          G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
      PROP_CAN_ACTIVATE_PULL,
      g_param_spec_boolean ("can-activate-pull", "Can activate pull",
          "Can activate in pull mode", DEFAULT_CAN_ACTIVATE_PULL,
          G_PARAM_READWRITE));

  /**
   * GstSphinxSink::handoff:
   * @sphinxsink: the sphinxsink instance
   * @buffer: the buffer that just has been received
   * @pad: the pad that received it
   *
   * This signal gets emitted before unreffing the buffer.
   */
  gst_sphinx_sink_signals[SIGNAL_HANDOFF] =
      g_signal_new ("handoff", G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST,
      G_STRUCT_OFFSET (GstSphinxSinkClass, handoff), NULL, NULL,
      gst_marshal_VOID__OBJECT_OBJECT, G_TYPE_NONE, 2,
      GST_TYPE_BUFFER, GST_TYPE_PAD);

  /**
   * GstSphinxSink::preroll-handoff:
   * @sphinxsink: the sphinxsink instance
   * @buffer: the buffer that just has been received
   * @pad: the pad that received it
   *
   * This signal gets emitted before unreffing the buffer.
   *
   * Since: 0.10.7
   */
  gst_sphinx_sink_signals[SIGNAL_PREROLL_HANDOFF] =
      g_signal_new ("preroll-handoff", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (GstSphinxSinkClass, preroll_handoff),
      NULL, NULL, gst_marshal_VOID__OBJECT_OBJECT, G_TYPE_NONE, 2,
      GST_TYPE_BUFFER, GST_TYPE_PAD);

  gstelement_class->change_state =
      GST_DEBUG_FUNCPTR (gst_sphinx_sink_change_state);

  gstbase_sink_class->event = GST_DEBUG_FUNCPTR (gst_sphinx_sink_event);
  gstbase_sink_class->preroll = GST_DEBUG_FUNCPTR (gst_sphinx_sink_preroll);
  gstbase_sink_class->render = GST_DEBUG_FUNCPTR (gst_sphinx_sink_render);
}

static void
gst_sphinx_sink_init (GstSphinxSink * sphinxsink, GstSphinxSinkClass * g_class)
{
  sphinxsink->silent = DEFAULT_SILENT;
  sphinxsink->dump = DEFAULT_DUMP;
  GST_BASE_SINK (sphinxsink)->sync = DEFAULT_SYNC;
  sphinxsink->last_message = g_strdup (DEFAULT_LAST_MESSAGE);
  sphinxsink->state_error = DEFAULT_STATE_ERROR;
  sphinxsink->signal_handoffs = DEFAULT_SIGNAL_HANDOFFS;
}

static void
gst_sphinx_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstSphinxSink *sink;

  sink = GST_SPHINX_SINK (object);

  switch (prop_id) {
    case PROP_SILENT:
      sink->silent = g_value_get_boolean (value);
      break;
    case PROP_STATE_ERROR:
      sink->state_error = g_value_get_enum (value);
      break;
    case PROP_DUMP:
      sink->dump = g_value_get_boolean (value);
      break;
    case PROP_SIGNAL_HANDOFFS:
      sink->signal_handoffs = g_value_get_boolean (value);
      break;
    case PROP_CAN_ACTIVATE_PUSH:
      GST_BASE_SINK (sink)->can_activate_push = g_value_get_boolean (value);
      break;
    case PROP_CAN_ACTIVATE_PULL:
      GST_BASE_SINK (sink)->can_activate_pull = g_value_get_boolean (value);
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
    case PROP_STATE_ERROR:
      g_value_set_enum (value, sink->state_error);
      break;
    case PROP_SILENT:
      g_value_set_boolean (value, sink->silent);
      break;
    case PROP_DUMP:
      g_value_set_boolean (value, sink->dump);
      break;
    case PROP_SIGNAL_HANDOFFS:
      g_value_set_boolean (value, sink->signal_handoffs);
      break;
    case PROP_LAST_MESSAGE:
      GST_OBJECT_LOCK (sink);
      g_value_set_string (value, sink->last_message);
      GST_OBJECT_UNLOCK (sink);
      break;
    case PROP_CAN_ACTIVATE_PUSH:
      g_value_set_boolean (value, GST_BASE_SINK (sink)->can_activate_push);
      break;
    case PROP_CAN_ACTIVATE_PULL:
      g_value_set_boolean (value, GST_BASE_SINK (sink)->can_activate_pull);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_sphinx_sink_event (GstBaseSink * bsink, GstEvent * event)
{
  GstSphinxSink *sink = GST_SPHINX_SINK (bsink);

  if (!sink->silent) {
    const GstStructure *s;
    gchar *sstr;

    GST_OBJECT_LOCK (sink);
    g_free (sink->last_message);

    if ((s = gst_event_get_structure (event)))
      sstr = gst_structure_to_string (s);
    else
      sstr = g_strdup ("");

    sink->last_message =
        g_strdup_printf ("event   ******* E (type: %d, %s) %p",
        GST_EVENT_TYPE (event), sstr, event);
    g_free (sstr);
    GST_OBJECT_UNLOCK (sink);

    g_object_notify (G_OBJECT (sink), "last_message");
  }

  return TRUE;
}

static GstFlowReturn
gst_sphinx_sink_preroll (GstBaseSink * bsink, GstBuffer * buffer)
{
  GstSphinxSink *sink = GST_SPHINX_SINK (bsink);

  if (!sink->silent) {
    GST_OBJECT_LOCK (sink);
    g_free (sink->last_message);

    sink->last_message = g_strdup_printf ("preroll   ******* ");
    GST_OBJECT_UNLOCK (sink);

    g_object_notify (G_OBJECT (sink), "last_message");
  }
  if (sink->signal_handoffs) {
    g_signal_emit (sink,
        gst_sphinx_sink_signals[SIGNAL_PREROLL_HANDOFF], 0, buffer,
        bsink->sinkpad);
  }
  return GST_FLOW_OK;
}

static GstFlowReturn
gst_sphinx_sink_render (GstBaseSink * bsink, GstBuffer * buf)
{
  GstSphinxSink *sink = GST_SPHINX_SINK_CAST (bsink);

  if (!sink->silent) {
    gchar ts_str[64], dur_str[64];

    GST_OBJECT_LOCK (sink);
    g_free (sink->last_message);

    if (GST_BUFFER_TIMESTAMP (buf) != GST_CLOCK_TIME_NONE) {
      g_snprintf (ts_str, sizeof (ts_str), "%" GST_TIME_FORMAT,
          GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buf)));
    } else {
      g_strlcpy (ts_str, "none", sizeof (ts_str));
    }

    if (GST_BUFFER_DURATION (buf) != GST_CLOCK_TIME_NONE) {
      g_snprintf (dur_str, sizeof (dur_str), "%" GST_TIME_FORMAT,
          GST_TIME_ARGS (GST_BUFFER_DURATION (buf)));
    } else {
      g_strlcpy (dur_str, "none", sizeof (dur_str));
    }

    sink->last_message =
        g_strdup_printf ("chain   ******* < (%5d bytes, timestamp: %s"
        ", duration: %s, offset: %" G_GINT64_FORMAT ", offset_end: %"
        G_GINT64_FORMAT ", flags: %d) %p", GST_BUFFER_SIZE (buf), ts_str,
        dur_str, GST_BUFFER_OFFSET (buf), GST_BUFFER_OFFSET_END (buf),
        GST_MINI_OBJECT (buf)->flags, buf);
    GST_OBJECT_UNLOCK (sink);

    g_object_notify (G_OBJECT (sink), "last_message");
  }
  if (sink->signal_handoffs)
    g_signal_emit (G_OBJECT (sink), gst_sphinx_sink_signals[SIGNAL_HANDOFF], 0,
        buf, bsink->sinkpad);

  if (sink->dump) {
    gst_util_dump_mem (GST_BUFFER_DATA (buf), GST_BUFFER_SIZE (buf));
  }

  return GST_FLOW_OK;
}

static GstStateChangeReturn
gst_sphinx_sink_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstSphinxSink *sphinxsink = GST_SPHINX_SINK (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (sphinxsink->state_error == SPHINX_SINK_STATE_ERROR_NULL_READY)
        goto error;
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      if (sphinxsink->state_error == SPHINX_SINK_STATE_ERROR_READY_PAUSED)
        goto error;
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      if (sphinxsink->state_error == SPHINX_SINK_STATE_ERROR_PAUSED_PLAYING)
        goto error;
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      if (sphinxsink->state_error == SPHINX_SINK_STATE_ERROR_PLAYING_PAUSED)
        goto error;
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      if (sphinxsink->state_error == SPHINX_SINK_STATE_ERROR_PAUSED_READY)
        goto error;
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      if (sphinxsink->state_error == SPHINX_SINK_STATE_ERROR_READY_NULL)
        goto error;
      GST_OBJECT_LOCK (sphinxsink);
      g_free (sphinxsink->last_message);
      sphinxsink->last_message = NULL;
      GST_OBJECT_UNLOCK (sphinxsink);
      break;
    default:
      break;
  }

  return ret;

  /* ERROR */
error:
  GST_ELEMENT_ERROR (element, CORE, STATE_CHANGE, (NULL), (NULL));
  return GST_STATE_CHANGE_FAILURE;
}
