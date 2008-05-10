;; backoff rules
(set! cstr-backoff_rules
  '(
    (_ #)))


;; SCHEME PATHS


;(defvar pl_multisyn_dir (cdr (assoc 'cstr_pl_em_multisyn voice-locations)))
(defvar pl_multisyn_dir (format nil "%s" (pwd)))
(defvar cstr_data_dir pl_multisyn_dir)

;;;  Add the directory contains general voice stuff to load-path
(set! load-path (cons (path-append pl_multisyn_dir "festvox/") load-path))

(defvar multisyn_lib_dir (path-append libdir "multisyn/"))

;; These may or may not be already loaded.
(if (not (member_string multisyn_lib_dir libdir))
    (set! load-path (cons multisyn_lib_dir load-path)))

;; REQUIRES
(require 'cstr_pl_phones)
(require 'cstr_pl_lex)
(require 'cstr_pl_lts)
(require 'cstr_pl_int)
(require 'cstr_pl_token)
(require 'cstr_pl_dur)
(require 'multisyn)

;; DATA PATHS

;; Location of Base utterances
(set! cstr_base_dirs (list (path-append cstr_data_dir "utt/")
			   (path-append cstr_data_dir "lpc/")
			   (path-append cstr_data_dir "lpc/")
			   (path-append cstr_data_dir "coef/")
			   ".utt" ".res" ".lpc" ".coef"))

(make_voice_definition 'cstr_pl_em_multisyn 
		       16000
		       'voice_cstr_pl_em_multisyn_configure
		       cstr-backoff_rules
		       cstr_data_dir
		       (list (list cstr_base_dirs "etc/txt.done.data")
			     (list cstr_base_dirs "etc/utts.pauses")))

(set_backtrace t)

(define (voice_cstr_pl_em_multisyn_configure_pre voice)
"Nothing important" nil)

(define (voice_cstr_pl_em_multisyn_configure voice)
  "voice_cstr_pl_em_configure voice)
 Set up the current voice to be male Canadian English (Jmk) for
 the multisyn unitselection engine."
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

  (set! int_params
    '((target_f0_mean 90) (target_f0_std 20)
      (model_f0_mean 90) (model_f0_std 20)))

  (Param.set 'Synth_Method 'MultiSyn)
  (Param.set 'unisyn.window_symmetric 0)
)
    
(proclaim_voice
 'cstr_pl_em_multisyn
 '((language polish)(gender male)(dialect none)
   (description "Polish multisyn")))

(provide 'cstr_pl_em_multisyn)
