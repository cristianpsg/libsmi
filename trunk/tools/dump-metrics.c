/*
 * dump-metrics.c --
 *
 *      Operations to compute and dump some MIB metrics.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
 * Copyright (c) 2002 J. Schoenwaelder, University of Osnabrueck.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-metrics.c,v 1.9 2002/11/19 10:34:34 schoenw Exp $
 */

/*
  # revisions
  # imports
  # row creations:
  # <basetype usage distribution>
  # <complexity of table indexes>
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"


static int raw = 0;

static int silent = 0;


typedef struct StatusCounter {
    unsigned long total;
    unsigned long current;
    unsigned long deprecated;
    unsigned long obsolete;
} StatusCounter;



typedef struct AccessCounter {
    unsigned long total;
    unsigned long noaccess;
    unsigned long notify;
    unsigned long readonly;
    unsigned long readwrite;
} AccessCounter;



typedef struct IndexCounter {
    unsigned long total;
    unsigned long index;
    unsigned long augment;
    unsigned long reorder;
    unsigned long sparse;
    unsigned long expand;
} IndexCounter;



typedef struct IndexLenCounter {
    unsigned long total;
    unsigned long length[11];
} IndexLenCounter;



typedef struct LengthCounter {
    unsigned long total;
    unsigned long descr;
    unsigned long descr_len;
    unsigned long reference;
    unsigned long reference_len;
    unsigned long units;
    unsigned long units_len;
    unsigned long format;
    unsigned long format_len;
} LengthCounter;



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
    IndexLenCounter indexLenTables;
    LengthCounter lengthTypes;
    LengthCounter lengthTables;
    LengthCounter lengthRows;
    LengthCounter lengthColumns;
    LengthCounter lengthScalars;
    LengthCounter lengthNotifications;
    LengthCounter lengthAll;
} Metrics;



typedef struct UsageCounter {
    char     *module;
    char     *name;
    unsigned count;
    struct UsageCounter *nextPtr;
} UsageCounter;


static UsageCounter *extTypeList = NULL;
static UsageCounter *extNodeList = NULL;
static UsageCounter *extModuleList = NULL;


typedef void	(*ForEachIndexFunc)	(FILE *f, SmiNode *groupNode, SmiNode *smiNode, void *data);

static void
foreachIndexDo(FILE *f, SmiNode *smiNode, ForEachIndexFunc func, void *data)
{
    SmiNode *indexNode = NULL, *iNode;
    SmiElement *smiElement;
    
    switch (smiNode->indexkind) {
    case SMI_INDEX_INDEX:
    case SMI_INDEX_REORDER:
	indexNode = smiNode;
	break;
    case SMI_INDEX_EXPAND:	/* TODO: we have to do more work here! */
	break;
    case SMI_INDEX_AUGMENT:
    case SMI_INDEX_SPARSE:
	indexNode = smiGetRelatedNode(smiNode);
	break;
    case SMI_INDEX_UNKNOWN:
	break;
    }
    if (indexNode) {
	for (smiElement = smiGetFirstElement(indexNode);
	     smiElement; smiElement = smiGetNextElement(smiElement)) {
	    iNode = smiGetElementNode(smiElement);
	    if (iNode) {
		(func) (f, smiNode, iNode, data);
	    }
	}
    }
}



static UsageCounter*
incrUsageCounter(UsageCounter *typeList, char *module, char *name)
{
    UsageCounter *uCnt;

    for (uCnt = typeList; uCnt; uCnt = uCnt->nextPtr) {
	if (strcmp(uCnt->module, module) == 0
	    && (! name || strcmp(uCnt->name, name) == 0)) {
	    break;
	}
    }

    if (! uCnt) {
	uCnt = (UsageCounter *) xmalloc(sizeof(UsageCounter));
	uCnt->module = xstrdup(module);
	uCnt->name = name ? xstrdup(name) : NULL;
	uCnt->count = 0;
	uCnt->nextPtr = typeList;
	typeList = uCnt;
    }

    uCnt->count++;
    return typeList;
}



