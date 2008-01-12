#!/usr/bin/perl
# sphinx-netclient.pl
# Copyright (c) 2005 Josh McAllister
#
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
#
# Written by Josh McAllister 


use Asterisk::AGI;
my $AGI = new Asterisk::AGI;
%input = $AGI->ReadParse();

sub asr {
use IO::Socket;
use FileHandle;
use IPC::Open2;

my $file = shift or return undef;
my $host = shift || 'localhost';
my $port = shift || '1069';
my $fh;

my $remote =  IO::Socket::INET->new(
		Proto    => "tcp",
		PeerAddr => "$host",
		PeerPort => "$port",
		) or return undef;

#Idea here being that you can pass a reference to an existing file handle... not yet implemented, just pass a filename.
if (ref $file) { 
   my $fh = $file;
} else {
   open (FH, $file) || return undef;
   $fh = *FH;
}

$file =~ /(gsm|wav)$/;
my $type = $1;
if ($type !~ /gsm|wav/) {
   warn "Unknown file type ($file)";
   return undef;
}
#print "FTYPE: $type\n";
$pid = open2(*SOXIN, *SOXOUT, "sox -t $type - -s -r 16000 -w -t wav - 2>/dev/null") || warn ("Could not open2.\n");

binmode $fh;
binmode SOXIN;
binmode SOXOUT;
binmode $remote;

while (defined(my $b = read $fh, my($buf), 4096)) {
   last if $b == 0;
   $count += $b;
   print SOXOUT $buf;
}
close SOXOUT;

$count = 0;
my $sox = "";
while (defined(my $b = read SOXIN, my($buf), 4096)) {
   last if $b == 0;
   $count += $b;
   $sox .= $buf;
}
print $remote length($sox) . "\n";
print $remote "$sox";
close SOXIN;

#print "DEBUG: Waiting for result.\n";
   
$count=0;
my $result = "";
while (defined(my $b = read $remote, my($buf), 4096)) {
   last if $b == 0;
   $count += $b;
   $result .= $buf;
}

close $fh;
close $remote;

return "$result";
}

sub confirm {
     my $v1response;
     my $v2response;

     $AGI->exec('Festival', "Здравствуйте, вас приветствует таксо парк"); 
      
      while (my $tries <= 3) {
         $tries++;
         

         $AGI->exec('Festival', "Куда вы хотите поехать"); 

         $AGI->record_file("/tmp/$$", 'gsm', '0', 3000);

         $v1response = asr("/tmp/$$.gsm");

         $AGI->exec('Festival', "Во сколько подать машину"); 

         $AGI->record_file("/tmp/$$", 'gsm', '0', 3000);
         
	 $v2response = asr("/tmp/$$.gsm");
	 
	 $AGI->exec('Festival', "Правильно я понял вы хотите поехать к $v1response в $v2response"); 

         $AGI->record_file("/tmp/$$", 'gsm', '0', 3000);
         
	 $v1response = asr("/tmp/$$.gsm");
	 
	 if ($v1response =~ /да/i ) {
  	    $AGI->exec('Festival', "Ваш заказ принят"); 
	    break;
	 } else {
	    $AGI->exec('Festival', "Попробуем ещё раз"); 
	 }
      }
}

confirm()
