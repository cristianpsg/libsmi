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
 * @(#) $Id: smilint.c,v 1.7 1999/04/09 18:47:08 strauss Exp $
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
    int flags;
    int config;
    
    config = 0;

    smiInit();

    flags = smiGetFlags();
    
    flags |= SMI_ERRORS;
    smiSetFlags(flags);
    
    while ((c = getopt(argc, argv, "sSvVl:c:L:")) != -1) {
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
	default:
	    fprintf(stderr, "Usage: %s [-vVsS] [-l level] [-c configfile]"
		    " [-L location] module\n", argv[0]);
	    exit(1);
	}
    }

#ifdef SMILINT_CONFIG_FILE
    if (!config)
	smiReadConfig(SMILINT_CONFIG_FILE);
#endif
        
    while (optind < argc) {
	smiLoadModule(argv[optind]);
	optind++;
    }
    
    exit(0);
}
