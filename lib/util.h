/*
 * util.h --
 *
 *      Misc utility functions.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @(#) $Id$
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

extern char *util_strndup(const char *s1, size_t n);

extern int util_strcmp(const char *s1, const char *s2);

extern void util_free(void *ptr);

extern time_t smiMkTime(const char *s);

#endif /* _UTIL_H */
