#
# Makefile.am --
#
#      Template to generate the tools Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.9 1999/07/02 14:04:03 strauss Exp $
#

INCLUDES		= -I../lib

EXTRA_DIST		= dump-smi.h dump-sming.h dump-data.h dump-imports.h \
			  dump-mosy.h dump-corba.h \
			  dump-ucdsnmp.h \
			  $(man_MANS) \
			  dump-java.h dump-jdmk.h \
			  dump-java.c dump-jdmk.c

bin_PROGRAMS		= smiquery smilint smidump

man_MANS		= smiquery.1 smilint.1 smidump.1

smiquery_SOURCES	= smiquery.c
smiquery_LDADD		= ../lib/libsmi.la

smilint_SOURCES		= smilint.c
smilint_LDADD		= ../lib/libsmi.la

smidump_SOURCES		= smidump.c dump-sming.c dump-smi.c \
			  dump-data.c dump-imports.c dump-mosy.c \
			  dump-corba.c dump-ucdsnmp.c
#			  dump-java.c dump-jdmk.c
smidump_LDADD		= ../lib/libsmi.la

