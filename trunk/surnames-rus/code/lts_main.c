#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "phoneset.h"
#include "cart_search.h"
#include "dict.h"

#define UNSTRESSED 1
#define STRESSED   2

/***************************************************************************
 *
 * Utterance are basic chunks of processed text. They has all information
 * about text markup we made.
 *
 ***************************************************************************/
typedef struct utterance
{
    char text[256];
    char phones[256];
    char stress[256];
} utterance;

/***************************************************************************
 *
 * LTS rules for Russian. Each rule has predessors, ancessors and letter.
 * The values of prediction are first and second phone if the are not zero.
 * This list should be ordered because the first rule matched is applied.
 *
 **************************************************************************/
typedef struct lts_rule
{
    char **predessors;
    char *letter;
    char **ancessors;
    char phone;
    char second_phone;
} lts_rule;

char *startsyl[] = {
    "#", "ъ", "ь", "а", "я", "о", "ё", "у", "ю", "э", "е", "и",
    NULL
};

char *softletters[] = {
    "я", "ё", "ю", "и", "ь", "е", NULL
};

char *letter_v[] = {
    "в", NULL
};

char *letter_n[] = {
    "н", NULL
};


lts_rule lts_ruleset[] = {

    {NULL, "а", NULL, PHONE_A, 0},
    {NULL, "э", NULL, PHONE_E, 0},
    {NULL, "ьо", NULL, PHONE_J, PHONE_O},
    {NULL, "о", NULL, PHONE_O, 0},
    {NULL, "у", NULL, PHONE_U, 0},
    {NULL, "ы", NULL, PHONE_Y, 0},

    {startsyl, "ю", NULL, PHONE_J, PHONE_U},
    {startsyl, "я", NULL, PHONE_J, PHONE_A},
    {startsyl, "ё", NULL, PHONE_J, PHONE_O},
    {startsyl, "е", NULL, PHONE_J, PHONE_E},

    {NULL, "ё", NULL, PHONE_O, 0},
    {NULL, "е", NULL, PHONE_E, 0},
    {NULL, "я", NULL, PHONE_A, 0},
    {NULL, "ю", NULL, PHONE_U, 0},
    {NULL, "и", NULL, PHONE_I, 0},
    
    {NULL, "вст", letter_v, PHONE_S, PHONE_T},
    {NULL, "стс", NULL, PHONE_S, PHONE_S},
    {NULL, "стьс", NULL, PHONE_S, PHONE_S},
    {NULL, "лнц", NULL, PHONE_N, PHONE_C},
    {NULL, "стц", NULL, PHONE_S, PHONE_C},
    {NULL, "здц", NULL, PHONE_Z, PHONE_C},
    {NULL, "нтц", NULL, PHONE_N, PHONE_C},
    {NULL, "ндц", NULL, PHONE_N, PHONE_C},
    {NULL, "ст", letter_n, PHONE_S, 0},
    {NULL, "зд", letter_n, PHONE_Z, 0},
    {NULL, "ндс", NULL, PHONE_N, PHONE_S},
    {NULL, "нтс", NULL, PHONE_N, PHONE_S},
    {NULL, "нгц", NULL, PHONE_N, PHONE_C},
    {NULL, "нгт", NULL, PHONE_N, PHONE_T},

    {NULL, "б", softletters, PHONE_BB, 0},
    {NULL, "в", softletters, PHONE_VV, 0},
    {NULL, "г", softletters, PHONE_GG, 0},
    {NULL, "д", softletters, PHONE_DD, 0},
    {NULL, "з", softletters, PHONE_ZZ, 0},
    {NULL, "к", softletters, PHONE_KK, 0},
    {NULL, "кк", softletters, PHONE_KK, 0},
    {NULL, "л", softletters, PHONE_LL, 0},
    {NULL, "м", softletters, PHONE_MM, 0},
    {NULL, "мм", softletters, PHONE_MM, 0},
    {NULL, "н", softletters, PHONE_NN, 0},
    {NULL, "нн", softletters, PHONE_NN, 0},
    {NULL, "п", softletters, PHONE_PP, 0},
    {NULL, "р", softletters, PHONE_RR, 0},
    {NULL, "с", softletters, PHONE_SS, 0},
    {NULL, "т", softletters, PHONE_TT, 0},
    {NULL, "ф", softletters, PHONE_FF, 0},
    {NULL, "х", softletters, PHONE_HH, 0},

    {NULL, "б", NULL, PHONE_B, 0},
    {NULL, "в", NULL, PHONE_V, 0},
    {NULL, "г", NULL, PHONE_G, 0},
    {NULL, "д", NULL, PHONE_D, 0},
    {NULL, "ж", NULL, PHONE_ZH, 0},
    {NULL, "з", NULL, PHONE_Z, 0},
    {NULL, "кк", NULL, PHONE_K, 0},
    {NULL, "к", NULL, PHONE_K, 0},
    {NULL, "л", NULL, PHONE_L, 0},
    {NULL, "м", NULL, PHONE_M, 0},
    {NULL, "мм", NULL, PHONE_M, 0},
    {NULL, "н", NULL, PHONE_N, 0},
    {NULL, "нн", NULL, PHONE_N, 0},
    {NULL, "п", NULL, PHONE_P, 0},
    {NULL, "р", NULL, PHONE_R, 0},
    {NULL, "с", NULL, PHONE_S, 0},
    {NULL, "сс", NULL, PHONE_S, 0},
    {NULL, "т", NULL, PHONE_T, 0},
    {NULL, "ф", NULL, PHONE_F, 0},
    {NULL, "х", NULL, PHONE_H, 0},
    {NULL, "ц", NULL, PHONE_C, 0},
    {NULL, "ч", NULL, PHONE_CH, 0},
    {NULL, "ш", NULL, PHONE_SH, 0},
    {NULL, "щ", NULL, PHONE_SCH, 0},
    {NULL, "й", NULL, PHONE_J, 0},

    {NULL, "-", NULL, 0, 0},
    {NULL, "ъ", NULL, 0, 0},
    {NULL, "ь", NULL, 0, 0},

    {NULL, NULL, NULL, 0}
};


