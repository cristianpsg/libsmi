/*
 * smi.c --
 *
 *      Interface Implementation of libsmi.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi.c,v 1.110 2002/06/10 09:15:15 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#include "smi.h"
#include "data.h"
#include "error.h"
#include "util.h"

#ifdef BACKEND_SMI
#include "scanner-smi.h"
#include "parser-smi.h"
#endif

#ifdef BACKEND_SMING
#include "scanner-sming.h"
#include "parser-sming.h"
#endif

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif



#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif



const char *smi_library_version = SMI_LIBRARY_VERSION;
const char *smi_version_string = SMI_VERSION_STRING;

Handle *smiHandle = NULL;



/*
 * Internal functions.
 */

static void getModulenameAndName(const char *arg1, const char *arg2,
				 char **module, char **name)
{
    char	    *p;
    int		    l;

    if ((!arg1) && (!arg2)) {
	*module = NULL;
	*name = NULL;
    } else if (!arg2) {
	if (isupper((int)arg1[0])) {
	    if ((p = strstr(arg1, "::"))) {
		/* SMIng style module/label separator */
		*name = smiStrdup(&p[2]);
		l = strcspn(arg1, "::");
		*module = smiStrndup(arg1, l);
	    } else if ((p = strchr(arg1, '!'))) {
		/* old scotty style module/label separator */
		*name = smiStrdup(&p[1]);
		l = strcspn(arg1, "!");
		*module = smiStrndup(arg1, l);
	    } else if ((p = strchr(arg1, '.'))) {
		/* SMIv1/v2 style module/label separator */
		*name = smiStrdup(&p[1]);
		l = strcspn(arg1, ".");
		*module = smiStrndup(arg1, l);
	    } else {
		*name = smiStrdup(arg1);
		*module = smiStrdup("");
	    }
	} else {
	    *name = smiStrdup(arg1);
	    *module = smiStrdup("");
	}
    } else if (!arg1) {
	if (isupper((int)arg2[0])) {
	    if ((p = strstr(arg2, "::"))) {
		/* SMIng style module/label separator */
		*name = smiStrdup(&p[2]);
		l = strcspn(arg2, "::");
		*module = smiStrndup(arg2, l);
	    } else if ((p = strchr(arg2, '!'))) {
		/* old scotty style module/label separator */
		*name = smiStrdup(&p[1]);
		l = strcspn(arg2, "!");
		*module = smiStrndup(arg2, l);
	    } else if ((p = strchr(arg2, '.'))) {
		/* SMIv1/v2 style module/label separator */
		*name = smiStrdup(&p[1]);
		l = strcspn(arg2, ".");
		*module = smiStrndup(arg2, l);
	    } else {
		*name = smiStrdup(arg2);
		*module = smiStrdup("");
	    }
	} else {
	    *name = smiStrdup(arg2);
	    *module = smiStrdup("");
	}
    } else {
	*module = smiStrdup(arg1);
	*name = smiStrdup(arg2);
    }
}



static Node *getNode(unsigned int oidlen, SmiSubid oid[])
{
    Node *nodePtr, *parentPtr;
    unsigned int i;

    for(nodePtr = smiHandle->rootNodePtr, i=0; i < oidlen; i++) {
	parentPtr = nodePtr;
	nodePtr = findNodeByParentAndSubid(parentPtr, oid[i]);
	if (!nodePtr) {
	    return parentPtr;
	}
    }
    
    return nodePtr;
}



static Object *getNextChildObject(Node *startNodePtr, Module *modulePtr,
				  SmiNodekind nodekind)
{
    Node   *nodePtr;
    Object *objectPtr = NULL;

    if (!startNodePtr || !modulePtr)
	return NULL;

    for (nodePtr = startNodePtr; nodePtr; nodePtr = nodePtr->nextPtr) {
	for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextSameNodePtr) {
	    if (((!modulePtr) || (objectPtr->modulePtr == modulePtr)) &&
		((nodekind == SMI_NODEKIND_ANY) ||
		 (nodekind & objectPtr->export.nodekind))) {
		break;
	    }
	}
	if (objectPtr) break;
	objectPtr = getNextChildObject(nodePtr->firstChildPtr,
				       modulePtr, nodekind);
	if (objectPtr) break;
    }

    return objectPtr;
}



/*
 * Interface Functions.
 */

