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
 * @(#) $Id: smi.c,v 1.17 1998/11/26 00:50:07 strauss Exp $
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
    static char *p[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
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
	i = (i+1) % 8;
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



void
splitNameAndModule(name, module, name2, module2)
    char *name;
    char *module;
    char *name2;
    char *module2;
{
    
    printDebug(5, "splitNameAndModule(\"%s\", \"%s\", ",
	       name, module ? module : "");

    if ((strchr(name, '!') || strchr(name, '.')) &&
	(!isdigit(name[0]))) {
	/*
	 * name in `Module!name' or `Module.name' form.
	 */
	if ((name[0] == '.') || (name[0] == '!')) {
	    strncpy(name2, strtok(name, "!."), SMI_MAX_OID);
	    module2[0] = 0;
	} else {
	    strncpy(module2, strtok(name, "!."), SMI_MAX_DESCRIPTOR);
	    strncpy(name2, strtok(NULL, " "), SMI_MAX_OID);
	}
    } else if (module && strlen(module)) {
	/*
	 * name in simple `name' form and module not empty.
	 */
	strncpy(module2, module, SMI_MAX_DESCRIPTOR);
	strncpy(name2, name, SMI_MAX_OID);
    } else {
	strncpy(name2, name, SMI_MAX_OID);
	module2[0] = 0;
    }

    printDebug(5, " \"%s\", \"%s\")\n", name2, module2);
}



Object *
getObject(name, module)
    char *name;
    char *module;
{
    Object	    *o = NULL;
    Node	    *n;
    char	    *elements, *element;
    smi_subid	    subid;
    
    printDebug(5, "getObject(%s, %s)\n", name, module ? module : "");

    if (isdigit(name[0])) {
	/*
	 * name in `1.3.0x6.1...' form.
	 */
	elements = strdup(name);
	/* TODO: success? */
	element = strtok(elements, ".");
	n = rootNode;
	while (element) {
	    subid = (unsigned int)strtoul(element, NULL, 0);
	    n = findNodeByParentAndSubid(n, subid);
	    if (!n) break;
	    element = strtok(NULL, ".");
	}
	free(elements);
	if (n)
	    o = findObjectByNodeAndModulename(n, module);
    } else {
	o = findObjectByModulenameAndName(module, name);
    }
    return o;
}



Type *
getType(name, module)
    char *name;
    char *module;
{
    Type	    *t = NULL;
    
    printDebug(5, "getType(%s, %s)\n", name, module ? module : "");

    t = findTypeByModulenameAndName(module, name);

    return t;
}



Macro *
getMacro(name, module)
    char *name;
    char *module;
{
    Macro	    *m = NULL;
    
    printDebug(5, "getMacro(%s, %s)\n", name, module ? module : "");

    m = findMacroByModulenameAndName(module, name);

    return m;
}



char *
dirname(path)
    char *path;
{
    static char *p = NULL;
    char *pp;
    
    p = realloc(p, strlen(path)+1);
    /* TODO: success? */
    strcpy(p, path);
    pp = strrchr(p, '/');
    if (pp) *pp = 0;
    return p;
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
smiAddLocation(name)
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
    smi_descriptor modulename;
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
    debugLevel = level;

    if (!initialized) smiInit();
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
    Location	      *l;
#ifdef PARSER
    Module	      *m;
    struct stat	      buf;
    char	      *path;
    int		      rc;
#endif
    
    printDebug(4, "smiGetModule(\"%s\", %d)\n", name, wantdescr);

#ifdef PARSER
    m = findModuleByName(name);
#endif
    
    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    path = malloc(strlen(l->name)+strlen(name)+6);
	    sprintf(path, "%s/%s", l->name, name);
	    if (!stat(path, &buf) && (!m)) {
		rc = readMibFile(path, name, flags | FLAG_WHOLEMOD);
		m = findModuleByName(name);
	    }
	    sprintf(path, "%s/%s.my", l->name, name);
	    if (!stat(path, &buf) && (!m)) {
		rc = readMibFile(path, name, flags | FLAG_WHOLEMOD);
		m = findModuleByName(name);
	    }
	    free(path);
	    
	    if (m) {
		break;
	    }
		
	} else if (l->type == LOCATION_FILE) {

	    if (m) {
		break;
	    }
		
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (m) {
	res.name = strdup(m->descriptor->name);
	res.oid = "TODO";
	if (wantdescr) {
	    res.lastupdated = getString(&m->lastUpdated, m);
	    res.organization = getString(&m->organization, m);
	    res.contactinfo = getString(&m->contactInfo, m);
	    res.description = getString(&m->description, m);
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
    Object          *o = NULL;
    Module	    *m;
    Location	    *l;
    char	    name2[SMI_MAX_OID+1];
    char	    module2[SMI_MAX_DESCRIPTOR+1];
    static char	    type[SMI_MAX_FULLNAME+1];
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif
    
    printDebug(4, "smiGetNode(\"%s\", \"%s\", %d)\n",
	       name, module ? module : "NULL", wantdescr);

    splitNameAndModule(name, module, name2, module2);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module2)) {
		if (!(m = findModuleByName(module2))) {

		    path = malloc(strlen(l->name)+strlen(module2)+6);
		    sprintf(path, "%s/%s", l->name, module2);
		    if (!stat(path, &buf)) {
			readMibFile(path, module2,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module2);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module2);
			if (!stat(path, &buf)) {
			    readMibFile(path, module2,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module2);
			}
			free(path);
		    }
		}
		
		if (m) {
		    o = getObject(name2, module2);
		    if (o) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module2)) {
		o = getObject(name2, module2);
		if (o) break;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (o) {
	res.name = strdup(o->descriptor->name);
	res.module = strdup(o->module->descriptor->name);
	res.oid = getOid(o->node);
	if (o->type) {
	    if (o->type->flags & FLAG_PARENTIMPORTED) {
		sprintf(type, "%s.%s",
		   ((Descriptor *)(o->type->parent))->module->descriptor->name,
			((Descriptor *)(o->type->parent))->name);
	    } else if (o->type->flags & FLAG_IMPORTED) {
		sprintf(type, "gaga");
	    } else {
		if (o->type->module && o->type->descriptor) {
		    sprintf(type, "%s.%s", o->type->module->descriptor->name,
			    o->type->descriptor->name);
		} else if (o->type->descriptor) {
		    sprintf(type, "%s", o->type->descriptor->name);
		} else {
		    sprintf(type, "%s", smiStringSyntax(o->type->syntax));
		}
	    }
	} else {
	    type[0] = 0;
	}
	res.type = type;
	res.decl = o->decl;
	res.access = o->access;
	res.status = o->status;
	res.syntax = o->type ? o->type->syntax : SMI_SYNTAX_UNKNOWN;
	if (wantdescr) {
	    res.description = getString(&o->description,
					o->module);
	} else {
	    res.description = "";
	}
	printDebug(5, " ... = (%p,%s)\n", o, res.name);
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
    Type	    *t = NULL;
    Module	    *m;
    Location	    *l;
    char	    name2[SMI_MAX_OID+1];
    char	    module2[SMI_MAX_DESCRIPTOR+1];
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif
    
    printDebug(4, "smiGetType(\"%s\", \"%s\", %d)\n",
	       name, module ? module : "NULL", wantdescr);

    splitNameAndModule(name, module, name2, module2);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module2)) {
		if (!(m = findModuleByName(module2))) {

		    path = malloc(strlen(l->name)+strlen(module2)+6);
		    sprintf(path, "%s/%s", l->name, module2);
		    if (!stat(path, &buf)) {
			readMibFile(path, module2,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module2);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module2);
			if (!stat(path, &buf)) {
			    readMibFile(path, module2,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module2);
			}
			free(path);
		    }
		}
		
		if (m) {
		    t = getType(name2, module2);
		    if (t) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module2)) {
		t = getType(name2, module2);
		if (t) break;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (t) {
	res.name = strdup(t->descriptor->name);
	res.module = strdup(t->module->descriptor->name);
	res.syntax = t->syntax;
	res.decl = t->decl;
	res.display = getString(&t->displayHint, t->module);
	res.status = t->status;
	if (wantdescr) {
	    res.description = getString(&t->description, t->module);
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
    Macro	    *ma = NULL;
    Module	    *m;
    Location	    *l;
    char	    name2[SMI_MAX_OID+1];
    char	    module2[SMI_MAX_DESCRIPTOR+1];
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif
    
    printDebug(4, "smiGetMacro(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

    splitNameAndModule(name, module, name2, module2);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module2)) {
		if (!(m = findModuleByName(module2))) {

		    path = malloc(strlen(l->name)+strlen(module2)+6);
		    sprintf(path, "%s/%s", l->name, module2);
		    if (!stat(path, &buf)) {
			readMibFile(path, module2,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module2);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module2);
			if (!stat(path, &buf)) {
			    readMibFile(path, module2,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module2);
			}
			free(path);
		    }
		}
		
		if (m) {
		    ma = getMacro(name2, module2);
		    if (ma) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module2)) {
		ma = getMacro(name2, module2);
		if (ma) break;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (ma) {
	res.name = strdup(ma->descriptor->name);
	res.module = strdup(ma->module->descriptor->name);
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
    Location		*l;
    Module		*m;
    Object		*o;
    Descriptor		*d;
    Node		*n, *nn;
    char		*elements, *element;
    char		name2[SMI_MAX_OID+1];
    char		module2[SMI_MAX_DESCRIPTOR+1];
    char		s[SMI_MAX_FULLNAME+1];
    smi_subid		subid;
    static char		*p = NULL;
    static		plen = 0;

    printDebug(4, "smiGetNames(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

    splitNameAndModule(name, module, name2, module2);

    if (strlen(module2)) {
	m = findModuleByName(module2);
    } else {
	m = NULL;
    }
    
    d = NULL;
    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    if (isdigit(name2[0])) {
	/*
	 * name2 in `1.3.0x6.1...' form.
	 */
	elements = strdup(name2);
	/* TODO: success? */
	element = strtok(elements, ".");
	n = rootNode;
	nn = NULL;
	while (element) {
	    subid = (unsigned int)strtoul(element, NULL, 0);
	    n = findNodeByParentAndSubid(n, subid);
	    if (!n) break;
	    nn = n;
	    element = strtok(NULL, ".");
	}
	free(elements);
    }

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if ((l->type == LOCATION_DIR) ||
		   (l->type == LOCATION_FILE)) {

	    if (m || !strlen(module2)) {
		
		if (isdigit(name2[0])) {

		    for (o = nn->firstObject; o; o = o->next) {
			if ((!strlen(module2)) || (o->module == m)) {
			    if (((l->type == LOCATION_FILE) &&
				 (!strcmp(l->name, o->module->path))) ||
				((l->type == LOCATION_DIR) &&
			       (!strcmp(l->name, dirname(o->module->path))))) {
				sprintf(s, "%s.%s",
					o->module->descriptor->name,
					o->descriptor->name);
				/* TODO: bounds check ($..^) */
				if (!strstr(p, s)) {
				    if (strlen(p)+strlen(s)+2 > plen) {
					p = realloc(p, strlen(p)+strlen(s)+2);
				    }
				    if (strlen(p)) strcat(p, " ");
				    strcat(p, s);
				}
			    }
			}
		    }
		    
		} else {

		    do {
			d = findNextDescriptor(name2, m, KIND_ANY, d);
			if (d &&
			    (((l->type == LOCATION_FILE) &&
			      (!strcmp(l->name, d->module->path))) ||
			     ((l->type == LOCATION_DIR) &&
			      (!strcmp(l->name, dirname(d->module->path))))) &&
			    (d->kind == KIND_OBJECT ||
			     d->kind == KIND_TYPE ||
			     d->kind == KIND_MACRO) &&
			    (!(d->flags & FLAG_IMPORTED)) &&
			    (d->flags & FLAG_MODULE)) {
			    sprintf(s, "%s.%s",
				    d->module->descriptor->name, d->name);
			    /* TODO: bounds check ($..^) */
			    if (!strstr(p, s)) {
				if (strlen(p)+strlen(s)+2 > plen) {
				    p = realloc(p, strlen(p)+strlen(s)+2);
				}
				if (strlen(p)) strcat(p, " ");
				strcat(p, s);
			    }
			}

		    } while (d);
		}
	    }
	    
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    res.namelist = p;
    return &res;
}



smi_namelist *
smiGetChildren(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    static smi_namelist res;
    Location		*l;
    Module		*m;
    Object		*o = NULL;
    Descriptor		*d;
    Node		*n = NULL;
    char		name2[SMI_MAX_OID+1];
    char		module2[SMI_MAX_DESCRIPTOR+1];
    char		*s;
    char		ss[SMI_MAX_FULLNAME+1];
    static char		*p = NULL;
    static		plen = 0;
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif

    printDebug(4, "smiGetChildren(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

    splitNameAndModule(name, module, name2, module2);

    d = NULL;
    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocation; l; l = l->next) {

	o = NULL;
	
	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module2)) {
		if (!(m = findModuleByName(module2))) {

		    path = malloc(strlen(l->name)+strlen(module2)+6);
		    sprintf(path, "%s/%s", l->name, module2);
		    if (!stat(path, &buf)) {
			readMibFile(path, module2,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module2);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module2);
			if (!stat(path, &buf)) {
			    readMibFile(path, module2,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module2);
			}
			free(path);
		    }
		}
		
		if (m) {
		    o = findObjectByModulenameAndName(module2, name2);
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module2)) {
		o = findObjectByModulenameAndName(module2, name2);
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}

	if (o) {
	    for (n = o->node->firstChild; n; n = n->next) {
		/*
		 * For child nodes that have a declaration in
		 * the same module as the parent, we assume
		 * this is want the user wants to get.
		 * Otherwise we return the first declaration we know.
		 * Last resort: the numerical OID.
		 */
		for (o = n->firstObject; o; o = o->next) {
		    if (o->module == m) {
			break;
		    }
		}
		if (!o) {
		    o = n->firstObject;
		}
		if (o) {
		    sprintf(ss, "%s.%s",
			    o->module->descriptor->name,
			    o->descriptor->name);
		    s = ss;
		} else {
		    s = getOid(n);
		}
		/* TODO: bounds check ($..^) */
		if (!strstr(p, s)) {
		    if (strlen(p)+strlen(s)+2 > plen) {
			p = realloc(p, strlen(p)+strlen(s)+2);
		    }
		    if (strlen(p)) strcat(p, " ");
		    strcat(p, s);
		}
	    }
	}

    }

    res.namelist = p;
    return &res;
}



smi_namelist *
smiGetMembers(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    static smi_namelist res;
    Location		*l;
    Module		*m;
    Object		*o = NULL;
    Descriptor		*d;
    Type		*t;
    char		name2[SMI_MAX_OID+1];
    char		module2[SMI_MAX_DESCRIPTOR+1];
    char		*s;
    char		ss[SMI_MAX_FULLNAME+1];
    static char		*p = NULL;
    static		plen = 0;
    List		*e;
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif

    printDebug(4, "smiGetMembers(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

    splitNameAndModule(name, module, name2, module2);

    d = NULL;
    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module2)) {
		if (!(m = findModuleByName(module2))) {

		    path = malloc(strlen(l->name)+strlen(module2)+6);
		    sprintf(path, "%s/%s", l->name, module2);
		    if (!stat(path, &buf)) {
			readMibFile(path, module2,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module2);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module2);
			if (!stat(path, &buf)) {
			    readMibFile(path, module2,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module2);
			}
			free(path);
		    }
		}
		
		if (m) {
		    if (isupper(name2[0])) {
			t = findTypeByModulenameAndName(module2, name2);
			if (t) {
			    if (t->syntax == SMI_SYNTAX_SEQUENCE) {
				for (e = (void *)t->parent; e; e = e->next) {
				    sprintf(ss, "%s.%s",
					    ((Object *)(e->ptr))->module->
					          descriptor->name,
					    ((Object *)(e->ptr))->
					          descriptor->name);
				    s = ss;
				    if (!strstr(p, s)) {
					if (strlen(p)+strlen(s)+2 > plen) {
					    p = realloc(p,
							strlen(p)+strlen(s)+2);
					}
					if (strlen(p)) strcat(p, " ");
					strcat(p, s);
				    }
				}
			    }
			}
		    } else if (islower(name2[0])) {
			o = findObjectByModulenameAndName(module2, name2);
			if (o) {
			    if (o->index) {
				for (e = (void *)o->index; e; e = e->next) {
				    sprintf(ss, "%s.%s",
					    ((Object *)(e->ptr))->module->
					          descriptor->name,
					    ((Object *)(e->ptr))->
					          descriptor->name);
				    s = ss;
				    if (!strstr(p, s)) {
					if (strlen(p)+strlen(s)+2 > plen) {
					    p = realloc(p,
							strlen(p)+strlen(s)+2);
					}
					if (strlen(p)) strcat(p, " ");
					strcat(p, s);
				    }
				}
			    }
			}
			if (p) break;
		    }
		}
	    }

	} else if (l->type == LOCATION_FILE) {
	    
	    if (strlen(module2)) {
		o = findObjectByModulenameAndName(module2, name2);
	    }
#endif
	} else {
	    
	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    res.namelist = p;
    return &res;
}



