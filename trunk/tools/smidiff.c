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
 * @(#) $Id: smidiff.c,v 1.4 2001/09/11 08:57:44 schoenw Exp $
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
#define ERR_RANGE_CHANGED               31

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
    { 3, ERR_RANGE_CHANGED, "range-changed",
      "range of `%s' changed" },
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


#if 0
#ifdef _DEBUG
#define SMI_DIFF_DBG_MSG( a, b ) smiDiffDbgMsg( ( a ), ( b ) )
#define SMI_DIFF_DBG_ENABLE_TAG( a ) smiDiffEnableTag( a )

#define SMI_DIFF_MAXTAGS 10
static char *tags[SMI_DIFF_MAXTAGS];
static int tagsize = 0;

#define SMI_DIFF_DBG_TAG_ACTIVE   1
#define SMI_DIFF_DBG_TAG_INACTIVE 0

static int active( const char *tag )
{
    int i;
    for (i = 0; i < tagsize; i++) {
	if (!strcmp( tag, tags[i])) {
	    return SMI_DIFF_DBG_TAG_ACTIVE;
	}
    }
    return SMI_DIFF_DBG_TAG_INACTIVE;
}

static void smiDiffEnableTag( const char *tag )
{
    if (! tag) {
	return;
    }
    
    if (tagsize < SMI_DIFF_MAXTAGS) {
	tags[tagsize++] = strdup(tag);
    }
}

static void smiDiffDbgMsg( const char *tag, const char *msg )
{
    if (active( tag ) == SMI_DIFF_DBG_TAG_ACTIVE) {
	printf( "%s: %s\n", tag, msg );
    }
}
#else if /* No debugging, please */
#define SMI_DIFF_DBG_MSG
#define SMI_DIFF_DBG_ENABLE_TAG
#endif /* _DEBUG */ 


char *oidToStr( unsigned int oidlen, SmiSubid *oid )
{
  int i;
  char buf[128];
  char *ret = (char *)malloc( oidlen * 4 );
  
  sprintf( buf, "%u", oid[0] ); 
  strcat( ret, buf );
  for( i = 1; i < oidlen; i++) {
    sprintf( buf, ".%u",oid[i] );
    strcat( ret, buf );
  }
  SMI_DIFF_DBG_MSG( "ots", ret );

  return ret;
}

#ifndef MAX
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif /* ifndef MAX */

#define SMI_DIFF_ERROR       -1
#define SMI_NO_DIFF           0
#define SMI_NAME_CHANGED      1
#define SMI_OID_LEN_CHANGED   2
#define SMI_OID_CHANGED       4
#define SMI_DECL_CHANGED      8
#define SMI_ACCESS_CHANGED   16
#define SMI_STATUS_CHANGED   32
#define SMI_FORMAT_CHANGED   64
#define SMI_VALUE_CHANGED   128
#define SMI_UNITS_CHANGED   256
#define SMI_DESCR_CHANGED   512

int nodecmp( SmiNode *oldNode, SmiNode *newNode )
{
  char dbgstr[100];

  if( ! ( oldNode && newNode ) )
    return SMI_DIFF_ERROR;

  /* compare names */
  if( strcmp( oldNode->name, newNode->name ) ) {
    /* TBD: dump a message */
    sprintf( dbgstr, "names differ: expected %s and found %s",
	     oldNode->name, newNode->name );
    SMI_DIFF_DBG_MSG( "cmp", dbgstr );
    
    /* O.K., names are different. If the oid is differnt as well,
       probably we are in a complete different node. */
    if( oldNode->oidlen != newNode->oidlen || 
	memcmp( oldNode->oid, newNode->oid, MAX( oldNode->oidlen,
						 newNode->oidlen ) ) ) 
	return SMI_OID_CHANGED && SMI_NAME_CHANGED;
    
    return SMI_NAME_CHANGED;
  }
  
  /* compare oids */
  if( oldNode->oidlen != newNode->oidlen || 
	memcmp( oldNode->oid, newNode->oid, MAX( oldNode->oidlen,
						 newNode->oidlen ) ) ) 
    return SMI_OID_CHANGED;
  
  /* compare decl */
  if( oldNode->decl != newNode->decl )
    return SMI_DECL_CHANGED;

  /* compare access */
  if( oldNode->access != newNode->access )
    return SMI_ACCESS_CHANGED;

  /* compare status */
  if( oldNode->status != newNode->status ) {
    switch( oldNode->status ) {
    case SMI_STATUS_CURRENT :
      if( newNode->status == SMI_STATUS_DEPRECATED ) {
	sprintf(dbgstr,
		"Legal Change: Staus changed from current to deprecated in %s",
		oldNode->name);
	SMI_DIFF_DBG_MSG( "cmp", dbgstr );
      }
      else if( newNode->status == SMI_STATUS_OBSOLETE ) {
	sprintf( dbgstr,
		 "Legal Change: Staus changed from current to obsolete in %s",
		 oldNode->name );
	SMI_DIFF_DBG_MSG("cmp", dbgstr );
      }
      break;
    case SMI_STATUS_DEPRECATED :
      if( newNode->status == SMI_STATUS_OBSOLETE ) {
	sprintf(dbgstr,
		"Legal Change: \
Staus changed from deprecated to obsolete in %s",
		oldNode->name);
	SMI_DIFF_DBG_MSG("cmp", dbgstr );
      }
      break;
    default : return SMI_STATUS_CHANGED;
    }
  }
  
  /* TBD: compare format */
  
  /* TBD: compare value */

  /* compare units */
  if( newNode->units ) {
    if( oldNode->units ) {
      if( strcmp( oldNode->units, newNode->units ) )
	return SMI_UNITS_CHANGED;
    }
    else {
      sprintf( dbgstr, "Legal Change: unit %s added to %s", 
	       newNode->units, newNode->name );
      SMI_DIFF_DBG_MSG( "cmp", dbgstr );
    }
  }
  else
    if( oldNode->units )
      return SMI_UNITS_CHANGED;

  /* compare description */
  if( ! newNode->description ) {
    if( oldNode->description ) 
      return SMI_DESCR_CHANGED; 
  }
  else if( strcmp( oldNode->description, newNode->description ) ) {
    SMI_DIFF_DBG_MSG( "cmp", "Legal Change: description altered");
    SMI_DIFF_DBG_MSG( "cmp", "Old description:");
    SMI_DIFF_DBG_MSG( "cmp", oldNode->description );
    SMI_DIFF_DBG_MSG( "cmp", "New description:");
    SMI_DIFF_DBG_MSG( "cmp", newNode->description );
    SMI_DIFF_DBG_MSG( "cmp", "Change is legal if it is a clarification" );
  }
    
  
  /* no differences found */
  return SMI_NO_DIFF;
}
#endif


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


static void
checkRanges(SmiModule *oldModule, int oldLine,
	    SmiModule *newModule, int newLine,
	    char *name, 
	    SmiRange *oldRange, /* first SmiRange */
	    SmiRange *newRange) /* first SmiRange */ 
{
  /* check existance of ranges */
  if (!oldRange && newRange) {
    printErrorAtLine(newModule, ERR_RANGE_CHANGED, newLine, name);
  }
  
  if (oldRange && !newRange) {
    printErrorAtLine(oldModule, ERR_RANGE_CHANGED, oldLine, name);
  }
  
  /* xxx check ranges in detail */
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
		smiGetFirstRange(oldType),
		smiGetFirstRange(newType));

    /* xxx check defval */

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
    /* check defval */
    /* check index */

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
#if 0
    SmiNode *node;
#endif

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

#if 0
    /* enable some debugging tags */
    SMI_DIFF_DBG_ENABLE_TAG( "nodes" );
    SMI_DIFF_DBG_ENABLE_TAG( "cmp" ); 
    SMI_DIFF_DBG_ENABLE_TAG( "ots" );
    SMI_DIFF_DBG_ENABLE_TAG( "fmt" );
    SMI_DIFF_DBG_ENABLE_TAG( "val" );
#endif
 
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

#if 0
    /* to switch a specific view, simply call smiInit() again */
    smiInit("smidiff:old");
    for(node = smiGetFirstNode(oldModule, SMI_NODEKIND_ANY);
	node;
        node = smiGetNextNode(node, SMI_NODEKIND_ANY)) {

	SmiNode *newnode;

      char dbgstr[100] ;
      
      /* try to find node with same name in the new module */
 
      sprintf( dbgstr, "Searching for node number %d (%s)...", 
	       oldCount, node->name );
      SMI_DIFF_DBG_MSG( "nodes", dbgstr );
      if( node->format ) {
	sprintf( dbgstr, "node %s has format %s", node->name, node->format );
	SMI_DIFF_DBG_MSG( "fmt", dbgstr );
      }
      sprintf( dbgstr, "%s has value (basetype is %d)", 
	       node->name, node->value.basetype );
      SMI_DIFF_DBG_MSG( "val", dbgstr );

      /* switch to the new module */
      smiInit( "smidiff:new" );
      
      newnode = smiGetNodeByOID( node->oidlen, node->oid );

      if (! newnode) {
	  fprintf( stderr, 
		   "node %s not found in new module. Perhaps oid changed.\n",  
		   node->name );
      } else {
	  int cmpres;
	sprintf( dbgstr, "... found %s", newnode->name );
	SMI_DIFF_DBG_MSG( "nodes", dbgstr );

	/* compare nodes */
	cmpres = nodecmp( node, newnode );
	switch( cmpres ) {
	case SMI_NO_DIFF : break;
	case ( SMI_NAME_CHANGED && SMI_OID_CHANGED ) :
	    /* in this case probably a node has been moved, so
	       let us try to find a node with a same name */
	    newnode = smiGetNode( newModule, node->name );
	    if( ! newnode )
		fprintf( stderr, 
			 "ERROR: Node %s has been deleted\n", node->name );
	    else
	      
	      fprintf( stderr,
			 "ERROR: Node %s has been moved from %s to %s\n",
		       node->name, oidToStr( node->oidlen, node->oid ), 
		       oidToStr( newnode->oidlen, newnode->oid ) );
	    
	    break;
	default: fprintf( stderr,
			  "ERROR: Node %s changed\n", node->name );
	}
      }
      
      /* switch back to old module */
      smiInit( "smidiff:old" );
    };
#endif

    smiInit("smidiff:old");
    smiExit();

    smiInit("smidiff:new");
    smiExit();

    return 0;
}

