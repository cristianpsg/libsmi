/*
 * smitree.c --
 *
 *      A simple SMI example program, printing a module's OID tree.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#include <stdio.h>
#include <string.h>
#include <smi.h>

int main(int argc, char *argv[])
{
    char *module;
    SmiNode *smiNode;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: smitree module\n");
        exit(1);
    }
 
    smiInit();
    module = argv[1];
    smiLoadModule(module);
 
    for(smiNode = smiGetFirstNode(module, SMI_NODEKIND_ANY); smiNode;
        smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

        printf("%*s%s::%s\n", smiNode->oidlen * 2, " ",
               smiNode->module, smiNode->name);
        
    };
    
    return 0;
}