static void
incrNodeUsageCounter(SmiModule *smiModule, SmiNode *smiNode)
{
    SmiType *smiType;
    
    smiType = smiGetNodeType(smiNode);
    if (! smiType) {
	return;
    }
    
   if (smiType->name) {
	char *extModule = smiGetTypeModule(smiType)->name;
	if (extModule && *extModule
	    && strcmp(extModule, smiModule->name) != 0) {
	    extTypeList = incrUsageCounter(extTypeList,
					   extModule, smiType->name);
	    extModuleList = incrUsageCounter(extModuleList, extModule, NULL);
	}
    }
}


static int
cmp(const void *va, const void *vb)
{
    UsageCounter **a = (UsageCounter **) va;
    UsageCounter **b = (UsageCounter **) vb;

    if ((*a)->count > (*b)->count) return -1;
    if ((*a)->count < (*b)->count) return 1;
    return 0;
}


static void
fprintTypeUsage(FILE *f, UsageCounter *typeUsageList)
{
    UsageCounter *uCnt;
    int modLen = 8, nameLen = 8;
    unsigned total = 0;
    int i, cnt = 0;
    UsageCounter **sortCnt;

    /* should be sorted */

    for (uCnt = typeUsageList, cnt = 0; uCnt; uCnt = uCnt->nextPtr, cnt++) {
	if (modLen < strlen(uCnt->module)) {
	    modLen = strlen(uCnt->module);
	}
	if (nameLen < strlen(uCnt->name)) {
	    nameLen = strlen(uCnt->name);
	}
	total += uCnt->count;
    }

    if (cnt == 0) {
	return;
    }

    /* create an array for a quick qsort */

    sortCnt = (UsageCounter **) xmalloc(cnt * sizeof(UsageCounter *));
    memset(sortCnt, 0, cnt * sizeof(UsageCounter *));
    for (uCnt = typeUsageList, i = 0; uCnt; uCnt = uCnt->nextPtr, i++) {
	sortCnt[i] = uCnt;
    }
    qsort(sortCnt, cnt, sizeof(UsageCounter *), cmp);
    
    if (! silent) {
	fputs(
"# The following table shows the distribution of the number of references\n"
"# to externally defined types in the set of loaded MIB modules.\n"
"\n", f);
    }
    fprintf(f, "%-*s %-*s EXT-USAGE\n", modLen, "MODULE", nameLen, "TYPE");

    for (i = 0; i < cnt; i++) {
	fprintf(f, "%-*s %-*s ",
		modLen, sortCnt[i]->module, nameLen, sortCnt[i]->name);
	if (raw) {
	    fprintf(f, "%8u\n", sortCnt[i]->count);
	} else {
	    fprintf(f, "%6.1f%%\n", (double) sortCnt[i]->count * 100 / total);
	}
    }

    xfree(sortCnt);
}



static void
fprintNodeUsage(FILE *f, UsageCounter *typeUsageList)
{
    UsageCounter *uCnt;
    int modLen = 8, nameLen = 8;
    unsigned total = 0;
    int i, cnt = 0;
    UsageCounter **sortCnt;

    /* should be sorted */

    for (uCnt = typeUsageList, cnt = 0; uCnt; uCnt = uCnt->nextPtr, cnt++) {
	if (modLen < strlen(uCnt->module)) {
	    modLen = strlen(uCnt->module);
	}
	if (nameLen < strlen(uCnt->name)) {
	    nameLen = strlen(uCnt->name);
	}
	total += uCnt->count;
    }

    if (cnt == 0) {
	return;
    }

    /* create an array for a quick qsort */

    sortCnt = (UsageCounter **) xmalloc(cnt * sizeof(UsageCounter *));
    memset(sortCnt, 0, cnt * sizeof(UsageCounter *));
    for (uCnt = typeUsageList, i = 0; uCnt; uCnt = uCnt->nextPtr, i++) {
	sortCnt[i] = uCnt;
    }
    qsort(sortCnt, cnt, sizeof(UsageCounter *), cmp);
    
    if (! silent) {
	fputs(
"# The following table shows the distribution of the number of references\n"
"# to externally defined nodes in the set of loaded MIB modules.\n"
"\n", f);
    }
    fprintf(f, "%-*s %-*s EXT-USAGE\n", modLen, "MODULE", nameLen, "NODE");

    for (i = 0; i < cnt; i++) {
	fprintf(f, "%-*s %-*s ",
		modLen, sortCnt[i]->module, nameLen, sortCnt[i]->name);
	if (raw) {
	    fprintf(f, "%8u\n", sortCnt[i]->count);
	} else {
	    fprintf(f, "%6.1f%%\n", (double) sortCnt[i]->count * 100 / total);
	}
    }

    xfree(sortCnt);
}



