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
 * @(#) $Id: smisubtree.c,v 1.12 2000/02/24 15:30:01 strauss Exp $
 */

#include <stdio.h>
#include <string.h>
#include <smi.h>
 
int main(int argc, char *argv[])
{
    SmiNode *smiNode;
    int oidlen, first = 1;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: smisubtree oid\n");
        exit(1);
    }
 
    smiInit(NULL);

    smiNode = smiGetNode(NULL, argv[1]);
    if (! smiNode) {
	fprintf(stderr, "smisubtree: node `%s' not found\n", argv[1]);
	exit(2);
    }
 
    for (oidlen = smiNode->oidlen;
	 smiNode && (first || smiNode->oidlen > oidlen);
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY), first = 0) {
	
        printf("%*s%-32s\n",
	       (smiNode->oidlen - oidlen + 1) * 2, "",
	       smiNode->name);
        
    };

    smiExit();
    exit(0);
}
