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
 * @(#) $Id: data.c,v 1.11 1998/11/02 19:29:02 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "error.h"
#include "data.h"



#define stringAccess(access) ( \
	(access == ACCESS_NOT)         ? "not-accessible" : \
	(access == ACCESS_NOTIFY)      ? "accessible-for-notify" : \
	(access == ACCESS_READ_ONLY)   ? "read-only" : \
	(access == ACCESS_READ_WRITE)  ? "read-write" : \
	(access == ACCESS_READ_CREATE) ? "read-create" : \
	(access == ACCESS_WRITE_ONLY)  ? "write-only" : \
					 "unknown" )

#define stringStatus(status) ( \
	(status == STATUS_CURRENT)     ? "current" : \
	(status == STATUS_DEPRECATED)  ? "deprecated" : \
	(status == STATUS_OBSOLETE)    ? "obsolete" : \
	(status == STATUS_MANDATORY)   ? "mandatory" : \
	(status == STATUS_OPTIONAL)    ? "optional" : \
					 "unknown" )

#define stringMacro(macro) ( \
	(macro == MACRO_UNKNOWN)           ? "UNKNOWN" : \
	(macro == MACRO_NONE)              ? "NONE" : \
	(macro == MACRO_OBJECTTYPE)        ? "OBJECTTYPE" : \
	(macro == MACRO_OBJECTIDENTITY)    ? "OBJECTIDENTITY" : \
	(macro == MACRO_MODULEIDENTITY)    ? "MODULEIDENTITY" : \
	(macro == MACRO_NOTIFICATIONTYPE)  ? "NOTIFICATIONTYPE" : \
	(macro == MACRO_TRAPTYPE)          ? "TRAPTYPE" : \
	(macro == MACRO_OBJECTGROUP)       ? "OBJECTGROUP" : \
	(macro == MACRO_NOTIFICATIONGROUP) ? "NOTIFICATIONGROUP" : \
	(macro == MACRO_MODULECOMPLIANCE)  ? "MODULECOMPLIANCE" : \
	(macro == MACRO_AGENTCAPABILITIES) ? "AGENTCAPABILITIES" : \
	(macro == MACRO_TC)                ? "TC" : \
					     "unknown" )

#define stringKind(kind) ( \
	(kind == KIND_ANY)                 ? "ANY" : \
	(kind == KIND_MODULE)              ? "MODULE" : \
	(kind == KIND_MACRO)               ? "MACRO" : \
	(kind == KIND_TYPE)                ? "TYPE" : \
	(kind == KIND_MIBNODE)             ? "MIBNODE" : \
	(kind == KIND_IMPORT)              ? "IMPORT" : \
					     "unknown" )

#define stringSyntax(syntax) ( \
	(syntax == SYNTAX_UNKNOWN)           ? "UNKNOWN" : \
	(syntax == SYNTAX_INTEGER)           ? "INTEGER" : \
	(syntax == SYNTAX_OCTET_STRING)      ? "OCTET_STRING" : \
	(syntax == SYNTAX_OBJECT_IDENTIFIER) ? "OBJECT_IDENTIFIER" : \
	(syntax == SYNTAX_SEQUENCE)          ? "SEQUENCE" : \
	(syntax == SYNTAX_SEQUENCE_OF)       ? "SEQUENCE_OF" : \
	(syntax == SYNTAX_IPADDRESS)         ? "IPADDRESS" : \
	(syntax == SYNTAX_COUNTER32)         ? "COUNTER32" : \
	(syntax == SYNTAX_GAUGE32)           ? "GAUGE32" : \
	(syntax == SYNTAX_UNSIGNED32)        ? "UNSIGNED32" : \
	(syntax == SYNTAX_TIMETICKS)         ? "TIMETICKS" : \
	(syntax == SYNTAX_OPAQUE)            ? "OPAQUE" : \
	(syntax == SYNTAX_COUNTER64)         ? "COUNTER64" : \
	(syntax == SYNTAX_CHOICE)            ? "CHOICE" : \
					       "unknown" )



