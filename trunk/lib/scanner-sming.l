/*
 * scanner-sming.l --
 *
 *      Lexical rules for scanning the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner-smi.l,v 1.3 1999/03/16 17:24:10 strauss Exp $
 */

%option noyywrap

%{
    
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "defs.h"    
#include "error.h"
#include "parser-sming.h"
#include "parser-sming.tab.h"
#include "scanner.h"
#include "scanner-sming.h"


    
#define MAX_NUMBER		"18446744073709551615" /* max Counter64 */
#define MAX_QUOTED_STRING_LENGTH 65535



/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)



YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];


 
int
smingEnterLexRecursion(file)
    FILE *file;
{
    if (lexDepth >= MAX_LEX_DEPTH) {
	return (-1);
    }
    yybuffer[lexDepth++] = YY_CURRENT_BUFFER;
    yy_switch_to_buffer(yy_create_buffer(file, YY_BUF_SIZE));
    return (lexDepth);
}


 
void
smingLeaveLexRecursion()
{    
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(yybuffer[--lexDepth]);
}



void
smingTrackInput(nl, parser)
    int nl;
    Module *parser;
{
    if (nl) {
	thisParser.line += 1;
	thisParser.column = 1;
	thisParser.character += yyleng;
	thisParser.linebuf[0] = 0;
    } else {
	if ((yytext[0] != '\t') || (yyleng != 1)) {
	    thisParser.column += yyleng;
	    if (thisParser.column+1 < sizeof(thisParser.linebuf)) {
		strcat(thisParser.linebuf, yytext);
	    }
	} else {
	    thisParser.column += (8 - ((thisParser.column-1) % 8));
	    if (thisParser.column+1 < sizeof(thisParser.linebuf)) {
		strcat(thisParser.linebuf, yytext);
	    }
	}
	thisParser.character += yyleng;
    }
}


 
%}



/*
 * Lex pattern definitions.
 */
delim		([^a-zA-Z0-9-])
eol             ("\n"|"\n\015"|"\015\n")



/*
 * Lex state definitions.
 */
%s		Comment
%s		String



%%



 /*
  * Lex rules.
  */

<INITIAL>"//" {
    smingTrackInput(0, parser);
    BEGIN(Comment);
}

<Comment>{eol} {
    smingTrackInput(1, parser);
    BEGIN(INITIAL);
}

<Comment>. {
    smingTrackInput(0, parser);
}

 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\{\}\(\)\;\,\-\.\|] {
    smingTrackInput(0, parser);
    return yytext[0];
}
    
<INITIAL>".." {
    smingTrackInput(0, parser);
    return DOT_DOT;
}

<INITIAL>"::" {
    smingTrackInput(0, parser);
    return COLON_COLON;
}

 /*
  * Lex rules for white space.
  */

<INITIAL>[ \t] {
    smingTrackInput(0, parser);
}

<INITIAL>{eol} {
    smingTrackInput(1, parser);
}

 /*
  * Lex rules for known keywords.
  */

<INITIAL>module/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return moduleKeyword;
}

 /*
  * Lex rules for descriptors.
  */

 /* e.g. module names: REF: draft,p.12-13 */
<INITIAL>[A-Z](-?[a-zA-Z0-9]+)*-? {
    smingTrackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    yylval.id = strdup(yytext);
    return ucIdentifier;
}

 /* same for lowercase names */
<INITIAL>[a-z](-?[a-zA-Z0-9]+)*-? {
    smingTrackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    yylval.id = strdup(yytext);
    return lcIdentifier;
}

 /*
  *
  */

. {
    smingTrackInput(0, parser);
    printError(parser, ERR_LEX_UNEXPECTED_CHAR);
}

%%
