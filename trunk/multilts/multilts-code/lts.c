#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "lts.h"
#include "Srv_Param.h"

#define DEBUG 0

#if DEBUG
static FILE* log_file;
#endif

static FILE* data_file;

static int data_file_nodes;
static int data_file_offsets;
static int data_file_values;

#define FEATURE_EOR 7
#define LETTER_PAU '#'
#define LETTER_ZERO '0'

#define STR_SIZE 256

typedef struct _cart_node {
    unsigned char feat;
    unsigned int value;
    unsigned char check;
} cart_node;

typedef struct value_node {
    unsigned char res;
    short score;
} value_node;

const char*  const phone_names [] = {
"zero",
"aa",
"ae",
"ah",
"ao",
"aw",
"ax",
"axh",
"axr",
"ay",
"b",
"bcl",
"ch",
"d",
"dcl",
"dh",
"dx",
"eh",
"el",
"em",
"en",
"epi",
"er",
"ey",
"f",
"g",
"gcl",
"hh",
"hv",
"ih",
"ix",
"iy",
"jh",
"k",
"kcl",
"l",
"m",
"n",
"ng",
"nx",
"ow",
"oy",
"p",
"pcl",
"q",
"r",
"s",
"sh",
"t",
"tcl",
"th",
"uh",
"uw",
"ux",
"v",
"w",
"y",
"z",
"zh",
};


const char value_names [][3] = {
{PHONE_AA, PHONE_ZERO},
{PHONE_AE, PHONE_ZERO},
{PHONE_AH, PHONE_ZERO},
{PHONE_AH, PHONE_S},
{PHONE_AH, PHONE_Z},
{PHONE_AO, PHONE_ZERO},
{PHONE_AW, PHONE_ZERO},
{PHONE_AXR, PHONE_ZERO},
{PHONE_AXR, PHONE_R},
{PHONE_AY, PHONE_ZERO},
{PHONE_BCL, PHONE_B, PHONE_ZERO},
{PHONE_TCL, PHONE_CH, PHONE_ZERO},
{PHONE_DCL, PHONE_D, PHONE_ZERO},
{PHONE_DH, PHONE_ZERO},
{PHONE_DX, PHONE_ZERO},
{PHONE_EH, PHONE_ZERO},
{PHONE_EH, PHONE_L},
{PHONE_EL, PHONE_ZERO},
{PHONE_EM, PHONE_ZERO},
{PHONE_EN, PHONE_ZERO},
{PHONE_ZERO, PHONE_ZERO},
{PHONE_ER, PHONE_ZERO},
{PHONE_EY, PHONE_ZERO},
{PHONE_F, PHONE_ZERO},
{PHONE_GCL, PHONE_G, PHONE_ZERO},
{PHONE_GCL, PHONE_G, PHONE_Z},
{PHONE_GCL, PHONE_G, PHONE_ZH},
{PHONE_HH, PHONE_ZERO},
{PHONE_IH, PHONE_ZERO},
{PHONE_IX, PHONE_ZERO},
{PHONE_JH, PHONE_ZERO},
{PHONE_KCL, PHONE_K, PHONE_ZERO},
{PHONE_KCL, PHONE_K, PHONE_S},
{PHONE_KCL, PHONE_K, PHONE_SH},
{PHONE_KCL, PHONE_K, PHONE_W},
{PHONE_L, PHONE_ZERO},
{PHONE_M, PHONE_ZERO},
{PHONE_M, PHONE_AH},
{PHONE_N, PHONE_ZERO},
{PHONE_NG, PHONE_ZERO},
{PHONE_OW, PHONE_ZERO},
{PHONE_OY, PHONE_ZERO},
{PHONE_PCL, PHONE_P, PHONE_ZERO},
{PHONE_R, PHONE_ZERO},
{PHONE_R, PHONE_IH},
{PHONE_R, PHONE_R},
{PHONE_S, PHONE_ZERO},
{PHONE_SH, PHONE_ZERO},
{PHONE_TCL, PHONE_T, PHONE_ZERO},
{PHONE_TH, PHONE_ZERO},
{PHONE_UH, PHONE_ZERO},
{PHONE_UX, PHONE_ZERO},
{PHONE_V, PHONE_ZERO},
{PHONE_W, PHONE_ZERO},
{PHONE_W, PHONE_AA},
{PHONE_W, PHONE_AE},
{PHONE_W, PHONE_AH},
{PHONE_Y, PHONE_ZERO},
{PHONE_Y, PHONE_AH},
{PHONE_Y, PHONE_AXR},
{PHONE_Y, PHONE_UH},
{PHONE_Y, PHONE_UX},
{PHONE_Z, PHONE_ZERO},
{PHONE_ZH, PHONE_ZERO},
};



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
    int  selections[STR_SIZE][LTS_NBEST];
    int  selections_buffer[STR_SIZE][LTS_NBEST];        
} utterance;

