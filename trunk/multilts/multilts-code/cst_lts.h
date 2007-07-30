/*************************************************************************/
/*                                                                       */
/*  Letter to sound rules                                                */
/*                                                                       */
/*************************************************************************/
#ifndef _CST_LTS_H__
#define _CST_LTS_H__

#define CST_LTS_EOR 255

typedef struct _cst_cart_node {
    unsigned char  feat;
    unsigned  short no_index;
    unsigned  short value;
} cst_cart_node;


extern const cst_cart_node const cmu_cart_nodes[];
extern const int offsets[];
extern const char* const cmu_values[];

#define LETTER_PAU 1
#define LETTER_ZERO ('a' - 1)
#define PHONE_PAU 100

#endif

