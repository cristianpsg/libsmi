/*
 * smisubstree.c --
 *
 *      A simple SMI example program, printing an OID's subtree.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smisubtree.c,v 1.4 1999/05/25 17:00:40 strauss Exp $
 */

#include <stdio.h>
#include <string.h>

#include "smi.h"

int
main(argc, argv)
    int argc;
    char *argv[];
{
    char *modulename, *fullname;
    SmiNode *smiNode, *smiStartNode;
    
    if (argc != 2) {
	fprintf(stderr, "Usage: smisubtree oid\n");
	exit(1);
    }

    fullname   = argv[1];
    modulename = smiModule(fullname);
    
    smiInit();

    smiLoadModule(modulename);

    for(smiNode = smiStartNode = smiGetNode(fullname, "");
	smiNode && strstr(smiNode->oid, smiStartNode->oid) == smiNode->oid;
	smiNode = smiGetNextNode(modulename, smiNode->name)) {

	printf("%s::%-16s   [%s]\n",
	       smiNode->module, smiNode->name,
	       smiStringBasetype(smiNode->basetype));
	
    };
    
    exit(0);
}
