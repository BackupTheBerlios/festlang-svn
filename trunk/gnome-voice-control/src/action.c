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
