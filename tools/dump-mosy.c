/*
 * dump-mosy.c --
 *
 *      Operations to dump MIB modules in the MOSY output format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-mosy.c,v 1.3 1999/06/16 15:52:23 strauss Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

#include "smi.h"

static char *ignoreTypeRanges[] = {
    "TimeTicks",
    "Counter32",
    "Gauge32",
    "Counter64",
    NULL
};


static char *stringStatus(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	(status == SMI_STATUS_OPTIONAL)    ? "optional" :
					     "<unknown>";
}


static char *stringAccess(SmiAccess access)
{
    return
	(access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" :
	(access == SMI_ACCESS_NOTIFY)	      ? "accessible-for-notify" :
	(access == SMI_ACCESS_READ_ONLY)      ? "read-only" :
	(access == SMI_ACCESS_READ_WRITE)     ? "read-write" :
	(access == SMI_ACCESS_READ_CREATE)    ? "read-create" :
						"<unknown>";
}


static char *stringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<UNKNOWN>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "OctetString" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectID" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "Unsigned32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "Integer32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "Unsigned64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "Integer64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "Float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "Float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "Float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "INTEGER" :
        (basetype == SMI_BASETYPE_BITS)              ? "Bits" :
	(basetype == SMI_BASETYPE_SEQUENCE)          ? "Aggregate" :
	(basetype == SMI_BASETYPE_SEQUENCEOF)	     ? "Aggregate" :
                                                   "<unknown>";
}


static char *getOidString(SmiNode *smiNode, int importedParent)
{
    SmiNode	 *parentNode, *node;
    static char	 s[200];
    char	 append[200];
    unsigned int i;
    
    append[0] = 0;

    parentNode = smiNode;

    do {

	if (parentNode->oidlen <= 1) {
	    break;
	}
	
	/* prepend the cut-off subidentifier to `append'. */
	strcpy(s, append);
	sprintf(append, ".%u%s", parentNode->oid[parentNode->oidlen-1], s);

	/* retrieve the parent SmiNode */
	node = parentNode;
	parentNode = smiGetParentNode(node);
	if (node != smiNode) {
	    smiFreeNode(node);
	}

	if (!parentNode) {
	    sprintf(s, "%s", append);
	    return s;
	}
	
	/* found an imported or a local parent node? */
	if ((parentNode->name && strlen(parentNode->name)) &&
	    (smiIsImported(smiNode->module,
			   parentNode->module, parentNode->name) ||
	     (!importedParent &&
	      !strcmp(parentNode->module, smiNode->module)))) {
	    sprintf(s, "%s%s", parentNode->name, append);
	    smiFreeNode(parentNode);
	    return s;
	}
	
    } while (parentNode);

    /* smiFreeNode(parentNode); */
    s[0] = 0;
    for (i=0; i < smiNode->oidlen; i++) {
	if (i) strcat(s, ".");
	sprintf(&s[strlen(s)], "%u", smiNode->oid[i]);
    }
    return s;
}


static char *getValueString(SmiValue *valuePtr)
{
    static char s[100];
    char        **p;
    
    s[0] = 0;
    
    switch (valuePtr->basetype) {
    case SMI_BASETYPE_UNSIGNED32:
	sprintf(s, "%lu", valuePtr->value.unsigned32);
	break;
    case SMI_BASETYPE_INTEGER32:
	sprintf(s, "%ld", valuePtr->value.integer32);
	break;
    case SMI_BASETYPE_UNSIGNED64:
	sprintf(s, "%llu", valuePtr->value.unsigned64);
	break;
    case SMI_BASETYPE_INTEGER64:
	sprintf(s, "%lld", valuePtr->value.integer64);
	break;
    case SMI_BASETYPE_FLOAT32:
    case SMI_BASETYPE_FLOAT64:
    case SMI_BASETYPE_FLOAT128:
	break;
    case SMI_BASETYPE_ENUM:
	sprintf(s, "%s", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_OCTETSTRING:
	sprintf(s, "\"%s\"", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	if (valuePtr->value.bits) {
	    for(p = valuePtr->value.bits; *p; p++) {
		if (p != valuePtr->value.bits)
		    sprintf(&s[strlen(s)], ", ");
		sprintf(&s[strlen(s)], "%s", *p);
	    }
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
    case SMI_BASETYPE_SEQUENCE:
    case SMI_BASETYPE_SEQUENCEOF:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	/* TODO */
	break;
    }

    return s;
}



static void printAssignements(char *modulename)
{
    int		 cnt = 0;
    SmiNode	 *smiNode;
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_NODE);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NODE)) {

	cnt++;

	if (! smiNode->description) {
	printf("%-20s %s\n", smiNode->name, getOidString(smiNode, 0));
	printf("%%n0 %-16s object-id\n", smiNode->name);
}
    }

    if (cnt) {
	printf("\n");
    }
}