/**********************************************************************
 *
 * check_context:
 *
 * Used in lts. Checks that current context which is pointed by text_idx
 * in string text is matched to the rule with index i.
 *
 * @text: null-terminated string
 * @text_idx: index in string
 * @i: index in LTS ruleset
 *
 * Returns: TRUE if context matches
 *
 **********************************************************************/
 
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

/**********************************************************************
 *
 * word_stress_lts:
 *
 * Convert string to the sequence of phones. According to LTS rules.
 * We go through the list of rules and search the first which can be
 * applied. If both contexts matches, we apply rule and append new phones
 * to the existing sequence. The first and last phones are always PAU.
 *
 * @text: null-terminated string
 * @phones: sequence of phones. Should be allocated already.
 *
 **********************************************************************/
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

/**********************************************************************
 *
 * word_dict_search:
 *
 * Implements binary search in dictionary. Note that it should be 
 * sorted in order to make this function working.
 *
 * @word: word to search for.
 *
 * Returns: which vowel is stressed.
 *
 **********************************************************************/
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

/**********************************************************************
 *
 * word_stress_cart:
 *
 * Fills stress array with values based on CART tree. It just takes
 * the last vowel with maximal stress probability. Although different
 * selection algorightms are possible
 *
 * @phones: sequence of phones
 * @stress: output array with stress flags. Should be allocated already.
 *
 **********************************************************************/
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

/**********************************************************************
 *
 * word_stress_dict:
 *
 * Fills stress array with values based on dictionary
 *
 * @phones: sequence of phones
 * @stress: output array with stress flags. Should be allocated already
 * @stress_ind: stressed vowel index predicted by dictionary.
 *
 **********************************************************************/
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

/**********************************************************************
 * 
 * next_is_stressed:
 *
 * Determines if next vowel is stressed, used in reduction algorithm.
 *
 * @utt: utterance with stress and phones filled. 
 * @i: vowel's index.
 *
 **********************************************************************/
int
next_is_stressed (utterance * utt, int i)
{
    int j;

    for (j = i + 1; utt->phones[j] != 0; j++)
      {
	  if (is_vowel (utt->phones[j]))
	      return (utt->stress[j] == STRESSED);
      }
    return 0;
}	/*next_is_stressed */

/**********************************************************************
 *
 * utterance_reduce_vowel:
 *
 * Reduces vowels in utterance according to predicted stress
 *
 * @utt: utterance with phones and stress filled
 *
 * The reduction rules are mostly taken from the article of Zaharov L.M.
 * Acoustic variativity of Russian M. 2004
 *
 *********************************************************************/
