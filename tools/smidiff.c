/*
 * smidiff.c --
 *
 *      Compute and check differences between MIB modules.
 *
 * Copyright (c) 2001 T. Klie, Technical University of Braunschweig.
 * Copyright (c) 2001 F. Strauss, Technical University of Braunschweig.
 * Copyright (c) 2001 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smidiff.c,v 1.6 2001/09/20 17:23:03 tklie Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"
#include "shhopt.h"


static int errorLevel = 6;	/* smidiff/libsmi error level (inclusive) */
static int sFlag = 0;		/* show the severity for error messages */


typedef struct Error {
    int level;	/* error level - roughly the same as smilint */
    int id;	/* error id used in the error() invocation */
    char *tag;	/* tag for error identification on cmd line */
    char *fmt;	/* the complete error format string */
} Error;


#define ERR_INTERNAL			0
#define ERR_TYPE_REMOVED		1
#define ERR_TYPE_ADDED			2
#define ERR_NODE_REMOVED		3
#define ERR_NODE_ADDED			4
#define ERR_BASETYPE_CHANGED		5
#define ERR_DECL_CHANGED		6
#define ERR_STATUS_CHANGED		8
#define ERR_PREVIOUS_DEFINITION		9
#define ERR_ILLEGAL_STATUS_CHANGE	10
#define ERR_DESCR_ADDED			11
#define ERR_DESCR_REMOVED		12
#define ERR_DESCR_CHANGED		13
#define ERR_REF_ADDED			14
#define ERR_REF_REMOVED			15
#define ERR_REF_CHANGED			16
#define ERR_FORMAT_ADDED		17
#define ERR_FORMAT_REMOVED		18
#define ERR_FORMAT_CHANGED		19
#define ERR_UNITS_ADDED			20
#define ERR_UNITS_REMOVED		21
#define ERR_UNITS_CHANGED		22
#define ERR_ACCESS_ADDED		23
#define ERR_ACCESS_REMOVED		24
#define ERR_ACCESS_CHANGED		25
#define ERR_NAME_ADDED			26
#define ERR_NAME_REMOVED		27
#define ERR_NAME_CHANGED		28
#define ERR_TO_IMPLICIT			29
#define ERR_FROM_IMPLICIT		30
#define ERR_RANGE_ADDED                 31
#define ERR_RANGE_REMOVED               32
#define ERR_RANGE_CHANGED               33
#define ERR_DEFVAL_ADDED		34
#define ERR_DEFVAL_REMOVED		35
#define ERR_DEFVAL_CHANGED		36

