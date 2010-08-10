/*************************************************************************/
/*                                                                       */
/*                Centre for Speech Technology Research                  */
/*                     University of Edinburgh, UK                       */
/*                      Copyright (c) 1996-1998                          */
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
/*                     Author :  Sergio Oller                            */
/*                     Date   :  2010                                    */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/* Additions to the UTF-8 library, and inclusion of the library          */
/*                                                                       */
/*=======================================================================*/
#ifndef __EST_UTF8_H__
#define __EST_UTF8_H__

#include "utf8/utf8.h"

void cp2utf8(utf8::uint32_t cp,char *utf8string);

/** If st is UTF-8, get the next Unicode Code Point and save it to cp.
  * If st is not UTF-8, return the next byte.
  * In any case, return the number of bytes read.
  */
int getnextcp(char *st, bool is_utf8, utf8::uint32_t &cp);

/** If st is UTF-8, get the previous Unicode Code Point and save it to cp.
  * If st is not UTF-8, return the previous byte.
  * In any case, return the number of bytes unread.
  */
int getprevcp(char *st_begin, char *st,
	      bool is_utf8, utf8::uint32_t &cp);
	      

#endif
