/************************************************************************************/
/* Copyright ?????????                                                              */
/* All rights reserved.                                                             */  
/*                                                                                  */
/************************************************************************************/
/*                                                                                  */
/* AUTHOR: Aby Louw                                                                 */
/* Email:                                                                           */
/* MODIFIED: Feb 10, 2004                                                           */
/*                                                                                  */
/************************************************************************************/

#include <cstdio>
#include "festival.h"
#include "intonation.h"
#include "intsint.h"

EST_String tones[] = {"M","S","T","H","U","B","L","D"}; //decl

static float conv_ms(float ms);
static float funcout(int x, float uph, float hm, float upt, float tm, float kv);
static void quasp(EST_FVector k, EST_FVector h, EST_FVector t, EST_Track &momel_curve, int nval);
static EST_String tone_specified(EST_Item *s);
static EST_Item *vowel_seg(EST_Item *syl);
static void init_intsint_params(void);
static void intsint_targets_to_f0(EST_Utterance *utt);

static float target_f0_top = 0.0;
static float target_f0_mid = 0.0;
static float target_f0_bottom = 0.0;

static float higher = 0.0;
static float lower = 0.0;
static float up = 0.0;
static float down = 0.0;

LISP Intonation_INTSINT_Utt(LISP utt)
{
    // For each relation of type Relation predict intonation events.  

    EST_Utterance *u = get_c_utt(utt);
    EST_Item *s;
    EST_String tone;
    LISP tone_tree;

    tone_tree = siod_get_lval("INTSINT_intonation_cart_tree","no tone cart tree");

    u->create_relation("IntEvent");
    u->create_relation("Intonation");

    for (s=u->relation("Syllable")->first(); s != 0; s=next(s))
    {
	if ((tone = tone_specified(s)) == "0") // check if pre-specified
	    tone = wagon_predict(s,tone_tree).string();
	if (tone != "0")
	    add_IntEvent(u,s,tone);
    }
    return utt;
}


static EST_String tone_specified(EST_Item *s)
{
    // If there is an explicit accent specifed on the related token
    // If there is check the syllable to see if its strees or a singleton
    
    EST_String ptone("0");
    EST_Item *ss = s->as_relation("SylStructure");
    EST_Item *word = parent(ss);

    if (!word)
	return ptone;
    
    EST_Item *token = parent(word,"Token");
    
    if (token)
	ptone = ffeature(token,"tone").string();
    
    if (ptone == "0")
    {
	ptone = ffeature(word,"tone").string();

	if (ptone == "0")
	    return ptone;
    }
    
    if (next(ss) == 0)  // final syllable in word
	return ptone;
    else
	return "NONE";  // pre-specified but inappropriate syllable in word
}


EST_Track calc_momel_curve(EST_FVector momel_time, EST_FVector momel_f0, int nval)
{
    //int mpoints = momel_time.length();

    // update April 21
    int mpoints = momel_time.length() + 2; //added 2 virtual points to ensure qs will not go to big or to small

    /* temporary target points */
    EST_FVector ft;
    ft.resize(mpoints);
    ft.zero();
    
    EST_FVector fh;
    fh.resize(mpoints);
    fh.zero();

    EST_FVector fk;
    fk.resize(mpoints);
    fk.zero();

    EST_FVector t;
    t.resize(mpoints);
    t.zero();

    EST_FVector h;
    h.resize(mpoints);
    h.zero();

    EST_FVector k;
    k.resize(mpoints);
    k.zero();

    /* always starts at 0.01, assuming f0 = 0 at this time */
    ft.a_check(0) = -5;
    fh.a_check(0) = 0;

    
    for (int i = 1; i < mpoints; i++)
    {
	ft.a_check(i) = 10 * momel_time.a_check(i-1);
	fh.a_check(i) = momel_f0.a_check(i-1);
    }

     /* assuming f0 = 0 at end time */
    ft.a_check(mpoints - 1) = 10*nval-15; /* 0.01 seconds less than inflection point fk */
    fh.a_check(mpoints - 1) = 0;


    int nvalm1 = mpoints;
 
    /* quadratic regression SPLINE */
    /* Inflection points */
    fk.a_check(0) = 0;

    for (int i = 1; i < (nvalm1-1); i++)
    {
	fk.a_check(i) =  (ft.a_check(i) + ft.a_check(i+1))/2.0;
    }

    //fk.a_check(nvalm1-1) = 10000; 
	
    // update 21 April
    fk.a_check(nvalm1-1) = nval*10-5;
 	
    /* conversion ms */
    for (int i = 0; i < nvalm1; i++)
    {
	t.a_check(i) = conv_ms(ft.a_check(i));
	k.a_check(i) = conv_ms(fk.a_check(i));
	h.a_check(i) = fh.a_check(i);
    }

    EST_Track momel_curve;
    
    quasp(k,h,t,momel_curve,nval);

    return momel_curve;
}
	

