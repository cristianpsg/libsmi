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
 * @(#) $Id: util.c,v 1.11 1999/06/09 19:43:32 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "smi.h"
#include "error.h"
#include "parser-smi.h"



void *util_malloc(size_t size)
{
    return calloc(size, 1);
}



void *util_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}



char *util_strdup(const char *s1)
{
    if (s1) {
	return strdup(s1);
    } else {
	return NULL;
    }
}



char *util_strndup(const char *s1, size_t n)
{
    char *p;
    
    p = util_malloc(n+1);
    strncpy(p, s1, n);
    p[n] = 0;
    return p;
}



int util_strcmp(const char *s1, const char *s2)
{
    if ((!s1) || (!s2))
	return -1;
    
    return strcmp(s1, s2);
}



char *util_strcat(char **s1, char *s2)
{
    char *s;
    
    if (!s2)
	return *s1;

    if ((!s1) || (!*s1))
	return s2;

    s = util_malloc(strlen(*s1) + strlen(s2) + sizeof(char));
    sprintf(s, "%s%s", *s1, s2);

    free(*s1);

    *s1 = s;
    
    return s;
}



void util_free(void *ptr)
{
    if (ptr) {
	free(ptr);
    }
}



int util_ispath(char *s)
{
    return (strchr(s, '.') || strchr(s, '/'));
}
