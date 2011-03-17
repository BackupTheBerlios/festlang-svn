/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2010  Consorcio Fernando de los Rios - Junta de Andalucia
 * Developed by Intelligent Dialogue Systems S.L. <info@indisys.es>
 *
 * asr.h:
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

#ifndef ASR_H
#define	ASR_H

#include "voicecontrol.h"

#ifdef	__cplusplus
extern "C" {
#endif

void
asr_setup(int* argc, char **argv [], gpointer data);

void
asr_init(VoiceControlApplet* applet);

void
asr_destroy(VoiceControlApplet* applet);

void
asr_start(VoiceControlApplet* applet);

void
asr_pause(VoiceControlApplet* applet);

void
asr_resume(VoiceControlApplet* applet);

void
asr_stop(VoiceControlApplet* applet);

gboolean
asr_running(VoiceControlApplet* applet);

void
asr_set_grammar(VoiceControlApplet* applet, GSList* commands);

void
asr_set_language(VoiceControlApplet* applet, gchar* lang);

#ifdef	__cplusplus
}
#endif

#endif	/* ASR_H */

