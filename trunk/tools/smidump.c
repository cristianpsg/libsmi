/*
 * smidump.c --
 *
 *      Dump a MIB module conforming to a given format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smidump.c,v 1.14 1999/06/22 11:18:17 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "smi.h"
#include "dump-smi.h"
#include "dump-sming.h"
#include "dump-data.h"
#include "dump-imports.h"
#include "dump-mosy.h"
#include "dump-java.h"
#include "dump-jdmk.h"



typedef struct {
    char *name;				  /* Name of the output driver. */
    int (*func) (const char *moduleName); /* Output generating function. */
} Driver;
 


static Driver driverTable[] = {
    { "SMIng",	 dumpSming },
    { "SMIv1",	 dumpSmiV1 },
    { "SMIv2",	 dumpSmiV2 },
    { "MOSY",	 dumpMosy },
    { "Imports", dumpImports },
    { "Java",    dumpJava },
    { "JDMK",    dumpJdmk },
#if 0
    { "Objects", dumpMibTree },
    { "Types",	 dumpTypes },
#endif
    { NULL, NULL }
};



void usage()
{
    Driver *driver = driverTable;
    
    fprintf(stderr,
	    "Usage: smidump [-Vh] [-f <format>] [-p <module>] <module_or_path>\n"
	    "-V                        show version and license information\n"
	    "-h                        show usage information\n"
	    "-f <format>               use <format> when dumping (default %s)\n", driver->name);
    fprintf(stderr,
	    "                          supported formats:");
    for (driver = driverTable; driver->name; driver++) {
	fprintf(stderr, " %s", driver->name);
    }
    fprintf(stderr, "\n");
    fprintf(stderr,
	    "-p <module>               preload <module>\n"
	    "<module_or_path>          plain name of MIB module or file path\n");
}



static void version()
{
    printf("smidump " VERSION "\n");
}



int
main(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    char *modulename;
    int flags;
    int errors = 0;
    Driver *driver = driverTable;
    
    smiInit();

    flags = smiGetFlags();
    
    while ((c = getopt(argc, argv, "Vhf:p:")) != -1) {
	switch (c) {
	case 'V':
	    version();
	    exit(0);
	case 'h':
	    usage();
	    exit(0);
	case 'p':
	    smiLoadModule(optarg);
	    break;
	case 'f':
	    for (driver = driverTable; driver->name; driver++) {
		if (strcasecmp(driver->name, optarg) == 0) {
		    break;
		}
	    }
	    if (!driver->name) {
		fprintf(stderr, "smidump: invalid dump format `%s'\n", optarg);
		exit(1);
	    }
	    break;
	default:
	    usage();
	    exit(1);
	}
    }

    while (optind < argc) {
	modulename = smiLoadModule(argv[optind]);
	if (modulename) {
	    errors += (driver->func)(modulename);
	} else {
	    fprintf(stderr, "smidump: cannot locate module `%s'\n",
		    argv[optind]);
	    exit(1);
	}
	optind++;
    }
    
    exit(errors);
}