int smiInit(const char *tag)
{
    char *p, *pp, *tag2;
#ifdef HAVE_PWD_H
    struct passwd *pw;
#endif

    smiHandle = findHandleByName(tag);
    if (smiHandle) {
	return 0;
    }
    smiHandle = addHandle(tag);
    
    smiDepth = 0;

    smiHandle->errorLevel = DEFAULT_ERRORLEVEL;
    smiHandle->errorHandler = smiErrorHandler;
#if !defined(_MSC_VER)
    smiHandle->cache = NULL;
    smiHandle->cacheProg = NULL;
#endif
    
    if (smiInitData()) {
	return -1;
    }

    /*
     * Setup the SMI MIB module search path:
     *  1. set to builtin DEFAULT_SMIPATH
     *  2. read global config file if present (append/prepend/replace)
     *  3. read user config file if present (append/prepend/replace)
     *  4. evaluate SMIPATH env-var if set (append/prepend/replace)
     */

    /* 1. set to builtin DEFAULT_SMIPATH */
    smiHandle->path = smiStrdup(DEFAULT_SMIPATH);

    tag2 = smiStrdup(tag);
    if (tag2) tag2 = strtok(tag2, ":");
    if (tag2) {
	/* 2. read global config file if present (append/prepend/replace) */
	smiReadConfig(DEFAULT_GLOBALCONFIG, tag2);
#ifdef HAVE_PWD_H
	pw = getpwuid(getuid());
	if (pw && pw->pw_dir) {
	    /* 3. read user config file if present (append/prepend/replace) */
	    p = smiMalloc(strlen(DEFAULT_USERCONFIG) +
			    strlen(pw->pw_dir) + 2);
	    sprintf(p, "%s%c%s", pw->pw_dir, DIR_SEPARATOR, DEFAULT_USERCONFIG);
	    smiReadConfig(p, tag2);
	    smiFree(p);
	}
#endif
    }
    smiFree(tag2);

    /* 4. evaluate SMIPATH env-var if set (append/prepend/replace) */
    p = getenv("SMIPATH");
    if (p) {
	if (p[0] == PATH_SEPARATOR) {
	    pp = smiMalloc(strlen(p) + strlen(smiHandle->path) + 1);
	    sprintf(pp, "%s%s", smiHandle->path, p);
	    smiFree(smiHandle->path);
	    smiHandle->path = pp;
	} else if (p[strlen(p)-1] == PATH_SEPARATOR) {
	    pp = smiMalloc(strlen(p) + strlen(smiHandle->path) + 1);
	    sprintf(pp, "%s%s", p, smiHandle->path);
	    smiFree(smiHandle->path);
	    smiHandle->path = pp;
	} else {
	    smiHandle->path = smiStrdup(p);
	}
    }
    
    if (!smiHandle->path) {
	return -1;
    }

    return 0;
}



void smiExit()
{
    if (!smiHandle)
	return;

    smiFreeData();

    smiFree(smiHandle->path);
#if !defined(_MSC_VER)
    smiFree(smiHandle->cache);
    smiFree(smiHandle->cacheProg);
#endif
    
    removeHandle(smiHandle);
    
    smiHandle = NULL;
    return;
}



char *smiGetPath()
{
    if (smiHandle->path) {
	return smiStrdup(smiHandle->path);
    } else {
	return NULL;
    }
}



int smiSetPath(const char *s)
{
    char *s2;

    if (!smiHandle) smiInit(NULL);

    if (!s) {
	smiFree(smiHandle->path);
	smiHandle->path = NULL;
	return 0;
    }
    
    s2 = smiStrdup(s);
    if (s2) {
	smiFree(smiHandle->path);
	smiHandle->path = s2;
	return 0;
    } else {
	return -1;
    }
    
}



void smiSetSeverity(char *pattern, int severity)
{
    smiSetErrorSeverity(pattern, severity);
}



