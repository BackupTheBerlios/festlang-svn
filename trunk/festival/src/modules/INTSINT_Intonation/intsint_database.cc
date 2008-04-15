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

#include <stdio.h>
#include "festival.h"
#include "intsint.h"
#include "intonation.h"


VAL_REGISTER_CLASS(intsintdb,Intsint_DB)
SIOD_REGISTER_CLASS(intsintdb,Intsint_DB)
static void intsint_load_utterances(Intsint_DB *isdb);

static LISP ISDB_list = NIL;
static Intsint_DB *current_isdb = 0;




static void isdb_add(const EST_String &name, Intsint_DB *isdb)
{
    LISP lpair;

    lpair = siod_assoc_str(name,ISDB_list);

    if (ISDB_list == NIL)
	gc_protect(&ISDB_list);
    
    if (lpair == NIL)
    {
	ISDB_list = cons(cons(strintern(name),
				   cons(siod(isdb),NIL)),
			    ISDB_list);
    }
    else
    {
	cwarn << "ISDB " << name << " recreated" << endl;
	// old one will be garbage collected
	setcar(cdr(lpair),siod(isdb));
    }

    return;
}



Intsint::Intsint()
{
    name = "";
}

Intsint::~Intsint()
{
    //nothing yet
}

Intsint_DB::Intsint_DB()
{
    gc_protect(&params);
}

static void del_intsint(void *s) { delete (Intsint *)s; }

Intsint_DB::~Intsint_DB()
{
    index.clear(del_intsint);
    gc_unprotect(&params);
}

LISP intsint_load_db(LISP params, LISP utts_names)
{
    Intsint_DB *isdb = new Intsint_DB;

    isdb->params = params;
    isdb->uttnames = utts_names;

    intsint_load_utterances(isdb);

    isdb_add(get_param_str("index_name",params,"intsint")+EST_String("_isdb"),isdb);

    current_isdb = isdb;
    
    return NIL;
}

static void intsint_load_utterances(Intsint_DB *isdb)
{
    EST_String utts_dir = EST_String("") +
	get_param_str("utts_dir",isdb->params,"./festival/utts/");
    
    Intsint *ls = 0;

    LISP utt_names = isdb->uttnames;

    cout <<"loading utts into intsint database ..."<<endl;

    while (utt_names)
    {
	Intsint *s = new Intsint;

	s->name = get_c_string(car(utt_names));
	
	EST_String utt_filename = utts_dir + s->name + 	get_param_str("utts_ext",isdb->params,".utt");
 	
	EST_Utterance u;
	if (u.load(utt_filename) != read_ok)
	{
	    EST_error("could not load utterance file \"%s\"\n",utt_filename.str());
	}

	s->utt = u;

	if (ls != 0)
	{
	    s->prev_unit = ls;
	    ls->next_unit = s;
	}

	isdb->index.add(s->name,s);
	ls = s;
	utt_names = cdr(utt_names);
    }
    cout <<endl;
}

Intsint_DB *check_isdb()
{
    if (current_isdb == 0)
    {
	cerr << "Intsint_DB: no database loaded\n";
	festival_error();
    }
    return current_isdb;
}

LISP isdb_list(void)
{
    // List names of all current defined insint members
    LISP d = NIL;
    LISP l;

    for (l=ISDB_list; l != NIL; l=cdr(l))
	d = cons(car(car(l)),d);

    return d;
}

LISP isdb_select(LISP dbname)
{
    // Select named isdb and make it current
    EST_String name = get_c_string(dbname);
    LISP lpair;

    lpair = siod_assoc_str(name,ISDB_list);

    if (lpair == NIL)
    {
	cerr << "Intsint_DB " << name << " not defined" << endl;
	festival_error();
    }
    else
	current_isdb = intsintdb(car(cdr(lpair)));

    return dbname;
}


