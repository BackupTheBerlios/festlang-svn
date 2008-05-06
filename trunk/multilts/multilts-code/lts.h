/*************************************************************************/
/*                                                                       */
/*  Letter to sound rules                                                */
/*                                                                       */
/*************************************************************************/
#ifndef _LTS_H__
#define _LTS_H__

#define LTS_NBEST 3

enum {
    PHONE_ZERO = 0,
    PHONE_PAU,
    PHONE_AA,
    PHONE_AE,
    PHONE_AH,
    PHONE_AO,
    PHONE_AW,
    PHONE_AX,
    PHONE_AXR,
    PHONE_AY,
    PHONE_B,
    PHONE_CH,
    PHONE_D,
    PHONE_DH,
    PHONE_EH,
    PHONE_ER,
    PHONE_EY,
    PHONE_F,
    PHONE_G,
    PHONE_HH,
    PHONE_IH,
    PHONE_IY,
    PHONE_JH,
    PHONE_K,
    PHONE_L,
    PHONE_M,
    PHONE_N,
    PHONE_NG,
    PHONE_OW,
    PHONE_OY,
    PHONE_P,
    PHONE_R,
    PHONE_S,
    PHONE_SH,
    PHONE_T,
    PHONE_TH,
    PHONE_UH,
    PHONE_UW,
    PHONE_V,
    PHONE_W,
    PHONE_Y,
    PHONE_Z,
    PHONE_ZH,
    PHONE_LAST
};

void lts (char *text, char **result);

void lts_dump_string (char *string);

#endif
