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
# @(#) $Id: Makefile.am,v 1.2 1999/10/06 17:37:18 strauss Exp $
#

MIBDIRS			= iana ietf irtf site

MIBS			= $(shell ls -1d [a-z]*/* | egrep -v 'CVS|Makefile')
mib_DATA		= $(MIBS)
SUBDIRS			= $(MIBDIRS)

install-exec-local:
	for dir in $(MIBDIRS) ; do \
		$(mkinstalldirs) $(DESTDIR)$(mibdir)/$$dir ; \
	done
