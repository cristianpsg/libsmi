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
 * @(#) $Id: smi.c,v 1.33 1999/06/02 16:52:34 strauss Exp $
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

#include "smi.h"
#include "error.h"
#include "data.h"
#include "util.h"

#ifdef BACKEND_SMI
#include "scanner-smi.h"
#include "parser-smi.h"
#endif

#ifdef BACKEND_SMING
#include "scanner-sming.h"
#include "parser-sming.h"
#endif

#ifdef BACKEND_SMI
extern int smidebug;
#endif

#ifdef BACKEND_SMING
extern int smingdebug;
#endif

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif


static int		initialized = 0;



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



char *getOid(Node *n)
{
    char o[SMI_MAX_OID+1+1];
    SmiSubid a[128];
    int i, l;

    for (i = 0; n && (n != rootNodePtr); n = n->parentPtr) {
	a[i++] = n->subid;
    }
    if (!n) {
	/* strip off heading `0.0' if in pending tree */
	i -= 1;
	strcpy(o, "<unknown>.");
    } else {
	strcpy(o, "");
    }
    for (l = strlen(o); i > 0; l += sprintf(&o[l], "%d.", a[--i]));
    o[l-1] = 0;
    return util_strdup(o);
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
	if (isupper((int)arg1[0])) {
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
		*module = NULL;
	    }
	} else {
	    *name = util_strdup(arg1);
	    *module = NULL;
	}
    } else if (!arg1) {
	if (isupper((int)arg2[0])) {
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
		*module = NULL;
	    }
	} else {
	    *name = util_strdup(arg2);
	    *module = NULL;
	}
    } else {
	*module = util_strdup(arg1);
	*name = util_strdup(arg2);
    }
}



Object *getObject(char *module, char *name)
{
    Object	    *objectPtr = NULL;
    Node	    *nodePtr;
    char	    *elements, *element;
    SmiSubid	    subid;
    
    if (isdigit((int)name[0])) {
	/*
	 * name in `1.3.0x6.1...' form.
	 */
	elements = util_strdup(name);
	element = strtok(elements, ".");
	nodePtr = rootNodePtr;
	while (element) {
	    subid = (unsigned int)strtoul(element, NULL, 0);
	    nodePtr = findNodeByParentAndSubid(nodePtr, subid);
	    if (!nodePtr) break;
	    element = strtok(NULL, ".");
	}
	free(elements);
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



Object *getNextChildObject(Node *startNodePtr, Module *modulePtr, SmiDecl decl)
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
		((decl == SMI_DECL_UNKNOWN) || (decl == objectPtr->decl))) {
		break;
	    }
	}
	if (objectPtr) break;
	objectPtr = getNextChildObject(nodePtr->firstChildPtr,
				       modulePtr, decl);
	if (objectPtr) break;
    }

    return objectPtr;
}



SmiModule *createSmiModule(Module *modulePtr)
{
    SmiModule     *smiModulePtr;
    
    if (modulePtr) {
	smiModulePtr = util_malloc(sizeof(SmiModule));
	
	smiModulePtr->name = modulePtr->name;
	if (modulePtr->objectPtr) {
	    smiModulePtr->object = modulePtr->objectPtr->name;
	} else {
	    smiModulePtr->object = NULL;
	}
	smiModulePtr->lastupdated = modulePtr->lastUpdated;
	if (modulePtr->organization) {
	    smiModulePtr->organization = modulePtr->organization;
	} else {
	    smiModulePtr->organization = NULL;
	}
	if (modulePtr->contactInfo) {
	    smiModulePtr->contactinfo = modulePtr->contactInfo;
	} else {
	    smiModulePtr->contactinfo = NULL;
	}
	if (modulePtr->objectPtr) {
	    if (modulePtr->objectPtr->description) {
		smiModulePtr->description = modulePtr->objectPtr->description;
	    } else {
		smiModulePtr->description = NULL;
	    }
	    if (modulePtr->objectPtr->reference) {
		smiModulePtr->reference = modulePtr->objectPtr->reference;
	    } else {
		smiModulePtr->reference = NULL;
	    }
	} else {
	    smiModulePtr->description = NULL;
	    smiModulePtr->reference = NULL;
	}
	return smiModulePtr;
    } else {
	return NULL;
    }
}



