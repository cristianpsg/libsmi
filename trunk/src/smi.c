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
 * @(#) $Id: smi.c,v 1.8 1998/11/20 19:33:24 strauss Exp $
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
    static char *p[5] = { NULL, NULL, NULL, NULL, NULL };
    static int i = 0;
    int fd;
    
    printDebug(5, "getString(%s %d/%d %s)\n", m->path,
	       s->fileoffset, s->length, s->ptr ? "mem" : "file");

    if (s->ptr) {
        return s->ptr;
    } else {
	if (s->fileoffset < 0) {
	    return NULL;
	}
	i = (i+1) % 5;
        p[i] = realloc(p[i], s->length+1);
        /* TODO: success? */
        fd = open(m->path, O_RDONLY);
        if (fd > 0) {
            lseek(fd, s->fileoffset, SEEK_SET);
            read(fd, p[i], s->length);
            /* TODO: loop if read() < length. */
            close(fd);
            p[i][s->length] = 0;
            return p[i];
        } else {
            return "<smi.c: getString(): could not read string from file>";
            /* TODO: error handling? */
        }
    }
}



char *
getOid(n)
    Node *n;
{
    static char o[SMI_MAX_OID+1+1];
    smi_subid a[128];
    int i, l;

    for (i = 0; n != rootNode; a[i++] = n->subid, n = n->parent);
    for (strcpy(o, ""), l = 0; i > 0; l += sprintf(&o[l], "%d.", a[--i]));
    o[l-1] = 0;

    return o;
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
	
    printDebug(4, "smiLoadMibModule(\"%s\")\n", modulename);

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
	    res.lastupdated = getString(&module->lastUpdated, module);
	    res.organization = getString(&module->organization, module);
	    res.contactinfo = getString(&module->contactInfo, module);
	    res.description = getString(&module->description, module);
	} else {
	    res.description = "";
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
    Object *object;
    Node *node;
    char *elements, *element, *element1;
    unsigned int subid;
    
    printDebug(4, "smiGetNode(\"%s\", \"%s\", %d)\n",
	       name, module ? module : "NULL", wantdescr);

    /* TODO: other than local resources */
    
    /* "1.3.6...", "system", "Module.system", "Module!system" */

    if (isdigit(name[0])) {
        /*
         * name in `1.3.0x6.1...' form. module not recognized.
         */
        elements = strdup(name);
        /* TODO: success? */
        element = strtok(elements, ".");
        node = rootNode;
        while (element) {
            subid = (unsigned int)strtoul(element, NULL, 0);
            node = findNodeByParentAndSubid(node, subid);
	    if (!node) break;
            element = strtok(NULL, ".");
        }
	free(elements);
	if ((!module) || (!strlen(module))) {
	    /*
	     * Note: If no module is given,
	     * it is undefined which Object we get.
	     */
	    object = node->firstObject;
	} else {
	    object = findObjectByNodeAndModulename(node, module);
	}
    } else if (strchr(name, '!') || strchr(name, '.')) {
        /*
         * name in `Module!name' or `Module.name' form.
         */
        elements = strdup(name);
        /* TODO: success? */
        element1 = strtok(elements, "!.");
        element = strtok(NULL, " ");
        object = findObjectByModulenameAndName(element1, element);
        free(elements);
    } else if (module && strlen(module)) {
	/*
	 * name in simple `name' form and module not empty.
	 */
	object = findObjectByModulenameAndName(module, name);
    } else {
	/* printError(NULL, ); */
	return NULL;
    }
	
    if (object) {
	res.name = strdup(object->descriptor->name);
	res.module = strdup(object->module->descriptor->name);
	res.oid = getOid(object->node);
	res.type = "TODO";
	res.decl = object->decl;
	res.access = object->access;
	res.status = object->status;
	if (wantdescr) {
	    res.description = getString(&object->description, object->module);
	} else {
	    res.description = "";
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
    
    printDebug(4, "smiGetType(\"%s\", \"%s\", %d)\n",
	       name, module ? module : "NULL", wantdescr);

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
	res.decl = type->decl;
	res.display = getString(&type->displayHint, type->module);
	res.status = type->status;
	if (wantdescr) {
	    res.description = getString(&type->description, type->module);
	} else {
	    res.description = "";
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
    
    printDebug(4, "smiGetMacro(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

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
