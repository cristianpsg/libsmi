/*
 * yang-check.h --
 *
 *      Definitions for the YANG (semantic) check functions.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Authors: Kaloyan Kanev, Siarhei Kuryla
 * @(#) $Id: check.h 10751 2008-11-06 22:05:48Z schoenw $
 */

#ifndef _YANG_CHECK_H
#define _YANG_CHECK_H

#include "error.h"

#include "yang-data.h"
#include "yang.h"


time_t checkDate(Parser *parserPtr, char *date);

void validateInclude(_YangNode *module, _YangNode *extModule);

void semanticAnalysis(_YangNode *module);

int isWSP(char ch);
#endif /* _YANG_CHECK_H */