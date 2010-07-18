###########################################################################
#                                                                         #
#  Contains implementations of INTSINT/MOMEL intonation theories          #
#                                                                         #
###########################################################################

## About this directory

## The name of this directory
DIRNAME=src/modules/INTSINT_Intonation

## A path from this directory to the top of the speech_tools tree,
TOP=../../..


###########################################################################
## The code in this directory

## The include files in this directory
H =

## The C++ sources which contain template instantiation code.
TSRCS = 

## All C++ sources.
CPPSRCS = intsint_database.cc INTSINT_Intonation.cc intsint_aux.cc

## Ansi C sources
CSRCS = 

## All sources
SRCS = $(CPPSRCS) $(CSRCS)

## Object files to be created
OBJS = $(CPPSRCS:.cc=.o) $(CSRCS:.c=.o)

###########################################################################
## Everything in this directory

## All files in this directory.
FILES = Makefile $(SRCS) $(H)

## Sub directories which need to be compiled when the library is rebuilt
##LIB_BUILD_DIRS=

## Sub directories which need to be built in a full rebuild
##BUILD_DIRS=$(LIB_BUILD_DIRS)

## All sub directories
##ALL_DIRS=$(BUILD_DIRS)

LOCAL_INCLUDES = -I../include

INLIB = $(TOP)/src/lib/libFestival.a

###########################################################################
## Any directory specific compilation rules would go here.

###########################################################################
## What we need to do to build this directory

ALL = .buildlib

###########################################################################
## Include the common speech_tools compilation rules

include $(TOP)/config/common_make_rules


