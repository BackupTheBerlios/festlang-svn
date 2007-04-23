#include "phoneset.h"
#include "cart_search.h"

#include <stdio.h>
#include <string.h>

extern const cart_node ru_stress_cart_nodes[];

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
#if DEBUG
          printf ("CART question %d answer is yes\n", ru_stress_cart_nodes[tree_index].type);
#endif
	  return find_stress_probability (phones, index, tree_index + 1);
      }
    else
      {
#if DEBUG
          printf ("CART question %d answer is no\n", ru_stress_cart_nodes[tree_index].type);
#endif
	  return find_stress_probability (phones, index,
					  ru_stress_cart_nodes[tree_index].
					  no_index);
      }

    return 0;
}	/*find_stress_probability */
