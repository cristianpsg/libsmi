/*
 * scanner.c --
 *
 *      Variables and functions needed by all scanner backends.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#if defined(BACKEND_SMI) || defined(BACKEND_SMING)

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "defs.h"    
#include "error.h"



#define thisParser      (*(Parser *)parser)



char *currentString = NULL;
int currentStringLength = 0;
int currentStringMaxLength = 0;
int currentStringColumn = 0;
int currentStringFetchLine = 0; 

int lexDepth = 0;



#endif
