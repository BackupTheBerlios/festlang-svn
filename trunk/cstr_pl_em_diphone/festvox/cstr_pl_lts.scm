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
;;;  Polish letter to sound rules
;;; down case mapping

(lts.ruleset
 pol_downcase
 ( )
 (

 ;; special chars. mapping   
 
 ( [ D ¯ ] = d ¿ )
 ( [ D ¬ ] = d ¼ )
 
 ( [ C Z ] = c z )
 ( [ S Z ] = s z )
 ( [ C H ] = c h )

 ( [ ¡ ] = ± )
 ( [ Ê ] = ê )
 ( [ Ó ] = ó )
 ( [ ¦ ] = ¶ )
 ( [ Æ ] = æ )
 ( [ ¬ ] = ¼ )
 ( [ ¯ ] = ¿ )
 ( [ £ ] = ³ )
 ( [ Ñ ] = ñ )


 ( [ d ¿ ] = d ¿ )
 ( [ d ¼ ] = d ¼ )
 
 ( [ c z ] = c z )
 ( [ s z ] = s z )
 ( [ c h ] = c h )

 ( [ ± ] = ± )
 ( [ ê ] = ê )
 ( [ ó ] = ó )
 ( [ ¶ ] = ¶ )
 ( [ æ ] = æ )
 ( [ ¼ ] = ¼ )
 ( [ ¿ ] = ¿ )
 ( [ ³ ] = ³ )
 ( [ ñ ] = ñ )

 ( [ D Z * ] = d ¿ )
 ( [ D Z ~ ] = d ¼ )
 ( [ O ~ ] = ± )
 ( [ E ~ ] = ê )
 ( [ U - ] = ó )
 ( [ C ~ ] = æ )
 ( [ S ~ ] = ¶ )
 ( [ Z ~ ] = ¼ )
 ( [ Z * ] = ¿ )
 ( [ L / ] = ³ )
 ( [ N ~ ] = ñ )

 ( [ d z * ] = d ¿ )
 ( [ d z ~ ] = d ¼ )
 ( [ o ~ ] = ± )
 ( [ e ~ ] = ê )
 ( [ u ~ ] = ó )
 ( [ c ~ ] = æ )
 ( [ s ~ ] = ¶ )
 ( [ z ~ ] = ¼ )
 ( [ z * ] = ¿ )
 ( [ l / ] = ³ )
 ( [ n ~ ] = ñ )

 ( [ R Z ] = r z )
 ( [ r z ] = r z )
 ( [ D Z ] = d z ) 
 ( [ d z ] = d z ) 
 ;; normal characters down_mapping

  ( [ I ] = i )
  ( [ Y ] = y )
  ( [ E ] = e )
  ( [ A ] = a )
  ( [ O ] = o )
  ( [ U ] = u )

  ( [ P ] = p )
  ( [ B ] = b )
  ( [ T ] = t )
  ( [ D ] = d )
  ( [ K ] = k )
  ( [ G ] = g )
  ( [ F ] = f )
  ( [ W ] = w )
  ( [ S ] = s )
  ( [ Z ] = z )
  ( [ H ] = h )
  ( [ C ] = c )
  ( [ M ] = m )
  ( [ N ] = n )
  ( [ L ] = l )
  ( [ R ] = r )
  ( [ J ] = j ) 
  ( [ V ] = v )
  ( [ X ] = x )
  ( [ Q ] = q )

  ( [ i ] = i )
  ( [ y ] = y )
  ( [ e ] = e )
  ( [ a ] = a )
  ( [ o ] = o )
  ( [ u ] = u )
 	

  
  ( [ p ] = p )
  ( [ b ] = b )
  ( [ t ] = t )
  ( [ d ] = d )
  ( [ k ] = k )
  ( [ g ] = g )
  ( [ f ] = f )
  ( [ w ] = w )
  ( [ s ] = s )
  ( [ z ] = z )
  ( [ h ] = h )
  ( [ c ] = c )
  ( [ m ] = m )
  ( [ n ] = n )
  ( [ l ] = l )
  ( [ r ] = r )
  ( [ j ] = j ) 
  ( [ v ] = v )
  ( [ x ] = x )
  ( [ q ] = q )

 ))  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; main letter to sound rules


