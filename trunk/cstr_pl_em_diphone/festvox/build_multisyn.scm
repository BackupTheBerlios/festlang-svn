(set_backtrace t)

(defvar closure_regex ".*_cl")
(defvar sp_threshold 0.060) ; was 0.050 for utts 5, was 0.02 for 5b
(defvar sp_closure_threshold 0.100)

(defvar multisyn_lib_dir (path-append libdir "multisyn/"))
(if (not (member_string multisyn_lib_dir libdir))
    (set! load-path (cons multisyn_lib_dir load-path)))
(set! load-path (cons "festvox/" load-path))

(set! debug nil)

(define (check_script infile lexicon)
"(check_script infile lexicon)
Check the script for out of lexicon words."
  (let (fd p)
    (setup_phoneset_and_lexicon lexicon)

;; Open input file
  (set! p (load infile t))

  (mapcar
     (lambda (l)
       (check_script_entry (cadr l))
       t)
     p)
t))

(define (check_script_entry text)
 "(check_script_entry entry)
Check the words to see if they are in the lexicon."
 (let ((utt1 (utt.synth_toSegment (eval (list 'Utterance 'Text text)))))
   (mapcar
    (lambda (word)
      (if (not (lex.lookup_all (item.feat word 'name)))
	    (format t "\"%s\" not in lexicon.  lts give %l\n" (item.feat word 'name) (lex.lookup (item.feat word 'name)))))
    (utt.relation.items utt1 'Word))))


(define (make_initial_phone_labs infile outfile lexicon)
  "(build_prompt infile outfile lexicon)
For each utterances in prompt file, synth labels for aligning."
  (let (fd p ofd tfd 
	   (cpath (pwd)))

    (setup_phoneset_and_lexicon lexicon)

;; Open output file.  
  (set! ofd (fopen outfile "w"))
;; open train.scp
  (set! tfd (fopen "alignment/train.scp" "w"))
;; Open input file
  (set! p (load infile t))
  (mapcar
     (lambda (l)
       (format t "%s\n" (car l))
       (format tfd "%s/mfcc/%s.mfcc\n" cpath (car l))
       (do_synth (car l) (cadr l) ofd)
       t)
     p)
  (fclose ofd)
  (fclose tfd)
t))


(define (do_synth name text ofd)
  "(do_synth name text ofd)
Synthesize given text and save waveform and labels for prompts."
  (let ((utt1 (utt.synth_toSegment (eval (list 'Utterance 'Text text)))))
    (utt.save.mlf utt1 (format nil "%s.lab" name) ofd)))

(define (utt.synth_toSegment utt)
  (Initialize utt)
  (Text utt)
  (Token_POS utt)    ;; when utt.synth is called
  (Token utt)
  (POS utt)
  (Phrasify utt)
  (Word utt)
  (Pauses utt)
  (Intonation utt)
  (PostLex utt))

(define (utt.save.mlf utt file ofd)
  "(utt.save.mlf utt file)
Save segments and other info for aligning."
  (format ofd "#!MLF!#\n")
  (format ofd "\"*/%s\"\n" file)
  (mapcar
   (lambda (seg)
     (if (or (string-equal (item.feat seg 'ph_ctype) "s") ; add stop and affricate closures
	     (string-equal (item.feat seg 'ph_ctype) "a"))
	 (format ofd "%s_cl\n" (safe_name seg)))
     (if (or (string-equal (item.feat seg 'name) "#")    ; substitute # with sil - hack!
	     (string-equal (item.feat seg 'name) "pau"))
	 (format ofd "%s\n" "sil")
	 (format ofd "%s\n" (safe_name seg)))
     (if (seg_word_final seg)                        ; add sp at word boundaries
	 (format ofd "sp\n")))
   (utt.relation.items utt 'Segment))
  (format ofd ".\n")
  utt)

(define (safe_name seg)
"(safe_name seg)
Check and replace segment names that cause problems."
(let ((name (item.feat seg "name")))
  (cond
   ((string-equal name "?") "Q")
   (t name))))

(define (unsafe_name seg)
"(unsafe_name seg)
Check and unreplace segment names that cause problems."
(let ((name (item.feat seg "name")))
  (cond
   ((string-equal name "Q") "?")
   (t name))))

(define (seg_word_final seg)
"(seg_word_final seg)
Is this segment word final?"
  (let ((this_seg_word (item.parent (item.relation.parent  seg 'SylStructure)))
	(silence (car (cadr (car (PhoneSet.description '(silences))))))
	next_seg_word)
    (if (item.next seg)
	(set! next_seg_word (item.parent (item.relation.parent (item.next seg) 'SylStructure))))
    (if (or (equal? this_seg_word next_seg_word)
	     (string-equal (item.feat seg "name") silence))
	nil
	t)))

(define (setup_phoneset_and_lexicon lexicon)
  (cond
    ((string-equal lexicon "cstr_pl_lex")
    
    (require 'cstr_pl_lex)
    (require 'cstr_pl_lts)
    (require 'cstr_pl_int)
    (require 'cstr_pl_token)

    (cstr_pl::select_phoneset)
    (cstr_pl::select_tokenizer)
    (cstr_pl::select_tagger)
    (cstr_pl::select_lexicon)
    (cstr_pl::select_phrasing)
    (cstr_pl::select_intonation)
    (cstr_pl::select_duration)
    (cstr_pl::select_f0model)
    )))

;; Alignment stuff (From festvox limited domain code)

(define (build_pauses_utts file lexicon)
  "(build_pauses_utts file) 
For each utterances in a pause prompt file, build utterances."
  (setup_phoneset_and_lexicon lexicon)
  (let ((p (load file t)))
    (mapcar
     (lambda (l)
       (format t "%s\n" (car l))
       (align_pauses_utt (car l))
       t)
     p)
    t))

(define (align_pauses_utt name)
  "(align_utt name) 
Load in the actual aligned pause segments and put them into the synthesizer utterance."
  (let ((utt1 (Utterance Text nil))
	(silence (car (cadr (car (PhoneSet.description '(silences))))))
	actual-segments)
    
    (Initialize utt1)
    (utt.relation.load utt1 'Segment 
		       (format nil "lab/%s.lab" name))
    (mapcar
     (lambda (seg)
       (cond
	((and (not (string-equal (item.name seg) silence))
	      (or (string-equal (item.name seg) "sil")
		  (string-equal (item.name seg) "sp") ;hkt small pause
		  (string-equal (item.name seg) "H#")
		  (string-equal (item.name seg) "h#")))
	 (item.set_feat seg "name" silence))
	((and (string-equal (item.name seg) "DB")
	      (item.next seg))
	 (item.set_feat (item.next seg) "cl_end" (item.feat seg "end"))
	 (item.delete seg)
	 (set! seg nil)))
       (if seg
	   (item.set_feat seg "start" "F:standard+unisyn_start")))
     (utt.relation.items utt1 'Segment))

    (utt.set_feat utt1 "fileid" name)
    (utt.save utt1 (format nil "utt/%s.utt" name))
    t))



(define (build_utts file lexicon)
  "(build_utts file lexicon) 
For each utterances in prompt file, synthesize and merge aligned labels
to predicted labels building a new utterances and saving it."
  (setup_phoneset_and_lexicon lexicon)
  (let ((p (load file t)))
    (mapcar
     (lambda (l)
       (format t "%s\n" (car l))
       (align_utt (car l) (cadr l))
       t)
     p)
    t))

(define (align_utt name text)
  "(align_utts file) 
Synth an utterance and load in the actualed aligned segments and merge
them into the synthesizer utterance."
  (let ((utt1 (utt.synth_toSegment (eval (list 'Utterance 'Text text))))
	(silence (car (cadr (car (PhoneSet.description '(silences))))))
	segments actual-segments pms)

    (set! pms (track.load (format nil "pm/%s.pm" name)))
	
    (utt.relation.load utt1 'actual-segment 
		       (format nil "lab/%s.lab" name))
    
    (set! segments (utt.relation.items utt1 'Segment))
    (set! actual-segments (utt.relation.items utt1 'actual-segment))

;    (format t "       Segments are: %l\n" (mapcar (lambda (x) (item.name x)) segments))
;    (format t "Actual Segments are: %l\n" (mapcar (lambda (x) (item.name x)) actual-segments))

    ;; These should align, but if the labels had to be hand edited
    ;; then they may not. 
    (while (and segments actual-segments)
      (format debug "seg:%l    actual: %l\n" (item.name (car segments)) (item.name (car actual-segments)))
      (cond
       ;; PROCESS STOP AND AFFRICATE CLOSURES IN ACTUAL-SEGMENTS.
       ;; Delete `_cl` segments, from actual marking the end as the join point in the next token.
       ((string-matches (item.name (car actual-segments)) closure_regex)
	(item.set_feat (cadr actual-segments) 
		       "cl_end" 
		       (item.feat (car actual-segments) "end"))
	(set! actual-segments (cdr actual-segments)))
       ;; FIX WIERD SILENCES NAMES IN ACTUAL-SEGMENTS
       ;; Change all possible silence names to that specified by the first silence defined by the phoneset.
       ((and (not (string-equal (item.name (car actual-segments)) silence))
	     (or (string-equal (item.name (car actual-segments)) "sil")
		 (string-equal (item.name (car actual-segments)) "sp") ;hkt small pause
		 (string-equal (item.name (car actual-segments)) "H#")
		 (string-equal (item.name (car actual-segments)) "h#")))
	;(format t " Fixing silence name.\n")
	(item.set_feat (car actual-segments) "name" silence))
       ;; DELETE ZERO LENGTH SILENCES FROM ACTUAL SEGMENTS
       ((and (string-equal (item.name (car actual-segments)) silence)
	     (item.prev (car actual-segments))
	     (eq (- (parse-number (item.feat (car actual-segments) "end"))
		    (parse-number (item.feat (item.prev (car actual-segments)) "end")))
		 0))
       	;(format t " Deleting zero length silence.\n")
	(item.delete (car actual-segments))
	(set! actual-segments (cdr actual-segments)))
       ;; INSERT MISSMATCHED SILENCES THAT APPEAR IN ACTUAL-SEGMENTS
       ;; If a silence occurs in actual-segments then insert it in
       ;; segments if it is not too short.
       ((and (not (string-equal (item.name (car segments))
				(item.name (car actual-segments))))
	     (string-equal (item.name (car actual-segments)) silence))
	;(format t " mismatched silence from actual segments.\n")
	;; If the current silence segment is not one of the short segment exceptions,
	;; insert a matching silence in segments.
	;;           ;; a short amount of silence 
	(if (not (or (and (item.prev (car actual-segments))
			  (< (- (parse-number (item.feat (car actual-segments) "end"))
				(parse-number (item.feat (item.prev (car actual-segments)) "end")))
			     sp_threshold))
		     ;; or silence that probably should be part of a following closure.
		     (and (item.prev (car actual-segments))
			  (item.next (car actual-segments))
			  (string-matches (item.name (item.next (car actual-segments))) closure_regex)
			  (< (- (parse-number (item.feat (car actual-segments) "end"))
				(parse-number (item.feat (item.prev (car actual-segments)) "end")))
			     sp_closure_threshold))))

	    (begin
	      ;(format t "inserting silence from actual segments.\n")
	      (item.insert (car segments)
			   (list silence (list (list "end" (item.feat (car actual-segments) "end"))
					       (list "score" (normalise_score (car actual-segments)))
					       (list "start" "F:standard+unisyn_start")))
			   'before)))
	(set! actual-segments (cdr actual-segments)))
       ;; DELETE MISSMATCHED SILENCES THAT OCCUR IN SEGMENTS
       ;; If there is no corresponding silence in actual-segments that it has to go.
       ((and (not (string-equal (item.name (car segments))
				(item.name (car actual-segments))))
	     (string-equal (item.name (car segments)) silence))
	(item.delete (car segments))
	(set! segments (cdr segments)))
       ;; SAFE SEGMENT NAME TO UNDO
       ;; This deals with phone name substitutions which cause problems as filenames.
       ((and (not (string-equal (item.name (car segments))
				(item.name (car actual-segments))))
	     (not (string-equal (unsafe_name (car actual-segments))
				(item.name (car actual-segments)))))
	(item.set_feat (car actual-segments) "name" (unsafe_name (car actual-segments))))
       ;; MATCHING SEGMENT
       ;; Eventually segments can actually match!
       ((string-equal (item.name (car segments))
		      (item.name (car actual-segments)))
	(item.set_feat (car segments) "end" (item.feat (car actual-segments) "end"))
	(item.set_feat (car segments) "score" (normalise_score (car actual-segments)))
	(item.set_feat (car segments) "start" "F:standard+unisyn_start")
	; copy cl_end for stops
	(if (> (item.feat (car actual-segments) "cl_end") 0)
	    (item.set_feat (car segments) "cl_end" (item.feat (car actual-segments) "cl_end")))
	; add a feature for dipthongs
	(if (string-equal (item.feat (car segments) "ph_vc") "d")
	    (item.set_feat (car segments) "dipth" 1))
	; check for at least one pitchmark
	(if (not (check_pms (car segments) pms))
	    (begin
	      (format t "Bad pitchmarking: %s %s.\n" (item.feat (car segments) "name") (item.feat (car segments) "end"))
	      (item.set_feat (car segments) "bad" "no_pms")))
	(set! segments (cdr segments))
	(set! actual-segments (cdr actual-segments)))
       ;; ALIGNMENT MISSMATCH ERROR
       (t
	(format t
		"align missmatch at %s (%f) %s (%f)\n"
		(item.name (car segments))
		(item.feat (car segments) "end")
		(item.name (car actual-segments))
		(item.feat (car actual-segments) "end"))
;;	(error)
	(set! segments (cdr segments))
	(set! actual-segments (cdr actual-segments))
	))
      )

    (mapcar
     (lambda (seg)
      ;; shorten and split sliences
      (while (and (string-equal (item.name seg) silence)
		  (> (item.feat seg "segment_duration") 0.300))
              (cond
               ((string-equal "h#" (item.feat seg "p.name"))
                (item.set_feat (item.prev seg) "end"
                               (+ 0.150 (item.feat seg "p.end"))))
               ((and (string-equal silence (item.feat seg "p.name"))
                     (string-equal silence (item.feat seg "p.p.name")))
                (item.set_feat (item.prev seg) "end"
                               (+ 0.150 (item.feat seg "p.end")))
                (item.set_feat (item.prev seg) "name" silence))
               (t
                (item.insert seg
                             (list silence
                                   (list 
                                    (list "end" (+ 0.150 
						   (item.feat seg "p.end")))
				    (list "score" (item.feat seg "score"))
				    (list "start" "F:standard+unisyn_start")))
                             'before)))))
     (utt.relation.items utt1 'Segment))

    (utt.relation.delete utt1 'actual-segment)
    (utt.set_feat utt1 "fileid" name)
    ;; If we have an F0 add in targets too
    (if (probe_file (format nil "f0/%s.f0" name))
	(build::add_targets utt1))
    (utt.save utt1 (format nil "utt/%s.utt" name))
    (utt.save.segs utt1 (format nil "utt/%s.lab" name))
    t))


(define (build::add_targets utt)
)

(define (check_pms_2 seg pms)
  "(check_pms seg pms)
Check there is at least one pitchmark in this segment. Or synthesis will fail!"
)

(define (check_pms seg pms)
  "(check_pms seg pms)
Check there is at least one pitchmark in this segment. Or synthesis will fail!"
  (let (start end)
    (set! end (item.feat seg "end"))
    (if (item.prev seg)
	(set! start (item.feat seg "start"))
	(set! start 0))
    (if (equal? (track.index_below pms start) (track.index_below pms end))
	nil
	t)))

(define (normalise_score seg)
"(normalise_score score)
Normalise alignmnet score by length."
(let (start end)
  (set! end (item.feat seg "end"))
  (if (item.prev seg)
      (set! start (item.feat seg "start"))
      (set! start 0))
  (exp (/ (parse-number (item.feat seg "score")) (- end start)))))


(define (add_duration_info_utts file durfile)
  "(add_duration_info utts file durfile) 
Load average durations info and for each utterances in prompt file mark outliers."
  (let ((uttslist (load file t))
	(durslist (load durfile t))
	(utt (Utterance Text ""))
	mean sd log_seg_dur)
    (mapcar
     (lambda (li)
       (format t "%s\n" (car li))
       (utt.load utt (format nil "utt/%s.utt" (car li)))
       ;; process utterance
       (mapcar
	(lambda (seg)
	  (if (assoc_string (item.feat seg "name") durslist)
	      (begin
		(set! mean (car (cadr (assoc_string (item.feat seg "name") durslist))))
		(set! sd   (car (cdr (cadr (assoc_string (item.feat seg "name") durslist)))))
		(set! log_seg_dur (log (- (item.feat seg "end") (item.feat seg "start"))))
		;(format t "seg: %s dur: %f mean: %f sd: %f range: %f:%f\n" (item.feat seg "name") (exp log_seg_dur) mean sd (exp (- mean (* 2 sd))) (exp (+ mean (* 3 sd))))
		(if (or (> log_seg_dur (+ mean (* 3 sd)))
			(< log_seg_dur (- mean (* 2 sd))))
		    (begin
		      ;(format t "duration outlier\n")
		      (item.set_feat seg "bad_dur" "outlier")))))
	  t)
	(utt.relation.items utt 'Segment))
       (utt.save utt (format nil "utt/%s.utt" (car li)))
       t)
     uttslist)
    t))

(define (dump_word_labs file utt_dir word_dir)
  "(dump_word_labs file utt_dir word_dir) 
Assumes utterances are already built."
  (let ((p (load file t))
	(utt (Utterance Text nil))
	wordfile)
    
    (mapcar
     (lambda (l)
       (format t "%s\n" (car l))
       (utt.load utt (format nil "%s/%s.utt" utt_dir (car l)))
       (set! wordfile (format nil "%s/%s.word" word_dir (car l)))
       (utt.save.words utt wordfile)
       t)
     p)
    t))

