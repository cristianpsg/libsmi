/*
 * smisubstree.c --
 *
 *      A simple SMI example program, printing an OID's subtree.
 *
 * Copyright (c) 1999 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smisubtree.c,v 1.2 1999/05/04 09:00:55 strauss Exp $
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
