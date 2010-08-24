/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                       Copyright (c) 1996,1997                         */
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
/*  Author :  Sergio Oller based on Alan W Black and Paul Taylor work    */
/*                     Date   :  December 2009                           */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                   Festival Path definitions                           */
/*                                                                       */
/*=======================================================================*/

/*
This file takes several path variables defined in the config files and uses them as
the default paths to look in festival.

Most of these variables can be changed at runtime while calling festival using a
"--libdir /new/lib/dir" for example.

How should these paths be set up? There are different options, based on how 
the different OS work.

UNIX based systems usually have a directory for binaries (/usr/bin/), 
a directory for architecture-dependent files (/usr/lib/festival/), 
a directory for architecture independent files (/usr/share/festival) , 
a directory for system-configuration files (/etc/festival), and may have
different directories for documentation and or the examples.

Hardcoding these paths into Festival seems a good choice, because UNIX users 
don't choose where should programs be installed (they use whatever the 
distribution says, and each distribution can customize paths at package building stage).

Windows systems usually put all the program related files at the same folder
where the binary file is. For example, if festival is installed at
 C:\Program files\festival, libdir should be expected at
 C:\Program files\festival\lib (and so on).
 
This also allows the not uncommon option of carrying festival on a pendrive.

I have been unable to find a C++  standard way to get the path to the 
festival.exe file. That's why windows API is required for a good Windows
compilation.



*/
#ifndef __FESTIVALPath_H__
#define __FESTIVALPath_H__


#define _S_S_S(S) #S
#define STRINGIZE(S) _S_S_S(S)

// Allow the path to be passed in without quotes because Windoze command line
// is stupid
// Extra level of indirection needed to get an extra macro expansion. Yeuch.

/* FIXME: As Windows doesn't use these paths anymore, maybe we could 
 * clean this a little bit */

#ifdef FTLIBDIRC
#define FTLIBDIR STRINGIZE(FTLIBDIRC)
#endif
#ifndef FTLIBDIR
#define FTLIBDIR "/usr/lib/festival/"
#endif

#ifdef FTETCDIRC
#    define FTETCDIR STRINGIZE(FTETCDIRC)
#endif
#ifndef FTETCDIR
#define FTETCDIR "/etc/festival/"
#endif

#ifdef FTEXAMPLESDIRC
#define FTEXAMPLESDIR STRINGIZE(FTEXAMPLESDIRC)
#endif
#ifndef FTEXAMPLESDIR
#define FTEXAMPLESDIR "/usr/share/doc/festival/examples/"
#endif

#ifdef FTDOCDIRC
#    define FTDOCDIR STRINGIZE(FTDOCDIRC)
#endif
#ifndef FTDOCDIR
#define FTDOCDIR "/usr/share/doc/festival/"
#endif

#ifdef FTOSTYPEC
#    define FTOSTYPE STRINGIZE(FTOSTYPEC)
#endif
#ifndef FTOSTYPE
#define FTOSTYPE ""
#endif

#endif /* __FESTIVALPath_H__ */
