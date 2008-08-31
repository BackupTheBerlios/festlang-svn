#ifndef LTS_INT_H
#define LTS_INT_H

/* This header is used in lts internally */

typedef struct _cart_node {
    unsigned char  feat : 3;
    unsigned int value : 21;
    unsigned char  check;
} cart_node;

typedef struct value_node {
    unsigned char res;
    short value;
} value_node;

#endif
