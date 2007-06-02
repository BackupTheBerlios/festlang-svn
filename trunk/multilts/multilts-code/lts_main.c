#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cst_lts.h"

#define DEBUG 0

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
    char phones[256];
    
    const char * const * phone_table;
    const char * const * letter_table;
    const cst_lts_addr * letter_index;
    const cst_lts_model * model;
} utterance;

int get_letter_index (utterance *utt, int i)
{
    int j;
    int result = LETTER_PAU;
    char buf[6];
    
    strncpy (buf, utt->text + i, utf8_length (utt->text + i));
    buf [utf8_length (utt->text + i)] = 0;

    for (j = 0; utt->letter_table[j] != NULL; j++)
      {
    	    if (strcmp (utt->letter_table [j], buf) == 0) 
    	      {
    	    	    result = j;
    	    	    break;
    	      }
      }
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
    printf ("state is %d offset is %d check is %d\n", start, state.feat, state.val);
#endif

    while (state.feat != CST_LTS_EOR)
    {
	if (vals[state.feat] == state.val)
	    nstate = state.qtrue;
	else
	    nstate = state.qfalse;

        cst_lts_get_state(&state,model,nstate,sizeof_cst_lts_rule);
#if DEBUG
	printf ("state is %d offset is %d check is %d\n", nstate, state.feat, state.val);
#endif
    }

    return (cst_lts_phone)state.val;
}

static cst_lts_addr letter_start (utterance *utt, int i)
{
    if (utt->letters[i] > LETTER_ZERO)
        return utt->letter_index[utt->letters[i] - LETTER_ZERO - 1];
    
    return -1;
}

static void fill_feats (utterance *utt, int i, cst_lts_letter *vector)
{
   int j;

   for (j = 0; j < 7; j++)
    {
	vector[j] = LETTER_ZERO;
    }

   for (j = -3; j < 0; j++)
    {
	if (i + j >= 0)
	    vector [j+4] = utt->letters[i+j];
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
    
    cst_lts_phone  phone;
    cst_lts_letter feature_vector[7];
    cst_lts_addr   letter_start_index;
    
    for (i = 0, j = 0; utt->letters[i] != 0; i++)
	{
	    letter_start_index = letter_start (utt, i);
	    if (letter_start_index == (cst_lts_addr) -1) 
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
					utt->model);	
#if DEBUG	
		    printf ("Result %d\n", phone);
#endif
		    if (strcmp (utt->phone_table[phone], "epsilon") == 0) {
			    continue;	
		    }
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
	printf ("# ");
     else
	printf ("%s ", utt->phone_table[utt->phones[i]]);
    printf ("\n");
} /* utterance_dump */

/***************************************************************************/
void utterance_init (utterance *utt, char *language)
{
    memset (utt->letters, 0, 256);
    memset (utt->phones, 0, 256);

    if (language && strcmp (language, "russian"))
      {
        utt->phone_table = cmu_lts_phone_table_russian;
	utt->letter_table = cmu_lts_letter_table_russian;
        utt->letter_index = cmu_lts_letter_index_russian;
        utt->model = cmu_lts_model_english;
        return;
      }
    
    if (language && strcmp (language, "spanish"))
      {
        utt->phone_table = cmu_lts_phone_table_spanish;
        utt->letter_table = cmu_lts_letter_table_spanish;
        utt->letter_index = cmu_lts_letter_index_spanish;
        utt->model = cmu_lts_model_spanish;
        return;
      }
    
    utt->phone_table = cmu_lts_phone_table_english;
    utt->letter_table = cmu_lts_letter_table_english;
    utt->letter_index = cmu_lts_letter_index_english;
    utt->model = cmu_lts_model_english;
    
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
