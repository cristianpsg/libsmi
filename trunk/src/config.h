/*
 * config.h --
 *
 *      libsmi configuration.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "defs.h"
#include "error.h"
#include "parser.h"



extern int flags;



extern int readConfig(const char *filename, int *flags);



#endif /* _CONFIG_H */
