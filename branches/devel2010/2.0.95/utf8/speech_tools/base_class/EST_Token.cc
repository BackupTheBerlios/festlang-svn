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
#include "EST_math.h"
#include "EST_Token.h"
#include "EST_string_aux.h"
#include "EST_cutils.h"
#include "EST_error.h"
#include "EST_utf8.h"
using namespace std;


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

static inline char *check_extend_str_in(char *str, int pos, int *max)
{
    // Check we are not at the end of the string, if so get some more
    // and copy the old one into the new one
    char *newstuff;
    
    if (pos >= *max - 4) // If we are close to the end (a UTF-8 char may be up to 4 bytes)
    {
	if (*max <= 4) *max=4; // Let's have space for at least one UTF-8 char!
	*max *= 2;
	newstuff = new char[*max];
	strncpy(newstuff,str,pos);
	delete [] str;
	return newstuff;
    }
    else 
	return str;
}

#define check_extend_str(STR, POS, MAX) \
	(((POS)== *(MAX)-4)?check_extend_str_in((STR),(POS),(MAX)):(STR))

ostream& operator<<(ostream& s, const EST_Token &p)
{
    s << "[TOKEN " << p.pname << "]";
    return s;
}


EST_Token &EST_Token::operator = (const EST_Token &a)
{
    linenum = a.linenum;
    linepos = a.linepos;
    p_filepos = a.p_filepos;
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
	p_table_wrong = false;
	return;
}

char EST_TokenTable::CheckCP(utf8::uint32_t cp)
{
	TokenTableLUT::iterator it;
	
	if (p_table_wrong == 1) build_tables();
	it = p_LUT.find(cp);
	if ( it != p_LUT.end() ) return it->second;	
	else return '\0';
}

void EST_TokenTable::TwoClassWarn(utf8::uint32_t cp, TokenTableLUT::mapped_type oldclass, TokenTableLUT::mapped_type newclass)
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

void EST_TokenTable::InsertCP(utf8::uint32_t cp, TokenTableLUT::mapped_type newclass) 
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
	for (p=st; *p;++p) InsertCP((utf8::uint32_t) *p, newclass);
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
      cout << utf8string << '\t' << iter->second << '\n';
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
	p_table_wrong=0;
	return;
}

EST_TokenStream::EST_TokenStream()
{
    tok_wspacelen = 64;  // will grow if necessary
    tok_wspace = new char[tok_wspacelen];
    tok_stufflen = 512;  // will grow if necessary
    tok_stuff = new char[tok_stufflen];
    tok_prepuncslen = 32;  // will grow if necessary
    tok_prepuncs = new char[tok_prepuncslen];

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
    eof_flag = FALSE;
    quotes = FALSE;
    p_filepos = 0;
    linepos = 1;  
    close_at_end=TRUE;
}

EST_TokenStream::~EST_TokenStream()
{
    if (type != tst_none) 
	close();
    delete [] tok_wspace;
    delete [] tok_stuff;
    delete [] tok_prepuncs;
    
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
    fp = fopen(filename,"rb");
    if (fp == NULL)
    {
	cerr << "Cannot open file " << filename << " as tokenstream" 
	    << endl;
	return -1;
    }
    Origin = filename;
    type = tst_file;

    return 0;
}

int EST_TokenStream::open(FILE *ofp, int close_when_finished)
{
    // absorb already open stream
    if (type != tst_none)
	close();
    default_values();
    fp = ofp;
    if (fp == NULL)
    {
	cerr << "Cannot absorb NULL filestream as tokenstream" << endl;
	return -1;
    }
    Origin = Token_Origin_FD;
    type = tst_file;
    
    close_at_end = close_when_finished;
    
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

    return 0;
}

int EST_TokenStream::open_string(const EST_String &newbuffer)
{
    // Make a tokenstream from an internal existing string/buffer
    const char *buf;
    if (type != tst_none)
	close();
    default_values();
    buf = (const char *)newbuffer;
    buffer_length = newbuffer.length();
    buffer = new char[buffer_length+1];
    memmove(buffer,buf,buffer_length+1);
    pos = 0;
    Origin = Token_Origin_String;
    type = tst_string;

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
      case tst_file:
	fseek(fp,0,SEEK_END);
	p_filepos = ftell(fp);
	return p_filepos;
      case tst_pipe:
	cerr << "EST_TokenStream seek on pipe not supported" << endl;
	return -1;
	break;
      case tst_istream:
	cerr << "EST_TokenStream seek on istream not yet supported" << endl;
	return -1;
	break;
      case tst_string:
	pos = buffer_length;
	return pos;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return -1;
    }

    return -1;  // can't get here 
}

