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
 * @(#) $Id: smi.c,v 1.69 2000/02/09 18:25:57 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif



const char *smi_library_version = SMI_LIBRARY_VERSION;
static int initialized = 0;



/*
 * Internal functions.
 */



int allowsNamedNumbers(SmiBasetype basetype)
{
    if ((basetype == SMI_BASETYPE_INTEGER32) ||
	(basetype == SMI_BASETYPE_BITS)) {
	return 1;
    } else {
	return 0;
    }
}



int allowsRanges(SmiBasetype basetype)
{
    if ((basetype == SMI_BASETYPE_INTEGER32) ||
	(basetype == SMI_BASETYPE_INTEGER64) ||
	(basetype == SMI_BASETYPE_UNSIGNED32) ||
	(basetype == SMI_BASETYPE_UNSIGNED64) ||
	(basetype == SMI_BASETYPE_OCTETSTRING) ||
	(basetype == SMI_BASETYPE_BITS)) {
	return 1;
    } else {
	return 0;
    }
}



void getModulenameAndName(char *arg1, char *arg2,
			  char **module, char **name)
{
    char	    *p;
    int		    l;

    if ((!arg1) && (!arg2)) {
	*module = NULL;
	*name = NULL;
    } else if (!arg2) {
#if 0
	if (isupper((int)arg1[0])) {
#endif
	    if ((p = strstr(arg1, "::"))) {
		/* SMIng style module/label separator */
		*name = util_strdup(&p[2]);
		l = strcspn(arg1, "::");
		*module = util_strndup(arg1, l);
	    } else if ((p = strchr(arg1, '!'))) {
		/* old scotty style module/label separator */
		*name = util_strdup(&p[1]);
		l = strcspn(arg1, "!");
		*module = util_strndup(arg1, l);
	    } else if ((p = strchr(arg1, '.'))) {
		/* SMIv1/v2 style module/label separator */
		*name = util_strdup(&p[1]);
		l = strcspn(arg1, ".");
		*module = util_strndup(arg1, l);
	    } else {
		*name = util_strdup(arg1);
		*module = util_strdup("");
	    }
#if 0
	} else {
	    *name = util_strdup(arg1);
	    *module = util_strdup("");
	}
#endif
    } else if (!arg1) {
#if 0
	if (isupper((int)arg2[0])) {
#endif
	    if ((p = strstr(arg2, "::"))) {
		/* SMIng style module/label separator */
		*name = util_strdup(&p[2]);
		l = strcspn(arg2, "::");
		*module = util_strndup(arg2, l);
	    } else if ((p = strchr(arg2, '!'))) {
		/* old scotty style module/label separator */
		*name = util_strdup(&p[1]);
		l = strcspn(arg2, "!");
		*module = util_strndup(arg2, l);
	    } else if ((p = strchr(arg2, '.'))) {
		/* SMIv1/v2 style module/label separator */
		*name = util_strdup(&p[1]);
		l = strcspn(arg2, ".");
		*module = util_strndup(arg2, l);
	    } else {
		*name = util_strdup(arg2);
		*module = util_strdup("");
	    }
#if 0
	} else {
	    *name = util_strdup(arg2);
	    *module = util_strdup("");
	}
#endif
    } else {
	*module = util_strdup(arg1);
	*name = util_strdup(arg2);
    }
}



Node *getNode(unsigned int oidlen, SmiSubid oid[])
{
    Node *nodePtr, *parentPtr;
    unsigned int i;

    for(nodePtr = rootNodePtr, i=0; i < oidlen; i++) {
	parentPtr = nodePtr;
	nodePtr = findNodeByParentAndSubid(parentPtr, oid[i]);
	if (!nodePtr) {
	    return parentPtr;
	}
    }
    
    return nodePtr;
}



Object *getObject(Module *modulePtr, char *name)
{
    Object	    *objectPtr = NULL;
    Node	    *nodePtr;
    SmiSubid	    oid[128];
    unsigned int    oidlen;
    char	    *p;
    
    if (isdigit((int)name[0])) {
	/*
	 * name in `1.3.0x6.1...' form.
	 */
	for (oidlen = 0, p = strtok(name, ". "); p && (oidlen < 128);
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
	if (modulePtr) {
	    objectPtr = findObjectByModuleAndName(modulePtr, name);
	} else {
	    objectPtr = findObjectByName(name);
	}
    }
    return objectPtr;
}



Object *getNextChildObject(Node *startNodePtr, Module *modulePtr,
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
		(!(objectPtr->flags & FLAG_IMPORTED)) &&
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



#if 0
SmiNode *createSmiNode(Object *objectPtr)
{
    SmiNode       *smiNodePtr;
    unsigned int  oidlen, i;
    SmiSubid	  oid[128];
    Node	  *nodePtr;
    
    if (objectPtr) {
	smiNodePtr     = util_malloc(sizeof(SmiNode));
	
	smiNodePtr->name   = objectPtr->name;
	if (objectPtr->modulePtr && objectPtr->modulePtr->export.name) {
	    smiNodePtr->module = objectPtr->modulePtr->export.name;
	} else {
	    smiNodePtr->module = NULL;
	}

	for (oidlen = 0, nodePtr = objectPtr->nodePtr;
	     (oidlen < 128) && nodePtr && (nodePtr != rootNodePtr);
	     nodePtr = nodePtr->parentPtr) {
	    oid[oidlen] = nodePtr->subid;
	    oidlen++;
	}
	smiNodePtr->oidlen = oidlen;
	smiNodePtr->oid    = util_malloc(oidlen * sizeof(SmiSubid));
	for (i=0; i < oidlen; i++) {
	    smiNodePtr->oid[i] = oid[oidlen-1-i];
	}
	
	if (objectPtr->typePtr) {
	    if (objectPtr->typePtr->export.name) {
		if (objectPtr->typePtr->modulePtr &&
		    objectPtr->typePtr->modulePtr->export.name &&
		    strlen(objectPtr->typePtr->modulePtr->export.name)) {
		  smiNodePtr->typemodule = objectPtr->typePtr->modulePtr->export.name;
		} else {
		    smiNodePtr->typemodule = NULL;
		}
		smiNodePtr->typename   = objectPtr->typePtr->export.name;
	    } else {
		/*
		 * This is an inlined type restriction. It is accessible
		 * by the same (lowercase!) name as the object.
		 */
		smiNodePtr->typemodule = objectPtr->modulePtr->export.name;
		smiNodePtr->typename   = objectPtr->name;
	    }
	} else {
	    smiNodePtr->typemodule = NULL;
	    smiNodePtr->typename   = NULL;
	}
	if (objectPtr->valuePtr) {
	    memcpy(&smiNodePtr->value, objectPtr->valuePtr,
		   sizeof(struct SmiValue));
	    if (smiNodePtr->value.format == SMI_VALUEFORMAT_OID) {
		smiNodePtr->value.value.ptr =
		    ((Object *)objectPtr->valuePtr->value.ptr)->name;
	    }
	} else {
	    smiNodePtr->value.basetype = SMI_BASETYPE_UNKNOWN;
	}
	smiNodePtr->units       = objectPtr->units;
	smiNodePtr->decl        = objectPtr->decl;
	smiNodePtr->nodekind    = objectPtr->nodekind;
	smiNodePtr->access      = objectPtr->access;
	smiNodePtr->status      = objectPtr->status;
	smiNodePtr->basetype    = objectPtr->typePtr ?
                   objectPtr->typePtr->export.basetype : SMI_BASETYPE_UNKNOWN;
	smiNodePtr->description = objectPtr->description;
	smiNodePtr->reference   = objectPtr->reference;

	if (objectPtr->indexPtr) {
	    smiNodePtr->indexkind         = objectPtr->indexPtr->indexkind;
	    smiNodePtr->implied           = objectPtr->indexPtr->implied;
	    if (objectPtr->indexPtr->rowPtr) {
		smiNodePtr->relatedmodule = objectPtr->indexPtr->rowPtr->
							      modulePtr->export.name;
		smiNodePtr->relatedname   = objectPtr->indexPtr->rowPtr->name;
	    } else {
	        smiNodePtr->relatedmodule = NULL;
	        smiNodePtr->relatedname   = NULL;
	    }
	} else {
	    smiNodePtr->indexkind         = SMI_INDEX_UNKNOWN;
	    smiNodePtr->implied           = 0;
	    smiNodePtr->relatedmodule     = NULL;
	    smiNodePtr->relatedname       = NULL;
	}

	if (objectPtr->flags & FLAG_CREATABLE) {
	    /*
	     * If new rows can be created, ...
	     */
	    smiNodePtr->create            = 1;
	} else {
	    smiNodePtr->create            = 0;
	}
	return smiNodePtr;
    } else {
	return NULL;
    }
}
#endif



/*
 * Interface Functions.
 */

int smiInit(const char *tag)
{
    char *p;
#ifdef HAVE_PWD_H
    struct passwd *pw;
#endif
    
    if (initialized) {
	return 0;
    }

    errorLevel = 3;
    smiDepth = 0;
    
    if (initData()) {
	return -1;
    }

    /* setup the smi MIB module search path */
    p = getenv("SMIPATH");
    if (p) {
	if (p[0] == ':') {
	    smiPath = util_malloc(strlen(p) + strlen(DEFAULT_SMIPATH));
	    sprintf(smiPath, "%s%s", DEFAULT_SMIPATH, p);
	} else if (p[strlen(p)-1] == ':') {
	    smiPath = util_malloc(strlen(p) + strlen(DEFAULT_SMIPATH));
	    sprintf(smiPath, "%s%s", p, DEFAULT_SMIPATH);
	} else {
	    smiPath = util_strdup(p);
	}
    } else {
	smiPath = util_strdup(DEFAULT_SMIPATH);
    }

    if (!smiPath) {
	return -1;
    }

    /* need this here, otherwise smiReadConfig would call smiInit in loop */
    initialized = 1;

    if (tag) {
	/* read global and user configuration */
	smiReadConfig(DEFAULT_GLOBALCONFIG, tag);
#ifdef HAVE_PWD_H
	pw = getpwuid(getuid());
	if (pw && pw->pw_dir) {
	    p = util_malloc(strlen(DEFAULT_USERCONFIG) +
			    strlen(pw->pw_dir) + 2);
	    sprintf(p, "%s/%s", pw->pw_dir, DEFAULT_USERCONFIG);
	    smiReadConfig(p, tag);
	}
#endif
    }
    
    return 0;
}



void smiExit()
{
    if (!initialized)
	return;

    freeData();

    util_free(smiPath);
    
    initialized = 0;
    return;
}



char *smiGetPath()
{
    if (smiPath) {
	return util_strdup(smiPath);
    } else {
	return NULL;
    }
}



int smiSetPath(const char *s)
{
    char *s2;

    if (!s) {
	util_free(smiPath);
	smiPath = NULL;
	return 0;
    }
    
    s2 = util_strdup(s);
    if (s2) {
	util_free(smiPath);
	smiPath = s2;
	return 0;
    } else {
	return -1;
    }
    
}



void smiSetSeverity(char *pattern, int severity)
{
    errorSeverity(pattern, severity);
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
		s = util_malloc(strlen(smiPath) + strlen(arg) + 2);
		sprintf(s, "%s:%s", smiPath, arg);
		util_free(smiPath);
		smiPath = s;
	    } else if (!strcmp(cmd, "level")) {
		smiSetErrorLevel(atoi(arg));
	    } else if (!strcmp(cmd, "hide")) {
		smiSetSeverity(arg, 9);
	    } else {
		printError(NULL, ERR_UNKNOWN_CONFIG_CMD, cmd, filename);
	    }
	}
	fclose(file);
	return 0;
    }
    return -1;
}



