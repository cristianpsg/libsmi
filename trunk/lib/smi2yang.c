/*
 * smi2yang.c --
 *
 *      Conversion of SMIv2 modules to YANG modules
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Author: Juergen Schoenwaelder
 * @(#) $Id: smi.c 20582 2009-09-03 10:34:06Z schoenw $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smi2yang.h"
#include "yang-data.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

YangNode *yangGetModuleFromSmiModule(SmiModule *smiModule)
{
    _YangNode    *node;

    /* TODO:
     * - fix addYangNode to pick up a suitable line number
     */
    
    fprintf(stderr, "** smi2yang starts here (%s) **\n", smiModule->name);
    node = addYangNode(smiModule->name, YANG_DECL_MODULE, NULL);
    fprintf(stderr, "** smi2yang ends here (%s) **\n", smiModule->name);
    
    return &(node->export);
}