void
utterance_reduce_vowel (utterance * utt)
{
    int i;

    for (i = 1; utt->phones[i] != 0; i++)
      {
	  if (is_vowel (utt->phones[i]) && utt->stress[i] == STRESSED)
	    {
		utt->phones[i] = utt->phones[i] - 6; 
		continue;
	    }
	  if ((utt->phones[i] == PHONE_A || utt->phones[i] == PHONE_O) &&
	       (is_pau(utt->phones[i - 1]) ||
	       is_pau(utt->phones[i + 1]) || is_vowel(utt->phones [i - 1])))
	     {
	        utt->phones[i] = PHONE_A;
	    	continue;
	     }
	  if ((utt->phones[i] == PHONE_A || utt->phones[i] == PHONE_O) &&
	      next_is_stressed (utt, i))
	     {
	        if (is_soft (utt->phones[i - 1]))
	            utt->phones[i] = PHONE_I;
	        else 
	    	    utt->phones[i] = PHONE_A;
	    	continue;
	     }
	  if ((utt->phones[i] == PHONE_Y || utt->phones[i] == PHONE_I ||
	      utt->phones[i] == PHONE_U ) &&
	      (next_is_stressed (utt, i) || is_pau(utt->phones[i - 1]) ||
	       is_pau(utt->phones[i + 1]) || is_vowel(utt->phones [i - 1])))
	     {
	    	continue;
	     }
	  if (utt->phones[i] == PHONE_E &&
	       (is_pau(utt->phones[i - 1]) || is_pau(utt->phones[i + 1]) ||
	       is_vowel(utt->phones [i - 1])))
	     {
		continue;     
	     }
	  if (utt->phones[i] == PHONE_E && next_is_stressed (utt, i))
	     {
	        if (is_soft (utt->phones[i - 1]))
	            utt->phones[i] = PHONE_I;
	        else 
	    	    utt->phones[i] = PHONE_Y;
	    	continue;
	     }
	  if (utt->phones[i] == PHONE_Y || utt->phones[i] == PHONE_I ||
	      utt->phones[i] == PHONE_A || utt->phones[i] == PHONE_E ||
	      utt->phones[i] == PHONE_O)
	     {
	        if (is_soft (utt->phones[i - 1]))
	            utt->phones[i] = PHONE_AE;
	        else 
	    	    utt->phones[i] = PHONE_AY;
	    	continue;
	     }
	  if (utt->phones[i] == PHONE_U)
		utt->phones[i] = PHONE_UR;
      }
}	/*utterance_reduce_vowel */

/**********************************************************************
 *
 * utterance_reduce_voicing:
 *
 * Reduces consonant's voicing
 *
 * @utt: utterance with phones and stress filled
 *
 *********************************************************************/
void
utterance_reduce_voicing (utterance * utt)
{
    int i;

    for (i = 1; utt->phones[i + 1] != 0; i++)
      {
        
            if (phone_is_voice_reducable (utt->phones[i]))
	      {
	    	    if (utt->phones[i + 1] == PHONE_PAU)
		       {
			    utt->phones[i] = phone_voice_reduce (utt->phones[i]);
			    continue;
		       }
		    if (utt->phones[i + 1] == PHONE_SYLBREAK && 
			phone_is_sonorant (utt->phones[i + 2]))
		       {
		    	    utt->phones[i] = phone_voice_reduce (utt->phones[i]);
			    continue;
		       }
		    if (utt->phones[i + 1] == PHONE_SYLBREAK && 
			phone_is_vv (utt->phones[i + 2]) &&
			(phone_is_sonorant (utt->phones[i + 3]) ||
			 is_vowel (utt->phones[i + 3])))
		       {
			    utt->phones[i] = phone_voice_reduce (utt->phones[i]);
			    continue;
		       }
		    if (phone_is_noise_voiceless (utt->phones[i + 1]))
		       {
			    utt->phones[i] = phone_voice_reduce (utt->phones[i]);
	    	       }
		    continue;
    	      }
            if (phone_is_voiceless_reducable (utt->phones[i]))
	      {
		    if (utt->phones[i + 1] == PHONE_SYLBREAK && 
			phone_is_vv (utt->phones[i + 2]) &&
			phone_is_noise_voiced (utt->phones[i + 3]))
		       {
			    utt->phones[i] = phone_voice_reduce (utt->phones[i]);
			    continue;
		       }
		    if (phone_is_noise_voiced (utt->phones[i + 1]))
		      {
			    utt->phones[i] = phone_voice_reduce (utt->phones[i]);
		      }
    	     }
      }
}	/*utterance_reduce_voicing */

/*********************************************************
 *
 * utterance_lts:
 *
 * Main function which does lts conversion. It takes
 * utterance's text, split it on words and converts them
 * to the sequence of phones according to the LTS rules.
 * The stress is assigned either with dictionary or with 
 * CART tree. After that phones are concatenated into single 
 * sequence and vowels are reduced by predicted stress.
 *
 * @utt: utterance with text filled
 *
 ********************************************************/

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

    utterance_reduce_vowel (utt);
    utterance_reduce_voicing (utt);
}

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
