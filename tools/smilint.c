/*
 * smilint.c --
 *
 *      MIB module checker main program.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @(#) $Id: smilint.c,v 1.8 1999/05/20 17:01:45 strauss Exp $
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