int smiModuleLoaded(char *module)
{
    return isInView(module);
}



char *smiLoadModule(char *module)
{
    Module *modulePtr;
    
    if (!initialized) smiInit(NULL);

    if (util_ispath(module)) {

	modulePtr = loadModule(module);

	if (modulePtr) {
	    if (!isInView(modulePtr->export.name)) {
		addView(modulePtr->export.name);
	    }
	    return modulePtr->export.name;
	} else {
	    return NULL;
	}

    } else {
	
	if (!isInView(module)) {
	    addView(module);
	}
	
	if ((modulePtr = findModuleByName(module))) {
	    /* already loaded. */
	    return modulePtr->export.name;
	} else {
	    if ((modulePtr = loadModule(module))) {
		return modulePtr->export.name;
	    } else {
		return NULL;
	    }
	}
    }
}
 


void smiSetErrorLevel(int level)
{
    if (!initialized) smiInit(NULL);
    
    errorLevel = level;
}



void smiSetFlags(int userflags)
{
    if (!initialized) smiInit(NULL);
    
    smiFlags = (smiFlags & ~SMI_FLAG_MASK) | userflags;
}



int smiGetFlags()
{
    if (!initialized) smiInit(NULL);
    
    return smiFlags & SMI_FLAG_MASK;
}



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
    module = util_strndup(fullname, len);

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



