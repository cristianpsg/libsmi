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
 * @(#) $Id$
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

#define F_OK	00
#define W_OK	02
#define R_OK	04

/*
 * The Win32 API does not have getopt (or something equivalent).
 * So we provide our own implementation with its own interface.
 */

extern int optind;
extern int opterr;
extern char *optarg;
extern int getopt(int argc, char **argv, char *opts);

#endif _WIN_H
