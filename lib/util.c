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
 * @(#) $Id: util.c,v 1.9 1999/05/31 11:58:38 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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



time_t smiMkTime(const char *s)
{
    struct tm  tm;
    char       tmp[3];
    time_t     t;
    
    tm.tm_isdst = 0;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_sec = 0;

    if (strlen(s) == 11) {
	/* seems to be SMIv2 11-char format yymmddhhmmZ */
	tmp[2] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 2));
	tm.tm_mon = atoi(strncpy(tmp, &s[2], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[4], 2));
	tm.tm_hour = atoi(strncpy(tmp, &s[6], 2));
	tm.tm_min = atoi(strncpy(tmp, &s[8], 2));
    } else if (strlen(s) == 13) {
	/* seems to be SMIv2 13-char format yyyymmddhhmmZ */
	tmp[4] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 4)) - 1900;
	tmp[2] = 0;
	tm.tm_mon = atoi(strncpy(tmp, &s[4], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[6], 2));
	tm.tm_hour = atoi(strncpy(tmp, &s[8], 2));
	tm.tm_min = atoi(strncpy(tmp, &s[10], 2));
    } else if (strlen(s) == 10) {
	/* seems to be SMIng 10-char format yyyy-mm-dd */
	tmp[4] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 4)) - 1900;
	tmp[2] = 0;
	tm.tm_mon = atoi(strncpy(tmp, &s[5], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[8], 2));
	tm.tm_hour = 0;
	tm.tm_min = 0;
    } else if (strlen(s) == 16) {
	/* seems to be SMIng 16-char format yyyy-mm-dd hh:mm */
	tmp[4] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 4)) - 1900;
	tmp[2] = 0;
	tm.tm_mon = atoi(strncpy(tmp, &s[5], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[8], 2));
	tm.tm_hour = atoi(strncpy(tmp, &s[11], 2));
	tm.tm_min = atoi(strncpy(tmp, &s[14], 2));
    } else {
	return 0;
    }

    putenv("TZ=UTC"); tzset();
    /* TODO: a better way to make mktime() use UTC !? */

    t = mktime(&tm);
   
    return t;
}
