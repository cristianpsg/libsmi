#
# This is the libsmi Makefile.
#
# @(#) $Id: Makefile,v 1.5 1998/11/25 14:36:33 strauss Exp $
#

MIBDIR	= ../scotty/tnm/mibs
PREFIX	= /usr/local

all: subdirs

subdirs:
	make -C src

test: test-miblint

install: install-prg install-html

test-miblint: miblint
	for f in ${MIBDIR}/* ; do \
	    if [ -f $$f ] ; then ./miblint -l9 $$f ; fi ; \
	done

miblint:
	make -C src miblint

libsmi.a:
	make -C src libsmi.a

parser.y.html: src/parser.y
	make -C src parser.y.html

install-prg: miblint mibs.conf
	cp smi.conf ${PREFIX}/etc/smi.conf
	cp smid.conf ${PREFIX}/etc/smid.conf
	cp smiquery.conf ${PREFIX}/etc/smiquery.conf
	#cp smiscotty.conf ${PREFIX}/etc/smiscotty.conf
	cp miblint ${PREFIX}/bin
	cp smiclient ${PREFIX}/bin
	cp smiquery ${PREFIX}/bin
	cp smid ${PREFIX}/sbin
	cp smi.h ${PREFIX}/include
	cp libsmi.a ${PREFIX}/lib
	cp miblint.1 ${PREFIX}/man/man1/miblint.1
	if [ ! -d ${PREFIX}/lib/smi ] ; then mkdir ${PREFIX}/lib/smi ; fi
	if [ ! -d ${PREFIX}/lib/smi/mibs ] ; then mkdir ${PREFIX}/lib/smi/mibs ; fi
	cp mibs/SNMP* mibs/RFC* ${PREFIX}/lib/smi/mibs

install-html: parser.y.html
	if [ -d /usr/home/strauss/WWW/sming ] ; then \
		cp parser.y.html /usr/home/strauss/WWW/sming ; \
	fi

clean:
	make -C src clean
	rm -f core

clobber: clean
	rm -f miblint libsmi.h libsmi.a parser.y.html smid smiclient

dist:
	rm -f mibs.tar.gz
	cd .. ; tar cvf mibs/mibs.tar libsmi
	gzip mibs.tar

