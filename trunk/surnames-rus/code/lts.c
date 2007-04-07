#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#include "phoneset.h"
#include "cart_search.h"

typedef struct utterance
{
	char text[256];
	char phones[256];
	char stress[256];
} utterance;

typedef struct lts_rule
{
	char **predessors;
	char *letter;
	char **ancessors;
	char phone;
	char second_phone;
} lts_rule;

char *startsyl[] = {
     "#", "ъ", "ь", "а", "я", "о", "ё", "у", "ю", "э", "е", "и", NULL
};

char *softletters[] = {
    "я", "ё", "ю", "и", "ь", "е", NULL
};

char *softsign[] = {
    "ь", NULL
};


lts_rule lts_ruleset[] = {
	{NULL, " ", NULL, PHONE_SYLBREAK},

	{NULL, "а", NULL, PHONE_A, 0},
	{NULL, "э", NULL, PHONE_E, 0},
	{softsign, "о", NULL, PHONE_IJ, PHONE_O},
	{NULL, "о", NULL, PHONE_O, 0},
	{NULL, "у", NULL, PHONE_U, 0},
	{NULL, "ы", NULL, PHONE_Y, 0},

	{startsyl,  "ю", NULL, PHONE_IJ, PHONE_U},
	{startsyl,  "я", NULL, PHONE_IJ, PHONE_A},
	{startsyl,  "ё", NULL, PHONE_IJ, PHONE_O},
	{startsyl,  "е", NULL, PHONE_IJ, PHONE_E},

	{NULL, "ё", NULL, PHONE_O, 0},
	{NULL, "е", NULL, PHONE_E, 0},
	{NULL, "я", NULL, PHONE_A, 0},
	{NULL, "ю", NULL, PHONE_U, 0},
	{NULL, "и", NULL, PHONE_I, 0},

	{NULL, "б", softletters, PHONE_BB, 0},
	{NULL, "в", softletters, PHONE_VV, 0},
	{NULL, "г", softletters, PHONE_GG, 0},
	{NULL, "д", softletters, PHONE_DD, 0},
	{NULL, "з", softletters, PHONE_ZZ, 0},
	{NULL, "к", softletters, PHONE_KK, 0},
	{NULL, "л", softletters, PHONE_LL, 0},
	{NULL, "м", softletters, PHONE_MM, 0},
	{NULL, "н", softletters, PHONE_NN, 0},
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
	{NULL, "к", NULL, PHONE_K, 0},
	{NULL, "л", NULL, PHONE_L, 0},
	{NULL, "м", NULL, PHONE_M, 0},
	{NULL, "н", NULL, PHONE_N, 0},
	{NULL, "п", NULL, PHONE_P, 0},
	{NULL, "р", NULL, PHONE_R, 0},
	{NULL, "с", NULL, PHONE_S, 0},
	{NULL, "т", NULL, PHONE_T, 0},
	{NULL, "ф", NULL, PHONE_F, 0},
	{NULL, "х", NULL, PHONE_H, 0},
	{NULL, "ц", NULL, PHONE_C, 0},
	{NULL, "ч", NULL, PHONE_CH, 0},
	{NULL, "ш", NULL, PHONE_SH, 0},
	{NULL, "щ", NULL, PHONE_SCH, 0},
	{NULL, "й", NULL, PHONE_IJ, 0},

	{NULL, "-", NULL, 0, 0},
	{NULL, "ъ", NULL, 0, 0},
	{NULL, "ь", NULL, 0, 0},

	{NULL, NULL, NULL, 0}
};

int
check_context (char *text, int text_idx, int i)
{
	int j, len;
	if (strncmp (text + text_idx,
		     lts_ruleset[i].letter,
		     strlen (lts_ruleset[i].letter)) != 0)
		return 0;

	if (lts_ruleset[i].ancessors == NULL &&
	    lts_ruleset[i].predessors == NULL)
		return 1;

	len = strlen (lts_ruleset[i].letter);
	
	if (lts_ruleset[i].ancessors != NULL) {
	    for (j = 0; lts_ruleset[i].ancessors[j] != 0; j++) {
		    if (strncmp (text + text_idx + len, lts_ruleset[i].ancessors[j], 
				 strlen (lts_ruleset[i].ancessors[j])) == 0)
			return 1;
		}
	}

	if (lts_ruleset[i].predessors != NULL) {
	    for (j = 0; lts_ruleset[i].predessors[j] != 0; j++) {
		    
		    if (lts_ruleset[i].predessors[j][0] == '#' &&
			(text_idx <= 0 || text[text_idx-1] == ' '))
			    return 1;
		    
		    if (text_idx < strlen(lts_ruleset[i].predessors[j])) 
			    continue;
		    if (strncmp (text + text_idx - strlen (lts_ruleset[i].predessors[j]), 
				 lts_ruleset[i].predessors[j], 
				 strlen (lts_ruleset[i].predessors[j])) == 0)
			return 1;

		}
	}
	return 0;
}

void
utterance_lts (utterance * utt)
{
	char *text = utt->text;
	char *phones = utt->phones;

	int text_idx;
	int phones_idx;
	int i, found_len;

	phones_idx = 0;
	phones[phones_idx++] = PHONE_PAU;

	for (text_idx = 0; text[text_idx] != 0;) {
		found_len = 1;
		for (i = 0; lts_ruleset[i].letter != 0; i++) {
			if (check_context (text, text_idx, i)) {

				if (lts_ruleset[i].phone != 0) {
					phones[phones_idx++] = lts_ruleset[i].phone;
					if (lts_ruleset[i].second_phone != 0) {
						phones[phones_idx++] = lts_ruleset[i].second_phone;
					}
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
}

void
utterance_stress (utterance * utt)
{
	int i, last_index;
	float min_probability, probability;
		
	for (i = 0; utt->phones[i] != 0; i++) {
		if (is_vowel (utt->phones[i])) {
		    probability = find_stress_probability (utt->phones, i, 0);
		    if (probability < min_probability) {
			    min_probability = probability;
			    last_index = i;
		    }
		}
		utt->stress[i] = 0;
		if (is_pau (utt->phones[i])) {
		    if (last_index >= 0) {
			    utt->stress[last_index] = 1;
		    }
		    last_index = -1;
		    min_probability = 2.0;
		}
	}
}

int
next_is_stressed (utterance *utt, int i)
{
    int j;
    
    for (j = i + 1; utt->phones[j] != 0; j++) {
	if (is_vowel (utt->phones[j])) 
	    return utt->stress[j];
    }
    return 1;
}

void
utterance_reduce (utterance *utt)
{
	int i;
	
	for (i = 1; utt->phones[i] != 0; i++) {
		if ((utt->phones[i] == PHONE_A || utt->phones[i] == PHONE_O) && utt->stress[i] == 0 && next_is_stressed (utt, i)) {
			utt->phones[i] = PHONE_AO;
			continue;
		}
		if ((utt->phones[i] == PHONE_E || utt->phones[i] == PHONE_I) && utt->stress[i] == 0 && next_is_stressed (utt, i)) {
			utt->phones[i] = PHONE_EI;
			continue;
		}
		if ((utt->phones[i] == PHONE_O || 
		     utt->phones[i] == PHONE_E || 
		     utt->phones[i] == PHONE_I || 
		     utt->phones[i] == PHONE_A) && utt->stress[i] == 0)
			utt->phones[i] = PHONE_AE;
	}
}

int
main ()
{
	utterance utt;

	while (fgets (utt.text, 256, stdin)) {

		utterance_lts (&utt);
		utterance_stress (&utt);
		utterance_reduce (&utt);

		dump_phones (utt.phones);
	}
}
