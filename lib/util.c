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
 * @(#) $Id: util.c,v 1.23 2000/06/08 09:36:15 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "util.h"


void *smiMalloc(size_t size)
{
    char *m = calloc(1, size);
    if (! m) {
	smiPrintError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

void *smiRealloc(void *ptr, size_t size)
{
    char *m = realloc(ptr, size);
    if (! m) {
	smiPrintError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

char *smiStrdup(const char *s1)
{
    if (s1) {
	char *m = strdup(s1);
	if (! m) {
	    smiPrintError(NULL, ERR_OUT_OF_MEMORY);
	}
	return m;
    } else {
	return NULL;
    }
}

char *smiStrndup(const char *s1, size_t n)
{
    char *m;
    
    m = smiMalloc(n+1);
    if (! m) {
	smiPrintError(NULL, ERR_OUT_OF_MEMORY);
    }
    strncpy(m, s1, n);
    m[n] = 0;
    return m;
}

void smiFree(void *ptr)
{
    if (ptr) {
	free(ptr);
    }
}

int smiIsPath(const char *s)
{
    return (strchr(s, '.') || strchr(s, '/'));
}

#ifndef HAVE_TIMEGM
time_t timegm(struct tm *tm)
{
    char *tz;
    static char *s = NULL;
    char *tofree = NULL;
    time_t t;
    
    /* ensure to call mktime() for UTC */
    tz = getenv("TZ");
    if (tz) {
	tofree = s;
	s = smiMalloc(strlen(tz)+4);
	sprintf(s, "TZ=%s", tz);
    }
    putenv("TZ=NULL");
    t = mktime(tm);
    if (tz) {
	putenv(s);
    } else {
	putenv("TZ");
    }
    if (tofree) smiFree(tofree);

    return t;
}
#endif