static void printTypedefs(char *modulename)
{
    int		   i;
    SmiType	   *smiType;
    SmiNamedNumber *nn;
    
    for(i = 0, smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(smiType)) {

	printf("%%%-19s %-16s %-15s \"%s\"\n", "tc", smiType->name,
	       stringBasetype(smiType->basetype),
	       smiType->format ? smiType->format : "");
	
	for(i = 0, nn = smiGetFirstNamedNumber(smiType->module, smiType->name);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    printf("%%%-19s %-16s %-15s %s\n", "es",
		   smiType->name, nn->name,
		   getValueString(nn->valuePtr));
	}
    }
}



static void printObjects(char *modulename)
{
    int		   i, j, ignore, cnt = 0, aggregate;
    char	   *typename, *indexname;
    SmiNode	   *smiNode;
    SmiIndex	   *index;
    SmiType	   *smiType;
    SmiNamedNumber *smiNamedNumber;
    SmiRange       *smiRange;
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	if (smiNode->nodekind != SMI_NODEKIND_NODE
	    && smiNode->nodekind != SMI_NODEKIND_SCALAR
	    && smiNode->nodekind != SMI_NODEKIND_TABLE
	    && smiNode->nodekind != SMI_NODEKIND_ROW
	    && smiNode->nodekind != SMI_NODEKIND_COLUMN) {
	    continue;
	}
	
	cnt++;

	if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	    if (smiNode->description) {
		printf("%-20s %s\n", smiNode->name, getOidString(smiNode, 0));
		printf("%%n0 %-16s object-id\n", smiNode->name);
	    }
	    continue;
	}

	aggregate = smiNode->nodekind == SMI_NODEKIND_TABLE
	    || smiNode->nodekind == SMI_NODEKIND_ROW;

	smiType = NULL;
	if (!aggregate && smiNode->typemodule && smiNode->typename) {
	    smiType = smiGetType(smiNode->typemodule, smiNode->typename);
	}

	typename = stringBasetype(smiNode->basetype);
	if (smiType && smiType->name && isupper((int) smiType->name[0])) {
	    typename = smiType->name;
	    if (smiType->parentmodule && smiType->parentname) {
		typename = smiType->parentname;
	    }
	}

	if (smiType && smiType->name
	    && islower((int) smiType->name[0])
	    && smiType->parentname) {
	    typename = smiType->parentname;
	    if (strcmp(typename, "OCTET STRING") == 0) {
		typename = "OctetString";
	    }
	    if (strcmp(typename, "Enum") == 0) {
		typename = "INTEGER";
	    }
	}

	if (aggregate) {
	    typename = "Aggregate";
	}
	
	printf("%-20s %-16s ", smiNode->name, getOidString(smiNode, 0));
	printf("%-15s %-15s %s\n", typename,
	       stringAccess(smiNode->access),
	       stringStatus(smiNode->status));
	
	if (smiNode->indexkind == SMI_INDEX_INDEX) {
	    printf("%%%-19s %-16s \"", "ei", smiNode->name);
	    indexname = NULL;
	    for (i = -1, index = smiGetFirstIndex(smiNode);
		 index; index = smiGetNextIndex(index), i++) {
		if (i > 0) printf(" ");
		if (indexname) {
		    printf(indexname);
		}
		indexname = index->name;
	    }
	    if (indexname) {
		printf("%s%s%s",
		       (i > 0) ? " " : "",
		       (smiNode->implied) ? "*" : "",
		       indexname);
	    }
	    printf("\"\n");
	}

	if (smiNode->indexkind == SMI_INDEX_AUGMENT && smiNode->relatedname) {
	    printf ("%%%-19s %-16s %s\n", "ea",
		    smiNode->name, smiNode->relatedname);
	}

	if (smiType && islower((int)(smiType->name[0]))) {
	    for(i = 0, smiNamedNumber = smiGetFirstNamedNumber(smiType->module,
							       smiType->name);
		smiNamedNumber;
		i++, smiNamedNumber = smiGetNextNamedNumber(smiNamedNumber)) {
		printf("%%%-19s %-16s %-15s %s\n", "ev",
		       smiType->name, smiNamedNumber->name,
		       getValueString(smiNamedNumber->valuePtr));
	    }

	    for (ignore = 0, j = 0; ignoreTypeRanges[j]; j++) {
		if (strcmp(smiType->name, ignoreTypeRanges[j]) == 0) {
		    ignore++;
		    break;
		}
	    }

	    if (! ignore) {
		for (smiRange = smiGetFirstRange(smiType->module,
						 smiType->name);
		     smiRange;
		     smiRange = smiGetNextRange(smiRange)) {
		    printf("%%%-19s %-16s %-15s ", "er",
			   smiType->name,
			   getValueString(smiRange->minValuePtr));
		    printf("%s\n", getValueString(smiRange->maxValuePtr));
		}
	    }
	}

	if (smiType) {
/*	    smiFreeType(smiType); */
	}
    }

    if (cnt) {
	printf("\n");
    }
}



