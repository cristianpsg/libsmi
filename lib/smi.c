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
 * @(#) $Id: smi.c,v 1.24 1999/05/20 08:51:16 strauss Exp $
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



#if 0
/*
 * add an entry to a malloc'ed array of char pointers. the new entry `name'
 * gets strdup'ed. the list keeps NULL terminated. the list pointer
 * (like argv) must be passed by reference, cos it must be realloc'ed.
 * e.g. addName(&argv, "gaga");
 */
void addName(char **list[], char *module, char *name)
{
  char **new;
  int i = 0;

  if (!module || !strlen(module) || !name || !strlen(name)) {
      return;
  }

  if (*list) for(i=0; (*list)[i]; i++);
  new = util_realloc((*list), sizeof(char *) * (i+2));
  new[i] = util_malloc(sizeof(char) *
		       (strlen(module) +
			strlen(SMI_NAMESPACE_OPERATOR) +
			strlen(name) + 1));
  sprintf(new[i], "%s%s%s", module, SMI_NAMESPACE_OPERATOR, name);
  new[i+1] = NULL;
  *list = new;
}



void addPtr(void *list, void *ptr)
        /* TODO **list[] */
{
  void **new;
  int i = 0;

  if (*list) for(i=0; (*list)[i]; i++);
  new = util_realloc((*list), sizeof(void *) * (i+2));
  new[i] = ptr;
  new[i+1] = NULL;
  *list = new;
}
#endif



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
	    if ((p = strstr(arg1, SMI_NAMESPACE_OPERATOR))) {
		/* SMIng style module/label separator */
		*name = util_strdup(&p[strlen(SMI_NAMESPACE_OPERATOR)]);
		l = strcspn(arg1, SMI_NAMESPACE_OPERATOR);
		*module = util_strndup(arg1, l);
	    } else if ((p = strchr(arg1, '!'))) {
		/* old scotty style module/label separator */
		name = util_strdup(&p[1]);
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
	    if ((p = strstr(arg2, SMI_NAMESPACE_OPERATOR))) {
		/* SMIng style module/label separator */
		*name = util_strdup(&p[strlen(SMI_NAMESPACE_OPERATOR)]);
		l = strcspn(arg2, SMI_NAMESPACE_OPERATOR);
		*module = util_strndup(arg2, l);
	    } else if ((p = strchr(arg2, '!'))) {
		/* old scotty style module/label separator */
		name = util_strdup(&p[1]);
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



Object *getObject(char *name, char *modulename)
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
	    if (strlen(modulename)) {
		objectPtr = findObjectByModulenameAndNode(modulename, nodePtr);
	    } else {
		objectPtr = findObjectByNode(nodePtr);
	    }
	}
    } else {
	if (strlen(modulename)) {
	    objectPtr = findObjectByModulenameAndName(modulename, name);
	} else {
	    objectPtr = findObjectByName(name);
	}
    }
    return objectPtr;
}



