/*
 * smidump.c --
 *
 *      Dump a MIB module conforming to a given format.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smidump.c,v 1.4 1999/04/09 18:47:07 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

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

#define SMIDUMP_CONFIG_FILE "/usr/local/etc/smidump.conf"



int
main(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    int dumpFormat;
    int flags;
    int config;
    
    dumpFormat = 1;
    config = 0;

    smiInit();

    flags = smiGetFlags();
    
    while ((c = getopt(argc, argv, "rRsSvVl:c:L:D:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg);
	    config++;
	    break;
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'v':
	    flags |= SMI_ERRORLINES;
	    smiSetFlags(flags);
	    break;
	case 'V':
	    flags &= ~SMI_ERRORLINES;
	    smiSetFlags(flags);
	    break;
	case 's':
	    /* print some module statistics */
	    flags |= SMI_STATS;
	    smiSetFlags(flags);
	    break;
	case 'S':
	    flags &= ~SMI_STATS;
	    smiSetFlags(flags);
	    break;
	case 'L':
	    smiAddLocation(optarg);
	    break;
	case 'D':
	    if (strstr(optarg, "mosy")) {
		dumpFormat = DUMP_MOSY;
	    } else if (strstr(optarg, "sming")) {
		dumpFormat = DUMP_SMING;
	    } else if (strstr(optarg, "smiv1")) {
		dumpFormat = DUMP_SMIV1;
	    } else if (strstr(optarg, "smiv2")) {
		dumpFormat = DUMP_SMIV2;
	    } else if (strstr(optarg, "objects")) {
		dumpFormat = DUMP_OBJECTS;
	    } else if (strstr(optarg, "types")) {
		dumpFormat = DUMP_TYPES;
	    }
	    break;
	default:
	    fprintf(stderr, "Usage: %s [-vVrRsS] [-l level] [-c configfile]"
		    " [-L location] [-D mosy|sming|smiv1|smiv2|objects|types] module\n", argv[0]);
	    exit(1);
	}
    }

#ifdef SMIDUMP_CONFIG_FILE
    if (!config)
	smiReadConfig(SMIDUMP_CONFIG_FILE);
#endif
        
    while (optind < argc) {
	smiLoadModule(argv[optind]);
	switch (dumpFormat) {
	case DUMP_SMING:
	    dumpSming(argv[optind]);
	    break;
	case DUMP_SMIV1:
	    dumpSmiV1(argv[optind]);
	    break;
	case DUMP_SMIV2:
	    dumpSmiV2(argv[optind]);
	    break;
	case DUMP_OBJECTS:
	    dumpMibTree();
	    break;
	case DUMP_TYPES:
	    dumpTypes();
	    break;
#if 0
	case DUMP_MOSY:
	    dumpMosy();
	    break;
#endif
	}
	optind++;
    }
    
    exit(0);
}
