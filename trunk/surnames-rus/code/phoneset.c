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
	(phone == PHONE_IJ);
}
