;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                  and Alan W Black and Kevin Lenzo                   ;;;
;;;                      Copyright (c) 1998-2005                        ;;;
;;;                        All Rights Reserved.                         ;;;
;;;                                                                     ;;;
;;; Permission is hereby granted, free of charge, to use and distribute ;;;
;;; this software and its documentation without restriction, including  ;;;
;;; without limitation the rights to use, copy, modify, merge, publish, ;;;
;;; distribute, sublicense, and/or sell copies of this work, and to     ;;;
;;; permit persons to whom this work is furnished to do so, subject to  ;;;
;;; the following conditions:                                           ;;;
;;;  1. The code must retain the above copyright notice, this list of   ;;;
;;;     conditions and the following disclaimer.                        ;;;
;;;  2. Any modifications must be clearly marked as such.               ;;;
;;;  3. Original authors' names are not deleted.                        ;;;
;;;  4. The authors' names are not used to endorse or promote products  ;;;
;;;     derived from this software without specific prior written       ;;;
;;;     permission.                                                     ;;;
;;;                                                                     ;;;
;;; CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK        ;;;
;;; DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     ;;;
;;; ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  ;;;
;;; SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE     ;;;
;;; FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   ;;;
;;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  ;;;
;;; AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         ;;;
;;; ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      ;;;
;;; THIS SOFTWARE.                                                      ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                      ;;
;;;  A generic voice definition file for a clunits synthesizer           ;;
;;;  Customized for: cstr_pl_em                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Ensure this version of festival has been compiled with clunits module
(require_module 'clunits)
(require 'clunits) ;; runtime scheme support

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'cstr_pl_em_clunits voice-locations)
    (defvar cstr_pl_em::dir 
      (cdr (assoc 'cstr_pl_em_clunits voice-locations)))
    (defvar cstr_pl_em::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append cstr_pl_em::dir "festvox/")))
    (begin
     (format stderr "cstr_pl_em::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  cstr_pl_em::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append cstr_pl_em::dir "festvox/") 
		      load-path))

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'cstr_pl_phones)
(require 'cstr_pl_lex)
(require 'cstr_pl_lts)
(require 'cstr_pl_token)
(require 'cstr_pl_int)

;;;
;;;  Code specific to the clunits waveform synthesis method
;;;

;;; Flag to save multiple loading of db
(defvar cstr_pl_em::clunits_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar cstr_pl_em::clunits_prompting_stage nil)
;;; Flag to allow new lexical items to be added only once
(defvar cstr_pl_em::clunits_added_extra_lex_items nil)

;;; (require 'cstr_pl_em_diphone)
;;; You may wish to change this (only used in building the voice)
(set! cstr_pl_em::closest_voice 'voice_cstr_pl_em_diphone)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to his list in cstr_pl_em_build.scm
(set! cstr_pl_em::dt_params
      (list
       (list 'db_dir cstr_pl_em::dir)
       '(name cstr_pl_em)
       '(index_name cstr_pl_em)
       '(f0_join_weight 1.0)
       '(join_weights
         (0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 ))
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "mcep/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_cstr_pl_em::clunit_name)
       ;;  Run time parameters 
       '(join_method windowed)
       ;; if pitch mark extraction is bad this is better than the above
;       '(join_method smoothedjoin)
;       '(join_method modified_lpc)
       '(continuity_weight 3)
;      '(log_scores 1)  ;; good for high variance joins (not so good for ldom)
       '(optimal_coupling 1)
       '(extend_selections 2)
       '(pm_coeffs_dir "mcep/")
       '(pm_coeffs_ext ".mcep")
       '(sig_dir "wav/")
       '(sig_ext ".wav")
;       '(pm_coeffs_dir "lpc/")
;       '(pm_coeffs_ext ".lpc")
;       '(sig_dir "lpc/")
;       '(sig_ext ".res")
;       '(clunits_debug 1)
))

(define (cstr_pl_em::nextvoicing i)
  (let ((nname (item.feat i "n.name")))
    (cond
     ((string-equal "+" (item.feat i "n.ph_vc"))
      "V")
     ((string-equal (item.feat i "n.ph_cvox") "+")
      "CVox")
     (t
      "UV"))))

(define (cstr_pl_em::clunit_name i)
  "(cstr_pl_em::clunit_name i)
Defines the unit name for unit selection for pl.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not cstr_pl_em::clunits_loaded)
	   (or (string-equal "h#" name) 
	       (string-equal "1" (item.feat i "ignore"))
	       (and (string-equal "pau" name)
		    (or (string-equal "pau" (item.feat i "p.name"))
			(string-equal "h#" (item.feat i "p.name")))
		    (string-equal "pau" (item.feat i "n.name")))))
      "ignore")
     ;; Comment out this if you want a more interesting unit name

     ;; Comment out the above if you want to use these rules
     ((string-equal "+" (item.feat i "ph_vc"))
      (string-append
       name
       "_"
       (item.feat i "R:SylStructure.parent.stress")
;;       "_"
;;       (cstr_pl_em::nextvoicing i)
       ))

     ((null nil)      name)


     ((string-equal name "pau")
      (string-append
       name
       "_"
       (cstr_pl_em::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (cstr_pl_em::nextvoicing i))))))

(define (cstr_pl_em::clunits_load)
  "(cstr_pl_em::clunits_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params cstr_pl_em::dt_params)
  (set! clunits_params cstr_pl_em::dt_params)
  (clunits:load_db clunits_params)
  (load (string-append
	 (string-append 
	  cstr_pl_em::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  ".tree")))
  (set! cstr_pl_em::clunits_clunit_selection_trees clunits_selection_trees)
  (set! cstr_pl_em::clunits_loaded t))

(define (cstr_pl_em::voice_reset)
  "(cstr_pl_em::voice_reset)
Reset global variables back to previous voice."
  (cstr_pl::reset_phoneset)
  (cstr_pl::reset_tokenizer)
  (cstr_pl::reset_tagger)
  (cstr_pl::reset_lexicon)
  (cstr_pl::reset_phrasing)
  (cstr_pl::reset_intonation)
  (cstr_pl::reset_duration)
  (cstr_pl::reset_f0model)
  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it

(define (voice_cstr_pl_em_clunits)
  "(voice_cstr_pl_em_clunits)
Define voice for pl."
  (voice_reset)

  ;; Select appropriate phone set
  (cstr_pl::select_phoneset)
  (cstr_pl::select_tokenizer)
  (cstr_pl::select_tagger)
  (cstr_pl::select_lexicon)
  (cstr_pl::select_phrasing)
  (cstr_pl::select_intonation)
  (cstr_pl::select_duration)
  (cstr_pl::select_f0model)

  ;; Waveform synthesis model: clunits

  ;; Load in the clunits databases (or select it if its already loaded)
  (if (not cstr_pl_em::clunits_prompting_stage)
      (begin
	(if (not cstr_pl_em::clunits_loaded)
	    (cstr_pl_em::clunits_load)
	    (clunits:select 'cstr_pl_em))
	(set! clunits_selection_trees 
	      cstr_pl_em::clunits_clunit_selection_trees)
	(Parameter.set 'Synth_Method 'Cluster)))

  (set! current_voice_reset cstr_pl_em::voice_reset)
  (set! current-voice 'cstr_pl_em_clunits)
)

(define (is_pau i)
  (if (phone_is_silence (item.name i))
      "1"
      "0"))

(provide 'cstr_pl_em_clunits)