(lts.ruleset
					;  Name of rule set
 polish
					;  Sets used in the rules
 (

  ( CON1 p b m n w f l ñ r k h g t d );; for purpose of softness
  ( V1 a e y ± ê u ó o )        ;; for purposes of softness
  ( CON2 b d g z ¿ ¼ p t k f s ¶ c æ h);; for assimilation purposes
  ( VC b d g z ¿ ¼ )               ;; voiced consonants but w & rz
  ( UC p t k f s ¶ c æ h )         ;; unvoiced consonants
  ( A # a e i y ê ± u ó o w m n r l ³);; vowels and end/beginning of
                                         ;; word and exceptions
  ( A1 # a e i y ± ê u ó o m n r l ³ );; to make sure w rz doesn't
                                         ;;  apply
  ( F f w s z ¶ ¼ ¿ h  )        ;; fricatives
  ( S p b t d k g )                      ;;  stops
  ( AFF c æ )                         ;; affricates
  ( NEUT m n l ³ r )         ;; do not check assimilation
  ( PB p b )                    ;; helpful with o~ e~ losing their character
  ( TDC t d c )                 ;; - || -
  ( DEN s z )                   ;; dentals
  ( KG k g )                    ;; velars

  ( PREP1 b d g w z ¿ ¼ ) ;; some prepositions followed by these voiced 
                                ;; cons.
  ( PREP2 ¶  æ )          ;; for preposition z
  ( PREP3 ¿ ¼)            ;; for preposition z
  ( PREP5 ¿ ¼ )           ;; for bez 
  )
 ;  Rules
 (

  ;; nasal vowels losing their character

  
  ( [ ê ] PB = e m )
  ( [ ± ] PB = o m )
 
  ( [ ê ] c z =  e n )
  ( [ ê ] d ¿ =  e n )
  ( [ ± ] c z = o n )
  ( [ ± ] d ¿ = o n )
  ( [ ê ] æ = e n+ )
  ( [ ê ] d ¼ = e n+ )
  ( [ ± ] æ = o n+ )
  ( [ ± ] d ¼ = o n+ ) 
;;ew dowywalenia
  ( [ ê ] ¶ = e n+ ) ;; wiezienie wiez
  ( [ ê ] z i = e n+ )
  ( [ ê ] ¼ = e n+ )
  ;; ( [ ê ] # = e ) 
  ( [ ê ] KG = e N )
  ( [ ± ] KG = o N )
  ( [ ê ] l = e )
  ( [ ê ] ³ = e )
  ( [ ± ] l = o )
  ( [ ± ] ³ = o )
  ( [ ê ] d z i = e n+ )
  ( [ ± ] d z i = o n+ )
  ( [ ê ] c i = e n+ )
  ( [ ± ] c i = o n+ )
  ( [ ê ] d z = e n )
  ( [ ± ] d z = o n )
  ( [ ê ] d r z = e n )
  ( [ ± ] d r z = o n )
  ( [ ê ] TDC = e n )
  ( [ ± ] TDC = o n )

  ;; making softness  palatalised

  ( CON1 [ i ] V1 = j ) 
  ( c h [ i ] V1 = j )

;;in the words in which the prefix od- or pod- is followed by a z,¼, or ¿ as
;;the initial sound of the next part of the word so that the groups dz,dzi,d¿,
;;are formed in spelling as in 'odzyskaæ' 'podziemny' 'od¿yæ'. such groups are
;;not pronounced as one affricate sound
  

  ;;( # p o [ d ] ¼ = d )
  ;;( # p o [ d ] ¿ = d )
  ;;( # p o [ d ] z i = d )

  ;;( # o [ d ] ¼ = d )
  ;;( # o [ d ] ¿ = d )
  ;;( # o [ d ] z i = d );;determine prefix



;; making softness  palatalised
  ( [ c i ] V1 = ts+ )
  ( [ s i ] V1 = s+ )
  ( [ z i ] V1 = z+ )
  ( [ n i ] V1 = n+ )
  ( [ d z i ] V1 = dz+ )


  ( [ c ] i = ts+ )
 ( [ s ] i = s+ )
  ( [ z ] i = z+ )
  ( [ n ] i = n+ )
  ( [ d z ] i = dz+ )



  ;; regressive assimilation 
  ;; takes care of four types of situations: (1) vc uc, (2) uc vc,   
  ;; (3) vc uc uc, (4) uc vc vc including these chunks at the 
  ;; beginning, middle and end of word

  ;; exceptions,
  
  ( [ s z ] c z = S )
  ( [ s z ] s z = S )
  ( [ c z ] c z = tS )
  ( [ c z ] s z = tS )
  ( s z [ w ] A1 = f )
  ( c z [ w ] A1 = f )
  ( c h [ w ] A1 = f )
  ( UC NEUT * [ w ] A1 = f )
  ( s z [ r z ] A1 = S ) 
  ( c z [ r z ] A1 = S ) 
  ( c h [ r z ] A1 = S ) 
  ( UC NEUT * [ r z ] A1 = S ) 

  ( A1 [ w ] CON2 * s z A1 = f )
  ( A1 [ w ] CON2 * c z A1 = f )
  ( A1 [ w ] CON2 * c h A1 = f )
  ( A1 [ w ] CON2 * UC A1 = f )

  ( A1 [ r z ] CON2 * s z A1 = S )
  ( A1 [ r z ] CON2 * c z A1 = S )
  ( A1 [ r z ] CON2 * c h A1 = S )
  ( A1 [ r z ] CON2 * UC A1 = S )

  ( A1 [ w ] s z A1 = f )
  ( A1 [ w ] c z A1 = f )
  ( A1 [ w ] c h A1 = f )
  ( A1 [ w ] UC + A1 = f )

  ( A1 [ r z ] s z A1 = S )
  ( A1 [ r z ] c z A1 = S )
  ( A1 [ r z ] c h A1 = S )
  ( A1 [ r z ] UC + A1 = S )

 

  ;; regressive assimilation ruls covering 4 cases: (1) vc  any cons  uc    
  ;; (2) uc any cons  vc, (3) vc any cons vc, (4) uc any cons uc : but 3 i
  ;; 4 not needed.

  ;; 1 

	
  ( [ b ] NEUT * CON2 * NEUT * s z NEUT * A = p )
  ( [ b ] NEUT * CON2 * NEUT * s z NEUT * r z = p )
  ( [ b ] NEUT * CON2 * NEUT * c z NEUT * A = p )
  ( [ b ] NEUT * CON2 * NEUT * c z NEUT * r z = p )
  ( [ b ] NEUT * CON2 * NEUT * c h NEUT * A = p )
  ( [ b ] NEUT * CON2 * NEUT * c h NEUT * r z = p )
  ( [ b ] NEUT * CON2 * NEUT * UC NEUT * A = p )
  ( [ b ] NEUT * CON2 * NEUT * UC NEUT * r z = p )

  ( [ g ] NEUT * CON2 * NEUT * s z NEUT * A = k )
  ( [ g ] NEUT * CON2 * NEUT * s z NEUT * r z = k )
  ( [ g ] NEUT * CON2 * NEUT * c z NEUT * A = k )
  ( [ g ] NEUT * CON2 * NEUT * c z NEUT * r z = k )
  ( [ g ] NEUT * CON2 * NEUT * c h NEUT * A = k )
  ( [ g ] NEUT * CON2 * NEUT * c h NEUT * r z = k )
  ( [ g ] NEUT * CON2 * NEUT * UC NEUT * A = k )
  ( [ g ] NEUT * CON2 * NEUT * UC NEUT * r z = k )

 
  ( [ ¿ ] NEUT * CON2 * NEUT * s z NEUT * A = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * s z NEUT * r z = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * c z NEUT * A = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * c z NEUT * r z = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * c h NEUT * A = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * c h NEUT * r z = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * UC NEUT * A = S )
  ( [ ¿ ] NEUT * CON2 * NEUT * UC NEUT * r z = S )


  ( [ ¼ ] NEUT * CON2 * NEUT * s z NEUT * A = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * s z NEUT * r z = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * c z NEUT * A = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * c z NEUT * r z = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * c h NEUT * A = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * c h NEUT * r z = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * UC NEUT * A = s+ )
  ( [ ¼ ] NEUT * CON2 * NEUT * UC NEUT * r z = s+ )

  ( [ r z ] NEUT * CON2 * NEUT * s z NEUT * A = S )
  ( [ r z ] NEUT * CON2 * NEUT * c z NEUT * A = S )
  ( [ r z ] NEUT * CON2 * NEUT * c h NEUT * A = S )
  ( [ r z ] NEUT * CON2 * NEUT * UC NEUT * A = S )

  ( [ d z ] NEUT * CON2 * NEUT * s z NEUT * A = ts )
  ( [ d z ] NEUT * CON2 * NEUT * s z NEUT * r z = ts )
  ( [ d z ] NEUT * CON2 * NEUT * c z NEUT * A = ts )
  ( [ d z ] NEUT * CON2 * NEUT * c z NEUT * r z = ts )
  ( [ d z ] NEUT * CON2 * NEUT * c h NEUT * A = ts )
  ( [ d z ] NEUT * CON2 * NEUT * c h NEUT * r z = ts )
  ( [ d z ] NEUT * CON2 * NEUT * UC NEUT * A = ts )
  ( [ d z ] NEUT * CON2 * NEUT * UC NEUT * r z = ts )

  ( [ d ¿ ] NEUT * CON2 * NEUT * s z NEUT * A = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * s z NEUT * r z = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * c z NEUT * A = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * c z NEUT * r z = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * c h NEUT * A = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * c h NEUT * r z = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * UC NEUT * A = tS )
  ( [ d ¿ ] NEUT * CON2 * NEUT * UC NEUT * r z = tS )

  ( [ d ¼ ] NEUT * CON2 * NEUT * s z NEUT * A = ts+ ) 
  ( [ d ¼ ] NEUT * CON2 * NEUT * s z NEUT * A = ts+ )
  ( [ d ¼ ] NEUT * CON2 * NEUT * c z NEUT * A = ts+ ) 
  ( [ d ¼ ] NEUT * CON2 * NEUT * c z NEUT * A = ts+ )
  ( [ d ¼ ] NEUT * CON2 * NEUT * c h NEUT * A = ts+ ) 
  ( [ d ¼ ] NEUT * CON2 * NEUT * c h NEUT * A = ts+ )
  ( [ d ¼ ] NEUT * CON2 * NEUT * UC NEUT * A = ts+ ) 
  ( [ d ¼ ] NEUT * CON2 * NEUT * UC NEUT * A = ts+ )

  ;;2

  ( [ p ] NEUT * CON2 * NEUT * s z = p ) ;; dodac do innych voiceless
  ( [ p ] NEUT * CON2 * NEUT * c z = p ) ;; dodac do innych voiceless
  ( [ p ] NEUT * CON2 * NEUT * r z = p ) ;; dodac do innych voiceless
  ( [ p ] NEUT * CON2 * NEUT * d z NEUT * A = b )
  ( [ p ] NEUT * CON2 * NEUT * d z NEUT * r z = b )
  ( [ p ] NEUT * CON2 * NEUT * d ¿ NEUT * A = b )
  ( [ p ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = b )
  ( [ p ] NEUT * CON2 * NEUT * d ¼ NEUT * A = b )
  ( [ p ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = b )
  ( [ p ] NEUT * CON2 * NEUT * VC NEUT * A = b )
 

  ( [ t ] NEUT * CON2 * NEUT * s z = t ) ;; dodac do innych voiceless
  ( [ t ] NEUT * CON2 * NEUT * c z = t ) ;; dodac do innych voiceless
  ( [ t ] NEUT * CON2 * NEUT * r z = t ) ;; dodac do innych voiceless
  ( [ t ] NEUT * CON2 * NEUT * d z NEUT * A = d )
  ( [ t ] NEUT * CON2 * NEUT * d z NEUT * r z = d )
  ( [ t ] NEUT * CON2 * NEUT * d ¿ NEUT * A = d )
  ( [ t ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = d )
  ( [ t ] NEUT * CON2 * NEUT * d ¼ NEUT * A = d )
  ( [ t ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = d )
  ( [ t ] NEUT * CON2 * NEUT * VC + NEUT * A = d )

  ( [ k ] NEUT * CON2 * NEUT * s z = k ) ;; dodac do innych voiceless
  ( [ k ] NEUT * CON2 * NEUT * c z = k ) ;; dodac do innych voiceless
  ( [ k ] NEUT * CON2 * NEUT * r z = k ) ;; dodac do innych voiceless
  ( [ k ] NEUT * CON2 * NEUT * d z NEUT * A = g )
  ( [ k ] NEUT * CON2 * NEUT * d z NEUT * r z = g )
  ( [ k ] NEUT * CON2 * NEUT * d ¿ NEUT * A = g )
  ( [ k ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = g )
  ( [ k ] NEUT * CON2 * NEUT * d ¼ NEUT * A = g )
  ( [ k ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = g )
  ( [ k ] NEUT * CON2 * NEUT * VC NEUT * A = g )

  ( [ f ] NEUT * CON2 * NEUT * s z = f ) ;; dodac do innych voiceless
  ( [ f ] NEUT * CON2 * NEUT * c z = f ) ;; dodac do innych voiceless
  ( [ f ] NEUT * CON2 * NEUT * r z = f ) ;; dodac do innych voiceless
  ( [ f ] NEUT * CON2 * NEUT * d z NEUT * A = v )
  ( [ f ] NEUT * CON2 * NEUT * d z NEUT * r z = v )
  ( [ f ] NEUT * CON2 * NEUT * d ¿ NEUT * A = v )
  ( [ f ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = v )
  ( [ f ] NEUT * CON2 * NEUT * d ¼ NEUT * A = v )
  ( [ f ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = v )
  ( [ f ] NEUT * CON2 * NEUT * VC NEUT * A = v )


  ( [ æ ] NEUT * CON2 * NEUT * s z = ts+ ) ;; dodac do innych voiceless
  ( [ æ ] NEUT * CON2 * NEUT * c z = ts+ ) ;; dodac do innych voiceless
  ( [ æ ] NEUT * CON2 * NEUT * r z = ts+ ) ;; dodac do innych voiceless
  ( [ æ ] NEUT * CON2 * NEUT * d z NEUT * A = dz+ )
  ( [ æ ] NEUT * CON2 * NEUT * d z NEUT * r z = dz+ )
  ( [ æ ] NEUT * CON2 * NEUT * d ¿ NEUT * A = dz+ )
  ( [ æ ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = dz+ )
  ( [ æ ] NEUT * CON2 * NEUT * d ¼ NEUT * A = dz+ )
  ( [ æ ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = dz+ )
  ( [ æ ] NEUT * CON2 * NEUT * VC NEUT * A = dz+ )


  ( [ ¶ ] NEUT * CON2 * NEUT * s z = s+ ) ;; dodac do innych voiceless
  ( [ ¶ ] NEUT * CON2 * NEUT * c z = s+ ) ;; dodac do innych voiceless
  ( [ ¶ ] NEUT * CON2 * NEUT * r z = s+ ) ;; dodac do innych voiceless
  ( [ ¶ ] NEUT * CON2 * NEUT * d z NEUT * A = z+ )
  ( [ ¶ ] NEUT * CON2 * NEUT * d z NEUT * r z = z+ )
  ( [ ¶ ] NEUT * CON2 * NEUT * d ¿ NEUT * A = z+ )
  ( [ ¶ ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = z+ )
  ( [ ¶ ] NEUT * CON2 * NEUT * d ¼ NEUT * A = z+ )
  ( [ ¶ ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = z+ )
  ( [ ¶ ] NEUT * CON2 * NEUT * VC NEUT * A = z+ )


  
  ( [ c h ] NEUT * CON2 * NEUT * r z = x ) 
  ( [ c z ] NEUT * CON2 * NEUT * r z = tS )
  ( [ c z ] NEUT * CON2 * NEUT * d z NEUT * A = dZ )
  ( [ c z ] NEUT * CON2 * NEUT * d z NEUT * r z = dZ )
  ( [ c z ] NEUT * CON2 * NEUT * d ¿ NEUT * A = dZ )
  ( [ c z ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = dZ )
  ( [ c z ] NEUT * CON2 * NEUT * d ¼ NEUT * A = dZ )
  ( [ c z ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = dZ )
  ( [ c z ] NEUT * CON2 * NEUT * VC NEUT * A = dZ )
  ( [ c z ] = tS )
  
  ( [ s z ] NEUT * CON2 * NEUT * r z = S )
  ( [ s z ] NEUT * CON2 * NEUT * d z NEUT * A = Z )
  ( [ s z ] NEUT * CON2 * NEUT * d z NEUT * r z = Z )
  ( [ s z ] NEUT * CON2 * NEUT * d ¿ NEUT * A = Z )
  ( [ s z ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = Z )
  ( [ s z ] NEUT * CON2 * NEUT * d ¼ NEUT * A = Z )
  ( [ s z ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = Z )
  ( [ s z ] NEUT * CON2 * NEUT * VC NEUT * A = Z )
  ( [ s z ] = S )

  ( [ d ] NEUT * CON2 * NEUT * s z NEUT * A = t )
  ( [ d ] NEUT * CON2 * NEUT * s z NEUT * r z = t )
  ( [ d ] NEUT * CON2 * NEUT * c z NEUT * A = t )
  ( [ d ] NEUT * CON2 * NEUT * c z NEUT * r z = t )
  ( [ d ] NEUT * CON2 * NEUT * c h NEUT * A = t )
  ( [ d ] NEUT * CON2 * NEUT * c h NEUT * r z = t )
  ( [ d ] NEUT * CON2 * NEUT * UC NEUT * A = t )
  ( [ d ] NEUT * CON2 * NEUT * UC NEUT * r z = t )

  ( [ z ] NEUT * CON2 * NEUT * s z NEUT * A = s )
  ( [ z ] NEUT * CON2 * NEUT * s z NEUT * r z = s )	
  ( [ z ] NEUT * CON2 * NEUT * c z NEUT * A = s )
  ( [ z ] NEUT * CON2 * NEUT * c z NEUT * r z = s )	
  ( [ z ] NEUT * CON2 * NEUT * c h NEUT * A = s )
  ( [ z ] NEUT * CON2 * NEUT * c h NEUT * r z = s )	
  ( [ z ] NEUT * CON2 * NEUT * UC NEUT * A = s )
  ( [ z ] NEUT * CON2 * NEUT * UC NEUT * r z = s )	

  ( [ s ] NEUT * CON2 * NEUT * r z = s )
  ( [ s ] NEUT * CON2 * NEUT * d z NEUT * A = z )
  ( [ s ] NEUT * CON2 * NEUT * d ¿ NEUT * A = z )
  ( [ s ] NEUT * CON2 * NEUT * d ¼ NEUT * A = z )
  ( [ s ] NEUT * CON2 * NEUT * VC NEUT * A = z )
  ( [ s ] NEUT * CON2 * NEUT * d z NEUT * r z = z )
  ( [ s ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = z )
  ( [ s ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = z )

  ( [ c ] NEUT * CON2 * NEUT * r z = ts )
  ( [ c ] NEUT * CON2 * NEUT * d z NEUT * A = dz )
  ( [ c ] NEUT * CON2 * NEUT * d z NEUT * r z = dz )
  ( [ c ] NEUT * CON2 * NEUT * d ¿ NEUT * A = dz )
  ( [ c ] NEUT * CON2 * NEUT * d ¿ NEUT * r z = dz )
  ( [ c ] NEUT * CON2 * NEUT * d ¼ NEUT * A = dz )
  ( [ c ] NEUT * CON2 * NEUT * d ¼ NEUT * r z = dz )
  ( [ c ] NEUT * CON2 * NEUT * VC NEUT * A = dz )


  ( [ w ] NEUT * CON2 * NEUT * s z NEUT * A = f )
  ( [ w ] NEUT * CON2 * NEUT * s z NEUT * r z = f )
  ( [ w ] NEUT * CON2 * NEUT * c z NEUT * A = f )
  ( [ w ] NEUT * CON2 * NEUT * c z NEUT * r z = f )
  ( [ w ] NEUT * CON2 * NEUT * c h NEUT * A = f )
  ( [ w ] NEUT * CON2 * NEUT * c h NEUT * r z = f )
  ( [ w ] NEUT * CON2 * NEUT * UC NEUT * A = f )
  ( [ w ] NEUT * CON2 * NEUT * UC NEUT * r z = f )

;; final devoicing
	
  ( [ b ] # = p )
  ( [ d ] # = t )
  ( [ g ] # = k )
  ( [ w ] # = f )
  ( [ z ] # = s )	
  ( [ ¿ ] # = S )
  ( [ r z ] # = S )
  ( [ ¼ ] # = s+ )
  ( [ d z ] # = ts )
  ( [ d ¿ ] # = tS )
  ( [ d ¼ ] # = ts+ ) 

;; some numerals

  ( [ p i ê æ ] d z i e = p j e n+ )
  ( [ s z e ¶ æ ] d z i e = S e z+ )
  ( [ d z i e w i ê æ d z i e ] = dz+ e v j e n+ dz+ e)
  ( [ p i ê æ s ] e t = p j e n+ ts )
  ( [ s z e ¶ æ s ] e t = S e j s )

  
  ;; n~ followed by a fricative becomes j

  ;;( [ ñ ] F = j )
  ;;( [ ñ ] c h = j )
  ;;( [ ñ ] r z = j )
  ;;( [ ñ ] s z = j )
  ;;( [ ñ ] F = j )

  ;; stop plus fricative/affricate = affricate
  ;; not sure about that
  ;;( [ t ] F = ts )
  ;;( [ d ] F = dz )

  ;; oral vowel plus nasal before fricative change into nasal vowel

  ( [ e n ] c h = e~ )
  ( [ e n ] s z = e~ )
  
;sprawdzic
  ( [ e n ] r z = e~ )
  ;( [ e n ] F = e~ ) pensjonat 
  
  
  
  ( [ o n ] c h = o~ )
  ( [ o n ] s z = o~ )
  ( [ o n ] r z = o~ )
;  ( [ o n ] F = o~ )

  ;; vowel vowel : usually a glide to separate them, but not always
  ;; general case, different glide depending on the front/back position 
  ;; other problems with vowels: where i V changes into j V

  ( [ a u ] = a w  )
;  ( [ a a ] = a a )
;  ( [ a o ] = a o )
;  ( [ a e ] = a e ) 
;  ( [ a i ] = a j i ) ;;wrong

;  ( [ e a ] = e a )
;  ( [ e o ] = e o )
;  ( [ e i ] = e j i )
 ( [ e u ] = e w )

;  ( [ i ] V1 =  j )

;  ( [ y ] V1 =  j )

;  ( [ o o ] = o o )
;  ( [ o e ] = o e )
;  ( [ o a ] = o a )
;  ( [ o i ] = o j i )
;  ( [ o ] V1 = o w )

;  ( [ u i ] = u j i )
;  ( [ u ] V1 = u w )
;  ( [ ó i ] = u j i )
;  ( [ ó ] V1 = u w )
 

;ks mw

( [ n ] k = N )
( [ n ]	g = N )
( [ n ] x = N )


  ;; assimilation according to place of articulation (introduce classes 
  ;; like palatal dental nasal)


  ;; the groups spelled strz, zdrz are usually pronounced S tS S, Z dZ
  ;; Z, the dental s, z changing into the alveolar S Z unfer the influence
  ;; of the following alveolar sounds

;  ( [ z d r z ] = Z dZ Z )
;  ( [ s t r z ] = S tS S )
;  ( [ t r z ] = tS S )
;  ( [ d r z ] = dZ Z )



  ;; dental followed by  alveolar or palatal changes into alveolar or
  ;; palatal

  ( [ s ] s z = S )
  ( [ s ] ¶ = s+ )
  ( [ s ] c z = S )

  ( [ z ] ¿ = Z )
  ( [ z ] r z = Z )
  ( [ z ] s z = S )
  ( [ z ] ¼ = z+ )
  ( [ z ] z i = z+ )
  ( [ z ] c z = S )
  ( [ z ] æ = s+ )
  ( [ z ] c i = s+ )
  ( [ z ] d ¼ = z+ )
  ( [ z ] d z i = z+ )
  ( [ z ] ¶ = s+ )
  ( [ z ] s i = s+ )
  ( [ z ] c z = S )

  ;; Vowels
  ( [ i ] = i )
  ( [ y ] = I )
  ( [ e ] = e )
  ( [ a ] = a )
  ( [ o ] = o )
  ( [ u ] = u )
  ( [ ó ] = u )
  ( [ ê ] = e~ )
  ( [ ± ] = o~ ) 
	
  ;; Consonants
  ( [ d z ] = dz )
  ( [ c z ] = tS )
  ( [ d ¿ ] = dZ ) 
  ( [ c h ] = x )
  ( [ d ¼ ] = dz+ )
  ( [ s z ] = S )
  ( [ p ] = p )
  ( [ b ] = b )
  ( [ t ] = t )
  ( [ d ] = d )
  ( [ k ] = k )
  ( [ g ] = g )
  ( [ f ] = f )
  ( [ w ] = v )
  ( [ s ] = s )
  
  ( [ z ] = z )
  ( [ ¿ ] = Z )
  ( [ r z ] = Z )
  ( [ ¶ ] = s+ )
  ( [ ¼ ] = z+ )
  ( [ h ] = x )
  ( [ c ] = ts )
  ( [ æ ] = ts+ )
 
  ( [ m ] = m )
  ( [ n ] = n )
  ( [ ñ ] = n+ )
  ( [ l ] = l )
  ( [ ³ ] = w )
  ( [ r ] = r )
  ( [ j ] = j ) 
  ( [ v ] = v )   ;; occurs only in foreign names and words
  ( [ x ] = k s ) ;; has been replaced in Polish by ks
  ( [ q ] = k u ) ;; occurs only in foreign names and words   
  


))


(provide 'cstr_pl_lts)