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
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <cspi/spi.h>
#include <libspi/keymasks.h>

typedef enum {
    ACTION_RUN_BROWSER,
    ACTION_RUN_TERMINAL,
    ACTION_RUN_TEXT_EDITOR,
    ACTION_RUN_MAIL,
    ACTION_CLOSE_WINDOW,
    ACTION_NEXT_WINDOW,
    ACTION_PREVIOUS_WINDOW,
    ACTION_MINIMIZE_WINDOW,
    ACTION_MAXIMIZE_WINDOW,
    ACTION_CLICK,
    ACTION_RIGHT_CLICK,
    ACTION_MAIN_MENU,
    /* FIXME: the rest is not implemented */
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_ESCAPE,
    ACTION_BACKSPACE,
    ACTION_DELETE,
    ACTION_INSERT,
    ACTION_HOME,
    ACTION_END,
    ACTION_PAGE_DOWN,
    ACTION_PAGE_UP,
    ACTION_ENTER,
    ACTION_SWITCH_LAYOUT,
    ACTION_MOUSE_LEFT,
    ACTION_MOUSE_RIGHT,
    ACTION_MOUSE_DOWN,
    ACTION_MOUSE_UP,
    ACTION_SHOW_HELP,
    ACTION_HOME_FOLDER,
    ACTION_SEARCH,
    ACTION_SHOW_DESKTOP,
    ACTION_COPY,
    ACTION_PASTE,
    ACTION_CUT,
    ACTION_LOCK_DESKTOP,
} VoiceAction;

typedef struct _VoiceActionCommand {
    char *command;
    VoiceAction action;
} VoiceActionCommand;

/* We collect a list of words and related actions here. Of course it's just
 * an early beginning and some actions are missing for sure. Submit us a 
 * request if you want a new one. We really don't want to have a customizable
 * list because we target AI database which will simply understand you.
 */

/* RIGHT CLICK needs to be before CLICK, as we search for this inside message */

static VoiceActionCommand commands[] = 
{
 {"RUN MAIL", ACTION_RUN_MAIL},
 {"RUN BROWSER", ACTION_RUN_BROWSER},
 {"RUN TEXT EDITOR", ACTION_RUN_TEXT_EDITOR},
 {"RUN TERMINAL", ACTION_RUN_TERMINAL},
 {"CLOSE WINDOW", ACTION_CLOSE_WINDOW},
 {"NEXT WINDOW", ACTION_NEXT_WINDOW},
 {"PREVIOUS WINDOW", ACTION_PREVIOUS_WINDOW},
 {"MINIMIZE WINDOW", ACTION_MINIMIZE_WINDOW},
 {"MAXIMIZE WINDOW", ACTION_MAXIMIZE_WINDOW},
 {"SWITCH WINDOW", ACTION_NEXT_WINDOW},
 {"RIGHT CLICK", ACTION_RIGHT_CLICK},
 {"CLICK", ACTION_CLICK},
 {"MAIN MENU", ACTION_MAIN_MENU},
 {NULL, 0},
};

gboolean 
do_action (VoiceAction action)
{
	WnckWindow *window;
	WnckScreen *screen;
	GList *windows;
	KeyCode keycode;
	int x, y;
	
	switch(action) {

	    case ACTION_RUN_BROWSER:
		    g_spawn_command_line_async ("epiphany", NULL);
		    break;

	    case ACTION_RUN_TERMINAL:
		    g_spawn_command_line_async ("gnome-terminal", NULL);
		    break;

	    case ACTION_RUN_TEXT_EDITOR:
    		    g_spawn_command_line_async ("gedit", NULL);
		    break;

	    case ACTION_RUN_MAIL:
		    g_spawn_command_line_async ("evolution", NULL);
	    	    break;
	    
	    case ACTION_CLOSE_WINDOW:
	    	    screen = wnck_screen_get_default ();
		    if (screen) {
			    window = wnck_screen_get_active_window (screen);
			    if (window)
				    wnck_window_close (window, GDK_CURRENT_TIME);
		    }
		    break;
    
	    case ACTION_MINIMIZE_WINDOW:
		    screen = wnck_screen_get_default ();
		    if (screen) {
    	    		    window = wnck_screen_get_active_window (screen);
		    	    if (window)
				    wnck_window_minimize (window);
		    }
		    break;

	    case ACTION_MAXIMIZE_WINDOW:
		    screen = wnck_screen_get_default ();
		    if (screen) {
    	    	    window = wnck_screen_get_active_window (screen);
		    	    if (window)
				    wnck_window_maximize (window);
		    }
		    break;

	    case ACTION_NEXT_WINDOW:
		    screen = wnck_screen_get_default ();
		    if (screen) {

			    windows = wnck_screen_get_windows_stacked (screen);
			    if (windows && windows->next)
				    wnck_window_activate (WNCK_WINDOW (windows->next->data), GDK_CURRENT_TIME);
		    }
	    case ACTION_PREVIOUS_WINDOW:
		    screen = wnck_screen_get_default ();
		    if (screen) {
			    window =  wnck_screen_get_previously_active_window (screen);
			    if (window)
					wnck_window_activate (window, GDK_CURRENT_TIME);
		    }
		    break;

	    /*
	     * should work exactly as one would have pressed mouse button 
	     * (on a window which doesn't have focus, on a gimp canvas etc.)
	     */
	    case ACTION_CLICK:
    		    gdk_display_get_pointer(gdk_display_get_default (), NULL, &x, &y, NULL);
		    if (!SPI_generateMouseEvent (x, y, "b1c")) {
	    		    g_warning ("Couldn't execute ACTION_CLICK");
		    }
#if DEBUG
		    g_message("Executed ACTION_CLICK at (%d, %d)", x, y);
#endif	    
		    break;
	    case ACTION_RIGHT_CLICK:
    		    gdk_display_get_pointer(gdk_display_get_default (), NULL, &x, &y, NULL);
		    if (!SPI_generateMouseEvent (x, y, "b2c")) {
	    		    g_warning ("Couldn't execute ACTION_RIGHT_CLICK");
		    }
#if DEBUG
		    g_message("Executed ACTION_RIGHT_CLICK at (%d, %d)", x, y);
#endif	    
		    break;
		    
	    case ACTION_MAIN_MENU:
/*
 * FIXME: hardcoded <Alt>F1; read curreny keybinding with gconf 
 * from /apps/metacity/global_keybindings/panel_main_menu
 */
		    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (GDK_F1, NULL, SPI_KEY_SYM);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
		    break;

	    default:
		    g_warning ("Not implemented yet");
		    break;
	}

	return FALSE;
}

GSList*
voice_control_action_append_commands (GSList *list)
{
	int i;
	
	for (i = 0; commands[i].command != NULL; i++) {
		list = g_slist_append (list, commands[i].command);
	}
	return list;
}

gboolean
voice_control_action_process_result (char *message)
{
	int i;
	int result = FALSE;
	
	for (i = 0; commands[i].command != NULL; i++) {
		if (g_strrstr (message, commands[i].command)) {
#if DEBUG
			g_message ("Found command '%s' in message '%s'\n", commands[i].command, message);
#endif
			g_idle_add ((GSourceFunc)do_action, GINT_TO_POINTER (commands[i].action));
			result = TRUE;
			break;
		}
	}
	return result;
}