static Error errors[] = {
    { 0, ERR_INTERNAL, "internal", 
      "internal error!!!" },
    { 1, ERR_TYPE_REMOVED, "type-removed",
      "type `%s' has been deleted" },
    { 5, ERR_TYPE_ADDED, "type-added",
      "type `%s' has been added" },
    { 1, ERR_NODE_REMOVED, "node-removed",
      "node `%s' has been deleted" },
    { 5, ERR_NODE_ADDED, "node-added",
      "node `%s' has been added" },
    { 1, ERR_BASETYPE_CHANGED, "basetype-changed",
      "base type of `%s' changed" },
    { 5, ERR_DECL_CHANGED, "decl-changed",
      "declaration changed for `%s'" },
    { 5, ERR_STATUS_CHANGED, "status-changed",
      "status changed for `%s'" },
    { 6, ERR_PREVIOUS_DEFINITION, "previous-definition",
      "previous definition of `%s'" },
    { 2, ERR_ILLEGAL_STATUS_CHANGE, "status-change-error",
      "illegal status change for `%s'" },
    { 5, ERR_DESCR_ADDED, "descr-added",
      "description added to `%s'" },
    { 2, ERR_DESCR_REMOVED, "descr-removed",
      "description removed from `%s'" },
    { 5, ERR_DESCR_CHANGED, "descr-changed",
      "description of `%s' changed" },
    { 5, ERR_REF_ADDED, "ref-added",
      "reference added to `%s'" },
    { 3, ERR_REF_REMOVED, "ref-removed",
      "reference removed from `%s'" },
    { 5, ERR_REF_CHANGED, "ref-changed",
      "reference of `%s' changed" },
    { 5, ERR_FORMAT_ADDED, "format-added",
      "format added to `%s'" },
    { 3, ERR_FORMAT_REMOVED, "format-removed",
      "format removed from `%s'" },
    { 5, ERR_FORMAT_CHANGED, "format-changed",
      "format of `%s' changed" },
    { 5, ERR_UNITS_ADDED, "units-added",
      "units added to `%s'" },
    { 3, ERR_UNITS_REMOVED, "units-removed",
      "units removed from `%s'" },
    { 5, ERR_UNITS_CHANGED, "units-changed",
      "units of `%s' changed" },
    { 5, ERR_ACCESS_ADDED, "access-added",
      "access added to `%s'" },
    { 3, ERR_ACCESS_REMOVED, "access-removed",
      "access removed from `%s'" },
    { 5, ERR_ACCESS_CHANGED, "access-changed",
      "access of `%s' changed" },
    { 5, ERR_NAME_ADDED, "name-added",
      "name added to `%s'" },
    { 3, ERR_NAME_REMOVED, "name-removed",
      "name removed from `%s'" },
    { 5, ERR_NAME_CHANGED, "name-changed",
      "name changed from `%s' to `%s'" },
    { 3, ERR_TO_IMPLICIT, "to-implicit",
      "implicit type for `%s' replaces type `%s'" },
    { 3, ERR_FROM_IMPLICIT, "from-implicit",
      "type `%s' replaces implicit type for `%s'" },
    { 3, ERR_RANGE_ADDED, "range-added",
      "range added to `%s'" },
    { 3, ERR_RANGE_REMOVED, "range-removed",
      "range removed from `%s'" },
    { 3, ERR_RANGE_CHANGED, "range-changed",
      "range of `%s' changed" },
    { 3, ERR_DEFVAL_ADDED, "defval-added",
      "default value added to `%s'" },
    { 3, ERR_DEFVAL_REMOVED, "defval-removed",
      "default value removed from `%s'" },
    { 3, ERR_DEFVAL_CHANGED, "defval-changed",
      "default value of `%s' changed" },
    { 0, 0, NULL, NULL }
};



static void
printError(SmiModule *smiModule, int id, int line, va_list ap)
{
    int i;

    /*
     * Search for the tag instead of just using the id as an index so
     * that we do not run into trouble if the id is bogus.
     */

    for (i = 0; errors[i].fmt; i++) {
	if (errors[i].id == id) break;
    }
    if (! errors[i].fmt) {
	i = 0;		/* assumes that 0 is the internal error */
    }
    
    if (errors[i].level <= errorLevel) {
	fprintf(stderr, "%s", smiModule->path);
	if (line >= 0) {
	    fprintf(stderr, ":%d", line);
	}
	fprintf(stderr, " ");
	if (sFlag) {
	    fprintf(stderr, "[%d] ", errors[i].level);
	}
	vfprintf(stderr, errors[i].fmt, ap);
	fprintf(stderr, "\n");
    }
}



static void
printErrorAtLine(SmiModule *smiModule, int id, int line, ...)
{
    va_list ap;

    va_start(ap, line);
    printError(smiModule, id, line, ap);
    va_end(ap);
}



static int
diffStrings(const char *s1, const char *s2)
{
    int i, j;

    for (i = 0, j = 0; s1[i] && s2[j]; i++, j++) {
	while (s1[i] && isspace((int) s1[i])) i++;
	while (s2[j] && isspace((int) s2[j])) j++;
	if (s1[i] != s2[j]) {
	    return 1;
	}
    }
    return (s1[i] != s2[j]);
}



static void
checkName(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *oldName, char *newName)
{
    if (!oldName && newName) {
	printErrorAtLine(newModule, ERR_NAME_ADDED,
			 newLine, newName);
    }

    if (oldName && !newName) {
	printErrorAtLine(oldModule, ERR_NAME_REMOVED,
			 oldLine, oldName);
    }
    
    if (oldName && newName && strcmp(oldName, newName) != 0) {
	printErrorAtLine(newModule, ERR_NAME_CHANGED,
			 newLine, oldName, newName);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, oldName);
    }
}



static void
checkDecl(SmiModule *oldModule, int oldLine,
	  SmiModule *newModule, int newLine,
	  char *name, SmiDecl oldDecl, SmiDecl newDecl)
{
    if (oldDecl == newDecl) {
	return;
    }
    
    printErrorAtLine(newModule, ERR_DECL_CHANGED,
		     newLine, name);
    printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
		     oldLine, name);
}



