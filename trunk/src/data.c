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
 * @(#) $Id: data.c,v 1.34 1998/12/22 17:09:12 strauss Exp $
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


Module          *firstModulePtr, *lastModulePtr;
Node		*rootNodePtr;
Node		*pendingNodePtr;
Type		*typeIntegerPtr, *typeOctetStringPtr, *typeObjectIdentifierPtr;



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
addModule(modulename, path, fileoffset, flags, parserPtr)
    const char	      *modulename;
    const char	      *path;
    off_t	      fileoffset;
    ModuleFlags	      flags;
    Parser	      *parserPtr;
{
    Module	      *modulePtr;

#ifdef DEBUG
    printDebug(4,
      "addModule(%s, %s, %d, %d, 0x%x)\n",
	       modulename, path, fileoffset, flags, parserPtr);
#endif
    
    modulePtr = (Module *)util_malloc(sizeof(Module));
    if (!modulePtr) {
	printError(parserPtr, ERR_ALLOCATING_MIBMODULE, strerror(errno));
	return (NULL);
    }
    
    modulePtr->path			         = util_strdup(path);
    modulePtr->fileoffset			 = fileoffset;
    modulePtr->flags				 = flags;
    
    modulePtr->firstObjectPtr			= NULL;
    modulePtr->lastObjectPtr			= NULL;
    modulePtr->firstTypePtr			= NULL;
    modulePtr->lastTypePtr			= NULL;
    modulePtr->firstMacroPtr			= NULL;
    modulePtr->lastMacroPtr			= NULL;
    modulePtr->firstImportPtr			= NULL;
    modulePtr->lastImportPtr			= NULL;
#if 0
    modulePtr->firstRevisionPtr			= NULL;
    modulePtr->lastRevisionPtr			= NULL;
#endif
    
    modulePtr->numImportedIdentifiers		= 0;
    modulePtr->numStatements			= 0;
    modulePtr->numModuleIdentities		= 0;
    
    modulePtr->lastUpdated.fileoffset		= -1;
    modulePtr->lastUpdated.length		= 0;
    modulePtr->organization.fileoffset		= -1;
    modulePtr->organization.length		= 0;
    modulePtr->contactInfo.fileoffset		= -1;
    modulePtr->contactInfo.length		= 0;
    modulePtr->description.fileoffset		= -1;
    modulePtr->description.length		= 0;
#ifdef TEXTS_IN_MEMORY
    modulePtr->lastUpdated.ptr			= NULL;
    modulePtr->organization.ptr			= NULL;
    modulePtr->contactInfo.ptr			= NULL;
    modulePtr->description.ptr			= NULL;
#endif

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
 * setModuleLastUpdated --
 *
 *      Set the lastUpdated string of a given Module.
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
setModuleLastUpdated(modulePtr, lastUpdatedPtr)
    Module	*modulePtr;
    String	*lastUpdatedPtr;
{
#ifdef DEBUG
    printDebug(5, "setModuleLastUpdated(0x%x(%s), 0x%x(%s))\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       lastUpdatedPtr,
	       lastUpdatedPtr->ptr ? lastUpdatedPtr->ptr : "...");
#endif

    /* TODO: why this check? */
    if (modulePtr->lastUpdated.fileoffset < 0) {
	modulePtr->lastUpdated.fileoffset = lastUpdatedPtr->fileoffset;
	modulePtr->lastUpdated.length = lastUpdatedPtr->length;
#ifdef TEXTS_IN_MEMORY
	modulePtr->lastUpdated.ptr = lastUpdatedPtr->ptr;
#endif
    }
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
setModuleOrganization(modulePtr, organizationPtr)
    Module *modulePtr;
    String *organizationPtr;
{
#ifdef DEBUG
    printDebug(5, "setModuleOrganization(0x%x(%s), px%x(%s))\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       organizationPtr,
	       organizationPtr->ptr ? organizationPtr->ptr : "[FILE]");
#endif

    /* TODO: why this check */
    if (modulePtr->organization.fileoffset < 0) {
	modulePtr->organization.fileoffset = organizationPtr->fileoffset;
	modulePtr->organization.length = organizationPtr->length;
#ifdef TEXTS_IN_MEMORY
	modulePtr->organization.ptr = organizationPtr->ptr;
#endif
    }
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
setModuleContactInfo(modulePtr, contactInfoPtr)
    Module	  *modulePtr;
    String	  *contactInfoPtr;
{
#ifdef DEBUG
    printDebug(5, "setModuleContactInfo(0x%x(%s), 0x%x(%s))\n",
	       modulePtr, modulePtr && modulePtr->name ? modulePtr->name : "",
	       contactInfoPtr,
	       contactInfoPtr->ptr ? contactInfoPtr->ptr : "[FILE]");
#endif

    /* TODO: why this check? */
    if (modulePtr->contactInfo.fileoffset < 0) {
	modulePtr->contactInfo.fileoffset = contactInfoPtr->fileoffset;
	modulePtr->contactInfo.length     = contactInfoPtr->length;
#ifdef TEXTS_IN_MEMORY
	modulePtr->contactInfo.ptr        = contactInfoPtr->ptr;
#endif
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setModuleDescription --
 *
 *      Set the description of a given Module.
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
setModuleDescription(modulePtr, descriptionPtr)
    Module *modulePtr;
    String *descriptionPtr;
{
#ifdef DEBUG
    printDebug(5, "setModuleDescription(0x%x(%s), 0x%x(%s))\n",
	       modulePtr, modulePtr->name, descriptionPtr,
	       descriptionPtr->ptr ? descriptionPtr->ptr : "[FILE]");
#endif
    
    if (modulePtr->description.fileoffset < 0) {
	modulePtr->description.fileoffset = descriptionPtr->fileoffset;
	modulePtr->description.length     = descriptionPtr->length;
#ifdef TEXTS_IN_MEMORY
	modulePtr->description.ptr        = descriptionPtr->ptr;
#endif
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

#ifdef DEBUG
    printDebug(6, "findModuleByName(%s)", modulename);
#endif
    
    for (modulePtr = firstModulePtr; modulePtr;
	 modulePtr = modulePtr->nextPtr) {
	if (!strcmp(modulePtr->name, modulename)) {
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
    
    importPtr->module			 = NULL; /* not yet known */
    importPtr->name		       	 = util_strdup(name);
    importPtr->kind			 = KIND_UNKNOWN;
    
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
    printDebug(4, "checkImports(%s, 0x%x)", modulename, parserPtr);
#endif

    for (importPtr = parserPtr->modulePtr->firstImportPtr;
	 importPtr; importPtr = importPtr->nextPtr) {

	if (smiGetNode(importPtr->name, modulename, 0)) {
	    importPtr->module = util_strdup(modulename);
	    importPtr->kind   = KIND_OBJECT;
	} else if (smiGetType(importPtr->name, modulename, 0)) {
	    importPtr->module = util_strdup(modulename);
	    importPtr->kind   = KIND_TYPE;
	} else if (smiGetMacro(importPtr->name, modulename)) {
	    importPtr->module = util_strdup(modulename);
	    importPtr->kind   = KIND_MACRO;
	} else {
	    n++;
	    printError(parserPtr, ERR_IDENTIFIER_NOT_IN_MODULE,
		       importPtr->name, modulename);
	}

    }

#ifdef DEBUG
    printDebug(4, " = %d\n", n);
#endif

    return (n);
}



#if 0
/*
 *----------------------------------------------------------------------
 *
 * addDescriptor --
 *
 *      Add a descriptor to the list of all descriptors. It gets
 *	linked into the lists of all descriptors, descriptors by
 *	module, by kind and by module and kind.
 *
 * Results:
 *      A pointer to the new Descriptor structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      When the defined descriptor has been forward referenced,
 *	the &ptr parameter may be change by addDescriptor().
 *
 *----------------------------------------------------------------------
 */

Descriptor *
addDescriptor(name, module, kind, ptr, flags, parser)
    const char *name;
    Module *module;
    DescriptorKind kind;
    void *ptr;
    Parser *parser;
{
    Descriptor *descriptor, *olddescriptor;
    Node *pending, *next;
    Type *t;
    
    printDebug(5, "addDescriptor(\"%s\", %s, %s, &%p, %d, parser)\n",
	       name, module &&
	         module->descriptor ? module->descriptor->name : "NULL",
	       stringKind(kind), ptr ? *(void **)ptr : NULL, flags);

    /*
     * If this new descriptor is found as pending type,
     * we have to complete those Type and Descriptor structs instead of
     * creating a new descriptor.
     */
    if ((module) && (kind == KIND_TYPE) &&
	(!(flags & FLAG_IMPORTED))) {
	t = findTypeByModuleAndName(module, name);
	if (t && (t->flags & FLAG_INCOMPLETE) && !(flags & FLAG_INCOMPLETE)) {
	    t->parent      = (*(Type **)ptr)->parent;
	    t->syntax      = (*(Type **)ptr)->syntax;
	    t->decl        = (*(Type **)ptr)->decl;
	    t->status      = (*(Type **)ptr)->status;
	    t->fileoffset  = (*(Type **)ptr)->fileoffset;
	    t->flags       = (*(Type **)ptr)->flags;
	    t->displayHint = (*(Type **)ptr)->displayHint;
	    t->description = (*(Type **)ptr)->description;
#ifdef TEXTS_IN_MEMORY
	    free((*(Type **)ptr)->description.ptr);
#endif
	    free(*(Type **)ptr);

	    printDebug(5, "... = %p\n", t->descriptor);
	    return t->descriptor;
	}
    }

    descriptor = (Descriptor *)malloc(sizeof(Descriptor));
    if (!descriptor) {
	printError(parser, ERR_ALLOCATING_DESCRIPTOR, strerror(errno));
	return (NULL);
    }

    descriptor->name = strdup(name);
    descriptor->module = module;
    descriptor->kind = kind;
    descriptor->flags = flags;
    if (ptr) {
	descriptor->ptr = *(void **)ptr;
    }

    /*
     * TODO: during development, there might be descriptors defined
     * with *ptr == NULL.
     */
    if (ptr && *(void **)ptr && (!(flags & FLAG_IMPORTED))) {
	switch (kind) {
	case KIND_MODULE:
	    ((Module *)(descriptor->ptr))->descriptor = descriptor;
	    break;
	case KIND_MACRO:
	    ((Macro *)(descriptor->ptr))->descriptor = descriptor;
	    break;
	case KIND_TYPE:
	    ((Type *)(descriptor->ptr))->descriptor = descriptor;
	    break;
	case KIND_OBJECT:
	    ((Object *)(descriptor->ptr))->descriptor = descriptor;
	    break;
	case KIND_IMPORTED:
	    break;
	case KIND_ANY:
	case KIND_IMPORT:
	    break;
	    ;
	}
    }

    /*
     * Link the new descriptor into the different lists.
     */
    descriptor->prev = lastDescriptor[KIND_ANY];
    descriptor->next = NULL;
    if (lastDescriptor[KIND_ANY]) {
	lastDescriptor[KIND_ANY]->next = descriptor;
    } else {
	firstDescriptor[KIND_ANY] = descriptor;
    }
    lastDescriptor[KIND_ANY] = descriptor;
    
    descriptor->prevSameKind = lastDescriptor[kind];
    descriptor->nextSameKind = NULL;
    if (lastDescriptor[kind]) {
	lastDescriptor[kind]->nextSameKind = descriptor;
    } else {
	firstDescriptor[kind] = descriptor;
    }
    lastDescriptor[kind] = descriptor;

    if (module) {
	descriptor->prevSameModule = module->lastDescriptor[KIND_ANY];
	descriptor->nextSameModule = NULL;
	if (module->lastDescriptor[KIND_ANY]) {
	    module->lastDescriptor[KIND_ANY]->nextSameModule = descriptor;
	} else {
	    module->firstDescriptor[KIND_ANY] = descriptor;
	}
	module->lastDescriptor[KIND_ANY] = descriptor;
	    
	descriptor->prevSameModuleAndKind = module->lastDescriptor[kind];
	descriptor->nextSameModuleAndKind = NULL;
	if (module->lastDescriptor[kind]) {
	    module->lastDescriptor[kind]->nextSameModuleAndKind =
		descriptor;
	} else {
	    module->firstDescriptor[kind] = descriptor;
	}
	module->lastDescriptor[kind] = descriptor;
    }

    /*
     * If this new descriptor is found on the pending descriptor
     * list (at depth==1 in pendingRootNode), we have to move the
     * corresponding subtree to the main tree.
     */
    if ((module) && (kind == KIND_OBJECT) &&
	(!(flags & FLAG_IMPORTED)) &&
	((*(Object **)ptr)->node->parent != pendingRootNode)) {
	
	/*
	 * check each root of pending subtrees. if it is the just defined
	 * oid, then move it to the main tree.
	 */
	for (pending = pendingRootNode->firstChild; pending;
	     pending = next) {

	    /*
	     * probably we change the contents of `pending', so remember
	     * the next pointer.
	     */
	    next = pending->next;

	    if (!strcmp(pending->firstObject->descriptor->name, name)) {

		/*
		 * remove `pending' from the pendingRootNode tree.
		 */
		if (pending->prev) {
		    pending->prev->next = pending->next;
		} else {
		    pendingRootNode->firstChild = pending->next;
		}
		if (pending->next) {
		    pending->next->prev = pending->prev;
		} else {
		    pendingRootNode->lastChild = pending->prev;
		}

		/*
		 * copy contents of the new node to pending.
		 */
		olddescriptor = pending->firstObject->descriptor;
		pending->subid = (*(Object **)ptr)->node->subid;
		
		pending->firstObject->flags = (*(Object **)ptr)->flags;
		pending->firstObject->decl = (*(Object **)ptr)->decl;
		pending->firstObject->fileoffset =
		    (*(Object **)ptr)->fileoffset;
		pending->firstObject->descriptor =
		    (*(Object **)ptr)->descriptor;

		/*
		 * now link pending into *ptr's place.
		 */
		pending->parent = (*(Object **)ptr)->node->parent;
		pending->next = (*(Object **)ptr)->node->next;
		pending->prev = (*(Object **)ptr)->node->prev;
		if ((*(Object **)ptr)->node->parent->firstChild ==
		    (*(Object **)ptr)->node) {
		    (*(Object **)ptr)->node->parent->firstChild = pending;
		} else {
		    (*(Object **)ptr)->node->prev->next = pending;
		}
		if ((*(Object **)ptr)->node->parent->lastChild ==
		    (*(Object **)ptr)->node) {
		    (*(Object **)ptr)->node->parent->lastChild = pending;
		} else {
		    (*(Object **)ptr)->node->next->prev = pending;
		}

		/*
		 * finally, delete the unneeded node and descriptor.
		 */
		deleteDescriptor(olddescriptor);
#ifdef TEXTS_IN_MEMORY
		free((*(Object **)ptr)->description.ptr);
#endif
		free(*(void **)ptr);
		*(Object **)ptr = pending->firstObject;
		descriptor->ptr = *(Object **)ptr;
		break;
	    }
	}
	
    }

    printDebug(5, "... = %p (ptr=%p)\n", descriptor, descriptor->ptr);
    
    return (descriptor);
}



/*
 *----------------------------------------------------------------------
 *
 * deleteDescriptor --
 *
 *      Delete a descriptor from the lists.
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
deleteDescriptor(descriptor)
    Descriptor *descriptor;
{

    printDebug(5, "deleteDescriptor(%s)\n", descriptor->name);

    if (descriptor->prev) {
	descriptor->prev->next = descriptor->next;
    } else {
	firstDescriptor[KIND_ANY] = descriptor->next;
    }
    if (descriptor->next) {
	descriptor->next->prev = descriptor->prev;
    } else {
	lastDescriptor[KIND_ANY] = descriptor->prev;
    }

    if (descriptor->prevSameModule) {
	descriptor->prevSameModule->nextSameModule =
	    descriptor->nextSameModule;
    } else {
	descriptor->module->firstDescriptor[KIND_ANY] =
	    descriptor->nextSameModule;
    }
    if (descriptor->nextSameModule) {
	descriptor->nextSameModule->prevSameModule =
	    descriptor->prevSameModule;
    } else {
	descriptor->module->lastDescriptor[KIND_ANY] =
	    descriptor->prevSameModule;
    }

    if (descriptor->prevSameKind) {
	descriptor->prevSameKind->nextSameKind =
	    descriptor->nextSameKind;
    } else {
	firstDescriptor[descriptor->kind] =
	    descriptor->nextSameKind;
    }
    if (descriptor->nextSameKind) {
	descriptor->nextSameKind->prevSameKind =
	    descriptor->prevSameKind;
    } else {
	lastDescriptor[descriptor->kind] =
	    descriptor->prevSameKind;
    }

    if (descriptor->prevSameModuleAndKind) {
	descriptor->prevSameModuleAndKind->nextSameModuleAndKind =
	    descriptor->nextSameModuleAndKind;
    } else {
	descriptor->module->firstDescriptor[descriptor->kind] =
	    descriptor->nextSameModuleAndKind;
    }
    if (descriptor->nextSameModuleAndKind) {
	descriptor->nextSameModuleAndKind->prevSameModuleAndKind =
	    descriptor->prevSameModuleAndKind;
    } else {
	descriptor->module->lastDescriptor[descriptor->kind] =
	    descriptor->prevSameModuleAndKind;
    }

}



/*
 *----------------------------------------------------------------------
 *
 * findDescriptor --
 *
 *      Lookup a Descriptor by its name, module and kind.
 *	The module argument might be NULL to search in all modules.
 *	The kind argument might be KIND_ANY to search for any kind.
 *
 * Results:
 *      A pointer to the Descriptor structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Descriptor *
findDescriptor(name, module, kind)
    const char     *name;
    Module         *module;
    DescriptorKind kind;
{
    Descriptor *descriptor;
    
    printDebug(5, "findDescriptor(\"%s\", %s, %s)", name,
	       module ? module->descriptor->name : "any-module",
	       stringKind(kind));

    for (descriptor = module ?
	     module->firstDescriptor[kind] : firstDescriptor[kind];
	 descriptor;
	 descriptor = module ? kind != KIND_ANY ?
	     descriptor->nextSameModuleAndKind : descriptor->nextSameModule
	                     : kind != KIND_ANY ?
	     descriptor->nextSameKind : descriptor->next) {
	if (!strcmp(descriptor->name, name)) {
	    printDebug(5, " = %s\n", descriptor->name);
	    return (descriptor);
	}
    }
    printDebug(5, " = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findNextDescriptor --
 *
 *      Lookup a Descriptor by its name, module and kind.
 *	The module argument might be NULL to search in all modules.
 *	The kind argument might be KIND_ANY to search for any kind.
 *	If the given Descriptor is not NULL, the search starts behind
 *	this this Descriptor.
 *
 * Results:
 *      A pointer to the Descriptor structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Descriptor *
findNextDescriptor(name, module, kind, start)
    const char     *name;
    Module         *module;
    DescriptorKind kind;
    Descriptor	   *start;
{
    Descriptor *descriptor;
    
    printDebug(5, "findNextDescriptor(\"%s\", %s, %s, %s)\n", name,
	       module ? module->descriptor->name : "any-module",
	       stringKind(kind), start ? start->name : "NULL");

    if (start == NULL) {
	return findDescriptor(name, module, kind);
    }
    
    for (descriptor = module ? kind != KIND_ANY ?
	     start->nextSameModuleAndKind : start->nextSameModule
	                     : kind != KIND_ANY ?
	     start->nextSameKind : start->next;
	 descriptor;
	 descriptor = module ? kind != KIND_ANY ?
	     descriptor->nextSameModuleAndKind : descriptor->nextSameModule
	                     : kind != KIND_ANY ?
	     descriptor->nextSameKind : descriptor->next) {
	if (!strcmp(descriptor->name, name)) {
	    printDebug(5, "... = %s\n", descriptor->name);
	    return (descriptor);
	}
    }
    printDebug(5, "... = NULL\n");
    return (NULL);
}
#endif



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
    printDebug(5, "addObject(0x%x%s, %d, %s, %d, 0x%x)\n",
	       parentNodePtr,
	       parentNodePtr == pendingNodePtr ? "(pending)" : "",
	       subid, flags, parserPtr);
#endif
    
    objectPtr = (Object *)util_malloc(sizeof(Object));
    if (!objectPtr) {
	printError(parserPtr, ERR_ALLOCATING_OBJECT, strerror(errno));
	return (NULL);
    }

    modulePtr = parserPtr->modulePtr;
    
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
    objectPtr->description.fileoffset		= -1;
    objectPtr->description.length		= 0;
#ifdef TEXTS_IN_MEMORY
    objectPtr->description.ptr			= NULL;
#endif
    
    objectPtr->nextPtr				= NULL;
    objectPtr->prevPtr				= modulePtr->lastObjectPtr;
    if (!modulePtr->firstObjectPtr)
	modulePtr->firstObjectPtr		= objectPtr;
    if (modulePtr->lastObjectPtr)
	modulePtr->lastObjectPtr->nextPtr	= objectPtr;
    modulePtr->lastObjectPtr			= objectPtr;
    
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
    objectPtr->nodePtr				      = nodePtr;
    objectPtr->flags				      = flags;
    objectPtr->name				      = NULL;
    objectPtr->fileoffset			      = -1;
    objectPtr->prevPtr				      = NULL;
    objectPtr->nextPtr				      = NULL;
    objectPtr->prevSameNodePtr			      = NULL;
    objectPtr->nextSameNodePtr			      = NULL;
    objectPtr->typePtr				      = NULL;
    objectPtr->indexPtr				      = NULL;
    objectPtr->decl				      = SMI_DECL_UNKNOWN;
    objectPtr->access				      = SMI_ACCESS_UNKNOWN;
    objectPtr->status				      = SMI_STATUS_UNKNOWN;
    objectPtr->description.fileoffset		      = -1;
    objectPtr->description.length		      = 0;
#ifdef TEXTS_IN_MEMORY
    objectPtr->description.ptr			      = NULL;
#endif

    objectPtr->nextPtr				= NULL;
    objectPtr->prevPtr				= modulePtr->lastObjectPtr;
    if (!modulePtr->firstObjectPtr)
	modulePtr->firstObjectPtr		= objectPtr;
    if (modulePtr->lastObjectPtr)
	modulePtr->lastObjectPtr->nextPtr	= objectPtr;
    modulePtr->lastObjectPtr			= objectPtr;
    
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
    nodePtr->parentNodePtr			= parentNodePtr;
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
    return nodePtr->parentNodePtr;
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

void
setObjectName(objectPtr, namePtr)
    Object	      *objectPtr;
    smi_descriptor    *namePtr;
{
#ifdef DEBUG
    printDebug(5, "setObjectName(0x%x, \"%s\")\n",
	       objectPtr, namePtr);
#endif

    objectPtr->name = util_strdup(namePtr);
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
	       objectPtr, objectPtr->name, typePtr, typePtr->name);
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
setObjectDescription(objectPtr, descriptionPtr)
    Object    *objectPtr;
    String    *descriptionPtr;
{
#ifdef DEBUG
    printDebug(5, "setObjectDescription(0x%x(%s), \"%s\"(%d,%d))\n",
	       objectPtr, objectPtr->name,
	       descriptionPtr->ptr ? descriptionPtr->ptr : "[FILE]",
	       descriptionPtr->fileoffset, descriptionPtr->length);
#endif

    /* TODO: why this check? */
    if (objectPtr->description.fileoffset < 0) {
	objectPtr->description.fileoffset = descriptionPtr->fileoffset;
	objectPtr->description.length = descriptionPtr->length;
#ifdef TEXTS_IN_MEMORY
	objectPtr->description.ptr = descriptionPtr->ptr;
#endif
    }
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
 * findObjectByNodeAndModule --
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
	 objectPtr = objectPtr->nextPtr) {
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
	 objectPtr = objectPtr->nextPtr) {
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
	    if (!strcmp(objectPtr->name, objectname)) {
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
	    if (!strcmp(objectPtr->name, objectname)) {
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
	    if (!strcmp(objectPtr->name, objectname)) {
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
	       typename, smiStringSyntax(syntax), flags, parserPtr);
#endif

    modulePtr = parserPtr->modulePtr;
    
    typePtr = util_malloc(sizeof(Type));
    if (!typePtr) {
	printError(parserPtr, ERR_ALLOCATING_TYPE, strerror(errno));
	return (NULL);
    }

    typePtr->name	                = util_strdup(typename);
    typePtr->modulePtr			= parserPtr->modulePtr;
    typePtr->syntax			= syntax;
    typePtr->decl			= SMI_DECL_UNKNOWN;
    typePtr->status			= SMI_STATUS_UNKNOWN;
    typePtr->flags			= flags;
    typePtr->parentType			= NULL;
    typePtr->description.fileoffset	= -1;
    typePtr->description.length		= 0;
    typePtr->displayHint.fileoffset	= -1;
    typePtr->displayHint.length		= 0;
#ifdef TEXTS_IN_MEMORY
    typePtr->description.ptr		= NULL;
    typePtr->displayHint.ptr		= NULL;
#endif

    typePtr->nextPtr			= NULL;
    typePtr->prevPtr			= modulePtr->lastTypePtr;
    if (!modulePtr->firstTypePtr)
	modulePtr->firstTypePtr		= typePtr;
    if (modulePtr->lastTypePtr)
	modulePtr->lastTypePtr->nextPtr	= typePtr;
    modulePtr->lastTypePtr		= typePtr;
    
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
    Type		  *TypePtr;
    Module		  *modulePtr;
    
#ifdef DEBUG
    printDebug(5, "duplicateType(0x%x, %d, 0x%x)",
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
    typePtr->flags			= templatePtr->flags;
    typePtr->parentType			= util_strdup(templatePtr->name);
    typePtr->description.fileoffset	= -1;
    typePtr->description.length		= 0;
    typePtr->displayHint.fileoffset	= -1;
    typePtr->displayHint.length		= 0;
#ifdef TEXTS_IN_MEMORY
    typePtr->description.ptr		= NULL;
    typePtr->displayHint.ptr		= NULL;
#endif

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
setTypeName(typePtr, namePtr)
    Type	      *typePtr;
    smi_descriptor    *namePtr;
{
#ifdef DEBUG
    printDebug(5, "setTypeName(0x%x, \"%s\")\n",
	       typePtr, namePtr);
#endif

    typePtr->name = util_strdup(namePtr);
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
	       typePtr, typePtr->name, smiStringStatus(status));
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
setTypeDescription(typePtr, descriptionPtr)
    Type	   *typePtr;
    String	   *descriptionPtr;
{
#ifdef DEBUG
    printDebug(5, "setTypeDescription(0x%x(%s), \"%s\"(%d,%d))\n",
	       typePtr, typePtr->name,
	       descriptionPtr->ptr ? descriptionPtr->ptr : "[FILE]",
	       descriptionPtr->fileoffset, descriptionPtr->length);
#endif
    
    if (typePtr->description.fileoffset < 0) {
	typePtr->description.fileoffset = descriptionPtr->fileoffset;
	typePtr->description.length = descriptionPtr->length;
#ifdef TEXTS_IN_MEMORY
	typePtr->description.ptr = descriptionPtr->ptr;
#endif
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
setTypeParent(typePtr, parent)
    Type	   *typePtr;
    const char	   *parent;
{
#ifdef DEBUG
    printDebug(5, "setTypeParent(0x%x(%s), %s)\n",
	       typePtr, typePtr->name, parent);
#endif
    
    if (!typePtr->parent) {
	typePtr->parent  = util_strdup(parent);
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypePointer --
 *
 *      Set the pointer to void data of a given Type.
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
setTypePointer(typePtr, pointer)
    Type	   *typePtr;
    void	   *pointer;
{
#ifdef DEBUG
    printDebug(5, "setTypePointer(0x%x(%s), 0x%x)\n",
	       typePtr, typePtr->name, pointer);
#endif
    
    if (!typePtr->pointer) {
	typePtr->pointer  = pointer;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeDisplayHint --
 *
 *      Set the displayHint of a given Type.
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
setTypeDisplayHint(typePtr, displayHintPtr)
    Type           *typePtr;
    String	   *displayHintPtr;
{
#ifdef DEBUG
    printDebug(5, "setTypeDisplayHint(0x%x(%s), \"%s\")\n",
	       typePtr, typePtr->name,
	       displayHintPtr->ptr ? displayHintPtr->ptr : "[FILE]",
	       displayHintPtr->fileoffset, displayHintPtr->length);
#endif
    
    if (typePtr->displayHint.fileoffset < 0) {
	typePtr->displayHint.fileoffset = displayHintPtr->fileoffset;
	typePtr->displayHint.length = displayHintPtr->length;
#ifdef TEXTS_IN_MEMORY
	typePtr->displayHint.ptr = displayHintPtr->ptr;
#endif
    }
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
	       typePtr, typePtr->name, fileoffset);
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
	       typePtr, typePtr->name, smiStringDecl(decl));
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
	       typePtr, typePtr->name, flags);
#endif
    
    typePtr->flags |= flags;
}



#if 0
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
    
#ifdef DEBUG
    printDebug(6, "findTypeByName(%s)", typename);
#endif
    
    for (descriptor = firstDescriptor[KIND_TYPE];
	 descriptor; descriptor = descriptor->nextSameKind) {
	if ((!strcmp(descriptor->name, name)) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
#ifdef DEBUG
	    printDebug(4, " = 0x%x(%s)\n", descriptor->ptr, descriptor->name);
#endif
	    return (descriptor->ptr);
	}
    }

#ifdef DEBUG
    printDebug(4, " = NULL\n");
#endif
    return (NULL);
}
#endif



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
	    if (!strcmp(typePtr->name, typename)) {
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
	    if (!strcmp(typePtr->name, typename)) {
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
    Module	    *modulePtr;
    
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
     * belonging to a dummy module "".
     */
    modulePtr = addModule("", "", -1, 0, NULL);
    objectPtr = addObject("ccitt", rootNodePtr, 0, 0, NULL);
    objectPtr = addObject("iso", rootNodePtr, 1, 0, NULL);
    objectPtr = addObject("joint-iso-ccitt", rootNodePtr, 2, 0, NULL);

    /*
     * Initialize the well-known ASN.1 Types.
     */
    typeIntegerPtr          = addType("INTEGER",
				      SMI_SYNTAX_INTEGER, 0, NULL);
    typeOctetStringPtr      = addType("OCTET STRING",
				      SMI_SYNTAX_OCTET_STRING, 0, NULL);
    typeObjectIdentifierPtr = addType("OBJECT IDENTIFIER",
				      SMI_SYNTAX_OBJECT_IDENTIFIER, 0, NULL);

    return (0);
}



/*
 *----------------------------------------------------------------------
 *
 * readMibFile --
 *
 *      Read a MIB File.
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
readMibFile(path, modulename, flags)
    const char      *path;
    const char      *modulename;
    ParserFlags     flags;
{
    Parser	    parser;
    char	    s[200];

#ifdef DEBUG
    printDebug(3, "readMibFile(%s, %s, %d)\n", path, modulename, flags);
#endif
    
    parser.path				= util_strdup(path);
    parser.module			= util_strdup(modulename);
    parser.flags			= flags;
    parser.modulePtr			= NULL;
    
    parser.file				= fopen(parser.path, "r");
    if (!parser.file) {
	printError(NULL, ERR_OPENING_INPUTFILE, parser.path,
		   strerror(errno));
	return (-1);
    } else {
	if (enterLexRecursion(parser.file) < 0) {
	    printError(&parser, ERR_MAX_LEX_DEPTH);
	    fclose(parser.file);
	    return (-1);
	}
	parser.line			= 1;
	parser.column			= 1;
	parser.character		= 1;
	yyparse((void *)&parser);
	leaveLexRecursion();
	fclose(parser.file);
	if (parser.flags & PARSER_FLAG_STATS) {
	    sprintf(s, " (%d lines)", parser.line-1);
	    printError(&parser, ERR_STATISTICS, s);
	}
    }

    return (0);
}