static void
fprintModuleUsage(FILE *f, UsageCounter *modUsageList)
{
    UsageCounter *uCnt;
    int modLen = 8;
    unsigned total = 0;
    int i, cnt = 0;
    UsageCounter **sortCnt;

    /* should be sorted */

    for (uCnt = modUsageList, cnt = 0; uCnt; uCnt = uCnt->nextPtr, cnt++) {
	if (modLen < strlen(uCnt->module)) {
	    modLen = strlen(uCnt->module);
	}
	total += uCnt->count;
    }

    if (cnt == 0) {
	return;
    }

    /* create an array for a quick qsort */

    sortCnt = (UsageCounter **) xmalloc(cnt * sizeof(UsageCounter *));
    memset(sortCnt, 0, cnt * sizeof(UsageCounter *));
    for (uCnt = modUsageList, i = 0; uCnt; uCnt = uCnt->nextPtr, i++) {
	sortCnt[i] = uCnt;
    }
    qsort(sortCnt, cnt, sizeof(UsageCounter *), cmp);

    if (! silent) {
	fputs(
"# The following table shows the distribution of the number of references\n"
"# to externally defined items (such as types or objects) accumulated by\n"
"# defining MIB module in the set of loaded MIB modules.\n"
"\n", f);
    }
    fprintf(f, "%-*s EXT-USAGE\n", modLen, "MODULE");

    for (i = 0; i < cnt; i++) {
	fprintf(f, "%-*s ", modLen, sortCnt[i]->module);
	if (raw) {
	    fprintf(f, "%8u\n", sortCnt[i]->count);
	} else {
	    fprintf(f, "%6.1f%%\n", (double) sortCnt[i]->count * 100 / total);
	}
    }

    xfree(sortCnt);
}



static void
freeUsageCounter(UsageCounter *usageCounterList)
{
    UsageCounter *uCnt, *p;
    
    for (uCnt = usageCounterList; uCnt; ) {
	p = uCnt, uCnt = uCnt->nextPtr;
	xfree(p->module);
	xfree(p->name);
	xfree(p);
    }
}


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
incrIndexLenCounter(IndexLenCounter *cnt, int len)
{
    cnt->total++;
    cnt->length[len]++;
}



static void
incrLengthCounter(LengthCounter *cnt, char *description, char *reference,
		  char *units, char *format)
{
    cnt->total++;
    if (description) {
	cnt->descr++;
	cnt->descr_len += strlen(description);
    }

    if (reference) {
	cnt->reference++;
	cnt->reference_len += strlen(reference);
    }
    if (units) {
	cnt->units++;
	cnt->units_len += strlen(units);
    }
    if (format) {
	cnt->format++;
	cnt->format_len += strlen(format);
    }
}



static void
count(FILE *f, SmiNode *row, SmiNode *col, void *data)
{
    int *cnt = (int *) data;

    (*cnt)++;
}


