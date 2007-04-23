#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "phoneset.h"
#include "cart_search.h"

/*---------------------------------------------------------------------------*/
typedef struct utterance
{
    char text[256];
    char phones[256];
    char stress[256];
} utterance;

/*---------------------------------------------------------------------------*/
typedef struct lts_rule
{
    char **predessors;
    char *letter;
    char **ancessors;
    char phone;
    char second_phone;
} lts_rule;

/*---------------------------------------------------------------------------*/
char *startsyl[] = {
    "#", "ß", "Ø", "Á", "Ñ", "Ï", "£", "Õ", "À", "Ü", "Å", "É",
    NULL
};

/*---------------------------------------------------------------------------*/
char *softletters[] = {
    "Ñ", "£", "À", "É", "Ø", "Å", NULL
};

/*---------------------------------------------------------------------------*/
char *softsign[] = {
    "Ø", NULL
};


/*---------------------------------------------------------------------------*/
lts_rule lts_ruleset[] = {
    {NULL, " ", NULL, PHONE_SYLBREAK},

    {NULL, "Á", NULL, PHONE_A, 0},
    {NULL, "Ü", NULL, PHONE_E, 0},
    {softsign, "Ï", NULL, PHONE_IJ, PHONE_O},
    {NULL, "Ï", NULL, PHONE_O, 0},
    {NULL, "Õ", NULL, PHONE_U, 0},
    {NULL, "Ù", NULL, PHONE_Y, 0},

    {startsyl, "À", NULL, PHONE_IJ, PHONE_U},
    {startsyl, "Ñ", NULL, PHONE_IJ, PHONE_A},
    {startsyl, "£", NULL, PHONE_IJ, PHONE_O},
    {startsyl, "Å", NULL, PHONE_IJ, PHONE_E},

    {NULL, "£", NULL, PHONE_O, 0},
    {NULL, "Å", NULL, PHONE_E, 0},
    {NULL, "Ñ", NULL, PHONE_A, 0},
    {NULL, "À", NULL, PHONE_U, 0},
    {NULL, "É", NULL, PHONE_I, 0},

    {NULL, "Â", softletters, PHONE_BB, 0},
    {NULL, "×", softletters, PHONE_VV, 0},
    {NULL, "Ç", softletters, PHONE_GG, 0},
    {NULL, "Ä", softletters, PHONE_DD, 0},
    {NULL, "Ú", softletters, PHONE_ZZ, 0},
    {NULL, "Ë", softletters, PHONE_KK, 0},
    {NULL, "Ì", softletters, PHONE_LL, 0},
    {NULL, "Í", softletters, PHONE_MM, 0},
    {NULL, "Î", softletters, PHONE_NN, 0},
    {NULL, "Ð", softletters, PHONE_PP, 0},
    {NULL, "Ò", softletters, PHONE_RR, 0},
    {NULL, "Ó", softletters, PHONE_SS, 0},
    {NULL, "Ô", softletters, PHONE_TT, 0},
    {NULL, "Æ", softletters, PHONE_FF, 0},
    {NULL, "È", softletters, PHONE_HH, 0},

    {NULL, "Â", NULL, PHONE_B, 0},
    {NULL, "×", NULL, PHONE_V, 0},
    {NULL, "Ç", NULL, PHONE_G, 0},
    {NULL, "Ä", NULL, PHONE_D, 0},
    {NULL, "Ö", NULL, PHONE_ZH, 0},
    {NULL, "Ú", NULL, PHONE_Z, 0},
    {NULL, "Ë", NULL, PHONE_K, 0},
    {NULL, "Ì", NULL, PHONE_L, 0},
    {NULL, "Í", NULL, PHONE_M, 0},
    {NULL, "Î", NULL, PHONE_N, 0},
    {NULL, "Ð", NULL, PHONE_P, 0},
    {NULL, "Ò", NULL, PHONE_R, 0},
    {NULL, "Ó", NULL, PHONE_S, 0},
    {NULL, "Ô", NULL, PHONE_T, 0},
    {NULL, "Æ", NULL, PHONE_F, 0},
    {NULL, "È", NULL, PHONE_H, 0},
    {NULL, "Ã", NULL, PHONE_C, 0},
    {NULL, "Þ", NULL, PHONE_CH, 0},
    {NULL, "Û", NULL, PHONE_SH, 0},
    {NULL, "Ý", NULL, PHONE_SCH, 0},
    {NULL, "Ê", NULL, PHONE_IJ, 0},

    {NULL, "-", NULL, 0, 0},
    {NULL, "ß", NULL, 0, 0},
    {NULL, "Ø", NULL, 0, 0},

    {NULL, NULL, NULL, 0}
};

