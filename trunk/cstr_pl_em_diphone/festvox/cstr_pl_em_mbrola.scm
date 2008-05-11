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

;; Try to find out where we are
(if (assoc 'cstr_pl_em_mbrola voice-locations)
     (defvar cstr_pl_em_dir 
       (cdr (assoc 'cstr_pl_em_mbrola voice-locations)))
     ;;; Not installed in Festival yet so assume running in place
     (defvar cstr_pl_em_dir (pwd)))
(set! load-path (cons (path-append cstr_pl_em_dir "festvox") load-path))

;;; other files we need
(require 'cstr_pl_phones)
(require 'cstr_pl_lex)
(require 'cstr_pl_lts)
(require 'cstr_pl_token)
(require 'cstr_pl_int)
(require 'cstr_pl_dur)
(require 'cstr_pl_tobi_f0)
(require 'mbrola)

(define (cstr_pl_em_mbrola_fix utt)
"(cstr_pl_em_mbrola_fix UTT)
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
  (cstr_pl::reset_phoneset)
  (cstr_pl::reset_tokenizer)
  (cstr_pl::reset_tagger)
  (cstr_pl::reset_lexicon)
  (cstr_pl::reset_phrasing)
  (cstr_pl::reset_intonation)
  (cstr_pl::reset_duration)
  (cstr_pl::reset_f0model)
)

;;;  Full voice definition 
(define (voice_cstr_pl_em_mbrola)
"(voice_cstr_pl_em_mbrola)
Set speaker to em in pl from cstr."
  (voice_reset)
  (Parameter.set 'Language 'cstr_pl)

  (cstr_pl::select_phoneset)
  (cstr_pl::select_tokenizer)
  (cstr_pl::select_tagger)
  (cstr_pl::select_lexicon)
  (cstr_pl::select_phrasing)
  (cstr_pl::select_duration)

;  (cstr_pl::select_intonation)
;  (cstr_pl::select_f0model)
;  (set! int_params
;    '((target_f0_mean 180) (target_f0_std 40)
;      (model_f0_mean 110) (model_f0_std 20)))

   (cstr_pl::select_tobi_f0)
   (set! int_lr_params
	'((target_f0_mean 169) (target_f0_std 40)
	  (model_f0_mean 169) (model_f0_std 66)))


 ;; Waveform synthesizer: MBROLA pl1 diphones
  (Parameter.set 'Synth_Method MBROLA_Synth)
  ;;  Because we need an extra parameter in the new version of mbrola
  ;;  we add that parameter to the database "name"
  (set! mbrola_progname "mbrola")
  ;;  Newer versions of mbrola require the -I flag
  (set! mbrola_database 
	(format 
	 nil
	 "-I %s%s %s%s "
         cstr_pl_em_dir "/mbrola-pl1/pl1_phones"
	 cstr_pl_em_dir "/mbrola-pl1/pl1" 
	 ))

;; Phone mapping!!!! add KM
(set! mbrola_phonemap_tab
      '(("?" . "_")  ; map glottal stops to pauses
        ("s+" . "s'") ; si
        ("z+" . "z'") ; zi
        ("ts+" . "ts'") ; ci
        ("dz+" . "dz'") ; dzi
        ("n+" . "n'") ; ni
        ("ng" . "N") ; N? 
        )  
       ) 


  ;; set callback to restore some original values changed by this voice
  (set! current_voice_reset cstr_pl_em_voice_reset)

  (set! current-voice 'cstr_pl_em_mbrola)
)

(proclaim_voice
 'cstr_pl_em_mbrola
 '((language pl)
   (gender female)
   (dialect polish)
   (description
    "COMMENT"
    )))

(provide 'cstr_pl_em_mbrola)
