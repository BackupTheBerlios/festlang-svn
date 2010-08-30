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

#include "inst_tmpl/token.cc"
using namespace std;

namespace EST
{


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
	if (p_LUT_wrong == true) build_tables();

	TokenTableLUT::iterator it = p_LUT.find(cp);
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
	    default: // Other classes, overwrite and warn
		p_LUT[cp]=newclass;
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
	InsertCP(ErrUnicodeChar,'!');
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
    
    if (vec.empty() == true ) return s;
    
    for (std::vector<UnicodeChar>::iterator it=vec.begin();
	 it < vec.end() && *it != ErrUnicodeChar;
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
    peeked_char.clear();
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
    
    if (nitems == 0) return 0;
    
    if (peeked_tokp)
    {
	cerr << "ERROR " << pos_description() 
	    << " peeked into binary data" << endl;
	return 0;
    }
    
    if(is_utf8() == true)
    {
	cerr << "ERROR: EST_TokenStream can't read binary data " <<
	        "from an UTF-8 TokenStream" << endl;
    }

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
        // We have to deal with unget characters:
	for (int i=0;
	     i<nitems;
	     ++i)
	{
	    int alreadyread=0;
	    while (peeked_char.empty() == false && alreadyread<size)
	    {
		if ( peeked_char.front() == ErrUnicodeChar)
		{
		    is->setstate(ios::eofbit);
		    peeked_char.pop_front();
		    break;
		}
		else
		{
		    *buf2++=(char) peeked_char.front();
		    peeked_char.pop_front();
		    ++alreadyread;
		}
	    }
	    
	    is->read(buf2,size-alreadyread);
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
    peeked_char.clear();
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
    peeked_char.clear();
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
  UnicodeChar cp=peeked_char.front();
  peeked_char.pop_front();
  return cp;
}


inline
UnicodeChar EST_TokenStream::getch_internal()
{
    // Gets the next Code Point or char from the stream.
    UnicodeChar cp;
        
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
	    int ch;
	    if ( (is->good()) && ( (ch=is->get()) != EOF ) )
		return ch;
	    else
		return ErrUnicodeChar;
	} else
	{
		std::istreambuf_iterator<char> it (*is);
		std::istreambuf_iterator<char> eos;
		if (it != eos) {
			cp = utf8::next(it,eos);
			if (it == eos) is->setstate(ios::eofbit);
			return cp;
		} else {
			if (it == eos) is->setstate(ios::eofbit);
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
  UnicodeChar cp;
  
  if (peeked_char.empty() == false) cp=getpeeked_internal();
  else  cp=getch_internal();
  
  if (cp == '\n') ++linepos;
  
  return cp;
}

inline UnicodeChar  EST_TokenStream::peekch_internal()
{
    // Return next character in stream (without reading it)
    if (peeked_char.empty() == true )
	peeked_char.push_back(getch_internal());
    return peeked_char.front();
}


UnicodeChar  EST_TokenStream::peekch()
{
  return peekch_internal();
}

inline 
void EST_TokenStream::ungetch(UnicodeChar cp, bool escaping=false)
{
    if (cp == '\n') --linepos;
    peeked_char.push_front(cp);
    if (escaping==true) ungetch(escape);
}

#define CLASS(C,CL) (p_table.CheckCP(C)==(CL))

#define CLASS2(C,CL1,CL2) (p_table.CheckCP(C)==(CL1)||p_table.CheckCP(C)==(CL2))


int EST_TokenStream::clear_tok_buffers(void)
{
    tok_wspace.vec.clear();
    tok_stuff.vec.clear();
    tok_prepuncs.vec.clear();
    tok_punc.vec.clear();

    return 0;
}

int EST_TokenStream::get_tok_finish(void)
{
//    cout << "debug: Finishing token" << endl;
    current_tok.set_whitespace((EST_String) tok_wspace);
    current_tok.set_token((EST_String) tok_stuff);
    current_tok.set_punctuation((EST_String) tok_punc);
    current_tok.set_prepunctuation((EST_String) tok_prepuncs);
    return 0;
}

int EST_TokenStream::get_tok_wspace()
{
    streampos tmpposition=tell();
    bool goon=true;
    char classCP;
    UnicodeChar c;
    
    while(goon)
    {
	c=getch();
//	cout << "debug tokwspace: |" << (char) c << "|" << endl;
	switch ( classCP=p_table.CheckCP(c) )
	{
	  case ' ': 
	    tok_wspace.vec.push_back(c);
	    tmpposition=tell();
	    break;
	  case '!':
	    // End of file reached. 
	  default :
	    goon=false;
	    break;
	}
    }
//    cout << "debug: parentesis:" << (char) c << endl;
    ungetch(c);
//    cout << "debug: test1: |" << (EST_String) tok_wspace << "|" << endl;
    
    if (tok_wspace.vec.front() == '\0') // feature paths will have null whitespace
	tok_wspace.vec.clear();
//    cout << "debug: test2: |" << (EST_String) tok_wspace << "|" << endl;
    current_tok.set_streampos(tmpposition);
    
    if ( classCP == '!')
    {
	is->setstate(ios::eofbit); // Just in case.
	get_tok_finish();
	return -1;
    }
    
    return 0;
}

int EST_TokenStream::get_tok_prepunct()
{
    bool goon=true;
    char classCP;
    UnicodeChar c;
    bool escaping;

    c=peekch();
//    cout << "debug tokwprepunct peek: " << (char) c << endl;
    if ( (quotes) && (c==quote))
    {
	getch();
	current_tok.set_quoted(TRUE);
    }
    
    if ( (current_tok.quoted() == FALSE) && p_table.CheckCP(c) == '@')
    {
//	cout << "debug: parentesis" << (char) c << endl; 
	getch();
	tok_stuff.vec.push_back(c);
	get_tok_finish();
	return -1;
    }
    
    while(goon)
    {
	c=getch();
//    	cout << "debug tok_prepunct: " << (char) c << endl;	
	if ( (current_tok.quoted() == TRUE ) && (c==quote) )
	{
	    get_tok_finish();
	    return -1;
	}

	if ( (current_tok.quoted() == TRUE ) && (c==escape) )
	{
	    c=getch();
	    escaping=true;
	}
	else
	    escaping=false;
	
	switch ( classCP=p_table.CheckCP(c) )
	{
	  case '$': // Prepunct
	  case '"': // defined both prepunct and punct.
	    tok_prepuncs.vec.push_back(c);
	    break;
	  case ' ': // Ignore whitespace between prepunct and string.
	    break;
	  case '@': // SingleChar
	    if ( current_tok.quoted() == TRUE )
	    {
		goon=false;
		break;
	    }
	    else
	    {
		tok_stuff.vec.push_back(c);
		get_tok_finish();
		return -1;
	    }
	    break;
	  case '!':
	    // End of file reached. 
	  default :
	    goon=false;
	    break;
	}
    }
    ungetch(c,escaping);
    
    if ( classCP == '!')
    {
	is->setstate(ios::eofbit); // Just in case.
	get_tok_finish();
	return -1;
    }
    
    return 0;
}

int EST_TokenStream::mv_stuff_punc()
{
    UnicodeChar c;
    // Check if the last tok_stuff character is a punctuation&prepunct
    // Or prepunctuation, and move it to tok_punc if necessary.
    for (c=tok_stuff.vec.back();
	 CLASS2(c,'$','"');
	 c=tok_stuff.vec.back()
	)
    {
	tok_stuff.vec.pop_back();
//	cout << "debug: popping " << (char) c << endl;
	tok_punc.vec.push_back(c);
    }
    return 0;
}

int EST_TokenStream::get_tok_string()
{
    bool goon=true;
    char classCP;
    UnicodeChar c;
    bool escaping;

    c=peekch();
//    cout << "debug tok_string peekch: " << (char) c << endl;
    if ( (current_tok.quoted() == FALSE) && p_table.CheckCP(c) == '@')
    {
	getch();
	tok_stuff.vec.push_back(c);
	get_tok_finish();
	return -1;
    }
    
    while(goon)
    {
//	cout << "debug: tok_stuff: |" << (EST_String) tok_stuff << "|" << endl;
	c=getch();
//	cout << "debug tok_string: " << c << endl;
	if ( (current_tok.quoted() == TRUE) && (c==quote))
	{
	    get_tok_finish();
	    return -1;
	}
	
	if ( (current_tok.quoted() == TRUE ) && (c==escape) )
	{
	    c=getch();
	    escaping=true;
	}
	else
	    escaping=false;
	
	switch ( classCP=p_table.CheckCP(c) )
	{
	  case '@':
	    if (current_tok.quoted() == TRUE )
	    {
		tok_stuff.vec.push_back(c);
		break;
	    }
	    else
	    {
		tok_stuff.vec.push_back(c);
		get_tok_finish();
		return -1;
	    }
	    break;
	  case '\0':
	  case '$': // Prepunct // Apostrophes can be in the middle.
	  case '"': // defined both prepunct and punct. // Apostrophe...
	    tok_stuff.vec.push_back(c);
	    break;
	  case ' ': // WhiteSpaces are ignored on quoted Tokens.
	    if ( (current_tok.quoted() == TRUE ) ) 
	    {
		tok_stuff.vec.push_back(c);
		break;
	    }
	    else
	    {
		goon=false;
		break;
	    }
	  case '.': // Punctuation
	    if ( (current_tok.quoted() == TRUE ) ) 
	    {
		tok_stuff.vec.push_back(c);
		break;
	    }
	  case '!':
//	    cout << "debug : I see the end" << endl;
	    // End of file reached. 
	  default :
	    goon=false;
	    break;
	}
	if (p_table.CheckCP(peekch()) == '@')
	{
	    if (classCP == ' ' || classCP == '$') ungetch(c,escaping);
	    mv_stuff_punc();
	    get_tok_finish();
	    return -1;
	}
    }
    ungetch(c,escaping);
//    cout << "debug: tok_stuff: |" << (EST_String) tok_stuff << "|" << endl;

    mv_stuff_punc();
    
    if ( classCP == '!')
    {
//	cout << "debug: end of file" << endl;
	is->setstate(ios::eofbit); // Just in case.
	get_tok_finish();
	return -1;
    }
    
    return 0;
}

int EST_TokenStream::get_tok_punc()
{
    bool goon=true;
    char classCP;
    UnicodeChar c;
    std::vector<UnicodeChar> temp;

    /* There are many different cases:
     * For "punctuation" characters that are not defined as "prepunctuation":
     * "a token here" -> token does not have punc
     * "a token, here" -> token has "," as punc
     * "a token , here" -> token has a space that we have to ignore and punc
     * "a token ,here" -> needs fixing also.
     * 
     * For characters defined both in punc and prepunc:
     * "a 'token' here" -> token has prepunct=' and punc='
     * "a ' token ' here -> token has prepunct=' and we can't know for sure if the second
     *   "'" belongs to "token" or to "here".
     */
    while(goon)
    {
	c=getch();
//        cout << "debug tok_punc first: " << (char) c << endl;
	switch ( classCP=p_table.CheckCP(c) )
	{
	  case '.': // Punctuation
	  case '"': // defined both prepunct and punct.
	     tok_punc.vec.push_back(c);
	     break;

	  case ' ': // WhiteSpace.
	    goon=false;
	    break;
	  case '!': // End of file reached. 
	    is->setstate(ios::eofbit); // Just in case.
	    get_tok_finish();
	    return -1;
	    break;
	  case '\0': // Regular character
	  case '$': // Prepunct
	  default:
	    ungetch(c);
	    get_tok_finish();
	    return -1;
	    break;
	}
    }
    temp.push_back(c);
    
    goon=true;
    while(goon)
    {
	c=getch();
//        cout << "debug tok_punc second: " << (char) c << endl;
	switch ( classCP=p_table.CheckCP(c) )
	{
	  case '.': // Punctuation
	     tok_punc.vec.push_back(c);
	     temp.clear(); // If there is a punc character, all the spaces before are ignored.
	     break;
	  case ' ': // WhiteSpace.
	    temp.push_back(c);
	    break;
	  case '!': // End of file reached. 
	    temp.push_back(c);
	    goon=false;
	    break;
	  case '@': // A SingleChar belongs to the next token.
	  case '\0': // Regular character
	  case '$': // Prepunct
	  case '"': // defined both prepunct and punct.
	  default:
	    temp.push_back(c);
	    goon=false;
	    break;
	}
    }
    
    while(temp.empty()==false)
    {
	c=temp.back();
	temp.pop_back();
	ungetch(c);
//	cout << "debug ungetting: |" << (char) c << "|" << endl;
    }
	
    
    if ( p_table.CheckCP(c) == '!')
    {
	is->setstate(ios::eofbit); // Just in case.
	get_tok_finish();
	return -1;
    }
    
    return 0;
}


EST_Token &EST_TokenStream::get(void)
{
    if (peeked_tokp)
    {
	peeked_tokp = FALSE;
	return current_tok;
    }

    clear_tok_buffers();
    current_tok.init();
    
    if (get_tok_wspace() !=0) return current_tok;
    if (get_tok_prepunct() != 0) return current_tok;
    if (get_tok_string() != 0 ) return current_tok;
    if (get_tok_punc() != 0 ) return current_tok;
    
    get_tok_finish();
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