SmiImport *createSmiImport(Import *importPtr)
{
    SmiImport *smiImportPtr;
    
    if (importPtr) {
	smiImportPtr = util_malloc(sizeof(SmiImport));
	smiImportPtr->module       = importPtr->modulePtr->name;
	smiImportPtr->importmodule = importPtr->importmodule;
	smiImportPtr->importname   = importPtr->importname;
	return smiImportPtr;
    } else {
	return NULL;
    }
}



SmiRevision *createSmiRevision(Revision *revisionPtr)
{
    SmiRevision *smiRevisionPtr;
    
    if (revisionPtr) {
        smiRevisionPtr = util_malloc(sizeof(SmiRevision));
	smiRevisionPtr->module = revisionPtr->modulePtr->name;
	smiRevisionPtr->date   = revisionPtr->date;
	if (revisionPtr->description) {
	    smiRevisionPtr->description = revisionPtr->description;
	} else {
	    smiRevisionPtr->description = NULL;
	}
	return smiRevisionPtr;
    } else {
	return NULL;
    }
}



SmiNode *createSmiNode(Object *objectPtr)
{
    SmiNode       *smiNodePtr;
    
    if (objectPtr) {
	smiNodePtr     = util_malloc(sizeof(SmiNode));
	
	smiNodePtr->name   = objectPtr->name;
	smiNodePtr->module = objectPtr->modulePtr->name;
	smiNodePtr->oid    = getOid(objectPtr->nodePtr);
	if (objectPtr->typePtr) {
	    if (objectPtr->typePtr->name) {
		if (strlen(objectPtr->typePtr->modulePtr->name)) {
		  smiNodePtr->typemodule = objectPtr->typePtr->modulePtr->name;
		} else {
		    smiNodePtr->typemodule = NULL;
		}
		smiNodePtr->typename   = objectPtr->typePtr->name;
	    } else {
		/*
		 * This is an inlined type restriction. It is accessible
		 * by the same (lowercase!) name as the object.
		 */
		smiNodePtr->typemodule = objectPtr->modulePtr->name;
		smiNodePtr->typename   = objectPtr->name;
	    }
	} else {
	    smiNodePtr->typemodule = NULL;
	    smiNodePtr->typename   = NULL;
	}
	smiNodePtr->valuePtr    = objectPtr->valuePtr;
	smiNodePtr->decl        = objectPtr->decl;
	smiNodePtr->access      = objectPtr->access;
	smiNodePtr->status      = objectPtr->status;
	smiNodePtr->basetype    = objectPtr->typePtr ?
	                   objectPtr->typePtr->basetype : SMI_BASETYPE_UNKNOWN;
	smiNodePtr->description = objectPtr->description;
	smiNodePtr->reference   = objectPtr->reference;

	if (objectPtr->indexPtr) {
	    smiNodePtr->indexkind         = objectPtr->indexPtr->indexkind;
	    smiNodePtr->implied           = objectPtr->indexPtr->implied;
	    if (objectPtr->indexPtr->rowPtr) {
		smiNodePtr->relatedmodule = objectPtr->indexPtr->rowPtr->
							      modulePtr->name;
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

#if 0
	    if (objectPtr->flags & FLAG_INDEXLABELS) {
		/*
		 * This object's index ptrs contain unchecked identifier
		 * strings instead of Object pointers, because it was read
		 * from an SMIng module with potential forward references
		 * in index clauses.
		 */
		for (listPtr = objectPtr->indexPtr->listPtr; listPtr;
		     listPtr = listPtr->nextPtr) {
		    if (listPtr->ptr) {
			addPtr(&smiNodePtr->index, listPtr->ptr);
		    }
		}
	    } else {
		for (listPtr = objectPtr->indexPtr->listPtr; listPtr;
		     listPtr = listPtr->nextPtr) {
		    if (listPtr->ptr) {
			addName(&smiNodePtr->index,
				((Object *)listPtr->ptr)->modulePtr->name,
				((Object *)listPtr->ptr)->name);
		    }
		}
	    }
	
	smiNodePtr->list	= NULL;
	if (objectPtr->flags & FLAG_CREATABLE) {
	    /*
	     * If new rows can be created, create at least an empty list,
	     * instead of returning NULL as the list.
	     */
	    addPtr(&smiNodePtr->list, NULL);
	}
	for (listPtr = objectPtr->listPtr; listPtr;
	     listPtr = listPtr->nextPtr) {
	    addPtr(&smiNodePtr->list, ((Object *)listPtr->ptr)->name);
	}
#endif
	
	return smiNodePtr;
    } else {
	return NULL;
    }
}



SmiType *createSmiType(Type *typePtr)
{
    SmiType *smiTypePtr;
    
    if (typePtr) {
	smiTypePtr = util_malloc(sizeof(SmiType));
	smiTypePtr->name         = typePtr->name;
	smiTypePtr->module       = typePtr->modulePtr->name;
	smiTypePtr->basetype	 = typePtr->basetype;
	if (typePtr->parentType &&
	    strstr(typePtr->parentType, "::")) {
	    smiTypePtr->parentmodule = smiModule(typePtr->parentType);
	    smiTypePtr->parentname   = smiDescriptor(typePtr->parentType);
	} else {
	    smiTypePtr->parentmodule = NULL;
	    smiTypePtr->parentname   = typePtr->parentType;
	}
	smiTypePtr->decl	 = typePtr->decl;
	smiTypePtr->format	 = typePtr->format;
	smiTypePtr->valuePtr	 = typePtr->valuePtr;
	smiTypePtr->units	 = typePtr->units;
	smiTypePtr->status	 = typePtr->status;
	smiTypePtr->description  = typePtr->description;
	smiTypePtr->reference    = typePtr->reference;
	return smiTypePtr;
    } else {
	return NULL;
    }
}



SmiNamedNumber *createSmiNamedNumber(Type *typePtr, NamedNumber *nnPtr)
{
    SmiNamedNumber *smiNamedNumberPtr;

    if (typePtr && nnPtr) {
	smiNamedNumberPtr = util_malloc(sizeof(SmiNamedNumber));
	smiNamedNumberPtr->module   = typePtr->modulePtr->name;
	smiNamedNumberPtr->type     = typePtr->name;
	smiNamedNumberPtr->name     = nnPtr->name;
	smiNamedNumberPtr->valuePtr = nnPtr->valuePtr;
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
	smiRangePtr->module      = typePtr->modulePtr->name;
	smiRangePtr->type        = typePtr->name;
	smiRangePtr->minValuePtr = rangePtr->minValuePtr;
	smiRangePtr->maxValuePtr = rangePtr->maxValuePtr;
	return smiRangePtr;
    } else {
	return NULL;
    }
}



SmiMacro *createSmiMacro(Macro *macroPtr)
{
    SmiMacro *smiMacroPtr;
    
    if (macroPtr) {
	smiMacroPtr = util_malloc(sizeof(SmiMacro));
	smiMacroPtr->name   = macroPtr->name;
	smiMacroPtr->module = macroPtr->modulePtr->name;
	return smiMacroPtr;
    } else {
	return NULL;
    }
}



SmiOption *createSmiOption(Object *objectPtr, Option *optionPtr)
{
    SmiOption *smiOptionPtr;
    
    if (optionPtr) {
        smiOptionPtr = util_malloc(sizeof(SmiOption));
	smiOptionPtr->compliancemodule = objectPtr->modulePtr->name;
	smiOptionPtr->compliancename   = objectPtr->name;
	smiOptionPtr->module           = optionPtr->objectPtr->modulePtr->name;
	smiOptionPtr->name             = optionPtr->objectPtr->name;
	smiOptionPtr->description      = optionPtr->description;
	return smiOptionPtr;
    } else {
	return NULL;
    }
}



SmiRefinement *createSmiRefinement(Object *objectPtr,
				   Refinement *refinementPtr)
{
    SmiRefinement *smiRefinementPtr;
    
    if (refinementPtr) {
        smiRefinementPtr = util_malloc(sizeof(SmiRefinement));
	smiRefinementPtr->compliancemodule = objectPtr->modulePtr->name;
	smiRefinementPtr->compliancename   = objectPtr->name;
	smiRefinementPtr->module           = refinementPtr->objectPtr->
							       modulePtr->name;
	smiRefinementPtr->name             = refinementPtr->objectPtr->name;
	if (refinementPtr->typePtr) {
	    smiRefinementPtr->typemodule   = refinementPtr->typePtr->
							       modulePtr->name;
	    smiRefinementPtr->typename     = refinementPtr->typePtr->name;
	} else {
	    smiRefinementPtr->typemodule   = NULL;
	    smiRefinementPtr->typename     = NULL;
	}
	if (refinementPtr->writetypePtr) {
	    smiRefinementPtr->writetypemodule  = refinementPtr->writetypePtr->
							       modulePtr->name;
	    smiRefinementPtr->writetypename    = refinementPtr->writetypePtr->
									  name;
	} else {
	    smiRefinementPtr->writetypemodule  = NULL;
	    smiRefinementPtr->writetypename    = NULL;
	}
	smiRefinementPtr->access           = refinementPtr->access;
	smiRefinementPtr->description      = refinementPtr->description;
	return smiRefinementPtr;
    } else {
	return NULL;
    }
}



/*
 * Interface Functions.
 */

void smiInit()
{
    char *p;
    
    initData();

    p = getenv("SMIPATH");
    if (p) {
	smiPath = util_strdup(p);
    } else {
	smiPath = util_strdup(DEFAULT_SMIPATH);
    }
    
    initialized   = 1;
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



int smiLoadModule(char *module)
{
    Module *modulePtr;
    
    if (!initialized) smiInit();

    if (util_ispath(module)) {

	modulePtr = loadModule(module);

	if (modulePtr) {
	    if (!isInView(modulePtr->name)) {
		addView(modulePtr->name);
	    }
	    return 0;
	} else {
	    return -1;
	}

    } else {
	
	if (!isInView(module)) {
	    addView(module);
	}
	
	if (findModuleByName(module)) {
	    /* already loaded. */
	    return 0;
	} else {
	    if (loadModule(module)) {
		return 0;
	    } else {
		return -1;
	    }
	}
    }
}
 


void smiSetErrorLevel(int level)
{
    if (!initialized) smiInit();
    
    errorLevel = level;
}



void smiSetFlags(int userflags)
{
    if (!initialized) smiInit();
    
    smiFlags = (smiFlags & ~SMI_FLAGMASK) | userflags;
}



int smiGetFlags()
{
    if (!initialized) smiInit();
    
    return smiFlags & SMI_FLAGMASK;
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
    
    return createSmiModule(modulePtr);
}



SmiModule *smiGetFirstModule()
{
    return createSmiModule(firstModulePtr);
}



SmiModule *smiGetNextModule(SmiModule *smiModulePtr)
{
    Module	      *modulePtr;

    if (!smiModulePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiModulePtr->name);

    smiFreeModule(smiModulePtr);
    
    if (!modulePtr) {
	return NULL;
    }
    
    return createSmiModule(modulePtr->nextPtr);
}



void smiFreeModule(SmiModule *smiModulePtr)
{
    util_free(smiModulePtr);
}



SmiImport *smiGetFirstImport(char *module)
{
    Module *modulePtr;
    
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

    return createSmiImport(modulePtr->firstImportPtr);
}



SmiImport *smiGetNextImport(SmiImport *smiImportPtr)
{
    Module	      *modulePtr;
    Import	      *importPtr;
    char	      *importmodule, *importname;

    if (!smiImportPtr) {
	return NULL;
    }

    modulePtr    = findModuleByName(smiImportPtr->module);
    if (!modulePtr) {
	modulePtr = loadModule(smiImportPtr->module);
    }

    importmodule = smiImportPtr->importmodule;
    importname   = smiImportPtr->importname;
        
    smiFreeImport(smiImportPtr);
    
    if (!modulePtr) {
	return NULL;
    }
    
    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if (!strcmp(importPtr->importname, importname) &&
	    !strcmp(importPtr->importmodule, importmodule)) {
	    return createSmiImport(importPtr->nextPtr);
	}
    }

    return NULL;
}



void smiFreeImport(SmiImport *smiImportPtr)
{
    util_free(smiImportPtr);
}



int smiIsImported(char *module, char *importmodule, char *importname)
{
    Import	   *importPtr;
    Module	   *modulePtr;
    char	   *importmodule2, *importname2;
    
    if ((!module) || (!importmodule) || (!importname)) {
	return 0;
    }
    
    modulePtr = findModuleByName(module);
    
    if (!modulePtr) {
	modulePtr = loadModule(module);
    }

    if (!modulePtr) {
	return 0;
    }
    
    getModulenameAndName(importmodule, importname,
			 &importmodule2, &importname2);

    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if ((!strcmp(importmodule2, importPtr->importmodule)) &&
	    (!strcmp(importname2, importPtr->importname))) {
	    util_free(importmodule2);
	    util_free(importname2);
	    return 1;
	}
    }

    util_free(importmodule2);
    util_free(importname2);
    return 0;
}



SmiRevision *smiGetFirstRevision(char *module)
{
    Module	        *modulePtr;

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

    return createSmiRevision(modulePtr->firstRevisionPtr);
}



SmiRevision *smiGetNextRevision(SmiRevision *smiRevisionPtr)
{
    Module	        *modulePtr;
    Revision	        *revisionPtr;
    time_t		date;
    
    if (!smiRevisionPtr) {
	return NULL;
    }

    modulePtr    = findModuleByName(smiRevisionPtr->module);
    if (!modulePtr) {
	modulePtr = loadModule(smiRevisionPtr->module);
    }

    date = smiRevisionPtr->date;
        
    smiFreeRevision(smiRevisionPtr);
    
    if (!modulePtr) {
	return NULL;
    }
    
    for (revisionPtr = modulePtr->firstRevisionPtr; revisionPtr;
	 revisionPtr = revisionPtr->nextPtr) {
	if (revisionPtr->date == date) {
	    return createSmiRevision(revisionPtr->nextPtr);
	}
    }

    return NULL;
}



void smiFreeRevision(SmiRevision *smiRevisionPtr)
{
    util_free(smiRevisionPtr);
}



SmiType *smiGetType(char *module, char *type)
{
    Type	    *typePtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *type2;

    if ((!module) && (!type)) {
	return NULL;
    }
    
    getModulenameAndName(module, type, &module2, &type2);

    if (module2) {
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
    return createSmiType(typePtr);
}



SmiType *smiGetFirstType(char *module)
{
    Module *modulePtr;
    Type *typePtr;
	  
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

    for (typePtr = modulePtr->firstTypePtr; typePtr;
	 typePtr = typePtr->nextPtr) {
	/* loop until we found a `real' type */
	if (typePtr && typePtr->name &&
	    isupper((int)typePtr->name[0]) &&
	    (!(typePtr->flags & FLAG_IMPORTED)) &&
	    (typePtr->basetype != SMI_BASETYPE_UNKNOWN) &&
	    (typePtr->basetype != SMI_BASETYPE_SEQUENCE) &&
	    (typePtr->basetype != SMI_BASETYPE_SEQUENCEOF)) {
	    break;
	}
    }
    
    return createSmiType(typePtr);
}



SmiType *smiGetNextType(SmiType *smiTypePtr)
{
    Module	      *modulePtr;
    Type	      *typePtr;
    char	      *type;
    
    if (!smiTypePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiTypePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiTypePtr->module);
    }

    type = smiTypePtr->name;
        
    smiFreeType(smiTypePtr);
    
    if (!modulePtr) {
	return NULL;
    }

    typePtr = findTypeByModuleAndName(modulePtr, type);

    if (!typePtr) {
	return NULL;
    }
    
    for (typePtr = typePtr->nextPtr; typePtr;
	 typePtr = typePtr->nextPtr) {
	/* loop until we found a `real' type */
	if (typePtr && typePtr->name &&
	    isupper((int)typePtr->name[0]) &&
	    (!(typePtr->flags & FLAG_IMPORTED)) &&
	    (typePtr->basetype != SMI_BASETYPE_UNKNOWN) &&
	    (typePtr->basetype != SMI_BASETYPE_SEQUENCE) &&
	    (typePtr->basetype != SMI_BASETYPE_SEQUENCEOF)) {
	    break;
	}
    }
    
    return createSmiType(typePtr);
}



void smiFreeType(SmiType *smiTypePtr)
{
    if (smiTypePtr) {
	util_free(smiTypePtr->parentmodule);
	util_free(smiTypePtr->parentname);
    }
    util_free(smiTypePtr);
}



SmiNamedNumber *smiGetFirstNamedNumber(char *module, char *type)
{
    Module  *modulePtr;
    Type    *typePtr;
    char    *module2, *type2;

    if ((!module) && (!type)) {
	return NULL;
    }
    
    getModulenameAndName(module, type, &module2, &type2);

    if (module2) {
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

    if ((!typePtr) || (!typePtr->listPtr) ||
	((typePtr->basetype != SMI_BASETYPE_ENUM) &&
	 (typePtr->basetype != SMI_BASETYPE_BITS))) {
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
    
    typePtr = findTypeByModulenameAndName(smiNamedNumberPtr->module,
					  smiNamedNumberPtr->type);

    if ((!typePtr) || (!typePtr->listPtr) ||
	((typePtr->basetype != SMI_BASETYPE_ENUM) &&
	 (typePtr->basetype != SMI_BASETYPE_BITS))) {
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



SmiRange *smiGetFirstRange(char *module, char *type)
{
    Module  *modulePtr;
    Type    *typePtr;
    char    *module2, *type2;

    if ((!module) && (!type)) {
	return NULL;
    }
    
    getModulenameAndName(module, type, &module2, &type2);

    if (module2) {
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

    if ((!typePtr) || (!typePtr->listPtr) ||
	(typePtr->basetype == SMI_BASETYPE_ENUM) ||
	(typePtr->basetype == SMI_BASETYPE_BITS)) {
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
    
    typePtr = findTypeByModulenameAndName(smiRangePtr->module,
					  smiRangePtr->type);

    if ((!typePtr) || (!typePtr->listPtr) ||
	(typePtr->basetype == SMI_BASETYPE_ENUM) ||
	(typePtr->basetype == SMI_BASETYPE_BITS)) {
	return NULL;
    }

    for (listPtr = typePtr->listPtr; listPtr; listPtr = listPtr->nextPtr) {
	if (((Range *)(listPtr->ptr))->minValuePtr == smiRangePtr->minValuePtr)
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



SmiMacro *smiGetMacro(char *module, char *macro)
{
    Macro	    *macroPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *macro2;
    
    if ((!module) && (!macro)) {
	return NULL;
    }
    
    getModulenameAndName(module, macro, &module2, &macro2);

    if (module2) {
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
    return createSmiMacro(macroPtr);
}



SmiMacro *smiGetFirstMacro(char *module)
{
    Module *modulePtr;

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

    return createSmiMacro(modulePtr->firstMacroPtr);
}



SmiMacro *smiGetNextMacro(SmiMacro *smiMacroPtr)
{
    Module	      *modulePtr;
    Macro	      *macroPtr;
    char	      *macro;
    
    if (!smiMacroPtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiMacroPtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiMacroPtr->module);
    }

    macro = smiMacroPtr->name;
        
    smiFreeMacro(smiMacroPtr);
    
    if (!modulePtr) {
	return NULL;
    }

    macroPtr = findMacroByModuleAndName(modulePtr, macro);

    if (!macroPtr) {
	return NULL;
    }
    
    return createSmiMacro(macroPtr->nextPtr);
}



void
smiFreeMacro(smiMacroPtr)
    SmiMacro     *smiMacroPtr;
{
    util_free(smiMacroPtr);
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
    }

    if (!modulePtr) {
	util_free(module2);
	util_free(node2);
	return NULL;
    }
    
    objectPtr = getObject(module2, node2);
    
    util_free(module2);
    util_free(node2);
    return createSmiNode(objectPtr);
}



SmiNode *smiGetFirstNode(char *module, SmiDecl decl)
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
    
    objectPtr = getNextChildObject(nodePtr, modulePtr, decl);

    if ((!objectPtr) && nodePtr) {
	goto again;
    }
    
    return createSmiNode(objectPtr);
}



SmiNode *smiGetNextNode(SmiNode *smiNodePtr, SmiDecl decl)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    Node	      *nodePtr;
    char	      *node;
    
    if (!smiNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNodePtr->module);
    }

    node = smiNodePtr->name;
        
    smiFreeNode(smiNodePtr);
    
    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, node);

    if (!objectPtr) {
	return NULL;
    }

    nodePtr = objectPtr->nodePtr;
    
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
    
    objectPtr = getNextChildObject(nodePtr, modulePtr, decl);

    if ((!objectPtr) && nodePtr) {
	goto again;
    }
    
    return createSmiNode(objectPtr);
}



SmiNode *smiGetFirstIndexNode(SmiNode *smiRowNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    
    if (!smiRowNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiRowNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiRowNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, smiRowNodePtr->name);

    if (!objectPtr) {
	return NULL;
    }

    if ((!objectPtr->indexPtr) || (!objectPtr->indexPtr->listPtr)) {
	return NULL;
    }

    return createSmiNode(objectPtr->indexPtr->listPtr->ptr);
}



SmiNode *smiGetNextIndexNode(SmiNode *smiRowNodePtr, SmiNode *smiIndexNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node;
    
    if ((!smiRowNodePtr) || (!smiIndexNodePtr)) {
	return NULL;
    }

    modulePtr = findModuleByName(smiRowNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiRowNodePtr->module);
    }

    module = smiIndexNodePtr->module;
    node = smiIndexNodePtr->name;
        
    smiFreeNode(smiIndexNodePtr);
    
    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, smiRowNodePtr->name);

    if (!objectPtr) {
	return NULL;
    }

    if ((!objectPtr->indexPtr) || (!objectPtr->indexPtr->listPtr)) {
	return NULL;
    }

    for (listPtr = objectPtr->indexPtr->listPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Object *)(listPtr->ptr))->modulePtr->name, module) &&
	    !strcmp(((Object *)(listPtr->ptr))->name, node)) {
	    if (listPtr->nextPtr) {
		return createSmiNode(listPtr->nextPtr->ptr);
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



SmiNode *smiGetFirstMemberNode(SmiNode *smiGroupNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    
    if (!smiGroupNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiGroupNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiGroupNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, smiGroupNodePtr->name);

    if ((!objectPtr) || (!objectPtr->listPtr)) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_OBJECTGROUP) &&
	(objectPtr->decl != SMI_DECL_NOTIFICATIONGROUP) &&
	(objectPtr->decl != SMI_DECL_GROUP)) {
	return NULL;
    }
						     
    return createSmiNode(objectPtr->listPtr->ptr);
}



