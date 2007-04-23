#include <stdio.h>
#include "phoneset.h"

/**********************************************************************/
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

/**********************************************************************/
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

/**********************************************************************/
int
is_vowel (char phone)
{
    return (phone >= PHONE_I) && (phone <= PHONE_AE);
}	/*is_vowel */

/**********************************************************************/
int
is_pau (char phone)
{
    return (phone <= PHONE_SYLBREAK);
}	/*is_pau */