LISP isdb_optimise(void)
{
    Intsint_DB *isdb = check_isdb();
    
    LISP utt_names = NIL;
    
    float meanf0 =  isdb->momel_mean;

    float linear_mean_f0 = round(linear(meanf0));

    float min_range = get_param_float("intsint_min_range",isdb->params,0.5);

    float max_range = get_param_float("intsint_max_range",isdb->params,2.5);

    float step_range = get_param_float("intsint_step_range",isdb->params,0.1);

    float mean_shift = get_param_float("intsint_mean_shift",isdb->params,50);

    float step_shift = get_param_float("intsint_step_shift",isdb->params,1);
    
    float higher = get_param_float("intsint_higher",isdb->params,0.5);

    float lower = get_param_float("intsint_lower",isdb->params,0.5);

    float up = get_param_float("intsint_up",isdb->params,0.25);

    float down = get_param_float("intsint_down",isdb->params,0.25);

    float min_mean = linear_mean_f0 - mean_shift;

    float max_mean = linear_mean_f0 + mean_shift;

    float best_mid = 0;

    float best_range = 0;

    float mid = 0;

    int range_to = (int)((max_range - min_range)/step_range + 0.5);
    int lm_to = (int)((max_mean - min_mean)/step_shift + 0.5); 

    
    cout <<"linear_mean_f0 = "<<linear_mean_f0<<endl;
    cout <<"searching f0 from "<<min_mean<<" Hz to "<<min_mean + step_shift*lm_to<<" Hz"<<endl;
    cout <<"smallest range is from "<<linear(octave(min_mean) - min_range/2.0)
	 <<" Hz to "<<linear(octave(min_mean) + min_range/2.0)<<" Hz"<<endl;
    cout <<"largest range is from "<<linear(octave(min_mean + step_shift*lm_to) - (min_range + (step_range*range_to))/2.0)
	 <<" Hz to "<<linear(octave(min_mean + step_shift*lm_to) + (min_range + (step_range*range_to))/2.0)<<" Hz"<<endl<<endl;

    cout <<"Optimising intsint labels for f0 search space ..."<<endl<<endl;
    
    float db_error2 = BIG_NUMBER;

    for (int i = 0; i < range_to; i++) 
    {
	float range = (min_range + step_range*i);
	cout <<"range = "<<range<<endl;

	for (int j = 0; j < lm_to; j++) 
	{
	    float lm = (min_mean + step_shift*j);
	    float min_error2 = 0;
	    mid = octave(lm);
	    
	    utt_names = isdb->uttnames;

	    while (utt_names)
	    {
		Intsint *temp = isdb->get_unit(get_c_string(car(utt_names)));
		
		optimise(temp,mid,range,&min_error2,higher,lower,up,down);
		
		utt_names = cdr(utt_names);
	    }

	    if (min_error2 < db_error2)
	    {
		db_error2 = min_error2;
		best_mid = mid;
		best_range = range;
	
		utt_names = isdb->uttnames;
		
		while (utt_names)
		{
		    Intsint *temp = isdb->get_unit(get_c_string(car(utt_names)));
		    EST_Utterance *u = &temp->utt;
		    EST_Item *intsint = u->relation("IntEvent")->head(); //first item
    
		    while (intsint)
		    {
			EST_String current_tone = intsint->name();
			intsint->set("Best_tone",current_tone);

			float current_f0 = intsint->F("Estimate_F0");
			intsint->set("Best_F0",current_f0);
			
			intsint = next(intsint);
		    }
		    
		    utt_names = cdr(utt_names);
		}
	    }
	} 
    } 


    mid = best_mid;
    float key = linear(mid);
    float bottom = round(linear(mid - best_range/2.0));
    float top = round(linear(mid + best_range/2.0));

    cout <<endl;
    cout <<"found:"<<endl;
    cout <<"mid = "<<key<<endl;
    cout <<"top = "<<top<<endl;
    cout <<"bottom = "<<bottom<<endl;

    isdb->top = top;
    isdb->bottom = bottom;
    isdb->mid = key;

    return NIL;
}


