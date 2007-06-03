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
} VoiceAction;

void 
do_action (VoiceAction action);

#endif /* _ACTION_H_ */

