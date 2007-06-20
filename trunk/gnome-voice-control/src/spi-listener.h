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

