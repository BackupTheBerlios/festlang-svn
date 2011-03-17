/* This file is a part of gnome-voice-control
 *
 * Copyright (C) 2010  Consorcio Fernando de los Rios - Junta de Andalucia
 * Developed by Intelligent Dialogue Systems S.L. <info@indisys.es>
 *
 * asrjulius.c:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include <config.h>

#include <locale.h>
#include <string.h>

#include "voicecontrol.h"

// Undef TRUE and FALSE defined by Glib because julius.h redifine them
#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#include "asr.h"
#include <julius/julius.h>
#include <julius/jfunc.h>

typedef struct {
    Recog*      decoder;
    gboolean    initialized;
    gboolean    pause;
    gchar*      name;
} ASRJulius;

GThread         *juliusThread;
gboolean        paused;

////////////////////
// Events
////////////////////

static void
on_init(VoiceControlApplet *voice_control)
{
    voice_control_set_state(voice_control, INIT, NULL);
}

static void
on_listening(Recog* recog, gpointer data)
{
    VoiceControlApplet * voice_control = VOICE_CONTROL_APPLET (data);

    if (((ASRJulius *) voice_control->asr)->pause == FALSE)
    {
        voice_control_set_state(voice_control, LISTEN, NULL);

#ifdef ENABLE_DEBUG_DUMP
        g_message("Recognizing...");
#endif
    }
}

static void
on_message(VoiceControlApplet *voice_control, const gchar *message)
{
    voice_control_set_state(voice_control, TEXT, (gchar*) message);

#ifdef ENABLE_DEBUG_DUMP
    g_message("Sending action: %s", message);
#endif
    // Look for a predefined voice control command
    if (voice_control_action_process_result (message))
            return;

    // Look for other possible event
    voice_control_process_action (voice_control, message);
}

static void
on_stop (gpointer data)
{
        VoiceControlApplet * voice_control = VOICE_CONTROL_APPLET (data);

	voice_control_set_state(voice_control, STOP, NULL);
	control_spi_listener_stop (voice_control->spi_listener);
}

static void
on_ready(Recog* recog, gpointer data)
{
    VoiceControlApplet * applet = VOICE_CONTROL_APPLET (data);

    if(((ASRJulius *) applet->asr)->initialized == TRUE)
    {
#ifdef ENABLE_DEBUG_DUMP
        g_message ("Sending ready");
#endif
        VoiceControlApplet * voice_control = VOICE_CONTROL_APPLET (data);

        voice_control_set_state(voice_control, READY, NULL);

#ifdef ENABLE_DEBUG_DUMP
        g_message("<<please speak>>");
#endif
        
    }
}

static void
on_error (VoiceControlApplet * voice_control, gchar *message)
{
    voice_control_set_state(voice_control, ERROR, message);
}

static void
on_pause (Recog* recog, gpointer data)
{
    VoiceControlApplet * voice_control = VOICE_CONTROL_APPLET (data);

    voice_control_set_state(voice_control, PAUSE, NULL);

    while (!recog->process_active)
    {
        g_usleep(100);
    }
}



/////////////////////////////////
// Auxiliary functions
/////////////////////////////////

static Jconf*
j_setup_jconf (gchar* lang)
{
    int i;

    gchar * path = NULL;
    Jconf* jconf = NULL;

    path = g_strdup_printf(GNOMEDATADIR "/gnome-voice-control/julius/desktop-control.jconf");

    jconf = j_config_load_file_new(path);

    g_free(path);

    return jconf;
}

static void
j_process_result(Recog * recog, void * data)
{
    int i;
    WORD_INFO *winfo = NULL;
    WORD_ID *seq = NULL;
    int seqnum;
    int n;
    gboolean result = FALSE;
    Sentence *s = NULL;
    RecogProcess *r = NULL;
    gchar* sentence = NULL;
    gfloat confidence = 0.0;
    gint count = 0;

    VoiceControlApplet * applet = VOICE_CONTROL_APPLET (data);

    /* all recognition results are stored at each recognition process
    instance */
    for(r=recog->process_list;r;r=r->next)
    {

        /* skip the process if the process is not alive */
        if (! r->live) continue;

        /* check result status */
        if (r->result.status < 0) /* no results obtained */
        {
              /* outout message according to the status code */
              switch(r->result.status)
              {
                  case J_RESULT_STATUS_REJECT_POWER:
                    g_warning("<input rejected by power>\n");
                    break;
                  case J_RESULT_STATUS_TERMINATE:
                    g_warning("<input teminated by request>\n");
                    break;
                  case J_RESULT_STATUS_ONLY_SILENCE:
                    g_warning("<input rejected by decoder (silence input result)>\n");
                    break;
                  case J_RESULT_STATUS_REJECT_GMM:
                    g_warning("<input rejected by GMM>\n");
                    break;
                  case J_RESULT_STATUS_REJECT_SHORT:
                    g_warning("<input rejected by short input>\n");
                    break;
                  case J_RESULT_STATUS_FAIL:
                    g_warning("<search failed>\n");
                    break;
              }
              /* continue to next process instance */
              continue;
        }

        if (r->result.status == 0)
        {
            /* output results for all the obtained sentences */
            winfo = r->lm->winfo;

            for(n = 0; n < r->result.sentnum; n++) { /* for all sentences */

                s = &(r->result.sent[n]);
                seq = s->word;
                seqnum = s->word_num;

                result = TRUE;

                /* Output word sequence */
                if(seqnum > 1)
                {
                    sentence = g_strdup(winfo->woutput[seq[1]]);
                    for(i = 2; i < seqnum - 1; i++)
                        sentence = g_strconcat(sentence, " ", winfo->woutput[seq[i]]
                                , NULL);
                }
                else /* For isolated words */
                {
                    sentence = g_strdup(winfo->woutput[seq[0]]);
                }

#ifdef ENABLE_DEBUG_DUMP
                g_message("Sentence: %s\n", sentence);
#endif

                /* Confidence scores */
#ifdef ENABLE_DEBUG_DUMP
                g_message("Score: %5.3f\n", s->score);
#endif

                if(seqnum > 1)
                {
#ifdef ENABLE_DEBUG_DUMP
                    g_message("Partial confidences: ");
#endif
                    for (i = 1; i < seqnum - 1; i++)
                    {
                        count++;
                        confidence = confidence + s->confidence[i];
#ifdef ENABLE_DEBUG_DUMP
                        g_message(" %5.3f", s->confidence[i]);
#endif
                    }

                }
                else /* For isolated words */
                {
                    count ++;
                    confidence = s->confidence[0];
                }
                confidence = confidence / count;
#ifdef ENABLE_DEBUG_DUMP
                g_message("Confidence: ");
                g_message(" %5.3f", confidence);
                g_message("\n");
#endif
            }
        }

        if(r->result.status == 0 && confidence >= CONF_THRESHOLD)
        {
#ifdef ENABLE_DEBUG_DUMP
            g_message("Sending results");
#endif
            on_message (applet, sentence);
        }
        else
            g_warning("Recognition rejected");

        g_free(sentence);

    }
}

