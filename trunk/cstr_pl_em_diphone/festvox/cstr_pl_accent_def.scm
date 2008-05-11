
(defvar boundaryre "x?[HML]\|")
(defvar accentre "[HML]")

(define (sylaccent_hstar syl)
"(sylaccent_hstar syll)
Is there accent information on that syllable"
(if 
   (and (string-matches (item.feat syl 'syl_accent) "H")
	(not (string-matches (item.feat syl 'syl_accent) boundaryre)))
   ;(format t "GOT H accent\n")
   ;(format t "NOT H\n")
1
0))

(define (sylaccent_mstar syl)
"(sylaccent_mstar syll)
Is there accent information on that syllable"
(if 
   (and (string-matches (item.feat syl 'syl_accent) "M")
	(not (string-matches (item.feat syl 'syl_accent) boundaryre)))
     ;(format t "GOT M accent\n")
1
0))

(define (sylaccent_lstar syl)
"(sylaccent_lstar syll)
Is there accent information on that syllable"
(if 
   (and (string-matches (item.feat syl 'syl_accent) "L")
	(not (string-matches (item.feat syl 'syl_accent) boundaryre)))
     ;(format t "GOT L accent\n")
1
0))

(define (syl_final_syl_in_phrase syl)
(if (eq? (item.feat syl 'syl_out) 0)
1
0))

(define(syl_penult_syl_in_word syl)
(if (eq? (item.feat syl "R:SylStructure.n.syl_break") 1)
1
0))

(define (sylaccent_accent syl)
"(sylaccent_accent syll)
Is there accent information on that syllable"
(if 
 (and (string-matches (item.feat syl 'syl_accent) accentre)
      (not (string-matches (item.feat syl 'syl_accent) boundaryre)))
 (item.feat syl 'syl_accent)
 "NONE"))

(define (sylaccent_boundary syl)
"(sylaccent_boundary syll)
Is there boundary information on that syllable"
(if (string-matches (item.feat syl 'syl_accent) boundaryre)
    (item.feat syl 'syl_accent)
    "NONE"))



(define (sylboundary_xhpc syl)
"(sylboundary_xhpc syll)
Is there boundary information on that syllable"
(if(string-matches (item.feat syl 'syl_accent) "xH\|")
;(format t "GOT xH boundary\n")
1
0))

(define (sylboundary_hpc syl)
"(sylboundary_hpc syll)
Is there boundary information on that syllable"
(if(string-matches (item.feat syl 'syl_accent) "H\|")
;(format t "GOT H boundary\n")
1
0))

(define (sylboundary_xlpc syl)
"(sylboundary_xlpc syll)
Is there boundary information on that syllable"
(if(string-matches (item.feat syl 'syl_accent) "xL\|")
;(format t "GOT xL boundary\n")
1
0))

(define (sylboundary_lpc syl)
"(sylboundary_lpc syll)
Is there boundary information on that syllable"
(if(string-matches (item.feat syl 'syl_accent) "L\|")
;(format t "GOT L boundary\n")
1
0))

(define (sylboundary_mpc syl)
"(sylboundary_mpc syll)
Is there boundary information on that syllable"
(if(string-matches (item.feat syl 'syl_accent) "M\|")
;(format t "GOT M boundary\n")
1
0))
