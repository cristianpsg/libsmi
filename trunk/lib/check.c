/*
 * check.c --
 *
 *      This module contains semantics checks that are shared between
 *	the SMI parser backends.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: check.c,v 1.17 2001/03/06 11:42:09 strauss Exp $
 */

#include <config.h>

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "error.h"
#include "util.h"
#include "data.h"
#include "check.h"
#include "smi.h"



/*
 *----------------------------------------------------------------------
 *
 * compareValues --
 *
 *	Compare two SmiValues a and b.
 *
 * Results:
 *	-1,0,1 if a is less than, equal, or greater than b.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
compareValues(SmiValue *a, SmiValue *b) {
    if (((a->basetype == SMI_BASETYPE_UNSIGNED32) &&
	 (b->basetype == SMI_BASETYPE_INTEGER32)) ||
	((a->basetype == SMI_BASETYPE_UNSIGNED32) &&
	 (a->value.unsigned32 > b->value.unsigned32)) ||
	((a->basetype == SMI_BASETYPE_INTEGER32) &&
	 (a->value.integer32 > b->value.integer32))) {
	return 1;
    } else if ((a->basetype == b->basetype) &&
	       (((a->basetype == SMI_BASETYPE_UNSIGNED32) &&
		 (a->value.unsigned32 == b->value.unsigned32)) ||
		((a->basetype == SMI_BASETYPE_INTEGER32) &&
		 (a->value.integer32 == b->value.integer32)))) {
	return 0;
    } else {
	return -1;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * redefinition --
 *
 *	Print out error messages about a (case) redefinition.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
redefinition(Parser *parser, int line1, char *name1, Module *module,
	     int line2, char *name2)
{
    char *tmp = parser->path;
    int equal = (strcmp(name1, name2) == 0);

    if (! module) {
	if (equal) {
	    if (line1) {
		smiPrintErrorAtLine(parser, ERR_REDEFINITION, line1, name1);
	    } else {
		smiPrintError(parser, ERR_REDEFINITION, name1);
	    }
	} else {
	    if (line1) {
		smiPrintErrorAtLine(parser, ERR_CASE_REDEFINITION,
						line1, name1, name2);
	    } else {
		smiPrintError(parser, ERR_CASE_REDEFINITION, name1, name2);
	    }
	}
    } else {
	if (equal) {
	    if (line1) {
		smiPrintErrorAtLine(parser, ERR_EXT_REDEFINITION, line1,
			      module->export.name, name1);
	    } else {
		smiPrintError(parser, ERR_EXT_REDEFINITION,
			      module->export.name, name1);
	    }
	} else {
	    if (line1) {
		smiPrintErrorAtLine(parser, ERR_EXT_CASE_REDEFINITION, line1,
			      name1, module->export.name, name2);
	    } else {
		smiPrintError(parser, ERR_EXT_CASE_REDEFINITION,
			      name1, module->export.name, name2);
	    }
	}
	parser->path = module->export.path;
    }
    smiPrintErrorAtLine(parser, ERR_PREVIOUS_DEFINITION, line2, name2);
    if (module) {
	parser->path = tmp;
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckObjectName --
 *
 *      Check whether a given object name already exists
 *	in a given module.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckObjectName(Parser *parser, Module *module, char *name)
{
    Object	*objectPtr;
    Type        *typePtr;
    Module	*modPtr;

    int errRedef = smiGetErrorSeverity(ERR_REDEFINITION);
    int errExtRedef = smiGetErrorSeverity(ERR_EXT_REDEFINITION);
    int errCaseRedef = smiGetErrorSeverity(ERR_CASE_REDEFINITION);
    int errExtCaseRedef = smiGetErrorSeverity(ERR_EXT_CASE_REDEFINITION);

    if (! (parser->flags & SMI_FLAG_ERRORS)
	|| (errRedef > smiErrorLevel
	    && errExtRedef > smiErrorLevel
	    && errCaseRedef > smiErrorLevel
	    && errExtCaseRedef > smiErrorLevel)) {
	return;
    }

    /*
     * This would really benefit from having a hash table...
     */

    for (modPtr = firstModulePtr;
	 modPtr; modPtr = modPtr->nextPtr) {

	/*
	 * Skip all external modules if we are not interested in
	 * generating warning on extern redefinitions.
	 */

	if (errExtRedef > smiErrorLevel
	    && errExtCaseRedef > smiErrorLevel
	    && modPtr != module) {
	    continue;
	}

        for (objectPtr = modPtr->firstObjectPtr;
	     objectPtr; objectPtr = objectPtr->nextPtr) {
	    if (! (objectPtr->flags & FLAG_INCOMPLETE)
		&& ! strcasecmp(name, objectPtr->export.name)) {
		redefinition(parser, 0, name,
			     modPtr == module ? NULL : objectPtr->modulePtr,
			     objectPtr->line, objectPtr->export.name);
	    }
	}
	for (typePtr = modPtr->firstTypePtr;
	     typePtr; typePtr = typePtr->nextPtr) {
	    /* TODO: must ignore SEQUENCE types here ... */
	    if (! (typePtr->flags & FLAG_INCOMPLETE)
		&& typePtr->export.name
		&& !strcasecmp(name, typePtr->export.name)) {
		redefinition(parser, 0, name,
			     modPtr == module ? NULL : typePtr->modulePtr,
			     typePtr->line, typePtr->export.name);
	    }
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckTypeName --
 *
 *      Check whether a given type name already exists
 *	in a given module.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckTypeName(Parser *parser, Module *module, char *name, int line)
{
    Object	*objectPtr;
    Type        *typePtr;
    Module	*modPtr;

    int errRedef = smiGetErrorSeverity(ERR_REDEFINITION);
    int errExtRedef = smiGetErrorSeverity(ERR_EXT_REDEFINITION);
    int errCaseRedef = smiGetErrorSeverity(ERR_CASE_REDEFINITION);
    int errExtCaseRedef = smiGetErrorSeverity(ERR_EXT_CASE_REDEFINITION);

    if (! (parser->flags & SMI_FLAG_ERRORS)
	|| (errRedef > smiErrorLevel
	    && errExtRedef > smiErrorLevel
	    && errCaseRedef > smiErrorLevel
	    && errExtCaseRedef > smiErrorLevel)) {
	return;
    }

    /*
     * This would really benefit from having a hash table...
     */

    for (modPtr = firstModulePtr;
	 modPtr; modPtr = modPtr->nextPtr) {

	/*
	 * Skip all external modules if we are not interested in
	 * generating warning on extern redefinitions.
	 */

	if (errExtRedef > smiErrorLevel
	    && errExtCaseRedef > smiErrorLevel
	    && modPtr != module) {
	    continue;
	}

	for (typePtr = modPtr->firstTypePtr;
	     typePtr; typePtr = typePtr->nextPtr) {
	    /* TODO: must ignore SEQUENCE types here ... */
	    if (! (typePtr->flags & FLAG_INCOMPLETE)
		&& typePtr->export.name
		&& !strcasecmp(name, typePtr->export.name)) {
		redefinition(parser, line, name,
			     modPtr == module ? NULL : typePtr->modulePtr,
			     typePtr->line, typePtr->export.name);
	    }
	}

        for (objectPtr = modPtr->firstObjectPtr;
	     objectPtr; objectPtr = objectPtr->nextPtr) {
	    if (! (objectPtr->flags & FLAG_INCOMPLETE)
		&& ! strcasecmp(name, objectPtr->export.name)) {
		redefinition(parser, line, name,
			     modPtr == module ? NULL : objectPtr->modulePtr,
			     objectPtr->line, objectPtr->export.name);
	    }
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckFormat --
 *
 *      Check whether a format specification is valid.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckFormat(Parser *parser, SmiBasetype basetype, char *format, int line)
{
    int n, repeat, error = 1;
    char *p = format;

    switch (basetype) {
    case SMI_BASETYPE_INTEGER32:
    case SMI_BASETYPE_INTEGER64:
    case SMI_BASETYPE_UNSIGNED32:
    case SMI_BASETYPE_UNSIGNED64:
	if (*p == 'x' || *p == 'o' || *p == 'b') {
	    p++;
	    error = (*p != 0);
	} else if (*p == 'd') {
	    p++;
	    if (! *p) {
		error = 0;
		break;
	    }
	    if (*p != '-') {
		error = 0;
		break;
	    }
	    for (n = 0, p++; *p && isdigit((int) *p); p++, n++) ;
	    error = (*p != 0 || n <= 0);
	}
	break;
    case SMI_BASETYPE_OCTETSTRING:
	while (*p) {
	    if ((repeat = (*p == '*'))) p++;                /* part 1 */
	    
	    for (n = 0; *p && isdigit((int) *p); p++, n++) ;/* part 2 */
	    if (! *p || n == 0) {
		break;
	    }
	    
	    if (*p != 'x' && *p != 'd' && *p != 'o'         /* part 3 */
		&& *p != 'a' && *p != 't') {
		break;
	    }
	    p++;
	    
	    if (*p                                          /* part 4 */
		&& ! isdigit((int) *p) && *p != '*') p++;
	    
	    if (repeat && *p                                /* part 5 */
		&& ! isdigit((int) *p) && *p != '*') p++;
	}
	error = *p;
	break;
    default:
	break;
    }

    if (error) {
	if (line) {
	    smiPrintErrorAtLine(parser, ERR_INVALID_FORMAT, line, format);
	} else {
	    smiPrintError(parser, ERR_INVALID_FORMAT, format);
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckNamedNumberRedefinition --
 *
 *      Check whether named numbers redefine names or numbers.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckNamedNumberRedefinition(Parser *parser, Type *type)
{
    List *list1Ptr, *list2Ptr;
    NamedNumber *nn1Ptr, *nn2Ptr;

    if (! type || (type->export.basetype != SMI_BASETYPE_ENUM
		      && type->export.basetype != SMI_BASETYPE_BITS)) {
	return;
    }
	    
    for (list1Ptr = type->listPtr;
	 list1Ptr; list1Ptr = list1Ptr->nextPtr) {
	
	nn1Ptr = (NamedNumber *)(list1Ptr->ptr);

	for (list2Ptr = list1Ptr->nextPtr;
	     list2Ptr; list2Ptr = list2Ptr->nextPtr) {
	    
	    nn2Ptr = (NamedNumber *)(list2Ptr->ptr);

	    if (type->export.basetype == SMI_BASETYPE_ENUM) {
		if (!strcmp(nn1Ptr->export.name, nn2Ptr->export.name)) {
		    smiPrintErrorAtLine(parser, ERR_ENUM_NAME_REDEFINITION,
					type->line,
					nn1Ptr->export.name);
		}
		if (nn1Ptr->export.value.value.integer32
		    == nn2Ptr->export.value.value.integer32) {
		    smiPrintErrorAtLine(parser, ERR_ENUM_NUMBER_REDEFINITION,
					type->line,
					nn1Ptr->export.value.value.integer32);
		}
	    }
	    if (type->export.basetype == SMI_BASETYPE_BITS) {
		if (!strcmp(nn1Ptr->export.name, nn2Ptr->export.name)) {
		    smiPrintErrorAtLine(parser, ERR_BITS_NAME_REDEFINITION,
					type->line,
					nn1Ptr->export.name);
		}
		if (nn1Ptr->export.value.value.unsigned32
		    == nn2Ptr->export.value.value.unsigned32) {
		    smiPrintErrorAtLine(parser, ERR_BITS_NUMBER_REDEFINITION,
					type->line,
					nn1Ptr->export.value.value.unsigned32);
		}
	    }
	}
    }   
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckNamedNumberSubtyping --
 *
 *      Check whether named numbers in a derived type are compatible
 *	with the named numbers in the parent type.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckNamedNumberSubtyping(Parser *parser, Type *type)
{
    List *list1Ptr, *list2Ptr;
    NamedNumber *nn1Ptr, *nn2Ptr;
	
    if (! type || ! type->parentPtr || ! type->parentPtr->parentPtr
	|| (type->export.basetype != SMI_BASETYPE_ENUM
	    && type->export.basetype != SMI_BASETYPE_BITS)) {
	return;
    }
    
    for (list1Ptr = type->listPtr;
	 list1Ptr; list1Ptr = list1Ptr->nextPtr) {
	
	nn1Ptr = (NamedNumber *)(list1Ptr->ptr);
	
	for (list2Ptr = type->parentPtr->listPtr;
	     list2Ptr; list2Ptr = list2Ptr->nextPtr) {
	    
	    nn2Ptr = (NamedNumber *)(list2Ptr->ptr);
	    
	    if (type->export.basetype == SMI_BASETYPE_ENUM) {
		if (! strcmp(nn1Ptr->export.name, nn2Ptr->export.name)
		    && nn1Ptr->export.value.value.integer32
		    == nn2Ptr->export.value.value.integer32) {
		    break;
		}
	    }
	    
	    if (type->export.basetype == SMI_BASETYPE_BITS) {
		if (! strcmp(nn1Ptr->export.name, nn2Ptr->export.name)
		    && nn1Ptr->export.value.value.unsigned32
		    == nn2Ptr->export.value.value.unsigned32) {
		    break;
		}
	    }
	}
	    
	if (! list2Ptr) {
	    if (type->export.basetype == SMI_BASETYPE_ENUM) {
		smiPrintErrorAtLine(parser, ERR_ENUM_SUBTYPE,
				    type->line,
				    nn1Ptr->export.name,
				    nn1Ptr->export.value.value.integer32,
				    type->parentPtr->export.name);
	    }
	    if (type->export.basetype == SMI_BASETYPE_BITS) {
		smiPrintErrorAtLine(parser, ERR_BITS_SUBTYPE,
				    type->line,
				    nn1Ptr->export.name,
				    type->parentPtr->export.name);
	    }
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckIndex --
 *
 *      Check whether an index conforms to the SMI restrictions.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckIndex(Parser *parser, Object *object)
{
    List *listPtr, *list2Ptr;
    Object *indexPtr;
    Type *typePtr, *rTypePtr;
    Range *rangePtr;
    NamedNumber *nnPtr;
    int minSize, maxSize, len = 0;

    for (listPtr = object->listPtr; listPtr; listPtr = listPtr->nextPtr) {
	
	indexPtr = (Object *) listPtr->ptr;
	typePtr = indexPtr->typePtr;

	/* checkObjects() already handles unknown objects */
	if (indexPtr->export.nodekind != SMI_NODEKIND_COLUMN &&
	    ((indexPtr->flags & FLAG_INCOMPLETE) == 0)) {
	    smiPrintErrorAtLine(parser, ERR_INDEX_NOT_COLUMN,
				indexPtr->line,
				indexPtr->export.name,
				object->export.name);
	}

	if (!typePtr)
	    continue;
	
	switch (typePtr->export.basetype) {
	case SMI_BASETYPE_INTEGER32:
	    for (rTypePtr = typePtr; rTypePtr && ! rTypePtr->listPtr;
 		 rTypePtr = rTypePtr->parentPtr) {
	    }
	    if (! rTypePtr) {
		if (object->modulePtr != indexPtr->modulePtr) {
		    smiPrintErrorAtLine(parser, ERR_INDEX_NO_RANGE_MOD,
					object->line,
					indexPtr->modulePtr->export.name,
					indexPtr->export.name,
					object->export.name);

		} else {
		    smiPrintErrorAtLine(parser, ERR_INDEX_NO_RANGE,
					indexPtr->line,
					indexPtr->export.name,
					object->export.name);
		}
	    } else {
		for (list2Ptr = rTypePtr->listPtr;
		     list2Ptr; list2Ptr = list2Ptr->nextPtr) {
		    rangePtr = (Range *) list2Ptr->ptr;
		    if (rangePtr->export.maxValue.value.integer32 < 0) {
			smiPrintErrorAtLine(parser, ERR_INDEX_RANGE_NEGATIVE,
					    indexPtr->line,
					    indexPtr->export.name,
					    object->export.name);
			break;
		    }
		}
	    }
	    len++;
	    break;
	case SMI_BASETYPE_OCTETSTRING:
	    /* TODO: We need to check ranges of parent types as well
	       if this type does not have a range restriction. */
	    for (rTypePtr = typePtr; rTypePtr && ! rTypePtr->listPtr;
		 rTypePtr = rTypePtr->parentPtr) {
	    }
	    minSize = 65535;
	    maxSize = -1;
	    if (! rTypePtr) {
		if (object->modulePtr != indexPtr->modulePtr) {
		    smiPrintErrorAtLine(parser, ERR_INDEX_NO_SIZE_MOD,
					object->line,
					indexPtr->modulePtr->export.name,
					indexPtr->export.name,
					object->export.name);
		} else {
		    smiPrintErrorAtLine(parser, ERR_INDEX_NO_SIZE,
					indexPtr->line,
					indexPtr->export.name,
					object->export.name);
		}
	    } else {
	        for (list2Ptr = rTypePtr->listPtr;
		     list2Ptr; list2Ptr = list2Ptr->nextPtr) {
		    rangePtr = (Range *) list2Ptr->ptr;
		    if (rangePtr->export.minValue.value.integer32 < minSize) {
			minSize = rangePtr->export.minValue.value.integer32;
		    }
		    if (rangePtr->export.maxValue.value.integer32 > maxSize) {
			maxSize = rangePtr->export.maxValue.value.integer32;
		    }
		}
		if (minSize == 65535) {
		    minSize = 0;
		}
		if (maxSize < 0) {
		    maxSize = 65535;
		}
	    }
	    len += maxSize;
	    if (!indexPtr->export.implied && minSize != maxSize) {
		len++;
	    }
	    break;
	case SMI_BASETYPE_OBJECTIDENTIFIER:
	    len += 128;
	    if (!indexPtr->export.implied) {
		len++;
	    }
	    break;
	case SMI_BASETYPE_UNSIGNED32:
	    len++;
	    break;
	case SMI_BASETYPE_INTEGER64:
	case SMI_BASETYPE_UNSIGNED64:
	case SMI_BASETYPE_FLOAT32:
	case SMI_BASETYPE_FLOAT64:
	case SMI_BASETYPE_FLOAT128:
	case SMI_BASETYPE_UNKNOWN:
	    smiPrintErrorAtLine(parser, ERR_INDEX_BASETYPE, object->line,
				typePtr->export.name ? typePtr->export.name
				                     : "[unknown]",
				indexPtr->export.name, object->export.name);
	    break;
	case SMI_BASETYPE_BITS:
	    /* TODO: BITS are somehow treated as octet strings - but
	       what is the max len? */
	    break;
	case SMI_BASETYPE_ENUM:
	    for (list2Ptr = typePtr->listPtr;
		 list2Ptr; list2Ptr = list2Ptr->nextPtr) {
		
		nnPtr = (NamedNumber *)(list2Ptr->ptr);

		if (nnPtr->export.value.value.integer32 < 0) {
			smiPrintErrorAtLine(parser, ERR_INDEX_ENUM_NEGATIVE,
					    indexPtr->line,
					    indexPtr->export.name,
					    object->export.name);
			break;
		}
	    }
	    len++;
	    break;
	}

	/*
	 * TODO: If SMIv2 or SMIng and if there is a non-index column,
	 * then warn about not not-accessible index components.
	 */
#if 0	
	if (indexPtr->export.access != SMI_ACCESS_NOT_ACCESSIBLE) {
	    fprintf(stderr, "** %s should be not-accessible??\n",
		    indexPtr->export.name);
	}
#endif
    }

    if (object->export.oidlen + 1 + len > 128) {
	smiPrintErrorAtLine(parser, ERR_INDEX_TOO_LARGE, object->line,
			    object->export.name);
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckAugment --
 *
 *      Check whether a table augmentation conforms to the SMI
 *	restrictions.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckAugment(Parser *parser, Object *object)
{
    if (! object->relatedPtr) {
	return;
    }

    if (object->relatedPtr->export.nodekind != SMI_NODEKIND_ROW) {
	smiPrintErrorAtLine(parser, ERR_AUGMENT_NO_ROW, object->line,
			    object->export.name,
			    object->relatedPtr->export.name);
	return;
    }
    
    if (object->relatedPtr->export.indexkind != SMI_INDEX_INDEX) {
	smiPrintErrorAtLine(parser, ERR_AUGMENT_NESTED, object->line,
			    object->export.name,
			    object->relatedPtr->export.name);
	return;
    }
    
    /*
     * TODO: Check the size of the instance identifier and the OID
     * for this entry node.
     */

#if 0
    fprintf(stderr, "** %s augments -> %s\n", object->export.name,
	    object->relatedPtr->export.name);
#endif
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckTypeRanges --
 *
 *      Check whether all ranges of a given type are valid.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckTypeRanges(Parser *parser, Type *type)
{
    List *p, *nextPtr, *pp, *nextPP;
    
    for (p = type->listPtr; p; p = nextPtr) {

	nextPtr = p->nextPtr;
	
	((Range *)p->ptr)->typePtr = type;

	if (type->export.basetype == SMI_BASETYPE_INTEGER32) {
	    if ((((Range *)p->ptr)->export.minValue.basetype ==
		 SMI_BASETYPE_UNSIGNED32) &&
		(((Range *)p->ptr)->export.minValue.value.unsigned32 >
		    2147483647)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    if ((((Range *)p->ptr)->export.maxValue.basetype ==
		 SMI_BASETYPE_UNSIGNED32) &&
		(((Range *)p->ptr)->export.maxValue.value.unsigned32 >
		    2147483647)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    ((Range *)p->ptr)->export.minValue.basetype =
		SMI_BASETYPE_INTEGER32;
	    ((Range *)p->ptr)->export.maxValue.basetype =
		SMI_BASETYPE_INTEGER32;
	}

	if (type->export.basetype == SMI_BASETYPE_UNSIGNED32) {
	    if ((((Range *)p->ptr)->export.minValue.basetype ==
		 SMI_BASETYPE_INTEGER32) &&
		(((Range *)p->ptr)->export.minValue.value.integer32 < 0)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    if ((((Range *)p->ptr)->export.maxValue.basetype ==
		 SMI_BASETYPE_INTEGER32) &&
		(((Range *)p->ptr)->export.maxValue.value.integer32 < 0)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    ((Range *)p->ptr)->export.minValue.basetype =
		SMI_BASETYPE_UNSIGNED32;
	    ((Range *)p->ptr)->export.maxValue.basetype =
		SMI_BASETYPE_UNSIGNED32;
	}

	if (type->export.basetype == SMI_BASETYPE_OCTETSTRING) {
	    if ((((Range *)p->ptr)->export.minValue.basetype ==
		 SMI_BASETYPE_INTEGER32) &&
		(((Range *)p->ptr)->export.minValue.value.integer32 < 0)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    if ((((Range *)p->ptr)->export.maxValue.basetype ==
		 SMI_BASETYPE_INTEGER32) &&
		(((Range *)p->ptr)->export.maxValue.value.integer32 < 0)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    if ((((Range *)p->ptr)->export.minValue.basetype ==
		 SMI_BASETYPE_UNSIGNED32) &&
		(((Range *)p->ptr)->export.minValue.value.unsigned32 > 65535)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    if ((((Range *)p->ptr)->export.maxValue.basetype ==
		 SMI_BASETYPE_UNSIGNED32) &&
		(((Range *)p->ptr)->export.maxValue.value.unsigned32 > 65535)) {
		smiPrintError(parser, ERR_RANGE_OUT_OF_BASETYPE);
	    }
	    ((Range *)p->ptr)->export.minValue.basetype =
		SMI_BASETYPE_UNSIGNED32;
	    ((Range *)p->ptr)->export.maxValue.basetype =
		SMI_BASETYPE_UNSIGNED32;
	}

	if (compareValues(&((Range *)p->ptr)->export.minValue,
			  &((Range *)p->ptr)->export.maxValue) > 0) {
	    SmiValue v;
	    v = ((Range *)p->ptr)->export.minValue;
	    ((Range *)p->ptr)->export.minValue = ((Range *)p->ptr)->export.maxValue;
	    ((Range *)p->ptr)->export.maxValue = v;
	    smiPrintError(parser, ERR_EXCHANGED_RANGE_LIMITS);
	}

	/* sort */
	p->nextPtr = NULL;
	if (p != type->listPtr) {
	    if (compareValues(&((Range *)p->ptr)->export.minValue,
	      	       &((Range *)type->listPtr->ptr)->export.minValue) <= 0) {
		if (compareValues(&((Range *)p->ptr)->export.maxValue,
	      	       &((Range *)type->listPtr->ptr)->export.minValue) >= 0) {
		    smiPrintError(parser, ERR_RANGE_OVERLAP);
		}
		smiPrintError(parser, ERR_RANGES_NOT_ASCENDING);
		p->nextPtr = type->listPtr;
		type->listPtr = p;
	    } else {
		for (pp = type->listPtr; pp; pp = nextPP) {
		    nextPP = pp->nextPtr;
		    if ((!nextPP) ||
			(compareValues(&((Range *)p->ptr)->export.minValue,
			     &((Range *)nextPP->ptr)->export.minValue) <= 0)) {
			if (((nextPP) &&
			     (compareValues(&((Range *)p->ptr)->export.maxValue,
					    &((Range *)nextPP->ptr)->export.minValue) >= 0)) ||
			    (compareValues(&((Range *)p->ptr)->export.minValue,
					   &((Range *)pp->ptr)->export.maxValue) <= 0)) {
			    smiPrintError(parser, ERR_RANGE_OVERLAP);
			}
			p->nextPtr = pp->nextPtr;
			pp->nextPtr = p;
			if (p->nextPtr) {
			    smiPrintError(parser, ERR_RANGES_NOT_ASCENDING);
			    pp->nextPtr = NULL;
			} 
			break;
		    }
		}
	    }
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckDefault --
 *
 *      Check whether the default value (if present) matches the
 *	underlying type and restrictions.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckDefault(Parser *parser, Object *object)
{
    List *p, *nextPtr;
    
    if (object->export.value.basetype != SMI_BASETYPE_UNKNOWN) {

	/*
	 * If defval type and object type don't match, check whether
	 * the defval value is in the allowed range of the object's basetype.
	 */
	if ((object->typePtr->export.basetype == SMI_BASETYPE_INTEGER32) ||
	    (object->typePtr->export.basetype == SMI_BASETYPE_ENUM)) {
	    if (((object->export.value.basetype ==
		  SMI_BASETYPE_INTEGER64) &&
		 ((object->export.value.value.integer64 < (SmiInteger64)(-2147483647 - 1)) ||
		  (object->export.value.value.integer64 > (SmiInteger64)2147483647))) ||
		((object->export.value.basetype ==
		  SMI_BASETYPE_UNSIGNED32) &&
		 ((object->export.value.value.unsigned32 > 2147483647))) ||
		((object->export.value.basetype ==
		  SMI_BASETYPE_UNSIGNED64) &&
		 ((object->export.value.value.unsigned32 > 2147483647)))) {
		smiPrintErrorAtLine(parser, ERR_DEFVAL_OUT_OF_BASETYPE,
				    object->line);
	    }
	}
	if (object->typePtr->export.basetype == SMI_BASETYPE_UNSIGNED32) {
	    if (((object->export.value.basetype ==
		  SMI_BASETYPE_INTEGER64) &&
		 ((object->export.value.value.integer64 < 0) ||
		  (object->export.value.value.integer64 > (SmiInteger64)4294967295UL))) ||
		((object->export.value.basetype ==
		  SMI_BASETYPE_INTEGER32) &&
		 ((object->export.value.value.integer32 < 0))) ||
		((object->export.value.basetype ==
		  SMI_BASETYPE_UNSIGNED64) &&
		 ((object->export.value.value.unsigned32 > (SmiUnsigned32)4294967295UL)))) {
		smiPrintErrorAtLine(parser, ERR_DEFVAL_OUT_OF_BASETYPE,
				    object->line);
	    }
	}

	/*
	 * "cast" the defval to the object's basetype.
	 */
	object->export.value.basetype = object->typePtr->export.basetype;

	/*
	 * check whether the defval matches the object's range restriction.
	 */
	if ((object->export.value.basetype == SMI_BASETYPE_UNSIGNED32) ||
	    (object->export.value.basetype == SMI_BASETYPE_UNSIGNED64) ||
	    (object->export.value.basetype == SMI_BASETYPE_INTEGER32) ||
	    (object->export.value.basetype == SMI_BASETYPE_INTEGER64)) {
	    for (p = object->typePtr->listPtr; p; p = nextPtr) {
		nextPtr = p->nextPtr;
		if ((compareValues(&((Range *)p->ptr)->export.minValue,
				   &object->export.value) <= 0) &&
		    (compareValues(&((Range *)p->ptr)->export.maxValue,
				   &object->export.value) >= 0)) {
		    break;
		}
	    }
	    if ((p == NULL) && object->typePtr->listPtr) {
		smiPrintErrorAtLine(parser, ERR_DEFVAL_OUT_OF_RANGE,
				    object->line);
	    }
	}

	/*
	 * check whether the defval matches the object's enumeration.
	 */
	if (object->export.value.basetype == SMI_BASETYPE_ENUM) {
	    for (p = object->typePtr->listPtr; p; p = nextPtr) {
		nextPtr = p->nextPtr;

		if (((NamedNumber *)(p->ptr))->export.value.value.integer32 ==
		    object->export.value.value.integer32) {
			break;
		}
	    }
	    if (p == NULL) {
		smiPrintErrorAtLine(parser, ERR_DEFVAL_OUT_OF_ENUM,
				    object->line);
	    }
	    
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiCheckTypeUsage --
 *
 *      Check whether the types of all objects are used appropriately.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckTypeUsage(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr;
    Module *tcModulePtr = NULL;
    Module *inetModulePtr = NULL;
    Type *rowStatusPtr = NULL;
    Type *storageTypePtr = NULL;
    Type *taddressPtr = NULL;
    Type *tdomainPtr = NULL;
    Type *inetAddressTypePtr = NULL;
    Type *inetAddressPtr = NULL;
    Type *inetAddressIPv4Ptr = NULL;
    Type *inetAddressIPv6Ptr = NULL;
    Type *inetAddressDNSPtr = NULL;
    NamedNumber *nnPtr;
    Node *nodePtr;
    
    tcModulePtr = findModuleByName("SNMPv2-TC");
    if (tcModulePtr) {
	rowStatusPtr = findTypeByModuleAndName(tcModulePtr, "RowStatus");
	storageTypePtr = findTypeByModuleAndName(tcModulePtr, "StorageType");
	taddressPtr = findTypeByModuleAndName(tcModulePtr, "TAddress");
	tdomainPtr = findTypeByModuleAndName(tcModulePtr, "TDomain");
    }
    inetModulePtr = findModuleByName("INET-ADDRESS-MIB");
    if (inetModulePtr) {
	inetAddressTypePtr = findTypeByModuleAndName(inetModulePtr,
						     "InetAddressType");
	inetAddressPtr = findTypeByModuleAndName(inetModulePtr,
						 "InetAddress");
	inetAddressIPv4Ptr = findTypeByModuleAndName(inetModulePtr,
						     "InetAddressIPv4");
	inetAddressIPv6Ptr = findTypeByModuleAndName(inetModulePtr,
						     "InetAddressIPv6");
	inetAddressDNSPtr = findTypeByModuleAndName(inetModulePtr,
						    "InetAddressDNS");
    }
    
    if (!tcModulePtr && !inetModulePtr) return;
    
    for (objectPtr = modulePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextPtr) {

	if (objectPtr->typePtr && objectPtr->typePtr) {

	    if (tcModulePtr) {

		/* check RowStatus DEFVAL */
		if (objectPtr->typePtr == rowStatusPtr) {
		    if ((objectPtr->export.value.value.integer32 >= 4) &&
			(objectPtr->export.value.value.integer32 <= 6)) {
			nnPtr = findTypeNamedNumber(rowStatusPtr,
				     objectPtr->export.value.value.integer32);
			smiPrintErrorAtLine(parserPtr,
					    ERR_ILLEGAL_ROWSTATUS_DEFAULT,
					    objectPtr->line,
					    nnPtr->export.name);
		    }
		}
		
		/* check StorageType DEFVAL */
		if (objectPtr->typePtr == storageTypePtr) {
		    if ((objectPtr->export.value.value.integer32 >= 4) &&
			(objectPtr->export.value.value.integer32 <= 5)) {
			nnPtr = findTypeNamedNumber(storageTypePtr,
				     objectPtr->export.value.value.integer32);
			smiPrintErrorAtLine(parserPtr,
					    ERR_ILLEGAL_STORAGETYPE_DEFAULT,
					    objectPtr->line,
					    nnPtr->export.name);
		    }
		}

		/* check TDomain/TAddress pair */
		if (objectPtr->typePtr == taddressPtr) {
		    nodePtr =
			findNodeByParentAndSubid(objectPtr->nodePtr->parentPtr,
						 objectPtr->nodePtr->subid-1);
		    if (!nodePtr ||
			nodePtr->lastObjectPtr->typePtr != tdomainPtr) {
			smiPrintErrorAtLine(parserPtr,
					    ERR_TADDRESS_WITHOUT_TDOMAIN,
					    objectPtr->line);
		    }
		}
		
	    }
	    if (inetModulePtr) {

		/* check InetAddressType/InetAddress pair */
		if (objectPtr->typePtr == inetAddressPtr) {
		    nodePtr =
			findNodeByParentAndSubid(objectPtr->nodePtr->parentPtr,
						 objectPtr->nodePtr->subid-1);
		    if (!nodePtr || nodePtr->lastObjectPtr->typePtr !=
			inetAddressTypePtr) {
			smiPrintErrorAtLine(parserPtr,
					    ERR_INETADDRESS_WITHOUT_TYPE,
					    objectPtr->line);
		    }
		}

		/* check InetAddressType subtyping */
		if (objectPtr->typePtr->parentPtr == inetAddressTypePtr) {
		    smiPrintErrorAtLine(parserPtr,
					ERR_INETADDRESSTYPE_SUBTYPED,
					objectPtr->line);
		}

		/* check InetAddressXXX usage  */
		if (objectPtr->typePtr == inetAddressIPv4Ptr ||
		    objectPtr->typePtr == inetAddressIPv6Ptr ||
		    objectPtr->typePtr == inetAddressDNSPtr) {
		    smiPrintErrorAtLine(parserPtr,
					ERR_INETADDRESS_SPECIFIC,
					objectPtr->line,
					objectPtr->typePtr->export.name);
		}

	    }
	    
	}
    }
}



static char *status[] = { "Unknown", "current", "deprecated",
			  "mandatory", "optional", "obsolete" };

/*
 *----------------------------------------------------------------------
 *
 * smiCheckComplianceStatus --
 *
 *      Make sure that all groups and objects in a compliance statement
 *      are at least as current as the compliance itself.
 *      XXX I'm not sure I traversed the whole compliance statement,
 *          this at least covers the common case
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckComplianceStatus(Parser *parser, Object *compliance)
{
    List *listPtr;
    Object *memberPtr;

    for (listPtr = compliance->listPtr;
	 listPtr; listPtr = listPtr->nextPtr) {
	
	memberPtr = (Object *) listPtr->ptr;
	if (!memberPtr)
	    continue;
	addObjectFlags(memberPtr, FLAG_INCOMPLIANCE);
	if (memberPtr->export.status > compliance->export.status) {
	    smiPrintErrorAtLine(parser, ERR_COMPLIANCE_GROUP_STATUS,
				compliance->line,
				status[compliance->export.status],
				compliance->export.name,
				status[memberPtr->export.status],
				memberPtr->export.name);
	}
    }
    for (listPtr = compliance->optionlistPtr;
	 listPtr; listPtr = listPtr->nextPtr) {
	
	memberPtr = ((Option *) listPtr->ptr)->objectPtr;
	addObjectFlags(memberPtr, FLAG_INCOMPLIANCE);
	if (memberPtr->export.status > compliance->export.status) {
	    smiPrintErrorAtLine(parser, ERR_COMPLIANCE_GROUP_STATUS,
				compliance->line,
				status[compliance->export.status],
				compliance->export.name,
				status[memberPtr->export.status],
				memberPtr->export.name);
	}
    }
    for (listPtr = compliance->refinementlistPtr;
	 listPtr; listPtr = listPtr->nextPtr) {
	
	memberPtr = ((Refinement *) listPtr->ptr)->objectPtr;
	addObjectFlags(memberPtr, FLAG_INCOMPLIANCE);
	if (memberPtr->export.status > compliance->export.status) {
	    smiPrintErrorAtLine(parser, ERR_COMPLIANCE_OBJECT_STATUS,
				compliance->line,
				status[compliance->export.status],
				compliance->export.name,
				status[memberPtr->export.status],
				memberPtr->export.name);
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * smiCheckGroupMembers --
 *
 *      Check whether only scalar and column nodes and notifications
 *	are contained in a conformance group.
 *
 *      Also ensure that group members are at least as current
 *      as the group itself.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckGroupMembers(Parser *parser, Object *group)
{
    List *listPtr;
    Object *memberPtr;
    int scalarsOrColumns = 0;
    int notifications = 0;
    
    for (listPtr = group->listPtr;
	 listPtr; listPtr = listPtr->nextPtr) {
	
	memberPtr = (Object *) listPtr->ptr;

	if (((memberPtr->export.nodekind == SMI_NODEKIND_COLUMN
	      || memberPtr->export.nodekind == SMI_NODEKIND_SCALAR)
	     && memberPtr->export.access != SMI_ACCESS_NOT_ACCESSIBLE)
	    || memberPtr->export.nodekind == SMI_NODEKIND_NOTIFICATION) {
	    if (memberPtr->export.nodekind == SMI_NODEKIND_NOTIFICATION) {
		notifications++;
	    } else {
		scalarsOrColumns++;
	    }
	    addObjectFlags(memberPtr, FLAG_INGROUP);
	} else if (!(memberPtr->flags & FLAG_INCOMPLETE)) {
	    /* unknown OIDs are already flagged */
	    smiPrintErrorAtLine(parser, ERR_INVALID_GROUP_MEMBER,
				group->line,
				memberPtr->export.name,
				group->export.name);
	}
	if (memberPtr->export.status > group->export.status) {
	    smiPrintErrorAtLine(parser, ERR_GROUP_OBJECT_STATUS,
				group->line,
				status[group->export.status],
				group->export.name,
				status[memberPtr->export.status],
				memberPtr->export.name);
	}
    }

    if (scalarsOrColumns && notifications) {
	smiPrintErrorAtLine(parser, ERR_MIXED_GROUP_MEMBERS,
			    group->line,
			    group->export.name);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * smiCheckGroupMembership --
 *
 *      Check whether scalar and column nodes and notifications are
 *	contained in at least one conformance group.
 *
 *	This function assumes that smiCheckGroupMembers() has been
 *	called on all group objects and smiCheckComplianceStatus()
 *      has been called on all compliance objects before.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckGroupMembership(Parser *parser, Object *objectPtr)
{
    int found;
    
    if (((objectPtr->export.nodekind == SMI_NODEKIND_COLUMN
	  || objectPtr->export.nodekind == SMI_NODEKIND_SCALAR)
	 && objectPtr->export.access != SMI_ACCESS_NOT_ACCESSIBLE)
	|| objectPtr->export.nodekind == SMI_NODEKIND_NOTIFICATION) {

	found = (objectPtr->flags & FLAG_INGROUP);
	    
	if (! found) {
	    if (objectPtr->export.nodekind == SMI_NODEKIND_NOTIFICATION) {
		smiPrintErrorAtLine(parser, ERR_NOTIFICATION_NOT_IN_GROUP,
				    objectPtr->line,
				    objectPtr->export.name);
	    } else {
		smiPrintErrorAtLine(parser, ERR_NODE_NOT_IN_GROUP,
				    objectPtr->line,
				    objectPtr->export.name);
	    }
	}
    }
    if (objectPtr->export.nodekind == SMI_NODEKIND_GROUP) {

	found = (objectPtr->flags & FLAG_INCOMPLIANCE);

	if (!found) {
	    smiPrintErrorAtLine(parser, ERR_GROUP_OPTIONAL,
				objectPtr->line,
				objectPtr->export.name);
	}
    }

}

/*
 *----------------------------------------------------------------------
 *
 * smiCheckObjectReuse --
 *
 *      Check whether a newly defined Object represents a duplicate
 *      or a reused OID.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Allocates a new Object and adjusts the objectPtr parameter.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckObjectReuse(Parser *parser, char *name, Object **objectPtr)
{
    if ((((*objectPtr)->flags & FLAG_INCOMPLETE) == 0) &&
	strcmp(name, (*objectPtr)->export.name)) {
	if ((*objectPtr)->export.decl >= SMI_DECL_OBJECTTYPE) {
	    smiPrintError(parser, ERR_OID_REGISTERED, name,
			  (*objectPtr)->export.name);
	} else {
	    smiPrintError(parser, ERR_OID_REUSE, name,
			  (*objectPtr)->export.name);
	}
	smiPrintErrorAtLine(parser, ERR_PREVIOUS_DEFINITION,
			    (*objectPtr)->line, (*objectPtr)->export.name);
	*objectPtr = duplicateObject(*objectPtr, 0, parser);
    }
    
    if ((*objectPtr)->modulePtr != parser->modulePtr) {
	*objectPtr = duplicateObject(*objectPtr, 0, parser);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * smiyyerror --
 *
 *      Prints an error message from the parser.  In SMIv1 and v2,
 *      a common error is to terminate a comment early, so if the
 *	current line contains a comment (parserPtr->lcline) print
 *	the ERR_COMMENT_TERMINATES.
 *
 *----------------------------------------------------------------------
 */
void smiyyerror(char *msg, Parser *parserPtr)
{
	if (parserPtr->line == parserPtr->lcline &&
	    parserPtr->modulePtr &&
	    (parserPtr->modulePtr->export.language == SMI_LANGUAGE_SMIV1 ||
	     parserPtr->modulePtr->export.language == SMI_LANGUAGE_SMIV2))
		smiPrintError(parserPtr, ERR_COMMENT_TERMINATES);
	smiPrintError(parserPtr, ERR_OTHER_ERROR, msg);
}
