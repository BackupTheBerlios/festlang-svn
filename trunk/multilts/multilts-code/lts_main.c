#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cst_lts.h"

#define DEBUG 1

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
    char text[256];
    char letters[256];
    int  phones[256];

    const cst_cart_node const * cmu_cart_nodes;
    const int *offsets;
    const char* const *cmu_values;
    
} utterance;

int get_letter_index (utterance *utt, int i)
{
    int j;
    int result = LETTER_PAU;
    char buf[6];
    
    strncpy (buf, utt->text + i, utf8_length (utt->text + i));
    buf [utf8_length (utt->text + i)] = 0;

    result = utt->text[i];
#if DEBUG
    printf ("Letter is %s index is %d\n", buf, result);
#endif
     return result;
}

/***************************************************************************/
void utterance_parse (utterance *utt)
{
    int i, j, whitespace;
    
    utt->letters [0] = LETTER_PAU;
    i = 0; j = 1; whitespace = 1;
    
    while (utt->text[i] != 0 && i < 256)
	{
	    if (utt->text[i] == ' ' ||
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

static unsigned short apply_model(char *vals,
			 	 unsigned short start, 
				 const cst_cart_node* model)
{
    cst_cart_node state;

    state = model [start];

#if DEBUG
    printf ("state is %d offset is %d check is %d\n", start, state.feat, state.value);
#endif

    while (state.feat != CST_LTS_EOR)
    {
	if (vals[state.feat] == state.value)
	    start = state.no_index;
	else
	    start++;

        state = model [start];
#if DEBUG
	printf ("state is %d offset is %d check is %d\n", start, state.feat, state.value);
#endif
    }

    return state.value;
}

static unsigned short letter_start (utterance *utt, int i)
{
    if (utt->letters[i] > LETTER_ZERO)
        return utt->offsets[utt->letters[i] - LETTER_ZERO - 1];
    
    return -1;
}

static void fill_feats (utterance *utt, int i,  char*vector)
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

void utterance_lts (utterance *utt)
{
    int i, j;
    
    unsigned short phone;
    unsigned char  feature_vector[7];
    unsigned short   letter_start_index;
    
    for (i = 0, j = 0; utt->letters[i] != 0; i++)
	{
	    letter_start_index = letter_start (utt, i);
	    if (letter_start_index == (unsigned short) -1) 
	        {
	    	    utt->phones[j] = PHONE_PAU;
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
					utt->cmu_cart_nodes);	
#if DEBUG	
		    printf ("Result %d\n", phone);
#endif
		    utt->phones[j] = phone;
	    	    j++;
	    	}
	}
} /* utterance_lts */

/***************************************************************************/
void utterance_dump (utterance *utt)
{
    int i;
    
    for (i = 0; utt->phones[i] != 0; i++)
     if (utt->phones[i] == PHONE_PAU)
	printf ("#\n");
     else
	printf ("%s\n", utt->cmu_values[utt->phones[i]]);
    printf ("\n");
} /* utterance_dump */

/***************************************************************************/
void utterance_init (utterance *utt, char *language)
{
    memset (utt->letters, 0, 256);
    memset (utt->phones, 0, 256);

    utt->cmu_cart_nodes = cmu_cart_nodes;
    utt->offsets = offsets;
    utt->cmu_values = cmu_values;
    
} /* utterance_init */

/***************************************************************************/
int
main (int argc, char* argv[])
{
    utterance utt;

    FILE *f = NULL;    
    char *language = NULL;
    int i;
    
    for (i = 1; i < argc; i++)
       {
    	    if (strncmp("-l",argv[i],2) == 0)
    	      {
    	    	    language = argv[i+1];
    	    	    i++;
    	    	    continue;
    	      }

    	    if (strncmp("--help",argv[i],6) == 0)
    	      {
	    	    printf ("Usage: lts <language> [filename]");
	    	    return 1;
    	      }

       	    f = fopen (argv[i],"r");
	    if (!f) 
    	       {
		    printf ("File %s not found\n", argv[i]);
		    return 1;
	       }
      }	

  
    while (fgets (utt.text, 256, f ? f : stdin))
      {
	    utterance_init (&utt, language);
	    utterance_parse (&utt);
	    utterance_lts (&utt);
	    utterance_dump (&utt);
      }

}	/*main */
