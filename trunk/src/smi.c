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
 * @(#) $Id: smi.c,v 1.5 1998/11/18 17:31:41 strauss Exp $
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

char *
getString(s, m)
    String *s;
    Module *m;
{
    char *p;
    int fd;
    
    if (s->ptr) {
        return s->ptr;
    } else {
        p = malloc(s->length+1);
        /* TODO: success? */
        /* TODO: who will free() this? */
        fd = open(m->path, O_RDONLY);
        if (fd > 0) {
            lseek(fd, s->fileoffset, SEEK_SET);
            read(fd, p, s->length);
            /* TODO: loop if read() < length. */
            close(fd);
            p[s->length] = 0;
            return p;
        } else {
            return "";
            /* TODO: error handling? */
        }
    }
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
		readMibFile(location->name, "", flags | FLAG_WHOLEFILE);
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



int
smiLoadMibModule(modulename)
    const char *modulename;
{
    Location *location;
    struct stat buf;
    char *path;
    int rc;
#ifdef PARSER
    Module *module;
#endif
	
    if (!initialized) smiInit();
    
    for (location = firstLocation; location; location = location->next) {

	if (location->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (location->type == LOCATION_DIR) {

	    path = malloc(strlen(location->name)+strlen(modulename)+6);
	    
	    sprintf(path, "%s/%s", location->name, modulename);
	    if (!stat(path, &buf)) {
		rc = readMibFile(path, modulename, flags | FLAG_WHOLEMOD);
		free(path);
		return rc;
	    }
	    
	    sprintf(path, "%s/%s.my", location->name, modulename);
	    if (!stat(path, &buf)) {
		rc = readMibFile(path, modulename, flags | FLAG_WHOLEMOD);
		free(path);
		return rc;
	    }
	    
	} else if (location->type == LOCATION_FILE) {

	    /* TODO */
	    module = findModuleByName(modulename);
	    /* TODO: compare filenames more intelligent */
	    if (module && (!strcmp(module->path, location->name))) {
		return 0; /* already loaded. */
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, location->name);
	    
	}
    }

    return -1;
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








smi_module *
smiGetModule(name, wantdescr)
    smi_descriptor name;
    int wantdescr;
{
    static smi_module res;
    Module *module;

    /* TODO: other than local resources */
    module = findModuleByName(name);
    
    if (module) {
	res.name = strdup(module->descriptor->name);
	res.oid = "TODO";
	if (wantdescr) {
	    res.description = getString(&module->description, module);
	} else {
	    res.description = NULL;
	}
	return &res;
    } else {
	return NULL;
    }
}



smi_node *
smiGetNode(name, module, wantdescr)
    smi_fullname name;
    smi_descriptor module;
    int wantdescr;
{
    static smi_node res;
    MibNode *node;
    char *elements, *element, *element1;
    unsigned int subid;
    
    /* TODO: other than local resources */
    
    /* "1.3.6...", "system", "Module.system", "Module!system" */

    if (isdigit(name[0])) {
        /*
         * name in `1.3.0x6.1...' form. module not recognized.
         */
        elements = strdup(name);
        /* TODO: success? */
        element = strtok(elements, ".");
        node = rootMibNode;
        while (element) {
            subid = (unsigned int)strtoul(element, NULL, 0);
            node = findMibNodeByParentAndSubid(node, subid);
	    if (!node) break;
            element = strtok(NULL, ".");
        }
	free(elements);
    } else if (strchr(name, '!') || strchr(name, '.')) {
        /*
         * name in `Module!name' or `Module.name' form.
         */
        elements = strdup(name);
        /* TODO: success? */
        element1 = strtok(elements, "!.");
        element = strtok(NULL, " ");
        node = findMibNodeByModulenameAndName(element1, element);
        free(elements);
    } else if (module && strlen(module)) {
	/*
	 * name in simple `name' form and module not empty.
	 */
	node = findMibNodeByModulenameAndName(module, name);
    } else {
	/* printError(NULL, ); */
	return NULL;
    }
	
    if (node) {
	res.name = strdup(node->descriptor->name);
	res.module = strdup(node->module->descriptor->name);
	res.oid = "TODO";
	res.type = "TODO";
	res.decl = node->macro;
	res.access = node->access;
	res.status = node->status;
	if (wantdescr) {
	    res.description = getString(&node->description, node->module);
	} else {
	    res.description = NULL;
	}
	return &res;
    } else {
	return NULL;
    }
}



smi_type *
smiGetType(name, module, wantdescr)
    smi_fullname name;
    smi_descriptor module;
    int wantdescr;
{
    static smi_type res;
    Type *type;
    char *elements, *element, *element1;
    
    /* TODO: other than local resources */
    
    /* "Type", "Module.Type", "Module!Type" */

    if (strchr(name, '!') || strchr(name, '.')) {
        /*
         * name in `Module!name' or `Module.name' form.
         */
        elements = strdup(name);
        /* TODO: success? */
        element1 = strtok(elements, "!.");
        element = strtok(NULL, " ");
        type = findTypeByModulenameAndName(element1, element);
        free(elements);
    } else if (module && strlen(module)) {
	/*
	 * name in simple `name' form and module not empty.
	 */
	type = findTypeByModulenameAndName(module, name);
    } else {
	/* printError(NULL, ); */
	return NULL;
    }
	
    if (type) {
	res.name = strdup(type->descriptor->name);
	res.module = strdup(type->module->descriptor->name);
	res.syntax = type->syntax;
	res.decl = type->macro;
	res.display = getString(&type->displayHint, type->module);
	res.status = type->status;
	if (wantdescr) {
	    res.description = getString(&type->description, type->module);
	} else {
	    res.description = NULL;
	}
	return &res;
    } else {
	return NULL;
    }
}



smi_macro *
smiGetMacro(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    static smi_macro res;
    Macro *macro;
    char *elements, *element, *element1;
    
    /* TODO: other than local resources */
    
    /* "MACRO", "Module.MACRO", "Module!MACRO" */

    if (strchr(name, '!') || strchr(name, '.')) {
        /*
         * name in `Module!name' or `Module.name' form.
         */
        elements = strdup(name);
        /* TODO: success? */
        element1 = strtok(elements, "!.");
        element = strtok(NULL, " ");
        macro = findMacroByModulenameAndName(element1, element);
        free(elements);
    } else if (module && strlen(module)) {
	/*
	 * name in simple `name' form and module not empty.
	 */
	macro = findMacroByModulenameAndName(module, name);
    } else {
	/* printError(NULL, ); */
	return NULL;
    }
	
    if (macro) {
	res.name = strdup(macro->descriptor->name);
	res.module = strdup(macro->module->descriptor->name);
	return &res;
    } else {
	return NULL;
    }
}



smi_namelist *
smiGetNames(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    return NULL;
}



smi_namelist *
smiGetChildren(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    return NULL;
}



smi_namelist *
smiGetMembers(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    return NULL;
}



smi_fullname *
smiGetParent(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    return NULL;
}
