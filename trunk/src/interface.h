/*
 * interface.h --
 *
 *      Interface of libsmi.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



extern int smiLoadMibModule(const char *modulename);

extern int smiInit();

extern char *smiGetMibNodeDescription(const char *node);

extern char *smiGetTypeDescription(const char *type);

extern char *smiGetMibNodeAccess(const char *node);

#endif /* _INTERFACE_H */
