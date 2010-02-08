#!/usr/local/bin/perl
###########################################################################
##                                                                       ##
##                                                                       ##
##              Carnegie Mellon University, Pittsburgh, PA               ##
##                      Copyright (c) 2004-2007                          ##
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
##          Date   :  Feb 2007                                           ##
##                                                                       ##
###########################################################################

$nargs = $#ARGV + 1;
if ($nargs < 2) {
  print "Usage: perl file.pl <in-file> <out-file>\n";
  exit;
}

$inF  = $ARGV[0];
$ouF  = $ARGV[1];

my @ln = &Get_Lines($inF);

open(fp_out, ">$ouF");

for (my $i = 0; $i <= $#ln; $i++) {
  
  my @wrd = &Get_Words($ln[$i]);
  if ($#wrd == -1) {
     next;
  }

  for (my $j = 0; $j <= $#wrd; $j++) {

     my $cw = $wrd[$j];
     &Make_SingleSpace(\$cw);

     if ($cw eq "'s") {
       print fp_out "$cw"; #no space before 's
       next;
     }

     #process each word;
     $cw =~ s/^[\']+//g;
     $cw =~ s/[\']+$//g;

     $cw =~ s/^[\`]+//g;
     $cw =~ s/[\`]+$//g;
     $cw =~ s/[\`]+//g;

     $cw =~ s/^[\"]+//g;
     $cw =~ s/[\"]+$//g;
     $cw =~ s/[\"]+//g;

     $cw =~ s/[\;]+$/,/g;
     $cw =~ s/^[\;]+/ /g;
     $cw =~ s/[\;]+/ /g;

     $cw =~ s/[\-]+$/ /g;
     $cw =~ s/[\-]+/ /g;

     $cw =~ s/[\_]+$/ /g;
     $cw =~ s/[\_]+/ /g;

     $cw =~ s/[\!]+/, /g;

     $cw =~ s/[\?]+/, /g;

     $cw =~ s/[\:]+$/, /g;
     $cw =~ s/[\)]+/ /g;
     $cw =~ s/[\(]+/ /g;

     $cw =~ s/[\,]+$/,/g;

     &Make_SingleSpace(\$cw);
     print fp_out " $cw";
  }
  print fp_out "\n";
}
close(fp_out);


my $twrd = 0;

my @nnl = &Get_Lines($ouF);

for (my $i = 0; $i <= $#nnl; $i++) {
  my @nwrd = &Get_Words($nnl[$i]);
  my $tt = $#nwrd + 1;
  $twrd = $twrd + $tt;
}

open(fp_out, ">$ouF");
print fp_out "$twrd\n";
for (my $i = 0; $i <= $#nnl; $i++) {
  my $myL = $nnl[$i];
  &Make_SingleSpace(\$myL);
  print fp_out "$myL\n";
}
close(fp_out);



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
