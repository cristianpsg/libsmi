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
# @(#) $Id: Makefile.am,v 1.7 1999/06/06 07:40:36 strauss Exp $
#

TESTMIBS =	SNMPv2-MIB IF-MIB MAU-MIB

test: test-smilint 

test-smilint: .PHONY
	-rm -rf test-smilint
	mkdir test-smilint
	SMIPATH=@smipath@ ; export SMIPATH
	for mib in $(TESTMIBS) ; do \
	    echo $$mib ; \
	    ../tools/smilint -l9 $$mib > test-smilint/$$mib 2>&1 ; \
	done
	@echo ""
	@echo "see directory test-smilint for results."

test-smidump-smi-sming: .PHONY
	-rm -rf test-smilint-smi-sming
	mkdir test-smidump-smi-sming
	SMIPATH=@smipath@ ; export SMIPATH
	for mib in $(TESTMIBS) ; do \
	    ../tools/smidump -fsming $$mib > test-smidump-smi-sming/$$mib 2>&1 ; \
	done
	@echo ""
	@echo "see directory test-smidump-smi-sming for results."

test-smidump-sming-sming: .PHONY
	-rm -rf test-smilint-sming-sming
	mkdir test-smidump-sming-sming
	SMIPATH=@smipath@ ; export SMIPATH
	for mib in $(TESTMIBS) ; do \
	    ../tools/smidump -fsming $$mib > test-smidump-sming-sming/$$mib 2>&1 ; \
	done
	@echo ""
	@echo "see directory test-smidump-sming-sming for results."

