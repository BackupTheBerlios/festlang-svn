/*************************************************************************/
/*                                                                       */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2007                            */
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
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*            Authors: Kishore Prahallad                                 */
/*            Email:   skishore@cs.cmu.edu                               */
/*                                                                       */
/*************************************************************************/
#include "islice_header.h"

#include <string.h>

class islice {

 private: 

typedef struct MS_WavHeader_struct{
   char riff[5];           //4 = "RIFF"  
   int  remainingBytes;    //8 length of packet to follow = filesize-8
   char waveFmt[9];        //16 = "WAVEfmt "
   int  lenFormatChunk;    //20 length of format chunk = 0x10
   char unDefined[3];      //22 always 01,00
   char channels[3];       //24 will be 01,00 in our case
   int  sampleRate;        //28 sampling frequency 16000 for us
   int  bytesPerSec;       //32 sample_rate * bytes_per_sample = 32000
   char bytesPerSample[3]; //34 for our case = 02,00
   char bitsPerSample[3];  //36 for our case = 16,00
   char data[5];           //40 = "data"
   int  dataByteCount;     //44 length of data to follow in bytes
 } MS_WavHeader; 

 MS_WavHeader wavH;
 int _sF;  //samp frequency
 int _frmSz; //frame size;
 int _frmSft; //frame shift
 int _hsize;  //header size;
 int _bps;  //bytes per sample
 int maxSmp; //maximum no. of samples;;
 float totdur; //total duration of the speech file;

 string spchF; //speech file name
 string trnpF; //transcription file name

 
 string *trnp; //transcription text - sequence of words.
 int nwrd; //no. of words in transcription

 float  *sblk; //speech block;
 int    nsmp;  //no. of samples;
 float  _durB;  //duration of the block;
 float  olpD;  //overlapping duration;
 float  _sfrac; //segmentation fraction (actually inverse of it is used).
 float  bT;    //begin time;
 int    bB;    //begin byte;
 int    nblk;  //no. of blocks...
 int    tI;    //text index...

 string basD;  //base dir
 string etcD;  //etc dir
 string blkF;  //temporary block file, where data is stored.
 string spsetF; //speech settings file.
 string wavListF; //wave list file
 string temptrnF; //temp transcription file.
 string featD;  //feat directory
 string modD;  //models directory..
 string tempD; //temporary directory..
 string tlabD; //temporary lab directory..
 string ftxtF; //final text file...

 string gtimF; //global time stamp of segments.
 string _gprefx; //global prefix...
 string _uprefx; //user prefix...

 float cb_dur; //current block duration/length
 float _pauD; // pause duration use to split during prunelab.pl

 public: 
   islice();
   ~islice();
   void init(string, string, int, int, string, float, float, float);
   void read_wave_header();
   void cal_beginbyte();
   void read_speech_block();
   void print_header();

   void dump_featsettings();
   void file_dir_names();
   void create_dir();
   void dump_speech_block();
   void read_speech_bytes(string blkF);
   void read_speech_bytes();
   void gen_feat_block();
   void process_blocks();
   void gen_segment_labels(int);
   void load_text();
   void dump_text_block();
   void prune_labels(int bno, float& tdur, int& tidx, string ndeF);

};

islice::islice() {
}
islice::~islice() {
}

void islice::init(string sFile, string tFile, int frmSz, int frmSft, string uprefx, float tsfrac, float tdurB, float tpauD)  {
  
  spchF = sFile;
  trnpF = tFile;

  FileExist((char*)sFile.c_str());
  FileExist((char*)tFile.c_str());

  _uprefx = uprefx;

  read_wave_header();
  //print_header();
  _hsize = 44;

  _sF = wavH.sampleRate;

  //cal bytes per sample;
  _bps = wavH.bytesPerSec / _sF;

  //cal max. samples;
  maxSmp = wavH.dataByteCount / _bps;
  
  //total duration
  totdur = (float) maxSmp / (float) _sF;

  cout<<"bytes per Sample "<<_bps<<endl;
  cout<<"samp freq: "<<_sF<<endl;
  cout<<"max Samples are: "<<maxSmp<<endl;
  cout<<"total duration: "<<totdur<<" S "<<endl;

  _frmSz = frmSz;
  _frmSft = frmSft;

  _sfrac = tsfrac;
  _pauD  = tpauD;

  _durB = tdurB; //60 sec - 1 min;

  //olpD = 60.0/2.0; //30 sec - 0.5 min;

  olpD = _durB / _sfrac;

  bT = 0;
  nsmp = 0;

  Alloc1f(sblk, 10); //dummy allocation of 10 bytes..

  file_dir_names();
  //create_dir();
  dump_featsettings();

  load_text();
}

