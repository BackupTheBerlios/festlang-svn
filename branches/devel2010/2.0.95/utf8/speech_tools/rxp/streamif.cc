
#include "stdio16.h"
#include "stdio16b.h"

using namespace std;

static int StreamRead(FILE16 *file, unsigned char *buf, int max_count);
static int StreamWrite(FILE16 *file, const unsigned char *buf, int count);
static int StreamSeek(FILE16 *file, long offset, int ptrname);
static int StreamClose(FILE16 *file);
static int StreamFlush(FILE16 *file);

FILE16 *MakeFILE16FromiStream(void *is,const char *type)
{
    FILE16 *file;
    if(!(file = MakeFILE16(type)))
        return 0;

    file->read = StreamRead;
    file->write = StreamWrite;
    file->seek = StreamSeek;
    file->close = StreamClose;
    file->flush = StreamFlush;
    file->stream = (std::istream *) is;

    return file;
}

static int StreamRead(FILE16 *file, unsigned char *buf, int max_count)
{
    istream *stream=file->stream;
    
    stream->read((char *)buf,max_count);
    
    return (stream->good()) ? stream->gcount() : -1;
}

static int StreamWrite(FILE16 *file, const unsigned char *buf, int count)
{
    istream *stream=file->stream;
    
    if (count == 0) return 0;
    
   // stream->write((char *)buf,count);
    cerr << "write not available in istream" << endl;
    return -1;
   // return (stream->bad()) ? -1 : 0;
}

static int StreamSeek(FILE16 *file, long int offset, int ptrname)
{
    istream *stream=file->stream;
    ios_base::seekdir fromwhere;
    
    switch (ptrname) {
        case SEEK_CUR:
           fromwhere=ios_base::cur;
           break;
        case SEEK_SET:
           fromwhere=ios_base::beg;
           break;
        case SEEK_END:
           fromwhere=ios_base::end;
           break;
        default:
           std::cerr << "StreamSeek from a strange place" << std::endl;
           return -1;
    }

    if ( (file->flags & FILE16_read) != 0)
        stream->seekg(offset,fromwhere);
//    if ( (file->flags & FILE16_write) != 0)
//        stream->seekp(offset,fromwhere);
    
    return stream->good() ? 0 : -1;

}

static int StreamClose(FILE16 *file)
{
//    istream *stream=file->stream;
    
 //   if ((file->flags & FILE16_close_underlying) != 0) delete stream;
    
    return 0;
}

static int StreamFlush(FILE16 *file)
{
//    istream *stream=file->stream;

//    if ( (file->flags & FILE16_write) != 0)
//        stream->flush();

//    return ((stream->good()) ?  0 : -1);
    return 0;
}

