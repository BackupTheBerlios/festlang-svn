#ifndef PHONESET_H
#define PHONESET_H

enum phoneset
{
    PHONE_PAU = 1,
    PHONE_SYLBREAK,

    PHONE_I,
    PHONE_Y,
    PHONE_E,
    PHONE_A,
    PHONE_O,
    PHONE_U,

    PHONE_AO,
    PHONE_EI,
    PHONE_AE,

    PHONE_P,
    PHONE_PP,
    PHONE_B,
    PHONE_BB,
    PHONE_T,
    PHONE_TT,
    PHONE_D,
    PHONE_DD,
    PHONE_K,
    PHONE_KK,
    PHONE_G,
    PHONE_GG,

    PHONE_C,
    PHONE_CH,
    PHONE_F,
    PHONE_FF,
    PHONE_V,
    PHONE_VV,
    PHONE_S,
    PHONE_SS,
    PHONE_Z,
    PHONE_ZZ,
    PHONE_SH,
    PHONE_SCH,
    PHONE_ZH,
    PHONE_H,
    PHONE_HH,

    PHONE_M,
    PHONE_MM,
    PHONE_N,
    PHONE_NN,
    PHONE_L,
    PHONE_LL,
    PHONE_R,
    PHONE_RR,
    PHONE_IJ,
};

void   dump_phones (char *phones);
int    is_vowel (char phone);
int    is_pau (char phone);

#endif
