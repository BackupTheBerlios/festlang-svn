/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2010  Consorcio Fernando de los Rios - Junta de Andalucia
 * Developed by Intelligent Dialogue Systems S.L. <info@indisys.es>
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
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>

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
    ACTION_PAGE_UP,
    ACTION_PAGE_DOWN,
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
    ACTION_ENTER,
    ACTION_COPY,
    ACTION_CUT,
    ACTION_PASTE,

    ACTION_MAIN_MENU,
    
    ACTION_MOUSE_LEFT,
    ACTION_MOUSE_RIGHT,
    ACTION_MOUSE_DOWN,
    ACTION_MOUSE_UP,
    
    ACTION_SEARCH,
    ACTION_SHOW_DESKTOP,
    ACTION_LOCK_DESKTOP,
    ACTION_SHOW_HELP,
    ACTION_HOME_FOLDER,
    ACTION_SUSPEND,
    ACTION_RUN_KEYBOARD,
    ACTION_RUN_EVIACAM,
    ACTION_REBOOT,
    ACTION_SHUTDOWN_SYSTEM,
    ACTION_LOGOUT_USER,
    ACTION_HIBERNATE,

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

static VoiceActionCommand commands[] = 
{
 {N_("RUN MAIL"), ACTION_RUN_MAIL},
 {N_("RUN BROWSER"), ACTION_RUN_BROWSER},
 {N_("RUN TEXT EDITOR"), ACTION_RUN_TEXT_EDITOR},
 {N_("RUN TERMINAL"), ACTION_RUN_TERMINAL},
 {N_("CLOSE WINDOW"), ACTION_CLOSE_WINDOW},
 {N_("NEXT WINDOW"), ACTION_NEXT_WINDOW},
 {N_("PREVIOUS WINDOW"), ACTION_PREVIOUS_WINDOW},
 {N_("MINIMIZE WINDOW"), ACTION_MINIMIZE_WINDOW},
 {N_("MAXIMIZE WINDOW"), ACTION_MAXIMIZE_WINDOW},
 {N_("SWITCH WINDOW"), ACTION_NEXT_WINDOW},
 {N_("RIGHT CLICK"), ACTION_RIGHT_CLICK},
 {N_("CLICK"), ACTION_CLICK},

 {N_("PAGE UP"), ACTION_PAGE_UP},
 {N_("PAGE DOWN"), ACTION_PAGE_DOWN}, 
 {N_("UP"), ACTION_UP},
 {N_("DOWN"), ACTION_DOWN},
 {N_("LEFT"), ACTION_LEFT},
 {N_("RIGHT"), ACTION_RIGHT},
 {N_("ESCAPE"), ACTION_ESCAPE},
 {N_("BACKSPACE"), ACTION_BACKSPACE},
 {N_("DELETE"), ACTION_DELETE},
 {N_("INSERT"), ACTION_INSERT},
 {N_("HOME"), ACTION_HOME},
 {N_("END"), ACTION_END},
 {N_("ENTER"), ACTION_ENTER},
 {N_("COPY"), ACTION_COPY},
 {N_("CUT"), ACTION_CUT},
 {N_("PASTE"), ACTION_PASTE},

 {N_("MOUSE LEFT"), ACTION_MOUSE_LEFT},
 {N_("MOUSE RIGHT"), ACTION_MOUSE_RIGHT},
 {N_("MOUSE DOWN"), ACTION_MOUSE_DOWN},
 {N_("MOUSE UP"), ACTION_MOUSE_UP},
 {N_("MAIN MENU"), ACTION_MAIN_MENU},
 {N_("SHOW HELP"), ACTION_SHOW_HELP},
 {N_("SUSPEND"), ACTION_SUSPEND}, // TODO
 {N_("RUN KEYBOARD"), ACTION_RUN_KEYBOARD},
 {N_("RUN EVIACAM"), ACTION_RUN_EVIACAM}, // TODO
 {N_("HOME FOLDER"), ACTION_HOME_FOLDER},
 {N_("LOCK DESKTOP"), ACTION_LOCK_DESKTOP},
 {N_("REBOOT"), ACTION_REBOOT},
 {N_("SHUTDOWN SYSTEM"), ACTION_SHUTDOWN_SYSTEM},
 {N_("LOGOUT USER"), ACTION_LOGOUT_USER},
 {N_("BUSCAR"), ACTION_SEARCH},
 {N_("SHOW DESKTOP"), ACTION_SHOW_DESKTOP},
 {N_("HIBERNATE"), ACTION_HIBERNATE},
 {NULL, 0},
};

