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
 * @(#) $Id: smi.c,v 1.2 1999/03/15 11:07:13 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h> /* bcopy */
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef linux
#include <getopt.h>
#endif

#include "defs.h"
#include "smi.h"
#include "error.h"
#include "data.h"

#ifdef BACKEND_SMI
#include "scanner-smi.h"
#include "parser-smi.h"
#endif

#ifdef BACKEND_SMI
extern int yydebug;
#endif



static int		initialized = 0;



/*
 * Internal functions.
 */

char *
getOid(n)
    Node *n;
{
    static char o[SMI_MAX_OID+1+1];
    smi_subid a[128];
    int i, l;

    for (i = 0;
	 n && (n != rootNodePtr);
	 a[i++] = n->subid, n = n->parentPtr);
    for (strcpy(o, n ? "" : "<incomplete>."), l = strlen(o);
	 i > 0;
	 l += sprintf(&o[l], "%d.", a[--i]));
    o[l-1] = 0;

    return o;
}



void
getModulenameAndName(spec, mod, modulename, name)
    smi_fullname    spec;
    smi_descriptor  mod;
    char	    *modulename;
    char	    *name;
{
    char	    *p;
    
    /* Module::label, Module::Label, Module::label.1.2,
     * label, label.1.2,
     */
    if (isupper((int)spec[0])) {
	if ((p = strchr(spec, '!'))) {
	    /* old scotty style module/label separator */
	    strncpy(name, &p[1], SMI_MAX_OID);
	    strncpy(modulename, spec, strcspn(spec, "!"));
	} else if ((p = strchr(spec, '.'))) {
	    /* SMIv1/v2 style module/label separator */
	    strncpy(name, &p[1], SMI_MAX_OID);
	    strncpy(modulename, spec, strcspn(spec, "."));
	} else if ((p = strstr(spec, SMI_NAMESPACE_OPERATOR))) {
	    /* SMIng style module/label separator */
	    strncpy(name, &p[strlen(SMI_NAMESPACE_OPERATOR)], SMI_MAX_OID);
	    strncpy(modulename, spec, strcspn(spec, SMI_NAMESPACE_OPERATOR));
	} else {
	    strncpy(name, spec, SMI_MAX_OID);
	    strncpy(modulename, mod, SMI_MAX_DESCRIPTOR);
	}
    } else {
	strncpy(name, spec, SMI_MAX_OID);
	strncpy(modulename, mod, SMI_MAX_DESCRIPTOR);
    }
}



void
createFullname(spec, mod, fullname)
    smi_fullname spec;
    smi_descriptor mod;
    char *fullname;
{
    printDebug(5, "createFullname(\"%s\", \"%s\", ...",
	       spec, mod ? mod : "NULL");

    if ((strchr(spec, '!') || strchr(spec, '.')) &&
	(isupper((int)spec[0]))) {
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
	(isupper((int)fullname[0]) || (fullname[0] == '.'))) {
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
    View *viewPtr;

    /* loop over every element */
    for (i = 0, num = 0, p = strtok(list, " ");
	 p; p = strtok(NULL, " ")) {
	/* filter modules not in our view */
	strncpy(m, p, (l = strcspn(p, ".")));
	m[l] = 0;
	for (viewPtr = firstViewPtr; viewPtr; viewPtr = viewPtr->nextPtr) 
	    if (!strcmp(m, viewPtr->name)) break;
	if ((!viewPtr) && (!(smiFlags & SMI_VIEWALL))) break;
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

    if (isdigit((int)name[0])) {
	/*
	 * name in `1.3.0x6.1...' form.
	 */
	elements = strdup(name);
	/* TODO: success? */
	element = strtok(elements, ".");
	n = rootNodePtr;
	while (element) {
	    subid = (unsigned int)strtoul(element, NULL, 0);
	    n = findNodeByParentAndSubid(n, subid);
	    if (!n) break;
	    element = strtok(NULL, ".");
	}
	free(elements);
	if (n)
	    o = findObjectByModulenameAndNode(module, n);
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

#ifdef SMI_CONFIG_FILE
    smiReadConfig(SMI_CONFIG_FILE);
#endif
}



int
smiAddLocation(name)
    const char *name;
{
    if (!initialized) smiInit();
    
    printDebug(4, "smiAddLocation(\"%s\")\n", name);

    if (addLocation(name, smiFlags)) {
	return 0;
    } else {
	return -1;
    }
}



int
smiLoadModule(modulename)
    smi_descriptor modulename;
{

    printDebug(4, "smiLoadModule(\"%s\")\n", modulename);

    if (!initialized) smiInit();

    if (findModuleByName(modulename)) {
	/* already loaded. */
	return 0;
    }

    addView(modulename);

    if (loadModule(modulename)) {
	return 0;
    } else {
	return -1;
    }
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
    
    smiFlags = (smiFlags & ~SMI_FLAGMASK) | userflags;
}



int
smiGetFlags()
{
    printDebug(4, "smiGetFlags()\n");

    if (!initialized) smiInit();
    
    return smiFlags & SMI_FLAGMASK;
}



int
smiReadConfig(filename)
    const char *filename;
{
    char line[201], cmd[201], arg1[201], arg2[201];
    FILE *file;
    
    if (!initialized) smiInit();
    
    file = fopen(filename, "r");
    if (!file) {
	printError(NULL, ERR_OPENING_CONFIGFILE, filename,
		   strerror(errno));
	return -1;
    } else {
	while (fgets(line, sizeof(line), file)) {
	    if (feof(file)) break;
	    sscanf(line, "%s %s %s", cmd, arg1, arg2);
	    if (cmd[0] == '#') continue;
	    if (!strcmp(cmd, "location")) {
		smiAddLocation(arg1);
 	    } else if (!strcmp(cmd, "preload")) {
		smiLoadModule(arg1);
	    } else if (!strcmp(cmd, "loglevel")) {
		errorLevel = atoi(arg1);
	    } else if (!strcmp(cmd, "debuglevel")) {
		debugLevel = atoi(arg1);
	    } else if (!strcmp(cmd, "yydebug")) {
#ifdef BACKEND_SMI
		yydebug = atoi(arg1);
#else
		;
#endif
	    } else if (!strcmp(cmd, "viewall")) {
		if (atoi(arg1))
		    smiFlags |= FLAG_VIEWALL;
		else
		    smiFlags &= ~FLAG_VIEWALL;
	    } else if (!strcmp(cmd, "statistics")) {
		if (atoi(arg1))
		    smiFlags |= FLAG_STATS;
		else
		    smiFlags &= ~FLAG_STATS;
	    } else if (!strcmp(cmd, "importlogging")) {
		if (atoi(arg1))
		    smiFlags |= FLAG_RECURSIVE;
		else
		    smiFlags &= ~FLAG_RECURSIVE;
	    } else if (!strcmp(cmd, "errorlines")) {
		if (atoi(arg1))
		    smiFlags |= FLAG_ERRORLINES;
		else
		    smiFlags &= ~FLAG_ERRORLINES;
	    } else {
		printError(NULL, ERR_UNKNOWN_CONFIG_DIRECTIVE,
			   filename, cmd);
	    }
	}
	fclose(file);
    }

    return 0;
}



smi_module *
smiGetModule(name)
    smi_descriptor    name;
{
    static smi_module smiModule;
    Module	      *modulePtr;
    
    printDebug(4, "smiGetModule(\"%s\")\n", name);

    modulePtr = findModuleByName(name);
    
    if (!modulePtr) {
	modulePtr = loadModule(name);
    }
    
    if (modulePtr) {
	smiModule.name = modulePtr->name;
	if (modulePtr->objectPtr) {
	    smiModule.object = modulePtr->objectPtr->name;
	}
	smiModule.lastupdated = modulePtr->lastUpdated;
	if (smiModule.organization) {
	    smiModule.organization = modulePtr->organization;
	} else {
	    smiModule.organization = "";
	}
	if (smiModule.contactinfo) {
	    smiModule.contactinfo = modulePtr->contactInfo;
	} else {
	    smiModule.contactinfo = "";
	}
	if (modulePtr->objectPtr) {
	    if (modulePtr->objectPtr->description) {
		smiModule.description = modulePtr->objectPtr->description;
	    } else {
		smiModule.description = "";
	    }
	    if (modulePtr->objectPtr->reference) {
		smiModule.reference = modulePtr->objectPtr->reference;
	    } else {
		smiModule.reference = "";
	    }
	} else {
	    smiModule.description = "";
	    smiModule.reference = "";
	}
	return &smiModule;
    } else {
	return NULL;
    }
}



smi_node *
smiGetNode(spec, mod)
    smi_fullname    spec;
    smi_descriptor  mod;
{
    static smi_node smiNode;
    Object          *objectPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    name[SMI_MAX_OID+1];
    char	    modulename[SMI_MAX_DESCRIPTOR+1];
    static char	    typename[SMI_MAX_FULLNAME+1];
    
    printDebug(4, "smiGetNode(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    getModulenameAndName(spec, mod, modulename, name);
    
    modulePtr = findModuleByName(modulename);
    if (!modulePtr) {
	modulePtr = loadModule(modulename);
    }
    if (modulePtr) {
	objectPtr = getObject(name, modulename);
    }
    if (objectPtr) {
	smiNode.name   = objectPtr->name;
	smiNode.module = objectPtr->modulePtr->name;
	smiNode.oid    = getOid(objectPtr->nodePtr);
	if (objectPtr->typePtr) {
	    sprintf(typename, "%s%s%s",
		    objectPtr->typePtr->modulePtr->name,
		    SMI_NAMESPACE_OPERATOR,
		    objectPtr->typePtr->name);
	} else {
	    typename[0] = 0;
	}
	smiNode.type   = typename;
	smiNode.decl   = objectPtr->decl;
	smiNode.access = objectPtr->access;
	smiNode.status = objectPtr->status;
	smiNode.syntax = objectPtr->typePtr ?
	                     objectPtr->typePtr->syntax : SMI_SYNTAX_UNKNOWN;
	smiNode.description = objectPtr->description;
	printDebug(5, " ... = (%p,%s)\n", objectPtr, smiNode.name);
	return &smiNode;
    } else {
	return NULL;
    }
    



#if 0    
    for (l = firstLocationPtr; l; l = l->nextPtr) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    getspec.name = sminame;
	    getspec.wantdescr = 1;
	    sminode = smiproc_node_1(&getspec, l->cl);
	    if (sminode && strlen(sminode->name)) {
		return sminode;
	    }

#ifdef BACKEND_SMI
	} else if (l->type == LOCATION_SMIDIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {
		    m = loadModule(module);
		}
		if (m) {
		    o = getObject(name, module);
		    if (o) break;
		}
	    }
	    
	} else if (l->type == LOCATION_SMIFILE) {

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
	res.name = strdup(o->name);
	res.module = strdup(o->modulePtr->name);
	res.oid = getOid(o->nodePtr);
	if (o->typePtr) {
	    sprintf(type, "%s.%s", o->typePtr->modulePtr->name,
		    o->typePtr->name);
	} else {
	    type[0] = 0;
	}
	res.type = type;
	res.decl = o->decl;
	res.access = o->access;
	res.status = o->status;
	res.syntax = o->typePtr ? o->typePtr->syntax : SMI_SYNTAX_UNKNOWN;
	res.description = o->description;
	printDebug(5, " ... = (%p,%s)\n", o, res.name);
	return &res;
    } else {
	return NULL;
    }
#endif
}



smi_type *
smiGetType(spec, mod)
    smi_fullname    spec;
    smi_descriptor  mod;
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
    
    printDebug(4, "smiGetType(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    for (l = firstLocationPtr; l; l = l->nextPtr) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    getspec.name = sminame;
	    getspec.wantdescr = 1;
	    smitype = smiproc_type_1(&getspec, l->cl);
	    if (smitype && strlen(smitype->name)) {
		return smitype;
	    }

#ifdef BACKEND_SMI
	} else if (l->type == LOCATION_SMIDIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {

		}
		
		if (m) {
		    t = getType(name, module);
		    if (t) break;
		}
	    }
	    
	} else if (l->type == LOCATION_SMIFILE) {

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
	res.name = strdup(t->name);
	res.module = strdup(t->modulePtr->name);
	res.syntax = t->syntax;
	res.decl = t->decl;
	res.format = t->format;
	res.status = t->status;
	res.description = t->description;
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
    
    printDebug(4, "smiGetMacro(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    for (l = firstLocationPtr; l; l = l->nextPtr) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    smimacro = smiproc_macro_1(&sminame, l->cl);
	    if (smimacro && strlen(smimacro->name)) {
		return smimacro;
	    }

#ifdef BACKEND_SMI
	} else if (l->type == LOCATION_SMIDIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {
		    m = loadModule(module);
		}
		if (m) {
		    ma = getMacro(name, module);
		    if (ma) break;
		}
	    }
	    
	} else if (l->type == LOCATION_SMIFILE) {

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
	res.name = strdup(ma->name);
	res.module = strdup(ma->modulePtr->name);
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
    static int		plen = 0;

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
    
    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocationPtr; l; l = l->nextPtr) {

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

#ifdef BACKEND_SMI
	} else if ((l->type == LOCATION_SMIDIR) ||
		   (l->type == LOCATION_SMIFILE)) {
	    
	    if (l->type == LOCATION_SMIDIR) {
		
		if ((!m) && (strlen(module))) {
		    if (!(m = findModuleByName(module))) {
			m = loadModule(module);
		    }
		}
	    }

	    if (m || !strlen(module)) {

		if (!nn) {
		    if (isdigit((int)name[0])) {
			elements = strdup(name);
			/* TODO: success? */
			element = strtok(elements, ".");
			n = rootNodePtr;
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
			    n = o->nodePtr;
			    nn = n;
			    element = strtok(NULL, ".");
			    while (element) {
				if (isdigit((int)element[0])) {
				    subid = (unsigned int)strtoul(element,
								  NULL, 0);
				    n = findNodeByParentAndSubid(n, subid);
				    /* if (!n) break; */
				    nn = n;
				} else {
				    for (n2 = n->firstChildPtr; n2;
					 n2 = n2->nextPtr) {
					for (o = n2->firstObjectPtr; o;
					     o = o->nextPtr) {
					    if (!strcmp(o->name, element)) {
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

		    for (o = nn->firstObjectPtr; o; o = o->nextPtr) {
			if (((!strlen(module)) || (o->modulePtr == m))) {
			    if (((l->type == LOCATION_SMIFILE) &&
				 (!strcmp(l->name, o->modulePtr->path))) ||
				((l->type == LOCATION_SMIDIR) &&
			       (!strcmp(l->name,
					dirname(o->modulePtr->path))))) {
				sprintf(ss, "%s.%s",
					o->modulePtr->name, o->name);
				if (strlen(p)+strlen(ss)+2 > plen) {
				    p = realloc(p, strlen(p)+strlen(ss)+2);
				}
				if (strlen(p)) strcat(p, " ");
				strcat(p, ss);
			    }
			}
		    }
		    
#if 0 /* TODO */
		} else {

		    do {
			d = findNextDescriptor(name, m, KIND_ANY, d);
			if (d &&
			    (((l->type == LOCATION_SMIFILE) &&
			      (!strcmp(l->name, d->module->path))) ||
			     ((l->type == LOCATION_SMIDIR) &&
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
#endif		    
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
    Node		*n = NULL;
    smi_namelist	*smilist;
    smi_fullname	sminame;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
    char		*s;
    char		ss[SMI_MAX_FULLNAME+1];
    static char		*p = NULL;
    static int		plen = 0;

    printDebug(4, "smiGetChildren(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocationPtr; l; l = l->nextPtr) {

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

#ifdef BACKEND_SMI
	} else if (l->type == LOCATION_SMIDIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {
		    m = loadModule(module);
		}
		if (m) {
		    o = findObjectByModulenameAndName(module, name);
		}
	    }
	    
	} else if (l->type == LOCATION_SMIFILE) {

	    if (strlen(module)) {
		o = findObjectByModulenameAndName(module, name);
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}

	if (o) {
	    for (n = o->nodePtr->firstChildPtr; n; n = n->nextPtr) {
		/*
		 * For child nodes that have a declaration in
		 * the same module as the parent, we assume
		 * this is want the user wants to get.
		 * Otherwise we return the first declaration we know.
		 * Last resort: the numerical OID.
		 */
		for (o = n->firstObjectPtr; o; o = o->nextPtr) {
		    if (o->modulePtr == m) {
			break;
		    }
		}
		if (!o) {
		    o = n->firstObjectPtr;
		}
		if (o) {
		    sprintf(ss, "%s.%s",
			    o->modulePtr->name,
			    o->name);
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
    Type		*t;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
    char		*s;
    char		ss[SMI_MAX_FULLNAME+1];
    static char		*p = NULL;
    static int		plen = 0;
    List		*e;

    printDebug(4, "smiGetMembers(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);

    if (!p) {
	p = malloc(200);
    }
    strcpy(p, "");

    for (l = firstLocationPtr; l; l = l->nextPtr) {

	if (l->type == LOCATION_RPC) {

	    /* TODO */

#ifdef BACKEND_SMI
	} else if (l->type == LOCATION_SMIDIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {
		    m = loadModule(module);
		}
		if (m) {
		    if (isupper((int)name[0])) {
			t = findTypeByModulenameAndName(module, name);
			if (t) {
			    if (t->syntax == SMI_SYNTAX_SEQUENCE) {
				/* It is a SEQUENCE -> return all columns. */
				for (e = (void *)t->sequencePtr; e;
				     e = e->nextPtr) {
				    sprintf(ss, "%s.%s",
					 ((Object *)(e->ptr))->modulePtr->name,
					    ((Object *)(e->ptr))->name);
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
		    } else if (islower((int)name[0])) {
			o = findObjectByModulenameAndName(module, name);
			if (o) {
			    if (o->indexPtr) {
				for (e = (void *)o->indexPtr; e;
				     e = e->nextPtr) {
				    sprintf(ss, "%s.%s",
					 ((Object *)(e->ptr))->modulePtr->name,
					    ((Object *)(e->ptr))->name);
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

	} else if (l->type == LOCATION_SMIFILE) {
	    
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
    View		*viewPtr;
    smi_fullname	sminame;
    smi_fullname	*sminame2;
    char		name[SMI_MAX_OID+1];
    char		module[SMI_MAX_DESCRIPTOR+1];
    char		fullname[SMI_MAX_FULLNAME+1];
    char		s[SMI_MAX_FULLNAME+1];
    
    printDebug(4, "smiGetParent(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    createFullname(spec, mod, fullname);
    createModuleAndName(fullname, module, name);
    
    for (l = firstLocationPtr; l; l = l->nextPtr) {

	if (l->type == LOCATION_RPC) {

	    sminame = fullname;
	    sminame2 = smiproc_parent_1(&sminame, l->cl);
	    if (sminame2 && strlen(*sminame2)) {
		return sminame2;
	    }

#ifdef BACKEND_SMI
	} else if (l->type == LOCATION_SMIDIR) {

	    if (strlen(module)) {
		if (!(m = findModuleByName(module))) {
		    m = loadModule(module);
		}
		if (m) {
		    o = getObject(name, module);
		    if (o) break;
		}
	    }
	    
	} else if (l->type == LOCATION_SMIFILE) {

	    if (strlen(module)) {
		o = getObject(name, module);
		if (o) break;
	    }
#endif
	} else {

	    printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, l->name);
	    
	}
    }

    if (o && (o->nodePtr) && (o->nodePtr->parentPtr)) {
	n = o->nodePtr->parentPtr;
	/*
	 * If the parent node has a declaration in
	 * the same module as the child, we assume
	 * this is what the user wants to get.
	 * Otherwise we return the first declaration we find in our
	 * view of modules.
	 * Last resort: the numerical OID.
	 */
	for (o = n->firstObjectPtr; o; o = o->nextPtr) {
	    if ((o->modulePtr == m) && (!(o->flags & FLAG_IMPORTED))) {
		break;
	    }
	}
	if (!o) {
	    for (o = n->firstObjectPtr; o; o = o->nextPtr) {
		for (viewPtr = firstViewPtr; viewPtr;
		     viewPtr = viewPtr->nextPtr) 
		    if (!strcmp(o->modulePtr->name, viewPtr->name))
			break;
		if (viewPtr && (!(o->flags & FLAG_IMPORTED))) {
		    break;
		}
	    }
	    if (!o) o = n->firstObjectPtr;
	}
	if (o) {
	    sprintf(s, "%s.%s",
		    o->modulePtr->name, o->name);
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



time_t
smiMkTime(const char *s)
{
    struct tm tm;
    char tmp[3];
    
    tm.tm_isdst = 0;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_sec = 0;

    if (strlen(s) == 11) {
	/* seems to be SMIv2 11-char format yymmddhhmmZ */
	tmp[2] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 2));
	tm.tm_mon = atoi(strncpy(tmp, &s[2], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[4], 2));
	tm.tm_hour = atoi(strncpy(tmp, &s[6], 2));
	tm.tm_min = atoi(strncpy(tmp, &s[8], 2));
    } else if (strlen(s) == 13) {
	/* seems to be SMIv2 11-char format yymmddhhmmZ */
	tmp[4] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 4)) - 1900;
	tmp[2] = 0;
	tm.tm_mon = atoi(strncpy(tmp, &s[4], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[6], 2));
	tm.tm_hour = atoi(strncpy(tmp, &s[8], 2));
	tm.tm_min = atoi(strncpy(tmp, &s[10], 2));
    }
    /* TODO: SMIng format */

    /* printf("XX %s -> %s %s", s, strncpy(tmp, &s[0], 2), asctime(&tm)); */
    return mktime(&tm);
}
