/*
 * yacc2x.l --
 *
 *      Lex scanner for converting yacc sources.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: yacc2x.l,v 1.1 1999/03/11 17:33:57 strauss Exp $
 */

%{
    
#include <stdio.h>
#include <errno.h>

#define YY_NO_UNPUT

#define printActions  0
#define printPrefix   0
#define printComments 0
#define printHtml     0
    
int nest;
 
%}

%s		Apostrophe
%s		Comment
%s		Action
%s		Rules

%%

<Rules>"/*" {
    BEGIN(Comment);
    if (printComments)
	printf("%s", yytext);
}

<Comment>"*/" {
    BEGIN(Rules);
    if (printComments)
	printf("%s", yytext);
}

<Comment>. {
    if (printComments)
	printf("%s", yytext);
}

<Rules>"'" {
    BEGIN(Apostrophe);
    printf("%s", yytext);
}
    
<Apostrophe>"'" {
    BEGIN(Rules);
    printf("%s", yytext);
}

<Rules>"{" {
    nest = 1;
    BEGIN(Action);
    if (printActions)
	printf("%s", yytext);
}

<Action>"{" {
    nest++;
    if (printActions)
	printf("%s", yytext);
}

<Action>"}" {
    if (! --nest) { BEGIN(Rules); }
    if (printActions)
	printf("%s", yytext);
}

<Action>. {
    if (printActions)
	printf("%s", yytext);
}

<Action>\n {
    if (printActions)
	printf("%s", yytext);
}

<INITIAL>"%%" {
    BEGIN(Rules);
    if (printPrefix)
	printf("%s", yytext);
}

<Rules>"%%" {
    BEGIN(INITIAL);
    if (printPrefix)
	printf("%s", yytext);
}

<Rules>^[A-Za-z0-9_-]* {
    if (printHtml)
	printf("<A NAME=\"%s\">%s</A>", yytext, yytext);
    else
	printf("%s", yytext);
}

 /* convention: non-terminals contain at least one lowercase character */
<Rules>[A-Za-z0-9_-]*[a-z][A-Za-z0-9_-]* {
    if (printHtml)
	printf("<A HREF=\"#%s\">%s</A>", yytext, yytext);
    else
	printf("%s", yytext);
}

<INITIAL>. {
    if (printPrefix)
	printf("%s", yytext);
}

<Rules>":" {
    printf(" -> ");
}

<Rules>. {
    printf("%s", yytext);
}

%%
