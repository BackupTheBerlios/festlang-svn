;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                         Copyright (c) 1998                            ;;
;;;                        All Rights Reserved.                           ;;
;;;                                                                       ;;
;;;  Permission to use, copy, modify, distribute this software and its    ;;
;;;  documentation for research, educational and individual use only, is  ;;
;;;  hereby granted without fee, subject to the following conditions:     ;;
;;;   1. The code must retain the above copyright notice, this list of    ;;
;;;      conditions and the following disclaimer.                         ;;
;;;   2. Any modifications must be clearly marked as such.                ;;
;;;   3. Original authors' names are not deleted.                         ;;
;;;  This software may not be used for commercial purposes without        ;;
;;;  specific prior written permission from the authors.                  ;;
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
;;;  Tokenization rules for polish
;;;
;;;  Particularly numbers and symbols.
;;; 

(require 'cstr_pl_number_decl)
    
(define (polish_number name)
"(polish_number name)

Convert a string of digits into a list of words saying the number."
  (if (string-matches name "0")
      (list "zero")
      (polish_number_from_digits (symbolexplode name))))

(define (just_zeros digits)
"(just_zeros digits)
If this only contains 0s then we just do something different."
 (cond
  ((not digits) t)
  ((string-equal "0" (car digits))
   (just_zeros (cdr digits)))
  (t nil)))
(define (polish_number_from_digits digits)
  "(polish_number_from_digits digits)
Takes a list of digits and converts it to a list of words
saying the number."
  (let ((l (length digits)))
    (cond
     ((equal? l 0)
      nil)
     ((string-equal (car digits) "0")
      (polish_number_from_digits (cdr digits)))
     ((equal? l 1);; single digit
      (cond 
       ((string-equal (car digits) "0") (list "zero"))
       ((string-equal (car digits) "1") (list "jeden"))
       ((string-equal (car digits) "2") (list "dwa"))
       ((string-equal (car digits) "3") (list "trzy"))
       ((string-equal (car digits) "4") (list "cztery"))
       ((string-equal (car digits) "5") (list "piêæ"))
       ((string-equal (car digits) "6") (list "sze¶æ"))
       ((string-equal (car digits) "7") (list "siedem"))
       ((string-equal (car digits) "8") (list "osiem"))
       ((string-equal (car digits) "9") (list "dziewiêæ"))
       ;; fill in the rest
       (t (list "equis"))));; $$$ what should say?
     ((equal? l 2);; less than 100
      (cond
       ((string-equal (car digits) "0");; 0x
	(polish_number_from_digits (cdr digits)))
     
       ((string-equal (car digits) "1");; 1x
	(cond
	 ((string-equal (car (cdr digits)) "0") (list "dziesiêæ"))
	 ((string-equal (car (cdr digits)) "1") (list "jedena¶cie"))
	 ((string-equal (car (cdr digits)) "2") (list "dwanas~cie"))
	 ((string-equal (car (cdr digits)) "3") (list "trzyna¶cie"))
	 ((string-equal (car (cdr digits)) "4") (list "czterna¶cie"))
	 ((string-equal (car (cdr digits)) "5") (list "piêtna¶cie"))
         ((string-equal (car (cdr digits)) "6") (list "szesna¶cie"))
	 ((string-equal (car (cdr digits)) "7") (list "siedemna¶cie"))
	 ((string-equal (car (cdr digits)) "8") (list "osiemna¶cie"))
         ((string-equal (car (cdr digits)) "9") (list "dziewiêtna¶cie"))
	 (t 
	  (cons "dieci" (polish_number_from_digits (cdr digits))))))
     
       ((string-equal (car digits) "2");; 2x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "dwadzie¶cia")
	    (cons "dwadzie¶cia" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "3");; 3x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "trzydzie¶ci")
	    (cons "trzydzie¶ci" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "4");; 4x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "czterdzie¶ci")
	    (cons "czterdzie¶ci" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "5");; 5x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "piêædziesi±t")
	    (cons "piêædziesi±t" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "6");; 6x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "sze¶ædziesi±t")
	    (cons "sze¶ædziesi±t" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "7");; 7x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "siedemdziesiat")
	    (cons "siedemdziesi±t" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "8");; 8x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "osiemdziesi±t")
	    (cons "osiemdziesi±t" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "9");; 9x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "dziewiêædziesi±t")
	    (cons "dziewiêædziesi±t" (polish_number_from_digits (cdr digits)))))

       ))

     ((equal? l 3);; in the hundreds
      (cond 
     
       ((string-equal (car digits) "1");; 1xx
	(if (just_zeros (cdr digits)) (list "sto")
	    (cons "sto" (polish_number_from_digits (cdr digits)))))

       ((string-equal (car digits) "2");; 2xx
	(cons "dwie¶cie" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "3");; 3xx
	(cons "trzysta" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "4");; 4xx
	(cons "czterysta" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "5");; 5xx
	(cons "piêæset" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "6");; 6xx
	(cons "sze¶æset" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "7");; 7xx
	(cons "siedemset" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "8");; 8xx
	(cons "osiemset" (polish_number_from_digits (cdr digits))))

       ((string-equal (car digits) "9");; 9xx
	(cons "dziewiêæset" (polish_number_from_digits (cdr digits))))

       (t;; ?xx
	(append (polish_number_from_digits (list (car digits))) 
		(list "cientos") 
		(polish_number_from_digits (cdr digits))))
       ))

     ((< l 7)
      (let ((sub_thousands 
	     (list 
	      (car (cdr (cdr (reverse digits))))
	      (car (cdr (reverse digits)))
	      (car (reverse digits))))
	    (thousands (reverse (cdr (cdr (cdr (reverse digits)))))))
	(set! x (polish_number_from_digits thousands))
	(append
	 (if (string-equal (car x) "jeden")
	   nil
	   x)
	 (cond
	  ((and (equal? (length thousands) 1) 
		(string-equal "1" (car thousands)))
	   (list "tysi±c"))
	  ((and (equal? (length thousands) 1) 
		(member_string (car thousands) '("2" "3" "4")))
	   (list "tysi±ce")) 
	  (t
	   (list "tysiêcy")))  ;; all others
         (polish_number_from_digits sub_thousands))))
     ((< l 13)
      (let ((sub_million 
	     (list 
	      (car (cdr (cdr (cdr (cdr (cdr(reverse digits)))))))
	      (car (cdr (cdr (cdr (cdr (reverse digits))))))
	      (car (cdr (cdr (cdr (reverse digits)))))
	      (car (cdr (cdr (reverse digits))))
	      (car (cdr (reverse digits)))
	      (car (reverse digits))
	      ))
	    (millions (reverse (cdr (cdr (cdr (cdr (cdr (cdr (reverse digits))))))))))
	(set! x (polish_number_from_digits millions))
      
	(append
	 (if (string-equal (car x) "jeden")
	   nil
	   x)
	 (cond
	  ((and (equal? (length millions) 1) 
		(string-equal "1" (car millions)))
	   (list "milion"))
	  ((and (equal? (length millions) 1) 
		(member_string (car millions) '("2" "3" "4")))
	   (list "miliony"))
	  (t
	   (list "milionów")))  ;; all others

        
	 (polish_number_from_digits sub_million))))
        
     (t
      (list "olbrzymi" "numer")))))

(define (say_polish_number token name)
  "(say_polish_number token name)
Gues case number etc for given number and modify the basic nomative
form acordingly."
  (let ((numberwords (polish_number name)))
    (cond
     ((member_string (item.feat token "n.name")
		     '("maja"))
;      (format t "its in may\n")
      (set! nnn (make_polish_number numberwords 'ord-gen-masc))
;      (format t "as %l\n" nnn)
      nnn)
     (t
      numberwords))))

(define (make_polish_number words form)
  "(make_polish_number words form)
Make give number words list into required form."
  (let ((rwords (reverse words)))
    (cond
     ((member_string (car words) '("milion" "miliony" "tysio~c" 
					    "tysio~c" "tysio~ce" ))
      (reverse
       (cons
	(modify_number_word (car rwords) form)
	(cdr rwords))))
     ((member_string (car words) '("dwadzies~cia" "trzydzies~ci"))
      (reverse
       (cons
	(modify_number_word (car rwords) form)
	(cdr rwords))))
     ((cdr rwords)
      (reverse
       (cons
	(modify_number_word (car rwords) form)
	(cons
	 (modify_number_word (car (cdr rwords)) form)
	 (cdr (cdr rwords))))))
     (t
      (reverse
       (cons
	(modify_number_word (car rwords) form)
	nil))))))

(define (modify_number_word word form)
  (let ((ttt (assoc (intern word) polish_number_declensions)))
    (if ttt 
	(let ((uuu (assoc form (cdr ttt))))
	  (if uuu
	      (car (cdr uuu))
	      word))
	word)))

(define (cstr_pl_token_to_words token name)
  "(cstr_pl_token_to_words TOKEN NAME)
Returns a list of words for the NAME from TOKEN in UTT.  This primarily
allows the treatment of numbers, money etc."
  (cond
   ((string-matches name "[1-9][0-9]+")
   (say_polish_number token name))
   ((and (string-equal name "w")
         (string-equal (item.feat token "punc") "."))
;    (format t "wiek %s\n" name)
    (list "wiek")) 
   ((and (member_string name '("o" "w" "z" "i" "a" "u"))
	 (not (equal? 1 (length (string-append (item.feat token "n.name"))))))
;    (format t "homographic prep %s\n" name)
    (list (list (list 'name name) (list 'pos "prep"))))
   ((string-matches name "[a-zA-Z]")
;    (format t "single character letter %s\n" name)
    (list (list (list 'name name) (list 'pos "nn"))))
   ((or (string-matches name "[A-Z][A-Z]")
	(string-matches name "[A-Z][A-Z][A-Z]")
	(and (string-matches name "[A-Z][A-Z]+")
	     (not (string-matches name ".*[AEIOU].*"))))
;    (format t "acronym %s\n" name)
     (mapcar
      (lambda (l)
	(list (list 'name l) (list 'pos "nn") (list "break" 1)))
      (symbolexplode name)))
   ((not (lts.in.alphabet name 'pol_downcase))
    ;; It contains some other than the lts can deal with
;    (format t "not in pol_downcase %s\n" name)
    (let ((subwords))
      (item.set_feat token "pos" "nn")
      (mapcar
       (lambda (letter)
	 ;; might be symbols or digits
	 (set! subwords
	       (append
		subwords
		(cond
		 ((string-matches letter "[0-9]")
		  (polish_number letter))
		 ((string-matches letter "[A-ZÁÉÍÓÚÜÑ]")
		    (polish_downcase letter))
		 (t
		  (list letter))))))
       (symbolexplode name))
      subwords))
   (t
    (list name))))

;;; Part of speech done by crude lookup using gpos 
(set! cstr_pl_guess_pos 
'((in "v" "s" "z")
  (det "a" "jako")
))

(define (cstr_pl::select_tokenizer)
  "(cstr_pl::select_tokenizer)
Set up tokenizer for pl."
  (set! token_to_words cstr_pl_token_to_words)
)

(define (cstr_pl::select_tagger)
  "(cstr_pl::select_tagger)
Set up the POS tagger for pl."
  (set! pos_lex_name nil)
  (set! guess_pos cstr_pl_guess_pos) 
)

(define (cstr_pl::reset_tagger)
  "(cstr_pl::reset_tagger)
Reset tagging information."
  t
)

(define (cstr_pl::reset_tokenizer)
  "(cstr_pl::reset_tokenizer)
Reset tokenizer information."
  t
)

(provide 'cstr_pl_token)
