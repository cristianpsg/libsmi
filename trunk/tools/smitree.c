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
 * @(#) $Id: smitree.c,v 1.3 1999/12/17 10:44:24 strauss Exp $
 */

#include <stdio.h>
#include <string.h>
#include <smi.h>

int main(int argc, char *argv[])
{
    SmiNode *smiNode;
    SmiModule *smiModule;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: smitree module\n");
        exit(1);
    }
 
    smiInit("smitree");
    smiLoadModule(argv[1]);
    smiModule = smiGetModule(argv[1]);
 
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
        smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

        printf("%*s%s::%s\n", smiNode->oidlen * 2, " ",
               smiModule->name, smiNode->name);
        
    };

    smiExit();
    return 0;
}

