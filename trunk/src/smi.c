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
 * @(#) $Id$
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



int
isSmiRpc(name)
    const char *name;
{
    return strstr(name, "smirpc://") == name ? 1 : 0;
}



int
smiAddLocation (location)
    const char *location;
{
    struct stat st;
    
    if (isSmiRpc(location)) {
	
    } else {
	if (stat(location, &st)) {
	    printError(NULL, ERR_LOCATION, location, strerror(errno));
	} else {
	    if (S_ISDIR(st.st_mode)) {
		addDirectory(location);
	    } else {
		readMibFile(location, "", flags | FLAG_WHOLEFILE);
	    }
	}
    }
 
    return 0;
}


int
smiLoadMibModule(modulename)
    const char *modulename;
{
    /* TODO */
    readMibFile(modulename, modulename, flags | FLAG_WHOLEMOD);

    return 0;
}
 


void
smiSetDebugLevel(level)
    int level;
{
    debugLevel = level;
}



void
smiSetErrorLevel(level)
    int level;
{
    errorLevel = level;
}



void
smiSetFlags(userflags)
    int userflags;
{
    flags = (flags & ~SMI_FLAGMASK) | userflags;
}



int
smiReadConfig(file)
    const char *file;
{
    return readConfig(file, &flags);
}
