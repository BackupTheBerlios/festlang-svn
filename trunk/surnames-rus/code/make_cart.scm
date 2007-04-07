(define (carttoC tree)
  "(carttoC NAME TREE ODIR)
Coverts a CART tree to a single C file called cart.c."
  (let 
    ((ofdc (fopen "cart.c" "w"))
     (ofdh (fopen "cart.h" "w")))

    (set! current_node -1)

    (do_carttoC ofdc ofdh tree)

    (fclose ofdc)
    (fclose ofdh)
    ))

(define (do_carttoC ofdc ofdh tree)
  "(do_carttoC ofdc ofdh tree)
Do the tree dump"

    (format ofdc "#include <stdlib.h>\n")
    (format ofdc "#include \"cart_search.h\"\n")
    (format ofdc "#include \"cart.h\"\n\n")
    (format ofdc "const cart_node ru_stress_cart_nodes[] = {\n")
    (carttoC_tree_nodes tree ofdc ofdh)
    (format ofdc "{CART_VALUE, 0, 0, NULL}};\n\n")

    (format ofdc "\n\n")

)

(defvar cart_operators
  '(
    ("sylpos" "CART_QUESTION_SYLPOS_SMALLER")
    ("num2end" "CART_QUESTION_NUM2END_SMALLER")
    ("ph_vfront" "CART_QUESTION_PH_VFRONT")
    ("ph_vheight" "CART_QUESTION_PH_VHEIGHT")
    ("ph_vrnd" "CART_QUESTION_PH_VRND")
    ("p.ph_vc" "CART_QUESTION_P_VC")
    ("p.ph_vheight" "CART_QUESTION_P_VHEIGTH")
    ("p.ph_vfront" "CART_QUESTION_P_VFRONT")
    ("p.ph_vrnd" "CART_QEUSTION_P_VRND")
    ("p.ph_cvox" "CART_QUESTION_P_CVOX")
    ("p.ph_ctype" "CART_QUESTION_P_CTYPE")
    ("p.ph_cplace" "CART_QUESTION_P_CPLACE")
    ("p.ph_csoft" "CART_QUESTION_P_CSOFT")
    ("n.ph_vc" "CART_QUESTION_N_VC")
    ("n.ph_vheight" "CART_QUESTION_N_VHEIGTH")
    ("n.ph_vfront" "CART_QUESTION_N_VFRONT")
    ("n.ph_vrnd" "CART_QEUSTION_N_VRND")
    ("n.ph_cvox" "CART_QUESTION_N_CVOX")
    ("n.ph_ctype" "CART_QUESTION_N_CTYPE")
    ("n.ph_cplace" "CART_QUESTION_N_CPLACE")
    ("n.ph_csoft" "CART_QUESTION_N_CSOFT")
    ("pos" "CART_QUESTION_POS")
    ))

(define (carttoC_tree_nodes tree ofdc ofdh)
  "(carttoC_tree_nodes tree ofdc ofdh)
Dump the nodes in the tree."
  (let ((this_node (set! current_node (+ 1 current_node))))
    (cond
     ((cdr tree) ;; a question node
      (format ofdc "{%s, %s, %s, %s},\n"
	      (cadr (assoc_string             ;; operator
		    (caar tree)
		     cart_operators))
	      (format nil "CTNODE_NO_%04d" this_node); the no node
	      (if (or (string-equal (caar tree) "sylpos") (string-equal (caar tree) "num2end"))
	        (caddr (car tree))
	        0)
	      (if (not (or (string-equal (caar tree) "sylpos") (string-equal (caar tree) "num2end")))
	        (format nil  "\"%s\"" (caddr (car tree)))
	        "NULL")
	       )
      (carttoC_tree_nodes (car (cdr tree)) ofdc ofdh)
      (format ofdh "#define CTNODE_NO_%04d %d\n"
	      this_node (+ 1 current_node))
      (carttoC_tree_nodes (car (cdr (cdr tree))) ofdc ofdh))
     (t  ;; a leaf node
      (format ofdc "{CART_VALUE, 0, %l, NULL},\n" (cadr (caar tree)))))))

(provide 'make_cart)