static void
j_recognize (gpointer data)
{
  int ret = 0;
  VoiceControlApplet *applet = (VoiceControlApplet *) data;

   switch(j_open_stream(((ASRJulius *) applet->asr)->decoder, NULL)) {
       case 0:			/* succeeded */
         break;
       case -1:      		/* error */
         g_critical("Error in input stream\n");
         return;
       case -2:			/* end of recognition process */
         g_warning("Failed to begin input stream\n");
         return;
   }
   
  /* enter main loop to recognize the input stream */
  /* finish after whole input has been processed and input reaches end */
  ret = j_recognize_stream(((ASRJulius *)applet->asr)->decoder);
  if(ret == -1)
  {
      g_critical("Recognition error");
  }
  else
  {
      g_message("Recognition thread closed");
  }

  g_thread_exit(&ret);

}

///////////////////////////////
// ASR Interface
///////////////////////////////

void
asr_setup(int* argc, char **argv [], gpointer data)
{
    // Not needed for this engine
}

void
asr_init(VoiceControlApplet* applet)
{
    Jconf* jconf = NULL;
    char* lang = NULL;
    char ** splits = NULL;

    applet->asr = g_malloc(sizeof(ASRJulius));

#ifdef ENABLE_DEBUG_DUMP
    /* output system information to log */
    j_enable_debug_message();
    j_enable_verbose_message();
#else
    j_disable_debug_message();
    j_disable_verbose_message();
#endif

    splits = g_strsplit(applet->lang, "_", 2);
    lang = splits[0];

    jconf = j_setup_jconf(lang);

    if (jconf != NULL)
    {
        setlocale (LC_ALL, "C");
        ((ASRJulius *) applet->asr)->decoder = j_create_instance_from_jconf(jconf);
        setlocale (LC_ALL, applet->lang);
    }
    else
    {
        j_output_argument_help(stdout);
        g_error("Cannot initialize recognition engine");
        return;
    }

    if (g_strcmp0(lang, "es") == 0)
    {
        j_process_deactivate(((ASRJulius *) applet->asr)->decoder, "en");
    } else j_process_deactivate(((ASRJulius *) applet->asr)->decoder, "es");

    ((ASRJulius *) applet->asr)->pause = FALSE;
    ((ASRJulius *) applet->asr)->initialized = FALSE;

    callback_add(((ASRJulius *) applet->asr)->decoder, CALLBACK_RESULT, j_process_result, applet);
    callback_add(((ASRJulius *) applet->asr)->decoder, CALLBACK_EVENT_SPEECH_START, on_listening, applet);
    callback_add(((ASRJulius *) applet->asr)->decoder, CALLBACK_EVENT_SPEECH_READY, on_ready, applet);
    callback_add(((ASRJulius *) applet->asr)->decoder, CALLBACK_PAUSE_FUNCTION, on_pause, applet);

    /**************************/
    /* Initialize audio input */
    /**************************/
    /* initialize audio input device */
    /* ad-in thread starts at this time for microphone */

    if (j_adin_init(((ASRJulius *) applet->asr)->decoder) == FALSE) {
      g_error("Cannot initialize input device");
      return;
    }

#ifdef ENABLE_DEBUG_DUMP
    /* output system information to log */
    j_recog_info(((ASRJulius *) applet->asr)->decoder);
#endif

    g_strfreev(splits);

}

