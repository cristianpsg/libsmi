/*
 * dump-metrics.c --
 *
 *      Operations to compute and dump MIB metrics.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
 * Copyright (c) 2002 J. Schoenwaelder, University of Osnabrueck.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-metrics.c,v 1.8 2002/10/30 09:17:37 schoenw Exp $
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


static int raw = 0;


typedef struct StatusCounter {
    unsigned int total;
    unsigned int current;
    unsigned int deprecated;
    unsigned int obsolete;
} StatusCounter;



typedef struct AccessCounter {
    unsigned int total;
    unsigned int noaccess;
    unsigned int notify;
    unsigned int readonly;
    unsigned int readwrite;
} AccessCounter;



typedef struct IndexCounter {
    unsigned int total;
    unsigned int index;
    unsigned int augment;
    unsigned int reorder;
    unsigned int sparse;
    unsigned int expand;
} IndexCounter;



typedef struct LengthCounter {
    unsigned int total;
    unsigned int description;
    unsigned int reference;
    unsigned int units;
    unsigned int format;
} LengthCounter;



typedef struct FeatureCounter {
    unsigned int total;
    unsigned int description;
    unsigned int reference;
    unsigned int units;
    unsigned int format;
} FeatureCounter;



typedef struct Metrics {
    StatusCounter statusTypes;
    StatusCounter statusTables;
    StatusCounter statusColumns;
    StatusCounter statusScalars;
    StatusCounter statusNotifications;
    StatusCounter statusGroups;
    StatusCounter statusCompliances;
    StatusCounter statusAll;
    AccessCounter accessColumns;
    AccessCounter accessScalars;
    AccessCounter accessAll;
    IndexCounter  indexTables;
    LengthCounter lengthTypes;
    LengthCounter lengthTables;
    LengthCounter lengthRows;
    LengthCounter lengthColumns;
    LengthCounter lengthScalars;
    LengthCounter lengthNotifications;
    LengthCounter lengthAll;
} Metrics;



static void
incrStatusCounter(StatusCounter *cnt, SmiStatus smiStatus)
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



static void
incrAccessCounter(AccessCounter *cnt, SmiAccess smiAccess)
{
    cnt->total++;
    switch (smiAccess) {
    case SMI_ACCESS_NOT_ACCESSIBLE:
	cnt->noaccess++;
	break;
    case SMI_ACCESS_NOTIFY:
	cnt->notify++;
	break;
    case SMI_ACCESS_READ_ONLY:
	cnt->readonly++;
	break;
    case SMI_ACCESS_READ_WRITE:
	cnt->readwrite++;
	break;
    case SMI_ACCESS_INSTALL:
    case SMI_ACCESS_INSTALL_NOTIFY:
    case SMI_ACCESS_REPORT_ONLY:
    case SMI_ACCESS_UNKNOWN:
    case SMI_ACCESS_NOT_IMPLEMENTED:
	break;
    }
}



static void
incrIndexCounter(IndexCounter *cnt, SmiIndexkind indexkind)
{
    cnt->total++;
    switch (indexkind) {
    case SMI_INDEX_INDEX:
	cnt->index++;
	break;
    case SMI_INDEX_AUGMENT:
	cnt->augment++;
	break;
    case SMI_INDEX_REORDER:
	cnt->reorder++;
	break;
    case SMI_INDEX_SPARSE:
	cnt->sparse++;
	break;
    case SMI_INDEX_EXPAND:
	cnt->expand++;
	break;
    case SMI_INDEX_UNKNOWN:
	break;
    }
}



static void
incrLengthCounter(LengthCounter *cnt, char *description, char *reference,
		  char *units, char *format)
{
    cnt->total++;
    if (description) {
	cnt->description += strlen(description);
    }
    if (reference) {
	cnt->reference += strlen(reference);
    }
    if (units) {
	cnt->units += strlen(units);
    }
    if (format) {
	cnt->format += strlen(format);
    }
}
    


static void
addMetrics(Metrics *metrics, SmiModule *smiModule)
{
    SmiNode *smiNode;
    SmiType *smiType;
    size_t len;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	len = smiNode->description ? strlen(smiNode->description) : 0;
	switch (smiNode->nodekind) {
	case SMI_NODEKIND_TABLE:
	    incrStatusCounter(&metrics->statusTables, smiNode->status);
	    incrStatusCounter(&metrics->statusAll, smiNode->status);
	    incrLengthCounter(&metrics->lengthTables,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    incrLengthCounter(&metrics->lengthAll,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    break;
	case SMI_NODEKIND_ROW:
	    incrIndexCounter(&metrics->indexTables, smiNode->indexkind);
	    incrLengthCounter(&metrics->lengthRows,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    incrLengthCounter(&metrics->lengthAll,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    break;
	case SMI_NODEKIND_COLUMN:
	    incrStatusCounter(&metrics->statusColumns, smiNode->status);
	    incrStatusCounter(&metrics->statusAll, smiNode->status);
	    incrAccessCounter(&metrics->accessColumns, smiNode->access);
	    incrAccessCounter(&metrics->accessAll, smiNode->access);
	    incrLengthCounter(&metrics->lengthColumns,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    incrLengthCounter(&metrics->lengthAll,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    break;
	case SMI_NODEKIND_SCALAR:
	    incrStatusCounter(&metrics->statusScalars, smiNode->status);
	    incrStatusCounter(&metrics->statusAll, smiNode->status);
	    incrAccessCounter(&metrics->accessScalars, smiNode->access);
	    incrAccessCounter(&metrics->accessAll, smiNode->access);
	    incrLengthCounter(&metrics->lengthScalars,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    incrLengthCounter(&metrics->lengthAll,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    break;
	case SMI_NODEKIND_NOTIFICATION:
	    incrStatusCounter(&metrics->statusNotifications, smiNode->status);
	    incrStatusCounter(&metrics->statusAll, smiNode->status);
	    incrLengthCounter(&metrics->lengthNotifications,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    incrLengthCounter(&metrics->lengthAll,
			      smiNode->description, smiNode->reference,
			      smiNode->units, smiNode->format);
	    break;
	case SMI_NODEKIND_GROUP:
	    incrStatusCounter(&metrics->statusGroups, smiNode->status);
	    incrStatusCounter(&metrics->statusAll, smiNode->status);
	    break;
	case SMI_NODEKIND_COMPLIANCE:
	    incrStatusCounter(&metrics->statusCompliances, smiNode->status);
	    incrStatusCounter(&metrics->statusAll, smiNode->status);
	    break;
	}
    }

    for (smiType = smiGetFirstType(smiModule);
	 smiType;
	 smiType = smiGetNextType(smiType)) {
	len = smiType->description ? strlen(smiType->description) : 0;
	incrStatusCounter(&metrics->statusTypes, smiType->status);
	incrStatusCounter(&metrics->statusAll, smiType->status);
	incrLengthCounter(&metrics->lengthTypes,
			  smiType->description, smiType->reference,
			  smiType->units, smiType->format);
	incrLengthCounter(&metrics->lengthAll,
			  smiType->description, smiType->reference,
			  smiType->units, smiType->format);
    }
}



static void
fprintStatusCounter(FILE *f, StatusCounter *cnt, char *s)
{
    if (!s || !cnt) {
	fprintf(f, "%-14s %8s %8s %11s %9s\n", "CATEGORY",
		"TOTAL", "CURRENT", "DEPRECATED", "OBSOLETE");
	return;
    }

    if (raw) {
	fprintf(f, "%-14s %8u %8u %11u %9u\n", s,
		cnt->total, cnt->current, cnt->deprecated, cnt->obsolete);
    } else {
	fprintf(f, "%-14s %8u %7.1f%% %10.1f%% %8.1f%%\n", s,
		cnt->total,
		cnt->total ? (double) cnt->current * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->deprecated * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->obsolete * 100 / cnt->total : 0);
    }
}



static void
fprintAccessCounter(FILE *f, AccessCounter *cnt, char *s)
{
    if (!s || !cnt) {
	fprintf(f, "%-14s %8s %10s %9s %7s %8s\n", "CATEGORY",
		"TOTAL", "READWRITE", "READONLY", "NOTIFY", "NOACCES");
	return;
    }

    if (raw) {
	fprintf(f, "%-14s %8u %10u %9u %7u %8u\n", s,
		cnt->total, cnt->readwrite, cnt->readonly,
		cnt->notify, cnt->noaccess);
    } else {
	fprintf(f, "%-14s %8u %9.1f%% %8.1f%% %6.1f%% %7.1f%%\n", s,
		cnt->total,
		cnt->total ? (double) cnt->readwrite * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->readonly * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->notify * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->noaccess * 100 / cnt->total : 0);
    }
}



static void
fprintIndexCounter(FILE *f, IndexCounter *cnt, char *s)
{
    if (! s || ! cnt) {
	fprintf(f, "%-14s %8s %8s %8s %8s %8s %8s\n", "CATEGORY",
		"TOTAL", "INDEX", "AUGMENT", "REORDER", "SPARSE", "EXPAND");
	return;
    }
    
    if (raw) {
	fprintf(f, "%-14s %8u %8u %8u %8u %8u %8u\n", s,
		cnt->total, cnt->index, cnt->augment,
		cnt->reorder, cnt->sparse, cnt->expand);
    } else {
	fprintf(f, "%-14s %8u %7.1f%% %7.1f%% %7.1f%% %7.1f%% %7.1f%%\n", s,
		cnt->total,
		cnt->total ? (double) cnt->index * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->augment * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->reorder * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->sparse * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->expand * 100 / cnt->total : 0);
    }
}



static void
fprintLengthCounter(FILE *f, LengthCounter *cnt, char *s)
{
    if (s) {
	fprintf(f, "%-14s %8u %12u %10u %8u %8u\n", s,
		cnt->total, cnt->description, cnt->reference,
		cnt->units, cnt->format);
    } else {
	fprintf(f, "%-14s %8s %12s %10s %8s %8s\n", "CATEGORY",
		"TOTAL", "DESCRIPTION", "REFERENCE", "UNIT", "FORMAT");
    }
}



static void
fprintMetrics(FILE *f, Metrics *metrics)
{
    unsigned int objects;

    objects = metrics->statusColumns.total + metrics->statusScalars.total;

    fprintStatusCounter(f, NULL, NULL);
    fprintStatusCounter(f, &metrics->statusTypes, "Types:");
    fprintStatusCounter(f, &metrics->statusTables, "Tables:");
    fprintStatusCounter(f, &metrics->statusColumns, "Columns:");
    fprintStatusCounter(f, &metrics->statusScalars, "Scalars:");
    fprintStatusCounter(f, &metrics->statusNotifications, "Notifications:");
    fprintStatusCounter(f, &metrics->statusGroups, "Groups:");
    fprintStatusCounter(f, &metrics->statusCompliances, "Compliances:");
    fprintStatusCounter(f, &metrics->statusAll, "Summary:");
    fprintf(f, "\n");
    fprintAccessCounter(f, NULL, NULL);
    fprintAccessCounter(f, &metrics->accessColumns, "Columns:");
    fprintAccessCounter(f, &metrics->accessScalars, "Scalars:");
    fprintAccessCounter(f, &metrics->accessAll, "Summary:");
    fprintf(f, "\n");
    fprintIndexCounter(f, NULL, NULL);
    fprintIndexCounter(f, &metrics->indexTables, "Tables:");
    fprintf(f, "\n");
    fprintLengthCounter(f, NULL, NULL);
    fprintLengthCounter(f, &metrics->lengthTypes, "Types:");
    fprintLengthCounter(f, &metrics->lengthTables, "Tables:");
    fprintLengthCounter(f, &metrics->lengthColumns, "Columns:");
    fprintLengthCounter(f, &metrics->lengthScalars, "Scalars:");
    fprintLengthCounter(f, &metrics->lengthNotifications, "Notifications:");
    fprintLengthCounter(f, &metrics->lengthAll, "Summary:");
}



static void
dumpMetrics(int modc, SmiModule **modv, int flags, char *output)
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
	for (i = 0; i < modc; i++) {
	    addMetrics(&metrics, modv[i]);
	}
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



void
initMetrics()
{
    static SmidumpDriverOption opt[] = {
	{ "raw", OPT_FLAG, &raw, 0,
	  "generate raw statistics (no percentages)"},
        { 0, OPT_END, 0, 0 }
    };

    static SmidumpDriver driver = {
	"metrics",
	dumpMetrics,
	0,
	0,
	"metrics characterizing MIB modules",
	opt,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}