int smiReadConfig(const char *filename, const char *tag)
{
    FILE *file;
    char buf[201];
    char *cmd, *arg, *s;
    
    file = fopen(filename, "r");
    if (file) {
	while (!feof(file)) {
	    if (!fgets(buf, 200, file)) continue;
	    if ((!strlen(buf)) || (buf[0] == '#')) continue;
	    cmd = strtok(buf, " \t\n\r");
	    if (!cmd) continue;
	    if (cmd[0] == '#') continue;
	    if (cmd[strlen(cmd)-1] == ':') {
		if (!tag) continue;
		cmd[strlen(cmd)-1] = 0;
		if (strcmp(cmd, tag)) continue;
		cmd = strtok(NULL, " \t\n\r");
	    }
	    arg = strtok(NULL, " \t\n\r");
	    if (!strcmp(cmd, "load")) {
		smiLoadModule(arg);
	    } else if (!strcmp(cmd, "path")) {
		if (arg) {
		    if (arg[0] == PATH_SEPARATOR) {
			s = smiMalloc(strlen(arg) + strlen(smiHandle->path) + 1);
			sprintf(s, "%s%s", smiHandle->path, arg);
			smiFree(smiHandle->path);
			smiHandle->path = s;
		    } else if (arg[strlen(arg)-1] == PATH_SEPARATOR) {
			s = smiMalloc(strlen(arg) + strlen(smiHandle->path) + 1);
			sprintf(s, "%s%s", arg, smiHandle->path);
			smiFree(smiHandle->path);
			smiHandle->path = s;
		    } else {
			smiHandle->path = smiStrdup(arg);
		    }
		}
	    } else if (!strcmp(cmd, "cache")) {
#if !defined(_MSC_VER)
		smiFree(smiHandle->cache);
		smiFree(smiHandle->cacheProg);
#endif
		if (arg && strcmp(arg, "off")) {
#if !defined(_MSC_VER)
		    smiHandle->cache = smiStrdup(arg);
		    arg = strtok(NULL, "\n\r");
		    smiHandle->cacheProg = smiStrdup(arg);
#else
		    smiPrintError(NULL, ERR_CACHE_CONFIG_NOT_SUPPORTED,
				  filename);
#endif
		}
	    } else if (!strcmp(cmd, "level")) {
		smiSetErrorLevel(atoi(arg));
	    } else if (!strcmp(cmd, "hide")) {
		smiSetSeverity(arg, 9);
	    } else {
		smiPrintError(NULL, ERR_UNKNOWN_CONFIG_CMD, cmd, filename);
	    }
	}
	fclose(file);
	return 0;
    }
    return -1;
}



int smiIsLoaded(const char *module)
{
    if (!module)
	return 0;
    
    return isInView(module);
}



char *smiLoadModule(const char *module)
{
    Module *modulePtr;
    
    if (!smiHandle) smiInit(NULL);

    if (smiIsPath(module)) {

	modulePtr = loadModule(module, NULL);

	if (modulePtr) {
	    if (!isInView(modulePtr->export.name)) {
		addView(modulePtr->export.name);
	    }
	    return modulePtr->export.name;
	} else {
	    return NULL;
	}

    } else {
	
	if ((modulePtr = findModuleByName(module))) {
	    /* already loaded. */
	    if (!isInView(module)) {
		addView(module);
	    }
	    return modulePtr->export.name;
	} else {
	    if ((modulePtr = loadModule(module, NULL))) {
		if (!isInView(module)) {
		    addView(module);
		}
		return modulePtr->export.name;
	    } else {
		return NULL;
	    }
	}
    }
}
 


void smiSetErrorLevel(int level)
{
    if (!smiHandle) smiInit(NULL);
    
    smiHandle->errorLevel = level;
}



void smiSetFlags(int userflags)
{
    if (!smiHandle) smiInit(NULL);
    
    smiHandle->flags = (smiHandle->flags & ~SMI_FLAG_MASK) | userflags;
}



int smiGetFlags()
{
    if (!smiHandle) smiInit(NULL);
    
    return smiHandle->flags & SMI_FLAG_MASK;
}



#if 0
char *smiModule(char *fullname)
{
    int	 len;
    char *module;

    if (!fullname) {
	return NULL;
    }

    if (islower((int)fullname[0])) {
	return NULL;
    }
    
    len = strcspn(fullname, "!.");
    len = MIN(len, strcspn(fullname, "::"));
    module = smiStrndup(fullname, len);

    return module;
}



char *smiDescriptor(char *fullname)
{
    char *p, *name;

    if (!fullname) {
	return NULL;
    }
    
    p = strstr(fullname, "::");
    if (p) {
	name = strdup(&p[2]);
    } else {
	p = strchr(fullname, '!');
	if (p) {
	    name = strdup(&p[1]);
	} else {
	    if (!isupper((int)fullname[0])) {
		name = strdup(fullname);
	    } else {
		p = strchr(fullname, '.');
		if (p) {
		    name = strdup(&p[1]);
		} else {
		    name = strdup(fullname);
		}
	    }
	}
    }
	 
    return name;
}
#endif



