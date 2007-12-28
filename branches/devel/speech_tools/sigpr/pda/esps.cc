/*
 * Copyright (C) 2000-2004 Jonas Beskow and Kåre Sjölander 
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

/*
 * This software has been licensed to the Centre of Speech Technology, KTH
 * by Microsoft Corp. with the terms in the accompanying file BSD.txt,
 * which is a BSD style license.
 *
 *    "Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. 
 *                   All rights reserved"
 *
 * Written by:  Derek Lin
 * Checked by:
 * Revised by:  David Talkin
 *
 */

/* A fundamental frequency estimation algorithm using the normalized
   cross correlation function and dynamic programming.  The algorithm
   implemented here is similar to that presented by B. Secrest and
   G. Doddington, "An integrated pitch tracking algorithm for speech
   systems", Proc. ICASSP-83, pp.1352-1355.  It is fully described
   by D. Talkin, "A robust algorithm for ptich tracking (RAPT)", in
   W. B. Kleijn & K. K. Paliwal (eds.) Speech Coding and Synthesis,
   (New York: Elsevier, 1995). */

/* For each frame, up to par->n_cands cross correlation peaks are
   considered as F0 intervals.  Each is scored according to its within-
   frame properties (relative amplitude, relative location), and
   according to its connectivity with each of the candidates in the
   previous frame.  An unvoiced hypothesis is also generated at each
   frame and is considered in the light of voicing state change cost,
   the quality of the cross correlation peak, and frequency continuity. */

/* At each frame, each candidate has associated with it the following
   items:
	its peak value
	its peak value modified by its within-frame properties
	its location
	the candidate # in the previous frame yielding the min. err.
		(this is the optimum path pointer!)
	its cumulative cost: (local cost + connectivity cost +
		cumulative cost of its best-previous-frame-match). */

/* Dynamic programming is then used to pick the best F0 trajectory and voicing
   state given the local and transition costs for the entire utterance. */

/* To avoid the necessity of computing the full crosscorrelation at
   the input sample rate, the signal is downsampled; a full ccf is
   computed at the lower frequency; interpolation is used to estimate the
   location of the peaks at the higher sample rate; and the fine-grained
   ccf is computed only in the vicinity of these estimated peak
   locations. */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <cfloat>

#include "esps.h"

/******************************** Definitions ****************************/
#define TRUE 1
#define FALSE 0

/* Possible values returned by the function f0(). */
#define F0_OK		0
#define F0_NO_RETURNS	1
#define F0_TOO_FEW_SAMPLES	2
#define F0_NO_INPUT	3
#define F0_NO_PAR	4
#define F0_BAD_PAR	5
#define F0_BAD_INPUT	6
#define F0_INTERNAL_ERR	7

/* Bits to specify optional pre-conditioning of speech signals by f0() */
/* These may be OR'ed together to specify all preprocessing. */
#define F0_PC_NONE	0x00		/* no pre-processing */
#define F0_PC_DC	0x01		/* remove DC */
#define F0_PC_LP2000	0x02		/* 2000 Hz lowpass */
#define F0_PC_HP100	0x04		/* 100 Hz highpass */
#define F0_PC_AR	0x08		/* inf_order-order LPC inverse filter */
#define F0_PC_DIFF	0x010		/* 1st-order difference */

/* f0_structs.h */

#define BIGSORD 100

typedef struct cross_rec { /* for storing the crosscorrelation information */
	float	rms;	/* rms energy in the reference window */
	float	maxval;	/* max in the crosscorr. fun. q15 */
	short	maxloc; /* lag # at which max occured	*/
	short	firstlag; /* the first non-zero lag computed */
	float	*correl; /* the normalized corsscor. fun. q15 */
} Cross;

typedef struct dp_rec { /* for storing the DP information */
	short	ncands;	/* # of candidate pitch intervals in the frame */
	short	*locs; /* locations of the candidates */
	float	*pvals; /* peak values of the candidates */
	float	*mpvals; /* modified peak values of the candidates */
	short	*prept; /* pointers to best previous cands. */
	float	*dpvals; /* cumulative error for each candidate */
} Dprec;

typedef struct windstat_rec {  /* for lpc stat measure in a window */
    float rho[BIGSORD+1];
    float err;
    float rms;
} Windstat;

typedef struct sta_rec {  /* for stationarity measure */
  float *stat;
  float *rms;
  float *rms_ratio;
} WStat;


typedef struct frame_rec{
  Cross *cp;
  Dprec *dp;
  float rms;
  struct frame_rec *next;
  struct frame_rec *prev;
} Frame;


/******************************** Code ***********************************/

int window(float *din, float *dout, int n, float preemp, int type);
static void get_cand(Cross *cross,float *peak,int *loc, int nlags,int *ncand, float cand_thresh);
static void peak(float *y, float *xp, float *yp);
static int lc_lin_fir(float fc,int *nf,float *coef);
static int downsamp(float *in, float *out, int samples, int * outsamps, int state_idx, 
		    int decimate, int ncoef, float fc[], int init);
static void do_ffir(float *buf,int in_samps,float *bufo, int *out_samps,int idx, int ncoef,
		    float *fc, int invert, int skip, int init);
Frame *
alloc_frame(int nlags, int ncands);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Return a time-weighting window of type type and length n in dout.
 * Dout is assumed to be at least n elements long.  Type is decoded in
 * the switch statement below.
 */
