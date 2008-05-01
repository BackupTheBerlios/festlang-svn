#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lts.h"
#include "lts_int.h"

#define DEBUG 0

extern const char * phone_names[];
extern const char value_names[][2];
extern const cart_node const cart_nodes[];
extern const int offsets[];
extern const short const values[];

#define LETTER_EOR 255
#define LETTER_PAU '#'
#define LETTER_ZERO '0'

#define REG_SIZE 40
#define STR_SIZE 256

/*************************************************************************
 *
 * utf8 helper
 *
 *************************************************************************/

static const char utf8_skip_data[256] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};

#define utf8_length(p) (utf8_skip_data[*(unsigned char *)(p)])
#define utf8_next_char(p) (char *)((p) + utf8_skip_data[*(unsigned char *)(p)])


/***************************************************************************
 *
 * Utterance are basic chunks of processed text. They has all information
 * about text markup we made.
 *
 ***************************************************************************/

typedef struct utterance
{
    char text[STR_SIZE];
    char letters[STR_SIZE];
    
    int  predictions[STR_SIZE];

    int  phones[STR_SIZE][REG_SIZE];
    int  selections[STR_SIZE][LTS_NBEST];
    int  selections_buffer[STR_SIZE][LTS_NBEST];
        
} utterance;

static int get_letter_index (utterance *utt, int i)
{
    int j;
    int result = LETTER_PAU;

    result = tolower(utt->text[i]);

    return result;
}

/***************************************************************************/
static void utterance_parse (utterance *utt)
{
    int i, j, whitespace;
    
    utt->letters [0] = LETTER_PAU;
    i = 0; j = 1; whitespace = 1;
    
    while (utt->text[i] != 0 && i < STR_SIZE)
	{	
	    if (utt->text[i] == '-' ||
		utt->text[i] == '\''
		) {
		i++;
		continue;
	    }
	    if (utt->text[i] == ' ' ||
		utt->text[i] == '_' ||
		utt->text[i] == '\"' ||
		utt->text[i] == '\t' ||
		utt->text[i] == '\n') {
		i++;
		if (!whitespace) {
		        utt->letters[j] = LETTER_PAU;
		        j++;
		        whitespace = 1;
		}
		continue;
	    }
		
	    utt->letters[j] = get_letter_index (utt, i);
	    i += utf8_length(utt->text + i);
	    j ++;
	    whitespace = 0;
	}
    if (whitespace = 0)
        utt->letters[j] = LETTER_PAU;
} /* utterance_parse */

/***************************************************************************/

static unsigned int apply_model(unsigned char *vals,
			 	 unsigned int start, 
				 const cart_node* model)
{
    cart_node state;

    state = model [start];

    while (state.feat != LETTER_EOR)
    {

#if DEBUG
	printf ("state is %d offset is %d check is %d\n", state.feat, start, state.check);
	printf ("Answer is %s\n", vals[state.feat] == state.check ? "yes" : "no");
#endif
	if (vals[state.feat] == state.check) {
	    start++;
	} else {
	    start = state.value;
	}

        state = model [start];
    }

    return state.value;
}

static unsigned int letter_start (utterance *utt, int i)
{
    if (utt->letters[i] >= 'a')
        return offsets[utt->letters[i] - 'a'];
    
    return -1;
}

static void fill_feats (utterance *utt, int i,  unsigned char*vector)
{
   int j;

   for (j = 0; j < 7; j++)
    {
	vector[j] = LETTER_ZERO;
    }

   for (j = -1; j > -4; j--)
    {
        vector [j+4] = utt->letters[i+j];
        if (utt->letters[i+j] == LETTER_PAU)
	    break;	
    }
    
   for (j = 1; j < 4; j++)
    {	
        vector [j+3] = utt->letters[i+j];
        if (utt->letters[i+j] == LETTER_PAU)
	    break;
    }
    
    return;
}

