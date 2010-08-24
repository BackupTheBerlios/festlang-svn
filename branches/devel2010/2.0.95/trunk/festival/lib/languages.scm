;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                Centre for Speech Technology Research                  ;;
;;;                     University of Edinburgh, UK                       ;;
;;;                       Copyright (c) 1996,2010                         ;;
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
;;;               Author: Sergio Oller
;;;                 Date: January 2010 
;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;
;;;     Language definitions, now it works similar to voice definitions.
;;;


(defvar system-language-path '("/usr/share/festival/languages/"))

(defvar language-path 
  (remove-duplicates
   (append (mapcar (lambda (d) (path-append d "languages/")) load-path)
	   (mapcar (lambda (d) (path-as-directory d)) system-language-path)
	   ))

  "language-path
   List of places to look for languages. If not set it is initialised from
   load-path by appending \"languages/\" to each directory with 
   system-language-path appended.")

(defvar language-locations ()
  "language-locations
   Association list recording where languages were found.")

(defvar language-location-trace nil
  "language-location-trace
   Set t to print language locations as they are found")

(defvar Language_aliases nil
  "Internal variable containing an association of language name
   aliases such as english-> british_english.")

(define (language.names.add language aliases)
"(language.alias.add LANGUAGE ALIASES)
Describe a language to the systen. LANGUAGE should be atomic name, that
conventionally will have language_ prepended to name the basic selection
function. ALIASES is a list of names for that language."
  (let (alias nil)

       (while aliases
        (set! alias (car aliases))
        (cond ( (not (assoc alias Language_aliases) nil)
	         (set! Language_aliases  (cons (cons alias language) Language_aliases))
              )
              (t
                 (set-cdr! (assoc alias Language_aliases) language)
              )
        )
        (set! aliases (cdr aliases))
       )
  )
)

(define (language-location name dir doc)
  "(language-location NAME DIR DOCSTRING)
   Record the location of a language. Called for each language found on language-path.
   Can be called in site-init or .festivalrc for additional languages which
   exist elsewhere."
  (let ((func_name (intern name))
	)

    (set! name (intern name))
    (set! language-locations (cons (cons name dir) language-locations))
    (eval (list 'autoload func_name dir doc))
    (if language-location-trace
	(format t "Language: %s %s.scm\n" name dir)
	)
    )
  )


(define (language.list)
"(language.list)
List of all (potential) languages in the system.  This checks the language-location
list of potential languages found be scanning the language-path at start up time."
   (mapcar car Language_aliases))

(define (language.select name)
"(language.select LANG)
Call function to set up language LANG.  This is normally done by 
prepending language_ to LANG and call it as a function."
       (set! lang (cdr(assoc name Language_aliases)))
       (if (string-equal lang nil)
           (set! lang name)
       )
   (cond 
      ((boundp (intern(string-append "language_" lang))) ;;if function "language_lang" exists, evaluate it
          (eval (list (intern (string-append "language_" lang))))
      )
      ((string-matches lang "klingon")
        (print "Klingon is not supported yet, using English:")
        (language.select 'english)
      )
      (t ;;else, print a message with available languages
        (print "Language not installed. The installed languages are:")
        (language.list)
      )
   )
nil
)


(define (search-for-languages)
  "(search-for-languages)
   Search down language-path to locate languages."

  (let ((dirs language-path)
	(dir nil)
	languages language
	name 
	)
    (while dirs
     (set! dir (car dirs))
       (setq languages (directory-entries dir t))
       (while languages
         (set! language (car languages))
         (if (string-matches language "language_.*scm$")
                (begin
                 (load (path-append dir language))
	         (language-location (path-basename language) (path-append dir (path-basename language)) "language found")
	        )
         ) 
       (set! languages (cdr languages))
      )
     (set! dirs (cdr dirs))
     )
    )
  )

(define (select_language language)
"(select_language LANG)
Chooses language. This function it's old and language.select should be used instead."
  (language.select language)
)

(search-for-languages)

(defvar language_default 'english)

(provide 'languages)
