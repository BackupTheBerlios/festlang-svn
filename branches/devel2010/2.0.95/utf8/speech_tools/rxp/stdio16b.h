/*************************************************************************/
/*                                                                       */
/* Copyright (c) 1997-98 Richard Tobin, Language Technology Group, HCRC, */
/* University of Edinburgh.                                              */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND,     */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF EDINBURGH BE LIABLE */
/* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF    */
/* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION    */
/* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.       */
/*                                                                       */
/*************************************************************************/
/* 
 * An implementation of printf that
 * - allows printing of 16-bit unicode characters and strings
 * - translates output to a specified character set
 *
 * "char8" is 8 bits and contains ISO-Latin-1 (or ASCII) values
 * "char16" is 16 bits and contains UTF-16 values
 * "Char" is char8 or char16 depending on whether CHAR_SIZE is 8 or 16
 *
 * Author: Richard Tobin
 */

#ifndef __rxp_stdio16B_H__
#define __rxp_stdio16B_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "charset.h"

typedef int ReadProc(FILE16 *file, unsigned char *buf, int max_count);
typedef int WriteProc(FILE16 *file, const unsigned char *buf, int count);
typedef int SeekProc(FILE16 *file, long offset, int ptrname);
typedef int FlushProc(FILE16 *file);
typedef int CloseProc(FILE16 *file);

FILE16 *MakeFILE16(const char *type);

#define FILE16_read             0x01
#define FILE16_write            0x02
#define FILE16_close_underlying 0x04



#ifdef __cplusplus
}
#endif
 


#ifdef __cplusplus

#include <iostream>


struct _FILE16 {
    void *handle;
    int handle2, handle3;
    ReadProc *read;
    WriteProc *write;
    SeekProc *seek;
    FlushProc *flush;
    CloseProc *close;
    int flags;
    CharacterEncoding enc;
    char16 save;
    std::istream *stream;
};
#else
struct _FILE16 {
    void *handle;
    int handle2, handle3;
    ReadProc *read;
    WriteProc *write;
    SeekProc *seek;
    FlushProc *flush;
    CloseProc *close;
    int flags;
    CharacterEncoding enc;
    char16 save;
    struct istream *stream;
};
#endif


#endif
