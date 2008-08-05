/* This file is a part of GStreamer
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
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
#include <gst/base/gstadapter.h>

#include <pocketsphinx.h>
#include <ad.h>
#include <cont_ad.h>
#include <fsg_model.h>

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


typedef struct _GstSphinxSink GstSphinxSink;
typedef struct _GstSphinxSinkClass GstSphinxSinkClass;


typedef struct _GstSphinxSinkAd {
    GstSphinxSink *self;
    int32 dummy;
    int32 sps;
    int32 bps;
    int32 calibrated;
    int32 initialized;
    int32 calibrate_started;
    int32 listening;
} GstSphinxSinkAd;

/**
 * GstSphinxSink:
 *
 * The opaque #GstSphinxSink data structure.
 */
struct _GstSphinxSink {
  GstBaseSink		element;

  cont_ad_t *cont;
  GstSphinxSinkAd ad;
  gint32 last_ts;
  
  ps_decoder_t *decoder;
  fsg_model_t *fsg;
  
  GstAdapter *adapter;  
};

struct _GstSphinxSinkClass {
  GstBaseSinkClass parent_class;

  void (*initialization)  (GstElement *element);
  void (*after_initialization)  (GstElement *element);
  void (*calibration)     (GstElement *element);
  void (*ready)           (GstElement *element);
  void (*listening)       (GstElement *element);
  void (*message)         (GstElement *element, gchar *message);
};

GType gst_sphinx_sink_get_type (void);

void gst_sphinx_sink_set_fsg (GstSphinxSink *sink, GSList *words);

G_END_DECLS

#endif /* __GST_SPHINX_SINK_H__ */