static void
yadayada(FILE *f, SmiNode *row, SmiNode *col, void *data)
{
    SmiModule *smiModule = (SmiModule *) data;

    if (col->name) {
	char *extModule = smiGetNodeModule(col)->name;
	if (extModule && *extModule
	    && strcmp(extModule, smiModule->name) != 0) {
	    extNodeList = incrUsageCounter(extNodeList,
					   extModule, col->name);
	    extModuleList = incrUsageCounter(extModuleList, extModule, NULL);
	}
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
	    {
		int cnt = 0;
		foreachIndexDo(NULL, smiNode, count, &cnt);
		incrIndexLenCounter(&metrics->indexLenTables, cnt);
		foreachIndexDo(NULL, smiNode, yadayada, smiModule);
	    }
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
	    incrNodeUsageCounter(smiModule, smiNode);
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
	    incrNodeUsageCounter(smiModule, smiNode);
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

	/*
	 * Ignore all types with empty descriptions coming from the
	 * "SNMPv2-SMI" module since they are not really defined
	 * types but part of the language itself.
	 */

	if (! smiType->description) {
	    SmiModule *m = smiGetTypeModule(smiType);
	    if (m && strcmp(m->name, "SNMPv2-SMI") == 0) {
		continue;
	    }
	}
    
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
	if (! silent) {
	    fputs(
"# The following table shows the status distribution of various\n"
"# definitions contained in the set of loaded MIB modules.\n"
"\n", f);
	}
	fprintf(f, "%-14s %8s %8s %11s %9s\n", "CATEGORY",
		"TOTAL", "CURRENT", "DEPRECATED", "OBSOLETE");
	return;
    }

    if (raw) {
	fprintf(f, "%-14s %8lu %8lu %11lu %9lu\n", s,
		cnt->total, cnt->current, cnt->deprecated, cnt->obsolete);
    } else {
	fprintf(f, "%-14s %8lu %7.1f%% %10.1f%% %8.1f%%\n", s,
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
	if (! silent) {
	    fputs(
"# The following table shows the access mode distribution of all scalar\n"
"# or column definitions contained in the set of loaded MIB modules.\n"
"\n", f);
	}
	fprintf(f, "%-14s %8s %10s %9s %7s %8s\n", "CATEGORY",
		"TOTAL", "READWRITE", "READONLY", "NOTIFY", "NOACCES");
	return;
    }

    if (raw) {
	fprintf(f, "%-14s %8lu %10lu %9lu %7lu %8lu\n", s,
		cnt->total, cnt->readwrite, cnt->readonly,
		cnt->notify, cnt->noaccess);
    } else {
	fprintf(f, "%-14s %8lu %9.1f%% %8.1f%% %6.1f%% %7.1f%%\n", s,
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
	if (! silent) {
	    fputs(
"# The following table shows the table index kind distribution of\n"
"# table definitions contained in the set of loaded MIB modules.\n"
"\n", f);
	fprintf(f, "%-14s %8s %8s %8s %8s %8s %8s\n", "CATEGORY",
		"TOTAL", "INDEX", "AUGMENT", "REORDER", "SPARSE", "EXPAND");
	}
	return;
    }
    
    if (raw) {
	fprintf(f, "%-14s %8lu %8lu %8lu %8lu %8lu %8lu\n", s,
		cnt->total, cnt->index, cnt->augment,
		cnt->reorder, cnt->sparse, cnt->expand);
    } else {
	fprintf(f, "%-14s %8lu %7.1f%% %7.1f%% %7.1f%% %7.1f%% %7.1f%%\n", s,
		cnt->total,
		cnt->total ? (double) cnt->index * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->augment * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->reorder * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->sparse * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->expand * 100 / cnt->total : 0);
    }
}



static void
fprintIndexLenCounter(FILE *f, IndexLenCounter *cnt, char *s)
{
    int i;
    
    if (! s || ! cnt) {
	if (! silent) {
	    fputs(
"# The following table shows the table index length distribution of\n"
"# table definitions contained in the set of loaded MIB modules.\n"
"\n", f);
	}
	fprintf(f, "%-14s %6s %5s %5s %5s %5s %5s %5s %5s %5s\n",
		"CATEGORY", "TOTAL",
		"[1]", "[2]", "[3]", "[4]", "[5]", "[6]", "[7]", "[8]");
	return;
    }

    fprintf(f, "%-14s %6lu", s, cnt->total);
    if (raw) {
	for (i = 1; i < 9; i++) {
	    fprintf(f, " %5lu", cnt->length[i]);
	}
    } else {
	for (i = 1; i < 9; i++) {
	    fprintf(f, " %4.1f%%", (double) cnt->length[i] * 100 / cnt->total);
	}
    }
    
    fprintf(f, "\n");
}



static void
fprintLengthCounter(FILE *f, LengthCounter *cnt, char *s)
{
    if (! s) {
	if (! silent) {
	    fputs(
"# The following table shows the text clause usage distribution of all\n"
"# definitions contained in the set of loaded MIB modules.\n"
"\n", f);
	}
	fprintf(f, "%-14s %8s %12s %10s %8s %8s\n", "CATEGORY",
		"TOTAL", "DESCRIPTION", "REFERENCE", "UNIT", "FORMAT");
	return;
    }
    
    if (raw) {
	fprintf(f, "%-14s %8lu %12lu %10lu %8lu %8lu\n", s,
		cnt->total, cnt->descr, cnt->reference,
		cnt->units, cnt->format);
    } else {
	fprintf(f, "%-14s %8lu %11.1f%% %9.1f%% %7.1f%% %7.1f%%\n", s,
		cnt->total,
		cnt->total ? (double) cnt->descr * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->reference * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->units * 100 / cnt->total : 0,
		cnt->total ? (double) cnt->format * 100 / cnt->total : 0);
    }
}



static void
fprintLengthCounter2(FILE *f, LengthCounter *cnt, char *s)
{
    if (! s) {
	if (! silent) {
	    fputs(
"# The following table shows the text clause length distribution of all\n"
"# definitions contained in the set of loaded MIB modules.\n"
"\n", f);
	}
	fprintf(f, "%-14s %8s %12s %10s %8s %8s\n", "CATEGORY",
		"TOTAL", "DESCRIPTION", "REFERENCE", "UNIT", "FORMAT");
	return;
    }
    
    if (raw) {
	fprintf(f, "%-14s %8lu %12lu %10lu %8lu %8lu\n", s,
		cnt->total, cnt->descr_len, cnt->reference_len,
		cnt->units_len, cnt->format_len);
    } else {
	fprintf(f, "%-14s %8lu %11.1fb %9.1fb %7.1fb %7.1fb\n", s,
		cnt->total,
		cnt->descr ? (double) cnt->descr_len / cnt->descr : 0,
		cnt->reference ? (double) cnt->reference_len / cnt->reference : 0,
		cnt->units ? (double) cnt->units_len / cnt->units : 0,
		cnt->format ? (double) cnt->format_len / cnt->format : 0);
    }
}



static void
fprintMetrics(FILE *f, Metrics *metrics)
{
    unsigned long objects;

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
    fprintIndexLenCounter(f, NULL, NULL);
    fprintIndexLenCounter(f, &metrics->indexLenTables, "Tables:");
    fprintf(f, "\n");
    fprintLengthCounter(f, NULL, NULL);
    fprintLengthCounter(f, &metrics->lengthTypes, "Types:");
    fprintLengthCounter(f, &metrics->lengthTables, "Tables:");
    fprintLengthCounter(f, &metrics->lengthColumns, "Columns:");
    fprintLengthCounter(f, &metrics->lengthScalars, "Scalars:");
    fprintLengthCounter(f, &metrics->lengthNotifications, "Notifications:");
    fprintLengthCounter(f, &metrics->lengthAll, "Summary:");
    fprintf(f, "\n");
    fprintLengthCounter2(f, NULL, NULL);
    fprintLengthCounter2(f, &metrics->lengthTypes, "Types:");
    fprintLengthCounter2(f, &metrics->lengthTables, "Tables:");
    fprintLengthCounter2(f, &metrics->lengthColumns, "Columns:");
    fprintLengthCounter2(f, &metrics->lengthScalars, "Scalars:");
    fprintLengthCounter2(f, &metrics->lengthNotifications, "Notifications:");
    fprintLengthCounter2(f, &metrics->lengthAll, "Summary:");
    fprintf(f, "\n");
    fprintTypeUsage(f, extTypeList);
    freeUsageCounter(extTypeList), extTypeList = NULL;
    fprintf(f, "\n");
    fprintNodeUsage(f, extNodeList);
    freeUsageCounter(extNodeList), extNodeList = NULL;
    fprintf(f, "\n");
    fprintModuleUsage(f, extModuleList);
    freeUsageCounter(extModuleList), extModuleList = NULL;
}



static void
dumpMetrics(int modc, SmiModule **modv, int flags, char *output)
{
    Metrics   metrics;
    int       i;
    FILE      *f = stdout;

    silent = (flags & SMIDUMP_FLAG_SILENT);

    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

    if (flags & SMIDUMP_FLAG_UNITE) {
	if (! silent) {
	    int pos = 8888;
	    fprintf(f, "# united module metrics (generated by smidump "
		    SMI_VERSION_STRING ")\n");
	    fprintf(f, "#\n# smidump -u -f metrics");
	    if (raw) fprintf(f, " --metrics-raw");
	    for (i = 0; i < modc; i++) {
		int len = strlen(modv[i]->name);
		if (pos + len > 70) {
		    fprintf(f, " \\\n#\t"), pos = 8;
		}
		fprintf(f, "%s ", modv[i]->name);
		pos += len + 1;
	    }
	    fprintf(f, "%s\n", (pos == 8) ? "" : "\n");
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
	    if (! silent) {
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