LISP isdb_add_intsint_labels(void)
{
    Intsint_DB *isdb = check_isdb();
 
    LISP utt_names = isdb->uttnames;

    EST_String momelqs_dir = EST_String("") +
	get_param_str("momelqs_dir",isdb->params,"no momel_qs dir");
    
    EST_String momelqs_ext = EST_String("") +
	get_param_str("momelqs_ext",isdb->params,".noextensions");
    
    EST_String momel_dir = EST_String("") +
	get_param_str("momel_dir",isdb->params,"no momel dir");
    
    EST_String momel_ext = EST_String("") +
	get_param_str("momel_ext",isdb->params,".noextensions");
    
     
    float threshold = get_param_float("intsint_threshold",isdb->params,0.05);
    float pause_length = get_param_float("pause_length",isdb->params,0.25);

    int stressed_syls =  get_param_int("only_stressed_syls",isdb->params,0);

    float momel_mean = 0;
    int nutts = 0;

    cout <<"adding intsint labels to utts in intsint database ... "<<endl;

    while (utt_names)
    {
	EST_String momelqs_filename = momelqs_dir + get_c_string(car(utt_names)) + momelqs_ext;
	EST_Track momelqs;
	if (momelqs.load(momelqs_filename) != read_ok)
	    EST_error("could not load momel quadratic spline file \"%s\"\n",momelqs_filename.str());

	EST_String momel_filename = momel_dir + get_c_string(car(utt_names)) + momel_ext;
	EST_Track momel;
	if (momel.load(momel_filename) != read_ok)
	    EST_error("could not load momel points file \"%s\"\n",momel_filename.str());


	nutts++;

 	float sumf0 = 0;
	int nval = 0;
	
	Intsint *temp = isdb->get_unit(get_c_string(car(utt_names)));

 	EST_Utterance *u = &temp->utt;

	//delete old relations
	u->remove_relation("IntEvent");
	u->remove_relation("Intonation");

	//create new relations
	u->create_relation("IntEvent");
	u->create_relation("Intonation");

	float FT = 0;
	float FB = 1000;

	float prevf0 = 0;
	float currentf0 = 0;
	float nextf0 = 0;
	float pause = 0;
   
	EST_Item *syl;
	EST_Item *intsint;
	EST_String label = "NONE";

	float mid_time = 0;
	
	printf ("proccessign utt %s\n", get_c_string(car(utt_names)));
	
	//first set a target f0 value to all stressed syllables
	for (syl=u->relation("Syllable")->first(); syl != 0; syl=next(syl))
	{
	    //only do stressed syls
	    if (stressed_syls == 1)
	    {
		if (syl->I("stress") == 1)
		{
		    mid_time = get_syl_vowel_mid_time(syl);
		    currentf0 = get_F0_track(momelqs,mid_time);

		    if (currentf0 < 0)
		    {
			// hack, sometimes the EST pda does not give nice F0 values, which then
			// messes with the Momel algorithm
			currentf0 = 100;
		    }

		    nval++;
		
		    sumf0 += octave(currentf0);
		
		    add_IntEvent(u,syl,label);
		
		    intsint = daughter1(syl->as_relation("Intonation"));

		    intsint->set("end",mid_time);

		
		    if (currentf0 > FT)
		    {
			FT = currentf0;
		    }
		    
		    if (currentf0 < FB)
		    { 
			FB = currentf0;
		    }
		    
		    intsint->set("F0",currentf0);
		    intsint->set("PAUSE",0);
		}
	    }
	    else
	    {
		mid_time = get_syl_vowel_mid_time(syl);
		currentf0 = get_F0_track(momelqs,mid_time);

		if (currentf0 > 0)
		{
		    nval++;
		    
		    sumf0 += octave(currentf0);
		
		    add_IntEvent(u,syl,label);
		    
		    intsint = daughter1(syl->as_relation("Intonation"));
		
		    intsint->set("end",mid_time);
		    
		    if (currentf0 > FT)
		    {
			FT = currentf0;
		    }
		    
		    if (currentf0 < FB)
		    { 
			FB = currentf0;
		    }
		    
		    intsint->set("F0",currentf0);
		    intsint->set("PAUSE",0);
		}
	    }
	}

	float meanf0;
	
	if(nval>0)
        	meanf0 = sumf0/nval;
	else
		meanf0 = 110.0;
	
	momel_mean += meanf0;
        printf ("Momel mean %g %g %d\n", momel_mean, sumf0, nval);

	//now code highest (T) and lowest (B) target INTSINT labels    
	for (intsint=u->relation("IntEvent")->first(); intsint != 0; intsint=next(intsint))
	{
	    currentf0 = intsint->F("F0");
	    
	    if (currentf0 >= ((1 - threshold)*FT))
	    {
		label = "T";
		intsint->set_name(label);
	    }
	    else if (currentf0 <= ((1 + threshold)*FB))
	    {
		label = "B";
		intsint->set_name(label);
	    }
	}
	
	// first label must be set to "M" if not a "T" or "B"
	intsint = u->relation("IntEvent")->first();
	if (intsint->name() == "NONE")
	{
	    label = "M";
	    intsint->set_name(label);
	}
	
	//now code the rest (M,U,H,D,L,S)
	for (intsint=next(u->relation("IntEvent")->first()); intsint != 0; intsint=next(intsint))
	{
	    if (intsint->name() == "NONE")
	    {
		pause = preceding_pause_length(intsint);
		currentf0 = intsint->F("F0");
		prevf0 = prev(intsint)->F("F0");

		if (next(intsint))
		{
		    nextf0 = next(intsint)->F("F0");
		}
		else
		    nextf0 = -1;
		
		//first check for pause
		if (pause >= pause_length)
		{
		    if (intsint->name() == "NONE")
		    {
			label = "M";
			intsint->set_name(label);
			intsint->set("PAUSE",1);
		    }
		}
		else if ((fabs(currentf0 - prevf0)/prevf0) <= threshold)
		{
		    label = "S";
		    intsint->set_name(label);
		}
		else if (currentf0 > prevf0)
		{
		    if (nextf0 != -1)
		    {
			if (currentf0 > nextf0)
			{
			    label = "H";
			    intsint->set_name(label);
			}
			else
			{
			    label = "U";
			    intsint->set_name(label);
			}
		    }
		    else
		    {
			label = "H";
			intsint->set_name(label);
		    }
		}
		else if (currentf0 < prevf0) 
		{
		    if (nextf0 != -1)
		    {
			if (currentf0 < nextf0)
			{
			    label = "L";
			    intsint->set_name(label);
			} 
			else
			{
			    label = "D";
			    intsint->set_name(label);
			}
		    }
		    else
		    {
			label = "L";
			intsint->set_name(label);
		    }
		}
	    }
	}
    
	utt_names = cdr(utt_names);
    }
    
    cout <<endl;

    isdb->momel_mean = momel_mean/nutts;
    printf ("Momel mean %g\n", momel_mean);

    return NIL;
}


