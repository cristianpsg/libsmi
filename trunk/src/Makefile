#
# This is the libsmi/src Makefile.
#
# @(#) $Id: Makefile,v 1.2 1998/10/13 17:11:42 strauss Exp $
#

CC	= gcc
CFLAGS	= -Wall -g -DTEXTS_IN_MEMORY=200

# We need flex and bison. Lex and yacc do not work.
LEX	= flex
YACC	= bison -y -v -t

LOBJS	= config.o scanner-flex.o parser-bison.o error.o data.o interface.o

default: libsmi.a libsmi.h miblint


scanner-flex.c: scanner.l scanner.h parser-bison.h
	$(LEX) -t scanner.l > scanner-flex.c

parser-bison.c parser-bison.h: parser.y scanner.h parser.h
	$(YACC) -d parser.y
	mv y.tab.c parser-bison-buggy.c
	mv y.tab.h parser-bison.h
#	there's a slight bug in bison 1.25 with pure reentrant parsers:
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    parser-bison-buggy.c > parser-bison.c

scanner-flex.o: scanner-flex.c scanner.h error.h defs.h

parser-bison.o: parser-bison.c parser-bison.h parser.h error.h defs.h config.h

config.o: parser-bison.c config.c config.h defs.h

error.o: error.c error.h defs.h

data.o: data.c data.h defs.h

interface.o: parser-bison.c interface.c interface.h defs.h

lint.o: lint.c error.h defs.h

libsmi.h: interface.h
	ln -f interface.h libsmi.h
	ln -f interface.h ../libsmi.h

libsmi.a: $(LOBJS)
	  ar ruv libsmi.a $(LOBJS)
	  ranlib libsmi.a
	  ln -f libsmi.a ../libsmi.a

miblint: libsmi.a lint.o
	$(CC) -o miblint lint.o libsmi.a -ll
	ln -f miblint ..


yacc2html.c: yacc2html.l
	$(LEX) -t yacc2html.l > yacc2html.c

yacc2html: yacc2html.o
	$(CC) -o yacc2html yacc2html.o -ll

parser.y.html: parser.y yacc2html
	echo '<HTML><HEAD><TITLE>parser.y</TITLE></HEAD><BODY><PRE>' \
	    > parser.y.html
	cat parser.y | \
	    sed -e 's!<!\&lt;!g' -e 's!>!\&gt;!g' | \
	    ./yacc2html | \
	    sed -e 's!REF:RFC\([0-9]*\),\([0-9\.]*\)!REF:<A HREF="http://www.ibr.cs.tu-bs.de/doc/rfc?PATTERN=\1#\2">RFC\1,\2</A>!g' \
	    >> parser.y.html
	echo '</PRE></BODY></HTML>' >> parser.y.html
	ln -f parser.y.html ..

clean:
	rm -f scanner-flex.c scanner-flex.o
	rm -f parser-bison.c parser-bison-buggy.c parser-bison.h parser-bison.o
	rm -f error.o lint.o data.o interface.o config.o
	rm -f core
	rm -f miblint libsmi.h libsmi.a
	rm -f y.tab.c y.tab.h y.output yacc.debug yacc.acts yacc.tmp
	rm -f yacc2html.c yacc2html.o yacc2html parser.y.html
