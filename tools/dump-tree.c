/*
 * dump-tree.c --
 *
 *      Operations to dump the OID tree in a human readable format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-tree.c,v 1.6 2000/02/10 10:09:45 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "smi.h"
#include "smidump.h"


static char *currentModule;


static char *getFlags(SmiNode *smiNode)
{

    switch (smiNode->access) {
    case SMI_ACCESS_UNKNOWN:
	return "---";
    case SMI_ACCESS_NOT_ACCESSIBLE:
	return "---";
    case SMI_ACCESS_NOTIFY:
	return "--n";
    case SMI_ACCESS_READ_ONLY:
	return "r-n";
    case SMI_ACCESS_READ_WRITE:
	return "rwn";
    }

    return "";
}



static char getStatusChar(SmiStatus status)
{
    switch (status) {
    case SMI_STATUS_UNKNOWN:
	return '+';
    case SMI_STATUS_CURRENT:
	return '+';
    case SMI_STATUS_DEPRECATED:
	return 'x';
    case SMI_STATUS_MANDATORY:
        return '+';
    case SMI_STATUS_OPTIONAL:
	return '+';
    case SMI_STATUS_OBSOLETE:
	return 'o';
    }

    return ' ';
}



static char *getTypeName(SmiNode *smiNode)
{
    char *type;
    SmiType *smiType, *parentType;

    smiType = smiGetNodeType(smiNode);

    if (!smiType || smiNode->nodekind == SMI_NODEKIND_TABLE)
	return NULL;
    
    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
	parentType = smiGetParentType(smiType);
	smiType = parentType;
    }

    type = xstrdup(smiType->name);
    return type;
}



static void printIndex(SmiNode *smiNode)
{
    char *indexname;
    int  i;
    SmiElement *smiElement;
    
    indexname = NULL;
    for (i = -1, smiElement = smiGetFirstElement(smiNode);
	 smiElement; smiElement = smiGetNextElement(smiElement), i++) {
	if (i > 0) printf(",");
	if (indexname) {
	    printf(indexname);
	}
	indexname = smiGetElementNode(smiElement)->name;
    }
    if (indexname) {
	printf("%s%s%s",
	       (i > 0) ? "," : "",
	       (smiNode->implied) ? "*" : "",
	       indexname);
    }
}



static void printObjects(SmiNode *smiNode)
{
    char *objectname;
    int  i;
    SmiElement *smiElement;

    objectname = NULL;
    for (i = -1, smiElement = smiGetFirstElement(smiNode);
	 smiElement;
	 smiElement = smiGetNextElement(smiElement), i++) {
	if (i > 0) printf(",");
	if (objectname) {
	    printf(objectname);
	}
	objectname = smiGetElementNode(smiElement)->name;
    }
    if (objectname) {
	printf("%s%s", (i > 0) ? "," : "", objectname);
    }
}



static int pruneSubTree(SmiNode *smiNode)
{
    SmiNode *childNode;

    if (! smiNode) {
	return 1;
    }

    if (strcmp(currentModule, smiGetNodeModule(smiNode)->name) == 0) {
	return 0;
    }

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (! pruneSubTree(childNode)) {
	    return 0;
	}
    }
    return 1;
}



static void dumpSubTree(SmiNode *smiNode, char *prefix, int typefieldlen)
{
    SmiNode     *childNode, *indexNode;
    SmiNodekind lastNodeKind = SMI_NODEKIND_UNKNOWN;
    SmiType     *type;
    int         i = 0, cnt, prefixlen, newtypefieldlen = 8;
    char        c = 0;
    char	*typename;

    if (smiNode) {
	prefixlen = strlen(prefix);
	switch (smiNode->nodekind) {
	case SMI_NODEKIND_SCALAR:
	case SMI_NODEKIND_COLUMN:
	    c = prefix[prefixlen-1];
	    prefix[prefixlen-1] = getStatusChar(smiNode->status);
	    typename = getTypeName(smiNode);
	    printf("%s-- %s %-*s %s(%u)\n",
		   prefix,
		   getFlags(smiNode),
		   typefieldlen,
		   typename,
		   smiNode->name,
		   smiNode->oid[smiNode->oidlen-1]);
	    xfree(typename);
	    if (c) {
		prefix[prefixlen-1] = c;
	    }
	    break;
	case SMI_NODEKIND_ROW:
	    if (prefixlen) {
		c = prefix[prefixlen-1];
		prefix[prefixlen-1] = getStatusChar(smiNode->status);
	    }
	    printf("%s--%s(%u) [", prefix,
		   smiNode->name,
		   smiNode->oid[smiNode->oidlen-1]);
	    switch (smiNode->indexkind) {
	    case SMI_INDEX_INDEX:
	    case SMI_INDEX_REORDER:
		printIndex(smiNode);
		break;
	    case SMI_INDEX_EXPAND:  /* TODO: we have to do more work here! */
		break;
	    case SMI_INDEX_AUGMENT:
	    case SMI_INDEX_SPARSE:
		indexNode = smiGetRelatedNode(smiNode);
		if (indexNode) {
		    printIndex(indexNode);
		}
		break;
	    case SMI_INDEX_UNKNOWN:
		break;	    
	    }
	    printf("]\n");
	    if (c) {
		prefix[prefixlen-1] = c;
	    }
	    break;
	case SMI_NODEKIND_NOTIFICATION:
	    if (prefixlen) {
		c = prefix[prefixlen-1];
		prefix[prefixlen-1] = getStatusChar(smiNode->status);
	    }
	    printf("%s--%s(%u) [", prefix,
		   smiNode->name,
		   smiNode->oid[smiNode->oidlen-1]);
	    printObjects(smiNode);
	    printf("]\n");
	    if (c) {
		prefix[prefixlen-1] = c;
	    }
	    break;
	default:
	    if (prefixlen) {
		c = prefix[prefixlen-1];
		prefix[prefixlen-1] = getStatusChar(smiNode->status);
	    }
	    printf("%s--%s(%u)\n", prefix,
		   smiNode->name,
		   smiNode->oid[smiNode->oidlen-1]);
	    if (c) {
		prefix[prefixlen-1] = c;
	    }
	}
	for (childNode = smiGetFirstChildNode(smiNode), cnt = 0;
	     childNode;
	     childNode = smiGetNextChildNode(childNode)) {
	    if (! pruneSubTree(childNode)) {
		type = smiGetNodeType(childNode);
		if (type) {
		    typename = getTypeName(childNode);
		    if (typename) {
			if (strlen(typename) > newtypefieldlen) {
			    newtypefieldlen = strlen(typename);
			}
			xfree(typename);
		    }
		}
		cnt++;
	    }
	}
	for (childNode = smiGetFirstChildNode(smiNode);
	     childNode;
	     childNode = smiGetNextChildNode(childNode)) {
	    char *newprefix;
	    if (pruneSubTree(childNode)) continue;
	    i++;
	    if ((childNode->nodekind != SMI_NODEKIND_COLUMN
		&& childNode->nodekind != SMI_NODEKIND_SCALAR)
		|| (lastNodeKind != childNode->nodekind)) {
		printf("%s  |\n", prefix);
	    }
	    newprefix = xmalloc(strlen(prefix)+10);
	    strcpy(newprefix, prefix);
	    if (cnt == 1 || cnt == i) {
		strcat(newprefix, "   ");
	    } else {
		strcat(newprefix, "  |");
	    }
	    dumpSubTree(childNode, newprefix, newtypefieldlen);
	    xfree(newprefix);
	    lastNodeKind = childNode->nodekind;
	}
    }
}



int dumpTree(char *modulename, int flags)
{
    SmiNode *smiNode;

    currentModule = modulename;

    if (! (flags & SMIDUMP_FLAG_SILENT)) {
	printf("# %s registration tree (generated by smidump "
	       VERSION ")\n\n", modulename);
    }

    smiNode = smiGetNode(NULL, "iso");
    if (smiNode) {
	dumpSubTree(smiNode, "", 0);
    }
    
    return 0;
}
