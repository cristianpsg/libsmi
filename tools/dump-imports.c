/*
 * dump-imports.c --
 *
 *      Operations to dump a module's import hierarchy.
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

#include "smi.h"
#include "util.h"



static void printImports(char *modulename, int indent)
{
    SmiImport     *smiImport;
    int i;
    char gotcha[100][100];
    int count[100];
    int n = 0;
    
    for(smiImport = smiGetFirstImport(modulename); smiImport;
	smiImport = smiGetNextImport(smiImport)) {
	
	for (i = 0;
	     (i < n) && strcmp(smiImport->importmodule, gotcha[i]); i++);
	if (i == n) {
	    strcpy(gotcha[n], smiImport->importmodule);
	    count[n] = 0;
	    n++;
	}
	count[i]++;
    }

    for (i = 0; i < n; i++) {
	printf("%*s%s [%d identifiers]\n",
	       indent + 4, "", gotcha[i], count[i]);
	printImports(gotcha[i], indent + 4);
    }
    
}



int dumpImports(char *modulename)
{
    SmiModule	 *smiModule;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	printf("%s\n", smiModule->name);
	printImports(smiModule->name, 0);
    }
    return 0;
}
