/*
 * check.h --
 *
 *      Definitions for the semantic check functions.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: data.h,v 1.62 2000/06/14 13:15:16 strauss Exp $
 */

#ifndef _CHECK_H
#define _CHECK_H


#include "data.h"
#include "smi.h"


extern void smiCheckObjectName(Parser *parser,
			       Module *module, char *name);

extern void smiCheckTypeName(Parser *parser,
			     Module *module, char *name);

extern void smiCheckFormat(Parser *parser,
			   SmiBasetype basetype, char *format);

#endif /* _CHECK_H */
