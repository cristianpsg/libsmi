/*
 * parser-sming.y --
 *
 *      Syntax rules for parsing the SMIng MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

%{

#ifdef BACKEND_SMING
    
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
    
#ifdef linux
#include <getopt.h>
#endif

#include "defs.h"
#include "smi.h"
#include "error.h"
#include "parser-sming.h"
#include "scanner-sming.h"
#include "data.h"
#include "util.h"
    


/*
 * These arguments are passed to yyparse() and yylex().
 */
#define YYPARSE_PARAM parserPtr
#define YYLEX_PARAM   parserPtr

    
    
#define thisParserPtr      ((Parser *)parserPtr)
#define thisModulePtr     (((Parser *)parserPtr)->modulePtr)


    
/*
 * NOTE: The argument lvalp ist not really a void pointer. Unfortunately,
 * we don't know it better at this point. bison generated C code declares
 * YYSTYPE just a few lines below based on the `%union' declaration.
 */
extern int yylex(void *lvalp, Parser *parserPtr);



%}

/*
 * The grammars start symbol.
 */
%start smingFile



/*
 * We call the parser from within the parser when IMPORTing modules,
 * hence we need reentrant parser code. This is a bison feature.
 */
%pure_parser



/*
 * The attributes.
 */
%union {
    char           *id;				/* identifier name           */
    int            err;				/* actually just a dummy     */
}



/*
 * Tokens and their attributes.
 */
%token DOT_DOT
%token COLON_COLON

%token <id>ucIdentifier
%token <id>lcIdentifier

%token <id>moduleKeyword



/*
 * Types of non-terminal symbols.
 */

%type <err>smingFile
%type <err>moduleStatement_optsep_0n
%type <err>moduleStatement_optsep_1n
%type <err>moduleStatement_optsep
%type <err>moduleStatement

%%

/*
 * Yacc rules.
 *
 */


/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
smingFile:		optsep moduleStatement_optsep_0n
    			{ $$ = 0; }
	;

moduleStatement_optsep_0n:	/* empty */
			{ $$ = 0; }
	|		moduleStatement_optsep_1n
			{ $$ = 0; }
	;

moduleStatement_optsep_1n:	moduleStatement_optsep
			{ $$ = 0; }
	|		moduleStatement_optsep_1n moduleStatement_optsep
			{ $$ = 0; }
        ;

moduleStatement_optsep:	moduleStatement optsep
			{ $$ = 0; }

moduleStatement:	moduleKeyword sep ucIdentifier
			sep
			lcIdentifier
			optsep
			{ printf("X1\n"); }
			"{"
			{ printf("X2\n"); }
			stmtsep
			{ printf("X3\n"); }
			"}" optsep ";"
			{
			    printf("XXX sming module\n");
			    $$ = 0;
			}
	;

optsep:			/* empty */
        ;			

sep:			/* empty */
        ;			

stmtsep:		/* empty */
        ;			

%%

#endif
			
