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
 * @(#) $Id: smilint.c,v 1.10 1999/05/27 20:06:02 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
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
    printf("smilint " VERSION "\n" COPYLEFT);
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
