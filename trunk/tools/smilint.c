/*
 * smilint.c --
 *
 *      MIB module checker main program.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smilint.c,v 1.4 1999/03/15 11:07:18 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"
#include "defs.h"
#include "error.h"
#include "scanner-smi.h"
#include "parser-smi.tab.h"
#include "data.h"

extern int yydebug;

char module[MAX_IDENTIFIER_LENGTH+1];



/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *      The main function.
 *
 * Results:
 *      TODO.
 *
 * Side effects:
 *      TODO.
 *
 *----------------------------------------------------------------------
 */

int
main(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    int dumpObjectsFlag, dumpTypesFlag, dumpMosyFlag, dumpSmingFlag;
    int flags;
    
    dumpObjectsFlag = 0;
    dumpTypesFlag = 0;
    dumpMosyFlag = 0;
    dumpSmingFlag = 0;

    strcpy(module, "");

    smiInit();
    
    smiSetDebugLevel(0);
    smiSetErrorLevel(3);
    flags = SMI_ERRORS | SMI_ERRORLINES;
    smiSetFlags(flags);
    
#ifdef SMI_CONFIG_FILE
    smiReadConfig(SMI_CONFIG_FILE);
#endif
    
    while ((c = getopt(argc, argv, "MDrRsSvVyYd:l:c:m:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg);
	    break;
	case 'm':
	    strncpy(module, optarg,
		    sizeof(module)-1);
	    break;
	case 'y':
	    yydebug = 1;
	    break;
	case 'Y':
	    yydebug = 0;
	    break;
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'd':
	    smiSetDebugLevel(atoi(optarg));
	    break;
	case 'v':
	    flags |= SMI_ERRORLINES;
	    break;
	case 'V':
	    flags &= ~SMI_ERRORLINES;
	    break;
	case 'r':
	    /* errors and statistics (if -s present) for imported modules */
	    flags |= SMI_RECURSIVE;
	    break;
	case 'R':
	    flags &= ~SMI_RECURSIVE;
	    break;
	case 's':
	    /* print some module statistics */
	    flags |= SMI_STATS;
	    break;
	case 'S':
	    flags &= ~SMI_STATS;
	    break;
	case 'D':
	    if (!strstr(optarg, "mosy")) dumpMosyFlag = 1;
	    if (!strstr(optarg, "sming")) dumpSmingFlag = 1;
	    if (!strstr(optarg, "objects")) dumpObjectsFlag = 1;
	    if (!strstr(optarg, "types")) dumpTypesFlag = 1;
	    break;
	default:
	    fprintf(stderr, "Usage: %s [-yYvVrRsS] [-d level] [-l level] [-c configfile]"
		    " [-m module] [-D mosy|sming|objects|types]\n", argv[0]);
	    exit(1);
	}
    }

    smiSetFlags(flags);
    
    while (optind < argc) {
	if (strlen(module)) {
	    smiAddLocation(argv[optind]);
	    smiLoadModule(module);
	} else {
	    smiAddLocation(argv[optind]);
	}
#if 0
	if (dumpObjectsFlag) {
	    dumpMibTree(rootNodePtr, "");
	}
	if (dumpTypesFlag) {
	    dumpTypes();
	}
	if (dumpMosyFlag) {
	    dumpMosy();
	}
	if (dumpSmingFlag) {
	    dumpSming();
	}
#endif
	optind++;
    }
    
    exit(0);
}