gboolean 
do_action (VoiceAction action)
{
	WnckWindow *window;
	WnckScreen *screen;
	GList *windows;
	KeyCode keycode, keycode2, keycode3;
	int x, y;
	GConfClient *client;
	gchar *default_app;
        gboolean error;
        const gchar *homedir;
	

	switch(action) {

	    case ACTION_RUN_BROWSER:
		    client = gconf_client_get_default ();

                    default_app = gconf_client_get_string (client, "/desktop/gnome/applications/browser/exec", NULL);

                    if (default_app != NULL)
			    g_spawn_command_line_async(default_app, NULL);
	    	    else
			    g_spawn_command_line_async ("firefox", NULL);
		    break;

	    case ACTION_RUN_TERMINAL:
		    client = gconf_client_get_default ();

	    	    default_app = gconf_client_get_string (client, "/desktop/gnome/applications/terminal/exec", NULL);

	    	    if (default_app != NULL)
			    g_spawn_command_line_async (default_app, NULL);
	    	    else
			    g_spawn_command_line_async ("gnome-terminal", NULL);

	    	    break;

	    case ACTION_RUN_TEXT_EDITOR:
    		    g_spawn_command_line_async ("gedit", NULL);
		    break;

	    case ACTION_RUN_MAIL:
		    g_spawn_command_line_async ("evolution", NULL);
    		    break;

	    case ACTION_CLOSE_WINDOW:
                    error = FALSE;
                    screen = wnck_screen_get_default ();
		    if (screen) {
		    window = wnck_screen_get_active_window (screen);
			    if (window)
                            {
                                wnck_window_close (window, GDK_CURRENT_TIME);
                            }else error = TRUE;
		    }else error = TRUE;

                    if(error == TRUE)
                    {
                        keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                        keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_F4);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                        SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    }

		    break;
    
	    case ACTION_MINIMIZE_WINDOW:
		    /*screen = wnck_screen_get_default ();
		    if (screen) {
    	    		    window = wnck_screen_get_active_window (screen);
		    	    if (window)
                            {
				    wnck_window_minimize (window);
                            } else error = TRUE;
		    } else error = TRUE;*/

                    //*/if(error == TRUE)
                    //{
                        keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                        keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_F9);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                        SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    //}

		    break;

	    case ACTION_MAXIMIZE_WINDOW:
		    /*screen = wnck_screen_get_default ();
		    if (screen) {
    	    	    window = wnck_screen_get_active_window (screen);
		    	    if (window)
                            {
				    wnck_window_maximize (window);
                            } else error = TRUE;
		    } else error = TRUE;*/

                    //if(error == TRUE)
                    //{
                        keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                        keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_F10);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                        SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    //}

		    break;

	    case ACTION_NEXT_WINDOW:
/*
		    screen = wnck_screen_get_default ();
		    if (screen) {

			    windows = wnck_screen_get_windows_stacked (screen);
			    if (windows && windows->next)
                            {
				    wnck_window_activate (WNCK_WINDOW (windows->next->data), GDK_CURRENT_TIME);
                            }else error = TRUE;
		    }
                    else error = TRUE;
*/

                    //if(error == TRUE)
                    //{
                        keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                        keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Tab);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                        SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESS);
                        SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_RELEASE);
                        SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    //}
                    break;

	    case ACTION_PREVIOUS_WINDOW:
/*
		    screen = wnck_screen_get_default ();
		    if (screen) {
			    window =  wnck_screen_get_previously_active_window (screen);
			    if (window)
                            {
					wnck_window_activate (window, GDK_CURRENT_TIME);
                            }else g_message("Couldn't execute action %s", _("PREVIOUS WINDOW"));
		    }else g_message("Couldn't execute action %s", _("PREVIOUS WINDOW"));
*/
                    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Shift_L);
                    keycode3 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Tab);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESS);
                    SPI_generateKeyboardEvent (keycode3, NULL, SPI_KEY_PRESSRELEASE);
                    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_RELEASE);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);


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
		    if (!SPI_generateMouseEvent (x, y, "b3c")) {
	    		    g_warning ("Couldn't execute ACTION_RIGHT_CLICK");
		    }
#if DEBUG
		    g_message("Executed ACTION_RIGHT_CLICK at (%d, %d)", x, y);
