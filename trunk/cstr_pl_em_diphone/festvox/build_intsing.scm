;************************************************************************************;
;* Copyright ?????????                                                              *;
;* All rights reserved.                                                             *;  
;*                                                                                  *;
;************************************************************************************;
;*                                                                                  *;
;* AUTHOR: Aby Louw                                                                 *;
;*                                                                                  *;
;* MODIFIED: Feb 10, 2004                                                           *;
;*                                                                                  *;
;************************************************************************************;

(voice_cstr_pl_em_mbrola)

(define (build_words_lab file)
  "(build_words_lab file) 
For each utterances in prompt file, build a word label file for view
in emulabel."
  (let ((p (load file t)))
    (mapcar
     (lambda (l)
       (format t "%s\n" (car l))
       (let ((my_utt (load_my_utt (car l) (cadr l))))
	 (let ((fd (fopen 
		   (string-append 
		    "./festival/intsint/words/"
		    (car l)
		    ".words")
		    "w")))
	   (format fd "signal %s\n" (car l))
	   (format fd "nflieds 1\n")
	   (format fd "#\n")
	   (let ((pause (utt.relation.first my_utt 'Segment)))
	     (if (string-equal (item.name (item.next pause)) "pau")
		 (set! pause (item.next pause)))
	     (format fd "%2.4f 100 %s\n" (item.feat pause "end") (item.name pause)))
	   (let ((word (utt.relation.first my_utt 'SylStructure)))
	     (while word
		    (if (not (string-equal (item.feat word "pos") "punc" ))
			(begin
			  (let ((syl (item.daughtern word)))
			    (let ((phoneme (item.daughtern syl)))
			      (format fd "%2.4f 100 %s\n" (item.feat phoneme "end") (item.name word))))))
		    (set! word (item.next word))))
	   (fclose fd)))
       t)
     p)
    t))

(define (build_syls_lab file)
  "(build_syls_lab file) 
For each utterances in prompt file, build a syllable label file for view
in emulabel."
  (let ((p (load file t)))
    (mapcar
     (lambda (l)
       (format t "%s\n" (car l))
       (let ((my_utt (load_my_utt (car l) (cadr l))))
	 (let ((fd (fopen 
		   (string-append 
		    "./festival/intsint/syllables/"
		    (car l)
		    ".syls")
		    "w")))
	   (format fd "signal %s\n" (car l))
	   (format fd "nflieds 1\n")
	   (format fd "#\n")
	   (let ((pause (utt.relation.first my_utt 'Segment)))
	     (if (string-equal (item.name (item.next pause)) "pau")
		 (set! pause (item.next pause)))
	     (format fd "%2.4f 100 %s\n" (item.feat pause "end") (item.name pause)))
	   (let ((word (utt.relation.first my_utt 'SylStructure)))
	     (while word
		    (if (not (string-equal (item.feat word "pos") "punc" ))
			(begin
			  (let ((syl (item.daughter1 word)))
			    (while syl
				   (let ((phoneme (item.daughtern syl)))
				     (format fd "%2.4f 100 %s\n" (item.feat phoneme "end") (item.name syl)))
				   (set! syl (item.next syl))))))
		    (set! word (item.next word))))
	   (fclose fd)))
       t)
     p)
    t))



(define (load_my_utt name text)
  "(load_utt NAME TEXT)
Load utterance into a utterance relation."
  (let ((utt_seg (Utterance Text text)))
    (utt.load utt_seg 
	      (string-append 
	       "./festival/utts/"
	       name
	       ".utt"))
    utt_seg))


(set! cstr_pl_em::intsint_params
      '(
	(utts_dir "festival/utts/")
	(utts_ext ".utt")
	(momelqs_dir "./festival/intsint/momel_qs/")
	(momelqs_ext ".momelqs")
	(momel_dir "./festival/intsint/momel/")
	(momel_ext ".momel")
	(intsint_dir "./festival/intsint/syl_intsint_lab/")
	(intsint_ext ".intsint")
	(intsint_utt_dir "festival/intsint/utts/")
	(intsint_utt_ext ".utt")
	(intsint_momelqs_dir "./festival/intsint/intsint_momel_qs/")
	(intsint_momelqs_ext ".imomelqs")
	(intsint_defs "etc/intsint.defs")
	;; intsint specific stuff
	(intsint_threshold 0.10)
	(pause_length 0.20)
	(intsint_min_range 0.5)     ;; octave f0
	(intsint_max_range 2.5)     ;; octave f0
	(intsint_step_range 0.1)    ;; octave f0
	(intsint_mean_shift 70)     ;; linear f0
	(intsint_step_shift 1)      ;; linear f0
	(intsint_higher 0.5)        ;; octave f0
	(intsint_lower 0.5)         ;; octave f0
	(intsint_up 0.25)           ;; octave f0
	(intsint_down 0.25)         ;; octave f0
	(only_stressed_syls 1)))

(define (optimise_intsint_labels file)
  "(optimise_intsint_labels file) 
For each utterance file (./festival/utts/) optimise the T,B,M instint labels for training"
  ;;load utts
  (set! intsint_list nil)
  (let ((p (load file t)))
    (mapcar
     (lambda (l)
       (set! intsint_list (cons (car l) intsint_list))
       t)
     p)
    t)
  
  (intsint::load_db cstr_pl_em::intsint_params intsint_list)
  (intsint::isdb_add_intsint_labels )
  (intsint::isdb_optimise )
  (intsint::isdb_save_intsint_labels )
  (intsint::isdb_save_intsint_utts )
  (intsint::isdb_calc_intsint_momelqs )
  )
 
(define (finalise_intsint_model modelname treename)
  "(finalise_intsint_model modelname treename)
Take the tree and create a scheme file which can
be useds as an INTSINT intonation model."
  (require 'cart_aux)
  (let (tree ofd)
    (set! tree (car (load (format nil "festival/intsint/tree/%s" treename) t)))
    (set! tree (cart_simplify_tree tree nil))
    (set! ofd (fopen (format nil "festvox/%s_intsint_tree.scm" modelname) "w"))
    (if (string-matches modelname "^(.*")
	(set! modelname (string-after modelname "(")))
    (if (string-matches modelname ".*)$")
	(set! modelname (string-before modelname ")")))
    (format ofd ";; INTSINT intonation model autotrained by festvox\n")
    (format ofd ";; %s\n" treename)
    (format ofd "(set! %s::intsint_tree '\n" modelname)
    (pprintf tree ofd)
    (format ofd ")\n")
    (format ofd "\n\n")
    (format ofd "(provide '%s)\n" (string-append modelname "_intsint_tree"))
    (format ofd "\n\n")
    (fclose ofd)
    )
  
)

(provide 'build_INTSINT_intonation)