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
 * @(#) $Id: smilint.c,v 1.12 1999/05/31 11:58:42 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "smi.h"



void
usage()
{
    fprintf(stderr,
	    "Usage: smilint [-Vhvs] [-l <level>] <module_or_path>\n"
	    "-V                        show version and license information\n"
	    "-h                        show usage information\n"
	    "-v                        print erroneous lines of MIB modules \n"
	    "-s                        print statistics on parsed MIB modules\n"
	    "-l <level>                set maximum level of errors and warnings\n"
	    "<module_or_path>          plain name of MIB module or file path\n");
}



void
version()
{
    printf("smilint " VERSION "\n");
}



int
main(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    int flags;
    smiInit();

    flags = smiGetFlags();
    
    flags |= SMI_ERRORS;
    smiSetFlags(flags);
    
    while ((c = getopt(argc, argv, "Vhsvl:")) != -1) {
	switch (c) {
	case 'V':
	    version();
	    exit(0);
	case 'h':
	    usage();
	    exit(0);
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'v':
	    flags |= SMI_ERRORLINES;
	    smiSetFlags(flags);
	    break;
	case 's':
	    flags |= SMI_STATS;
	    smiSetFlags(flags);
	    break;
	default:
	    usage();
	    exit(1);
	}
    }

    while (optind < argc) {
	smiLoadModule(argv[optind]);
	optind++;
    }
    
    exit(0);
}
