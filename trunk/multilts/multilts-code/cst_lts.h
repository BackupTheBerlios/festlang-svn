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

extern const char * const cmu_lts_phone_table_english[];
extern const char * const cmu_lts_letter_table_english[];
extern const cst_lts_addr cmu_lts_letter_index_english[];
extern const cst_lts_model cmu_lts_model_english[];

extern const char * const cmu_lts_phone_table_spanish[];
extern const char * const cmu_lts_letter_table_spanish[];
extern const cst_lts_addr cmu_lts_letter_index_spanish[];
extern const cst_lts_model cmu_lts_model_spanish[];

extern const char * const cmu_lts_phone_table_russian[];
extern const char * const cmu_lts_letter_table_russian[];
extern const cst_lts_addr cmu_lts_letter_index_russian[];
extern const cst_lts_model cmu_lts_model_russian[];

#define LETTER_PAU 1
#define LETTER_ZERO 2
#define PHONE_PAU 100

#endif

