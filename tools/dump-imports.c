/*
 * dump-imports.c --
 *
 *      Operations to dump import hierarchies in a human readable format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-imports.c,v 1.2 1999/07/02 14:04:06 strauss Exp $
 */

#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"


typedef struct Imports {
    char *module;
    int  count;
} Imports;



static void *safeMalloc(size_t size)
{
    char *m = malloc(size);
    if (! m) {
	fprintf(stderr, "smidump: malloc failed - running out of memory\n");
	exit(1);
    }
    return m;
}



static char *safeStrdup(const char *s)
{
    char *m = strdup(s);
    if (! m) {
	fprintf(stderr, "smidump: strdup failed - running out of memory\n");
	exit(1);
    }
    return m;
}



static Imports *getImports(char *modulename, int *n)
{
    SmiImport *smiImport;
    Imports   *imports;
    int       i, size;
    
    for(smiImport = smiGetFirstImport(modulename), *n = 0;
	smiImport;
	smiImport = smiGetNextImport(smiImport)) (*n)++;

    size = (*n + 1) * sizeof(Imports);
    imports = safeMalloc(size);
    memset(imports, 0, size);

    for(smiImport = smiGetFirstImport(modulename), *n = 0;
	smiImport;
	smiImport = smiGetNextImport(smiImport)) {
	
	for (i = 0; i < *n; i++) {
	    if (strcmp(smiImport->importmodule, imports[i].module) == 0)
		break;
	}
	
	if (i == *n) {
	    imports[i].module = safeStrdup(smiImport->importmodule);
	    if (imports[i].module) {
		imports[i].count = 0;
		(*n)++;
	    }
	}
	imports[i].count++;
    }

    return imports;
}



static void freeImports(Imports *imports, int n)
{
    int i;

    for (i = 0; i < n; i++) {
	free(imports[i].module);
    }

    free(imports);
}



static int printImports(char *modulename, char *prefix)
{
    Imports *imports;
    int     i, n, recurse = 0, done = 0;

    imports = getImports(modulename, &n);

    for (i = 0; i < n; i++) {
	char *newprefix;
	recurse = (smiGetFirstImport(imports[i].module) == NULL);
	if (recurse) {
	    printf("%s  |\n", prefix);
	}
	printf("%s  +--%s [%d identifier%s]\n", prefix, imports[i].module, 
	       imports[i].count, imports[i].count > 1 ? "s" : "");
	newprefix = safeMalloc(strlen(prefix)+10);
	strcpy(newprefix, prefix);
	if (i == n-1) {
	    strcat(newprefix, "   ");
	} else {
	    strcat(newprefix, "  |");
	}
	done = printImports(imports[i].module, newprefix);
	if (! recurse && done) {
	    if (i == n-1) {
		printf("%s   \n", prefix);
	    } else {
		printf("%s  |\n", prefix);
	    }
	}
	free(newprefix);
    }

    freeImports(imports, n);

    return recurse;
}



int dumpImports(char *modulename)
{
    SmiModule	 *smiModule;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }
    
    printf("%s\n", smiModule->name);
    printImports(smiModule->name, "");

    return 0;
}
