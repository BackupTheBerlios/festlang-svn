#ifndef CART_H
#define CART_H

enum
{
    CART_QUESTION_NUM2END_SMALLER,
    CART_QUESTION_SYLPOS_SMALLER,
    CART_QUESTION_PH_VFRONT,
    CART_QUESTION_PH_VHEIGHT,
    CART_QUESTION_PH_VRND,
    CART_QUESTION_P_VC,
    CART_QUESTION_P_VHEIGTH,
    CART_QUESTION_P_VFRONT,
    CART_QEUSTION_P_VRND,
    CART_QUESTION_P_CVOX,
    CART_QUESTION_P_CTYPE,
    CART_QUESTION_P_CPLACE,
    CART_QUESTION_P_CSOFT,
    CART_QUESTION_N_VC,
    CART_QUESTION_N_VHEIGTH,
    CART_QUESTION_N_VFRONT,
    CART_QEUSTION_N_VRND,
    CART_QUESTION_N_CVOX,
    CART_QUESTION_N_CTYPE,
    CART_QUESTION_N_CPLACE,
    CART_QUESTION_N_CSOFT,
    CART_QUESTION_POS,
    CART_VALUE
};

typedef struct cart_node
{
    int type:6;
    short no_index;

    float value_float;
    char *value_string;
} cart_node;


/**********************************************************************/
void dump_feat (char *phones, int index, int feat);

float find_stress_probability (char *phones, int index, int tree_index);

#endif
