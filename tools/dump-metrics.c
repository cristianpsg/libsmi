/*
 * dump-metrics.c --
 *
 *      Operations to compute and dump MIB metrics.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-metrics.c,v 1.7 2000/12/11 08:41:22 strauss Exp $
 */

/*
  # revisions
  # imports
  # row creations:
  <basetype usage distribution>
  <complexity of table indexes>
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"


typedef struct StatusCounter {
    unsigned int total;
    unsigned int current;
    unsigned int deprecated;
    unsigned int obsolete;
} StatusCounter;



typedef struct Metrics {
    StatusCounter tables;
    StatusCounter columns;
    StatusCounter scalars;
    StatusCounter notifications;
    unsigned int writable;
} Metrics;



static void incrStatusCounter(StatusCounter *cnt, SmiStatus smiStatus)
{
    cnt->total++;
    switch (smiStatus) {
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_MANDATORY:
    case SMI_STATUS_OPTIONAL:
	cnt->current++;
	break;
    case SMI_STATUS_DEPRECATED:
	cnt->deprecated++;
	break;
    case SMI_STATUS_OBSOLETE:
	cnt->obsolete++;
	break;
    case SMI_STATUS_UNKNOWN:
	break;
    }
}



static void addMetrics(Metrics *metrics, SmiModule *smiModule)
{
    SmiNode *smiNode;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	switch (smiNode->nodekind) {
	case SMI_NODEKIND_TABLE:
	    incrStatusCounter(&metrics->tables, smiNode->status);
	    break;
	case SMI_NODEKIND_COLUMN:
	    incrStatusCounter(&metrics->columns, smiNode->status);
	    if (smiNode->access == SMI_ACCESS_READ_WRITE) {
		metrics->writable++;
	    }
	    break;
	case SMI_NODEKIND_SCALAR:
	    incrStatusCounter(&metrics->scalars, smiNode->status);
	    if (smiNode->access == SMI_ACCESS_READ_WRITE) {
		metrics->writable++;
	    }
	    break;
	case SMI_NODEKIND_NOTIFICATION:
	    incrStatusCounter(&metrics->notifications, smiNode->status);
	    break;
	}
    }
}



static void fprintStatusCounter(FILE *f, StatusCounter *cnt, char *s)
{
    fprintf(f, "%-14s %3d (%3d current, %3d deprecated, %3d obsolete)\n", s,
	    cnt->total, cnt->current, cnt->deprecated, cnt->obsolete);
}



static void fprintMetrics(FILE *f, Metrics *metrics)
{
    unsigned int objects;

    objects = metrics->columns.total + metrics->scalars.total;
    
    fprintStatusCounter(f, &metrics->tables, "Tables:");
    fprintStatusCounter(f, &metrics->columns, "Columns:");
    fprintStatusCounter(f, &metrics->scalars, "Scalars:");
    fprintStatusCounter(f, &metrics->notifications, "Notifications:");

    fprintf(f, "%-14s %3d (%d%%)\n", "Writable:", metrics->writable,
	    objects ? (metrics->writable * 100 / objects) : 0);
}



static void dumpMetrics(int modc, SmiModule **modv, int flags, char *output)
{
    Metrics   metrics;
    int       i;
    FILE      *f = stdout;

    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

    if (flags & SMIDUMP_FLAG_UNITE) {
	if (! (flags & SMIDUMP_FLAG_SILENT)) {
	    fprintf(f, "# united module metrics (generated by smidump "
		    SMI_VERSION_STRING ")\n\n");
	}
	for (i = 0; i < modc; i++) {
	    memset(&metrics, 0, sizeof(Metrics));
	}
	addMetrics(&metrics, modv[i]);
	fprintMetrics(f, &metrics);
    } else {
	for (i = 0; i < modc; i++) {
	    if (! (flags & SMIDUMP_FLAG_SILENT)) {
		fprintf(f, "# %s module metrics (generated by smidump "
			SMI_VERSION_STRING ")\n\n", modv[i]->name);
	    }
	    memset(&metrics, 0, sizeof(Metrics));
	    addMetrics(&metrics, modv[i]);
	    fprintMetrics(f, &metrics);
	}
    }

    if (fflush(f) || ferror(f)) {
	perror("smidump: write error");
	exit(1);
    }

    if (output) {
	fclose(f);
    }
}



void initMetrics()
{
    
    static SmidumpDriver driver = {
	"metrics",
	dumpMetrics,
	SMI_FLAG_NODESCR,
	0,
	"metrics characterizing MIB modules",
	NULL,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}