static void
checkStatus(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *name, SmiStatus oldStatus, SmiStatus newStatus)
{
    if (oldStatus == newStatus) {
	return;
    }
    
    if (! (((oldStatus == SMI_STATUS_CURRENT
	     && (newStatus == SMI_STATUS_DEPRECATED
		 || newStatus == SMI_STATUS_OBSOLETE)))
	   || ((oldStatus == SMI_STATUS_DEPRECATED
		&& newStatus == SMI_STATUS_OBSOLETE)))) {
	printErrorAtLine(newModule, ERR_ILLEGAL_STATUS_CHANGE,
			 newLine, name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, name);
	return;
    }

    printErrorAtLine(newModule, ERR_STATUS_CHANGED,
		     newLine, name);
    printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
		     oldLine, name);
}



static void
checkAccess(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *name, SmiAccess oldAccess, SmiAccess newAccess)
{
    if (oldAccess == newAccess) {
	return;
    }

    if (oldAccess == SMI_ACCESS_UNKNOWN) {
	printErrorAtLine(newModule, ERR_ACCESS_ADDED,
			 newLine, name);
    } else if (newAccess == SMI_ACCESS_UNKNOWN) {
	printErrorAtLine(newModule, ERR_ACCESS_REMOVED,
			 newLine, name);
    } else {
	printErrorAtLine(newModule, ERR_ACCESS_CHANGED,
			 newLine, name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, name);
    }
}



static void
checkDescription(SmiModule *oldModule, int oldLine,
		 SmiModule *newModule, int newLine,
		 char *name, char *oldDescr, char *newDescr)
{
    if (!oldDescr && newDescr) {
	printErrorAtLine(newModule, ERR_DESCR_ADDED,
			 newLine, name);
    }

    if (oldDescr && !newDescr) {
	printErrorAtLine(oldModule, ERR_DESCR_REMOVED,
			 oldLine, name);
    }

    if (oldDescr && newDescr && diffStrings(oldDescr, newDescr)) {
	printErrorAtLine(newModule, ERR_DESCR_CHANGED,
			 newLine, name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, name);
    }
}



static void
checkReference(SmiModule *oldModule, int oldLine,
	       SmiModule *newModule, int newLine,
	       char *name, char *oldRef, char *newRef)
{
    if (!oldRef && newRef) {
	printErrorAtLine(newModule, ERR_REF_ADDED,
			 newLine, name);
    }

    if (oldRef && !newRef) {
	printErrorAtLine(oldModule, ERR_REF_REMOVED,
			 oldLine, name);
    }
    
    if (oldRef && newRef && diffStrings(oldRef, newRef) != 0) {
	printErrorAtLine(newModule, ERR_REF_CHANGED,
			 newLine, name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, name);
    }
}



static void
checkFormat(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *name, char *oldFormat, char *newFormat)
{
    if (!oldFormat && newFormat) {
	printErrorAtLine(newModule, ERR_FORMAT_ADDED,
			 newLine, name);
    }

    if (oldFormat && !newFormat) {
	printErrorAtLine(oldModule, ERR_FORMAT_REMOVED,
			 oldLine, name);
    }
    
    /* ignore all white space changes here? */
    
    if (oldFormat && newFormat && strcmp(oldFormat, newFormat) != 0) {
	printErrorAtLine(newModule, ERR_FORMAT_CHANGED,
			 newLine, name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, name);
    }
}



static void
checkUnits(SmiModule *oldModule, int oldLine,
	   SmiModule *newModule, int newLine,
	   char *name, char *oldUnits, char *newUnits)
{
    if (!oldUnits && newUnits) {
	printErrorAtLine(newModule, ERR_UNITS_ADDED,
			 newLine, name);
    }

    if (oldUnits && !newUnits) {
	printErrorAtLine(oldModule, ERR_UNITS_REMOVED,
			 oldLine, name);
    }
    
    if (oldUnits && newUnits && strcmp(oldUnits, newUnits) != 0) {
	printErrorAtLine(newModule, ERR_UNITS_CHANGED,
			 newLine, name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 oldLine, name);
    }
}



