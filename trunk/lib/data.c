/*
 * data.c --
 *
 *      Operations on the main data structures.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: data.c,v 1.70 2000/02/24 10:35:38 strauss Exp $
 */

#include <config.h>

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "error.h"
#include "util.h"
#include "data.h"
#include "smi.h"

#ifdef BACKEND_SMI
#include "scanner-smi.h"
extern int smiparse();
#endif

#ifdef BACKEND_SMING
#include "scanner-sming.h"
extern int smingparse();
#endif



#define stringKind(kind) ( \
	(kind == KIND_ANY)                 ? "ANY" : \
	(kind == KIND_MODULE)              ? "MODULE" : \
	(kind == KIND_MACRO)               ? "MACRO" : \
	(kind == KIND_TYPE)                ? "TYPE" : \
	(kind == KIND_OBJECT)              ? "OBJECT" : \
	(kind == KIND_IMPORT)              ? "IMPORT" : \
					     "unknown" )



View	        *firstViewPtr, *lastViewPtr;
Module          *firstModulePtr, *lastModulePtr;
Node		*rootNodePtr;
Node		*pendingNodePtr;
Type		*typeOctetStringPtr, *typeObjectIdentifierPtr,
		*typeInteger32Ptr, *typeUnsigned32Ptr,
		*typeInteger64Ptr, *typeUnsigned64Ptr,
		*typeFloat32Ptr, *typeFloat64Ptr,
		*typeFloat128Ptr,
		*typeEnumPtr, *typeBitsPtr;
int		smiFlags;
char		*smiPath;
int		smiDepth = 0;


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

    viewPtr = (View *)util_malloc(sizeof(View));

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
 * isInView --
 *
 *      Check, whether a given module is in the current view.
 *
 * Results:
 *      != 0 if in view, 0 otherwise.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
