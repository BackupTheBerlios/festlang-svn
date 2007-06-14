;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                  and Alan W Black and Kevin Lenzo                   ;;;
;;;                      Copyright (c) 1998-2000                        ;;;
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
;;;
;;; Tokenizer for ru
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;; Voice/ru token_to_word rules 
(define (msu_ru_nsh::token_to_words token name)
  "(msu_ru_nsh::token_to_words token name)
Specific token to word rules for the voice msu_ru_nsh.  Returns a list
of words that expand given token with name."
  (cond
   ((string-matches name "[0-9]*")
    (if (string-equal (item.feat token "n.name") "года")
    (msu_ru::number token name "year")
    (if 
	(or 
	    (string-equal (item.feat token "n.name") "января")
	    (string-equal (item.feat token "n.name") "февраля")
	    (string-equal (item.feat token "n.name") "марта")
	    (string-equal (item.feat token "n.name") "апреля")
	    (string-equal (item.feat token "n.name") "мая")
	    (string-equal (item.feat token "n.name") "июня")
	    (string-equal (item.feat token "n.name") "июля")
	    (string-equal (item.feat token "n.name") "августа")
	    (string-equal (item.feat token "n.name") "сентября")
	    (string-equal (item.feat token "n.name") "октября")
	    (string-equal (item.feat token "n.name") "ноября")
	    (string-equal (item.feat token "n.name") "декабря"))
        (msu_ru::number token name "day")
        (msu_ru::number token name nil))))
   ((string-matches name "1/2")
    (list "половина"))
   (t ;; when no specific rules apply do the general ones
    (list name))))

(define (msu_ru::number token name flag)
  "(msu_ru::number token name flag) reads number"
  (if (string-matches name "0")
      (list "ноль")
      (russian_number_from_digits (symbolexplode name) flag)))

(define (msu_ru::just_zeros digits)
"(just_zeros digits)
If this only contains 0s then we just do something different."
 (cond
  ((not digits) t)
  ((string-equal "0" (car digits))
   (msu_ru::just_zeros (cdr digits)))
  (t nil)))

