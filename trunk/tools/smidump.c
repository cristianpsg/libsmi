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
 * @(#) $Id: smidump.c,v 1.8 1999/05/28 14:10:31 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "smi.h"
#include "dump-smi.h"
#include "dump-sming.h"
#include "dump-data.h"

#define DUMP_SMING	1
#define DUMP_MOSY	2
#define DUMP_OBJECTS	3
#define DUMP_TYPES	4
#define DUMP_SMIV1      5
#define DUMP_SMIV2      6



void
usage()
{
    fprintf(stderr,
	    "Usage: smidump [-Vh] [-f <format>] [-p <module>] <module_or_path>\n"
	    "-V                        show version and license information\n"
	    "-h                        show usage information\n"
	    "-f <format>               use <format> when dumping\n"
	    "-p <module>               preload <module>\n"
	    "<module_or_path>          plain name of MIB module or file path\n"
	    "   supported formats are: sming, smiv2, mosy\n");
}



void
version()
{
    printf("smidump " VERSION "\n");
}



int
main(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    char *dumpFormat = NULL;
    int flags;
    int errors = 0;
    
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
	    dumpFormat = optarg;
	    break;
	default:
	    usage();
	    exit(1);
	}
    }

    while (optind < argc) {
	smiLoadModule(argv[optind]);
	if ((!dumpFormat) || (!strcasecmp(dumpFormat, "SMIng"))) {
	    errors += dumpSming(argv[optind]);
	} else if (!strcasecmp(dumpFormat, "SMIv1")) {
	    errors += dumpSmiV1(argv[optind]);
	} else if (!strcasecmp(dumpFormat, "SMIv2")) {
	    errors += dumpSmiV2(argv[optind]);
#if 0
	} else if (!strcasecmp(dumpFormat, "MOSY")) {
	    errors += dumpMosy();
	} else if (!strcasecmp(dumpFormat, "Objects")) {
	    errors += dumpMibTree();
	} else if (!strcasecmp(dumpFormat, "Types")) {
	    errors += dumpTypes();
#endif
	} else {
	    fprintf(stderr, "Unsupported dump format `%s'\n", dumpFormat);
	    exit(1);
	}
	optind++;
    }
    
    exit(errors);
}
