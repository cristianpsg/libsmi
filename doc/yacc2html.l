/*
 * yacc2html.l --
 *
 *      Lex scanner for HTMLifying yacc sources.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: yacc2html.l,v 1.1 1999/03/11 17:33:56 strauss Exp $
 */

%{
    
#include <stdio.h>
#include <errno.h>

#define YY_NO_UNPUT

#define printActions 0
    
int nest;
 
%}

%s		Apostrophe
%s		Comment
%s		Action
%s		Rules

%%

<Rules>"/*" {
    BEGIN(Comment);
    printf("%s", yytext);
}

<Comment>"*/" {
    BEGIN(Rules);
    printf("%s", yytext);
}

<Comment>. {
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
    printf("%s", yytext);
}

<Rules>"%%" {
    BEGIN(INITIAL);
    printf("%s", yytext);
}

<Rules>^[A-Za-z0-9_-]* {
    printf("<A NAME=\"%s\">%s</A>", yytext, yytext);
}

 /* convention: non-terminals contain at least one lowercase character */
<Rules>[A-Za-z0-9_-]*[a-z][A-Za-z0-9_-]* {
    printf("<A HREF=\"#%s\">%s</A>", yytext, yytext);
}

<INITIAL,Rules>. {
    printf("%s", yytext);
}

%%
