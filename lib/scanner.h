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
 * @(#) $Id: scanner.h,v 1.5 1999/06/10 15:28:23 strauss Exp $
 */

#include "error.h"



#define MAX_LEX_DEPTH		 20



extern char            *currentString;
extern int	       currentStringLength;
extern int	       currentStringMaxLength;
extern int	       currentStringColumn;
extern int	       currentStringFetchLine; 
extern int	       lexDepth;
#ifdef YY_BUFFER_STATE
extern YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];
#endif


#define thisParser (*(Parser *)parser)