SmiNode *smiGetNextMemberNode(SmiNode *smiGroupNodePtr,
			      SmiNode *smiMemberNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node;
    
    if ((!smiGroupNodePtr) || (!smiMemberNodePtr)) {
	return NULL;
    }

    modulePtr = findModuleByName(smiGroupNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiGroupNodePtr->module);
    }

    module = smiMemberNodePtr->module;
    node = smiMemberNodePtr->name;
        
    smiFreeNode(smiMemberNodePtr);

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr, smiGroupNodePtr->name);

    if (!objectPtr) {
	return NULL;
    }

    if (!objectPtr->listPtr) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_OBJECTGROUP) &&
	(objectPtr->decl != SMI_DECL_NOTIFICATIONGROUP) &&
	(objectPtr->decl != SMI_DECL_GROUP)) {
	return NULL;
    }

    for (listPtr = objectPtr->listPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Object *)(listPtr->ptr))->modulePtr->name, module) &&
	    !strcmp(((Object *)(listPtr->ptr))->name, node)) {
	    if (listPtr->nextPtr) {
		return createSmiNode(listPtr->nextPtr->ptr);
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



SmiNode *smiGetFirstObjectNode(SmiNode *smiNotificationNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    
    if (!smiNotificationNodePtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiNotificationNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNotificationNodePtr->module);
    }

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiNotificationNodePtr->name);

    if ((!objectPtr) || (!objectPtr->listPtr)) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_NOTIFICATION) &&
	(objectPtr->decl != SMI_DECL_NOTIFICATIONTYPE)) {
	return NULL;
    }
						     
    return createSmiNode(objectPtr->listPtr->ptr);
}



