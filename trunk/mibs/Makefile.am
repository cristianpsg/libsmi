#
# Makefile.am --
#
#      Template to generate the mibs Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.6 1999/10/05 16:10:41 strauss Exp $
#

MIBDIRS			= $(shell ls -1d [a-z]*')
MIBS			= $(shell ls -1 [a-z]*/* | egrep -v 'CVS|Makefile')
EXTRA_DIST		= $(MIBS)
mib_DATA		= $(MIBS)

install-exec-local:
	for dir in $(MIBDIRS) ; do \
		$(mkinstalldirs) $(DESTDIR)$(mibdir)/$$dir ; \
	done
