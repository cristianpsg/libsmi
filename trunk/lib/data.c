/*
 * data.c --
 *
 *      Operations on the main data structures.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: data.c,v 1.7 1999/03/17 19:09:06 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "error.h"
#include "util.h"
#include "data.h"
#include "smi.h"



#define stringKind(kind) ( \
	(kind == KIND_ANY)                 ? "ANY" : \
	(kind == KIND_MODULE)              ? "MODULE" : \
	(kind == KIND_MACRO)               ? "MACRO" : \
	(kind == KIND_TYPE)                ? "TYPE" : \
	(kind == KIND_OBJECT)              ? "OBJECT" : \
	(kind == KIND_IMPORT)              ? "IMPORT" : \
					     "unknown" )


View	        *firstViewPtr, *lastViewPtr;
Location	*firstLocationPtr, *lastLocationPtr;
Module          *firstModulePtr, *lastModulePtr;
Node		*rootNodePtr;
Node		*pendingNodePtr;
Type		*typeIntegerPtr, *typeOctetStringPtr, *typeObjectIdentifierPtr;
int		smiFlags;


/*
 *----------------------------------------------------------------------
 *
 * addView --
 *
 *      Add a module to the `view' (the list of modules, seen by the user).
 *
 * Results:
 *      A pointer to the new View structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

View *
addView(modulename)
    const char	      *modulename;
{
    View	      *viewPtr;

#ifdef DEBUG
    printDebug(4,
      "addView(%s)\n", modulename);
#endif
    
    viewPtr = (View *)util_malloc(sizeof(View));
    if (!viewPtr) {
	printError(NULL, ERR_ALLOCATING_VIEW, strerror(errno));
	return (NULL);
    }

    viewPtr->name				= util_strdup(modulename);
    viewPtr->nextPtr				= NULL;
    viewPtr->prevPtr				= lastViewPtr;
    if (!firstViewPtr) firstViewPtr		= viewPtr;
    if (lastViewPtr) lastViewPtr->nextPtr	= viewPtr;
    lastViewPtr	     				= viewPtr;
    
    return (viewPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * addLocation --
 *
 *      Create a new location to look for modules.
 *
 * Results:
 *      A pointer to the new Location structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Location *
addLocation(location, flags)
    const char	      *location;
    ModuleFlags	      flags;
{
    struct stat	      st;
    Location	      *locationPtr;
    Parser	      parser;
    char	      s[200];

#ifdef DEBUG
    printDebug(4,
      "addLocation(%s, %d)\n", location, flags);
#endif
    
    locationPtr = (Location *)util_malloc(sizeof(Location));
    if (!locationPtr) {
	printError(NULL, ERR_ALLOCATING_LOCATION, strerror(errno));
	return (NULL);
    }
    locationPtr->type = LOCATION_UNKNOWN;

#ifdef BACKEND_RPC
    if (strstr(location, "smirpc://") == location) {
	locationPtr->type = LOCATION_RPC;
	locationPtr->name = util_strdup(&location[9]);
	/*
	 * We must use TCP, since some messages may exceed the UDP limitation
	 * of messages larger than UDPMSGSIZE==8800 (at least on Linux).
	 */
	locationPtr->cl   = clnt_create(locationPtr->name,
				     SMIPROG, SMIVERS, "tcp");
	if (!locationPtr->cl) {
	    clnt_pcreateerror(locationPtr->name);
	    free(locationPtr);
	    return NULL;
	}
    }
#endif
#ifdef BACKEND_SMI
    if (strstr(location, "smi:") == location) {
	locationPtr->name = util_strdup(&location[4]);
	if (stat(locationPtr->name, &st)) {
	    printError(NULL, ERR_LOCATION, location, strerror(errno));
	    free(locationPtr);
	    return NULL;
	} else {
	    if (S_ISDIR(st.st_mode)) {
		locationPtr->type = LOCATION_SMIDIR;
	    } else {
		locationPtr->type = LOCATION_SMIFILE;
		/*
		 * MIB locations of type `file' are read immediately.
		 */
		parser.path		= util_strdup(locationPtr->name);
		parser.locationPtr	= locationPtr;
		parser.flags		= flags;
		parser.modulePtr	= NULL;
		parser.file		= fopen(parser.path, "r");
		if (!parser.file) {
		    printError(NULL, ERR_OPENING_INPUTFILE, parser.path,
			       strerror(errno));
		    free(locationPtr);
		    return NULL;
		} else {
		    if (enterLexRecursion(parser.file) < 0) {
			printError(&parser, ERR_MAX_LEX_DEPTH);
			fclose(parser.file);
			free(locationPtr);
			return NULL;
		    }
		    parser.line		= 1;
		    parser.column	= 1;
		    parser.character	= 1;
		    yyparse((void *)&parser);
		    leaveLexRecursion();
		    fclose(parser.file);
		    if (parser.flags & FLAG_STATS) {
			sprintf(s, " (%d lines)", parser.line-1);
			printError(&parser, ERR_STATISTICS, s);
		    }
		}
	    }
	}
    }
#endif

#ifdef BACKEND_SMING
    if (strstr(location, "sming:") == location) {
	locationPtr->name = util_strdup(&location[6]);
	if (stat(locationPtr->name, &st)) {
	    printError(NULL, ERR_LOCATION, location, strerror(errno));
	    free(locationPtr);
	    return NULL;
	} else {
	    if (S_ISDIR(st.st_mode)) {
		locationPtr->type = LOCATION_SMINGDIR;
	    } else {
		locationPtr->type = LOCATION_SMINGFILE;
		/*
		 * MIB locations of type `file' are read immediately.
		 */
		/* TODO */
	    }
	}
    }
