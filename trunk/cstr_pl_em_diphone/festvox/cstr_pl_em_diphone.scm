;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                         Copyright (c) 1998                            ;;
;;;                        All Rights Reserved.                           ;;
;;;                                                                       ;;
;;;  Permission is hereby granted, free of charge, to use and distribute  ;;
;;;  this software and its documentation without restriction, including   ;;
;;;  without limitation the rights to use, copy, modify, merge, publish,  ;;
;;;  distribute, sublicense, and/or sell copies of this work, and to      ;;
;;;  permit persons to whom this work is furnished to do so, subject to   ;;
;;;  the following conditions:                                            ;;
;;;   1. The code must retain the above copyright notice, this list of    ;;
;;;      conditions and the following disclaimer.                         ;;
;;;   2. Any modifications must be clearly marked as such.                ;;
;;;   3. Original authors' names are not deleted.                         ;;
;;;   4. The authors' names are not used to endorse or promote products   ;;
;;;      derived from this software without specific prior written        ;;
;;;      permission.                                                      ;;
;;;                                                                       ;;
;;;  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        ;;
;;;  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ;;
;;;  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ;;
;;;  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     ;;
;;;  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ;;
;;;  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ;;
;;;  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ;;
;;;  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ;;
;;;  THIS SOFTWARE.                                                       ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;  An example diphone voice
;;;
;;;  Authors: Dominika Oliver and Alan W Black (awb@cs.cmu.edu)
;;;

;;; Try to find out where we are
(if (assoc 'cstr_pl_em_diphone voice-locations)
    (defvar cstr_pl_em_dir 
      (cdr (assoc 'cstr_pl_em_diphone voice-locations)))
    ;;; Not installed in Festival yet so assume running in place
    (defvar cstr_pl_em_dir (pwd)))

(if (not (probe_file (path-append cstr_pl_em_dir "festvox/")))
    (begin
     (format stderr "cstr_pl_em: Can't find voice scm files they are not in\n")
     (format stderr "   %s\n" (path-append cstr_pl_em_dir "festvox/"))
     (format stderr "   Either the voice isn't linked into Festival\n")
     (format stderr "   or you are starting festival in the wrong directory\n")
     (error)))

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append cstr_pl_em_dir "festvox/") load-path))

;;; other files we need
(require 'cstr_pl_phones)
(require 'cstr_pl_lex)
(require 'cstr_pl_lts)
(require 'cstr_pl_token)
(require 'cstr_pl_int)

;;;  Ensure we have a festival with the right diphone support compiled in
(require_module 'UniSyn)

(set! cstr_pl_em_lpc_sep 
      (list
       '(name "cstr_pl_em_lpc_sep")
       (list 'index_file (path-append cstr_pl_em_dir "dic/emdiph.est"))
       '(grouped "false")
       (list 'coef_dir (path-append cstr_pl_em_dir "lpc"))
       (list 'sig_dir  (path-append cstr_pl_em_dir "lpc"))
       '(coef_ext ".lpc")
       '(sig_ext ".res")
       (list 'default_diphone 
	     (string-append
	      (car (cadr (car (PhoneSet.description '(silences)))))
	      "-"
	      (car (cadr (car (PhoneSet.description '(silences)))))))))

(set! cstr_pl_em_lpc_group 
      (list
       '(name "em_lpc_group")
       (list 'index_file 
	     (path-append cstr_pl_em_dir "group/cstr_pl_em_diphone.group"))
       '(grouped "true")
       (list 'default_diphone 
	     (string-append
	      (car (cadr (car (PhoneSet.description '(silences)))))
	      "-"
	      (car (cadr (car (PhoneSet.description '(silences)))))))))

;; Go ahead and set up the diphone db
;(set! cstr_pl_em_db_name (us_diphone_init cstr_pl_em_lpc_sep))
;; Once you've built the group file you can comment out the above and
;; uncomment the following.
(set! cstr_pl_em_db_name (us_diphone_init cstr_pl_em_lpc_group))

(define (cstr_pl_em_diphone_fix utt)
"(cstr_pl_em_diphone_fix UTT)
Map phones to phonological variants if the diphone database supports
them."
  (mapcar
   (lambda (s)
     (let ((name (item.name s)))
       ;; Check and do something maybe 
       ))
   (utt.relation.items utt 'Segment))
  utt)

(define (cstr_pl_em_voice_reset)
  "(cstr_pl_em_voice_reset)
Reset global variables back to previous voice."
  ;; whatever
)

;;;  Full voice definition 
(define (voice_cstr_pl_em_diphone)
"(voice_cstr_pl_em_diphone)
Set speaker to em in pl from cstr."
  (voice_reset)
  (Parameter.set 'Language 'cstr_pl)

  ;; Select appropriate phone set
  (cstr_pl::select_phoneset)
  (cstr_pl::select_tokenizer)
  (cstr_pl::select_tagger)
  (cstr_pl::select_lexicon)
  (cstr_pl::select_phrasing)
  (cstr_pl::select_intonation)
  (cstr_pl::select_duration)
  (cstr_pl::select_f0model)

  ;; Waveform synthesizer: diphones
  (set! UniSyn_module_hooks (list cstr_pl_em_diphone_fix))
  (set! us_abs_offset 0.0)
  (set! window_factor 1.0)
  (set! us_rel_offset 0.0)
  (set! us_gain 0.9)

  (set! int_params
    '((target_f0_mean 165) (target_f0_std 15)
      (model_f0_mean 90) (model_f0_std 20)))

  (Parameter.set 'Synth_Method 'UniSyn)
  (Parameter.set 'us_sigpr 'lpc)
  (us_db_select cstr_pl_em_db_name)

  ;; set callback to restore some original values changed by this voice
  (set! current_voice_reset cstr_pl_em_voice_reset)

  (set! current-voice 'cstr_pl_em_diphone)
)

(proclaim_voice
 'cstr_pl_em_diphone
 '((language pl)
   (gender COMMENT)
   (dialect COMMENT)
   (description
    "COMMENT"
    )
   (builtwith festvox-1.2)))

(provide 'cstr_pl_em_diphone)
