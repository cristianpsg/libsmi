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
 * @(#) $Id: lint.c,v 1.16 1998/10/08 14:47:52 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "defs.h"
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



void
readConfig(filename)
    const char *filename;
{
    char line[201], cmd[201], arg1[201], arg2[201];
    FILE *file;
    
    file = fopen(filename, "r");
    if (!file) {
	printError(NULL, ERR_OPENING_CONFIGFILE, filename,
		   strerror(errno));
    } else {
	while (fgets(line, sizeof(line), file)) {
	    if (feof(file)) break;
	    sscanf(line, "%s %s %s", cmd, arg1, arg2);
	    if (cmd[0] == '#') continue;
	    if (!strcmp(cmd, "directory")) {
		addDirectory(arg1);
	    } else if (!strcmp(cmd, "loglevel")) {
		errorLevel = atoi(arg1);
	    } else if (!strcmp(cmd, "debuglevel")) {
		debugLevel = atoi(arg1);
	    } else if (!strcmp(cmd, "yydebug")) {
		yydebug = atoi(arg1);
	    } else if (!strcmp(cmd, "statistics")) {
		if (atoi(arg1))
		    flags |= FLAG_STATS;
		else
		    flags &= ~FLAG_STATS;
	    } else if (!strcmp(cmd, "importlogging")) {
		if (atoi(arg1))
		    flags |= FLAG_RECURSIVE;
		else
		    flags &= ~FLAG_RECURSIVE;
	    } else if (!strcmp(cmd, "errorlines")) {
		printErrorLines = atoi(arg1);
	    } else {
		printError(NULL, ERR_UNKNOWN_CONFIG_DIRECTIVE,
			   filename, cmd);
	    }
	}
	fclose(file);
    }
}



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
    int dumpMib;
    
    yydebug = 0;
    printErrorLines = 0;
    errorLevel = 3;
    debugLevel = 0;
    dumpMib = 0;

    flags = FLAG_WHOLEFILE | FLAG_ERRORS | FLAG_WHOLEMOD;
    strcpy(module, "");

    initData();
    
#ifdef CONFIG_FILE
    readConfig(CONFIG_FILE);
#endif
    
    while ((c = getopt(argc, argv, "DrRsSvVyYd:l:c:m:")) != -1) {
	switch (c) {
	case 'c':
	    readConfig(optarg);
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
	    dumpMib = 1;
	    break;
	default:
	    fprintf(stderr, "Usage: %s [-yYvVrRsS] [-d level] [-l level] [-c configfile]"
		    " [-m module] \n", argv[0]);
	    exit(1);
	}
    }

    while (optind < argc) {
	readMibFile(argv[optind], module, flags);
	if (dumpMib) {
	    dumpMibTree(rootMibNode, "");
	}
	optind++;
    }
    
    exit(0);
}