static SmiType*
findTypeWithRange(SmiType *smiType)
{
    SmiType *iterType;

    for (iterType = smiType; iterType; iterType = smiGetParentType(iterType)) {
	if (smiGetFirstRange(iterType)) {
	    return iterType;
	}
    }
    return NULL;
}



static void
checkRanges(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *name, 
	    SmiType *oldType, SmiType *newType)
{
    SmiType *oldTwR, *newTwR; /* parent types with ranges */

    oldTwR = findTypeWithRange(oldType);
    newTwR = findTypeWithRange(newType);
    
    if (!oldTwR && newTwR) {
	printErrorAtLine(newModule, ERR_RANGE_ADDED, newLine, name);
    }
  
    if (oldTwR && !newTwR) {
	printErrorAtLine(oldModule, ERR_RANGE_REMOVED, oldLine, name);
    }

    if (oldTwR && newTwR) {
	SmiRange *oldRange, *newRange;
	oldRange = smiGetFirstRange(oldTwR);
	newRange = smiGetFirstRange(newTwR);
	
	/* xxx check ranges in detail */
    }
}



static void
checkDefVal(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *name,
	    SmiValue oldVal, SmiValue newVal)
{
    int i, changed = 0;
    
    if ((oldVal.basetype != SMI_BASETYPE_UNKNOWN) && 
	(newVal.basetype == SMI_BASETYPE_UNKNOWN)) {
	printErrorAtLine(oldModule, ERR_DEFVAL_REMOVED, oldLine, name);
	return;
    }

    if ((oldVal.basetype == SMI_BASETYPE_UNKNOWN) && 
	(newVal.basetype != SMI_BASETYPE_UNKNOWN)) {
	printErrorAtLine(oldModule, ERR_DEFVAL_ADDED, oldLine, name);
	return;
    }

    if (oldVal.basetype != newVal.basetype) {
	printErrorAtLine(newModule, ERR_DEFVAL_CHANGED, newLine,name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION, oldLine, name);
	return;
    }

    switch (oldVal.basetype) {
    case SMI_BASETYPE_INTEGER32:
	changed = (oldVal.value.integer32 != newVal.value.integer32);
	break;
    case SMI_BASETYPE_UNSIGNED32:
	changed = (oldVal.value.unsigned32 != newVal.value.unsigned32);
	break;
    case SMI_BASETYPE_INTEGER64:
	changed = (oldVal.value.integer64 != newVal.value.integer64);
	break;
    case SMI_BASETYPE_UNSIGNED64:
	changed = (oldVal.value.unsigned64 != newVal.value.unsigned64);
	break;
    case SMI_BASETYPE_FLOAT32:
	changed = (oldVal.value.float32 != newVal.value.float32);
	break;
    case SMI_BASETYPE_FLOAT64:
	changed = (oldVal.value.float64 != newVal.value.float64);
	break;
    case SMI_BASETYPE_FLOAT128:
	changed = (oldVal.value.float128 != newVal.value.float128);
	break;
    case SMI_BASETYPE_OCTETSTRING:
    case SMI_BASETYPE_BITS:
	changed = (oldVal.len != newVal.len)
	    || memcmp(oldVal.value.ptr, newVal.value.ptr, oldVal.len != 0);
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	changed = (oldVal.len != newVal.len);
	for (i = 0; !changed && i < oldVal.len; i++) {
	    changed = (oldVal.value.oid[i] != newVal.value.oid[i]);
	}
	break;
    case SMI_BASETYPE_ENUM :
	/* TBD */
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    }
	
    if (changed) {
	printErrorAtLine(newModule, ERR_DEFVAL_CHANGED, newLine,name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION, oldLine, name);
    }
}



