;;;
;;; Post lexical rules

(define (pol_postlex_wj_insertion utt)
  "(postlex_wj_insertion utt)
Insertion of w or j between word boundaries ending starting with vowels."
  (mapcar
   (lambda (s)
     (let ((name (item.name s)))
       (cond
	((and (string-equal name "i")
	      (member_string (item.feat s "p.name") 
			     '("e~")))
	 (item.insert s (list "j") 'before))
	((and (member_string name '("e~"))
	      (member_string (item.feat s "p.name") 
			     '("e~")))
	 (item.insert s (list "w") 'before))
	((and (member_string name '("e~"))
	      (string-equal (item.feat s "p.name") "i"))
	 (item.insert s (list "j") 'before)))))
   (utt.relation.items utt 'Segment))
  utt)


;;o~ denasalisation

(define (pol_postlex_o_change utt)
  "(postlex_o_change utt)
Change of o~ between word boundaries ending ."
  (mapcar
   (lambda (s)
     (let ((name (item.name s)))
       (cond
	((and (string-equal name "o~")
	      (not (item.relation.next s 'SylStructure))
	      (not (item.next (item.relation.parent s 'SylStructure))))
	 (cond
	  ((member_string (item.feat s "n.name")
			  '("p" "b"))
	   (item.set_name s "o")
	   (item.insert s (list "m") 'after))
	  ((member_string (item.feat s "n.name")
			  '("k" "g"))
	   (item.set_name s "o")
	   (item.insert s (list "N") 'after))
	  ((member_string (item.feat s "n.name")
			  '("t" "d" "ts" "dz" "dZ" "tS"))
	   (item.set_name s "o")
	   (item.insert s (list "n") 'after))
	  ((member_string (item.feat s "n.name")
			  '("dz+" "ts+"))
	   (item.set_name s "o")
	   (item.insert s (list "n+") 'after))
	  ((member_string (item.feat s "n.name")
			  '("l" "w"))
	   (item.set_name s "o")))))))
   (utt.relation.items utt 'Segment))
  utt)

(set! pol_postlex_voicing_preps
      '("bez" "nad" "od" "pod" "przed" "przez")) ;;deleted z,w as they interfered with devoicing
(set! pol_postlex_envoiced
      '((p b)
	(t d)
	(k g)
	(f v)
	(s z)
	(S Z)
	(s+ z+)
	(ts dz)
	(tS dZ)
	(ts+ dz+)
	))

(define (pol_postlex_voicing utt)
  "(postlex_voicing utt)
Add voicing to final consonants in final syllable of words when
followed by a word with starts with a voiced consonants (except sonorants)."
  (mapcar
   (lambda (word)
     (let ((fsyl (item.relation.daughtern word 'SylStructure))
	   nph nword)
       (set! nword (item.next word))
       (if nword
	   (set! nph (item.relation.daughter1 
		      (item.relation.daughter1 nword 'SylStructure)
		      'SylStructure)))
       (if nph
	   (mapcar
	(lambda (s)
	  (if (and (string-equal "coda" (item.feat s "seg_onsetcoda"))
		   (string-equal "-" (item.feat s "ph_cvox"))
		   (or 
		    (and (string-equal "+" (item.feat nph "ph_cvox"))
			 ;; not sonorant
			 (member_string (item.feat nph "ph_ctype")
					'("a" "f" "s")))
		    (and (member_string (item.name word) 
					pol_postlex_voicing_preps)
		         (or	
			   (string-equal "+" (item.feat nph "ph_cvox"))
			   (string-equal "+" (item.feat nph "ph_vc"))))))
	      (let ((voiced (car (cdr (assoc 
				       (intern (item.name s)) 
				       pol_postlex_envoiced)))))
		(if voiced
		    (item.set_name s voiced)))))
	(item.relation.daughters fsyl 'SylStructure)))))
   (utt.relation.items utt 'Word))
  utt)

(define (pol_postlex_voicing utt)
  "(postlex_voicing utt)
Add voicing to final consonants in final syllable of words when
followed by a word with starts with a voiced consonants (except sonorants)."
  (mapcar
   (lambda (word)
     (let ((fsyl (item.relation.daughtern word 'SylStructure))
	   nph nword)
       (set! nword (item.next word))
       (if nword
	   (set! nph (item.relation.daughter1 
		      (item.relation.daughter1 nword 'SylStructure)
		      'SylStructure)))
       (if nph
	   (mapcar
	(lambda (s)
	  (if (and (string-equal "coda" (item.feat s "seg_onsetcoda"))
		   (string-equal "-" (item.feat s "ph_cvox"))
		   (or 
		    (and (string-equal "+" (item.feat nph "ph_cvox"))
			 ;; not sonorant
			 (member_string (item.feat nph "ph_ctype")
					'("a" "f" "s")))
		    (and (member_string (item.name word) 
					pol_postlex_voicing_preps)
		         (or	
			   (string-equal "+" (item.feat nph "ph_cvox"))
			   (string-equal "+" (item.feat nph "ph_vc"))))))
	      (let ((voiced (car (cdr (assoc 
				       (intern (item.name s)) 
				       pol_postlex_envoiced)))))
		(if voiced
		    (item.set_name s voiced)))))
	(item.relation.daughters fsyl 'SylStructure)))))
   (utt.relation.items utt 'Word))
  utt)

(define (pol_postlex_deletion utt)
  "(pl_postlex_deletion utt)
remove trailing t before unvoiced consonants combination."
  (mapcar
   (lambda (s)
      (format stderr "%s " (item.name s))
      (cond 
        ((and (string-equal (item.feat s "pp.name") "e")
    	      (string-equal (item.feat s "p.name") "s")
    	      (string-equal (item.feat s "name") "t")
    	      (or (string-equal (item.feat s "n.name") "p")
    	    	  (and (string-equal (item.feat s "n.name") "f")
    	    	       (string-equal (item.feat s "nn.name") "S"))
    	    	  (string-equal (item.feat s "n.name") "t")
    	    	  )
    	      )
    	      (item.delete s))
      )
    )
   (utt.relation.items utt 'Segment))
   (format stderr "\n")
  utt)

(provide 'cstr_pl_plr)