void islice::load_text() {

 ifstream fp_in;
 fp_in.open((char*)trnpF.c_str(), ios::in);
 fp_in>>nwrd;
 Alloc1s(trnp, nwrd);
 for (int i = 0; i < nwrd; i++) {
  fp_in>>trnp[i];
 }
 fp_in.close();

}


void islice::dump_featsettings() {
  
  ofstream fp_out;
  ofstream fp_out1;

  fp_out.open((char*)spsetF.c_str(), ios::out);
  if (fp_out == 0) {
    cout<<"Cannot open "<<spsetF<<endl;
    cout<<"Aborting.."<<endl;
    exit(1);
  }
  fp_out<<"WaveDir: "<<tempD<<"/"<<endl;
  fp_out<<"HeaderBytes: 0"<<endl;
  fp_out<<"SamplingFreq: "<<_sF<<endl;
  fp_out<<"FrameSize: "<<_frmSz<<endl;
  fp_out<<"FrameShift: "<<_frmSft<<endl;
  fp_out<<"LpOrder: 12"<<endl;
  fp_out<<"CepsNum: 16"<<endl;
  fp_out<<"FeatDir: "<<featD<<"/"<<endl;
  fp_out<<"Ext: .raw"<<endl;

  fp_out.close();

  /* fp_out1.open((char*)wavListF.c_str(), ios::out);
  fp_out1<<"NoOfFiles: 1"<<endl;
  fp_out1<<"blkf";
  fp_out1.close();
  */
  

}

void islice::file_dir_names() {

  basD = "islice";
  etcD = basD + "/" + "etc";
  featD = basD + "/" + "feat";
  modD  = basD + "/" + "mod";
  tempD = basD + "/" + "temp";
  tlabD = basD + "/" + "tlab";

  spsetF = etcD + "/" + "mysp_settings";
  wavListF = etcD + "/" + "mywave_list";

  blkF = tempD + "/" + "blkf.raw";
  temptrnF = tempD + "/" + "blkf.pmt"; //prompt file..
  ftxtF = "etc/" + _uprefx+ "_txt.seg.data";
  gtimF = "etc/" + _uprefx + "_tim.seg.data";
  _gprefx = _uprefx+"_seg";

}

void islice::create_dir() {

  char cmd[1000];
  sprintf(cmd, "mkdir %s",(char*)basD.c_str());
  system(cmd);

  sprintf(cmd, "mkdir %s",(char*)etcD.c_str());
  system(cmd);

  sprintf(cmd, "mkdir %s",(char*)featD.c_str());
  system(cmd);

  sprintf(cmd, "mkdir %s", (char*)tempD.c_str());
  system(cmd);

  //$EHMMDIR/bin/edec ehmm/etc/ph_list.int ehmm/etc/txt.phseq.data.int 1 ehmm/feat ft ehmm/etc/mysp_settings ehmm/mod


}

void islice::read_wave_header() {

 /* typedef struct MS_WavHeader{
   char riff[5];           //4 = "RIFF"  
   int  remainingBytes;    //8 length of packet to follow = filesize-8
   char waveFmt[9];        //16 = "WAVEfmt "
   int  lenFormatChunk;    //20 length of format chunk = 0x10
   char unDefined[3];      //22 always 01,00
   char channels[3];       //24 will be 01,00 in our case
   int  sampleRate;        //28 sampling frequency 16000 for us
   int  bytesPerSec;       //32 sample_rate * bytes_per_sample = 32000
   char bytesPerSample[3]; //34 for our case = 02,00
   char bitsPerSample[3];  //36 for our case = 16,00
   char data[5];           //40 = "data"
   int  dataByteCount;     //44 length of data to follow in bytes
 }; */

 wavH.riff[4] = '\0';
 wavH.waveFmt[8] = '\0';
 wavH.unDefined[2] = '\0';
 wavH.channels[2] = '\0';
 wavH.bytesPerSample[2] = '\0';
 wavH.bitsPerSample[2] = '\0';
 wavH.data[4] = '\0';

 ifstream fp_bin;
   fp_bin.open((char*)spchF.c_str(), ios::in | ios::binary);

   fp_bin.read((char*)wavH.riff, 4);   //4
   fp_bin.read((char*)&wavH.remainingBytes, 4); //8
   fp_bin.read((char*)wavH.waveFmt, 8); //16
   fp_bin.read((char*)&wavH.lenFormatChunk, 4); //20
   fp_bin.read((char*)wavH.unDefined, 2); //22
   fp_bin.read((char*)wavH.channels, 2); //24
   fp_bin.read((char*)&wavH.sampleRate, 4); //28
   fp_bin.read((char*)&wavH.bytesPerSec, 4); //32
   fp_bin.read((char*)wavH.bytesPerSample, 2); //34
   fp_bin.read((char*)wavH.bitsPerSample, 2); //36
   fp_bin.read((char*)wavH.data, 4); //40
   fp_bin.read((char*)&wavH.dataByteCount, 4); //44
 fp_bin.close();

 if (strcmp(wavH.riff, "RIFF") != 0) {
   cout<<"wavFile: "<<spchF<<" is not in MS RIFF format..."<<endl;
   cout<<"wavH.riff: "<<wavH.riff<<endl;
   cout<<"Aborting.... "<<endl;
   exit(1);
 }
}