smi_fullname *
smiGetParent(name, module)
    smi_fullname name;
    smi_descriptor module;
{
    static smi_fullname res;
    Object		*o = NULL;
    Node		*n;
    Module		*m;
    Location		*l;
    char		name2[SMI_MAX_OID+1];
    char		module2[SMI_MAX_DESCRIPTOR+1];
    char		s[SMI_MAX_FULLNAME+1];
#ifdef PARSER
    struct stat		buf;
    char		*path;
#endif
    
    printDebug(4, "smiGetParent(\"%s\", \"%s\")\n",
	       name, module ? module : "NULL");

    splitNameAndModule(name, module, name2, module2);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module2)) {
		if (!(m = findModuleByName(module2))) {

		    path = malloc(strlen(l->name)+strlen(module2)+6);
		    sprintf(path, "%s/%s", l->name, module2);
		    if (!stat(path, &buf)) {
			readMibFile(path, module2,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module2);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module2);
			if (!stat(path, &buf)) {
			    readMibFile(path, module2,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module2);
			}
			free(path);
		    }
		}
		
		if (m) {
		    o = getObject(name2, module2);
		    if (o) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module2)) {
		o = getObject(name2, module2);
		if (o) break;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (o && (o->node) && (o->node->parent)) {
	n = o->node->parent;
	/*    fprintf(stderr, "XXX 2 %p \n", o);

	 * If the parent node has a declaration in
	 * the same module as the child, we assume
	 * this is want the user wants to get.
	 * Otherwise we return the first declaration we know.
	 * Last resort: the numerical OID.
	 */
	for (o = n->firstObject; o; o = o->next) {
	    if ((o->module == m) && (!(o->flags & FLAG_IMPORTED))) {
		break;
	    }
	}
	if (!o) {
	    o = n->firstObject;
	}
	if (o && (o->descriptor)) {
	    sprintf(s, "%s.%s",
		    o->module->descriptor->name,
		    o->descriptor->name);
	    res = s;
	} else {
	    res = getOid(n);
	}
	fprintf(stderr, "XXX   %s \n", res);
	printDebug(5, " ... = %s\n", res);
	return &res;
    } else {
	return NULL;
    }
}
