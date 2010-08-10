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

void cp2utf8(utf8::uint32_t cp,char *utf8string)
{
    if (cp < 0x0080)
    {
        utf8string[0]=(char) cp;
        utf8string[1]='\0';
    } else if (cp < 0x0800)
    {
        utf8string[0]=(char) (0xC0 | (cp >> 6 ));
        utf8string[1]=(char) (0x80 | (cp & 0x003F));
        utf8string[2]='\0';
    } else if (cp < 0xFFFF)
    {
        utf8string[0]=(char) (0xE0 | (cp >> 12 ));
        utf8string[1]=(char) (0x80 | ((cp >> 6) & 0x003F));
        utf8string[2]=(char) (0x80 | (cp & 0x003F));
        utf8string[3]='\0';
    } else if (cp < 0x10FFFF)
    {
        utf8string[0]=(char) (0xF0 | (cp >> 18 ));
        utf8string[1]=(char) (0x80 | ((cp >> 12) & 0x003F));
        utf8string[2]=(char) (0x80 | ((cp >> 6 ) & 0x003F));
        utf8string[3]=(char) (0x80 | (cp & 0x003F));
        utf8string[4]='\0';
    } else
    {
        std::cerr << "invalid Code point: " << cp << std::endl;
        utf8string[0]='\0';
    }
    return;
}


int getnextcp(char *st, bool is_utf8, utf8::uint32_t &cp)
{
    if (is_utf8 ==false)
    {
	cp=(utf8::uint32_t) *st;
	return 1;
    } else
    {
	char *it=st;
	cp=utf8::next(it,it+sizeof(it));
	return it-st;
    }
}

int getprevcp(char *st_begin, char *st,
	      bool is_utf8, utf8::uint32_t &cp)
{
    if (st<=st_begin) {cp=0;return 0;}
    if (is_utf8 == false)
    {
	cp=(utf8::uint32_t) *(st-1);
	return 1;
    } else
    {
	char *it=st,*begin=st_begin;
	cp=utf8::prior(it,begin);
	return st-it;
    }
}