/**********************************************************************/
int
check_context (char *text, int text_idx, int i)
{
    int j, len;

    if (strncmp (text + text_idx,
		 lts_ruleset[i].letter, strlen (lts_ruleset[i].letter)) != 0)
	return 0;

    if (lts_ruleset[i].ancessors == NULL && lts_ruleset[i].predessors == NULL)
	return 1;

    len = strlen (lts_ruleset[i].letter);

    if (lts_ruleset[i].ancessors != NULL)
      {
	  for (j = 0; lts_ruleset[i].ancessors[j] != 0; j++)
	    {
		if (strncmp
		    (text + text_idx + len, lts_ruleset[i].ancessors[j],
		     strlen (lts_ruleset[i].ancessors[j])) == 0)
		    return 1;
	    }
      }

    if (lts_ruleset[i].predessors != NULL)
      {
	  for (j = 0; lts_ruleset[i].predessors[j] != 0; j++)
	    {

		if (lts_ruleset[i].predessors[j][0] == '#' &&
		    (text_idx <= 0 || text[text_idx - 1] == ' '))
		    return 1;

		if (text_idx < strlen (lts_ruleset[i].predessors[j]))
		    continue;
		if (strncmp
		    (text + text_idx - strlen (lts_ruleset[i].predessors[j]),
		     lts_ruleset[i].predessors[j],
		     strlen (lts_ruleset[i].predessors[j])) == 0)
		    return 1;

	    }
      }
    return 0;
}	/*check_context */

/**********************************************************************/
void
utterance_lts (utterance * utt)
{
    char *text = utt->text;
    char *phones = utt->phones;

    int text_idx;
    int phones_idx;
    int i;
    int found_len;

    phones_idx = 0;
    phones[phones_idx++] = PHONE_PAU;

    for (text_idx = 0; text[text_idx] != 0;)
      {
	  found_len = 1;
	  for (i = 0; lts_ruleset[i].letter != 0; i++)
	    {
		if (check_context (text, text_idx, i))
		  {

		      if (lts_ruleset[i].phone != 0)
			{
			    phones[phones_idx++] = lts_ruleset[i].phone;
			    if (lts_ruleset[i].second_phone != 0)
				phones[phones_idx++] =
				    lts_ruleset[i].second_phone;
			}

		      found_len = strlen (lts_ruleset[i].letter);
		      break;
		  }
	    }
	  text_idx += found_len;
      }

    phones[phones_idx++] = PHONE_PAU;
    phones[phones_idx++] = 0;

    return;
}	/*utterance_lts */

/**********************************************************************/
void
utterance_stress (utterance * utt)
{
    int i, last_index;
    char  min_probability, probability;

    for (i = 0; utt->phones[i] != 0; i++)
      {
	  if (is_vowel (utt->phones[i]))
	    {
		probability = find_stress_probability (utt->phones, i, 0);
#if DEBUG
		printf ("Phone %d probability %d \n", utt->phones[i], probability);
#endif
		if (probability <= min_probability)
		  {
		      min_probability = probability;
		      last_index = i;
		  }
	    }
	  utt->stress[i] = 0;
	  if (is_pau (utt->phones[i]))
	    {
		if (last_index >= 0)
		  {
		      utt->stress[last_index] = 1;
		  }
		last_index = -1;
		min_probability = (int) (2.0 * FLOAT_SCALE);
	    }
      }
}	/*utterance_stress */

/**********************************************************************/
int
next_is_stressed (utterance * utt, int i)
{
    int j;

    for (j = i + 1; utt->phones[j] != 0; j++)
      {
	  if (is_vowel (utt->phones[j]))
	      return utt->stress[j];
      }
    return 1;
}	/*next_is_stressed */

/**********************************************************************/
void
utterance_reduce (utterance * utt)
{
    int i;

    for (i = 1; utt->phones[i] != 0; i++)
      {
	  if ((utt->phones[i] == PHONE_A || utt->phones[i] == PHONE_O) &&
	      utt->stress[i] == 0 && next_is_stressed (utt, i))
	    {
		utt->phones[i] = PHONE_AO;
		continue;
	    }
	  if ((utt->phones[i] == PHONE_E || utt->phones[i] == PHONE_I) &&
	      utt->stress[i] == 0 && next_is_stressed (utt, i))
	    {
		utt->phones[i] = PHONE_EI;
		continue;
	    }
	  if ((utt->phones[i] == PHONE_O ||
	       utt->phones[i] == PHONE_E ||
	       utt->phones[i] == PHONE_I ||
	       utt->phones[i] == PHONE_A) && utt->stress[i] == 0)
	      utt->phones[i] = PHONE_AE;
      }
}	/*utterance_reduce */

/**********************************************************************/
int
main ()
{
    utterance utt;

    while (fgets (utt.text, 256, stdin))
      {

	  utterance_lts (&utt);
	  utterance_stress (&utt);
	  utterance_reduce (&utt);

	  dump_phones (utt.phones);
      }
}	/*main */
