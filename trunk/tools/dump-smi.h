/*
 * dump-smi.h --
 *
 *      Operations to dump SMIv1/v2 module information.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-smi.h,v 1.3 1999/05/27 20:06:00 strauss Exp $
 */

#ifndef _DUMP_SMI_H
#define _DUMP_SMI_H



extern int dumpSmiV1(const char *modulename);

extern int dumpSmiV2(const char *modulename);



#endif /* _DUMP_SMI_H */
