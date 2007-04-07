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
	  printf ("%s ", phoneset[phones[index] - 1].vfront);
	  break;
      case CART_QUESTION_PH_VHEIGHT:
	  printf ("%s ", phoneset[phones[index] - 1].vheight);
	  break;
      case CART_QUESTION_PH_VRND:
	  printf ("%s ", phoneset[phones[index] - 1].vrnd);
	  break;
      case CART_QUESTION_P_VC:
	  printf ("%s ", phoneset[phones[index - 1] - 1].vc);
	  break;
      case CART_QUESTION_P_VHEIGTH:
	  printf ("%s ", phoneset[phones[index - 1] - 1].vheight);
	  break;
      case CART_QUESTION_P_VFRONT:
	  printf ("%s ", phoneset[phones[index - 1] - 1].vfront);
	  break;
      case CART_QEUSTION_P_VRND:
	  printf ("%s ", phoneset[phones[index - 1] - 1].vrnd);
	  break;
      case CART_QUESTION_P_CVOX:
	  printf ("%s ", phoneset[phones[index - 1] - 1].cvox);
	  break;
      case CART_QUESTION_P_CTYPE:
	  printf ("%s ", phoneset[phones[index - 1] - 1].ctype);
	  break;
      case CART_QUESTION_P_CPLACE:
	  printf ("%s ", phoneset[phones[index - 1] - 1].cplace);
	  break;
      case CART_QUESTION_P_CSOFT:
	  printf ("%s ", phoneset[phones[index - 1] - 1].csoft);
	  break;
      case CART_QUESTION_N_VC:
	  printf ("%s ", phoneset[phones[index + 1] - 1].vc);
	  break;
      case CART_QUESTION_N_VHEIGTH:
	  printf ("%s ", phoneset[phones[index + 1] - 1].vheight);
	  break;
      case CART_QUESTION_N_VFRONT:
	  printf ("%s ", phoneset[phones[index + 1] - 1].vfront);
	  break;
      case CART_QEUSTION_N_VRND:
	  printf ("%s ", phoneset[phones[index + 1] - 1].vrnd);
	  break;
      case CART_QUESTION_N_CVOX:
	  printf ("%s ", phoneset[phones[index + 1] - 1].cvox);
	  break;
      case CART_QUESTION_N_CTYPE:
	  printf ("%s ", phoneset[phones[index + 1] - 1].ctype);
	  break;
      case CART_QUESTION_N_CPLACE:
	  printf ("%s ", phoneset[phones[index + 1] - 1].cplace);
	  break;
      case CART_QUESTION_N_CSOFT:
	  printf ("%s ", phoneset[phones[index + 1] - 1].csoft);
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
const char *
get_pos (char *phones, int index)
{
    int pau_index;
    const char *result;

    pau_index = index;
    while (!is_pau (phones[pau_index]))
      {
	  pau_index++;
      }

    if (pau_index < 4)
      {
	  result = "name";
      }
    else if (phones[pau_index - 1] == PHONE_CH &&
	     phones[pau_index - 2] == PHONE_I &&
	     phones[pau_index - 3] == PHONE_VV)
      {
	  result = "sname";
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_N &&
	     phones[pau_index - 3] == PHONE_V)
      {
	  result = "sname";
      }
    else if (phones[pau_index - 1] == PHONE_V &&
	     (phones[pau_index - 2] == PHONE_O
	      || phones[pau_index - 2] == PHONE_E))
      {
	  result = "surname-ovev";
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_V)
      {
	  result = "surname-ovev";
      }
    else if (phones[pau_index - 1] == PHONE_C &&
	     phones[pau_index - 2] == PHONE_N &&
	     phones[pau_index - 3] == PHONE_A)
      {
	  result = "surname-yan";
      }
    else if (phones[pau_index - 1] == PHONE_N &&
	     phones[pau_index - 2] == PHONE_A)
      {
	  result = "surname-yan";
      }
    else if (phones[pau_index - 1] == PHONE_O &&
	     phones[pau_index - 2] == PHONE_K)
      {
	  result = "surname-ko";
      }
    else if (phones[pau_index - 1] == PHONE_K &&
	     phones[pau_index - 2] == PHONE_U)
      {
	  result = "surname-uk";
      }
    else if (phones[pau_index - 1] == PHONE_IJ &&
	     phones[pau_index - 2] == PHONE_I &&
	     phones[pau_index - 3] == PHONE_KK)
      {
	  result = "surname-ski";
      }
    else if (phones[pau_index - 1] == PHONE_A &&
	     phones[pau_index - 2] == PHONE_IJ &&
	     phones[pau_index - 3] == PHONE_A)
      {
	  result = "surname-ski";
      }
    else if (phones[pau_index - 1] == PHONE_H &&
	     (phones[pau_index - 2] == PHONE_I
	      || phones[pau_index - 2] == PHONE_Y))
      {
	  result = "surname-ih";
      }
    else if (phones[pau_index - 1] == PHONE_N &&
	     (phones[pau_index - 2] == PHONE_I
	      || phones[pau_index - 2] == PHONE_Y))
      {
	  result = "surname-in";
      }
    else
      {
	  result = "name";
      }

#if DEBUG
    printf ("pos is %s\n", result);
#endif

    return result;
}	/*get_pos */

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
	  return result < ru_stress_cart_nodes[tree_index].value_float;
      case CART_QUESTION_NUM2END_SMALLER:
	  result = 0;
	  for (i = index; phones[i] > PHONE_SYLBREAK; i++)
	    {
		if (is_vowel (phones[i]))
		    result++;
	    }
	  return result < ru_stress_cart_nodes[tree_index].value_float;
      case CART_QUESTION_PH_VFRONT:
	  return (strcmp (phoneset[phones[index] - 1].vfront,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_PH_VHEIGHT:
	  return (strcmp (phoneset[phones[index] - 1].vheight,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_PH_VRND:
	  return (strcmp (phoneset[phones[index] - 1].vrnd,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_VC:
	  return (strcmp (phoneset[phones[index - 1] - 1].vc,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_VHEIGTH:
	  return (strcmp (phoneset[phones[index - 1] - 1].vheight,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_VFRONT:
	  return (strcmp (phoneset[phones[index - 1] - 1].vfront,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QEUSTION_P_VRND:
	  return (strcmp (phoneset[phones[index - 1] - 1].vrnd,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_CVOX:
	  return (strcmp (phoneset[phones[index - 1] - 1].cvox,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_CTYPE:
	  return (strcmp (phoneset[phones[index - 1] - 1].ctype,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_CPLACE:
	  return (strcmp (phoneset[phones[index - 1] - 1].cplace,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_P_CSOFT:
	  return (strcmp (phoneset[phones[index - 1] - 1].csoft,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_VC:
	  return (strcmp (phoneset[phones[index + 1] - 1].vc,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_VHEIGTH:
	  return (strcmp (phoneset[phones[index + 1] - 1].vheight,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_VFRONT:
	  return (strcmp (phoneset[phones[index + 1] - 1].vfront,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QEUSTION_N_VRND:
	  return (strcmp (phoneset[phones[index + 1] - 1].vrnd,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_CVOX:
	  return (strcmp (phoneset[phones[index + 1] - 1].cvox,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_CTYPE:
	  return (strcmp (phoneset[phones[index + 1] - 1].ctype,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_CPLACE:
	  return (strcmp (phoneset[phones[index + 1] - 1].cplace,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_N_CSOFT:
	  return (strcmp (phoneset[phones[index + 1] - 1].csoft,
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
      case CART_QUESTION_POS:
	  return (strcmp (get_pos (phones, index),
			  ru_stress_cart_nodes[tree_index].value_string) ==
		  0);
	  break;
      default:
	  return 0;
      }
    return 0;
}	/*ask_question */

/**********************************************************************/
float
find_stress_probability (char *phones, int index, int tree_index)
{
    if (ru_stress_cart_nodes[tree_index].type == CART_VALUE)
      {
	  return (float) ru_stress_cart_nodes[tree_index].value_float;
      }

    if (ask_question (phones, index, tree_index))
      {
	  return find_stress_probability (phones, index, tree_index + 1);
      }
    else
      {
	  return find_stress_probability (phones, index,
					  ru_stress_cart_nodes[tree_index].
					  no_index);
      }

    return 0.0;
}	/*find_stress_probability */
