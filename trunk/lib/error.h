/*
 * error.h --
 *
 *      Definitions for error handling.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: error.h,v 1.17 2000/06/14 13:15:16 strauss Exp $
 */

#ifndef _ERROR_H
#define _ERROR_H

#include "data.h"
#include "errormacros.h" /* list of error macros generated from error.c */



#ifdef yyerror
#undef yyerror
#endif
#define yyerror(msg) smiPrintError(parserPtr, ERR_OTHER_ERROR, msg)


extern int smiErrorLevel;	/* Higher levels produce more warnings */

extern void smiSetErrorSeverity(char *pattern, int severity);

extern int smiGetErrorSeverity(int id);

extern char* smiGetErrorTag(int id);

extern char* smiGetErrorMsg(int id);

extern void smiPrintError(Parser *parser, int id, ...);

extern void smiPrintErrorAtLine(Parser *parser, int id, int line, ...);

#endif /* _ERROR_H */
