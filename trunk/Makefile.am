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
# @(#) $Id: Makefile.am,v 1.21 2001/11/27 16:31:49 strauss Exp $
#

AUTOMAKE_OPTIONS	= no-dependencies foreign

EXTRA_DIST		= ANNOUNCE \
			  win/Makefile.mingw win/README.mingw win/README.win \
			  win/config.h win/config.h.in \
			  win/makefile win/GNUmakefile win/win.c win/win.h \
			  smi.conf-example libsmi.m4

SUBDIRS			= lib tools mibs test doc

m4datadir		= $(datadir)/aclocal
m4data_DATA		= libsmi.m4

anoncvs:
	rsync -a --delete $(HOME)/.cvs-repository/libsmi /.amd/anoncvs
	chgrp -R anoncvs /.amd/anoncvs/libsmi
	chmod -R g+w /.amd/anoncvs/libsmi
#
#	mkdir /tmp/libsmi-diff
#	tar xzCf /tmp/libsmi-diff /ftp/local/libsmi/libsmi-current.tar.gz
#	cd /tmp/libsmi-diff ; cvs -Q co libsmi
#	-cd /tmp/libsmi-diff ; diff -r -u -N libsmi-* libsmi > /ftp/local/libsmi/libsmi-current-cvs.diff
#	rm -rf /tmp/libsmi-diff
