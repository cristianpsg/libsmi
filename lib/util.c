/*
 * util.c --
 *
 *      Misc utility functions.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: util.c,v 1.15 2000/02/07 23:23:29 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "util.h"


void *util_calloc(size_t n, size_t size)
{
    char *m = calloc(n, size);
    if (! m) {
	printError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

void *util_malloc(size_t size)
{
    char *m = calloc(1, size);
    if (! m) {
	printError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

void *util_realloc(void *ptr, size_t size)
{
    char *m = realloc(ptr, size);
    if (! m) {
	printError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

char *util_strdup(const char *s1)
{
    if (s1) {
	char *m = strdup(s1);
	if (! m) {
	    printError(NULL, ERR_OUT_OF_MEMORY);
	}
	return m;
    } else {
	return NULL;
    }
}

char *util_strndup(const char *s1, size_t n)
{
    char *p;
    
    p = util_malloc(n+1);
    if (! p) {
	printError(NULL, ERR_OUT_OF_MEMORY);
    }
    strncpy(p, s1, n);
    p[n] = 0;
    return p;
}

void util_free(void *ptr)
{
    if (ptr) {
	free(ptr);
    }
}

int util_ispath(const char *s)
{
    return (strchr(s, '.') || strchr(s, '/'));
}

#ifndef HAVE_TIMEGM
time_t timegm(struct tm *tm)
{
    char *tz;
    time_t t;
    
    /* ensure to call mktime() for UTC */
    tz = getenv("TZ");
    if (tz) tz = strdup(tz);
    setenv("TZ", "NULL", 1);
    t = mktime(&tm);
    if (tz) {
	setenv("TZ", tz, 1);
	free(tz);
    } else {
	unsetenv("TZ");
    }

    return t;
}
#endif