void islice::print_header() {

  cout<<"File Name: "<<spchF<<endl;

  cout<<"Riff (4): "<<wavH.riff<<endl;
  cout<<"Remaining Bytes (8): "<<wavH.remainingBytes<<endl;
  cout<<"waveFmt (16): "<<wavH.waveFmt<<endl;
  cout<<"LenFormatChunk (20): "<<wavH.lenFormatChunk<<endl;
  cout<<"unDefined (22): "<<wavH.unDefined<<endl;
  cout<<"Channels (24): "<<wavH.channels<<endl;
  cout<<"SampleRate (28): "<<wavH.sampleRate<<endl;
  cout<<"bytesPerSec (32): "<<wavH.bytesPerSec<<endl;
  cout<<"bytesPerSample (34): "<<wavH.bytesPerSample<<endl;
  cout<<"bitsPerSample (36): "<<wavH.bitsPerSample<<endl;
  cout<<"Data (40): "<<wavH.data<<endl;
  cout<<"dataByteCount (44): "<<wavH.dataByteCount<<endl;
}

void islice::cal_beginbyte() {
 
 int bS; //begin sample;
 bS = (int) (bT * (float)_sF);
 bB = bS * _bps;
 bB = _hsize + bB;

 nsmp = (int) (_durB * (float)_sF);

 if (bS + nsmp >= maxSmp) {  //adjust to the last block.....
                             //or handle files less than 1 min;
   nsmp = maxSmp - bS;
 }

 cb_dur = (float) nsmp  / (float)_sF;

 cout<<"Processing from: "<<bS<<" to "<<bS + nsmp<<" max: "<<maxSmp<<" cb_dur: "<<cb_dur<<endl;
}

void islice::dump_speech_block() {
  cal_beginbyte();
  read_speech_bytes(blkF);

  //Delet1f(sblk);
  //Alloc1f(sblk, nsamp);
  //read_speech_bytes();
  //write_speech_block();
}

void islice::read_speech_bytes(string blkF) {

  ifstream fp_in;
  ofstream fp_out;
  short int temp;

  fp_in.open((char*)spchF.c_str(), ios::in | ios::binary);
  fp_out.open((char*)blkF.c_str(), ios::out | ios::binary);

  fp_in.seekg(bB);
  cout<<"bB: "<<bB<<" nsmp: "<<nsmp<<endl;

  for (int i = 0; i < nsmp; i++) {
    fp_in.read((char*)&temp, _bps);
    fp_out.write((char*)&temp, _bps);
  } 
  fp_in.close();
  fp_out.close();
}

void islice::read_speech_bytes() {

  ifstream fp_in;
  short int temp;

  fp_in.open((char*)spchF.c_str(), ios::in | ios::binary);
  fp_in.seekg(bB);
  for (int i = 0; i < nsmp; i++) {
    fp_in.read((char*)&temp, _bps);
    sblk[i] = (float) temp;
  } 
  fp_in.close();
}

void islice::gen_feat_block() {
   
  //system feature extraction code... 
  char cmd[1000];
  sprintf(cmd, "$EHMMDIR/bin/FeatureExtraction %s %s", (char*)spsetF.c_str(), (char*)wavListF.c_str());
  system(cmd);
   
}

