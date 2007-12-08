/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * spi-listener.h:
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

#ifndef __CONTROL_SPI_LISTENER_H__
#define __CONTROL_SPI_LISTENER_H__

#include <cspi/spi.h>

G_BEGIN_DECLS

#define CONTROL_SPI_LISTENER_TYPE (control_spi_listener_get_type())
#define CONTROL_SPI_LISTENER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), CONTROL_SPI_LISTENER_TYPE, ControlSpiListener))

typedef struct _ControlSpiListener ControlSpiListener;
typedef struct _ControlSpiListenerClass ControlSpiListenerClass;

typedef struct	_AccessibleItem {
	Accessible *accessible;
	gchar *name;
} AccessibleItem;

struct _ControlSpiListener {
    GObject parent;

    GQueue *event_queue;
    GSList *actions;
    Accessible *root;
    
    void *window_listener;
    void *showing_listener;
    void *keys_listener;

    gboolean is_keypressed;
    
    guint idle_id;
};

struct _ControlSpiListenerClass {
  GObjectClass parent_class;

  void (*changed) (ControlSpiListener *listener);
};

GType control_spi_listener_get_type (void);
void control_spi_listener_start (ControlSpiListener *listener);
void control_spi_listener_stop (ControlSpiListener *listener);
GSList* control_spi_listener_get_object_list (ControlSpiListener *listener);

G_END_DECLS

#endif /* __CONTROL_SPI_LISTENER_H__ */

