/*
 * config.c --
 *
 *      libsmi configuration.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: config.c,v 1.1 1998/11/04 02:14:51 strauss Exp $
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


int
readConfig(filename, flags)
    const char *filename;
    int *flags;
{
    char line[201], cmd[201], arg1[201], arg2[201];
    FILE *file;
    
    file = fopen(filename, "r");
    if (!file) {
	printError(NULL, ERR_OPENING_CONFIGFILE, filename,
		   strerror(errno));
	return -1;
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
		    *flags |= FLAG_STATS;
		else
		    *flags &= ~FLAG_STATS;
	    } else if (!strcmp(cmd, "importlogging")) {
		if (atoi(arg1))
		    *flags |= FLAG_RECURSIVE;
		else
		    *flags &= ~FLAG_RECURSIVE;
	    } else if (!strcmp(cmd, "errorlines")) {
		if (atoi(arg1))
		    *flags |= FLAG_ERRORLINES;
		else
		    *flags &= ~FLAG_ERRORLINES;
	    } else {
		printError(NULL, ERR_UNKNOWN_CONFIG_DIRECTIVE,
			   filename, cmd);
	    }
	}
	fclose(file);
    }

    return 0;
}

