;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;;                  Language Technologies Institute                    ;;;
;;;                     Carnegie Mellon University                      ;;;
;;;                         Copyright (c) 2000                          ;;;
;;;                        All Rights Reserved.                         ;;;
;;;                                                                     ;;;
;;; Permission is hereby granted, free of charge, to use and distribute ;;;
;;; this software and its documentation without restriction, including  ;;;
;;; without limitation the rights to use, copy, modify, merge, publish, ;;;
;;; distribute, sublicense, and/or sell copies of this work, and to     ;;;
;;; permit persons to whom this work is furnished to do so, subject to  ;;;
;;; the following conditions:                                           ;;;
;;;  1. The code must retain the above copyright notice, this list of   ;;;
;;;     conditions and the following disclaimer.                        ;;;
;;;  2. Any modifications must be clearly marked as such.               ;;;
;;;  3. Original authors' names are not deleted.                        ;;;
;;;  4. The authors' names are not used to endorse or promote products  ;;;
;;;     derived from this software without specific prior written       ;;;
;;;     permission.                                                     ;;;
;;;                                                                     ;;;
;;; CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK        ;;;
;;; DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING     ;;;
;;; ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT  ;;;
;;; SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE     ;;;
;;; FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES   ;;;
;;; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN  ;;;
;;; AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,         ;;;
;;; ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF      ;;;
;;; THIS SOFTWARE.                                                      ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;             Author: Alan W Black (awb@cs.cmu.edu)                   ;;;
;;;               Date: January 2000                                    ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                     ;;;
;;; Generate a C compilable CART trees.                                 ;;;
;;;                                                                     ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (carttoC name trees odir)
  "(carttoC NAME TREES ODIR)
Coverts a CART trees to a single C file called ODIR/NAME_cart.c."
  (let 
    ((ofdc (fopen (path-append odir (string-append name "_cart.c")) "w"))
     (ofdh (fopen (path-append odir (string-append name "_cart.h")) "w"))
     (ofvc (fopen (path-append odir (string-append name "_values.c")) "w")))

    (set! current_node -1)
    (set! val_table nil)
    (set! letter_table nil)

    (set! cart_name name)
    (format ofdc "#include \"cst_lts.h\"\n")
    (format ofdc "#include \"cmu_cart.h\"\n")
    (format ofdc "\n\n")
    (format ofdc "static const cst_cart_node %s_cart_nodes[] = {\n" name)

    (format ofvc "static const char* %s_values[] = {\n" name)

    (mapcar
      (lambda (l)
        (let ((tree (car (cdr l))))
	    (format stderr "Processing %s\n" (car l))
	    (set! letter_table (cons (list (car l) (+ current_node 1)) letter_table))
	    (carttoC_tree_nodes tree ofdc ofdh ofvc)
	    (format ofdc "{ 255, 0, 0},\n")
	    (set! current_node (+ current_node 1))
	    ))
	trees)
    (format ofdc "};\n\n")

    (format stderr "%l\n" letter_table)
    (format ofdc "static const int offsets[] = {\n")
    (mapcar 
       (lambda (l)
	(format ofdc "%s,\n" (cadr l)))
    	(reverse letter_table))
    
    (format ofdc "};\n\n")
    (format ofdh "\n\n")
    (format ofvc "};\n\n")
    
    (fclose ofdc)
    (fclose ofdh)
    (fclose ofvc)
    ))

(define (carttoC_feat_num fname)
  "(carttoC_feat_num fname)
Returns the feature number."
    (cond
     ((string-equal fname "p.p.p.p.name") 0)
     ((string-equal fname "p.p.p.name") 1)
     ((string-equal fname "p.p.name") 2)
     ((string-equal fname "p.name") 3)
     ((string-equal fname "n.name") 4)
     ((string-equal fname "n.n.name") 5)
     ((string-equal fname "n.n.n.name") 6)
     ((string-equal fname "n.n.n.n.name") 7)
     (t (error (format nil "unknown feat %s\n" fname)))))

(define (carttoC_val_table ofdh f operator)
  (let ((fn (assoc_string
	     (if (string-equal operator "is")
		 (format nil "is_%s" f)
		 f)
	     val_table)))
    (cond
     (fn
      (cadr fn))
     (t
      (let ((nname (format nil "%d" (length val_table))))
	(set! val_table
	      (cons (list 
		     (if (string-equal operator "is")
			 (format nil "is_%s" f)
			 f)
		     nname
		     (format ofdh "\"%s\",\n" f))
		    val_table))
	(carttoC_val_table ofdh f operator))))))

(define (carttoC_question_val val)
 "(carttoC_question_val val)
 Convert question to binary"
    (format nil "'%s'" val)
)

(define (carttoC_tree_nodes tree ofdc ofdh ofvc)
  "(carttoC_tree_nodes tree ofdc ofdh ofvc)
Dump the nodes in the tree."
  (let ((this_node (set! current_node (+ 1 current_node))))
    (cond
     ((cdr tree) ;; a question node
      (format ofdc "{ %d, %s, %s},\n"
	      (carttoC_feat_num (caar tree))  ;; the feature
	      (format nil "CTNODE_%s_NO_%05d" cart_name this_node); the no node
	      (carttoC_question_val (nth 2 (car tree))))
      (carttoC_tree_nodes (car (cdr tree)) ofdc ofdh ofvc)
      (format ofdh "#define CTNODE_%s_NO_%05d %d\n" cart_name
	      this_node (+ 1 current_node))
      (carttoC_tree_nodes (car (cdr (cdr tree))) ofdc ofdh ofvc))
     (t  ;; a leaf node
      (format ofdc
	      "{ 255, %s, 0 },\n"
	      (carttoC_extract_answer ofvc tree))))))

(define (carttoC_extract_answer ofvc tree)
  "(carttoC_extract_answer tree)
Get answer from leaf node.  (this can be redefined if you want different
behaviour)."
  (carttoC_val_table ofvc
		     (format nil "%l" (car tree))
		     'none))

(provide 'make_cart)