SmiNode *smiGetNextObjectNode(SmiNode *smiNotificationNodePtr,
			      SmiNode *smiObjectNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node;
    
    if ((!smiNotificationNodePtr) || (!smiObjectNodePtr)) {
	return NULL;
    }

    modulePtr = findModuleByName(smiNotificationNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiNotificationNodePtr->module);
    }

    module = smiObjectNodePtr->module;
    node = smiObjectNodePtr->name;
        
    smiFreeNode(smiObjectNodePtr);

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiNotificationNodePtr->name);

    if (!objectPtr) {
	return NULL;
    }

    if (!objectPtr->listPtr) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_NOTIFICATION) &&
	(objectPtr->decl != SMI_DECL_NOTIFICATIONTYPE)) {
	return NULL;
    }

    for (listPtr = objectPtr->listPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Object *)(listPtr->ptr))->modulePtr->name, module) &&
	    !strcmp(((Object *)(listPtr->ptr))->name, node)) {
	    if (listPtr->nextPtr) {
		return createSmiNode(listPtr->nextPtr->ptr);
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



SmiNode *smiGetFirstMandatoryNode(SmiNode *smiComplianceNodePtr)
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

    if ((!objectPtr) || (!objectPtr->listPtr)) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_COMPLIANCE) &&
	(objectPtr->decl != SMI_DECL_MODULECOMPLIANCE)) {
	return NULL;
    }
						     
    return createSmiNode(objectPtr->listPtr->ptr);
}



