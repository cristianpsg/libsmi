/*
 * scanner-sming.h --
 *
 *      Definition of lexical tokens of the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner-sming.h,v 1.3 1999/05/31 11:58:35 strauss Exp $
 */

#ifndef _SCANNER_SMING_H
#define _SCANNER_SMING_H

#include <stdio.h>

#include "parser-sming.h"

#define YY_NO_UNPUT

extern char *yytext;
extern int yyleng;

extern int smingEnterLexRecursion(FILE *file);
extern void smingLeaveLexRecursion();

#endif /* _SCANNER_SMING_H */
