#
# Makefile.am --
#
#      Template to generate the toplevel Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.10 1999/12/14 15:34:42 strauss Exp $
#

AUTOMAKE_OPTIONS	= no-dependencies foreign

EXTRA_DIST		= ANNOUNCE

SUBDIRS			= lib tools mibs test doc

anoncvs:
	rsync -a $(HOME)/.cvs-repository/libsmi /usr/local/cvsroot-anon
	chgrp -R anoncvs /usr/local/cvsroot-anon/libsmi
	chmod -R g+w /usr/local/cvsroot-anon/libsmi