static int get_letter_index (utterance *utt, int i)
{
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
	    if (utt->text[i] > 'z' || utt->text[i] < 'A') {
		i++;
		continue;
	    }
		
	    utt->letters[j] = get_letter_index (utt, i);
	    i += utf8_length(utt->text + i);
	    j ++;
	    whitespace = 0;
	}
    if (whitespace == 0)
        utt->letters[j] = LETTER_PAU;
}

static unsigned int apply_model(unsigned char *vals,
			 	 unsigned int start)
{
    cart_node state;
    char buf[4];
    
    fseek (data_file, data_file_nodes + start * 4, SEEK_SET);
    fread (buf, 1, 4, data_file);
    state.check  = buf[3];
    state.feat = buf[0] & 0x7;
    state.value = (*(int *)buf & 0xffffff) >> 3;

    while (state.feat != FEATURE_EOR) {

#if DEBUG
	fprintf (log_file, "state is %d offset is %d check is %d\n", state.feat, start, state.check);
	fprintf (log_file, "Answer is %s\n", vals[state.feat] == state.check ? "yes" : "no");
#endif
	if (vals[state.feat] == state.check) {
	    start++;
	} else {
	    start = state.value;
	}

        fseek (data_file, data_file_nodes + start * 4, SEEK_SET);
        fread (buf, 1, 4, data_file);
	state.check  = buf[3];
        state.feat = buf[0] & 0x7;
	state.value = (*(int *)buf & 0xffffff) >> 3;
    }

    return state.value;
}

static unsigned int letter_start (utterance *utt, int i)
{
    int letter_offset;
    
    if (utt->letters[i] >= 'a') {
	fseek (data_file, data_file_offsets + (utt->letters[i] - 'a') * sizeof(int), SEEK_SET);
	fread (&letter_offset, 1, sizeof(int), data_file);
        return letter_offset;
    }
    
    return -1;
}

static void fill_feats (utterance *utt, int i,  unsigned char*vector)
{
    int j;

    for (j = 0; j < 7; j++) {
	vector[j] = LETTER_ZERO;
    }

    for (j = -1; j > -4; j--) {
        vector [j+4] = utt->letters[i+j];
        if (utt->letters[i+j] == LETTER_PAU)
	    break;		
    }
    
    for (j = 1; j < 4; j++) {	
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
	    	    continue;
	        }
    	    else 
		{
		    fill_feats (utt, i, feature_vector);
#if DEBUG
		    {
		      int k;
		      fprintf (log_file, "Vector is ");
		      for (k = 1; k < 7; k++)
		    	 fprintf (log_file, "%d ", feature_vector[k]);
		      fprintf (log_file, "letter is %d start index is %d\n", utt->letters[i], letter_start_index);
    		    }
#endif
		    phone = apply_model(feature_vector,
		    		        letter_start_index);
#if DEBUG
		    fprintf (log_file, "Result %d\n", phone);
#endif
		    utt->predictions[j] = phone;
	    	    j++;
	    	}
	}
    utt->predictions[j] = -1;
    j++;
}

