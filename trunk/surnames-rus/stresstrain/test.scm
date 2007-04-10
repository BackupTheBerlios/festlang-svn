

(load "train.tree.scm")
(load "words.scm")

;;; Lexical stress assignment
;;;

(define (add_lex_stress word pos phones tree)
  "(add_lex_stress word syls)
Predict lexical stress by decision tree."
  (let ((utt (Utterance Text ""))
	(si)
	(nphones))
    (utt.relation.create utt 'Letter)
    (set! si (utt.relation.append utt 'Letter))
    (item.set_feat si 'name "pau")
    (set! si (utt.relation.append utt 'Letter))
    (utt.relation.append utt 'Letter)
    
    (item.set_feat si 'pos pos)
    (item.set_feat si 'numsyls (count_syls phones))
    (item.set_feat si 'sylpos 1)
    (set! nphones (add_lex_stress_syl phones si tree))
;   (format t "%l\n" phones)
;   (format t "%l\n" nphones)
    nphones))

(define (count_syls phones)
  (cond
   ((null phones) 0)
   ((string-matches (car phones) "[aeyiou]")
    (+ 1 (count_syls (cdr phones))))
   (t (count_syls (cdr phones)))))

(define (add_lex_stress_syl phones si tree)
  "(add_lex_stress_syl phones si tree)
Add lexical stressing."
  (cond
   ((null phones) nil)
   ((string-matches (car phones) "[aeiouy]")
    (item.set_feat si 'phone (car phones))
    (item.set_feat si 'name (car phones))
    (item.set_feat si 'num2end 
			 (- (+ 1 (item.feat si 'numsyls))
			    (item.feat si 'sylpos)))
    (item.set_feat si 'sylpos
			 (+ 1 (item.feat si 'sylpos)))
    (if (null (cdr phones))
        (item.set_feat (item.next si) 'name "pau")
        (item.set_feat (item.next si) 'name (car (cdr phones))))

    (set! stress (wagon_predict si tree))

    (if t
    (format stderr "%l %d %d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" 
	    stress
	    (item.feat si 'num2end) 
	    (item.feat si 'sylpos)
	    (item.feat si 'ph_vfront)
	    (item.feat si 'ph_vheight)
	    (item.feat si 'ph_vrnd)

	    (item.feat si 'p.ph_vc)
	    (item.feat si 'p.ph_vfront)
	    (item.feat si 'p.ph_vheight)
	    (item.feat si 'p.ph_vrnd)
	    (item.feat si 'p.ph_cvox)
	    (item.feat si 'p.ph_ctype)
	    (item.feat si 'p.ph_cplace)
	    (item.feat si 'p.ph_csoft)

	    (item.feat si 'n.ph_vc)
	    (item.feat si 'n.ph_vfront)
	    (item.feat si 'n.ph_vheight)
	    (item.feat si 'n.ph_vrnd)
	    (item.feat si 'n.ph_cvox)
	    (item.feat si 'n.ph_ctype)
	    (item.feat si 'n.ph_cplace)
	    (item.feat si 'n.ph_csoft)

	    (item.feat si 'pos)
	    (item.feat (item.next si) 'name)
	    ))

    (item.set_feat (item.prev si) 'name (car phones)) 
    (cons
     (if (not (string-equal stress "0"))
	 (string-append (car phones) stress)
	 (car phones))
     (add_lex_stress_syl (cdr phones) si tree)))
   (t 
    (item.set_feat (item.prev si) 'name (car phones)) 
    (cons
     (car phones)
     (add_lex_stress_syl (cdr phones) si tree)))))

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
    (format stderr "%d %d %d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n" 
;	    (car (car (cdr stress)))
	    is_stress
	    (item.feat si 'num2end) 
	    (item.feat si 'sylpos)
	    (item.feat si 'ph_vfront)
	    (item.feat si 'ph_vheight)
	    (item.feat si 'ph_vrnd)

	    (item.feat si 'p.ph_vc)
	    (item.feat si 'p.ph_vfront)
	    (item.feat si 'p.ph_vheight)
	    (item.feat si 'p.ph_vrnd)
	    (item.feat si 'p.ph_cvox)
	    (item.feat si 'p.ph_ctype)
	    (item.feat si 'p.ph_cplace)
	    (item.feat si 'p.ph_csoft)

	    (item.feat si 'n.ph_vc)
	    (item.feat si 'n.ph_vfront)
	    (item.feat si 'n.ph_vheight)
	    (item.feat si 'n.ph_vrnd)
	    (item.feat si 'n.ph_cvox)
	    (item.feat si 'n.ph_ctype)
	    (item.feat si 'n.ph_cplace)
	    (item.feat si 'n.ph_csoft)

	    (item.feat si 'pos)
	    ))
    (item.set_feat (item.prev si) 'name (car phones)) 
    (dump_lex_stress (cdr phones) si stress))
   (t
     (item.set_feat (item.prev si) 'name (car phones)) 
     (dump_lex_stress (cdr phones) si stress))))

(voice_msu_ru_nsh_cg)

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
        
        (format stderr "%l %l\n" phones stress)

        (utt.relation.create utt 'Letter)
        (set! si (utt.relation.append utt 'Letter))
        (item.set_feat si 'name "pau")
        (set! si (utt.relation.append utt 'Letter))
	(utt.relation.append utt 'Letter)
	
        (item.set_feat si 'numsyls (count_syls phones))
        (item.set_feat si 'sylpos 1)
        (item.set_feat si 'pos (car (cdr stress)))

	(dump_lex_stress phones si (caar (cddr stress)))
    		
	(dump_lex_stress_feats (cdr words))))))

(define (guess_pos word)
""
    (let ((w (string-append word "#")))    
    (cond 
	    ((string-matches w ".*вич#") 'sname)
	    ((string-matches w ".*вна#") 'sname)
	    ((string-matches w ".*ин#") 'surname-in)
	    ((string-matches w ".*ина#") 'surname-in)
	    ((string-matches w ".*ян#") 'surname-yan)
	    ((string-matches w ".*янц#") 'surname-yan)
	    ((string-matches w ".*ев#") 'surname-ovev)
	    ((string-matches w ".*ева#") 'surname-ovev)
	    ((string-matches w ".*ёв#") 'surname-ovev)
	    ((string-matches w ".*ёва#") 'surname-ovev)
	    ((string-matches w ".*ов#") 'surname-ovev)
	    ((string-matches w ".*ова#") 'surname-ovev)
	    ((string-matches w ".*ский#") 'surname-ski)
	    ((string-matches w ".*ская#") 'surname-ski)
	    ((string-matches w ".*ко#") 'surname-ko)
	    ((string-matches w ".*их#") 'surname-ih)
	    ((string-matches w ".*ых#") 'surname-ih)
	    ((string-matches w ".*ук#") 'surname-uk)
	    ((string-matches w ".*ик#") 'surname-ik)
	    (t 'name))
))

(define (dump_results words)
  "(dump_results words)
Dump predicted results."
  (cond
    ((null words) nil)
    (t 
	(let ((utt (Utterance Text ""))
	     (si)
	     (stress)
	     (pos))
	(set! word (car words))
    
        (set! letters (lts.apply word 'russian_downcase))
        (set! phones (lts.apply letters 'msu_ru))
        
        (set! pos (guess_pos word))
        
        (set! stress (add_lex_stress word pos phones my_stress_tree))

        (format stderr "%l\n" stress)
        
        (dump_results (cdr words))
        ))))
