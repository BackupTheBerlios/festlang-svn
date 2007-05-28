#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cst_lts.h"

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
#define PHONE_LEN 10

typedef struct utterance
{
    char text[256];
    char letters[256][PHONE_LEN];
    char phones[256][PHONE_LEN];
} utterance;

/***************************************************************************/
void utterance_parse (utterance *utt)
{
    int i, j, whitespace;

    memset (utt->letters, 0, 256 * PHONE_LEN);
    memset (utt->phones, 0, 256 * PHONE_LEN);
    
    strncpy (utt->letters [0], "#", PHONE_LEN);
    i = 0; j = 1; whitespace = 1;
    
    while (utt->text[i] != 0 && i < 256)
	{
	    if (utt->text[i] == ' ' ||
		utt->text[i] == '\t' ||
		utt->text[i] == '\n') {
		i++;
		if (!whitespace) {
		        strncpy (utt->letters[j], "#", PHONE_LEN);
		        j++;
		        whitespace = 1;
		}
		continue;
	    }
		
	    strncpy (utt->letters[j], utt->text + i, utf8_length(utt->text + i));
	    i += utf8_length(utt->text + i);
	    j ++;
	    whitespace = 0;
	}
    if (whitespace = 0)
        strncpy (utt->letters[j], "#", PHONE_LEN);
} /* utterance_parse */

/***************************************************************************/

static void cst_lts_get_state(cst_lts_rule *state,
			      const cst_lts_model *model,
			      unsigned short n,
			      int rule_size)
{
    memmove(state,&model[n*rule_size],rule_size);
}

static cst_lts_phone apply_model(cst_lts_letter *vals,
			 	 cst_lts_addr start, 
				 const cst_lts_model* model)
{
    cst_lts_rule state;
    unsigned short nstate;
    static const int sizeof_cst_lts_rule = 6;

    cst_lts_get_state(&state,model,start,sizeof_cst_lts_rule);

#if DEBUG
    printf ("state is %d offset is %d check is %c\n", start, state.feat, state.val >= 'a' ? state.val : '0');
#endif

    while (state.feat != CST_LTS_EOR)
    {
	if (vals[state.feat] == state.val)
	    nstate = state.qtrue;
	else
	    nstate = state.qfalse;

        cst_lts_get_state(&state,model,nstate,sizeof_cst_lts_rule);
#if DEBUG
	printf ("state is %d offset is %d check is %c\n", nstate, state.feat, state.val >= 'a' ? state.val : '0');
#endif
    }

    return (cst_lts_phone)state.val;
}

static cst_lts_addr letter_start (char *letter)
{
    if (letter[0] >= 'a' && letter[0] <= 'z')
	    return cmu_lts_letter_index[letter[0] - 'a'];
    else 
	    return -1;
}

static void fill_feats (utterance *utt, int i, cst_lts_letter *vector)
{
   int j, end = 0;
   for (j = -3; j < 0; j++)
    {
	if (i + j < 0)
	    vector [j+4] = '0';
	else 
	    vector [j+4] = utt->letters[i+j][0];
    }
    
   for (j = 1; j < 4; j++)
    {	
	if (end)
	    vector [j+3] = '0';
	else 
	    vector [j+3] = utt->letters[i+j][0];
	    if (utt->letters[i+j][0] == '#')
		end = 1;
    }
    
    return;
}

void utterance_lts (utterance *utt)
{
    int i, j;
    
    cst_lts_phone  phone;
    cst_lts_letter feature_vector[7];
    cst_lts_addr   letter_start_index;
    
    for (i = 0, j = 0; utt->letters[i][0] != 0; i++)
	{
	    letter_start_index = letter_start (utt->letters[i]);
	    if (letter_start_index == (cst_lts_addr) -1) 
	        {
	    	    strncpy(utt->phones[j], "#", PHONE_LEN);
	    	    j++;
	        }
    	    else 
		{
		    fill_feats (utt, i, feature_vector);
#if DEBUG
		    printf ("Vector is %.7s start index is %d\n", feature_vector, letter_start_index);
#endif
		    phone = apply_model(feature_vector,
		    		        letter_start_index,
					cmu_lts_model);
	
		    if (strcmp (cmu_lts_phone_table[phone], "epsilon") == 0) {
			    continue;	
		    }
	    	    strncpy(utt->phones[j], cmu_lts_phone_table [phone], 
	    		    PHONE_LEN);
	    	    j++;
	    	}
	}
} /* utterance_lts */

/***************************************************************************/
void utterance_dump (utterance *utt)
{
    int i;
    
    for (i = 0; utt->phones[i][0] != 0; i++)
	printf ("%s ", utt->phones[i]);
    printf ("\n");
} /* utterance_dump */

/***************************************************************************/
int
main (int argc, char* argv[])
{
    utterance utt;
    FILE *f = NULL;
    
    if (argc == 2)
      {
  	    f = fopen (argv[1],"r");
    	    if (!f) 
    	       {
    		    printf ("File %s not found\n", argv[1]);
		    return 1;
    	    }
      }	
    if (argc > 2)
      {
    	    printf ("Usage: lts [filename]");
      }

    while (fgets (utt.text, 256, f ? f : stdin))
      {
	    utterance_parse (&utt);
	    utterance_lts (&utt);
	    utterance_dump (&utt);
      }

}	/*main */
