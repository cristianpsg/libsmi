#
# Makefile.am --
#
#      Template to generate the mibs/irtf Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.2 2003/12/01 12:24:21 strauss Exp $
#

irtfdir			= $(mibdir)/irtf
MIBS			= $(shell ls -1 $(srcdir) | egrep -v 'CVS|Makefile')
EXTRA_DIST              = $(MIBS)
irtf_DATA		= $(MIBS)
