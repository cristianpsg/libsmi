/*
 * mibdump.h --
 *
 *      This header contains the entry points into the modules
 *	which dump MIB contents in various output format.
 *
 * Copyright (c) 2002 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 2002 Torsten Klie, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#ifndef _MIBDUMP_H
#define _MIBDUMP_H

#include "shhopt.h"



/*
 * The following flags can be passed to output drivers in the flags
 * member of the struct above.
 */

#define MIBDUMP_FLAG_SILENT	0x01	/* suppress comments */
#define MIBDUMP_FLAG_UNITE	0x02	/* generated united output */

#define MIBDUMP_FLAG_CURRENT	0x04	/* not yet used */
#define MIBDUMP_FLAG_DEPRECATED	0x08	/* not yet used */
#define MIBDUMP_FLAG_OBSOLETE	0x08	/* not yet used */
#define MIBDUMP_FLAG_COMPACT	0x10	/* not yet used */



/*
 * Driver capability flags which are used to warn about options not
 * understood by a particular output driver.
 */

#define MIBDUMP_DRIVER_CANT_UNITE	0x02
#define MIBDUMP_DRIVER_CANT_OUTPUT	0x04



/*
 * The data structure which represents a driver specific option.
 * A static array of these options (with the last option's type
 * being OPT_END) is used in MibdumpDriver.
 * The MibdumpDriverOption structure is based on shhopt.h:optStruct.
 */

typedef struct MibdumpDriverOption {
    char *name;
    optArgType type;
    void *arg;
    int flags;
    char *descr;
} MibdumpDriverOption;



/*
 * The data structure which represents the entry point for an output
 * driver. The ignflags contain the driver capabilities as described
 * above.
 */

typedef struct MibdumpDriver {
    char *name;				/* Name of the output driver. */
    void (*func) (int, SmiModule **,	/* Output generating function. */
		  int, char *);
    int smiflags;			/* Flags for the SMI parser. */
    int ignflags;			/* Output driver flags ignored. */
    char *descr;			/* Short description. */
    MibdumpDriverOption *opt;           /* Driver specific options. */
    struct MibdumpDriver *next;
} MibdumpDriver;



/*
 * The entry points for the output drivers that currently exist for
 * mibdump.
 */

extern void initXml(void);


/*
 * The functions are wrappers for the malloc functions which handle
 * memory allocation errors by terminating the program.
 */

extern void *xmalloc(size_t size);
extern void *xrealloc(void *ptr, size_t size);
extern char *xstrdup(const char *s);
extern void xfree(void *ptr);

extern void mibdumpRegisterDriver(MibdumpDriver *driver);

#endif /* _MIBDUMP_H */
