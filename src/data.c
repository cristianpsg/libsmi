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
 * @(#) $Id: data.c,v 1.28 1998/11/25 03:47:42 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "error.h"
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



Descriptor	*firstDescriptor[NUM_KINDS];
Descriptor	*lastDescriptor[NUM_KINDS];
					/* List of known Descriptors (OID    */
					/* labels, TCs Types, Modules).      */

Node		*rootNode;

Node		*pendingRootNode;

Type		*typeInteger, *typeOctetString, *typeObjectIdentifier;



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Module functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



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
addModule(name, path, fileoffset, flags, parser)
    const char *name;
    const char *path;
    off_t fileoffset;
    Flags flags;
    Parser *parser;
{
    Module *module;
    Descriptor *descriptor;
    int i;
    
    printDebug(4, "addModule(\"%s\", \"%s\", %d, parser)\n",
	       name, path, fileoffset);

    module = (Module *)malloc(sizeof(Module));
    if (!module) {
	printError(parser, ERR_ALLOCATING_MIBMODULE, strerror(errno));
	return (NULL);
    }
    
    module->path = strdup(path);
    module->fileoffset = fileoffset;
    module->flags = flags;
    module->lastUpdated.fileoffset = -1;
    module->lastUpdated.length = 0;
    module->organization.fileoffset = -1;
    module->organization.length = 0;
    module->contactInfo.fileoffset = -1;
    module->contactInfo.length = 0;
    module->description.fileoffset = -1;
    module->description.length = 0;
#ifdef TEXTS_IN_MEMORY
    module->lastUpdated.ptr = NULL;
    module->organization.ptr = NULL;
    module->contactInfo.ptr = NULL;
    module->description.ptr = NULL;
#endif
    for (i = 0; i < NUM_KINDS; i++) {
	module->firstDescriptor[i] = NULL;
	module->lastDescriptor[i] = NULL;
    }
    
    /*
     * Create a Descriptor.
     */
    module->descriptor = NULL;
    descriptor = addDescriptor(name, module, KIND_MODULE, &module,
			       flags & FLAGS_GENERAL, parser);
    module->descriptor = descriptor;
    
    return (module);
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
setModuleLastUpdated(module, lastUpdated)
    Module *module;
    String *lastUpdated;
{
    printDebug(5, "setModuleLastUpdated(%s, \"%s\")\n",
	       module->descriptor ? module->descriptor->name : "?",
	       lastUpdated->ptr ? lastUpdated->ptr : "...");

    if (module->lastUpdated.fileoffset < 0) {
	module->lastUpdated.fileoffset = lastUpdated->fileoffset;
	module->lastUpdated.length = lastUpdated->length;
#ifdef TEXTS_IN_MEMORY
	module->lastUpdated.ptr = lastUpdated->ptr;
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
setModuleOrganization(module, organization)
    Module *module;
    String *organization;
{
    printDebug(5, "setModuleOrganization(%s, \"%s\")\n",
	       module->descriptor ? module->descriptor->name : "?",
	       organization->ptr ? organization->ptr : "...");

    if (module->organization.fileoffset < 0) {
	module->organization.fileoffset = organization->fileoffset;
	module->organization.length = organization->length;
#ifdef TEXTS_IN_MEMORY
	module->organization.ptr = organization->ptr;
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
setModuleContactInfo(module, contactInfo)
    Module *module;
    String *contactInfo;
{
    printDebug(5, "setModuleContactInfo(%s, \"%s\")\n",
	       module->descriptor ? module->descriptor->name : "?",
	       contactInfo->ptr ? contactInfo->ptr : "...");

    if (module->contactInfo.fileoffset < 0) {
	module->contactInfo.fileoffset = contactInfo->fileoffset;
	module->contactInfo.length = contactInfo->length;
#ifdef TEXTS_IN_MEMORY
	module->contactInfo.ptr = contactInfo->ptr;
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
setModuleDescription(module, description)
    Module *module;
    String *description;
{
    printDebug(5, "setModuleDescription(%s, \"%s\")\n",
	       module->descriptor ? module->descriptor->name : "?",
	       description->ptr ? description->ptr : "...");

    if (module->description.fileoffset < 0) {
	module->description.fileoffset = description->fileoffset;
	module->description.length = description->length;
#ifdef TEXTS_IN_MEMORY
	module->description.ptr = description->ptr;
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
findModuleByName(name)
    const char *name;
{
    Descriptor *descriptor;

    printDebug(4, "findModuleByName(\"%s\")", name);

    for (descriptor = firstDescriptor[KIND_MODULE]; descriptor;
	 descriptor = descriptor->nextSameKind) {
	if ((!strcmp(descriptor->name, name)) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
	    printDebug(4, " = \"%s\"\n", descriptor->name);
	    return (descriptor->module);
	}
    }

    printDebug(4, " = NULL\n");

    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Import identifier functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



/*
 *----------------------------------------------------------------------
 *
 * addImportDescriptor --
 *
 *      Adds a descriptor to the actual parser's list of descriptors to
 *	be imported. This list may be checked (and implicitly
 *	deleted) by checkImportDescriptors() afterwards.
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
addImportDescriptor(name, parser)
    const char *name;
    Parser *parser;
{
    Descriptor *descriptor;
    
    printDebug(4, "addImportDescriptor(\"%s\", parser)\n", name);

    descriptor = addDescriptor(name, parser->thisModule,
			       KIND_IMPORT, NULL, 0, parser);
    if (!descriptor) {
	printError(parser, ERR_ALLOCATING_IMPORTDESCRIPTOR, strerror(errno));
	return (-1);
    }
    
    return (0);
}



/*
 *----------------------------------------------------------------------
 *
 * checkImportDescriptors --
 *
 *      Check wheather all descriptors in the actual parser's list
 *	are imported by a given Module. Implicitly delete all items
 *	from the list and append them to a list of imported descriptors
 *	for the current module.
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
checkImportDescriptors(modulename, parser)
    char *modulename;
    Parser *parser;
{
    Descriptor *descriptor, *mod;
    
    printDebug(4, "checkImportDescriptors(%s, parser)\n", modulename);
    
    while (parser->thisModule->firstDescriptor[KIND_IMPORT]) {
	descriptor = parser->thisModule->firstDescriptor[KIND_IMPORT];
	/*
	 * We add a module descriptor with FLAG_IMPORTED and
	 * without a module struct...
	 */
	mod = addDescriptor(modulename, parser->thisModule, KIND_MODULE, NULL,
			    FLAG_IMPORTED, parser);

	/*
	 * ...just to mark the imported descriptors to imported from
	 * this module.
	 */
	if (smiGetNode(descriptor->name, modulename, 0)) {
	    addDescriptor(descriptor->name, parser->thisModule,
			  KIND_OBJECT, &mod, FLAG_IMPORTED, parser);
	} else if (smiGetType(descriptor->name, modulename, 0)) {
	    addDescriptor(descriptor->name, parser->thisModule,
			  KIND_TYPE, &mod, FLAG_IMPORTED, parser);
	} else if (smiGetMacro(descriptor->name, modulename)) {
	    addDescriptor(descriptor->name, parser->thisModule,
			  KIND_MACRO, &mod, FLAG_IMPORTED, parser);
	} else {
	    printError(parser, ERR_IDENTIFIER_NOT_IN_MODULE,
		       descriptor->name, modulename);
	}

	deleteDescriptor(descriptor);
    }

    return (0);
}



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Descriptor functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



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
		free((*(Object **)ptr)->node);
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



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Object functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */


    
/*
 *----------------------------------------------------------------------
 *
 * addObject --
 *
 *      Create a new Object and Node or update an existing one. Also creates
 *	a Descriptor if needed and updates other Objects and Nodes according
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
addObject(parent, subid, module, flags, parser)
    Node *parent;
    smi_subid subid;
    Module *module;
    Flags flags;
    Parser *parser;
{
    Object *object;
    Node *node;
    
    printDebug(5, "addObject(%p%s, %d, %s, %d, parser)\n",
	       parent, parent == pendingRootNode ? "(==pending)" : "",
	       subid,
	       module &&
	         module->descriptor ? module->descriptor->name : "NULL",
	       flags);

    object = (Object *)malloc(sizeof(Object));
    if (!object) {
	printError(parser, ERR_ALLOCATING_OBJECT, strerror(errno));
	return (NULL);
    }

    object->node = NULL;
    object->prev = NULL;
    object->next = NULL;
    object->module = module;
    object->type = NULL;
    object->index = NULL;
    object->fileoffset = -1;
    object->decl = SMI_DECL_UNKNOWN;
    object->access = SMI_ACCESS_UNKNOWN;
    object->status = SMI_STATUS_UNKNOWN;
    object->flags = flags;
    object->descriptor = NULL;
    object->description.fileoffset = -1;
    object->description.length = 0;
#ifdef TEXTS_IN_MEMORY
    object->description.ptr = NULL;
#endif
    
    /*
     * Link it into the tree.
     */
    if ((parent == pendingRootNode) ||
	(!(node = findNodeByParentAndSubid(parent, subid)))) {
	node = addNode(parent, subid, flags, parser);
	node->firstObject = object;
	node->lastObject = object;
	
    } else {
	object->prev = node->lastObject;
	node->lastObject->next = object;
	node->lastObject = object;
    }
    object->node = node;
    
    printDebug(5, "... = %p\n", object);
    
    return (object);
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
duplicateObject(template, module, flags, parser)
    Object *template;
    Module *module;
    Flags flags;
    Parser *parser;
{
    Object *object;
    
    printDebug(5, "duplicateObject(%p, %s, %d, parser)",
	       template,
	       module &&
	         module->descriptor ? module->descriptor->name : "NULL",
	       flags);

    object = (Object *)malloc(sizeof(Object));
    if (!object) {
	printError(parser, ERR_ALLOCATING_OBJECT, strerror(errno));
	return (NULL);
    }

    object->node = template->node;
    object->prev = template->node->lastObject;
    object->next = NULL;
    object->node->lastObject->next = object;
    object->node->lastObject = object;
    
    object->module = module;
    object->type = NULL;
    object->fileoffset = -1;
    object->decl = SMI_DECL_UNKNOWN;
    object->access = SMI_ACCESS_UNKNOWN;
    object->status = SMI_STATUS_UNKNOWN;
    object->flags = flags;
    object->descriptor = NULL;
    object->description.fileoffset = -1;
    object->description.length = 0;
#ifdef TEXTS_IN_MEMORY
    object->description.ptr = NULL;
#endif
    
    printDebug(5, " = %p (node=%p)\n", object, object->node);
    
    return (object);
}



Node *
addNode (parent, subid, flags, parser)
    Node *parent;
    smi_subid subid;
    Flags flags;
    Parser *parser;
{
    Node *node;
    Node *c;
    
    printDebug(5, "addNode(%p%s, %d, %d, parser)",
	       parent, parent == pendingRootNode ? "(==pending)" : "",
	       subid, flags);

    node = (Node *)malloc(sizeof(Node));
    if (!node) {
	printError(parser, ERR_ALLOCATING_NODE, strerror(errno));
	return (NULL);
    }
    node->flags = flags;
    node->subid = subid;
    node->parent = parent;
    node->firstChild = NULL;
    node->lastChild = NULL;
    node->firstObject = NULL;
    node->lastObject = NULL;

    if (parent) {
	if (parent->firstChild) {
	    for (c = parent->firstChild;
		 c && (c->subid < subid);
		 c = c->next);
	    if (c) {
		if (c != parent->firstChild) {
		    c->prev->next = node;
		    node->prev = c->prev;
		    c->prev = node;
		    node->next = c;
		} else {
		    c->prev = node;
		    node->next = c;
		    node->prev = NULL;
		    parent->firstChild = node;
		}
	    } else {
		node->next = NULL;
		node->prev = parent->lastChild;
		parent->lastChild->next = node;
		parent->lastChild = node;
	    }
	} else {
	    parent->firstChild = node;
	    parent->lastChild = node;
	    node->next = NULL;
	    node->prev = NULL;
	}
    }

    printDebug(5, " = %p\n", node);
    
    return node;
}



Node *
createNodes(oid)
    const char *oid;
{
    char *p, *elements;
    Node *parent;
    smi_subid subid;
    
    printDebug(5, "createNodes(%s)\n", oid);

    parent = rootNode;
    elements = strdup(oid);
    /* TODO: success? */
    p = strtok(elements, ".");
    do {
	subid = (unsigned int)strtoul(p, NULL, 0);
	if (!(parent = findNodeByParentAndSubid(parent, subid))) {
	    parent = addNode(parent, subid, 0, NULL);
	}
    } while ((p = strtok(NULL, ".")));

    free(elements);

    return parent;
}



Node *
getParent(node)
    Node *node;
{
    return node->parent;
}



smi_subid
getLastSubid(oid)
    const char *oid;
{
    char *p;

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
 * setObjectSyntax --
 *
 *      Set the syntax (pointer to a Type struct) of a given Object.
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
setObjectSyntax(object, type)
    Object *object;
    Type *type;
{
    printDebug(5, "setObjectSyntax(%p (%s), %p (%s))\n", object,
	       object->descriptor ? object->descriptor->name : "?", type,
	       type && type->descriptor ? type->descriptor->name : "NULL");

    if (object->type == NULL) {
	object->type = type;
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
setObjectAccess(object, access)
    Object *object;
    smi_access access;
{
    printDebug(5, "setObjectAccess(%p (%s), %s)\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       smiStringAccess(access));

    if (object->access == SMI_ACCESS_UNKNOWN) {
	object->access = access;
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
setObjectStatus(object, status)
    Object *object;
    smi_status status;
{
    printDebug(5, "setObjectStatus(%p (%s), %s)\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       smiStringStatus(status));

    if (object->status == SMI_STATUS_UNKNOWN) {
	object->status = status;
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
setObjectDescription(object, description)
    Object *object;
    String *description;
{
    printDebug(5, "setObjectDescription(%p (%s), \"%s\" (%d,%d))\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       description->ptr ? description->ptr : "...",
	       description->fileoffset, description->length);

    if (object->description.fileoffset < 0) {
	object->description.fileoffset = description->fileoffset;
	object->description.length = description->length;
#ifdef TEXTS_IN_MEMORY
	object->description.ptr = description->ptr;
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
setObjectFileOffset(object, fileoffset)
    Object *object;
    off_t fileoffset;
{
    printDebug(5, "setObjectFileOffset(%p (%s), %d)\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       fileoffset);

    if (object->fileoffset < 0) {
	object->fileoffset = fileoffset;
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
setObjectDecl(object, decl)
    Object *object;
    smi_decl decl;
{
    printDebug(5, "setObjectDecl(%p (%s), %s)\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       smiStringDecl(decl));

    if (object->decl == SMI_DECL_UNKNOWN) {
	object->decl = decl;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setObjectFlags --
 *
 *      Add(!) flags to the flags of a given Object.
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
setObjectFlags(object, flags)
    Object *object;
    Flags flags;
{
    printDebug(5, "setObjectFlags(%p (%s), %d)\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       flags);

    object->flags |= flags;
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
setObjectIndex(object, list)
    Object *object;
    List *list;
{
    printDebug(5, "setObjectIndex(%p (%s), %p)\n", object,
	       object->descriptor ? object->descriptor->name : "?",
	       list);

    object->index = list;
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
findNodeByParentAndSubid(parent, subid)
    Node *parent;
    unsigned int subid;
{
    Node *node;
    
    printDebug(4, "findNodeByParentAndSubid(...%d, %d)",
	       parent ? parent->subid : 0, subid);

    if (parent) {
	for (node = parent->firstChild; node; node = node->next) {
	    if (node->subid == subid) {
		printDebug(4, " = %p\n", node);
		return (node);
	    }
	}
    }
    
    printDebug(4, " = NULL\n");

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
findObjectByNodeAndModule(node, module)
    Node *node;
    Module *module;
{
    Object *object;

    printDebug(4, "findObjectByNodeAndModule(%p, %s)", node,
	       module ? module->descriptor->name : "NULL");

    for (object = node->firstObject; object; object = object->next) {
	if (object->module == module) {
	    printDebug(4, " = %p (%s)\n", object, object->descriptor->name);
	    return (object);
	}
    }

    printDebug(4, " = NULL\n");

    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByNodeAndModulename --
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
findObjectByNodeAndModulename(node, modulename)
    Node *node;
    const char *modulename;
{
    Object *object;

    printDebug(4, "findObjectByNodeAndModulename(%p, \"%s\")", node,
	       modulename);

    for (object = node->firstObject; object; object = object->next) {
	if (!strcmp(object->module->descriptor->name, modulename)) {
	    printDebug(4, " = %p (%s)\n", object, object->descriptor->name);
	    return (object);
	}
    }

    printDebug(4, " = NULL\n");

    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByName --
 *
 *      Lookup a Object by a given Descriptor name. Note, that
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
findObjectByName(name)
    const char *name;
{
    Descriptor *descriptor;

    printDebug(4, "findObjectByName(\"%s\")", name);

    for (descriptor = firstDescriptor[KIND_OBJECT];
	 descriptor; descriptor = descriptor->nextSameKind) {
	if ((!strcmp(descriptor->name, name)) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
	    /*
	     * We return the first matching object.
	     * TODO: probably we should check if there are more matching
	     *       objects, and give a warning if there's another one.
	     */
	    printDebug(4, " = %p (%s)\n", descriptor->ptr, descriptor->name);
	    return (descriptor->ptr);
	}
    }

    printDebug(4, " = NULL\n");

    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByModulenameAndName --
 *
 *      Lookup a Object by a given Module and Descriptor name.
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
findObjectByModulenameAndName(modulename, name)
    const char *modulename;
    const char *name;
{
    Descriptor *descriptor;
    Module *module;
    
    printDebug(4, "findObjectByModulenameAndName(\"%s\", \"%s\")\n",
	       modulename, name);

    module = findModuleByName(modulename);
    if (module) {
	for (descriptor = module->firstDescriptor[KIND_OBJECT];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if ((!strcmp(descriptor->name, name)) &&
		(!(descriptor->flags & FLAG_IMPORTED))) {
		printDebug(4, "... = %p (%s.%s)\n", descriptor->ptr,
		       ((Object *)(descriptor->ptr))->module->descriptor->name,
			   descriptor->name);
		return (descriptor->ptr);
	    }
	}
    }

    /*
     * Some toplevel Objects seem to be always known.
     */
    if ((!strcmp(name, "iso")) || (!strcmp(name, "ccitt")) ||
	(!strcmp(name, "joint-iso-ccitt"))) {
	return findObjectByName(name);
    }
    
    printDebug(4, "... = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findObjectByModuleAndName --
 *
 *      Lookup a Object by a given Module and Descriptor name.
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
findObjectByModuleAndName(module, name)
    Module *module;
    const char *name;
{
    Descriptor *descriptor;
    
    printDebug(4, "findObjectByModuleAndName(%s, \"%s\")",
	       module ? module->descriptor->name : "NULL" , name);

    if (module) {
	for (descriptor = module->firstDescriptor[KIND_OBJECT];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if ((!strcmp(descriptor->name, name)) &&
		(!(descriptor->flags & FLAG_IMPORTED))) {
		printDebug(4, " = %p (%s)\n", descriptor->ptr,
			   descriptor->name);
		return (descriptor->ptr);
	    }
	}
    }
    
    /*
     * Some toplevel Objects seem to be always known.
     */
    if ((!strcmp(name, "iso")) || (!strcmp(name, "ccitt")) ||
	(!strcmp(name, "joint-iso-ccitt"))) {
	return findObjectByName(name);
    }
    
    printDebug(4, " = NULL\n");
    return (NULL);
}



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



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Type functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



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
addType(parent, syntax, module, flags, parser)
    Type       *parent;
    smi_syntax syntax;
    Module     *module;
    Flags      flags;
    Parser     *parser;
{
    Type *type;

    printDebug(5, "addType(%p (%s), %s, %s, %d, parser)",
	       parent, parent &&
	         parent->descriptor ? parent->descriptor->name : "",
	       smiStringSyntax(syntax),
	       module &&
	         module->descriptor ? module->descriptor->name : "NULL",
	       flags);

    /*
     * Otherwise, create the Type.
     */
    type = malloc(sizeof(Type));
    if (!type) {
	printError(parser, ERR_ALLOCATING_TYPE, strerror(errno));
	return (NULL);
    }

    type->module = module;
    type->parent = parent;
    if (parent &&
	(syntax != SMI_SYNTAX_SEQUENCE) &&
	(syntax != SMI_SYNTAX_SEQUENCE_OF)) {
	type->syntax = parent->syntax;
    } else {
	type->syntax = syntax;
    }
    type->decl = SMI_DECL_UNKNOWN;
    type->status = SMI_STATUS_UNKNOWN;
    type->flags = flags;
    type->descriptor = NULL;
    type->description.fileoffset = -1;
    type->description.length = 0;
    type->displayHint.fileoffset = -1;
    type->displayHint.length = 0;
#ifdef TEXTS_IN_MEMORY
    type->description.ptr = NULL;
    type->displayHint.ptr = NULL;
#endif

    printDebug(5, " = %p\n", type);
    
    return (type);
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
setTypeStatus(type, status)
    Type       *type;
    smi_status status;
{
    printDebug(5, "setTypeStatus(%s, %s)\n",
	       type->descriptor ? type->descriptor->name : "?",
	       smiStringStatus(status));

    if (type->status == SMI_STATUS_UNKNOWN) {
	type->status = status;
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
setTypeDescription(type, description)
    Type *type;
    String *description;
{
    printDebug(5, "setTypeDescription(%s, \"%s\")\n",
	       type->descriptor ? type->descriptor->name : "?",
	       description->ptr ? description->ptr : "...");

    if (type->description.fileoffset < 0) {
	type->description.fileoffset = description->fileoffset;
	type->description.length = description->length;
#ifdef TEXTS_IN_MEMORY
	type->description.ptr = description->ptr;
#endif
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
setTypeDisplayHint(type, displayHint)
    Type *type;
    String *displayHint;
{
    printDebug(5, "setTypeDisplayHint(%s, \"%s\")\n",
	       type->descriptor ? type->descriptor->name : "?",
	       displayHint->ptr);

    if (type->displayHint.fileoffset < 0) {
	type->displayHint.fileoffset = displayHint->fileoffset;
	type->displayHint.length = displayHint->length;
#ifdef TEXTS_IN_MEMORY
	type->displayHint.ptr = displayHint->ptr;
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
setTypeFileOffset(type, fileoffset)
    Type *type;
    off_t fileoffset;
{
    printDebug(5, "setTypeFileOffset(%s, %d)\n",
	       type->descriptor ? type->descriptor->name : "?",
	       fileoffset);

    if (type->fileoffset < 0) {
	type->fileoffset = fileoffset;
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
setTypeDecl(type, decl)
    Type     *type;
    smi_decl decl;
{
    printDebug(5, "setTypeDecl(%s, %s)\n",
	       type->descriptor ? type->descriptor->name : "?",
	       smiStringDecl(decl));

    if (type->decl == SMI_DECL_UNKNOWN) {
	type->decl = decl;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeFlags --
 *
 *      Add(!) flags to the flags of a given Type.
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
setTypeFlags(type, flags)
    Type *type;
    Flags flags;
{
    printDebug(5, "setTypeFlags(%s, %d)\n",
	       type->descriptor ? type->descriptor->name : "?",
	       flags);

    type->flags |= flags;
}



/*
 *----------------------------------------------------------------------
 *
 * findTypeByName --
 *
 *      Lookup a Type by a given Descriptor name.
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
findTypeByName(name)
    const char *name;
{
    Descriptor *descriptor;
    
    printDebug(4, "findTypeByName(\"%s\")", name);

    for (descriptor = firstDescriptor[KIND_TYPE];
	 descriptor; descriptor = descriptor->nextSameKind) {
	if ((!strcmp(descriptor->name, name)) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
	    printDebug(4, " = %p (%s)\n", descriptor->ptr, descriptor->name);
	    return (descriptor->ptr);
	}
    }
	
    printDebug(4, " = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findTypeByModulenameAndName --
 *
 *      Lookup a Type by a given Module and Descriptor name.
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
findTypeByModulenameAndName(modulename, name)
    const char *modulename;
    const char *name;
{
    Descriptor *descriptor;
    Module *module;
    
    printDebug(4, "findTypeByModulenameAndName(\"%s\", \"%s\")\n",
	       modulename, name);

    module = findModuleByName(modulename);
    if (module) {
	for (descriptor = module->firstDescriptor[KIND_TYPE];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if ((!strcmp(descriptor->name, name)) &&
		(!(descriptor->flags & FLAG_IMPORTED))) {
		printDebug(4, "... = %p (%s)\n", descriptor->ptr,
			   descriptor->name);
		return (descriptor->ptr);
	    }
	}
    }
	
    printDebug(4, "... = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findTypeByModuleAndName --
 *
 *      Lookup a Type by a given Module and Descriptor name.
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
findTypeByModuleAndName(module, name)
    Module *module;
    const char *name;
{
    Descriptor *descriptor;
    
    printDebug(4, "findTypeByModuleAndName(%s, \"%s\")",
	       module->descriptor->name, name);

    for (descriptor = module->firstDescriptor[KIND_TYPE];
	 descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	if ((!strcmp(descriptor->name, name)) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
	    printDebug(4, " = %p (%s)\n", descriptor->ptr, descriptor->name);
	    return (descriptor->ptr);
	}
    }
	
    printDebug(4, " = NULL\n");
    return (NULL);
}



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



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Macro functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



/*
 *----------------------------------------------------------------------
 *
 * addMacro --
 *
 *      Create a new Macro structure. Also creates a Descriptor if
 *      needed.
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
addMacro(name, module, fileoffset, flags, parser)
    const char *name;
    Module *module;
    off_t fileoffset;
    int flags;
    Parser *parser;
    
{
    Macro *macro;
    Descriptor *descriptor;

    printDebug(5, "addMacro(\"%s\", %s, %d, %d, parser)\n",
	       name, module->descriptor->name, fileoffset, flags);

    /*
     * Check wheather this macro already exists.
     */
    macro = findMacroByModuleAndName(module, name);

    /*
     * If it exists by declaration in this module, it's an error.
     */
    if (macro) {
	printError(parser, ERR_MACRO_ALREADY_EXISTS,
		   macro->descriptor->module->descriptor->name,
		   macro->descriptor->name);
    } else {

	/*
	 * Otherwise, create the Macro.
	 */
	macro = malloc(sizeof(Macro));
	if (!macro) {
	    printError(parser, ERR_ALLOCATING_MACRO, strerror(errno));
	    return (NULL);
	}
	    
	macro->module = module;
	macro->fileoffset = fileoffset;
	macro->flags = flags;
	    
	/*
	 * Create a Descriptor.
	 */
	descriptor = addDescriptor(name, module, KIND_MACRO, &macro,
				   flags & FLAGS_GENERAL, parser);
	macro->descriptor = descriptor;
    }
    
    return (macro);
}



/*
 *----------------------------------------------------------------------
 *
 * findMacroByModuleAndName --
 *
 *      Lookup a Macro by a given Module and Descriptor name.
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
findMacroByModuleAndName(module, name)
    Module *module;
    const char *name;
{
    Descriptor *descriptor;
    
    printDebug(4, "findMacroByModuleAndName(%s, \"%s\")",
	       module->descriptor->name, name);

    for (descriptor = module->firstDescriptor[KIND_MACRO];
	 descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	if ((!strcmp(descriptor->name, name)) &&
	    (!(descriptor->flags & FLAG_IMPORTED))) {
	    printDebug(4, " = %s\n", descriptor->name);
	    return (descriptor->ptr);
	}
    }
	
    printDebug(4, " = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findMacroByModulenameAndName --
 *
 *      Lookup a Macro by a given Module and Descriptor name.
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
findMacroByModulenameAndName(modulename, name)
    const char *modulename;
    const char *name;
{
    Descriptor *descriptor;
    Module *module;
    
    printDebug(4, "findMacroByModulenameAndName(\"%s\", \"%s\")\n",
	       modulename, name);

    module = findModuleByName(modulename);
    if (module) {
	for (descriptor = module->firstDescriptor[KIND_MACRO];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if ((!strcmp(descriptor->name, name)) &&
		(!(descriptor->flags & FLAG_IMPORTED))) {
		printDebug(4, "... = %s\n", descriptor->name);
		return (descriptor->ptr);
	    }
	}
    }
	
    printDebug(4, "... = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * General functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



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
    int i;
    Object *object;
    
    for (i = 0; i < NUM_KINDS; i++) {
	firstDescriptor[i] = NULL;
	lastDescriptor[i] = NULL;
    }

    /*
     * Initialize a root MibNode for the main MIB tree.
     */
    object = addObject(NULL, 0, NULL, FLAG_ROOT, NULL);
    rootNode = object->node;
    
    /*
     * Initialize a root MibNode for pending (forward referenced) nodes.
     */
    object = addObject(NULL, 0, NULL, FLAG_ROOT, NULL);
    pendingRootNode = object->node;
    
    /*
     * Initialize the top level well-known nodes, ccitt, iso, joint-iso-ccitt.
     */
    object = addObject(rootNode, 0, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("ccitt", NULL, KIND_OBJECT, &object, FLAG_PERMANENT, NULL);
    object = addObject(rootNode, 1, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("iso", NULL, KIND_OBJECT, &object, FLAG_PERMANENT, NULL);
    object = addObject(rootNode, 2, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("joint-iso-ccitt", NULL, KIND_OBJECT, &object,
		  FLAG_PERMANENT, NULL);

    /*
     * Initialize the well-known (ASN.1 (and SNMPv2-TC/SMI??)) Types.
     */
    
    /* ASN.1 */
    typeInteger = addType(NULL, SMI_SYNTAX_INTEGER,
			  NULL, FLAG_PERMANENT, NULL);
    addDescriptor("INTEGER", NULL, KIND_TYPE, &typeInteger,
		  FLAG_PERMANENT, NULL);
    
#if 0 /* SNMPv2-SMI */
    addDescriptor("Integer32", NULL, KIND_TYPE, &typeInteger,
		  FLAG_PERMANENT, NULL);
#endif

    /* ASN.1 */
    typeOctetString = addType(NULL, SMI_SYNTAX_OCTET_STRING,
			      NULL, FLAG_PERMANENT, NULL);
    addDescriptor("OCTET STRING", NULL, KIND_TYPE, &typeOctetString,
		  FLAG_PERMANENT, NULL);
    
    /* ASN.1 */
    typeObjectIdentifier = addType(NULL, SMI_SYNTAX_OBJECT_IDENTIFIER,
				   NULL, FLAG_PERMANENT, NULL);
    addDescriptor("OBJECT IDENTIFIER", NULL, KIND_TYPE, &typeObjectIdentifier,
		  FLAG_PERMANENT, NULL);
    
#if 0
    type = addType(NULL, SMI_SYNTAX_SEQUENCE, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("SEQUENCE", NULL, KIND_TYPE, &type, FLAG_PERMANENT, NULL);
#endif
    
#if 0
    type = addType(NULL, SMI_SYNTAX_SEQUENCE_OF, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("SEQUENCE_OF", NULL, KIND_TYPE, &type, FLAG_PERMANENT, NULL);
#endif

#if 0
    /* SNMPv2-SMI */
    typeIpAddress = addType(NULL, SMI_SYNTAX_IPADDRESS,
			    NULL, FLAG_PERMANENT, NULL);
    
    typeCounter32 = addType(NULL, SMI_SYNTAX_COUNTER32,
			    NULL, FLAG_PERMANENT, NULL);
    
    typeGauge32 = addType(NULL, SMI_SYNTAX_GAUGE32,
			  NULL, FLAG_PERMANENT, NULL);
    
    typeUnsigned32 = addType(NULL, SMI_SYNTAX_UNSIGNED32,
			     NULL, FLAG_PERMANENT, NULL);

    typeTimeTicks = addType(NULL, SMI_SYNTAX_TIMETICKS,
			    NULL, FLAG_PERMANENT, NULL);
    
    typeOpaque = addType(NULL, SMI_SYNTAX_OPAQUE,
			 NULL, FLAG_PERMANENT, NULL);
    
    typeCounter64 = addType(NULL, SMI_SYNTAX_COUNTER64,
			    NULL, FLAG_PERMANENT, NULL);
#endif
    
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
    const char *path;
    const char *modulename;
    int flags;
{
    Parser parser;
    char s[200];
    
    printDebug(3, "readMibFile(\"%s\", \"%s\", %d)\n",
	       path, modulename, flags);

    parser.path = strdup (path);
    parser.module = strdup(modulename);
    parser.flags = flags;
    
    parser.file = fopen(parser.path, "r");
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
	parser.firstImportDescriptor = NULL;
	parser.line      = 1;
	parser.column    = 1;
	parser.character = 1;
	yyparse((void *)&parser);
	leaveLexRecursion();
	fclose(parser.file);
	if (parser.flags & FLAG_STATS) {
	    sprintf(s, " (%d lines)", parser.line-1);
	    printError(&parser, ERR_STATISTICS, s);
	}
    }

    return (0);
}