static void printNotifications(char *modulename)
{
    int		 cnt = 0;
    SmiNode	 *smiNode;
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_NOTIFICATION);
	smiNode; 
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	cnt++;
	
	printf ("%-20s %s\n", smiNode->name, getOidString(smiNode, 0));
	printf ("%%n0 %-16s notification\n", smiNode->name);
    }

    if (cnt) {
	printf("\n");
    }
}



static void printGroups(char *modulename)
{
    SmiNode	*smiNode, *smiNodeMember;
    int		cnt = 0, objects, notifications;
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_GROUP);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {

	cnt ++;

	for (objects = 0, notifications = 0,
	     smiNodeMember = smiGetFirstMemberNode(smiNode);
	     smiNodeMember;
	     smiNodeMember = smiGetNextMemberNode(smiNode, smiNodeMember)) {

	    objects += 
		(smiNodeMember->nodekind == SMI_NODEKIND_SCALAR)
		|| (smiNodeMember->nodekind == SMI_NODEKIND_COLUMN);
	    notifications +=
		(smiNodeMember->nodekind == SMI_NODEKIND_NOTIFICATION);
	}

	printf ("%-20s %s\n", smiNode->name, getOidString(smiNode, 0));
	printf ("%%n0 %-16s %s\n", smiNode->name,
		(objects && ! notifications) ? "object-group" :
		(! objects && notifications) ? "notification-group" : "group");
    }

    if (cnt) {
	printf("\n");
    }
}



static void printCompliances(char *modulename)
{
    int		  cnt = 0;
    SmiNode	  *smiNode;
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_COMPLIANCE);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	
	cnt++;

	printf ("%-20s %s\n", smiNode->name, getOidString(smiNode, 0));
	printf ("%%n0 %-16s module-compliance\n", smiNode->name);
    }
	    
    if (cnt) {
	printf("\n");
    }
}



int dumpMosy(char *modulename)
{
    SmiModule	 *smiModule;
    SmiNode	 *smiNode;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    }

    printf("-- automatically generated by smidump %s, do not edit!\n",
	   VERSION);
    printf("\n-- object definitions compiled from %s\n\n", modulename);


    if (smiModule->object) {
	smiNode = smiGetNode(smiModule->name, smiModule->object);
	printf("%-20s %s\n", smiModule->object, getOidString(smiNode, 0));
	printf("%%n0 %-16s module-compliance\n", smiModule->object);
	printf("\n");
    }
    
    printAssignements(modulename);
    printTypedefs(modulename);
    printObjects(modulename);
    printNotifications(modulename);
    printGroups(modulename);
    printCompliances(modulename);

    return 0;
}
