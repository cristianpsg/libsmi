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
 * @(#) $Id: dump-data.c,v 1.7 1999/05/31 11:58:40 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "data.h"
#include "smi.h"



void
dumpSubTree(subRootPtr, prefix)
    Node *subRootPtr;
    const char *prefix;
{
    Node *n;
    Object *o;
    char s[200];
    
    if (subRootPtr) {
	if (subRootPtr == rootNodePtr) {
	    s[0] = 0;
	} else {
	    if (subRootPtr->firstObjectPtr) {
		sprintf(s, "%s.%s(%d)", prefix,
			subRootPtr->firstObjectPtr->name,
			subRootPtr->subid);
	    } else {
		sprintf(s, "%s.%d", prefix, subRootPtr->subid);
	    }
	    fprintf(stderr, "%s", s);
	    if (subRootPtr->firstObjectPtr) {
		fprintf(stderr, " ==");
		for (o = subRootPtr->firstObjectPtr; o;
		     o = o->nextSameNodePtr) {
		    if (o->modulePtr && !(o->flags & FLAG_IMPORTED)) {
			fprintf(stderr,
				" %s::%s", o->modulePtr->name, o->name);
		    }
		}
	    }
	    fprintf(stderr, "\n");
	}
	for (n = subRootPtr->firstChildPtr; n; n = n->nextPtr) {
	    dumpSubTree(n, s);
	}
    }
}



void
dumpMibTree()
{
    dumpSubTree(rootNodePtr, "");
    dumpSubTree(pendingNodePtr, "[pending]");
}



void
dumpTypes()
{
    View     *viewPtr;
    Module   *modulePtr;
    Type     *typePtr;

    for (viewPtr = firstViewPtr;
	 viewPtr; viewPtr = viewPtr->nextPtr) {
	modulePtr = findModuleByName(viewPtr->name);
	for (typePtr = modulePtr->firstTypePtr;
	     typePtr; typePtr = typePtr->nextPtr) {
	    printf("%s.%s\n",
		   typePtr->modulePtr->name ? typePtr->modulePtr->name : "-",
		   typePtr->name ? typePtr->name : "-");
	}
    }
}
