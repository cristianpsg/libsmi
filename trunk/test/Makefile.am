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
# @(#) $Id: Makefile.am,v 1.13 2000/02/11 11:08:31 strauss Exp $
#

TESTMIBTOPDIR		= ../mibs
TESTMIBDIR		= $(TESTMIBTOPDIR)/ietf:$(TESTMIBTOPDIR)/iana:$(TESTMIBTOPDIR)/irtf:$(TESTMIBTOPDIR)/site:$(TESTMIBTOPDIR)/tubs
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

TESTS_ENVIRONMENT	= SMIPATH="$(TESTMIBDIR)" TESTMIBS="$(TESTMIBS)"

EXTRA_DIST		= $(TESTS) filter

SUBDIRS                 = mibs dumps

clean-local:
	rm -rf *.out

sync-dumps:
	echo "I hope you know, what you are doing..." ; sleep 5
	for f in *.out/*.corba-idl *.out/*.corba-oid *.out/*.imports *.out/*.mosy *.out/*.sming *.out/*.smiv1 *.out/*.smiv2 *.out/*.diff *.out/*.smiv2.lint *.out/*.tree *.out/*.types ; do \
	    cmp -s $$f dumps/`basename $$f` || cp $$f dumps/`basename $$f` ; \
	done
	
