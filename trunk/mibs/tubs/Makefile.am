#
# Makefile.am --
#
#      Template to generate the mibs/tubs Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.1 2000/02/11 11:08:31 strauss Exp $
#

ietfdir			= $(mibdir)/ietf
MIBS			= $(shell ls -1 $(srcdir) | egrep -v 'CVS|Makefile')
EXTRA_DIST              = $(MIBS)
ietf_DATA		= $(MIBS)