SmiModule *smiGetModule(char *module)
{
    Module	      *modulePtr;
    
    if (!module) {
	return NULL;
    }

    modulePtr = findModuleByName(module);
    
    if (!modulePtr) {
	modulePtr = loadModule(module);
    }
    
    return &modulePtr->export;
}



SmiModule *smiGetFirstModule()
{
    Module	      *modulePtr;

    for (modulePtr = firstModulePtr;
	 modulePtr->export.name &&
	     (strlen(modulePtr->export.name) == 0) &&
	     modulePtr;
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
	 modulePtr->export.name &&
	     (strlen(modulePtr->export.name) == 0) &&
	     modulePtr;
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



SmiType *smiGetType(SmiModule *smiModulePtr, char *type)
{
    Type	    *typePtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *type2;

    if ((!smiModulePtr) && (!type)) {
	return NULL;
    }
    
    modulePtr = (Module *)smiModulePtr;

    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, type,
			 &module2, &type2);

    if (!modulePtr && module2) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2);
	}
    }
    
    if (!modulePtr) {
	util_free(module2);
	util_free(type2);
	return NULL;
    }
    
    typePtr = findTypeByModuleAndName(modulePtr, type2);
    
    util_free(module2);
    util_free(type2);
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
	if (typePtr && typePtr->export.name && strlen(typePtr->export.name) &&
	    isupper((int)typePtr->export.name[0]) &&
	    (!(typePtr->flags & FLAG_IMPORTED)) &&
	    (typePtr->export.basetype != SMI_BASETYPE_UNKNOWN)) {
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
	if (typePtr && typePtr->export.name && strlen(typePtr->export.name) &&
	    isupper((int)typePtr->export.name[0]) &&
	    (!(typePtr->flags & FLAG_IMPORTED)) &&
	    (typePtr->export.basetype != SMI_BASETYPE_UNKNOWN)) {
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
    
    return &typePtr->export;
}



SmiModule *smiGetTypeModule(SmiType *smiTypePtr)
{
    return &((Type *)smiTypePtr)->modulePtr->export;
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
    
    if ((!smiModulePtr) && (!macro)) {
	return NULL;
    }
    
    modulePtr = (Module *)smiModulePtr;

    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, macro,
			 &module2, &macro2);

    if (!modulePtr && module2) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2);
	}
    }

    if (!modulePtr) {
	util_free(module2);
	util_free(macro2);
	return NULL;
    }
    
    macroPtr = findMacroByModuleAndName(modulePtr, macro2);
    
    util_free(module2);
    util_free(macro2);
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



