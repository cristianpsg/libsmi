/*
 * scanner-smi.h --
 *
 *      Definition of lexical tokens of the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner.h,v 1.1.1.1 1998/10/09 10:16:33 strauss Exp $
 */

#ifndef _SCANNER_SMI_H
#define _SCANNER_SMI_H

#include <stdio.h>

#include "parser-smi.h"

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

#endif /* _SCANNER_SMI_H */
