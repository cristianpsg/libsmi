/*
 * scanner-smi.h --
 *
 *      Definition of lexical tokens of the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner-smi.h,v 1.4 1999/05/31 11:58:34 strauss Exp $
 */

#ifndef _SCANNER_SMI_H
#define _SCANNER_SMI_H

#include <stdio.h>

#include "parser-smi.h"

#define YY_NO_UNPUT

extern char *yytext;
extern int yyleng;

extern int smiEnterLexRecursion(FILE *file);
extern void smiLeaveLexRecursion();

#endif /* _SCANNER_SMI_H */