int EST_TokenStream::seek(int position)
{
    peeked_charp = FALSE;
    peeked_tokp = FALSE;

    switch (type)
    {
      case tst_none: 
	cerr << "EST_TokenStream unset" << endl;
	return -1;
	break;
      case tst_file:
	p_filepos = position;
	return fseek(fp,position,SEEK_SET);
      case tst_pipe:
	cerr << "EST_TokenStream seek on pipe not supported" << endl;
	return -1;
	break;
      case tst_istream:
	cerr << "EST_TokenStream seek on istream not yet supported" << endl;
	return -1;
	break;
      case tst_string:
	if (position >= pos)
	{
	    pos = position;
	    return -1;
	}
	else
	{
	    pos = position;
	    return 0;
	}
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return -1;
    }

    return -1;  // can't get here 

}

static int stdio_fread(void *buff,int size,int nitems,FILE *fp)
{
    // So it can find the stdio one rather than the TokenStream one
    return fread(buff,size,nitems,fp);
}

int EST_TokenStream::fread(void *buff, int size, int nitems)
{
    // switching into binary mode for current position
    int items_read;

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
      case tst_file:
	items_read = stdio_fread(buff,(size_t)size,(size_t)nitems,fp);
	p_filepos += items_read*size;
	return items_read;
      case tst_pipe:
	cerr << "EST_TokenStream fread pipe not yet supported" << endl;
	return 0;
	break;
      case tst_istream:
    is->read((char*)buff,size*nitems);
    if (is->eof() == true ) return 0;
	return nitems;
      case tst_string:
	if ((buffer_length-pos)/size < nitems)
	    items_read = (buffer_length-pos)/size;
	else
	    items_read = nitems;
	memcpy(buff,&buffer[pos],items_read*size);
	pos += items_read*size;
	return items_read;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return EOF;
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
      case tst_file:
	if (close_at_end)
	  fclose(fp);
      case tst_pipe:
	// close(fd);
	break;
      case tst_istream:
	break;
      case tst_string:
	delete [] buffer;
	buffer = 0;
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
      case tst_file:
        fp = freopen(Origin,"rb",fp);
	p_filepos = 0;
	break;
      case tst_pipe:
	cerr << "EST_TokenStream: can't rewind pipe" << endl;
	return -1;
	break;
      case tst_istream:
	cerr << "EST_TokenStream: can't rewind istream" << endl;
	break;
      case tst_string:
	pos = 0;
	break;
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	break;
    }

    linepos = 1;
    peeked_charp = FALSE;
    peeked_tokp = FALSE;
    eof_flag = FALSE;

    return 0;
}

