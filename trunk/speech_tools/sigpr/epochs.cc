/*
 *    "Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. 
 *                   All rights reserved"
 *
 * Written by:  Derek Lin
 * Revised by:  David Talkin
 *
 * The following terms apply to all files associated
 * with the software unless explicitly disclaimed in individual files.
 * 
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 *  they apply.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
 * DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
 * IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
 * NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 */

#include "stdlib.h"
#include "sigpr/EST_filter.h"
#include "sigpr/EST_pitchmark.h"
#include "ling_class/EST_Relation.h"
#include "EST_math.h"
#include "EST_inline_utils.h"
#include "EST_wave_aux.h"
#include "EST_track_aux.h"

/* The algorithm is as follows:

   (1) A variety of signals, including the original PCM data may be used.
   For optimal selection of the exact point of excitation the signal may
   be prepared as follows:
   
    (a) Highpass the signal with a symmetric, non-causal FIR to remove
           DC, breath noise, etc.
       (b) Filter the result with an autoregressive filter of order
           int(2 + Fs/1000) after preemphasizing with 1st order filter (.97).
       (c) If necessary, apply phase correction to restore peakiness of
           "glottal flow derivative" (e.g. for TIMIT).

   (2)  Find all local maxima in the signal.  The number
   of these may be reduced while maintaining accuracy by constraining
   their absolute values to be greater than some running threshold (like
   the local rms). 

   (3) Associate with each peak: its time location, its polarity, its
   absolute value and first derivative.  

   (4) For each peak of a given polarity: 
       (a) Compute a local cost based on the relative local amplitude of
           the peak and the degree to which it fits a glottal flow
	   derivative model. (E.g. amplitude/(local rms) or
	   amplitude/(highest amp. in region); add in (s(t+.2ms) - s(t)).
       (b) For all preceeding peaks of the same polarity in an interval
            near the estimated F0 interval:
	      (i) Compute the transition cost based on the closeness of
	          the interpeak interval (IPI) to the estimated period
		  of the previous best candidate (E.g. abs(log(IPI*F0))
		  and on the similarity of the peak amplitudes
		  (E.g. abs(log(val1/val2))).
	      (ii) Save a backpointer to the previous peak which has the
	           lowest sum of accumulated cost and transition cost.
	      (iii) Assign the cost of the current peak selection to be
	            the sum of its local cost, transition cost and best
		    connection cost.  Save the interval to the best previous
		    peak for subsequent interations.
(5) Starting at the end of the signal (or at the end of each voiced
    interval), examine the peaks in a voiced interval corresponding to the
    F0 range of interest and select the lowest cost peak in that interval
    as the starting point for backtracking to select the most likely set
    of peaks.
*/

#define AMP 10000
#define min(x,y) ((x > y)? y : x)
#define max(x,y) ((x > y)? x : y)
#define BUF_END_TIME(lx)	(lx.end())
#define BUF_START_TIME(lx)	0.0
#define PTIME(p) (BUF_START_TIME(lx) + ((double)(p)->sample)/lx.sample_rate())
#define ITIME(i) (BUF_START_TIME(lx) + ((double)(i))/lx.sample_rate())

/* DP fudge factors: */
float CLIP = 0.5,		/* clipping level for local RMS */
  PEAK = 1.0,			/* weight given to peak quality */
  TSIM = .4,			/* cost of period dissimilarities */
  PSIM = .05,			/* cost for peak quality dissimilarity */
  PQUAL = .35,			/* relative contribution of shape to peak quality */
  FDOUB = .7,			/* cost of each frequency doubling/halving */
  AWARD = .4,			/* award for selecting a peak */
  VOFFCOST = 0.2,		/* cost for V-UV transition */
  VONCOST = 0.2,		/* cost for UV-V transition */
  VONOFF = 0.3,			/* cost for rms rise/fall appropriateness */
  UVCOST = 0.7,			/* cost of unvoiced classification */
  VUCOST, JITTER = .1;		/* reasonable inter-period variation */

char *parfile = NULL;
typedef struct peak
{
  int sample;
  float value;
  float rms;
  struct pcand *cands;
  struct peak *next, *prev;
} Peak;

