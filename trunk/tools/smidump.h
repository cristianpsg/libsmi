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
 * @(#) $Id: smidump.h,v 1.16 2000/11/09 22:29:54 strauss Exp $
 */

#ifndef _SMIDUMP_H
#define _SMIDUMP_H

#include "shhopt.h"



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
 * Driver capability flags which are used to warn about options not
 * understood by a particular output driver.
 */

#define SMIDUMP_DRIVER_CANT_UNITE	0x02
#define SMIDUMP_DRIVER_CANT_OUTPUT	0x04



/*
 * The data structure which represents a driver specific option.
 * A static array of these options (with the last option's type
 * being OPT_END) is used in SmidumpDriver.
 * The SmidumpDriverOption structure is based on shhopt.h:optStruct.
 */

typedef struct SmidumpDriverOption {
    char *name;
    optArgType type;
    void *arg;
    int flags;
    char *descr;
} SmidumpDriverOption;



/*
 * The data structure which represents the entry point for an output
 * driver. The ignflags contain the driver capabilities as described
 * above.
 */

typedef struct SmidumpDriver {
    char *name;				/* Name of the output driver. */
    void (*func) (int, SmiModule **,	/* Output generating function. */
		  int, char *);
    int smiflags;			/* Flags for the SMI parser. */
    int ignflags;			/* Output driver flags ignored. */
    char *descr;			/* Short description. */
    SmidumpDriverOption *opt;           /* Driver specific options. */
    struct SmidumpDriver *next;
} SmidumpDriver;



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

extern void smidumpRegisterDriver(SmidumpDriver *driver);

#endif /* _SMIDUMP_H */
