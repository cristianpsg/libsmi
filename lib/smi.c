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
 * @(#) $Id: smi.c,v 1.64 2000/02/06 23:30:58 strauss Exp $
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



Object *getObject(char *module, char *name)
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
	    if (module) {
		objectPtr = findObjectByModulenameAndNode(module, nodePtr);
	    } else {
		objectPtr = findObjectByNode(nodePtr);
	    }
	}
    } else {
	if (module) {
	    objectPtr = findObjectByModulenameAndName(module, name);
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
		 (nodekind & objectPtr->nodekind))) {
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



SmiListItem *createSmiListItem(Object *listObjectPtr, Object *objectPtr, int number)
{
    SmiListItem *smiListItemPtr;

    if (objectPtr) {
	smiListItemPtr = util_malloc(sizeof(SmiListItem));
	smiListItemPtr->listmodule = listObjectPtr->modulePtr->export.name;
	smiListItemPtr->listname   = listObjectPtr->name;
	smiListItemPtr->module     = objectPtr->modulePtr->export.name;
	smiListItemPtr->name       = objectPtr->name;
	smiListItemPtr->number     = number;
	return smiListItemPtr;
    } else {
	return NULL;
    }
}



SmiNamedNumber *createSmiNamedNumber(Type *typePtr, NamedNumber *nnPtr)
{
    SmiNamedNumber *smiNamedNumberPtr;

    if (typePtr && nnPtr) {
	smiNamedNumberPtr = util_malloc(sizeof(SmiNamedNumber));
	smiNamedNumberPtr->typemodule = typePtr->modulePtr->export.name;
	smiNamedNumberPtr->typename   = typePtr->export.name;
	smiNamedNumberPtr->name       = nnPtr->name;
	memcpy(&smiNamedNumberPtr->value, nnPtr->valuePtr,
	       sizeof(struct SmiValue));
	return smiNamedNumberPtr;
    } else {
	return NULL;
    }
}



SmiRange *createSmiRange(Type *typePtr, Range *rangePtr)
{
    SmiRange *smiRangePtr;

    if (typePtr && rangePtr) {
	smiRangePtr = util_malloc(sizeof(SmiRange));
	smiRangePtr->typemodule  = typePtr->modulePtr->export.name;
	smiRangePtr->typename    = typePtr->export.name;
	memcpy(&smiRangePtr->minValue, rangePtr->minValuePtr,
	       sizeof(struct SmiValue));
	memcpy(&smiRangePtr->maxValue, rangePtr->maxValuePtr,
	       sizeof(struct SmiValue));
	return smiRangePtr;
    } else {
	return NULL;
    }
}



SmiOption *createSmiOption(Object *objectPtr, Option *optionPtr)
{
    SmiOption *smiOptionPtr;
    
    if (optionPtr) {
        smiOptionPtr = util_malloc(sizeof(SmiOption));
	smiOptionPtr->compliancemodule = objectPtr->modulePtr->export.name;
	smiOptionPtr->compliancename   = objectPtr->name;
	smiOptionPtr->module           = optionPtr->objectPtr->modulePtr->export.name;
	smiOptionPtr->name             = optionPtr->objectPtr->name;
	smiOptionPtr->description      = optionPtr->description;
	return smiOptionPtr;
    } else {
	return NULL;
    }
}



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



void smiFreeModule(SmiModule *smiModulePtr)
{
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



void smiFreeImport(SmiImport *smiImportPtr)
{
}



int smiIsImported(SmiModule *smiModulePtr,
		  char *module, char *name)
{
    Import	   *importPtr;
    Module	   *modulePtr;
    char	   *module2, *name2;
    
    if ((!smiModulePtr) || (!module) || (!name)) {
	return 0;
    }
    
    modulePtr = (Module *)smiModulePtr;
    
    getModulenameAndName(module, name,
			 &module2, &name2);

    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if ((!strcmp(module2, importPtr->export.module)) &&
	    (!strcmp(name2, importPtr->export.name))) {
	    util_free(module2);
	    util_free(name2);
	    return 1;
	}
    }

    util_free(module2);
    util_free(name2);
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



void smiFreeRevision(SmiRevision *smiRevisionPtr)
{
}



SmiType *smiGetType(SmiModule *smiModulePtr, char *type)
{
    Type	    *typePtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *type2;

    if ((!smiModulePtr) && (!type)) {
	return NULL;
    }
    
    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, type,
			 &module2, &type2);

    if (module2) {
	if ((smiModulePtr) && !strcmp(module2, smiModulePtr->name)) {
	    modulePtr = (Module *)smiModulePtr;
	} else {
	    if (!(modulePtr = findModuleByName(module2))) {
		modulePtr = loadModule(module2);
	    }
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



void smiFreeType(SmiType *smiTypePtr)
{
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
    
    return createSmiNamedNumber(typePtr, typePtr->listPtr->ptr);
}



SmiNamedNumber *smiGetNextNamedNumber(SmiNamedNumber *smiNamedNumberPtr)
{
    Type  *typePtr;
    List  *listPtr;
    
    if (!smiNamedNumberPtr) {
	return NULL;
    }
    
    typePtr = findTypeByModulenameAndName(smiNamedNumberPtr->typemodule,
					  smiNamedNumberPtr->typename);

    if ((!typePtr) || (!typePtr->listPtr) ||
	((typePtr->export.basetype != SMI_BASETYPE_ENUM) &&
	 (typePtr->export.basetype != SMI_BASETYPE_BITS))) {
	return NULL;
    }

    for (listPtr = typePtr->listPtr; listPtr; listPtr = listPtr->nextPtr) {
	if (((NamedNumber *)(listPtr->ptr))->name == smiNamedNumberPtr->name)
	    break;
    }

    smiFreeNamedNumber(smiNamedNumberPtr);
    
    if ((!listPtr) || (!listPtr->nextPtr)) {
	return NULL;
    }
	
    return createSmiNamedNumber(typePtr, listPtr->nextPtr->ptr);
}



void smiFreeNamedNumber(SmiNamedNumber *smiNamedNumberPtr)
{
    util_free(smiNamedNumberPtr);
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

    return createSmiRange(typePtr, typePtr->listPtr->ptr);
}



SmiRange *smiGetNextRange(SmiRange *smiRangePtr)
{
    Type  *typePtr;
    List  *listPtr;

    if (!smiRangePtr) {
	return NULL;
    }
    
    typePtr = findTypeByModulenameAndName(smiRangePtr->typemodule,
					  smiRangePtr->typename);

    if ((!typePtr) || (!typePtr->listPtr) ||
	(typePtr->export.basetype == SMI_BASETYPE_ENUM) ||
	(typePtr->export.basetype == SMI_BASETYPE_BITS)) {
	return NULL;
    }
 
    for (listPtr = typePtr->listPtr; listPtr; listPtr = listPtr->nextPtr) {
	if (!memcmp(((Range *)listPtr->ptr)->minValuePtr,
		    &smiRangePtr->minValue, sizeof(struct SmiValue)))
	    break;
    }

    smiFreeRange(smiRangePtr);
    
    if ((!listPtr) || (!listPtr->nextPtr)) {
	return NULL;
    }
	
    return createSmiRange(typePtr, listPtr->nextPtr->ptr);
}



void smiFreeRange(SmiRange *smiRangePtr)
{
    util_free(smiRangePtr);
}



SmiMacro *smiGetMacro(SmiModule *smiModulePtr, char *macro)
{
    Macro	    *macroPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *macro2;
    
    if ((!smiModulePtr) && (!macro)) {
	return NULL;
    }
    
    getModulenameAndName(smiModulePtr ? smiModulePtr->name : NULL, macro,
			 &module2, &macro2);

    if (module2) {
	if ((smiModulePtr) && !strcmp(module2, smiModulePtr->name)) {
	    modulePtr = (Module *)smiModulePtr;
	} else {
	    if (!(modulePtr = findModuleByName(module2))) {
		modulePtr = loadModule(module2);
	    }
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
    return &macroPtr->export;
}



SmiMacro *smiGetFirstMacro(SmiModule *smiModulePtr)
{
    if (!smiModulePtr) {
	return NULL;
    }
    
    return &((Module *)smiModulePtr)->firstMacroPtr->export;
}



SmiMacro *smiGetNextMacro(SmiMacro *smiMacroPtr)
{
    if (!smiMacroPtr) {
	return NULL;
    }

    return &((Macro *)smiMacroPtr)->nextPtr->export;
}


SmiModule *smiGetMacroModule(SmiMacro *smiMacroPtr)
{
    return &((Macro *)smiMacroPtr)->modulePtr->export;
}



void smiFreeMacro(SmiMacro *smiMacroPtr)
{
}



SmiNode *smiGetNode(char *module, char *node)
{
    Object	    *objectPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *node2;
    
    if ((!module) && (!node)) {
	return NULL;
    }
    
    getModulenameAndName(module, node, &module2, &node2);

    if (module2) {
	if (!(modulePtr = findModuleByName(module2))) {
	    modulePtr = loadModule(module2);
	}

	if (!modulePtr) {
	    util_free(module2);
	    util_free(node2);
	    return NULL;
	}
    }

    objectPtr = getObject(module2, node2);
    
    util_free(module2);
    util_free(node2);
    
    return createSmiNode(objectPtr);
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

    return createSmiNode(objectPtr);
}



SmiNode *smiGetFirstNode(char *module, SmiNodekind nodekind)
{
    Module *modulePtr;
    Node   *nodePtr = NULL;
    Object *objectPtr;

    if (!module) {
	return NULL;
    }
    
    modulePtr = findModuleByName(module);
    
    if (!modulePtr) {
	modulePtr = loadModule(module);
    }
    
    if (!modulePtr) {
	return NULL;
    }

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
    
    return createSmiNode(objectPtr);
}



SmiNode *smiGetNextNode(SmiNode *smiNodePtr, SmiNodekind nodekind)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNodePtr->module);
    }

    if ((smiNodePtr->name) && strlen(smiNodePtr->name)) {

	objectPtr = findObjectByModuleAndName(modulePtr, smiNodePtr->name);

	if (!objectPtr) {
	    smiFreeNode(smiNodePtr);
	    return NULL;
	}

	nodePtr = objectPtr->nodePtr;
	
    } else {
	
	nodePtr = getNode(smiNodePtr->oidlen, smiNodePtr->oid);
	    
    }
        
    smiFreeNode(smiNodePtr);
    
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

    return createSmiNode(objectPtr);
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

    modulePtr = findModuleByName(smiNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    if ((smiNodePtr->name) && strlen(smiNodePtr->name)) {

	objectPtr = findObjectByModuleAndName(modulePtr, smiNodePtr->name);

	if (!objectPtr) {
	    return NULL;
	}

	nodePtr = objectPtr->nodePtr;
	
    } else {
	
	nodePtr = getNode(smiNodePtr->oidlen, smiNodePtr->oid);
	    
    }

    if ((!nodePtr) || (nodePtr == rootNodePtr)) {
	return NULL;
    }

    nodePtr = nodePtr->parentPtr;

    /*
     * First, try to find a definition in the same module.
     */
    objectPtr = NULL;
    if (smiNodePtr->module) {
	objectPtr = findObjectByModulenameAndNode(smiNodePtr->module, nodePtr);
    }

    /*
     * If found, check if it's imported. In case, get the original definition.
     */
    if (objectPtr && (objectPtr->flags & FLAG_IMPORTED)) {
	importPtr = findImportByName(objectPtr->name, objectPtr->modulePtr);
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
	    o.modulePtr = modulePtr;
	    o.name = NULL;
	    o.fileoffset = -1;
	    o.decl = SMI_DECL_UNKNOWN;
	    o.nodekind = SMI_NODEKIND_UNKNOWN;
	    o.flags = 0;
	    o.typePtr = NULL;
	    o.access = SMI_ACCESS_UNKNOWN;
	    o.status = SMI_STATUS_UNKNOWN;
	    o.indexPtr = NULL;
	    o.listPtr = NULL;
	    o.optionlistPtr = NULL;
	    o.refinementlistPtr = NULL;
	    o.description = NULL;
	    o.reference = NULL;
	    o.format = NULL;
	    o.units = NULL;
	    o.valuePtr = NULL;
	    o.nodePtr = nodePtr;
	    objectPtr = &o;
	}
    }

    return createSmiNode(objectPtr);
}



SmiNode *smiGetFirstChildNode(SmiNode *smiNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, smiNodePtr->name);

    if (!objectPtr) {
	return NULL;
    }

    nodePtr = objectPtr->nodePtr;
    
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

    return createSmiNode(objectPtr);
}



SmiNode *smiGetNextChildNode(SmiNode *smiNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    
    if (!smiNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNodePtr->module);
    }

    if (!modulePtr) {
	smiFreeNode(smiNodePtr);
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, smiNodePtr->name);

    smiFreeNode(smiNodePtr);
    
    if (!objectPtr) {
	return NULL;
    }

    nodePtr = objectPtr->nodePtr;

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
    
    return createSmiNode(objectPtr);
}



SmiNode *smiGetModuleIdentityNode(SmiModule *smiModulePtr)
{
    Module	      *modulePtr;
    
    if (!smiModulePtr) {
	return NULL;
    }

    return createSmiNode(((Module *)smiModulePtr)->objectPtr);
}



SmiModule *smiGetNodeModule(SmiNode *smiNodePtr)
{
    return &((Object *)smiNodePtr)->modulePtr->export;
}



SmiType *smiGetNodeType(SmiNode *smiNodePtr)
{
    Type *typePtr;
    
    typePtr = findTypeByModulenameAndName(smiNodePtr->typemodule,
					  smiNodePtr->typename);
    return &typePtr->export;
    /* return &((Object *)smiNodePtr)->typePtr->export; */
}



void smiFreeNode(SmiNode *smiNodePtr)
{
    util_free(smiNodePtr->oid);
    util_free(smiNodePtr);
}



SmiListItem *smiGetFirstListItem(SmiNode *smiListNodePtr)
{
    Module	      *modulePtr;
    Object	      *listObjectPtr;
    List	      *listPtr;
    
    if (!smiListNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiListNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiListNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    listObjectPtr = findObjectByModuleAndName(modulePtr, smiListNodePtr->name);

    if (!listObjectPtr) {
	return NULL;
    }

    if (listObjectPtr->nodekind == SMI_NODEKIND_ROW) {
	if ((!listObjectPtr->indexPtr) ||
	    (!listObjectPtr->indexPtr->listPtr)) {
	    return NULL;
	} else {
	    listPtr = listObjectPtr->indexPtr->listPtr;
	}
    } else {
	listPtr = listObjectPtr->listPtr;
    }

    if (!listPtr) {
	return NULL;
    }
    
    return createSmiListItem(listObjectPtr, listPtr->ptr, 1);
}



SmiListItem *smiGetNextListItem(SmiListItem *smiListItemPtr)
{
    Module	      *modulePtr;
    Object	      *listObjectPtr;
    List	      *listPtr;
    SmiIdentifier     listname;
    int		      number, i;
    
    if (!smiListItemPtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiListItemPtr->listmodule);

    if (!modulePtr) {
	modulePtr = loadModule(smiListItemPtr->listmodule);
    }

    listname = smiListItemPtr->listname;
    number = smiListItemPtr->number;
    
    smiFreeListItem(smiListItemPtr);
    
    if (!modulePtr) {
	return NULL;
    }

    listObjectPtr = findObjectByModuleAndName(modulePtr, listname);

    if (!listObjectPtr) {
	return NULL;
    }

    if (listObjectPtr->nodekind == SMI_NODEKIND_ROW) {
	if ((!listObjectPtr->indexPtr) ||
	    (!listObjectPtr->indexPtr->listPtr)) {
	    return NULL;
	} else {
	    listPtr = listObjectPtr->indexPtr->listPtr;
	}
    } else {
	listPtr = listObjectPtr->listPtr;
    }
    
    for (i = 1; (i <= (number+1)) && listPtr;
	 i++, listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) && (i == (number+1))) {
	    return createSmiListItem(listObjectPtr, listPtr->ptr, i);
	}
    }
    
    return NULL;
}