(define (russian_number_from_digits digits flag)
  "(russian_number_from_digits digits flag)
Takes a list of digits and converts it to a list of words
saying the number."
  (let ((l (length digits)))
    (cond
     ((equal? l 0)
      nil)
     ((string-equal (car digits) "0")
      (russian_number_from_digits (cdr digits) flag))
     ((equal? l 1);; single digit
      (if (string-equal flag "year")
          (cond 
	   ((string-equal (car digits) "0") (list "ого"))
           ((string-equal (car digits) "1") (list "первого"))
	   ((string-equal (car digits) "2") (list "второго"))
           ((string-equal (car digits) "3") (list "третьего"))
	   ((string-equal (car digits) "4") (list "четвёртого"))
           ((string-equal (car digits) "5") (list "пятого"))
	   ((string-equal (car digits) "6") (list "шестого"))
	   ((string-equal (car digits) "7") (list "седьмого"))
           ((string-equal (car digits) "8") (list "восьмого"))
	   ((string-equal (car digits) "9") (list "девятого"))
	   (t nil))
	  (if (string-equal flag "day")
          (cond 
	   ((string-equal (car digits) "0") (list "ое"))
           ((string-equal (car digits) "1") (list "первое"))
	   ((string-equal (car digits) "2") (list "второе"))
           ((string-equal (car digits) "3") (list "третье"))
	   ((string-equal (car digits) "4") (list "четвёртое"))
           ((string-equal (car digits) "5") (list "пятое"))
	   ((string-equal (car digits) "6") (list "шестое"))
	   ((string-equal (car digits) "7") (list "седьмое"))
           ((string-equal (car digits) "8") (list "восьмое"))
	   ((string-equal (car digits) "9") (list "девятое"))
	   (t nil))
	  (if (string-equal flag "thousand")
	   (cond 
           ((string-equal (car digits) "1") (list "одна"))
	   ((string-equal (car digits) "2") (list "две"))
           ((string-equal (car digits) "3") (list "три"))
	   ((string-equal (car digits) "4") (list "четыре"))
           ((string-equal (car digits) "5") (list "пять"))
	   ((string-equal (car digits) "6") (list "шесть"))
	   ((string-equal (car digits) "7") (list "седь"))
           ((string-equal (car digits) "8") (list "восемь"))
	   ((string-equal (car digits) "9") (list "девять"))
	   (t nil))	  
          (cond 
	   ((string-equal (car digits) "0") (list "ноль"))
           ((string-equal (car digits) "1") (list "один"))
	   ((string-equal (car digits) "2") (list "два"))
           ((string-equal (car digits) "3") (list "три"))
	   ((string-equal (car digits) "4") (list "четыре"))
           ((string-equal (car digits) "5") (list "пять"))
	   ((string-equal (car digits) "6") (list "шесть"))
	   ((string-equal (car digits) "7") (list "семь"))
           ((string-equal (car digits) "8") (list "восемь"))
	   ((string-equal (car digits) "9") (list "девять"))
	   (t nil))))))
     ((equal? l 2);; less than 100
      (cond
       ((string-equal (car digits) "0");; 0x
	(russian_number_from_digits (cdr digits) flag))
     
       ((string-equal (car digits) "1");; 1x
	(cond
	 ((string-equal (car (cdr digits)) "0") (list "десять"))
	 ((string-equal (car (cdr digits)) "1") (list "одиннадцать"))
	 ((string-equal (car (cdr digits)) "2") (list "двенадцать"))
	 ((string-equal (car (cdr digits)) "3") (list "тринадцать"))
	 ((string-equal (car (cdr digits)) "4") (list "четырнадцать"))
	 ((string-equal (car (cdr digits)) "5") (list "пятнадцать"))
	 ((string-equal (car (cdr digits)) "6") (list "шестнадцать"))
	 ((string-equal (car (cdr digits)) "7") (list "семнадцать"))
	 ((string-equal (car (cdr digits)) "8") (list "восемнадцать"))
	 ((string-equal (car (cdr digits)) "9") (list "девятнадцать"))
	 ))
       
       ((string-equal (car digits) "2");; 2x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "двадцать")
	    (cons "двадцать" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "3");; 3x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "тридцать")
	    (cons "тридцать" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "4");; 4x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "сорок")
	    (cons "сорок" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "5");; 5x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "пятьдесят")
	    (cons "пятьдесят" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "6");; 6x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "шестьдесят")
	    (cons "шестьдесят" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "7");; 7x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "семьдесят")
	    (cons "семьдесят" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "8");; 8x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "восемьдесят")
	    (cons "восемьдесят" (russian_number_from_digits (cdr digits) flag))))

       ((string-equal (car digits) "9");; 9x
	(if (string-equal (car (cdr digits)) "0") 
	    (list "девяносто")
	    (cons "девяносто" (russian_number_from_digits (cdr digits) flag))))

       ))

     ((equal? l 3);; in the hundreds
      (cond 
     
       ((string-equal (car digits) "1");; 1xx
	(cons "сто" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "2");; 2xx
	(cons "двести" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "3");; 3xx
	(cons "триста" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "4");; 4xx
	(cons "четыреста" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "5");; 5xx
	(cons "пятьсот" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "6");; 6xx
	(cons "шестьсот" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "7");; 7xx
	(cons "семьсот" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "8");; 8xx
	(cons "восемьсот" (russian_number_from_digits (cdr digits) flag)))

       ((string-equal (car digits) "9");; 9xx
	(cons "девятьсот" (russian_number_from_digits (cdr digits) flag)))

       ))

     ((< l 7)
      (let ((sub_thousands 
	     (list 
	      (car (cdr (cdr (reverse digits))))
	      (car (cdr (reverse digits)))
	      (car (reverse digits))))
	    (thousands (reverse (cdr (cdr (cdr (reverse digits)))))))
	(set! x (russian_number_from_digits thousands "thousand"))
	(append
	  (cond
	    ((string-matches (car x) "один") (append x (list "тысяча")))
	    ((string-matches (car x) ".*два[ ]*") (append x (list "тысячи")))
	    ((string-matches (car x) ".*три[ ]*") (append x (list "тысячи")))
	    ((string-matches (car x) ".*четыре[ ]*") (append x (list "тысячи")))
	    (t (append x (list "тысяч"))))
	 (russian_number_from_digits sub_thousands flag))))

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
	(set! x (russian_number_from_digits millions nil))
	(append
	  (cond
	    ((string-matches (car x) "один") (append x (list "миллион")))
	    ((string-matches (car x) ".*два[ ]*") (append x (list "миллиона")))
	    ((string-matches (car x) ".*три[ ]*") (append x (list "миллиона")))
	    ((string-matches (car x) ".*четыре[ ]*") (append x (list "миллиона")))
	    (t (append x (list "миллионов")))
	  )
	 (russian_number_from_digits sub_million flag))))

     (t
      (list "ну" "очень" "большое" "число")))))
  


(define (msu_ru_nsh::select_tokenizer)
  "(msu_ru_nsh::select_tokenizer)
Set up tokenizer for ru."
  (Parameter.set 'Language 'msu_ru)
  (Parameter.set 'Token_Method 'Token_Any)

  (set! token_to_words msu_ru_nsh::token_to_words)
)

(define (msu_ru_nsh::reset_tokenizer)
  "(msu_ru_nsh::reset_tokenizer)
Reset any globals modified for this voice.  Called by 
(msu_ru_nsh::voice_reset)."
  ;; None

  t
)

(provide 'msu_ru_nsh_tokenizer)
