/*
 * dump-yang.c --
 *
 *      Operations to dump MIB modules in the YANG output format.
 *
 * Copyright (c) 2007 J. Schoenwaelder, Jacobs University Bremen.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-yang.c 13463 2009-03-16 07:54:49Z schoenw $
 */

#include <config.h>

#ifdef BACKEND_YANG

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"
#include "fprint.h"
#include "fortopat.h"

static int sflag = 0;		/* generate smi: extensions */
static int nflag = 0;		/* generate notifications */
static int INDENT = 2;		/* indent factor */

static void fprintYangNode(FILE *f, int indent, YangNode* nodePtr)
{
    switch (nodePtr->nodeKind) {
        case YANG_DECL_UNKNOWN_STATEMENT:
            fprintSegment(f, indent, nodePtr->value, 0);
            if (nodePtr->extra) {
                fprint(f, " \"%s\"", nodePtr->extra);
            }
            break;
        default: 
            fprintSegment(f, indent, yandDeclKeyword[nodePtr->nodeKind], 0);
            if (nodePtr->value) {
                fprint(f, " \"%s\"", nodePtr->value);
            }
    }
    
    YangNode *childPtr = yangGetFirstChildNode(nodePtr);
    if (childPtr) {
        fprint(f, " {\n");
        for (; childPtr; childPtr = yangGetNextSibling(childPtr)) {
            fprintYangNode(f, indent + INDENT, childPtr);
        }
        fprintSegment(f, indent, "}\n", 0);
    } else {
        fprint(f, ";\n");
    }
    
}

static void
dumpYangSK(int modc, SmiModule **modv, int flags, char *output)
{
    SmiModule   *smiModule;
    int		i;
    FILE	*f = stdout;

    if (output) {
        f = fopen(output, "w");
        if (!f) {
            fprintf(stderr, "smidump: cannot open %s for writing: ", output);
            perror(NULL);
            exit(1);
        }
    }

    for (i = 0; i < modc; i++) {        
        smiModule = modv[i];
        YangNode *module = yangGetModule(smiModule->name);
        fprintYangNode(f, 0, module);        
    }

    if (fflush(f) || ferror(f)) {
        perror("smidump: write error");
        exit(1);
    }

    if (output) {
        fclose(f);
    }
}



void initYangSK()
{
    static SmidumpDriverOption opt[] = {
	{ "smi-extensions", OPT_FLAG, &sflag, 0,
	  "generate smi extensions" },
	{ "no-notifications", OPT_FLAG, &nflag, 0,
	  "do not generate notifications" },
	{ "indent", OPT_INT, &INDENT, 0,
	  "indentation (default 2)" },
        { 0, OPT_END, 0, 0 }
    };

    static SmidumpDriver driver = {
	"yang-sk",
	dumpYangSK,
	0,
	SMIDUMP_DRIVER_CANT_UNITE | SMIDUMP_DRIVER_CANT_SMI | SMIDUMP_DRIVER_CANT_SPPI,
	"YANG format",
	opt,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}

#endif

