;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                         Copyright (c) 1998                            ;;
;;;                        All Rights Reserved.                           ;;
;;;                                                                       ;;
;;;  Permission to use, copy, modify, distribute this software and its    ;;
;;;  documentation for research, educational and individual use only, is  ;;
;;;  hereby granted without fee, subject to the following conditions:     ;;
;;;   1. The code must retain the above copyright notice, this list of    ;;
;;;      conditions and the following disclaimer.                         ;;
;;;   2. Any modifications must be clearly marked as such.                ;;
;;;   3. Original authors' names are not deleted.                         ;;
;;;  This software may not be used for commercial purposes without        ;;
;;;  specific prior written permission from the authors.                  ;;
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
;;; Phoneset definition 
;;;

(defPhoneSet
  cstr_pl
  ;;;  Phone Features
  (
   ;; vowel or consonant
   (vc + -)  
   ;; vowel characteristic: oral nasal
   (vcar o n 0)
   ;; vowel height: high mid low
   (vheight 1 2 3 -)
   ;; vowel frontness: front mid back
   (vfront 1 2 3 -)
   ;; lip rounding
   (vrnd + -)
   ;; consonant type: stop fricative affricate nasal lateral glide trill
   (ctype s f a n l g t 0)
   ;; place of articulation: labial alveolar palatal labio-dental
   ;;                         dental velar
   (cplace l a p b d v 0)
   ;; consonant voicing
   (cvox + -)
   ;; consonant quality: hard soft
   (cq h s 0 )     
    )
    
  (
   (B1 - 0 - - - 0 0 - 0)
   (B2 - 0 - - - 0 0 - 0)
   (B3 - 0 - - - 0 0 - 0)
   (B4 - 0 - - - 0 0 - 0)
   (B5 - 0 - - - 0 0 - 0)
   (B6 - 0 - - - 0 0 - 0)
   (B7 - 0 - - - 0 0 - 0)
   (B8 - 0 - - - 0 0 - 0)
   
   (#  - 0 - - - 0 0 - 0)
   (a  + o 3 2 - 0 0 - 0)
   (e  + o 2 1 - 0 0 - 0)
   (i  + o 1 1 - 0 0 - 0)
   (I  + o 1 1 - 0 0 - 0)
   (o  + o 2 3 + 0 0 - 0)
   (u  + o 1 3 + 0 0 - 0)
   (e~ + n 2 1 + 0 0 - 0)
   (o~ + n 2 3 + 0 0 - 0)

   (p - 0 - - - s l - h)
   (b - 0 - - - s l + h)
   (t - 0 - - - s d - h)
   (d - 0 - - - s d + h)
   (k - 0 - - - s v + h)
   (g - 0 - - - s v + h)
   (h - 0 - - - s v + h)
   (f - 0 - - - f b - h)
   (v - 0 - - - f b + h)
   (s - 0 - - - f d - h)
   (z - 0 - - - f d + h)
   (S - 0 - - - f a - h)
   (Z - 0 - - - f a + h)
  (s+ - 0 - - - f p - s)
  (z+ - 0 - - - f p + s)
   (x - 0 - - - f v - h)
  (ts - 0 - - - a d - h)
  (dz - 0 - - - a d + h)
  (tS - 0 - - - a a - h)
  (dZ - 0 - - - a a + h)
 (ts+ - 0 - - - a p - s)
 (dz+ - 0 - - - a p + s)
   (m - 0 - - - n l + h)
   (n - 0 - - - n d + h)
  (n+ - 0 - - - n p + s)
   (N - 0 - - - n v + h)
   (l - 0 - - - l a + h)
   (r - 0 - - - t a + h)
   (w - 0 - - - g v + h)
   (j - 0 - - - g p + h)
   
  )
)
(PhoneSet.silences '(#))

(define (cstr_pl::select_phoneset)
  "(cstr_pl::select_phoneset)
Set up phone set for cstr_pl."
  (Parameter.set 'PhoneSet 'cstr_pl)
  (PhoneSet.select 'cstr_pl)
)

(define (cstr_pl::reset_phoneset)
  "(cstr_pl::reset_phoneset)
Reset phone set for cstr_pl."
  t
)

(provide 'cstr_pl_phones)
