;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                       Copyright (c) 1996,1997                         ;;
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
;;;   First attempt at a linear regression model to predict F0 values.
;;;   This is an attempt to reimplement the work in Black and
;;;   Hunt ICSLP96, though this model probably isn't as good.
;;;



(require 'cstr_pl_accent_def)

(define (emph_syl syl)
  (if (string-equal (item.feat syl "tobi_accent") "NONE")
      0.0
      (if (string-equal (item.feat 
			 syl "R:SylStructure.parent.R:Token.parent.EMPH") "1")
	  2.0
	  0.0)))

(set! point_f0_lr_start
'(
( Intercept 227.083191  )
( lisp_sylaccent_hstar 22.948582  )
( lisp_sylaccent_mstar 0.000000  )
( lisp_sylaccent_lstar 0.000000  )
( lisp_sylboundary_xhpc 0.000000  )
( lisp_sylboundary_hpc 9.309658  )
( lisp_sylboundary_xlpc 0.889460  )
( lisp_sylboundary_lpc -8.597830  )
( lisp_sylboundary_mpc 0.820188  )
( p.lisp_sylaccent_hstar 26.309608  )
( p.lisp_sylaccent_mstar -15.710565  )
( p.lisp_sylaccent_lstar -48.467010  )
( pp.lisp_sylaccent_hstar -8.863343  )
( pp.lisp_sylaccent_mstar -3.752390  )
( pp.lisp_sylaccent_lstar -2.057967  )
( n.lisp_sylaccent_hstar 0.000000  )
( n.lisp_sylaccent_mstar 0.000000  )
( n.lisp_sylaccent_lstar 0.000000  )
( nn.lisp_sylaccent_hstar 0.000000  )
( nn.lisp_sylaccent_mstar 7.111853  )
( nn.lisp_sylaccent_lstar 2.849408  )
( p.lisp_sylboundary_xhpc 0.000000  )
( p.lisp_sylboundary_hpc 9.621013  )
( p.lisp_sylboundary_xlpc 0.000000  )
( p.lisp_sylboundary_lpc 8.992164  )
( p.lisp_sylboundary_mpc 0.000000  )
( pp.lisp_sylboundary_xhpc 0.000000  )
( pp.lisp_sylboundary_hpc -4.339097  )
( pp.lisp_sylboundary_xlpc 0.000000  )
( pp.lisp_sylboundary_lpc -7.106846  )
( pp.lisp_sylboundary_mpc -12.654645  )
( n.lisp_sylboundary_xhpc -48.000275  )
( n.lisp_sylboundary_hpc -7.953351  )
( n.lisp_sylboundary_xlpc 8.565118  )
( n.lisp_sylboundary_lpc 10.342055  )
( n.lisp_sylboundary_mpc 0.000000  )
( nn.lisp_sylboundary_xhpc 0.000000  )
( nn.lisp_sylboundary_hpc 0.000000  )
( nn.lisp_sylboundary_xlpc 0.000000  )
( nn.lisp_sylboundary_lpc 4.765723  )
( nn.lisp_sylboundary_mpc -1.439720  )
( pos_in_word 0.000000  )
( pp.syl_break 1.978405  )
( p.syl_break 5.389155  )
( syl_break -2.971599  )
( n.syl_break 0.000000  )
( nn.syl_break -0.815883  )
( pp.stress 0.000000  )
( p.stress 14.927862  )
( stress -5.334178  )
( n.stress 0.000000  )
( nn.stress -0.458603  )
( syl_in -2.247452  )
( syl_out 0.192226  )
( ssyl_in 0.000000  )
( ssyl_out 0.000000  )
( asyl_in 0.000000  )
( asyl_out 1.973858  )
( last_accent -1.847388  )
( next_accent 1.476241  )
( syl_numphones 0.000000  )
))


