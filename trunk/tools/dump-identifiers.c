/*
 * dump-identifiers.c --
 *
 *      Operations to dump flat identifier lists for SMI modules.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-identifiers.c,v 1.6 2000/08/24 12:09:18 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "smi.h"
#include "smidump.h"


static int pmodc = 0;
static SmiModule **pmodv = NULL;

static int moduleLen = 0;
static int identifierLen = 0;



static int pruneSubTree(SmiNode *smiNode)
{
    SmiModule *smiModule;
    SmiNode   *childNode;
    int	      i;

    if (! smiNode) {
	return 1;
    }

    smiModule = smiGetNodeModule(smiNode);
    for (i = 0; i < pmodc; i++) {
	if (strcmp(pmodv[i]->name, smiModule->name) == 0) {
	    return 0;
	}
    }

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (! pruneSubTree(childNode)) {
	    return 0;
	}
    }
    return 1;
}



static void dumpNodeIdentifiers(FILE *f, SmiNode *smiNode)
{
    SmiModule *smiModule;
    SmiNode   *childNode;
    int       i;

    if (smiNode) {

	smiModule = smiGetNodeModule(smiNode);

	if (smiNode->name && smiModule->name) {
	    for (i = 0; i < pmodc; i++) {
		if (strcmp(pmodv[i]->name, smiModule->name) == 0) {
		    break;
		}
	    }
	    if (i < pmodc) {
		fprintf(f, "%*s %*s ",
			-moduleLen, smiModule->name,
			-identifierLen, smiNode->name);
		for (i = 0; i < smiNode->oidlen; i++) {
		    fprintf(f, i ? ".%u" : "%u", smiNode->oid[i]);
		}
		fprintf(f, "\n");
	    }
	}
	    
	for (childNode = smiGetFirstChildNode(smiNode);
	     childNode;
	     childNode = smiGetNextChildNode(childNode)) {
	    
	    if (! pruneSubTree(childNode)) {
		dumpNodeIdentifiers(f, childNode);
	    }
	}
    }
}



static void dumpTypeIdentifiers(FILE *f, int modc, SmiModule **modv)
{
    SmiType   *smiType;
    int	      i;

    for (i = 0; i < modc; i++) {
	for (smiType = smiGetFirstType(modv[i]);
	     smiType;
	     smiType = smiGetNextType(smiType)) {
	    if (smiType->name) {
		fprintf(f, "%*s %*s\n",
			-moduleLen, modv[i]->name,
			-identifierLen, smiType->name);
	    }
	}
    }
}



void dumpIdentifiers(int modc, SmiModule **modv, int flags, char *output)
{
    SmiNode   *smiNode;
    int	      i, len;
    FILE      *f = stdout;

    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

    for (moduleLen = 0, identifierLen = 0, i = 0; i < modc; i++) {
	len = strlen(modv[i]->name);
	if (len > moduleLen) moduleLen = len;
	for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_ANY);
	     smiNode;
	     smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	    if (smiNode->name) {
		len = strlen(smiNode->name);
		if (len > identifierLen) identifierLen = len;
	    }
	}
    }

    if (flags & SMIDUMP_FLAG_UNITE) {

	pmodc = modc;
	pmodv = modv;
	
	if (! (flags & SMIDUMP_FLAG_SILENT)) {
	    fprintf(f, "# united list of identifiers (generated by smidump "
		    SMI_VERSION_STRING ")\n\n");
	}

	dumpTypeIdentifiers(f, modc, modv);
	smiNode = smiGetNode(NULL, "iso");
	if (smiNode) {
	    dumpNodeIdentifiers(f, smiNode);
	}

    } else {

	for (i = 0; i < modc; i++) {

	    pmodc = 1;
	    pmodv = &(modv[i]);

	    if (! (flags & SMIDUMP_FLAG_SILENT)) {
		fprintf(f, "# %s list of identifiers (generated by smidump "
			SMI_VERSION_STRING ")\n\n",
			modv[i]->name);
	    }

	    dumpTypeIdentifiers(f, 1, &(modv[i]));
	    smiNode = smiGetNode(NULL, "iso");
	    if (smiNode) {
		dumpNodeIdentifiers(f, smiNode);
	    }
	}
    }

    if (output) {
	fclose(f);
    }
}
