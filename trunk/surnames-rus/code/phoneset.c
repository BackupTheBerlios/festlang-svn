#include <stdio.h>
#include "phoneset.h"

/**********************************************************************
 *
 * Phoneset array is used to convert phones to their text representation
 *
 **********************************************************************/

char* phoneset[] = {
    "pau",
    "sylbreak",
    "ii",
    "yy",
    "ee",
    "aa",
    "oo",
    "uu",
    "i",
    "y",
    "e",
    "a",
    "o",
    "u",
    "ae",
    "ay",
    "ur",
    "p",
    "pp",
    "b",
    "bb",
    "t",
    "tt",
    "d",
    "dd",
    "k",
    "kk",
    "g",
    "gg",
    "c",
    "ch",
    "f",
    "ff",
    "v",
    "vv",
    "s",
    "ss",
    "z",
    "zz",
    "sh",
    "sch",
    "zh",
    "h",
    "hh",
    "m",
    "mm",
    "n",
    "nn",
    "l",
    "ll",
    "r",
    "rr",
    "j",
};

/**********************************************************************
 *
 * dump_phones: 
 * 
 * Dumps the sequence of phones to standard output
 *
 * @phones: null-terminated string of phones
 *
 **********************************************************************/
void
dump_phones (char *phones)
{
    int i;

    for (i = 0; phones[i] != 0; i++)
      {
	  printf ("%s ", phoneset[phones[i] - 1]);
      }
    printf ("\n");
}	/*dump_phones */


/**********************************************************************
 *
 * is_vowel: 
 * 
 * Returns TRUE if phone is vowel and FALSE otherwise
 *
 * @phone: phone
 *
 **********************************************************************/

int
is_vowel (char phone)
{
    return (phone >= PHONE_I) && (phone <= PHONE_UR);
}	/*is_vowel */

/**********************************************************************
 *
 * is_vowel: 
 *
 * Returns TRUE if phone is vowel and FALSE otherwise
 *
 * @phone: phone
 *
 **********************************************************************/
int
is_pau (char phone)
{
    return (phone <= PHONE_SYLBREAK);
}	/*is_pau */


/**********************************************************************
 *
 * is_soft: 
 *
 * Returns TRUE if phone is soft and FALSE otherwise
 *
 * @phone: phone
 *
 **********************************************************************/
int
is_soft (char phone)
{
    return 	
	(phone == PHONE_PP) ||
	(phone == PHONE_BB) ||
	(phone == PHONE_TT) ||
	(phone == PHONE_DD) ||
	(phone == PHONE_KK) ||
	(phone == PHONE_GG) ||
	(phone == PHONE_CH) ||
	(phone == PHONE_FF) ||
	(phone == PHONE_VV) ||
	(phone == PHONE_SS) ||
	(phone == PHONE_ZZ) ||
	(phone == PHONE_SCH) ||
	(phone == PHONE_ZH) ||
	(phone == PHONE_HH) ||
	(phone == PHONE_MM) ||
	(phone == PHONE_NN) ||
	(phone == PHONE_LL) ||
	(phone == PHONE_RR) ||
	(phone == PHONE_J);
}

/**********************************************************************
 *
 * phone_is_sonorant: 
 *
 * Returns TRUE if phone is sonorant
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_is_sonorant (char phone)
{
    return (phone >= PHONE_M) && (phone <= PHONE_J);
}

/**********************************************************************
 *
 * phone_is_vv: 
 *
 * Returns TRUE if phone is v or vv
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_is_vv (char phone)
{
    return (phone == PHONE_V) || (phone <= PHONE_VV);
}

/**********************************************************************
 *
 * phone_is_voice_reducable: 
 *
 * Returns TRUE if phone is reducable to voiceless variant
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_is_voice_reducable (char phone)
{
    return (phone == PHONE_B) || 
	   (phone == PHONE_BB) || 
	   (phone == PHONE_V) || 
	   (phone == PHONE_VV) || 
	   (phone == PHONE_ZH) ||
	   (phone == PHONE_Z) || 
	   (phone == PHONE_ZZ) || 
	   (phone == PHONE_G) || 
	   (phone == PHONE_GG) || 
	   (phone == PHONE_D) || 
	   (phone == PHONE_DD); 
}

/**********************************************************************
 *
 * phone_is_voiceless_reducable: 
 *
 * Returns TRUE if phone is reducable to voiced variant
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_is_voiceless_reducable (char phone)
{
    return (phone == PHONE_P) || 
	   (phone == PHONE_PP) || 
	   (phone == PHONE_F) || 
	   (phone == PHONE_FF) || 
	   (phone == PHONE_SH) || 
	   (phone == PHONE_S) || 
	   (phone == PHONE_SS) || 
	   (phone == PHONE_K) || 
	   (phone == PHONE_KK) || 
	   (phone == PHONE_T) || 
	   (phone == PHONE_TT);
}

/**********************************************************************
 *
 * phone_voice_reduce: 
 *
 * Reduces phone to voiced or voiceless variant
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_voice_reduce (char phone)
{
    switch (phone) 
	{
	   case PHONE_B: return PHONE_P;
	   case PHONE_P: return PHONE_B;
	   case PHONE_BB: return PHONE_PP;
	   case PHONE_PP: return PHONE_BB;
	   case PHONE_V: return PHONE_F;
	   case PHONE_F: return PHONE_V;
	   case PHONE_VV: return PHONE_FF;
	   case PHONE_FF: return PHONE_VV;
	   case PHONE_ZH: return PHONE_SH;
	   case PHONE_SH: return PHONE_ZH;
	   case PHONE_Z: return PHONE_S;
	   case PHONE_S: return PHONE_Z;
	   case PHONE_ZZ: return PHONE_SS;
	   case PHONE_SS: return PHONE_ZZ;
	   case PHONE_G: return PHONE_K;
	   case PHONE_K: return PHONE_G;
	   case PHONE_GG: return PHONE_KK;
	   case PHONE_KK: return PHONE_GG;
	   case PHONE_D: return PHONE_T;
	   case PHONE_T: return PHONE_D;
	   case PHONE_DD: return PHONE_TT;
	   case PHONE_TT: return PHONE_DD;
	   default: return phone;
	}
}

/**********************************************************************
 *
 * phone_is_noise_voiced: 
 *
 * Returns TRUE if phone is voiced noise:
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_is_noise_voiced (char phone)
{
    return (phone == PHONE_B) || 
	   (phone == PHONE_BB) || 
	   (phone == PHONE_ZH) || 
	   (phone == PHONE_Z) || 
	   (phone == PHONE_ZZ) || 
	   (phone == PHONE_D) || 
	   (phone == PHONE_DD) || 
	   (phone == PHONE_G) || 
	   (phone == PHONE_GG);
}

/**********************************************************************
 *
 * phone_is_noise_voiceless: 
 *
 * Returns TRUE if phone is voiceless noise:
 *
 * @phone: phone
 *
 **********************************************************************/
int
phone_is_noise_voiceless (char phone)
{
    return (phone == PHONE_P) || 
	   (phone == PHONE_PP) || 
	   (phone == PHONE_SH) || 
	   (phone == PHONE_S) || 
	   (phone == PHONE_SS) || 
	   (phone == PHONE_T) || 
	   (phone == PHONE_TT) || 
	   (phone == PHONE_K) || 
	   (phone == PHONE_KK) || 
	   (phone == PHONE_SCH);
}

