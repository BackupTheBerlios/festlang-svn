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
;;;  Customized for: msu_ru_nsh                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Ensure this version of festival has been compiled with clunits module
(require_module 'clunits)
(require 'clunits) ;; runtime scheme support

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'msu_ru_nsh_clunits voice-locations)
    (defvar msu_ru_nsh::dir 
      (cdr (assoc 'msu_ru_nsh_clunits voice-locations)))
    (defvar msu_ru_nsh::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append msu_ru_nsh::dir "festvox/")))
    (begin
     (format stderr "msu_ru_nsh::clunits: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  msu_ru_nsh::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append msu_ru_nsh::dir "festvox/") 
		      load-path))
(set! load-path (cons (path-append msu_ru_nsh::dir "lib/") 
		      load-path))

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'msu_ru_nsh_phoneset)
(require 'msu_ru_nsh_tokenizer)
(require 'msu_ru_nsh_tagger)
(require 'msu_ru_nsh_lexicon)
(require 'msu_ru_nsh_phrasing)
(require 'msu_ru_nsh_intonation)
(require 'msu_ru_nsh_durdata)
(require 'msu_ru_nsh_f0model)
;; ... and others as required

;;;
;;;  Code specific to the clunits waveform synthesis method
;;;

;;; Flag to save multiple loading of db
(defvar msu_ru_nsh::clunits_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar msu_ru_nsh::clunits_prompting_stage nil)
;;; Flag to allow new lexical items to be added only once
(defvar msu_ru_nsh::clunits_added_extra_lex_items nil)

;;; You may wish to change this (only used in building the voice)
(set! msu_ru_nsh::closest_voice 'voice_msu_ru_nsh_clunits)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to his list in msu_ru_nsh_build.scm
(set! msu_ru_nsh::dt_params
      (list
       (list 'db_dir msu_ru_nsh::dir)
       '(name msu_ru_nsh)
       '(index_name msu_ru_nsh)
       '(f0_join_weight 0.0)
       '(join_weights
         (0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 ))
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "mcep/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_msu_ru_nsh::clunit_name)
       ;;  Run time parameters 
       '(join_method windowed)
       ;; if pitch mark extraction is bad this is better than the above
;       '(join_method smoothedjoin)
;      '(join_method modified_lpc)
       '(continuity_weight 20.0)
       '(log_scores 1)  ;; good for high variance joins (not so good for ldom)
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
(define (msu_ru_nsh::nextvoicing i)
  (let ((nname (item.feat i "n.name")))
    (cond
;     ((string-equal nname "pau")
;      "PAU")
     ((string-equal "+" (item.feat i "n.ph_vc"))
      "V")
     ((string-equal (item.feat i "n.ph_cvox") "+")
      "CVox")
     (t
      "UV"))))

(define (msu_ru_nsh::clunit_name i)
  "(msu_ru_nsh::clunit_name i)
Defines the unit name for unit selection for ru.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not msu_ru_nsh::clunits_loaded)
	   (or (string-equal "h#" name) 
	       (string-equal "1" (item.feat i "ignore"))
	       (and (string-equal "pau" name)
		    (or (string-equal "pau" (item.feat i "p.name"))
			(string-equal "h#" (item.feat i "p.name")))
		    (string-equal "pau" (item.feat i "n.name")))))
      "ignore")
     ;; Comment out this if you want a more interesting unit name
     ((null nil)
      name)

     ;; Comment out the above if you want to use these rules
     ((string-equal "+" (item.feat i "ph_vc"))
      (string-append
       name
       "_"
       (item.feat i "R:SylStructure.parent.stress")
       "_"
       (msu_ru_nsh::nextvoicing i)))
     ((string-equal name "pau")
      (string-append
       name
       "_"
       (msu_ru_nsh::nextvoicing i)))
     (t
      (string-append
       name
       "_"
;       (item.feat i "seg_onsetcoda")
;       "_"
       (msu_ru_nsh::nextvoicing i))))))

(define (msu_ru_nsh::clunits_load)
  "(msu_ru_nsh::clunits_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params msu_ru_nsh::dt_params)
  (set! clunits_params msu_ru_nsh::dt_params)
  (clunits:load_db clunits_params)
  (load (string-append
	 (string-append 
	  msu_ru_nsh::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  ".tree")))
  (set! msu_ru_nsh::clunits_clunit_selection_trees clunits_selection_trees)
  (set! msu_ru_nsh::clunits_loaded t))

(define (msu_ru_nsh::voice_reset)
  "(msu_ru_nsh::voice_reset)
Reset global variables back to previous voice."
  (msu_ru_nsh::reset_phoneset)
  (msu_ru_nsh::reset_tokenizer)
  (msu_ru_nsh::reset_tagger)
  (msu_ru_nsh::reset_lexicon)
  (msu_ru_nsh::reset_phrasing)
  (msu_ru_nsh::reset_intonation)
  (msu_ru_nsh::reset_f0model)
  t
)

;; This function is called to setup a voice.  It will typically
;; simply call functions that are defined in other files in this directory
;; Sometime these simply set up standard Festival modules othertimes
;; these will be specific to this voice.
;; Feel free to add to this list if your language requires it

(define (voice_msu_ru_nsh_clunits)
  "(voice_msu_ru_nsh_clunits)
Define voice for ru."
  ;; *always* required
  (voice_reset)

  ;; Select appropriate phone set
  (msu_ru_nsh::select_phoneset)

  ;; Select appropriate tokenization
  (msu_ru_nsh::select_tokenizer)

  ;; For part of speech tagging
  (msu_ru_nsh::select_tagger)

  (msu_ru_nsh::select_lexicon)

  (msu_ru_nsh::select_phrasing)

  (msu_ru_nsh::select_intonation)

  (set! duration_cart_tree msu_ru_nsh::zdur_tree)
  (set! duration_ph_info msu_ru_nsh::phone_durs)
  (Parameter.set 'Duration_Method 'Tree_ZScores)
  (Parameter.set 'Duration_Stretch 1.0)

  (msu_ru_nsh::select_f0model)

  ;; Waveform synthesis model: clunits

  ;; Load in the clunits databases (or select it if its already loaded)
  (if (not msu_ru_nsh::clunits_prompting_stage)
      (begin
	(if (not msu_ru_nsh::clunits_loaded)
	    (msu_ru_nsh::clunits_load)
	    (clunits:select 'msu_ru_nsh))
	(set! clunits_selection_trees 
	      msu_ru_nsh::clunits_clunit_selection_trees)
	(Parameter.set 'Synth_Method 'Cluster)))

  (set! current_voice_reset msu_ru_nsh::voice_reset)

  (set! current-voice 'msu_ru_nsh_clunits)
)

(define (is_pau i)
  (if (phone_is_silence (item.name i))
      "1"
      "0"))

(proclaim_voice
 'msu_ru_nsh_clunits
  '((language russian)
    (gender male)
    (dialect moscow)
    (coding utf-8)
    (description
      "Russian festival voice.")))
                  
(provide 'msu_ru_nsh_clunits)