static void utterance_select (utterance *utt)
{
    int i;
#if DEBUG
    int j;
#endif

    for (i = 0; utt->predictions[i] >= 0; i++) {
	    value_node val;
	    fseek (data_file, data_file_values + utt->predictions[i] * LTS_NBEST, SEEK_SET);
#if DEBUG
    	    
	    fprintf (log_file, "offset: %d values: ", utt->predictions[i]);
	    for (j = 0; j < LTS_NBEST; j++) {
		 fread (&val, 1, sizeof (value_node), data_file);
	         fprintf (log_file, "item %d score %d ", val.res, val.score);
	    }
	    fprintf (log_file, "\n");
#endif
	    fseek (data_file, data_file_values + utt->predictions[i] * LTS_NBEST * sizeof(value_node), SEEK_SET);
    	    fread (&val, 1, sizeof (value_node), data_file);
    	    utt->selections[i][0] = val.res;
    }

    {
        int k = 1;
      
        while (k < LTS_NBEST) {
            char best_phone;
            int  best_score;
            int  best_i;

	    best_score = -20000;
	    best_phone = 0;
	    best_i = -1;

	    for (i = 0; utt->predictions[i] >= 0; i++) {
		int j;
		value_node val;
	    
	        fseek (data_file, data_file_values + 
	    	                  utt->predictions[i] * LTS_NBEST * sizeof(value_node) 
	    	                  + sizeof(value_node), SEEK_SET); /* No need to check the best one */
		
		for (j = 1; j < LTS_NBEST; j++) {
			int check;
			int l;

	                fread (&val, 1, sizeof (value_node), data_file);    
	                if (val.res == 0)
	            	    break;
			
			check = 0;
			for (l = 0; l < k; l++)
			    if (val.res == utt->selections[i][l]) {
				check = 1;
				break;
			    }
			
			if (val.score > best_score && check == 0) {
			    best_phone = val.res;
			    best_score = val.score;
			    best_i = i;
			}
		}
    	    }    
#if DEBUG
	    fprintf (log_file, "Found best one at step %d offset %d phone %d \n", k, best_i, best_phone);
#endif
	    for (i = 0; utt->predictions[i] >= 0; i++) {
		utt->selections[i][k] = utt->selections[i][0];
	    }    
	    if (best_i >= 0)
    		utt->selections[best_i][k] = best_phone;
	    k++;
	}
    }
}

static void utterance_dump_buffer (utterance *utt, char **result)
{
    int i, j;
    
    for (j = 0; j < LTS_NBEST; j++) {
	result[j][0] = 0;
        for (i = 0; utt->predictions[i] >= 0; i++)
	     if (utt->selections[i][j] != PHONE_ZERO) {
		strncat (result[j], value_names[utt->selections[i][j] - 1], 3);
	}
    }
}

static void utterance_init (utterance *utt)
{
    memset (utt->letters, 0, STR_SIZE);
    memset (utt->predictions, 0, STR_SIZE);
    memset (utt->selections, 0, STR_SIZE * LTS_NBEST);    
}

/***************************************************************************/

void lts_init()
{	
    char tree_filename[STR_SIZE];

    ParamGetValue("G2P_Tree_File", PARAM_STRING, tree_filename);

    if (data_file == NULL) {
	    data_file = fopen(tree_filename, "r");
	    fread (&data_file_nodes, 1, sizeof(int), data_file);
	    fread (&data_file_values, 1, sizeof(int), data_file);
	    fread (&data_file_offsets, 1, sizeof(int), data_file);
#if DEBUG
	    log_file = fopen("g2p_log.txt","wt");
#endif
    }
}

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

void lts_dump_string (char *str)
{
    int i;
    
    for (i = 0; str[i] != 0; i++)
	if (str[i] < PHONE_LAST)
	    printf ("%s ", phone_names [(int)str[i]]);
    printf ("\n");
}

int lts_copy_string (char** phones, char *str)
{
    int i;
    int num_phon;
    num_phon = 0;
    
    for (i = 0; str[i] != 0; i++) {
	    if (str[i] < PHONE_LAST) {
	    	    strcpy(phones[i], phone_names[(int)str[i]]);
		    num_phon++;
	    }
    }
    return num_phon;
}

void lts_free()
{
#if DEBUG
    fclose(log_file);
#endif
    fclose(data_file);
    data_file = NULL;
}
