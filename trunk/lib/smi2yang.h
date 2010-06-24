/*
 * smi2yang.h --
 *
 *      Conversion of SMIv2 modules to YANG modules
 *
 * Copyright (c) 1999,2000 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Author: Juergen Schoenwaelder
 * @(#) $Id: data.c 12198 2009-01-05 08:37:07Z schoenw $
 */

#ifndef _SMI2YANG_H
#define _SMI2YANG_H

#include <smi.h>
#include <yang.h>

#ifdef __cplusplus
extern "C" {
#endif

extern YangNode *yangGetModuleFromSmiModule(SmiModule *smiModule);

#ifdef __cplusplus
}
#endif

#endif /* _SMI2YANG_H */


