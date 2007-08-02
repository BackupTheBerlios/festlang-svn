/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * action.h:
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

#ifndef _ACTION_H_
#define _ACTION_H_

typedef enum {
    ACTION_RUN_BROWSER,
    ACTION_RUN_TERMINAL,
    ACTION_RUN_MAIL,
    ACTION_CLOSE_WINDOW,
    ACTION_NEXT_WINDOW,
    ACTION_MINIMIZE_WINDOW,
    ACTION_MAXIMIZE_WINDOW,
    ACTION_RUN_TEXT_EDITOR
} VoiceAction;

void 
do_action (VoiceAction action);

#endif /* _ACTION_H_ */

