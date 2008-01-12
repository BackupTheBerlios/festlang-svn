#!/usr/bin/perl

# sphinx-netserver.pl 
# Copyright (c) 2005 Josh McAllister
#
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
#
# Written by Josh McAllister 


use IO::Socket;
use Symbol;
use POSIX;
$SPHINXDIR='/usr/share/sphinx2';
use Speech::Recognizer::SPX qw(:uttproc :fbs $SPHINXDIR);
#use Speech::Recognizer::SPX::Server;

# establish SERVER socket, bind and listen.
$server = IO::Socket::INET->new(LocalPort => 1069,
                                Type      => SOCK_STREAM,
                                Proto     => 'tcp',
                                Reuse     => 1,
                                Listen    => 10 )
  or die "making socket: $@\n";

# global variables
$PREFORK                = 1;        # number of children to maintain
$MAX_CLIENTS_PER_CHILD  = 50;        # number of clients each child should process
%children               = ();       # keys are current child process IDs
$children               = 0;        # current number of children

sub REAPER {                        # takes care of dead children
    $SIG{CHLD} = \&REAPER;
    my $pid = wait;
    $children --;
    delete $children{$pid};
}

sub HUNTSMAN {                      # signal handler for SIGINT
    local($SIG{CHLD}) = 'IGNORE';   # we're going to kill our children
    kill 'INT' => keys %children;
    exit;                           # clean up with dignity
}
    
# Fork off our children.
for (1 .. $PREFORK) {
    make_new_child();
}

# Install signal handlers.
$SIG{CHLD} = \&REAPER;
$SIG{INT}  = \&HUNTSMAN;

# And maintain the population.
while (1) {
    sleep;                          # wait for a signal (i.e., child's death)
    for ($i = $children; $i < $PREFORK; $i++) {
        make_new_child();           # top up the child pool
    }
}

sub make_new_child {
    my $pid;
    my $sigset;
    
    # block signal for fork
    $sigset = POSIX::SigSet->new(SIGINT);
    sigprocmask(SIG_BLOCK, $sigset)
        or die "Can't block SIGINT for fork: $!\n";
    
    die "fork: $!" unless defined ($pid = fork);
    
    if ($pid) {
        # Parent records the child's birth and returns.
        sigprocmask(SIG_UNBLOCK, $sigset)
            or die "Can't unblock SIGINT for fork: $!\n";
        $children{$pid} = 1;
        $children++;
        return;
    } else {	#Child
        # Child can *not* return from this subroutine.
        $SIG{INT} = 'DEFAULT';      # make SIGINT kill us as it did before
    
        # unblock signals
        sigprocmask(SIG_UNBLOCK, $sigset)
            or die "Can't unblock SIGINT for fork: $!\n";
   
        # Initialize sphinx 
	fbs_init({-live         => 'FALSE',
		  -adcin 	=> 'yes',
		  -feat		=> '1s_c_d_dd',
		  -hmm   	=> '/home/shmyrev/work/asterisk-rus-taxi/model',
		  -dict 	=> '/home/shmyrev/work/asterisk-rus-taxi/model/dict',
		  -fsg		=> '/home/shmyrev/work/asterisk-rus-taxi/model/test.fsg'});

        # handle connections until we've reached $MAX_CLIENTS_PER_CHILD
        for ($i=0; $i < $MAX_CLIENTS_PER_CHILD; $i++) {
           my $buf = undef;
           $client = $server->accept() or last;
	   uttproc_begin_utt();
	   my $count = 0;
           my $datasize = readline $client;
           chomp $datasize;
           my $b = read ($client, my($buf), $datasize);
           #print "SERVER DEBUG: Expecting $datasize bytes, got $b bytes.\n";
	   uttproc_rawdata($buf, 1) or die "uttproc_rawdata failed";
           uttproc_end_utt();
	   my ($fr, $hyp) = uttproc_result(1);
	   #print "frames $fr\n";
           print STDERR "SERVER RESULT: $hyp\n";
           print $client "$hyp";
	   close $client;
        }
        # tidy up gracefully and finish
    
        # this exit is VERY important, otherwise the child will become
        # a producer of more and more children, forking yourself into
        # process death.
        fbs_end();
        exit;
    }
}