#endif

    if (locationPtr->type == LOCATION_UNKNOWN) {
	printError(NULL, ERR_UNKNOWN_LOCATION_TYPE, location);
	return NULL;
    }
    
    locationPtr->prevPtr = lastLocationPtr;
    locationPtr->nextPtr = NULL;
    if (lastLocationPtr) {
	lastLocationPtr->nextPtr = locationPtr;
    } else {
	firstLocationPtr = locationPtr;
    }
    lastLocationPtr = locationPtr;
    
    return (locationPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * addModule --
 *
 *      Create a new MIB module.
 *
 * Results:
 *      A pointer to the new Module structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Module *
addModule(modulename, path, locationPtr, fileoffset, flags, parserPtr)
    const char	      *modulename;
    const char	      *path;
    Location	      *locationPtr;
    off_t	      fileoffset;
    ModuleFlags	      flags;
    Parser	      *parserPtr;
{
    Module	      *modulePtr;

#ifdef DEBUG
    printDebug(4,
      "addModule(%s, %s, 0x%x(%s), %d, %d, 0x%x)\n",
	       modulename, path, locationPtr,
	       locationPtr ? locationPtr->name : "",
	       fileoffset, flags, parserPtr);
#endif
    
    modulePtr = (Module *)util_malloc(sizeof(Module));
    if (!modulePtr) {
	printError(parserPtr, ERR_ALLOCATING_MIBMODULE, strerror(errno));
	return (NULL);
    }

    modulePtr->name				= util_strdup(modulename);
    modulePtr->path			        = util_strdup(path);
    modulePtr->locationPtr			= locationPtr;
    modulePtr->fileoffset			= fileoffset;
    modulePtr->flags				= flags;
    modulePtr->objectPtr			= NULL;
    
    modulePtr->firstObjectPtr			= NULL;
    modulePtr->lastObjectPtr			= NULL;
    modulePtr->firstTypePtr			= NULL;
    modulePtr->lastTypePtr			= NULL;
    modulePtr->firstMacroPtr			= NULL;
    modulePtr->lastMacroPtr			= NULL;
    modulePtr->firstImportPtr			= NULL;
    modulePtr->lastImportPtr			= NULL;
    modulePtr->firstRevisionPtr			= NULL;
    modulePtr->lastRevisionPtr			= NULL;
    
    modulePtr->numImportedIdentifiers		= 0;
    modulePtr->numStatements			= 0;
    modulePtr->numModuleIdentities		= 0;
    
    modulePtr->lastUpdated			= 0;
    modulePtr->organization			= NULL;
    modulePtr->contactInfo			= NULL;

    modulePtr->nextPtr				= NULL;
    modulePtr->prevPtr				= lastModulePtr;
    if (!firstModulePtr) firstModulePtr		= modulePtr;
    if (lastModulePtr) lastModulePtr->nextPtr	= modulePtr;
    lastModulePtr				= modulePtr;
    
    return (modulePtr);
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleIdentityObject --
 *
 *      Set the objectPtr of a given Module to the OBJECT-IDENTITY object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setModuleIdentityObject(modulePtr, objectPtr)
    Module	*modulePtr;
    Object	*objectPtr;
{
#ifdef DEBUG
    printDebug(5, "setModuleIdentityObject(0x%x(%s), 0x%x(%s))\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       objectPtr,
	       objectPtr->name ? objectPtr->name : "\"\"");
#endif

    modulePtr->objectPtr = objectPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleLastUpdated --
 *
 *      Set the lastUpdated time_t value of a given Module.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setModuleLastUpdated(modulePtr, lastUpdated)
    Module	*modulePtr;
    time_t	lastUpdated;
{
#ifdef DEBUG
    printDebug(5, "setModuleLastUpdated(0x%x(%s), %ld)\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       lastUpdated);
#endif

    modulePtr->lastUpdated = lastUpdated;
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleOrganization --
 *
 *      Set the organization string of a given Module.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setModuleOrganization(modulePtr, organization)
    Module *modulePtr;
    char *organization;
{
#ifdef DEBUG
    printDebug(5, "setModuleOrganization(0x%x(%s), %s)\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       organization);
#endif

    modulePtr->organization = util_strdup(organization);
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleContactInfo --
 *
 *      Set the contactInfo string of a given Module.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setModuleContactInfo(modulePtr, contactInfo)
    Module	  *modulePtr;
    char	  *contactInfo;
{
#ifdef DEBUG
    printDebug(5, "setModuleContactInfo(0x%x(%s), %s)\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       contactInfo);
#endif

    modulePtr->contactInfo = util_strdup(contactInfo);
}



/*
 *----------------------------------------------------------------------
 *
 * findModuleByName --
 *
 *      Lookup a Module by a given name.
 *
 * Results:
 *      A pointer to the Module structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Module *
findModuleByName(modulename)
    const char  *modulename;
{
    Module	*modulePtr;

#ifdef DEBUG
    printDebug(6, "findModuleByName(%s)", modulename);
#endif
    
    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	if ((modulePtr->name) && !strcmp(modulePtr->name, modulename)) {
#ifdef DEBUG
	    printDebug(4, " = 0x%x(%s)\n", modulePtr, modulePtr->name);
#endif
	    return (modulePtr);
	}
    }

#ifdef DEBUG
    printDebug(6, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * addRevision --
 *
 *      Adds a revision entry for the given module.
 *
 * Results:
 *      0 on success or -1 on an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Revision *
addRevision(date, description, parserPtr)
    time_t	  date;
    const char	  *description;
    Parser	  *parserPtr;
{
    Revision	  *revisionPtr;
    Module	  *modulePtr;
#ifdef DEBUG
    printDebug(4, "addRevision(%ld, %s, 0x%x)\n",
	       date, description, parserPtr);
#endif

    revisionPtr = (Revision *)util_malloc(sizeof(Revision));
    if (!revisionPtr) {
	printError(parserPtr, ERR_ALLOCATING_REVISION, strerror(errno));
	return (NULL);
    }

    modulePtr = parserPtr->modulePtr;
    
    revisionPtr->date		       	 = date;
    revisionPtr->description	       	 = util_strdup(description);
    
    revisionPtr->nextPtr		 = NULL;
    revisionPtr->prevPtr		 = modulePtr->lastRevisionPtr;
    if (!modulePtr->firstRevisionPtr)
	modulePtr->firstRevisionPtr	 = revisionPtr;
    if (modulePtr->lastRevisionPtr)
	modulePtr->lastRevisionPtr->nextPtr = revisionPtr;
    modulePtr->lastRevisionPtr		 = revisionPtr;
    
    return (revisionPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * addImport --
 *
 *      Adds a descriptor to the actual module's list of imported
 *      descriptors. This list may be checked by checkImports()
 *	afterwards.
 *
 * Results:
 *      0 on success or -1 on an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Import *
addImport(name, parserPtr)
    const char    *name;
    Parser	  *parserPtr;
{
    Import        *importPtr;
    Module	  *modulePtr;
#ifdef DEBUG
    printDebug(4, "addImport(%s, 0x%x)\n", name, parserPtr);
#endif

    importPtr = (Import *)util_malloc(sizeof(Import));
    if (!importPtr) {
	printError(parserPtr, ERR_ALLOCATING_IMPORT, strerror(errno));
	return (NULL);
    }

    modulePtr = parserPtr->modulePtr;
    
    importPtr->name		       	 = util_strdup(name);
    importPtr->module			 = NULL; /* not yet known */
    importPtr->kind			 = KIND_UNKNOWN; /* not yet known */
    
    importPtr->nextPtr			 = NULL;
    importPtr->prevPtr			 = modulePtr->lastImportPtr;
    if (!modulePtr->firstImportPtr)
	modulePtr->firstImportPtr	 = importPtr;
    if (modulePtr->lastImportPtr)
	modulePtr->lastImportPtr->nextPtr = importPtr;
    modulePtr->lastImportPtr		 = importPtr;
    
    return (importPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * checkImports --
 *
 *      Check wheather all descriptors in the actual module's list
 *	are imported by a given Module. Implicitly set all Imports'
 *	module names.
 *
 * Results:
 *      0 on success or -1 on an error or number of descriptors not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
checkImports(modulename, parserPtr)
    char        *modulename;
    Parser	*parserPtr;
{
    int         n = 0;
    Import      *importPtr;
    
#ifdef DEBUG
    printDebug(4, "checkImports(%s, 0x%x) ...\n", modulename, parserPtr);
#endif

    for (importPtr = parserPtr->modulePtr->firstImportPtr;
	 importPtr; importPtr = importPtr->nextPtr) {

	if (importPtr->kind == KIND_UNKNOWN) {
	    if (smiGetNode(importPtr->name, modulename)) {
		importPtr->module = util_strdup(modulename);
		importPtr->kind   = KIND_OBJECT;
	    } else if (smiGetType(importPtr->name, modulename)) {
		importPtr->module = util_strdup(modulename);
		importPtr->kind   = KIND_TYPE;
	    } else if (smiGetMacro(importPtr->name, modulename)) {
		importPtr->module = util_strdup(modulename);
		importPtr->kind   = KIND_MACRO;
	    } else {
		n++;
		importPtr->module = util_strdup(modulename);
		printError(parserPtr, ERR_IDENTIFIER_NOT_IN_MODULE,
			   importPtr->name, modulename);
	    }
	}
    }

#ifdef DEBUG
    printDebug(4, "... = %d\n", n);
#endif

    return (n);
}



/*
 *----------------------------------------------------------------------
 *
 * findImportByName --
 *
 *      Lookup an import descriptor by its name and the module to look in.
 *
 * Results:
 *      A pointer to the Import structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Import *
findImportByName(importname, modulePtr)
    const char     *importname;
    Module	   *modulePtr;
{
    Import           *importPtr;

#ifdef DEBUG
    printDebug(4, "findImportByName(%s, 0x%x)", importname, modulePtr);
#endif

    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if (!strcmp(importPtr->name, importname)) {
#ifdef DEBUG
	    printDebug(4, " = 0x%x(%s)\n", importPtr, importPtr->name);
#endif
		return (importPtr);
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
    
}



/*
 *----------------------------------------------------------------------
 *
 * findImportByModulenameAndName --
 *
 *      Lookup an import descriptor by its name and the modulename
 *	it is imported from and the module to look in.
 *
 * Results:
 *      A pointer to the Import structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Import *
findImportByModulenameAndName(modulename, importname, modulePtr)
    const char     *modulename;
    const char     *importname;
    Module	   *modulePtr;
{
    Import           *importPtr;

#ifdef DEBUG
    printDebug(4, "findImportByModulenameAndName(%s, %s, 0x%x)",
	modulename, importname, modulePtr);
#endif

    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if ((!strcmp(importPtr->name, importname)) &&
	    (!strcmp(importPtr->module, modulename))) {
#ifdef DEBUG
	    printDebug(4, " = 0x%x(%s)\n", importPtr, importPtr->name);
#endif
	    return (importPtr);
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);

}



/*
 *----------------------------------------------------------------------
 *
 * addObject --
 *
 *      Create a new Object and Node or update an existing one.
 *	Also updates other Objects and Nodes according
 *	to the PendingNode information.
 *
 * Results:
 *      A pointer to the new Object structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
addObject(objectname, parentNodePtr, subid, flags, parserPtr)
    const char	     *objectname;
    Node             *parentNodePtr;
    smi_subid	     subid;
    ObjectFlags	     flags;
    Parser	     *parserPtr;
{
    Object	     *objectPtr;
    Node	     *nodePtr;
    Module	     *modulePtr;

#ifdef DEBUG
    printDebug(5, "addObject(%s, 0x%x%s, %d, %d, 0x%x)\n",
	       objectname, parentNodePtr,
	       parentNodePtr == pendingNodePtr ? "(pending)" : "",
	       subid, flags, parserPtr);
#endif
    
    objectPtr = (Object *)util_malloc(sizeof(Object));
    if (!objectPtr) {
	printError(parserPtr, ERR_ALLOCATING_OBJECT, strerror(errno));
	return (NULL);
    }

    modulePtr = parserPtr ? parserPtr->modulePtr : NULL;
    
    objectPtr->modulePtr		        = modulePtr;
    objectPtr->name				= util_strdup(objectname);
    objectPtr->fileoffset			= -1;
    objectPtr->nodePtr				= NULL;
    objectPtr->prevSameNodePtr			= NULL;
    objectPtr->nextSameNodePtr			= NULL;
    objectPtr->typePtr				= NULL;
    objectPtr->indexPtr				= NULL;
    objectPtr->decl				= SMI_DECL_UNKNOWN;
    objectPtr->access				= SMI_ACCESS_UNKNOWN;
    objectPtr->status				= SMI_STATUS_UNKNOWN;
    objectPtr->flags				= flags;
    objectPtr->description			= NULL;
    objectPtr->reference			= NULL;
    objectPtr->units				= NULL;
    
    objectPtr->nextPtr				= NULL;
    if (modulePtr) {
        objectPtr->prevPtr			= modulePtr->lastObjectPtr;
	if (!modulePtr->firstObjectPtr)
	    modulePtr->firstObjectPtr		= objectPtr;
	if (modulePtr->lastObjectPtr)
	    modulePtr->lastObjectPtr->nextPtr	= objectPtr;
	modulePtr->lastObjectPtr		= objectPtr;
    } else {
	objectPtr->prevPtr			= NULL;
    }
    
    /*
     * Link it into the tree.
     */
    if ((parentNodePtr == pendingNodePtr) ||
	(!(nodePtr = findNodeByParentAndSubid(parentNodePtr, subid)))) {

	/* a new Node has to be created for this Object */
	nodePtr = addNode(parentNodePtr, subid, flags, parserPtr);
	nodePtr->firstObjectPtr			      = objectPtr;
	nodePtr->lastObjectPtr			      = objectPtr;
	
    } else {

        objectPtr->prevSameNodePtr		      = nodePtr->lastObjectPtr;
	if (!nodePtr->firstObjectPtr)
	    nodePtr->firstObjectPtr	              = objectPtr;
	if (nodePtr->lastObjectPtr)
	    nodePtr->lastObjectPtr->nextSameNodePtr   = objectPtr;
	nodePtr->lastObjectPtr			      = objectPtr;
    }
    objectPtr->nodePtr				      = nodePtr;

#ifdef DEBUG
    printDebug(5, "...addObject() = 0x%x\n", objectPtr);
#endif
    
    return (objectPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * duplicateObject --
 *
 *      Create a new Object as a duplicate of a given one but with
 *      an affiliation to another module with new flags and with
 *	uninitialzied values.
 *
 * Results:
 *      A pointer to the new Object structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
duplicateObject(templatePtr, flags, parserPtr)
    Object		  *templatePtr;
    ObjectFlags		  flags;
    Parser		  *parserPtr;
{
    Object		  *objectPtr;
    Node		  *nodePtr;
    Module		  *modulePtr;
    
#ifdef DEBUG
    printDebug(5, "duplicateObject(0x%x, %d, 0x%x)",
	       templatePtr, flags, parserPtr);
#endif
    
    objectPtr = (Object *)util_malloc(sizeof(Object));
    if (!objectPtr) {
	printError(parserPtr, ERR_ALLOCATING_OBJECT, strerror(errno));
	return (NULL);
    }

    modulePtr = parserPtr->modulePtr;
    nodePtr   = templatePtr->nodePtr;
    
    objectPtr->modulePtr		              = modulePtr;
    objectPtr->name				      = NULL;
    objectPtr->fileoffset			      = -1;
    objectPtr->nodePtr				      = nodePtr;
    objectPtr->prevSameNodePtr			      = NULL;
    objectPtr->nextSameNodePtr			      = NULL;
    objectPtr->typePtr				      = NULL;
    objectPtr->indexPtr				      = NULL;
    objectPtr->decl				      = SMI_DECL_UNKNOWN;
    objectPtr->access				      = SMI_ACCESS_UNKNOWN;
    objectPtr->status				      = SMI_STATUS_UNKNOWN;
    objectPtr->flags				      = flags;
    objectPtr->description			      = NULL;
    objectPtr->reference			      = NULL;
    objectPtr->units				      = NULL;

    objectPtr->nextPtr				= NULL;
    if (modulePtr) {
        objectPtr->prevPtr			= modulePtr->lastObjectPtr;
	if (!modulePtr->firstObjectPtr)
	    modulePtr->firstObjectPtr		= objectPtr;
	if (modulePtr->lastObjectPtr)
	    modulePtr->lastObjectPtr->nextPtr	= objectPtr;
	modulePtr->lastObjectPtr		= objectPtr;
    } else {
	objectPtr->prevPtr			= NULL;
    }
    
    objectPtr->prevSameNodePtr			      = nodePtr->lastObjectPtr;
    if (!nodePtr->firstObjectPtr)
	nodePtr->firstObjectPtr			      = objectPtr;
    if (nodePtr->lastObjectPtr)
	nodePtr->lastObjectPtr->nextSameNodePtr       = objectPtr;
    nodePtr->lastObjectPtr			      = objectPtr;
    objectPtr->nodePtr				      = nodePtr;

#ifdef DEBUG
    printDebug(5, "...duplicateObject() = 0x%x\n", objectPtr);
#endif
    
    return (objectPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * addNode --
 *
 *      Create a new Node by a given parent Node and subid.
 *
 * Results:
 *      A pointer to the new Node structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Node *
addNode (parentNodePtr, subid, flags, parserPtr)
    Node	    *parentNodePtr;
    smi_subid	    subid;
    NodeFlags	    flags;
    Parser	    *parserPtr;
{
    Node	    *nodePtr;
    Node	    *c;

#ifdef DEBUG
    printDebug(5, "addNode(0x%x%s, %d, %d, 0x%x)",
	       parentNodePtr,
	       parentNodePtr == pendingNodePtr ? "(pending)" : "",
	       subid, flags, parserPtr);
#endif

    nodePtr = (Node *)util_malloc(sizeof(Node));
    if (!nodePtr) {
	printError(parserPtr, ERR_ALLOCATING_NODE, strerror(errno));
	return (NULL);
    }
    
    nodePtr->flags				= flags;
    nodePtr->subid				= subid;
    nodePtr->parentPtr				= parentNodePtr;
    nodePtr->firstChildPtr			= NULL;
    nodePtr->lastChildPtr			= NULL;
    nodePtr->firstObjectPtr			= NULL;
    nodePtr->lastObjectPtr			= NULL;

    if (parentNodePtr) {
	if (parentNodePtr->firstChildPtr) {
	    for (c = parentNodePtr->firstChildPtr;
		 c && (c->subid < subid);
		 c = c->nextPtr);
	    if (c) {
		if (c != parentNodePtr->firstChildPtr) {
		    c->prevPtr->nextPtr = nodePtr;
		    nodePtr->prevPtr = c->prevPtr;
		    c->prevPtr = nodePtr;
		    nodePtr->nextPtr = c;
		} else {
		    c->prevPtr = nodePtr;
		    nodePtr->nextPtr = c;
		    nodePtr->prevPtr = NULL;
		    parentNodePtr->firstChildPtr = nodePtr;
		}
	    } else {
		nodePtr->nextPtr = NULL;
		nodePtr->prevPtr = parentNodePtr->lastChildPtr;
		parentNodePtr->lastChildPtr->nextPtr = nodePtr;
		parentNodePtr->lastChildPtr = nodePtr;
	    }
	} else {
	    parentNodePtr->firstChildPtr = nodePtr;
	    parentNodePtr->lastChildPtr = nodePtr;
	    nodePtr->nextPtr = NULL;
	    nodePtr->prevPtr = NULL;
	}
    }

    printDebug(5, " = 0x%x\n", nodePtr);
    
    return nodePtr;
}



/*
 *----------------------------------------------------------------------
 *
 * createNodes --
 *
 *      Create all missing Nodes down the tree along all subids of
 *	a given Oid.
 *
 * Results:
 *      A pointer to the leaf Node structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Node *
createNodes(oid)
    const char		*oid;
{
    char		*p, *elements;
    Node		*parentNodePtr;
    smi_subid		subid;

#ifdef DEBUG
    printDebug(5, "createNodes(%s)\n", oid);
#endif

    parentNodePtr = rootNodePtr;
    elements = util_strdup(oid);

    p = strtok(elements, ".");
    do {
	subid = (unsigned int)strtoul(p, NULL, 0);
	if (!(parentNodePtr = findNodeByParentAndSubid(parentNodePtr,
						       subid))) {
	    parentNodePtr = addNode(parentNodePtr, subid, 0, NULL);
	}
    } while ((p = strtok(NULL, ".")));

    free(elements);

    return parentNodePtr;
}



/*
 *----------------------------------------------------------------------
 *
 * getParentNode --
 *
 *      Return the parent of a given Node.
 *
 * Results:
 *      A pointer to the parent Node structure.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Node *
getParentNode(nodePtr)
    Node *nodePtr;
{
    return nodePtr->parentPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * getLastSubid --
 *
 *      Return the value of the last subid of a given Oid.
 *
 * Results:
 *      A pointer to the parent Node structure.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

smi_subid
getLastSubid(oid)
    const char    *oid;
{
    char	  *p;

    p = strrchr(oid, '.');
    if (p) {
	return (unsigned int)strtoul(&p[1], NULL, 0);
    } else {
	return (unsigned int)strtoul(oid, NULL, 0);
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectName --
 *
 *      Set the name of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
setObjectName(objectPtr, name)
    Object	      *objectPtr;
    smi_descriptor    name;
{
    Node	      *nodePtr, *nextPtr;
    Module	      *modulePtr;
    Object	      *newObjectPtr;
    
#ifdef DEBUG
    printDebug(5, "setObjectName(0x%x, \"%s\")\n", objectPtr, name);
#endif

    objectPtr->name = util_strdup(name);

    /*
     * If this name is found on the pending list (at depth==1 in
     * pendingRootNode), we have to move the corresponding subtree to
     * the main tree.
     */
    for (nodePtr = pendingNodePtr->firstChildPtr; nodePtr;
	 nodePtr = nextPtr) {

	/*
	 * probably we change the contents of `pending', so remember
	 * the next pointer.
	 */
	nextPtr = nodePtr->nextPtr;

	if (!strcmp(nodePtr->firstObjectPtr->name, name)) {

	    /*
	     * remove nodePtr from the pendingRootNode tree.
	     */
	    if (nodePtr->prevPtr) {
		nodePtr->prevPtr->nextPtr = nodePtr->nextPtr;
	    } else {
		pendingNodePtr->firstChildPtr = nodePtr->nextPtr;
	    }
	    if (nodePtr->nextPtr) {
		nodePtr->nextPtr->prevPtr = nodePtr->prevPtr;
	    } else {
		pendingNodePtr->lastChildPtr = nodePtr->prevPtr;
	    }
	    
	    /*
	     * copy contents of the new node to pending.
	     */
	    nodePtr->subid = objectPtr->nodePtr->subid;
	    nodePtr->flags = objectPtr->nodePtr->flags;
	    
	    /*
	     * now link pending node into place.
	     */
	    nodePtr->parentPtr = objectPtr->nodePtr->parentPtr;
	    nodePtr->nextPtr = objectPtr->nodePtr->nextPtr;
	    nodePtr->prevPtr = objectPtr->nodePtr->prevPtr;
	    if (objectPtr->nodePtr->parentPtr->firstChildPtr ==
		objectPtr->nodePtr) {
		objectPtr->nodePtr->parentPtr->firstChildPtr = nodePtr;
	    } else {
		objectPtr->nodePtr->prevPtr->nextPtr = nodePtr;
	    }
	    if (objectPtr->nodePtr->parentPtr->lastChildPtr ==
		objectPtr->nodePtr) {
		objectPtr->nodePtr->parentPtr->lastChildPtr = nodePtr;
	    } else {
		objectPtr->nodePtr->nextPtr->prevPtr = nodePtr;
	    }

	    free(objectPtr->nodePtr);
	    newObjectPtr = nodePtr->firstObjectPtr;
	    modulePtr = newObjectPtr->modulePtr;
	    if (modulePtr->objectPtr == objectPtr) {
		modulePtr->objectPtr = newObjectPtr;
	    }
	    if (modulePtr->firstObjectPtr == objectPtr) {
		modulePtr->firstObjectPtr = objectPtr->nextPtr;
		modulePtr->firstObjectPtr->prevPtr = NULL;
	    }
	    if (modulePtr->lastObjectPtr == objectPtr) {
		modulePtr->lastObjectPtr = objectPtr->prevPtr;
		modulePtr->lastObjectPtr->nextPtr = NULL;
	    }
	    free(objectPtr);
	    return newObjectPtr;
	}
    }
    return objectPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectType --
 *
 *      Set the syntax/type (pointer to a Type struct) of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectType(objectPtr, typePtr)
    Object		   *objectPtr;
    Type		   *typePtr;
{
#ifdef DEBUG
    printDebug(5, "setObjectType(0x%x(%s), 0x%x(%s))\n",
	       objectPtr, objectPtr->name, typePtr,
	       typePtr->name ? typePtr->name : "\"\"");
#endif

    /* TODO: why this check? */
    if (objectPtr->typePtr == NULL) {
	objectPtr->typePtr = typePtr;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectAccess --
 *
 *      Set the access of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectAccess(objectPtr, access)
    Object		   *objectPtr;
    smi_access		   access;
{
#ifdef DEBUG
    printDebug(5, "setObjectAccess(0x%x(%s), %s)\n",
	       objectPtr, objectPtr->name, smiStringAccess(access));
#endif

    /* TODO: why this check? */
    if (objectPtr->access == SMI_ACCESS_UNKNOWN) {
	objectPtr->access = access;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectStatus --
 *
 *      Set the status of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectStatus(objectPtr, status)
    Object		   *objectPtr;
    smi_status		   status;
{
#ifdef DEBUG
    printDebug(5, "setObjectStatus(0x%x(%s), %s)\n",
	       objectPtr,  objectPtr->name, smiStringStatus(status));
#endif

    /* TODO: why this check? */
    if (objectPtr->status == SMI_STATUS_UNKNOWN) {
	objectPtr->status = status;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectDescription --
 *
 *      Set the description of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectDescription(objectPtr, description)
    Object    *objectPtr;
    char      *description;
{
#ifdef DEBUG
    printDebug(5, "setObjectDescription(0x%x(%s), %s)\n",
	       objectPtr, objectPtr->name, description);
#endif

    objectPtr->description = util_strdup(description);
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectReference --
 *
 *      Set the reference of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectReference(objectPtr, reference)
    Object    *objectPtr;
    char      *reference;
{
#ifdef DEBUG
    printDebug(5, "setObjectReference(0x%x(%s), %s)\n",
	       objectPtr, objectPtr->name, reference);
#endif

    objectPtr->reference = util_strdup(reference);
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectUnits --
 *
 *      Set the units of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectUnits(objectPtr, units)
    Object    *objectPtr;
    char      *units;
{
#ifdef DEBUG
    printDebug(5, "setObjectUnits(0x%x(%s), %s)\n",
	       objectPtr, objectPtr->name, units);
#endif

    objectPtr->units = util_strdup(units);
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectFileOffset --
 *
 *      Set the fileoffset of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectFileOffset(objectPtr, fileoffset)
    Object      *objectPtr;
    off_t       fileoffset;
{
#ifdef DEBUG
    printDebug(5, "setObjectFileOffset(0x%x(%s), %d)\n",
	       objectPtr, objectPtr->name, fileoffset);
#endif

    /* TODO: why this check? */
    if (objectPtr->fileoffset < 0) {
	objectPtr->fileoffset = fileoffset;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectDecl --
 *
 *      Set the declaring macro of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectDecl(objectPtr, decl)
    Object	*objectPtr;
    smi_decl    decl;
{
#ifdef DEBUG
    printDebug(5, "setObjectDecl(0x%x(%s), %s)\n",
	       objectPtr, objectPtr->name, smiStringDecl(decl));
#endif

    /* TODO: why this check? */
    if (objectPtr->decl == SMI_DECL_UNKNOWN) {
	objectPtr->decl = decl;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * addObjectFlags --
 *
 *      Add flags to the flags of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
addObjectFlags(objectPtr, flags)
    Object	   *objectPtr;
    ObjectFlags    flags;
{
#ifdef DEBUG
    printDebug(5, "addObjectFlags(0x%x(%s), %d)\n",
	       objectPtr, objectPtr->name, flags);
#endif

    objectPtr->flags |= flags;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectIndex --
 *
 *      Set the list of INDEX elements of a given Object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setObjectIndex(objectPtr, listPtr)
    Object	 *objectPtr;
    List	 *listPtr;
{
#ifdef DEBUG
    printDebug(5, "setObjectIndex(0x%x(%s), 0x%x)\n",
	       objectPtr, objectPtr->name, listPtr);
#endif
    
    objectPtr->indexPtr = listPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * findNodeByParentAndSubid --
 *
 *      Lookup a Node by a given parent and subid value.
 *
 * Results:
 *      A pointer to the Node structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Node *
findNodeByParentAndSubid(parentNodePtr, subid)
    Node            *parentNodePtr;
    unsigned int    subid;
{
    Node *nodePtr;

#ifdef DEBUG
    printDebug(4, "findNodeByParentAndSubid(0x%x, %d)",
	       parentNodePtr, subid);
#endif
    
    if (parentNodePtr) {
	for (nodePtr = parentNodePtr->firstChildPtr; nodePtr;
	     nodePtr = nodePtr->nextPtr) {
	    if (nodePtr->subid == subid) {
#ifdef DEBUG
		printDebug(4, " = %p\n", nodePtr);
#endif
		return (nodePtr);
	    }
	}
    }
    
#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByNode --
 *
 *      Lookup an Object by a given Node. Note, that their might be
 *	multiple definitions for one node.
 *
 * Results:
 *      A pointer to the first Object structure in the current View or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
findObjectByNode(nodePtr)
    Node      *nodePtr;
{
    Object    *objectPtr;
    View      *viewPtr;
    
#ifdef DEBUG
    printDebug(4, "findObjectByNode(0x%x)", nodePtr);
#endif
    
    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	 objectPtr = objectPtr->nextSameNodePtr) {
	for (viewPtr = firstViewPtr; viewPtr; viewPtr = viewPtr->nextPtr) {
	    if (!strcmp(objectPtr->modulePtr->name, viewPtr->name)) {
#ifdef DEBUG
		printDebug(4, " = 0x%x(%s)\n", objectPtr, objectPtr->name);
#endif
		return (objectPtr);
	    }
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByModuleAndNode --
 *
 *      Lookup an Object by a given Node and Module. This is necessary
 *	since there might be different declarations in different modules
 *	for the same OID.
 *
 * Results:
 *      A pointer to the Object structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
findObjectByModuleAndNode(modulePtr, nodePtr)
    Module    *modulePtr;
    Node      *nodePtr;
{
    Object    *objectPtr;

#ifdef DEBUG
    printDebug(4, "findObjectByModuleAndNode(0x%x, 0x%x)",
	       modulePtr, nodePtr);
#endif
    
    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	 objectPtr = objectPtr->nextSameNodePtr) {
	if (objectPtr->modulePtr == modulePtr) {
#ifdef DEBUG
	    printDebug(4, " = 0x%x(%s)\n", objectPtr, objectPtr->name);
#endif
	    return (objectPtr);
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByModulenameAndNode --
 *
 *      Lookup an Object by a given Node and Modulename. This is necessary
 *	since there might be different declarations in different modules
 *	for the same OID.
 *
 * Results:
 *      A pointer to the Object structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
findObjectByModulenameAndNode(modulename, nodePtr)
    const char *modulename;
    Node       *nodePtr;
{
    Object     *objectPtr;

#ifdef DEBUG
    printDebug(4, "findObjectByModulenameAndNode(%s, 0x%x)",
	       modulename, nodePtr);
#endif

    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	 objectPtr = objectPtr->nextSameNodePtr) {
	if (!strcmp(objectPtr->modulePtr->name, modulename)) {
#ifdef DEBUG
	    printDebug(4, " = 0x%x(%s)\n", objectPtr, objectPtr->name);
#endif
	    return (objectPtr);
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByName --
 *
 *      Lookup an Object by a given name. Note, that
 *	there might be more than one Object with the same name.
 *	In this case, it is undefined which Object is returned.
 *
 * Results:
 *      A pointer to the Object structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
findObjectByName(objectname)
    const char       *objectname;
{
    Module	     *modulePtr;
    Object           *objectPtr;

#ifdef DEBUG
    printDebug(4, "findObjectByName(%s)", objectname);
#endif

    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->name) && !strcmp(objectPtr->name, objectname)) {
		/*
		 * We return the first matching object.
		 * TODO: probably we should check if there are more matching
		 *       objects, and give a warning if there's another one.
		 */
#ifdef DEBUG
		printDebug(4, " = 0x%x(%s)\n", objectPtr, objectPtr->name);
#endif
		return (objectPtr);
	    }
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByModulenameAndName --
 *
 *      Lookup a Object by a given Module and name.
 *
 * Results:
 *      A pointer to the Object structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
findObjectByModulenameAndName(modulename, objectname)
    const char       *modulename;
    const char	     *objectname;
{
    Module	     *modulePtr;
    Object	     *objectPtr;

#ifdef DEBUG
    printDebug(4, "findObjectByModulenameAndName(%s, %s)\n",
	       modulename, objectname);
#endif

    modulePtr = findModuleByName(modulename);

    if (modulePtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->name) && !strcmp(objectPtr->name, objectname)) {
#ifdef DEBUG
		printDebug(4, " = 0x%x(%s)\n", objectPtr, objectPtr->name);
#endif
		return (objectPtr);
	    }
	}
    }

    /*
     * Some toplevel Objects seem to be always known.
     */
    if ((!strcmp(objectname, "iso")) ||
	(!strcmp(objectname, "ccitt")) ||
	(!strcmp(objectname, "joint-iso-ccitt"))) {
	return findObjectByName(objectname);
    }

#ifdef DEBUG
    printDebug(4, "... = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByModuleAndName --
 *
 *      Lookup a Object by a given Module and name.
 *
 * Results:
 *      A pointer to the Object structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
findObjectByModuleAndName(modulePtr, objectname)
    Module        *modulePtr;
    const char    *objectname;
{
    Object	  *objectPtr;
    
#ifdef DEBUG
    printDebug(4, "findObjectByModuleAndName(0x%x(%s), %s)",
	       modulePtr, modulePtr->name, objectname);
#endif

    if (modulePtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->name) && !strcmp(objectPtr->name, objectname)) {
#ifdef DEBUG
		printDebug(4, " = 0x%x(%s)\n", objectPtr, objectPtr->name);
#endif
		return (objectPtr);
	    }
	}
    }
    
    /*
     * Some toplevel Objects seem to be always known.
     */
    if ((!strcmp(objectname, "iso")) ||
	(!strcmp(objectname, "ccitt")) ||
	(!strcmp(objectname, "joint-iso-ccitt"))) {
#ifdef DEBUG
	printDebug(4, " ...\n");
#endif
	return findObjectByName(objectname);
    }
    
#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



#if 0
/*
 *----------------------------------------------------------------------
 *
 * dumpMibTree --
 *
 *      Dump MIB subtree with a certain root to stderr.
 *	For debugging purpose.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
dumpMibTree(root, prefix)
    Node *root;
    const char *prefix;
{
    Node *c;
    char s[200];
    
    if (root) {
	if (root == rootNode) {
	    s[0] = 0;
	} else {
	    if (root->firstObject->descriptor) {
		if (root->flags & FLAG_NOSUBID) {
		    sprintf(s, "%s.%s(?)", prefix,
			    root->firstObject->descriptor->name);
		} else {
		    sprintf(s, "%s.%s(%d)", prefix,
			    root->firstObject->descriptor->name, root->subid);
		}
	    } else {
		if (root->flags & FLAG_NOSUBID) {
		    sprintf(s, "%s.?", prefix);
		} else {
		    sprintf(s, "%s.%d", prefix, root->subid);
		}
	    }
	    fprintf(stderr, "%s", s);
	    if (root->firstObject->module) {
		fprintf(stderr, " == %s!%s",
			root->firstObject->module->descriptor->name,
			root->firstObject->descriptor->name);
	    }
	    if (root->firstObject->type &&
		root->firstObject->type->descriptor) {
		fprintf(stderr, "  [%s]",
			root->firstObject->type->descriptor->name);
	    } else {
		if (root->firstObject->type &&
		    root->firstObject->type->parent &&
		    root->firstObject->type->parent->descriptor) {
		    fprintf(stderr, "  [[%s]]",
			    root->firstObject->type->parent->descriptor->name);
		}
	    }
	    fprintf(stderr, "\n");
	}
	for (c = root->firstChild; c; c = c->next) {
	    dumpMibTree(c, s);
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * dumpMosy --
 *
 *      Dump in Mosy format.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
dumpMosy(root)
    Node *root;
{
    Node *c;
    char s[200];
    
    if (root) {
	if (root != rootNode) {
	    if ((root->flags & FLAG_MODULE) &&
		(root->firstObject->descriptor &&
		 root->parent->firstObject->descriptor)) {
		sprintf(s, "%s.%d",
			root->parent->firstObject->descriptor->name,
			root->subid);
		if (root->firstObject->decl == SMI_DECL_OBJECTTYPE) {
		    printf("%-19s %-19s %-15s %-15s %s\n",
			   root->firstObject->descriptor->name,
			   s,
			   "<type>",
			   smiStringAccess(root->firstObject->access),
			   smiStringStatus(root->firstObject->status));
		} else {
		    printf("%-19s %s\n",
			   root->firstObject->descriptor->name,
			   s);
		}
	    }
	}
	for (c = root->firstChild; c; c = c->next) {
	    dumpMosy(c);
	}
    }
}
#endif



/*
 *----------------------------------------------------------------------
 *
 * addType --
 *
 *      Create a new Type structure.
 *
 * Results:
 *      A pointer to the new Type structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *---------------------------------------------------------------------- */

Type *
addType(typename, syntax, flags, parserPtr)
    const char     *typename;
    smi_syntax	   syntax;
    TypeFlags      flags;
    Parser	   *parserPtr;
{
    Type	   *typePtr;
    Module	   *modulePtr;
    
#ifdef DEBUG
    printDebug(4, "addType(%s, %s, %d, 0x%x)",
	       typename ? typename : "\"\"", smiStringSyntax(syntax),
	       flags, parserPtr);
#endif

    modulePtr = parserPtr ? parserPtr->modulePtr : NULL;
    
    typePtr = util_malloc(sizeof(Type));
    if (!typePtr) {
	printError(parserPtr, ERR_ALLOCATING_TYPE, strerror(errno));
	return (NULL);
    }

    if (typename) {
	typePtr->name	                = util_strdup(typename);
    } else {
	typePtr->name			= NULL;
    }
    typePtr->modulePtr			= modulePtr;
    typePtr->syntax			= syntax;
    typePtr->decl			= SMI_DECL_UNKNOWN;
    typePtr->status			= SMI_STATUS_UNKNOWN;
    typePtr->flags			= flags;
    typePtr->sequencePtr		= NULL;
    typePtr->parentType			= NULL;
    typePtr->description		= NULL;
    typePtr->reference			= NULL;
    typePtr->format			= NULL;
    typePtr->units			= NULL;

    typePtr->nextPtr			= NULL;
    if (modulePtr) {
	typePtr->prevPtr		= modulePtr->lastTypePtr;
	if (!modulePtr->firstTypePtr)
	    modulePtr->firstTypePtr	= typePtr;
	if (modulePtr->lastTypePtr)
	    modulePtr->lastTypePtr->nextPtr = typePtr;
	modulePtr->lastTypePtr		= typePtr;
    } else {
	typePtr->prevPtr		= NULL;
    }
	
#ifdef DEBUG
    printDebug(4, " = 0x%x\n", typePtr);
#endif
    return (typePtr);
}



/*
 *----------------------------------------------------------------------
 *
 * duplicateType --
 *
 *      Create a new Type as a duplicate of a given one but with
 *      an affiliation to the current module.
 *
 * Results:
 *      A pointer to the new Type structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Type *
duplicateType(templatePtr, flags, parserPtr)
    Type		  *templatePtr;
    TypeFlags		  flags;
    Parser		  *parserPtr;
{
    Type		  *typePtr;
    Module		  *modulePtr;
    
#ifdef DEBUG
    printDebug(5, "duplicateType(0x%x, %d, 0x%x)...\n",
	       templatePtr, flags, parserPtr);
#endif
    
    typePtr = (Type *)util_malloc(sizeof(Type));
    if (!typePtr) {
	printError(parserPtr, ERR_ALLOCATING_TYPE, strerror(errno));
	return (NULL);
    }

    modulePtr = parserPtr->modulePtr;
    
    typePtr->name	                = NULL;
    typePtr->modulePtr			= modulePtr;
    typePtr->syntax			= templatePtr->syntax;
    typePtr->decl			= templatePtr->decl;
    typePtr->status			= templatePtr->syntax;
    typePtr->sequencePtr		= NULL;
    typePtr->flags			= templatePtr->flags;
    typePtr->parentType			= util_strdup(templatePtr->name);
    typePtr->description		= NULL;
    typePtr->reference			= NULL;
    typePtr->format			= NULL;
    typePtr->units			= NULL;

    typePtr->nextPtr			= NULL;
    typePtr->prevPtr			= modulePtr->lastTypePtr;
    if (!modulePtr->firstTypePtr)
	modulePtr->firstTypePtr		= typePtr;
    if (modulePtr->lastTypePtr)
	modulePtr->lastTypePtr->nextPtr	= typePtr;
    modulePtr->lastTypePtr		= typePtr;
    
#ifdef DEBUG
    printDebug(5, "...duplicateType() = 0x%x\n", typePtr);
#endif
    
    return (typePtr);
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeName --
 *
 *      Set the name of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeName(typePtr, name)
    Type	      *typePtr;
    smi_descriptor    name;
{
#ifdef DEBUG
    printDebug(5, "setTypeName(0x%x, \"%s\")\n",
	       typePtr, name);
#endif

    typePtr->name = util_strdup(name);
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeStatus --
 *
 *      Set the status of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeStatus(typePtr, status)
    Type       *typePtr;
    smi_status status;
{
#ifdef DEBUG
    printDebug(5, "setTypeStatus(0x%x(%s), %s)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"",
	       smiStringStatus(status));
#endif

    /* TODO: why this check? */
    if (typePtr->status == SMI_STATUS_UNKNOWN) {
	typePtr->status = status;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeDescription --
 *
 *      Set the description of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeDescription(typePtr, description)
    Type	   *typePtr;
    char	   *description;
{
#ifdef DEBUG
    printDebug(5, "setTypeDescription(0x%x(%s), %s)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"",
	       description);
#endif
    
    typePtr->description = util_strdup(description);
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeParent --
 *
 *      Set the parent of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeParent(typePtr, parent)
    Type	   *typePtr;
    const char	   *parent;
{
#ifdef DEBUG
    printDebug(5, "setTypeParent(0x%x(%s), %s)\n",
	       typePtr, typePtr->name ? typePtr->name : "", parent);
#endif
    
    if (!typePtr->parentType) {
	typePtr->parentType  = util_strdup(parent);
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeSequence --
 *
 *      Set the pointer to a struct list in case of a SEQUENCE type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeSequencePtr(typePtr, sequencePtr)
    Type	   *typePtr;
    struct List	   *sequencePtr;
{
#ifdef DEBUG
    printDebug(5, "setTypeSequencePtr(0x%x(%s), 0x%x)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"", sequencePtr);
#endif
    
    if (!typePtr->sequencePtr) {
	typePtr->sequencePtr  = sequencePtr;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeFormat --
 *
 *      Set the format (displayHint) of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeFormat(typePtr, format)
    Type           *typePtr;
    char	   *format;
{
#ifdef DEBUG
    printDebug(5, "setTypeFormat(0x%x(%s), %s)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"", format);
#endif
    
    typePtr->format = util_strdup(format);
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeUnits --
 *
 *      Set the units of a given Type. Note: units of types are only
 *	present in SMIng, not in SMIv2.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeUnits(typePtr, units)
    Type           *typePtr;
    char	   *units;
{
#ifdef DEBUG
    printDebug(5, "setTypeUnits(0x%x(%s), %s)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"", units);
#endif
    
    typePtr->units = util_strdup(units);
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeFileOffset --
 *
 *      Set the fileoffset of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeFileOffset(typePtr, fileoffset)
    Type     *typePtr;
    off_t    fileoffset;
{
#ifdef DEBUG
    printDebug(5, "setTypeFileOffset(0x%x(%s), %d)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"", fileoffset);
#endif

    /* TODO: why this check? */
    if (typePtr->fileoffset < 0) {
	typePtr->fileoffset = fileoffset;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeDecl --
 *
 *      Set the declaring macro of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
setTypeDecl(typePtr, decl)
    Type     *typePtr;
    smi_decl decl;
{
#ifdef DEBUG
    printDebug(5, "setTypeDecl(0x%x(%s), %s)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"",
	       smiStringDecl(decl));
#endif
    
    /* TODO: which this check? */
    if (typePtr->decl == SMI_DECL_UNKNOWN) {
	typePtr->decl = decl;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * addTypeFlags --
 *
 *      Add flags to the flags of a given Type.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
addTypeFlags(typePtr, flags)
    Type        *typePtr;
    TypeFlags   flags;
{
#ifdef DEBUG
    printDebug(5, "addTypeFlags(0x%x(%s), %d)\n",
	       typePtr, typePtr->name ? typePtr->name : "\"\"", flags);
#endif
    
    typePtr->flags |= flags;
}



/*
 *----------------------------------------------------------------------
 *
 * findTypeByName --
 *
 *      Lookup a Type by a given name.
 *
 * Results:
 *      A pointer to the Type structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Type *
findTypeByName(typename)
    const char *typename;
{
    Module *modulePtr;
    Type   *typePtr;
    
#ifdef DEBUG
    printDebug(6, "findTypeByName(%s)", typename);
#endif
    
    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->name) && !strcmp(typePtr->name, typename)) {
#ifdef DEBUG
		printDebug(4, " = 0x%x(%s)\n", typePtr, typePtr->name);
#endif
		return (typePtr);
	    }
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findTypeByModulenameAndName --
 *
 *      Lookup a Type by a given Module and name.
 *
 * Results:
 *      A pointer to the Type structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Type *
findTypeByModulenameAndName(modulename, typename)
    const char *modulename;
    const char *typename;
{
    Type       *typePtr;
    Module     *modulePtr;

#ifdef DEBUG
    printDebug(6, "findTypeByModulenameAndName(%s, %s)\n",
	       modulename, typename);
#endif

    modulePtr = findModuleByName(modulename);

    if (modulePtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->name) && !strcmp(typePtr->name, typename)) {
#ifdef DEBUG
		printDebug(6, "... = 0x%x(%s)\n", typePtr, typePtr->name);
#endif
		return (typePtr);
	    }
	}
    }
	
#ifdef DEBUG
    printDebug(6, "... = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findTypeByModuleAndName --
 *
 *      Lookup a Type by a given Module and name.
 *
 * Results:
 *      A pointer to the Type structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Type *
findTypeByModuleAndName(modulePtr, typename)
    Module      *modulePtr;
    const char  *typename;
{
    Type        *typePtr;

#ifdef DEBUG
    printDebug(6, "findTypeByModuleAndName(0x%x(%s), %s)",
	       modulePtr, modulePtr->name, typename);
#endif

    if (modulePtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->name) && !strcmp(typePtr->name, typename)) {
#ifdef DEBUG
		printDebug(6, "... = 0x%x(%s)\n", typePtr, typePtr->name);
#endif
		return (typePtr);
	    }
	}
    }

#ifdef DEBUG
    printDebug(6, " = NULL\n");
#endif
    return (NULL);
}



#if 0
/*
 *----------------------------------------------------------------------
 *
 * dumpTypes --
 *
 *      Dump Type hirarchie.
 *	For debugging purpose.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
dumpTypes()
{
    Type *t;
    Descriptor *d;

    for (d = firstDescriptor[KIND_TYPE]; d; d = d->nextSameKind) {
	if (d->module) {
	    fprintf(stderr, "%s!", d->module->descriptor->name);
	}
	for (t = d->ptr; t && t->syntax != SMI_SYNTAX_SEQUENCE;
	     t = t->parent) {
	    fprintf(stderr, "%s", t->descriptor && t->descriptor->name ? t->descriptor->name : "?");
	    if (t->syntax) {
		fprintf(stderr, "(%s)", smiStringSyntax(t->syntax));
	    }
	    fprintf(stderr, " <- ");
	}
	fprintf(stderr, "\n");
    }
    
}
#endif
 


/*
 *----------------------------------------------------------------------
 *
 * addMacro --
 *
 *      Create a new Macro structure.
 *
 * Results:
 *      A pointer to the new Macro structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *---------------------------------------------------------------------- */

Macro *
addMacro(macroname, fileoffset, flags, parserPtr)
    const char    *macroname;
    off_t	  fileoffset;
    MacroFlags	  flags;
    Parser	  *parserPtr;
    
{
    Macro	  *macroPtr;
    Module	  *modulePtr;
    
#ifdef DEBUG
    printDebug(5, "addMacro(%s, %d, %d, 0x%x)\n",
	       macroname, fileoffset, flags, parserPtr);
#endif

    modulePtr = parserPtr->modulePtr;
    
    /* TODO: Check wheather this macro already exists?? */

    macroPtr = (Macro *)util_malloc(sizeof(Macro));
    if (!macroPtr) {
	printError(parserPtr, ERR_ALLOCATING_MACRO, strerror(errno));
	return (NULL);
    }

	    
    macroPtr->modulePtr  = parserPtr->modulePtr;
    macroPtr->name	 = util_strdup(macroname);
    macroPtr->fileoffset = fileoffset;
    macroPtr->flags      = flags;
	    
    macroPtr->nextPtr				= NULL;
    macroPtr->prevPtr				= modulePtr->lastMacroPtr;
    if (!modulePtr->firstMacroPtr)
	modulePtr->firstMacroPtr		= macroPtr;
    if (modulePtr->lastMacroPtr)
	modulePtr->lastMacroPtr->nextPtr	= macroPtr;
    modulePtr->lastMacroPtr			= macroPtr;
    
    return (macroPtr);
}



/*
 *----------------------------------------------------------------------
 *
 * findMacroByModuleAndName --
 *
 *      Lookup a Macro by a given Module and name.
 *
 * Results:
 *      A pointer to the Macro structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Macro *
findMacroByModuleAndName(modulePtr, macroname)
    Module     *modulePtr;
    const char *macroname;
{
    Macro      *macroPtr;
    
#ifdef DEBUG
    printDebug(6, "findMacroByModuleAndName(0x%x(%s), %s)",
	       modulePtr, modulePtr->name, macroname);
#endif
    
    if (modulePtr) {
	for (macroPtr = modulePtr->firstMacroPtr; macroPtr;
	     macroPtr = macroPtr->nextPtr) {
	    if (!strcmp(macroPtr->name, macroname)) {
#ifdef DEBUG
		printDebug(6, "... = 0x%x(%s)\n", macroPtr, macroPtr->name);
#endif
		return (macroPtr);
	    }
	}
    }

#ifdef DEBUG
    printDebug(6, " = NULL\n");
#endif
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findMacroByModulenameAndName --
 *
 *      Lookup a Macro by a given Module and name.
 *
 * Results:
 *      A pointer to the Macro structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Macro *
findMacroByModulenameAndName(modulename, macroname)
    const char *modulename;
    const char *macroname;
{
    Module     *modulePtr;
    Macro      *macroPtr;

#ifdef DEBUG
    printDebug(6, "findMacroByModulenameAndName(%s, %s)\n",
	       modulename, macroname);
#endif

    modulePtr = findModuleByName(modulename);
	
    if (modulePtr) {
	for (macroPtr = modulePtr->firstMacroPtr; macroPtr;
	     macroPtr = macroPtr->nextPtr) {
	    if (!strcmp(macroPtr->name, macroname)) {
#ifdef DEBUG
		printDebug(6, "... = 0x%x(%s)\n", macroPtr, macroPtr->name);
#endif
		return (macroPtr);
	    }
	}
    }

    printDebug(6, "... = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * initData --
 *
 *      Initialize all need data structures at program start.
 *
 * Results:
 *      0 on success or -1 on an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
initData()
{
    Object	    *objectPtr;
    Parser	    parser;
    
    smiFlags = 0;
    
    firstLocationPtr = NULL;
    lastLocationPtr = NULL;
    firstModulePtr = NULL;
    lastModulePtr = NULL;
    firstViewPtr = NULL;
    lastViewPtr = NULL;
    
    /*
     * Initialize a root Node for the main MIB tree.
     */
    rootNodePtr = addNode(NULL, 0, NODE_FLAG_ROOT, NULL);
    
    /*
     * Initialize a root Node for pending (forward referenced) nodes.
     */
    pendingNodePtr = addNode(NULL, 0, NODE_FLAG_ROOT, NULL);
    
    /*
     * Initialize the top level well-known nodes, ccitt, iso, joint-iso-ccitt
     * belonging to a dummy module "". This is needed for SMIv1/v2. SMIng
     * defines it in a special SMIng module.
     */
    parser.path			= NULL;
    parser.locationPtr		= NULL;
    parser.flags		= smiFlags;
    parser.file			= NULL;
    parser.modulePtr = addModule("", "", NULL, 0, 0, NULL);

    addView("");

    objectPtr = addObject("ccitt", rootNodePtr, 0, 0, &parser);
    objectPtr = addObject("iso", rootNodePtr, 1, 0, &parser);
    objectPtr = addObject("joint-iso-ccitt", rootNodePtr, 2, 0, &parser);

    /*
     * Initialize the well-known ASN.1 Types for SMIv1/v2 and
     * the well-known SMIng Types.
     */
    typeIntegerPtr          = addType("INTEGER",
				      SMI_SYNTAX_INTEGER32, 0, &parser);
    typeOctetStringPtr      = addType("OCTET STRING",
				      SMI_SYNTAX_OCTETSTRING, 0, &parser);
    typeObjectIdentifierPtr = addType("OBJECT IDENTIFIER",
				      SMI_SYNTAX_OBJECTIDENTIFIER, 0, &parser);

    addType("OctetString", SMI_SYNTAX_OCTETSTRING, 0, NULL);
    addType("ObjectIdentifier", SMI_SYNTAX_OBJECTIDENTIFIER, 0, NULL);
    addType("TimeTicks", SMI_SYNTAX_TIMETICKS, 0, NULL);
    addType("Opaque", SMI_SYNTAX_OPAQUE, 0, NULL);
    addType("IpAddress", SMI_SYNTAX_IPADDRESS, 0, NULL);
    addType("Integer32", SMI_SYNTAX_INTEGER32, 0, NULL);
    addType("Unsigned32", SMI_SYNTAX_UNSIGNED32, 0, NULL);
    addType("Counter32", SMI_SYNTAX_COUNTER32, 0, NULL);
    addType("Gauge32", SMI_SYNTAX_GAUGE32, 0, NULL);
    addType("Integer64", SMI_SYNTAX_INTEGER64, 0, NULL);
    addType("Unsigned64", SMI_SYNTAX_UNSIGNED64, 0, NULL);
    addType("Counter64", SMI_SYNTAX_COUNTER64, 0, NULL);
    addType("Gauge64", SMI_SYNTAX_GAUGE64, 0, NULL);
    addType("Float32", SMI_SYNTAX_FLOAT32, 0, NULL);
    addType("Float64", SMI_SYNTAX_FLOAT64, 0, NULL);
    addType("Float128", SMI_SYNTAX_FLOAT128, 0, NULL);
    addType("Enum", SMI_SYNTAX_ENUM, 0, NULL);
    addType("Bits", SMI_SYNTAX_BITS, 0, NULL);

    return (0);
}



/*
 *----------------------------------------------------------------------
 *
 * loadModule --
 *
 *      Load a MIB module. It is search by the location list.
 *	If it is found in the location filesystem it is read completely.
 *	If it is found through an RPC location, just the module structure
 *	is read.
 *
 * Results:
 *      0 on success or -1 on an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Module *
loadModule(modulename)
    char	    *modulename;
{
    Location	    *locationPtr;
    Parser	    parser;
    Module	    *modulePtr;
    smi_module	    *smimodule;
    char	    s[200];
    char	    *path;
    struct stat	    buf;
    int		    st;
    
#ifdef DEBUG
    printDebug(3, "loadModule(%s)\n", modulename);
#endif

    if (!strlen(modulename)) {
	return NULL;
    }
    
    for (locationPtr = firstLocationPtr; locationPtr;
	 locationPtr = locationPtr->nextPtr) {

#ifdef BACKEND_RPC
	if (locationPtr->type == LOCATION_RPC) {
	    
	    smimodule = smiproc_module_1(&modulename, locationPtr->cl);
	    if (smimodule && strlen(smimodule->name)) {
	        /* the RPC server knows this module */
		modulePtr = addModule(modulename, "", locationPtr, 0, 0, NULL);
		setModuleLastUpdated(modulePtr, smimodule->lastupdated);
		setModuleOrganization(modulePtr, smimodule->organization);
		setModuleContactInfo(modulePtr, smimodule->contactinfo);
		/* TODO: setModuleIdentityObject */
		/* TODO: setObjectDescription */
		/* TODO: setObjectReference */
		return modulePtr;
	    }
	}
#endif

#ifdef BACKEND_SMI
	if (locationPtr->type == LOCATION_SMIDIR) {

	    path = malloc(strlen(locationPtr->name)+strlen(modulename)+6);
	    
	    sprintf(path, "%s/%s", locationPtr->name, modulename);
	    if ((st = stat(path, &buf))) {
		sprintf(path, "%s/%s.my", locationPtr->name, modulename);
		st = stat(path, &buf);
	    }
	    if (!st) {
		parser.path			= util_strdup(path);
		parser.locationPtr		= locationPtr;
		parser.flags			= smiFlags;
		parser.modulePtr		= NULL;
		parser.file			= fopen(parser.path, "r");
		if (!parser.file) {
		    printError(NULL, ERR_OPENING_INPUTFILE, parser.path,
			       strerror(errno));
		} else {
		    if (enterLexRecursion(parser.file) < 0) {
			printError(&parser, ERR_MAX_LEX_DEPTH);
			fclose(parser.file);
		    }
		    parser.line			= 1;
		    parser.column		= 1;
		    parser.character		= 1;
		    yyparse((void *)&parser);
		    leaveLexRecursion();
		    fclose(parser.file);
		    if (parser.flags & FLAG_STATS) {
			sprintf(s, " (%d lines)", parser.line-1);
			printError(&parser, ERR_STATISTICS, s);
		    }
		}

		free(path);

		return findModuleByName(modulename);
		
	    }
	    
	} else if (locationPtr->type == LOCATION_SMIFILE) {

	    /* TODO */
	    modulePtr = findModuleByName(modulename);
	    if (modulePtr) {
		return (modulePtr);
	    }
	}
#endif
    }

    return NULL;
}