SmiNode *smiGetNextMandatoryNode(SmiNode *smiComplianceNodePtr,
				 SmiNode *smiMandatoryNodePtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node;
    
    if ((!smiComplianceNodePtr) || (!smiMandatoryNodePtr)) {
	return NULL;
    }

    modulePtr = findModuleByName(smiComplianceNodePtr->module);

    if (!modulePtr) {
	modulePtr = loadModule(smiComplianceNodePtr->module);
    }

    module = smiMandatoryNodePtr->module;
    node = smiMandatoryNodePtr->name;
        
    smiFreeNode(smiMandatoryNodePtr);

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiComplianceNodePtr->name);

    if (!objectPtr) {
	return NULL;
    }

    if (!objectPtr->listPtr) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_COMPLIANCE) &&
	(objectPtr->decl != SMI_DECL_MODULECOMPLIANCE)) {
	return NULL;
    }
						     
    for (listPtr = objectPtr->listPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Object *)(listPtr->ptr))->modulePtr->name, module) &&
	    !strcmp(((Object *)(listPtr->ptr))->name, node)) {
	    if (listPtr->nextPtr) {
		return createSmiNode(listPtr->nextPtr->ptr);
	    } else {
		return NULL;
	    }
	}
    }
    
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
	    objectPtr = findObjectByModulenameAndNode(importPtr->importmodule,
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
    }

    return createSmiNode(objectPtr);
}



