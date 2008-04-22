/************************************************************************************/
/* Copyright ?????????                                                              */
/* All rights reserved.                                                             */  
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR: Aby Louw                                                                 */
/* Email:                                                                           */
/* MODIFIED: Feb 10, 2004                                                           */
/*                                                                                  */
/************************************************************************************/

#include <stdio.h>
#include "festival.h"
#include "intsint.h"

void festival_INTSINT_Intonation_init(void)
{
    festival_def_utt_module("Intonation_INTSINT",Intonation_INTSINT_Utt,
    "(Intonation_INTSINT UTT)\n\
  This method predicts INTSINT labels for IntEvents on syllabels.\n\
  This module is called through the Intonation module.");
    festival_def_utt_module("Int_Targets_INTSINT",Int_Targets_INTSINT_Utt,
    "(Int_Targets_INTSINT UTT)\n\
  This method calculates a Momel curve from the predicted INTSINT labels for\n\
  IntEvents on syllabels. It then adds a f0 target to utt. This module is called\n\
  through the Int_Targets module.");
    init_subr_2("intsint::load_db",intsint_load_db,
    "(intsint:load_db PARAMS UTT_LIST)\n\
  Load all IntEvent relations for marking with intsint labels.");
    init_subr_0("intsint::isdb_list",isdb_list,
    "(intsint::isdb_list)\n\
  List names of currently loaded intsint databases.");
    init_subr_1("intsint::isdb_select",isdb_select,
    "(intsint::isdb_select DBNAME)\n\
  Select isdb name DBNAME and make it current.");
    init_subr_0("intsint::isdb_optimise",isdb_optimise,
    "(intsint::isdb_optimise)\n\
  Optimise labels for speaker's range.");
    init_subr_0("intsint::isdb_add_intsint_labels",isdb_add_intsint_labels,
    "(intsint::isdb_add_intsint_labels)\n\
  Add intsint labels to the IntEvent and Intonation relations of the utts.");
    init_subr_0("intsint::isdb_save_intsint_labels",isdb_save_intsint_labels,
    "(intsint::isdb_save_intsint_labels)\n\
  Save intsint labels to file in intsint_dir defined in voice file.");
  init_subr_0("intsint::isdb_save_intsint_utts",isdb_save_intsint_utts,
    "(intsint::isdb_save_intsint_utts)\n\
  Save utterances with the intsint labels to intsint_utt_dir defined in voice file.");
  init_subr_0("intsint::isdb_calc_intsint_momelqs",isdb_calc_intsint_momelqs,
    "(intsint::isdb_calc_intsint_momelqs)\n\
  Calculate and save the momel quadratic spline curve for the optimised intsint labels.");
}
    
