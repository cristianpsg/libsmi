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
 * @(#) $Id: util.c,v 1.19 2000/02/12 10:56:21 strauss Exp $
 */

#include <config.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "util.h"


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
    char *m;
    
    m = util_malloc(n+1);
    if (! m) {
	printError(NULL, ERR_OUT_OF_MEMORY);
    }
    strncpy(m, s1, n);
    m[n] = 0;
    return m;
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
    static char *s = NULL;
    time_t t;
    
    /* ensure to call mktime() for UTC */
    tz = getenv("TZ");
    if (tz) {
	if (s) util_free(s);
	s = util_malloc(strlen(tz)+4);
	sprintf(s, "TZ=%s", tz);
    }
    putenv("TZ=NULL");
    t = mktime(tm);
    if (tz) {
	putenv(s);
    } else {
	putenv("TZ");
    }

    return t;
}
#endif