SmiNode *smiGetFirstChildNode(SmiNode *smiNodePtr)
{
    /* TODO */
    return NULL;
}



SmiNode *smiGetNextChildNode(SmiNode *smiNodePtr)
{
    /* TODO */
    return NULL;
}



void
smiFreeNode(smiNodePtr)
    SmiNode     *smiNodePtr;
{
    util_free(smiNodePtr);
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

    if ((objectPtr->decl != SMI_DECL_COMPLIANCE) &&
	(objectPtr->decl != SMI_DECL_MODULECOMPLIANCE)) {
	return NULL;
    }
						     
    return createSmiOption(objectPtr, objectPtr->optionlistPtr->ptr);
}



SmiOption *smiGetNextOption(SmiOption *smiOptionPtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node;
    
    if (!smiOptionPtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiOptionPtr->compliancemodule);

    if (!modulePtr) {
	modulePtr = loadModule(smiOptionPtr->compliancemodule);
    }

    module = smiOptionPtr->module;
    node = smiOptionPtr->name;
        
    smiFreeOption(smiOptionPtr);

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiOptionPtr->compliancename);

    if (!objectPtr) {
	return NULL;
    }

    if (!objectPtr->optionlistPtr) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_COMPLIANCE) &&
	(objectPtr->decl != SMI_DECL_MODULECOMPLIANCE)) {
	return NULL;
    }
						     
    for (listPtr = objectPtr->optionlistPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Option *)(listPtr->ptr))->objectPtr->modulePtr->name,
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