int xget_window(float *dout, int n, int type)
{
  static float *din = NULL;
  static int n0 = 0;
  float preemp = 0.0;

  if(n > n0) {
    register float *p;
    register int i;
    
    if(din) free((void *)din);
    din = NULL;
    if(!(din = (float*)malloc(sizeof(float)*n))) {
      fprintf(stderr,"Allocation problems in xget_window()\n");
      return(FALSE);
    }
    for(i=0, p=din; i++ < n; ) *p++ = 1;
    n0 = n;
  }
  return(window(din, dout, n, preemp, type));
}
  
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Apply a rectangular window (i.e. none).  Optionally, preemphasize. */
void xrwindow(float *din, float *dout, int n, float preemp)
{
  register float *p;
 
/* If preemphasis is to be performed,  this assumes that there are n+1 valid
   samples in the input buffer (din). */
  if(preemp != 0.0) {
    for( p=din+1; n-- > 0; )
      *dout++ = (float)((*p++) - (preemp * *din++));
  } else {
    for( ; n-- > 0; )
      *dout++ =  *din++;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Generate a cos^4 window, if one does not already exist. */
void xcwindow(float *din, float *dout, int n, float preemp)
{
  register int i;
  register float *p;
  static int wsize = 0;
  static float *wind=NULL;
  register float *q, co;
 
  if(wsize != n) {		/* Need to create a new cos**4 window? */
    register double arg, half=0.5;
    
    if(wind) wind = (float*)realloc((void *)wind,n*sizeof(float));
    else wind = (float*)malloc(n*sizeof(float));
    wsize = n;
    for(i=0, arg=3.1415927*2.0/(wsize), q=wind; i < n; ) {
      co = (float) (half*(1.0 - cos((half + (double)i++) * arg)));
      *q++ = co * co * co * co;
    }
  }
/* If preemphasis is to be performed,  this assumes that there are n+1 valid
   samples in the input buffer (din). */
  if(preemp != 0.0) {
    for(i=n, p=din+1, q=wind; i--; )
      *dout++ = (float) (*q++ * ((float)(*p++) - (preemp * *din++)));
  } else {
    for(i=n, q=wind; i--; )
      *dout++ = *q++ * *din++;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Generate a Hamming window, if one does not already exist. */
void xhwindow(float *din, float *dout, int n, float preemp)
{
  register int i;
  register float *p;
  static int wsize = 0;
  static float *wind=NULL;
  register float *q;

  if(wsize != n) {		/* Need to create a new Hamming window? */
    register double arg, half=0.5;
    
    if(wind) wind = (float*)realloc((void *)wind,n*sizeof(float));
    else wind = (float*)malloc(n*sizeof(float));
    wsize = n;
    for(i=0, arg=3.1415927*2.0/(wsize), q=wind; i < n; )
      *q++ = (float) (.54 - .46 * cos((half + (double)i++) * arg));
  }
/* If preemphasis is to be performed,  this assumes that there are n+1 valid
   samples in the input buffer (din). */
  if(preemp != 0.0) {
    for(i=n, p=din+1, q=wind; i--; )
      *dout++ = (float) (*q++ * ((float)(*p++) - (preemp * *din++)));
  } else {
    for(i=n, q=wind; i--; )
      *dout++ = *q++ * *din++;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Generate a Hanning window, if one does not already exist. */
void xhnwindow(float *din, float *dout, int n, float preemp)
{
  register int i;
  register float *p;
  static int wsize = 0;
  static float *wind=NULL;
  register float *q;

  if(wsize != n) {		/* Need to create a new Hanning window? */
    register double arg, half=0.5;
    
    if(wind) wind = (float*)realloc((void *)wind,n*sizeof(float));
    else wind = (float*)malloc(n*sizeof(float));
    wsize = n;
    for(i=0, arg=3.1415927*2.0/(wsize), q=wind; i < n; )
      *q++ = (float) (half - half * cos((half + (double)i++) * arg));
  }
/* If preemphasis is to be performed,  this assumes that there are n+1 valid
   samples in the input buffer (din). */
  if(preemp != 0.0) {
    for(i=n, p=din+1, q=wind; i--; )
      *dout++ = (float) (*q++ * ((float)(*p++) - (preemp * *din++)));
  } else {
    for(i=n, q=wind; i--; )
      *dout++ = *q++ * *din++;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Apply a window of type type to the short PCM sequence of length n
 * in din.  Return the floating-point result sequence in dout.  If preemp
 * is non-zero, apply preemphasis to tha data as it is windowed.
 */
int window(float *din, float *dout, int n, float preemp, int type)
{
  switch(type) {
  case 0:			/* rectangular */
    xrwindow(din, dout, n, preemp);
    break;
  case 1:			/* Hamming */
    xhwindow(din, dout, n, preemp);
    break;
  case 2:			/* cos^4 */
    xcwindow(din, dout, n, preemp);
    break;
  case 3:			/* Hanning */
    xhnwindow(din, dout, n, preemp);
    break;
  default:
    fprintf(stderr,"Unknown window type (%d) requested in window()\n",type);
    return(FALSE);
  }
  return(TRUE);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Compute the pp+1 autocorrelation lags of the windowsize samples in s.
 * Return the normalized autocorrelation coefficients in r.
 * The rms is returned in e.
 */
void xautoc(int windowsize,float * s, int p, float *r,float * e )
{
  register int i, j;
  register float *q, *t, sum, sum0;

  for( i=windowsize, q=s, sum0=0.0; i--;) {
    sum = *q++;
    sum0 += sum*sum;
  }
  *r = 1.;			/* r[0] will always =1. */
  if(sum0 == 0.0) {		/* No energy: fake low-energy white noise. */
    *e = 1.;			/* Arbitrarily assign 1 to rms. */
    /* Now fake autocorrelation of white noise. */
    for ( i=1; i<=p; i++){
      r[i] = 0.;
    }
    return;
  }
  *e = (float) sqrt((double)(sum0/windowsize));
  sum0 = (float) (1.0/sum0);
  for( i=1; i <= p; i++){
    for( sum=0.0, j=windowsize-i, q=s, t=s+i; j--; )
      sum += (*q++) * (*t++);
    *(++r) = sum*sum0;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Using Durbin's recursion, convert the autocorrelation sequence in r
 * to reflection coefficients in k and predictor coefficients in a.
 * The prediction error energy (gain) is left in *ex.
 * Note: durbin returns the coefficients in normal sign format.
 *	(i.e. a[0] is assumed to be = +1.)
 */
void xdurbin ( float *r, float *k, float *a, int p, float *ex)
{
  float  bb[BIGSORD];
  register int i, j;
  register float e, s, *b = bb;

  e = *r;
  *k = -r[1]/e;
  *a = *k;
  e *= (float) (1. - (*k) * (*k));
  for ( i=1; i < p; i++){
    s = 0;
    for ( j=0; j<i; j++){
      s -= a[j] * r[i-j];
    }
    k[i] = ( s - r[i+1] )/e;
    a[i] = k[i];
    for ( j=0; j<=i; j++){
      b[j] = a[j];
    }
    for ( j=0; j<i; j++){
      a[j] += k[i] * b[i-j-1];
    }
    e *= (float) ( 1. - (k[i] * k[i]) );
  }
  *ex = e;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Compute the autocorrelations of the p LP coefficients in a. 
 *  (a[0] is assumed to be = 1 and not explicitely accessed.)
 *  The magnitude of a is returned in c.
 *  2* the other autocorrelation coefficients are returned in b.
 */
void xa_to_aca (float * a,float * b,float * c, int p )
{
  register float  s, *ap, *a0;
  register int  i, j;

  for ( s=1., ap=a, i = p; i--; ap++ )
    s += *ap * *ap;

  *c = s;
  for ( i = 1; i <= p; i++){
    s = a[i-1];
    for (a0 = a, ap = a+i, j = p-i; j--; )
      s += (*a0++ * *ap++);
    *b++ = (float) (2. * s);
  }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Compute the Itakura LPC distance between the model represented
 * by the signal autocorrelation (r) and its residual (gain) and
 * the model represented by an LPC autocorrelation (c, b).
 * Both models are of order p.
 * r is assumed normalized and r[0]=1 is not explicitely accessed.
 * Values returned by the function are >= 1.
 */
float xitakura ( int p, float *b, float *c, float *r, float *gain )
{
  register float s;

  for( s= *c; p--; )
    s += *r++ * *b++;

  return (s/ *gain);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Compute the time-weighted RMS of a size segment of data.  The data
 * is weighted by a window of type w_type before RMS computation.  w_type
 * is decoded above in window().
 */
float wind_energy(float *data,int size, int w_type)
{
  static int nwind = 0;
  static float *dwind = NULL;
  register float *dp, sum, f;
  register int i;

  if(nwind < size) {
    if(dwind) dwind = (float*)realloc((void *)dwind,size*sizeof(float));
    else dwind = (float*)malloc(size*sizeof(float));
    if(!dwind) {
      fprintf(stderr,"Can't allocate scratch memory in wind_energy()\n");
      return(0.0);
    }
  }
  if(nwind != size) {
    xget_window(dwind, size, w_type);
    nwind = size;
  }
  for(i=size, dp = dwind, sum = 0.0; i-- > 0; ) {
    f = *dp++ * (float)(*data++);
    sum += f*f;
  }
  return((float)sqrt((double)(sum/size)));
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Generic autocorrelation LPC analysis of the short-integer data
 * sequence in data.
 */
int xlpc(int lpc_ord,float lpc_stabl,int wsize,float *data,
         float *lpca, float *ar,float *lpck, float *normerr,float *rms,float preemp, int type)
{
  static float *dwind=NULL;
  static int nwind=0;
  float rho[BIGSORD+1], k[BIGSORD], a[BIGSORD+1],*r,*kp,*ap,en,er,wfact=1.0;

  if((wsize <= 0) || (!data) || (lpc_ord > BIGSORD)) return(FALSE);
  
  if(nwind != wsize) {
    if(dwind) dwind = (float*)realloc((void *)dwind,wsize*sizeof(float));
    else dwind = (float*)malloc(wsize*sizeof(float));
    if(!dwind) {
      fprintf(stderr,"Can't allocate scratch memory in lpc()\n");
      return(FALSE);
    }
    nwind = wsize;
  }
  
  window(data, dwind, wsize, preemp, type);
  if(!(r = ar)) r = rho;	/* Permit optional return of the various */
  if(!(kp = lpck)) kp = k;	/* coefficients and intermediate results. */
  if(!(ap = lpca)) ap = a;
  xautoc( wsize, dwind, lpc_ord, r, &en );
  if(lpc_stabl > 1.0) {	/* add a little to the diagonal for stability */
    int i;
    float ffact;
    ffact = (float) (1.0/(1.0 + exp((-lpc_stabl/20.0) * log(10.0))));
    for(i=1; i <= lpc_ord; i++) rho[i] = ffact * r[i];
    *rho = *r;
    r = rho;
    if(ar)
      for(i=0;i<=lpc_ord; i++) ar[i] = r[i];
  }
  xdurbin ( r, kp, &ap[1], lpc_ord, &er);
  switch(type) {		/* rms correction for window */
  case 0:
    wfact = 1.0;		/* rectangular */
    break;
  case 1:
    wfact = .630397f;		/* Hamming */
    break;
  case 2:
    wfact = .443149f;		/* (.5 - .5*cos)^4 */
    break;
  case 3:
    wfact = .612372f;		/* Hanning */
    break;
  }
  *ap = 1.0;
  if(rms) *rms = en/wfact;
  if(normerr) *normerr = er;
  return(TRUE);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Return a sequence based on the normalized crosscorrelation of the signal
   in data.
 *
  data is the input speech array
  size is the number of samples in each correlation
  start is the first lag to compute (governed by the highest expected F0)
  nlags is the number of cross correlations to compute (set by lowest F0)
  engref is the energy computed at lag=0 (i.e. energy in ref. window)
  maxloc is the lag at which the maximum in the correlation was found
  maxval is the value of the maximum in the CCF over the requested lag interval
  correl is the array of nlags cross-correlation coefficients (-1.0 to 1.0)
 *
 */
void crossf(float *data, int size, int start, int nlags, float *engref, int *maxloc, float *maxval, float *correl)
{
  static float *dbdata=NULL;
  static int dbsize = 0;
  register float *dp, *ds, sum, st;
  register int j;
  register  float *dq, t, *p, engr, *dds, amax;
  register  double engc;
  int i, iloc, total;
  int sizei, sizeo, maxsize;

  /* Compute mean in reference window and subtract this from the
     entire sequence.  This doesn't do too much damage to the data
     sequenced for the purposes of F0 estimation and removes the need for
     more principled (and costly) low-cut filtering. */
  if((total = size+start+nlags) > dbsize) {
    if(dbdata)
      free((void *)dbdata);
    dbdata = NULL;
    dbsize = 0;
    if(!(dbdata = (float*)malloc(sizeof(float)*total))) {
      fprintf(stderr,"Allocation failure in crossf()\n");
      return;
    }
    dbsize = total;
  }
  for(engr=0.0, j=size, p=data; j--; ) engr += *p++;
  engr /= size;
  for(j=size+nlags+start, dq = dbdata, p=data; j--; )  *dq++ = *p++ - engr;

  maxsize = start + nlags;
  sizei = size + start + nlags + 1;
  sizeo = nlags + 1;
 
  /* Compute energy in reference window. */
  for(j=size, dp=dbdata, sum=0.0; j--; ) {
    st = *dp++;
    sum += st * st;
  }

  *engref = engr = sum;
  if(engr > 0.0) {    /* If there is any signal energy to work with... */
    /* Compute energy at the first requested lag. */  
    for(j=size, dp=dbdata+start, sum=0.0; j--; ) {
      st = *dp++;
      sum += st * st;
    }
    engc = sum;

    /* COMPUTE CORRELATIONS AT ALL OTHER REQUESTED LAGS. */
    for(i=0, dq=correl, amax=0.0, iloc = -1; i < nlags; i++) {
      for(j=size, sum=0.0, dp=dbdata, dds = ds = dbdata+i+start; j--; )
	sum += *dp++ * *ds++;
      *dq++ = t = (float) (sum/sqrt((double)(engc*engr))); /* output norm. CC */
      engc -= (double)(*dds * *dds); /* adjust norm. energy for next lag */
      if((engc += (double)(*ds * *ds)) < 1.0)
	engc = 1.0;		/* (hack: in case of roundoff error) */
      if(t > amax) {		/* Find abs. max. as we go. */
	amax = t;
	iloc = i+start;
      }
    }
    *maxloc = iloc;
    *maxval = amax;
  } else {	/* No energy in signal; fake reasonable return vals. */
    *maxloc = 0;
    *maxval = 0.0;
    for(p=correl,i=nlags; i-- > 0; )
      *p++ = 0.0;
  }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Return a sequence based on the normalized crosscorrelation of the
   signal in data.  This is similar to crossf(), but is designed to
   compute only small patches of the correlation sequence.  The length of
   each patch is determined by nlags; the number of patches by nlocs, and
   the locations of the patches is specified by the array locs.  Regions
   of the CCF that are not computed are set to 0. 
 *
  data is the input speech array
  size is the number of samples in each correlation
  start0 is the first (virtual) lag to compute (governed by highest F0)
  nlags0 is the number of lags (virtual+actual) in the correlation sequence
  nlags is the number of cross correlations to compute at each location
  engref is the energy computed at lag=0 (i.e. energy in ref. window)
  maxloc is the lag at which the maximum in the correlation was found
  maxval is the value of the maximum in the CCF over the requested lag interval
  correl is the array of nlags cross-correlation coefficients (-1.0 to 1.0)
  locs is an array of indices pointing to the center of a patches where the
       cross correlation is to be computed.
  nlocs is the number of correlation patches to compute.
 *
 */
void crossfi(float *data, int size, int start0, int nlags0,int nlags,float *engref, int *maxloc, float *maxval, float *correl, int *locs, int nlocs)
{
  static float *dbdata=NULL;
  static int dbsize = 0;
  register float *dp, *ds, sum, st;
  register int j;
  register  float *dq, t, *p, engr, *dds, amax;
  register  double engc;
  int i, iloc, start, total;

  /* Compute mean in reference window and subtract this from the
     entire sequence. */
  if((total = size+start0+nlags0) > dbsize) {
    if(dbdata)
      free((void *)dbdata);
    dbdata = NULL;
    dbsize = 0;
    if(!(dbdata = (float*)malloc(sizeof(float)*total))) {
      fprintf(stderr,"Allocation failure in crossfi()\n");
      return;
    }
    dbsize = total;
  }
  for(engr=0.0, j=size, p=data; j--; ) engr += *p++;
  engr /= size;
/*  for(j=size+nlags0+start0, t = -2.1, amax = 2.1, dq = dbdata, p=data; j--; ) {
    if(((smax = *p++ - engr) > t) && (smax < amax))
      smax = 0.0;
    *dq++ = smax;
  } */
  for(j=size+nlags0+start0, dq = dbdata, p=data; j--; ) {
    *dq++ = *p++ - engr;
  }

  /* Zero the correlation output array to avoid confusing the peak
     picker (since all lags will not be computed). */
  for(p=correl,i=nlags0; i-- > 0; )
    *p++ = 0.0;

  /* compute energy in reference window */
  for(j=size, dp=dbdata, sum=0.0; j--; ) {
    st = *dp++;
    sum += st * st;
  }

  *engref = engr = sum;
   amax=0.0;
  iloc = -1;
  if(engr > 0.0) {
    for( ; nlocs > 0; nlocs--, locs++ ) {
      start = *locs - (nlags>>1);
      if(start < start0)
	start = start0;
      dq = correl + start - start0;
      /* compute energy at first requested lag */  
      for(j=size, dp=dbdata+start, sum=0.0; j--; ) {
	st = *dp++;
	sum += st * st;
      }
      engc = sum;

      /* COMPUTE CORRELATIONS AT ALL REQUESTED LAGS */
      for(i=0; i < nlags; i++) {
	for(j=size, sum=0.0, dp=dbdata, dds = ds = dbdata+i+start; j--; )
	  sum += *dp++ * *ds++;
	if(engc < 1.0)
	  engc = 1.0;		/* in case of roundoff error */
	*dq++ = t = (float) (sum/sqrt((double)(10000.0 + (engc*engr))));
	engc -= (double)(*dds * *dds);
	engc += (double)(*ds * *ds);
	if(t > amax) {
	  amax = t;
	  iloc = i+start;
	}
      }
    }
    *maxloc = iloc;
    *maxval = amax;
  } else {
    *maxloc = 0;
    *maxval = 0.0;
  }
}


static int debug_level = 0;

/*
 * Some consistency checks on parameter values.
 * Return a positive integer if any errors detected, 0 if none.
 */

int
check_f0_params(F0_params *par, double sample_freq)
{
  int	  error = 0;
  double  dstep;

  if((par->cand_thresh < 0.01) || (par->cand_thresh > 0.99)) {
    error++;
  }
  if((par->wind_dur > .1) || (par->wind_dur < .0001)) {
    error++;
  }
  if((par->n_cands > 100) || (par->n_cands < 3)){
    error++;
  }
  if((par->max_f0 <= par->min_f0) || (par->max_f0 >= (sample_freq/2.0)) ||
     (par->min_f0 < (sample_freq/10000.0))){
    error++;
  }
  dstep = ((double)((int)(0.5 + (sample_freq * par->frame_step))))/sample_freq;
  if(dstep != par->frame_step) {
    if(debug_level)
    par->frame_step = (float) dstep;
  }
  if((par->frame_step > 0.1) || (par->frame_step < (1.0/sample_freq))){
    error++;
  }

  return(error);
}

/* ----------------------------------------------------------------------- */
void get_fast_cands(float *fdata, float *fdsdata, int ind, 
		    int step, int size, int dec, int start, 
		    int nlags, float *engref, int *maxloc, float *maxval,
		    Cross* cp, float *peaks, int *locs, int *ncand, F0_params *par)
{
  int decind, decstart, decnlags, decsize, i, j, *lp;
  float *corp, xp, yp, lag_wt;
  register float *pe;

  lag_wt = par->lag_weight/nlags;
  decnlags = 1 + (nlags/dec);
  if((decstart = start/dec) < 1) decstart = 1;
  decind = (ind * step)/dec;
  decsize = 1 + (size/dec);
  corp = cp->correl;
    
  crossf(fdsdata + decind, decsize, decstart, decnlags, engref, maxloc,
	maxval, corp);
  cp->maxloc = *maxloc;	/* location of maximum in correlation */
  cp->maxval = *maxval;	/* max. correlation value (found at maxloc) */
  cp->rms = (float) sqrt(*engref/size); /* rms in reference window */
  cp->firstlag = decstart;

  get_cand(cp,peaks,locs,decnlags,ncand,par->cand_thresh); /* return high peaks in xcorr */

  /* Interpolate to estimate peak locations and values at high sample rate. */
  for(i = *ncand, lp = locs, pe = peaks; i--; pe++, lp++) {
    j = *lp - decstart - 1;
    peak(&corp[j],&xp,&yp);
    *lp = (*lp * dec) + (int)(0.5+(xp*dec)); /* refined lag */
    *pe = yp*(1.0f - (lag_wt* *lp)); /* refined amplitude */
  }
  
  if(*ncand >= par->n_cands) {	/* need to prune candidates? */
    register int *loc, *locm, lt;
    register float smaxval, *pem;
    register int outer, inner, lim;
    for(outer=0, lim = par->n_cands-1; outer < lim; outer++)
      for(inner = *ncand - 1 - outer,
	  pe = peaks + (*ncand) -1, pem = pe-1,
	  loc = locs + (*ncand) - 1, locm = loc-1;
	  inner--;
	  pe--,pem--,loc--,locm--)
	if((smaxval = *pe) > *pem) {
	  *pe = *pem;
	  *pem = smaxval;
	  lt = *loc;
	  *loc = *locm;
	  *locm = lt;
	}
    *ncand = par->n_cands-1;  /* leave room for the unvoiced hypothesis */
  }
  crossfi(fdata + (ind * step), size, start, nlags, 7, engref, maxloc,
	  maxval, corp, locs, *ncand);

  cp->maxloc = *maxloc;	/* location of maximum in correlation */
  cp->maxval = *maxval;	/* max. correlation value (found at maxloc) */
  cp->rms = (float) sqrt(*engref/size); /* rms in reference window */
  cp->firstlag = start;
  get_cand(cp,peaks,locs,nlags,ncand,par->cand_thresh); /* return high peaks in xcorr */
    if(*ncand >= par->n_cands) {	/* need to prune candidates again? */
    register int *loc, *locm, lt;
    register float smaxval, *pe, *pem;
    register int outer, inner, lim;
    for(outer=0, lim = par->n_cands-1; outer < lim; outer++)
      for(inner = *ncand - 1 - outer,
	  pe = peaks + (*ncand) -1, pem = pe-1,
	  loc = locs + (*ncand) - 1, locm = loc-1;
	  inner--;
	  pe--,pem--,loc--,locm--)
	if((smaxval = *pe) > *pem) {
	  *pe = *pem;
	  *pem = smaxval;
	  lt = *loc;
	  *loc = *locm;
	  *locm = lt;
	}
    *ncand = par->n_cands - 1;  /* leave room for the unvoiced hypothesis */
  }
}

/* ----------------------------------------------------------------------- */
float *downsample(float *input,int samsin,int state_idx,double freq,int *samsout,
		 int decimate, int first_time, int last_time)
{
  static float	b[2048];
  static float *foutput = NULL;
  float	beta = 0.0f;
  static int	ncoeff = 127, ncoefft = 0;
  int init;

  if(input && (samsin > 0) && (decimate > 0) && *samsout) {
    if(decimate == 1) {
      return(input);
    }

    if(first_time){
      int nbuff = (samsin/decimate) + (2*ncoeff);

      ncoeff = ((int)(freq * .005)) | 1;
      beta = .5f/decimate;
      foutput = (float*)realloc((void *)foutput, sizeof(float) * nbuff);
      /*      spsassert(foutput, "Can't allocate foutput in downsample");*/
      for( ; nbuff > 0 ;)
	foutput[--nbuff] = 0.0;

      if( !lc_lin_fir(beta,&ncoeff,b)) {
	fprintf(stderr,"\nProblems computing interpolation filter\n");
	free((void *)foutput);
	return(NULL);
      }
      ncoefft = (ncoeff/2) + 1;
    }		    /*  endif new coefficients need to be computed */

    if(first_time) init = 1;
    else if (last_time) init = 2;
    else init = 0;
    
    if(downsamp(input,foutput,samsin,samsout,state_idx,decimate,ncoefft,b,init)) {
      return(foutput);
    } else
      fprintf(stderr,"Problems in downsamp() in downsample()\n");
  } else
    fprintf(stderr,"Bad parameters passed to downsample()\n");
  
  return(NULL);
}

/* ----------------------------------------------------------------------- */
/* Get likely candidates for F0 peaks. */
static void get_cand(Cross *cross,float *peak,int *loc, int nlags,int *ncand, float cand_thresh)
{
  register int i, lastl, *t;
  register float o, p, q, *r, *s, clip;
  int start, ncan, maxl;

  clip = (float) (cand_thresh * cross->maxval);
  maxl = cross->maxloc;
  lastl = nlags - 2;
  start = cross->firstlag;

  r = cross->correl;
  o= *r++;			/* first point */
  q = *r++;	                /* middle point */
  p = *r++;
  s = peak;
  t = loc;
  ncan=0;
  for(i=1; i < lastl; i++, o=q, q=p, p= *r++){
    if((q > clip) &&		/* is this a high enough value? */
      (q >= p) && (q >= o)){ /* NOTE: this finds SHOLDERS and PLATEAUS
				      as well as peaks (is this a good idea?) */
	*s++ = q;		/* record the peak value */
	*t++ = i + start;	/* and its location */
	ncan++;			/* count number of peaks found */
      }
  }
/*
  o = q;
  q = p;
  if( (q > clip) && (q >=0)){
    *s++ = q;
    *t++ = i+start;
    ncan++;
  }
*/
  *ncand = ncan;
}

/* ----------------------------------------------------------------------- */
/* buffer-to-buffer downsample operation */
/* This is STRICTLY a decimator! (no upsample) */
static int downsamp(float *in, float *out, int samples, int * outsamps, int state_idx, 
		    int decimate, int ncoef, float fc[], int init)
{
  if(in && out) {
    do_ffir(in, samples, out, outsamps, state_idx, ncoef, fc, 0, decimate, init);
    return(TRUE);
  } else
    printf("Bad signal(s) passed to downsamp()\n");
  return(FALSE);
}

/*      ----------------------------------------------------------      */
/* fc contains 1/2 the coefficients of a symmetric FIR filter with unity
    passband gain.  This filter is convolved with the signal in buf.
    The output is placed in buf2.  If(invert), the filter magnitude
    response will be inverted.  If(init&1), beginning of signal is in buf;
    if(init&2), end of signal is in buf.  out_samps is set to the number of
    output points placed in bufo. */

static void do_ffir(float *buf,int in_samps,float *bufo, int *out_samps,int idx, int ncoef,
		    float *fc, int invert, int skip, int init)
{
  register float *dp1, *dp2, *dp3, sum, integral;
  static float *co=NULL, *mem=NULL;
  static float state[1000];
  static int fsize=0, resid=0;
  register int i, j, k, l;
  register float *sp;
  register float *buf1;

  buf1 = buf;
  if(ncoef > fsize) {/*allocate memory for full coeff. array and filter memory */    fsize = 0;
    i = (ncoef+1)*2;
    if(!((co = (float *)realloc((void *)co, sizeof(float)*i)) &&
	 (mem = (float *)realloc((void *)mem, sizeof(float)*i)))) {
      fprintf(stderr,"allocation problems in do_fir()\n");
      return;
    }
    fsize = ncoef;
  }

  /* fill 2nd half with data */
  for(i=ncoef, dp1=mem+ncoef-1; i-- > 0; )  *dp1++ = *buf++;  

  if(init & 1) {	/* Is the beginning of the signal in buf? */
    /* Copy the half-filter and its mirror image into the coefficient array. */
    for(i=ncoef-1, dp3=fc+ncoef-1, dp2=co, dp1 = co+((ncoef-1)*2),
	integral = 0.0; i-- > 0; )
      if(!invert) *dp1-- = *dp2++ = *dp3--;
      else {
	integral += (sum = *dp3--);
	*dp1-- = *dp2++ = -sum;
      }
    if(!invert)  *dp1 = *dp3;	/* point of symmetry */
    else {
      integral *= 2;
      integral += *dp3;
      *dp1 = integral - *dp3;
    }

    for(i=ncoef-1, dp1=mem; i-- > 0; ) *dp1++ = 0;
  }
  else
    for(i=ncoef-1, dp1=mem, sp=state; i-- > 0; ) *dp1++ = *sp++;

  i = in_samps;
  resid = 0;

  k = (ncoef << 1) -1;	/* inner-product loop limit */

  if(skip <= 1) {       /* never used */
/*  *out_samps = i;	
    for( ; i-- > 0; ) {	
      for(j=k, dp1=mem, dp2=co, dp3=mem+1, sum = 0.0; j-- > 0;
	  *dp1++ = *dp3++ )
	sum += *dp2++ * *dp1;

      *--dp1 = *buf++;	
      *bufo++ = (sum < 0.0)? sum -0.5 : sum +0.5; 
    }
    if(init & 2) {	
      for(i=ncoef; i-- > 0; ) {
	for(j=k, dp1=mem, dp2=co, dp3=mem+1, sum = 0.0; j-- > 0;
	    *dp1++ = *dp3++ )
	  sum += *dp2++ * *dp1;
	*--dp1 = 0.0;
	*bufo++ = (sum < 0)? sum -0.5 : sum +0.5; 
      }
      *out_samps += ncoef;
    }
    return;
*/
  } 
  else {			/* skip points (e.g. for downsampling) */
    /* the buffer end is padded with (ncoef-1) data points */
    for( l=0 ; l < *out_samps; l++ ) {
      for(j=k-skip, dp1=mem, dp2=co, dp3=mem+skip, sum=0.0; j-- >0;
	  *dp1++ = *dp3++)
	sum += *dp2++ * *dp1;
      for(j=skip; j-- >0; *dp1++ = *buf++) /* new data to memory */
	sum += *dp2++ * *dp1;
      *bufo++ = (sum<0.0) ? sum -0.5f : sum +0.5f;
    }
    if(init & 2){
      resid = in_samps - *out_samps * skip;
      for(l=resid/skip; l-- >0; ){
	for(j=k-skip, dp1=mem, dp2=co, dp3=mem+skip, sum=0.0; j-- >0;
	    *dp1++ = *dp3++)
	    sum += *dp2++ * *dp1;
	for(j=skip; j-- >0; *dp1++ = 0.0)
	  sum += *dp2++ * *dp1;
	*bufo++ = (sum<0.0) ? sum -0.5f : sum +0.5f;
	(*out_samps)++;
      }
    }
    else
      for(dp3=buf1+idx-ncoef+1, l=ncoef-1, sp=state; l-- >0; ) *sp++ = *dp3++;
  }
}

/*      ----------------------------------------------------------      */
static int lc_lin_fir(float fc,int *nf,float *coef)
/* create the coefficients for a symmetric FIR lowpass filter using the
   window technique with a Hanning window. */
{
    register int	i, n;
    register double	twopi, fn, c;

    if(((*nf % 2) != 1))
	*nf = *nf + 1;
    n = (*nf + 1)/2;

    /*  Compute part of the ideal impulse response (the sin(x)/x kernel). */
    twopi = M_PI * 2.0;
    coef[0] = (float) (2.0 * fc);
    c = M_PI;
    fn = twopi * fc;
    for(i=1;i < n; i++) coef[i] = (float)(sin(i * fn)/(c * i));

    /* Now apply a Hanning window to the (infinite) impulse response. */
    /* (Probably should use a better window, like Kaiser...) */
    fn = twopi/(double)(*nf);
    for(i=0;i<n;i++) 
	coef[n-i-1] *= (float)((.5 - (.5 * cos(fn * ((double)i + 0.5)))));
    
    return(TRUE);
}


/* ----------------------------------------------------------------------- */
/* Use parabolic interpolation over the three points defining the peak
 * vicinity to estimate the "true" peak. */
static void peak(float *y, float *xp, float *yp)
{
  register float a, c;
  
  a = (float)((y[2]-y[1])+(.5*(y[0]-y[2])));
  if(fabs(a) > .000001) {
    *xp = c = (float)((y[0]-y[2])/(4.0*a));
    *yp = y[1] - (a*c*c);
  } else {
    *xp = 0.0;
    *yp = y[1];
  }
}

/*
 * READ_SIZE: length of input data frame in sec to read
 * DP_CIRCULAR: determines the initial size of DP circular buffer in sec
 * DP_HIST: stored frame history in second before checking for common path 
 *      DP_CIRCULAR > READ_SIZE, DP_CIRCULAR at least 2 times of DP_HIST 
 * DP_LIMIT: in case no convergence is found, DP frames of DP_LIMIT secs
 *      are kept before output is forced by simply picking the lowest cost
 *      path
 */

#define READ_SIZE 0.2
#define DP_CIRCULAR 1.5
#define DP_HIST 0.5
#define DP_LIMIT 1.0

/* 
 * stationarity parameters -
 * STAT_WSIZE: window size in sec used in measuring frame energy/stationarity
 * STAT_AINT: analysis interval in sec in measuring frame energy/stationarity
 */
#define STAT_WSIZE 0.030
#define STAT_AINT 0.020

/*
 * headF points to current frame in the circular buffer, 
 * tailF points to the frame where tracks start
 * cmpthF points to starting frame of converged path to backtrack
 */

static Frame *headF = NULL, *tailF = NULL, *cmpthF = NULL;

static  int *pcands = NULL;	/* array for backtracking in convergence check */
static int cir_buff_growth_count = 0;

static int size_cir_buffer,	/* # of frames in circular DP buffer */
           size_frame_hist,	/* # of frames required before convergence test */
           size_frame_out,	/* # of frames before forcing output */
           num_active_frames,	/* # of frames from tailF to headF */
           output_buf_size;	/* # of frames allocated to output buffers */

/* 
 * DP parameters
 */
static float tcost, tfact_a, tfact_s, frame_int, vbias, fdouble, wdur, ln2,
             freqwt, lagwt;
static int step, size, nlags, start, stop, ncomp, *locs = NULL;
static short maxpeaks;

static int wReuse = 0;  /* number of windows seen before resued */
static Windstat *windstat = NULL;

static float *f0p = NULL, *vuvp = NULL, *plocs = NULL, *rms_speech = NULL, 
             *acpkp = NULL, *peaks = NULL;
static int first_time = 1, pad;


/*	Round the argument to the nearest integer.			*/
int eround(double flnum)
{
	return((flnum >= 0.0) ? (int)(flnum + 0.5) : (int)(flnum - 0.5));
}

/*--------------------------------------------------------------------*/
int
get_Nframes(long buffsize, int pad, int step)
{
  if (buffsize < pad)
    return (0);
  else
    return ((buffsize - pad)/step);
}


/*--------------------------------------------------------------------*/
int
init_dp_f0(double freq, F0_params *par, long *buffsize, long *sdstep)
{
  int nframes;
  int i;
  int stat_wsize, agap, ind, downpatch;

/*
 * reassigning some constants 
 */

  tcost = par->trans_cost;
  tfact_a = par->trans_amp;
  tfact_s = par->trans_spec;
  vbias = par->voice_bias;
  fdouble = par->double_cost;
  frame_int = par->frame_step;
  
  step = eround(frame_int * freq);
  size = eround(par->wind_dur * freq);
  frame_int = (float)(((float)step)/freq);
  wdur = (float)(((float)size)/freq);
  start = eround(freq / par->max_f0);
  stop = eround(freq / par->min_f0);
  nlags = stop - start + 1;
  ncomp = size + stop + 1; /* # of samples required by xcorr
			      comp. per fr. */
  maxpeaks = 2 + (nlags/2);	/* maximum number of "peaks" findable in ccf */
  ln2 = (float)log(2.0);
  size_frame_hist = (int) (DP_HIST / frame_int);
  size_frame_out = (int) (DP_LIMIT / frame_int);

/*
 * SET UP THE D.P. WEIGHTING FACTORS:
 *      The intent is to make the effectiveness of the various fudge factors
 *      independent of frame rate or sampling frequency.                
 */
  
  /* Lag-dependent weighting factor to emphasize early peaks (higher freqs)*/
  lagwt = par->lag_weight/stop;
  
  /* Penalty for a frequency skip in F0 per frame */
  freqwt = par->freq_weight/frame_int;
  
  i = (int) (READ_SIZE *freq);
  if(ncomp >= step) nframes = ((i-ncomp)/step ) + 1;
  else nframes = i / step;

  /* *buffsize is the number of samples needed to make F0 computation
     of nframes DP frames possible.  The last DP frame is patched with
     enough points so that F0 computation on it can be carried.  F0
     computaion on each frame needs enough points to do

     1) xcross or cross correlation measure:
           enough points to do xcross - ncomp

     2) stationarity measure:
           enough to make 30 msec windowing possible - ind

     3) downsampling:
           enough to make filtering possible -- downpatch
 
     So there are nframes whole DP frames, padded with pad points
     to make the last frame F0 computation ok.

  */

  /* last point in data frame needs points of 1/2 downsampler filter length 
     long, 0.005 is the filter length used in downsampler */
  downpatch = (((int) (freq * 0.005))+1) / 2;

  stat_wsize = (int) (STAT_WSIZE * freq);
  agap = (int) (STAT_AINT * freq);
  ind = ( agap - stat_wsize ) / 2;
  i = stat_wsize + ind;
  pad = downpatch + ((i>ncomp) ? i:ncomp);
  *buffsize = nframes * step + pad;
  *sdstep = nframes * step;
  
  /* Allocate space for the DP storage circularly linked data structure */

  size_cir_buffer = (int) (DP_CIRCULAR / frame_int);

  /* creating circularly linked data structures */
  tailF = alloc_frame(nlags, par->n_cands);
  headF = tailF;

  /* link them up */
  for(i=1; i<size_cir_buffer; i++){
    headF->next = alloc_frame(nlags, par->n_cands);
    headF->next->prev = headF;
    headF = headF->next;
  }
  headF->next = tailF;
  tailF->prev = headF;

  headF = tailF;

  /* Allocate sscratch array to use during backtrack convergence test. */
  if( ! pcands ) {
    pcands = (int *) malloc( par->n_cands * sizeof(int));
    /*    spsassert(pcands,"can't allocate pathcands");*/
  }

  /* Allocate arrays to return F0 and related signals. */

  /* Note: remember to compare *vecsize with size_frame_out, because
     size_cir_buffer is not constant */
  output_buf_size = size_cir_buffer;
  rms_speech = (float*)malloc(sizeof(float) * output_buf_size);
  /*  spsassert(rms_speech,"rms_speech malloc failed");*/
  f0p = (float*)malloc(sizeof(float) * output_buf_size);
  /*  spsassert(f0p,"f0p malloc failed");*/
  vuvp = (float*)malloc(sizeof(float)* output_buf_size);
  /*  spsassert(vuvp,"vuvp malloc failed");*/
  acpkp = (float*)malloc(sizeof(float) * output_buf_size);
  /*  spsassert(acpkp,"acpkp malloc failed");*/
  plocs = (float*)malloc(sizeof(float) * output_buf_size);
  /*  spsassert(acpkp,"acpkp malloc failed");*/

  /* Allocate space for peak location and amplitude scratch arrays. */
  peaks = (float*)malloc(sizeof(float) * maxpeaks);
  /*  spsassert(peaks,"peaks malloc failed");*/
  locs = (int*)malloc(sizeof(int) * maxpeaks);
  /*  spsassert(locs, "locs malloc failed");*/
  
  /* Initialise the retrieval/saving scheme of window statistic measures */
  wReuse = agap / step;
  if (wReuse){
      windstat = (Windstat *) malloc( wReuse * sizeof(Windstat));
      /*      spsassert(windstat, "windstat malloc failed");*/
      for(i=0; i<wReuse; i++){
	  windstat[i].err = 0;
	  windstat[i].rms = 0;
      }
  }

  if(debug_level){
    fprintf(stderr, "done with initialization:\n");
    fprintf(stderr,
	    " size_cir_buffer:%d  xcorr frame size:%d start lag:%d nlags:%d\n",
	    size_cir_buffer, size, start, nlags);
  }

  num_active_frames = 0;
  first_time = 1;

  return(0);
}

static WStat*
get_stationarity(float *fdata, double freq, int buff_size, int nframes, int frame_step, int first_time);

/*--------------------------------------------------------------------*/
int
dp_f0(float *fdata, int buff_size, int sdstep, double freq,
      F0_params *par, float **f0p_pt, float **vuvp_pt, float **plocs_pt,
      float **rms_speech_pt, float **acpkp_pt, int *vecsize, int last_time)
{
  float  maxval, engref, *sta, *rms_ratio, *dsdata;
  register float ttemp, ftemp, ft1, ferr, err, errmin;
  register int  i, j, k, loc1, loc2;
  int   nframes, maxloc, ncand, ncandp, minloc,
        decimate, samsds;

  WStat *wstat = NULL;

  nframes = get_Nframes((long) buff_size, pad, step); /* # of whole frames */

  if(debug_level)
    fprintf(stderr,
	    "******* Computing %d dp frames ******** from %d points\n", nframes, buff_size);

  /* Now downsample the signal for coarse peak estimates. */

  decimate = (int)(freq/2000.0);    /* downsample to about 2kHz */
  if (decimate <= 1)
    dsdata = fdata;
  else {
    samsds = ((nframes-1) * step + ncomp) / decimate;
    dsdata = downsample(fdata, buff_size, sdstep, freq, &samsds, decimate, 
			first_time, last_time);
    if (!dsdata) {
      fprintf(stderr, "can't get downsampled data.\n");
      return 1;
    }
  }

  /* Get a function of the "stationarity" of the speech signal. */

  wstat = get_stationarity(fdata, freq, buff_size, nframes, step, first_time);
  if (!wstat) { 
    fprintf(stderr, "can't get stationarity\n");
    return(1);
  }
  sta = wstat->stat;
  rms_ratio = wstat->rms_ratio;

  /***********************************************************************/
  /* MAIN FUNDAMENTAL FREQUENCY ESTIMATION LOOP */
  /***********************************************************************/
  if(!first_time && nframes > 0) headF = headF->next;

  for(i = 0; i < nframes; i++) {
 
    /* NOTE: This buffer growth provision is probably not necessary.
       It was put in (with errors) by Derek Lin and apparently never
       tested.  My tests and analysis suggest it is completely
       superfluous. DT 9/5/96 */
    /* Dynamically allocating more space for the circular buffer */
    if(headF == tailF->prev){
      Frame *frm;

      if(cir_buff_growth_count > 5){
	fprintf(stderr,
		"too many requests (%d) for dynamically allocating space.\n   There may be a problem in finding converged path.\n",cir_buff_growth_count);
	return(1);
      }
      if(debug_level) 
	fprintf(stderr, "allocating %d more frames for DP circ. buffer.\n", size_cir_buffer);
      frm = alloc_frame(nlags, par->n_cands);
      headF->next = frm;
      frm->prev = headF;
      for(k=1; k<size_cir_buffer; k++){
	frm->next = alloc_frame(nlags, par->n_cands);
	frm->next->prev = frm;
	frm = frm->next;
      }
      frm->next = tailF;
      tailF->prev = frm;
      cir_buff_growth_count++;
    }

    headF->rms = wstat->rms[i];
    get_fast_cands(fdata, dsdata, i, step, size, decimate, start,
		   nlags, &engref, &maxloc,
		   &maxval, headF->cp, peaks, locs, &ncand, par);
    
    /*    Move the peak value and location arrays into the dp structure */
    {
      register float *ftp1, *ftp2;
      register short *sp1;
      register int *sp2;
      
      for(ftp1 = headF->dp->pvals, ftp2 = peaks,
	  sp1 = headF->dp->locs, sp2 = locs, j=ncand; j--; ) {
	*ftp1++ = *ftp2++;
	*sp1++ = *sp2++;
      }
      *sp1 = -1;		/* distinguish the UNVOICED candidate */
      *ftp1 = maxval;
      headF->dp->mpvals[ncand] = vbias+maxval; /* (high cost if cor. is high)*/
    }

    /* Apply a lag-dependent weight to the peaks to encourage the selection
       of the first major peak.  Translate the modified peak values into
       costs (high peak ==> low cost). */
    for(j=0; j < ncand; j++){
      ftemp = 1.0f - ((float)locs[j] * lagwt);
      headF->dp->mpvals[j] = 1.0f - (peaks[j] * ftemp);
    }
    ncand++;			/* include the unvoiced candidate */
    headF->dp->ncands = ncand;

    /*********************************************************************/
    /*    COMPUTE THE DISTANCE MEASURES AND ACCUMULATE THE COSTS.       */
    /*********************************************************************/

    ncandp = headF->prev->dp->ncands;
    for(k=0; k<ncand; k++){	/* for each of the current candidates... */
      minloc = 0;
      errmin = FLT_MAX;
      if((loc2 = headF->dp->locs[k]) > 0) { /* current cand. is voiced */
	for(j=0; j<ncandp; j++){ /* for each PREVIOUS candidate... */
	  /*    Get cost due to inter-frame period change. */
	  loc1 = headF->prev->dp->locs[j];
	  if (loc1 > 0) { /* prev. was voiced */
	    ftemp = (float) log(((double) loc2) / loc1);
	    ttemp = (float) fabs(ftemp);
	    ft1 = (float) (fdouble + fabs(ftemp + ln2));
	    if (ttemp > ft1)
	      ttemp = ft1;
	    ft1 = (float) (fdouble + fabs(ftemp - ln2));
	    if (ttemp > ft1)
	      ttemp = ft1;
	    ferr = ttemp * freqwt;
	  } else {		/* prev. was unvoiced */
	    ferr = tcost + (tfact_s * sta[i]) + (tfact_a / rms_ratio[i]);
	  }
	  /*    Add in cumulative cost associated with previous peak. */
	  err = ferr + headF->prev->dp->dpvals[j];
	  if(err < errmin){	/* find min. cost */
	    errmin = err;
	    minloc = j;
	  }
	}
      } else {			/* this is the unvoiced candidate */
	for(j=0; j<ncandp; j++){ /* for each PREVIOUS candidate... */
	  
	  /*    Get voicing transition cost. */
	  if (headF->prev->dp->locs[j] > 0) { /* previous was voiced */
	    ferr = tcost + (tfact_s * sta[i]) + (tfact_a * rms_ratio[i]);
	  }
	  else
	    ferr = 0.0;
	  /*    Add in cumulative cost associated with previous peak. */
	  err = ferr + headF->prev->dp->dpvals[j];
	  if(err < errmin){	/* find min. cost */
	    errmin = err;
	    minloc = j;
	  }
	}
      }
      /* Now have found the best path from this cand. to prev. frame */
      if (first_time && i==0) {		/* this is the first frame */
	headF->dp->dpvals[k] = headF->dp->mpvals[k];
	headF->dp->prept[k] = 0;
      } else {
	headF->dp->dpvals[k] = errmin + headF->dp->mpvals[k];
	headF->dp->prept[k] = minloc;
      }
    } /*    END OF THIS DP FRAME */

    if (i < nframes - 1)
      headF = headF->next;
    
    if (debug_level >= 2) {
      fprintf(stderr,"%d engref:%10.0f max:%7.5f loc:%4d\n",
	      i,engref,maxval,maxloc);
    }
    
  } /* end for (i ...) */

  /***************************************************************/
  /* DONE WITH FILLING DP STRUCTURES FOR THE SET OF SAMPLED DATA */
  /*    NOW FIND A CONVERGED DP PATH                             */
  /***************************************************************/

  *vecsize = 0;			/* # of output frames returned */

  num_active_frames += nframes;

  if( num_active_frames >= size_frame_hist  || last_time ){
    Frame *frm;
    int  num_paths, best_cand, frmcnt, checkpath_done = 1;
    float patherrmin;
      
    if(debug_level)
      fprintf(stderr, "available frames for backtracking: %d\n",
num_active_frames);
      
    patherrmin = FLT_MAX;
    best_cand = 0;
    num_paths = headF->dp->ncands;

    /* Get the best candidate for the final frame and initialize the
       paths' backpointers. */
    frm = headF;
    for(k=0; k < num_paths; k++) {
      if (patherrmin > headF->dp->dpvals[k]){
	patherrmin = headF->dp->dpvals[k];
	best_cand = k;	/* index indicating the best candidate at a path */
      }
      pcands[k] = frm->dp->prept[k];
    }

    if(last_time){     /* Input data was exhausted. force final outputs. */
      cmpthF = headF;		/* Use the current frame as starting point. */
    } else {
      /* Starting from the most recent frame, trace back each candidate's
	 best path until reaching a common candidate at some past frame. */
      frmcnt = 0;
      while (1) {
	frm = frm->prev;
	frmcnt++;
	checkpath_done = 1;
	for(k=1; k < num_paths; k++){ /* Check for convergence. */
	  if(pcands[0] != pcands[k])
	    checkpath_done = 0;
	}
	if( ! checkpath_done) { /* Prepare for checking at prev. frame. */
	  for(k=0; k < num_paths; k++){
	    pcands[k] = frm->dp->prept[pcands[k]];
	  }
	} else {	/* All paths have converged. */
	  cmpthF = frm;
	  best_cand = pcands[0];
	  if(debug_level)
	    fprintf(stderr,
		    "paths went back %d frames before converging\n",frmcnt);
	  break;
	}
	if(frm == tailF){	/* Used all available data? */
	  if( num_active_frames < size_frame_out) { /* Delay some more? */
	    checkpath_done = 0; /* Yes, don't backtrack at this time. */
	    cmpthF = NULL;
	  } else {		/* No more delay! Force best-guess output. */
	    checkpath_done = 1;
	    cmpthF = headF;
	    /*	    fprintf(stderr,
		    "WARNING: no converging path found after going back %d frames, will use the lowest cost path\n",num_active_frames);*/
	  }
	  break;
	} /* end if (frm ...) */
      }	/* end while (1) */
    } /* end if (last_time) ... else */

    /*************************************************************/
    /* BACKTRACKING FROM cmpthF (best_cand) ALL THE WAY TO tailF    */
    /*************************************************************/
    i = 0;
    frm = cmpthF;	/* Start where convergence was found (or faked). */
    while( frm != tailF->prev && checkpath_done){
      if( i == output_buf_size ){ /* Need more room for outputs? */
	output_buf_size *= 2;
	if(debug_level)
	  fprintf(stderr,
		  "reallocating space for output frames: %d\n",
		  output_buf_size);
	rms_speech = (float *) realloc((void *) rms_speech,
				       sizeof(float) * output_buf_size);
	/*	spsassert(rms_speech, "rms_speech realloc failed in dp_f0()");*/
	f0p = (float *) realloc((void *) f0p,
				sizeof(float) * output_buf_size);
	/*	spsassert(f0p, "f0p realloc failed in dp_f0()");*/
	vuvp = (float *) realloc((void *) vuvp, sizeof(float) * output_buf_size);
	/*	spsassert(vuvp, "vuvp realloc failed in dp_f0()");*/
	acpkp = (float *) realloc((void *) acpkp, sizeof(float) * output_buf_size);
	/*	spsassert(acpkp, "acpkp realloc failed in dp_f0()");*/
	plocs = (float *) realloc((void *) plocs, sizeof(float) * output_buf_size);
	/*	spsassert(acpkp, "acpkp realloc failed in dp_f0()");*/
      }
      rms_speech[i] = frm->rms;
      acpkp[i] =  frm->dp->pvals[best_cand];
      loc1 = frm->dp->locs[best_cand];
      vuvp[i] = 1.0;
      best_cand = frm->dp->prept[best_cand];
      ftemp = (float) loc1;
      if(loc1 > 0) {		/* Was f0 actually estimated for this frame? */
	if (loc1 > start && loc1 < stop) { /* loc1 must be a local maximum. */
	  float cormax, cprev, cnext, den;
		  
	  j = loc1 - start;
	  cormax = frm->cp->correl[j];
	  cprev = frm->cp->correl[j+1];
	  cnext = frm->cp->correl[j-1];
	  den = (float) (2.0 * ( cprev + cnext - (2.0 * cormax) ));
	  /*
	   * Only parabolic interpolate if cormax is indeed a local 
	   * turning point. Find peak of curve that goes though the 3 points
	   */
		  
	  if (fabs(den) > 0.000001)
	    ftemp += 2.0f - ((((5.0f*cprev)+(3.0f*cnext)-(8.0f*cormax))/den));
	
	  plocs[i] = ((float)((start - i) * 160 + ftemp)) / freq;
	}
	f0p[i] = (float) (freq/ftemp);
      } else {		/* No valid estimate; just fake some arbitrary F0. */
	f0p[i] = 0;
	vuvp[i] = 0.0;
	plocs[i] = 0.0;
      }
      frm = frm->prev;
	  
      if (debug_level >= 2)
	fprintf(stderr," i:%4d%8.1f%8.1f\n",i,f0p[i],vuvp[i]);
      /* f0p[i] starts from the most recent one */ 
      /* Need to reverse the order in the calling function */
      i++;
    } /* end while() */
    if (checkpath_done){
      *vecsize = i;
      tailF = cmpthF->next;
      num_active_frames -= *vecsize;
    }
  } /* end if() */

  if (debug_level)
    fprintf(stderr, "writing out %d frames.\n", *vecsize);
  
  *f0p_pt = f0p;
  *vuvp_pt = vuvp;
  *acpkp_pt = acpkp;
  *plocs_pt = plocs;
  *rms_speech_pt = rms_speech;
  /*  *acpkp_pt = acpkp;*/
  
  if(first_time) first_time = 0;
  return(0);
}


/*--------------------------------------------------------------------*/
Frame *
alloc_frame(int nlags, int ncands)
{
  Frame *frm;
  int j;

  frm = (Frame*)malloc(sizeof(Frame));
  frm->dp = (Dprec *) malloc(sizeof(Dprec));
  /*  spsassert(frm->dp,"frm->dp malloc failed in alloc_frame");*/
  frm->dp->ncands = 0;
  frm->cp = (Cross *) malloc(sizeof(Cross));
  /*  spsassert(frm->cp,"frm->cp malloc failed in alloc_frame");*/
  frm->cp->correl = (float *) malloc(sizeof(float) * nlags);
  /*  spsassert(frm->cp->correl, "frm->cp->correl malloc failed");*/
  /* Allocate space for candidates and working arrays. */
  frm->dp->locs = (short*)malloc(sizeof(short) * ncands);
  /*  spsassert(frm->dp->locs,"frm->dp->locs malloc failed in alloc_frame()");*/
  frm->dp->pvals = (float*)malloc(sizeof(float) * ncands);
/*  spsassert(frm->dp->pvals,"frm->dp->pvals malloc failed in alloc_frame()");*/
  frm->dp->mpvals = (float*)malloc(sizeof(float) * ncands);
  /*  spsassert(frm->dp->mpvals,"frm->dp->mpvals malloc failed in alloc_frame()");*/
  frm->dp->prept = (short*)malloc(sizeof(short) * ncands);
  /*  spsassert(frm->dp->prept,"frm->dp->prept malloc failed in alloc_frame()");*/
  frm->dp->dpvals = (float*)malloc(sizeof(float) * ncands);
  /*  spsassert(frm->dp->dpvals,"frm->dp->dpvals malloc failed in alloc_frame()");*/
    
  /*  Initialize the cumulative DP costs to zero */
  for(j = ncands-1; j >= 0; j--)
    frm->dp->dpvals[j] = 0.0;

  return(frm);
}


/*--------------------------------------------------------------------*/
/* push window stat to stack, and pop the oldest one */

static int
save_windstat(float *rho, int order, float err, float rms)
{
    int i,j;

    if(wReuse > 1){               /* push down the stack */
	for(j=1; j<wReuse; j++){
	    for(i=0;i<=order; i++) windstat[j-1].rho[i] = windstat[j].rho[i];
	    windstat[j-1].err = windstat[j].err;
	    windstat[j-1].rms = windstat[j].rms;
	}
	for(i=0;i<=order; i++) windstat[wReuse-1].rho[i] = rho[i]; /*save*/
	windstat[wReuse-1].err = (float) err;
	windstat[wReuse-1].rms = (float) rms;
	return 1;
    } else if (wReuse == 1) {
	for(i=0;i<=order; i++) windstat[0].rho[i] = rho[i];  /* save */
	windstat[0].err = (float) err;
	windstat[0].rms = (float) rms;
	return 1;
    } else 
	return 0;
}


/*--------------------------------------------------------------------*/
static int
retrieve_windstat(float *rho, int order, float *err, float *rms)
{
    Windstat wstat;
    int i;
	
    if(wReuse){
	wstat = windstat[0];
	for(i=0; i<=order; i++) rho[i] = wstat.rho[i];
	*err = wstat.err;
	*rms = wstat.rms;
	return 1;
    }
    else return 0;
}


/*--------------------------------------------------------------------*/
static float
get_similarity(int order, int size, float *pdata, float *cdata,
	       float *rmsa, float *rms_ratio, float pre, float stab, int w_type, int init)
{
  float rho3[BIGSORD+1], err3, rms3, rmsd3, b0, t, a2[BIGSORD+1], 
      rho1[BIGSORD+1], a1[BIGSORD+1], b[BIGSORD+1], err1, rms1, rmsd1;

/* (In the lpc() calls below, size-1 is used, since the windowing and
   preemphasis function assumes an extra point is available in the
   input data array.  This condition is apparently no longer met after
   Derek's modifications.) */

  /* get current window stat */
  xlpc(order, stab, size-1, cdata,
      a2, rho3, (float *) NULL, &err3, &rmsd3, pre, w_type);
  rms3 = wind_energy(cdata, size, w_type);
  
  if(!init) {
      /* get previous window stat */
      if( !retrieve_windstat(rho1, order, &err1, &rms1)){
	  xlpc(order, stab, size-1, pdata,
	      a1, rho1, (float *) NULL, &err1, &rmsd1, pre, w_type);
	  rms1 = wind_energy(pdata, size, w_type);
      }
      xa_to_aca(a2+1,b,&b0,order);
      t = xitakura(order,b,&b0,rho1+1,&err1) - .8f;
      if(rms1 > 0.0)
	  *rms_ratio = (0.001f + rms3)/rms1;
      else
	  if(rms3 > 0.0)
	      *rms_ratio = 2.0;	/* indicate some energy increase */
	  else
	      *rms_ratio = 1.0;	/* no change */
  } else {
      *rms_ratio = 1.0;
      t = 10.0;
  }
  *rmsa = rms3;
  save_windstat( rho3, order, err3, rms3);
  return((float)(0.2/t));
}


/* -------------------------------------------------------------------- */
/* This is an ad hoc signal stationarity function based on Itakura
 * distance and relative amplitudes.
 */
/* 
  This illustrates the window locations when the very first frame is read.
  It shows an example where each frame step |  .  | is 10 msec.  The
  frame step size is variable.  The window size is always 30 msec.
  The window centers '*' is always 20 msec apart.
  The windows cross each other right at the center of the DP frame, or
  where the '.' is.

                          ---------*---------   current window

              ---------*---------  previous window

  |  .  |  .  |  .  |  .  |  .  |  .  |  .  |  .  |  .  |
              ^           ^  ^
              ^           ^  ^
              ^           ^  fdata
              ^           ^
              ^           q
	      p

                          ---
                          ind

  fdata, q, p, ind, are variables used below.
   
*/

static WStat *wstat = NULL;
static float *mem = NULL;

static WStat*
get_stationarity(float *fdata, double freq, int buff_size, int nframes, int frame_step, int first_time)
{
  static int nframes_old = 0, memsize;
  float preemp = 0.4f, stab = 30.0f;
  float *p, *q, *r, *datend;
  int ind, i, j, m, size, order, agap, w_type = 3;

  agap = (int) (STAT_AINT *freq);
  size = (int) (STAT_WSIZE * freq);
  ind = (agap - size) / 2;

  if( nframes_old < nframes || !wstat || first_time){
    /* move this to init_dp_f0() later */
    nframes_old = nframes;
    if(wstat){
      free((char *) wstat->stat);
      free((char *) wstat->rms);
      free((char *) wstat->rms_ratio);
      free((char *) wstat);
    }
    if (mem) free((void *)mem); 
    wstat = (WStat *) malloc(sizeof(WStat));
    /*    spsassert(stat,"stat malloc failed in get_stationarity");*/
    wstat->stat = (float*)malloc(sizeof(float)*nframes);
    /*    spsassert(stat->stat,"stat->stat malloc failed in get_stationarity");*/
    wstat->rms = (float*)malloc(sizeof(float)*nframes);
    /*    spsassert(stat->rms,"stat->rms malloc failed in get_stationarity");*/
    wstat->rms_ratio = (float*)malloc(sizeof(float)*nframes);
    /*    spsassert(stat->rms_ratio,"stat->ratio malloc failed in get_stationarity");*/
    memsize = (int) (STAT_WSIZE * freq) + (int) (STAT_AINT * freq);
    mem = (float *) malloc( sizeof(float) * memsize);
    /*    spsassert(mem, "mem malloc failed in get_stationarity()");*/
    for(j=0; j<memsize; j++) mem[j] = 0;
  }
  
  if(nframes == 0) return(wstat);

  q = fdata + ind;
  datend = fdata + buff_size;

  if((order = (int) (2.0 + (freq/1000.0))) > BIGSORD) {
    fprintf(stderr,
	    "Optimim order (%d) exceeds that allowable (%d); reduce Fs\n",order, BIGSORD);
    order = BIGSORD;
  }

  /* prepare for the first frame */
  for(j=memsize/2, i=0; j<memsize; j++, i++) mem[j] = fdata[i];

  /* never run over end of frame, should already taken care of when read */

  for(j=0, p = q - agap; j < nframes; j++, p += frame_step, q += frame_step){
      if( (p >= fdata) && (q >= fdata) && ( q + size <= datend) )
	  wstat->stat[j] = get_similarity(order,size, p, q, 
					     &(wstat->rms[j]),
					     &(wstat->rms_ratio[j]),preemp,
					     stab,w_type, 0);
      else {
	  if(first_time) {
	      if( (p < fdata) && (q >= fdata) && (q+size <=datend) )
		  wstat->stat[j] = get_similarity(order,size, NULL, q,
						     &(wstat->rms[j]),
						     &(wstat->rms_ratio[j]),
						     preemp,stab,w_type, 1);
	      else{
		  wstat->rms[j] = 0.0;
		  wstat->stat[j] = 0.01f * 0.2f;   /* a big transition */
		  wstat->rms_ratio[j] = 1.0;   /* no amplitude change */
	      }
	  } else {
	      if( (p<fdata) && (q+size <=datend) ){
		  wstat->stat[j] = get_similarity(order,size, mem, 
						     mem + (memsize/2) + ind,
						     &(wstat->rms[j]),
						     &(wstat->rms_ratio[j]),
						     preemp, stab,w_type, 0);
		  /* prepare for the next frame_step if needed */
		  if(p + frame_step < fdata ){
		      for( m=0; m<(memsize-frame_step); m++) 
			  mem[m] = mem[m+frame_step];
		      r = q + size;
		      for( m=0; m<frame_step; m++) 
			  mem[memsize-frame_step+m] = *r++;
		  }
	      }
	  }
      }
  }

  /* last frame, prepare for next call */
  for(j=(memsize/2)-1, p=fdata + (nframes * frame_step)-1; j>=0 && p>=fdata; j--)
    mem[j] = *p--;
  return(wstat);
}

void free_dp_f0()
{
  int i;
  Frame *frm, *next;
  
  free((void *)pcands);
  pcands = NULL;
  
  free((void *)rms_speech);
  rms_speech = NULL;
  
  free((void *)f0p);
  f0p = NULL;
  
  free((void *)vuvp);
  vuvp = NULL;

  free((void *)plocs);
  plocs = NULL;
  
  free((void *)acpkp);
  acpkp = NULL;
  
  free((void *)peaks);
  peaks = NULL;
  
  free((void *)locs);
  locs = NULL;
  
  if (wReuse) {
    free((void *)windstat);
    windstat = NULL;
  }
  
  frm = headF;
  
  for(i = 0; i < size_cir_buffer; i++) {
    next = frm->next;
    free((void *)frm->cp->correl);
    free((void *)frm->dp->locs);
    free((void *)frm->dp->pvals);
    free((void *)frm->dp->mpvals);
    free((void *)frm->dp->prept);
    free((void *)frm->dp->dpvals);
    free((void *)frm->cp);
    free((void *)frm->dp);
    free((void *)frm);
    frm = next;
  }
  headF = NULL;
  tailF = NULL;
  
  free((void *)wstat->stat);
  free((void *)wstat->rms);
  free((void *)wstat->rms_ratio);

  free((void *)wstat);
  wstat = NULL;

  free((void *)mem);
  mem = NULL;
}
