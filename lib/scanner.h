/*
 * scanner.h --
 *
 *      Variables and functions needed by all scanner backends.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner.h,v 1.2 1999/05/25 17:00:33 strauss Exp $
 */

#include "defs.h"    
#include "error.h"



#define MAX_LEX_DEPTH		 20



extern char            *currentString;
extern int	       currentStringLength;
extern int	       currentStringMaxLength;
extern int	       currentStringColumn;
extern int	       currentStringFetchLine; 
extern int	       lexDepth;
extern YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];
 


#define thisParser (*(Parser *)parser)



/* extern void yyerror(const char *msg); */



extern void trackInput(int nl, Module *parser);
			        /* XXX ??? */