SmiModule *smiGetModule(const char *module)
{
    Module	      *modulePtr;
    
    if (!module) {
	return NULL;
    }

    modulePtr = findModuleByName(module);
    
    if (!modulePtr) {
	modulePtr = loadModule(module, NULL);
    }
    
    return &modulePtr->export;
}



SmiModule *smiGetFirstModule()
{
    Module	      *modulePtr;

    for (modulePtr = smiHandle->firstModulePtr;
	 modulePtr && modulePtr->export.name &&
	     (strlen(modulePtr->export.name) == 0);
	 modulePtr = modulePtr->nextPtr);

    return &modulePtr->export;
}



SmiModule *smiGetNextModule(SmiModule *smiModulePtr)
{
    Module	      *modulePtr;
    
    if (!smiModulePtr) {
	return NULL;
    }

    
    for (modulePtr = ((Module *)smiModulePtr)->nextPtr;
	 modulePtr && modulePtr->export.name &&
	     (strlen(modulePtr->export.name) == 0);
	 modulePtr = modulePtr->nextPtr);

    return &modulePtr->export;
}



SmiImport *smiGetFirstImport(SmiModule *smiModulePtr)
{
    if (!smiModulePtr) {
	return NULL;
    }
    
    return &((Module *)smiModulePtr)->firstImportPtr->export;
}



SmiImport *smiGetNextImport(SmiImport *smiImportPtr)
{
    if (!smiImportPtr) {
	return NULL;
    }

    return &((Import *)smiImportPtr)->nextPtr->export;
}



int smiIsImported(SmiModule *smiModulePtr,
		  SmiModule *importedModulePtr,
		  char *importedName)
{
    Import	   *importPtr;
    Module	   *modulePtr;
    char	   *importedModule;
    
    if ((!smiModulePtr) || (!importedName)) {
	return 0;
    }

    modulePtr = (Module *)smiModulePtr;
    
    if (importedModulePtr) {
	importedModule = importedModulePtr->name;
    } else {
	importedModule = NULL;
    }
	
    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if ((!strcmp(importedName, importPtr->export.name)) &&
	    ((!importedModule) ||
	     (!strcmp(importedModule, importPtr->export.module)))) {
	    return 1;
	}
    }

    return 0;
}



SmiRevision *smiGetFirstRevision(SmiModule *smiModulePtr)
{
    if (!smiModulePtr) {
	return NULL;
    }
    
    return &((Module *)smiModulePtr)->firstRevisionPtr->export;
}



SmiRevision *smiGetNextRevision(SmiRevision *smiRevisionPtr)
{
    if (!smiRevisionPtr) {
	return NULL;
    }

    return &((Revision *)smiRevisionPtr)->nextPtr->export;
}



int smiGetRevisionLine(SmiRevision *smiRevisionPtr)
{
    return ((Revision *)smiRevisionPtr)->line;
}



SmiType *smiGetType(SmiModule *smiModulePtr, char *type)
{
    Type	    *typePtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *type2;

    if (!type) {
	return NULL;
    }

    modulePtr = (Module *)smiModulePtr;

    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, type,
			 &module2, &type2);
    
    if (!modulePtr && module2 && strlen(module2)) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2, NULL);
	}
    }

    if (modulePtr) {
	typePtr = findTypeByModuleAndName(modulePtr, type2);
    } else {
	typePtr = findTypeByName(type2);
    }
    
    smiFree(module2);
    smiFree(type2);

    if (!typePtr ||
	typePtr->export.basetype == SMI_BASETYPE_UNKNOWN) {
	return NULL;
    }
    
    return &typePtr->export;
}



SmiType *smiGetFirstType(SmiModule *smiModulePtr)
{
    Type *typePtr;
    
    if (!smiModulePtr) {
	return NULL;
    }
    
    for (typePtr = ((Module *)smiModulePtr)->firstTypePtr; typePtr;
	 typePtr = typePtr->nextPtr) {
	/* loop until we found a `real' type */
	if (typePtr->export.name &&
	    typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
	    break;
	}
    }
    
    return &typePtr->export;
}