(set! point_f0_lr_mid
'(
( Intercept 181.645477  )
( lisp_sylaccent_hstar 46.219643  )
( lisp_sylaccent_mstar 0.000000  )
( lisp_sylaccent_lstar -43.307701  )
( lisp_sylboundary_xhpc 180.305496  )
( lisp_sylboundary_hpc 72.319702  )
( lisp_sylboundary_xlpc -28.095213  )
( lisp_sylboundary_lpc -8.140679  )
( lisp_sylboundary_mpc 3.475137  )
( p.lisp_sylaccent_hstar -24.830767  )
( p.lisp_sylaccent_mstar -2.947019  )
( p.lisp_sylaccent_lstar 0.000000  )
( pp.lisp_sylaccent_hstar 16.675552  )
( pp.lisp_sylaccent_mstar 5.502328  )
( pp.lisp_sylaccent_lstar 0.000000  )
( n.lisp_sylaccent_hstar 6.968537  )
( n.lisp_sylaccent_mstar 0.000000  )
( n.lisp_sylaccent_lstar 0.000000  )
( nn.lisp_sylaccent_hstar 8.813722  )
( nn.lisp_sylaccent_mstar 12.910862  )
( nn.lisp_sylaccent_lstar 8.778764  )
( p.lisp_sylboundary_xhpc 0.000000  )
( p.lisp_sylboundary_hpc -9.881900  )
( p.lisp_sylboundary_xlpc 0.000000  )
( p.lisp_sylboundary_lpc 0.000000  )
( p.lisp_sylboundary_mpc -11.295980  )
( pp.lisp_sylboundary_xhpc 0.000000  )
( pp.lisp_sylboundary_hpc 0.000000  )
( pp.lisp_sylboundary_xlpc 0.000000  )
( pp.lisp_sylboundary_lpc -6.316674  )
( pp.lisp_sylboundary_mpc -11.698448  )
( n.lisp_sylboundary_xhpc -21.423374  )
( n.lisp_sylboundary_hpc 2.937437  )
( n.lisp_sylboundary_xlpc 6.973793  )
( n.lisp_sylboundary_lpc -13.795300  )
( n.lisp_sylboundary_mpc 0.157556  )
( nn.lisp_sylboundary_xhpc 0.000000  )
( nn.lisp_sylboundary_hpc 0.000000  )
( nn.lisp_sylboundary_xlpc 0.000000  )
( nn.lisp_sylboundary_lpc 11.951673  )
( nn.lisp_sylboundary_mpc 0.000000  )
( pos_in_word 7.192840  )
( pp.syl_break 0.000000  )
( p.syl_break 15.426723  )
( syl_break -12.819371  )
( n.syl_break 0.000000  )
( nn.syl_break 3.217823  )
( pp.stress -16.555235  )
( p.stress 10.422596  )
( stress 10.363349  )
( n.stress -16.963490  )
( nn.stress -6.439211  )
( syl_in -3.071732  )
( syl_out 0.000000  )
( ssyl_in 2.875093  )
( ssyl_out 0.000000  )
( asyl_in 0.000000  )
( asyl_out 2.482778  )
( last_accent -1.647135  )
( next_accent 1.544728  )
( syl_numphones 14.692733 )
))

(set! point_f0_lr_end
'(
( Intercept 251.596588  )
( lisp_sylaccent_hstar 38.518406  )
( lisp_sylaccent_mstar 0.000000  )
( lisp_sylaccent_lstar -18.368965  )
( lisp_sylboundary_xhpc 152.834076  )
( lisp_sylboundary_hpc 63.489269  )
( lisp_sylboundary_xlpc -37.102856  )
( lisp_sylboundary_lpc -12.368526  )
( lisp_sylboundary_mpc 0.000000  )
( p.lisp_sylaccent_hstar 0.000000  )
( p.lisp_sylaccent_mstar 0.000000  )
( p.lisp_sylaccent_lstar 9.465753  )
( pp.lisp_sylaccent_hstar 0.000000  )
( pp.lisp_sylaccent_mstar 0.000000  )
( pp.lisp_sylaccent_lstar -3.215894  )
( n.lisp_sylaccent_hstar 13.015020  )
( n.lisp_sylaccent_mstar 0.000000  )
( n.lisp_sylaccent_lstar 0.000000  )
( nn.lisp_sylaccent_hstar 0.000000  )
( nn.lisp_sylaccent_mstar 11.728186  )
( nn.lisp_sylaccent_lstar 4.770989  )
( p.lisp_sylboundary_xhpc 0.000000  )
( p.lisp_sylboundary_hpc 0.000000  )
( p.lisp_sylboundary_xlpc 0.000000  )
( p.lisp_sylboundary_lpc -16.009277  )
( p.lisp_sylboundary_mpc -8.388575  )
( pp.lisp_sylboundary_xhpc 0.000000  )
( pp.lisp_sylboundary_hpc 0.000000  )
( pp.lisp_sylboundary_xlpc 0.000000  )
( pp.lisp_sylboundary_lpc 0.000000  )
( pp.lisp_sylboundary_mpc 2.464320  )
( n.lisp_sylboundary_xhpc 0.000000  )
( n.lisp_sylboundary_hpc 14.158019  )
( n.lisp_sylboundary_xlpc 0.000000  )
( n.lisp_sylboundary_lpc 3.115832  )
( n.lisp_sylboundary_mpc 2.228758  )
( nn.lisp_sylboundary_xhpc -51.690842  )
( nn.lisp_sylboundary_hpc 0.000000  )
( nn.lisp_sylboundary_xlpc 10.036235  )
( nn.lisp_sylboundary_lpc 12.045238  )
( nn.lisp_sylboundary_mpc -2.341511  )
( pos_in_word 0.000000  )
( pp.syl_break 1.691974  )
( p.syl_break 0.000000  )
( syl_break -2.300985  )
( n.syl_break -5.612568  )
( nn.syl_break 2.021977  )
( pp.stress 0.000000  )
( p.stress 0.000000  )
( stress 18.818323  )
( n.stress -12.498960  )
( nn.stress 0.000000  )
( syl_in -1.666471  )
( syl_out 0.000000  )
( ssyl_in 0.000000  )
( ssyl_out 0.000000  )
( asyl_in 0.000000  )
( asyl_out 4.703995  )
( last_accent -1.061787  )
( next_accent 1.257976  )
( syl_numphones -24.677370 )
))

(provide 'pointlr)

