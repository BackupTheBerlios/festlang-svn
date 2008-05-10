;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;; Aby Louw, March 2004                                                ;;;
;;;                                                                     ;;;
;;; F0 generation for INTSINT intonation                                ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Load any necessary files here
(require 'intsint_intonation)

(set! intsint_target_params
      '(
	(intsint_higher 0.5)
	(intsint_lower 0.5)
	(intsint_up 0.25)
	(intsint_down 0.25)
	;; These numbers modifies the speakers range.
	(target_f0_top 230)
	(target_f0_mid 165) ;; speaker's mean F0
	(target_f0_bottom 90)))

(define (cstr_pl_em::select_intsint_f0)
  "(cstr_pl_em::select_intsint_f0)
Set up the intsint F0 model for English."
  (Parameter.set 'Int_Target_Method Int_Targets_INTSINT)
  )

(define (cstr_pl_em::reset_intsint_f0)
  "(cmu_us_slt_arctic::reset_intsint_f0)
Reset F0 model information."
  t
)

(provide 'cstr_pl_intsint_f0)