SmiType *smiGetNextType(SmiType *smiTypePtr)
{
    Type *typePtr;

    if (!smiTypePtr) {
	return NULL;
    }

    for (typePtr = ((Type *)smiTypePtr)->nextPtr; typePtr;
	 typePtr = typePtr->nextPtr) {
	/* loop until we found a `real' type */
	if (typePtr->export.name &&
	    typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
	    break;
	}
    }
    
    return &typePtr->export;
}


SmiType *smiGetParentType(SmiType *smiTypePtr)
{
    Type *typePtr;

    if (!smiTypePtr) {
	return NULL;
    }

    typePtr = ((Type *)smiTypePtr)->parentPtr;
    
    if (!typePtr ||
	typePtr->export.basetype == SMI_BASETYPE_UNKNOWN) {
	return NULL;
    }
    
    return &typePtr->export;
}



SmiModule *smiGetTypeModule(SmiType *smiTypePtr)
{
    return &((Type *)smiTypePtr)->modulePtr->export;
}

int smiGetTypeLine(SmiType *smiTypePtr)
{
    return ((Type *)smiTypePtr)->line;
}



SmiNamedNumber *smiGetFirstNamedNumber(SmiType *smiTypePtr)
{
    Type    *typePtr;

    typePtr = (Type *)smiTypePtr;
    
    if ((!typePtr) || (!typePtr->listPtr) ||
	((typePtr->export.basetype != SMI_BASETYPE_ENUM) &&
	 (typePtr->export.basetype != SMI_BASETYPE_BITS))) {
	return NULL;
    }
    
    return &((NamedNumber *)typePtr->listPtr->ptr)->export;
}



SmiNamedNumber *smiGetNextNamedNumber(SmiNamedNumber *smiNamedNumberPtr)
{
    Type  *typePtr;
    List  *listPtr;
    
    if (!smiNamedNumberPtr) {
	return NULL;
    }
    
    typePtr = ((NamedNumber *)smiNamedNumberPtr)->typePtr;

    
    if ((!typePtr) || (!typePtr->listPtr) ||
	((typePtr->export.basetype != SMI_BASETYPE_ENUM) &&
	 (typePtr->export.basetype != SMI_BASETYPE_BITS))) {
	return NULL;
    }

    for (listPtr = typePtr->listPtr; listPtr; listPtr = listPtr->nextPtr) {
	if (((NamedNumber *)(listPtr->ptr))->export.name ==
	                                               smiNamedNumberPtr->name)
	    break;
    }

    if ((!listPtr) || (!listPtr->nextPtr)) {
	return NULL;
    }
	
    return &((NamedNumber *)listPtr->nextPtr->ptr)->export;
}



SmiRange *smiGetFirstRange(SmiType *smiTypePtr)
{
    Type    *typePtr;

    typePtr = (Type *)smiTypePtr;
    
    if ((!typePtr) || (!typePtr->listPtr) ||
	(typePtr->export.basetype == SMI_BASETYPE_ENUM) ||
	(typePtr->export.basetype == SMI_BASETYPE_BITS)) {
	return NULL;
    }

    return &((Range *)typePtr->listPtr->ptr)->export;
}



SmiRange *smiGetNextRange(SmiRange *smiRangePtr)
{
    Type  *typePtr;
    List  *listPtr;

    if (!smiRangePtr) {
	return NULL;
    }
    
    typePtr = ((Range *)smiRangePtr)->typePtr;

    if ((!typePtr) || (!typePtr->listPtr) ||
	(typePtr->export.basetype == SMI_BASETYPE_ENUM) ||
	(typePtr->export.basetype == SMI_BASETYPE_BITS)) {
	return NULL;
    }
 
    for (listPtr = typePtr->listPtr; listPtr; listPtr = listPtr->nextPtr) {
	if (!memcmp(&((Range *)listPtr->ptr)->export.minValue,
		    &smiRangePtr->minValue, sizeof(struct SmiValue)))
	    break;
    }

    if ((!listPtr) || (!listPtr->nextPtr)) {
	return NULL;
    }
	
    return &((Range *)listPtr->nextPtr->ptr)->export;
}



