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
 * @(#) $Id: smi.c,v 1.14 1999/03/30 22:27:32 strauss Exp $
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

#ifdef BACKEND_SMI
extern int yydebug;
#endif

#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif


static int		initialized = 0;



/*
 * Internal functions.
 */



int
allowsNamedNumbers(basetype)
    SmiBasetype basetype;
{
    if ((basetype == SMI_BASETYPE_INTEGER32) ||
	(basetype == SMI_BASETYPE_BITS)) {
	return 1;
    } else {
	return 0;
    }
}



int
allowsRanges(basetype)
    SmiBasetype basetype;
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



/*
 * add an entry to a malloc'ed array of char pointers. the new entry `name'
 * gets strdup'ed. the list keeps NULL terminated. the list pointer
 * (like argv) must be passed by reference, cos it must be realloc'ed.
 * e.g. addName(&argv, "gaga");
 */
void
addName(list, module, name)
    char      **list[];
    char      *module;
    char      *name;
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



void
addPtr(list, ptr)
    void      **list[];
    void      *ptr;
{
  void **new;
  int i = 0;

  if (*list) for(i=0; (*list)[i]; i++);
  new = util_realloc((*list), sizeof(void *) * (i+2));
  new[i] = ptr;
  new[i+1] = NULL;
  *list = new;
}



char *
getOid(n)
    Node *n;
{
    char o[SMI_MAX_OID+1+1];
    SmiSubid a[128];
    int i, l;

    for (i = 0; n && (n != rootNodePtr); n = n->parentPtr) {
	a[i++] = n->subid;
    }
    for (strcpy(o, n ? "" : "<incomplete>."), l = strlen(o);
	 i > 0;
	 l += sprintf(&o[l], "%d.", a[--i]));
    o[l-1] = 0;

    return util_strdup(o);
}



void
getModulenameAndName(spec, mod, modulename, name)
    char	    *spec;
    char	    *mod;
    char	    *modulename;
    char	    *name;
{
    char	    *p;
    int		    l;
    
    if (isupper((int)spec[0])) {
	if ((p = strstr(spec, SMI_NAMESPACE_OPERATOR))) {
	    /* SMIng style module/label separator */
	    strncpy(name, &p[strlen(SMI_NAMESPACE_OPERATOR)], SMI_MAX_OID);
	    l = strcspn(spec, SMI_NAMESPACE_OPERATOR);
	    strncpy(modulename, spec, l);
	    modulename[l] = 0;
	} else if ((p = strchr(spec, '!'))) {
	    /* old scotty style module/label separator */
	    strncpy(name, &p[1], SMI_MAX_OID);
	    l = strcspn(spec, "!");
	    strncpy(modulename, spec, l);
	    modulename[l] = 0;
	} else if ((p = strchr(spec, '.'))) {
	    /* SMIv1/v2 style module/label separator */
	    strncpy(name, &p[1], SMI_MAX_OID);
	    l = strcspn(spec, ".");
	    strncpy(modulename, spec, l);
	    modulename[l] = 0;
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
    char	    *spec;
    char	    *mod;
    char	    *fullname;
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
    int  l;
    
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
getObject(name, modulename)
    char *name;
    char *modulename;
{
    Object	    *objectPtr = NULL;
    Node	    *nodePtr;
    char	    *elements, *element;
    SmiSubid	    subid;
    
    printDebug(5, "getObject(%s, %s)\n", name, modulename ? modulename : "");

    if (isdigit((int)name[0])) {
	/*
	 * name in `1.3.0x6.1...' form.
	 */
	elements = strdup(name);
	/* TODO: success? */
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



Object *
getNextChildObject(startNodePtr, modulePtr)
    Node   *startNodePtr;
    Module *modulePtr;
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



SmiNode *
createSmiNode(objectPtr)
    Object        *objectPtr;
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
	    for (listPtr = objectPtr->indexPtr->listPtr; listPtr;
		 listPtr = listPtr->nextPtr) {
		addName(&smiNodePtr->index,
			((Object *)listPtr->ptr)->modulePtr->name,
			((Object *)listPtr->ptr)->name);
	    }
	    if (objectPtr->indexPtr->rowPtr) {
		smiNodePtr->relatedrow = objectPtr->indexPtr->rowPtr->name;
	    }
	}
	
	smiNodePtr->list	= NULL;
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
    char      *modulename;
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



SmiModule *
smiGetModule(name)
    char    *name;
{
    SmiModule	      *smiModulePtr;
    Module	      *modulePtr;
    
    printDebug(4, "smiGetModule(\"%s\")\n", name);

    modulePtr = findModuleByName(name);
    
    if (!modulePtr) {
	modulePtr = loadModule(name);
    }
    
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



SmiRevision *
smiGetFirstRevision(modulename)
    char		*modulename;
{
    SmiRevision		*smiRevisionPtr;
    Module	        *modulePtr;
    
    printDebug(4, "smiGetFirstRevision(\"%s\")\n", modulename);

    modulePtr = findModuleByName(modulename);
    
    if (!modulePtr) {
	modulePtr = loadModule(modulename);
    }
    
    if (modulePtr && (modulePtr->firstRevisionPtr)) {
	smiRevisionPtr = util_malloc(sizeof(SmiRevision));
	
	smiRevisionPtr->date = modulePtr->firstRevisionPtr->date;
	if (modulePtr->firstRevisionPtr->description) {
	    smiRevisionPtr->description =
		modulePtr->firstRevisionPtr->description;
	} else {
	    smiRevisionPtr->description = NULL;
	}
	return smiRevisionPtr;
    }

    return NULL;
}



SmiRevision *
smiGetNextRevision(modulename, date)
    char	      *modulename;
    time_t	      date;
{
    SmiRevision		*smiRevisionPtr;
    Module	        *modulePtr;
    Revision	        *revisionPtr;
    
    printDebug(4, "smiGetNextRevision(\"%s\", %ld)\n", modulename, date);

    modulePtr = findModuleByName(modulename);
    
    if (!modulePtr) {
	modulePtr = loadModule(modulename);
    }
    
    if (modulePtr && (modulePtr->firstRevisionPtr)) {
	for (revisionPtr = modulePtr->firstRevisionPtr;
	     revisionPtr && (revisionPtr->date != date);
	     revisionPtr = revisionPtr->nextPtr);
	revisionPtr = revisionPtr->nextPtr;
	if (revisionPtr) {
	    smiRevisionPtr = util_malloc(sizeof(SmiRevision));

	    smiRevisionPtr->date = revisionPtr->date;
	    if (revisionPtr->description) {
	    smiRevisionPtr->description = revisionPtr->description;
	    } else {
		smiRevisionPtr->description = NULL;
	    }
	    return smiRevisionPtr;
	}
    }

    return NULL;
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
    
    printDebug(4, "smiGetNode(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    getModulenameAndName(spec, mod, modulename, name);

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
    printDebug(4, "smiGetFirstNode(\"%s\")\n", modulename);
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
    
    printDebug(4, "smiGetNextNode(\"%s\", %s)\n",
	       modulename, name ? name : "");

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



SmiType *
smiGetType(spec, mod)
    char	    *spec;
    char	    *mod;
{
    SmiType	    *smiTypePtr;
    Type	    *typePtr = NULL;
    Module	    *modulePtr = NULL;
    List	    *listPtr;
    char	    name[SMI_MAX_OID+1];
    char	    modulename[SMI_MAX_DESCRIPTOR+1];
    
    printDebug(4, "smiGetType(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    getModulenameAndName(spec, mod, modulename, name);

    if (modulename) {
	modulePtr = findModuleByName(modulename);
	if (!modulePtr) {
	    modulePtr = loadModule(modulename);
	}
    }

    typePtr = getType(name, modulename);
    
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



SmiType *
smiGetFirstType(modulename)
    char	*modulename;
{
    printDebug(4, "smiGetFirstType(\"%s\")\n", modulename);
    return smiGetNextType(modulename, NULL);
}



SmiType *
smiGetNextType(modulename, name)
    char	      *modulename;
    char	      *name;
{
    SmiType	      *smiTypePtr;
    Module	      *modulePtr;
    Type	      *typePtr;
    int		      hit;
    List	      *listPtr;
    
    printDebug(4, "smiGetNextType(\"%s\", %s)\n",
	       modulename, name ? name : "");

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
	    smiTypePtr = util_malloc(sizeof(SmiType));
	    
	    smiTypePtr->name        = typePtr->name;
	    smiTypePtr->module	    = typePtr->modulePtr->name;
	    smiTypePtr->basetype	    = typePtr->basetype;
	    smiTypePtr->parent	    = typePtr->parentType;
	    smiTypePtr->list	    = NULL;
	    for (listPtr = typePtr->listPtr; listPtr;
		 listPtr = listPtr->nextPtr) {
		addPtr(&smiTypePtr->list, listPtr->ptr);
	    }
	    smiTypePtr->decl	    = typePtr->decl;
	    smiTypePtr->format	    = typePtr->format;
	    smiTypePtr->value	    = typePtr->valuePtr;
	    smiTypePtr->units	    = typePtr->units;
	    smiTypePtr->status      = typePtr->status;
	    smiTypePtr->description = typePtr->description;
	    smiTypePtr->reference   = typePtr->reference;
	    return smiTypePtr;
	}
    }

    return NULL;
}



SmiMacro *
smiGetMacro(spec, mod)
    char            *spec;
    char	    *mod;
{
    SmiMacro	    *smiMacroPtr;
    Macro	    *macroPtr = NULL;
    Module	    *modulePtr = NULL;
    char	    name[SMI_MAX_OID+1];
    char	    modulename[SMI_MAX_DESCRIPTOR+1];
    
    printDebug(4, "smiGetMacro(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    getModulenameAndName(spec, mod, modulename, name);

    if (modulename) {
	modulePtr = findModuleByName(modulename);
	if (!modulePtr) {
	    modulePtr = loadModule(modulename);
	}
    }

    macroPtr = getMacro(name, modulename);
    
    if (macroPtr) {
	smiMacroPtr = util_malloc(sizeof(SmiMacro));

	smiMacroPtr->name   = macroPtr->name;
	smiMacroPtr->module = macroPtr->modulePtr->name;
	return smiMacroPtr;
    } else {
	return NULL;
    }
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
    
    printDebug(4, "smiGetNames(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    getModulenameAndName(spec, mod, modulename, name);

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
	/* TODO: success? */
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



char **
smiGetChildren(spec, mod)
    char	        *spec;
    char		*mod;
{
#if 0
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
#endif
    return NULL;
}



char **
smiGetMembers(spec, mod)
    char	        *spec;
    char		*mod;
{
    Module		*modulePtr = NULL;
    Object		*objectPtr;
    Import		*importPtr;
    char	        name[SMI_MAX_OID+1];
    char	        modulename[SMI_MAX_DESCRIPTOR+1];
    char		**list = NULL;
    
    printDebug(4, "smiGetMembers(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    /*
     * First determine the kind of input to decide what kind of member
     * list we are looking for. It might be
     *
     * - a module name, to retrieve all imported identifiers,
     * - a row (SEQUENCE), to retrieve all of its columnar object types,
     * - a table (SEQUENCE OF), to retrieve all of its index object types,
     * - a notification type, to retrieve all of its variables,
     * - an object group, to retrieve all of its objects types,
     * - a notification group, to retrieve all of its notification types,
     *
     */

    getModulenameAndName(spec, mod, modulename, name);

    if (!strlen(modulename)) {
	strncpy(modulename, name, SMI_MAX_DESCRIPTOR);
	name[0] = 0;
    }
    
    if (modulename) {
	modulePtr = findModuleByName(modulename);
	if (!modulePtr) {
	    modulePtr = loadModule(modulename);
	}
    }

    if (!modulePtr) {
	return NULL;
    }

    if (!strlen(name)) {

	/*
	 * a module name, to retrieve all imported identifiers
	 */
	for (importPtr = modulePtr->firstImportPtr; importPtr;
	     importPtr = importPtr->nextPtr) {
	    addName(&list, importPtr->module, importPtr->name);
	}

    } else if (!isupper((int)name[0])) {

	/*
	 * a type
	 */

    } else if (!isupper((int)name[0])) {

	/*
	 * any object type, look closer...
	 */
	objectPtr = findObjectByModulenameAndName(modulename, name);
	if (objectPtr) {
	    if (objectPtr->typePtr->basetype == SMI_BASETYPE_SEQUENCEOF) {
		/*
		 * a table, to retrieve all of its index object types
		 */
	    } else if (objectPtr->typePtr->basetype == SMI_BASETYPE_SEQUENCE) {
		/*
		 * a row, to retrieve all of its columnar object types
		 */
	    }
	    /*
	     * a notification type, to retrieve all of its variables
	     */
	    /*
	     * an object group, to retrieve all of its objects types
	     */
	    /*
	     * a notification group, to retrieve all of its notification types
	     */
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
    
    printDebug(4, "smiGetParent(\"%s\", \"%s\")\n",
	       spec, mod ? mod : "NULL");

    parent = util_malloc(sizeof(char) * (SMI_MAX_FULLNAME+1));
	    
    getModulenameAndName(spec, mod, modulename, name);

    if (modulename) {
	if (!findModuleByName(modulename)) {
	    loadModule(modulename);
	}
    }

    /*
     * If the parent node has a declaration in the same module as
     * the child, we assume this is what the user wants to get.
     * Otherwise we return the first declaration we find in our
     * view of modules.  Last resort: the numerical OID.
     */
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
	    if (smiIsImported(modulename, s)) {
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
		strcpy(parent, p);
		sprintf(&parent[strlen(parent)], ".%d", nodePtr->subid);
	    }
	} else {
	    /*
	     * call getParent() recursively to get the most verbose
	     * name that is in the current view.
	     */
	    strncpy(child, getOid(nodePtr), SMI_MAX_FULLNAME);
	    p = smiGetParent(child, "");
	    strcpy(parent, p);
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

    printDebug(5, " ... = %s\n", parent ? parent : "");

    return parent;
}



/*
 *
 */



time_t
smiMkTime(s)
    const char *s;
{
    struct tm  tm;
    char       tmp[3];
    time_t     t;
    
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
	/* seems to be SMIv2 13-char format yyyymmddhhmmZ */
	tmp[4] = 0;
	tm.tm_year = atoi(strncpy(tmp, &s[0], 4)) - 1900;
	tmp[2] = 0;
	tm.tm_mon = atoi(strncpy(tmp, &s[4], 2)) - 1;
	tm.tm_mday = atoi(strncpy(tmp, &s[6], 2));
	tm.tm_hour = atoi(strncpy(tmp, &s[8], 2));
	tm.tm_min = atoi(strncpy(tmp, &s[10], 2));
    }
    /* TODO: SMIng format */

    putenv("TZ=UTC"); tzset();
    /* TODO: a better way to make mktime() use UTC !? */

    t = mktime(&tm);
   
    return t;
}



/* NOTE: not reentrent. returning a static pointer. */
char *
smiCTime(t)
    time_t t;
{
    static char   s[27];
    struct tm	  *tm;

    tm = gmtime(&t);
    sprintf(s, "%04d-%02d-%02d %02d:%02d",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min);
    return s;
}



/* NOTE: not reentrent. returning a static pointer. */
char *
smiModule(fullname)
    char            *fullname;
{
    static char	    modulename[SMI_MAX_DESCRIPTOR+1];
    int		    len;

    if (!fullname) return NULL;
    
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



int
smiIsImported(modulename, fullname)
    char	   *modulename;
    char	   *fullname;
{
    Import	   *importPtr;
    Module	   *modulePtr;
    char	   *importedModule;
    char	   *importedDescriptor;
    
    modulePtr = findModuleByName(modulename);
    
    if (!modulePtr) {
	modulePtr = loadModule(modulename);
    }
    
    if (modulePtr) {

        /* TODO: ensure, that imports are loaded if module location is rpc */

	importedModule = smiModule(fullname);
	importedDescriptor = smiDescriptor(fullname);
	
	for (importPtr = modulePtr->firstImportPtr; importPtr;
	     importPtr = importPtr->nextPtr) {
	    if ((!strcmp(importedModule, importPtr->module)) &&
		(!strcmp(importedDescriptor, importPtr->name))) {
		return 1;
	    }
	}
    }

    return 0;
}
