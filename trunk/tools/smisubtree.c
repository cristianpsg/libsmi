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
 * @(#) $Id: smisubtree.c,v 1.9 2000/02/10 22:35:04 strauss Exp $
 */

#include <config.h>

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
 
    for((smiNode = smiGetNode(NULL, argv[1])) &&
	    (oidlen = smiNode->oidlen);
	smiNode && (first || smiNode->oidlen > oidlen);
        smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY),
	    first = 0) {
 
        printf("%*s%-32s\n",
	       (smiNode->oidlen - oidlen + 1) * 2, " ",
	       smiNode->name);
        
    };
    
    exit(0);
}
