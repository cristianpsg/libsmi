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
 * @(#) $Id: smidump.h,v 1.15 2000/11/08 18:11:09 strauss Exp $
 */

#ifndef _SMIDUMP_H
#define _SMIDUMP_H


/*
 * The following flags can be passed to output drivers in the flags
 * member of the struct above.
 */

#define SMIDUMP_FLAG_SILENT	0x01	/* suppress comments */
#define SMIDUMP_FLAG_UNITE	0x02	/* generated united output */

#define SMIDUMP_FLAG_CURRENT	0x04	/* not yet used */
#define SMIDUMP_FLAG_DEPRECATED	0x08	/* not yet used */
#define SMIDUMP_FLAG_OBSOLETE	0x08	/* not yet used */
#define SMIDUMP_FLAG_COMPACT	0x10	/* not yet used */


/*
 * The entry points for the output drivers that currently exist for
 * smidump.
 */

extern void dumpSmiV1(int modc, SmiModule **modv, int flags, char *output);
extern void dumpSmiV2(int modc, SmiModule **modv, int flags, char *output);
extern void dumpSming(int modc, SmiModule **modv, int flags, char *output);
extern void dumpImports(int modc, SmiModule **modv, int flags, char *output);
extern void dumpTypes(int modc, SmiModule **modv, int flags, char *output);
extern void dumpTree(int modc, SmiModule **modv, int flags, char *output);
extern void dumpIdentifiers(int modc, SmiModule **modv, int flags, char *output);
extern void dumpMetrics(int modc, SmiModule **modv, int flags, char *output);
extern void dumpMosy(int modc, SmiModule **modv, int flags, char *output);
extern void dumpXml(int modc, SmiModule **modv, int flags, char *output);
extern void dumpCorba(int modc, SmiModule **modv, int flags, char *output);
extern void dumpCMDia(int modc, SmiModule **modv, int flags, char *output);
extern void dumpCMXplain(int modc, SmiModule **modv, int flags, char *output);
extern void dumpNetSnmp(int modc, SmiModule **modv, int flags, char *output);
extern void dumpJax(int modc, SmiModule **modv, int flags, char *output);
extern void dumpPython(int modc, SmiModule **modv, int flags, char *output);

extern void dumpSql(int modc, SmiModule **modv, int flags, char *output);
extern void dumpFigTree(int modc, SmiModule **modv, int flags, char *output);
extern void dumpFigUml(int modc, SmiModule **modv, int flags, char *output);


/*
 * The functions are wrappers for the malloc functions which handle
 * memory allocation errors by terminating the program.
 */

extern void *xmalloc(size_t size);
extern void *xrealloc(void *ptr, size_t size);
extern char *xstrdup(const char *s);
extern void xfree(void *ptr);


#endif /* _SMIDUMP_H */