void
smiFreeOption(smiOptionPtr)
    SmiOption *smiOptionPtr;
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

    if ((objectPtr->decl != SMI_DECL_COMPLIANCE) &&
	(objectPtr->decl != SMI_DECL_MODULECOMPLIANCE)) {
	return NULL;
    }
						     
    return createSmiRefinement(objectPtr, objectPtr->refinementlistPtr->ptr);
}



SmiRefinement *smiGetNextRefinement(SmiRefinement *smiRefinementPtr)
{
    Module	      *modulePtr;
    Object	      *objectPtr;
    List	      *listPtr;
    SmiIdentifier     module, node;
    
    if (!smiRefinementPtr) {
	return NULL;
    }

    modulePtr = findModuleByName(smiRefinementPtr->compliancemodule);

    if (!modulePtr) {
	modulePtr = loadModule(smiRefinementPtr->compliancemodule);
    }

    module = smiRefinementPtr->module;
    node = smiRefinementPtr->name;
        
    smiFreeRefinement(smiRefinementPtr);

    if (!modulePtr) {
	return NULL;
    }

    objectPtr = findObjectByModuleAndName(modulePtr,
					  smiRefinementPtr->compliancename);

    if (!objectPtr) {
	return NULL;
    }

    if (!objectPtr->refinementlistPtr) {
	return NULL;
    }

    if ((objectPtr->decl != SMI_DECL_COMPLIANCE) &&
	(objectPtr->decl != SMI_DECL_MODULECOMPLIANCE)) {
	return NULL;
    }
						     
    for (listPtr = objectPtr->refinementlistPtr; listPtr;
	 listPtr = listPtr->nextPtr) {
	if ((listPtr->ptr) &&
	    !strcmp(((Refinement *)(listPtr->ptr))->objectPtr->modulePtr->name,
		    module) &&
	    !strcmp(((Refinement *)(listPtr->ptr))->objectPtr->name, node)) {
	    if (listPtr->nextPtr) {
		return createSmiRefinement(objectPtr, listPtr->nextPtr->ptr);
	    } else {
		return NULL;
	    }
	}
    }
    
    return NULL;
}



void
smiFreeRefinement(smiRefinementPtr)
    SmiRefinement *smiRefinementPtr;
{
    util_free(smiRefinementPtr);
}



