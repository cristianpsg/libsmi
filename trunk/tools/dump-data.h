/*
 * dump-data.h --
 *
 *      Operations to dump libsmi internal data structures.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-data.h,v 1.4 1999/05/31 11:58:40 strauss Exp $
 */

#ifndef _DUMP_DATA_H
#define _DUMP_DATA_H



extern void dumpMibTree(const char *modulename);

extern void dumpTypes(const char *modulename);



#endif /* _DUMP_DATA_H */
