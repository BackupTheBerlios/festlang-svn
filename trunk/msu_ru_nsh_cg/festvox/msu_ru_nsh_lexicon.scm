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
(lex.add.entry '("-" punc nil))
(lex.add.entry '("\"" punc nil))
(lex.add.entry '("`" punc nil))
(lex.add.entry '("?" punc nil))
(lex.add.entry '("!" punc nil))
(lex.add.entry '("--" punc nil))
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
  ( [ á ] = Á )
  ( [ â ] = Â )
  ( [ ÷ ] = × )
  ( [ ç ] = Ç )
  ( [ ä ] = Ä )
  ( [ å ] = Å )
  ( [ ³ ] = £ )
  ( [ ö ] = Ö )
  ( [ ú ] = Ú )
  ( [ é ] = É )
  ( [ ê ] = Ê ) 
  ( [ ë ] = Ë )
  ( [ ì ] = Ì )
  ( [ í ] = Í )
  ( [ î ] = Î )
  ( [ ï ] = Ï )
  ( [ ð ] = Ð )
  ( [ ò ] = Ò )
  ( [ ó ] = Ó )
  ( [ ô ] = Ô )
  ( [ õ ] = Õ )
  ( [ æ ] = Æ )
  ( [ è ] = È )
  ( [ ã ] = Ã )
  ( [ þ ] = Þ )
  ( [ û ] = Û )
  ( [ ý ] = Ý )
  ( [ ÿ ] = ß )
  ( [ ù ] = Ù )
  ( [ ø ] = Ø )
  ( [ ü ] = Ü )
  ( [ à ] = À )
  ( [ ñ ] = Ñ )

  ( [ Á ] = Á )
  ( [ Â ] = Â )
  ( [ × ] = × )
  ( [ Ç ] = Ç )
  ( [ Ä ] = Ä )
  ( [ Å ] = Å )
  ( [ £ ] = £ )
  ( [ Ö ] = Ö )
  ( [ Ú ] = Ú )
  ( [ É ] = É )
  ( [ Ê ] = Ê ) 
  ( [ Ë ] = Ë )
  ( [ Ì ] = Ì )
  ( [ Í ] = Í )
  ( [ Î ] = Î )
  ( [ Ï ] = Ï )
  ( [ Ð ] = Ð )
  ( [ Ò ] = Ò )
  ( [ Ó ] = Ó )
  ( [ Ô ] = Ô )
  ( [ Õ ] = Õ )
  ( [ Æ ] = Æ )
  ( [ È ] = È )
  ( [ Ã ] = Ã )
  ( [ Þ ] = Þ )
  ( [ Û ] = Û )
  ( [ Ý ] = Ý )
  ( [ ß ] = ß )
  ( [ Ù ] = Ù )
  ( [ Ø ] = Ø )
  ( [ Ü ] = Ü )
  ( [ À ] = À )
  ( [ Ñ ] = Ñ )

  ( [ - ] = - )
  
  ( [ 1 ] = Ï Ä É Î )
  ( [ 2 ] = Ä × Á )
  ( [ 3 ] = Ô Ò É )
  ( [ 4 ] = Þ Å Ô Ù Ò Å )
  ( [ 5 ] = Ð Ñ Ô Ø)
  ( [ 6 ] = Û Å Ó Ô Ø )
  ( [ 7 ] = Ó Å Í Ø )
  ( [ 8 ] = × Ï Ó Å Í Ø )
  ( [ 9 ] = Ä Å × Ñ Ô Ø )
  ( [ 0 ] = Î Ï Ì Ø )
  
  ( [ A ] = Á )
  ( [ B ] = Â )
  ( [ C ] = Ã )
  ( [ D ] = Ä )
  ( [ E ] = Å )
  ( [ F ] = Æ )
  ( [ G ] = Ö )
  ( [ H ] = È )
  ( [ I ] = Á Ê )
  ( [ J ] = Ä Ö É )
  ( [ K ] = Ë )
  ( [ L ] = Ì )
  ( [ M ] = Í )
  ( [ N ] = Î )
  ( [ O ] = Ï )
  ( [ P ] = Ð )
  ( [ Q ] = Ë Õ )
  ( [ R ] = Ò )
  ( [ S ] = Ó )
  ( [ T ] = Ô )
  ( [ U ] = Õ )
  ( [ V ] = × )
  ( [ W ] = × )
  ( [ X ] = É Ë Ó )
  ( [ Y ] = Á Ê )
  ( [ Z ] = Ú )

  ( [ a ] = Á )
  ( [ b ] = Â )
  ( [ c ] = Ã )
  ( [ d ] = Ä )
  ( [ e ] = Å )
  ( [ f ] = Æ )
  ( [ g ] = Ö )
  ( [ h ] = È )
  ( [ i ] = Á Ê )
  ( [ j ] = Ä Ö É )
  ( [ k ] = Ë )
  ( [ l ] = Ì )
  ( [ m ] = Í )
  ( [ n ] = Î )
  ( [ o ] = Ï )
  ( [ p ] = Ð )
  ( [ q ] = Ë Õ )
  ( [ r ] = Ò )
  ( [ s ] = Ó )
  ( [ t ] = Ô )
  ( [ u ] = Õ )
  ( [ v ] = × )
  ( [ w ] = × )
  ( [ x ] = É Ë Ó )
  ( [ y ] = Á Ê )
  ( [ z ] = Ú )

))


(define (russian_downcase word)
  "(russian_downcase WORD)
Downs case word by letter to sound rules since builtin downcase is only ascii."
  (lts.apply word 'russian_downcase))

(define (russian_contains_vowel phones)
  (cond
   ((null phones) nil)
   ((member_string (car phones) '(a e i o u y))
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
  ((and (member_string (car phones) '(a e i o u y))
	(russian_contains_vowel (cdr phones)))
   (let ((nsyls (cons (cons (car phones) (car syls)) (cdr syls)))
	 (nphones (cdr phones)))
     (cond
      ((and (member_string (car nphones) '(l ll m mm n nn r rr))
	    (not (member_string (car (cdr nphones)) 
				'(a e i o y u l ll m mm n nn r rr))))
       (set! nsyls (cons (cons (car nphones) (car nsyls)) (cdr nsyls)))
       (set! nphones (cdr nphones)))
      ((and (string-equal (car nphones) "j")
	    (not (member_string (car (cdr nphones)) 
				'(a e i o u y))))
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

(define (add_lex_stress word phones tree)
  "(add_lex_stress word syls)
Predict lexical stress by decision tree."
  (let ((utt (Utterance Text ""))
	(si)
	(stress))

    (utt.relation.create utt 'Letter)
    (set! si (utt.relation.append utt 'Letter))

    (item.set_feat si 'numsyls (count_syls phones))
    (item.set_feat si 'sylpos 1)
    (item.set_feat si 'pname "pau")
    (item.set_feat si 'last (get_last phones))

    (set! stress (add_lex_stress_syl phones si tree))
    
;;  (format t "%l\n" phones)
;;  (format t "%l\n" stress)
    stress))

;;;  Function called when word not found in lexicon
(define (msu_ru_lts_function word features)
  "(msu_ru_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."
;;  (format stderr "failed to find pronunciation for %s\n" word)
  (let (phones syls letters stress)

    (set! letters (lts.apply word 'russian_downcase))
    (set! phones (lts.apply letters 'msu_ru))

    (set! syls (russian_syllabify phones nil))

;;; More intelligently assign stress, not ready yet
    (set! stress (add_lex_stress word phones msu_ru_nsh_stress_tree))

    (set! syls 
      (cond ((and (>= (length syls) stress) (> stress 0)) 
    	   (set-car! (cdr (nth (- stress 1) syls)) 1) syls)
            (t syls)))
    
    (list word features syls)
    ))

(define (strconcat list)
    "convert list into string"
    (cond ((null list) "")
	  (t (string-append (car list) (strconcat (cdr list)))))
)
 
;;;  Function called for word in lexicon
(define (msu_ru_pre_hook_function entry)
  "(msu_ru_pre_hook_function entry)
  convert stress and word to syllables"
  (list (strconcat (lts.apply entry 'russian_downcase)))
)

;;;  Function called for word in lexicon
(define (msu_ru_post_hook_function entry)
  "(msu_ru_post_hook_function entry)
  convert stress and word to syllables"
  (let ((word (car entry))
	(pos (car (cdr entry)))
	(stress (car (car (cdr (cdr entry)))))
	syls letters phones result)
  (set! result
  (cond 
    ((or (eq? pos 'punc) (null pos)) entry)
    (t 
      (set! letters (lts.apply word 'russian_downcase))
      (set! phones (lts.apply letters 'msu_ru))
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
        ((and (member_string segment_name '(a o u i y e))
    	    (string-equal (item.feat s "R:SylStructure.parent.stress") "1"))
    		(item.set_name s (cadr (assoc segment_name '(("a" "aa") 
    				      ("o" "oo")
    				      ("y" "yy")
    				      ("i" "ii")
    				      ("e" "ee")
    				      ("u" "uu"))))))


        ((and (member_string segment_name '(a o)) 
    	      (or (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
    	          (string-equal (item.feat s "p.ph_vc") "+")))
    		    (item.set_name s "a"))

        ((and (member_string segment_name '(a o)) 
    	      (string-equal (item.feat s "R:SylStructure.parent.R:Syllable.n.stress") "1"))
    	      (if (string-equal (item.feat s "p.ph_csoft") "+")
    		    (item.set_name s "i")
    		    (item.set_name s "a")))

        ((and (member_string segment_name '(y i u)) 
    	      (or (string-equal (item.feat s "R:SylStructure.parent.R:Syllable.n.stress") "1")
    	          (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
    	          (string-equal (item.feat s "p.ph_vc" "+"))))
    	       nil)

        ((and (string-equal segment_name "e") 
    	      (or (phone_is_silence (item.feat s "p.name"))
    	          (phone_is_silence (item.feat s "n.name"))
    	          (string-equal (item.feat s "p.ph_vc" "+"))))
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

        ((string-equal segment_name "u") 
    		    (item.set_name s "ur")))))
  (utt.relation.items utt 'Segment))
    
  (mapcar
   (lambda (s)
     (let ((segment_name (item.name s)) (s1 (item.next s)))
     (if (and (string-equal "1" (item.feat s "syl_final"))
	 (> (item.feat s "R:SylStructure.parent.syl_break") 0))
     
	 (mapcar 
	  (lambda (r)
	    (if (string-equal (car r) (item.name s)) 
		(if (or 
		     (member_string (item.name s1) msu_ru::sonorlist) 
		     (phone_is_silence (item.name s1)) 
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
		      (item.set_name s (car r)))))
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
    
;;    (format stderr "%s " (item.name s))
     
      ))
   (utt.relation.items utt 'Segment))
;; (format stderr "\n")
)
   

;; You may or may not be able to write a letter to sound rule set for
;; your language.  If its largely lexicon based learning a rule
;; set will be better and easier that writing one (probably).
(lts.ruleset
 msu_ru
   (  
     (SOFTLETTERS 
	 ;;; Letter that do make previous consonant soft
            Ñ £ À É Ø Å
     )
     (STARTSYL # ß Ø Á Ñ Ï £ Õ À Ü Å É)
   )
 (
;; LTS rules



;; Some exceptions

    ( # [ Þ Ô ] Ï = sh t )
    ( Ó É Î [ Ô ] Å Ú = t )
    ( É Î [ Ô ] Å Ò Æ = t )
    ( É Î [ Ô ] Å Ò Ð = t )
    ( [ Ó Û ] Å Ó Ô = sh )
    ( # [ Å Å ] # = j e j o )
    
    ( Ó Ô Ò Ï [ Ç ] Ï  # = g )
    ( Í Î Ï [ Ç ] Ï  # = g )
    ( Ä Ï Ò Ï [ Ç ] Ï  # = g )
    ( Ë Ò Å Í Ì [ Å ] ×  = o )
     
    ( Ï [ Ç ] Ï # = v )
    ( Å [ Ç ] Ï # = v )

    ( Ì [ Ø Ï ] = j o )
    
;; Simple vowels

    ( [ Á ] = a )
    ( [ Ù ] = y )
    ( [ Ï ] = o )
    ( [ Õ ] = u )
    ( [ Ü ] = e )

;; Vovel expansion

    ( STARTSYL [ À ] = j u )
    ( STARTSYL [ Ñ ] = j a )
    ( STARTSYL [ Å ] = j e )
    ( STARTSYL [ £ ] = j o )

    ( [ £ ] = o )
    ( [ Å ] = e )
    ( [ Ñ ] = a )
    ( [ À ] = u )
    ( [ É ] = i )


;; Non readable phonems

    ( [ × ] Ó Ô × = ) 
    ( [ Ó Ô ] Ó = s )
    ( [ Ó Ô Ø ] Ó = s )
    ( [ Ò Ä ] Ã = r )
    ( [ Ò Ä ] Þ = r )
    ( [ Ì Î ] Ã = n )
    ( [ Ó Ô ] Ã = s )
    ( [ Ú Ä ] Ã = z )
    ( [ Î Ô ] Ã = n )
    ( [ Î Ä ] Ã = n )
    ( [ Ó Ô ] Î = s )    
    ( [ Ú Ä ] Î = z )    
    ( [ Î Ô ] Ó = n )    
    ( [ Î Ä ] Ó = n )    
    ( [ Î Ç ] Ô = n )    
    
;; Work around doubled consonants 

    ( [ Î ] Î = )
    ( [ Ó ] Ó = )
    ( [ Í ] Í = )
    ( [ Ë ] Ë = )

;; Basic rules
        
    ( [ Â ] SOFTLETTERS = bb )
    ( [ × ] SOFTLETTERS = vv )
    ( [ Ç ] SOFTLETTERS = gg )
    ( [ Ä ] SOFTLETTERS = dd )
    ( [ Ú ] SOFTLETTERS = zz )
    ( [ Ë ] SOFTLETTERS = kk )
    ( [ Ì ] SOFTLETTERS = ll )
    ( [ Í ] SOFTLETTERS = mm )
    ( [ Î ] SOFTLETTERS = nn )
    ( [ Ð ] SOFTLETTERS = pp )
    ( [ Ò ] SOFTLETTERS = rr )
    ( [ Ó ] SOFTLETTERS = ss )
    ( [ Ô ] SOFTLETTERS = tt )
    ( [ Æ ] SOFTLETTERS = ff )
    ( [ È ] SOFTLETTERS = hh )

;; Simple consonant

    ( [ Â ] = b )
    ( [ × ] = v )
    ( [ Ç ] = g )
    ( [ Ä ] = d )
    ( [ Ö ] = zh )
    ( [ Ú ] = z )
    ( [ Ë ] = k )
    ( [ Ì ] = l )
    ( [ Í ] = m )
    ( [ Î ] = n )
    ( [ Ð ] = p )
    ( [ Ò ] = r )
    ( [ Ó ] = s )
    ( [ Ô ] = t )
    ( [ Æ ] = f )
    ( [ È ] = h )
    ( [ Ã ] = c )
    ( [ Þ ] = ch )
    ( [ Û ] = sh )
    ( [ Ý ] = sch )
    ( [ Ê ] = j )

    ( [ ß ] =  )
    ( [ Ø ] =  )
    ( [ - ] =  )
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