/* millisecond conversion function  */
static float conv_ms(float ms)
{
    int pas = 10;
    float out = 0;
    
    out = ms/pas + 0.5;
    return out;
}

/* calculate quadratic function output */
static float funcout(int x, float uph, float hm, float upt, float tm, float kv)
{
    float outfunc = 0;

    outfunc = uph + ((hm-uph) * (x-upt) * (x-upt))/((kv-upt) * (tm-upt));
    return outfunc;
}

/* quadratic spline function */
static void quasp(EST_FVector k, EST_FVector h, EST_FVector t, EST_Track &momel_curve, int nval)
{
    float y = 0;
    int index = -1;
    int nvalm1 = k.length();
    
    int point_k0 = (int)(k.a_check(0) + 0.5);
    int x = 0; /* counter */

    EST_FVector temp_momel;
    temp_momel.resize((int)k.a_check(nvalm1-1));
  
    for (x = 0; x <= point_k0; x++)
    {
	y = funcout(x,h.a_check(0),h.a_check(1),t.a_check(0),t.a_check(1),k.a_check(0));
	index = index + 1;
	temp_momel.a_check(index) = y;
    }
 
    for (int i = 1; i <= (nvalm1 - 2); i++)
    {
	int point_ti = (int)t.a_check(i);
	int point_ki = (int)k.a_check(i);

	for (; x < point_ti; x++)
	{
	    int j = i - 1;
	    y = funcout(x,h.a_check(i),h.a_check(j),t.a_check(i),t.a_check(j),k.a_check(j));
	    index = index + 1;
	    temp_momel.a_check(index) = y;
	}

	for (; x < point_ki; x++)
	{
	    int j = i + 1;
	    y = funcout(x,h.a_check(i),h.a_check(j),t.a_check(i),t.a_check(j),k.a_check(i));
	    index = index + 1;
	    temp_momel.a_check(index) = y;
	}
    }

    int m = nvalm1 - 1;
    int point_km = (int)k.a_check(m);

    for (; x < point_km; x++)
    {
	int j = m - 1;
	y = funcout(x,h.a_check(m),h.a_check(j),t.a_check(m),t.a_check(j),k.a_check(j));
	index = index + 1;
	temp_momel.a_check(index) = y;
    }

    momel_curve.resize(nval,1);
    momel_curve.fill_time(0.01);

    for (int i = 0; i < nval; i++)
    {
	momel_curve.a(i,0) = temp_momel.a_check(i);
    }
}





float make_estimate(EST_String tone, float last_target, float mid, float top,
		    float bottom, float higher, float lower, float up, float down)
{
    float est = 0;

    if (tone == "M")
    {
      	est = mid;
    }
    else if (tone == "S")
    {
	est = last_target;
    }
    else if (tone == "T")
    {
	est = top;
    }
    else if (tone == "H")
    {
	est = last_target + (top - last_target) * higher;
    }
    else if (tone == "U")
    {
	est = last_target + (top - last_target) * up;
    }
    else if (tone == "B")
    {
	est = bottom;
    }
    else if (tone == "L")
    {
	est = last_target - (last_target - bottom) * lower;
    }
    else if (tone == "D")
    {
	est = last_target - (last_target - bottom) * down;
    }
    else if (tone == "NONE")
    {
	est = 0;
    }
    else 
    {
	EST_error("Insint_ISDB: unknown tone \"%s\"\n",tone.str());
    }
    
    return est;
}



