/*
 * smi_svc_init.c --
 *
 *      Initialization routine for the SMI RPC server.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
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

extern int yydebug;


int
smi_svc_init(argc, argv)
    int argc;
    char *argv[];
{
    char c;
    int flags;
    
    smiInit();
    
    smiSetDebugLevel(9);
    smiSetErrorLevel(9);
    flags = SMI_ERRORS | SMI_ERRORLINES;
    smiSetFlags(flags);
    
#ifdef CONFIG_FILE
    smiReadConfig(CONFIG_FILE);
#endif
    
    while ((c = getopt(argc, argv, "rRsSvVyYd:l:c:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg);
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
	default:
	    fprintf(stderr, "Usage: %s [-yYvVrRsS] [-d level] [-l level] [-c configfile] file\n", argv[0]);
	    exit(1);
	}
    }

    smiSetFlags(flags);
    
    while (optind < argc) {
	smiAddLocation(argv[optind]);
	optind++;
    }

    return 0;
}
