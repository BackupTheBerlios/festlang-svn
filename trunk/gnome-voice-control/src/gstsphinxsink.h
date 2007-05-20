/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gstsphinxsink.h:
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


#ifndef __GST_SPHINX_SINK_H__
#define __GST_SPHINX_SINK_H__

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS


#define GST_TYPE_SPHINX_SINK \
  (gst_sphinx_sink_get_type())
#define GST_SPHINX_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_SPHINX_SINK,GstSphinxSink))
#define GST_SPHINX_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_SPHINX_SINK,GstSphinxSinkClass))
#define GST_IS_SPHINX_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_SPHINX_SINK))
#define GST_IS_SPHINX_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_SPHINX_SINK))
#define GST_SPHINX_SINK_CAST(obj) ((GstSphinxSink *)obj)

/**
 * GstSphinxSinkStateError:
 * @SPHINX_SINK_STATE_ERROR_NONE: no error
 * @SPHINX_SINK_STATE_ERROR_NULL_READY: cause the NULL to READY state change to fail
 * @SPHINX_SINK_STATE_ERROR_READY_PAUSED: cause the READY to PAUSED state change to fail:
 * @SPHINX_SINK_STATE_ERROR_PAUSED_PLAYING: cause the PAUSED to PLAYING state change to fail:
 * @SPHINX_SINK_STATE_ERROR_PLAYING_PAUSED: cause the PLAYING to PAUSED state change to fail:
 * @SPHINX_SINK_STATE_ERROR_PAUSED_READY: cause the PAUSED to READY state change to fail:
 * @SPHINX_SINK_STATE_ERROR_READY_NULL: cause the READY to NULL state change to fail:
 *
 * Possible state change errors for the state-error property.
 */
typedef enum {
  SPHINX_SINK_STATE_ERROR_NONE = 0,
  SPHINX_SINK_STATE_ERROR_NULL_READY,
  SPHINX_SINK_STATE_ERROR_READY_PAUSED,
  SPHINX_SINK_STATE_ERROR_PAUSED_PLAYING,
  SPHINX_SINK_STATE_ERROR_PLAYING_PAUSED,
  SPHINX_SINK_STATE_ERROR_PAUSED_READY,
  SPHINX_SINK_STATE_ERROR_READY_NULL
} GstSphinxSinkStateError;

typedef struct _GstSphinxSink GstSphinxSink;
typedef struct _GstSphinxSinkClass GstSphinxSinkClass;

/**
 * GstSphinxSink:
 *
 * The opaque #GstSphinxSink data structure.
 */
struct _GstSphinxSink {
  GstBaseSink		element;

  gboolean		silent;
  gboolean		dump;
  gboolean		signal_handoffs;
  GstSphinxSinkStateError state_error;
  gchar			*last_message;
};

struct _GstSphinxSinkClass {
  GstBaseSinkClass parent_class;

  /* signals */
  void (*handoff) (GstElement *element, GstBuffer *buf, GstPad *pad);
  void (*preroll_handoff) (GstElement *element, GstBuffer *buf, GstPad *pad);
};

GType gst_sphinx_sink_get_type (void);

G_END_DECLS

#endif /* __GST_SPHINX_SINK_H__ */
