/*
 * check.h --
 *
 *      Definitions for the semantic check functions.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: check.h,v 1.1 2000/06/18 11:23:13 strauss Exp $
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

extern void smiCheckTypeRanges(Parser *parser,
			       Type *type);

extern void smiCheckNamedNumberRedefinition(Parser *parser, Type *type);

extern void smiCheckNamedNumberSubtyping(Parser *parser, Type *type);

extern void smiCheckIndex(Parser *parser, Object *object);

extern void smiCheckAugment(Parser *parser, Object *object);

#endif /* _CHECK_H */
