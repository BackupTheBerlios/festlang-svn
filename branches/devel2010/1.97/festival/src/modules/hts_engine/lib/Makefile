 ###########################################################################
 ##                                                                       ##
 ##                Centre for Speech Technology Research                  ##
 ##                     University of Edinburgh, UK                       ##
 ##                         Copyright (c) 1996                            ##
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
 ##  THE UNIVERSITY OF EDINBURGH AND THE CONTRIBUTORS TO THIS WORK        ##
 ##  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ##
 ##  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ##
 ##  SHALL THE UNIVERSITY OF EDINBURGH NOR THE CONTRIBUTORS BE LIABLE     ##
 ##  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ##
 ##  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ##
 ##  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ##
 ##  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ##
 ##  THIS SOFTWARE.                                                       ##
 ##                                                                       ##
 ###########################################################################
 ##                                                                       ##
 ##                 Author: Sergio Oller (sergioller@gmail.com)           ##
 ##                         Universitat Politècnica de Catalunya          ##
 ## --------------------------------------------------------------------  ##
 ##  Contains an implementation of hts_engine as a module for festival.   ##
 ##                                                                       ##
 ###########################################################################


###########################################################################
## About this directory

## The name of this directory
DIRNAME=src/modules/hts_engine/lib

## A path from this directory to the top of the speech_tools tree,
TOP=../../../..

###########################################################################
## The code in this directory

## The include files in this directory
H = HTS_hidden.h mlsa_resynthesis.h

## The C++ sources which contain template instantiation code.
#TSRCS = foo.cc

## All C++ sources.
CPPSRCS = HTS_audio.cc \
	HTS_engine.cc \
	HTS_gstream.cc \
	HTS_label.cc \
	HTS_misc.cc \
	HTS_model.cc \
	HTS_pstream.cc \
	HTS_sstream.cc \
	HTS_vocoder.cc \
	mlsa_resynthesis.cc

## Ansi C sources
CSRCS=

## All sources
SRCS = $(CPPSRCS) $(CSRCS)

## Object files to be created
OBJS = $(CPPSRCS:.cc=.o) $(CSRCS:.c=.o)

###########################################################################
## Everything in this directory

## All files in this directory.
FILES = Makefile $(SRCS) $(H)

## Sub directories which need to be compiled when the library is rebuilt
LIB_BUILD_DIRS=

## Sub directories which need to be built in a full rebuild
BUILD_DIRS=$(LIB_BUILD_DIRS)

## All sub directories
ALL_DIRS=$(BUILD_DIRS)

## Module to be added to the festival library:
INLIB = $(TOP)/src/lib/libFestival.a

###########################################################################
## Any directory specific compilation rules would go here.

##

###########################################################################
## What we need to do to build this directory

ALL = .buildlib

###########################################################################
## Include the common speech_tools compilation rules

include $(TOP)/config/common_make_rules


