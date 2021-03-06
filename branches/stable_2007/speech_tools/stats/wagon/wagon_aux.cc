/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                      Copyright (c) 1996,1997                          */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*                     Author :  Alan W Black                            */
/*                     Date   :  May 1996                                */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  Various method functions                                             */
/*=======================================================================*/

#include <cstdlib>
#include <iostream>
#include <cstring>
#include "EST_unix.h"
#include "EST_cutils.h"
#include "EST_Token.h"
#include "EST_Wagon.h"

EST_Val WNode::predict(const WVector &d)
{
    if (leaf())
	return impurity.value();
    else if (question.ask(d))
	return left->predict(d);
    else
	return right->predict(d);
}

WNode *WNode::predict_node(const WVector &d)
{
    if (leaf())
	return this;
    else if (question.ask(d))
	return left->predict_node(d);
    else
	return right->predict_node(d);
}

int WNode::pure(void)
{
    //  A node is pure if it has no sub-nodes or its not of type class

    if ((left == 0) && (right == 0))
	return TRUE;
    else if (get_impurity().type() != wnim_class)
	return TRUE;
    else
	return FALSE;
}

void WNode::prune(void)
{
    // Check all sub-nodes and if they are all of the same class 
    // delete their sub nodes.  Returns pureness of this node

    if (pure() == FALSE)
    {
	// Ok lets try and make it pure
	if (left != 0) left->prune();
	if (right != 0) right->prune();

	// Have to check purity as well as values to ensure left and right
	// don't further split
	if ((left->pure() == TRUE) && ((right->pure() == TRUE)) &&
	    (left->get_impurity().value() == right->get_impurity().value()))
	{
	     delete left; left = 0;
	     delete right; right = 0;
	}
    }    

}

void WNode::held_out_prune()
{
    // prune tree with held out data
    // Check if node's questions differentiates for the held out data
    // if not, prune all sub_nodes

    // Rescore with prune data
    set_impurity(WImpurity(get_data()));  // for this new data

    if (left != 0)
    {
	wgn_score_question(question,get_data());
	if (question.get_score() < get_impurity().measure())
	{  // its worth goint ot the next level
	    wgn_find_split(question,get_data(),
		       left->get_data(),
		       right->get_data());
	    left->held_out_prune();
	    right->held_out_prune();
	}
	else
	{  // not worth the split so prune both sub_nodes
	    delete left; left = 0;
	    delete right; right = 0;
	}
    }    
}

void WNode::print_out(ostream &s, int margin)
{
    int i;

    s << endl;
    for (i=0;i<margin;i++) s << " ";
    s << "(";
    if (left==0) // base case
	s << impurity;
    else
    {
	s << question;
	left->print_out(s,margin+1);
	right->print_out(s,margin+1);
    }
    s << ")";
}

ostream & operator <<(ostream &s, WNode &n)
{
    // Output this node and its sub-node 

    n.print_out(s,0);
    s << endl;
    return s;
}

void WDataSet::load_description(const EST_String &fname, LISP ignores)
{
    // Initialise a dataset with sizes and types
    EST_String tname;
    int i;
    LISP description,d;

    description = car(vload(fname,1));
    dlength = siod_llength(description);

    p_type.resize(dlength);
    p_ignore.resize(dlength);
    p_name.resize(dlength);

    if (wgn_predictee_name == "")
	wgn_predictee = 0;  // default predictee is first field
    else
	wgn_predictee = -1;

    for (i=0,d=description; d != NIL; d=cdr(d),i++)
    {
	p_name[i] = get_c_string(car(car(d)));
	tname = get_c_string(car(cdr(car(d))));
	p_ignore[i] = FALSE;
	if ((wgn_predictee_name != "") && (wgn_predictee_name == p_name[i]))
	    wgn_predictee = i;
	if ((wgn_count_field_name != "") && 
	    (wgn_count_field_name == p_name[i]))
	    wgn_count_field = i;
	if ((tname == "count") || (i == wgn_count_field))
	{
	    // The count must be ignored, repeat it if you want it too
	    p_type[i] = wndt_ignore;  // the count must be ignored
	    p_ignore[i] = TRUE;
	    wgn_count_field = i;
	}
	else if ((tname == "ignore") || (siod_member_str(p_name[i],ignores)))
	{
	    p_type[i] = wndt_ignore;  // user specified ignore
	    p_ignore[i] = TRUE;
	    if (i == wgn_predictee)
		wagon_error(EST_String("predictee \"")+p_name[i]+
			    "\" can't be ignored \n");
	}
	else if (siod_llength(car(d)) > 2)
	{
	    LISP rest = cdr(car(d));
	    EST_StrList sl;
	    siod_list_to_strlist(rest,sl);
	    p_type[i] = wgn_discretes.def(sl);
	    if (streq(get_c_string(car(rest)),"_other_"))
		wgn_discretes[p_type[i]].def_val("_other_");
	}
	else if (tname == "binary")
	    p_type[i] = wndt_binary;
	else if (tname == "cluster")
	    p_type[i] = wndt_cluster;
	else if (tname == "vector")
	    p_type[i] = wndt_vector;
	else if (tname == "trajectory")
	    p_type[i] = wndt_trajectory;
	else if (tname == "matrix")
	    p_type[i] = wndt_matrix;
	else if (tname == "float")
	    p_type[i] = wndt_float;
	else 
	{
	    wagon_error(EST_String("Unknown type \"")+tname+
			"\" for field number "+itoString(i)+
                        "/"+p_name[i]+" in description file \""+fname+"\"");
	}
    }

    if (wgn_predictee == -1)
    {
	wagon_error(EST_String("predictee field \"")+wgn_predictee_name+
			"\" not found in description ");
    }
}

