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
 * @(#) $Id: smilint.c,v 1.26 2000/04/10 14:20:27 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"



void usage()
{
    fprintf(stderr,
	    "Usage: smilint [-Vhsr] [-c <configfile>] [-p <module>] [-l <level>]\n"
	    "               [-i <error-pattern>] <module_or_path>\n"
	    "-V                    show version and license information\n"
	    "-h                    show usage information\n"
	    "-s                    print statistics on parsed MIB modules\n"
	    "-r                    print errors also for imported modules\n"
	    "-c <configfile>       load a specific configuration file\n"
	    "-p <module>           preload <module>\n"
	    "-l <level>            set maximum level of errors and warnings\n"
	    "-i <error-pattern>    ignore errors matching prefix pattern\n"
	    "<module_or_path>      plain name of MIB module or file path\n");
}



void version()
{
    printf("smilint " VERSION "\n");
}



int main(int argc, char *argv[])
{
    char c;
    int flags, i;

    for (i = 1; i < argc; i++) if (strstr(argv[i], "-c") == argv[i]) break;
    if (i == argc) 
	smiInit("smilint");
    else
	smiInit(NULL);

    flags = smiGetFlags();
    flags |= SMI_FLAG_ERRORS;
    flags |= SMI_FLAG_NODESCR;
    smiSetFlags(flags);
    
    while ((c = getopt(argc, argv, "Vhsrp:l:i:c:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg, "smiquery");
	    break;
	case 'V':
	    version();
	    return 0;
	case 'e':
	    /* smiPrintErrors(); */
	    return 0;
	case 'h':
	    usage();
	    return 0;
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

    return 0;
}