Directory	*firstDirectory;	/* List of directories to search for */
Directory	*lastDirectory;		/* MIB files.                        */

Descriptor	*firstDescriptor[NUM_KINDS];
Descriptor	*lastDescriptor[NUM_KINDS];
					/* List of known Descriptors (OID    */
					/* labels, TCs Types, Modules).      */

MibNode		*rootMibNode;

MibNode		*pendingRootMibNode;

Type		*typeInteger, *typeOctetString, *typeObjectIdentifier;



/*
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * File and directory functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */



/*
 *----------------------------------------------------------------------
 *
 * addDirectory --
 *
 *      Add a directory to the list of directories to search
 *	for MIB module files.
 *
 * Results:
 *      A pointer to the new Directory structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

Directory *
addDirectory(dir)
    const char *dir;
{
    Directory *directory;
    
    printDebug(4, "addDirectory(\"%s\")\n", dir);

    directory = (Directory *)malloc(sizeof(Directory));
    if (!directory) {
	printError(NULL, ERR_ALLOCATING_DIRECTORY, strerror(errno));
	return (NULL);
    }
    
    strncpy(directory->dir, dir, sizeof(directory->dir)-1);

    directory->prev = lastDirectory;
    directory->next = NULL;
    if (lastDirectory) {
	lastDirectory->next = directory;
    } else {
	firstDirectory = directory;
    }
    lastDirectory = directory;
    
    return (directory);
}



/*
 *----------------------------------------------------------------------
 *
 * findFileByModulename --
 *
 *      Lookup a path by a given MIB module name.
 *
 * Results:
 *      A pointer to a static string containing the path or
 *	NULL if it's not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

char *
findFileByModulename(name)
    const char *name;
{
    struct stat buf;
    Directory *directory;
    static char path[MAX_PATH_LENGTH*2+2];
    
    for (directory = firstDirectory; directory; directory = directory->next) {
	
	sprintf(path, "%s/%s", directory->dir, name);
	if (!stat(path, &buf))
	    return (path);
	
	sprintf(path, "%s/%s.my", directory->dir, name);
	if (!stat(path, &buf))
	    return (path);
	
    }
    
    return (NULL);
}



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
    
    strncpy(module->path, path, sizeof(module->path)-1);
    module->fileoffset = fileoffset;
    module->flags = flags;
    for (i = 0; i < NUM_KINDS; i++) {
	module->firstDescriptor[i] = NULL;
	module->lastDescriptor[i] = NULL;
    }
    
    /*
     * Create a Descriptor.
     */
    module->descriptor = NULL;
    descriptor = addDescriptor(name, module, KIND_MODULE, module,
			       flags & FLAGS_GENERAL, parser);
    module->descriptor = descriptor;
    
    return (module);
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
	if (!strcmp(descriptor->name, name)) {
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
 *	from the list.
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
checkImportDescriptors(module, parser)
    Module *module;
    Parser *parser;
{
    Descriptor *descriptor;
    
    printDebug(4, "checkImportIdentifiers(%s, parser)\n",
	       module ? module->descriptor->name : "NULL");

    while (parser->firstImportDescriptor) {
	descriptor = parser->firstImportDescriptor;
	if (module) {
	    if (findMibNodeByModuleAndName(module, descriptor->name)) {
		;
	    } else if (findTypeByModuleAndName(module, descriptor->name)) {
		;
	    } else if (findMacroByModuleAndName(module, descriptor->name)) {
		;
	    } else {
		printError(parser, ERR_IDENTIFIER_NOT_IN_MODULE,
			   descriptor->name, module->descriptor->name);
	    }
	}
	
	parser->firstImportDescriptor = descriptor->nextSameModuleAndKind;
	free(descriptor);
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
 *      None.
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
    MibNode *pending, *next;
    Type *t;
    
    printDebug(5, "addDescriptor(\"%s\", %s, %s, 0x%p, %d, parser)\n",
	       name, module &&
	         module->descriptor ? module->descriptor->name : "NULL",
	       stringKind(kind), ptr, flags);

    /*
     * If this new descriptor is found as pending type,
     * we have to complete that Type and Descriptor structs instead of
     * creating a new descriptor.
     */
    if ((module) && (kind == KIND_TYPE)) {
	t = findTypeByModuleAndName(module, name);
	if (t && (t->flags & FLAG_INCOMPLETE) && !(flags & FLAG_INCOMPLETE)) {
	    t->parent = ((Type *)ptr)->parent;
	    t->syntax = ((Type *)ptr)->syntax;
	    t->macro = ((Type *)ptr)->macro;
	    t->status = ((Type *)ptr)->status;
	    t->fileoffset = ((Type *)ptr)->fileoffset;
	    t->flags = ((Type *)ptr)->flags;
	    t->displayHint = ((Type *)ptr)->displayHint;
	    t->description = ((Type *)ptr)->description;
	    free(ptr);
	    return t->descriptor;
	}
    }
    descriptor = (Descriptor *)malloc(sizeof(Descriptor));
    if (!descriptor) {
	printError(parser, ERR_ALLOCATING_DESCRIPTOR, strerror(errno));
	return (NULL);
    }

    strncpy(descriptor->name, name, sizeof(descriptor->name)-1);
    descriptor->ptr = ptr;
    descriptor->module = module;
    descriptor->kind = kind;
    descriptor->flags = flags;

    /*
     * TODO: during development, there might be descriptors defined
     * with ptr == NULL.
     */
    if (ptr) {
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
	case KIND_MIBNODE:
	    ((MibNode *)(descriptor->ptr))->descriptor = descriptor;
	    break;
	case KIND_ANY:
	case KIND_IMPORT:
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
     * list (at depth==1 in pendingRootMibNode), we have to move the
     * corresponding subtree to the main tree.
     */
    if ((module) && (kind == KIND_MIBNODE) &&
	(((MibNode *)ptr)->parent != pendingRootMibNode)) {
	
	/*
	 * check each root of pending subtrees. if its the just defined
	 * oid, then move it to the main tree.
	 */
	for (pending = pendingRootMibNode->firstChild; pending;
	     pending = next) {

	    /*
	     * probably we change the contents of `pending', so remember
	     * the next pointer.
	     */
	    next = pending->next;
	    
	    if (!strcmp(pending->descriptor->name, name)) {

		/*
		 * remove `pending' from the pendingRootMibNode tree.
		 */
		if (pending->prev) {
		    pending->prev->next = pending->next;
		} else {
		    pendingRootMibNode->firstChild = pending->next;
		}
		if (pending->next) {
		    pending->next->prev = pending->prev;
		} else {
		    pendingRootMibNode->lastChild = pending->prev;
		}

		/*
		 * copy contents of the new node to pending.
		 */
		olddescriptor = pending->descriptor;
		pending->flags = ((MibNode *)ptr)->flags;
		pending->macro = ((MibNode *)ptr)->macro;
		pending->fileoffset = ((MibNode *)ptr)->fileoffset;
		pending->subid = ((MibNode *)ptr)->subid;
		pending->descriptor = ((MibNode *)ptr)->descriptor;

		/*
		 * now link pending into ptr's place.
		 */
		pending->parent = ((MibNode *)ptr)->parent;
		pending->next = ((MibNode *)ptr)->next;
		pending->prev = ((MibNode *)ptr)->prev;
		if (((MibNode *)ptr)->parent->firstChild == ptr) {
		    ((MibNode *)ptr)->parent->firstChild = pending;
		} else {
		    ((MibNode *)ptr)->prev->next = pending;
		}
		if (((MibNode *)ptr)->parent->lastChild == ptr) {
		    ((MibNode *)ptr)->parent->lastChild = pending;
		} else {
		    ((MibNode *)ptr)->next->prev = pending;
		}

		/*
		 * finally, delete the unneeded node and descriptor.
		 */
		deleteDescriptor(olddescriptor);
		free(ptr);
		break;
	    }
	}
	
    }

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
    
    printDebug(5, "findDescriptor(name=\"%s\", module=%s, kind=%s)", name,
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
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *
 * Mib node functions.
 *
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 *----------------------------------------------------------------------
 */


    
/*
 *----------------------------------------------------------------------
 *
 * addMibNode --
 *
 *      Create a new MibNode or update an existing one. Also creates
 *	a Descriptor if needed and updates other MibNodes according
 *	the PendingMibNode information.
 *
 * Results:
 *      A pointer to the new MibNode structure or
 *	NULL if terminated due to an error.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

MibNode *
addMibNode(parent, subid, module, flags, parser)
    MibNode *parent;
    unsigned int subid;
    Module *module;
    Flags flags;
    Parser *parser;
{
    MibNode *node;
    MibNode *c;
    
    printDebug(5, "addMibNode(%s, %d, %s, %d, parser)\n",
	       parent &&
	         parent->descriptor ? parent->descriptor->name : "NULL",
	       subid,
	       module &&
	         module->descriptor ? module->descriptor->name : "NULL",
	       flags);

    node = (MibNode *)malloc(sizeof(MibNode));
    if (!node) {
	printError(parser, ERR_ALLOCATING_MIBNODE, strerror(errno));
	return (NULL);
    }

    node->module = module;
    node->subid = subid;
    node->fileoffset = 0;
    node->macro = MACRO_UNKNOWN;
    node->flags = flags;
    node->descriptor = NULL;
    node->description.fileoffset = 0;
    node->description.length = 0;
#ifdef TEXTS_IN_MEMORY
    node->description.ptr = NULL;
#endif
    
    /*
     * Link it into the tree.
     */
    node->parent = parent;
    node->firstChild = NULL;
    node->lastChild = NULL;

    if (parent) {
	if (parent->firstChild) {
	    for (c = parent->firstChild; c && (c->subid < subid); c = c->next);
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

    return (node);
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeSyntax --
 *
 *      Set the syntax (pointer to a Type struct) of a given MibNode.
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
setMibNodeSyntax(node, type)
    MibNode *node;
    Type *type;
{
    printDebug(5, "setMibNodeSyntax(%s, %s)\n",
	       node->descriptor ? node->descriptor->name : "?",
	       type && type->descriptor ? type->descriptor->name : "NULL");

    node->type = type;
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeAccess --
 *
 *      Set the access of a given MibNode.
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
setMibNodeAccess(node, access)
    MibNode *node;
    Access access;
{
    printDebug(5, "setMibNodeAccess(%s, %s)\n",
	       node->descriptor ? node->descriptor->name : "?",
	       stringAccess(access));

    node->access = access;
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeStatus --
 *
 *      Set the status of a given MibNode.
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
setMibNodeStatus(node, status)
    MibNode *node;
    Status status;
{
    printDebug(5, "setMibNodeStatus(%s, %s)\n",
	       node->descriptor ? node->descriptor->name : "?",
	       stringStatus(status));

    node->status = status;
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeDescription --
 *
 *      Set the description of a given MibNode.
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
setMibNodeDescription(node, description)
    MibNode *node;
    String *description;
{
    printDebug(5, "setMibNodeDescription(%s, \"%s\")\n",
	       node->descriptor ? node->descriptor->name : "?",
	       description->ptr ? description->ptr : "...");

    node->description.fileoffset = description->fileoffset;
    node->description.length = description->length;
#ifdef TEXTS_IN_MEMORY
    node->description.ptr = description->ptr;
#endif
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeFileOffset --
 *
 *      Set the fileoffset of a given MibNode.
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
setMibNodeFileOffset(node, fileoffset)
    MibNode *node;
    off_t fileoffset;
{
    printDebug(5, "setMibNodeFileOffset(%s, %d)\n",
	       node->descriptor ? node->descriptor->name : "?",
	       fileoffset);

    node->fileoffset = fileoffset;
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeMacro --
 *
 *      Set the macro of a given MibNode.
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
setMibNodeMacro(node, macro)
    MibNode *node;
    DeclMacro macro;
{
    printDebug(5, "setMibNodeMacro(%s, %s)\n",
	       node->descriptor ? node->descriptor->name : "?",
	       stringMacro(macro));

    node->macro = macro;
}



/*
 *----------------------------------------------------------------------
 *
 * setMibNodeFlags --
 *
 *      Add(!) flags to the flags of a given MibNode.
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
setMibNodeFlags(node, flags)
    MibNode *node;
    Flags flags;
{
    printDebug(5, "setMibNodeFlags(%s, %d)\n",
	       node->descriptor ? node->descriptor->name : "?",
	       flags);

    node->flags |= flags;
}



#if 0
/*
 *----------------------------------------------------------------------
 *
 * changeMibNode --
 *
 *	Set the Descriptor if not yet set and descriptor != NULL.
 *      Set the DeclMacro if macro != MACRO_NONE.
 *	Add Flags if flags != 0.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void changeMibNode(mibnode, descriptor, macro, flags)
    Descriptor *descriptor;
    MibNode *mibnode;
    DeclMacro macro;
    Flags flags;
{
    if ((descriptor) && (!mibnode->descriptor)) {
	mibnode->descriptor = descriptor;
    }
    if (macro != MACRO_NONE) {
	mibnode->macro = macro;
    }
    if (flags) {
	mibnode->flags |= flags;
    }
}
#endif



#if 0
/*
 *----------------------------------------------------------------------
 *
 * findMibNodeByOID --
 *
 *      Lookup a MibNode by a given OID value.
 *
 * Results:
 *      A pointer to the MibNode structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

MibNode *
findMibNodeByOID(oid)
    const char *oid;
{
    Descriptor *descriptor;
    
    printDebug(4, "findMibNodeByOID(\"%s\")", oid);

    for (descriptor = firstDescriptor[KIND_MIBNODE]; descriptor;
	 descriptor = descriptor->nextSameKind) {
	if (!strcmp(((MibNode *)descriptor->ptr)->oid, oid)) {
	    printDebug(4, " = \"%s\"\n", descriptor->name);
	    return (descriptor->ptr);
	}
    }
    
    printDebug(4, " = NULL\n");

    return (NULL);
}
#endif


/*
 *----------------------------------------------------------------------
 *
 * findMibNodeByParentAndSubid --
 *
 *      Lookup a MibNode by a given parent and subid value.
 *
 * Results:
 *      A pointer to the MibNode structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

MibNode *
findMibNodeByParentAndSubid(parent, subid)
    MibNode *parent;
    unsigned int subid;
{
    MibNode *node;
    
    printDebug(4, "findMibNodeByParentAndSubid(...%d, %d)",
	       parent ? parent->subid : 0, subid);

    if (parent) {
	for (node = parent->firstChild; node; node = node->next) {
	    if (node->subid == subid) {
		printDebug(4, " = \"%s\"\n", node->descriptor->name);
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
 * findMibNodeByName --
 *
 *      Lookup a MibNode by a given Descriptor name. Note, that
 *	there might be more than on MibNodes with the same name.
 *	In this case, it is undefined which MibNode is returned.
 *
 * Results:
 *      A pointer to the MibNode structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

MibNode *
findMibNodeByName(name)
    const char *name;
{
    Descriptor *descriptor;

    printDebug(4, "findMibNodeByName(\"%s\")", name);

    for (descriptor = firstDescriptor[KIND_MIBNODE];
	 descriptor; descriptor = descriptor->nextSameKind) {
	if (!strcmp(descriptor->name, name)) {
	    /*
	     * We return the first matching node.
	     * TODO: probably we should check if there are more matching
	     *       nodes, and give a warning if there's another one.
	     */
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
 * findMibNodeByModulenameAndName --
 *
 *      Lookup a MibNode by a given Module and Descriptor name.
 *
 * Results:
 *      A pointer to the MibNode structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

MibNode *
findMibNodeByModulenameAndName(modulename, name)
    const char *modulename;
    const char *name;
{
    Descriptor *descriptor;
    Module *module;
    
    printDebug(4, "findMibNodeByModulenameAndName(\"%s\", \"%s\")",
	       modulename, name);

    module = findModuleByName(modulename);
    if (module) {
	for (descriptor = module->firstDescriptor[KIND_MIBNODE];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if (!strcmp(descriptor->name, name)) {
		printDebug(4, " = %s\n", descriptor->name);
		return (descriptor->ptr);
	    }
	}
    }
    
    printDebug(4, " = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * findMibNodeByModuleAndName --
 *
 *      Lookup a MibNode by a given Module and Descriptor name.
 *
 * Results:
 *      A pointer to the MibNode structure or
 *	NULL if it is not found.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

MibNode *
findMibNodeByModuleAndName(module, name)
    Module *module;
    const char *name;
{
    Descriptor *descriptor;
    
    printDebug(4, "findMibNodeByModuleAndName(%s, \"%s\")",
	       module ? module->descriptor->name : "NULL" , name);

    if (module) {
	for (descriptor = module->firstDescriptor[KIND_MIBNODE];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if (!strcmp(descriptor->name, name)) {
		printDebug(4, " = %s\n", descriptor->name);
		return (descriptor->ptr);
	    }
	}
    } else {
	return (findMibNodeByName(name));
    }
    
    printDebug(4, " = NULL\n");
    return (NULL);
}



/*
 *----------------------------------------------------------------------
 *
 * deleteMibTree --
 *
 *      delete MIB subtree including its descriptors.
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
deleteMibTree(root)
    MibNode *root;
{
    MibNode *c;
    
    printDebug(5, "deleteMibTree(%s)\n",
	       root && root->descriptor ? root->descriptor->name : "NULL");

    if (root) {

	/*
	 * first, walk down the tree recursively.
	 */
	for (c = root->firstChild; c; c = c->next) {
	    deleteMibTree(c);
	}
	
	/*
	 * delete the corresponding descriptor.
	 */
	if (root->descriptor) {
	    deleteDescriptor(root->descriptor);
	}
	
	/*
	 * remove from linkage in the same depth.
	 */
	if (root->prev) {
	    root->prev->next = root->next;
	}
	if (root->next) {
	    root->next->prev = root->prev;
	}

	/*
	 * no need to adapt linkage below, since all childs are also deleted.
	 */

	/*
	 * adapt parent's linkage.
	 */
	if (root->parent->firstChild == root) {
	    root->parent->firstChild = root->next;
	}
	if (root->parent->lastChild == root) {
	    root->parent->lastChild = root->prev;
	}
	
	free(root);
    }
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
    MibNode *root;
    const char *prefix;
{
    MibNode *c;
    char s[200];
    
    if (root) {
	if (root == rootMibNode) {
	    s[0] = 0;
	} else {
	    if (root->descriptor) {
		if (root->flags & FLAG_NOSUBID) {
		    sprintf(s, "%s.%s(?)", prefix,
			    root->descriptor->name);
		} else {
		    sprintf(s, "%s.%s(%d)", prefix,
			    root->descriptor->name, root->subid);
		}
	    } else {
		if (root->flags & FLAG_NOSUBID) {
		    sprintf(s, "%s.?", prefix);
		} else {
		    sprintf(s, "%s.%d", prefix, root->subid);
		}
	    }
	    fprintf(stderr, "%s", s);
	    if (root->module) {
		fprintf(stderr, " == %s!%s",
			root->module->descriptor->name,
			root->descriptor->name);
	    }
	    if (root->type && root->type->descriptor) {
		fprintf(stderr, "  [%s]", root->type->descriptor->name);
	    } else {
		if (root->type && root->type->parent &&
		    root->type->parent->descriptor) {
		    fprintf(stderr, "  [[%s]]",
			    root->type->parent->descriptor->name);
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
    MibNode *root;
{
    MibNode *c;
    char s[200];
    
    if (root) {
	if (root != rootMibNode) {
	    if ((root->flags & FLAG_MODULE) &&
		(root->descriptor && root->parent->descriptor)) {
		sprintf(s, "%s.%d",
			root->parent->descriptor->name, root->subid);
		if (root->macro == MACRO_OBJECTTYPE) {
		    printf("%-19s %-19s %-15s %-15s %s\n",
			   root->descriptor->name,
			   s,
			   "<type>",
			   stringAccess(root->access),
			   stringStatus(root->status));
		} else {
		    printf("%-19s %s\n",
			   root->descriptor->name,
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
    Syntax     syntax;
    Module     *module;
    Flags      flags;
    Parser     *parser;
{
    Type *type;

    printDebug(5, "addType(%s, %s, %s, %d, parser)\n",
	       parent &&
	         parent->descriptor ? parent->descriptor->name : "NULL",
	       stringSyntax(syntax),
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
    if (parent) {
	type->syntax = parent->syntax;
    } else {
	type->syntax = syntax;
    }
    type->macro = MACRO_UNKNOWN;
    type->flags = flags;
    type->descriptor = NULL;
    type->description.fileoffset = 0;
    type->description.length = 0;
#ifdef TEXTS_IN_MEMORY
    type->description.ptr = NULL;
#endif

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
    Type *type;
    Status status;
{
    printDebug(5, "setTypeStatus(%s, %s)\n",
	       type->descriptor ? type->descriptor->name : "?",
	       stringStatus(status));

    type->status = status;
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

    type->description.fileoffset = description->fileoffset;
    type->description.length = description->length;
#ifdef TEXTS_IN_MEMORY
    type->description.ptr = description->ptr;
#endif
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

    type->displayHint.fileoffset = displayHint->fileoffset;
    type->displayHint.length = displayHint->length;
#ifdef TEXTS_IN_MEMORY
    type->displayHint.ptr = displayHint->ptr;
#endif
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

    type->fileoffset = fileoffset;
}



/*
 *----------------------------------------------------------------------
 *
 * setTypeMacro --
 *
 *      Set the macro of a given Type.
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
setTypeMacro(type, macro)
    Type *type;
    DeclMacro macro;
{
    printDebug(5, "setTypeMacro(%s, %s)\n",
	       type->descriptor ? type->descriptor->name : "?",
	       stringMacro(macro));

    type->macro = macro;
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
	if (!strcmp(descriptor->name, name)) {
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
    
    printDebug(4, "findTypeByModulenameAndName(\"%s\", \"%s\")",
	       modulename, name);

    module = findModuleByName(modulename);
    if (module) {
	for (descriptor = module->firstDescriptor[KIND_TYPE];
	     descriptor; descriptor = descriptor->nextSameModuleAndKind) {
	    if (!strcmp(descriptor->name, name)) {
		printDebug(4, " = %s\n", descriptor->name);
		return (descriptor->ptr);
	    }
	}
    }
	
    printDebug(4, " = NULL\n");
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
	if (!strcmp(descriptor->name, name)) {
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
	for (t = d->ptr; t; t = t->parent) {
	    fprintf(stderr, "%s", t->descriptor && t->descriptor->name ? t->descriptor->name : "?");
	    if (t->syntax) {
		fprintf(stderr, "(%s)", stringSyntax(t->syntax));
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
	descriptor = addDescriptor(name, module, KIND_MACRO, macro,
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
	if (!strcmp(descriptor->name, name)) {
	    printDebug(4, " = %s\n", descriptor->name);
	    return (descriptor->ptr);
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
    MibNode *node;
    
    firstDirectory = NULL;
    for (i = 0; i < NUM_KINDS; i++) {
	firstDescriptor[i] = NULL;
	lastDescriptor[i] = NULL;
    }

    /*
     * Initialize a root MibNode for the main MIB tree.
     */
    rootMibNode = addMibNode(NULL, 0, NULL, FLAG_ROOT, NULL);
    
    /*
     * Initialize a root MibNode for pending (forward referenced) nodes.
     */
    pendingRootMibNode = addMibNode(NULL, 0, NULL, FLAG_ROOT, NULL);

    /*
     * Initialize the top level well-known nodes, ccitt, iso, joint-iso-ccitt.
     */
    node = addMibNode(rootMibNode, 0, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("ccitt", NULL, KIND_MIBNODE, node, FLAG_PERMANENT, NULL);
    node = addMibNode(rootMibNode, 1, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("iso", NULL, KIND_MIBNODE, node, FLAG_PERMANENT, NULL);
    node = addMibNode(rootMibNode, 2, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("joint-iso-ccitt", NULL, KIND_MIBNODE, node,
		  FLAG_PERMANENT, NULL);

    /*
     * Initialize the well-known (ASN.1 (and SNMPv2-TC/SMI??)) Types.
     */
    
    /* ASN.1 */
    typeInteger = addType(NULL, SYNTAX_INTEGER,
			  NULL, FLAG_PERMANENT, NULL);
    addDescriptor("INTEGER", NULL, KIND_TYPE, typeInteger,
		  FLAG_PERMANENT, NULL);
    
#if 0 /* SNMPv2-SMI */
    addDescriptor("Integer32", NULL, KIND_TYPE, typeInteger,
		  FLAG_PERMANENT, NULL);
#endif

    /* ASN.1 */
    typeOctetString = addType(NULL, SYNTAX_OCTET_STRING,
			      NULL, FLAG_PERMANENT, NULL);
    addDescriptor("OCTET STRING", NULL, KIND_TYPE, typeOctetString,
		  FLAG_PERMANENT, NULL);
    
    /* ASN.1 */
    typeObjectIdentifier = addType(NULL, SYNTAX_OBJECT_IDENTIFIER,
				   NULL, FLAG_PERMANENT, NULL);
    addDescriptor("OBJECT IDENTIFIER", NULL, KIND_TYPE, typeObjectIdentifier,
		  FLAG_PERMANENT, NULL);
    
#if 0
    type = addType(NULL, SYNTAX_SEQUENCE, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("SEQUENCE", NULL, KIND_TYPE, type, FLAG_PERMANENT, NULL);
#endif
    
#if 0
    type = addType(NULL, SYNTAX_SEQUENCE_OF, NULL, FLAG_PERMANENT, NULL);
    addDescriptor("SEQUENCE_OF", NULL, KIND_TYPE, type, FLAG_PERMANENT, NULL);
#endif

#if 0
    /* SNMPv2-SMI */
    typeIpAddress = addType(NULL, SYNTAX_IPADDRESS,
			    NULL, FLAG_PERMANENT, NULL);
    
    typeCounter32 = addType(NULL, SYNTAX_COUNTER32,
			    NULL, FLAG_PERMANENT, NULL);
    
    typeGauge32 = addType(NULL, SYNTAX_GAUGE32,
			  NULL, FLAG_PERMANENT, NULL);
    
    typeUnsigned32 = addType(NULL, SYNTAX_UNSIGNED32,
			     NULL, FLAG_PERMANENT, NULL);

    typeTimeTicks = addType(NULL, SYNTAX_TIMETICKS,
			    NULL, FLAG_PERMANENT, NULL);
    
    typeOpaque = addType(NULL, SYNTAX_OPAQUE,
			 NULL, FLAG_PERMANENT, NULL);
    
    typeCounter64 = addType(NULL, SYNTAX_COUNTER64,
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

    strncpy(parser.path, path, sizeof(parser.path)-1);
    strncpy(parser.module, modulename, sizeof(parser.module)-1);
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

