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
 * @(#) $Id: smilint.c,v 1.5 1999/03/16 17:24:14 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"

#define SMILINT_CONFIG_FILE "/usr/local/etc/smilint.conf"



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

#ifdef SMILINT_CONFIG_FILE
    smiReadConfig(SMILINT_CONFIG_FILE);
#endif
        
    flags = smiGetFlags();

    flags |= SMI_ERRORS;
    smiSetFlags(flags);
    
    while ((c = getopt(argc, argv, "sSvVd:l:c:L:")) != -1) {
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
	default:
	    fprintf(stderr, "Usage: %s [-vVsS] [-d level] [-l level] [-c configfile]"
		    " [-L location] module\n", argv[0]);
	    exit(1);
	}
    }

    while (optind < argc) {
	smiLoadModule(argv[optind]);
	optind++;
    }
    
    exit(0);
}
