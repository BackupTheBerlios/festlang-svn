#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "phoneset.h"
#include "cart_search.h"
#include "dict.h"

#define UNSTRESSED 1
#define STRESSED   2

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
word_lts (char *text, char *phones)
{
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
int
word_dict_search (char *text)
{
    int a = 0;
    int b = DICT_SIZE;
    int c;
    
    while (a + 1 < b) 
	{
	    c = (a + b) / 2;
	    if (strcmp (text, dict[c].word) <= 0)
		    b = c;
	    else
		    a = c;
	}

    if (strcmp (text, dict[a].word) == 0)
	    return dict[a].stress;
    else if (strcmp (text, dict[b].word) == 0)
	    return dict[b].stress;
    else 
	    return -1;
}      /* word_dict_search */

/**********************************************************************/
void
word_stress_cart (char *phones, char *stress)
{
    int i, last_index;
    char  min_probability, probability;

    last_index = -1;
    for (i = 0; phones[i] != 0; i++)
      {	
    	 if (is_vowel (phones[i]))
	    {
		probability = find_stress_probability (phones, i, 0);
#if DEBUG
		printf ("Phone %d probability %d \n", phones[i], probability);
#endif
		if (probability <= min_probability)
		  {
		      min_probability = probability;
		      last_index = i;
		  }
	    }
        	
	  stress[i] = UNSTRESSED;
	  if (is_pau (phones[i]))
	    {
		if (last_index >= 0)
		  {
		      stress[last_index] = STRESSED;
		  }
		last_index = -1;
		min_probability = (int) (2.0 * FLOAT_SCALE);
	    }
      }
}	/*utterance_stress */

/**********************************************************************/
void
word_stress_dict (char *phones, char *stress, int stress_ind)
{
    int i, vowel_index;

    for (i = 0, vowel_index = 0; phones[i] != 0; i++)
	{
	    if (is_vowel (phones[i]))
		    vowel_index ++;
	    if (vowel_index == stress_ind)
		{
		    stress[i] = STRESSED;
		    vowel_index++;
		}
	    else
		    stress[i] = UNSTRESSED;
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
	      return (utt->stress[j] == STRESSED);
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
	      utt->stress[i] == UNSTRESSED && next_is_stressed (utt, i))
	    {
		utt->phones[i] = PHONE_AO;
		continue;
	    }
	  if ((utt->phones[i] == PHONE_E || utt->phones[i] == PHONE_I) &&
	      utt->stress[i] == UNSTRESSED && next_is_stressed (utt, i))
	    {
		utt->phones[i] = PHONE_EI;
		continue;
	    }
	  if ((utt->phones[i] == PHONE_O ||
	       utt->phones[i] == PHONE_E ||
	       utt->phones[i] == PHONE_I ||
	       utt->phones[i] == PHONE_A) && utt->stress[i] == UNSTRESSED)
	      utt->phones[i] = PHONE_AE;
      }
}	/*utterance_reduce */

void
utterance_lts (utterance *utt)
{
    char phones[256];
    char stress[256];
    char* str, *saveptr, *word;
    int dict_stress;
    int index;

    memset (utt->phones, 0, sizeof(utt->phones));
    memset (utt->stress, 0, sizeof(utt->stress));
    
    index = 1;
    utt->phones[0] = PHONE_PAU;
    utt->stress[0] = UNSTRESSED;

    for (str = utt->text; ; str = NULL) 
        {
             word = strtok_r(str, " \t\n", &saveptr);
             if (word == NULL)
            	    break;
            	    
#if DEBUG
	     printf ("Processing word |%s|\n", word);
#endif

             memset (phones, 0, sizeof(phones));
             memset (stress, 0, sizeof(stress));
             
             word_lts(word, phones);

             dict_stress = word_dict_search (word);
             
             if (dict_stress > 0)
               {
                  word_stress_dict (phones, stress, dict_stress);	
               }
             else
               {
                  word_stress_cart (phones, stress);
               }

    	      strcpy (utt->phones + index, phones + 1);
    	      strcpy (utt->stress + index, stress + 1);

    	      index += strlen (phones) - 2; /* Without leading and trailing pau */
    	      utt->phones[index] = PHONE_SYLBREAK;
    	      index ++;
	}
    utt->phones[index - 1] = PHONE_PAU;

    utterance_reduce (utt);
}

/**********************************************************************/
int
main ()
{
    utterance utt;

    while (fgets (utt.text, 256, stdin))
      {

	  utterance_lts (&utt);
	  dump_phones (utt.phones);

#if DEBUG
	  int i;
	  for (i = 0; utt.stress[i] != 0; i++)
		printf ("%d ",utt.stress[i]);
	  printf ("\n");
#endif
      }
}	/*main */
