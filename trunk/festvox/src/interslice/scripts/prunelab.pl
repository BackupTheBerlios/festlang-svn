#!/usr/local/bin/perl
###########################################################################
##                                                                       ##
##                                                                       ##
##              Carnegie Mellon University, Pittsburgh, PA               ##
##                         Copyright (c) 2007                            ##
##                        All Rights Reserved.                           ##
##                                                                       ##
##  Permission is hereby granted, free of charge, to use and distribute  ##
##  this software and its documentation without restriction, including   ##
##  without limitation the rights to use, copy, modify, merge, publish,  ##
##  distribute, sublicense, and/or sell copies of this work, and to      ##
##  permit persons to whom this work is furnished to do so, subject to   ##
##  the following conditions:                                            ##
##   1. The code must retain the above copyright notice, this list of    ##
##      conditions and the following disclaimer.                         ##
##   2. Any modifications must be clearly marked as such.                ##
##   3. Original authors' names are not deleted.                         ##
##   4. The authors' names are not used to endorse or promote products   ##
##      derived from this software without specific prior written        ##
##      permission.                                                      ##
##                                                                       ##
##  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         ##
##  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ##
##  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ##
##  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      ##
##  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ##
##  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ##
##  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ##
##  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ##
##  THIS SOFTWARE.                                                       ##
##                                                                       ##
###########################################################################
##                                                                       ##
##          Author :  S P Kishore (skishore@cs.cmu.edu)                  ##
##          Date   :  February 2007                                      ##
##                                                                       ##
###########################################################################

$nargs = $#ARGV + 1;
if ($nargs < 8) {
  print "Usage: perl file.pl <lab-file> <dict-file> <o-labfile> <o-txtfile in append-mode> <time-stampF> <nde-F> <_sfrac> <minPauD>\n";
  exit;
}
#nde-F for non-deterministic ending  - 1 and 0 - for not.

$labF = $ARGV[0];
$dicF = $ARGV[1];
$olabF = $ARGV[2];
$otxtF = $ARGV[3];
$otimF = $ARGV[4];
$_ndeF = $ARGV[5];
$_sfrac = $ARGV[6]; #segmentation fraction...
$_minPauD = $ARGV[7];

my @labl = &Get_Lines($labF);
my @dicl = &Get_Lines($dicF);

