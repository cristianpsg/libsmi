#
# This is the libsmi/src Makefile.
#
# @(#) $Id: Makefile,v 1.19 1998/12/04 14:36:44 strauss Exp $
#
#
# NOTE: We need flex and bison. Lex and yacc do not work.
#

CC	= gcc
CFLAGS	= -fPIC -Wall -g -DTEXTS_IN_MEMORY=200 -DPARSER -DRPC_SVC_FG -DDEBUG
LEX	= flex
YACC	= bison -y -v -t
RPCGEN	= rpcgen

LOBJS	= config.o scanner-flex.o parser-bison.o error.o data.o \
	  smi.o smi_clnt.o smi_xdr.o


default: libsmi.a miblint smid smiclient smiquery

smid.c smi_xdr.c smi_clnt.c smi.h: smi.x smi.h-add
	$(RPCGEN) smi.x
	cat smi.h-add >> smi.h
	cat smi_svc.c | awk '{if ($$0 ~ "^main") { x=1 } ; if (x && ($$0 == "")) { printf "\n        smi_svc_init(argc, argv);\n\n" ; x=0 } else { print } }' | sed -e 's/main(.*)/main(int argc, char *argv[])/' > smid.c

smid: smi_svc_proc.o smi_svc_init.o smid.o smi_xdr.o libsmi.a
	#$(CC) -o smid smi_svc_proc.o smi_svc_init.o smid.o smi_xdr.o libsmi.a -lrpcsvc -lnsl -ll -lefence
	$(CC) -o smid smi_svc_proc.o smi_svc_init.o smid.o smi_xdr.o libsmi.a -lrpcsvc -lnsl -ll
	ln -f smid ..

smiclient: smiclient.o smi_clnt.o smi_xdr.o
	$(CC) -o smiclient smiclient.o smi_clnt.o smi_xdr.o -lnsl
	ln -f smiclient ..

smiquery: smiquery.o smi.h libsmi.a
	$(CC) -o smiquery smiquery.o libsmi.a -lnsl -ll
	ln -f smiquery ..

scanner-flex.c: scanner.l scanner.h parser-bison.h
	$(LEX) -t scanner.l > scanner-flex.c

parser-bison.c parser-bison.h: parser.y scanner.h parser.h
	$(YACC) -d parser.y
	mv y.tab.c parser-bison-buggy.c
	mv y.tab.h parser-bison.h
#	there's a slight bug in bison 1.25 with pure reentrant parsers:
	sed -e 's/int yyparse (void);/int yyparse ();/' \
	    parser-bison-buggy.c > parser-bison.c

libsmi.a: $(LOBJS)
	ar ruv libsmi.a $(LOBJS)
	ranlib libsmi.a
	ln -f smi.h ../smi.h
	ln -f libsmi.a ../libsmi.a

libsmi.so: $(LOBJS)
	$(LD) -G -z text $(LOBJS) -o libsmi.so -ll -lnsl
	ln -f libsmi.so ..

miblint: libsmi.a lint.o
	$(CC) -o miblint lint.o libsmi.a -ll -lnsl
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
	rm -f error.o lint.o data.o interface.o config.o smiclient.o
	rm -f smi_clnt.o smiclient smi_svc_init.o smid.c smiquery smiquery.o
	rm -f core miblint libsmi.a smid.o smid smi_svc.o smid.o smi_xdr.o
	rm -f smi.h smi_svc_proc.o smi_xdr.c smi_svc.c smi_clnt.c smi.o
	rm -f y.tab.c y.tab.h y.output yacc.debug yacc.acts yacc.tmp
	rm -f yacc2html.c yacc2html.o yacc2html parser.y.html TAGS

smi.o: smi.c smi.h

smi_xdr.o: smi_xdr.c smi.h

smi_svc_proc.o: smi_svc_proc.c smi.h

smid.o: smid.c

smi_svc_init.o: smi_svc_init.c 

scanner-flex.o: scanner-flex.c scanner.h error.h defs.h

parser-bison.o: parser-bison.c parser-bison.h parser.h error.h defs.h config.h

config.o: parser-bison.c config.c config.h defs.h smi.h

error.o: error.c error.h defs.h smi.h

data.o: data.c data.h defs.h smi.h

lint.o: lint.c error.h defs.h smi.h

smi_clnt.o: smi_clnt.c

smiclient.o: smiclient.c

smiquery.o: smiquery.c


