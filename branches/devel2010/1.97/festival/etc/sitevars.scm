;;
;;  festival-vars.scm -- Festival Variable Configuration (language: Scheme)
;;

;; This file is loaded from datadir/init.scm after the basic Scheme library
;;functions are loaded but before any of the festival related functions
;;are loaded.  This file is intended to set various path names before
;;various subsystems are loaded.  Typically variables such as `lexdir'
;;(the directory where the lexicons are held), and `voices_dir' (pointing
;;to voice directories) should be reset here if necessary.

(provide 'sitevars)
