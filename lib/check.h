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
 * @(#) $Id: check.h,v 1.8 2001/05/23 15:25:53 schoenw Exp $
 */

#ifndef _CHECK_H
#define _CHECK_H


#include "data.h"
#include "smi.h"


extern void smiCheckObjectName(Parser *parser, Module *module, char *name);

extern void smiCheckTypeName(Parser *parser,
			     Module *module, char *name, int line);

extern void smiCheckFormat(Parser *parser,
			   SmiBasetype basetype, char *format, int line);

extern void smiCheckTypeRanges(Parser *parser, Type *type);

extern void smiCheckTypeUsage(Parser *parserPtr, Module *modulePtr);

extern void smiCheckDefault(Parser *parser, Object *object);

extern void smiCheckComplianceStatus(Parser *parser, Object *compliance);

extern void smiCheckNamedNumberRedefinition(Parser *parser, Type *type);

extern void smiCheckNamedNumberSubtyping(Parser *parser, Type *type);

extern void smiCheckNamedNumbersOrder(Parser *parser, Type *type);

extern void smiCheckIndex(Parser *parser, Object *object);

extern void smiCheckAugment(Parser *parser, Object *object);

extern void smiCheckGroupMembership(Parser *parser, Object *object);

extern void smiCheckGroupMembers(Parser *parser, Object *group);

extern void smiCheckObjectReuse(Parser *parser,
				char *name, Object **objectPtr);

extern void smiCheckNotificationMembers(Parser *parser,
					Object *object);

extern void smiyyerror(char *msg, Parser *parserPtr);

#endif /* _CHECK_H */