static void
checkTypes(SmiModule *oldModule, SmiType *oldType,
	   SmiModule *newModule, SmiType *newType)
{
    checkName(oldModule, smiGetTypeLine(oldType),
	      newModule, smiGetTypeLine(newType),
	      oldType->name, newType->name);
	
    if (oldType->basetype != newType->basetype) {
	printErrorAtLine(newModule, ERR_BASETYPE_CHANGED,
			 smiGetTypeLine(newType), newType->name);
	printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			 smiGetTypeLine(oldType), oldType->name);
    }

    checkRanges(oldModule, smiGetTypeLine(oldType),
		newModule, smiGetTypeLine(newType),
		oldType->name,
		oldType,
		newType);

    checkDefVal(oldModule, smiGetTypeLine(oldType),
		newModule, smiGetTypeLine(newType),
		oldType->name, 
		oldType->value, newType->value);

    checkDecl(oldModule, smiGetTypeLine(oldType),
	      newModule, smiGetTypeLine(newType),
	      newType->name,
	      oldType->decl, newType->decl);

    checkStatus(oldModule, smiGetTypeLine(oldType),
		newModule, smiGetTypeLine(newType),
		newType->name, oldType->status, newType->status);

    checkFormat(oldModule, smiGetTypeLine(oldType),
		newModule, smiGetTypeLine(newType),
		newType->name,
		oldType->format, newType->format);

    checkUnits(oldModule, smiGetTypeLine(oldType),
	       newModule, smiGetTypeLine(newType),
	       newType->name,
	       oldType->units, newType->units);

    checkDescription(oldModule, smiGetTypeLine(oldType),
		     newModule, smiGetTypeLine(newType),
		     newType->name,
		     oldType->description, newType->description);

    checkReference(oldModule, smiGetTypeLine(oldType),
		   newModule, smiGetTypeLine(newType),
		   newType->name,
		   oldType->reference, newType->reference);
}



static void
diffTypes(SmiModule *oldModule, const char *oldTag,
	  SmiModule *newModule, const char *newTag)
{
    SmiType *oldType, *newType;

    /*
     * First check whether the old type definitions still exist and
     * whether the updates (if any) are consistent with the SMI rules.
     */
    
    smiInit(oldTag);
    for (oldType = smiGetFirstType(oldModule);
	 oldType;
	 oldType = smiGetNextType(oldType)) {
	
	smiInit(newTag);
	newType = smiGetType(newModule, oldType->name);
	if (newType) {
	    checkTypes(oldModule, oldType, newModule, newType);
	} else {
	    printErrorAtLine(oldModule, ERR_TYPE_REMOVED,
			     smiGetTypeLine(oldType), oldType->name);
	}
	smiInit(oldTag);
    }

    /*
     * Let's see if there are any new definitions.
     */

    smiInit(newTag);
    for (newType = smiGetFirstType(newModule);
	 newType;
	 newType = smiGetNextType(newType)) {
	
	smiInit(oldTag);
	oldType = smiGetType(oldModule, newType->name);
	if (! oldType) {
	    printErrorAtLine(newModule, ERR_TYPE_ADDED,
			     smiGetTypeLine(newType), newType->name);
	}
	smiInit(newTag);
    }
}



static void
checkObjects(SmiModule *oldModule, SmiNode *oldNode,
	     SmiModule *newModule, SmiNode *newNode)
{
    SmiType *oldType, *newType;

    oldType = smiGetNodeType(oldNode);
    newType = smiGetNodeType(newNode);

    checkName(oldModule, smiGetNodeLine(oldNode),
	      newModule, smiGetNodeLine(newNode),
	      oldNode->name, newNode->name);
    
    if (oldType && newType) {
	if (oldType->name && !newType->name) {
	    printErrorAtLine(newModule, ERR_TO_IMPLICIT,
			     smiGetNodeLine(newNode),
			     newNode->name, oldType->name);
	    printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			     smiGetNodeLine(oldNode), oldNode->name);
	} else if (!oldType->name && newType->name) {
	    printErrorAtLine(newModule, ERR_FROM_IMPLICIT,
			     smiGetNodeLine(newNode),
			     newType->name, oldNode->name);
	    printErrorAtLine(oldModule, ERR_PREVIOUS_DEFINITION,
			     smiGetNodeLine(oldNode), oldNode->name);
	} else {	
	    checkTypes(oldModule, oldType, newModule, newType);
	}
    }

    checkDecl(oldModule, smiGetNodeLine(oldNode),
	      newModule, smiGetNodeLine(newNode),
	      newNode->name, oldNode->decl, newNode->decl);

    checkStatus(oldModule, smiGetNodeLine(oldNode),
		newModule, smiGetNodeLine(newNode),
		newNode->name, oldNode->status, newNode->status);

    checkAccess(oldModule, smiGetNodeLine(oldNode),
		newModule, smiGetNodeLine(newNode),
		newNode->name, oldNode->access, newNode->access);

    /* check nodekind */
    /* check names */

    checkDefVal(oldModule, smiGetNodeLine(oldNode),
		newModule, smiGetNodeLine(newNode),
		newNode->name,
		oldNode->value, newNode->value);

    /* check index */

    checkDefVal(oldModule, smiGetNodeLine(oldNode),
		newModule, smiGetNodeLine(newNode),
		newNode->name,
		oldNode->value, newNode->value);

    checkFormat(oldModule, smiGetNodeLine(oldNode),
		newModule, smiGetNodeLine(newNode),
		newNode->name,
		oldNode->format, newNode->format);

    checkUnits(oldModule, smiGetNodeLine(oldNode),
	       newModule, smiGetNodeLine(newNode),
	       newNode->name,
	       oldNode->units, newNode->units);

    checkDescription(oldModule, smiGetNodeLine(oldNode),
		     newModule, smiGetNodeLine(newNode),
		     newNode->name,
		     oldNode->description, newNode->description);

    checkReference(oldModule, smiGetNodeLine(oldNode),
		   newModule, smiGetNodeLine(newNode),
		   newNode->name,
		   oldNode->reference, newNode->reference);
}



