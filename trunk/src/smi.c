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
 * @(#) $Id: smi.c,v 1.4 1998/11/17 16:09:21 strauss Exp $
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
#include "smi.h"
#include "config.h"
#include "error.h"

#ifdef PARSER
#include "data.h"
#endif



typedef enum LocationType {
    LOCATION_FILE	     = 0,
    LOCATION_DIR	     = 1,
    LOCATION_DBM	     = 2,
    LOCATION_RPC	     = 3
} LocationType;

typedef struct Location {
    char	    *name;
    LocationType    type;
    struct Location *next;
    struct Location *prev;
} Location;



static int		flags = 0;
static int		initialized = 0;

static Location		*firstLocation = NULL;
static Location		*lastLocation = NULL;



/*
 * Internal functions.
 */

Location *
findLocationByModule(name)
    const char *name;
{
    struct stat buf;
    Location *location;
    char *path;
#ifdef PARSER
    Module *module;
#endif
    
    printDebug(4, "findLocationByModule(\"%s\")\n", name);

    for (location = firstLocation; location; location = location->next) {

	if (location->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (location->type == LOCATION_DIR) {

	    path = malloc(strlen(location->name)+strlen(name)+6);
	    
	    sprintf(path, "%s/%s", location->name, name);
	    if (!stat(path, &buf)) {
		free(path);
		return location;
	    }
	    
	    sprintf(path, "%s/%s.my", location->name, name);
	    if (!stat(path, &buf)) {
		free(path);
		return location;
	    }
	    
	} else if (location->type == LOCATION_FILE) {

	    /* TODO */
	    module = findModuleByName(name);
	    /* TODO: compare filenames more intelligent */
	    if (module && (!strcmp(module->path, location->name))) {
		return location;
	    }
	    
#endif
	} else {

	    printError(ERR_UNKNOWN_LOCATION_TYPE, location->name);
	    
	}
	
    }
    
    return NULL;
}



/*
 * Interface Functions.
 */

void
smiInit()
{
    initData();
    
    initialized   = 1;
    
    firstLocation = NULL;
    lastLocation  = NULL;
}



int
smiAddLocation (name)
    const char *name;
{
    Location *location;
    struct stat st;
    
    if (!initialized) smiInit();
    
    printDebug(4, "smiAddLocation(\"%s\")\n", name);

    location = (Location *)malloc(sizeof(Location));
    if (!location) {
	printError(NULL, ERR_ALLOCATING_LOCATION, strerror(errno));
	return -1;
    }
    
    if (strstr(name, "smirpc://") == name) {
	location->type = LOCATION_RPC;
	location->name = strdup(&name[9]);
#ifdef PARSER
    } else {
	if (stat(name, &st)) {
	    printError(NULL, ERR_LOCATION, name, strerror(errno));
	    free(location);
	    return -1;
	} else {
	    if (S_ISDIR(st.st_mode)) {
		location->type = LOCATION_DIR;
		location->name = strdup(name);
	    } else {
		location->type = LOCATION_FILE;
		location->name = strdup(name);
		/*
		 * MIB file locations are read immediately.
		 */
		readMibFile(loc, "", flags | FLAG_WHOLEFILE);
	    }
	}
#endif
    }
    
    location->prev = lastLocation;
    location->next = NULL;
    if (lastLocation) {
	lastLocation->next = location;
    } else {
	firstLocation = location;
    }
    lastLocation = location;
    
    return 0;
}



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
