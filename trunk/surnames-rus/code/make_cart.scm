
(defvar float_scale 40)

(define (carttoC)
  "(carttoC)
Coverts a CART tree to a single C file called cart.c."
  (let 
    ((ofdc (fopen "cart.c" "w"))
     (ofdh (fopen "cart.h" "w"))
     (treefd (fopen "../stresstrain/train.tree" "r")))
    
    (set! current_node -1)

    (do_carttoC ofdc ofdh (readfp treefd))

    (fclose ofdc)
    (fclose ofdh)
    (fclose treefd)
    ))

(define (do_carttoC ofdc ofdh tree)
  "(do_carttoC ofdc ofdh tree)
Do the tree dump"

    (format ofdc "#include <stdlib.h>\n")
    (format ofdc "#include \"cart_search.h\"\n")
    (format ofdc "#include \"cart.h\"\n\n")
    (format ofdc "#include \"phoneset.h\"\n\n")
    (format ofdc "const cart_node ru_stress_cart_nodes[] = {\n")
    (carttoC_tree_nodes tree ofdc ofdh)
    (format ofdc "{CART_VALUE, 0, 0}};\n\n")

    (format ofdc "\n\n")

)

(defvar cart_operators
  '(
    ("sylpos" "CART_QUESTION_SYLPOS_SMALLER")
    ("num2end" "CART_QUESTION_NUM2END_SMALLER")
    ("lasttttt.name" "CART_QUESTION_LASTTTTT_NAME")
    ("lastttt.name" "CART_QUESTION_LASTTTT_NAME")
    ("lasttt.name" "CART_QUESTION_LASTTT_NAME")
    ("lastt.name" "CART_QUESTION_LASTT_NAME")
    ("last.name" "CART_QUESTION_LAST_NAME")
    ("p.name" "CART_QUESTION_P_NAME")
    ("name" "CART_QUESTION_NAME")
    ("n.name" "CART_QUESTION_N_NAME")
    ("nn.name" "CART_QUESTION_NN_NAME")
    ("nnn.name" "CART_QUESTION_NNN_NAME")
    ))

(defvar phone_to_chars
  '(
    ("nil" "PHONE_PAU")
    ("0" "PHONE_PAU")
    ("pau" "PHONE_PAU")
    ("i" "PHONE_I")
    ("y" "PHONE_Y")
    ("e" "PHONE_E")
    ("a" "PHONE_A")
    ("o" "PHONE_O")
    ("u" "PHONE_U")
    ("ao" "PHONE_AO")
    ("ei" "PHONE_EI")
    ("ae" "PHONE_AE")
    ("p" "PHONE_P")
    ("pp" "PHONE_PP")
    ("b" "PHONE_B")
    ("bb" "PHONE_BB")
    ("t" "PHONE_T")
    ("tt" "PHONE_TT")
    ("d" "PHONE_D")
    ("dd" "PHONE_DD")
    ("k" "PHONE_K")
    ("kk" "PHONE_KK")
    ("g"  "PHONE_G")
    ("gg" "PHONE_GG")
    ("c" "PHONE_C")
    ("ch" "PHONE_CH")
    ("f" "PHONE_F")
    ("ff" "PHONE_FF")
    ("v" "PHONE_V")
    ("vv" "PHONE_VV")
    ("s" "PHONE_S")
    ("ss" "PHONE_SS")
    ("z" "PHONE_Z")
    ("zz" "PHONE_ZZ")
    ("sh" "PHONE_SH")
    ("sch" "PHONE_SCH")
    ("zh" "PHONE_ZH")
    ("h" "PHONE_H")
    ("hh" "PHONE_HH")
    ("m" "PHONE_M")
    ("mm" "PHONE_MM")
    ("n" "PHONE_N")
    ("nn" "PHONE_NN")
    ("l" "PHONE_L")
    ("ll" "PHONE_LL")
    ("r" "PHONE_R")
    ("rr" "PHONE_RR")
    ("ij" "PHONE_IJ")
  ))

(define (carttoC_tree_nodes tree ofdc ofdh)
  "(carttoC_tree_nodes tree ofdc ofdh)
Dump the nodes in the tree."
  (let ((this_node (set! current_node (+ 1 current_node))))
    (cond
     ((cdr tree) ;; a question node
      (format ofdc "{%s, %s, %s},\n"
	      (cadr (assoc_string             ;; operator
		    (caar tree)
		     cart_operators))
	      (format nil "CTNODE_NO_%04d" this_node); the no node
	      (if (or (string-equal (caar tree) "sylpos") (string-equal (caar tree) "num2end"))
	            (* (caddr (car tree)) float_scale)
	          (if (string-matches (caar tree) ".*name")

		    	(format nil "%s" (cadr (assoc_string
	    	    		 (caddr (car tree))
	    		    	     phone_to_chars)))
				     		      
    		      (format nil  "\'%s\'" (caddr (car tree))))
		  ))
      (carttoC_tree_nodes (car (cdr tree)) ofdc ofdh)
      (format ofdh "#define CTNODE_NO_%04d %d\n"
	      this_node (+ 1 current_node))
      (carttoC_tree_nodes (car (cdr (cdr tree))) ofdc ofdh))
     (t  ;; a leaf node
      (format ofdc "{CART_VALUE, 0, %d},\n" (* (cadr (caar tree)) float_scale))))))

(provide 'make_cart)
