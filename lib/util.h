/*
 * util.h --
 *
 *      Misc utility functions.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: util.h,v 1.2 1999/03/25 21:57:48 strauss Exp $
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "smi.h"
#include "defs.h"
#include "error.h"
#include "parser-smi.h"



void *util_malloc(size_t size);

void *util_realloc(void *ptr, size_t size);

char *util_strdup(const char *s1);

int util_strcmp(const char *s1, const char *s2);


#endif /* _UTIL_H */
