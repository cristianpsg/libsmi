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
 * @(#) $Id: util.h,v 1.3 1999/04/09 18:47:04 strauss Exp $
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "smi.h"
#include "defs.h"
#include "error.h"
#include "parser-smi.h"



extern void *util_malloc(size_t size);

extern void *util_realloc(void *ptr, size_t size);

extern char *util_strdup(const char *s1);

extern int util_strcmp(const char *s1, const char *s2);

extern time_t smiMkTime(const char *s);

#endif /* _UTIL_H */
