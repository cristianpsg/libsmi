#
# Makefile.am --
#
#      Template to generate the pibs Makefile.in using automake.
#
# Copyright (c) 2002 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.4 2000/02/11 11:08:27 strauss Exp $
#

PIBDIRS			= ietf tubs site

PIBS			= $(shell ls -1d [a-z]*/* | egrep -v 'CVS|Makefile')
pib_DATA		= $(PIBS)
SUBDIRS			= $(PIBDIRS)

install-exec-local:
	for dir in $(PIBDIRS) ; do \
		$(mkinstalldirs) $(DESTDIR)$(pibdir)/$$dir ; \
	done
