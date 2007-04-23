#include "phoneset.h"
#include "cart_search.h"

#include <stdio.h>
#include <string.h>

extern const cart_node ru_stress_cart_nodes[];

/**********************************************************************/
void
dump_feat (char *phones, int index, int feat)
{
#if DEBUG
    int i;
    int result;

    switch (feat)
      {
      case CART_QUESTION_SYLPOS_SMALLER:
	  result = 1;
	  for (i = index; i > 0 && phones[i] > PHONE_SYLBREAK; i--)
	    {
		if (is_vowel (phones[i]))
		    result++;
	    }
	  printf ("%d ", result);
	  break;
      case CART_QUESTION_NUM2END_SMALLER:
	  result = 0;
	  for (i = index; phones[i] > PHONE_SYLBREAK; i++)
	    {
		if (is_vowel (phones[i]))
		    result++;
	    }
	  printf ("%d ", result);
	  break;
      case CART_QUESTION_PH_VFRONT:
	  printf ("%c ", phoneset[phones[index] - 1].vfront);
	  break;
      case CART_QUESTION_PH_VHEIGHT:
	  printf ("%c ", phoneset[phones[index] - 1].vheight);
	  break;
      case CART_QUESTION_PH_VRND:
	  printf ("%c ", phoneset[phones[index] - 1].vrnd);
	  break;
      case CART_QUESTION_P_VC:
	  printf ("%c ", phoneset[phones[index - 1] - 1].vc);
	  break;
      case CART_QUESTION_P_VHEIGTH:
	  printf ("%c ", phoneset[phones[index - 1] - 1].vheight);
	  break;
      case CART_QUESTION_P_VFRONT:
	  printf ("%c ", phoneset[phones[index - 1] - 1].vfront);
	  break;
      case CART_QEUSTION_P_VRND:
	  printf ("%c ", phoneset[phones[index - 1] - 1].vrnd);
	  break;
      case CART_QUESTION_P_CVOX:
	  printf ("%c ", phoneset[phones[index - 1] - 1].cvox);
	  break;
      case CART_QUESTION_P_CTYPE:
	  printf ("%c ", phoneset[phones[index - 1] - 1].ctype);
	  break;
      case CART_QUESTION_P_CPLACE:
	  printf ("%c ", phoneset[phones[index - 1] - 1].cplace);
	  break;
      case CART_QUESTION_P_CSOFT:
	  printf ("%s ", phoneset[phones[index - 1] - 1].csoft);
	  break;
      case CART_QUESTION_N_VC:
	  printf ("%c ", phoneset[phones[index + 1] - 1].vc);
	  break;
      case CART_QUESTION_N_VHEIGTH:
	  printf ("%c ", phoneset[phones[index + 1] - 1].vheight);
	  break;
      case CART_QUESTION_N_VFRONT:
	  printf ("%c ", phoneset[phones[index + 1] - 1].vfront);
	  break;
      case CART_QEUSTION_N_VRND:
	  printf ("%c ", phoneset[phones[index + 1] - 1].vrnd);
	  break;
      case CART_QUESTION_N_CVOX:
	  printf ("%c ", phoneset[phones[index + 1] - 1].cvox);
	  break;
      case CART_QUESTION_N_CTYPE:
	  printf ("%c ", phoneset[phones[index + 1] - 1].ctype);
	  break;
      case CART_QUESTION_N_CPLACE:
	  printf ("%c ", phoneset[phones[index + 1] - 1].cplace);
	  break;
      case CART_QUESTION_N_CSOFT:
	  printf ("%c ", phoneset[phones[index + 1] - 1].csoft);
	  break;
      case CART_QUESTION_POS:
	  break;
      default:
	  break;
      }
    return;
#endif
}	/*dump_feat */


