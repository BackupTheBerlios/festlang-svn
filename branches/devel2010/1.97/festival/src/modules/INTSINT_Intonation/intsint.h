/************************************************************************************/
/* Copyright ?????????                                                              */
/* All rights reserved.                                                             */  
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR: Aby Louw                                                                 */
/* Email:                                                                           */
/* MODIFIED: Feb 10, 2004                                                           */
/*                                                                                  */
/************************************************************************************/

#ifndef __INTSINT_H__
#define __INTSINT_H__

#include "EST_StringTrie.h"

#define SAMPLE_RATE 0.01
const float BIG_NUMBER = 9999.0;

class Intsint {
  public:
    Intsint();
    ~Intsint();
    
    EST_String name;

    EST_Utterance utt;
    class Intsint *prev_unit;
    class Intsint *next_unit;
};


class Intsint_DB {
  public:
    Intsint_DB();
    ~Intsint_DB();

    LISP params;
    LISP uttnames;
    EST_StringTrie index;
    float momel_mean;
    float mid;
    float top;
    float bottom;
      

    Intsint *get_unit(const EST_String &name)
	{ return (Intsint *)index.lookup(name); }
};

LISP intsint_load_db(LISP params, LISP utts_names);
//LISP intsint_add_labels(void);

Intsint_DB *check_isdb();
LISP isdb_list(void);
LISP isdb_select(LISP dbname);
LISP isdb_optimise(void);
LISP isdb_add_intsint_labels(void);
LISP isdb_save_intsint_labels(void);
LISP isdb_save_intsint_utts(void);
LISP isdb_calc_intsint_momelqs(void);

float get_F0_track(EST_Track tr, float t);
float preceding_pause_length(EST_Item *i);
float get_syl_vowel_mid_time(EST_Item *syl);
float octave(float t);
float linear(float t);
int round(float t);
void optimise(Intsint *is, float mid, float range, float *min_error2,
	      float higher, float lower, float up, float down);

float make_estimate(EST_String tone, float last_target, float mid, float top,
		    float bottom, float higher, float lower, float up, float down);
EST_Track calc_momel_curve(EST_FVector momel_time, EST_FVector momel_f0, int nval);

LISP Intonation_INTSINT_Utt(LISP utt);
LISP Int_Targets_INTSINT_Utt(LISP utt);



#endif
