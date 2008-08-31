/*************************************************************************/
/*                                                                       */
/*  Letter to sound rules                                                */
/*                                                                       */
/*************************************************************************/
#ifndef _LTS_H__
#define _LTS_H__

/* The number of best guesses. It couldn't be changed now */
#define LTS_NBEST 3

/* This enumeration has phones from the TIMIT phoneset for phone-based representation.
   Phone zero marks the end of the string and used internally */
enum {
    PHONE_ZERO = 0,
    PHONE_AA,
    PHONE_AE,
    PHONE_AH,
    PHONE_AO,
    PHONE_AW,
    PHONE_AX,
    PHONE_AXH,
    PHONE_AXR,
    PHONE_AY,
    PHONE_B,
    PHONE_BCL,
    PHONE_CH,
    PHONE_D,
    PHONE_DCL,
    PHONE_DH,
    PHONE_DX,
    PHONE_EH,
    PHONE_EL,
    PHONE_EM,
    PHONE_EN,
    PHONE_EPI,
    PHONE_ER,
    PHONE_EY,
    PHONE_F,
    PHONE_G,
    PHONE_GCL,
    PHONE_HH,
    PHONE_HV,
    PHONE_IH,
    PHONE_IX,
    PHONE_IY,
    PHONE_JH,
    PHONE_K,
    PHONE_KCL,
    PHONE_L,
    PHONE_M,
    PHONE_N,
    PHONE_NG,
    PHONE_NX,
    PHONE_OW,
    PHONE_OY,
    PHONE_P,
    PHONE_PCL,
    PHONE_Q,
    PHONE_R,
    PHONE_S,
    PHONE_SH,
    PHONE_T,
    PHONE_TCL,
    PHONE_TH,
    PHONE_UH,
    PHONE_UW,
    PHONE_UX,
    PHONE_V,
    PHONE_W,
    PHONE_Y,
    PHONE_Z,
    PHONE_ZH,
    PHONE_LAST
};

/* 
   Initialize data structures
*/
void lts_init (void);

/* 
   Main functoin. Converts text to phones 
   
   @text: the string, which probably consists of several words. Only ASCII symbols are allowed, others
          are just ignored. The string shouldn't be longer than 256 symbols.
   @result: the storage for result, it must be preallocated array of the size LTS_NBEST 
	    of pointers to preallocated strings.
*/
void lts (char *text, char **result);

/* Helper functoin. Dumps on the standard output the contents of the string with phone-based
   representation of the phones. Used for debugging.
*/
void lts_dump_string (char *string);

#endif
