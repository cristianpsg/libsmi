/*
 * scanner.h --
 *
 *      Definition of lexical tokens of the MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner.h,v 1.12 1998/08/24 00:00:29 strauss Exp $
 */

#ifndef _SCANNER_H
#define _SCANNER_H

#include <stdio.h>

#include "parser.h"

#define YY_NO_UNPUT

/*
 *----------------------------------------------------------------
 * Defines for MIB module language specific use.
 *----------------------------------------------------------------
 */

#define MAX_NUMBER		"18446744073709551615" /* max Counter64 */
#define MAX_BIN_STRING_LENGTH	128
#define MAX_HEX_STRING_LENGTH	128
#define MAX_QUOTED_STRING_LENGTH 65535

#define MAX_LEX_DEPTH		 20


extern void yyerror(const char *msg);

/* we need a reentrant parser, so yylex gets an argument */
#define YY_DECL int yylex YY_PROTO((YYSTYPE *lvalp, void *parser))

extern char *yytext;
extern int yyleng;

extern int enterLexRecursion(FILE *file);
extern void leaveLexRecursion();
extern int lexDepth;

#endif /* _SCANNER_H */
