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
# @(#) $Id: Makefile.am,v 1.15 2000/03/21 10:36:55 strauss Exp $
#

INCLUDES		= -I../lib

EXTRA_DIST		= smidump.h \
			  $(man_MANS) \
			  dump-java.c dump-jdmk.c dump-fig.c

bin_PROGRAMS		= smiquery smilint smidump

bin_SCRIPTS		= smistrip

man_MANS		= smiquery.1 smilint.1 smidump.1 smistrip.1

smiquery_SOURCES	= smiquery.c
smiquery_LDADD		= ../lib/libsmi.la

smilint_SOURCES		= smilint.c
smilint_LDADD		= ../lib/libsmi.la

smidump_SOURCES		= smidump.c smidump.h dump-sming.c dump-smi.c \
			  dump-imports.c dump-types.c dump-tree.c \
			  dump-mosy.c dump-corba.c dump-ucdsnmp.c \
			  dump-dia.c dump-jax.c dump-xml.c dump-cm.c
#			  dump-fig.c
#			  dump-java.c dump-jdmk.c
smidump_LDADD		= ../lib/libsmi.la

	