static void
diffObjects(SmiModule *oldModule, const char *oldTag,
	    SmiModule *newModule, const char *newTag)
{
    SmiNode *oldNode, *newNode;
    SmiNodekind nodekinds;

    nodekinds =  SMI_NODEKIND_NODE | SMI_NODEKIND_TABLE |
	SMI_NODEKIND_ROW | SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR;
    
    /*
     * First check whether the old node definitions still exist and
     * whether the updates (if any) are consistent with the SMI rules.
     */
    
    smiInit(oldTag);
    for(oldNode = smiGetFirstNode(oldModule, nodekinds);
	oldNode;
        oldNode = smiGetNextNode(oldNode, nodekinds)) {
	
	smiInit(newTag);
	newNode = smiGetNodeByOID(oldNode->oidlen, oldNode->oid);
	if (newNode
	    && newNode->oidlen == oldNode->oidlen
	    && smiGetNodeModule(newNode) == newModule) {
	    checkObjects(oldModule, oldNode, newModule, newNode);
	} else {
	    printErrorAtLine(oldModule, ERR_NODE_REMOVED,
			     smiGetNodeLine(oldNode), oldNode->name);
	}
	smiInit(oldTag);
    }

    /*
     * Let's see if there are any new definitions.
     */

    smiInit(newTag);
    for (newNode = smiGetFirstNode(newModule, nodekinds);
	 newNode;
	 newNode = smiGetNextNode(newNode, nodekinds)) {
	
	smiInit(oldTag);
	oldNode = smiGetNodeByOID(newNode->oidlen, newNode->oid);
	if (! oldNode
	    || newNode->oidlen != oldNode->oidlen
	    || smiGetNodeModule(oldNode) != oldModule) {
	    printErrorAtLine(newModule, ERR_NODE_ADDED,
			     smiGetNodeLine(newNode), newNode->name);
	}
	smiInit(newTag);
    }
}



static void
checkNotifications(SmiModule *oldModule, SmiNode *oldNode,
		   SmiModule *newModule, SmiNode *newNode)
{
    checkDecl(oldModule, smiGetNodeLine(oldNode),
	      newModule, smiGetNodeLine(newNode),
	      newNode->name, oldNode->decl, newNode->decl);

    checkStatus(oldModule, smiGetNodeLine(oldNode),
		newModule, smiGetNodeLine(newNode),
		newNode->name, oldNode->status, newNode->status);

    /* check names */
    /* check objects */

    checkDescription(oldModule, smiGetNodeLine(oldNode),
		     newModule, smiGetNodeLine(newNode),
		     newNode->name,
		     oldNode->description, newNode->description);

    checkReference(oldModule, smiGetNodeLine(oldNode),
		   newModule, smiGetNodeLine(newNode),
		   newNode->name,
		   oldNode->reference, newNode->reference);
}



