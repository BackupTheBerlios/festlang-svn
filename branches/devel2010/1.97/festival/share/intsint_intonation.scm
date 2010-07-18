;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Copyright ?????????                                                              ;;
;; All rights reserved.                                                             ;;  
;;                                                                                  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                                  ;;
;; AUTHOR: Aby Louw                                                                 ;;
;; Email:                                                                           ;;
;; MODIFIED: Feb 10, 2004                                                           ;;
;;                                                                                  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;  These modules should predict intonation events/labels
;;;  based on information in the phrase and word streams

(require 'intonation)

;; Aby Louw
;; This is just a redefinition of the (Intonation utt) scheme entry point.
;; It now includes the INTSINT/MOMEL intonation module 

(define (Intonation utt)
"(Intonation utt)                                
Select between different intonation modules depending on the Parameter
Int_Method.  Currently offers three types: Simple, hats on each content
word; ToBI, a tree method for predicting ToBI accents; and Default a
really bad method with a simple downward sloping F0.  This is the first
of a two-stage intonation prediction process.  This adds accent-like
features to syllables, the second, Int_Targets generates the F0 contour
itself. [see Intonation]
Aby Louw : Added another intonation module, based on INTSINT labels derived
from the MOMEL points."  
  (let ((rval (apply_method 'Int_Method utt)))
    (Parameter.get 'Int_Method)
    (cond   
     (rval rval) ;; new style
     ((eq 'Simple (Parameter.get 'Int_Method))
      (Intonation_Simple utt))
     ((eq 'ToBI (Parameter.get 'Int_Method))
      (Intonation_Tree utt))
     ((eq 'General (Parameter.get 'Int_Method))
      (Intonation_Simple utt))  ;; yes this is a duplication
     ((eq 'INTSINT (Parameter.get 'Int_Method))  ;; Aby Louw: added to original (see intonation.scm)
      (Intonation_INTSINT utt))
     (t
      (Intonation_Default utt)))))




;; Aby Louw
;; Another redefinition, this time of the (Int_Targets utt) scheme entry point.
;; It now includes the INTSINT/MOMEL intonation module 
(define (Int_Targets utt)
"(Int_Targets utt)
The second stage in F0 prediction.  This generates F0 targets 
related to segments using one of three methods, a simple hat, 
linear regression based on ToBI markings, and a simple declining
slope.  This second part deals with actual F0 values and durations,
while the previous section only deals with accent (and boundary tone)
assignment. [see Intonation]
Aby Louw : Added F0 target prediction based on the INTSINT/MOMEL intonation module." 
  (let ((rval (apply_method 'Int_Target_Method utt)))
    (cond
     (rval rval) ;; new style
     ((eq 'Simple (Parameter.get 'Int_Method))
      (Int_Targets_Simple utt))
     ((eq 'ToBI (Parameter.get 'Int_Method))
      (Int_Targets_LR utt))
     ((eq 'General (Parameter.get 'Int_Method))
      (Int_Targets_General utt))
     ((eq 'INTSINT (Parameter.get 'Int_Method))  ;; Aby Louw: added to original (see intonation.scm)
      (Int_Targets_INTSINT utt))
     (t
      (Int_Targets_Default utt)))))


(provide 'intsint_intonation)
