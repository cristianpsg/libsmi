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
 * @(#) $Id: smi_svc_init.c,v 1.3 1998/11/25 14:36:34 strauss Exp $
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
    
    smiInit();
    
    smiSetDebugLevel(0);
    smiSetErrorLevel(3);
    smiSetFlags(SMI_ERRORS | SMI_ERRORLINES);
    
#ifdef SMID_CONFIG_FILE
    smiReadConfig(SMID_CONFIG_FILE);
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
	    smiSetFlags(smiGetFlags() | SMI_ERRORLINES);
	    break;
	case 'V':
	    smiSetFlags(smiGetFlags() & ~SMI_ERRORLINES);
	    break;
	case 'r':
	    /* errors and statistics (if -s present) for imported modules */
	    smiSetFlags(smiGetFlags() | SMI_RECURSIVE);
	    break;
	case 'R':
	    smiSetFlags(smiGetFlags() & ~SMI_RECURSIVE);
	    break;
	case 's':
	    /* print some module statistics */
	    smiSetFlags(smiGetFlags() | SMI_STATS);
	    break;
	case 'S':
	    smiSetFlags(smiGetFlags() & ~SMI_STATS);
	    break;
	default:
	    fprintf(stderr, "Usage: %s [-yYvVrRsS] [-d level] [-l level] [-c configfile] file\n", argv[0]);
	    exit(1);
	}
    }

    while (optind < argc) {
	smiAddLocation(argv[optind]);
	optind++;
    }

    return 0;
}