my $lastl = $labl[$#labl];
my @lw = &Get_Words($lastl);
my $etime = $lw[0];
####my $mtime = $etime / 2.0;
my $mtime = $etime / $_sfrac;
print "Middle time: $mtime Tot: Time: $etime, Frac is: $_sfrac\n";

$sI = 1;
for (my $i = 1; $i <= $#labl; $i++) {
   my @wrd = &Get_Words($labl[$i]);
   if (!IsSilence($wrd[$#wrd])) { 
    $sI = $i; #sI - start index;;
    last;
   }
}

print "Si is $sI\n";

#remove pau from dict file;;
@ndicl;
$k = 0;
for (my $j = 0; $j <= $#dicl; $j++) {
   my @dw = &Get_Words($dicl[$j]);
   if (IsSilence($dw[0])) {
     next;
   } else {
     $ndicl[$k] = $dicl[$j];
     $k++;
   }
}

my @lbw;

open(fp_otxt, ">>$otxtF");
open(fp_otim, ">$otimF");

$wrdU = 0;
for (my $j = 0; $j <= $#ndicl; $j++) {

   my @dw = &Get_Words($ndicl[$j]);

   my $apF = 0;
   my $ucF = 0;

   if ($j + 1 <= $#ndicl) {
     #read next word;;
     my @dw1 = &Get_Words($ndicl[$j + 1]);
     my $nwrd = $dw1[0];
     my @nl = split(//, $nwrd);
     my $nL = $nl[0];
     my $unL = uc($nL); #do upper case of the letter..
     if ($nL eq $unL) {
       $ucF = 1;
     }
     if ($nwrd eq "'s") {
       $apF = 1;
     }
   }

   for (my $k = 1; $k <= $#dw; $k++) {
      my $z = $k - 1;
      $z = $sI + $z;
      @lbw = &Get_Words($labl[$z]);
      if ($lbw[2] ne $dw[$k]) {
        print "mismatch occured at Time: $lbw[0] with the word $dw[0] : $lbw[2] with $dw[$k] - k = $k \n";
	exit;
      }
   }
   $ttime = $lbw[0]; #storing the time stamp of the last phone..


   print fp_otxt "$dw[0]";

   $sI = $sI + $#dw;
   @lbw = &Get_Words($labl[$sI]);

   if (IsSilence($lbw[2])) {
     my $smbl = &Get_Symbol($lbw[2], $ucF);
     print fp_otxt "$smbl ";
     $wrdU++;
   }elsif ($apF == 1) {
     #don't print a space anticaping 's to join with the current word..
   } else {
     print fp_otxt " ";
     $wrdU++;
   }

   #####if ($ttime >= $mtime && 1 == $_ndeF && IsSilence($lbw[2])) {
   if ($wrdU > 3 && 1 == $_ndeF && IsSilence($lbw[2])) {

      my $stime = $lbw[0];
      my $paut  = ($stime - $ttime);

      my $stm1 = 0; #the below is for pauses split into two pauses...
      if ($sI <= $#labl) {
        $sI++;
        my @ttl = &Get_Words($labl[$sI]);
	if (IsSilence($ttl[2])) {
          $stm1 = $ttl[0];
          @lbw = &Get_Words($labl[$sI]); ##filling lbw with the required.
	} else {
          $sI--;
	}
      }

      if ($stm1 > 0) {
        $paut = $paut + ($stm1 - $stime);
      }


      if ($paut >= $_minPauD) {

        $ttime = $ttime + ($paut / 2.0);
        print fp_otim "$ttime $wrdU\n";
        last; #stop the for loop;

      } else {

         while (IsSilence($lbw[2])) {
           $sI++;
	   if ($sI > $#labl) {
	     print "This shouldn't occur - sI exceeding $#labl\n";
	     last;
	   }
           @lbw = &Get_Words($labl[$sI]);
         }

      }

   }else {
      while (IsSilence($lbw[2])) {
          $sI++;
	  if ($sI > $#labl) {
	     print "This shouldn't occur - sI exceeding $#labl\n";
	     last;
	  }
          @lbw = &Get_Words($labl[$sI]);
      }
   }

}

if (0 == $_ndeF) {
  print fp_otim "$ttime $wrdU\n";
}
close(fp_otxt);
close(fp_otim);

my @timl = &Get_Lines($otimF);
my @twrd = &Get_Words($timl[0]);
my $ctim = $twrd[0];


open(fp_olab, ">$olabF");
print fp_olab "#\n";
for (my $i = 1; $i <= $#labl; $i++) {
  my @lbw = &Get_Words($labl[$i]);
  if ($lbw[0] <= $ctim) {
    print fp_olab "$labl[$i]";
  }else {
   last;
  }
}
close(fp_olab);

sub IsSilence() {

  my $sw = shift(@_);
  my $rv = 0;
  if ($sw eq "0" || $sw eq "pau" || $sw eq "PAU" || $sw eq "sil" || $sw eq "SIL" || $sw eq "ssil") {
    $rv = 1;
  }
  return $rv;
}

sub Get_Symbol() {

 my $sw = shift(@_);
 my $sf = shift(@_);

 $rv = ",";
 if ($sw eq "ssil") {
   $rv = " -";
 }elsif ($sf == 1) {
   $rv = "."; 
 }
 return $rv;

}

sub Make_SingleSpace() {
   chomp(${$_[0]});
   ${$_[0]} =~ s/[\s]+$//;
   ${$_[0]} =~ s/^[\s]+//;
   ${$_[0]} =~ s/[\s]+/ /g;
   ${$_[0]} =~ s/[\t]+/ /g;
}

sub Check_FileExistence() {
  my $inF = shift(@_); 
  if (!(-e $inF)) { 
    print "Cannot open $inF \n";
    exit;
  } 
  return 1;
}

sub Get_Lines() {
  my $inF = shift(@_); 
  &Check_FileExistence($inF);
  open(fp_llr, "<$inF");
  my @dat = <fp_llr>;
  close(fp_llr);
  return @dat;
}

sub Get_Words() {
  my $ln = shift(@_);
  &Make_SingleSpace(\$ln);
  my @wrd = split(/ /, $ln);
  return @wrd;
}

sub Get_ProcessedLines() {
  my $inF = shift(@_);
  &Check_FileExistence($inF);
  open(fp_llr, "<$inF");
  my @dat = <fp_llr>;
  close(fp_llr);

  my @nd;
  for (my $i = 0; $i <= $#dat; $i++) {
     my $tl = $dat[$i];
     &Make_SingleSpace(\$tl);
     $nd[$i]  = $tl;
  }
  return @nd;
}
