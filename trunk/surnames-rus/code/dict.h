#ifndef _DICT_H_
#define _DICT_H_

#define DICT_SIZE 999

typedef struct dict_node {
    char *word;
    int stress;
} dict_node;

extern const dict_node dict[];

#endif
