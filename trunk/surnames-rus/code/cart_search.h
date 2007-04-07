#ifndef CART_H
#define CART_H

#define FLOAT_SCALE 40

enum {
    POS_NAME = 1,
    POS_SNAME,
    POS_SURNAME_OVEV,
    POS_SURNAME_YAN,
    POS_SURNAME_KO,
    POS_SURNAME_UK,
    POS_SURNAME_SKI,
    POS_SURNAME_IH,
    POS_SURNAME_IN,
};

typedef enum
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
} cart_question_type;

typedef struct cart_node
{
    int type : 6;
    int no_index : 18;
    char value;
} cart_node;

/**********************************************************************/

void dump_feat (char *phones, int index, int feat);

char find_stress_probability (char *phones, int index, int tree_index);

#endif
