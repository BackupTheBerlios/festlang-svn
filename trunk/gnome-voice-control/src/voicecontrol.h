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

#ifndef VOICECONTROL_H
#define	VOICECONTROL_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <panel-applet.h>
#include <glib/gi18n.h>

#include "spi-listener.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define VOICE_CONTROL_APPLET(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), \
				 voice_control_applet_get_type(),          \
				 VoiceControlApplet))
#define VOICE_CONTROL_IS_APPLET(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), \
				   VOICE_CONTROL_TYPE_APPLET))

#define VOICE_CONTROL_ICON "gnome-grecord"

#define N_VOICE_CONTROL_LISTENERS 4

enum voice_control_state {
    INIT,
    ERROR,
    STOP,
    READY,
    PAUSE,
    LOAD,
    LISTEN,
    TEXT
};

typedef struct {
	PanelApplet                 applet;

	guint                       listeners [N_VOICE_CONTROL_LISTENERS];

	GtkWidget*                  about_dialog;
	GtkWidget*                  frame;
	GtkWidget*                  state_label;

	GtkTooltips*                tooltips;
        gchar*                      tooltip;
	guint                       ready_timeout;

	gpointer                    asr;

	ControlSpiListener*         spi_listener;

        gchar*                      lang;
        gint                        state;
} VoiceControlApplet;

typedef struct {
	PanelAppletClass klass;
} VoiceControlAppletClass;

static void  voice_control_applet_init (VoiceControlApplet      *voice_control);
static void  voice_control_applet_class_init (VoiceControlAppletClass *klass);
void  voice_control_set_state (VoiceControlApplet *voice_control, gint state, gchar* text);
gint  voice_control_get_state (VoiceControlApplet *voice_control);
void  voice_control_set_text (VoiceControlApplet *voice_control, gchar *message, gchar *tooltip);
gboolean voice_control_process_action (VoiceControlApplet *voice_control, const gchar *message);


#ifdef	__cplusplus
}
#endif

#endif	/* VOICECONTROL_H */

