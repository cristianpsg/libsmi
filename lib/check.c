/*
 * check.c --
 *
 *      This module contains semantics checks that are shared between
 *	the SMI parser backends.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: data.c,v 1.85 2000/06/14 13:57:33 strauss Exp $
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
redefinition(Parser *parser, char *name1, Module *module,
	     int line, char *name2)
{
    char *tmp = parser->path;
    int equal = (strcmp(name1, name2) == 0);

    if (! module) {
	if (equal) {
	    smiPrintError(parser, ERR_REDEFINITION, name1);
	} else {
	    smiPrintError(parser, ERR_CASE_REDEFINITION, name1, name2);
	}
    } else {
	if (equal) {
	    smiPrintError(parser, ERR_EXT_REDEFINITION,
			  name1, module->export.name);
	} else {
	    smiPrintError(parser, ERR_EXT_CASE_REDEFINITION,
			  name1, name2, module->export.name);
	}
	parser->path = module->export.path;
    }
    smiPrintErrorAtLine(parser, ERR_PREVIOUS_DEFINITION, line, name2);
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
 *      1 on success or 0 if the name already exists.
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
		redefinition(parser, name,
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
		redefinition(parser, name,
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
 *      1 on success or 0 if the type already exists.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckTypeName(Parser *parser, Module *module, char *name)
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
		redefinition(parser, name,
			     modPtr == module ? NULL : typePtr->modulePtr,
			     typePtr->line, typePtr->export.name);
	    }
	}

        for (objectPtr = modPtr->firstObjectPtr;
	     objectPtr; objectPtr = objectPtr->nextPtr) {
	    if (! (objectPtr->flags & FLAG_INCOMPLETE)
		&& ! strcasecmp(name, objectPtr->export.name)) {
		redefinition(parser, name,
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
 *      1 on success or 0 if the format is invalid.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiCheckFormat(Parser *parser, SmiBasetype basetype, char *format)
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
	smiPrintError(parser, ERR_INVALID_FORMAT, format);
    }
}
