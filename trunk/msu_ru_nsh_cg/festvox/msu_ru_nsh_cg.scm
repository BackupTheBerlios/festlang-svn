;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
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
;;;  A generic voice definition file for a clustergen synthesizer        ;;
;;;  Customized for: msu_ru_nsh                                       ;;
;;;                                                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Try to find the directory where the voice is, this may be from
;;; .../festival/lib/voices/ or from the current directory
(if (assoc 'msu_ru_nsh_cg voice-locations)
    (defvar msu_ru_nsh::dir 
      (cdr (assoc 'msu_ru_nsh_cg voice-locations)))
    (defvar msu_ru_nsh::dir (string-append (pwd) "/")))

;;; Did we succeed in finding it
(if (not (probe_file (path-append msu_ru_nsh::dir "festvox/")))
    (begin
     (format stderr "msu_ru_nsh::clustergen: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append  msu_ru_nsh::dir "festvox/"))
     (format stderr "   Either the voice isn't linked in Festival library\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append msu_ru_nsh::dir "festvox/") 
		      load-path))
(set! load-path (cons (path-append msu_ru_nsh::dir "lib/") 
		      load-path))

(require 'clustergen)  ;; runtime scheme support

;;; Voice specific parameter are defined in each of the following
;;; files
(require 'msu_ru_nsh_phoneset)
(require 'msu_ru_nsh_tokenizer)
(require 'msu_ru_nsh_tagger)
(require 'msu_ru_nsh_lexicon)
(require 'msu_ru_nsh_phrasing)
(require 'msu_ru_nsh_intonation)
(require 'msu_ru_nsh_durdata_cg) 
(require 'msu_ru_nsh_f0model)
(require 'msu_ru_nsh_statenames)

;;;
;;;  Code specific to the clustergen waveform synthesis method
;;;

(set! cluster_synth_method 
  (if (boundp 'mlsa_resynthesis)
      cg_wave_synth
      cg_wave_synth_external))

;;; Flag to save multiple loading of db
(defvar msu_ru_nsh::cg_loaded nil)
;;; When set to non-nil clunits voices *always* use their closest voice
;;; this is used when generating the prompts
(defvar msu_ru_nsh::clunits_prompting_stage nil)

;;;  These are the parameters which are needed at run time
;;;  build time parameters are added to this list from build_clunits.scm
(set! msu_ru_nsh_cg::dt_params
      (list
       (list 'db_dir 
             (if (string-matches msu_ru_nsh::dir ".*/")
                 msu_ru_nsh::dir
                 (string-append msu_ru_nsh::dir "/")))
       '(name msu_ru_nsh)
       '(index_name msu_ru_nsh)
       '(trees_dir "festival/trees/")
       '(catalogue_dir "festival/clunits/")
       '(coeffs_dir "ccoefs/")
       '(coeffs_ext ".mcep")
       '(clunit_name_feat lisp_msu_ru_nsh::cg_name)))

;; So as to fit nicely with existing clunit voices we check need to 
;; prepend these params if we already have some set.
(if (boundp 'msu_ru_nsh::dt_params)
    (set! msu_ru_nsh::dt_params
          (append 
           msu_ru_nsh_cg::dt_params
           msu_ru_nsh::dt_params))
    (set! msu_ru_nsh::dt_params msu_ru_nsh_cg::dt_params))

(define (msu_ru_nsh::cg_name i)
  (let ((x nil))
  (if (assoc 'cg::trajectory clustergen_mcep_trees)
      (set! x i)
      (set! x (item.relation.parent i 'mcep_link)))

  (let ((ph_clunit_name 
         (msu_ru_nsh::clunit_name_real
          (item.relation
           (item.relation.parent x 'segstate)
           'Segment))))
    (cond
     ((string-equal ph_clunit_name "ignore")
      "ignore")
     (t
      (item.name i)))))
)

(define (msu_ru_nsh::clunit_name_real i)
  "(msu_ru_nsh::clunit_name i)
Defines the unit name for unit selection for ru.  The can be modified
changes the basic classification of unit for the clustering.  By default
this we just use the phone name, but you may want to make this, phone
plus previous phone (or something else)."
  (let ((name (item.name i)))
    (cond
     ((and (not msu_ru_nsh::cg_loaded)
	   (or (string-equal "h#" name) 
	       (string-equal "1" (item.feat i "ignore"))
	       (and (string-equal "pau" name)
		    (or (string-equal "pau" (item.feat i "p.name"))
			(string-equal "h#" (item.feat i "p.name")))
		    (string-equal "pau" (item.feat i "n.name")))))
      "ignore")
     ;; Comment out this if you want a more interesting unit name
     ((null nil)
      name))))

(define (msu_ru_nsh::cg_load)
  "(msu_ru_nsh::cg_load)
Function that actual loads in the databases and selection trees.
SHould only be called once per session."
  (set! dt_params msu_ru_nsh::dt_params)
  (set! clustergen_params msu_ru_nsh::dt_params)
  (set! msu_ru_nsh::param_vectors
        (track.load
	 (string-append 
	  msu_ru_nsh::dir "/"
	  (get_param 'trees_dir dt_params "trees/")
	  (get_param 'index_name dt_params "all")
	  "_mcep.params")))
  (set! msu_ru_nsh::clustergen_mcep_trees
        (load (string-append
               (string-append 
                msu_ru_nsh::dir "/"
                (get_param 'trees_dir dt_params "trees/")
                (get_param 'index_name dt_params "all")
                "_mcep.tree")) t))

;  (if (null (assoc 'cg::trajectory msu_ru_nsh::clustergen_mcep_trees))
      (set! msu_ru_nsh::clustergen_f0_trees
            (load (string-append
                   (string-append 
                    msu_ru_nsh::dir "/"
                    (get_param 'trees_dir dt_params "trees/")
                    (get_param 'index_name dt_params "all")
                    "_f0.tree")) t))
;                    )

;;  (if (null (assoc 'cg::trajectory msu_ru_nsh::clustergen_mcep_trees))
;;      (set! msu_ru_nsh::clustergen_npow_trees
;;            (load (string-append
;;                   (string-append 
;;                    msu_ru_nsh::dir "/"
;;                    (get_param 'trees_dir dt_params "trees/")
;;                    (get_param 'index_name dt_params "all")
;;                    "_npow.tree")) t))
;;                    )
                    
  (set! msu_ru_nsh::cg_loaded t))

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

(define (voice_msu_ru_nsh_cg)
  "(voice_msu_ru_nsh_cg)
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

  ;; For CG voice there is no duration modeling at the seg level

  (Parameter.set 'Duration_Method 'Default)
  (Parameter.set 'Duration_Stretch 1.0)
  (set! duration_cart_tree_cg msu_ru_nsh::zdur_tree)
  (set! duration_ph_info_cg msu_ru_nsh::phone_durs)

  (msu_ru_nsh::select_f0model)

  ;; Waveform synthesis model: cluster_gen
  (set! phone_to_states msu_ru_nsh::phone_to_states)
  (if (not msu_ru_nsh::clunits_prompting_stage)
      (begin
	(if (not msu_ru_nsh::cg_loaded)
	    (msu_ru_nsh::cg_load))
        (set! clustergen_param_vectors msu_ru_nsh::param_vectors)
	(set! clustergen_mcep_trees msu_ru_nsh::clustergen_mcep_trees)
        (if (boundp 'msu_ru_nsh::clustergen_f0_trees)
            (set! clustergen_f0_trees msu_ru_nsh::clustergen_f0_trees))
        (if (boundp 'msu_ru_nsh::clustergen_npow_trees)
            (set! clustergen_npow_trees msu_ru_nsh::clustergen_npow_trees))
	(Parameter.set 'Synth_Method 'ClusterGen)
      ))

  (set! current_voice_reset msu_ru_nsh::voice_reset)
  (set! current-voice 'msu_ru_nsh_cg)
)

(proclaim_voice
 'msu_ru_nsh_cg
  '((language russian)
    (gender male)
    (dialect moscow)
    (coding utf-8)
    (description
      "Russian clustergen festival voice.")))

(provide 'msu_ru_nsh_cg)

