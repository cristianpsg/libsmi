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
 * @(#) $Id: config.h,v 1.1 1998/11/04 02:14:52 strauss Exp $
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "defs.h"
#include "error.h"
#include "parser-smi.h"



extern int flags;



extern int readConfig(const char *filename, int *flags);



#endif /* _CONFIG_H */
