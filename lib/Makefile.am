#
# Makefile.am --
#
#      Template to generate libsmi Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.10 1999/12/14 15:02:58 strauss Exp $
#

EXTRA_DIST		= parser-sming.y parser-smi.y \
			  scanner-sming.l scanner-smi.l \
			  data.h error.h util.h scanner-sming.h \
			  parser-smi.h parser-sming.h scanner-smi.h \
			  parser-smi.tab.h parser-sming.tab.h \
			  $(man_MANS)
include_HEADERS		= smi.h
CLEANFILES		= parser-smi.output parser-sming.output
man_MANS		= libsmi.3 smi_config.3 smi_module.3 smi_macro.3 \
			  smi_node.3 smi_type.3
lib_LTLIBRARIES		= libsmi.la
libsmi_la_SOURCES	= data.c error.c util.c smi.c \
		  	  parser-smi.tab.c scanner-smi.c \
		  	  parser-sming.tab.c scanner-sming.c
libsmi_la_LDFLAGS	= -version-info @LIBTOOL_VERSIONING@

parser-smi.tab.c: parser-smi.y scanner-smi.h parser-smi.h
	@if [ "$(BISON)" ] ; then \
	    "$(BISON)" -v -t -d -psmi parser-smi.y ; \
	else \
	    echo "" ; \
	    echo "NOTE: we cannot build the new parser-smi.tab.c from parser-smi.y" ; \
	    echo "" ; \
	fi
#	# bison-1.25 has a wrong yyparse() definition for pure reentrant code.
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    parser-smi.tab.c > parser-smi.tab.c.tmp && \
	mv parser-smi.tab.c.tmp parser-smi.tab.c

parser-sming.tab.c: parser-sming.y scanner-sming.h parser-sming.h
	@if [ "$(BISON)" ] ; then \
	    "$(BISON)" -v -t -d -psming parser-sming.y ; \
	else \
	    echo "" ; \
	    echo "NOTE: we cannot build the new parser-sming.tab.c from parser-sming.y" ; \
	    echo "" ; \
	fi
#	# bison-1.25 has a wrong yyparse() definition for pure reentrant code.
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    parser-sming.tab.c > parser-sming.tab.c.tmp && \
	mv parser-sming.tab.c.tmp parser-sming.tab.c

scanner-smi.c: scanner-smi.l scanner-smi.h parser-smi.tab.h
	@if [ "$(FLEX)" ] ; then \
	    "$(FLEX)" -Psmi -t scanner-smi.l > scanner-smi.c ; \
	else \
	    echo "" ; \
	    echo "NOTE: we cannot build the new scanner-smi.c from scanner-smi.l" ; \
	    echo "" ; \
	fi

scanner-sming.c: scanner-sming.l scanner-sming.h parser-sming.tab.h
	@if [ "$(FLEX)" ] ; then \
	    "$(FLEX)" -Psming -t scanner-sming.l > scanner-sming.c ; \
	else \
	    echo "" ; \
	    echo "NOTE: we cannot build the new scanner-sming.c from scanner-sming.l" ; \
	    echo "" ; \
	fi

error.h data.lo: errormacros.h

errormacros.h: error.c
	cat error.c | grep ERR_ | \
	sed -e 's/^.*\(ERR_[a-zA-Z0-9_]*\).*$$/\1/' | \
	awk '{printf "#define %-40s %d\n", $$1, NR-1}' > errormacros.h