static void
diffNotifications(SmiModule *oldModule, const char *oldTag,
		  SmiModule *newModule, const char *newTag)
{
    SmiNode *oldNode, *newNode;

    /*
     * First check whether the old node definitions still exist and
     * whether the updates (if any) are consistent with the SMI rules.
     */
    
    smiInit(oldTag);
    for(oldNode = smiGetFirstNode(oldModule, SMI_NODEKIND_NOTIFICATION);
	oldNode;
        oldNode = smiGetNextNode(oldNode, SMI_NODEKIND_NOTIFICATION)) {
	
	smiInit(newTag);
	newNode = smiGetNodeByOID(oldNode->oidlen, oldNode->oid);
	if (newNode
	    && newNode->oidlen == oldNode->oidlen
	    && smiGetNodeModule(newNode) == newModule) {
	    checkNotifications(oldModule, oldNode, newModule, newNode);
	} else {
	    printErrorAtLine(oldModule, ERR_NODE_REMOVED,
			     smiGetNodeLine(oldNode), oldNode->name);
	}
	smiInit(oldTag);
    }

    /*
     * Let's see if there are any new definitions.
     */

    smiInit(newTag);
    for (newNode = smiGetFirstNode(newModule, SMI_NODEKIND_NOTIFICATION);
	 newNode;
	 newNode = smiGetNextNode(newNode, SMI_NODEKIND_NOTIFICATION)) {
	
	smiInit(oldTag);
	oldNode = smiGetNodeByOID(newNode->oidlen, newNode->oid);
	if (! oldNode
	    || newNode->oidlen != oldNode->oidlen
	    || smiGetNodeModule(oldNode) != oldModule) {
	    printErrorAtLine(newModule, ERR_NODE_ADDED,
			     smiGetNodeLine(newNode), newNode->name);
	}
	smiInit(newTag);
    }
}



static void
usage()
{
    fprintf(stderr,
	    "Usage: smidiff [options] oldmodule newmodule\n"
	    "  -V, --version        show version and license information\n"
	    "  -h, --help           show usage information\n"
	    "  -c, --config=file    load a specific configuration file\n"
	    "  -l, --level=level    set maximum level of errors and warnings\n"
	    "  -p, --preload=module preload <module>\n");
}



static void help() { usage(); exit(0); }
static void version() { printf("smidiff " SMI_VERSION_STRING "\n"); exit(0); }
static void config(char *filename) { smiReadConfig(filename, "smidiff"); }
static void level(int lev) { errorLevel = lev; }
static void preload(char *module) { smiLoadModule(module); }



int
main(int argc, char *argv[])
{
    SmiModule *oldModule, *newModule;

    static optStruct opt[] = {
	/* short long              type        var/func       special       */
	{ 'h', "help",           OPT_FLAG,   help,          OPT_CALLFUNC },
	{ 'V', "version",        OPT_FLAG,   version,       OPT_CALLFUNC },
	{ 'c', "config",         OPT_STRING, config,        OPT_CALLFUNC },
	{ 'l', "level",          OPT_INT,    level,         OPT_CALLFUNC },
	{ 'p', "preload",        OPT_STRING, preload,       OPT_CALLFUNC },
	{ 's', "severity",       OPT_FLAG,   &sFlag,        0 },
	{ 0, 0, OPT_END, 0, 0 }  /* no more options */
    };
    
    optParseOptions(&argc, argv, opt, 0);

    if (argc != 3) {
	usage();
	return 1;
    }

    /* the `:' separates the view identifier */
    smiInit("smidiff:old");
    smiSetErrorLevel(errorLevel);
    oldModule = smiGetModule(smiLoadModule(argv[1]));
 
    smiInit("smidiff:new");
    smiSetErrorLevel(errorLevel);
    newModule = smiGetModule(smiLoadModule(argv[2]));

    /* diffModules(oldModule, "smidiff:old", newModule, "smidiff:new"); */
    diffTypes(oldModule, "smidiff:old", newModule, "smidiff:new");
    diffObjects(oldModule, "smidiff:old", newModule, "smidiff:new");
    diffNotifications(oldModule, "smidiff:old", newModule, "smidiff:new");
    /* diffGroups(oldModule, "smidiff:old", newModule, "smidiff:new") */
    /* diffCompliances(oldModule, "smidiff:old", newModule, "smidiff:new") */

    smiInit("smidiff:old");
    smiExit();

    smiInit("smidiff:new");
    smiExit();

    return 0;
}

