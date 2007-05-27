/*************************************************************************/
/*                                                                       */
/*  Letter to sound rules                                                */
/*                                                                       */
/*************************************************************************/
#ifndef _CST_LTS_H__
#define _CST_LTS_H__

#define CST_LTS_EOR 255

typedef unsigned short cst_lts_addr;
typedef unsigned char  cst_lts_feat;
typedef unsigned char  cst_lts_letter;
typedef unsigned char  cst_lts_phone;
typedef unsigned char  cst_lts_model;

typedef struct cst_lts_rule_struct {
    cst_lts_feat   feat;
    cst_lts_letter val;
    cst_lts_addr   qtrue;
    cst_lts_addr   qfalse;
} cst_lts_rule;

extern const char * const cmu_lts_phone_table[];
extern const char * const cmu_lts_letter_table[];
extern const cst_lts_addr cmu_lts_letter_index[];
extern const cst_lts_model cmu_lts_model[];

#endif

