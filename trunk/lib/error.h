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
 * @(#) $Id: error.h,v 1.15 1999/12/13 16:15:59 strauss Exp $
 */

#ifndef _ERROR_H
#define _ERROR_H

#include "data.h"
#include "errormacros.h" /* list of error macros generated from error.c */



#ifdef yyerror
#undef yyerror
#endif
#define yyerror(msg) printError(parserPtr, ERR_OTHER_ERROR, msg)


extern int errorLevel;                   /* Higher level for more warnings   */

extern void errorSeverity(char *pattern, int severity);

extern void printError(Parser *parserPtr, int id, ...);

extern void printErrorAtLine(Parser *parserPtr, int id, int line, ...);

#endif /* _ERROR_H */
