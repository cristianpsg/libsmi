/*
 * dump-data.c --
 *
 *      Operations to dump libsmi internal data structures.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-data.c,v 1.9 1999/06/07 15:44:12 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include "smi.h"


static char *currentModule;


static char *getFlags(SmiNode *smiNode)
{
    static char flags[5];

    switch (smiNode->access) {
    case SMI_ACCESS_UNKNOWN:
	flags[0] = ' ';
    case SMI_ACCESS_NOT_ACCESSIBLE:
	flags[0] = '-'; break;
    case SMI_ACCESS_NOTIFY:
	flags[0] = 'N'; break;
    case SMI_ACCESS_READ_ONLY:
	flags[0] = 'R'; break;
    case SMI_ACCESS_READ_WRITE:
	flags[0] = 'W'; break;
    }

    flags[1] = '-';

    switch (smiNode->status) {
    case SMI_STATUS_UNKNOWN:
	flags[2] = ' '; break;
    case SMI_STATUS_CURRENT:
	flags[2] = 'C'; break;
    case SMI_STATUS_DEPRECATED:
	flags[2] = 'D'; break;
    case SMI_STATUS_OBSOLETE:
	flags[2] = 'O'; break;
    case SMI_STATUS_MANDATORY:
	flags[2] = 'M'; break;
    case SMI_STATUS_OPTIONAL:
	flags[2] = 'P'; break;
    }

    flags[3] = 0;

    return flags;
}



static void printIndex(SmiNode *smiNode)
{
    char *indexname;
    int  i;
    SmiIndex *smiIndex;
    
    indexname = NULL;
    for (i = -1, smiIndex = smiGetFirstIndex(smiNode);
	 smiIndex; smiIndex = smiGetNextIndex(smiIndex), i++) {
	if (i > 0) printf(",");
	if (indexname) {
	    printf(indexname);
	}
	indexname = smiIndex->name;
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
    SmiNode *objectNode;
    
    objectname = NULL;
    for (i = -1, objectNode = smiGetFirstObjectNode(smiNode);
	 objectNode;
	 objectNode = smiGetNextObjectNode(smiNode, objectNode), i++) {
	if (i > 0) printf(",");
	if (objectname) {
	    printf(objectname);
	}
	objectname = objectNode->name;
    }
    if (objectname) {
	printf("%s%s", (i > 0) ? "," : "", objectname);
    }
}



static int pruneSubTree(SmiNode *smiNode)
{
    SmiNode *childNode;
    
    if (smiNode) {
	if (strcmp(currentModule, smiNode->module) == 0) {
	    return 0;
	} else {
	    for (childNode = smiGetFirstChildNode(smiNode);
		 childNode;
		 childNode = smiGetNextChildNode(childNode)
		) {
		if (! pruneSubTree(childNode)) {
		    return 0;
		}
	    }
	    return 1;
	}
    } else {
	return 1;
    }
}



static void dumpOidSubTree(SmiNode *smiNode, char *prefix, int typefieldlen)
{
    SmiNode     *childNode;
    SmiNodekind lastNodeKind = SMI_NODEKIND_UNKNOWN;
    int         i = 0, cnt, prefixlen, newtypefieldlen = 8;
    char        c = 0;

    if (smiNode) {
	prefixlen = strlen(prefix);
	switch (smiNode->nodekind) {
	case SMI_NODEKIND_SCALAR:
	case SMI_NODEKIND_COLUMN:
	    c = prefix[prefixlen-1]; prefix[prefixlen-1] = '+';
	    printf("%s-- %s %-*s %s(%u)\n", prefix,
		   getFlags(smiNode),
		   typefieldlen,
		   smiNode->typename,
		   smiNode->name,
		   smiNode->oid[smiNode->oidlen-1]);
	    if (c) {
		prefix[prefixlen-1] = c;
	    }
	    break;
	case SMI_NODEKIND_ROW:
	    if (prefixlen) {
		c = prefix[prefixlen-1]; prefix[prefixlen-1] = '+';
	    }
	    printf("%s--%s(%u) [", prefix,
		   smiNode->name,
		   smiNode->oid[smiNode->oidlen-1]);
	    printIndex(smiNode);
	    printf("]\n");
	    if (c) {
		prefix[prefixlen-1] = c;
	    }
	    break;
	case SMI_NODEKIND_NOTIFICATION:
	    if (prefixlen) {
		c = prefix[prefixlen-1]; prefix[prefixlen-1] = '+';
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
		c = prefix[prefixlen-1]; prefix[prefixlen-1] = '+';
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
		if (childNode->typename && (strlen(childNode->typename) > newtypefieldlen)) {
		    newtypefieldlen = strlen(childNode->typename);
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
	    newprefix = malloc(strlen(prefix)+10);
	    strcpy(newprefix, prefix);
	    if (cnt == 1 || cnt == i) {
		strcat(newprefix, "   ");
	    } else {
		strcat(newprefix, "  |");
	    }
	    dumpOidSubTree(childNode, newprefix, newtypefieldlen);
	    free(newprefix);
	    lastNodeKind = childNode->nodekind;
	}
    }
}



int dumpOidTree(char *modulename)
{
    currentModule = modulename;
    dumpOidSubTree(smiGetNode(NULL, "iso"), "", 0);
    return 0;
}



static void printType(SmiType *smiType, int level)
{
    SmiType *parentType;

    if (smiType->parentmodule && smiType->parentname) {
	parentType = smiGetType(smiType->parentmodule, smiType->parentname);
	if (parentType) {
	    printType(parentType, level+1);
	}
	printf("%*c%s\n", 2 * level, ' ', smiType->name);
	return;
    } else if (! smiType->parentmodule && smiType->parentname) {
	printf("%*c%s\n", 2 * level, ' ', smiType->parentname);
	printf("%*c%s\n", 2 * (level + 1), ' ', smiType->name);
	return;
    } else {
	printf("%*c%s\n", 2 * level, ' ', smiType->name);
    }
    return;
}



int dumpTypes(char *modulename)
{
    SmiType *smiType;

    for (smiType = smiGetFirstType(modulename);
	 smiType; smiType = smiGetNextType(smiType)) {
	printType(smiType, 1);
    }
    return 0;
}
