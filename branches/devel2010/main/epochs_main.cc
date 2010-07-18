/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                         Copyright (c) 1996                            */
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
/*                    Author :  Nickolay V. Shmyrev                      */
/*                    Date   :  2007                                     */
/*-----------------------------------------------------------------------*/
/*        Pitchmarking program using dynamic programming                 */
/*************************************************************************/

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "EST_unix.h"
#include "EST_cmd_line_options.h"
#include "EST_cmd_line.h"
#include "EST_speech_class.h"
#include "sigpr/EST_pitchmark.h"


static EST_write_status save_msec(EST_Track &pm, EST_String filename);
static EST_write_status save_ogi_bin(EST_Track &pm, EST_String filename, int sr);
void  			pm_to_label(EST_Track &pm, EST_Relation &lab);
void   			set_options(EST_Features &op, EST_Option &al);


/** @name <command> epochs </command> <emphasis> Find periodic marks in a speech </emphasis>

  * @id epochs-manual
  * @toc */

//@{


/**@name Synopsis
  */
//@{

//@synopsis

/**
<command>epochs</command> locates instants of glottal closure in a waveform, 
and performs post-processing to produce even pitchmarks. 
Pitchmarking is performed by calling the
<function>epochs()</function> function
**/

//@}

/**@name OPTIONS
  */
//@{

//@options

//@}


int main (int argc, char *argv[])
{
    EST_Track pm;
    EST_Wave lx;
    EST_Option al;
    EST_Features op;
    EST_String out_file("-");
    EST_StrList files;

    parse_command_line
	(argc, argv, 
       EST_String("[input file] -o [output file] [options]")+
       "Summary: pitchmark waveform files\n"
       "use \"-\" to make input and output files stdin/out\n"
       "-h               Options help\n\n"+
       options_wave_input()+
       options_track_output()+
	 "-f0 <ofile> Calculate F0 from pitchmarks and save to file\n\n"
	 "-style <string>  \"track\" or \"lab\"\n\n", files, al);

    set_options(op, al);

    out_file = al.present("-o") ? al.val("-o") : (EST_String)"-";

    if (read_wave(lx, files.first(), al) != read_ok)
	exit(-1);

    if (al.present("-inv"))
	invert(lx);

    pm = epochs(lx);

    if (al.present("-f0"))
    {
	EST_Track f0;
	pm_to_f0(pm, f0);
	f0.save(al.val("-f0"));
    }

    // various options for filling the gaps between distant pitchmarks
    // and removing pitchmarks that are too close together

    if (al.present("-style"))
    {
	// label format
	if (al.val("-style").contains("lab"))
	{
	    EST_Relation lab;
	    pm_to_label(pm, lab);
	    if (lab.save(out_file + ".pm_lab") != write_ok)
		exit(-1);
	}
	// save file in "traditional" milli-second format
	if (al.val("-style").contains("msec"))
	    save_msec(pm, out_file + ".pm");

	// ogi binary integer sample point format
	if (al.val("-style").contains("ogi_bin"))
	    save_ogi_bin(pm, out_file + ".pmv", lx.sample_rate());
    }
    else if (pm.save(out_file, al.val("-otype", 0)) != write_ok)
    {
	cerr << "pitchmark: failed to write output to \"" 
	    << out_file << "\"" << endl;
	exit(-1);
    }
    return 0;
}

static EST_write_status save_msec(EST_Track &pm, EST_String filename)
{
    ostream *outf;
    
    if (filename == "-")
	outf = &cout;
    else
	outf = new ofstream(filename);
    
    if (!(*outf))
	return write_fail;
    
    outf->precision(5);
    outf->setf(ios::fixed, ios::floatfield);
    outf->width(8);
    
    for (int i = 0; i < pm.num_frames(); ++i)
	*outf << pm.t(i)  * 1000.0 << endl;
    
    return write_ok;
}

static EST_write_status save_ogi_bin(EST_Track &pm, EST_String filename, int sr)
{
    int *d;
    FILE *fp;
    int i;
    
    d = new int[pm.num_frames()];
    
    for (i = 0; i < pm.num_frames(); ++i)
	d[i] = int(pm.t(i) * (float) sr);
    
    if ((fp = fopen(filename, "wb")) == NULL)
	return misc_write_error;
    
    if (fwrite(d, pm.num_frames(), sizeof(int), fp) != 1)
    {
	fclose(fp);
	return misc_write_error;
    }
    delete d;
    
    return write_ok;
}

void override_lib_ops(EST_Option &op, EST_Option &al)
{	
        return;
}

void set_options(EST_Features &op, EST_Option &al)
{
	return;
}

/** @name Examples
<formalpara><title>Basic Pitchmarking</title>
<para>
<screen>
$ epochs kdt_010.wav -o kdt_010.pm -otype est
</screen>
</para> 
</formalpara>
*/

//@{
//@}