SmiMacro *smiGetMacro(SmiModule *smiModulePtr, char *macro)
{
    Macro	    *macroPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *macro2;
    
    if (!macro) {
	return NULL;
    }

    modulePtr = (Module *)smiModulePtr;

    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, macro,
			 &module2, &macro2);
    
    if (!modulePtr && module2 && strlen(module2)) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2, NULL);
	}
    }

    if (modulePtr) {
	macroPtr = findMacroByModuleAndName(modulePtr, macro2);
    } else {
	macroPtr = findMacroByName(macro2);
    }
    
    smiFree(module2);
    smiFree(macro2);
    return macroPtr ? &macroPtr->export : NULL;
}



SmiMacro *smiGetFirstMacro(SmiModule *smiModulePtr)
{
    if (!smiModulePtr) {
	return NULL;
    }
    
    return ((Module *)smiModulePtr)->firstMacroPtr ?
	&((Module *)smiModulePtr)->firstMacroPtr->export : NULL;
}



SmiMacro *smiGetNextMacro(SmiMacro *smiMacroPtr)
{
    if (!smiMacroPtr) {
	return NULL;
    }

    return ((Macro *)smiMacroPtr)->nextPtr ?
	&((Macro *)smiMacroPtr)->nextPtr->export : NULL;
}


SmiModule *smiGetMacroModule(SmiMacro *smiMacroPtr)
{
    return &((Macro *)smiMacroPtr)->modulePtr->export;
}


