/* ----------------------------------------------------------------- */
/*           The HMM-Based Speech Synthesis System (HTS)             */
/*           hts_engine API developed by HTS Working Group           */
/*           http://hts-engine.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2001-2008  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/*                2001-2008  Tokyo Institute of Technology           */
/*                           Interdisciplinary Graduate School of    */
/*                           Science and Engineering                 */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */
/*  ---------------------------------------------------------------  */
/*  Modified by Sergio Oller (sergioller@gmail.com)                  */
/*  September 2009                                                   */
/*  Make it compile with c++ and integrate as a Festival module      */
/*  ---------------------------------------------------------------  */

#include <cstdarg>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "festival.h"
#include "mlsa_resynthesis.h"
#include "HTS_engine.h"

using namespace std;

/* Usage: output usage */
void Usage(void)
{
   HTS_show_copyright(stderr);
   fprintf(stderr, "\n");
   fprintf(stderr, "hts_engine - An HMM-based speech synthesis engine\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "  usage:\n");
   fprintf(stderr, "       hts_engine [ options ] [ infile ] \n");
   fprintf(stderr,
           "  options:                                                                   [  def][ min--max]\n");
   fprintf(stderr,
           "    -td tree       : decision trees file for state duration                  [  N/A]\n");
   fprintf(stderr,
           "    -tf tree       : decision trees file for Log F0                          [  N/A]\n");
   fprintf(stderr,
           "    -tm tree       : decision trees file for spectrum                        [  N/A]\n");
   fprintf(stderr,
           "    -md pdf        : model file for state duration                           [  N/A]\n");
   fprintf(stderr,
           "    -mf pdf        : model file for Log F0                                   [  N/A]\n");
   fprintf(stderr,
           "    -mm pdf        : model file for spectrum                                 [  N/A]\n");
   fprintf(stderr,
           "    -df win        : window file for calculation delta of Log F0             [  N/A]\n");
   fprintf(stderr,
           "    -dm win        : filename of delta coeffs for spectrum                   [  N/A]\n");
   fprintf(stderr,
           "    -db            : regard window files of delta coeffs as binary floats    [  N/A]\n");
   fprintf(stderr,
           "    -od s          : filename of output label with duration                  [  N/A]\n");
   fprintf(stderr,
           "    -of s          : filename of output Log F0                               [  N/A]\n");
   fprintf(stderr,
           "    -om s          : filename of output spectrum                             [  N/A]\n");
   fprintf(stderr,
           "    -or s          : filename of output raw audio (generated speech)         [  N/A]\n");
   fprintf(stderr,
           "    -ow s          : filename of output wav audio (generated speech)         [  N/A]\n");
   fprintf(stderr,
           "    -ot s          : filename of output trace information                    [  N/A]\n");
   fprintf(stderr,
           "    -vp            : use phoneme alignment for duration                      [  N/A]\n");
   fprintf(stderr,
           "    -i  i f1 .. fi : enable interpolation & specify number(i),coefficient(f) [    1][   1-- ]\n");
   fprintf(stderr,
           "    -s  i          : sampling frequency                                      [16000][   1--48000]\n");
   fprintf(stderr,
           "    -p  i          : frame period (point)                                    [   80][   1--]\n");
   fprintf(stderr,
           "    -a  f          : all-pass constant                                       [ 0.42][ 0.0--1.0]\n");
   fprintf(stderr,
           "    -g  i          : gamma = -1 / i (if i=0 then gamma=0)                    [    0][   0-- ]\n");
   fprintf(stderr,
           "    -b  f          : postfiltering coefficient                               [  0.0][-0.8--0.8]\n");
   fprintf(stderr,
           "    -l             : regard input as log gain and output linear one (LSP)    [  N/A]\n");
   fprintf(stderr,
           "    -r  f          : speech speed rate                                       [  1.0][ 0.0--10.0]\n");
   fprintf(stderr,
           "    -fs f          : multilply F0                                            [  1.0][ 0.1--1.9]\n");
   fprintf(stderr,
           "    -fm f          : add F0             f                                    [  0.0][-200.0--200.0]\n");
   fprintf(stderr,
           "    -u  f          : voiced/unvoiced threshold                               [  0.5][ 0.0--1.0]\n");
   fprintf(stderr,
           "    -cf pdf        : filename of GV for Log F0                               [  N/A]\n");
   fprintf(stderr,
           "    -cm pdf        : filename of GV for spectrum                             [  N/A]\n");
   fprintf(stderr,
           "    -jf f          : weight of GV for Log F0                                 [  0.7][ 0.0--2.0]\n");
   fprintf(stderr,
           "    -jm f          : weight of GV for spectrum                               [  1.0][ 0.0--2.0]\n");
   fprintf(stderr, "  infile:\n");
   fprintf(stderr, "    label file\n");
   fprintf(stderr, "  note:\n");
   fprintf(stderr,
           "    option '-d' may be repeated to use multiple delta parameters.\n");
   fprintf(stderr,
           "    generated spectrum and log F0 sequences are saved in natural\n");
   fprintf(stderr, "    endian, binary (float) format.\n");
   fprintf(stderr, "\n");

   exit(0);
}

/* Error: output error message */
void Error(const int error, const char *message, ...)
{
   va_list arg;

   fflush(stdout);
   fflush(stderr);

   if (error > 0)
      fprintf(stderr, "\nError: ");
   else
      fprintf(stderr, "\nWarning: ");

   va_start(arg, message);
   vfprintf(stderr, message, arg);
   va_end(arg);

   fflush(stderr);

   if (error > 0)
      exit(error);
}

/* Getfp: wrapper for fopen */
FILE *Getfp(const char *name, const char *opt)
{
   FILE *fp = fopen(name, opt);

   if (fp == NULL)
      Error(2, "Getfp: Cannot open %s.\n", name);

   return (fp);
}

/* GetNumInterp: get number of speakers for interpolation from argv */
int GetNumInterp(int argc, char **argv_search)
{
   int num_interp = 1;
   while (--argc) {
      if (**++argv_search == '-') {
         if (*(*argv_search + 1) == 'i') {
            num_interp = atoi(*++argv_search);
            if (num_interp < 1) {
               num_interp = 1;
            }
            --argc;
         }
      }
   }
   return (num_interp);
}

static FILE *do_fopen(const char *fname,const char *mode)
{
    FILE *fd;

    fd = fopen(fname,mode);
    if (fd == NULL)
	{
		cerr << "hts_engine: failed to open " << fname << endl;
		festival_error();
	}
	return fd;
}

LISP HTS_Synthesize_Utt(LISP utt)
{
	EST_Utterance *u = get_c_utt(utt);
	EST_Item *item = 0;
	LISP hts_engine_params = NIL;
	LISP hts_output_params = NIL;

	int usegv=1;
	int version;
	int i; 
	double f;
	FILE *labfp = NULL;
#ifndef HTS_EMBEDDED
	FILE *lf0fp = NULL, *mcepfp = NULL;
#endif				/* !HTS_EMBEDDED */
	FILE *durfp = NULL, *wavfp = NULL, *rawfp = NULL, *tracefp = NULL;

	/* file pointers */
	FILE *fp_ws_mcp[3] = {NULL, NULL, NULL};
	FILE *fp_ws_lf0[3] = {NULL, NULL, NULL};
	int  num_ws_mcp = 3,num_ws_lf0 = 3;

	FILE *fp_ms_dur = NULL;
	FILE *fp_ms_mcp = NULL;
	FILE *fp_ms_lf0 = NULL;

	FILE *fp_ts_dur = NULL;
	FILE *fp_ts_mcp = NULL;
	FILE *fp_ts_lf0 = NULL;

	FILE *fp_ms_gvm = NULL;
	FILE *fp_ms_gvl = NULL;
  
	/* engine */
	HTS_Engine engine;
	
	/* global engine parameters */
	int sampling_rate = 16000;
	int fperiod = 80;
	double alpha = 0.42;
	double stage = 0.0;			/* gamma = -1.0/stage */
	double beta = 0.0;
	double uv_threshold = 0.5;
	double gv_weight_lf0 = 0.7;
	double gv_weight_mcp = 1.0;
	double f0_std = 1.0;
	double f0_mean = 0.0;
	HTS_Boolean phoneme_alignment = FALSE;
    const char *phoneme_alignment_text;
	double speech_speed = 1.0;
	HTS_Boolean use_log_gain = TRUE;
	const char *use_log_gain_text;

	/* Get voice specific params */
	hts_engine_params = siod_get_lval("hts_engine_params",
			"HTS_ENGINE: no parameters set for module");
	/* We should be internalize these ones more */
	hts_output_params = siod_get_lval("hts_output_params",
			"HTS_ENGINE: no output parameters set for module");
	
	version = get_param_int("-version",hts_engine_params,0);
	
	switch(version)
	{
		case 1:
		/* Load files */
		fp_ws_lf0[0] = do_fopen(get_param_str("-df1",hts_engine_params,
												"hts/lf0.win1"),"rb");
		fp_ws_lf0[1] = do_fopen(get_param_str("-df2",hts_engine_params,
												"hts/lf0.win2"),"rb");
		fp_ws_lf0[2] = do_fopen(get_param_str("-df3",hts_engine_params,
												"hts/lf0.win3"),"rb");
		fp_ws_mcp[0] = do_fopen(get_param_str("-dm1",hts_engine_params,
												"hts/mgc.win1"),"rb");
		fp_ws_mcp[1] = do_fopen(get_param_str("-dm2",hts_engine_params,
												"hts/mgc.win2"),"rb");
		fp_ws_mcp[2] = do_fopen(get_param_str("-dm3",hts_engine_params,
											"hts/mgc.win3"),"rb");
		fp_ts_lf0 = do_fopen(get_param_str("-tf",hts_engine_params,
												"hts/tree-lf0.inf"),"r");	
		fp_ts_dur = do_fopen(get_param_str("-td",hts_engine_params,
												"hts/tree-dur.inf"),"r");
		fp_ts_mcp = do_fopen(get_param_str("-tm",hts_engine_params,
												"hts/tree-mgc.inf"),"r");
		fp_ms_mcp = do_fopen(get_param_str("-mm",hts_engine_params,
												"hts/mgc.pdf"),"rb");
		fp_ms_dur = do_fopen(get_param_str("-md",hts_engine_params,
												"hts/dur.pdf"),"rb");	
		fp_ms_lf0 = do_fopen(get_param_str("-mf",hts_engine_params,
												"hts/lf0.pdf"),"rb");
		fp_ms_gvm = fopen(get_param_str("-cm",hts_engine_params,
												"hts/gv-mgc.pdf"),"rb");
		if (fp_ms_gvm == NULL)
		   usegv=0;
		fp_ms_gvl = fopen(get_param_str("-cf",hts_engine_params,
												"hts/gv-lf0.pdf"),"rb");
		if (fp_ms_gvl == NULL)
		   usegv=0;									
		
		break;
		case 0:
		fp_ws_lf0[1] = do_fopen(get_param_str("-df1",hts_engine_params,
											"hts/lf0_dyn.win"),"rb");
		fp_ws_lf0[2] = do_fopen(get_param_str("-df2",hts_engine_params,
											"hts/lf0_acc.win"),"rb");
		fp_ws_mcp[1] = do_fopen(get_param_str("-dm1",hts_engine_params,
											"hts/mcep_dyn.win"),"rb");
		fp_ws_mcp[2] = do_fopen(get_param_str("-dm2",hts_engine_params,
											"hts/mcep_acc.win"),"rb");
		fp_ts_lf0 = do_fopen(get_param_str("-tf",hts_engine_params,
				   "hts/trees-lf0.inf"),"r");	
		fp_ts_dur = do_fopen(get_param_str("-td",hts_engine_params,
				   "hts/trees-dur.inf"),"r");
		fp_ts_mcp = do_fopen(get_param_str("-tm",hts_engine_params,
				   "hts/trees-mcep.inf"),"r");
		fp_ms_mcp = do_fopen(get_param_str("-mm",hts_engine_params,
				   "hts/mcep.pdf"), "rb");
		fp_ms_dur = do_fopen(get_param_str("-md",hts_engine_params,
				   "hts/duration.pdf"),"rb");	
		fp_ms_lf0 = do_fopen(get_param_str("-mf",hts_engine_params,
				   "hts/lf0.pdf"), "rb");
		usegv=0;
		break;
		default:
		cerr << "hts_engine: Unable to work with this version\n";
		festival_error();
	}
	/* We should internalize these ones more */
	rawfp = do_fopen(get_param_str("-or",hts_output_params,
												"tmp.raw"), "wb");
	lf0fp = do_fopen(get_param_str("-of",hts_output_params,
												"tmp.f0"), "wb");
	mcepfp = do_fopen(get_param_str("-om",hts_output_params,
												"tmp.mcep"), "wb");
	labfp = do_fopen(get_param_str("-labelfile",hts_output_params,
												"utt.feats"), "r");

	/* Load engine parameters: */
	switch(version)
	{
		case 1:
		sampling_rate = get_param_float("-s",hts_engine_params,16000);
		fperiod = get_param_float("-p",hts_engine_params,80);
		alpha = get_param_float("-a",hts_engine_params,0.42);
		stage = get_param_float("-g",hts_engine_params,0.0); /* gamma = -1.0/stage */
		beta = get_param_float("-b",hts_engine_params,0.0);
		uv_threshold = get_param_float("-u",hts_engine_params,0.5);
		gv_weight_lf0 = get_param_float("-jf",hts_engine_params,0.7);
		gv_weight_mcp = get_param_float("-jm",hts_engine_params,1.0);
		f0_std = get_param_float("-fs",hts_engine_params,1.0);
		f0_mean = get_param_float("-fm",hts_engine_params,0.0);
		phoneme_alignment_text = get_param_str("-vp",hts_engine_params,"FALSE");
		speech_speed = get_param_float("-r",hts_engine_params,1.0);
		use_log_gain_text = get_param_str("-l",hts_engine_params,"TRUE");
	
		if (*phoneme_alignment_text=='T' || *phoneme_alignment_text == 't')
			phoneme_alignment = TRUE;
		if (*use_log_gain_text=='F' || *use_log_gain_text == 'f')
			use_log_gain = FALSE;
		
		break;
		case 0:
		// gp.RHO      = 0.0; 
		if (get_param_float("-r",hts_engine_params,0.0) != 0)
			cerr << "hts_engine: Ignoring parameter \"RHO\" (control duration parameter).\n";
		//gp.ALPHA    = 0.42;
		alpha = get_param_float("-a",hts_engine_params,0.42);
		//gp.F0_STD   = 1.0;
		f0_std = get_param_float("-fs",hts_engine_params,1.0);
		//gp.F0_MEAN  = 0.0;
		f0_mean = get_param_float("-fm",hts_engine_params,0.0);
		//gp.UV       = 0.5;
		uv_threshold = get_param_float("-u",hts_engine_params,0.5);
		//gp.LENGTH   = 0;
		if ((int)get_param_float("-l",hts_engine_params,0.0) != 0)
			cerr << "hts_engine: Ignoring parameter \"LENGTH\" (specified utterance length).\n";
		break;
		default:
		cerr << "hts_engine: Unable to work with this version\n";
		festival_error();
	}
	/* initialize (stream[0] = spectrum , stream[1] = lf0) */
	HTS_Engine_initialize(&engine, 2);
	/* load duration model */
	HTS_Engine_load_duration_from_fp(&engine, &fp_ms_dur,
												 &fp_ts_dur, 1);

	/* load stream[0] (spectrum model) */
	HTS_Engine_load_parameter_from_fp(&engine, &fp_ms_mcp,
									&fp_ts_mcp, fp_ws_mcp,0,
									 FALSE, num_ws_mcp, 1);

	/* load stream[1] (lf0 model) */
	HTS_Engine_load_parameter_from_fp(&engine, &fp_ms_lf0, 
									&fp_ts_lf0, fp_ws_lf0,
									1, TRUE, num_ws_lf0, 1);
    if (usegv==1){
		/* load gv[0] (GV for spectrum) */
		HTS_Engine_load_gv_from_fp(&engine, &fp_ms_gvm, 0, 1);
		/* load gv[1] (GV for lf0) */
		HTS_Engine_load_gv_from_fp(&engine, &fp_ms_gvl, 1, 1);
	}
	/* set parameter */
	HTS_Engine_set_sampling_rate(&engine, sampling_rate);
	HTS_Engine_set_fperiod(&engine, fperiod);
	HTS_Engine_set_alpha(&engine, alpha);
	HTS_Engine_set_gamma(&engine, stage);
	HTS_Engine_set_log_gain(&engine, use_log_gain);
	HTS_Engine_set_beta(&engine, beta);
	HTS_Engine_set_msd_threshold(&engine, 1, uv_threshold);		/* set voiced/unvoiced threshold for stream[1] */
	
	if (usegv==1){
	   HTS_Engine_set_gv_weight(&engine, 0, gv_weight_mcp);
	   HTS_Engine_set_gv_weight(&engine, 1, gv_weight_lf0);
    }

	HTS_Engine_set_duration_interpolation_weight(&engine, 0, 1.0);
	HTS_Engine_set_parameter_interpolation_weight(&engine, 0, 0,1.0);
	HTS_Engine_set_parameter_interpolation_weight(&engine, 1, 0,1.0);
	
	if (usegv==1){
	    HTS_Engine_set_gv_interpolation_weight(&engine, 0, 0, 1.0);
	    HTS_Engine_set_gv_interpolation_weight(&engine, 1, 0, 1.0);
    }
	/* synthesis */
	HTS_Engine_load_label_from_fp(&engine, labfp);		/* load label file */
	if (!phoneme_alignment)		/* modify label */
		for (i = 0; i < HTS_Label_get_size(&engine.label); i++)
			HTS_Label_set_frame(&engine.label, i, -1.0);
	if (speech_speed != 1.0)	/* modify label */
		HTS_Label_set_speech_speed(&engine.label, speech_speed);
	HTS_Engine_create_sstream(&engine);  /* parse label and determine state duration */
	if (f0_std != 1.0 || f0_mean != 0.0) {		/* modify f0 */
		for (i = 0; i < HTS_SStreamSet_get_total_state(&engine.sss); i++) {
			f = exp(HTS_SStreamSet_get_mean(&engine.sss, 1, i, 0));
			f = f0_std * f + f0_mean;
			if (f < 10.0)
				f = 10.0;
			HTS_SStreamSet_set_mean(&engine.sss, 1, i, 0, log(f));
		}
	}
	HTS_Engine_create_pstream(&engine);  /* generate speech parameter vector sequence */

	HTS_Engine_create_gstream(&engine);  /* synthesize speech */
	
	/* output */
	if (tracefp != NULL)
		HTS_Engine_save_infomation(&engine, tracefp);
	if (durfp != NULL)
		HTS_Engine_save_label(&engine, durfp);
	if (rawfp)
		HTS_Engine_save_generated_speech(&engine, rawfp);
	if (wavfp)
		HTS_Engine_save_riff(&engine, wavfp);
#ifndef HTS_EMBEDDED
	if (mcepfp)
		HTS_Engine_save_generated_parameter(&engine, mcepfp, 0);
	if (lf0fp)
		HTS_Engine_save_generated_parameter(&engine, lf0fp, 1);
#endif								/* !HTS_EMBEDDED */

	/* Load back in the waveform */
	EST_Wave *w = new EST_Wave;

	/* Close files */
	if (fp_ws_lf0[0] != NULL)
		fclose(fp_ws_lf0[0]);
	fclose(fp_ws_lf0[1]);
	fclose(fp_ws_lf0[2]);
	if (fp_ws_mcp[0] != NULL)
		fclose(fp_ws_mcp[0]);
	fclose(fp_ws_mcp[1]);
	fclose(fp_ws_mcp[2]);
	fclose(fp_ts_lf0);
	fclose(fp_ts_dur);
	fclose(fp_ts_mcp);
	fclose(fp_ms_mcp);
	fclose(fp_ms_dur);
	fclose(fp_ms_lf0);
	if (fp_ms_gvm != NULL)
		fclose(fp_ms_gvm);
	if (fp_ms_gvl != NULL)
		fclose(fp_ms_gvl);
	
	/* free */
	HTS_Engine_refresh(&engine);

	/* free memory */
	HTS_Engine_clear(&engine);

	/* close output files */
#ifndef HTS_EMBEDDED
	if (mcepfp != NULL)
		fclose(mcepfp);
	if (lf0fp != NULL)
		fclose(lf0fp);
#endif								/* !HTS_EMBEDDED */
	if (wavfp != NULL)
		fclose(wavfp);
	if (rawfp != NULL)
		fclose(rawfp);
	if (durfp != NULL)
		fclose(durfp);
	if (tracefp != NULL)
		fclose(tracefp);
	if (labfp != NULL)
		fclose(labfp);

	if (u->relation("Segment")->first())  /* only if there segments */
		w->load_file(get_param_str("-or",hts_output_params,"tmp.raw"),
							"raw", sampling_rate,
							"short", str_to_bo("native"), 1);

	item = u->create_relation("Wave")->append();
	item->set_val("wave",est_val(w));

	return utt;
}

LISP mlsa_resynthesis(LISP ltrack);

void festival_hts_engine_init(void)
{
	proclaim_module("hts_engine");

	festival_def_utt_module("HTS_Synthesize",HTS_Synthesize_Utt,
	"(HTS_Synthesis UTT)\n\
  Synthesize a waveform using the HTS Engine and the current models");
	init_subr_1("mlsa_resynthesis",mlsa_resynthesis,
					"(mlsa_resynthesis TRACK)\n\
  Return a WAVE synthesized from the F0/MCEP TRACK.");
}