static void utterance_lts (utterance *utt)
{
    int i, j;
    
    unsigned int phone;
    unsigned char  feature_vector[7];
    unsigned int   letter_start_index;
    
    for (i = 0, j = 0; utt->letters[i] != 0; i++)
	{
	    letter_start_index = letter_start (utt, i);
	    if (letter_start_index == (unsigned int) -1) 
	        {
	    	    utt->predictions[j] = PHONE_PAU;
	    	    j++;
	    	    continue;
	        }
    	    else 
		{
		    fill_feats (utt, i, feature_vector);
#if DEBUG
		    {
		      int k;
		      printf ("Vector is ");
		      for (k = 1; k < 7; k++)
		    	  printf ("%d ", feature_vector[k]);
		      printf ("letter is %d start index is %d\n", utt->letters[i], letter_start_index);
    		    }
#endif
		    phone = apply_model(feature_vector,
		    		        letter_start_index,
					cart_nodes);	
#if DEBUG	
		    printf ("Result %d\n", phone);
#endif
		    utt->predictions[j] = phone;
	    	    j++;
	    	}
	}
} /* utterance_lts */

/***************************************************************************/
static void utterance_select (utterance *utt)
{
    char *buf;
    int i;
    
    for (i = 0; utt->predictions[i] != 0; i++)
     if (utt->predictions[i] == PHONE_PAU)
	utt->phones[i][0] = PHONE_PAU;
     else {
        int j, k;
#if DEBUG
        printf ("%d ", utt->predictions[i]);
#endif
	for (j = utt->predictions[i], k = 0; values[j] < 100; j+=2,k+=2) {
	    utt->phones[i][k] = values[j] + 1;
	    utt->phones[i][k+1] = values[j+1];
#if DEBUG
    	    printf ("%s %d ", value_names[values[j] - 1], values[j+1]);
    	    printf ("%d %d ", values[j], values[j+1]); */
#endif	
	}
#if DEBUG
        printf ("\n");
#endif	
    }

    for (i = 0; utt->predictions[i] != 0; i++) {
	int j;
	int best = -20000;
	
	for (j = 0; utt->phones[i][j] != 0; j+=2) {
		if (utt->phones[i][j + 1] > best) {
		    utt->selections[i][0] = utt->phones[i][j];
		    best = utt->phones[i][j + 1];
		}
	    }
    }    
    
    {
        int k = 1;
      
        while (k < LTS_NBEST) {
            int best_first; int best_first_i; int best_first_j;

	    best_first = -20000;
	    best_first_i = -1;
	    best_first_j = -1;
	    for (i = 0; utt->predictions[i] != 0; i++) {
		int j;
	
		for (j = 0; utt->phones[i][j] != 0; j+=2) {
			int check;
			int l;
			
			check = 0;
			for (l = 0; l < k; l++)
			    if (utt->phones[i][j] == utt->selections[i][l]) {
				check = 1;
				break;
			    }
			
			if (utt->phones[i][j + 1] > best_first && check == 0) {
			    best_first_i = i;
			    best_first_j = j;
			    best_first = utt->phones[i][j + 1];
			}
		}
    	    }    
	    for (i = 0; utt->predictions[i] != 0; i++) {
		utt->selections[i][k] = utt->selections[i][0];
	    }    
#if DEBUG
	    printf ("%d %d\n", best_first_i, best_first_j);
#endif
	    if (best_first_i > 0)
    		utt->selections[best_first_i][k] = utt->phones[best_first_i][best_first_j];
	    k++;
	}
    }

} /* utterance_select */

/***************************************************************************/
static void utterance_dump_buffer (utterance *utt, char **result)
{
    int i, j;
    char buf[10];
    
    for (j = 0; j < LTS_NBEST; j++) {
	result[j][0] = 0;
        for (i = 0; utt->predictions[i] != 0; i++)
	     if (utt->selections[i][j] != PHONE_PAU) {
		strncat (result[j], value_names[utt->selections[i][j] - 2], 2);
	}
    }
} /* utterance_dump_buffer */

/***************************************************************************/
static void utterance_init (utterance *utt)
{
    memset (utt->letters, 0, STR_SIZE);
    memset (utt->predictions, 0, STR_SIZE);
    memset (utt->phones, 0, STR_SIZE * REG_SIZE);
    memset (utt->selections, 0, STR_SIZE * LTS_NBEST);
    
} /* utterance_init */

void lts (char *text, char **result)
{
    utterance utt;
    
    utterance_init (&utt);
    strncpy (utt.text, text, STR_SIZE);
    utterance_parse (&utt);
    utterance_lts (&utt);
    utterance_select (&utt);
    utterance_dump_buffer (&utt, result);    
}

void lts_dump_string (char *string)
{
    int i;
    
    for (i = 0; string[i] != 0; i++)
	if (string[i] < PHONE_LAST)
	    printf ("%s ", phone_names [string[i]]);
    printf ("\n");
}
