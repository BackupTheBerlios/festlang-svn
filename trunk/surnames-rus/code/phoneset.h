#ifndef PHONESET_H
#define PHONESET_H

enum phoneset
{
    PHONE_PAU = 1,
    PHONE_SYLBREAK,

    PHONE_II,
    PHONE_YY,
    PHONE_EE,
    PHONE_AA,
    PHONE_OO,
    PHONE_UU,

    PHONE_I,
    PHONE_Y,
    PHONE_E,
    PHONE_A,
    PHONE_O,
    PHONE_U,

    PHONE_AE,
    PHONE_AY,
    PHONE_UR,

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
    PHONE_J,
};

void   dump_phones (char *phones);
int    is_vowel (char phone);
int    is_pau (char phone);
int    is_soft (char phone);

int    phone_is_sonorant (char phone);
int    phone_is_vv (char phone);
int    phone_is_voice_reducable (char phone);
int    phone_is_voiceless_reducable (char phone);
int    phone_voice_reduce (char phone);
int    phone_is_noise_voiced (char phone);
int    phone_is_noise_voiceless (char phone);

#endif
