#
# This is the libsmi Makefile.
#
# @(#) $Id: Makefile,v 1.3 1998/11/04 02:14:48 strauss Exp $
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
	cp mibs.conf ${PREFIX}/etc/mibs.conf
	cp miblint ${PREFIX}/bin
	cp libsmi.a ${PREFIX}/lib
	cp miblint.1 ${PREFIX}/man/man1/miblint.1
	if [ ! -d ${PREFIX}/lib/mibs ] ; then mkdir ${PREFIX}/lib/mibs ; fi
	cp mibs/SNMP* mibs/RFC* ${PREFIX}/lib/mibs

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