/**********************************************************************/
const char
get_pos (char *phones, int index)
{
    int pau_index;
    char result;

    pau_index = index;
    while (!is_pau (phones[pau_index]))
      {
	  pau_index++;
      }

    if (pau_index < 4)
      {
	  result = POS_NAME;
      }
    else if (phones[pau_index - 1] == PHONE_CH &&
	     phones[pau_index - 2] == PHONE_I &&
	     phones[pau_index - 3] == PHONE_VV)
      {
	  result = POS_SNAME;
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_N &&
	     phones[pau_index - 3] == PHONE_V)
      {
	  result = POS_SNAME;
      }
    else if (phones[pau_index - 1] == PHONE_V &&
	     phones[pau_index - 2] == PHONE_O)
      {
	  result = POS_SURNAME_OV;
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_V &&
	     phones[pau_index - 3] == PHONE_O)
      {
	  result = POS_SURNAME_OV;
      }
    else if (phones[pau_index - 1] == PHONE_V &&
	     phones[pau_index - 2] == PHONE_E)
      {
	  result = POS_SURNAME_EV;
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_V &&
	     phones[pau_index - 3] == PHONE_E)
      {
	  result = POS_SURNAME_EV;
      }
    else if (phones[pau_index - 1] == PHONE_C &&
	     phones[pau_index - 2] == PHONE_N &&
	     phones[pau_index - 3] == PHONE_A)
      {
	  result = POS_SURNAME_YAN;
      }
    else if (phones[pau_index - 1] == PHONE_N &&
	     phones[pau_index - 2] == PHONE_A)
      {
	  result = POS_SURNAME_YAN;
      }
    else if (phones[pau_index - 1] == PHONE_O &&
	     phones[pau_index - 2] == PHONE_K)
      {
	  result = POS_SURNAME_KO;
      }
    else if (phones[pau_index - 1] == PHONE_K &&
	     phones[pau_index - 2] == PHONE_U)
      {
	  result = POS_SURNAME_UK;
      }
    else if (phones[pau_index - 1] == PHONE_IJ &&
	     phones[pau_index - 2] == PHONE_I &&
	     phones[pau_index - 3] == PHONE_KK &&
	     phones[pau_index - 4] == PHONE_S || phones[pau_index - 4] == PHONE_C)
      {
	  result = POS_SURNAME_SKI;
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_IJ &&
	     phones[pau_index - 3] == PHONE_A &&
	     phones[pau_index - 4] == PHONE_K)
      {
	  result = POS_SURNAME_SKI;
      }
    else if (phones[pau_index - 1] == PHONE_H &&
	     (phones[pau_index - 2] == PHONE_I
	      || phones[pau_index - 2] == PHONE_Y))
      {
	  result = POS_SURNAME_IH;
      }
    else if (phones[pau_index - 1] == PHONE_N &&
	     (phones[pau_index - 2] == PHONE_I
	      || phones[pau_index - 2] == PHONE_Y))
      {
	  result = POS_SURNAME_IN;
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_N &&
	     (phones[pau_index - 3] == PHONE_I
	      || phones[pau_index - 3] == PHONE_Y))
      {
	  result = POS_SURNAME_IN;
      }
    else if (phones[pau_index - 1] == PHONE_K &&
	     phones[pau_index - 2] == PHONE_I)
      {
	  result = POS_SURNAME_IK;
      }
    else
      {
	  result = POS_NAME;
      }
#if DEBUG
    printf ("Predicted pos is %d\n", result);
#endif
    return result;
}	/*get_pos */

/**********************************************************************/
char get_last (char *phones, int index, int offset)
{
    int i;
    
    for (i = index; phones[i] != PHONE_PAU && phones[i] != PHONE_SYLBREAK; i++);
    
    if (i > offset)
	    return phones[i - offset];
	    
    return 0;
}       /*get_last*/

/**********************************************************************/
char get_ind (char *phones, int index, int offset)
{
    int i;
    
    for (i = 0; i <= offset; i++)
	if (is_pau (phones [index + i]))
	    return PHONE_PAU;
	
    if (index + offset > 0)
	    return phones[index + offset];
	    
    return 0;
}       /*get_ind*/

