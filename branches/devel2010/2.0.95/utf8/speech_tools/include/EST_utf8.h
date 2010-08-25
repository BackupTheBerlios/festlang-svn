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

#include <vector>
#include "EST_String.h"
#include "utf8/utf8.h"

#define ErrUnicodeChar 0xE000  // First character from the Unicode Private Use Area
                               // We use it as an EOF mark.

namespace EST
{
typedef utf8::uint32_t UnicodeChar;

/** cp2utf8 converts an UnicodeChar "cp" into a UTF-8 string ( maybe 0-ended)
 *  Returns 0 if cp is valid or -1 if cp is invalid.
 *  cp2utf8 does not allocate space, so there should be up to 5 bytes free
 *  in utf8string, as an UTF-8 character can be up to 4 four bytes long
 *  and cp2utf8 may add a 0 at the end if appendzero is false.
 */
char* cp2utf8(UnicodeChar cp,char *utf8string,bool appendzero=true);

int append (UnicodeChar cp, bool is_utf8, EST_String &st);
int append (std::vector<UnicodeChar> vec, bool is_utf8, EST_String &st);

/** If st is UTF-8, get the next Unicode Code Point and save it to cp.
  * If st is not UTF-8, return the next byte.
  * In any case, return the number of bytes read.
  */
int getnextcp(char *st, bool is_utf8, UnicodeChar &cp);

/** If st is UTF-8, get the previous Unicode Code Point and save it to cp.
  * If st is not UTF-8, return the previous byte.
  * In any case, return the number of bytes unread.
  */
int getprevcp(char *st_begin, char *st,
	      bool is_utf8, UnicodeChar &cp);
	      

}
#endif
