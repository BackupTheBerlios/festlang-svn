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
/*                         Author :  Alan W Black                        */
/*                         Date   :  April 1996                          */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/* A Tokenize class, both for Tokens (Strings plus alpha)                */
/* EST_TokenStream for strings, FILE *, files, pipes etc                 */
/*                                                                       */
/*=======================================================================*/
#include <cstdio>
#include <iostream>
#include "EST_unix.h"
#include <cstdlib>
#include <climits>
#include <cstring>
#include <string>
#include "EST_math.h"
#include "EST_Token.h"
#include "EST_string_aux.h"
#include "EST_cutils.h"
#include "EST_error.h"
#include "EST_utf8.h"
using namespace std;

namespace EST
{
#if 0
#if defined(INSTANTIATE_TEMPLATES)
template utf8::uint32_t utf8::next<char*>(char*&, char*);
template utf8::uint32_t utf8::prior<char*>(char*&, char*);
template char* utf8::append<char*>(utf8::uint32_t, char*);
template class std::_Rb_tree<utf8::uint32_t,std::pair <utf8::uint32_t const, char>, \
	std::_Select1st<std::pair<utf8::uint32_t const, char> >, \
	std::less<utf8::uint32_t>, \
	std::allocator<std::pair<utf8::uint32_t const, char> > >;

template char* utf8::find_invalid<char*>(char*, char*);
template utf8::uint32_t utf8::unchecked::next<char*>(char*&);

template utf8::uint32_t utf8::next<std::istreambuf_iterator<char, std::char_traits<char> > > \
			          (std::istreambuf_iterator<char, std::char_traits<char> >&, \
				   std::istreambuf_iterator<char, std::char_traits<char> >);

template utf8::internal::utf_error utf8::internal::validate_next<char*>(char*&, char*, utf8::uint32_t *);
template utf8::internal::utf_error utf8::internal::get_sequence_1<char*>(char*&, char*, utf8::uint32_t*);
template utf8::internal::utf_error utf8::internal::get_sequence_2<char*>(char*&, char*, utf8::uint32_t*);
template utf8::internal::utf_error utf8::internal::get_sequence_3<char*>(char*&, char*, utf8::uint32_t*);
template utf8::internal::utf_error utf8::internal::get_sequence_4<char*>(char*&, char*, utf8::uint32_t*);

template utf8::internal::utf_error utf8::internal::validate_next<std::istreambuf_iterator<char, std::char_traits<char> > >(std::istreambuf_iterator<char, std::char_traits<char> >&, std::istreambuf_iterator<char, std::char_traits<char> >, utf8::uint32_t*);

template utf8::internal::utf_error utf8::internal::get_sequence_1<std::istreambuf_iterator<char, std::char_traits<char> > >(std::istreambuf_iterator<char, std::char_traits<char> >&, std::istreambuf_iterator<char, std::char_traits<char> >, utf8::uint32_t*);
template utf8::internal::utf_error utf8::internal::get_sequence_2<std::istreambuf_iterator<char, std::char_traits<char> > >(std::istreambuf_iterator<char, std::char_traits<char> >&, std::istreambuf_iterator<char, std::char_traits<char> >, utf8::uint32_t*);
template utf8::internal::utf_error utf8::internal::get_sequence_3<std::istreambuf_iterator<char, std::char_traits<char> > >(std::istreambuf_iterator<char, std::char_traits<char> >&, std::istreambuf_iterator<char, std::char_traits<char> >, utf8::uint32_t*);
template utf8::internal::utf_error utf8::internal::get_sequence_4<std::istreambuf_iterator<char, std::char_traits<char> > >(std::istreambuf_iterator<char, std::char_traits<char> >&, std::istreambuf_iterator<char, std::char_traits<char> >, utf8::uint32_t*);
template class utf8::iterator<char*>;
template class utf8::iterator<char*>(__gnu_cxx::__normal_iterator<char*, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, __gnu_cxx::__normal_iterator<char*, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
//template utf8::iterator<char*> (tok_stuff.begin(),tok_stuff.begin(),tok_stuff.end());

#endif
#endif

const EST_String EST_Token_Default_WhiteSpaceChars = " \t\n\r";
const EST_String EST_Token_Default_SingleCharSymbols = "(){}[]";
const EST_String EST_Token_Default_PrePunctuationSymbols = "\"'`({[";
const EST_String EST_Token_Default_PunctuationSymbols = "\"'`.,:;!?]})";

const EST_String EST_Token_Default_WhiteSpaceCharsUTF8 = " \t\n\r             ​  ⁠";
const EST_String EST_Token_Default_SingleCharSymbolsUTF8 = "";
const EST_String EST_Token_Default_PrePunctuationSymbolsUTF8 = "\"'([{«ʻ‹¿¡`";
const EST_String EST_Token_Default_PunctuationSymbolsUTF8 = "!\"'(),.:;¿?[]`{}¡§«»­ʹʺʻʼʽ؟჻᛫᛬᛭⁕⁖⁗⁘⁙⁚⁛⁜⁝⁞‹›※‼‽⁅⁆⁇⁈⁉";

const EST_String Token_Origin_FD = "existing file descriptor";
const EST_String Token_Origin_Stream = "existing istream";
const EST_String Token_Origin_String = "existing string";

static EST_Regex RXanywhitespace("[ \t\n\r]");


ostream& operator<<(ostream& s, const EST_Token &p)
{
    s << "[TOKEN " << p.pname << "]";
    return s;
}


EST_Token &EST_Token::operator = (const EST_Token &a)
{
    linenum = a.linenum;
    linepos = a.linepos;
    p_streamposition = a.p_streamposition;
    p_quoted = a.p_quoted;
    space = a.space;
    prepunc = a.prepunc;
    pname = a.pname;
    punc = a.punc;
    return *this;
}

const EST_String EST_Token::pos_description() const
{
    return "line "+itoString(linenum)+" char "+itoString(linepos);
}

EST_Token &EST_Token::operator = (const EST_String &a)
{
    pname = a;
    return *this;
}

EST_TokenTable::EST_TokenTable(const EST_String ws=EST_Token_Default_WhiteSpaceChars, \
                               const EST_String sc=EST_String::Empty, \
                               const EST_String ps=EST_String::Empty, \
                               const EST_String pps=EST_String::Empty, \
                               bool is_utf8=false)
{
	default_values(ws,sc,ps,pps,is_utf8);
	build_tables();	
}

void EST_TokenTable::default_values(const EST_String ws, \
                                    const EST_String sc, \
                                    const EST_String ps, \
                                    const EST_String pps, \
                                    bool is_utf8)
{
	p_WhiteSpaceChars = ws;
	p_SingleCharSymbols = sc;
	p_PunctuationSymbols = ps;
	p_PrePunctuationSymbols = pps;
	p_isutf8 = is_utf8;
	p_LUT_wrong = true;
	return;
}

TokenTableLUT::mapped_type EST_TokenTable::CheckCP(UnicodeChar cp)
{
	TokenTableLUT::iterator it;
	
	if (p_LUT_wrong == true) build_tables();
	it = p_LUT.find(cp);
	if ( it != p_LUT.end() ) return it->second;	
	else return '\0';
}

void EST_TokenTable::TwoClassWarn(UnicodeChar cp, TokenTableLUT::mapped_type oldclass,
                                  TokenTableLUT::mapped_type newclass)
{
    char utf8char[5];
    if (p_isutf8 == true)
    {
	cp2utf8(cp,utf8char);
	EST_warning("Character '%s' has two classes, '%c' and '%c'",
				utf8char, oldclass,newclass);
    } else
    {
	EST_warning("Character '%c' has two classes, '%c' and '%c'",
				(char) cp, oldclass,newclass);
    }
    
}

void EST_TokenTable::InsertCP(UnicodeChar cp, TokenTableLUT::mapped_type newclass) 
{
    TokenTableLUT::iterator iter =p_LUT.find(cp);
    TokenTableLUT::mapped_type oldclass;
    
    if (iter == p_LUT.end() )
	p_LUT[cp]=newclass;
    else
    {
	oldclass= iter->second;
	switch (newclass) {
	    case ' ': //WhiteSpace or SingleChar
	    case '@':
		TwoClassWarn(cp,oldclass,newclass);
		return;
	    case '.':  //Punctuation
		if (oldclass != '@')
		    TwoClassWarn(cp,oldclass,newclass);
		return;
	    case '$': //Prepunctuation
		if (oldclass == '@')
		    return;
		else if (oldclass == '.')
		    p_LUT[cp]='"';
		else
		    TwoClassWarn(cp,oldclass,newclass);
		return;
	}
    }
}

void EST_TokenTable::InsertCharSt(EST_String st, const char newclass)
{
	char *p;
	for (p=st; *p;++p) InsertCP((UnicodeChar) *p, newclass);
}

void EST_TokenTable::InsertUtf8St(EST_String st,const char newclass)
{
	utf8::iterator<char*> p(st,st,(char*) st+sizeof(st));
	for(;*p;++p) InsertCP(*p, newclass);
}

void EST_TokenTable::print_table()
{
    char utf8string[5];
  // use const_iterator to walk through elements of pairs
   for ( TokenTableLUT::const_iterator iter = p_LUT.begin();
      iter != p_LUT.end(); ++iter )
      {
      cp2utf8(iter->first,utf8string);
      std::cout << utf8string << '\t' << iter->second << std::endl;
      }
    
}

void EST_TokenTable::build_tables()
{
	p_LUT.clear();
	if(p_isutf8==false)
	{
		InsertCharSt(p_WhiteSpaceChars,' ');
		InsertCharSt(p_SingleCharSymbols,'@');
		InsertCharSt(p_PunctuationSymbols,'.');
		InsertCharSt(p_PrePunctuationSymbols,'$');
	}
	else
	{
		InsertUtf8St(p_WhiteSpaceChars,' ');
		InsertUtf8St(p_SingleCharSymbols,'@');
		InsertUtf8St(p_PunctuationSymbols,'.');
		InsertUtf8St(p_PrePunctuationSymbols,'$');
	}
	p_LUT_wrong=false;
	return;
}

UnicodeStr::UnicodeStr(bool unicode)
{
    p_isunicode=unicode;  
}

ostream& operator <<(ostream& s, UnicodeStr &p)
{
    EST_String t=(EST_String) p;
    s << t;
    return s;
}

UnicodeStr::operator EST_String ()
{
    EST_String s;
    
    for (std::vector<UnicodeChar>::iterator it=vec.begin();
	 it < vec.end() && *it != 0;
	 ++it)
    {
	append(*it,p_isunicode,s);
    }
    return s;
}


EST_String& operator <<(EST_String& s, UnicodeStr &p)
{
    s+=(EST_String) p;
    return s;
}

EST_TokenStream::EST_TokenStream()
{
    // Initial sizes
    tok_wspace.vec.reserve(64);
    tok_wspace.p_isunicode=false;
    tok_stuff.vec.reserve(512);
    tok_stuff.p_isunicode=false;
    tok_prepuncs.vec.reserve(32);
    tok_prepuncs.p_isunicode=false;
    default_values();
}

EST_TokenStream::EST_TokenStream(EST_TokenStream &s)
{
    (void)s;

    cerr << "TokenStream: warning passing TokenStream not as reference" 
	<< endl;

    // You *really* shouldn't use this AT ALL unless you
    // fully understand its consequences, you'll be copying open
    // files and moving file pointers all over the place
    // basically *DON'T* do this, pass the stream by reference

    // Now there may be occasions when you do want to do this for example
    // when you need to do far look ahead or check point as you read
    // but they are obscure and I'm not sure how to do that for all
    // the file forms supported by the TokenStream.  If you do
    // I can write a clone function that might do it.

}

void EST_TokenStream::default_values()
{
    type = tst_none;
    peeked_tokp = FALSE;
    peeked_charp = FALSE;
    quotes = FALSE;
    linepos = 1;  
    close_at_end=TRUE;
}

EST_TokenStream::~EST_TokenStream()
{
    if (type != tst_none) 
	close();
    
}

ostream& operator<<(ostream& s, EST_TokenStream &p)
{
    s << "[TOKENSTREAM ";
    switch (p.type)
    {
      case tst_none: 
	cerr << "UNSET"; break;
      case tst_file:
	cerr << "FILE"; break;
      case tst_pipe:
	cerr << "PIPE";	break;
      case tst_istream:
	cerr << "ISTREAM"; break;
      case tst_string:
	cerr << "STRING"; break;
      default:
	cerr << "UNKNOWN" << endl;
    }
    s << "]";
    
    return s;
}

int EST_TokenStream::open(const EST_String &filename)
{
    if (type != tst_none)
	close();
    default_values();
    is=new ifstream(filename,ios_base::in|ios_base::binary);
    if (is->fail())
    {
	std::cerr << "Cannot open file " << filename << " as tokenstream" 
	    << std::endl;
	return -1;
    }
    Origin = filename;
    type = tst_file;
    close_at_end=TRUE;
    return 0;
}

int EST_TokenStream::open(istream &newis)
{
    // absorb already open istream 
    if (type != tst_none)
	close();
    default_values();
    is = &newis;
    Origin = Token_Origin_Stream;
    type = tst_istream;
    close_at_end=FALSE;
    return 0;
}

int EST_TokenStream::open_string(const EST_String &newbuffer)
{
    // Make a tokenstream from an internal existing string/buffer
    if (type != tst_none)
	close();
    default_values();
    std::string tmpstring((const char*) newbuffer, newbuffer.length());
    is=new istringstream(tmpstring,stringstream::in|stringstream::binary);
    Origin = Token_Origin_String;
    type = tst_string;
    close_at_end=TRUE;
    return 0;
}

int EST_TokenStream::seek_end()
{
    // This isn't actually useful but people expect it 
    peeked_charp = FALSE;
    peeked_tokp = FALSE;

    switch (type)
    {
      case tst_none: 
	cerr << "EST_TokenStream unset" << endl;
	return -1;
	break;
      case tst_pipe:
	cerr << "EST_TokenStream seek_end on pipe not supported" << endl;
	return -1;
	break;
      case tst_file:
      case tst_istream:
      case tst_string:
        is->seekg(0,std::ios_base::end);
	return is->tellg();
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return -1;
    }

    return -1;  // can't get here 
}

int EST_TokenStream::seek(streampos position)
{
    switch (type)
    {
      case tst_none: 
	cerr << "EST_TokenStream unset" << endl;
	return -1;
	break;
      case tst_pipe:
	cerr << "EST_TokenStream seek on pipe with streampos not supported" << endl;
	return -1;
	break;
      case tst_file:
      case tst_istream:
      case tst_string:
        is->seekg(position);
	if (is->good()) return 0;
	else return -1;
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return -1;
    }
    
    return -1;  // can't get here 
}

int EST_TokenStream::fread(void *buff, int size, int nitems)
{
    // switching into binary mode for current position
      char *buf2=(char *) buff;
    // so we can continue to read afterwards
    if (peeked_tokp)
    {
	cerr << "ERROR " << pos_description() 
	    << " peeked into binary data" << endl;
	return 0;
    }

    peeked_charp = FALSE;
    peeked_tokp = FALSE;

    switch (type)
    {
      case tst_none: 
	cerr << "EST_TokenStream unset" << endl;
	return 0;
	break;
      case tst_pipe:
	cerr << "EST_TokenStream fread pipe not yet supported" << endl;
	return 0;
	break;
      case tst_file:
      case tst_istream:
      case tst_string:
	for (int i=0;
	     i<nitems;
	     ++i)
	{
	    is->read(buf2,size);
	    buf2+=is->gcount();
	}
	
        if (is->good() == false )
	{
	    std::cerr << "EST_TokenStream fread error" << std::endl;
	    return 0;
	}
	else
	    return nitems;
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return -1;
	break;
    }

    return 0;  // can't get here 

}
    
void EST_TokenStream::close(void)
{
    // close any files (if they were used)
    
    switch (type)
    {
      case tst_none: 
	break;
      case tst_pipe:
	// close(fd);
	break;
      case tst_file:
      case tst_istream:
      case tst_string:
        if (close_at_end == TRUE)
	{
	    delete is;
	    is=NULL;
	}
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	break;
    }

    type = tst_none;
    peeked_charp = FALSE;
    peeked_tokp = FALSE;

}

int EST_TokenStream::restart(void)
{
    // For paul, the only person I know who uses this
    
    switch (type)
    {
      case tst_none: 
	break;
      case tst_pipe:
	cerr << "EST_TokenStream: can't rewind pipe" << endl;
	return -1;
	break;
      case tst_file:
      case tst_istream:
      case tst_string:
        is->seekg(0,ios_base::beg);
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	break;
    }

    linepos = 1;
    peeked_charp = FALSE;
    peeked_tokp = FALSE;

    return 0;
}

EST_TokenStream & EST_TokenStream::operator >>(EST_Token &p)
{
    return get(p);
}
 
EST_TokenStream & EST_TokenStream::operator >>(EST_String &p)
{
    EST_Token t;

    get(t);
    p = t.string();
    return *this;
}

EST_TokenStream &EST_TokenStream::get(EST_Token &tok)
{
    tok = get();
    return *this;
}

EST_Token EST_TokenStream::get_upto(const EST_String &s)
{
    // Returns a concatenated token form here to next symbol that matches s
    // including s (though not adding s on the result)
    // Not really for the purist but lots of times very handy
    // Note this is not very efficient
    EST_String result;
    EST_Token t;

    for (result=EST_String::Empty; (t=get()) != s; )
    {
	result += t.whitespace() + t.prepunctuation() +
	    t.string() + t.punctuation();
	if (eof())
	{
	    cerr << "EST_TokenStream: end of file when looking for \"" <<
		s << "\"" << endl;
	    break;
	}
    }

    return EST_Token(result);
}

EST_Token EST_TokenStream::get_upto_eoln(void)
{
    // Swallow the lot up to end of line 
    // assumes \n is a whitespace character

    EST_String result(EST_String::Empty);

    while (!eoln())
    {
	EST_Token &t=get();
	result += t.whitespace() + t.prepunctuation();

	if (quotes)
	    result += quote_string(t.string());
	else
	    result += t.string();

	result += t.punctuation();

	if (eof())
	{
//	    cerr << "EST_TokenStream: end of file when looking for end of line"
//		<< endl;
	    break;
	}
    }
    // So that the next call works I have to step over the eoln condition
    // That involves removing the whitespace upto and including the next 
    // \n in the peek token.

    char *w = wstrdup(peek().whitespace());
    int i;
    for (i=0; w[i] != 0; i++)
	if (w[i] == '\n')   // maybe not portable 
	    peek().set_whitespace(&w[i+1]);

    wfree(w);

    static EST_Token result_t;

    result_t.set_token(result);

    return result_t;
}

EST_Token &EST_TokenStream::must_get(EST_String expected, bool *ok)
{
    EST_Token &tok = get();

    if (tok != expected)
    {
        if (ok != NULL)
        {
            *ok=FALSE;
            return tok;
        }
        else
            EST_error("Expected '%s' got '%s' at %s", 
                      (const char *)expected, 
                      (const char *)(EST_String)tok,
                      (const char *)pos_description());
    }

    if (ok != NULL)
        *ok=TRUE;
    return tok;
}

inline UnicodeChar  EST_TokenStream::getpeeked_internal(void)
{
  peeked_charp = FALSE;
  return peeked_char;
}


inline
UnicodeChar EST_TokenStream::getch_internal()
{
    UnicodeChar cp;
    // Return next character in stream
    if (EST_TokenStream::peeked_charp)
    {
      return getpeeked_internal();
    }
    
    switch (type)
    {
      case tst_none: 
	cerr << "EST_TokenStream unset" << endl;
	return ErrUnicodeChar;
	break;
      case tst_pipe:
	cerr << "EST_TokenStream pipe not yet supported" << endl;
	return ErrUnicodeChar;
	break;
      case tst_file:
      case tst_istream:
      case tst_string:
	if (p_table.isutf8() ==false)
	{
	    return (is->good()) ? is->get() : ErrUnicodeChar;
	} else
	{
		istreambuf_iterator<char> it(is->rdbuf());
		istreambuf_iterator<char> eos;
		if (it != eos) {
			cp = utf8::next(it,eos);
			return cp;
		} else {
			return ErrUnicodeChar;
		}
	}
        break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return ErrUnicodeChar;
    }

    return ErrUnicodeChar;  // can't get here 
}

UnicodeChar EST_TokenStream::getch(void)
{
  return getch_internal();
}

inline UnicodeChar  EST_TokenStream::peekch_internal()
{
    // Return next character in stream (without reading it)

    if (!peeked_charp)
	peeked_char = getch_internal();
    peeked_charp = TRUE;
    return peeked_char;
}


UnicodeChar  EST_TokenStream::peekch(void)
{
  return peekch_internal();
  
}

#define CLASS(C,CL) (p_table.CheckCP(C)==(CL))

#define CLASS2(C,CL1,CL2) (p_table.CheckCP(C)==(CL1)||p_table.CheckCP(C)==(CL2))


EST_Token &EST_TokenStream::get(void)
{
    if (peeked_tokp)
    {
		peeked_tokp = FALSE;
		return current_tok;
    }

    streampos tmpposition;
    std::vector<UnicodeChar> temp;
    UnicodeStr temp2;
    std::vector<UnicodeChar>::iterator it,endit,word,it2;
    UnicodeChar c;
    
    tok_wspace.vec.clear();
    tok_stuff.vec.clear();
    tok_prepuncs.vec.clear();
    tok_punc.vec.clear();

    tmpposition=tell();
    c=getch_internal();
    while(CLASS(c,' ') && ( ! eof() ) && (c!=ErrUnicodeChar) )
    {
	if (c == '\n') linepos++;
	tok_wspace.vec.push_back(c);
	tmpposition=tell();
	c=getch_internal();
    }
    current_tok.init();
    current_tok.set_streampos(tmpposition);
    if ( ! eof() )
    {
	if ((quotes) &&  // quoted strings (with escapes) are allowed
	    (c == quote))
	{
	    while ( (c = getch_internal()), ! eof() )
	    {
		if (c == quote)
		    break;
		if (c == escape)
		    c = getch_internal();
		tok_stuff.vec.push_back(c);
	    }
	    current_tok.set_quoted(TRUE);
	}
	else            // standard whitespace separated tokens
	{
	    tok_stuff.vec.push_back(c);
	    while(
		  !CLASS(c,'@') &&
		  !CLASS(c=peekch_internal(),' ') && 
		  !CLASS(c,'@') &&
		  ( ! eof() ))
	    {
		// note, we must have peeked to get here.
		tok_stuff.vec.push_back(getpeeked_internal());
	    }
	}
	
	// Are there any punctuation symbols at the start?
	it=tok_stuff.vec.begin();
	
	for (;
	    ((it < tok_stuff.vec.end()) && CLASS2(*it, '$', '"') && *it!=ErrUnicodeChar);
	    ++it);
	
	if ( (it > tok_stuff.vec.begin() ) && (it < tok_stuff.vec.end()))  // there are
	{
	    tok_prepuncs.vec.insert(tok_prepuncs.vec.end(),tok_stuff.vec.begin(),it);
	    current_tok.set_prepunctuation((EST_String) tok_prepuncs);
	    word=it;
	}
	else
	{
	    current_tok.set_prepunctuation(EST_String::Empty);
	    word = tok_stuff.vec.begin();
	}

	// Are there any punctuation symbols at the end
	
	for (it=tok_stuff.vec.end()-1; 
	     ((it != word) && CLASS2(*it,'.','"'));
	     --it);
	++it;
	if (it < tok_stuff.vec.end()) // There are
	{
	    tok_punc.vec.insert(tok_punc.vec.end(),it,tok_stuff.vec.end());
	    current_tok.set_punctuation((EST_String) tok_punc);

	}
	else
	{
	    current_tok.set_punctuation(EST_String::Empty);
	}

	temp2.vec.assign(word,it);
	temp2.p_isunicode=tok_stuff.p_isunicode;
	current_tok.set_token((EST_String) temp2);
	
	if (*(tok_wspace.vec.begin()) == '\0') // feature paths will have null whitespace
	    current_tok.set_whitespace(EST_String::Empty);
	else
	    current_tok.set_whitespace((EST_String) tok_wspace);
    }
    else
    {
	current_tok.set_token(EST_String::Empty);
	current_tok.set_whitespace((EST_String) tok_wspace);
	current_tok.set_punctuation(EST_String::Empty);
	current_tok.set_prepunctuation(EST_String::Empty);
    }

    return current_tok;
}

int EST_TokenStream::eoln(void)
{
    // This doesn't really work if there are blank lines (and you want
    // to know about them)

    if ((peek().whitespace().contains("\n")) ||	eof())
	return TRUE;
    else
	return FALSE;

}

EST_String quote_string(const EST_String &s,
			const EST_String &quote, 
			const EST_String &escape, 
			int force)
{
    // Quotes s always if force true, or iff s contains whitespace,
    // quotes or escapes force is false
    // Note quote and escape are assumed to be string of length 1
    EST_String quoted_form;
    if ((force) || 
	(s.contains(quote)) ||
	(s.contains(escape)) ||
	(s.contains(RXanywhitespace)) ||
	(s.length() == 0))
    {
	// bigger than the quoted form could ever be
	int i,j;
	char *quoted = new char[s.length()*(quote.length()+escape.length())+
		       1+quote.length()+quote.length()];
	quoted[0] = quote(0);
	for (i=1,j=0; j < s.length(); j++,i++)
	{
	    if (s(j) == quote(0))
		quoted[i++] = escape(0);
	    else if (s(j) == escape(0))
		quoted[i++] = escape(0);
	    quoted[i] = s(j);
	}
	quoted[i++] = quote(0);
	quoted[i] = '\0';
	quoted_form = quoted;
	delete [] quoted;
	return quoted_form;
    }
    else 
      return s;
}

const EST_String EST_TokenStream::pos_description()
{
    return Origin+":"+itoString(linepos);
}
}
