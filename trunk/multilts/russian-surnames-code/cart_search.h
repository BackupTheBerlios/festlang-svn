#ifndef CART_H
#define CART_H

#define FLOAT_SCALE 40

typedef enum
{
    CART_QUESTION_NUM2END_SMALLER,
    CART_QUESTION_SYLPOS_SMALLER,
    CART_QUESTION_LASTTTTT_NAME,
    CART_QUESTION_LASTTTT_NAME,
    CART_QUESTION_LASTTT_NAME,
    CART_QUESTION_LASTT_NAME,
    CART_QUESTION_LAST_NAME,
    CART_QUESTION_P_NAME,
    CART_QUESTION_NAME,
    CART_QUESTION_N_NAME,
    CART_QUESTION_NN_NAME,
    CART_QUESTION_NNN_NAME,
    CART_VALUE
} cart_question_type;

typedef struct cart_node
{
    int type : 6;
    int no_index : 18;
    unsigned char value;
} cart_node;

char find_stress_probability (char *phones, int index, int tree_index);

#endif
