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
 * @(#) $Id: util.c,v 1.26 2001/08/15 17:07:04 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif



#ifdef HAVE_DMALLOC_H

void *_smiMalloc(char *file, int line, size_t size)
{
    char *m = _calloc_leap(file, line, 1, size);
    if (! m) {
	smiPrintError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

void *_smiRealloc(char *file, int line, void *ptr, size_t size)
{
    char *m = _realloc_leap(file, line, ptr, size);
    if (! m) {
	smiPrintError(NULL, ERR_OUT_OF_MEMORY);
    }
    return m;
}

char *_smiStrdup(char *file, int line, const char *s1)
{
    if (s1) {
	char *m = _strdup_leap(file, line, s1);
	if (! m) {
	    smiPrintError(NULL, ERR_OUT_OF_MEMORY);
	}
	return m;
    } else {
	return NULL;
    }
}

char *_smiStrndup(char *file, int line, const char *s1, size_t n)
{
    char *m;
    
    m = _smiMalloc(file, line, n+1);
    if (! m) {
	smiPrintError(NULL, ERR_OUT_OF_MEMORY);
    }
    strncpy(m, s1, n);
    m[n] = 0;
    return m;
}

void _smiFree(char *file, int line, void *ptr)
{
    if (ptr) {
	_free_leap(file, line, ptr);
    }
}

#else

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

#endif



int smiIsPath(const char *s)
{
    return (strchr(s, '.') || strchr(s, DIR_SEPARATOR));
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



int smiTypeDerivedFrom(Type *typePtr, Type *parentTypePtr)
{
    Type *t;
    
    for (t = parentTypePtr; t != NULL; t = t->parentPtr) {
	if (typePtr == t) {
	    return 1;
	}
    }

    return 0;
}
