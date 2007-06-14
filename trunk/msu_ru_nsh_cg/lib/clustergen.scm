;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;                     Carnegie Mellon University                        ;;
;;;                      Copyright (c) 2005-2006                          ;;
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
;;;  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         ;;
;;;  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ;;
;;;  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO         ;;
;;;  EVENT SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE       ;;
;;;  LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY     ;;
;;;  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,      ;;
;;;  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS       ;;
;;;  ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR              ;;
;;;  PERFORMANCE OF THIS SOFTWARE.                                        ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;  Author: Alan W Black (awb@cs.cmu.edu) Nov 2005                       ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;                                                                       ;;
;;;  Run Time Synthesis support for clustergen (HMM-generation) voices    ;;
;;;                                                                       ;;
;;;  This is voice-independant, and should be in festival/lib but is      ;;
;;;  currently copied into each voice                                     ;;
;;;                                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defvar cluster_synth_pre_hooks nil)
(defvar cluster_synth_post_hooks nil)
(defvar clustergen_mcep_trees nil)
(defvar cg:frame_shift 0.005)

(defSynthType ClusterGen

    (apply_hooks cluster_synth_pre_hooks utt)

    (set! clustergen_utt utt) ;; for debugging

    ;; Build the state relation
    (ClusterGen_make_HMMstate utt)
    ;; Predict number of frames, then predict the frame values
    (ClusterGen_make_mcep utt) ;; durations for # of vectors
    (if (assoc 'cg::trajectory clustergen_mcep_trees)
        (ClusterGen_predict_trajectory utt) ;; predict trajectory
        (ClusterGen_predict_mcep utt) ;; predict vector types
        )

    ;; Convert predicted mcep track into a waveform
    (cluster_synth_method utt)

    (apply_hooks cluster_synth_post_hooks utt)
    utt
)

(define (cg_wave_synth_external utt)
  ;; before we had it built-in to Festival
  (let ((trackname (make_tmp_filename))
        (wavename (make_tmp_filename))
        )
    (track.save (utt.feat utt "param_track") trackname "est")
    (system
     (format nil "$FESTVOXDIR/src/clustergen/cg_resynth %s %s"
             trackname wavename))
    (utt.import.wave utt wavename)
;;    (delete-file trackname)
    (delete-file wavename)
    utt)
)

(define (cg_duration i)
  (if (item.prev i)
      (- (item.feat i "end") (item.feat i "p.end"))
      (item.feat i "end")))

(define (cg_state_pos i)
  (let ((n (item.name i)))
  (cond
   ((not (string-equal n (item.feat i "p.name")))
    "b")
   ((string-equal n (item.feat i "n.name"))
    "m")
   (t
    "e"))))

(define (cg_wave_synth utt)
    (utt.relation.create utt 'Wave)
    (item.set_feat 
     (utt.relation.append utt 'Wave) 
     "wave" 
     (mlsa_resynthesis (utt.feat utt "param_track")))
    utt)

(define (ClusterGen_predict_states seg)
  ;; The names may change
  (cdr (assoc_string (item.name seg) phone_to_states)))

(define (ClusterGen_make_HMMstate utt)
  (let ((states)
        (segstate)
        (statepos))
    ;; Make HMMstate relation and items (three per phone)
    (utt.relation.create utt "HMMstate")
    (utt.relation.create utt "segstate")
    
    (mapcar
     (lambda (seg)
       (set! statepos 1)
       (set! states (ClusterGen_predict_states seg))
       (set! segstate (utt.relation.append utt 'segstate seg))
       (while states
          (set! state (utt.relation.append utt 'HMMstate))
          (item.append_daughter segstate state)
          (item.set_feat state "name" (car states))
          (item.set_feat state "statepos" statepos)
          (set! statepos (+ 1 statepos))
          (set! states (cdr states)))
       )
     (utt.relation.items utt 'Segment))
    )
)

(define (ClusterGen_state_duration state)
  (let ((zdur (wagon_predict state duration_cart_tree_cg))
        (ph_info (assoc_string (item.name state) duration_ph_info_cg))
        (seg_stretch (item.feat state "R:segstate.parent.dur_stretch"))
        (syl_stretch (item.feat state "R:segstate.parent.R:SylStructure.parent.dur_stretch"))
        (tok_stretch (item.feat state "R:segstate.parent.R:SylStructure.parent.parent.R:Token.parent.dur_stretch"))
        (global_stretch (Parameter.get 'Duration_Stretch))
        (stretch 1.0))
    (if (string-matches (item.name state) "#_.*")
        ;; Its a pau so explicitly set the duration
        ;; Note we want sentence internal pauses to be about 100ms
        ;; and sentence final pauses to be 150ms, but there will also
        ;; sentence initial pauses of 150ms so we can treat all pauses as
        ;; 100ms, there are three states so we use 50ms
        (set! zdur 
              (/ (- 0.05 (car (cdr ph_info)))
                 (car (cdr (cdr ph_info))))))
    (if (not (string-equal seg_stretch "0"))
        (setq stretch (* stretch seg_stretch)))
    (if (not (string-equal syl_stretch "0"))
        (setq stretch (* stretch syl_stretch)))
    (if (not (string-equal tok_stretch "0"))
        (setq stretch (* stretch tok_stretch)))
    (if (not (string-equal global_stretch "0"))
        (setq stretch (* stretch global_stretch)))
    (if ph_info
        (* stretch
           (+ (car (cdr ph_info)) ;; mean
              (* (car (cdr (cdr ph_info))) ;; stddev
                 zdur)))
        (begin
          (format t "ClusterGen_state_duration: no dur phone info for %s\n"
                  (item.name state))
          0.1))))

(define (ClusterGen_make_mcep utt)
  (let ((num_frames 0)
        (frame_advance cg:frame_shift)
        (end 0.0))

    ;; Make HMMstate relation and items (three per phone)
    (utt.relation.create utt "mcep")
    (utt.relation.create utt "mcep_link")
    (mapcar
     (lambda (state)
       ;; Predict Duration
       (set! start end)
       (set! end (+ start (ClusterGen_state_duration state)))
       (item.set_feat state "end" end)
       ;; create that number of mcep frames up to state end
       (set! mcep_parent (utt.relation.append utt 'mcep_link state))
       (while (<= (* num_frames frame_advance) end)
              (set! mcep_frame (utt.relation.append utt 'mcep))
              (item.append_daughter mcep_parent mcep_frame)
              (item.set_feat mcep_frame "frame_number" num_frames)
              (item.set_feat mcep_frame "name" (item.name mcep_parent))
              (set! num_frames (+ 1 num_frames))
              )
       )
     (utt.relation.items utt 'HMMstate))

    ;; Copy the final state end back up on to the segment for consistency
    (mapcar
     (lambda (seg)
       (item.set_feat seg "end" (item.feat seg "R:segstate.daughtern.end")))
     (utt.relation.items utt 'Segment))

    (utt.set_feat utt "param_track_num_frames" num_frames)
    utt)
)

(define (cg_F0_smooth track j)
  (let ((p 0.0)
        (i 0)
        (num_frames (- (track.num_frames track) 1)))

    (set! i 1)
    (while (< i num_frames)
      (set! this (track.get track i j))
      (set! next (track.get track (+ i 1) j))
      (if (> this 0.0)
          (track.set 
           track i j
           (/ (+ (if (> p 0.0) p this)
                 this
                 (if (> next 0.0) next this))
              3.0)))
      (set! p this)
      (set! i (+ 1 i)))
    )
)

(define (cg_mcep_smooth track j)
  (let ((p 0.0)
        (i 0)
        (num_frames (- (track.num_frames track) 1)))

    (set! i 1)
    (while (< i num_frames)
      (set! this (track.get track i j))
      (set! next (track.get track (+ i 1) j))
      (track.set 
       track i j
       (/ (+ p this next) 3.0))
      (set! p this)
      (set! i (+ 1 i)))
    )
)

;; For normal synthesis make unvoiced states unvoiced, but we don't
;; do this during testing
(defvar cg_predict_unvoiced t)

(define (ClusterGen_predict_F0 mcep npow_val f0_val param_track)
  "(ClusterGen_predict_F0 mcep npow_val f0_val param_track)
Predict the F0 (or not)."
  (if (and cg_predict_unvoiced
           (string-equal "-"
            (item.feat 
             mcep "R:mcep_link.parent.R:segstate.parent.ph_vc"))
           (or (string-equal "-"
            (item.feat 
             mcep "R:mcep_link.parent.R:segstate.parent.ph_cvox"))
	    (and (string-equal "+"
            (item.feat 
             mcep "R:mcep_link.parent.R:segstate.parent.ph_cvox"))             
           (< npow_val 0.0))))
      (track.set param_track i 0 0.0) ;; make it unvoiced
      (track.set param_track i 0 f0_val)) ;; make it voiced
      (format stderr "%d\n" i)
      (track.set param_track i 0 0.0) ;; make it unvoiced
  )

(define (ClusterGen_predict_mcep utt)
  (let ((param_track nil)
        (frame_advance cg:frame_shift)
        (frame nil) (f nil) (f0_val)
        (num_channels (/ (track.num_channels clustergen_param_vectors) 2))
        )

    ;; Predict mcep values
    (set! i 0)
    (set! param_track
          (track.resize nil
           (utt.feat utt "param_track_num_frames")
           num_channels))
    (utt.set_feat utt "param_track" param_track)
    (mapcar
     (lambda (mcep)
       ;; Predict mcep frame
       (let ((mcep_tree (assoc_string (item.name mcep) clustergen_mcep_trees))
             (f0_tree (assoc_string (item.name mcep) clustergen_f0_trees))
             (npow_tree (assoc_string (item.name mcep) clustergen_npow_trees)))
         (if (null mcep_tree)
             (format t "ClusterGen: can't find cluster tree for %s\n"
                     (item.name mcep))
             (begin
               ;; F0 prediction
               
               (set! f0_val (wagon mcep (cadr f0_tree)))
               (set! npow_val (wagon mcep (cadr npow_tree)))

               (ClusterGen_predict_F0 mcep (cadr npow_val) (cadr f0_val) param_track)

               ;; MCEP prediction
               (set! frame (wagon mcep (cadr mcep_tree)))
               (set! j 1)
               (set! f (car frame))
               (while (< j num_channels)
                  (track.set param_track i j 
                    (track.get clustergen_param_vectors f (* 2 j)))
                  (set! j (+ 1 j)))))
         
         (track.set_time param_track i (* i frame_advance))
         (set! i (+ 1 i))))
     (utt.relation.items utt 'mcep))
    (cg_F0_smooth param_track 0)
    (mapcar
     (lambda (x)
       (cg_mcep_smooth param_track x))
     '( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25))
    utt
  )
)

(define (cg_voiced state)
  "(cg_voiced state)
t if this state is voices, nil otherwise."
  (if (and cg_predict_unvoiced
           (string-equal "-" (item.feat state "R:segstate.parent.ph_vc"))
           (string-equal "-" (item.feat state "R:segstate.parent.ph_cvox")))
      nil
      t))

(define (ClusterGen_predict_trajectory utt)
  (let ((param_track nil)
        (frame_advance cg:frame_shift)
        (frame nil) (f nil) (f0_val)
        (num_channels (track.num_channels clustergen_param_vectors))
;        (num_channels (track.num_channels traj::clustergen_param_vectors))
        )

    ;; Predict mcep values
    (set! i 0)
    (set! param_track
          (track.resize nil
           (utt.feat utt "param_track_num_frames")
           num_channels))
    (utt.set_feat utt "param_track" param_track)
;    (set! param_track (utt.feat utt "param_track"))
    (mapcar
     (lambda (state)
       ;; Predict mcep frame
       (let ((mcep_tree (assoc_string (item.name state) clustergen_mcep_trees))
             (f0_tree (assoc_string (item.name state) clustergen_f0_trees))
             (npow_tree (assoc_string (item.name state) clustergen_npow_trees))
             )
         (set! f0_val (wagon state (cadr f0_tree)))
         (set! npow_val (wagon state (cadr npow_tree)))
;	  (set! npow_val '(0 0))
;	  (set! f0_val '(0 0))
         (if (null mcep_tree)
             (format t "ClusterGen: can't find cluster tree for %s\n"
                     (item.name state))
             (begin
               ;; feature prediction (F0 and mcep)
               (set! trajectory (wagon state (cadr mcep_tree)))
               (if (item.relation.daughters state 'mcep_link)
                   (begin
                    (if (assoc 'cg::trajectory_ola clustergen_mcep_trees)
                     (cg:add_trajectory_ola
                      (caar trajectory)
                      (cadr (car trajectory))
                      state
                      num_channels
                      param_track
                      frame_advance)
                     (cg:add_trajectory
                      (caar trajectory)
                      (cadr (car trajectory))
                      state
                      num_channels
                      param_track
                      frame_advance 
                      (cadr f0_val)
                      (cadr npow_val))))
                     
                      )))

                      ))               
     (utt.relation.items utt 'HMMstate))

;    (track.save param_track "trajectory.track")

     (cg_F0_smooth param_track 0)
     (mapcar
     (lambda (x)
       (cg_mcep_smooth param_track x))
;       '( 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25)
      '( 1 2 3 )
     )
    utt
  )
)

(define (cg:add_trajectory s_start s_frames state num_channels
                           param_track frame_advance f0_val npow_val)
"(cg:add_trajectory start n state num_channels)
Add trajectory to daughters of state, interpolating as necessary."
  (let ((j 0) (i 0)
        (mceps (item.relation.daughters state 'mcep_link)))

    (set! t_start (item.feat (car mceps) "frame_number"))    
    (set! t_frames (length mceps))
    (set! m (/ s_frames t_frames))
    (set! f 0)

    (while (< i t_frames)
       ;; find f
       (set! s_pos (+ s_start f))

  (if (and cg_predict_unvoiced
	   (string-equal "-"
            (item.feat 
             state "R:segstate.parent.ph_vc"))
           (or (string-equal "-"
            (item.feat 
             state "R:segstate.parent.ph_cvox"))
	    (and (string-equal "+"
            (item.feat 
             state "R:segstate.parent.ph_cvox"))
           (< npow_val -7.0))))
      (track.set param_track (+ i t_start) 0 0.0) ;; make it unvoiced
      (track.set param_track (+ i t_start) 0 f0_val)) ;; make it voiced
      
;;       (if (cg_voiced state)
;;       (track.set param_track (+ i t_start) 0
;;                      (track.get clustergen_param_vectors s_pos 0)))

       (set! j 1)
       (while (< j num_channels)
              (track.set param_track (+ i t_start) j 
                         (track.get clustergen_param_vectors 
                                    s_pos j))
              (set! j (+ 1 j)))
       (set! f (+ m f))
       (track.set_time param_track 
                       (+ i t_start) (* (+ i t_start) frame_advance))
       (set! i (+ i 1))
       )
    )
  )

(define (cg:add_trajectory_ola s_start s_frames state num_channels
                           param_track frame_advance)
"(cg:add_trajectory start n state num_channels)
Add trajectory to daughters of state, interpolating as necessary."
  (let ((j 0) (i 0) (s1l 0) (s2l 0) (m 0.0) (w 0.0)
        (t_start 0) (t_frames 0) (s_offset 0)
        (mceps1 nil) (mceps2 nil))

    (set! i 0)
    (while (< i s_frames)
     (if (equal? -1.0 (track.get clustergen_param_vectors (+ s_start i) 0))
         (set! s1l i))
     (set! i (+ i 1)))

    (if (and (item.prev state) 
             (item.relation.daughters (item.prev state) 'mcep_link)
             (> s1l 0))
        (begin ;; do overlap on previous 
          (set! mceps1 (item.relation.daughters (item.prev state) 'mcep_link))
          (set! first_half_delta (/ 1.0 (length mceps1)))
          (set! t_start (item.feat (car mceps1) "frame_number"))
          (set! t_frames (length mceps1))
          (set! m (/ s1l t_frames))
          (set! i 0)
          (set! w 0.0)
          (while (< i t_frames)
           (set! s_offset (nint (* i m)))
           (if (not (< s_offset s1l))
               (begin
;                 (format t "boing pre\n")
                 (set! s_offset (- s1l 1))))
           (set! s_pos (+ s_start s_offset))
           (if (< (track.get clustergen_param_vectors s_pos 0) 0)
               (format t "assigning pre -1/-2 %d %d %f\n" s_pos i m))
           ;; F0 Prediction
           (if (cg_voiced state)
               (track.set param_track (+ i t_start) 0
                (+ (* (- 1.0 w) (track.get param_track (+ i t_start) 0))
                   (* w (track.get clustergen_param_vectors s_pos 0)))))

           ;; MCEP Prediction
           (set! j 1)
           (while (< j num_channels)
             (track.set param_track (+ i t_start) j 
              (+ (* (- 1.0 w) (track.get param_track (+ i t_start) j))
                 (* w 
                    (track.get clustergen_param_vectors s_pos (* 2 j))
                    )
                 )
              )
             (set! j (+ 1 j)))
           (set! i (+ 1 i))
           (set! w (+ w first_half_delta))
           (if (> w 1.0) (set! w 1.0))
           )
          ))

    ;; do assignment on current unit 
    (set! mceps2 (item.relation.daughters state 'mcep_link))
    (set! t_start (item.feat (car mceps2) "frame_number"))
    (set! t_frames (length mceps2))
    (set! s2l (- s_frames (+ s1l 2)))
    (set! s2_start (+ s_start s1l 1))
    (set! m (/ s2l t_frames))
    (set! i 0)
    (while (< i t_frames)
     (set! s_offset (nint (* i m)))
     (if (not (< s_offset s2l))
         (set! s_offset (- s2l 1)))
     (set! s_pos (+ s2_start s_offset))
     (if (< (track.get clustergen_param_vectors s_pos 0) 0)
         (format t "assigning -1/-2 %d %d %f %f\n" s_pos i m
                 (track.get clustergen_param_vectors s_pos 0)))
     ;; F0 Prediction
     (if (cg_voiced state)
         (track.set param_track (+ i t_start) 0
                    (track.get clustergen_param_vectors s_pos 0)))
     ;; MCEP Prediction
     (set! j 1)
     (while (< j num_channels)
      (track.set param_track (+ i t_start) j 
                 (track.get clustergen_param_vectors s_pos (* 2 j)))
      (set! j (+ 1 j)))
     (track.set_time param_track 
                     (+ i t_start) (* (+ i t_start) frame_advance))
     (set! i (+ 1 i))
    )
  )
)

;; Sort of historical it should be set in INST_LANG_VOX_cg.scm
;; but maybe not in old instantiations
(defvar cluster_synth_method 
  (if (boundp 'mlsa_resynthesis)
      cg_wave_synth
      cg_wave_synth_external ))

(provide 'clustergen)
