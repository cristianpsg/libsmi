#
# This is the libsmi Makefile.
#
# @(#) $Id: Makefile,v 1.14 1999/03/23 22:55:36 strauss Exp $
#

MIBDIR		= ../scotty/tnm/mibs
PREFIX		= /usr/local

DEFINES		= -DRPC_SVC_FG -DDEBUG -DBACKEND_RPC -DBACKEND_SMI -DBACKEND_SMING
CC		= gcc
CFLAGS		= -I. -Ilib -Wall -g $(DEFINES)
LD		= gcc
#LDFLAGS		= -ldbmalloc
LDFLAGS		= 
RANLIB		= ranlib
AR		= ar
RPCGEN		= rpcgen
BISON		= bison
FLEX		= flex

LIBSMI_OBJS	= lib/data.o lib/error.o lib/util.o lib/smi.o \
		  lib/parser-smi.tab.o lib/scanner-smi.o \
		  lib/smi_clnt.o lib/smi_xdr.o

LIBSMI_STATIC	= lib/libsmi.a

all: tools/smilint tools/smidump tools/smiquery


tools/smid.c lib/smi.h lib/smi_xdr.c lib/smi_clnt.c: lib/smi.h-add lib/smi.x
	$(RPCGEN) lib/smi.x
#	# add some definitions to smi.h
	cat lib/smi.h-add >> lib/smi.h
#	# patch the main function created by rpcgen so that it calls
#	# smi_svc_init() and write the patched file to tools/smid.c
	cat lib/smi_svc.c | awk '{if ($$0 ~ "^main") { x=1 } ; if (x && ($$0 == "")) { printf "\n        smi_svc_init(argc, argv);\n\n" ; x=0 } else { print } }' | sed -e 's/main(.*)/main(int argc, char *argv[])/' > tools/smid.c

lib/parser-smi.tab.c lib/parser-smi.tab.h: lib/parser-smi.y lib/scanner-smi.h lib/parser-smi.h
	$(BISON) -v -t -d lib/parser-smi.y
#	# bison-1.25 has a wrong yyparse() definition for pure reentrant code.
#	# bison-1.27 is ok and does not need this.
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    lib/parser-smi.tab.c > lib/parser-smi.tab.c.tmp && \
	mv lib/parser-smi.tab.c.tmp lib/parser-smi.tab.c



lib/scanner-smi.c: lib/scanner-smi.l lib/scanner-smi.h lib/parser-smi.tab.h
	$(FLEX) -t lib/scanner-smi.l > lib/scanner-smi.c

lib/smi_xdr.o: lib/smi_xdr.c
#	# Linux' rpcgen produced code that leads to warnings otherwise.
	$(CC) $(CFLAGS) -Wno-unused -c $< -o $@

$(LIBSMI_STATIC): lib/smi.h $(LIBSMI_OBJS)
	$(AR) ruv $@ $(LIBSMI_OBJS)
	$(RANLIB) $@
	
tools: tools/smilint tools/smidump tools/smiquery tools/smiclient tools/smid

tools/smilint: $(LIBSMI_STATIC) tools/smilint.o
	$(LD) $(LDFLAGS) -o tools/smilint tools/smilint.o $(LIBSMI_STATIC) -ll -lnsl

tools/smidump: tools/smidump.o tools/dump-sming.o tools/dump-data.o $(LIBSMI_STATIC)
	$(LD) $(LDFLAGS) -o tools/smidump $^ -ll -lnsl

tools/smiquery: $(LIBSMI_STATIC) tools/smiquery.o
	$(LD) $(LDFLAGS) -o tools/smiquery tools/smiquery.o $(LIBSMI_STATIC) -ll -lnsl

clean:
	rm -f lib/*.o lib/*.a lib/*.tab.[hc] lib/*.tab.c.tmp lib/scanner-smi.c lib/smi.h lib/smi_xdr.c lib/smi_clnt.c lib/smi_svc.c lib/*.output tools/*.o tools/smid.c core */core doc/parser.y.html

install: install-prg install-conf install-dev install-lib install-html

#install-prg: tools/smilint tools/smidump tools/smiclient tools/smiquery tools/smid doc/smilint.1
install-prg: tools/smilint tools/smidump tools/smiquery doc/smilint.1
	cp tools/smilint tools/smidump tools/smiclient tools/smiquery tools/smid ${PREFIX}/bin
	cp doc/smilint.1 ${PREFIX}/man/man1

install-conf:
	cp etc/smi*.conf ${PREFIX}/etc

install-dev: lib/smi.h lib/libsmi.a
	cp lib/smi.h ${PREFIX}/include
	cp lib/libsmi.a ${PREFIX}/lib

install-lib:
	if [ ! -d ${PREFIX}/lib/smi ] ; then mkdir ${PREFIX}/lib/smi ; fi
	if [ ! -d ${PREFIX}/lib/smi/mibs ] ; then mkdir ${PREFIX}/lib/smi/mibs ; fi
	cp mibs/SNMP* mibs/RFC* ${PREFIX}/lib/smi/mibs

install-html: doc/parser.y.html
#	if [ -d /usr/home/strauss/WWW/sming ] ; then \
#		cp doc/parser.y.html /usr/home/strauss/WWW/sming ; \
#	fi

doc/parser.y.html: lib/parser-smi.y
#	make -C src parser.y.html




clobber: clean
	rm -f miblint libsmi.h libsmi.a parser.y.html smid smiclient smiquery

dist:
	rm -f mibs.tar.gz
	cd .. ; tar cvf mibs/mibs.tar libsmi
	gzip mibs.tar

test: test-miblint

test-miblint: miblint
	for f in ${MIBDIR}/* ; do \
	    if [ -f $$f ] ; then ./miblint -l9 $$f ; fi ; \
	done

