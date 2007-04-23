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
    "i",
    "y",
    "e",
    "a",
    "o",
    "u",
    "ao",
    "ei",
    "ae",
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
    "ij",
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
    return (phone >= PHONE_I) && (phone <= PHONE_AE);
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

