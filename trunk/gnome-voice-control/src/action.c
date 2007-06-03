#include <action.h>
#include <gdk/gdk.h>

void 
do_action (VoiceAction action)
{

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
	    default:
		break;
	}

	return;
}
