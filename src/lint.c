/*
 * lint.c --
 *
 *      MIB module checker main program.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: lint.c,v 1.4 1998/11/04 02:14:57 strauss Exp $
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
#include "config.h"
#include "error.h"
#include "scanner.h"
#include "parser-bison.h"
#include "data.h"

/* flex/bison */
extern int yydebug;
extern int yyparse(void *);
extern FILE *yyin;

Parser toplevelParser;
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
    int dumpMibFlag, dumpTypesFlag, dumpMosyFlag;
    int flags;
    
    dumpMibFlag = 0;
    dumpTypesFlag = 0;
    dumpMosyFlag = 0;

    strcpy(module, "");

    initData();

    smiSetDebugLevel(0);
    smiSetErrorLevel(3);
    flags = SMI_ERRORS | SMI_ERRORLINES;
#ifdef CONFIG_FILE
    smiReadConfig(CONFIG_FILE);
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
	    dumpMibFlag = 1;
	    dumpTypesFlag = 1;
	    break;
	case 'M':
	    dumpMosyFlag = 1;
	    break;
	default:
	    fprintf(stderr, "Usage: %s [-yYvVrRsS] [-d level] [-l level] [-c configfile]"
		    " [-m module] \n", argv[0]);
	    exit(1);
	}
    }

    smiSetFlags(flags);
    
    while (optind < argc) {
	if (strlen(module)) {
	    smiAddLocation(argv[optind]);
	    smiLoadMibModule(module);
	} else {
	    smiAddLocation(argv[optind]);
	}
	if (dumpMibFlag) {
	    dumpMibTree(rootMibNode, "");
	}
	if (dumpTypesFlag) {
	    dumpTypes();
	}
	if (dumpMosyFlag) {
	    dumpMosy(rootMibNode);
	}
	optind++;
    }
    
    exit(0);
}
