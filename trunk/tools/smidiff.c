/*
 * smidiff.c --
 *
 *      ...
 *
 * Copyright (c) 2001 ...
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
    SmiModule *oldModule, *newModule;
    SmiNode *node;
    int oldCount, newCount;

    oldCount = newCount = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: smidiff oldmodule newmodule\n");
        exit(1);
    }
 
    /* the `:' separates the view identifier */
    smiInit("smidiff:old");
    oldModule = smiGetModule(smiLoadModule(argv[1]));
 
    smiInit("smidiff:new");
    newModule = smiGetModule(smiLoadModule(argv[2]));
 
    /* to switch a specific view, simply call smiInit() again */
    smiInit("smidiff:old");
    for(node = smiGetFirstNode(oldModule, SMI_NODEKIND_ANY);
	node;
        node = smiGetNextNode(node, SMI_NODEKIND_ANY)) {
	oldCount++;
    };

    smiInit("smidiff:new");
    for(node = smiGetFirstNode(newModule, SMI_NODEKIND_ANY);
	node;
        node = smiGetNextNode(node, SMI_NODEKIND_ANY)) {
	newCount++;
    };

    printf("The old module has %d nodes.\n", oldCount);
    printf("The new module has %d nodes.\n", newCount);

    smiInit("smidiff:old");
    smiExit();

    smiInit("smidiff:new");
    smiExit();

    return 0;
}