Object *getNextChildObject(Node *startNodePtr, Module *modulePtr)
{
    Node   *nodePtr;
    Object *objectPtr = NULL;

    if (!startNodePtr || !modulePtr)
	return NULL;

    for (nodePtr = startNodePtr; nodePtr; nodePtr = nodePtr->nextPtr) {
	for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextSameNodePtr) {
	    if ((objectPtr->modulePtr == modulePtr) &&
		(!(objectPtr->flags & FLAG_IMPORTED))) {
		break;
	    }
	}
	if (objectPtr) break;
	objectPtr = getNextChildObject(nodePtr->firstChildPtr, modulePtr);
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
	smiImportPtr->importmodule = importPtr->module;
	smiImportPtr->importname   = importPtr->name;
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
	smiRevisionPtr->date = modulePtr->firstRevisionPtr->date;
	if (modulePtr->firstRevisionPtr->description) {
	    smiRevisionPtr->description =
		modulePtr->firstRevisionPtr->description;
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
    SmiOption	  *smiOption;
    SmiRefinement *smiRefinement;
    char          typename[SMI_MAX_FULLNAME+1];
    List          *listPtr;
    
    if (objectPtr) {
	smiNodePtr     = util_malloc(sizeof(SmiNode));
	
	smiNodePtr->name   = objectPtr->name;
	smiNodePtr->module = objectPtr->modulePtr->name;
	smiNodePtr->oid    = getOid(objectPtr->nodePtr);
	if (objectPtr->typePtr) {
	    if (objectPtr->typePtr->name) {
		if (objectPtr->typePtr->modulePtr) {
		    sprintf(typename, "%s%s%s",
			    objectPtr->typePtr->modulePtr->name,
			    SMI_NAMESPACE_OPERATOR,
			    objectPtr->typePtr->name);
		} else {
		    sprintf(typename, "%s",
			    objectPtr->typePtr->name);
		}
		smiNodePtr->type   = util_strdup(typename);
	    } else {
		/*
		 * This is an inlined type restriction. It is accessible
		 * by the same (lowercase!) name as the object.
		 */
		sprintf(typename, "%s%s%s",
			objectPtr->modulePtr->name,
			SMI_NAMESPACE_OPERATOR,
			objectPtr->name);
		smiNodePtr->type   = util_strdup(typename);
	    }
	} else {
	    smiNodePtr->type   = NULL;
	}
	smiNodePtr->value      = objectPtr->valuePtr;
	smiNodePtr->decl       = objectPtr->decl;
	smiNodePtr->access     = objectPtr->access;
	smiNodePtr->status     = objectPtr->status;
	smiNodePtr->basetype     = objectPtr->typePtr ?
	                     objectPtr->typePtr->basetype : SMI_BASETYPE_UNKNOWN;
	if (objectPtr->description) {
	    smiNodePtr->description = objectPtr->description;
	} else {
	    smiNodePtr->description = NULL;
	}
	if (objectPtr->reference) {
	    smiNodePtr->reference = objectPtr->reference;
	} else {
	    smiNodePtr->reference = NULL;
	}

	smiNodePtr->indexkind  = SMI_INDEX_UNKNOWN;
	smiNodePtr->implied    = 0;
	smiNodePtr->index      = NULL;
	smiNodePtr->relatedrow = NULL;
	if (objectPtr->indexPtr) {
	    smiNodePtr->indexkind  = objectPtr->indexPtr->indexkind;
	    smiNodePtr->implied    = objectPtr->indexPtr->implied;
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
		if (objectPtr->indexPtr->rowPtr) {
		    smiNodePtr->relatedrow =
			(char *)objectPtr->indexPtr->rowPtr;
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
		if (objectPtr->indexPtr->rowPtr) {
		    smiNodePtr->relatedrow = objectPtr->indexPtr->rowPtr->name;
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

	smiNodePtr->option	= NULL;
	for (listPtr = objectPtr->optionlistPtr; listPtr;
	     listPtr = listPtr->nextPtr) {
	    smiOption = util_malloc(sizeof(SmiOption));
	    smiOption->name        = ((Option *)listPtr->ptr)->objectPtr->name;
	    smiOption->module      =
		          ((Option *)listPtr->ptr)->objectPtr->modulePtr->name;
	    smiOption->description = ((Option *)listPtr->ptr)->description;
	    addPtr(&smiNodePtr->option, smiOption);
	}

	smiNodePtr->refinement  = NULL;
	for (listPtr = objectPtr->refinementlistPtr; listPtr;
	     listPtr = listPtr->nextPtr) {
	    smiRefinement = util_malloc(sizeof(SmiRefinement));
	    smiRefinement->name    =
		                 ((Refinement *)listPtr->ptr)->objectPtr->name;
	    smiRefinement->module  =
		      ((Refinement *)listPtr->ptr)->objectPtr->modulePtr->name;
	    smiRefinement->type    =
		            ((Refinement *)listPtr->ptr)->typePtr ?
		            ((Refinement *)listPtr->ptr)->typePtr->name : NULL;
	    smiRefinement->writetype =
		       ((Refinement *)listPtr->ptr)->writetypePtr ?
		       ((Refinement *)listPtr->ptr)->writetypePtr->name : NULL;
	    smiRefinement->access  = ((Refinement *)listPtr->ptr)->access;
	    smiRefinement->description =
		                     ((Refinement *)listPtr->ptr)->description;
	    addPtr(&smiNodePtr->refinement, smiRefinement);
	}
	return smiNodePtr;
    } else {
	return NULL;
    }
}



SmiModule *createSmiType(Type *typePtr)
{
    SmiType *smiTypePtr;
    
    if (typePtr) {
	smiTypePtr = util_malloc(sizeof(SmiType));
	smiTypePtr->name        = typePtr->name;
	smiTypePtr->module      = typePtr->modulePtr->name;
	smiTypePtr->basetype	= typePtr->basetype;
	smiTypePtr->parent	= typePtr->parentType;
        smiTypePtr->list	= NULL;
	    for (listPtr = typePtr->listPtr; listPtr;
		 listPtr = listPtr->nextPtr) {
		addPtr(&smiTypePtr->list, listPtr->ptr);
	    }
	smiTypePtr->decl	= typePtr->decl;
	smiTypePtr->format	= typePtr->format;
	smiTypePtr->value	= typePtr->valuePtr;
	smiTypePtr->units	= typePtr->units;
	smiTypePtr->status	= typePtr->status;
	smiTypePtr->description = typePtr->description;
	smiTypePtr->reference   = typePtr->reference;
	return smiTypePtr;
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
	smiPath = SMI_PATH;
    }
    
    initialized   = 1;
}



int smiLoadModule(char modulename)
{

    if (!initialized) smiInit();

    if (!isInView(modulename)) {
	addView(modulename);
    }

    if (findModuleByName(modulename)) {
	/* already loaded. */
	return 0;
    }

    if (loadModule(modulename)) {
	return 0;
    } else {
	return -1;
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
    if (smiModulePtr) {
	free(smiModulePtr);
    }
}



SmiImport *smiGetFirstImport(char *module)
{
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
    SmiImport	      *smiImportPtr;
    Module	      *modulePtr;
    Import	      *importPtr;
    char	      *importname;
    char	      *importmodule;

    if (!smiImportPtr) {
	return NULL;
    }

    modulePtr    = findModuleByName(smiImportPtr->module);
    if (!modulePtr) {
	modulePtr = loadModule(module);
    }

    importmodule = smiImport->importmodule;
    importname   = smiImport->importname;
        
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
    if (smiImportPtr) {
	free(smiImportPtr);
    }
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
    SmiRevision		*smiRevisionPtr;
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
    SmiRevision		*smiRevisionPtr;
    Module	        *modulePtr;
    Revision	        *revisionPtr;
    time_t		date;
    
    if (!smiRevisionPtr) {
	return NULL;
    }

    modulePtr    = findModuleByName(smiRevisionPtr->module);
    if (!modulePtr) {
	modulePtr = loadModule(module);
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
    if (smiRevisionPtr) {
	free(smiRevisionPtr);
    }
}



SmiType *smiGetType(char *module, char *type)
{
    Type	    *typePtr = NULL;
    Module	    *modulePtr = NULL;
    char	    *module2, *type2;

    if ((!module) || (!type)) {
	return NULL;
    }
    
    getModulenameAndName(module, type, &module2, &type2);

    if (module2) {
	if (!findModuleByName(module2)) {
	    loadModule(module2);
	}
    }

    typePtr = findTypeByModulenameAndName(module2, type2);
    
    util_free(module2);
    util_free(type2);
    return createSmiType(typePtr);
}



SmiType *smiGetFirstType(char *module)
{
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

    modulePtr    = findModuleByName(smiTypePtr->module);
    if (!modulePtr) {
	modulePtr = loadModule(module);
    }

    type = smiTypePtr->name;
        
    smiFreeType(smiTypePtr);
    
    if (!modulePtr) {
	return NULL;
    }
XXX    
    for (revisionPtr = modulePtr->firstRevisionPtr; revisionPtr;
	 revisionPtr = revisionPtr->nextPtr) {
	if (revisionPtr->date == date) {
	    return createSmiRevision(revisionPtr->nextPtr);
	}
    }

    return NULL;






    modulePtr = findModuleByName(modulename);
    
    if (!modulePtr) {
	modulePtr = loadModule(modulename);
    }
    
    if (modulePtr && (modulePtr->firstTypePtr)) {
	for (hit = 0, typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    /*
	     * If the name argument is an empty string, we will
	     * return the first type.
	     */
	    if (!name) {
		hit = 1;
	    }
	    /*
	     * If we have found the type named name, we will return
	     * the next one.
	     */
	    if (typePtr->name && name && !strcmp(typePtr->name, name)) {
		hit = 1;
		typePtr = typePtr->nextPtr;
		if (!typePtr) break;
	    }
	    /*
	     * If hit, continue loop, until we have found a real type.
	     */
	    if (hit && typePtr && typePtr->name &&
		isupper((int)typePtr->name[0]) &&
		(!(typePtr->flags & FLAG_IMPORTED)) &&
		(typePtr->basetype != SMI_BASETYPE_UNKNOWN) &&
		(typePtr->basetype != SMI_BASETYPE_SEQUENCE) &&
		(typePtr->basetype != SMI_BASETYPE_SEQUENCEOF)) {
		break;
	    }
	}
	     
	if (typePtr) {
	    return createSmiType(typePtr);
	}
    }

    return NULL;
}



void
smiFreeType(smiTypePtr)
    SmiType     *smiTypePtr;
{
    free(smiTypePtr);
}



SmiMacro *
smiGetMacro(module, macro)
    char            *spec;
    char	    *mod;
{
    Macro	    *macroPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    name[SMI_MAX_OID+1];
    char	    modulename[SMI_MAX_DESCRIPTOR+1];
    
    getModulenameAndName(module, macro, &modulename, &name);

    if (modulename) {
	modulePtr = findModuleByName(modulename);
	if (!modulePtr) {
	    modulePtr = loadModule(modulename);
	}
    }

    macroPtr = findMacroByModuleAndName(modulePtr, name);
    
    if (macroPtr) {
	return createSmiMacro(macroPtr);
    } else {
	return NULL;
    }
}



void
smiFreeMacro(smiMacroPtr)
    SmiMacro     *smiMacroPtr;
{
    free(smiMacroPtr);
}



SmiNode *
smiGetNode(spec, mod)
    char	    *spec;
    char	    *mod;
{
    Object          *objectPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    name[SMI_MAX_OID+1];
    char	    modulename[SMI_MAX_DESCRIPTOR+1];
    
    getModulenameAndName(mod, spec, &modulename, &name);

    if (modulename) {
	modulePtr = findModuleByName(modulename);
	if (!modulePtr) {
	    modulePtr = loadModule(modulename);
	}
    }

    objectPtr = getObject(name, modulename);

    return createSmiNode(objectPtr);
}



SmiNode *
smiGetFirstNode(modulename)
    char	*modulename;
{
    return smiGetNextNode(modulename, NULL);
}



SmiNode *
smiGetNextNode(modulename, name)
    char	      *modulename;
    char	      *name;
{
    Module	      *modulePtr;
    Node	      *nodePtr = NULL;
    Object	      *objectPtr;
    
    modulePtr = findModuleByName(modulename);
    
    if (!modulePtr) {
	modulePtr = loadModule(modulename);
    }

    /*
     * If there's a `name', find the node to start from.
     * Otherwise, start from the root.
     */
    if (name) {
	objectPtr = findObjectByModulenameAndName(modulename, name);
	if (objectPtr) {
	    nodePtr = objectPtr->nodePtr;
	}
    } else {
	nodePtr = rootNodePtr;
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
    
    objectPtr = getNextChildObject(nodePtr, modulePtr);

    if ((!objectPtr) && nodePtr) {
	goto again;
    }
    
    return createSmiNode(objectPtr);
}



void
smiFreeNode(smiNodePtr)
    SmiNode         *smiNodePtr;
{
    free(smiNodePtr);
    /* TODO free sub-structs */
}



char **
smiGetNames(spec, mod)
    char	        *spec;
    char		*mod;
{
    Module		*modulePtr = NULL;
    Object		*objectPtr;
    Node		*nodePtr = NULL;
    Type		*typePtr = NULL;
    char	        name[SMI_MAX_OID+1];
    char	        modulename[SMI_MAX_DESCRIPTOR+1];
    char	        *elements, *element;
    SmiSubid	        subid;
    char		**list = NULL;
    
    getModulenameAndName(mod, spec, &modulename, &name);

    if (modulename) {
	modulePtr = findModuleByName(modulename);
	if (!modulePtr) {
	    modulePtr = loadModule(modulename);
	}
    }

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
	    if (strlen(modulename)) {
		objectPtr = findObjectByModulenameAndNode(modulename, nodePtr);
		addName(&list, objectPtr->modulePtr->name, objectPtr->name);
	    } else {
		for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
		     objectPtr = objectPtr->nextPtr) {
		    if (objectPtr->modulePtr &&
			isInView(objectPtr->modulePtr->name)) {
			addName(&list, objectPtr->modulePtr->name,
				objectPtr->name);
		    }
		}
	    }
	}
    } else {
	if (islower((int)name[0])) {
	    if (strlen(modulename)) {
		objectPtr = findObjectByModulenameAndName(modulename, name);
		if (objectPtr) {
		    addName(&list, objectPtr->modulePtr->name,
			    objectPtr->name);
		}
	    } else {
		for (objectPtr = findObjectByName(name); objectPtr;
		     objectPtr = findNextObjectByName(name, objectPtr)) {
		    if (objectPtr->modulePtr &&
			isInView(objectPtr->modulePtr->name)) {
			addName(&list, objectPtr->modulePtr->name,
				objectPtr->name);
		    }
		}
	    }
	} else {
	    if (strlen(modulename)) {
		typePtr = findTypeByModulenameAndName(modulename, name);
		if (typePtr) {
		    addName(&list, typePtr->modulePtr->name, typePtr->name);
		}
	    } else {
		for (typePtr = findTypeByName(name); typePtr;
		     typePtr = findNextTypeByName(name, typePtr)) {
		    if (typePtr->modulePtr &&
			isInView(typePtr->modulePtr->name)) {
			addName(&list, typePtr->modulePtr->name,
				typePtr->name);
		    }
		}
	    }
	}
    }

    return list;
}




char *
smiGetParent(spec, mod)
    char	        *spec;
    char		*mod;
{
    Object		*objectPtr = NULL;
    Node		*nodePtr;
    Module		*modulePtr;
    View		*viewPtr;
    Import		*importPtr;
    char		name[SMI_MAX_OID+1];
    char		modulename[SMI_MAX_DESCRIPTOR+1];
    char		s[SMI_MAX_FULLNAME+1];
    char		*modulenamePtr;
    char		child[SMI_MAX_FULLNAME+1];
    char		*p;
    char		*parent;
    
    parent = util_malloc(sizeof(char) * (SMI_MAX_FULLNAME+1));
	    
    getModulenameAndName(mod, spec, &modulename, &name);

    if (modulename) {
	if (!findModuleByName(modulename)) {
	    loadModule(modulename);
	}
    }

    objectPtr = getObject(name, modulename);
    if (!objectPtr) objectPtr = getObject(name, "");

    if (objectPtr && (objectPtr->nodePtr->parentPtr)) {
	modulePtr = objectPtr->modulePtr;
	nodePtr = objectPtr->nodePtr->parentPtr;

	/*
	 * Try to find the parent object in the same module.
	 * This might be an imported descriptor, marked by
	 * objectPtr->flags & FLAG_IMPORTED.
	 */
	for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextSameNodePtr) {
	    if (objectPtr->modulePtr == modulePtr) {
		break;
	    }
	}

	/*
	 * If it's not found in the current module, try to find it in
	 * a module imported by the current module.
	 */
	if (!objectPtr) {
	    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
		 objectPtr = objectPtr->nextSameNodePtr) {
		for (importPtr = modulePtr->firstImportPtr;
		     importPtr; importPtr = importPtr->nextPtr) {
		    if (!strcmp(objectPtr->modulePtr->name,
				importPtr->module)) {
			break;
		    }
		}
		if (importPtr) {
		    break;
		}
	    }
	}

	/*
	 * If it's not yet found, try to find it in
	 * any module in the current view.
	 */
	if (!objectPtr) {
	    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
		 objectPtr = objectPtr->nextSameNodePtr) {
		for (viewPtr = firstViewPtr; viewPtr;
		     viewPtr = viewPtr->nextPtr) {
		    if ((!strcmp(objectPtr->modulePtr->name, viewPtr->name)) &&
			(!(objectPtr->flags & FLAG_IMPORTED))) {
			break;
		    }
		}
		if (viewPtr) {
		    break;
		}
	    }
	}

	/*
	 * Now, if we have found the parent object, create the appropriate
	 * OID string. Otherwise, call getParent() recursively.
	 */
	if (objectPtr) {
	    sprintf(s, "%s%s%s", objectPtr->modulePtr->name,
		    SMI_NAMESPACE_OPERATOR, objectPtr->name);
	    if (smiIsImported(modulename, s, NULL)) {
		strcpy(parent, s);
	    } else if (!(objectPtr->flags & FLAG_INCOMPLETE)) {
		if (!strlen(objectPtr->modulePtr->name)) {
		    sprintf(parent, "%s", objectPtr->name);
		} else {
		    if (objectPtr->flags & FLAG_IMPORTED) {
			for(importPtr = objectPtr->modulePtr->firstImportPtr;
			    importPtr; importPtr = importPtr->nextPtr) {
			    if (!strcmp(importPtr->name, objectPtr->name)) {
				break;
			    }
			}
			modulenamePtr = importPtr->module;
		    } else {
			modulenamePtr = objectPtr->modulePtr->name;
		    }
		    sprintf(parent, "%s%s%s", modulenamePtr,
			    SMI_NAMESPACE_OPERATOR, objectPtr->name);
		}
	    } else {
		strncpy(child, getOid(nodePtr), SMI_MAX_FULLNAME);
		p = smiGetParent(child, modulename);
		if (p) {
		    strcpy(parent, p);
		} else {
		    parent[0] = 0;
		}
		sprintf(&parent[strlen(parent)], ".%d", nodePtr->subid);
	    }
	} else {
	    /*
	     * call getParent() recursively to get the most verbose
	     * name that is in the current view.
	     */
	    strncpy(child, getOid(nodePtr), SMI_MAX_FULLNAME);
	    p = smiGetParent(child, "");
	    if (p) strcpy(parent, p);
	    sprintf(&parent[strlen(parent)], ".%d", nodePtr->subid);
	}

    } else {
	p = strrchr(name, '.');
	if (p && isdigit((int)p[1])) {
	    sprintf(s, "%s%s%s", modulename, SMI_NAMESPACE_OPERATOR,
		    name);
	    p = strrchr(s, '.');
	    p[0] = 0;
	    strncpy(parent, s, SMI_MAX_FULLNAME);
	} else {
	    free(parent);
	    parent = NULL;
	}
    }

    return parent;
}



/*
 *
 */



/* NOTE: not reentrent. returning a static pointer. */
char *
smiModule(fullname)
    char            *fullname;
{
    static char	    modulename[SMI_MAX_DESCRIPTOR+1];
    int		    len;

    if (!fullname) return NULL;

    if (islower((int)fullname[0]))
	return "";
    
    len = strcspn(fullname, "!.");
    len = MIN(len, strcspn(fullname, SMI_NAMESPACE_OPERATOR));
    len = MIN(len, SMI_MAX_DESCRIPTOR);
    strncpy(modulename, fullname, len);
    modulename[len] = 0;

    return modulename;
}



/* NOTE: not reentrent. returning a static pointer. */
char *
smiDescriptor(fullname)
    char	    *fullname;
{
    static char	    name[SMI_MAX_DESCRIPTOR+1];
    char	    *p;

    if (!fullname) return NULL;
    
    p = strstr(fullname, SMI_NAMESPACE_OPERATOR);
    if (p) {
	strcpy(name, &p[strlen(SMI_NAMESPACE_OPERATOR)]);
    } else {
	p = strchr(fullname, '!');
	if (p) {
	    strcpy(name, &p[1]);
	} else {
	    if (!isupper((int)fullname[0])) {
		strcpy(name, fullname);
	    } else {
		p = strchr(fullname, '.');
		if (p) {
		    strcpy(name, &p[1]);
		} else {
		    strcpy(name, fullname);
		}
	    }
	}
    }
	 
    return name;
}



