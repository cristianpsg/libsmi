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
 * @(#) $Id: lint.c,v 1.3 1998/10/29 13:59:24 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "defs.h"
#include "config.h"
#include "error.h"
#include "scanner.h"
#include "parser-bison.h"
#include "data.h"

extern int yydebug;
extern int yyparse(void *);
extern FILE *yyin;



Parser toplevelParser;
int flags;
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
    
    yydebug = 0;
    printErrorLines = 0;
    errorLevel = 3;
    debugLevel = 0;
    dumpMibFlag = 0;
    dumpTypesFlag = 0;
    dumpMosyFlag = 0;

    flags = FLAG_WHOLEFILE | FLAG_ERRORS | FLAG_WHOLEMOD;
    strcpy(module, "");

    initData();
    
#ifdef CONFIG_FILE
    readConfig(CONFIG_FILE, &flags);
#endif
    
    while ((c = getopt(argc, argv, "MDrRsSvVyYd:l:c:m:")) != -1) {
	switch (c) {
	case 'c':
	    readConfig(optarg, &flags);
	    break;
	case 'm':
	    strncpy(module, optarg,
		    sizeof(module)-1);
	    flags &= ~FLAG_WHOLEFILE;
	    break;
	case 'y':
	    yydebug = 1;
	    break;
	case 'Y':
	    yydebug = 0;
	    break;
	case 'l':
	    errorLevel = atoi(optarg);
	    break;
	case 'd':
	    debugLevel = atoi(optarg);
	    break;
	case 'v':
	    printErrorLines = 1;
	    break;
	case 'V':
	    printErrorLines = 0;
	    break;
	case 'r':
	    /* errors and statistics (if -s present) for imported modules */
	    flags |= FLAG_RECURSIVE;
	    break;
	case 'R':
	    flags &= ~FLAG_RECURSIVE;
	    break;
	case 's':
	    /* print some module statistics */
	    flags |= FLAG_STATS;
	    break;
	case 'S':
	    flags &= ~FLAG_STATS;
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

    while (optind < argc) {
	readMibFile(argv[optind], module, flags);
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