SmiNode *smiGetNode(SmiModule *smiModulePtr, const char *node)
{
    Object	    *objectPtr = NULL;
    Module	    *modulePtr = NULL;
    Node            *nodePtr;
    char	    *module2, *node2, *p;
    unsigned int    oidlen;
    SmiSubid	    oid[128];
    
    if (!node) {
	return NULL;
    }

    modulePtr = (Module *)smiModulePtr;

    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, node,
			 &module2, &node2);
    
    if (!modulePtr && module2 && strlen(module2)) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2, NULL);
	}
    }

    if (isdigit((int)node2[0])) {
	for (oidlen = 0, p = strtok(node2, ". "); p;
	     oidlen++, p = strtok(NULL, ". ")) {
	    oid[oidlen] = strtoul(p, NULL, 0);
	}
	nodePtr = getNode(oidlen, oid);
	if (nodePtr) {
	    if (modulePtr) {
		objectPtr = findObjectByModuleAndNode(modulePtr, nodePtr);
	    } else {
		objectPtr = findObjectByNode(nodePtr);
	    }
	}
    } else {
	p = strtok(node2, ". ");
	if (modulePtr) {
	    objectPtr = findObjectByModuleAndName(modulePtr, p);
	} else {
	    objectPtr = findObjectByName(p);
	}
    }
    
    smiFree(module2);
    smiFree(node2);
    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetNodeByOID(unsigned int oidlen, SmiSubid oid[])
{
    Node            *nodePtr;
    Object	    *objectPtr;
    
    if (!oidlen) {
	return NULL;
    }

    nodePtr = getNode(oidlen, oid);

    if (!nodePtr) {
	return NULL;
    }
    
    objectPtr = findObjectByNode(nodePtr);

    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetFirstNode(SmiModule *smiModulePtr, SmiNodekind nodekind)
{
    Module *modulePtr;
    Node   *nodePtr = NULL;
    Object *objectPtr;

    if (!smiModulePtr) {
	return NULL;
    }
    
    modulePtr = (Module *)smiModulePtr;

    if (modulePtr && modulePtr->prefixNodePtr) {
	/* start at the common oid prefix of this module */
	nodePtr = modulePtr->prefixNodePtr;
    } else {
	nodePtr = smiHandle->rootNodePtr->firstChildPtr;
    }

    do {
	objectPtr = getNextChildObject(nodePtr, modulePtr, nodekind);
	
	if (objectPtr)
	    return &objectPtr->export;
	
	if (nodePtr->firstChildPtr) {
	    nodePtr = nodePtr->firstChildPtr;
	} else if (nodePtr->nextPtr) {
	    nodePtr = nodePtr->nextPtr;
	} else {
	    for (nodePtr = nodePtr->parentPtr;
		 (nodePtr->parentPtr) && (!nodePtr->nextPtr);
		 nodePtr = nodePtr->parentPtr);
	    nodePtr = nodePtr->nextPtr;
	}
    } while (nodePtr);

    return NULL;
}



SmiNode *smiGetNextNode(SmiNode *smiNodePtr, SmiNodekind nodekind)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    int               i;
    
    if (!smiNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiNodePtr;
    nodePtr = objectPtr->nodePtr;
    modulePtr = objectPtr->modulePtr;

    if (!modulePtr) {
	return NULL;
    }

    if (!nodePtr) {
	return NULL;
    }

    do {
	if (nodePtr->firstChildPtr) {
	    nodePtr = nodePtr->firstChildPtr;
	} else if (nodePtr->nextPtr) {
	    nodePtr = nodePtr->nextPtr;
	} else {
	    for (nodePtr = nodePtr->parentPtr;
		 (nodePtr->parentPtr) && (!nodePtr->nextPtr);
		 nodePtr = nodePtr->parentPtr);
	    nodePtr = nodePtr->nextPtr;
	    /* did we move outside the common oid prefix of this module? */
	    for (i = 0; i < modulePtr->prefixNodePtr->oidlen; i++)
		if ((!nodePtr) ||
		    (nodePtr->oid[i] != modulePtr->prefixNodePtr->oid[i]))
		    return NULL;
	}

	objectPtr = getNextChildObject(nodePtr, modulePtr, nodekind);

	if (objectPtr)
	    return &objectPtr->export;
	
    } while (nodePtr);

    return NULL;
}



SmiNode *smiGetParentNode(SmiNode *smiNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Import	      *importPtr;
    Node	      *nodePtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiNodePtr;
    nodePtr = objectPtr->nodePtr;
    modulePtr = objectPtr->modulePtr;

    if (!nodePtr) {
	return NULL;
    }

    if (nodePtr == smiHandle->rootNodePtr) {
	return NULL;
    }

    nodePtr = nodePtr->parentPtr;

    /*
     * First, try to find a definition in the same module.
     */
    objectPtr = NULL;
    if (modulePtr) {
	objectPtr = findObjectByModuleAndNode(modulePtr, nodePtr);
    }

    /*
     * If found, check if it's imported. In case, get the original definition.
     */
    if (objectPtr) {
	importPtr = findImportByName(objectPtr->export.name,
				     objectPtr->modulePtr);
	if (importPtr) {
	    objectPtr = findObjectByModulenameAndNode(importPtr->export.module,
						      nodePtr);
	} else {
	    objectPtr = NULL;
	}
    }
    
    /*
     * If not yet found, try to find any definition.
     */
    if (!objectPtr) {
	objectPtr = findObjectByNode(nodePtr);

	if (!objectPtr) {
	    /* in implicitly created node, e.g. gaga.0 in an object
	     * definition with oid == gaga.0.1.
	     */
	    objectPtr = addObject("<implicit>",
				  nodePtr->parentPtr, nodePtr->subid,
				  0, NULL);
	    objectPtr->nodePtr = nodePtr;
	    objectPtr->modulePtr = modulePtr;
	}
    }

    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetRelatedNode(SmiNode *smiNodePtr)
{
    if (!smiNodePtr) {
	return NULL;
    }

    return &((Object *)smiNodePtr)->relatedPtr->export;
}



SmiNode *smiGetFirstChildNode(SmiNode *smiNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;

    if (!smiNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiNodePtr;
    nodePtr = objectPtr->nodePtr;
    modulePtr = objectPtr->modulePtr;

    if (!nodePtr) {
	return NULL;
    }

    nodePtr = nodePtr->firstChildPtr;

    if (!nodePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndNode(modulePtr, nodePtr);
    if (!objectPtr) objectPtr = findObjectByNode(nodePtr);

    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetNextChildNode(SmiNode *smiNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiNodePtr;
    nodePtr = objectPtr->nodePtr;
    modulePtr = objectPtr->modulePtr;

    if (!nodePtr) {
	return NULL;
    }

    nodePtr = nodePtr->nextPtr;

    if (!nodePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndNode(modulePtr, nodePtr);
    if (!objectPtr) objectPtr = findObjectByNode(nodePtr);
    
    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetModuleIdentityNode(SmiModule *smiModulePtr)
{
    if (!smiModulePtr) {
	return NULL;
    }

    return &((Module *)smiModulePtr)->objectPtr->export;
}



SmiModule *smiGetNodeModule(SmiNode *smiNodePtr)
{
    return &((Object *)smiNodePtr)->modulePtr->export;
}



SmiType *smiGetNodeType(SmiNode *smiNodePtr)
{
    Type *typePtr;

    typePtr = ((Object *)smiNodePtr)->typePtr;
    
    if (!typePtr ||
	typePtr->export.basetype == SMI_BASETYPE_UNKNOWN) {
	return NULL;
    }
    
    return &typePtr->export;
}

int smiGetNodeLine(SmiNode *smiNodePtr)
{
    return ((Object *)smiNodePtr)->line;
}



SmiElement *smiGetFirstElement(SmiNode *smiNodePtr)
{
    List	      *listPtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    listPtr = ((Object *)smiNodePtr)->listPtr;

    return (SmiElement *)listPtr;
}



SmiElement *smiGetNextElement(SmiElement *smiElementPtr)
{
    List	      *listPtr;
    
    if (!smiElementPtr) {
	return NULL;
    }

    listPtr = ((List *)smiElementPtr)->nextPtr;
    
    return (SmiElement *)listPtr;
}



SmiNode *smiGetElementNode(SmiElement *smiElementPtr)
{
    if ((Object *)((List *)smiElementPtr)->ptr)
        return &((Object *)((List *)smiElementPtr)->ptr)->export;
    else
        return NULL;
}



SmiOption *smiGetFirstOption(SmiNode *smiComplianceNodePtr)
{
    Object	      *objectPtr;
    
    if (!smiComplianceNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiComplianceNodePtr;
    
    if (!objectPtr->optionlistPtr) {
	return NULL;
    }

    if (objectPtr->export.nodekind != SMI_NODEKIND_COMPLIANCE) {
	return NULL;
    }
						     
    return &((Option *)objectPtr->optionlistPtr->ptr)->export;
}



SmiOption *smiGetNextOption(SmiOption *smiOptionPtr)
{
    List	      *listPtr;
    
    if (!smiOptionPtr) {
	return NULL;
    }
						     
    for (listPtr =
	    ((Option *)smiOptionPtr)->compliancePtr->optionlistPtr;
	 listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((Option *)(listPtr->ptr) == (Option *)smiOptionPtr) {
	    if (listPtr->nextPtr) {
		return &((Option *)listPtr->nextPtr->ptr)->export;
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



SmiNode *smiGetOptionNode(SmiOption *smiOptionPtr)
{
    return &((Option *)smiOptionPtr)->objectPtr->export;
}



SmiRefinement *smiGetFirstRefinement(SmiNode *smiComplianceNodePtr)
{
    Object	      *objectPtr;
    
    if (!smiComplianceNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiComplianceNodePtr;
    
    if (!objectPtr->refinementlistPtr) {
	return NULL;
    }

    if (objectPtr->export.nodekind != SMI_NODEKIND_COMPLIANCE) {
	return NULL;
    }
						     
    return &((Refinement *)objectPtr->refinementlistPtr->ptr)->export;
}



SmiRefinement *smiGetNextRefinement(SmiRefinement *smiRefinementPtr)
{
    List	      *listPtr;
    
    if (!smiRefinementPtr) {
	return NULL;
    }
						     
    for (listPtr =
	    ((Refinement *)smiRefinementPtr)->compliancePtr->refinementlistPtr;
	 listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((Refinement *)(listPtr->ptr) == (Refinement *)smiRefinementPtr) {
	    if (listPtr->nextPtr) {
		return &((Refinement *)listPtr->nextPtr->ptr)->export;
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



SmiNode *smiGetRefinementNode(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->objectPtr->export;
}



SmiType *smiGetRefinementType(SmiRefinement *smiRefinementPtr)
{
    Type *typePtr;

    typePtr = ((Refinement *)smiRefinementPtr)->typePtr;
    
    if (!typePtr ||
	typePtr->export.basetype == SMI_BASETYPE_UNKNOWN) {
	return NULL;
    }
    
    return &typePtr->export;
}



SmiType *smiGetRefinementWriteType(SmiRefinement *smiRefinementPtr)
{
    Type *typePtr;

    typePtr = ((Refinement *)smiRefinementPtr)->writetypePtr;
    
    if (!typePtr ||
	typePtr->export.basetype == SMI_BASETYPE_UNKNOWN) {
	return NULL;
    }
    
    return &typePtr->export;
}



SmiElement *smiGetFirstUniquenessElement(SmiNode *smiNodePtr)
{
    List	      *listPtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    listPtr = ((Object *)smiNodePtr)->uniquenessPtr;

    return (SmiElement *)listPtr;
}



