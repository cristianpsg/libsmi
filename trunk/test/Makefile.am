#
# Makefile.am --
#
#      Template to generate test Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.5 1999/06/08 20:24:23 strauss Exp $
#

TESTMIBS =	SNMPv2-MIB IF-MIB MAU-MIB RMON2-MIB

TESTS			= smilint.test smidump-1.test smidump-2.test \
			  smidump-3.test

TESTS_ENVIRONMENT	= SMIPATH=@smipath@ TESTMIBS="$(TESTMIBS)"

EXTRA_DIST		= smilint.test smidump-1.test smidump-2.test \
			  smidump-3.test \
			  filter

