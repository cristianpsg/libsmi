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
 * @(#) $Id: smi.c,v 1.10 1998/11/22 22:58:23 strauss Exp $
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
	    return "";
	}
	i = (i+1) % 5;
        p[i] = realloc(p[i], s->length+1);
        /* TODO: success? */
        fd = open(m->path, O_RDONLY);
        if (fd > 0) {
            lseek(fd, s->fileoffset, SEEK_SET);
            read(fd, p[i], s->length);
            /* TODO: loop if read() < length. */
	    /* TODO: success? */
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

    for (i = 0;
	 n && (n != rootNode);
	 a[i++] = n->subid, n = n->parent);
    for (strcpy(o, n ? "" : "<incomplete>."), l = strlen(o);
	 i > 0;
	 l += sprintf(&o[l], "%d.", a[--i]));
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
#ifdef PARSER
    Module *module;
    struct stat buf;
    char *path;
    int rc;
#endif
	
    printDebug(4, "smiLoadMibModule(\"%s\")\n", modulename);

    if (!initialized) smiInit();

    for (location = firstLocation; location; location = location->next) {

	if (location->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (location->type == LOCATION_DIR) {

	    module = findModuleByName(modulename);
	    if (module) {
		return 0; /* already loaded. */
	    }

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
    Location *location;
#ifdef PARSER
    Module *module;
    struct stat buf;
    char *path;
    int rc;
#endif
    
#ifdef PARSER
    module = findModuleByName(name);
#endif
    
    for (location = firstLocation; location; location = location->next) {

	if (location->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (location->type == LOCATION_DIR) {

	    path = malloc(strlen(location->name)+strlen(name)+6);
	    
	    sprintf(path, "%s/%s", location->name, name);
	    if (!stat(path, &buf) && (!module)) {
		rc = readMibFile(path, name, flags | FLAG_WHOLEMOD);
		free(path);
		module = findModuleByName(name);
	    }
	    
	    sprintf(path, "%s/%s.my", location->name, name);
	    if (!stat(path, &buf) && (!module)) {
		rc = readMibFile(path, name, flags | FLAG_WHOLEMOD);
		free(path);
		module = findModuleByName(name);
	    }
	    
	    if (module) {
		res.name = strdup(module->descriptor->name);
		res.oid = "TODO";
		if (wantdescr) {
		    res.lastupdated = getString(&module->lastUpdated, module);
		    res.organization = getString(&module->organization,
						 module);
		    res.contactinfo = getString(&module->contactInfo, module);
		    res.description = getString(&module->description, module);
		} else {
		    res.description = "";
		}
		return &res;
	    }
		
	} else if (location->type == LOCATION_FILE) {

	    /* TODO: compare filenames more intelligent */
	    if (module) {
		res.name = strdup(module->descriptor->name);
		res.oid = "TODO";
		if (wantdescr) {
		    res.lastupdated = getString(&module->lastUpdated, module);
		    res.organization = getString(&module->organization,
						 module);
		    res.contactinfo = getString(&module->contactInfo, module);
		    res.description = getString(&module->description, module);
		} else {
		    res.description = "";
		}
		return &res;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, location->name);
	    
	}
    }

    return NULL;
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
    char *elements;
    char *element;
    char modulename[SMI_MAX_DESCRIPTOR+1], nodename[SMI_MAX_DESCRIPTOR+1];
    unsigned int subid;
    Location *location;
#ifdef PARSER
    struct stat buf;
    char *path;
    int rc;
#endif
    
    printDebug(4, "smiGetNode(\"%s\", \"%s\", %d)\n",
	       name, module ? module : "NULL", wantdescr);

    strcpy(modulename, "");
    strcpy(nodename, "");
    
    if (strchr(name, '!') || strchr(name, '.')) {
	/*
	 * name in `Module!name' or `Module.name' form.
	 */
	elements = strdup(name);
	/* TODO: success? */
	strncpy(modulename, strtok(elements, "!."), sizeof(modulename)-1);
	strncpy(nodename, strtok(NULL, " "), sizeof(modulename)-1);
    } else if (module && strlen(module)) {
	/*
	 * name in simple `name' form and module not empty.
	 */
	strncpy(modulename, module, sizeof(modulename)-1);
	strncpy(nodename, name, sizeof(modulename)-1);
    }
    
    for (location = firstLocation; location; location = location->next) {

	if (location->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (location->type == LOCATION_DIR) {

	    if (strlen(modulename)) {
		if (!findModuleByName(modulename)) {

		    path = malloc(strlen(location->name)+strlen(modulename)+6);
		    
		    sprintf(path, "%s/%s", location->name, modulename);
		    if (!stat(path, &buf)) {
			rc = readMibFile(path, modulename,
					 flags | FLAG_WHOLEMOD);
			free(path);
		    }
	    
		    sprintf(path, "%s/%s.my", location->name, modulename);
		    if (!stat(path, &buf)) {
			rc = readMibFile(path, modulename,
					 flags | FLAG_WHOLEMOD);
			free(path);
		    }
		    
		}

		/* lookup in modulename */
	    }
	    
	} else if (location->type == LOCATION_FILE) {

	    if (strlen(modulename)) {

		if (isdigit(name[0])) {
		    /*
		     * name in `1.3.0x6.1...' form.
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
		    if (elements) free(elements);
		    object = findObjectByNodeAndModulename(node, modulename);
		} else {
		    object = findObjectByModulenameAndName(modulename,
							   nodename);
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
			res.description = getString(&object->description,
						    object->module);
		    } else {
			res.description = "";
		    }
		    return &res;
		}
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, location->name);
	    
	}
    }

    return NULL;
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
    static smi_namelist res;
    char *n;
    Module *m;
    char *elements;
    char s[SMI_MAX_FULLNAME+1];
    static char *p = NULL;
    static plen = 0;
    Descriptor *descriptor;
    
    printDebug(4, "smiGetNames(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

    /* TODO: other than local resources */

    if (strchr(name, '!') || strchr(name, '.')) {
        /*
         * name in `Module!name' or `Module.name' form.
         */
        elements = strdup(name);
        /* TODO: success? */
        n = strtok(elements, "!.");
        m = findModuleByName(strtok(NULL, " "));
        free(elements);
	if (!m) {
	    return NULL;
	}
    } else {
	n = name;
	if ((!module) || (!strlen(module))) {
	    m = NULL;
	} else {
	    m = findModuleByName(module);
	    if (!m) {
		return NULL;
	    }
	}
    }

    descriptor = NULL;
    if (!p) {
	p = malloc(200);
    }
    p[0] = 0;
    do {
	descriptor = findNextDescriptor(n, m, KIND_ANY, descriptor);
	if (descriptor &&
	    (descriptor->kind == KIND_OBJECT ||
	     descriptor->kind == KIND_TYPE ||
	     descriptor->kind == KIND_MACRO) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
	    sprintf(s, "%s.%s",
		    descriptor->module->descriptor->name, descriptor->name);
	    if (strlen(p)+strlen(s)+2 > plen) {
		p = realloc(p, strlen(p)+strlen(s)+2);
	    }
	    if (strlen(p)) {
		strcat(p, " ");
	    }
	    strcat(p, s);
	}
    } while (descriptor);

    res.namelist = p;
    return &res;
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
