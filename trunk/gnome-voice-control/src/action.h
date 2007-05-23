#ifndef _ACTION_H_
#define _ACTION_H_

typedef enum {
    ACTION_MINIMIZE_ACTIVE_WINDOW,
    ACTION_MAXIMIZE_ACTIVE_WINDOW,
    ACTION_UNMAXIMIZE_ACTIVE_WINDOW,
    ACTION_FULLSCREEN_ACTIVE_WINDOW,
    ACTION_UNFULLSCREEN_ACTIVE_WINDOW,
    ACTION_CLOSE_ACTIVE_WINDOW
} VoiceAction;

void 
do_action (VoiceAction action);

#endif /* _ACTION_H_ */