const int WQuestion::ask(const WVector &w) const
{
    // Ask this question of the given vector
    switch (op)
    {
      case wnop_equal:    // for numbers
	if (w.get_flt_val(feature_pos) == operand1.Float())
	    return TRUE;
	else 
	    return FALSE;
      case wnop_binary:    // for numbers
	if (w.get_int_val(feature_pos) == 1)
	    return TRUE;
	else 
	    return FALSE;
      case wnop_greaterthan:
	if (w.get_flt_val(feature_pos) > operand1.Float())
	    return TRUE;
	else 
	    return FALSE;
      case wnop_lessthan:
	if (w.get_flt_val(feature_pos) < operand1.Float())
	    return TRUE;
	else 
	    return FALSE;
      case wnop_is:       // for classes
	if (w.get_int_val(feature_pos) == operand1.Int())
	    return TRUE;
	else
	    return FALSE;
      case wnop_in:       // for subsets -- note operand is list of ints
	if (ilist_member(operandl,w.get_int_val(feature_pos)))
	    return TRUE;
	else
	    return FALSE;
      default:
	wagon_error("Unknown test operator");
    }
    
    return FALSE;
}

ostream& operator<<(ostream& s, const WQuestion &q)
{
    EST_String name;
    static EST_Regex needquotes(".*[()'\";., \t\n\r].*");

    s << "(" << wgn_dataset.feat_name(q.get_fp());
    switch (q.get_op())
    {
      case wnop_equal:
	s << " = " << q.get_operand1().string();
	break;
      case wnop_binary:
	break;
      case wnop_greaterthan:
	s << " > " << q.get_operand1().Float();
	break;
      case wnop_lessthan:
	s << " < " << q.get_operand1().Float();
	break;
      case wnop_is:
	name = wgn_discretes[wgn_dataset.ftype(q.get_fp())].
	    name(q.get_operand1().Int());
	s << " is ";
	if (name.matches(needquotes))
	    s << quote_string(name,"\"","\\",1);
	else
	    s << name;
	break;
      case wnop_matches:
	name = wgn_discretes[wgn_dataset.ftype(q.get_fp())].
	    name(q.get_operand1().Int());
	s << " matches " << quote_string(name,"\"","\\",1);
	break;
      case wnop_in:
	s << " in (";
	for (int l=0; l < q.get_operandl().length(); l++)
	{
	    name = wgn_discretes[wgn_dataset.ftype(q.get_fp())].
		name(q.get_operandl().nth(l));
	    if (name.matches(needquotes))
		s << quote_string(name,"\"","\\",1);
	    else
		s << name;
	    s << " ";
	}
	s << ")";
	break;
        // SunCC wont let me add this
//      default:
//	s << " unknown operation ";
    }
    s << ")";
    
    return s;
}

