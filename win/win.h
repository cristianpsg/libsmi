/*
 * win.h --
 *
 *      Some helper functions to make libsmi compile with vc++ for win32.
 *
 * Copyright (c) 2000 E. Schoenfelder, Gaertner Datensysteme Braunschweig.
 * Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: win.h,v 1.3 2000/06/21 10:33:43 strauss Exp $
 */

#ifndef _WIN_H
#define _WIN_H

/*
 * The Win32 API provides use with stricmp(), which is a string
 * lowercase compare function. This should be equivalent with
 * strcasecmp().
 */

#define strcasecmp stricmp

/*
 * The access() function exists in the Win32 API, but there are no
 * defines for the mode parameter. So we provided them here.
 */

#ifndef F_OK
#define F_OK	00
#endif
#ifndef W_OK
#define W_OK	02
#endif
#ifndef R_OK
#define R_OK	04
#endif

/*
 * The Win32 API does not have getopt (or something equivalent).
 * So we provide our own implementation with its own interface.
 */

extern int optind;
extern int opterr;
extern char *optarg;
extern int getopt(int argc, char **argv, char *opts);

/*
 * isascii() is a BSD extension not present in the Win32 API.
 */

#ifndef isascii
#define isascii(c) !((c) & ~0177))
#endif

#endif _WIN_H