void islice::process_blocks() {

  nblk = (int) (totdur / olpD) + 1;
  cout<<"****** Max Blocks are: "<<nblk<<" *************\n";

  float tdur; //temp duration
  int tidx;   //temp idx..

  int tp1;


  ofstream fp_out;
  ofstream fp_tim;

  fp_out.open((char*)ftxtF.c_str(), ios::out);
  fp_out.close();

  fp_tim.open((char*)gtimF.c_str(), ios::out);

  bT = 0; //begin Time..
  tI = 0; //text Index..

  int ndeF = 1; //non-deterministic ending....
  string ndeS;

  char tstr[100];
  string tpfx;

  //usefule for debugging...
  //nblk = 3; bT = 725.43; tI = 2308; //used for debugging..
  //790.545 tI 2541
  //bT = 790.545; tI = 2541; nblk = 3;
  //bT  = 786.07;  tI = 2526; nblk = 3;

  int bsnm = 0; //block serial number
  ndeF = 1;
  ndeS = "1";

  //while (bsnm < nblk && bT < totdur && ndeF == 1) {
  while (bT < totdur && ndeF == 1) {

    cout<<"processing from "<<bT<<endl;

    dump_speech_block();
    dump_text_block();


    //if (bsnm == nblk - 1) {  //merge last three blocks...
    if (cb_dur <= 2 * olpD) {  //merge last three blocks...
      ndeF = 0;
      ndeS = "0";
    }

    gen_segment_labels(ndeF);  //use edec..
    prune_labels(bsnm, tdur, tidx, ndeS);

    tp1 = bsnm + 1;

    if (tp1 < 10) {
      sprintf(tstr, "000%d", tp1);
    }else if (tp1 < 100) {
      sprintf(tstr, "00%d", tp1);
    }else if (tp1 < 1000) {
      sprintf(tstr, "0%d", tp1);
    }else {
      sprintf(tstr, "%d", tp1);
    }

    tpfx = tstr;
    tpfx = _gprefx + tpfx;

    fp_tim<<tpfx<<" "<<bT<<" "<<bT + tdur<<endl;

    //bT = bT + olpD;
    bT = bT + tdur;
    tI = tI + tidx;

    cout<<"bT "<<bT<<" tI "<<tI<<endl;
    bsnm++;

    //int myc; cout<<"ENTER INT: "<<endl; cin>>myc;
  }
  fp_tim.close();
}

void islice::prune_labels(int bno, float& tdur, int& tidx, string ndeF) {

 string prefx;
 string labF;
 string timF;
 string txtF;

 string dictF;
 string olabF;

 txtF = ftxtF;

 char tmp[2000];
 int tbno = bno + 1;

 if (tbno < 10) {
   sprintf(tmp, "000%d", tbno);
 }else if (tbno < 100) {
   sprintf(tmp, "00%d", tbno);
 }else if (tbno < 1000) {
   sprintf(tmp, "0%d", tbno);
 }else {
   sprintf(tmp, "%d", tbno);
 }

 prefx = tmp;
 prefx = _gprefx + prefx;
 labF  = "lab/" + prefx + ".lab";
 timF  = tempD + "/" + "o.tim";

 olabF = "lab/blkf.lab";
 dictF = tempD + "/" + "blkf.dict";

 string cmd;
 char tmp_f[200];
 char s_paud[200];
 sprintf(tmp_f, "%f", _sfrac);
 sprintf(s_paud, "%f", _pauD);

 cmd = "perl $ISLICEDIR/bin/prunelab.pl " + olabF + " " + dictF + " " + labF + " " + txtF + " " + timF + " " + ndeF + " " + tmp_f + " " + s_paud;

 ofstream fp_out;
 fp_out.open((char*)txtF.c_str(), ios::app);
 fp_out<<"( "<<prefx<<" \" ";
 fp_out.close();

 //cout<<"Cmd is "<<cmd<<endl;
 system((char*)cmd.c_str());

 fp_out.open((char*)txtF.c_str(), ios::app);
 fp_out<<" \" )"<<endl;
 fp_out.close();

 ifstream fp_in;

 fp_in.open((char*)timF.c_str(), ios::in);
 fp_in>>tdur>>tidx;
 fp_in.close();

 //perl file.pl <lab-file> <dict-file> <o-labfile> <o-txtfile in append-mode> <time-stampF>

}

void islice::dump_text_block() {

 int max_nwb; //max no of words in the block;

 int tlen; //temp. block length; 
 int idx; //index;

 //max_nwb = 250;
 max_nwb = 125;
 
 if (tI + max_nwb >= nwrd) {
   tlen = nwrd - tI;
 }else {
   tlen = max_nwb;
 }

 ofstream fp_out;
 fp_out.open((char*)temptrnF.c_str(), ios::out);

 fp_out<<"( blkf \" ";

 for (int i = 0; i < tlen; i++) {
  idx = tI + i;
  fp_out<<trnp[idx]<<" ";
 }
 fp_out<<"\")"<<endl;
 fp_out.close();

}

void islice::gen_segment_labels(int ndeF) {

  system("$ISLICEDIR/bin/do_islice phseq");
  system("$ISLICEDIR/bin/do_islice feats");
  if (1 == ndeF) {
    system("$ISLICEDIR/bin/do_islice align1");
  } else {
    system("$ISLICEDIR/bin/do_islice align");
  }

}

int main(int argc, char *argv[]) {
  
  if (argc < 5) { 
    cout<<"pass <speech-file> <text-file> <user-prefx> <pauD>\n";
    exit(1);
  }

  string spchF;
  string trnF;
  string upfx;
  float pauD;

  spchF = argv[1];
  trnF = argv[2];
  upfx = argv[3];
  pauD = atof(argv[4]);

  islice lslice;
  lslice.init(spchF, trnF, 160, 80, upfx, 3.0, 30, pauD); 
  //last parameter dictates size of segments, 5.0 implies 1/5.0 of 60 s
  lslice.process_blocks();
}