SmiNode *smiGetNode(SmiModule *smiModulePtr, char *node)
{
    Object	    *objectPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *node2;
    
    if ((!smiModulePtr) && (!node)) {
	return NULL;
    }

    modulePtr = (Module *)smiModulePtr;

    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, node,
			 &module2, &node2);

    if (!modulePtr && module2) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2);
	}
    }

    if (!modulePtr) {
	util_free(module2);
	util_free(node2);
	return NULL;
    }
    
    objectPtr = getObject(modulePtr, node2);
    
    util_free(module2);
    util_free(node2);
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
    
    nodePtr = rootNodePtr;

 again:
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
    
    objectPtr = getNextChildObject(nodePtr, modulePtr, nodekind);

    if ((!objectPtr) && nodePtr) {
	goto again;
    }
    
    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetNextNode(SmiNode *smiNodePtr, SmiNodekind nodekind)
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

    if (!modulePtr) {
	return NULL;
    }

    if (!nodePtr) {
	return NULL;
    }

 again:
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
    
    objectPtr = getNextChildObject(nodePtr, modulePtr, nodekind);

    if ((!objectPtr) && nodePtr) {
	goto again;
    }

    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetParentNode(SmiNode *smiNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Import	      *importPtr;
    Node	      *nodePtr;
    Object	      o;
    
    if (!smiNodePtr) {
	return NULL;
    }

    objectPtr = (Object *)smiNodePtr;
    nodePtr = objectPtr->nodePtr;
    modulePtr = objectPtr->modulePtr;

    if (!nodePtr) {
	return NULL;
    }

    if (nodePtr == rootNodePtr) {
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
    if (objectPtr && (objectPtr->flags & FLAG_IMPORTED)) {
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
	    objectPtr = addObject("[implicit]",
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
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    
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

#if 0
    objectPtr = findObjectByModuleAndNode(modulePtr, nodePtr);

    if (!objectPtr) {
	objectPtr = findObjectByNode(nodePtr);
    }
#else
    objectPtr = findObjectByNode(nodePtr);
#endif

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

#if 0
    objectPtr = findObjectByModuleAndNode(modulePtr, nodePtr);

    if (!objectPtr) {
	objectPtr = findObjectByNode(nodePtr);
    }
#else
    objectPtr = findObjectByNode(nodePtr);
#endif
    
    return objectPtr ? &objectPtr->export : NULL;
}



SmiNode *smiGetModuleIdentityNode(SmiModule *smiModulePtr)
{
    Module	      *modulePtr;
    
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
    return &((Object *)smiNodePtr)->typePtr->export;
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
    Module	      *modulePtr;
    Object	      *listObjectPtr;
    List	      *listPtr;
    SmiIdentifier     listname;
    int		      number, i;
    
    if (!smiElementPtr) {
	return NULL;
    }

    listPtr = ((List *)smiElementPtr)->nextPtr;
    
    return (SmiElement *)listPtr;
}



SmiNode *smiGetElementNode(SmiElement *smiElementPtr)
{
    return &((Object *)((List *)smiElementPtr)->ptr)->export;
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
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     compliance;
    
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
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node, compliance;
    
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



SmiModule *smiGetRefinementModule(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->objectPtr->modulePtr->export;
}



SmiNode *smiGetRefinementNode(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->objectPtr->export;
}



SmiType *smiGetRefinementType(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->typePtr->export;
}



SmiType *smiGetRefinementWriteType(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->writetypePtr->export;
}
