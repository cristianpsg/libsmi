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
 * @(#) $Id: smisubtree.c,v 1.7 1999/12/12 12:51:08 strauss Exp $
 */

#include <stdio.h>
#include <string.h>
#include <smi.h>

int main(int argc, char *argv[])
{
    char *modulename, *fullname, *oid;
    SmiNode *smiNode;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: smisubtree oid\n");
        exit(1);
    }
 
    fullname   = argv[1];
    modulename = smiModule(fullname);
    
    smiInit("smisubtree");
 
    smiLoadModule(modulename);
 
    smiNode = smiGetNode(NULL, fullname);
    if (smiNode) oid = smiNode->oid;
 
    for(; smiNode && (strstr(smiNode->oid, oid) == smiNode->oid);
        smiNode = smiGetNextNode(smiNode, SMI_DECL_UNKNOWN)) {
 
        printf("%s.%-26s [%s]\n",
               smiNode->module, smiNode->name, smiNode->oid);
        
    };
    
    smiFreeNode(smiNode);
    smiExit();
    free(modulename);
    exit(0);
}

