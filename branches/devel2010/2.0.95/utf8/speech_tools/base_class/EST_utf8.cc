/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                         Copyright (c) 2010                            */
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
/*                      Author :  Sergio Oller                           */
/*                      Date   :  2010                                   */
/*-----------------------------------------------------------------------*/
/*                    General UTF-8 functions                            */
/*                                                                       */
/*=======================================================================*/

#include <iostream>
#include "EST_utf8.h"

namespace EST {
char* cp2utf8(UnicodeChar cp,char *utf8string,bool appendzero)
{
    char *t;
    t=utf8::append(cp,utf8string);
    *t='\0';
    return t+1;

}

int append (UnicodeChar cp, bool is_utf8, EST_String &st)
{
    if (is_utf8 ==false)
    {
	char stri[2]={(char) cp, '\0' };
	st+=stri;
	return 0;
    } else
    {
	char utf8char[5];
	cp2utf8(cp,utf8char,true);
	st+=utf8char;
	return 0;
    }
}

int append (std::vector<UnicodeChar> vec, bool is_utf8, EST_String &st)
{
    for (std::vector<UnicodeChar>::iterator it=vec.begin();
	 it != vec.end();
	 ++it)
	append(*it,is_utf8,st);
    return 0;
}


int getnextcp(char *st, bool is_utf8, UnicodeChar &cp)
{
    if (is_utf8 ==false)
    {
	cp=(UnicodeChar) *st;
	return 1;
    } else
    {
	char *it=st;
	cp=utf8::next(it,it+sizeof(it));
	return it-st;
    }
}

int getprevcp(char *st_begin, char *st,
	      bool is_utf8, UnicodeChar &cp)
{
    if (st<=st_begin) {cp=0;return 0;}
    if (is_utf8 == false)
    {
	cp=(UnicodeChar) *(st-1);
	return 1;
    } else
    {
	char *it=st,*begin=st_begin;
	cp=utf8::prior(it,begin);
	return st-it;
    }
}
}