void
asr_destroy(VoiceControlApplet* applet)
{
    if(((ASRJulius *)applet->asr)->initialized == TRUE
            || ((ASRJulius *)applet->asr)->pause == TRUE)
    {
        j_close_stream(((ASRJulius *)applet->asr)->decoder);
        ((ASRJulius *) applet->asr)->initialized = FALSE;
        ((ASRJulius *) applet->asr)->pause = FALSE;
    }

    if(juliusThread != NULL)
        g_thread_join(juliusThread);

    j_recog_free(((ASRJulius *)applet->asr)->decoder);

    ((ASRJulius *) applet->asr)->initialized = FALSE;

    g_free(applet->asr);
}

void
asr_start(VoiceControlApplet* applet)
{
    GError* error;

    if(((ASRJulius *) applet->asr)->initialized == FALSE)
    {
        on_init(applet);

        ((ASRJulius *) applet->asr)->initialized = TRUE;

        if(((ASRJulius *) applet->asr)->pause == TRUE)
        {
            ((ASRJulius *) applet->asr)->pause = FALSE;
        }

        juliusThread = g_thread_create((GThreadFunc) j_recognize, applet, TRUE, &error);
        if(juliusThread == NULL)
        {
          g_critical("Recognition thread cannot be created: %s", error->message);
          return;
        }
    }
    else
        g_warning("Recognizer already in use");
}

void
asr_pause(VoiceControlApplet* applet)
{
    if ( ((ASRJulius *) applet->asr)->pause == FALSE 
            &&  ((ASRJulius *) applet->asr)->initialized == TRUE)
    {
        ((ASRJulius *) applet->asr)->pause = TRUE;
        j_request_pause(((ASRJulius *) applet->asr)->decoder);
    }
    else if(((ASRJulius *) applet->asr)->initialized == TRUE)
    {
        g_warning("Recognizer is currently paused");
    }
}

void
asr_resume(VoiceControlApplet* applet)
{
    if (((ASRJulius *) applet->asr)->pause == TRUE)
    {
        j_request_resume(((ASRJulius *) applet->asr)->decoder);
        ((ASRJulius *) applet->asr)->pause = FALSE;
    }
}

void
asr_stop(VoiceControlApplet* applet)
{
    if ( asr_running(applet) == TRUE )
    {

        ((ASRJulius *) applet->asr)->initialized = FALSE;

        if (((ASRJulius *) applet->asr)->pause = TRUE)
            asr_resume(applet);

        ((ASRJulius *) applet->asr)->pause = FALSE;
        
        /* calling j_close_stream(recog) at any time will terminate
         recognition and exit j_recognize_stream() */
        j_close_stream(((ASRJulius *) applet->asr)->decoder);

        g_message("Recognizer stopped");
    }
    else
        g_warning("Recognizer is currently stoped");
}

gboolean
asr_running(VoiceControlApplet* applet)
{
    return ((ASRJulius *) applet->asr)->initialized;
}

void
asr_set_grammar(VoiceControlApplet* applet, GSList* commands)
{
    // Not implemented yet
}

void
asr_set_language(VoiceControlApplet* applet, gchar* lang)
{
    gchar ** splits = NULL;
    gchar* old_lang = NULL;
    gboolean started = FALSE;

    splits = g_strsplit(applet->lang, "_", 2);
    old_lang = splits[0];

    if(asr_running(applet) == TRUE)
    {
        started = TRUE;
        asr_stop(applet);
    }

    if (g_strcmp0(lang, applet->lang) != 0)
    {
        j_process_activate(((ASRJulius *) applet->asr)->decoder, lang);
        j_process_deactivate(((ASRJulius *) applet->asr)->decoder, old_lang);
    }

    if(started == TRUE)
        asr_start(applet);

    g_free(splits);
}
