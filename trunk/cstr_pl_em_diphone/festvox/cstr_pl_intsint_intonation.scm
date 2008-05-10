;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;; Aby Louw, March 2004                                                ;;;
;;;                                                                     ;;;
;;; INTSINT Intonation setup for voice                                  ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Load any necessary files here
(require 'intsint_intonation)

;; your trained tree
(require 'cstr_pl_intsint_tree)

(define (cstr_pl_em::select_intsint_intonation)
  "(cstr_pl_em::select_intsint_intonation)
Set up intonation for English."
  (set! INTSINT_intonation_cart_tree cstr_pl_em::intsint_tree)
  (Parameter.set 'Int_Method Intonation_INTSINT)
)

(define (cstr_pl_em::reset_intsint_intonation)
  "(cstr_pl_em::reset_intsint_intonation)
Reset intonation information."
  t
)

(provide 'cstr_pl_intsint_intonation)
