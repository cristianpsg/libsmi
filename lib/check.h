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
 * @(#) $Id: check.h,v 1.4 2000/11/09 22:17:58 strauss Exp $
 */

#ifndef _CHECK_H
#define _CHECK_H


#include "data.h"
#include "smi.h"


extern void smiCheckObjectName(Parser *parser,
			       Module *module, char *name);

extern void smiCheckTypeName(Parser *parser,
			     Module *module, char *name, int line);

extern void smiCheckFormat(Parser *parser,
			   SmiBasetype basetype, char *format, int line);

extern void smiCheckTypeRanges(Parser *parser,
			       Type *type);

extern void smiCheckNamedNumberRedefinition(Parser *parser, Type *type);

extern void smiCheckNamedNumberSubtyping(Parser *parser, Type *type);

extern void smiCheckIndex(Parser *parser, Object *object);

extern void smiCheckAugment(Parser *parser, Object *object);

extern void smiCheckGroupMembership(Parser *parser, Object *object);

extern void smiCheckGroupMembers(Parser *parser, Object *group);

extern void smiCheckObjectReuse(Parser *parser, char *name, Object **objectPtr);

#endif /* _CHECK_H */