void optimise(Intsint *is, float mid, float range, float *min_error2,
	      float higher, float lower, float up, float down)
{
    EST_StrList tones_list;  // decl
    EST_Litem *p; //decl

    EST_Utterance *u = &is->utt;

    // append adds items on to the end of a list
    for (unsigned int i1 = 0; i1 < sizeof(tones) /sizeof(tones[0]); i1++)
	tones_list.append(tones[i1]);

    float top = mid + range/2.0;
    float bottom = mid - range/2.0;

    EST_String label = "NONE";

    EST_Item *intsint = u->relation("IntEvent")->head(); //first item
  

    float f0 = octave(intsint->F("F0"));

    if ((top - f0) < fabs(f0 - mid))
    {
	label = "T";
	intsint->set_name(label);
    }
    else if ((f0 - bottom) < fabs(f0 - mid))
    {
	label = "B";
	intsint->set_name(label);
    }
    else 
    { 
	label = "M";
	intsint->set_name(label);
    }
    
    float last_estimate = 0;

    float testimate = make_estimate(intsint->name(),last_estimate,mid,top,bottom,higher,lower,up,down);
    
    intsint->set("Estimate_F0",linear(testimate));

    double error = fabs(testimate - f0);
    double ss_error = error*error;
    last_estimate = testimate;
    
    intsint = next(intsint);
 
    while (intsint)
    {
	float target = octave(intsint->F("F0"));

	// after pause choose from [MTB]
	if (intsint->I("PAUSE") == 1)
	{
	    if ((top - target) < fabs(target - mid))
	    {
		label = "T";
		intsint->set_name(label);
	    }
	    else if ((target - bottom) < fabs(target - mid))
	    {
		label = "B";
		intsint->set_name(label);
	    }
	    else
	    {
		label = "M";
		intsint->set_name(label);
	    }
	    // #elsewhere any tone except M
 	} 
	else 
	{ 
 	    float min_difference = BIG_NUMBER;
	    EST_String best_tone = "NONE";

	    for (p = tones_list.head(); p != 0; p = next(p))
	    {
		if (tones_list(p) != "M")
		{
		    testimate = make_estimate(tones_list(p),last_estimate,mid,top,bottom,higher,lower,up,down);
		    
		    float difference = fabs(target - testimate);
		    
		    if (difference < min_difference) 
		    {
			min_difference = difference;
			best_tone = tones_list(p);
		    }
		}
	    }
	    intsint->set_name(best_tone);
 	} 

	float qestimate = make_estimate(intsint->name(),last_estimate,mid,top,bottom,higher,lower,up,down);
	intsint->set("Estimate_F0",linear(qestimate));

 	error = fabs(qestimate - octave(intsint->F("F0")));
	ss_error += error*error;
 	last_estimate = qestimate;

	intsint = next(intsint);

    } 
    
    (*min_error2)+= ss_error; 
}



float octave(float t)
{
    return log(t)/log((float)2);
}

float linear(float t)
{
    return pow(2,t);
}

int round(float t)
{
    return int(t + 0.5);
}



//get the F0 from track of time t
float get_F0_track(EST_Track tr, float t)
{
    int index = tr.index(t);
    
    float F0 = tr.a(index,0);

    return F0;
}

float preceding_pause_length(EST_Item *i)
{
    float pause = 0;

    EST_Item *syl = parent(i->as_relation("Intonation"));

    EST_Item *phone = daughter1(syl->as_relation("SylStructure"))->as_relation("Segment");
    
    //just for safety
    if (prev(phone))
    {
	if (ph_is_silence(prev(phone)->name()))
	{
	    if (prev(prev(phone)))
	    {
		pause = prev(phone)->F("end") - prev(prev(phone))->F("end");
	    }
	    else
	    {
		pause = prev(phone)->F("end");
	    }
	    
	    return pause;
	}
	else
	{
	    return pause;
	}
    }
    else
	return pause;
}


//get the time of the middle of the vowel in the syllable
float get_syl_vowel_mid_time(EST_Item *syl)
{
    EST_Item *phone;
    EST_Item *prev_phone;
  

    float vowel_start = 0;
    float vowel_end = 0;
    float mid_time = 0;
    
    phone = vowel_seg(syl);
    prev_phone = prev_sibling(phone,"Segment");
    
    vowel_start = prev_phone->F("end");
    vowel_end = phone->F("end");
    mid_time = (vowel_end - vowel_start)/2.0 + vowel_start;

    return mid_time;
}

