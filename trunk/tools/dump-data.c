/*
 * dump-data.c --
 *
 *      Operations to dump libsmi internal data structures.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @(#) $Id: dump-data.c,v 1.5 1999/05/20 08:51:17 strauss Exp $
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
	    printf("%s%s%s\n",
		   typePtr->modulePtr->name ? typePtr->modulePtr->name : "-",
		   SMI_NAMESPACE_OPERATOR,
		   typePtr->name ? typePtr->name : "-");
	}
    }
}
