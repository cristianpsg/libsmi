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
 * @(#) $Id: smidump.c,v 1.1 1999/03/26 17:03:09 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"
#include "dump-sming.h"
#include "dump-data.h"


#define DUMP_SMING	1
#define DUMP_MOSY	2
#define DUMP_OBJECTS	3
#define DUMP_TYPES	4

#define SMIDUMP_CONFIG_FILE "/usr/local/etc/smidump.conf"



int
main(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    int dumpFormat;
    int flags;
    
    dumpFormat = 1;

    smiInit();

#ifdef SMIDUMP_CONFIG_FILE
    smiReadConfig(SMIDUMP_CONFIG_FILE);
#endif
        
    flags = smiGetFlags();
    
    while ((c = getopt(argc, argv, "rRsSvVd:l:c:L:D:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg);
	    break;
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'd':
	    smiSetDebugLevel(atoi(optarg));
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
	    } else if (strstr(optarg, "objects")) {
		dumpFormat = DUMP_OBJECTS;
	    } else if (strstr(optarg, "types")) {
		dumpFormat = DUMP_TYPES;
	    }
	    break;
	default:
	    fprintf(stderr, "Usage: %s [-vVrRsS] [-d level] [-l level] [-c configfile]"
		    " [-L location] [-D mosy|sming|objects|types] module\n", argv[0]);
	    exit(1);
	}
    }

    while (optind < argc) {
	smiLoadModule(argv[optind]);
	switch (dumpFormat) {
	case DUMP_SMING:
	    dumpSming(argv[optind]);
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