LISP Int_Targets_INTSINT_Utt(LISP utt)
{
    // Predict F0 targets using Linear regression
    EST_Utterance *u = get_c_utt(utt);
    EST_Item *p;
    EST_String intsint_tone;

    init_intsint_params();

    u->create_relation("Target");
  
  
    //target values
    float pos = 0;
    float f0 = 0;
    

    //get first INTSINT label
    p = u->relation("Intonation")->head(); //first syllable

    //get tone for first label
    intsint_tone = daughter1(p)->name();

    //get mid vowel time for first label
    pos =  get_syl_vowel_mid_time(p);
  
    if (intsint_tone == "T")
    {
	f0 = target_f0_top;
    }
    else if (intsint_tone == "B")
    {
	f0 = target_f0_bottom;
    }
    else if (intsint_tone == "M")
    {
	f0 = target_f0_mid;
    }
    else
    {
	EST_error("Int_Targets_INTSINT: IntEvent does not start with [T/M/B], starts with \"%s\"\n",intsint_tone.str());
    }

    //first target
    add_target(u,vowel_seg(p),pos,f0);

    float last_estimate = f0;
    
    for (p=next(u->relation("Intonation")->head()); p != 0; p=next(p))
    {
	//make estimate of f0
	f0 = make_estimate(daughter1(p)->name(),last_estimate,target_f0_mid,target_f0_top,target_f0_bottom,higher,lower,up,down);

	//get mid vowel time for f0
	pos =  get_syl_vowel_mid_time(p);
	
	//add target
	add_target(u,vowel_seg(p),pos,f0);
    }
    
   
    intsint_targets_to_f0(u);

    return utt;
}



static void intsint_targets_to_f0(EST_Utterance *utt)
{
    EST_Track *f0 = new EST_Track;
    
    utt->create_relation("f0");
    EST_Item *f = utt->relation("f0")->append();

    f->set("name", "f0");
    f->set_val("f0", est_val(f0));

    EST_Item *p;
   
    //make momel vectors
    EST_FVector momel_time;
    EST_FVector momel_f0;
    int count_momel = 0;

    for (p = utt->relation("Target")->head(); p != 0; p=next(p))
    {
	count_momel++;

	momel_time.resize(count_momel);
	momel_f0.resize(count_momel);

	momel_time.a_check(count_momel - 1) = daughter1(p)->F("pos")/SAMPLE_RATE;
	momel_f0.a_check(count_momel - 1) = daughter1(p)->F("f0");
    }

    //get length of utt
    p = utt->relation("Segment")->tail();
    float utt_length = p->F("end");
    int nval = int(utt_length/SAMPLE_RATE + 0.5);

    EST_Track momel_curve = calc_momel_curve(momel_time,momel_f0,nval);

  
 //    float start = 0.0;
//     float end = 0.0;
//     //now zero the unvoiced portions
//     for (p = utt->relation("Segment")->head(); p != 0; p=next(p))
//     {
// 	start = end;
// 	end = p->F("end");

// 	int start_track = momel_curve.index(start);
// 	int end_track = momel_curve.index_below(end);

// 	//phone is unvoiced, 0 momel curve in this region
// 	if (!(ph_is_voiced(p->name()) || ph_is_vowel(p->name())))
// 	{
// 	    for (int i = start_track; i <= end_track; i++)
// 	    {
// 		momel_curve.a(i,0) = 0.0;
// 	    }
// 	}
//     }

      
    (*f0) = momel_curve;
}
    
static void init_intsint_params(void)
{
    LISP params;

    params = siod_get_lval("intsint_target_params","no target params");

    higher = get_param_float("intsint_higher",params,0.5);
    lower = get_param_float("intsint_lower",params,0.5);
    up = get_param_float("intsint_up",params,0.25);
    down = get_param_float("intsint_down",params,0.25);

    target_f0_top = get_param_float("target_f0_top",params,0.0);
    target_f0_mid = get_param_float("target_f0_mid",params,0.0);
    target_f0_bottom = get_param_float("target_f0_bottom",params,0.0);
}


static EST_Item *vowel_seg(EST_Item *syl)
{
    // return related to vowel segment
    EST_Item *p;

    for (p=daughter1(syl,"SylStructure"); p != 0; p=next(p))
	if (ph_is_vowel(p->name()))
	    return p;

    // No vowel found, so return first daughter.
    return daughter1(syl,"SylStructure");
}
    
