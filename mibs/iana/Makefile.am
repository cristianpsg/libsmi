#
# Makefile.am --
#
#      Template to generate the mibs/iana Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.1 1999/12/12 12:21:05 strauss Exp $
#

ianadir			= $(mibdir)/iana
MIBS			= $(shell ls -1 $(srcdir) | egrep -v 'CVS|Makefile')
EXTRA_DIST		= $(MIBS)
iana_DATA		= $(MIBS)