Peak *neg = NULL, *pos = NULL;
float srate, fratio, range = 0.7, ln2;
short *ppul, *npul;
int imin, imax, debug_level = 0, peaks, tcands;

void
new_peak (Peak ** pp, int loc, float val, short rms)
{
  register Peak *pk;

  if (!(pk = (Peak *) malloc (sizeof (Peak)))) {
    printf ("Can't allocate a Peak.\n");
    exit (-1);
  }
  pk->sample = loc;
  pk->value = val;
  pk->rms = rms;
  pk->prev = *pp;
  pk->next = NULL;
  pk->cands = NULL;
  *pp = pk;
  if (pk->prev)
    pk->prev->next = pk;
  return;
}

typedef struct pcand
{
  Peak *this_peak;		/* parent peak at head of this list */
  struct pcand *best;		/* best previous candidate resulting from DP */
  int inter;			/* interval (in samples) to "best" */
  int type;			/* voiced=1 or unvoiced=0 */
  float cost;			/* cost of choosing "best" */
  struct pcand *next;		/* next candidate for this peak */
} Pcand;

Pcand neg_p = { NULL, NULL, 0, 0, (float) 0.0, NULL }, pos_p = {
NULL, NULL, 0, 0, (float) 0.0, NULL};

/*------------------------------------------------------*/
Pcand *
link_new_cand (Peak * pp, Pcand * pc, int linter, float tcost, int type)
{
  Pcand *p;

  if ((p = (Pcand *) malloc (sizeof (Pcand)))) {
    if (pp) {
      p->next = pp->cands;
      pp->cands = p;
    }
    else
      p->next = NULL;
    p->inter = linter;
    p->best = pc;
    p->cost = tcost + pc->cost;
    p->type = type;
    if (debug_level == 256)
      printf ("%f %f %d %p\n", p->cost, tcost, p->inter, pc->this_peak);
    p->this_peak = pp;
    return (p);
  }
  return (NULL);
}

/*------------------------------------------------------*/
Pcand *
get_best_track (Peak * pp)
{
  register Pcand *pc;

  if (!(pp && (pc = pp->cands)))
    return (NULL);
  else {
    float cmin, cc;
    Pcand *pmin;

    if (!pc->this_peak)
      return (NULL);
    cmin = pc->cost;
    pmin = pc;
    while ((pc = pc->next) != NULL) {
      if (((cc = pc->cost) < cmin)) {
	cmin = cc;
	pmin = pc;
      }
    }
    return (pmin);
  }
}

/*------------------------------------------------------*/
/* Return the lowest cost candidate from all peaks within the maximum plausible
   pitch period. */
Pcand *
get_best_peak (Peak * pp)
{
  register Pcand *pc, *pmin;
  register int low;

  if (pp) {
    low = pp->sample - imin;
    pmin = get_best_track (pp);
    while ((pp = pp->prev) && (pp->sample >= low)) {
      if ((pc = get_best_track (pp)) && pc->this_peak) {
	if (pc->cost < pmin->cost)
	  pmin = pc;
      }
      else
	return (pmin);
    }
    return (pmin);
  }
  return (NULL);
}

void clobber_history ();
void do_dp (Peak * pp, Pcand * pcinit, EST_Wave lx);

/*------------------------------------------------------*/

#define NONE 0
#define POSITIVE 1
#define NEGATIVE 2

/*------------------------------------------------------*/
void
clobber_history ()
{
  Peak *p;
  Pcand *pc, *pcn;

  while (neg) {
    pc = neg->cands;
    while (pc) {
      pcn = pc->next;
      free (pc);
      pc = pcn;
    }
    p = neg->prev;
    free (neg);
    neg = p;
  }
  while (pos) {
    pc = pos->cands;
    while (pc) {
      pcn = pc->next;
      free (pc);
      pc = pcn;
    }
    p = pos->prev;
    free (pos);
    pos = p;
  }
}