LISP isdb_save_intsint_labels(void)
{
    Intsint_DB *isdb = check_isdb();
 
    LISP utt_names = isdb->uttnames;

    EST_String intsint_dir = EST_String("") +
	get_param_str("intsint_dir",isdb->params,"syl_intsint_lab/");
    
    EST_String intsint_ext = EST_String("") +
	get_param_str("intsint_ext",isdb->params,".intsint");
    
    cout <<"writing intsint label files for utterances in intsint database ..."<<endl;

    while (utt_names)
    {
	EST_String intsint_label_filename = intsint_dir + get_c_string(car(utt_names)) + intsint_ext;
	
	Intsint *temp = isdb->get_unit(get_c_string(car(utt_names)));

	EST_Utterance u = temp->utt;

	EST_Relation *ilabel = u.relation("IntEvent");
	
	ilabel->f.set("timing_style", "end");
	ilabel->remove_item_feature("F0");
	ilabel->remove_item_feature("id");
	ilabel->remove_item_feature("PAUSE");
	ilabel->remove_item_feature("Estimate_F0");
	ilabel->remove_item_feature("Best_F0");
	
	EST_Item *intsint = ilabel->head();
	
	while (intsint)
	{
	    intsint->set_name(intsint->S("Best_tone"));
	    intsint = next(intsint);
	}

	ilabel->remove_item_feature("Best_tone");

	if (ilabel->save(intsint_label_filename) != write_ok)
	{
	    EST_error("Intsint_DB: can't write label file \"%s\"\n",intsint_label_filename.str());
	}

	utt_names = cdr(utt_names);
    }
    
    cout <<endl;
	
    return NIL;
}