isInView(modulename)
    const char	      *modulename;
{
    View	      *viewPtr;

    if (smiFlags & SMI_FLAG_VIEWALL) {
	return 1;
    }
    
    for (viewPtr = firstViewPtr; viewPtr; viewPtr = viewPtr->nextPtr) {
	if (!strcmp(modulename, viewPtr->name)) {
	    return 1;
	}
    }
    return 0;
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
addModule(modulename, path, flags, parserPtr)
    char	      *modulename;
    char	      *path;
    ModuleFlags	      flags;
    Parser	      *parserPtr;
{
    Module	      *modulePtr;

    modulePtr = (Module *)util_malloc(sizeof(Module));

    modulePtr->export.name			= modulename;
    modulePtr->export.path			= path;
    modulePtr->export.language			= SMI_LANGUAGE_UNKNOWN;
    modulePtr->export.organization		= NULL;
    modulePtr->export.contactinfo		= NULL;
    modulePtr->export.description		= NULL;
    modulePtr->export.reference			= NULL;

    modulePtr->lastUpdated			= 0;
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
    modulePtr->export.organization = organization;
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
setModuleContactInfo(modulePtr, contactinfo)
    Module	  *modulePtr;
    char	  *contactinfo;
{
    modulePtr->export.contactinfo = contactinfo;
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleDescription --
 *
 *      Set the description string of a given Module.
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
setModuleDescription(modulePtr, description, parserPtr)
    Module *modulePtr;
    char *description;
    Parser *parserPtr;
{
    if (modulePtr->export.description)
	util_free(modulePtr->export.description);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(description);
	modulePtr->export.description = NULL;
    } else {
	modulePtr->export.description = description;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleReference --
 *
 *      Set the reference string of a given Module.
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
setModuleReference(modulePtr, reference, parserPtr)
    Module *modulePtr;
    char *reference;
    Parser *parserPtr;
{
    if (modulePtr->export.reference)
	util_free(modulePtr->export.reference);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(reference);
	modulePtr->export.reference = NULL;
    } else {
	modulePtr->export.reference = reference;
    }
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

    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	if ((modulePtr->export.name) &&
	    !strcmp(modulePtr->export.name, modulename)) {
	    return (modulePtr);
	}
    }

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
    char	  *description;
    Parser	  *parserPtr;
{
    Revision	  *revisionPtr;
    Module	  *modulePtr;

    revisionPtr = (Revision *)util_malloc(sizeof(Revision));

    modulePtr = parserPtr->modulePtr;

    revisionPtr->modulePtr		 = modulePtr;
    revisionPtr->export.date	       	 = date;
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(description);
	revisionPtr->export.description	 = NULL;
    } else {
	revisionPtr->export.description	 = description;
    }
    revisionPtr->line			 = parserPtr ? parserPtr->line : -1;

    /* TODO: probably, we should sort revisions by date by inserting
       new ones at the right position!? */
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

    importPtr = (Import *)util_malloc(sizeof(Import));

    modulePtr = parserPtr->modulePtr;

    importPtr->modulePtr		 = modulePtr;
    importPtr->export.name       	 = util_strdup(name);
    importPtr->export.module		 = NULL; /* not yet known */
    importPtr->kind			 = KIND_UNKNOWN; /* not yet known */
    importPtr->use			 = 0;
    importPtr->line			 = parserPtr ? parserPtr->line : -1;
    
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
 * setImportModulename --
 *
 *      Set the modulename part of a given Import struct.
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
setImportModulename(importPtr, modulename)
    Import    *importPtr;
    char      *modulename;
{
    importPtr->export.module = modulename;
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
checkImports(modulePtr, parserPtr)
    Module      *modulePtr;
    Parser	*parserPtr;
{
    int         n = 0;
    Import      *importPtr;
    SmiNode	*smiNode;
    SmiType	*smiType;
    SmiMacro	*smiMacro;
    
    for (importPtr = parserPtr->modulePtr->firstImportPtr;
	 importPtr; importPtr = importPtr->nextPtr) {

	if (importPtr->kind == KIND_UNKNOWN) {
	    if (modulePtr) {
		if ((smiNode = smiGetNode(&modulePtr->export,
					  importPtr->export.name))) {
		    importPtr->export.module =
			util_strdup(modulePtr->export.name);
		    importPtr->kind	= KIND_OBJECT;
		} else if ((smiType = smiGetType(&modulePtr->export,
						 importPtr->export.name))) {
		    importPtr->export.module =
			util_strdup(modulePtr->export.name);
		    importPtr->kind	= KIND_TYPE;
		} else if ((smiMacro = smiGetMacro(&modulePtr->export,
						   importPtr->export.name))) {
		    importPtr->export.module =
			util_strdup(modulePtr->export.name);
		    importPtr->kind = KIND_MACRO;
		} else {
		    n++;
		    importPtr->export.module =
			util_strdup(modulePtr->export.name);
		    printError(parserPtr, ERR_IDENTIFIER_NOT_IN_MODULE,
			       importPtr->export.name, modulePtr->export.name);
		    importPtr->kind   = KIND_NOTFOUND;
		}
	    } else {
		    n++;
		    importPtr->export.module = util_strdup("");
		    importPtr->kind   = KIND_NOTFOUND;
	    }
	}
    }

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
findImportByName(name, modulePtr)
    const char     *name;
    Module	   *modulePtr;
{
    Import           *importPtr;

    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if (!strcmp(importPtr->export.name, name)) {
		return (importPtr);
	}
    }

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
findImportByModulenameAndName(modulename, name, modulePtr)
    const char     *modulename;
    const char     *name;
    Module	   *modulePtr;
{
    Import           *importPtr;

    for (importPtr = modulePtr->firstImportPtr; importPtr;
	 importPtr = importPtr->nextPtr) {
	if ((!strcmp(importPtr->export.name, name)) &&
	    (!strcmp(importPtr->export.module, modulename))) {
	    return (importPtr);
	}
    }

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
    char	     *objectname;
    Node             *parentNodePtr;
    SmiSubid	     subid;
    ObjectFlags	     flags;
    Parser	     *parserPtr;
{
    Object	     *objectPtr;
    Node	     *nodePtr;
    Module	     *modulePtr;

    objectPtr = (Object *)util_malloc(sizeof(Object));

    modulePtr = parserPtr ? parserPtr->modulePtr : NULL;
    
    objectPtr->export.name		= util_strdup(objectname);
    objectPtr->export.decl		= SMI_DECL_UNKNOWN;
    objectPtr->export.access		= SMI_ACCESS_UNKNOWN;
    objectPtr->export.status		= SMI_STATUS_UNKNOWN;
    objectPtr->export.format		= NULL;
    objectPtr->export.value.basetype	= SMI_BASETYPE_UNKNOWN;
    objectPtr->export.units		= NULL;
    objectPtr->export.description	= NULL;
    objectPtr->export.reference		= NULL;
    objectPtr->export.indexkind		= SMI_INDEX_UNKNOWN;
    objectPtr->export.implied		= 0;
    objectPtr->export.create		= 0;
    objectPtr->export.nodekind		= SMI_NODEKIND_UNKNOWN;
					
    objectPtr->modulePtr		= modulePtr;
    objectPtr->nodePtr			= NULL;
    objectPtr->prevSameNodePtr		= NULL;
    objectPtr->nextSameNodePtr		= NULL;
    objectPtr->typePtr			= NULL;
    objectPtr->listPtr			= NULL;
    objectPtr->flags			= flags;
    objectPtr->line			= parserPtr ? parserPtr->line : -1;
    					
    objectPtr->export.oidlen            = 0;     /* filled in by  */
    objectPtr->export.oid               = NULL;  /* second pass.  */
    
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
    
    objectPtr = (Object *)util_malloc(sizeof(Object));

    modulePtr = parserPtr->modulePtr;
    nodePtr   = templatePtr->nodePtr;
    
    objectPtr->export.name			= NULL;
    objectPtr->export.decl			= SMI_DECL_UNKNOWN;
    objectPtr->export.access			= SMI_ACCESS_UNKNOWN;
    objectPtr->export.status			= SMI_STATUS_UNKNOWN;
    objectPtr->export.format			= NULL;
    objectPtr->export.value.basetype		= SMI_BASETYPE_UNKNOWN;
    objectPtr->export.units			= NULL;
    objectPtr->export.description		= NULL;
    objectPtr->export.reference			= NULL;
    objectPtr->export.indexkind			= SMI_INDEX_UNKNOWN;
    objectPtr->export.implied			= 0;
    objectPtr->export.create			= 0;
    objectPtr->export.nodekind			= SMI_NODEKIND_UNKNOWN;
						
    objectPtr->modulePtr		        = modulePtr;
    objectPtr->nodePtr				= nodePtr;
    objectPtr->prevSameNodePtr			= NULL;
    objectPtr->nextSameNodePtr			= NULL;
    objectPtr->typePtr				= NULL;
    objectPtr->listPtr				= NULL;
    objectPtr->flags				= flags;
    objectPtr->line				= parserPtr ? parserPtr->line : -1;

    objectPtr->export.oidlen                    = 0;     /* filled in by  */
    objectPtr->export.oid                       = NULL;  /* second pass.  */
    
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
    SmiSubid	    subid;
    NodeFlags	    flags;
    Parser	    *parserPtr;
{
    Node	    *nodePtr;
    Node	    *c;

    nodePtr = (Node *)util_malloc(sizeof(Node));
    
    nodePtr->flags				= flags;
    nodePtr->subid				= subid;
    nodePtr->parentPtr				= parentNodePtr;
    nodePtr->firstChildPtr			= NULL;
    nodePtr->lastChildPtr			= NULL;
    nodePtr->firstObjectPtr			= NULL;
    nodePtr->lastObjectPtr			= NULL;

    /*
     * this cannot be set in all situations (pending sub trees).
     * we delay it to the second pass.
     */
    nodePtr->oidlen                             = 0;
    nodePtr->oid				= NULL;

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

Node *createNodes(unsigned int oidlen, SmiSubid *oid)
{
    Node	 *parentNodePtr, *nodePtr;
    unsigned int i;

    parentNodePtr = rootNodePtr;

    for(i = 0; i < oidlen; i++) {
	if (!(nodePtr = findNodeByParentAndSubid(parentNodePtr, oid[i]))) {
	    nodePtr = addNode(parentNodePtr, oid[i], 0, NULL);
	}
	parentNodePtr = nodePtr;
    }

    return parentNodePtr;    
}



/*
 *----------------------------------------------------------------------
 *
 * createNodesByOidString --
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
createNodesByOidString(oid)
    const char		*oid;
{
    char		*p, *elements;
    Node		*parentNodePtr, *nodePtr;
    SmiSubid		subid;

    parentNodePtr = rootNodePtr;
    elements = util_strdup(oid);

    p = strtok(elements, ".");
    do {
	subid = (unsigned int)strtoul(p, NULL, 0);
	if (!(nodePtr = findNodeByParentAndSubid(parentNodePtr,
						       subid))) {
	    nodePtr = addNode(parentNodePtr, subid, 0, NULL);
	}
	parentNodePtr = nodePtr;
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
 * mergeNodeTrees --
 *
 *      Merge the subtree rooted at `from' into the `to' tree recursively
 *      and release the `from' tree.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void mergeNodeTrees(Node *toNodePtr, Node *fromNodePtr)
{
    Node	      *nodePtr, *toChildPtr, *nextPtr;
    Object	      *objectPtr;

    /* (1) merge lists of Objects for this node */
    if (fromNodePtr->firstObjectPtr) {
	if (!toNodePtr->firstObjectPtr) {
	    toNodePtr->firstObjectPtr = fromNodePtr->firstObjectPtr;
	    toNodePtr->lastObjectPtr = fromNodePtr->lastObjectPtr;
	} else {
	    fromNodePtr->firstObjectPtr->prevSameNodePtr =
		toNodePtr->lastObjectPtr;
	    toNodePtr->lastObjectPtr->nextSameNodePtr =
		fromNodePtr->firstObjectPtr;
	    toNodePtr->lastObjectPtr = fromNodePtr->lastObjectPtr;
	}
    }
    for (objectPtr = fromNodePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextSameNodePtr) {
	objectPtr->nodePtr = toNodePtr;

    }
    
    /* (2) loop: merge all first-level `from' sub-trees to `to' */
    /* adjust all `from' sub-nodes' parentPtrs */
    for (nodePtr = fromNodePtr->firstChildPtr; nodePtr;
	 nodePtr = nodePtr->nextPtr) {
	nodePtr->parentPtr = toNodePtr;
    }
    if (!toNodePtr->firstChildPtr) {
	/*
	 * if `to' has no sub-nodes, just move the `from' sub-nodes.
	 */
	toNodePtr->firstChildPtr = fromNodePtr->firstChildPtr;
	toNodePtr->lastChildPtr = fromNodePtr->lastChildPtr;
    } else {
	/*
	 * otherwise, we really have to merge both trees...
	 */
	for (nodePtr = fromNodePtr->firstChildPtr; nodePtr; ) {
	    nextPtr = nodePtr->nextPtr;
	    if ((toChildPtr = findNodeByParentAndSubid(toNodePtr,
						       nodePtr->subid))) {
		/*
		 * if a sub-node with the same subid is already present
		 * in `to', merge them recursively.
		 */
		mergeNodeTrees(toChildPtr, nodePtr);
	    } else {
		/*
		 * otherwise, move the sub-tree from `from' to `to'.
		 */
		if (nodePtr->subid < toNodePtr->firstChildPtr->subid) {
		    /* move to the head. */
		    nodePtr->nextPtr = toNodePtr->firstChildPtr;
		    toNodePtr->firstChildPtr = nodePtr;
		} else if (nodePtr->subid > toNodePtr->lastChildPtr->subid) {
		    /* move to the end. */
		    nodePtr->prevPtr = toNodePtr->lastChildPtr;
		    toNodePtr->lastChildPtr->nextPtr = nodePtr;
		    toNodePtr->lastChildPtr = nodePtr;
		} else {
		    /* move to the appropriate place in the `to' list. */
		    for (toChildPtr = toNodePtr->firstChildPtr;
			 toChildPtr->nextPtr->subid < nodePtr->subid;
			 toChildPtr = toChildPtr->nextPtr);
		    toChildPtr->nextPtr->prevPtr = nodePtr;
		    nodePtr->nextPtr = toChildPtr->nextPtr;
		    nodePtr->prevPtr = toChildPtr;
		    toChildPtr->nextPtr = nodePtr;
		}
	    }
	    nodePtr = nextPtr;
	}
    }

    util_free(fromNodePtr);
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectName --
 *
 *      Set the name of a given Object. Combine two Objects if the name
 *	already exists.
 *
 * Results:
 *	(Object *) of the potentially combined object.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Object *
setObjectName(objectPtr, name)
    Object	      *objectPtr;
    char	      *name;
{
    Node	      *nodePtr, *nextPtr;
    Module	      *modulePtr;
    Object	      *newObjectPtr;

    if (objectPtr->export.name) {
	util_free(objectPtr->export.name);
    }
    objectPtr->export.name = name;

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

	if (!strcmp(nodePtr->firstObjectPtr->export.name, name)) {

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

	    objectPtr->nodePtr->firstObjectPtr = NULL;
	    objectPtr->nodePtr->lastObjectPtr = NULL;
	    
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

	    mergeNodeTrees(objectPtr->nodePtr, nodePtr);
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
 *      Set the type (pointer to a Type struct) of a given Object.
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
    objectPtr->typePtr = typePtr;
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
    SmiAccess		   access;
{
    objectPtr->export.access = access;
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
    SmiStatus		   status;
{
    objectPtr->export.status = status;
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
setObjectDescription(objectPtr, description, parserPtr)
    Object    *objectPtr;
    char      *description;
    Parser    *parserPtr;
{
    if (objectPtr->export.description)
	util_free(objectPtr->export.description);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(description);
	objectPtr->export.description = NULL;
    } else {
	objectPtr->export.description = description;
    }
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
setObjectReference(objectPtr, reference, parserPtr)
    Object    *objectPtr;
    char      *reference;
    Parser    *parserPtr;
{
    if (objectPtr->export.reference)
	util_free(objectPtr->export.reference);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(reference);
	objectPtr->export.reference = NULL;
    } else {
	objectPtr->export.reference = reference;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectFormat --
 *
 *      Set the format of a given Object.
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
setObjectFormat(objectPtr, format)
    Object    *objectPtr;
    char      *format;
{
    if (objectPtr->export.format) util_free(objectPtr->export.format);
    objectPtr->export.format = format;
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
    if (objectPtr->export.units) util_free(objectPtr->export.units);
    objectPtr->export.units = units;
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
    SmiDecl     decl;
{
    objectPtr->export.decl = decl;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectNodekind --
 *
 *      Set the language independant SmiNodekind of a given Object.
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
setObjectNodekind(objectPtr, nodekind)
    Object	*objectPtr;
    SmiNodekind nodekind;
{
    objectPtr->export.nodekind = nodekind;
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
    objectPtr->flags |= flags;
}



/*
 *----------------------------------------------------------------------
 *
 * deleteObjectFlags --
 *
 *      Delete flags from the flags of a given Object.
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
deleteObjectFlags(objectPtr, flags)
    Object	   *objectPtr;
    ObjectFlags    flags;
{
    objectPtr->flags &= ~flags;
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

/* TODO remove me
void
setObjectIndex(objectPtr, indexPtr)
    Object	 *objectPtr;
    Index	 *indexPtr;
{
    objectPtr->indexPtr = indexPtr;
}
*/


/*
 *----------------------------------------------------------------------
 *
 * setObjectList --
 *
 *      Set the list of objects of a notification type or object group
 *	or the list of notifications of a notification group.
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
setObjectList(objectPtr, listPtr)
    Object	 *objectPtr;
    List	 *listPtr;
{
    objectPtr->listPtr = listPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectRelated --
 *
 *      Set the related object of a given object (e.g. SMIv2 AUGMENTS)
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
setObjectRelated(objectPtr, relatedPtr)
    Object	 *objectPtr;
    Object	 *relatedPtr;
{
    objectPtr->relatedPtr = relatedPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectImplied --
 *
 *      Set the implied flag of a given object
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
setObjectImplied(objectPtr, implied)
    Object	 *objectPtr;
    int		 implied;
{
    objectPtr->export.implied = implied;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectCreate --
 *
 *      Set the create flag of a given (table entry) object
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
setObjectCreate(objectPtr, create)
    Object	 *objectPtr;
    int		 create;
{
    objectPtr->export.create = create;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectIndexkind --
 *
 *      Set the indexkind of a given (table entry) object
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
setObjectIndexkind(objectPtr, indexkind)
    Object	 *objectPtr;
    SmiIndexkind indexkind;
{
    objectPtr->export.indexkind = indexkind;
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectValue --
 *
 *      Set the default value pointer of a given Object.
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
setObjectValue(objectPtr, valuePtr)
    Object	 *objectPtr;
    SmiValue	 *valuePtr;
{
    objectPtr->export.value = *valuePtr;
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
    
    if (parentNodePtr) {
	for (nodePtr = parentNodePtr->firstChildPtr; nodePtr;
	     nodePtr = nodePtr->nextPtr) {
	    if (nodePtr->subid == subid) {
		return (nodePtr);
	    }
	}
    }
    
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findNodeByOidString --
 *
 *      Lookup a Node by a given string of concatinated numerical subids.
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
findNodeByOidString(oid)
    char            *oid;
{
    Node *nodePtr;
    char *s;
    char *p;
    
    s = util_strdup(oid);
    nodePtr = rootNodePtr;
    for(p = strtok(s, ". "); p && nodePtr; p = strtok(NULL, ". ")) {
	nodePtr = findNodeByParentAndSubid(nodePtr, atoi(p));
    }
    
    free(s);
    return (nodePtr);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByNode --
 *
 *      Lookup an Object by a given Node. Note, that there might be
 *	multiple definitions for one node.
 *
 * Results:
 *      A pointer to the first Object structure in the current View or
 *	a pointer to the first Object if none is in the current View or
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
    Object    *goodObjectPtr = NULL;

    /* first, try to find an object in the current view. */
    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	 objectPtr = objectPtr->nextSameNodePtr) {
	if (isInView(objectPtr->modulePtr->export.name)) {
	    if (! goodObjectPtr) {
		goodObjectPtr = objectPtr;
	    } else if (objectPtr->modulePtr->export.language
		       > goodObjectPtr->modulePtr->export.language) {
		goodObjectPtr = objectPtr;
	    }
	}
    }

    return goodObjectPtr ? goodObjectPtr : nodePtr->firstObjectPtr;
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

    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	 objectPtr = objectPtr->nextSameNodePtr) {
	if (objectPtr->modulePtr == modulePtr) {
	    return (objectPtr);
	}
    }

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

    for (objectPtr = nodePtr->firstObjectPtr; objectPtr;
	 objectPtr = objectPtr->nextSameNodePtr) {
	if (!strcmp(objectPtr->modulePtr->export.name, modulename)) {
	    return (objectPtr);
	}
    }

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

    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->export.name) &&
		!strcmp(objectPtr->export.name, objectname)) {
		/*
		 * We return the first matching object.
		 * TODO: probably we should check if there are more matching
		 *       objects, and give a warning if there's another one.
		 */
		return (objectPtr);
	    }
	}
    }

    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findNextObjectByName --
 *
 *      Lookup the next Object by a given name. Note, that
 *	there might be more than one Object with the same name.
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
findNextObjectByName(objectname, prevObjectPtr)
    const char       *objectname;
    Object	     *prevObjectPtr;
{
    Module	     *modulePtr;
    Object           *objectPtr;

    for (modulePtr = prevObjectPtr->modulePtr->nextPtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->export.name)
		&& !strcmp(objectPtr->export.name, objectname)) {
		/*
		 * We return the first matching object.
		 * TODO: probably we should check if there are more matching
		 *       objects, and give a warning if there's another one.
		 */
		return (objectPtr);
	    }
	}
    }

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

    modulePtr = findModuleByName(modulename);

    if (modulePtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->export.name) &&
		!strcmp(objectPtr->export.name, objectname)) {
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
    
    if (modulePtr) {
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = objectPtr->nextPtr) {
	    if ((objectPtr->export.name) &&
		!strcmp(objectPtr->export.name, objectname)) {
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
    
    return (NULL);
}



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
 *----------------------------------------------------------------------
 */

Type *
addType(typename, basetype, flags, parserPtr)
    const char     *typename;
    SmiBasetype	   basetype;
    TypeFlags      flags;
    Parser	   *parserPtr;
{
    Type	   *typePtr;
    Module	   *modulePtr;
    
    modulePtr = parserPtr ? parserPtr->modulePtr : NULL;
    
    typePtr = util_malloc(sizeof(Type));

    if (typename) {
	typePtr->export.name	        = util_strdup(typename);
    } else {
	typePtr->export.name		= NULL;
    }
    typePtr->export.basetype		= basetype;
    typePtr->export.decl		= SMI_DECL_UNKNOWN;
    typePtr->export.format		= NULL;
    typePtr->export.value.basetype	= SMI_BASETYPE_UNKNOWN;
    typePtr->export.units		= NULL;
    typePtr->export.status		= SMI_STATUS_UNKNOWN;
    typePtr->export.description		= NULL;
    typePtr->export.reference		= NULL;

    typePtr->modulePtr			= modulePtr;
    typePtr->listPtr			= NULL;
    typePtr->flags			= flags;
    typePtr->parentPtr                  = NULL;
    typePtr->line			= parserPtr ? parserPtr->line : -1;
    
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
    
    typePtr = (Type *)util_malloc(sizeof(Type));

    modulePtr = parserPtr->modulePtr;
    
    typePtr->export.name	        = NULL;
    typePtr->export.basetype		= templatePtr->export.basetype;
    typePtr->export.decl		= SMI_DECL_IMPLICIT_TYPE;
    typePtr->export.format		= NULL;
    typePtr->export.value.basetype	= SMI_BASETYPE_UNKNOWN;
    typePtr->export.units		= NULL;
    typePtr->export.status		= templatePtr->export.status;
    typePtr->export.description		= NULL;
    typePtr->export.reference		= NULL;

    typePtr->modulePtr			= modulePtr;
    typePtr->listPtr			= NULL;
    typePtr->flags			= templatePtr->flags;
    typePtr->parentPtr                  = templatePtr;
    typePtr->line			= parserPtr ? parserPtr->line : -1;

    typePtr->nextPtr			= NULL;
    typePtr->prevPtr			= modulePtr->lastTypePtr;
    if (!modulePtr->firstTypePtr)
	modulePtr->firstTypePtr		= typePtr;
    if (modulePtr->lastTypePtr)
	modulePtr->lastTypePtr->nextPtr	= typePtr;
    modulePtr->lastTypePtr		= typePtr;
    
    return (typePtr);
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeName --
 *
 *      Set the name of a given Type. If it already exists, merge the
 *	two types.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Type *
setTypeName(typePtr, name)
    Type	      *typePtr;
    char	      *name;
{
    Type              *type2Ptr;

    if (typePtr->export.name) {
	util_free(typePtr->export.name);
    }
    
    if (!name) {
	typePtr->export.name = NULL;
	return typePtr;
    }
	
    typePtr->export.name = util_strdup(name);

    /*
     * If a type with this name already exists, it must be a forward
     * reference and both types have to be merged.
     */
    for (type2Ptr = typePtr->modulePtr->firstTypePtr; type2Ptr;
	 type2Ptr = type2Ptr->nextPtr) {

	if (type2Ptr->export.name &&
	    (!strcmp(type2Ptr->export.name, name)) &&
	    (type2Ptr != typePtr)) {

	    /*
	     * remove typePtr from the type list.
	     */
	    if (typePtr->prevPtr) {
		typePtr->prevPtr->nextPtr = typePtr->nextPtr;
	    } else {
		typePtr->modulePtr->firstTypePtr = typePtr->nextPtr;
	    }
	    if (typePtr->nextPtr) {
		typePtr->nextPtr->prevPtr = typePtr->prevPtr;
	    } else {
		typePtr->modulePtr->lastTypePtr = typePtr->prevPtr;
	    }

	    type2Ptr->export.basetype     = typePtr->export.basetype;
	    type2Ptr->export.decl         = typePtr->export.decl;
	    type2Ptr->export.format       = typePtr->export.format;
	    type2Ptr->export.value        = typePtr->export.value;
	    type2Ptr->export.units        = typePtr->export.units;
	    type2Ptr->export.status       = typePtr->export.status;
	    type2Ptr->export.description  = typePtr->export.description;
	    type2Ptr->export.reference    = typePtr->export.reference;
	    
	    type2Ptr->parentPtr    = typePtr->parentPtr;
	    type2Ptr->listPtr      = typePtr->listPtr;
	    type2Ptr->flags        = typePtr->flags;
	    type2Ptr->line         = typePtr->line;

	    util_free(typePtr->export.name);
	    util_free(typePtr);

	    return type2Ptr;
	}
    }
    return typePtr;
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
    SmiStatus  status;
{
    typePtr->export.status = status;
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeBasetype --
 *
 *      Set the basetype of a given Type.
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
setTypeBasetype(typePtr, basetype)
    Type       *typePtr;
    SmiBasetype  basetype;
{
    typePtr->export.basetype = basetype;
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
setTypeDescription(typePtr, description, parserPtr)
    Type	   *typePtr;
    char	   *description;
    Parser         *parserPtr;
{
    if (typePtr->export.description)
	util_free(typePtr->export.description);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(description);
	typePtr->export.description = NULL;
    } else {
	typePtr->export.description = description;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeReference --
 *
 *      Set the reference of a given Type.
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
setTypeReference(typePtr, reference, parserPtr)
    Type	   *typePtr;
    char	   *reference;
    Parser         *parserPtr;
{
    if (typePtr->export.reference)
	util_free(typePtr->export.reference);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(reference);
	typePtr->export.reference = NULL;
    } else {
	typePtr->export.reference = reference;
    }
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
setTypeParent(Type *typePtr, Type *parentPtr)
{
    typePtr->parentPtr = parentPtr;
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeList --
 *
 *      Set the pointer to a struct list. This used for
 *	- columns of a SEQUENCE type,
 *	- enumeration items of an enumeration integer type,
 *	- min-max pair items of a range restricted type,
 *	- min-max pars items of a size restricted type.
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
setTypeList(typePtr, listPtr)
    Type	   *typePtr;
    struct List	   *listPtr;
{
    if (!typePtr->listPtr) {
	typePtr->listPtr  = listPtr;
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
    if (typePtr->export.format) util_free(typePtr->export.format);
    typePtr->export.format = format;
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
    if (typePtr->export.units) util_free(typePtr->export.units);
    typePtr->export.units = units;
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
    SmiDecl  decl;
{
    typePtr->export.decl = decl;
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeValue --
 *
 *      Set the default value pointer of a given Type.
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
setTypeValue(typePtr, valuePtr)
    Type	 *typePtr;
    SmiValue	 *valuePtr;
{
    typePtr->export.value = *valuePtr;
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
    typePtr->flags |= flags;
}



/*
 *----------------------------------------------------------------------
 *
 * deleteTypeFlags --
 *
 *      Delete flags from the flags of a given Type.
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
deleteTypeFlags(typePtr, flags)
    Type        *typePtr;
    TypeFlags   flags;
{
    typePtr->flags &= ~flags;
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
    
    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->export.name) &&
		!strcmp(typePtr->export.name, typename)) {
		return (typePtr);
	    }
	}
    }

    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findNextTypeByName --
 *
 *      Lookup the next Type by a given name.
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
findNextTypeByName(typename, prevTypePtr)
    const char *typename;
    Type       *prevTypePtr;
{
    Module *modulePtr;
    Type   *typePtr;
    
    for (modulePtr = prevTypePtr->modulePtr->nextPtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->export.name) &&
		!strcmp(typePtr->export.name, typename)) {
		return (typePtr);
	    }
	}
    }

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

    modulePtr = findModuleByName(modulename);

    if (modulePtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->export.name) && !strcmp(typePtr->export.name, typename)) {
		return (typePtr);
	    }
	}
    }
	
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

    if (modulePtr) {
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = typePtr->nextPtr) {
	    if ((typePtr->export.name) &&
		!strcmp(typePtr->export.name, typename)) {
		return (typePtr);
	    }
	}
    }

    return (NULL);
}



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
addMacro(macroname, flags, parserPtr)
    const char    *macroname;
    MacroFlags	  flags;
    Parser	  *parserPtr;
    
{
    Macro	  *macroPtr;
    Module	  *modulePtr;
    
    modulePtr = parserPtr->modulePtr;
    
    /* TODO: Check wheather this macro already exists?? */

    macroPtr = (Macro *)util_malloc(sizeof(Macro));
	    
    macroPtr->export.name 	 = util_strdup(macroname);
    macroPtr->export.status      = SMI_STATUS_UNKNOWN;
    macroPtr->export.description = NULL;
    macroPtr->export.reference   = NULL;

    macroPtr->modulePtr   	 = parserPtr->modulePtr;
    macroPtr->flags       	 = flags;
    macroPtr->line	  	 = parserPtr ? parserPtr->line : -1;
    
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
 * setMacroStatus --
 *
 *      Set the status of a given Macro.
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
setMacroStatus(macroPtr, status)
    Macro      *macroPtr;
    SmiStatus  status;
{
    macroPtr->export.status = status;
}



/*
 *----------------------------------------------------------------------
 *
 * setMacroDescription --
 *
 *      Set the description of a given Macro.
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
setMacroDescription(macroPtr, description, parserPtr)
    Macro	   *macroPtr;
    char	   *description;
    Parser	   *parserPtr;
{
    if (macroPtr->export.description) util_free(macroPtr->export.description);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(description);
	macroPtr->export.description = NULL;
    } else {
	macroPtr->export.description = description;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setMacroReference --
 *
 *      Set the reference of a given Macro.
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
setMacroReference(macroPtr, reference, parserPtr)
    Macro	   *macroPtr;
    char	   *reference;
    Parser         *parserPtr;
{
    if (macroPtr->export.reference)
	util_free(macroPtr->export.reference);
    if (parserPtr->flags & SMI_FLAG_NODESCR) {
	util_free(reference);
	macroPtr->export.reference = NULL;
    } else {
	macroPtr->export.reference = reference;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setMacroDecl --
 *
 *      Set the declaring macro of a given Macro.
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
setMacroDecl(macroPtr, decl)
    Macro    *macroPtr;
    SmiDecl  decl;
{
    macroPtr->export.decl = decl;
}



/*
 *----------------------------------------------------------------------
 *
 * findMacroByName --
 *
 *      Lookup a Macro by a given name.
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
findMacroByName(macroname)
    const char *macroname;
{
    Module *modulePtr;
    Macro   *macroPtr;
    
    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	for (macroPtr = modulePtr->firstMacroPtr; macroPtr;
	     macroPtr = macroPtr->nextPtr) {
	    if ((macroPtr->export.name) &&
		!strcmp(macroPtr->export.name, macroname)) {
		return (macroPtr);
	    }
	}
    }

    return (NULL);
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
    
    if (modulePtr) {
	for (macroPtr = modulePtr->firstMacroPtr; macroPtr;
	     macroPtr = macroPtr->nextPtr) {
	    if (!strcmp(macroPtr->export.name, macroname)) {
		return (macroPtr);
	    }
	}
    }

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

    modulePtr = findModuleByName(modulename);
	
    if (modulePtr) {
	for (macroPtr = modulePtr->firstMacroPtr; macroPtr;
	     macroPtr = macroPtr->nextPtr) {
	    if (!strcmp(macroPtr->export.name, macroname)) {
		return (macroPtr);
	    }
	}
    }

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
    parser.flags		= smiFlags;
    parser.file			= NULL;
    parser.line			= -1;
    parser.modulePtr = addModule(util_strdup(""), util_strdup(""), 0, NULL);

    addView("");

    objectPtr = addObject("ccitt", rootNodePtr, 0, 0, &parser);
    objectPtr->export.oid = objectPtr->nodePtr->oid =
	util_malloc(sizeof(int));
    objectPtr->export.oidlen = objectPtr->nodePtr->oidlen = 1;
    objectPtr->nodePtr->oid[0] = 0;
    objectPtr = addObject("iso", rootNodePtr, 1, 0, &parser);
    objectPtr->export.oid = objectPtr->nodePtr->oid =
	util_malloc(sizeof(int));
    objectPtr->export.oidlen = objectPtr->nodePtr->oidlen = 1;
    objectPtr->nodePtr->oid[0] = 1;
    objectPtr = addObject("joint-iso-ccitt", rootNodePtr, 2, 0, &parser);
    objectPtr->export.oid = objectPtr->nodePtr->oid =
	util_malloc(sizeof(int));
    objectPtr->export.oidlen = objectPtr->nodePtr->oidlen = 1;
    objectPtr->nodePtr->oid[0] = 2;
    
    
    typeOctetStringPtr =
	addType("OctetString", SMI_BASETYPE_OCTETSTRING, 0, &parser);
    typeObjectIdentifierPtr =
	addType("ObjectIdentifier", SMI_BASETYPE_OBJECTIDENTIFIER, 0, &parser);
    typeInteger32Ptr =
	addType("Integer32", SMI_BASETYPE_INTEGER32, 0, &parser);
    typeUnsigned32Ptr =
	addType("Unsigned32", SMI_BASETYPE_UNSIGNED32, 0, &parser);
    typeInteger64Ptr =
	addType("Integer64", SMI_BASETYPE_INTEGER64, 0, &parser);
    typeUnsigned64Ptr =
	addType("Unsigned64", SMI_BASETYPE_UNSIGNED64, 0, &parser);
    typeFloat32Ptr =
	addType("Float32", SMI_BASETYPE_FLOAT32, 0, &parser);
    typeFloat64Ptr =
	addType("Float64", SMI_BASETYPE_FLOAT64, 0, &parser);
    typeFloat128Ptr =
	addType("Float128", SMI_BASETYPE_FLOAT128, 0, &parser);
    typeEnumPtr =
	addType("Enumeration", SMI_BASETYPE_ENUM, 0, &parser);
    typeBitsPtr =
	addType("Bits", SMI_BASETYPE_BITS, 0, &parser);

    return (0);
}



/*
 *----------------------------------------------------------------------
 *
 * freeNodeTree --
 *
 *      Free all node of a node (sub)tree.
 *
 * Results:
 *      0 on success or -1 on an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
freeNodeTree(Node *rootPtr)
{
    Node *nodePtr, *nextPtr;
    
    for (nodePtr = rootPtr->firstChildPtr; nodePtr; nodePtr = nextPtr) {
	nextPtr = nodePtr->nextPtr;
	freeNodeTree(nodePtr);
	util_free(nodePtr);
    }
}



/*
 *----------------------------------------------------------------------
 *
 * freeData --
 *
 *      Free all data structures.
 *
 * Results:
 *      0 on success or -1 on an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
freeData()
{
    View       *viewPtr, *nextViewPtr;
    Macro      *macroPtr, *nextMacroPtr;
    Module     *modulePtr, *nextModulePtr;
    Import     *importPtr, *nextImportPtr;
    Revision   *revisionPtr, *nextRevisionPtr;
    List       *listPtr, *nextListPtr;
    Type       *typePtr, *nextTypePtr;
    Object     *objectPtr, *nextObjectPtr;

    for (viewPtr = firstViewPtr; viewPtr; viewPtr = nextViewPtr) {
	nextViewPtr = viewPtr->nextPtr;
	util_free(viewPtr->name);
	util_free(viewPtr);
    }

    for (modulePtr = firstModulePtr; modulePtr; modulePtr = nextModulePtr) {
	nextModulePtr = modulePtr->nextPtr;

	for (importPtr = modulePtr->firstImportPtr; importPtr;
	     importPtr = nextImportPtr) {
	    nextImportPtr = importPtr->nextPtr;
	    util_free(importPtr->export.module);
	    util_free(importPtr->export.name);
	    util_free(importPtr);
	}

	for (revisionPtr = modulePtr->firstRevisionPtr; revisionPtr;
	     revisionPtr = nextRevisionPtr) {
	    nextRevisionPtr = revisionPtr->nextPtr;
	    util_free(revisionPtr->export.description);
	    util_free(revisionPtr);
	}

	for (macroPtr = modulePtr->firstMacroPtr; macroPtr;
	     macroPtr = nextMacroPtr) {
	    nextMacroPtr = macroPtr->nextPtr;
	    util_free(macroPtr->export.name);
	    util_free(macroPtr);
	}

#if 1
	for (typePtr = modulePtr->firstTypePtr; typePtr;
	     typePtr = nextTypePtr) {
	    nextTypePtr = typePtr->nextPtr;
	    for (listPtr = typePtr->listPtr; listPtr;
		 listPtr = nextListPtr) {
		nextListPtr = listPtr->nextPtr;
		if ((typePtr->export.basetype == SMI_BASETYPE_BITS) ||
		    (typePtr->export.basetype == SMI_BASETYPE_ENUM)) {
		    util_free(((NamedNumber *)(listPtr->ptr))->export.name);
		    util_free((NamedNumber *)(listPtr->ptr));
		} else if ((typePtr->export.basetype == SMI_BASETYPE_INTEGER32) ||
			   (typePtr->export.basetype == SMI_BASETYPE_INTEGER64) ||
			   (typePtr->export.basetype == SMI_BASETYPE_UNSIGNED32) ||
			   (typePtr->export.basetype == SMI_BASETYPE_UNSIGNED64) ||
			   (typePtr->export.basetype == SMI_BASETYPE_OCTETSTRING)) {
		    util_free((Range *)(listPtr->ptr));
		}
		util_free(listPtr);
	    }
	    util_free(typePtr->export.name);
	    util_free(typePtr->export.format);
	    util_free(typePtr->export.units);
	    util_free(typePtr->export.description);
	    util_free(typePtr->export.reference);
	    util_free(typePtr);
	}
#endif
	
	for (objectPtr = modulePtr->firstObjectPtr; objectPtr;
	     objectPtr = nextObjectPtr) {
	    nextObjectPtr = objectPtr->nextPtr;
	    util_free(objectPtr->export.name);
	    util_free(objectPtr->export.description);
	    util_free(objectPtr->export.reference);
	    util_free(objectPtr->export.format);
	    util_free(objectPtr->export.units);
	    for (listPtr = objectPtr->listPtr; listPtr;
		 listPtr = nextListPtr) {
		nextListPtr = listPtr->nextPtr;
		util_free(listPtr);
	    }
	    for (listPtr = objectPtr->optionlistPtr; listPtr;
		 listPtr = nextListPtr) {
		nextListPtr = listPtr->nextPtr;
		util_free(((Option *)(listPtr->ptr))->export.description);
		util_free((Option *)(listPtr->ptr));
		util_free(listPtr);
	    }
	    for (listPtr = objectPtr->refinementlistPtr; listPtr;
		 listPtr = nextListPtr) {
		nextListPtr = listPtr->nextPtr;
		util_free((Refinement *)(listPtr->ptr));
		util_free(listPtr);
	    }
	    util_free(objectPtr);
	}

	util_free(modulePtr->export.name);
	util_free(modulePtr->export.path);
	util_free(modulePtr->export.organization);
	util_free(modulePtr->export.contactinfo);
	util_free(modulePtr);
    }

    freeNodeTree(rootNodePtr);
    freeNodeTree(pendingNodePtr);
    
    return;
}



/*
 *----------------------------------------------------------------------
 *
 * loadModule --
 *
 *      Load a MIB module. If modulename is a plain name, the file is
 *	search along the SMIPATH environment variable. If modulename
 *	contains a `.' or `/' it is assumed to be the path.
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
    Parser	    parser;
    char	    s[200];
    char	    *path = NULL, *dir, *smipath;
    int		    sming = 0;
    int             c;
    FILE	    *file;
    
    if ((!modulename) || !strlen(modulename)) {
	return NULL;
    }

    if (!util_ispath(modulename)) {
	/*
	 * A plain modulename. Lookup the path along SMIPATH...
	 */
	if (!smiPath) {
	    return NULL;
	}
	
	smipath = util_strdup(smiPath);
	for (dir = strtok(smipath, ":"); dir; dir = strtok(NULL, ":")) {
	    path = malloc(strlen(dir)+strlen(modulename)+8);
	    sprintf(path, "%s/%s", dir, modulename);
	    if (! access(path, R_OK)) {
		break;
	    }
	    sprintf(path, "%s/%s.my", dir, modulename);
	    if (! access(path, R_OK)) {
		break;
	    }
	    sprintf(path, "%s/%s.smiv2", dir, modulename);
	    if (! access(path, R_OK)) {
		break;
	    }
	    sprintf(path, "%s/%s.sming", dir, modulename);
	    if (! access(path, R_OK)) {
		break;
	    }
	    util_free(path);
	    path = NULL;
	}
	util_free(smipath);
    } else {
	/*
	 * A full path. Take it.
	 */
	path = util_strdup(modulename);
    }

    if (!path) {
	return NULL;
    }

    /*
     * Look into the file to determine whether it contains
     * SMIv1/SMIv2 or SMIng definitions.
     */

    file = fopen(path, "r");
    if (! file) {
	printError(NULL, ERR_OPENING_INPUTFILE, path, strerror(errno));
	util_free(path);
	return NULL;
    }
    while ((c = fgetc(file))) {
	if (c == '-' || isupper(c)) {
	    sming = 0;
	    break;
	} else if (c == '/' || c == 'm') {
	    sming = 1;
	    break;
	} else if (c == EOF || ! isspace(c)) {
	    printError(NULL, ERR_ILLEGAL_INPUTFILE, path);
	    util_free(path);
	    return NULL;
	}
    }
    rewind(file);

    if (sming == 0) {
#ifdef BACKEND_SMI
	parser.path			= util_strdup(path);
	parser.flags			= smiFlags;
	parser.modulePtr		= NULL;
	parser.file			= file;
	if (smiEnterLexRecursion(parser.file) < 0) {
	    printError(&parser, ERR_MAX_LEX_DEPTH);
	    fclose(parser.file);
	}
	smiDepth++;
	parser.line			= 1;
	smiparse((void *)&parser);
	if (parser.flags & SMI_FLAG_STATS) {
	    sprintf(s, "(%d lines)", parser.line-1);
	    printError(&parser, ERR_STATISTICS, s);
	}
	smiLeaveLexRecursion();
	smiDepth--;
	fclose(parser.file);
	return parser.modulePtr;
#else
	printError(NULL, ERR_SMI_NOT_SUPPORTED, path);
	util_free(path);
	return NULL;
#endif
    }

    if (sming == 1) {
#ifdef BACKEND_SMING
	parser.path			= util_strdup(path);
	parser.flags			= smiFlags;
	parser.modulePtr		= NULL;
	parser.file			= file;
	if (smingEnterLexRecursion(parser.file) < 0) {
	    printError(&parser, ERR_MAX_LEX_DEPTH);
	    fclose(parser.file);
	}
	smiDepth++;
	parser.line			= 1;
	smingparse((void *)&parser);
	if (parser.flags & SMI_FLAG_STATS) {
	    sprintf(s, "(%d lines)", parser.line-1);
	    printError(&parser, ERR_STATISTICS, s);
	}
	smingLeaveLexRecursion();
	smiDepth--;
	fclose(parser.file);
	return parser.modulePtr;
#else
	printError(NULL, ERR_SMING_NOT_SUPPORTED, path);
	util_free(path);
	return NULL;
#endif
    }

    util_free(path);
    return NULL;
}


/*
 *----------------------------------------------------------------------
 *
 * checkObjectName --
 *
 *      Check wheather a given object name already exists
 *	in a given module.
 *
 * Results:
 *      1 on success or 0 if the name already exists.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
checkObjectName(modulePtr, name, parserPtr)
    Module	*modulePtr;
    char        *name;
    Parser	*parserPtr;
{
    Object	*objectPtr;
    Type        *typePtr;

    /*
     * This would really benefit from having a hash table...
     */

    if (modulePtr) {
        for (objectPtr = modulePtr->firstObjectPtr;
	     objectPtr; objectPtr = objectPtr->nextPtr) {
	    if (! (objectPtr->flags & FLAG_INCOMPLETE)
		&& ! strcasecmp(name, objectPtr->export.name)) {
		if (! strcmp(name, objectPtr->export.name)) {
		    printError(parserPtr, ERR_REDEFINITION, name);
		    return 0;
		} else {
		    printError(parserPtr, ERR_CASE_REDEFINITION,
			       name, objectPtr->export.name);
		}
	    }
	}
	for (typePtr = modulePtr->firstTypePtr;
	     typePtr; typePtr = typePtr->nextPtr) {
	    /* TODO: must ignore SEQUENCE types here ... */
	    if (! (typePtr->flags & FLAG_INCOMPLETE)
		&& typePtr->export.name
		&& !strcasecmp(name, typePtr->export.name)) {
		printError(parserPtr, ERR_CASE_REDEFINITION,
			   name, typePtr->export.name);
	    }
	}
    }
    return 1;
}



/*
 *----------------------------------------------------------------------
 *
 * checkFormat --
 *
 *      Check whether a format specification is valid.
 *
 * Results:
 *      1 on success or 0 if the format is invalid.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int checkFormat(basetype, p)
    SmiBasetype basetype;
    char *p;
{
    int n, repeat;

    switch (basetype) {
    case SMI_BASETYPE_INTEGER32:
    case SMI_BASETYPE_INTEGER64:
    case SMI_BASETYPE_UNSIGNED32:
    case SMI_BASETYPE_UNSIGNED64:
	if (*p == 'x' || *p == 'o' || *p == 'b') {
	    p++;
	    return (*p == 0);
	} else if (*p == 'd') {
	    p++;
	    if (! *p) return 1;
	    if (*p != '-') return 0;
	    for (n = 0, p++; *p && isdigit((int) *p); p++, n++) ;
	    return (*p == 0 && n > 0);
	}
	return 0;
    case SMI_BASETYPE_OCTETSTRING:
	while (*p) {
	    if ((repeat = (*p == '*'))) p++;                /* part 1 */
	    
	    for (n = 0; *p && isdigit((int) *p); p++, n++) ;/* part 2 */
	    if (! *p || n == 0) return 0;
	    
	    if (*p != 'x' && *p != 'd' && *p != 'o'         /* part 3 */
		&& *p != 'a' && *p != 't') return 0;
	    p++;
	    
	    if (*p                                          /* part 4 */
		&& ! isdigit((int) *p) && *p != '*') p++;
	    
	    if (repeat && *p                                /* part 5 */
		&& ! isdigit((int) *p) && *p != '*') p++;
	}
	return 1;
    default:
	return 0;
    }
}
