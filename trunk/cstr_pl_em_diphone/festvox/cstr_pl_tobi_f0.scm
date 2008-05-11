;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;; F0 generation for tobi intonation	                                ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Load any necessary files here

(require 'cstr_pl_tobi)
(require 'cstr_pl_pointlr)

(define (cstr_pl::select_tobi_f0)
  "(cstr_pl::select_tobi_f0)
Set up the tobi F0 model for English."

    (Parameter.set 'Int_Method Intonation_Tree)
    (set! int_tone_cart_tree cstr_pl_tone_cart_tree)
    (set! int_accent_cart_tree cstr_pl_accent_cart_tree)

    (set! f0_lr_start point_f0_lr_start) 
    (set! f0_lr_mid point_f0_lr_mid)
    (set! f0_lr_end point_f0_lr_end)

    (set! int_lr_params
	'((target_f0_mean 169) (target_f0_std 40)
	  (model_f0_mean 169) (model_f0_std 66)))

    (Parameter.set 'Int_Target_Method Int_Targets_LR)
)

(define (cstr_pl::reset_tobi_f0)
  "(cmu_pl::reset_tobi_f0)
Reset F0 model information."
  t
)

(provide 'cstr_pl_tobi_f0)