EST_Val WImpurity::value(void)
{
    // Returns the recommended value for this 
    EST_String s;
    double prob;
    
    if (t==wnim_unset)
    {
	cerr << "WImpurity: no value currently set\n";
	return EST_Val(0.0);
    }
    else if (t==wnim_class)
	return EST_Val(p.most_probable(&prob));
    else if (t==wnim_cluster)
	return EST_Val(a.mean());
    else if (t==wnim_vector)
	return EST_Val(a.mean()); /* wnim_vector */
    else if (t==wnim_trajectory)
	return EST_Val(a.mean()); /* NOT YET WRITTEN */
    else
	return EST_Val(a.mean());
}

double WImpurity::samples(void)
{
    if (t==wnim_float)
	return a.samples();
    else if (t==wnim_class)
	return (int)p.samples();
    else if (t==wnim_cluster)
	return members.length();
    else if (t==wnim_vector)
	return members.length();
    else if (t==wnim_trajectory)
	return members.length();
    else
	return 0;
}

WImpurity::WImpurity(const WVectorVector &ds)
{
    int i;

    t=wnim_unset;
    a.reset(); trajectory=0; l=0; width=0;
    for (i=0; i < ds.n(); i++)
    {
	if (wgn_count_field == -1)
	    cumulate((*(ds(i)))[wgn_predictee],1);
	else
	    cumulate((*(ds(i)))[wgn_predictee],
		     (*(ds(i)))[wgn_count_field]);
    }
}

float WImpurity::measure(void)
{
    if (t == wnim_float)
	return a.variance()*a.samples();
    else if (t == wnim_vector)
	return vector_impurity();
    else if (t == wnim_trajectory)
	return trajectory_impurity();
    else if (t == wnim_matrix)
	return a.variance()*a.samples();
    else if (t == wnim_class)
	return p.entropy()*p.samples();
    else if (t == wnim_cluster)
	return cluster_impurity();
    else
    {
	cerr << "WImpurity: can't measure unset object" << endl;
	return 0.0;
    }
}

