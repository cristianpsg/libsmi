/*
 * smidump.h --
 *
 *      This header contains the entry points into the modules
 *	which dump MIB modules in various output format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smidump.h,v 1.12 2000/05/26 16:17:49 strauss Exp $
 */

#ifndef _SMIDUMP_H
#define _SMIDUMP_H


/*
 * Smidump allocates the following structure for each module given on
 * the command line. A pointer to this structure is passed to the
 * output drivers. The structure is in fact the root of a linked list
 * of all modules already passed to the output driver, which is useful
 * when generating united output.
 */

typedef struct Module {
    SmiModule	  *smiModule;	/* SMI module to dump (may be NULL) */
    int		  flags;	/* flags passed to the output driver */
    struct Module *nextPtr;	/* next already processed module */
} Module;


/*
 * The following flags can be passed to output drivers in the flags
 * member of the struct above.
 */

#define SMIDUMP_FLAG_SILENT	0x01
#define SMIDUMP_FLAG_UNITE	0x02
#define SMIDUMP_FLAG_CURRENT	0x04
#define SMIDUMP_FLAG_DEPRECATED	0x08
#define SMIDUMP_FLAG_OBSOLETE	0x08
#define SMIDUMP_FLAG_COMPACT	0x10


/*
 * The entry points for the output drivers that currently exist for
 * smidump.
 */

extern void dumpSmiV1(Module *module);
extern void dumpSmiV2(Module *module);
extern void dumpSming(Module *module);
extern void dumpImports(Module *module);
extern void dumpTypes(Module *module);
extern void dumpTree(Module *module);
extern void dumpIdentifiers(Module *module);
extern void dumpMetrics(Module *module);
extern void dumpMosy(Module *module);
extern void dumpXml(Module *module);
extern void dumpCorbaIdl(Module *module);
extern void dumpCorbaOid(Module *module);
extern void dumpCMDia(Module *module);
extern void dumpCMXplain(Module *module);
extern void dumpUcdH(Module *module);
extern void dumpUcdC(Module *module);
extern void dumpJax(Module *module);

extern void dumpSql(Module *module);
extern void dumpFigTree(Module *module);
extern void dumpFigUml(Module *module);


/*
 * The functions are wrappers for the malloc functions which handle
 * memory allocation errors by terminating the program.
 */

extern void *xmalloc(size_t size);
extern void *xrealloc(void *ptr, size_t size);
extern char *xstrdup(const char *s);
extern void xfree(void *ptr);


#endif /* _SMIDUMP_H */
