

(load "words.scm")


(voice_msu_ru_nsh_clunits)

(Parameter.set 'Synth_Method 'None)

;;; Lexical stress assignment
;;;

(define (count_syls phones)
  (cond
   ((null phones) 0)
   ((string-matches (car phones) "[aeyiou]")
    (+ 1 (count_syls (cdr phones))))
   (t (count_syls (cdr phones)))))

(define (get_last phones) 
    (cond 
     ((null (cddr (cdddr phones))) phones)
     (t (get_last (cdr phones)))
))

(define (dump_lex_stress phones si stress)
  "(dump_lex_stress phones si stress)
Dump lex stress."
  (cond
   ((null phones) nil)
   ((string-matches (car phones) "[ayouei]*")
    (item.set_feat si 'phone (car phones))
    (item.set_feat si 'name (car phones))
    (if (null (cdr phones))
        (item.set_feat (item.next si) 'name "pau")
        (item.set_feat (item.next si) 'name (car (cdr phones))))
    (item.set_feat si 'num2end 
			 (- (+ 1 (item.feat si 'numsyls))
			    (item.feat si 'sylpos)))
    (item.set_feat si 'sylpos
			 (+ 1 (item.feat si 'sylpos)))

    (let 
	((is_stress (cond 
		((eq (+ stress 1) (item.feat si 'sylpos)) 1)
		 (t 0)
	    )))
    (format stderr "%d %d %d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" 
	    is_stress
	    (item.feat si 'num2end) 
	    (item.feat si 'sylpos)
	    
	    (item.feat si 'name)
	    (item.feat si 'ph_vfront)
	    (item.feat si 'ph_vheight)
	    (item.feat si 'ph_vrnd)

	    (item.feat si 'pp.name)

	    (item.feat si 'p.name)
	    (item.feat si 'p.ph_vc)
	    (item.feat si 'p.ph_vfront)
	    (item.feat si 'p.ph_vheight)
	    (item.feat si 'p.ph_vrnd)
	    (item.feat si 'p.ph_cvox)
	    (item.feat si 'p.ph_ctype)
	    (item.feat si 'p.ph_cplace)
	    (item.feat si 'p.ph_csoft)

	    (item.feat si 'n.name)
	    (item.feat si 'n.ph_vc)
	    (item.feat si 'n.ph_vfront)
	    (item.feat si 'n.ph_vheight)
	    (item.feat si 'n.ph_vrnd)
	    (item.feat si 'n.ph_cvox)
	    (item.feat si 'n.ph_ctype)
	    (item.feat si 'n.ph_cplace)
	    (item.feat si 'n.ph_csoft)

	    (car (cddr phones))
	    (car (cdddr phones))
	    (car (cdr (cdddr phones)))

	    (item.feat si 'pos)
	    (car (item.feat si 'last))
	    (cadr (item.feat si 'last))
	    (caddr (item.feat si 'last))
	    (car (cdddr (item.feat si 'last)))
	    (cadr (cdddr (item.feat si 'last)))
	    ))
    (item.set_feat (item.prev (item.prev si)) 'name (item.feat si 'p.name)) 
    (item.set_feat (item.prev si) 'name (car phones)) 
    (dump_lex_stress (cdr phones) si stress))
   (t
     (item.set_feat (item.prev (item.prev si)) 'name (item.feat si 'p.name)) 
     (item.set_feat (item.prev si) 'name (car phones)) 
     (dump_lex_stress (cdr phones) si stress))))



(define (dump_lex_stress_feats words)
  "(dump_lex_stress_feats words)
Dump features for lexical stress prediction."
  (cond
    ((null words) nil)
    (t 
	(let ((utt (Utterance Text ""))
	     (si)
	     (stress))
	(set! word (car words))
    
        (set! letters (lts.apply word 'russian_downcase))
        (set! phones (lts.apply letters 'msu_ru))
        (set! syls (russian_syllabify phones nil))
        (set! stress (lex.lookup word))
        
        (format stderr "%l %l %l\n" phones stress (get_last phones))

        (utt.relation.create utt 'Letter)
        (set! si (utt.relation.append utt 'Letter))
        (item.set_feat si 'name "pau")
        (set! si (utt.relation.append utt 'Letter))
        (item.set_feat si 'name "pau")
        (set! si (utt.relation.append utt 'Letter))
	(utt.relation.append utt 'Letter)
	
        (item.set_feat si 'numsyls (count_syls phones))
        (item.set_feat si 'sylpos 1)
        (item.set_feat si 'pos (car (cdr stress)))
	(item.set_feat si 'last (get_last phones))

	(dump_lex_stress phones si (caar (cddr stress)))
    		
	(dump_lex_stress_feats (cdr words))))))

(define (dump_dict words)
  "(dump_dict words)
Dump dictionary with results."
  (cond
    ((null words) nil)
    (t 
	(format t "%s" (car words))
	(utt.synth (eval (list 'Utterance 'Text (car words))))
        (dump_dict (cdr words)))))
