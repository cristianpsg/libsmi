/*
 * smi.c --
 *
 *      Interface Implementation of libsmi.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi.c,v 1.3 1998/11/16 09:00:10 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "defs.h"
#include "config.h"
#include "error.h"
#include "data.h"
#include "smi.h"




static int flags;
static int initialized = 0;


void
smiInit()
{
    initData();
    initialized = 1;
}



int
smiAddLocation (location)
    const char *location;
{
    if (!initialized) smiInit();
    
    addLocation(location, flags);
 
    return 0;
}


int
smiLoadMibModule(modulename)
    const char *modulename;
{
    if (!initialized) smiInit();
    
    /* TODO */
    
    readMibFile(modulename, modulename, flags | FLAG_WHOLEMOD);

    return 0;
}
 


void
smiSetDebugLevel(level)
    int level;
{
    if (!initialized) smiInit();
    
    debugLevel = level;
}



void
smiSetErrorLevel(level)
    int level;
{
    if (!initialized) smiInit();
    
    errorLevel = level;
}



void
smiSetFlags(userflags)
    int userflags;
{
    if (!initialized) smiInit();
    
    flags = (flags & ~SMI_FLAGMASK) | userflags;
}



int
smiReadConfig(file)
    const char *file;
{
    if (!initialized) smiInit();
    
    return readConfig(file, &flags);
}
