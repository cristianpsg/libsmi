/*
 * smilint.c --
 *
 *      MIB module checker main program.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smilint.c,v 1.21 1999/12/13 16:16:01 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "smi.h"



void usage()
{
    fprintf(stderr,
	    "Usage: smilint [-Vhvs] [-l <level>] <module_or_path>\n"
	    "-V                    show version and license information\n"
	    "-h                    show usage information\n"
	    "-s                    print statistics on parsed MIB modules\n"
	    "-r                    print errors also for imported modules\n"
	    "-i <error-pattern>    ignore errors matching prefix pattern\n"
	    "-p <module>           preload <module>\n"
	    "-l <level>            set maximum level of errors and warnings\n"
	    "<module_or_path>      plain name of MIB module or file path\n");
}



void version()
{
    printf("smilint " VERSION "\n");
}



int main(int argc, char *argv[])
{
    char c;
    int flags;

    smiInit("smilint");

    flags = smiGetFlags();
    
    flags |= SMI_FLAG_ERRORS;
    smiSetFlags(flags);
    
    while ((c = getopt(argc, argv, "Vhsrp:l:i:")) != -1) {
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
	case 'i':
	    smiSetSeverity(optarg, 9);
	    break;
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'r':
	    flags |= SMI_FLAG_RECURSIVE;
	    smiSetFlags(flags);
	    break;
	case 's':
	    flags |= SMI_FLAG_STATS;
	    smiSetFlags(flags);
	    break;
	default:
	    usage();
	    exit(1);
	}
    }

    while (optind < argc) {
	if (smiLoadModule(argv[optind]) == NULL) {
	    fprintf(stderr, "smilint: cannot locate module `%s'\n",
		    argv[optind]);
	    exit(1);
	}
	optind++;
    }

    smiExit();
    
    exit(0);
}