float WImpurity::vector_impurity()
{
    // Find the mean/stddev for all values in all vectors
    // sum the variances and multiply them by the number of members
    EST_Litem *pp;
    int i,j;
    EST_SuffStats b;
    double count = 1;

    a.reset();

#if 1
    /* simple distance */
    for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
    {
        b.reset();
        for (pp=members.head(); pp != 0; pp=next(pp))
        {
            i = members.item(pp);
            b += wgn_VertexTrack.a(i,j);
        }
        a += b.stddev();
        count = b.samples();
    }
#endif

#if 0
    /* full covariance */
    /* worse in listening experiments */
    EST_SuffStats **cs;
    int mmm;
    cs = new EST_SuffStats *[wgn_VertexTrack_end+1];
    for (j=0; j<=wgn_VertexTrack_end; j++)
        cs[j] = new EST_SuffStats[wgn_VertexTrack_end+1];
    /* Find means for diagonal */
    for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
    {
        for (pp=members.head(); pp != 0; pp=next(pp))
            cs[j][j] += wgn_VertexTrack.a(members.item(pp),j);
    }
    for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
        for (i=j+1; i<wgn_VertexTrack_end; i++)
            for (pp=members.head(); pp != 0; pp=next(pp))
            {
                mmm = members.item(pp);
                cs[i][j] += (wgn_VertexTrack.a(mmm,i)-cs[j][j].mean())*
                    (wgn_VertexTrack.a(mmm,j)-cs[j][j].mean());
            }
    for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
        for (i=j+1; i<wgn_VertexTrack_end; i++)
            a += cs[i][j].stddev();
    count = cs[0][0].samples();
#endif

#if 0
    // look at mean euclidean distance between vectors 
    EST_Litem *qq;
    int x,y;
    double d,q;
    count = 0;
    for (pp=members.head(); pp != 0; pp=next(pp))
    {
        x = members.item(pp);
        count++;
        for (qq=next(pp); qq != 0; qq=next(qq))
        {
            y = members.item(qq);
            for (q=0.0,j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
            {
                d = wgn_VertexTrack(x,j)-wgn_VertexTrack(y,j);
                q += d*d;
            }
            a += sqrt(q);
        }

    }
#endif

    // This is sum of stddev*samples
    return a.mean() * count; 
}

WImpurity::~WImpurity()
{
    int j;

    if (trajectory != 0)
    {
        for (j=0; j<l; j++)
            delete [] trajectory[j];
        delete [] trajectory;
        trajectory = 0;
        l = 0;
    }
}


float WImpurity::trajectory_impurity()
{
    // Find the mean length of all the units in the cluster
    // Create that number of points
    // Interpolate each unit to that number of points
    // collect means and standard deviations for each point
    // impurity is sum of the variance for each point and each coef
    // multiplied by the number of units.
    EST_Litem *pp;
    int i, j;
    int s, ti, ni, q;
    int s1l, s2l;
    double n, m, m1, m2, w;
    EST_SuffStats lss, stdss;
    EST_SuffStats l1ss, l2ss;
    int l1, l2;
    int ola=0;

    if (trajectory != 0)
    {   /* already done this */
        return score;
    }

    lss.reset();
    l = 0;
    for (pp=members.head(); pp != 0; pp=next(pp))
    {
        i = members.item(pp);
        for (q=0; q<wgn_UnitTrack.a(i,1); q++)
        {
            ni = (int)wgn_UnitTrack.a(i,0)+q;
            if (wgn_VertexTrack.a(ni,0) == -1.0)
            {
                l1ss += q;
                ola = 1;
                break;
            }
        }
        if (q==wgn_UnitTrack.a(i,1))
        {   /* can't find -1 center point, so put all in l2 */
            l1ss += 0;
            l2ss += q;
        }
        else
            l2ss += wgn_UnitTrack.a(i,1) - (q+1) - 1;
        lss += wgn_UnitTrack.a(i,1); /* length of each unit in the cluster */
        if (wgn_UnitTrack.a(i,1) > l)
            l = (int)wgn_UnitTrack.a(i,1);
    }

    if (ola==0)  /* no -1's so its not an ola type cluster */
    {
        l = ((int)lss.mean() < 7) ? 7 : (int)lss.mean();

        /* a list of SuffStats on for each point in the trajectory */
        trajectory = new EST_SuffStats *[l];
        width = wgn_VertexTrack_end+1;
        for (j=0; j<l; j++)
            trajectory[j] = new EST_SuffStats[width];

        for (pp=members.head(); pp != 0; pp=next(pp))
        {   /* for each unit */
            i = members.item(pp);
            m = (float)wgn_UnitTrack.a(i,1)/(float)l; /* find interpolation */
            s = (int)wgn_UnitTrack.a(i,0); /* start point */
            for (ti=0,n=0.0; ti<l; ti++,n+=m)
            {
                ni = (int)n;  // hmm floor or nint ??
                for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
                    trajectory[ti][j] += wgn_VertexTrack.a(s+ni,j);
            }
        }

        /* find sum of sum of stddev for all coefs of all traj points */
        stdss.reset();
        for (ti=0; ti<l; ti++)
            for (j=wgn_VertexTrack_start+1; j<=wgn_VertexTrack_end; j++)
                stdss += trajectory[ti][j].stddev();

        // This is sum of all stddev * samples
        score = stdss.mean() * members.length();
    }
    else
    {   /* OLA model */
        l1 = (l1ss.mean() < 10.0) ? 10 : (int)l1ss.mean();
        l2 = (l2ss.mean() < 10.0) ? 10 : (int)l2ss.mean();
        l = l1 + l2 + 1 + 1;

        /* a list of SuffStats on for each point in the trajectory */
        trajectory = new EST_SuffStats *[l];
        for (j=0; j<l; j++)
            trajectory[j] = new EST_SuffStats[wgn_VertexTrack_end+1];

        for (pp=members.head(); pp != 0; pp=next(pp))
        {   /* for each unit */
            i = members.item(pp);
            s1l = 0;
            s = (int)wgn_UnitTrack.a(i,0); /* start point */
            for (q=0; q<wgn_UnitTrack.a(i,1); q++)
                if (wgn_VertexTrack.a(s+q,0) == -1.0)
                {
                    s1l = q; /* printf("awb q is -1 at %d\n",q); */
                    break;
                }
            s2l = (int)wgn_UnitTrack.a(i,1) - (s1l + 2);
            m1 = (float)(s1l)/(float)l1; /* find interpolation step */
            m2 = (float)(s2l)/(float)l2; /* find interpolation step */
            /* First half */
            for (ti=0,n=0.0; s1l > 0 && ti<l1; ti++,n+=m1)
            {
                ni = s + (((int)n < s1l) ? (int)n : s1l - 1);
                for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
                    trajectory[ti][j] += wgn_VertexTrack.a(ni,j);
            }
            ti = l1; /* do it explicitly in case s1l < 1 */
            for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
                trajectory[ti][j] += -1;
            /* Second half */
            s += s1l+1;
            for (ti++,n=0.0; s2l > 0 && ti<l-1; ti++,n+=m2)
            {
                ni = s + (((int)n < s2l) ? (int)n : s2l - 1);
                for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
                    trajectory[ti][j] += wgn_VertexTrack.a(ni,j);
            }
            for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
                trajectory[ti][j] += -2;
        }

        /* find sum of sum of stddev for all coefs of all traj points */
        /* windowing the sums with a triangular weight window         */
        stdss.reset();
        m = 1.0/(float)l1;
        for (w=0.0,ti=0; ti<l1; ti++,w+=m)
            for (j=wgn_VertexTrack_start+1; j<=wgn_VertexTrack_end; j++)
                stdss += trajectory[ti][j].stddev() * w;
        m = 1.0/(float)l2;
        for (w=1.0,ti++; ti<l-1; ti++,w-=m)
            for (j=wgn_VertexTrack_start+1; j<=wgn_VertexTrack_end; j++)
                stdss += trajectory[ti][j].stddev() * w;
    
        // This is sum of all stddev * samples
        score = stdss.mean() * members.length();
    }
    return score;
}

float WImpurity::cluster_impurity()
{
    // Find the mean distance between all members of the dataset
    // Uses the global DistMatrix for distances between members of
    // the cluster set.  Distances are assumed to be symmetric thus only
    // the bottom half of the distance matrix is filled
    EST_Litem *pp, *q;
    int i,j;
    double dist;

    a.reset();
    for (pp=members.head(); pp != 0; pp=next(pp))
    {
	i = members.item(pp);
	for (q=next(pp); q != 0; q=next(q))
	{
	    j = members.item(q);
	    dist = (j < i ? wgn_DistMatrix.a_no_check(i,j) :
 		            wgn_DistMatrix.a_no_check(j,i));
	    a+=dist;  // cumulate for whole cluster
	}
    }

    // This is sum distance between cross product of members
//    return a.sum();
    return a.stddev() * a.samples();
}

float WImpurity::cluster_distance(int i)
{
    // Distance this unit is from all others in this cluster
    // in absolute standard deviations from the the mean.
    float dist = cluster_member_mean(i);
    float mdist = dist-a.mean();
    
    if (mdist == 0.0)
	return 0.0;
    else
	return fabs((dist-a.mean())/a.stddev());

}

int WImpurity::in_cluster(int i)
{
    // Would this be a member of this cluster?.  Returns 1 if 
    // its distance is less than at least one other
    float dist = cluster_member_mean(i);
    EST_Litem *pp;

    for (pp=members.head(); pp != 0; pp=next(pp))
    {
	if (dist < cluster_member_mean(members.item(pp)))
	    return 1;
    }
    return 0;
}

float WImpurity::cluster_ranking(int i)
{
    // Position in ranking closest to centre
    float dist = cluster_distance(i);
    EST_Litem *pp;
    int ranking = 1;

    for (pp=members.head(); pp != 0; pp=next(pp))
    {
	if (dist >= cluster_distance(members.item(pp)))
	    ranking++;
    }

    return ranking;
}

float WImpurity::cluster_member_mean(int i)
{
    // Returns the mean difference between this member and all others
    // in cluster
    EST_Litem *q;
    int j,n;
    double dist,sum;

    for (sum=0.0,n=0,q=members.head(); q != 0; q=next(q))
    {
	j = members.item(q);
	if (i != j)
	{
	    dist = (j < i ? wgn_DistMatrix(i,j) : wgn_DistMatrix(j,i));
	    sum += dist;
	    n++;
	}
    }

    return ( n == 0 ? 0.0 : sum/n );
}

void WImpurity::cumulate(const float pv,double count)
{
    // Cumulate data for impurity calculation 

    if (wgn_dataset.ftype(wgn_predictee) == wndt_cluster)
    {
	t = wnim_cluster;
	members.append((int)pv);
    }
    else if (wgn_dataset.ftype(wgn_predictee) == wndt_vector)
    {
	t = wnim_vector;
	members.append((int)pv);
    }
    else if (wgn_dataset.ftype(wgn_predictee) == wndt_trajectory)
    {
	t = wnim_trajectory;
	members.append((int)pv);
    }
    else if (wgn_dataset.ftype(wgn_predictee) >= wndt_class)
    {
	if (t == wnim_unset)
	    p.init(&wgn_discretes[wgn_dataset.ftype(wgn_predictee)]);
	t = wnim_class;
	p.cumulate((int)pv,count);
    }
    else if (wgn_dataset.ftype(wgn_predictee) == wndt_binary)
    {
	t = wnim_float;
	a.cumulate((int)pv,count);
    }
    else if (wgn_dataset.ftype(wgn_predictee) == wndt_float)
    {
	t = wnim_float;
	a.cumulate(pv,count);
    }
    else
    {
	wagon_error("WImpurity: cannot cumulate EST_Val type");
    }
}

ostream & operator <<(ostream &s, WImpurity &imp)
{
    int j,i;
    EST_SuffStats b;

    if (imp.t == wnim_float)
	s << "(" << imp.a.stddev() << " " << imp.a.mean() << ")";
    else if (imp.t == wnim_vector)
    {
	EST_Litem *p;
	s << "((";
        imp.vector_impurity();
        if (wgn_vertex_output == "mean")  //output means
        {
            for (j=0; j<wgn_VertexTrack.num_channels(); j++)
            {
                b.reset();
                for (p=imp.members.head(); p != 0; p=next(p))
                {
                    b += wgn_VertexTrack.a(imp.members.item(p),j);
                }
                s << "(" << b.mean() << " " << b.stddev() << ")";
                if (j+1<wgn_VertexTrack.num_channels())
                    s << " ";
            }
        }
        else /* output best in the cluster */
        {
            /* print out vector closest to center, rather than average */
            double best = WGN_HUGE_VAL;
            double x,d;
            int bestp = 0;
            EST_SuffStats *cs;

            cs = new EST_SuffStats [wgn_VertexTrack_end+1];
            
            for (j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
            {
                cs[j].reset();
                for (p=imp.members.head(); p != 0; p=next(p))
                {
                    cs[j] += wgn_VertexTrack.a(imp.members.item(p),j);
                }
            }

            for (p=imp.members.head(); p != 0; p=next(p))
            {
                for (x=0,j=wgn_VertexTrack_start; j<=wgn_VertexTrack_end; j++)
                {
                    d = (wgn_VertexTrack.a(imp.members.item(p),j)-cs[j].mean())
                        /* / b.stddev() */ ;
                    x += d*d;
                }
                if (x < best)
                {
                    bestp = imp.members.item(p);
                    best = x;
                }
            }
            for (j=0; j<wgn_VertexTrack.num_channels(); j++)
            {
                s << "( ";
                s << wgn_VertexTrack.a(bestp,j);
                s << " 0 "; // fake stddev
                s << " ) ";
                if (j+1<wgn_VertexTrack.num_channels())
                    s << " ";
            }

            delete [] cs;
        }
	s << ") ";
	s << imp.a.mean() << ")";
    }
    else if (imp.t == wnim_trajectory)
    {   
	s << "((";
        imp.trajectory_impurity();
        for (i=0; i<imp.l; i++)
        {
            s << "(";
            for (j=0; j<wgn_VertexTrack.num_channels(); j++)
            {
                s << "(" << imp.trajectory[i][j].mean() << " " 
                  << imp.trajectory[i][j].stddev() << " " << ")";
            }
            s << ")\n";
        }
	s << ") ";
	// Mean of cross product of distances (cluster score)
	s << imp.a.mean() << ")";
    }
    else if (imp.t == wnim_cluster)
    {
	EST_Litem *p;
	s << "((";
	for (p=imp.members.head(); p != 0; p=next(p))
	{
	    // Ouput cluster member and its mean distance to others
	    s << "(" << imp.members.item(p) << " " <<
		imp.cluster_member_mean(imp.members.item(p)) << ")";
	    if (next(p) != 0)
		s << " ";
	}
	s << ") ";
	// Mean of cross product of distances (cluster score)
	s << imp.a.mean() << ")";
    }
    else if (imp.t == wnim_class)
    {
	int i;
	EST_String name;
	double prob;

	s << "(";
	for (i=imp.p.item_start(); !imp.p.item_end(i); i=imp.p.item_next(i))
	{
	    imp.p.item_prob(i,name,prob);
	    s << "(" << name << " " << prob << ") ";
	}
	s << imp.p.most_probable(&prob) << ")";
    }
    else
	s << "([WImpurity unset])";
    
    return s;
}




