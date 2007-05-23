#include <action.h>
#include <gdk/gdk.h>

void 
do_action (VoiceAction action)
{
    GdkDisplayManager *display_manager;
    GdkDisplay        *display;
    GdkScreen         *screen;
    GdkWindow         *window;

    display_manager = gdk_display_manager_get();
    display =  gdk_display_manager_get_default_display(display_manager);
    screen = gdk_display_get_default_screen(display);
    window = gdk_screen_get_active_window(screen); 

    switch(action){
        case ACTION_MINIMIZE_ACTIVE_WINDOW:

        break;
        case ACTION_MAXIMIZE_ACTIVE_WINDOW:
            gdk_window_maximize(window);
        break;
        case ACTION_UNMAXIMIZE_ACTIVE_WINDOW:
            gdk_window_unmaximize(window);
        break;
        case ACTION_FULLSCREEN_ACTIVE_WINDOW:
            gdk_window_fullscreen(window);
        break;
        case ACTION_UNFULLSCREEN_ACTIVE_WINDOW:
            gdk_window_unfullscreen(window);
        break;
        case ACTION_CLOSE_ACTIVE_WINDOW:

        break;  
        default:

        break;  
    }

	return;
}

