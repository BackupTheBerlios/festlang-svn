###########################################################################
#                                                                         #
#  Contains implementations of INTSINT/MOMEL intonation theories          #
#                                                                         #
###########################################################################
TOP=../../..
DIRNAME=src/modules/INTSINT_Intonation
H = 
SRCS = intsint_database.cc INTSINT_Intonation.cc intsint_aux.cc
OBJS = $(SRCS:.cc=.o)

FILES = Makefile $(SRCS) $(H)

LOCAL_INCLUDES = -I../include

INLIB = $(TOP)/src/lib/libFestival.a

ALL = .buildlib

include $(TOP)/config/common_make_rules