/*------------------------------------------------------*/
void
do_dp (Peak * pp, Pcand * pcinit, EST_Wave lx)
{
  register Peak *p, *pstart, *phold, *bestp;
  register Pcand *pc, *pmin, *uvpmin, *bestuv;
  register int low, high, cands = 0;
  register float cmin, tcmin = 0, uvcmin = 0, uvtcmin = 0, tc = 0, cc, linter = 0, ttemp,
    ftemp, ft1, tcost, vcost, vutcost, uvtcost = 0, ucost, maxcost;

  low = pp->sample - imin;
  high = pp->sample - imax;
  p = pp->prev;
  if (debug_level == 1)
    printf ("%f ", PTIME (p));
  while (p && (p->sample > high))
    p = p->prev;
  if (!(pstart = p)) {
    link_new_cand (pp, pcinit, 0, 0.0, 1);
    link_new_cand (pp, pcinit, 0, 0.0, 0);
    if (debug_level == 1)
      printf ("(no pre at T=%f)\n", PTIME (pp));
    return;
  }
  peaks++;
  ln2 = log ((double) 2.0);
  vcost = PEAK * pp->value - AWARD;
  ucost = PEAK * (UVCOST - pp->value) - AWARD;
  pmin = NULL;
  uvpmin = NULL;
  bestuv = NULL;
  bestp = NULL;
  maxcost = uvcmin = 1.0e30;
  phold = p;
  while (p && (p->sample >= low)) {	/* find highest peak as UV cand. */
    if (p->value < maxcost) {
      maxcost = p->value;
      bestp = p;
    }
    p = p->prev;
  }
  /* There are always pleanty of low-quality peaks;  question is:
     Is the BEST previous peak still better classified as UNVOICED? */
  if (bestp && (pc = bestp->cands)) {
    while (pc) {
      if (!pc->type) {		/* get UV-V transition cost */
	bestuv = pc;
	uvtcost = vcost + VONCOST + VONOFF * log (bestp->rms / pp->rms);
	break;
      }
      pc = pc->next;
    }
  }				/* (Now have the unvoiced hypothesis of the HIGHEST peak.) */
  p = phold;
  while (p && (p->sample >= low)) {	/* for each possible candidate */
    linter = pp->sample - p->sample;	/* local time interval */
    tcost = vcost + (PSIM * fabs (log (pp->value / p->value)));
    vutcost = VUCOST + VONOFF * log (pp->rms / p->rms);
    if ((pc = p->cands)) {
      cmin = 1.0e30;
      while (pc) {		/* for each of its candidates */
	if (pc->type) {		/* is it a voiced hypothesis? */
	  if (pc->inter && (pc->best->type)) {	/* prev. per. available? */
	    ttemp = fabs (log (linter / pc->inter));
	    ftemp = (ttemp > (ft1 = FDOUB + fabs (ttemp - ln2))) ?
	      ft1 : ttemp;
	  }
	  else
	    ftemp = JITTER;
	  if ((cc = ((tc = (tcost + (TSIM * ftemp))) + pc->cost)) < cmin) {
	    cmin = cc;
	    tcmin = tc;
	    pmin = pc;
	  }
	  /* Now compute previous voiced to current unvoiced cost. */
	  if ((cc = vutcost + pc->cost) < uvcmin) {
	    uvcmin = cc;
	    uvtcmin = vutcost;
	    uvpmin = pc;
	  }
	}
	else {			/* Check for unvoiced-to-voiced transition as best. */
	  if (pc == bestuv) {	/* is it the LEAST likely unvoiced candidate? */
	    if ((cc = uvtcost + pc->cost) < cmin) {
	      cmin = cc;
	      tcmin = uvtcost;
	      pmin = pc;
	    }
	  }
	}
	pc = pc->next;
      }
    }
    else {
      printf ("Peak with no candidates in dp_dp()!\n");
    }
    if (!link_new_cand (pp, pmin, (int) linter, tcmin, 1)) {	/* voiced cands. */
      printf ("Problems with link_new_cand()\n");
      exit (-1);
    }
    cands++;
    if (debug_level == 1)
      printf ("%f:%f;%fL%d ", PTIME (p), tc, pmin->cost, (int) linter);
    p = p->prev;
  }				/* finished all previous peaks in F0 range */

  /* get the cost of the unvoiced-unvoiced transition */
  if (bestuv && ((cc = ucost + bestuv->cost) < uvcmin)) {
    uvcmin = cc;
    uvtcmin = ucost;
    uvpmin = bestuv;
  }

  if (uvpmin) {			/* record the best connection for the unvoiced hypothesis  */
    if (!link_new_cand (pp, uvpmin, (int) linter, uvtcmin, 0)) {	/* unvoiced cand. */
      printf ("Problems with link_new_cand()\n");
      exit (-1);
    }
    cands++;
  }
  if (!pmin)			/* find a bogus best to maintain continuity */
    if ((pc = get_best_track (pstart))) {
      link_new_cand (pp, pc, pc->inter, 0.0, 1);	/* equal cost V and UV cands. */
      link_new_cand (pp, pc, pc->inter, 0.0, 0);
      cands += 2;
    }
    else
      printf ("No prev. candidates and no track at T=%f)\n", PTIME (pp));
  if (debug_level == 128)
    printf ("%f  %d\n", PTIME (pp), cands);
  tcands += cands;
  return;

}

