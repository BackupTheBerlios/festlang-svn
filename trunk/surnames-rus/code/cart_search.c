#include "phoneset.h"
#include "cart_search.h"

#include <stdio.h>
#include <string.h>

extern const cart_node ru_stress_cart_nodes[];

/**********************************************************************
 *
 * get_last
 *
 * Gets the phone with offset relative to the end of word. It takes
 * into account that we can step over word boundaries. This function
 * should be certainly optimized for reentrancy.
 *
 * @phones: string of phones
 * @index: current phone index
 * @offset: relative offset
 *
 * Returns: phone at offset
 *
 **********************************************************************/
char get_last (char *phones, int index, int offset)
{
    int i;
    
    for (i = index; phones[i] != PHONE_PAU && phones[i] != PHONE_SYLBREAK; i++);
    
    if (i > offset)
	    return phones[i - offset];
	    
    return 0;
}       /*get_last*/

/**********************************************************************
 *
 * get_index
 *
 * Gets the phone with offset relative to the current phone. It takes
 * into account that we can step over word boundaries.
 *
 * @phones: string of phones
 * @index: current phone index
 * @offset: relative offset
 *
 * Returns: phone at offset
 *
 **********************************************************************/
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

/*********************************************************************
 * 
 * ask_question:
 *
 * ask a question about phone. The questions have different types. Usually
 * it's either distance from the word boundary or the name of phone in 
 * neighbourhood. 
 *
 *
 * @phones: string of phones
 * @index: index in string
 * @tree_index: tree node index to ask question
 * 
 * Returns: 1 if answer is yes 0 otherwise.
 *
 *********************************************************************/
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
          printf ("result is %d %d\n",get_ind (phones, index, 3),
			  ru_stress_cart_nodes[tree_index].value);
	  return (get_ind (phones, index, 3) ==
			  ru_stress_cart_nodes[tree_index].value);
	  break;
      default:
	  return 0;
      }
    return 0;
}	/*ask_question */

/***************************************************************************
 *
 * find_stress_probability:
 *
 * Finds probability of the stress at the given index with the CART tree.
 * This is recursive function that asks question in order to move from main
 * tree node to leafs.
 *
 * @phones: sequence of phones.
 * @index: index of vowel in the sequence. 
 * @tree_index: index of node in cart tree. Used in recursion.
 * Should be 0 for actual call.
 *
 * Returns: Probabilty of stress converted to integer with FLOAT_SCALE
 *
 **************************************************************************/
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
