/*
 * smidump.h --
 *
 *      This header contains the entry points into the modules
 *	which dump MIB modules in various output format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#ifndef _SMIDUMP_H
#define _SMIDUMP_H


extern void *xmalloc(size_t size);
extern char *xstrdup(const char *s);
extern void xfree(void *ptr);

extern int dumpSmiV1(char *modulename);
extern int dumpSmiV2(char *modulename);
extern int dumpSming(char *modulename);

extern int dumpImports(char *modulename);
extern int dumpTypes(char *modulename);
extern int dumpTree(char *modulename);

extern int dumpMosy(char *modulename);

extern int dumpCorbaIdl(char *modulename);
extern int dumpCorbaOid(char *modulename);

extern int dumpUcdH(char *modulename);
extern int dumpUcdC(char *modulename);

extern int dumpJava(char *modulename);
extern int dumpJdmk(char *modulename);

extern int dumpFig(char *modulename);



#endif /* _SMIDUMP_H */

