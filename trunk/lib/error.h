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
 * @(#) $Id: error.h,v 1.12 1999/06/07 15:44:01 strauss Exp $
 */

#ifndef _ERROR_H
#define _ERROR_H

#include "scanner-smi.h"
#include "parser-smi.h"

#include "errormacros.h" /* list of error macros generated from error.c */



#ifdef yyerror
#undef yyerror
#endif
#define yyerror(msg) printError(parserPtr, ERR_OTHER_ERROR, msg)


extern int line;			 /* Current line in source file.     */
extern int column;			 /* Current column in current line.  */
extern int character;			 /* Current absolute byte position.  */

extern int errorLevel;                   /* Higher level for more warnings   */

extern void printError(Parser *parserPtr, int id, ...);

#endif /* _ERROR_H */