#endif	    
		    break;

    		    
	    case ACTION_PAGE_UP:
		    SPI_generateKeyboardEvent (GDK_Page_Up, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_PAGE_DOWN:
		    SPI_generateKeyboardEvent (GDK_Page_Down, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_UP:
		    SPI_generateKeyboardEvent (GDK_Up, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_DOWN:
		    SPI_generateKeyboardEvent (GDK_Down, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_LEFT:
		    SPI_generateKeyboardEvent (GDK_Left, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_RIGHT:
		    SPI_generateKeyboardEvent (GDK_Right, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_ESCAPE:
		    SPI_generateKeyboardEvent (GDK_Escape, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_BACKSPACE:
		    SPI_generateKeyboardEvent (GDK_BackSpace, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_DELETE:
		    SPI_generateKeyboardEvent (GDK_Delete, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_INSERT:
		    SPI_generateKeyboardEvent (GDK_Insert, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_HOME:
		    SPI_generateKeyboardEvent (GDK_Home, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_END:
		    SPI_generateKeyboardEvent (GDK_End, NULL, SPI_KEY_SYM);
		    break;

	    case ACTION_ENTER:
		    SPI_generateKeyboardEvent (GDK_KP_Enter, NULL, SPI_KEY_SYM);
		    break;
    
	    case ACTION_COPY:
		    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Control_L);
		    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_C);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
		    break;
    		    
	    case ACTION_CUT:
		    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Control_L);
		    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_X);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
		    break;

	    case ACTION_PASTE:
		    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Control_L);
		    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_V);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
		    break;

	    case ACTION_MAIN_MENU:
/*
 * FIXME: hardcoded <Alt>F1; read curreny keybinding with gconf 
 * from /apps/metacity/global_keybindings/panel_main_menu
 */
		    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_F1);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
		    break;

	    case ACTION_MOUSE_LEFT:
                    SPI_generateMouseEvent(-10,0,"rel");
		    break;	

	    case ACTION_MOUSE_RIGHT:
		    SPI_generateMouseEvent(10,0,"rel");
		    break;

    	    case ACTION_MOUSE_DOWN:
		    SPI_generateMouseEvent(0,10,"rel");
		    break;

	    case ACTION_MOUSE_UP:
		    SPI_generateMouseEvent(0,-10,"rel");
		    break;	

            case ACTION_SHOW_HELP:
                    SPI_generateKeyboardEvent (GDK_F1, NULL, SPI_KEY_SYM);
                    break;

            case ACTION_HOME_FOLDER:
                    homedir = g_getenv ("HOME");
                    if (!homedir)
                        homedir = g_get_home_dir ();
                    g_spawn_command_line_async (g_strconcat("nautilus ", homedir, NULL), NULL);
                    break;

            case ACTION_LOCK_DESKTOP:
                    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Control_L);
                    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                    keycode3 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_L);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode3, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_RELEASE);
                    break;

            case ACTION_SEARCH:
                    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Control_L);
                    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_F);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    break;

            case ACTION_RUN_KEYBOARD:
                    g_spawn_command_line_async ("onboard", NULL);
                    break;

            case ACTION_RUN_EVIACAM:
                    g_spawn_command_line_async ("eviacam", NULL);
                    break;

            case ACTION_SHUTDOWN_SYSTEM:
                    SPI_generateKeyboardEvent (GDK_PowerOff, NULL, SPI_KEY_SYM);
	    	    break;

            case ACTION_REBOOT:
                    SPI_generateKeyboardEvent (GDK_PowerOff, NULL, SPI_KEY_SYM);
                    break;

            case ACTION_LOGOUT_USER:
                    g_spawn_command_line_async ("gnome-session-save --kill", NULL);
                    break;

            case ACTION_SUSPEND:
                    // Use GDK_Sleep tu suspend system without asking the user
                    SPI_generateKeyboardEvent (GDK_PowerOff, NULL, SPI_KEY_SYM);
                    break;

            case ACTION_SHOW_DESKTOP:
                    keycode = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Control_L);
                    keycode2 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_Alt_L);
                    keycode3 = XKeysymToKeycode(GDK_DISPLAY(), (KeySym) GDK_D);
                    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_PRESS);
                    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_PRESS);
		    SPI_generateKeyboardEvent (keycode3, NULL, SPI_KEY_PRESSRELEASE);
		    SPI_generateKeyboardEvent (keycode, NULL, SPI_KEY_RELEASE);
                    SPI_generateKeyboardEvent (keycode2, NULL, SPI_KEY_RELEASE);
                    break;

            case ACTION_HIBERNATE:
                    SPI_generateKeyboardEvent (GDK_PowerOff, NULL, SPI_KEY_SYM);
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
		list = g_slist_append (list, _(commands[i].command));
	}
	return list;
}

gboolean
voice_control_action_process_result (const char *message)
{
	int i;
	int result = FALSE;

#ifdef ENABLE_DEBUG_DUMP
        g_message("Processing: %s", message);
#endif

	for (i = 0; commands[i].command != NULL; i++) {
#if ENABLE_DEBUG_DUMP
                g_message("Comparing %s with %s", message, _(commands[i].command));
#endif
		if (strcmp (message, _(commands[i].command)) == 0) {
                    
#if ENABLE_DEBUG_DUMP
			g_message ("Found command '%s' in message '%s'\n", commands[i].command, message);
#endif
			g_idle_add ((GSourceFunc)do_action, GINT_TO_POINTER (commands[i].action));
			result = TRUE;
			break;
		}
	}
	return result;
}

