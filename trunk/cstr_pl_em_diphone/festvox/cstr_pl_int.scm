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
;;;  Accent and F0 prediction
;;;

(define (since_punctuation word)
 "(since_punctuation word)
Number of words since last punctuation or beginning of utterance."
 (cond
   ((null word) 0) ;; beginning or utterance
   ((not (string-equal "0" (item.feat word "p.lisp_token_end_punc"))) 0)
   (t
    (+ 1 (since_punctuation (item.prev word))))))

(define (until_punctuation word)
 "(until_punctuation word)
Number of words until next punctuation or end of utterance."
 (cond
   ((null word) 0) ;; beginning or utterance
   ((not (string-equal "0" (token_end_punc word))) 0)
   (t
    (+ 1 (until_punctuation (item.next word))))))

(set! cstr_pl_phrase_cart_tree
'
((lisp_token_end_punc in ("?" "." ":" ";"))
  ((BB))
  ((lisp_token_end_punc in ("'" "\"" ","))
   ((B))
   ((n.name is 0)  ;; end of utterance
    ((BB))
    ((lisp_since_punctuation > 5)
     ((lisp_until_punctuation > 5)
      ((gpos is content)
       ((n.gpos is content)
        ((NB))
        ((B)))   ;; not content so a function word
       ((NB)))   ;; this is a function word
      ((NB)))    ;; too close to punctuation
     ((NB)))     ;; too soon after punctuation
    ((NB))))))

(set! cstr_pl_phrase_cart_tree_1
'
((lisp_token_end_punc in ("?" "." ":"))
  ((BB))
  ((lisp_token_end_punc in ("'" "\"" "," ";"))
   ((BB))
   ((n.name is 0)  ;; end of utterance
    ((BB))
    ((lisp_since_punctuation > 5)
     ((lisp_until_punctuation > 5)
      ((gpos is content)
       ((n.gpos content)
        ((BB))
        ((BB)))   ;; not content so a function word
       ((BB)))   ;; this is a function word
      ((BB)))    ;; too close to punctuation
     ((BB)))     ;; too soon after punctuation
    ((BB))))))

(define (cstr_pl::select_phrasing)
  "(cstr_pl::select_phrasing)
Set up the phrasing module for Polish."
  (set! phrase_cart_tree cstr_pl_phrase_cart_tree)
  (Parameter.set 'Phrase_Method 'cart_tree)
)

(define (cstr_pl::reset_phrasing)
  "(cstr_pl::reset_phrasing)
Reset phrasing information."
  t
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Intonation
(set! cstr_pl_accent_cart_tree
  '
  (
   (R:SylStructure.parent.gpos is content)
    ( (stress is 1)
       ((Accented))
       ((NONE))
    )
  )
)

(define (cstr_pl::select_intonation)
  "(cstr_pl::select_intonation)
Set up intonation for pl."
  ;; Accent and tone prediction
  (set! int_accent_cart_tree cstr_pl_accent_cart_tree)
  (Parameter.set 'Int_Method 'Simple)
)

(define (cstr_pl::reset_intonation)
  "(cstr_pl::reset_intonation)
Reset intonation information."
  t
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require 'cstr_pl_f0)

(define (Int_Targets_Tree utt)
  "(Int_Targets_Tree utt)
For each syllable in a phrase add start mid and end F0 targets."
  (utt.relation.create utt 'Target)
  (mapcar
   (lambda (syl)
     (Tree_Predict_Targets utt syl))
   (utt.relation.items utt 'Syllable))
  utt)

(define (Tree_Predict_Targets utt syl)
  "(Tree_Predict_Targets utt syl)
Add targets to start (if immediately after a pause) mid vowel
and end for this syllable."
  (if (tpt_after_pause syl)
      (tpt_add_target
       utt
       (item.relation.daughter1 syl 'SylStructure)
       0
       (wagon_predict syl F0start_tree)))
  (tpt_add_target utt (tpt_find_syl_vowel syl) 50
	      (wagon_predict syl F0mid_tree))
  (tpt_add_target utt (item.relation.daughtern syl 'SylStructure) 100
	      (wagon_predict syl F0end_tree)))

(define (tpt_after_pause syl)
  "(tpt_after_pause syl)
Returns t if segment immediately before this is a pause (or utterance
start).  nil otherwise."
  (let ((pseg (item.relation.prev (item.relation.daughter1 syl 'SylStructure)
				  'Segment)))
    (if (or (not pseg)
	    (member_string
	     (item.name pseg)
	     (car (cdr (car (PhoneSet.description '(silences)))))))
	t
	nil)))

(define (tpt_find_syl_vowel syl)
  "(tpt_find_syl_vowel syl)
Find the item that is the vowel in syl."
  (let ((v (item.relation.daughtern syl 'SylStructure)))
    (mapcar
     (lambda (s)
       (if (string-equal "+" (item.feat s "ph_vc"))
	   (set! v s)))
     (item.relation.daughters syl 'SylStructure))
    v))

(define (tpt_f0_map_value value)
  "(tpt_f0_map_value value)
Map F0 vlaue through means and standard deviations in int_params."
  (let ((target_f0_mean (get_param 'target_f0_mean int_params 90))
	(target_f0_stddev (get_param 'target_f0_stddev int_params 20))
	(model_f0_mean (get_param 'model_f0_mean int_params 90))
	(model_f0_stddev (get_param 'model_f0_stddev int_params 20)))
    (+ (* (/ (- value model_f0_mean) model_f0_stddev)
	  target_f0_stddev) target_f0_mean)))

(define (tpt_add_target utt seg pos value)
  "(tpt_add_target utt seg pos value)
Add Target at pos and value related to seg."
  (let ((tseg (item.relation seg 'Target))
	(ntarg))
    (if (null tseg)
	(set! tseg (utt.relation.append utt 'Target seg)))
    (set! ntarg (item.append_daughter tseg))
    (format stderr "Adding f0 segment %s value %f\n" (item.name seg) value)
    (item.set_feat ntarg 'f0 (tpt_f0_map_value value))
    (item.set_feat ntarg 'pos 
		   (+ (item.feat seg "segment_start")
		      (* (/ pos 100) (item.feat seg "segment_duration"))))))


(define (cstr_pl::select_f0model)
  "(cstr_pl::select_f0model)
Set up the F0 model for cstr_pl."

 (set! F0start_tree cstr_pl::start_f0)
 (set! F0mid_tree cstr_pl::mid_f0)
 (set! F0end_tree cstr_pl::end_f0)
 (set! int_params
    '((target_f0_mean 165) (target_f0_std 15)
      (model_f0_mean 110) (model_f0_std 30)))
  (Parameter.set 'Int_Target_Method Int_Targets_Tree)
)

(define (cstr_pl::reset_f0model)
  "(cstr_pl::reset_f0model)
Reset F0 model information."
  t
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require 'cstr_pl_dur)

(define (cstr_pl::select_duration)
  "(cstr_pl::select_duration)
Set up duration model."
  (set! duration_cart_tree cstr_pl_em::zdur_tree)
  (set! duration_ph_info cstr_pl_em::phone_durs)
  (Parameter.set 'Duration_Method 'Tree_ZScores)
)

(define (cstr_pl::reset_duration)
  "(cstr_pl::reset_duration)
Reset duration information."
  t
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(provide 'cstr_pl_int)