int EST_TokenStream::append (utf8::uint32_t cp,char *here)
{
    if (p_table.isutf8() ==false)
    {
	    *here=(char) cp;
	    return 1;
    } else
	    return utf8::append(cp,here)-here;
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

inline utf8::uint32_t  EST_TokenStream::getpeeked_internal(void)
{
  peeked_charp = FALSE;
  return peeked_char;
}

inline
utf8::uint32_t EST_TokenStream::getch_internal()
{
    utf8::uint32_t cp;
    // Return next character in stream
    if (EST_TokenStream::peeked_charp)
    {
      return getpeeked_internal();
    }
    
    switch (type)
    {
      case tst_none: 
	cerr << "EST_TokenStream unset" << endl;
	return EOF;
	break;
      case tst_file:
	{
		if (p_table.isutf8() == false)
		{
			p_filepos++;
			unsigned char lc;
			if (stdio_fread(&lc,1,1,fp) == 0) {
				eof_flag = TRUE;
				return 0;
			}
			else
				return (utf8::uint32_t)lc;
		} else
		{
			//The UTF-8 library uses streams.
			char buffer[5]="\0\0\0\0";
			char *p=buffer;
			for (int i=0;i<4;++i)
			{
				p_filepos++;
				if (stdio_fread(&buffer+i,1,1,fp) == 0) {
					eof_flag=TRUE;
					return EOF;
				}
				if (utf8::is_valid(p,p+i+1))
					return utf8::unchecked::next(p);
			}
			return 0xFFFD; // invalid UTF-8, returning replacement char.
			cerr << "EST_TokenStream had invalid UTF-8 content" << endl;
		}
	}
/*	return getc(fp); */
      case tst_pipe:
	cerr << "EST_TokenStream pipe not yet supported" << endl;
	return EOF;
	break;
      case tst_istream:

	if (p_table.isutf8() ==false)
	{
		p_filepos++;
		return is->get();
	} else
	{
		istreambuf_iterator<char> it(is->rdbuf());
		istreambuf_iterator<char> eos;
		if (it != eos) {
			cp = utf8::next(it,eos);
			p_filepos+=is->gcount();
			return cp;
		} else {
			eof_flag=TRUE;
			return EOF;
		}
	}

      case tst_string:
	if (pos < buffer_length)
	{
	    int bytesread;
	    bytesread=getnextcp(&(buffer[pos]),p_table.isutf8(),cp);
	    p_filepos+=bytesread;
	    pos+=bytesread;
	    return cp;
	}
	else
	{
	    eof_flag=TRUE;
	    return EOF;
	}
	    
	    
      default:
	cerr << "EST_TokenStream: unknown type" << endl;
	return EOF;
    }

    return EOF;  // can't get here 
}

utf8::uint32_t EST_TokenStream::getch(void)
{
  return getch_internal();
}

inline utf8::uint32_t  EST_TokenStream::peekch_internal()
{
    // Return next character in stream (without reading it)

    if (!peeked_charp)
	peeked_char = getch_internal();
    peeked_charp = TRUE;
    return peeked_char;
}


utf8::uint32_t  EST_TokenStream::peekch(void)
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

    char *word;
    int i,j,k;
    utf8::uint32_t c;

    for (i=k=0;
         (CLASS(c=getch_internal(),' ') && 
	       ( ! eof_flag ));)
    {
		if (c == '\n') linepos++;
		tok_wspace = check_extend_str(tok_wspace,i,&tok_wspacelen);	
		k=append(c,&(tok_wspace[i]));
		i+=k;
    }
    tok_wspace[i] = '\0';

    current_tok.init();

    if ( ! eof_flag )
    {
	current_tok.set_filepos(p_filepos-k);

	if ((quotes) &&  // quoted strings (with escapes) are allowed
	    (c == quote))
	{
	    for (i=0;
		 ((c = getch_internal()), ! eof_flag)
		 ;)
	    {
			if (c == quote)
				break;
			tok_stuff = check_extend_str(tok_stuff,i,&tok_stufflen);
			if (c == escape)
				c = getch_internal();
			i+=append(c,&(tok_stuff[i]));
	    }
	    current_tok.set_quoted(TRUE);
	}
	else            // standard whitespace separated tokens
	{
	    for (i=0,i+=append(c,&(tok_stuff[i])); 
		 (
		  !CLASS(c,'@') &&
		  !CLASS(c=peekch_internal(),' ') && 
		  !CLASS(c,'@') &&
		  ( ! eof_flag )) ;)
	    {
		tok_stuff = check_extend_str(tok_stuff,i,&tok_stufflen);
		// note, we must have peeked to get here.
		i+=append(getpeeked_internal(),&(tok_stuff[i]));
	    }
	}
	tok_stuff[i] = '\0';
	// Are there any punctuation symbols at the start?
	for (j=0, k=getnextcp(&(tok_stuff[j]),p_table.isutf8(),c); 
	     ((j < i) && CLASS2(c, '$', '"') && c!=0);
	     j+=k,k=getnextcp(&(tok_stuff[j]),p_table.isutf8(),c));
	if ((j > 0) && (j < i))  // there are
	{
	    tok_prepuncs = check_extend_str(tok_prepuncs,j+1,&tok_prepuncslen);
	    memmove(tok_prepuncs,tok_stuff,j);
	    tok_prepuncs[j] = '\0';
	    current_tok.set_prepunctuation(tok_prepuncs);
	    word=&tok_stuff[j];
	    i-=j;  // reduce size by number of prepuncs
	}
	else
	{
	    current_tok.set_prepunctuation(EST_String::Empty);
	    word = tok_stuff;
	}
	// Are there any punctuation symbols at the end
	for (j=i, k=getprevcp(word,&(word[j]),p_table.isutf8(),c);
	     ((j > 0) && CLASS2(c,'.','"') && k!=0);
	     j-=k, k=getprevcp(word,&(word[j]),p_table.isutf8(),c));
	if (word[j] != '\0')
	{
	    current_tok.set_punctuation(&word[j]);
	    word[j] = '\0';
	}
	else
	    current_tok.set_punctuation(EST_String::Empty);
	    
	current_tok.set_token(word);
	if (tok_wspace[0] == '\0') // feature paths will have null whitespace
	    current_tok.set_whitespace(EST_String::Empty);
	else
	    current_tok.set_whitespace(tok_wspace);
    }
    else
    {
	current_tok.set_token(EST_String::Empty);
	current_tok.set_whitespace(tok_wspace);
	current_tok.set_punctuation(EST_String::Empty);
	current_tok.set_prepunctuation(EST_String::Empty);
	eof_flag = TRUE;
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
