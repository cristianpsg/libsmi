/*
 * util.c --
 *
 *      Misc utility functions.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: util.c,v 1.1 1999/03/11 17:33:06 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "smi.h"
#include "defs.h"
#include "error.h"
#include "parser-smi.h"



void *
util_malloc(size)
    size_t  size;
{
    return calloc(size, 1);
}



void *
util_realloc(ptr, size)
    void    *ptr;
    size_t  size;
{
    return realloc(ptr, size);
}



char *
util_strdup(s1)
    const char *s1;
{
    return strdup(s1);
}
