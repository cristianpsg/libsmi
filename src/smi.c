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
 * @(#) $Id: smi.c,v 1.23 1998/12/01 16:59:36 strauss Exp $
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
#include <strings.h>

#include "defs.h"
#include "smi.h"
#include "config.h"
#include "error.h"

#ifdef PARSER
#include "data.h"
#endif



#define MIN(X,Y)	((X) < (Y) ? (X) : (Y))



typedef enum LocationType {
    LOCATION_FILE	     = 0,
    LOCATION_DIR	     = 1,
    LOCATION_DBM	     = 2,
    LOCATION_RPC	     = 3
} LocationType;

typedef struct Location {
    char	    *name;
    LocationType    type;
    CLIENT	    *cl;
    struct Location *next;
    struct Location *prev;
} Location;

typedef struct View {
    char	    *name;
    struct View	    *next;
    struct View	    *prev;
} View;



static int		flags = 0;
static int		initialized = 0;

static Location		*firstLocation = NULL;
static Location		*lastLocation = NULL;

static View		*firstView = NULL;
static View		*lastView = NULL;



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
createFullname(spec, mod, fullname)
    char *spec;
    char *mod;
    char *fullname;
{
    printDebug(5, "createFullname(\"%s\", \"%s\", ...",
	       spec, mod ? mod : "NULL");

    if ((strchr(spec, '!') || strchr(spec, '.')) &&
	(isupper(spec[0]))) {
	strncpy(fullname, spec, SMI_MAX_DESCRIPTOR + 1 + SMI_MAX_OID);
    } else if (mod && strlen(mod)) {
	strncpy(fullname, mod, SMI_MAX_DESCRIPTOR);
	strncat(fullname, ".", SMI_MAX_DESCRIPTOR + 1);
	strncat(fullname, spec, SMI_MAX_DESCRIPTOR + 1 + SMI_MAX_OID);
    } else {
	strncpy(fullname, spec, SMI_MAX_OID);
    }

    printDebug(5, " fullname=\"%s\")\n", fullname);
}



void
createModuleAndName(fullname, module, name)
    char *fullname;
    char *module;
    char *name;
{
    int l;
    
    printDebug(5, "createModuleAndName(\"%s\", ...", fullname);

    if ((l = strcspn(fullname, ".")) &&
	(isupper(fullname[0]) || (fullname[0] == '.'))) {
	strcpy(module, fullname);
	module[l] = 0;
	strcpy(name, &fullname[l+1]);
    } else {
	strcpy(name, fullname);
	module[0] = 0;
    }

    printDebug(5, " module=\"%s\", name=\"%s\")\n", module, name);
}



void
filterNamelist(list)
    char *list;
{
    char *a[100];
    int num, i, j, l;
    char *p;
    char m[SMI_MAX_DESCRIPTOR+1];
    View *view;

    /* loop over every element */
    for (i = 0, num = 0, p = strtok(list, " ");
	 p; p = strtok(NULL, " ")) {
	/* filter modules not in our view */
	strncpy(m, p, (l = strcspn(p, ".")));
	m[l] = 0;
	for (view = firstView; view; view = view->next) 
	    if (!strcmp(m, view->name)) break;
	if ((!view) && (!(flags & SMI_VIEWALL))) break;
	/* filter duplicates */
	for (j = 0; j < num; j++) if (!strcmp(p, a[j])) break;
	if (j == num) {
	    a[num] = p;
	    num++;
	}
    }
    /* rebuild result list */
    for (i = 0, p = list; i < num; i++) {
	l = strlen(a[i]);
	bcopy(a[i], p, l);
	p += l;
	p[0] = ' ';
	p += 1;
    }
    if (p != list) {
	p--;
	p[0] = 0;
    }
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

    firstView     = NULL;
    lastView      = NULL;
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
	/*
	 * We must use TCP, since some messages may exceed the UDP limitation
	 * of messages larger than UDPMSGSIZE==8800 (at least on Linux).
	 */
	location->cl   = clnt_create(location->name,
				     SMIPROG, SMIVERS, "tcp");
	if (!location->cl) {
	    clnt_pcreateerror(location->name);
	    free(location);
	    return -1;
	}
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
    View *view;
    smi_getspec getspec;
    smi_module *smimodule;
#ifdef PARSER
    Module *module;
    struct stat buf;
    char *path;
    int rc;
#endif
	
    printDebug(4, "smiLoadMibModule(\"%s\")\n", modulename);

    if (!initialized) smiInit();

    view = (View *)malloc(sizeof(View));
    if (!view) {
	printError(NULL, ERR_ALLOCATING_VIEW, strerror(errno));
	return -1;
    }
    view->name = strdup(modulename);
    view->prev = lastView;
    view->next = NULL;
    if (lastView) {
	lastView->next = view;
    } else {
	firstView = view;
    }
    lastView = view;    
    
    for (location = firstLocation; location; location = location->next) {

	if (location->type == LOCATION_RPC) {
	    
	    getspec.name = modulename;
	    getspec.wantdescr = 0;
	    smimodule = smiproc_module_1(&getspec, location->cl);
	    if (smimodule && strlen(smimodule->name)) {
	        /* the RPC server knows this module */
		return 0;
	    }
	    
#ifdef PARSER
	} else if (location->type == LOCATION_DIR) {

	    module = findModuleByName(modulename);
	    if (module) {
	        /* already loaded. */
		return 0;
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
    printDebug(4, "smiSetDebugLevel(%d)\n", level);

    debugLevel = level;

    if (!initialized) smiInit();
}



void
smiSetErrorLevel(level)
    int level;
{
    printDebug(4, "smiSetErrorLevel(%d)\n", level);

    if (!initialized) smiInit();
    
    errorLevel = level;
}



void
smiSetFlags(userflags)
    int userflags;
{
    printDebug(4, "smiSetFlags(%x)\n", userflags);

    if (!initialized) smiInit();
    
    flags = (flags & ~SMI_FLAGMASK) | userflags;
}



int
smiGetFlags()
{
    printDebug(4, "smiGetFlags()\n");

    if (!initialized) smiInit();
    
    return flags & SMI_FLAGMASK;
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
    smi_getspec	      getspec;
    smi_module	      *smimodule;
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

	    getspec.name = name;
	    getspec.wantdescr = wantdescr;
	    smimodule = smiproc_module_1(&getspec, l->cl);
	    if (smimodule && strlen(smimodule->name)) {
		return smimodule;
	    }

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
smiGetNode(spec, mod, wantdescr)
    smi_fullname    spec;
    smi_descriptor  mod;
    int wantdescr;
{
    static smi_node res;
    Object          *o = NULL;
    Module	    *m = NULL;
    Location	    *l = NULL;
    smi_getspec	    getspec;
    smi_node	    *sminode;
    smi_fullname    sminame;
    char	    name[SMI_MAX_OID+1];
    char	    module[SMI_MAX_DESCRIPTOR+1];
    static char	    type[SMI_MAX_FULLNAME+1];
    char	    fullname[SMI_MAX_FULLNAME+1];
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif
    
    printDebug(4, "smiGetNode(\"%s\", \"%s\", %d)\n",
	       spec, mod ? mod : "NULL", wantdescr);

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    getspec.name = sminame;
	    getspec.wantdescr = wantdescr;
	    sminode = smiproc_node_1(&getspec, l->cl);
	    if (sminode && strlen(sminode->name)) {
		return sminode;
	    }

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		    path = malloc(strlen(l->name)+strlen(module)+6);
		    sprintf(path, "%s/%s", l->name, module);
		    if (!stat(path, &buf)) {
			readMibFile(path, module,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}
			free(path);
		    }
		}
		
		if (m) {
		    o = getObject(name, module);
		    if (o) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module)) {
		o = getObject(name, module);
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
                ((Descriptor *)(((Descriptor *)(o->type->parent))->ptr))->name,
			((Descriptor *)(o->type->parent))->name);
	    } else if (o->type->flags & FLAG_IMPORTED) {
		sprintf(type, "%s.%s",
                ((Descriptor *)(((Descriptor *)(o->type->parent))->ptr))->name,
			((Descriptor *)(o->type->parent))->name);
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
smiGetType(spec, mod, wantdescr)
    smi_fullname    spec;
    smi_descriptor  mod;
    int wantdescr;
{
    static smi_type res;
    Type	    *t = NULL;
    Module	    *m = NULL;
    Location	    *l = NULL;
    smi_getspec	    getspec;
    smi_type	    *smitype;
    smi_fullname    sminame;
    char	    name[SMI_MAX_OID+1];
    char	    module[SMI_MAX_DESCRIPTOR+1];
    char	    fullname[SMI_MAX_FULLNAME+1];
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif
    
    printDebug(4, "smiGetType(\"%s\", \"%s\", %d)\n",
	       spec, mod ? mod : "NULL", wantdescr);

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    getspec.name = sminame;
	    getspec.wantdescr = wantdescr;
	    smitype = smiproc_type_1(&getspec, l->cl);
	    if (smitype && strlen(smitype->name)) {
		return smitype;
	    }

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		    path = malloc(strlen(l->name)+strlen(module)+6);
		    sprintf(path, "%s/%s", l->name, module);
		    if (!stat(path, &buf)) {
			readMibFile(path, module,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}
			free(path);
		    }
		}
		
		if (m) {
		    t = getType(name, module);
		    if (t) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module)) {
		t = getType(name, module);
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
smiGetMacro(spec, mod)
    smi_fullname    spec;
    smi_descriptor  mod;
{
    static smi_macro res;
    Macro	    *ma = NULL;
    Module	    *m = NULL;
    Location	    *l = NULL;
    smi_macro	    *smimacro;
    smi_fullname    sminame;
    char	    name[SMI_MAX_OID+1];
    char	    module[SMI_MAX_DESCRIPTOR+1];
    char	    fullname[SMI_MAX_FULLNAME+1];
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif
    
    printDebug(4, "smiGetMacro(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    smimacro = smiproc_macro_1(&sminame, l->cl);
	    if (smimacro && strlen(smimacro->name)) {
		return smimacro;
	    }

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		    path = malloc(strlen(l->name)+strlen(module)+6);
		    sprintf(path, "%s/%s", l->name, module);
		    if (!stat(path, &buf)) {
			readMibFile(path, module,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}
			free(path);
		    }
		}
		
		if (m) {
		    ma = getMacro(name, module);
		    if (ma) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module)) {
		ma = getMacro(name, module);
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
smiGetNames(spec, mod)
    smi_fullname	spec;
    smi_descriptor	mod;
{
    static smi_namelist res;
    Location		*l;
    Module		*m = NULL;
    Object		*o;
    Descriptor		*d;
    Node		*n, *n2, *nn = NULL;
    smi_namelist	*smilist;
    char		*elements, *element;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
    smi_fullname	sminame;
    char		ss[SMI_MAX_FULLNAME+1];
    smi_subid		subid;
    static char		*p = NULL;
    static		plen = 0;
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif

    printDebug(4, "smiGetNames(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

#if 0
    if (strlen(module)) {
	m = findModuleByName(module);
    } else {
	m = NULL;
    }
#endif
    
    d = NULL;
    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocation; l; l = l->next) {

	if ((l->type == LOCATION_RPC) && (l->cl)) {

	    sminame = fullname;
	    smilist = smiproc_names_1(&sminame, l->cl);
	    if (smilist && (smilist->namelist)) {
		if (strlen(p)+strlen(smilist->namelist)+2 > plen) {
		    p = realloc(p, strlen(p)+strlen(smilist->namelist)+2);
		}
		if (strlen(p)) strcat(p, " ");
		strcat(p, smilist->namelist);
	    }

#ifdef PARSER
	} else if ((l->type == LOCATION_DIR) ||
		   (l->type == LOCATION_FILE)) {
	    
	    if (l->type == LOCATION_DIR) {
		
		if ((!m) && (strlen(module))) {
		    if (!(m = findModuleByName(module))) {

			path = malloc(strlen(l->name)+strlen(module)+6);
			sprintf(path, "%s/%s", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}

			if (!m) {
			    sprintf(path, "%s/%s.my", l->name, module);
			    if (!stat(path, &buf)) {
				readMibFile(path, module,
					    flags | FLAG_WHOLEMOD);
				m = findModuleByName(module);
			    }
			    free(path);
			}
		    }
		}
	    }

	    if (m || !strlen(module)) {

		if (!nn) {
		    if (isdigit(name[0])) {
			elements = strdup(name);
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
		    } else if (strchr(name, '.')) {
			elements = strdup(name);
			element = strtok(elements, ".");
			o = findObjectByName(element);
			if (o) {
			    n = o->node;
			    nn = n;
			    element = strtok(NULL, ".");
			    while (element) {
				if (isdigit(element[0])) {
				    subid = (unsigned int)strtoul(element,
								  NULL, 0);
				    n = findNodeByParentAndSubid(n, subid);
				    /* if (!n) break; */
				    nn = n;
				} else {
				    for (n2 = n->firstChild; n2;
					 n2 = n2->next) {
					for (o = n2->firstObject; o;
					     o = o->next) {
					    if (!strcmp(o->descriptor->name,
							element)) {
						nn = n2;
						break;
					    }
					}
				    }
				}
				element = strtok(NULL, ".");
			    }
			}
		    }
		}
		
		if (nn) {

		    for (o = nn->firstObject; o; o = o->next) {
			if ((o->descriptor) &&
			    ((!strlen(module)) || (o->module == m))) {
			    if (((l->type == LOCATION_FILE) &&
				 (!strcmp(l->name, o->module->path))) ||
				((l->type == LOCATION_DIR) &&
			       (!strcmp(l->name, dirname(o->module->path))))) {
				sprintf(ss, "%s.%s",
					o->module->descriptor->name,
					o->descriptor->name);
				if (strlen(p)+strlen(ss)+2 > plen) {
				    p = realloc(p, strlen(p)+strlen(ss)+2);
				}
				if (strlen(p)) strcat(p, " ");
				strcat(p, ss);
			    }
			}
		    }
		    
		} else {

		    d = NULL;
		    do {
			d = findNextDescriptor(name, m, KIND_ANY, d);
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
			    sprintf(ss, "%s.%s",
				    d->module->descriptor->name, d->name);
			    if (strlen(p)+strlen(ss)+2 > plen) {
				p = realloc(p, strlen(p)+strlen(ss)+2);
			    }
			    if (strlen(p)) strcat(p, " ");
			    strcat(p, ss);
			}

		    } while (d);
		}
	    }
	    
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    filterNamelist(p);
    
    res.namelist = p;
    return &res;
}



smi_namelist *
smiGetChildren(spec, mod)
    smi_fullname	spec;
    smi_descriptor	mod;
{
    static smi_namelist res;
    Location		*l;
    Module		*m;
    Object		*o = NULL;
    Descriptor		*d;
    Node		*n = NULL;
    smi_namelist	*smilist;
    smi_fullname	sminame;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
    char		*s;
    char		ss[SMI_MAX_FULLNAME+1];
    static char		*p = NULL;
    static		plen = 0;
#ifdef PARSER
    struct stat	    buf;
    char	    *path;
#endif

    printDebug(4, "smiGetChildren(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    d = NULL;
    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocation; l; l = l->next) {

	o = NULL;
	
	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    smilist = smiproc_children_1(&sminame, l->cl);
	    if (smilist && strlen(smilist->namelist)) {

		for(s = strtok(smilist->namelist, " ");
		    s; s = strtok(NULL, " ")) {
		    
		    if (strlen(p)+strlen(s)+2 > plen) {
			p = realloc(p, strlen(p)+strlen(s)+2);
		    }
		    if (strlen(p)) strcat(p, " ");
		    strcat(p, s);
		}
		
	    }

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		    path = malloc(strlen(l->name)+strlen(module)+6);
		    sprintf(path, "%s/%s", l->name, module);
		    if (!stat(path, &buf)) {
			readMibFile(path, module,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}
			free(path);
		    }
		}
		
		if (m) {
		    o = findObjectByModulenameAndName(module, name);
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module)) {
		o = findObjectByModulenameAndName(module, name);
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
		if (strlen(p)+strlen(s)+2 > plen) {
		    p = realloc(p, strlen(p)+strlen(s)+2);
		}
		if (strlen(p)) strcat(p, " ");
		strcat(p, s);
	    }
	}

    }

    filterNamelist(p);
    
    res.namelist = p;
    return &res;
}



smi_namelist *
smiGetMembers(spec, mod)
    smi_fullname	spec;
    smi_descriptor	mod;
{
    static smi_namelist res;
    Location		*l;
    Module		*m;
    Object		*o = NULL;
    Descriptor		*d;
    Type		*t;
    smi_fullname	sminame;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
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
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

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

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		    path = malloc(strlen(l->name)+strlen(module)+6);
		    sprintf(path, "%s/%s", l->name, module);
		    if (!stat(path, &buf)) {
			readMibFile(path, module,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}
			free(path);
		    }
		}
		
		if (m) {
		    if (isupper(name[0])) {
			t = findTypeByModulenameAndName(module, name);
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
		    } else if (islower(name[0])) {
			o = findObjectByModulenameAndName(module, name);
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
	    
	    if (strlen(module)) {
		o = findObjectByModulenameAndName(module, name);
	    }
#endif
	} else {
	    
	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    filterNamelist(p);
    
    res.namelist = p;
    return &res;
}



smi_fullname *
smiGetParent(spec, mod)
    smi_fullname	spec;
    smi_descriptor	mod;
{
    static smi_fullname res;
    Object		*o = NULL;
    Node		*n;
    Module		*m;
    Location		*l;
    View		*view;
    smi_fullname	sminame;
    smi_fullname	*sminame2;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
    char		s[SMI_MAX_FULLNAME+1];
#ifdef PARSER
    struct stat		buf;
    char		*path;
#endif
    
    printDebug(4, "smiGetParent(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);
    
    for (l = firstLocation; l; l = l->next) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    sminame2 = smiproc_parent_1(&sminame, l->cl);
	    if (sminame2 && strlen(*sminame2)) {
		return sminame2;
	    }

#ifdef PARSER
	} else if (l->type == LOCATION_DIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		    path = malloc(strlen(l->name)+strlen(module)+6);
		    sprintf(path, "%s/%s", l->name, module);
		    if (!stat(path, &buf)) {
			readMibFile(path, module,
				    flags | FLAG_WHOLEMOD);
			m = findModuleByName(module);
		    }

		    if (!m) {
			sprintf(path, "%s/%s.my", l->name, module);
			if (!stat(path, &buf)) {
			    readMibFile(path, module,
					flags | FLAG_WHOLEMOD);
			    m = findModuleByName(module);
			}
			free(path);
		    }
		}
		
		if (m) {
		    o = getObject(name, module);
		    if (o) break;
		}
	    }
	    
	} else if (l->type == LOCATION_FILE) {

	    if (strlen(module)) {
		o = getObject(name, module);
		if (o) break;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (o && (o->node) && (o->node->parent)) {
	n = o->node->parent;
	/*
	 * If the parent node has a declaration in
	 * the same module as the child, we assume
	 * this is what the user wants to get.
	 * Otherwise we return the first declaration we find in our
	 * view of modules.
	 * Last resort: the numerical OID.
	 */
	for (o = n->firstObject; o; o = o->next) {
	    if ((o->module == m) && (!(o->flags & FLAG_IMPORTED))) {
		break;
	    }
	}
	if (!o) {
	    for (o = n->firstObject; o; o = o->next) {
		for (view = firstView; view; view = view->next) 
		    if (!strcmp(o->module->descriptor->name, view->name))
			break;
		if (view && (!(o->flags & FLAG_IMPORTED))) {
		    break;
		}
	    }
	    if (!o) o = n->firstObject;
	}
	if (o && (o->descriptor)) {
	    sprintf(s, "%s.%s",
		    o->module->descriptor->name,
		    o->descriptor->name);
	    res = s;
	} else {
	    res = getOid(n);
	}
	printDebug(5, " ... = %s\n", res);
	return &res;
    } else {
	return NULL;
    }
}
