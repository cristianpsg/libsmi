#
# Makefile.am --
#
#      Template to generate libsmi Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# @(#) $Id$
#

AUTOMAKE_OPTIONS	= no-dependencies foreign

EXTRA_DIST		= parser-sming.y parser-smi.y \
			  scanner-sming.l scanner-smi.l \
			  defs.h data.h error.h util.h scanner-sming.h \
			  parser-smi.h parser-sming.h scanner-smi.h scanner.h \
			  parser-smi.tab.h parser-sming.tab.h
include_HEADERS		= smi.h
CLEANFILES		= parser-smi.output parser-sming.output
lib_LTLIBRARIES		= libsmi.la
libsmi_la_SOURCES	= data.c error.c util.c smi.c scanner.c \
		  	  parser-smi.tab.c scanner-smi.c \
		  	  parser-sming.tab.c scanner-sming.c
libsmi_la_LDFLAGS	= -version-info 1:1:0

parser-smi.tab.c: parser-smi.y scanner-smi.h parser-smi.h
	$(BISON) -v -t -d -psmi parser-smi.y
#	# bison-1.25 has a wrong yyparse() definition for pure reentrant code.
#	# bison-1.27 is ok and would not the lines below.
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    parser-smi.tab.c > parser-smi.tab.c.tmp && \
	mv parser-smi.tab.c.tmp parser-smi.tab.c

parser-sming.tab.c: parser-sming.y scanner-sming.h parser-sming.h
	$(BISON) -v -t -d -psming parser-sming.y
#	# bison-1.25 has a wrong yyparse() definition for pure reentrant code.
#	# bison-1.27 is ok and would not the lines below.
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    parser-sming.tab.c > parser-sming.tab.c.tmp && \
	mv parser-sming.tab.c.tmp parser-sming.tab.c

scanner-smi.c: scanner-smi.l scanner-smi.h parser-smi.tab.h
	$(FLEX) -Psmi -t scanner-smi.l > scanner-smi.c

scanner-sming.c: scanner-sming.l scanner-sming.h parser-sming.tab.h
	$(FLEX) -Psming -t scanner-sming.l > scanner-sming.c

