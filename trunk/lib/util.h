/*
 * util.h --
 *
 *      Misc utility functions.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: util.h,v 1.16 2000/02/22 17:11:12 strauss Exp $
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "smi.h"
#include "error.h"
#include "parser-smi.h"

#ifdef __CHECKER__
#undef HAVE_TIMEGM
#undef HAVE_STRTOULL
#undef HAVE_STRTOUQ
#undef HAVE_STRTOLL
#undef HAVE_STRTOQ
#endif

/*
 * Make sure we have strtoull() and strtoll() on this system.
 */

#ifndef HAVE_STRTOULL
#ifdef HAVE_STRTOUQ
#define strtoull strtouq
#else
#define strtoull strtoul	/* TODO */
#endif
#endif

#ifndef HAVE_STRTOLL
#ifdef HAVE_STRTOQ
#define strtoll strtoq
#else
#define strtoll strtol
#endif
#endif

extern void *util_malloc(size_t size);

extern void *util_realloc(void *ptr, size_t size);

extern char *util_strdup(const char *s1);

extern char *util_strndup(const char *s1, size_t n);

extern void util_free(void *ptr);

extern int util_ispath(const char *s);

#endif /* _UTIL_H */

