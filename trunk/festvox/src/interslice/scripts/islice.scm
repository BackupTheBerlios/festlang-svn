;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                         Copyright (c) 2007                          ;;;
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
;;; Give the islice split, re-align this to the festival utterance      ;;;
;;; split                                                               ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (skip_word word i)
  "move forward in item for the number of phones in word"
  (let ((n (length 
            (apply
             append
             (mapcar
              (lambda (syl)
                (item.relation.daughters syl 'SylStructure))
              (item.relation.daughters word 'SylStructure))))))
    (while (> n 0)
       (if (item.next i)
           (set! i (item.next i)))
       (set! n (- n 1)))

    i))

(define (phone_match a b)
  ;; because some phones are getting reduced (not sure why)
  ;; we allow a fuzzy match is something is a schwa -- if
  ;; this really doesn't match it'll fail on the next phone
  (or (string-equal a b)
      (and (string-equal a "ax")
           (string-matches b "[aeiou].*"))
      (and (string-equal b "ax")
           (string-matches a "[aeiou].*"))))

(define (first_nonsil_in_utt cas_item)
  (if (or (null (item.prev cas_item))
          (string-equal "pau" (item.feat cas_item "p.name"))
          (string-equal "ssil" (item.feat cas_item "p.name")))
      t
      nil))

(define (find_utt_boundaries festsplit islicesplit islicetimes newfestsplit)
  "(find_utt_boundaries festsplit islicesplit newfestsplit)
Iterates through the original split (based on text analysis, to find where
the breaks are in acoustic based split and output the start and end times
for each of text analysis based utterance."

  (set! silence (car (cadr (car (PhoneSet.description '(silences))))))
  (set! ofd (fopen newfestsplit "w"))
  (set! asplit (load islicesplit t))
  (set! timesplit (load islicetimes t))
  (set! cas_utt nil) ;; the current asplit utt
  (set! cas_pos 0.0) ;; over all time index
  (set! cas_item nil)
  (set! tutt_start 0.0)
  (set! tutt_end 0.0)
  (set! tutt_pos 0.0)
  (set! tutt_move nil)
  (set! last_word nil)
  (set! this_word nil)
  (mapcar
   (lambda (tutt_entry)
;     (format t "%l\n" tutt_entry)
     (set! tutt (SynthText (cadr tutt_entry)))
     (set! tutt_item (utt.relation.first tutt 'Segment))
     (set! tutt_start tutt_end)
     ;; try to align the tutt items with the acoustic items
     (while tutt_item
        (if (and (item.relation.parent tutt_item 'SylStructure)
                 tutt_move)
            (begin
              (set! last_word this_word)
              (set! this_word 
                    (item.relation.parent
                     (item.relation.parent tutt_item 'SylStructure)
                     'SylStructure))
              '(format t "next word %s %s %s\n"
                      (if tutt_item
                          (item.name tutt_item)
                          "NONE")
                      (if last_word
                          (item.name last_word)
                          "NONE")
                      (if this_word
                          (item.name this_word)
                          "NONE"))))
        (if (null cas_item)
            (begin
              (set! cas_utt (Utterance Text ""))
              (if asplit
                  (begin
                    (utt.relation.load 
                     cas_utt 'Segment
                     (format nil "lab/%s.lab" (caar asplit)))
                    (set! cas_item (utt.relation.first cas_utt 'Segment))
                    (set! asplit (cdr asplit))
                    (set! tutt_pos (car (cdr (car timesplit))))
                    (set! timesplit (cdr timesplit)))
                  (begin ;; end of acoustics
                    ))))
        (set! tutt_move t)
        (cond
         ((null cas_item)  ;; no more acoustic labels
          (set! tutt_item nil)
          (set! tutt_move nil)
          )
         ((phone_match (item.name tutt_item) (item.name cas_item))
          (item.set_feat 
           tutt_item "end"
           (+ tutt_pos (item.feat cas_item "end")))
          (set! tutt_end (item.feat tutt_item "end"))
          (set! tutt_item (item.next tutt_item))
          (set! cas_item (item.next cas_item)))
         ((or (string-equal silence (item.name tutt_item))
              (string-equal "ssil" (item.name tutt_item)))
          (set! tutt_item (item.next tutt_item)))
         ((or (string-equal silence (item.name cas_item))
              (string-equal "ssil" (item.name cas_item)))
          (set! cas_item (item.next cas_item))
          (set! tutt_move nil))
         ((and 
           (or (format t "ccc %l %l %s %s %s\n"
                       (first_nonsil_in_utt cas_item)
                       last_word 
                       (item.name cas_item)
                       (if last_word
                           (item.feat last_word "R:SylStructure.daughter1.daughter1.name")
                           "NONE")
                       (if last_word
                           (item.feat last_word "name")
                           "NONE"))
               t)
           (first_nonsil_in_utt cas_item)
           last_word 
           (phone_match
            (item.name cas_item)
            (item.feat last_word "R:SylStructure.daughter1.daughter1.name")))
          ;; Sometimes a word gets put on the end of the asplit utt
          ;; and at the beginning -- so a mismatch may effectively be a 
          ;; repeated word -- so skip back to the last word and see if
          ;; it fits -- if it does skip that number of phones
          (format t "misalign WORD SKIPPING %s %s \n"
                  (item.name last_word)
                  (caar asplit))
          (set! this_word nil) ;; ensure we don't get two skips in a row
          (set! cas_item (skip_word last_word cas_item))
         )
         (t
          (format t "misalign %s (%s %s) %s (%s) %f\n" (item.name tutt_item)
                  (item.feat tutt_item "R:SylStructure.parent.parent.name")
                  (item.feat tutt_item "R:SylStructure.parent.parent.R:Word.n.name")
                  (item.name cas_item) 
                  (caar asplit)
                  tutt_end)
          (error)
          ))
        )

     (format ofd "( %s %l %f %f )\n"
             (car tutt_entry)
             (cadr tutt_entry)
             tutt_start
             tutt_end)
;     (format t "( %s %l %f %f )\n"
;             (car tutt_entry)
;             (cadr tutt_entry)
;             tutt_start
;             tutt_end)
         
     )
   (load festsplit t))
     
  (fclose ofd)
)

(define (text_features utt)
  "(text_features ofd txtfile) 
Used as a post filter for generating prompt files, and saving
paragraph information."
  (if (string-matches 
       (item.feat 
        (utt.relation.first utt 'Token)
        "whitespace")
       ".*\n.*\n\\(.\\|\n\\)*")
      (format ofd "((para start)) ")
      (format ofd "((para mid)) "))
)

(set! word_output_hook text_features)

(provide 'islice)
