(load "festvox/msu_ru_nsh_clunits.scm")

(voice_msu_ru_nsh_clunits)

(set! infile (fopen "input.txt" "r"))
(set! outfile (fopen "output.txt" "w"))

(Parameter.set 'Synth_Method 'None)

(while (not (equal? (set! line (readfp infile)) (eof-val)))
      (let ((utt (utt.synth (eval (list 'Utterance 'Text line)))))
         (format outfile "%s%s " (string-before line "+") (string-after line "+"))
         (mapcar
	     (lambda (seg)
	       (if (not (string-equal (item.name seg) "pau"))
	        (format outfile "%s " (item.name seg))))
	     (utt.relation.items utt 'Segment))
         (format outfile "\n")))

(fclose infile)
(fclose outfile)

