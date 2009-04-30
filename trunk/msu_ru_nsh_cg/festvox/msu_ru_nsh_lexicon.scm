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
;;; Lexicon, LTS and Postlexical rules for msu_ru
;;;

;;; There are phones (sounds, I can be wrong in terminology) and
;;; phonemes, phoneme can represent different phones depending on the
;;; position in the word, stress and so on. For diphone voice I think it's
;;; better to cover the whole range of phones (sounds) in different
;;; combinations. 
;;; 
;;; For cluster synthesis we need phoneme transcription (i.e. not convert
;;; word to it's actual phones) but to phonemes. Conversion to phonemes in
;;; russian is very easy - each letter corresponds to several phonemes
;;; one-to-one. Conversion to phones is harder and require deeper knowledge,
;;; but it can be also described as hand-written rules. But as I've pointed
;;; above, we don't need phone transcription, only phoneme one, so it's very
;;; easy to build LTS rules. And existing LTS rules from my voice describe 
;;; language very close. The only problem is to assign stress which is very 
;;; dynamic.
;;;
;;;
;;; For phonetic information we'll refer on book
;;; Kasatkin L.L. "Phonetics of the modern Russian language" MSU 2003.

(define (msu_ru_addenda)
  "(msu_ru_addenda)
Basic lexicon should (must ?) have basic letters, symbols and punctuation."

;;; Symbols ...
(lex.add.entry 
 '("*" nil ( ((u) 0) ((m n o) 1) ((zh i tt) 0) )))
(lex.add.entry 
 '("%" nil ( ((p r o) 1) ((c e n t) 0) )))

;; Basic punctuation must be in with nil pronunciation
(lex.add.entry '("." punc nil))
(lex.add.entry '("'" punc nil))
(lex.add.entry '(":" punc nil))
(lex.add.entry '(";" punc nil))
(lex.add.entry '("," punc nil))
(lex.add.entry '("-" punc (((pau) 0))))
;(lex.add.entry '("-" punc nil))
(lex.add.entry '("\"" punc nil))
(lex.add.entry '("`" punc nil))
(lex.add.entry '("?" punc nil))
(lex.add.entry '("!" punc nil))
(lex.add.entry '("--" punc (((pau) 0))))
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Hand written letter to sound rules
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(lts.ruleset
 russian_downcase
 ( )
 (
  ( [ А ] = а )
  ( [ Б ] = б )
  ( [ В ] = в )
  ( [ Г ] = г )
  ( [ Д ] = д )
  ( [ Е ] = е )
  ( [ Ё ] = ё )
  ( [ Ж ] = ж )
  ( [ З ] = з )
  ( [ И ] = и )
  ( [ Й ] = й ) 
  ( [ К ] = к )
  ( [ Л ] = л )
  ( [ М ] = м )
  ( [ Н ] = н )
  ( [ О ] = о )
  ( [ П ] = п )
  ( [ Р ] = р )
  ( [ С ] = с )
  ( [ Т ] = т )
  ( [ У ] = у )
  ( [ Ф ] = ф )
  ( [ Х ] = х )
  ( [ Ц ] = ц )
  ( [ Ч ] = ч )
  ( [ Ш ] = ш )
  ( [ Щ ] = щ )
  ( [ Ъ ] = ъ )
  ( [ Ы ] = ы )
  ( [ Ь ] = ь )
  ( [ Э ] = э )
  ( [ Ю ] = ю )
  ( [ Я ] = я )

  ( [ а ] = а )
  ( [ б ] = б )
  ( [ в ] = в )
  ( [ г ] = г )
  ( [ д ] = д )
  ( [ е ] = е )
  ( [ ё ] = ё )
  ( [ ж ] = ж )
  ( [ з ] = з )
  ( [ и ] = и )
  ( [ й ] = й ) 
  ( [ к ] = к )
  ( [ л ] = л )
  ( [ м ] = м )
  ( [ н ] = н )
  ( [ о ] = о )
  ( [ п ] = п )
  ( [ р ] = р )
  ( [ с ] = с )
  ( [ т ] = т )
  ( [ у ] = у )
  ( [ ф ] = ф )
  ( [ х ] = х )
  ( [ ц ] = ц )
  ( [ ч ] = ч )
  ( [ ш ] = ш )
  ( [ щ ] = щ )
  ( [ ъ ] = ъ )
  ( [ ы ] = ы )
  ( [ ь ] = ь )
  ( [ э ] = э )
  ( [ ю ] = ю )
  ( [ я ] = я )

  ( [ - ] = - )
  ( [ "'" ] = - )
  ( [ "+" ] = "+" )
  ( [ "." ] = т о ч к а )
  ( [ "," ] = - )
  ( [ ":" ] = д в о е т о ч и е )
  ( [ "_" ] = п о д ч ё р к и в а н и е )
  ( [ "/" ] = с л э ш )
  ( [ "@" ] = с о б а к а )
  ( [ "?" ] = в о п р о с )
  ( [ "=" ] = р а в н о )
  ( [ "&" ] = и )
  ( [ "<" ] = - )
  ( [ ">" ] = - )  
 
  ( [ 1 ] = о д и н )
  ( [ 2 ] = д в а )
  ( [ 3 ] = т р и )
  ( [ 4 ] = ч е т ы р е )
  ( [ 5 ] = п я т ь)
  ( [ 6 ] = ш е с т ь )
  ( [ 7 ] = с е м ь )
  ( [ 8 ] = в о с е м ь )
  ( [ 9 ] = д е в я т ь )
  ( [ 0 ] = н о л ь )
  
  ( [ A ] = а )
  ( [ B ] = б )
  ( [ C ] = ц )
  ( [ D ] = д )
  ( [ E ] = е )
  ( [ F ] = ф )
  ( [ G ] = ж )
  ( [ H ] = х )
  ( [ I ] = а й )
  ( [ J ] = д ж и )
  ( [ K ] = к )
  ( [ L ] = л )
  ( [ M ] = м )
  ( [ N ] = н )
  ( [ O ] = о )
  ( [ P ] = п )
  ( [ Q ] = к у )
  ( [ R ] = р )
  ( [ S ] = с )
  ( [ T ] = т )
  ( [ U ] = у )
  ( [ V ] = в )
  ( [ W ] = в )
  ( [ X ] = и к с )
  ( [ Y ] = а й )
  ( [ Z ] = з )

  ( e [ a ] = )
  ( [ a ] = а )
  ( [ b ] = б )
  ( [ c ] = ц )
  ( [ d ] = д )
  ( [ e ] = е )
  ( [ f ] = ф )
  ( [ g ] = ж )
  ( [ h ] = х )
  ( [ i ] = а й )
  ( [ j ] = д ж и )
  ( [ k ] = к )
  ( [ l ] = л )
  ( [ m ] = м )
  ( [ n ] = н )
  ( [ o ] = о )
  ( [ p ] = п )
  ( [ q ] = к у )
  ( [ r ] = р )
  ( [ s ] = с )
  ( [ t ] = т )
  ( [ u ] = у )
  ( [ v ] = в )
  ( [ w ] = в )
  ( [ x ] = и к с )
  ( [ y ] = а й )
  ( [ z ] = з )
))


(define (russian_downcase word)
  "(russian_downcase WORD)
Downs case word by letter to sound rules since builtin downcase is only ascii."
  (lts.apply word 'russian_downcase))

(define (is_vowel phone) 
    (member_string phone '(a e i o u y aa ee ii oo uu yy)))

(define (russian_contains_vowel phones)
  (cond
   ((null phones) nil)
   ((is_vowel (car phones))
    t)
   (t (russian_contains_vowel (cdr phones)))))


(define (russian_syllabify phones syls)
"(russian_syllabify phones syls)
Syllabify phones into syllables for Russian."
 (cond
  ((null phones)
   (mapcar
    (lambda (syl) (list (reverse syl) 0))
    (reverse syls)))
  ((and (is_vowel (car phones))
	(russian_contains_vowel (cdr phones)))
   (let ((nsyls (cons (cons (car phones) (car syls)) (cdr syls)))
	 (nphones (cdr phones)))
     (cond
      ((and (member_string (car nphones) '(l ll m mm n nn r rr))
	    (not (member_string (car (cdr nphones)) 
				'(a e i o y u aa ee ii oo yy uu l ll m mm n nn r rr))))
       (set! nsyls (cons (cons (car nphones) (car nsyls)) (cdr nsyls)))
       (set! nphones (cdr nphones)))
      ((and (string-equal (car nphones) "j")
	    (not (is_vowel (car (cdr nphones)))))
       (set! nsyls (cons (cons (car nphones) (car nsyls)) (cdr nsyls)))
       (set! nphones (cdr nphones)))
       )
     (russian_syllabify nphones (cons nil nsyls))))
  (t
   (russian_syllabify
    (cdr phones)
    (cons (cons (car phones) (car syls)) (cdr syls))))))

(define (count_syls phones)
  (cond
   ((null phones) 0)
   ((string-matches (car phones) "[aeyiou]")
    (+ 1 (count_syls (cdr phones))))
   (t (count_syls (cdr phones)))))

(define (get_last phones) 
    (cond 
     ((null (cddr (cdddr phones))) phones)
     (t (get_last (cdr phones)))))

(define (add_lex_stress_syl phones si tree)
  "(add_lex_stress_syl phones si tree)
Add lexical stressing."
  (cond
   ((null phones) 0)
   ((string-matches (car phones) "[aeiouy].*")
    
    (item.set_feat si 'num2end (- (+ 1 (item.feat si 'numsyls))
			       (item.feat si 'sylpos)))    
    (item.set_feat si 'name (car phones))	       
    (item.set_feat si 'nname (cadr phones))	       
    (item.set_feat si 'nnname (car (cddr phones)))
    (item.set_feat si 'nnnname (cadr (cddr phones)))
    (item.set_feat si 'lastttttname (car (item.feat si 'last)))
    (item.set_feat si 'lasttttname (cadr (item.feat si 'last)))	       
    (item.set_feat si 'lastttname (caddr (item.feat si 'last)))	       
    (item.set_feat si 'lasttname (car (cdddr (item.feat si 'last))))
    (item.set_feat si 'lastname (car (cdr (cdddr (item.feat si 'last)))))
		
    (set! stress (wagon_predict si tree))
        
    (item.set_feat si 'sylpos (+ 1 (item.feat si 'sylpos)))
    (item.set_feat si 'pname (car phones))	       
        
    (if (not (string-equal stress "0"))
	 (- (item.feat si 'sylpos) 1)
	 (add_lex_stress_syl (cdr phones) si tree)))
   (t    
    (item.set_feat si 'pname (car phones))	       
    (add_lex_stress_syl (cdr phones) si tree))))

(define (stressed_offset phones count)
"Offset of stressed syllable in syllables"
    (cond 
        ((null phones) -1000)
	((member_string (car phones) '(aa oo uu ee yy ii))
	  (+ count 1))    
	((is_vowel (car phones)) (+ (stressed_offset (cdr phones) count) 1))
	(t (stressed_offset (cdr phones) count)))
)

(define (add_lex_stress word phones tree)
  "(add_lex_stress word syls)
Predict lexical stress by decision tree."
  (let ((utt (Utterance Text ""))
	(si)
	(stress))
    
    (if (< 0 (stressed_offset phones 0))
	(begin
    	    (set! stress (stressed_offset phones 0))
;	    (format stderr "offset is %l\n" stress)
	    )
	(begin    
    	    (utt.relation.create utt 'Letter)
	    (set! si (utt.relation.append utt 'Letter))

	    (item.set_feat si 'numsyls (count_syls phones))
	    (item.set_feat si 'sylpos 1)
	    (item.set_feat si 'pname "pau")
	    (item.set_feat si 'last (get_last phones))

	    (set! stress (add_lex_stress_syl phones si tree))))
    
;;  (format t "%l\n" phones)
;;  (format t "%l\n" stress)
    stress))

;;;  Function called when word not found in lexicon
(define (msu_ru_lts_function word features)
  "(msu_ru_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."
;;  (format stderr "failed to find pronunciation for %s\n" word)
  (let (phones syls letters stress downcase_letters)

    (set! letters (utf8explode word));; utf8 letter based

    (set! downcase_letters (lts.apply letters 'russian_downcase))

    (set! phones (lts.apply downcase_letters 'msu_ru))

    (set! syls (russian_syllabify phones nil))

;;; More intelligently assign stress, not ready yet
    (set! stress (add_lex_stress word phones msu_ru_nsh_stress_tree))

    (set! syls 
      (cond ((and (>= (length syls) stress) (> stress 0)) 
    	   (set-car! (cdr (nth (- stress 1) syls)) 1) syls)
            (t syls)))
    
    (format stderr "Not in a dictionary %l %l\n" word syls)
    (list word features syls)
    ))

(define (fix_yo phones stress) 
    (let (newphones nil)
    (mapcar 
      (lambda (phone)
       (set! newphones (cons 
        (if (string-equal phone "e")
    	    'o
    	    phone) newphones)))
    phones)
;;    (format stderr "%l\n" (reverse newphones))
    (reverse newphones))
)

(define (strconcat list)
    "convert list into string"
    (cond ((null list) "")
	  (t (string-append (car list) (strconcat (cdr list)))))
)
 
;;;  Function called for word in lexicon
(define (msu_ru_pre_hook_function entry)
  "(msu_ru_pre_hook_function entry)
  convert stress and word to syllables"
  (let (letters downcase_letters)

       (set! letters (utf8explode entry));; utf8 letter based
       (set! downcase_letters (lts.apply letters 'russian_downcase))
       (list (strconcat (lts.apply downcase_letters 'russian_downcase))))
)

;;;  Function called for word in lexicon
(define (msu_ru_post_hook_function entry)
  "(msu_ru_post_hook_function entry)
  convert stress and word to syllables"
  (let ((word (car entry))
	(pos (car (cdr entry)))
	(stress (car (car (cdr (cdr entry)))))
	(fix (car (cdr (cdr (cdr entry)))))
	syls letters downcase letters phones result)
  (set! result
  (cond 
    ((or (eq? pos 'punc) (null pos)) entry)
    (t 
      (set! letters (utf8explode word));; utf8 letter based
      (set! downcase_letters (lts.apply letters 'russian_downcase))
      (set! phones (lts.apply downcase_letters 'msu_ru))
      (if (not (null fix))
        (set! phones (fix_yo phones stress)))
      (set! syls (russian_syllabify phones nil))
      (set! syls 
          (cond ((and (>= (length syls) stress) (> stress 0)) 
    		   (set-car! (cdr (nth (- stress 1) syls)) 1) syls)
    	        (t syls)))
      (list word pos syls)
      )))
;;   (format stderr "%l\n" result)
   result))


(define (msu_ru::postlex_reduce_vowel utt)
"(msu_ru::postlex_reduce_vowel utt)
Reduce vowels according to the stress."
;;  (format stderr "Reduction ")
  (mapcar
   (lambda (s)
     (let ((segment_name (item.name s)) (s1 (item.next s)))
     (cond 
       ;; небыло stress
        ((and (string-equal (item.feat s "nn.name") "o")
    	      (string-equal (item.feat s "n.name") "l")
    	      (string-equal segment_name "y")
    	      (string-equal (item.feat s "p.name") "b")
    	      (string-equal (item.feat s "pp.name") "i")
    	      (not (null (item.prev s)))
    	      (string-equal (item.feat (item.prev s) "pp.name") "nn"))
    		 (begin 
    		   (item.set_name (item.prev (item.prev s)) "ee")
        	   (item.set_name s "ay")))
        ((and (member_string segment_name '(a o u i y e))
    	    (string-equal (item.feat s "R:SylStructure.parent.stress") "1"))
    		(if (and (string-equal segment_name "i")
    			 (string-equal (item.feat s "p.name") "s"))
    		(item.set_name s "yy")
    		(item.set_name s (cadr (assoc segment_name 
    				    '(("a" "aa") 
    				      ("o" "oo")
    				      ("y" "yy")
    				      ("i" "ii")
    				      ("e" "ee")
    				      ("u" "uu")))))))

        ((and (member_string segment_name '(a o)) 
    	      (or (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
		  (string-equal (item.feat s "p.name") "j")
    	          (string-equal (item.feat s "p.ph_vc") "+")))
    		    (item.set_name s "a"))

        ((and (member_string segment_name '(a o)) 
    	      (string-equal (item.feat s "R:SylStructure.parent.R:Syllable.n.stress") "1"))
    	      (if (and (string-equal (item.feat s "p.ph_csoft") "+")
	    	       (< (item.feat s "p.syl_break" ) 0))
    		    (item.set_name s "i")
    		    (item.set_name s "a")))

        ((and (member_string segment_name '(y i u)) 
    	      (or (string-equal (item.feat s "R:SylStructure.parent.R:Syllable.n.stress") "1")
    	          (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
		  (string-equal (item.feat s "p.name") "j")
    	          (string-equal (item.feat s "p.ph_vc") "+")))
    	       nil)

        ((and (string-equal segment_name "e") 
    	      (or (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
		  (string-equal (item.feat s "p.name") "j")
    	          (string-equal (item.feat s "p.ph_vc") "+")))
    	          (item.set_name s "e"))

        ((and (string-equal segment_name "e") 
    	      (string-equal (item.feat s "R:SylStructure.parent.R:Syllable.n.stress") "1"))
    	      (if (string-equal (item.feat s "p.ph_csoft") "+")
    		    (item.set_name s "i")
    		    (item.set_name s "y")))

        ((member_string segment_name '(a o i e y))
    	      (if (string-equal (item.feat s "p.ph_csoft") "+")
    		    (item.set_name s "ae")
    		    (item.set_name s "ay")))

        ((and (string-equal segment_name "u") 
    	      (or (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
		  (string-equal (item.feat s "p.name") "j")
    	          (string-equal (item.feat s "p.ph_vc") "+")))
    	          (item.set_name s "u"))

        ((string-equal segment_name "u") 
    		    (item.set_name s "ur")))))
  (utt.relation.items utt 'Segment))
    
  (mapcar
   (lambda (s)
     (let ((segment_name (item.name s)) (s1 (item.next s)))
     (if (and (string-equal "1" (item.feat s "syl_final"))
	      (> (item.feat s "R:SylStructure.parent.syl_break") 0)
	      (not (member_string (item.feat s "R:SylStructure.parent.parent.R:Word.pos") '(aux in wp cc))))
     
	 (mapcar 
	  (lambda (r)
	    (if (string-equal (car r) (item.name s)) 
		(if (or 
		     (member_string (item.name s1) msu_ru::sonorlist) 
		     (phone_is_silence (item.name s1)) 
		     (string-equal "+" (item.feat s1 "ph_vc"))
		     (and 
		      (or (string-equal "v" (item.name s1)) 
			  (string-equal "vv" (item.name s1)))	
		      (or (string-equal "+" (item.feat s1 "n.ph_vc"))
			  (member_string (item.feat s1 "n.name") msu_ru::sonorlist))))
		    (item.set_name s (car (cdr r)))))

	    (if (string-equal (car (cdr r)) (item.name s)) 
		  (if 
		     (and 
		       (or (string-equal "v" (item.name s1)) 
		 	   (string-equal "vv" (item.name s1)))
		       (member_string (item.feat s1 "n.name") msu_ru::voicelist))
		      (item.set_name s (car r))))

	    (if (and (string-equal "z" (item.name s))
		     (member_string (item.name s1) msu_ru::voicelesslist))
		     (item.set_name s "s")))
	  msu_ru::reducelist)

	  (mapcar 
		  (lambda (r)
    		    (if (and (string-equal (car r) (item.name s))
			     (member_string (item.name s1) msu_ru::voicelesslist))
			     (item.set_name s (car (cdr r))))
	    
		    (if (and (string-equal (car (cdr r)) (item.name s)) 
			     (member_string (item.name s1) msu_ru::voicelist))
			     (item.set_name s (car r))))
	   msu_ru::reducelist))
    
    (format stderr "%s " (item.name s))
 
      ))
   (utt.relation.items utt 'Segment))
   (format stderr "\n")
)
   

;; You may or may not be able to write a letter to sound rule set for
;; your language.  If its largely lexicon based learning a rule
;; set will be better and easier that writing one (probably).
(lts.ruleset
 msu_ru
   (  
     (SOFTLETTERS 
	 ;;; Letter that do make previous consonant soft
            я ё ю и ь е
     )
     (STARTSYL # ъ ь а я о ё у ю э е и ы)
   )
 (
;; LTS rules


;; Some exceptions

    ( # [ ч т ] о = sh t )
    ( и [ ч ] н а я = sh )
    ( о [ ч ] н и к = sh )
    ( н е [ ч ] н о = sh )
    ( р о [ ч ] н о = sh )

    ( с и н [ т ] е з = t )
    ( и н [ т ] е р в = t )
    ( и н [ т ] е р ф = t )
    ( и н [ т ] е р п = t )
    ( э с [ т ] е т = t )
    ( а [ н ] е л я = n )
    ( # с о [ н ] е т = n )
    ( т у н [ н ] е л = n )
    ( # [ б ] е к и н г = b )
    ( # [ б ] е й к е р = b )
    ( # м о [ д ] е с т = d )
    ( # э к [ з ] е м = z )
    ( # э [ н ] е й = n )
    ( # б [ р ] е н д и # = r )
    ( # а р т и [ л л ] е р = ll )
    ( # ж е [ н ] щ и н = nn )
    
    ( п о л о [ г ] о # = g )
    ( с т р о [ г ] о # = g )
    ( # н е м н о [ г ] о = g )
    ( # н а м н о [ г ] о = g )
    ( # м н о [ г ] о = g )
    ( # д о р о [ г ] о # = g )
    ( # б о [ г ] # = h )

    ( е [ г ] о д н я # = v )
    ( о [ г ] о # = v )
    ( е [ г ] о # = v )
    ( е [ г ] о с я # = v )
    ( е [ г ] о - н = v )

    ( [ г ] к = h )
    ( к р е м л [ е ] в  = o )    
    ( # [ е е ] # = j e j o )
    ( л [ ь о ] = j o )
    ( [ с ь о ] = ss j o )
    ( # [ э ] к с к у р = i )
    ( # [ э ] л е к т р = i )
    
;; Stress, we use + sign here because the most of the usual symbols are handled as punctuation

    ( [ "+" а ] = aa )
    ( [ "+" ы ] = yy )
    ( [ "+" о ] = oo )
    ( [ "+" у ] = uu )
    ( [ "+" э ] = ee )
    ( STARTSYL [ "+" ю ] = j uu )
    ( STARTSYL [ "+" я ] = j aa )
    ( STARTSYL [ "+" е ] = j ee )
    ( [ "+" е ] = ee )
    ( [ "+" я ] = aa )
    ( [ "+" ю ] = uu )
    ( [ "+" и ] = ii )

;; Simple vowels

    ( [ а ] = a )
    ( [ ы ] = y )
    ( [ о ] = o )
    ( [ у ] = u )
    ( [ э ] = e )
    ( STARTSYL [ ю ] = j u )
    ( STARTSYL [ я ] = j a )
    ( STARTSYL [ е ] = j e )
    ( STARTSYL [ ё ] = j oo )
    ( [ ё ] = oo )
    ( [ е ] = e )
    ( [ я ] = a )
    ( [ ю ] = u )
    ( [ и ] = i )

;; Non readable phonems

    ( [ в ] с т в = ) 
    ( [ с т ] с = s )
    ( [ с т ь ] с = s )
    ( [ р д ] ц = r )
    ( [ р д ] ч = r )
    ( [ л н ] ц = n )
    ( [ с т ] ц = s )
    ( [ з д ] ц = z )
    ( [ н т ] ц = n )
    ( [ н д ] ц = n )
    ( [ н д ] ш = n )
    ( [ с т ] н = s )    
    ( [ з д ] н = z )    
    ( [ н т ] с = n )    
    ( [ н д ] с = n )    
    ( [ н г ] т = n )    
    ( [ с ш ] е с т = sh )
    ( [ с ч ] = sch )
    ( [ ч ] ш = t )
    ( [ д ] ц = )
    ( [ т с ] я = c )
    
;; Work around doubled consonants 

    ( # э [ м ] м = m )
    ( [ б ] б = )
    ( [ р ] р = )
    ( [ н ] н = )
    ( [ м ] м = )
    ( [ с ] с = )
    ( [ к ] к = )
    ( [ п ] п = )

;; Basic rules
        
    ( [ б ] SOFTLETTERS = bb )
    ( [ в ] SOFTLETTERS = vv )
    ( [ г ] SOFTLETTERS = gg )
    ( [ д ] SOFTLETTERS = dd )
    ( [ з ] SOFTLETTERS = zz )
    ( [ к ] SOFTLETTERS = kk )
    ( [ л ] SOFTLETTERS = ll )
    ( [ м ] SOFTLETTERS = mm )
    ( [ н ] SOFTLETTERS = nn )
    ( [ п ] SOFTLETTERS = pp )
    ( [ р ] SOFTLETTERS = rr )
    ( [ с ] SOFTLETTERS = ss )
    ( [ т ] SOFTLETTERS = tt )
    ( [ ф ] SOFTLETTERS = ff )
    ( [ х ] SOFTLETTERS = hh )

    ( [ б ] + SOFTLETTERS = bb )
    ( [ в ] + SOFTLETTERS = vv )
    ( [ г ] + SOFTLETTERS = gg )
    ( [ д ] + SOFTLETTERS = dd )
    ( [ з ] + SOFTLETTERS = zz )
    ( [ к ] + SOFTLETTERS = kk )
    ( [ л ] + SOFTLETTERS = ll )
    ( [ м ] + SOFTLETTERS = mm )
    ( [ н ] + SOFTLETTERS = nn )
    ( [ п ] + SOFTLETTERS = pp )
    ( [ р ] + SOFTLETTERS = rr )
    ( [ с ] + SOFTLETTERS = ss )
    ( [ т ] + SOFTLETTERS = tt )
    ( [ ф ] + SOFTLETTERS = ff )
    ( [ х ] + SOFTLETTERS = hh )

;; Simple consonant

    ( [ б ] = b )
    ( [ в ] = v )
    ( [ г ] = g )
    ( [ д ] = d )
    ( [ ж ] = zh )
    ( [ з ] = z )
    ( [ к ] = k )
    ( [ л ] = l )
    ( [ м ] = m )
    ( [ н ] = n )
    ( [ п ] = p )
    ( [ р ] = r )
    ( [ с ] = s )
    ( [ т ] = t )
    ( [ ф ] = f )
    ( [ х ] = h )
    ( [ ц ] = c )
    ( [ ч ] = ch )
    ( [ ш ] = sh )
    ( [ щ ] = sch )
    ( [ й ] = j )

    ( [ ъ ] =  )
    ( [ ь ] =  )
    ( [ - ] =  )
    ( [ + ] =  )
  ))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Lexicon definition
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(lex.create "msu_ru")
(lex.set.phoneset "msu_ru")
(lex.set.lts.method 'msu_ru_lts_function)
(lex.set.pre_hooks 'msu_ru_pre_hook_function)
(lex.set.post_hooks 'msu_ru_post_hook_function)
;;; If you have a compiled lexicon uncomment this
(lex.set.compile.file (path-append msu_ru_nsh::dir "dict/msu_ru_nsh_dict.scm"))
(load (path-append msu_ru_nsh::dir "dict/msu_ru_nsh_stress.tree"))
(msu_ru_addenda)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;; Lexicon setup
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (msu_ru_nsh::select_lexicon)
  "(msu_ru_nsh::select_lexicon)
Set up the lexicon for msu_ru."
  (lex.select "msu_ru")
  (set! postlex_rules_hooks (list msu_ru::postlex_reduce_vowel))
)

(define (msu_ru_nsh::reset_lexicon)
  "(msu_ru_nsh::reset_lexicon)
Reset lexicon information."
  t
)

(provide 'msu_ru_nsh_lexicon)