LISP isdb_save_intsint_utts(void)
{
    Intsint_DB *isdb = check_isdb();
 
    LISP utt_names = isdb->uttnames;

    EST_String intsint_utt_dir = EST_String("") +
	get_param_str("intsint_utt_dir",isdb->params,"./festival/intsint_utts/");
    
    EST_String intsint_utt_ext = EST_String("") +
	get_param_str("intsint_utt_ext",isdb->params,".utt");

    EST_String intsint_defs_file = EST_String("") + get_param_str("intsint_defs",isdb->params,"/etc/intsint.defs");

    ofstream defsfile(intsint_defs_file);

    defsfile <<"top\t"<<isdb->top<<endl;
    defsfile <<"mid\t"<<isdb->mid<<endl;
    defsfile <<"bottom\t"<<isdb->bottom<<endl;

    defsfile.close();
     
    cout <<"writing intsint utt files for utterances in intsint database ..."<<endl;

    while (utt_names)
    {
	EST_String intsint_utt_filename = intsint_utt_dir + get_c_string(car(utt_names)) + intsint_utt_ext;

	Intsint *temp = isdb->get_unit(get_c_string(car(utt_names)));

	EST_Utterance *u = &temp->utt;

	EST_Relation *ilabel = u->relation("IntEvent");
	
	EST_Item *intsint = ilabel->head();
	
	while (intsint)
	{
	    intsint->set_name(intsint->S("Best_tone"));
	    intsint = next(intsint);
	}

	if (u->save(intsint_utt_filename) != write_ok)
	{
	    EST_error("Intsint_DB: can't write utt file \"%s\"\n",intsint_utt_filename.str());
	}

	utt_names = cdr(utt_names);
    }

    cout <<endl;
	
    return NIL;
}

LISP isdb_calc_intsint_momelqs(void)
{
    Intsint_DB *isdb = check_isdb();
    
    LISP utt_names = isdb->uttnames;

    
    EST_String imomelqs_dir = EST_String("") +
	get_param_str("intsint_momelqs_dir",isdb->params,"intsint_momel_qs/");
    
    EST_String imomelqs_ext = EST_String("") +
	get_param_str("intsint_momelqs_ext",isdb->params,".imomelqs");

    float higher = get_param_float("intsint_higher",isdb->params,0.5);

    float lower = get_param_float("intsint_lower",isdb->params,0.5);

    float up = get_param_float("intsint_up",isdb->params,0.25);

    float down = get_param_float("intsint_down",isdb->params,0.25);

    float top = isdb->top;
    float mid = isdb->mid;
    float bottom = isdb->bottom;
    
    cout <<"calculating momel quadratic spline from optimised intsint labels ..."<<endl;

    while (utt_names)
    {
	Intsint *temp = isdb->get_unit(get_c_string(car(utt_names)));

	EST_Utterance *u = &temp->utt;

	EST_Item *last_p = u->relation("Segment")->tail();

	float utt_length = last_p->F("end");

	int nval = int(utt_length/SAMPLE_RATE + 0.5);

	EST_FVector ft;
	EST_FVector fh;

	int ival = 0;

	EST_Item *p = u->relation("IntEvent")->head(); //first item

	ft.resize(ival + 1);
	fh.resize(ival + 1);

	ft.a_check(ival) = p->F("end")/SAMPLE_RATE;

	
	if (p->name() == "T")
	{
	    fh.a_check(ival) = top;
	}
	else if (p->name() == "B")
	{
	    fh.a_check(ival) = bottom;
	}
	else if (p->name() == "M")
	{
	    fh.a_check(ival) = mid;
	}
	else
	{
	    EST_error("Intsint_DB: intsint label file \"%s\" does not start with [T/M/B]\n",get_c_string(car(utt_names)));
	}

	float last_estimate = fh.a_check(ival);

	ival++;

	for (p = next(u->relation("IntEvent")->head()); p != 0; p=next(p))
	{
	    ft.resize(ival + 1);
	    fh.resize(ival + 1);
	    
	    fh.a_check(ival) = make_estimate(p->name(),last_estimate,mid,top,bottom,higher,lower,up,down);
	    ft.a_check(ival) = p->F("end")/SAMPLE_RATE;

	    last_estimate = fh.a_check(ival);
	    ival++;
	}

	EST_Track momel_curve = calc_momel_curve(ft,fh,nval);
	momel_curve.set_equal_space(true);

	EST_String intsint_imomelqs_filename = imomelqs_dir + get_c_string(car(utt_names)) + imomelqs_ext;

	if (momel_curve.save(intsint_imomelqs_filename,"ssff") != write_ok)
	{
	    EST_error("Intsint_DB: can't write momel quadratic spline curve file \"%s\"\n",intsint_imomelqs_filename.str());
	}

	utt_names = cdr(utt_names);
    }

    cout <<endl;
	
    return NIL;	
}