int eround (float x)
{
    return (int) x;
}

EST_Track
epochs (EST_Wave & lx)
{
  EST_Track pm;
  pm.set_equal_space (false);

  register int i, j, k, npeak = 1, ppeak = 1;
  register short *p, *q, *r, s, t, pm2, pm1, *ppm, thresh, *data;
  register Pcand *pk1, *pk2;
  register double ssq;
  float wsize = .02, amax, maxrms, f0min = 70, f0max = 180, val, ppval, npval;
  short *rms, *scrp;
  int off, npoints, msec;

  if (debug_level)
    printf
      ("PEAK:%f TSIM:%f PSIM:%f PQUAL:%f FDOUB:%f AWARD:%f\n VONCOST:%f VOFFCOST:%f VONOFF:%f UVCOST:%f\n",
       PEAK, TSIM, PSIM, PQUAL, FDOUB, AWARD, VONCOST, VOFFCOST, VONOFF,
       UVCOST);

  srate = lx.sample_rate ();
  imin = eround (srate / f0min);
  imax = eround (srate / f0max);
  tcands = 0;
  peaks = 0;
  VUCOST = PEAK * UVCOST - AWARD + VOFFCOST;
  JITTER = log (1.0 + JITTER);
  if ((rms = (short *) malloc (sizeof (short) * lx.length ())) &&
      (scrp = (short *) malloc (sizeof (short) * lx.length ())) &&
      (ppul = (short *) malloc (sizeof (short) * lx.length ())) &&
      (data = (short *) malloc (sizeof (short) * lx.length ())) &&
      (npul = (short *) malloc (sizeof (short) * lx.length ()))) {

    /* Zero out the new epoch pulse array. */
    for (i = lx.length (), p = ppul, q = npul; i-- > 0;)
      *p++ = *q++ = 0;
    for (i = 0, p = data; i < lx.length (); i++)
      *p++ = lx.a_no_check (i);

    /* Compute a running estimate of the rms using a rectangular window
       of duration wsize seconds. */
    for (k = eround (wsize * lx.sample_rate () * .5), i = 0, ssq = 0.0, q =
	 p = data; i < k; i++, p++)
      ssq += *p * *p;
    for (k *= 2, r = rms, maxrms = 0.0; i < k; i++, p++) {
      *r++ = eround ((amax = sqrt (ssq / i)));
      if (amax > maxrms)
	maxrms = amax;
      ssq += *p * *p;
    }

    for (j = lx.length () - i; j-- > 0; q++, p++) {
      ssq -= *q * *q;
      *r++ = eround ((amax = ((ssq > 0.0) ? sqrt (ssq / i) : 0.0)));
      if (amax > maxrms)
	maxrms = amax;
      ssq += *p * *p;
    }

    for (ssq = (ssq > 0.0) ? sqrt (ssq / i) : 0.0, k /= 2; k-- > 0;)
      *r++ = eround (ssq);

    neg_p.cost = 0.0, pos_p.cost = 0.0;

    off = eround (BUF_START_TIME (lx) * lx.sample_rate ());
    npoints = eround (1 + (BUF_END_TIME (lx) * lx.sample_rate ()));

    msec = eround (.0002 * lx.sample_rate ());
    if (msec == 0) /* ~number of samples in .2ms */
      msec = 1;
    maxrms /= 100.0;

    /* Find all peaks with absolute value greater than the local rms*clip. */
    for (p = data + off, pm2 = *p++,
	 ppm = p + msec, pm1 = *p++, j = off + npoints - 2,
	 i = off + 1, q = rms + off; i < j; i++, ppm++) {
      scrp[i] = 0;
      s = *p++;
      t = *q / 3;
      thresh = eround (CLIP * *q++);
      if (!t)
	t = 1;

      if (ppeak && (pm1 > thresh) && (pm1 > maxrms)) {	/* large pos. peak possible? */
	if ((s < pm1) && (pm1 >= pm2)) {   /* it's a positive peak. */
	  val = (1.0 - PQUAL) * pm1 + PQUAL * (pm1 - *ppm);
	  if (val > 0.0) {
	    val = ((float) t) / val;
	    scrp[i] = eround (100.0 / val);
	    if (debug_level == 2)
	      printf ("+ %f %f \n", ITIME (i), val);
	    new_peak (&pos, i, val, t);
	    do_dp (pos, &pos_p, lx);
	  }
	}
      }
      else {			/* maybe it's a large neg. peak... */
	if (npeak && (-pm1 > thresh) && (s > pm1) && (pm1 <= pm2)) {
	  val = -((1.0 - PQUAL) * pm1 + PQUAL * (pm1 - *ppm));
	  if (val > 0.0) {
	    val = ((float) t) / val;
	    scrp[i] = eround (-100.0 / val);
	    if (debug_level == 2)
	      printf ("- %f %f\n", ITIME (i), val);
	    new_peak (&neg, i, val, t);
	    do_dp (neg, &neg_p, lx);
	  }
	}
      }
      pm2 = pm1;
      pm1 = s;
    }

    /* Now backtrack to find optimum voicing track. */
    pk1 = get_best_peak (pos);
    if (pk1) {
      pos_p.cost = pk1->cost;
      if (debug_level == 64)
	printf ("+%7.4f pos_cum:%f\n", ITIME (i), pos_p.cost);
    }
    pk2 = get_best_peak (neg);
    if (pk2) {
      neg_p.cost = pk2->cost;
      if (debug_level == 64)
	printf ("-%7.4f neg_cum:%f\n", ITIME (i), neg_p.cost);
    }
    while (pk1 && pk1->this_peak) {
      if (pk1->type)
	ppul[pk1->this_peak->sample] = AMP;
      if (debug_level == 1)
	printf ("+t:%f v:%d\n", ITIME (pk1->this_peak->sample),
		ppul[pk1->this_peak->sample]);
      pk1 = pk1->best;
    }
    while (pk2 && pk2->this_peak) {
      if (pk2->type)
	npul[pk2->this_peak->sample] = -AMP;
      if (debug_level == 1)
	printf ("-t:%f v:%d\n", ITIME (pk2->this_peak->sample),
		npul[pk2->this_peak->sample]);
      pk2 = pk2->best;
    }
    clobber_history ();

    if((ppeak && !npeak) || (ppeak && npeak &&
	 ((ppval = pos_p.cost) <
	  (npval = neg_p.cost)))) {
	data = ppul;
    } else {
	data = npul;
    }
    
    data = ppul;

    pm.resize(lx.num_samples(), EST_CURRENT);

    j = 0;
    for(i=0, p = data; i < lx.length(); i++)
	 if(*p++) {
	    pm.t(j++) = ITIME(i);
	 }

    pm.resize(j, EST_CURRENT);

    if (debug_level == 1)
        printf ("total peaks:%d, total cands:%d, cands/peaks:%f\n", peaks, tcands,
	        ((double) tcands) / peaks);
  }


  return pm;
}

EST_Track
epochs (EST_Wave & lx, EST_Features & op)
{
  EST_Track pm;

  return epochs (lx);
}
