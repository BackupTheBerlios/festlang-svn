/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2007  Nickolay V. Shmyrev  <nshmyrev@yandex.ru>
 *
 * action.c:
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define WNCK_I_KNOW_THIS_IS_UNSTABLE

#include "action.h"
#include <libwnck/screen.h>
#include <libwnck/window.h>
#include <gdk/gdk.h>

void 
do_action (VoiceAction action)
{
	WnckWindow *window;
	WnckScreen *screen;
	GList *windows;
	
        switch(action){
    	    case ACTION_RUN_BROWSER:
    		g_spawn_command_line_async ("epiphany", NULL);
    		break;
	    case ACTION_RUN_TERMINAL:
    		g_spawn_command_line_async ("gnome-terminal", NULL);
		break;
	    case ACTION_RUN_MAIL:
    		g_spawn_command_line_async ("evolution", NULL);
		break;
            case ACTION_RUN_TEXT_EDITOR:
    		g_spawn_command_line_async ("gedit", NULL);
                break;
	    case ACTION_CLOSE_WINDOW:
		screen = wnck_screen_get_default ();
		if (screen)
		  {
    	        	window = wnck_screen_get_active_window (screen);
		        if (window)
				wnck_window_close (window, GDK_CURRENT_TIME);
		  }
		break;
	    case ACTION_MINIMIZE_WINDOW:
		screen = wnck_screen_get_default ();
		if (screen)
		  {
    	        	window = wnck_screen_get_active_window (screen);
		        if (window)
				wnck_window_minimize (window);
		  }
		break;
	    case ACTION_MAXIMIZE_WINDOW:
		screen = wnck_screen_get_default ();
		if (screen)
		  {
    	        	window = wnck_screen_get_active_window (screen);
		        if (window)
				wnck_window_maximize (window);
		  }
		break;
	    case ACTION_NEXT_WINDOW:
		screen = wnck_screen_get_default ();
		if (screen)
		  {
			windows = wnck_screen_get_windows_stacked (screen);
			if (windows && windows->next)
			    wnck_window_activate (WNCK_WINDOW (windows->next->data), GDK_CURRENT_TIME);
		  }
		break;
	    default:
		break;
	}

	return;
}
