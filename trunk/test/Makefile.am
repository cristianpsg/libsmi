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
# @(#) $Id: Makefile.am,v 1.10 1999/10/05 17:10:25 strauss Exp $
#

TESTMIBS		= SNMPv2-MIB IF-MIB MAU-MIB RMON2-MIB

TESTS			= smilint-smiv2.test \
			  smidump-orig-smiv2.test \
			  smidump-smiv1.test \
			  smidump-smiv2.test \
			  smidump-smiv2-smiv2.test \
			  smidump-sming.test \
			  smidump-mosy.test \
			  smidump-tree.test \
			  smidump-types.test \
			  smidump-imports.test \
			  smidump-corba-idl.test \
			  smidump-corba-oid.test \
			  parser.test

TESTS_ENVIRONMENT	= SMIPATH="@smipath@:" TESTMIBS="$(TESTMIBS)"

EXTRA_DIST		= $(TESTS) filter

SUBDIRS                 = mibs dumps

clean-local:
	rm -rf *.out
