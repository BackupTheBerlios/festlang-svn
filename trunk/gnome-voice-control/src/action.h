#ifndef _ACTION_H_
#define _ACTION_H_

typedef enum {
    ACTION_MINIMIZE_WINDOW,
    ACTION_MAXIMIZE_WINDOW,
    ACTION_CLOSE_WINDOW
} VoiceAction;

void 
do_action (VoiceAction action);

#endif /* _ACTION_H_ */