/**********************************************************************/
int
ask_question (char *phones, int index, int tree_index)
{
    int i;
    int result;

    switch (ru_stress_cart_nodes[tree_index].type)
      {
      case CART_QUESTION_SYLPOS_SMALLER:
	  result = 1;
	  for (i = index; i > 0 && phones[i] > PHONE_SYLBREAK; i--)
	    {
		if (is_vowel (phones[i]))
		    result++;
	    }
	  return result * FLOAT_SCALE < ru_stress_cart_nodes[tree_index].value;
      case CART_QUESTION_NUM2END_SMALLER:
	  result = 0;
	  for (i = index; phones[i] > PHONE_SYLBREAK; i++)
	    {
		if (is_vowel (phones[i]))
		    result++;
	    }
	  return result * FLOAT_SCALE < ru_stress_cart_nodes[tree_index].value;
      case CART_QUESTION_PH_VFRONT:
	  return (phoneset[phones[index] - 1].vfront ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_PH_VHEIGHT:
	  return (phoneset[phones[index] - 1].vheight ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_PH_VRND:
	  return (phoneset[phones[index] - 1].vrnd ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_VC:
	  return (phoneset[phones[index - 1] - 1].vc ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_VHEIGTH:
	  return (phoneset[phones[index - 1] - 1].vheight ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_VFRONT:
	  return (phoneset[phones[index - 1] - 1].vfront ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QEUSTION_P_VRND:
	  return (phoneset[phones[index - 1] - 1].vrnd ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_CVOX:
	  return (phoneset[phones[index - 1] - 1].cvox ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_CTYPE:
	  return (phoneset[phones[index - 1] - 1].ctype ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_CPLACE:
	  return (phoneset[phones[index - 1] - 1].cplace ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_P_CSOFT:
	  return (phoneset[phones[index - 1] - 1].csoft ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_VC:
	  return (phoneset[phones[index + 1] - 1].vc ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_VHEIGTH:
	  return (phoneset[phones[index + 1] - 1].vheight ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_VFRONT:
	  return (phoneset[phones[index + 1] - 1].vfront ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QEUSTION_N_VRND:
	  return (phoneset[phones[index + 1] - 1].vrnd ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_CVOX:
	  return (phoneset[phones[index + 1] - 1].cvox ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_CTYPE:
	  return (phoneset[phones[index + 1] - 1].ctype ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_CPLACE:
	  return (phoneset[phones[index + 1] - 1].cplace ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_N_CSOFT:
	  return (phoneset[phones[index + 1] - 1].csoft ==
			  ru_stress_cart_nodes[tree_index].value);
      case CART_QUESTION_POS:
	  return (get_pos (phones, index) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_LASTTTTT_NAME:
	  return (get_last (phones, index, 5) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_LASTTTT_NAME:
	  return (get_last (phones, index, 4) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_LASTTT_NAME:
	  return (get_last (phones, index,3) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_LASTT_NAME:
	  return (get_last (phones, index, 2) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_LAST_NAME:
	  return (get_last (phones, index, 1) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_P_NAME:
	  return (get_ind (phones, index, -1) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_NAME:
	  return (get_ind (phones, index, 0) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_N_NAME:
	  return (get_ind (phones, index, 1) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_NN_NAME:
	  return (get_ind (phones, index, 2) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      case CART_QUESTION_NNN_NAME:
	  return (get_ind (phones, index, 3) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      default:
	  return 0;
      }
    return 0;
}	/*ask_question */

/**********************************************************************/
char
find_stress_probability (char *phones, int index, int tree_index)
{
    if (ru_stress_cart_nodes[tree_index].type == CART_VALUE)
      {
	  return ru_stress_cart_nodes[tree_index].value;
      }

    if (ask_question (phones, index, tree_index))
      {
#if 1
          printf ("CART question %d answer is yes\n", ru_stress_cart_nodes[tree_index].type);
#endif
	  return find_stress_probability (phones, index, tree_index + 1);
      }
    else
      {
#if 1
          printf ("CART question %d answer is no\n", ru_stress_cart_nodes[tree_index].type);
#endif
	  return find_stress_probability (phones, index,
					  ru_stress_cart_nodes[tree_index].
					  no_index);
      }

    return 0;
}	/*find_stress_probability */