void smiFreeListItem(SmiListItem *smiListItemPtr)
{
    util_free(smiListItemPtr);
}



SmiOption *smiGetFirstOption(SmiNode *smiComplianceNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    
    if (!smiComplianceNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiComplianceNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiComplianceNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiComplianceNodePtr->name);

    if ((!objectPtr) || (!objectPtr->optionlistPtr)) {
	return NULL;
    }

    if (objectPtr->nodekind != SMI_NODEKIND_COMPLIANCE) {
	return NULL;
    }
						     
    return createSmiOption(objectPtr, objectPtr->optionlistPtr->ptr);
}



SmiOption *smiGetNextOption(SmiOption *smiOptionPtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node, compliance;
    
    if (!smiOptionPtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiOptionPtr->compliancemodule);

    if (!modulePtr) {
	modulePtr = loadModule(smiOptionPtr->compliancemodule);
    }

    module = smiOptionPtr->module;
    node = smiOptionPtr->name;
    compliance = smiOptionPtr->compliancename;
        
    smiFreeOption(smiOptionPtr);

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, compliance);

    if (!objectPtr) {
	return NULL;
    }

    if (!objectPtr->optionlistPtr) {
	return NULL;
    }

    if (objectPtr->nodekind != SMI_NODEKIND_COMPLIANCE) {
	return NULL;
    }
						     
    for (listPtr = objectPtr->optionlistPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Option *)(listPtr->ptr))->objectPtr->modulePtr->export.name,
		    module) &&
	    !strcmp(((Option *)(listPtr->ptr))->objectPtr->name, node)) {
	    if (listPtr->nextPtr) {
		return createSmiOption(objectPtr, listPtr->nextPtr->ptr);
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



void smiFreeOption(SmiOption *smiOptionPtr)
{
    util_free(smiOptionPtr);
}



SmiRefinement *smiGetFirstRefinement(SmiNode *smiComplianceNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    
    if (!smiComplianceNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiComplianceNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiComplianceNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiComplianceNodePtr->name);

    if ((!objectPtr) || (!objectPtr->refinementlistPtr)) {
	return NULL;
    }

    if (objectPtr->nodekind != SMI_NODEKIND_COMPLIANCE) {
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
    /* return &((Refinement *)smiRefinementPtr)->objectPtr->export; */
    return createSmiNode(((Refinement *)smiRefinementPtr)->objectPtr);
}



SmiType *smiGetRefinementType(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->typePtr->export;
}



SmiType *smiGetRefinementWriteType(SmiRefinement *smiRefinementPtr)
{
    return &((Refinement *)smiRefinementPtr)->writetypePtr->export;
}



void smiFreeRefinement(SmiRefinement *smiRefinementPtr)
{
    util_free(smiRefinementPtr);
}

