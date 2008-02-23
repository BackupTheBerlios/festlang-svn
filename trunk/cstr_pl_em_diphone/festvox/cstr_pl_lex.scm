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
;;; lexical look up, from words to pronunciations
;;;

;;;  Function called when word not found in lexicon
(define (cstr_pl_lts_function word features)
  "(cstr_pl_lts_function WORD FEATURES)
Return pronunciation of word not in lexicon."
  (let (phones syls letters)
    ;;; Here we can add multiple LTS sets if required e.g. down casing
    ;;; and normalisation of 7 bit represenations of accented characters
    (set! letters (lts.apply word 'pol_downcase))
    (set! phones (lts.apply letters 'polish))
    ;; Need to assign lexical stress
   (set! syls (polish_syllabification phones nil))
    (set! syls (polish_lex_stress syls))
    (list word nil syls)
    ))

(define (pol_contains_vowel phones)
  (cond
   ((null phones) nil)
   ((member_string (car phones) '("a" "e" "I" "i" "o" "u" "e~" "o~"))
    t)
   (t (pol_contains_vowel (cdr phones)))))

(define (polish_syllabification phones syls)
"(polish_syllabification phones syls)
Syllabify phones into syllables for Polish."
 (cond
  ((null phones)
   (mapcar
    (lambda (syl) (list (reverse syl) 0))
    (reverse syls)))
  ((and (member_string (car phones) '("a" "e" "I" "i" "o" "u" "e~" "o~"))
	(pol_contains_vowel (cdr phones)))
   (let ((nsyls (cons (cons (car phones) (car syls)) (cdr syls)))
	 (nphones (cdr phones)))
     (cond
      ((and (member_string (car nphones) '("m" "n" "n+" "N" "l" "j" "w"))
	    (not (member_string (car (cdr nphones)) 
				'("a" "e" "I" "i" "o" "u" "e~" "o~"))))
       (set! nsyls (cons (cons (car nphones) (car nsyls)) (cdr nsyls)))
       (set! nphones (cdr nphones))))
     (polish_syllabification nphones (cons nil nsyls))))
  (t
   (polish_syllabification
    (cdr phones)
    (cons (cons (car phones) (car syls)) (cdr syls))))))

(define (polish_lex_stress syls)
"(polish_lex_stress syls)
Add lexical stress to penultimate syllable."
  (if (> (length syls) 1)
      (set-car! (cdr (nth (- (length syls) 2) syls)) 1))
  syls)



(define (cstr_pl_addenda)
  "(pl_addenda)
Basic lexicon should (must ?) basic letters and punctuation."

;;; Polish lexicon
(lex.add.entry '("a" nn (((a) 0))))
(lex.add.entry '("±" nn (((o~) 0))))
(lex.add.entry '("o~" nn (((o~) 0))))
(lex.add.entry '("b" nn (((b e) 0))))
(lex.add.entry '("c" nn (((ts e) 0))))
(lex.add.entry '("æ" nn (((ts+ e) 0))))
(lex.add.entry '("c~" nn (((ts+ e) 0))))
(lex.add.entry '("d" nn (((d e) 0))))
(lex.add.entry '("e" nn (((e) 0))))
(lex.add.entry '("ê" nn (((e~) 0))))
(lex.add.entry '("e~" nn (((e~) 0))))
(lex.add.entry '("f" nn (((e f) 0))))
(lex.add.entry '("g" nn (((g j e) 0))))
(lex.add.entry '("h" nn (((h a) 0))))
(lex.add.entry '("i" nn (((i) 0))))
(lex.add.entry '("j" nn (((j o t) 0))))
(lex.add.entry '("k" nn (((k a) 0))))
(lex.add.entry '("l" nn (((e l) 0))))
(lex.add.entry '("³" nn (((e w) 0))))
(lex.add.entry '("l/" nn (((e w) 0))))
(lex.add.entry '("m" nn (((e m) 0))))
(lex.add.entry '("n" nn (((e n) 0))))
(lex.add.entry '("ñ" nn (((e n+) 0))))
(lex.add.entry '("n~" nn (((e n+) 0))))
(lex.add.entry '("o" nn (((o) 0))))
(lex.add.entry 
'("ó" nn (((o) 0) ((k r e) 0) ((s k o) 0) ((v a) 1) ((n e) 0))))
(lex.add.entry 
'("u~" nn (((o) 0) ((k r e) 0) ((s k o) 0) ((v a) 1) ((n e) 0))))
(lex.add.entry '("p" nn (((p e) 0))))
(lex.add.entry '("r" nn (((e r) 0))))
(lex.add.entry '("s" nn (((e s) 0))))
(lex.add.entry '("¶" nn (((e s+) 0))))
(lex.add.entry '("s~" nn (((e s+) 0))))
(lex.add.entry '("t" nn (((t e) 0))))
(lex.add.entry '("u" nn (((u) 0))))
(lex.add.entry '("w" nn (((v u) 0)))) ;;homograph letter w & 'in' prep
(lex.add.entry '("y" nn (((i) 1)((g r e k) 0))))
(lex.add.entry '("z" nn (((z e t) 0))))
(lex.add.entry '("¼" nn (((z+ e t) 0))))
(lex.add.entry '("z~" nn (((z+ e t) 0))))
(lex.add.entry '("¿" nn (((Z e t) 0))))
(lex.add.entry '("z*" nn (((Z e t) 0))))
(lex.add.entry '("v" nn (((f a w) 0))))
(lex.add.entry '("x" nn (((i k s) 0))))
(lex.add.entry '("q" nn (((k u) 0))))
(lex.add.entry '("rz" nn (((e r) 0) ((z e t) 1))))
(lex.add.entry '("ch" nn (((ts e) 0) ((x a ) 1))))
(lex.add.entry '("sz" nn (((e S) 0))))


(lex.add.entry 
 '("*" n (((g v i a) 1) ((s t k a) 0))))
(lex.add.entry 
 '("%" n (((p r o) 1) ((ts e n t) 0))))
;;(lex.add.entry 
;; '("&" n
(lex.add.entry 
 '("$" n (((d o) 1) ((l a r) 0))))
;;(lex.add.entry 
;; '("#" n (((x a S) 0))))
(lex.add.entry 
 '("@" n (((e t) 0))))
(lex.add.entry 
 '("+" n (((p l u s) 0))))
(lex.add.entry 
 '("^" n (((k a) 0) ((r e) 1) ((t k a) 0))))
(lex.add.entry 
 '("~" n (((t y) 1) ((l d a) 0))))
(lex.add.entry 
 '("=" n (((r u) 1) (( v n a) 0) (( s+ e ) 0)))) 
;(lex.add.entry 
; '("/" n 
;(lex.add.entry 
; '("\\" n 
;(lex.add.entry 
; '("_" n 
;(lex.add.entry 
; '("|" n 
;(lex.add.entry 
; '(">" n 
;(lex.add.entry 
; '("<" n 
;(lex.add.entry 
; '("[" n 
;(lex.add.entry 
; '("]" n 
(lex.add.entry 
 '(" " n (((s p a) 1) (( ts j a) 0))))

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

;; (lex.add.entry '("r" nn (((r o k) 0))));; depends on number etc
(lex.add.entry '("¶w" nn (((s+ f j e n) 1) ((t I) 0))))
;(lex.add.entry '("s~w" nn (((s+ f j e n) 1) ((t I) 0))))
(lex.add.entry '("tzw" nn (((t a g) 0) ((z v a) 1) ((n I) 0))))
(lex.add.entry '("ds" nn (((d o) 0) ((s p r a f) 1))))
(lex.add.entry '("dr" nn (((d o) 1) ((k t o r) 0))))
(lex.add.entry '("prof" nn (((p r o) 0) ((f e) 1) ((s o r) 0))))
(lex.add.entry '("mgr" nn (((m a) 0) ((g i) 1) ((s t e r ) 0))))
(lex.add.entry '("Ks" nn (((k s+ o n ts) 0))))
(lex.add.entry '("godz" nn (((g o) 0) ((dz+ i) 1) ((n a) 0))))
(lex.add.entry '
("tys" nn (((t I) 0) ((s+ e n) 1) ((ts I) 0)))) ;; depends on number
(lex.add.entry '
("ha" nn (((x e) 0) ((k t a) 1) ((r u f) 0))));; depends on number
(lex.add.entry '
("mln" nn (((m i) 0) ((l j o) 1) ((n u f) 0))));; depends on number
(lex.add.entry '
("proc" nn (((p r o) 1) ((ts e n t) 0))));; depends on number
(lex.add.entry '
("pkt" nn (((p u n) 1) ((k t u f) 0))))
(lex.add.entry '("z³" nn (((z w o) 1) ((t I x) 0))))
(lex.add.entry '("str" nn (((s t r o) 1) ((n a) 0))))
;;(lex.add.entry '("w." nn (((v j e k) 0))))
(lex.add.entry '
("itd" nn (((i) 0) ((t a k) 0) ((d a) 1) ((l e j) 0)))) 
(lex.add.entry 
'("itp" nn (((i) 0) (( t I m) 0) ((p o) 0) ((d o) 1) ((b n e) 0))))   
(lex.add.entry 
'("m#in" nn (((m j e n) 1) ((dz I) 0) ((#) 0) ((i n) 0) ((n I) 1) ((m i) 0))))
;; how to make sure these are two words
(lex.add.entry 
'("n.e" nn (((n a) 1) ((S e j) 0) ((e) 1) ((r I) 0))))
(lex.add.entry 
'("np" nn (((n a) 0) ((p S I) 1) ((k w a t) 0))))
(lex.add.entry 
'("ok" nn (((o) 0) ((k o) 1) ((w o) 0))))
(lex.add.entry 
'("p.n.e" nn (((p S e t) 0) ((n a) 1) ((S o~) 0) ((#) 0) ((e) 1) ((r o~) 0))))
(lex.add.entry 
'("fot" nn (((f o) 0) ((t o) 0) ((g r a 1)) ((f j a) 0))))
(lex.add.entry 
'("cstr" nn (((s i) 1) ((j e s) 1) ((t i) 0) ((j a r) 1)))) 
(lex.add.entry 
'("jab³ko" nn (((j a) 1) ((p k o) 0))))
(lex.add.entry 
'("jabl/ko" nn (((j a) 1) ((p k o) 0)))) 
(provide 'pollex)
;;exceptions : numbers
(lex.add.entry 
'("piêædziesi±t" nn (((p j e n+) 0) ((dz+ e) 1) ((s+ o n t) 0))))
(lex.add.entry 
'("pie~c~dziesio~t" nn (((p j e n+) 0) ((dz+ e) 1) ((s+ o n t) 0))))
(lex.add.entry 
'("sze¶ædziesi±t" nn (((S e) 0) ((z+ dz+ e) 1) ((s+ o n t) 0))))
(lex.add.entry 
'("szes~c~dziesio~t" nn (((S e) 0) ((z+ dz+ e) 1) ((s+ o n t) 0))))
(lex.add.entry 
'("piêæset" nn (((p j e n+) 1) ((ts e t) 0))))
(lex.add.entry 
'("pie~c~set" nn (((p j e n+) 1) ((ts e t) 0))))
(lex.add.entry 
'("sze¶æset" nn (((S e j) 1) ((s e t) 0))))
(lex.add.entry 
'("szes~c~set" nn (((S e j) 1) ((s e t) 0))))
(lex.add.entry
'("dziewiêædziesi±t" 
nn (((dz+ e) 0) ((v j e n+) 0) ((dz+ e) 1) ((s+ o n t) 0))))
(lex.add.entry
'("dziewiêæset" nn (((dz+ e) 1) ((v j e n+) 0) ((ts e t) 0))))
(lex.add.entry
'("dziewie~c~dziesio~t" 
nn (((dz+ e) 0) ((v j e n+) 0) ((dz+ e) 1) ((s+ o n t) 0))))
(lex.add.entry
'("dziewie~c~set" nn (((dz+ e) 1) ((v j e n+) 0) ((ts e t) 0))))
(lex.add.entry 
'("miêkki" nn (((m j e N) 1) ((k i) 0))))
(lex.add.entry 
'("mie~kki" nn (((m j e N) 1) ((k i) 0))))
(lex.add.entry 





;;    MW

'("rmf" nn (((e r)0) ((e m) 1) ((e f) 0))))
(lex.add.entry 
'("psl" nn (((p e)0) ((e s) 1) ((e l) 0))))
(lex.add.entry 
'("onz" nn (((o)0) ((e n) 1) ((z e t) 0))))
(lex.add.entry 
'("pkp" nn (((p e)0) ((k a) 1) ((p e) 0))))
(lex.add.entry 
'("msz" nn (((e m)0) ((e s) 1) ((z e t) 0))))
(lex.add.entry 
'("prl" nn (((p e)0) ((e r) 1) ((e l) 0))))
(lex.add.entry 
'("sdrp" nn (((e s)0) ((d e) 0) ((e r) 1)((p e) 0))))
(lex.add.entry 
'("pkz" nn (((p e)0) ((k a) 1) ((z e t) 0))))
(lex.add.entry 
'("nfi" nn (((e n)0) ((e f) 1) ((i) 0))))
(lex.add.entry 
'("fair" nn (((f e r) 0))))
(lex.add.entry 
'("pzpn" nn (((p e)0) ((z e t) 0) ((p e) 1)((e n) 0))))
(lex.add.entry 
'("cwks" nn (((ts e)0) ((v u) 0) ((k a) 1)((e s) 0))))
(lex.add.entry 
'("usa" nn (((u)0) ((e s) 1) ((a) 0))))
(lex.add.entry 
'("oecd" nn (((o)0) ((e) 0) ((ts e) 1)((d e) 0))))
(lex.add.entry 
'("prg" nn (((p e)0) ((e r) 1) ((g j e) 0))))
(lex.add.entry 
'("pbsz" nn (((p e)0) ((b e)0) ((e s) 1) ((z e t) 0))))
(lex.add.entry 
'("ub" nn (((u) 1) ((b e) 0))))
(lex.add.entry 
'("bbwr" nn (((b e)0) ((b e)0) ((v u) 1) ((e r) 0))))
(lex.add.entry 
'("msw" nn (((e m)0) ((e s) 1) ((v u) 0))))
(lex.add.entry 
'("sld" nn (((e s)0) ((e l) 1) ((d e) 0))))
(lex.add.entry 
'("bhp" nn (((b e)0) ((x a) 1) ((p e) 0))))
(lex.add.entry 
'("dokp" nn (((d e)0) ((o)0) ((k a) 1) ((p e) 0))))
(lex.add.entry 
'("wtz" nn (((v u)0) ((t e) 1) ((z e t) 0))))
(lex.add.entry 
'("pnb" nn (((p e)0) ((e n) 1) ((b e) 0))))
(lex.add.entry 
'("odzierzo~" nn (((o)0) ((dz+ e) 1) ((Z o~) 0))))
(lex.add.entry 
'("kc" nn (((k a) 1) ((ts e) 0))))
(lex.add.entry 
'("pzpr" nn (((p e)0) ((z e t)0) ((p e) 1) ((e r) 0))))
(lex.add.entry 
'("mtp" nn (((e m)0) ((t e) 1) ((p e) 0))))
(lex.add.entry 
'("mfw" nn (((e m)0) ((e f) 1) ((v u) 0))))
(lex.add.entry 
'("rp" nn (((e r) 1) ((p e) 0))))
(lex.add.entry 
'("hbo" nn (((x a)0) ((b e) 1) ((o) 0))))
(lex.add.entry 
'("kbwe" nn (((k a)0) ((b e)0) ((v u) 1) ((e) 0))))
(lex.add.entry 
'("gps" nn (((dZ i)0) ((p i) 1) ((e s) 0))))
(lex.add.entry 
'("cpn" nn (((ts e)0) ((p e) 1) ((e n) 0))))
(lex.add.entry 
'("pps" nn (((p e)0) ((p e) 1) ((e s) 0))))
(lex.add.entry 
'("mwgzz" nn (((e m)0) ((v u)0) ((g j e)0) ((z e t) 1) ((z e t) 0))))
(lex.add.entry 
'("kbw" nn (((k a)0) ((b e) 1) ((v u) 0))))
(lex.add.entry 
'("kgb" nn (((k a)0) ((g j e) 1) ((b e) 0))))
(lex.add.entry 
'("ll" nn (((e l) 1)((e l) 0))))
(lex.add.entry 
'("kgb" nn (((k a)0) ((g j e) 1) ((b e) 0))))
(lex.add.entry 
'("dna" nn (((d e)0) ((e n) 1) ((a) 0))))
(lex.add.entry 
'("nbp" nn (((e n) 0) ((b e) 1) ((p e) 0))))
(lex.add.entry 
'("grp" nn (((g j e) 0)((e r) 1)((p e) 0))))
(lex.add.entry 
'("nkwd" nn (((e n)0) ((k a)0) ((v u) 1) ((d e) 0))))
(lex.add.entry 
'("hr" nn (((x a) 1)((e r) 0))))
(lex.add.entry 
'("wdr" nn (((v u) 0)((d e) 1)((e r) 0))))
(lex.add.entry 
'("usa" nn (((u) 0)((w e s) 1)((a) 0))))
(lex.add.entry 
'("sssr" nn (((e s)0) ((e s)0) ((e s) 1) ((e r) 0))))
(lex.add.entry 
'("zsrr" nn (((z e t)0) ((e s)0) ((e r) 1) ((e r) 0))))
(lex.add.entry 
'("nrd" nn (((e n) 0)((e r) 1)((d e) 0))))
(lex.add.entry 
'("nrfu" nn (((e n)0) ((e r)0) ((e) 1) ((f u) 0))))
(lex.add.entry 
'("awsu" nn (((a)0) ((v u)0) ((e) 1) ((s u) 0))))
(lex.add.entry 
'("pgr" nn (((p e) 0)((g j e) 1)((e r) 0))))
(lex.add.entry 
'("ltd" nn (((e l) 0)((t e) 1)((d e) 0))))
(lex.add.entry 
'("zhp" nn (((z e t) 0)((x a) 1)((p e) 0))))
(lex.add.entry 
'("bbn" nn (((b e) 0)((b e) 1)((e n) 0))))
(lex.add.entry 
'("nsa" nn (((e n) 0)((e s) 1)((a) 0))))
(lex.add.entry 
'("kbn" nn (((k a) 0)((b e) 1)((e n) 0))))
(lex.add.entry 
'("rsw" nn (((e r) 0)((e s) 1)((v u) 0))))
(lex.add.entry 
'("eb" nn (((e) 1)((b e) 0))))
(lex.add.entry 
'("nmt" nn (((e n) 0)((e m) 1)((t e) 0))))
(lex.add.entry 
'("gsm" nn (((g j e) 0)((e s) 1)((e m) 0))))
(lex.add.entry 
'("krr" nn (((k a) 0)((e r) 1)((e r) 0))))
(lex.add.entry 
'("kpn" nn (((k a) 0)((p e) 1)((e n) 0))))
(lex.add.entry 
'("psc" nn (((p e) 0)((e s) 1)((ts e) 0))))
(lex.add.entry 
'("pzz" nn (((p e) 0)((z e t) 1)((z e t) 0))))
(lex.add.entry 
'("bg¯" nn (((b e) 0)((g j e) 1)((Z e t) 0))))
(lex.add.entry 
'("pfron" nn (((p e) 1)((f r o n) 0))))


(lex.add.entry 
'("jr" nn (((j o t) 1)((e r) 0))))




(lex.add.entry 
'("dvd" nn (((d i) 0)((v i) 1)((d i) 0))))  
(lex.add.entry 
'("cia" nn (((s i) 0)((a j) 1)((e j) 0)))) 
(lex.add.entry 
'("fbi" nn (((e f) 0)((b i) 1)((a j) 0))))
(lex.add.entry 
'("rnb" nn (((a r) 0)((e n) 1)((b i) 0))))
(lex.add.entry 
'("mtv" nn (((e m) 0)((t i) 1)((v i) 0))))
(lex.add.entry 
'("bbc" nn (((b i) 0)((b i) 1)((s i) 0))))



;; ang i imiona
(lex.add.entry 
'("pipes" nn (((p a j)1) ((p s)0) )))
(lex.add.entry 
'("jimmy" nn (((dZ i) 1)((m i) 0))))
(lex.add.entry 
'("carter" nn (((k a r)1) ((t e r) 0))))
(lex.add.entry 
'("happy" nn (((x e)1) ((p i) 0))))
(lex.add.entry 
'("phare" nn (((f a) 1) ((r e) 0))))
(lex.add.entry 
'("connolly" nn (((k o n) 0) ((n o) 1) ((l i) 0))))
(lex.add.entry 
'("quo" nn (((k f o) 0))))
(lex.add.entry 
'("tokyo" nn (((t o k) 1) ((i o) 0))))
(lex.add.entry 
'("mitsubishi" nn (((m i t) 0) ((s u) 0) ((b i) 1) ((S i) 0))))
(lex.add.entry 
'("einstein" nn (((a j n) 1) ((S t a j n) 0))))
(lex.add.entry 
'("weekend" nn (((w i k) 1) ((e n t) 0))))
(lex.add.entry 
'("weekendu" nn (((w i k) 0) ((e n) 1) ((d u) 0))))
(lex.add.entry 
'("conditio" nn (((k o n) 0) ((d i) 1) ((t j o) 0))))
(lex.add.entry 
'("sine" nn (((s i n) 0))))
(lex.add.entry 
'("qua" nn (((k w a) 0))))

(lex.add.entry 
'("holly" nn (((x o) 1) ((l i) 0))))
(lex.add.entry 
'("large" nn (((l a r tS) 0))))
(lex.add.entry 
'("the" nn (((d e) 0))))
(lex.add.entry 
'("dutch" nn (((d a tS) 0))))
(lex.add.entry 
'("disease" nn (((d i) 1) ((z i s) 0))))
(lex.add.entry 
'("alpha" nn (((a l) 1) ((f a) 0))))
(lex.add.entry 
'("jet" nn (((dZ e t) 0))))
(lex.add.entry 
'("schulza" nn (((S u l) 1) ((ts a) 0))))
(lex.add.entry 
'("dz*itsu" nn (((dZ i ts) 1) ((s u) 0))))
(lex.add.entry 
'("wie~z~ba" nn (((v j e~ z+) 1) ((b a) 0))))
(lex.add.entry 
'("iveco" nn (((i)0) ((v e) 1) ((k o) 0))))
(lex.add.entry 
'("coltranea" nn (((k o l) 0) ((t r a) 1)((n a) 0))))
(lex.add.entry 
'("milesa" nn (((m a j) 0) ((l e) 1)((s a) 0))))
(lex.add.entry 
'("franza" nn (((f r a n) 1)((ts a) 0))))
(lex.add.entry 
'("dokey" nn (((d o) 1)((k e j) 0))))
(lex.add.entry 
'("cool" nn (((k u l) 0))))
(lex.add.entry 
'("icchakiem" nn (((i) 0)((tS a) 1)((k j e m) 0))))
(lex.add.entry 
'("bowie" nn (((b o) 1)((w i) 0))))
(lex.add.entry 
'("davidem" nn (((d e j) 0)((v i) 1)((d e m) 0))))
(lex.add.entry 
'("yengibarjanem" nn (((j e N) 0) ((g i) 0) ((b a r) 0)((j a) 1) ((n e m) 0))))
(lex.add.entry 
'("einsteina" nn (((a j n) 0)((S t a j) 1)((n a) 0))))
(lex.add.entry 
'("deeds" nn (((d e t s) 0))))
(lex.add.entry 
'("beatles" nn (((b i) 1)((t e l s) 0))))
(lex.add.entry 
'("lowell" nn (((l a) 1)((w e l) 0))))
(lex.add.entry 
'("ganz" nn (((g a n ts) 0))))
(lex.add.entry 
'("babaloo" nn (((b a) 0)((b a) 1)((l u) 0))))
(lex.add.entry 
'("glucksmann" nn (((g l u k s) 1)((m a n n) 0))))
(lex.add.entry 
'("sic" nn (((s i k) 0))))
(lex.add.entry 
'("manhattanie" nn (((m a n) 0)((x a) 0)((t a) 1)((n+ e) 0))))
(lex.add.entry 
'("amused" nn (((e) 1)((m j u s t) 0))))
(lex.add.entry 
'("death" nn (((d e f) 0))))
(lex.add.entry 
'("floyd" nn (((f l o j t) 0))))
(lex.add.entry 
'("grapea" nn (((g r e j) 1)((p a) 0))))
(lex.add.entry 
'("human" nn (((x j u) 1)((m a n) 0))))
(lex.add.entry 
'("behaviour" nn (((b i) 0)((x e j) 1)((v j o r) 0))))
(lex.add.entry 
'("violenty" nn (((v a) 0)((j o) 0)((l e n) 1)((t i) 0))))
(lex.add.entry 
'("boy" nn (((b o j) 0))))
(lex.add.entry 
'("anchor" nn (((e n) 1)((k e r) 0))))
(lex.add.entry 
'("maurizio" nn (((m a w) 0)((r i ts) 1)((i o) 0))))
 (lex.add.entry 
'("brian" nn (((b r a) 1)((j a n) 0))))
(lex.add.entry 
'("pagliacci" nn (((p a g) 0)((l j a) 1)((tS i) 0))))
(lex.add.entry 
'("ruggero" nn (((r u) 0)((dZ e) 1)((r o) 0))))
(lex.add.entry 
'("cavallo" nn (((k a) 0)((v a l) 1)((l o) 0))))
(lex.add.entry 
'("robbie" nn (((r o) 1)((b i) 0))))
(lex.add.entry 
'("williams" nn (((w i l) 1)((j a m s) 0))))
(lex.add.entry 
'("show" nn (((S o w) 0))))
(lex.add.entry 
'("michael" nn (((m a j) 1)((k e l) 0))))
(lex.add.entry 
'("jackson" nn (((dZ e k) 1)((s o n) 0))))
(lex.add.entry 
'("somewhere" nn (((s a m) 1)((w e r) 0))))
(lex.add.entry 
'("belong" nn (((b i) 1)((l o N k) 0))))
(lex.add.entry 
'("lying" nn (((l a) 1)((i n g) 0))))
(lex.add.entry 
'("you" nn (((j u) 0))))
(lex.add.entry 
'("axl" nn (((a k) 1)((s e l) 0))))
(lex.add.entry 
'("rose" nn (((r o s) 0))))
(lex.add.entry 
'("slash" nn (((s l e S) 0))))
(lex.add.entry 
'("sound" nn (((s a w n t) 0))))
(lex.add.entry 
'("mayslesuw" nn (((m e j) 0)((s l e) 1)((s u f) 0))))  
(lex.add.entry 
'("jazzowej" nn (((dZ e) 0)((z o) 1)((v e j) 0)))) 
(lex.add.entry 
'("carlos" nn (((k a r) 1)((l o s) 0))))
(lex.add.entry 
'("sideman" nn (((s a j d) 1)((m a n) 0))))
(lex.add.entry 
'("moonlight" nn (((m u n) 1)((l a j t) 0))))
(lex.add.entry 
'("shadow" nn (((S a) 1)((d o w) 0))))
(lex.add.entry 
'("shadows" nn (((S a) 1)((d o w s) 0))))
(lex.add.entry 
'("wall" nn (((w o l) 0))))
(lex.add.entry 
'("bloodflowers" nn (((b l a d) 0)((f l a) 1)((w e r s) 0))))    
(lex.add.entry 
'("francisco" nn (((f r a n) 0)((s i s) 1)((k o) 0))))
(lex.add.entry 
'("stew" nn (((s t j u) 0))))
(lex.add.entry 
'("hamm" nn (((x a m) 0))))
(lex.add.entry 
'("matt" nn (((m a t) 0))))
(lex.add.entry 
'("bisson" nn (((b i) 1)((s o n) 0))))
(lex.add.entry 
'("jeff" nn (((dZ e f) 0)))) 
(lex.add.entry 
'("campitelli" nn (((k a m) 0)((p i) 0)((t e l) 1)((l i) 0))))
(lex.add.entry 
'("pellingtona" nn (((p e) 0)((l i N g) 0)((t o) 1)((n a) 0))))
(lex.add.entry 
'("all" nn (((o l) 0)))) 
(lex.add.entry 
'("smash" nn (((s m a S) 0))))
(lex.add.entry 
'("mouth" nn (((m a w f) 0))))
(lex.add.entry 
'("way" nn (((w e j) 0))))
(lex.add.entry 
'("proclaimers" nn (((p r o) 0)((k l e j) 1)((m e r s) 0))))
(lex.add.entry 
'("jazzu" nn (((dZ e) 1)((z u) 0))))
(lex.add.entry 
'("aarona" nn (((a) 0)((r o) 1)((n a) 0))))
(lex.add.entry 
'("coplanda" nn (((k o p) 0)((l a n) 1)((d a) 0))))
(lex.add.entry 
'("jack" nn (((dZ e k) 0))))
(lex.add.entry 
'("fate" nn (((f e j t) 0))))
(lex.add.entry 
'("weekendowych" nn (((w i) 0)((k e n) 0)((d o) 1)((v I x) 0))))
(lex.add.entry 
'("sutherlanda" nn (((s a) 0)((d e r) 0)((l a n) 1)((d a) 0))))
(lex.add.entry 
'("davidovich" nn (((d a) 0)((v i) 0)((d o) 1)((v i tS) 0))))
(lex.add.entry 
'("julie" nn (((dZ u) 1)((l j e) 0))))
(lex.add.entry 
'("delpy" nn (((d e l) 1)((p i) 0))))
(lex.add.entry 
'("petera" nn (((p i) 0)((t e) 1)((r a) 0))))
(lex.add.entry 
'("weira" nn (((w e j) 1)((r a) 0))))
(lex.add.entry 
'("jean" nn (((Z a n) 0))))
(lex.add.entry 
'("paul" nn (((p o l) 0))))
(lex.add.entry 
'("sequele" nn (((s e) 0)((k f e) 1)((l e) 0))))
(lex.add.entry 
'("krauze" nn (((k r a w) 1)((z e) 0))))
(lex.add.entry 
'("hollywood" nn (((x o) 0)((l I) 1)((w u t) 0))))
(lex.add.entry 
'("mansfieldem" nn (((m a n s) 0)((f i l) 1)((d e m) 0))))
(lex.add.entry 
'("tarzan" nn (((t a r) 1)((z a n) 0))))
(lex.add.entry 
'("rocky" nn (((r o) 1)((k i) 0))))
(lex.add.entry 
'("elizabeth" nn (((e) 0)((l i z a) 1)((b e t) 0))))
(lex.add.entry 
'("mcneill" nn (((m a k) 1)((n e j l) 0))))
(lex.add.entry 
'("pierce" nn (((p i r s) 0))))
(lex.add.entry 
'("hills" nn (((x i l s) 0))))
(lex.add.entry 
'("lubitsch" nn (((l u) 1)((b i tS) 0))))
(lex.add.entry 
'("angeli" nn (((a n) 0)((dZ e) 1)((l i) 0))))
(lex.add.entry 
'("pope" nn (((p o w p) 0))))
(lex.add.entry 
'("downey" nn (((d a w) 1)((n e j) 0))))
(lex.add.entry 
'("molly" nn (((m o) 1)((l i) 0))))
(lex.add.entry 
'("baseballowy" nn (((b e j z) 0)((b o) 0)((l o) 1)((v I) 0))))
(lex.add.entry 
'("nicholas" nn (((n+ i) 0)((k o) 1)((l a s) 0))))
(lex.add.entry 
'("conradem" nn (((k o n) 0)((r a) 1)((d e m) 0))))
(lex.add.entry 
'("sean" nn (((S o n) 0))))
(lex.add.entry 
'("penn" nn (((p e n) 0))))
(lex.add.entry 
'("james" nn (((dZ e j m s) 0))))
(lex.add.entry 
'("bossuw" nn (((b o) 1)((s u f) 0))))
(lex.add.entry 
'("john" nn (((dZ o n) 0))))
(lex.add.entry 
'("hughes" nn (((x a g s) 0))))
(lex.add.entry 
'("oak" nn (((o w k) 0))))
(lex.add.entry 
'("line" nn (((l a j n) 0))))
(lex.add.entry 
'("jonathana" nn (((j o) 0)((n a) 0)((t a) 1)((n a) 0))))
(lex.add.entry 
'("trencha" nn (((t r e n) 1)((tS a) 0))))
(lex.add.entry 
'("blade" nn (((b l e j d) 0))))
(lex.add.entry 
'("wesley" nn (((w e s) 1)((l e j) 0))))
(lex.add.entry 
'("snipes" nn (((s n a j p s) 0))))
(lex.add.entry 
'("stephen" nn (((s t e) 1)((f e n) 0))))
(lex.add.entry 
'("dorff" nn (((d o r f) 0))))
(lex.add.entry 
'("romancy" nn (((r o) 0)((m a n) 1)((s i) 0))))
(lex.add.entry 
'("vonneguta" nn (((v o n) 0)((g u) 1)((t a) 0))))
(lex.add.entry 
'("felice" nn (((f e) 1)((l i s) 0))))
(lex.add.entry 
'("lilly" nn (((l i) 1)((l i) 0))))
(lex.add.entry 
'("schrader" nn (((S r e) 1)((d e r) 0))))
(lex.add.entry 
'("juliane" nn (((j u l) 1)((j a n) 0))))
(lex.add.entry 
'("kyhler" nn (((k I) 1)((l a) 0))))
(lex.add.entry 
'("johanna" nn (((j o) 0)((a n) 1)((n a) 0))))
(lex.add.entry 
'("heike" nn (((x a j) 1)((k e) 0))))
(lex.add.entry 
'("makatsch" nn (((m a) 1)((k a tS) 0))))
(lex.add.entry 
'("heike" nn (((x a j) 1)((k e) 0))))
(lex.add.entry 
'("detlev" nn (((d e t) 1)((l j u) 0))))
(lex.add.entry 
'("buck" nn (((b u k) 0))))
(lex.add.entry 
'("humphreyem" nn (((x a m) 0)((f r e) 1)((j e m) 0))))
(lex.add.entry 
'("sheridan" nn (((S e) 0)((r i) 1)((d a n) 0)))) 
(lex.add.entry 
'("roycea" nn (((r o j) 1)((s a) 0))))
(lex.add.entry 
'("downeya" nn (((d a w) 0)((n e) 1)((j a) 0))))
(lex.add.entry 
'("payne" nn (((p e j n) 0))))
(lex.add.entry 
'("becky" nn (((b e) 1)((k i) 0))))
(lex.add.entry 
'("angeles" nn (((a n) 0)((dZ e) 1)((l e s) 0))))
(lex.add.entry 
'("endre" nn (((e n d r) 0))))
(lex.add.entry 
'("marianne" nn (((m a r) 1)((j a n) 0))))
(lex.add.entry 
'("thrillera" nn (((t r i) 0)((l e) 1)((r a) 0))))
(lex.add.entry 
'("maine" nn (((m e j n) 0))))
(lex.add.entry 
'("costner" nn (((k o s t) 1)((n e r) 0))))
(lex.add.entry 
'("sonny" nn (((s o) 1)((n I) 0))))
(lex.add.entry 
'("pepper" nn (((p e) 1)((p e r) 0))))
(lex.add.entry 
'("ss" nn (((e s) 1)((e s) 0))))
(lex.add.entry 
'("walwrighta" nn (((w a l) 0)((r a j) 1)((t a) 0))))
(lex.add.entry 
'("joel" nn (((dZ o) 1)((e l) 0))))
(lex.add.entry 
'("campbell" nn (((k a m) 1)((b e l) 0))))
(lex.add.entry 
'("thor" nn (((t o r) 0))))
(lex.add.entry 
'("carlsson" nn (((k a r l) 1)((s o n) 0))))
(lex.add.entry 
'("von" nn (((f o n) 0))))
(lex.add.entry 
'("crosby" nn (((k r o s) 1)((b i) 0))))
(lex.add.entry 
'("sinatra" nn (((s i) 0)((n a t) 1)((r a) 0))))
(lex.add.entry 
'("darryl" nn (((d a) 1)((r I l) 0))))
(lex.add.entry 
'("straight" nn (((s t r e j t) 0))))
(lex.add.entry 
'("moultona" nn (((m o l) 0)((t o) 1)((n a) 0))))
(lex.add.entry 
'("barretta" nn (((b a) 0)((r e t) 1)((t a) 0))))
(lex.add.entry 
'("janet" nn (((dZ a) 1)((n e t) 0))))
(lex.add.entry 
'("hoffmana" nn (((x o f) 0)((m a) 1)((n a) 0))))
(lex.add.entry 
'("marthy" nn (((m a r) 1)((t i) 0))))
(lex.add.entry 
'("coolidge" nn (((k u) 1)((l i dZ) 0))))
(lex.add.entry 
'("blacky" nn (((b l e) 1)((k i) 0))))
(lex.add.entry 
'("picassa" nn (((p i) 0)((k a s) 1)((s a) 0))))
(lex.add.entry 
'("stein" nn (((S t a j n) 0))))
(lex.add.entry 
'("george" nn (((dZ o r dZ) 0))))
(lex.add.entry 
'("lucas" nn (((l u) 1)((k a s) 0))))
(lex.add.entry 
'("shepherd" nn (((S e) 1)((p a r t) 0))))
(lex.add.entry 
'("cohen" nn (((k o) 1)((e n) 0))))
(lex.add.entry 
'("roy" nn (((r o j) 0))))
(lex.add.entry 
'("soderbergh" nn (((s o) 0)((d e r) 1)((b e r g) 0))))
(lex.add.entry 
'("crash" nn (((k r e S) 0))))
(lex.add.entry 
'("cannes" nn (((k a n) 0))))
(lex.add.entry 
'("vincent" nn (((v i n) 1)((s e n t) 0))))
(lex.add.entry 
'("cercle" nn (((s e r l) 0))))
(lex.add.entry 
'("rouge" nn (((r u S) 0))))
(lex.add.entry 
'("saturday" nn (((s a) 0)((t u r) 1)((d e j) 0))))
(lex.add.entry 
'("night" nn (((n a j t) 0))))
(lex.add.entry 
'("live" nn (((l a j f) 0))))
(lex.add.entry 
'("capitol" nn (((k a) 0)((p i) 1)((t o l) 0))))
(lex.add.entry 
'("pictures" nn (((p i k) 1)((tS e r s) 0))))
(lex.add.entry 
'("eiffla" nn (((a j) 1)((f l a) 0))))
(lex.add.entry 
'("pulp" nn (((p a l p) 0))))
(lex.add.entry 
'("fiction" nn (((f i k) 1)((S I n) 0))))
(lex.add.entry 
'("andie" nn (((a n) 1)((d i) 0))))
(lex.add.entry 
'("macdowell" nn (((m a k) 0)((d a) 1)((w e l) 0))))
(lex.add.entry 
'("joe" nn (((dZ o w) 0))))
(lex.add.entry 
'("conchita" nn (((k o n) 0)((tS i) 1)((t a) 0))))
(lex.add.entry 
'("nick" nn (((n+ i k) 0))))
(lex.add.entry 
'("steven" nn (((s t i) 1)((v e n) 0))))
(lex.add.entry 
'("schub" nn (((S u p) 0))))
(lex.add.entry 
'("dannyego" nn (((d e) 0)((n i e) 1)((g o) 0))))
(lex.add.entry 
'("yentl" nn (((j e n t l) 0))))
(lex.add.entry 
'("yvonne" nn (((i) 1)((v o n) 0))))
(lex.add.entry 
'("charlie" nn (((i) 1)((v o n) 0))))
(lex.add.entry 
'("yvonne" nn (((i) 1)((v o n) 0))))
(lex.add.entry 
'("charlie" nn (((tS e r) 1)((l i) 0))))
(lex.add.entry 
'("bobby" nn (((b o) 1)((b i) 0))))
(lex.add.entry 
'("rayburn" nn (((r e j) 1)((b e r n) 0))))
(lex.add.entry 
'("baseballisto~" nn (((b e j z) 0)((b o) 0)((l i s) 1)((t o~) 0))))
(lex.add.entry 
'("saegebrecht" nn (((z a) 0)((g e) 1)((b r e x t) 0))))
(lex.add.entry 
'("fette~" nn (((f e) 1)((t e~) 0))))
(lex.add.entry 
'("brooks" nn (((b r u k s) 0))))
(lex.add.entry 
'("richard" nn (((r i) 1)((tS a r t) 0))))
(lex.add.entry 
'("gottlieb" nn (((g o t) 1)((l i p) 0))))
(lex.add.entry 
'("josepha" nn (((dZ o) 0)((z e) 1)((f a) 0))))
(lex.add.entry 
'("kesselringa" nn (((k e) 0)((s e l) 0)((r i N) 1)((g a) 0))))
(lex.add.entry 
'("selznicka" nn (((s e l z) 0)((n+ i) 1)((k a) 0))))
(lex.add.entry 
'("scotta" nn (((s k o) 1)((t a) 0))))
(lex.add.entry 
'("fitzgeralda" nn (((f i dz) 0)((Z e) 0)((r a l) 1)((d a) 0))))
(lex.add.entry 
'("whitfielda" nn (((w i t) 0)((f i l) 1)((d a) 0))))
(lex.add.entry 
'("hitchcocka" nn (((x i tS) 0)((k o) 1)((k a) 0))))
(lex.add.entry 
'("ice" nn (((a j s) 0))))
(lex.add.entry 
'("cube" nn (((k j u b) 0))))
(lex.add.entry 
'("jacquesa" nn (((Z a) 1)((k a) 0))))
(lex.add.entry 
'("tourniera" nn (((t u r) 0)((n+ e) 1)((r a) 0))))
(lex.add.entry 
'("jeanne" nn (((Z a n) 0))))
(lex.add.entry 
'("luynes" nn (((l a j n) 0))))
(lex.add.entry 
'("comtesse" nn (((k o m) 1)((t i s) 0))))
(lex.add.entry 
'("verue" nn (((v e r u) 0))))
(lex.add.entry 
'("patricka" nn (((p a t) 0)((r i) 1)((k a) 0))))
(lex.add.entry 
'("kelly" nn (((k e) 1)((l i) 0))))
(lex.add.entry 
'("strozziego" nn (((s t r o ts) 0)((ts i e) 1)((g o) 0))))
(lex.add.entry 
'("eisenbergera" nn (((a j) 0)((s e n) 0)((b e r) 0)((g e) 1)((r a) 0))))
(lex.add.entry 
'("leigh" nn (((l a j k) 0))))
(lex.add.entry 
'("eddie" nn (((e) 1)((d i) 0))))
(lex.add.entry 
'("bodyguard" nn (((b o) 0)((d I) 1)((g a r t) 0))))
(lex.add.entry 
'("orazio" nn (((o) 0)((r a) 1)((z j o) 0))))
(lex.add.entry 
'("kevina" nn (((k e) 0)((v i) 1)((n a) 0))))
(lex.add.entry 
'("smitha" nn (((s m i) 1)((f a) 0))))
(lex.add.entry 
'("clerks" nn (((k l e r k s) 0))))
(lex.add.entry 
'("sheila" nn (((S e j) 1)((l a) 0))))
(lex.add.entry 
'("bisset" nn (((b i) 1)((s e t) 0))))
(lex.add.entry 
'("jacqueline" nn (((Z a k) 1)((l i n) 0))))
(lex.add.entry 
'("anthony" nn (((a n) 0)((t o) 1)((n i) 0))))
(lex.add.entry 
'("theresa" nn (((t e) 0)((r e) 1)((s a) 0))))
(lex.add.entry 
'("bessie" nn (((b e s) 1)((s i) 0))))
(lex.add.entry 
'("sadie" nn (((s e) 1)((d i) 0))))
(lex.add.entry 
'("delany" nn (((d e) 0)((l a) 1)((n i) 0))))
(lex.add.entry 
'("paula" nn (((p o) 1)((l a) 0))))
(lex.add.entry 
'("branco" nn (((b r a n) 1)((k o) 0))))
(lex.add.entry 
'("cukor" nn (((s u) 1)((k o r) 0))))
(lex.add.entry 
'("jeepem" nn (((dZ i) 1)((p e m) 0))))
(lex.add.entry 
'("danceny" nn (((d a n) 0)((s e) 1)((n i) 0))))
(lex.add.entry 
'("keanu" nn (((k i) 0)((j a) 1)((n o) 0))))
(lex.add.entry 
'("reeves" nn (((r i f s) 0))))
(lex.add.entry 
'("jamesa" nn (((dZ e j m) 1)((s a) 0))))
(lex.add.entry 
'("ivory" nn (((a j) 0)((v o) 1)((r I) 0))))
(lex.add.entry 
'("schaffnera" nn (((S a f) 0)((n e) 1)((r a) 0))))
(lex.add.entry 
'("duvall" nn (((d u) 1)((v a l) 0))))
(lex.add.entry 
'("earl" nn (((e r l) 0))))
(lex.add.entry 
'("pilcher" nn (((p i l) 1)((tS e r) 0))))
(lex.add.entry 
'("lee" nn (((l i) 0))))
(lex.add.entry 
'("lyn" nn (((l i n) 0))))
(lex.add.entry 
'("ralpha" nn (((r a l) 1)((f a) 0))))
(lex.add.entry 
'("bakshi" nn (((b a k) 1)((S i) 0))))
(lex.add.entry 
'("howard" nn (((x o) 1)((w a r t) 0))))
(lex.add.entry 
'("keel" nn (((k i l) 0))))
(lex.add.entry 
'("pontiee" nn (((p o n) 1)((t j e) 0))))
(lex.add.entry 
'("ruth" nn (((r u t) 0))))
(lex.add.entry 
'("prawder" nn (((p r a w) 1)((d e r) 0))))
(lex.add.entry 
'("ismail" nn (((i s) 1)((m e j l) 0))))
(lex.add.entry 
'("merchant" nn (((m e r) 1)((tS a n t) 0))))
(lex.add.entry 
'("vince" nn (((v i n s) 0))))
(lex.add.entry 
'("fay" nn (((f e j) 0))))
(lex.add.entry 
'("bonnie" nn (((b o) 1)((n+ i) 0))))
(lex.add.entry 
'("clyde" nn (((k l a j d) 0))))
(lex.add.entry 
'("alphaville" nn (((a l) 0)((f a) 1)((v i l) 0))))
(lex.add.entry 
'("bancroft" nn (((b a n) 1)((k r o f t) 0))))
(lex.add.entry 
'("maclaine" nn (((m a k) 1)((l e j n) 0))))
(lex.add.entry 
'("michaela" nn (((m a j) 0)((k e) 1)((l a) 0))))
(lex.add.entry 
'("jazzie" nn (((dZ e) 1)((z+ e) 0))))
(lex.add.entry 
'("harrison" nn (((x a) 0)((r i) 1)((s o n) 0))))
(lex.add.entry 
'("united" nn (((j u) 0)((n a j) 1)((t e t) 0))))
(lex.add.entry 
'("center" nn (((s e n) 1)((t e r) 0))))
(lex.add.entry 
'("jordan" nn (((dZ o r) 1)((d a n) 0))))
(lex.add.entry 
'("dolittle" nn (((d u) 1)((l i t l) 0))))
(lex.add.entry 
'("murphy" nn (((m e r) 1)((f i) 0))))
(lex.add.entry 
'("ferraday" nn (((f e) 0)((r a) 1)((d a j) 0))))
(lex.add.entry 
'("rock" nn (((r o k) 0))))
(lex.add.entry 
'("hudson" nn (((x a t) 1)((s o n) 0))))
(lex.add.entry 
'("peter" nn (((p i) 1)((t e r) 0))))
(lex.add.entry 
'("weir" nn (((w e j r) 0))))
(lex.add.entry 
'("free" nn (((f r i) 0))))
(lex.add.entry 
'("fall" nn (((f o l) 0))))
(lex.add.entry 
'("peggy" nn (((p e) 1)((g i) 0))))
(lex.add.entry 
'("sue" nn (((s j u) 0))))
(lex.add.entry 
'("kathleen" nn (((k e t) 1)((l i n) 0))))
(lex.add.entry 
'("turner" nn (((t a r) 1)((n e r) 0))))
(lex.add.entry 
'("charliem" nn (((tS e r) 1)((l i m) 0))))
(lex.add.entry 
'("nicolas" nn (((n+ i) 0)((k o) 1)((l a s) 0))))
(lex.add.entry 
'("cage" nn (((k e j dZ) 0))))
(lex.add.entry 
'("raynea" nn (((r e j) 1)((n a) 0))))
(lex.add.entry 
'("stewardesa" nn (((s t j u) 0)((a r) 0)((d e) 1)((s a) 0))))
(lex.add.entry 
'("eastwooda" nn (((i s t) 0)((w u) 1)((d a) 0))))
(lex.add.entry 
'("croft" nn (((k r o f t) 0))))
(lex.add.entry 
'("jazz" nn (((dZ e s) 0))))
(lex.add.entry 
'("city" nn (((s i) 1)((t i) 0))))
(lex.add.entry 
'("jazzmanuw" nn (((dZ e z) 0)((m a) 1)((n u f) 0))))
(lex.add.entry 
'("webstera" nn (((w e p) 0)((s t e) 1)((r a) 0))))
(lex.add.entry 
'("herschela" nn (((x e r) 0)((S e) 1)((l a) 0))))
(lex.add.entry 
'("david" nn (((d e j) 1)((v i t) 0))))
(lex.add.entry 
'("murray" nn (((m e) 1)((r e j) 0))))
(lex.add.entry 
'("charlotte" nn (((S a r) 1)((l o t) 0))))
(lex.add.entry 
'("goldie" nn (((g o l) 1)((d i) 0))))
(lex.add.entry 
'("hawn" nn (((x o w n) 0))))
(lex.add.entry 
'("chaplina" nn (((tS e p) 0)((l i) 1)((n a) 0))))
(lex.add.entry 
'("westerny" nn (((w e s) 0)((t e r) 1)((n I) 0))))
(lex.add.entry 
'("western" nn (((w e s) 1)((t e r n) 0))))
(lex.add.entry 
'("science" nn (((s a) 1)((j e n s) 0))))
(lex.add.entry 
'("fiction" nn (((f i k) 1)((S I n) 0))))
(lex.add.entry 
'("westernu" nn (((w e s) 0)((t e r) 1)((n u) 0))))
(lex.add.entry 
'("thriller" nn (((t r i) 1)((l e r) 0))))
(lex.add.entry 
'("bean" nn (((b i n) 0))))
(lex.add.entry 
'("lloyd" nn (((l o j t) 0))))
(lex.add.entry 
'("jeffrey" nn (((dZ e f) 1)((r i) 0))))
(lex.add.entry 
'("lynn" nn (((l i n) 0))))
(lex.add.entry 
'("humphrey" nn (((x a m) 1)((f r e j) 0))))
(lex.add.entry 
'("lynn" nn (((l i n) 0))))
(lex.add.entry 
'("quaid" nn (((k w e j t) 0))))
(lex.add.entry 
'("chicago" nn (((S i) 0)((k a) 1)((g o) 0))))
(lex.add.entry 
'("jay" nn (((dZ e j) 0))))
(lex.add.entry 
'("claire" nn (((k l e r) 0))))
(lex.add.entry 
'("andyego" nn (((e n) 0)((d j e) 1)((g o) 0))))
(lex.add.entry 
'("jazzowy" nn (((dZ e) 0)((z o) 1)((v I) 0))))
(lex.add.entry 
'("stewardese~" nn (((s t j u) 0)((a r) 0)((d e) 1)((s e~) 0))))
(lex.add.entry 
'("angela" nn (((a n) 0)((dZ e) 1)((l a) 0))))
(lex.add.entry 
'("bassett" nn (((b a) 1)((s e t) 0))))
(lex.add.entry 
'("alcatraz" nn (((a l) 0)((k a) 1)((t r a s) 0))))
(lex.add.entry 
'("morris" nn (((m o) 1)((r i s) 0))))
(lex.add.entry 
'("clint" nn (((k l i n t) 0))))
(lex.add.entry 
'("eastwood" nn (((i s t) 1)((w u t) 0))))
(lex.add.entry 
'("josef" nn (((dZ o) 1)((s e f) 0))))
(lex.add.entry 
'("cirque" nn (((s i r k) 0))))
(lex.add.entry 
'("soleil" nn (((s o) 1)((l a j l) 0))))
(lex.add.entry 
'("bungee" nn (((b a N) 1)((dZ i) 0))))
(lex.add.entry 
'("chopina" nn (((S o) 0)((p e) 1)((n a) 0))))
(lex.add.entry 
'("wodewilowy" nn (((w u t) 0)((w i) 0)((l o) 1)((v I) 0))))
(lex.add.entry 
'("stanley" nn (((s t a n) 1)((l e j) 0))))
(lex.add.entry 
'("ipkiss" nn (((i p) 1)((k i s) 0))))
(lex.add.entry 
'("jim" nn (((dZ i m) 0))))
(lex.add.entry 
'("carrey" nn (((k e) 1)((r I) 0))))
(lex.add.entry 
'("mickey" nn (((m i) 1)((k i) 0))))
(lex.add.entry 
'("hope" nn (((x o w p) 0))))
(lex.add.entry 
'("branagh" nn (((b r a) 1)((n a k) 0))))
(lex.add.entry 
'("sir" nn (((s e r) 0))))
(lex.add.entry 
'("henryego" nn (((x e n) 0)((r j e) 1)((g o) 0))))
(lex.add.entry 
'("haggarda" nn (((x a) 0)((g a r) 1)((t a) 0))))
(lex.add.entry 
'("westernem" nn (((w e s) 0)((t e r) 1)((n e m) 0))))
(lex.add.entry 
'("spaghetti" nn (((s p a) 0)((g e) 1)((t i) 0))))
(lex.add.entry 
'("russell" nn (((r a) 1)((s e l) 0))))
(lex.add.entry 
'("crowe" nn (((k r o w) 0))))
(lex.add.entry 
'("sophie" nn (((s o) 1)((f i) 0))))
(lex.add.entry 
'("hitchcock" nn (((x i tS) 1)((k o k) 0))))
(lex.add.entry 
'("irwinga" nn (((i r) 0)((w i N) 1)((g a) 0))))
(lex.add.entry 
'("washingtona" nn (((w o) 0)((S i n k) 0)((t o) 1)((n a) 0))))
(lex.add.entry 
'("chungking" nn (((tS u N) 1)((k i N k) 0))))
(lex.add.entry 
'("nicka" nn (((n+ i) 1)((k a) 0))))
(lex.add.entry 
'("ashley" nn (((e S) 1)((l e j) 0))))
(lex.add.entry 
'("reilly" nn (((r e j) 1)((l i) 0))))
(lex.add.entry 
'("walters" nn (((w o l) 1)((t e r s) 0))))
(lex.add.entry 
'("izzy" nn (((i z) 1)((z i) 0))))
(lex.add.entry 
'("stanleya" nn (((s t a n) 0)((l e) 1)((j a) 0))))
(lex.add.entry 
'("goldkissa" nn (((g o l t) 0)((k i) 1)((s a) 0))))
(lex.add.entry 
'("falls" nn (((f o l s) 0))))
(lex.add.entry 
'("twin" nn (((t w i n) 0))))
(lex.add.entry 
'("peaks" nn (((p i k s) 0))))
(lex.add.entry 
'("lynch" nn (((l i n tS) 0))))
(lex.add.entry 
'("lyncha" nn (((l i n) 1)((tS a) 0))))
(lex.add.entry 
'("winterbottom" nn (((w i n) 0)((t e r) 0)((b a) 1)((t o m) 0))))
(lex.add.entry 
'("wonderland" nn (((w o n) 0)((d e r) 1)((l a n t) 0))))
(lex.add.entry 
'("oscarowym" nn (((o s) 0)((k a) 0)((r o) 1)((v I m) 0))))
(lex.add.entry 
'("express" nn (((e k s) 1)((p r e s) 0))))
(lex.add.entry 
'("blair" nn (((b l e r) 0))))
(lex.add.entry 
'("witch" nn (((v i tS) 0))))
(lex.add.entry 
'("project" nn (((p r o) 1)((dZ e k t) 0))))
(lex.add.entry 
'("blue" nn (((b l u) 0))))
(lex.add.entry 
'("jane" nn (((dZ e j n) 0))))
(lex.add.entry 
'("juliette" nn (((Z u l) 1)((j e) 0))))
(lex.add.entry 
'("lewis" nn (((l u) 1)((i s) 0))))
(lex.add.entry 
'("william" nn (((w i l) 1)((j a m) 0))))
(lex.add.entry 
'("hurt" nn (((x a r t) 0))))
(lex.add.entry 
'("boyd" nn (((b o j t) 0))))
(lex.add.entry 
'("guy" nn (((g a j) 0))))
(lex.add.entry 
'("pearce" nn (((p i r s) 0))))
(lex.add.entry 
'("ives" nn (((i f s) 0))))
(lex.add.entry 
'("carl" nn (((k a r l) 0))))
(lex.add.entry 
'("erzac" nn (((e r) 1)((z a k) 0))))
(lex.add.entry 
'("sacrum" nn (((s a k) 1)((r u m) 0))))
(lex.add.entry 
'("bulworth" nn (((b u l) 1)((w o r t) 0))))
(lex.add.entry 
'("warren" nn (((w o) 1)((r e n) 0))))
(lex.add.entry 
'("beatty" nn (((b i) 1)((t i) 0))))
(lex.add.entry 
'("grieco" nn (((g r e) 1)((k o) 0))))
(lex.add.entry 
'("hathaway" nn (((x a) 0)((d a) 1)((w e j) 0))))
(lex.add.entry 
'("ridleya" nn (((r i d) 0)((l e) 1)((j a) 0))))
(lex.add.entry 
'("johna" nn (((dZ o) 1)((n a) 0))))
(lex.add.entry 
'("hollywoodzko~" nn (((x o) 0)((l I) 0)((w u ts) 1)((k o~) 0))))
(lex.add.entry 
'("quasi" nn (((k f a) 1)((z i) 0))))
(lex.add.entry 
'("stich" nn (((s t i tS) 0))))
(lex.add.entry 
'("tommy" nn (((t o) 1)((m i) 0))))
(lex.add.entry 
'("jones" nn (((dZ o n s) 0))))
(lex.add.entry 
'("will" nn (((w I l) 0))))
(lex.add.entry 
'("smith" nn (((s m I f) 0))))
(lex.add.entry 
'("flynn" nn (((f l i n) 0))))
(lex.add.entry 
'("boyle" nn (((b o j l) 0))))
(lex.add.entry 
'("rosario" nn (((r o) 0)((z a r) 1)((j o) 0))))
(lex.add.entry 
'("dawson" nn (((d a w) 1)((s o n) 0))))
(lex.add.entry 
'("abronsius" nn (((a) 0)((b r o n) 1)((s j u s) 0))))
(lex.add.entry 
'("hitchcockiem" nn (((x i tS) 0)((k o) 1)((k j e m) 0))))
(lex.add.entry 
'("bestselleruw" nn (((b e s t) 0)((s e) 0)((l e) 1)((r u f) 0))))
(lex.add.entry 
'("felicity" nn (((f e) 0)((l i) 0)((s i) 1)((t i) 0))))
(lex.add.entry 
'("hugh" nn (((x j u) 0))))
(lex.add.entry 
'("musical" nn (((m j u) 0)((z i) 1)((k a l) 0))))
(lex.add.entry 
'("lady" nn (((l e j) 1)((d i) 0))))
(lex.add.entry 
'("shirley" nn (((S i r) 1)((l e j) 0))))
(lex.add.entry 
'("valentine" nn (((v a) 0)((l e n) 1)((t a j n) 0))))
(lex.add.entry 
'("scott" nn (((s k o t) 0))))
(lex.add.entry 
'("ridley" nn (((r i d) 1)((l e j) 0))))
(lex.add.entry 
'("lancey" nn (((l a n) 1)((s e j) 0))))
(lex.add.entry 
'("emily" nn (((e) 0)((m i) 1)((l i) 0))))
(lex.add.entry 
'("remake" nn (((r i) 1)((m e j k) 0))))
(lex.add.entry 
'("lucy" nn (((l u) 1)((s i) 0))))
(lex.add.entry 
'("picciafuoco" nn (((p i) 0)((tS i a) 0)((f u) 1)((k o) 0))))
(lex.add.entry 
'("walta" nn (((w o l) 1)((t a) 0))))
(lex.add.entry 
'("disneya" nn (((d i s) 0)((n e) 1)((j a) 0))))
(lex.add.entry 
'("osborne" nn (((o z) 1)((b o r n) 0))))
(lex.add.entry 
'("zhang" nn (((S a N k) 0))))
(lex.add.entry 
'("tahiti" nn (((t a) 0)((i) 1)((t i) 0))))
(lex.add.entry 
'("woodstok" nn (((w u t) 1)((s t o k) 0))))
(lex.add.entry 
'("acme" nn (((a) 0)((ts e) 0)((e m) 1)((e) 0))))
(lex.add.entry 
'("myersem" nn (((m a) 0)((j e r) 1)((s e m) 0))))
(lex.add.entry 
'("powers" nn (((p a) 1)((w e r s) 0))))
(lex.add.entry 
'("giovanna" nn (((dZ j o) 0)((v a n) 1)((n a) 0))))
(lex.add.entry 
'("cohn" nn (((k o n) 0))))
(lex.add.entry 
'("thielemans" nn (((t i l) 1)((m a n s) 0))))
(lex.add.entry 
'("walter" nn (((w o l) 1)((t e r) 0))))
(lex.add.entry 
'("becker" nn (((b e) 1)((k e r) 0))))
(lex.add.entry 
'("ricky" nn (((r i) 1)((k i) 0))))
(lex.add.entry 
'("lawson" nn (((l o w) 1)((s o n) 0))))
(lex.add.entry 
'("potter" nn (((p o) 1)((t e r) 0))))
(lex.add.entry 
'("vinnie" nn (((v i) 1)((n+ i) 0))))
(lex.add.entry 
'("colaiuta" nn (((k o) 0)((l a) 0)((j u) 1)((t a) 0))))
(lex.add.entry 
'("tranea" nn (((t r e) 1)((n a) 0))))
(lex.add.entry 
'("sing" nn (((s i N k) 0))))
(lex.add.entry 
'("four" nn (((f o r) 0))))
(lex.add.entry 
'("brothers" nn (((b r a) 1)((d e r s) 0))))
(lex.add.entry 
'("take" nn (((t e j k) 0))))
(lex.add.entry 
'("train" nn (((tS r e j n) 0))))
(lex.add.entry 
'("mood" nn (((m u t) 0))))
(lex.add.entry 
'("cherokee" nn (((tS e) 0)((r o) 1)((k i) 0))))
(lex.add.entry 
'("deja" nn (((d e) 1)((Z a) 0))))
(lex.add.entry 
'("vu" nn (((v i) 0))))
(lex.add.entry 
'("jazzstep" nn (((dZ e s) 1)((s t e p) 0))))
(lex.add.entry 
'("sidi" nn (((s i) 1)((d i) 0))))
(lex.add.entry 
'("shikha" nn (((S i) 1)((k a) 0))))
(lex.add.entry 
'("prime" nn (((p r a j m) 0))))
(lex.add.entry 
'("time" nn (((t a j m) 0))))
(lex.add.entry 
'("dancing" nn (((d e n) 1)((s i N k) 0))))
(lex.add.entry 
'("your" nn (((j o r) 0))))
(lex.add.entry 
'("head" nn (((x e t) 0))))
(lex.add.entry 
'("world" nn (((w o r l t) 0))))
(lex.add.entry 
'("music" nn (((m j u) 1)((z i k) 0))))
(lex.add.entry 
'("hancock" nn (((x a n) 1)((k o k) 0))))
(lex.add.entry 
'("jazzowo~" nn (((dZ e) 0)((z o) 1)((v o~) 0))))
(lex.add.entry 
'("drumnbassowych" nn (((d r a m) 0)((e n) 0)((b e j) 0)((s o) 1)((v I x) 0))))
(lex.add.entry 
'("junglowych" nn (((dZ a N) 0)((g l o) 1)((v I x) 0))))
(lex.add.entry 
'("simple" nn (((s i m p l) 0))))
(lex.add.entry 
'("acoustic" nn (((a) 0)((k u s) 1)((t i k) 0))))
(lex.add.entry 
'("dave" nn (((d e j v) 0))))
(lex.add.entry 
'("douglas" nn (((d a) 1)((g l a s) 0))))
(lex.add.entry 
'("washington" nn (((w o) 0)((S i N k) 1)((t o n) 0))))
(lex.add.entry 
'("britney" nn (((b r i t) 1)((n e j) 0))))
(lex.add.entry 
'("spears" nn (((s p i r s) 0))))
(lex.add.entry 
'("blues" nn (((b l u s) 0))))
(lex.add.entry 
'("soulowe" nn (((s o w) 0)((l o) 1)((v e) 0))))
(lex.add.entry 
'("jazzowe" nn (((dZ e) 0)((z o) 1)((v e) 0))))
(lex.add.entry 
'("johnsonem" nn (((dZ o n) 0)((s o) 1)((n e m) 0))))
(lex.add.entry 
'("quiet" nn (((k w a) 1)((j e t) 0))))
(lex.add.entry 
'("new" nn (((n+ u) 0))))
(lex.add.entry 
'("soulu" nn (((s o w) 1)((l u) 0))))
(lex.add.entry 
'("funky" nn (((f a N) 1)((k i) 0))))
(lex.add.entry 
'("pastforward" nn (((p a s t) 0)((f o r) 1)((w e r t) 0))))
(lex.add.entry 
'("collage" nn (((k o) 1)((l e dZ) 0))))
(lex.add.entry 
'("cecilii" nn (((ts e) 0)((ts i) 1)((l i i) 0))))
(lex.add.entry 
'("rossiniego" nn (((r o) 0)((s i) 0)((n+ e) 1)((g o) 0))))
(lex.add.entry 
'("cecilia" nn (((ts e) 0)((ts i) 1)((l j a) 0))))
(lex.add.entry 
'("american" nn (((a) 0)((m e) 0)((r i) 1)((k a n) 0))))
(lex.add.entry 
'("scholar" nn (((s k o) 1)((l a r) 0))))
(lex.add.entry 
'("maynard" nn (((m e j) 1)((n a r t) 0))))
(lex.add.entry 
'("szathmary" nn (((S e t) 0)((m e) 1)((r i) 0))))
(lex.add.entry 
'("wharton" nn (((w o r) 1)((t o n) 0))))
(lex.add.entry 
'("mcluhanowi" nn (((m a k) 0)((l a) 0)((n o) 1)((v i) 0))))
(lex.add.entry 
'("umschlagplatz" nn (((u m) 0)((S l a g) 1)((p l a ts) 0))))
(lex.add.entry 
'("brainarda" nn (((b r e j) 0)((n a r) 1)((d a) 0))))
(lex.add.entry 
'("conrada" nn (((k o n) 0)((r a) 1)((d a) 0))))
(lex.add.entry 
'("jeanette" nn (((dZ a) 1)((n e t) 0))))
(lex.add.entry 
'("winterson" nn (((w i n) 0)((t e r) 1)((s o n) 0))))
(lex.add.entry 
'("philipa" nn (((f i) 0)((l i) 1)((p a) 0))))
(lex.add.entry 
'("gould" nn (((g o l t) 0))))
(lex.add.entry 
'("huelle" nn (((x j u) 1)((e l) 0))))
(lex.add.entry 
'("josephem" nn (((dZ o) 0)((z e) 1)((f e m) 0))))
(lex.add.entry 
'("ratzingerem" nn (((r a) 0)((ts i N) 0)((g e) 1)((r e m) 0))))
(lex.add.entry 
'("seewald" nn (((z e) 0)((j e) 1)((v a l t) 0))))
(lex.add.entry 
'("stewart" nn (((s t j u) 1)((a r t) 0))))
(lex.add.entry 
'("walser" nn (((w o l) 1)((s e r) 0))))
(lex.add.entry 
'("ziemann" nn (((ts i) 1)((m a n) 0))))
(lex.add.entry 
'("riecke" nn (((r i) 1)((k e) 0))))
(lex.add.entry 
'("wilmuta" nn (((w i l) 0)((m u) 1)((t a) 0))))
(lex.add.entry 
'("dolly" nn (((d o) 1)((l i) 0))))
(lex.add.entry 
'("henriques" nn (((x e n) 1)((r i k) 0))))
(lex.add.entry 
'("hellera" nn (((x e) 0)((l e) 1)((r a) 0))))
(lex.add.entry 
'("keseya" nn (((k e) 0)((s e) 1)((j a) 0))))
(lex.add.entry 
'("weimar" nn (((v e j) 1)((m a r) 0))))
(lex.add.entry 
'("goethe" nn (((g e) 1)((t e) 0))))
(lex.add.entry 
'("nath" nn (((n a t) 0))))
(lex.add.entry 
'("jonathan" nn (((dZ o) 0)((n a) 1)((t a n) 0))))
(lex.add.entry 
'("bridgeman" nn (((b r i tS) 1)((m e n) 0))))
(lex.add.entry 
'("siedlung" nn (((z i d) 1)((l u N k) 0))))
(lex.add.entry 
'("wallis" nn (((w o) 1)((l i s) 0))))
(lex.add.entry 
'("simpson" nn (((s i m) 1)((p s o n) 0))))
(lex.add.entry 
'("hartwig" nn (((x a r t) 1)((w i k) 0))))
(lex.add.entry 
'("rabelais" nn (((r a) 0)((b e) 1)((l e j) 0))))
(lex.add.entry 
'("romain" nn (((r o) 1)((m e j n) 0))))
(lex.add.entry 
'("rolland" nn (((r o) 1)((l a n t) 0))))
(lex.add.entry 
'("isaac" nn (((i) 0)((z a) 1)((a k) 0))))
(lex.add.entry 
'("zinger" nn (((z i N) 1)((g e r) 0))))
(lex.add.entry 
'("world" nn (((w o r t) 0))))
(lex.add.entry 
'("trade" nn (((t r e j d) 0))))
(lex.add.entry 
'("vasco" nn (((v a s) 1)((k o) 0))))
(lex.add.entry 
'("ylljet" nn (((i l) 1)((j e t) 0))))
(lex.add.entry 
'("heller" nn (((x e) 1)((l e r) 0))))
(lex.add.entry 
'("nicki" nn (((n+ i) 1)((k i) 0))))
(lex.add.entry 
'("mass" nn (((m a s) 0))))
(lex.add.entry 
'("huellego" nn (((x j u) 0)((e) 0)((l e) 1)((g o) 0))))
(lex.add.entry 
'("joyce" nn (((dZ o j s) 0))))
(lex.add.entry 
'("pynchon" nn (((p I n) 1)((tS o n) 0))))
(lex.add.entry 
'("facto" nn (((f a k) 1)((t o) 0))))
(lex.add.entry 
'("tischner" nn (((t i S) 1)((n e r) 0))))
(lex.add.entry 
'("bestsellerami" nn (((b e s t) 0)((s e) 0)((l e) 0)((r a) 1)((m i) 0))))
(lex.add.entry 
'("crabtree" nn (((k r a p) 1)((t r i) 0))))
(lex.add.entry 
'("clay" nn (((k l e j) 0))))
(lex.add.entry 
'("jonsona" nn (((dZ o n) 0)((s o) 1)((n a) 0))))
(lex.add.entry 
'("joycea" nn (((dZ o j) 1)((s a) 0))))
(lex.add.entry 
'("blakea" nn (((b l e j) 1)((k a) 0))))
(lex.add.entry 
'("lewisa" nn (((l u) 0)((i) 1)((s a) 0))))
(lex.add.entry 
'("faulknera" nn (((f o l) 0)((k n e) 1)((r a) 0))))
(lex.add.entry 
'("virginii" nn (((v i r) 0)((dZ i) 1)((n+ i) 0))))
(lex.add.entry 
'("vaterland" nn (((f a) 0)((t e r) 1)((l a n t) 0))))
(lex.add.entry 
'("pascal" nn (((p a s) 1)((k a l) 0))))
(lex.add.entry 
'("hume" nn (((x j u m) 0))))
(lex.add.entry 
'("schopenhauer" nn (((S o) 0)((p e n) 0)((x a) 1)((w e r) 0))))
(lex.add.entry 
'("nietzsche" nn (((n+ i) 1)((tS e) 0))))
(lex.add.entry 
'("wolwerina" nn (((v o l) 0)((v e) 0)((r a j) 1)((n a) 0))))
(lex.add.entry 
'("hulk" nn (((x a l k) 0))))
(lex.add.entry 
'("kietha" nn (((k i) 1)((t a) 0))))
(lex.add.entry 
'("copywriter" nn (((k o) 0)((p i) 0)((r a j) 1)((t e r) 0))))
(lex.add.entry 
'("ieyasu" nn (((j e) 0)((j a) 1)((s u) 0))))
(lex.add.entry 
'("bridget" nn (((b r i) 1)((dZ e t) 0))))
(lex.add.entry 
'("harryego" nn (((x a) 0)((r e) 1)((g o) 0))))
(lex.add.entry 
'("pottera" nn (((p o) 0)((t e) 1)((r a) 0))))
(lex.add.entry 
'("fitzgeraldowie" nn (((f i dz) 0)((Z e) 0)((r a l) 0)((d o) 1)((v j e) 0))))
(lex.add.entry 
'("jagdish" nn (((dZ e g) 1)((d I S) 0))))
(lex.add.entry 
'("bhagwati" nn (((b x a) 0)((g w a) 1)((t i) 0))))
(lex.add.entry 
'("columbia" nn (((k o) 0)((l u m) 1)((b j a) 0))))
(lex.add.entry 
'("chafetz" nn (((S a) 1)((f e ts) 0))))
(lex.add.entry 
'("beerholm" nn (((b e r) 1)((x o l m) 0))))
(lex.add.entry 
'("kehlmanna" nn (((k e l) 0)((m a) 1)((n a) 0))))
(lex.add.entry 
'("plath" nn (((p l a f) 0))))
(lex.add.entry 
'("hughesa" nn (((x a g) 1)((s a) 0))))
(lex.add.entry 
'("leibniza" nn (((l a j) 0)((b n+ i) 1)((ts a) 0))))
(lex.add.entry 
'("helsinek" nn (((x e l) 0)((s i) 1)((n e k) 0))))
(lex.add.entry 
'("villqista" nn (((v i l) 0)((k i) 1)((s t a) 0))))
(lex.add.entry 
'("jessica" nn (((dZ e) 0)((s i) 1)((k a) 0))))
(lex.add.entry 
'("shaggy" nn (((S e) 1)((g i) 0))))
(lex.add.entry 
'("kylie" nn (((k a l) 1)((j e) 0))))
(lex.add.entry 
'("minogue" nn (((m i) 1)((n o k) 0))))
(lex.add.entry 
'("outkast" nn (((a w t) 1)((k a s t) 0))))
(lex.add.entry 
'("and" nn (((e n t) 0))))
(lex.add.entry 
'("sioux" nn (((s j u k s) 0))))
(lex.add.entry 
'("zanuck" nn (((z a) 1)((n u k) 0))))
(lex.add.entry 
'("quiz" nn (((k w i s) 0))))
(lex.add.entry 
'("hollywoodzkich" nn (((x o) 0)((l I) 0)((w u ts) 1)((k i x) 0))))
(lex.add.entry 
'("joseph" nn (((dZ o) 1)((s e f) 0))))
(lex.add.entry 
'("wiseman" nn (((w a j s) 1)((m e n) 0))))
(lex.add.entry 
'("kenneth" nn (((k e) 1)((n e t) 0))))
(lex.add.entry 
'("branagh" nn (((b r a) 1)((n a g) 0))))
(lex.add.entry 
'("hippis" nn (((x i) 1)((p i s) 0))))
(lex.add.entry 
'("hair" nn (((x e r) 0))))
(lex.add.entry 
'("francesco" nn (((f r a n) 0)((tS e s) 1)((k o) 0))))
(lex.add.entry 
'("rosi" nn (((r o) 1)((s i) 0))))
(lex.add.entry 
'("slackerzy" nn (((s l a) 0)((k e) 1)((Z I) 0))))
(lex.add.entry 
'("powersie" nn (((p a) 0)((w e r) 1)((s+ e) 0))))
(lex.add.entry 
'("swingujo~cego" nn (((s w i N) 0)((g u) 0)((j o n) 0)((ts e) 1)((g o) 0))))
(lex.add.entry 
'("jarmusch" nn (((j a r) 1)((m u S) 0))))
(lex.add.entry 
'("fincher" nn (((f i n) 1)((tS e r) 0))))
(lex.add.entry 
'("teena" nn (((t i) 1)((n a) 0))))
(lex.add.entry 
'("swank" nn (((s w o N k) 0))))
(lex.add.entry 
'("oscar" nn (((o s) 1)((k a r) 0))))
(lex.add.entry 
'("will" nn (((w i l) 0))))
(lex.add.entry 
'("hailey" nn (((x e j) 1)((l i) 0))))
(lex.add.entry 
'("calvin" nn (((k e l) 1)((v i n) 0))))
(lex.add.entry 
'("cambridge" nn (((k e m) 1)((b r i dZ) 0))))
(lex.add.entry 
'("thrillery" nn (((t r i) 0)((l e) 1)((r I) 0))))
(lex.add.entry 
'("yates" nn (((j e j t s) 0))))
(lex.add.entry 
'("adderleyem" nn (((a) 0)((d e r) 0)((l e) 1)((j e m) 0))))
(lex.add.entry 
'("mcleanem" nn (((m a k) 0)((l i) 1)((n e m) 0))))
(lex.add.entry 
'("chwin" nn (((tS w i n) 0))))
(lex.add.entry 
'("tequili" nn (((t e) 0)((k i) 1)((l i) 0))))
(lex.add.entry 
'("garrinchy" nn (((g e) 0)((r i n) 1)((tS i) 0))))
(lex.add.entry 
'("grossman" nn (((g r o s) 1)((m a n) 0))))
(lex.add.entry 
'("spielberg" nn (((S p i l) 1)((b e r k) 0))))
(lex.add.entry 
'("hook" nn (((x u k) 0))))
(lex.add.entry 
'("stephens" nn (((s t e) 1)((f e n s) 0))))
(lex.add.entry 
'("magazine" nn (((m a) 0)((g a) 1)((z i n) 0))))
(lex.add.entry 
'("christopher" nn (((k r i) 0)((s t o) 1)((f e r) 0))))
(lex.add.entry 
'("aids" nn (((e j ts) 0))))
(lex.add.entry 
'("falzmann" nn (((f a l ts) 1)((m a n) 0))))
(lex.add.entry 
'("retaliation" nn (((r e) 0)((t a l) 0)((j e j) 1)((S I n) 0))))
(lex.add.entry 
'("strategy" nn (((s t r a) 0)((t e) 1)((dZ I) 0))))
(lex.add.entry 
'("quarzwerke" nn (((k f a r ts) 0)((v e r) 1)((k e) 0))))
(lex.add.entry 
'("boeinguw" nn (((b e) 0)((i N) 1)((g u f) 0))))
(lex.add.entry 
'("thomasa" nn (((t o) 0)((m a) 1)((s a) 0))))
(lex.add.entry 
'("rue" nn (((r u) 0))))
(lex.add.entry 
'("spiegel" nn (((S p i) 1)((g e l) 0))))
(lex.add.entry 
'("bull" nn (((b u l) 0))))
(lex.add.entry 
'("crimen" nn (((k r i) 1)((m e n) 0))))
(lex.add.entry 
'("lege" nn (((l e k) 0))))
(lex.add.entry 
'("poena" nn (((p w e) 1)((n a) 0))))
(lex.add.entry 
'("cordlesstelephone" nn (((k o r d) 0)((l e s) 0)((t e) 0)((l e) 1)((f o n) 0))))
(lex.add.entry 
'("expose" nn (((e k s) 0)((p o) 1)((z e) 0))))
(lex.add.entry 
'("stolzman" nn (((S t o l ts) 1)((m a n) 0))))
(lex.add.entry 
'("bonn" nn (((b o n) 0))))
(lex.add.entry 
'("danzigau" nn (((d a n) 0)((ts i) 1)((g a w) 0))))
(lex.add.entry 
'("syssmuth" nn (((z I s) 1)((m u t) 0))))
(lex.add.entry 
'("lobbing" nn (((l o) 1)((b i N k) 0))))





;; czas
(lex.add.entry 
'("podzielic~" pron (((p o) 0) ((dz+ e) 1)((l i ts+) 0))))
(lex.add.entry 
'("zidentyfikowac~" pron (((z i) 0) ((d e n) 0) ((t I) 0) ((f i) 0) ((k o) 1) ((v a ts+) 0))))
(lex.add.entry 
'("zintegrowane" pron (((z i n) 0) ((t e ) 0) ((g r o) 0) ((v a) 1) ((n e) 0))))

(lex.add.entry 
'("odzwierciedlac~" pron (((o d) 0) ((z v j e r) 0) ((ts+ e) 1) ((d l a ts+) 0))))
(lex.add.entry 
'("przemarzl/o" pron (((p S e) 0) ((m a r) 1)((z w o) 0))))
(lex.add.entry 
'("nauke~" pron (((n a) 0) ((u) 1)((k e~) 0))))
(lex.add.entry 
'("nieuczciwe" pron (((n+ e) 0) ((u tS) 0) ((ts+ i) 1) ((v e) 0))))
(lex.add.entry 
'("nieudany" pron (((n+ e) 0) ((u) 0) ((d a) 1) ((n I) 0))))
(lex.add.entry 
'("nauczyciela" pron (((n a) 0) ((u) 0) ((tS I) 0)((ts+ e) 1) ((l a) 0))))
(lex.add.entry 
'("tadeusza" pron (((t a) 0) ((d e) 0) ((u) 1) ((S a) 0))))
(lex.add.entry 
'("mateusz" pron (((m a) 0) ((t e) 1)((u S) 0))))
(lex.add.entry 
'("muzeum" pron (((m u) 0) ((z e) 1)((u m) 0))))

(lex.add.entry 
'("coltrana" pron (((k o l) 0) ((t r a) 1)((n a) 1))))

(lex.add.entry 
'("konstytucyjny" pron (((k o n)0)((s t I)0)((t u)0)((ts I j)1)((n I) 0))))
(lex.add.entry 
'("niekonsekwencjami" pron (((n+ e) 0)((k o n) 0) ((s e k) 0) ((f e n) 0) ((ts j a) 1) ((m i) 0))))
(lex.add.entry 
'("donrzuan" pron (((d o n) 0)((Z u) 1)((a n) 0))))
(lex.add.entry 
'("myslovitz" pron (((m I) 0)((s l o) 1)((v i ts) 0))))
(lex.add.entry 
'("zanussi" pron (((z a) 0)((n u s) 1)((s i) 0))))
(lex.add.entry 
'("skonsumowany" pron (((s k o n) 0)((s u)0)((m o)0)((v a)1)((n I)0))))
(lex.add.entry 
'("afganistan" pron (((a f) 0)((g a)0)((n+ i)1)((s t a n)0))))
(lex.add.entry 
'("odznaka" nn (((o d) 0)((z n a) 1)((k a) 0))))
(lex.add.entry 
'("zegarmistrz" nn (((z e) 0)((g a r) 1)((m i S tS) 0))))
(lex.add.entry 
'("spieszy" nn (((s+ p j e) 1)((S I) 0))))
(lex.add.entry 
'("olbrychski" nn (((o l) 0)((b r I) 1)((s k i) 0))))
(lex.add.entry 
'("suspensu" pron (((s u s) 0) ((p e n) 1)((s u) 0))))
(lex.add.entry 
'("odzyskaniu" pron (((o d) 0)((z I s)0)((k a)1)((n+ u)0))))
(lex.add.entry 
'("zrzedno~" nn (((z Z e d) 1)((n o~) 0))))
(lex.add.entry 
'("jabl/kach" nn (((j a p) 1)((k a x) 0))))
(lex.add.entry 
'("mugl/by" nn (((m u g) 1)((b I) 0))))
(lex.add.entry 
'("nieuwage~" pron (((n+ e) 0)((u)0)((v a)1)((g e~)0))))
(lex.add.entry 
'("nauki" pron (((n a) 0) ((u) 1)((k i) 0))))
(lex.add.entry 
'("naukowej" pron (((n a) 0)((u)0)((k o)1)((v e j)0))))
(lex.add.entry 
'("pil/sudskiego" pron (((p i w) 0)((s u ts)0)((k j e)1)((g o)0))))
(lex.add.entry 
'("mugl/" pron (((m u g) 0))))
(lex.add.entry 
'("obmierzl/ej" pron (((o b) 0)((m j e r)1)((z w e j)0))))
(lex.add.entry 
'("zmarzl/" pron (((z m a r z w) 0))))
(lex.add.entry 
'("zmarzl/a" nn (((z m a r) 1)((z w a) 0))))
(lex.add.entry 
'("zmarzno~ts~" nn (((z m a r) 1)((z n o n+ ts+) 0))))
(lex.add.entry 
'("zamarzl/" nn (((z a) 1)((m a r z w) 0))))
(lex.add.entry 
'("zamarzno~ts~" pron (((z a) 0)((m a r z)1)((n o n+ ts+)0))))
(lex.add.entry 
'("zamarzl/a" pron (((z a) 0)((m a r)1)((z w a)0))))
(lex.add.entry 
'("naukowa" pron (((n a) 0)((u)0)((k o)1)((v a)0))))
(lex.add.entry 
'("naukowe" pron (((n a) 0)((u)0)((k o)1)((v e)0))))
(lex.add.entry 
'("naukowych" pron (((n a) 0)((u)0)((k o)1)((v I x)0))))
(lex.add.entry 
'("prywatyzaci" pron (((p r I) 0)((v a)0)((t I)0)((z a)1)((ts i)0))))
;; dodane bo tak bylo w nagraniu, zeby nie ustawiac na stale takiej wymowy na koncu zamiast ji jest i

(lex.add.entry 
'("nieuleczalnym" nn (((n+ e) 0)((u) 0)((l e) 0)((tS a l) 1)((n I m) 0))))
(lex.add.entry 
'("tadeusz" pron (((t a) 0)((d e)1)((u S)0))))
(lex.add.entry 
'("kle~ske~" nn (((k l e n) 1)((s k e~) 0))))
(lex.add.entry 
'("zrzucic~" nn (((z Z u) 1)((ts+ i ts+) 0))))
(lex.add.entry 
'("zidiocial/ego" nn (((z i) 0)((d j o) 0)((ts+ a) 0)((w e) 1)((g o) 0))))
(lex.add.entry 
'("szpicbrudka" pron (((S p i ts) 0)((b r u t)1)((k a)0))))
(lex.add.entry 
'("odzwierciedlenie" pron (((o d) 0)((z v j e r) 0)((ts+ e) 0)((d l e) 1)((n+ e) 0))))
(lex.add.entry 
'("horrendalne" pron (((x o) 0)((r e n) 0)((d a l) 1)((n e) 0))))
(lex.add.entry 
'("nieujawnienie" pron (((n+ e) 0)((u) 0)((j a v) 0)((n+ e) 1)((n+ e) 0))))
(lex.add.entry 
'("muwco~" nn (((m u f) 1)((ts o m) 0))))
(lex.add.entry 
'("wal/e~sa" pron (((v a) 0)((w e n)1)((s a)0))))
(lex.add.entry 
'("dotychczasowe" pron (((d o) 0)((t I x) 0)((tS a) 0)((s o) 1)((v e) 0))))
(lex.add.entry 
'("dotychczas" pron (((d o) 0)((t I x)1)((tS a s)0))))
(lex.add.entry 
'("zrzekl/" pron (((z) 1)((Z e k w) 0))))
(lex.add.entry 
'("odznaczen~" pron (((o d) 0)((z n a)1)((tS e n+)0))))
(lex.add.entry 
'("odznaczane" pron (((o d) 0)((z n a) 0)((tS a) 1)((n e) 0))))
(lex.add.entry 
'("mugl/bym" nn (((m u g) 1)((b I m) 0))))
(lex.add.entry 
'("zintensyfikowac~" pron (((z i n) 0)((t e n) 0)((s I) 0)((f i) 0)((k o) 1)((v a ts+) 0))))
(lex.add.entry 
'("nauczycieli" pron (((n a) 0)((u) 0)((tS I) 0)((ts+ e) 1)((l i) 0))))
(lex.add.entry 
'("zaufania" pron (((z a) 0)((u) 0)((f a) 1)((n+ a) 0))))
(lex.add.entry 
'("nieuzasadniona" pron (((n+ e) 0)((u) 0)((z a) 0)((s a d) 0)((n+ o) 1)((n a) 0))))
(lex.add.entry 
'("nieudana" pron (((n+ e) 0)((u) 0)((d a) 1)((n a) 0))))
(lex.add.entry 
'("zrzo~dzenie" pron (((z Z o n) 0)((dz e) 1)((n+ e) 0))))


(lex.add.entry 
'("dziewie~c~dziesie~ciu" pron (((dz+ e) 0)((v j e n+) 0)((dz+ e) 0)((s+ e n+) 1)((ts+ u) 0))))
(lex.add.entry 
'("dziewie~c~dziesio~tych" pron (((dz+ e) 0)((v j e n+) 0)((dz+ e) 0)((s+ o n) 1)((t I x) 0))))
(lex.add.entry 
'("szes~c~dziesio~tych" pron (((S e z+) 0)((dz+ e) 0)((s+ o n) 1)((t I x) 0))))
(lex.add.entry 
'("szes~c~dziesie~ciu" pron (((S e z+) 0)((dz+ e) 0)((s+ e n+) 1)((ts+ u) 0))))
(lex.add.entry 
'("pie~tnas~cie" pron (((p j e t) 0)((n a s+)1)((ts+ e)0))))
(lex.add.entry 
'("liczb" pron (((l i tS p) 0))))
(lex.add.entry 
'("liczby" nn (((l i tS) 1)((b I) 0))))


;;(lex.add.entry 
;;'("podzial/em" nn (((p o) 0)((dz+ a) 1)((w e m) 0))))






;;prepositions,

(lex.add.entry 
'("w" prep-acc (((f) 0))))                          ;denoting time
(lex.add.entry 
'("z" prep-loc (((s) 0))))                           ;with
(lex.add.entry 
'("okol/o" prep-gen (((o) 0) ((k o) 0) ((w o) 0))))  ;about 
(lex.add.entry 
'("oko³o" prep-gen (((o) 0) ((k o) 0) ((w o) 0))))
(lex.add.entry 
'("o" prep-loc (((o) 0))))                            ;at
(lex.add.entry 
'("po" prep-loc (((p o) 0))))                         ;after
(lex.add.entry 
'("przy" prep-loc (((p S I) 0))))                     ; close to
(lex.add.entry 
'("na" prep-loc (((n a) 0))))                          ; in
(lex.add.entry 
'("u" prep-gen (((u) 0))))                            ; at somebody's house
(lex.add.entry 
'("do" prep-gen (((d o) 0))))                          ; to 
(lex.add.entry 
'("przez" prep-acc (((p S e s) 0))))                   ; by
(lex.add.entry 
'("przed" prep-inst (((p S e t) 0))))                  ; before
(lex.add.entry 
'("obok" prep-gen (((o) 0) ((b o k) 0))))              ; next to
(lex.add.entry 
'("dla" prep-gen (((d l a) 0))))                       ; for
(lex.add.entry 
'("ze" prep-inst (((z e) 0))))                          ; with
(lex.add.entry 
'("za" prep-acc (((z a) 0))))            ; buying for 'za 2 z³ote'
(lex.add.entry 
'("od" prep-gen (((o t) 0))))                             ;from
(lex.add.entry 
'("we" prep-acc (((v e) 0))))           ; in, with days of the week
(lex.add.entry 
'("poza" (((p o) 1) ((z a) 0))))                          ; beyond
(lex.add.entry 
'("bez" prep-gen (((b e s) 0))))                         ; without
(lex.add.entry 
'("ponad" prep-acc (((p o) 0) ((n a t) 0))))              ; more than
(lex.add.entry 
'("powyz*ej" prep-gen (((p o) 0) ((v I) 0) ((Z e j) 0))))  ; above
(lex.add.entry 
'("powy¿ej" prep-gen (((p o) 0) ((v I) 0) ((Z e j) 0))))  ; above
(lex.add.entry 
'("ku" prep-dat (((k u) 0))))                              ; towards
(lex.add.entry 
'("wobec" prep-gen (((v o) 0) ((b e ts) 0))))             ;with regard to
(lex.add.entry 
'("wzgle~dem" prep-gen (((v z g l e~) 0) ((d e m) 0))))  ;with regard to
(lex.add.entry 
'("wzglêdem" prep-gen (((v z g l e~) 0) ((d e m) 0))))   ;with regard to
(lex.add.entry 
'("poniz*ej" prep-gen (((p o) 0) ((n i) 0) ((Z e j) 0)))) ;less than
(lex.add.entry 
'("poni¿ej" prep-gen (((p o) 0) ((n i) 0) ((Z e j) 0))))  ;less than

;pronouns,conjunctions etc (definitely unstressed)                                                                                             

;;(lex.add.entry 
;;'("sie~" pron (((s+ e) 0)))) ;reflexive pronoun
;;(lex.add.entry 
;;'("siê" pron (((s+ e) 0))))  ;reflexive pronoun
;; demonstrative pronouns
(lex.add.entry 
'("ten" pron (((t e n) 0)))  ; demonstrative pronoun
(lex.add.entry 
'("ta" pron (((t a) 0))))    ; demonstrative pronoun
(lex.add.entry 
'("to" pron (((t o) 0))))    ; demonstrative pronoun
(lex.add.entry 
'("co" pron (((ts o) 0))))   ; relative pronoun
(lex.add.entry 
'("i" conj (((i) 0))))           ; conjunction
(lex.add.entry 
'("a" conj (((a) 0))))           ;conjunction
(lex.add.entry 
'("ale" conj (((a l e) 0))))     ;conjunction
(lex.add.entry 
'("oraz" conj (((o) 0) ((r a s) 0))))  ;conjunction
(lex.add.entry 
'("ani" conj (((a) 0) ((n i) 0)))) ;conjunction
(lex.add.entry 
'("tez*" conj (((t e S) 0))))     ;conjunction
(lex.add.entry 
'("te¿" conj (((t e S) 0))))      ;conjunction
(lex.add.entry 
'("lub" conj (((l u p) 0))))      ;conjunction
(lex.add.entry 
'("czy" conj (((tS I) 0))))       ;conjunction
(lex.add.entry 
'("lecz" conj (((l e tS) 0))))    ;conjunction
(lex.add.entry 
'("bo" conj ((((b o) 0))))        ;conjunction
(lex.add.entry 
'("z*e" conj (((Z e) 0))))        ;conjunction
(lex.add.entry 
'("¿e" conj (((Z e) 0))))         ;conjunction
(lex.add.entry 
'("niz*" conj (((n i S) 0))))     ;conjunction
(lex.add.entry 
'("ni¿" conj (((n i S) 0))))      ;conjunction

; monosyllabic forms of possessive pronouns 
(lex.add.entry 
'("mu~j" pron (((m u j) o))))
(lex.add.entry 
'("mój" pron (((m u j) o))))
(lex.add.entry 
'("twu~j" pron (((t f u j) 0)))) 
(lex.add.entry 
'("twój" pron (((t f u j) 0)))) 
(lex.add.entry 
'("nasz" pron (((n a S) 0))))
(lex.add.entry 
'("wasz" pron (((v a S) 0))))
(lex.add.entry 
'("jej" pron (((j e j) 0))))
(lex.add.entry 
'("ich" pron (((i x) 0))))
(lex.add.entry 
'("swu~j" pron (((s f u j) 0))))
(lex.add.entry 
'("swój" pron (((s f u j) 0))))
(lex.add.entry 

;;personal pronouns
'("ja" pron (((j a) 0))))
(lex.add.entry 
'("ty" pron (((t I) 0))))
(lex.add.entry 
'("on" pron (((o n) 0))))
(lex.add.entry 
'("ona" pron (((o) 0) ((n a) 0))))
(lex.add.entry 
'("ono" pron (((o) 0) ((n o) 0))))
(lex.add.entry 
'("my" pron (((m I) 0))))
(lex.add.entry 
'("wy" pron (((v I) 0))))
(lex.add.entry 
'("oni" pron (((o) 0) ((n i) 0))))
(lex.add.entry 
'("one" pron (((o) 0) ((n e) 0))))
(lex.add.entry 
'("mnie" pron (((m n+ e) 0))))
(lex.add.entry 
'("mno~" pron (((m n o~) 0))))
(lex.add.entry 
'("mn±" pron (((m n o~) 0))))
(lex.add.entry 
'("mi" pron (((m i) 0))))
(lex.add.entry 
'("ciebie" pron (((ts+ e) 0) ((b j e) 0))))
(lex.add.entry 
'("ci" pron (((ts+ i) 0))))
(lex.add.entry 
'("cie~" pron (((ts+ e) 0))))
(lex.add.entry 
'("ciê" pron (((ts+ e) 0)))) 
(lex.add.entry 
'("tobie" pron (((t o) 0) ((b j e) 0))))
(lex.add.entry 
'("tobo~" pron (((t o) 0) ((b o~) 0))))
(lex.add.entry 
'("tob±" pron (((t o) 0) ((b o~) 0))))
(lex.add.entry 
'("jego" pron (((j e) 0) ((g o) 0))))
(lex.add.entry 
'("go" pron (((g o) 0))))
(lex.add.entry 
'("jemu" pron (((j e) 0) ((m u) 0))))
(lex.add.entry 
'("mu" pron (((m u) 0)))))
(lex.add.entry 
'("nim" pron (((n i m) 0))))
(lex.add.entry 
'("jej" pron (((j e j) 0))))
(lex.add.entry 
'("jo~" pron (((j o~) 0))))
(lex.add.entry 
'("j±" pron (((j o~) 0))))
(lex.add.entry 
'("nio~" pron (((n+ j o~) 0))))
(lex.add.entry 
'("ni±" pron  (((n+ j o~) 0))))
(lex.add.entry 
'("niej" pron (((n+ j e j) 0))))
(lex.add.entry 
'("je" pron (((j e) 0))))
(lex.add.entry 
'("nas" pron (((n a s) 0))))
(lex.add.entry 
'("nam" pron (((n a m) 0))))
(lex.add.entry 
'("nami" pron (((n a) 0) ((m i) 0))))
(lex.add.entry 
'("was" pron (((v a s) 0))))
(lex.add.entry 
'("wam" pron (((v a m) 0))))
(lex.add.entry 
'("wami" pron (((v a) 0) ((m i) 0))))
(lex.add.entry 
'("ich" pron (((i x) 0))))
(lex.add.entry 
'("im" pron (((i m) 0)))))
(lex.add.entry 
'("nich" pron (((n+ i x) 0))))
(lex.add.entry 
'("nimi" pron (((n+ i) 0) ((m i) 0))))

;;zwiesz paralingwistyka
(lex.add.entry
'("ooo" nn ((( B1 ) 0 ))))
(lex.add.entry
'("ccm" nn ((( B2 ) 0 ))))
(lex.add.entry
'("mhm" nn ((( B3 ) 0 ))))
(lex.add.entry
'("aaa" nn ((( B4 ) 0 ))))
(lex.add.entry
'("mhmaaa" nn ((( B5 ) 0 ))))
(lex.add.entry
'("haha" nn ((( B6 ) 0 ))))
(lex.add.entry
'("ble" nn ((( B7 ) 0 ))))
(lex.add.entry
'("hrr" nn ((( B8 ) 0 ))))


)

;;; Lexicon definition

(lex.create "cstr_pl")
(lex.set.phoneset "cstr_pl")
(lex.set.lts.method 'cstr_pl_lts_function)
;(lex.set.compile.file (path-append cstr_pl_em_dir "cstr_pl_lex.out"))
(cstr_pl_addenda)

(require 'cstr_pl_plr)

(define (cstr_pl::select_lexicon)
  "(cstr_pl::select_lexicon)
Set up the lexicon for cstr_pl."
  ;; Lexicon selection
  (lex.select "cstr_pl")
  (set! postlex_rules_hooks nil)
)

(define (cstr_pl::reset_lexicon)
  "(cstr_pl::reset_lexicon)
Reset lexicon information."
  t
)

(provide 'cstr_pl_lex)
