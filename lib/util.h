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
 * @(#) $Id: util.h,v 1.18 2000/04/10 15:55:18 strauss Exp $
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>

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

extern void *smiMalloc(size_t size);

extern void *smiRealloc(void *ptr, size_t size);

extern char *smiStrdup(const char *s1);

extern char *smiStrndup(const char *s1, size_t n);

extern void smiFree(void *ptr);

extern int smiIsPath(const char *s);

#endif /* _UTIL_H */

