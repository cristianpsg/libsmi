/*
 * parser-smi.y --
 *
 *      Syntax rules for parsing the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: parser-smi.y,v 1.135 2000/12/05 08:23:31 strauss Exp $
 */

%{

#include <config.h>
    
#ifdef BACKEND_SMI

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>

#if defined(_MSC_VER)
#include <malloc.h>
#endif

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "smi.h"
#include "error.h"
#include "parser-smi.h"
#include "scanner-smi.h"
#include "data.h"
#include "check.h"
#include "util.h"
    


/*
 * These arguments are passed to yyparse() and yylex().
 */
#define YYPARSE_PARAM parserPtr
#define YYLEX_PARAM   parserPtr

    
    
#define thisParserPtr      ((Parser *)parserPtr)
#define thisModulePtr     (((Parser *)parserPtr)->modulePtr)


    
/*
 * NOTE: The argument lvalp is not really a void pointer. Unfortunately,
 * we don't know it better at this point. bison generated C code declares
 * YYSTYPE just a few lines below based on the `%union' declaration.
 */
extern int yylex(void *lvalp, Parser *parserPtr);



static Node	   *parentNodePtr;
static int	   impliedFlag;
static SmiBasetype defaultBasetype;
static Module      *complianceModulePtr = NULL;
static Module      *capabilitiesModulePtr = NULL;
static SmiNodekind variationkind;
static int         firstStatementLine = 0;
 
#define MAX_UNSIGNED32		4294967295
#define MIN_UNSIGNED32		0
#define MAX_INTEGER32		2147483647
#define MIN_INTEGER32		-2147483648

#define SMI_EPOCH	631152000	/* 01 Jan 1990 00:00:00 */ 

 

static void
checkNameLen(Parser *parser, char *name, int error_32, int error_64)
{
    int len = strlen(name);
    
    if (len > 64) {
	smiPrintError(parser, error_64, name);
    } else if (len > 32) {
	smiPrintError(parser, error_32, name);
    }
}


 
static void
checkModuleIdentity(Parser *parserPtr, Module *modulePtr)
{
    if ((modulePtr->export.language == SMI_LANGUAGE_SMIV2)
	&& (modulePtr->numModuleIdentities < 1)
	&& strcmp(modulePtr->export.name, "SNMPv2-SMI")
	&& strcmp(modulePtr->export.name, "SNMPv2-CONF")
	&& strcmp(modulePtr->export.name, "SNMPv2-TC")) {
	smiPrintError(parserPtr, ERR_NO_MODULE_IDENTITY);
    }
}



static void
checkObjects(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr;
    Node *nodePtr;
    int i;
    
    for (objectPtr = modulePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextPtr) {

	Object *parentPtr;
	
	if (objectPtr->nodePtr->parentPtr &&
	    objectPtr->nodePtr->parentPtr->lastObjectPtr) {
	    parentPtr = objectPtr->nodePtr->parentPtr->lastObjectPtr;
	} else {
	    parentPtr = NULL;
	}

	/*
	 * Set nodekinds of all newly defined objects.
	 */
	
	if (objectPtr->export.decl == SMI_DECL_MODULEIDENTITY) {
	    objectPtr->export.nodekind = SMI_NODEKIND_NODE;
	} else if ((objectPtr->export.decl == SMI_DECL_VALUEASSIGNMENT) ||
		   (objectPtr->export.decl == SMI_DECL_OBJECTIDENTITY)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_NODE;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
		   (objectPtr->typePtr) &&
		   (objectPtr->typePtr->export.decl == SMI_DECL_IMPL_SEQUENCEOF)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_TABLE;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
		   (objectPtr->export.indexkind != SMI_INDEX_UNKNOWN)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_ROW;
	} else if ((objectPtr->export.decl == SMI_DECL_NOTIFICATIONTYPE) ||
		   (objectPtr->export.decl == SMI_DECL_TRAPTYPE)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_NOTIFICATION;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTGROUP) ||
		   (objectPtr->export.decl == SMI_DECL_NOTIFICATIONGROUP)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_GROUP;
	} else if (objectPtr->export.decl == SMI_DECL_MODULECOMPLIANCE) {
	    objectPtr->export.nodekind = SMI_NODEKIND_COMPLIANCE;
	} else if (objectPtr->export.decl == SMI_DECL_AGENTCAPABILITIES) {
	    objectPtr->export.nodekind = SMI_NODEKIND_CAPABILITIES;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
		   (parentPtr) &&
		   (parentPtr->export.indexkind != SMI_INDEX_UNKNOWN)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_COLUMN;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
		   (parentPtr) &&
		   (parentPtr->export.indexkind == SMI_INDEX_UNKNOWN)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_SCALAR;
	}
    }

    for (objectPtr = modulePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextPtr) {

	/*
	 * Check whether the associated type resolves to a known base type.
	 */
	
	if (objectPtr->typePtr
	    && (objectPtr->export.nodekind == SMI_NODEKIND_COLUMN
		|| objectPtr->export.nodekind == SMI_NODEKIND_SCALAR)
	    && objectPtr->typePtr->export.basetype == SMI_BASETYPE_UNKNOWN) {
	    smiPrintErrorAtLine(parserPtr, ERR_BASETYPE_UNKNOWN,
				objectPtr->line,
				objectPtr->typePtr->export.name ?
				objectPtr->typePtr->export.name : "[unknown]",
				objectPtr->export.name);
	    if (objectPtr->nodePtr->parentPtr->firstObjectPtr->export.nodekind
		== SMI_NODEKIND_TABLE) {
		/* the parent node is a table node, so assume this is
		 *  a row node. this adjusts missing INDEXs in RFC 1158.
		 */
		objectPtr->export.nodekind = SMI_NODEKIND_ROW;
	    }
	}

	/*
	 * Check whether groups only contain scalars, columns and
	 * notifications.
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_GROUP) {
	    smiCheckGroupMembers(parserPtr, objectPtr);
	}

	/*
	 * Check whether tables and rows are not accessible
	 * (RFC 2578 7.1.12).
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_TABLE
	    && objectPtr->export.access != SMI_ACCESS_NOT_ACCESSIBLE) {
	    smiPrintErrorAtLine(parserPtr, ERR_TABLE_ACCESS,
				objectPtr->line, objectPtr->export.name);
	}
	
	if (objectPtr->export.nodekind == SMI_NODEKIND_ROW
	    && objectPtr->export.access != SMI_ACCESS_NOT_ACCESSIBLE) {
	    smiPrintErrorAtLine(parserPtr, ERR_ROW_ACCESS,
				objectPtr->line, objectPtr->export.name);
	}
	
	/*
	 * Check whether a row's subid is 1, see RFC 2578 7.10 (1).
	 */
	
	if (objectPtr->export.nodekind == SMI_NODEKIND_ROW) {
	    int len;
	    
	    if (objectPtr->nodePtr->subid != 1) {
		smiPrintErrorAtLine(parserPtr, ERR_ROW_SUBID_ONE,
				    objectPtr->line, objectPtr->export.name);
	    }

	    len = strlen(objectPtr->export.name);
	    if (len < 6 || strcmp(objectPtr->export.name+len-5, "Entry")) {
		smiPrintErrorAtLine(parserPtr, ERR_ROWNAME_ENTRY,
				    objectPtr->line, objectPtr->export.name);
	    }
	}

	if (objectPtr->export.nodekind == SMI_NODEKIND_TABLE) {
	    int len;

	    len = strlen(objectPtr->export.name);
	    if (len < 6 || strcmp(objectPtr->export.name+len-5, "Table")) {
		smiPrintErrorAtLine(parserPtr, ERR_TABLENAME_TABLE,
				    objectPtr->line, objectPtr->export.name);
	    }
	}

        /*
	 * Complain about empty description clauses.
	 */

	if (! parserPtr->flags & SMI_FLAG_NODESCR
	    && objectPtr->export.nodekind != SMI_NODEKIND_UNKNOWN
	    && objectPtr->export.nodekind != SMI_NODEKIND_NODE
	    && (! objectPtr->export.description
		|| ! objectPtr->export.description[0])) {
	    smiPrintErrorAtLine(parserPtr, ERR_EMPTY_DESCRIPTION,
				objectPtr->line, objectPtr->export.name);
	}
	
	/*
	 * TODO: check whether the row is the only node below the
         * table node
	 */

	/*
	 * Check references to unknown identifiers.
	 */

	if (objectPtr->flags & FLAG_INCOMPLETE) {
	    if (objectPtr->export.name) {
		smiPrintErrorAtLine(parserPtr, ERR_UNKNOWN_OIDLABEL,
				    objectPtr->line, objectPtr->export.name);
	    } else {
		smiPrintErrorAtLine(parserPtr, ERR_IMPLICIT_NODE,
				    objectPtr->line);
	    }
	}

	/*
	 * Adjust the status of implicit type definitions.
	 */
	
	if (objectPtr->typePtr
	    && (objectPtr->typePtr->export.decl == SMI_DECL_IMPLICIT_TYPE)
	    && (objectPtr->typePtr->export.status == SMI_STATUS_UNKNOWN)) {
	    objectPtr->typePtr->export.status = objectPtr->export.status;
	}

	/*
	 * Link implicit type definition from refinements into
	 * the type derivation tree. Adjust the status of implicit
	 * type definitions in refinements. 
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_COMPLIANCE) {

	    List *listPtr;

	    for (listPtr = objectPtr->refinementlistPtr;
		 listPtr;
		 listPtr = listPtr->nextPtr) {

		Refinement *refinementPtr;
		Type *typePtr;
		
		refinementPtr = ((Refinement *)(listPtr->ptr));
		typePtr = refinementPtr->typePtr;
		if (typePtr) {
		    if (typePtr->export.status == SMI_STATUS_UNKNOWN) {
			typePtr->export.status = objectPtr->export.status;
		    }
		}

		typePtr = refinementPtr->writetypePtr;
		if (typePtr) {
		    if (typePtr->export.status == SMI_STATUS_UNKNOWN) {
			typePtr->export.status = objectPtr->export.status;
		    }
		}
		
	    }
	    /* relocate the refinement type into the type tree */
	    /* relocate the write refinement type into the type tree */
	}

	/*
	 * Set the oidlen/oid values that are not yet correct.
	 */

	if (objectPtr->export.oidlen == 0) {
	    if (objectPtr->nodePtr->oidlen == 0) {
		for (nodePtr = objectPtr->nodePtr, i = 1;
		     nodePtr->parentPtr != pendingNodePtr &&
			 nodePtr->parentPtr != rootNodePtr &&
			 nodePtr != nodePtr->parentPtr &&
			 i <= 128;
		     nodePtr = nodePtr->parentPtr, i++);
		if ((objectPtr->export.name) &&
		    ((i > 128) || (nodePtr == nodePtr->parentPtr))) {
		    smiPrintErrorAtLine(parserPtr, ERR_OID_RECURSIVE,
					objectPtr->line,
					objectPtr->export.name);
		}
		objectPtr->nodePtr->oid = smiMalloc(i * sizeof(SmiSubid));
		objectPtr->nodePtr->oidlen = i;
		for (nodePtr = objectPtr->nodePtr; i > 0; i--) {
		    objectPtr->nodePtr->oid[i-1] = nodePtr->subid;
		    nodePtr = nodePtr->parentPtr;
		}
	    }
	    objectPtr->export.oidlen = objectPtr->nodePtr->oidlen;
	    objectPtr->export.oid = objectPtr->nodePtr->oid;
	}

	/*
	 * Check table linkage constraints for row objects.
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_ROW) {
	    switch (objectPtr->export.indexkind) {
	    case SMI_INDEX_INDEX:
		smiCheckIndex(parserPtr, objectPtr);
		break;
	    case SMI_INDEX_AUGMENT:
		smiCheckAugment(parserPtr, objectPtr);
		break;
	    default:
		break;
	    }
	}
	
	/*
	 * Determine the longest common OID prefix of all nodes.
	 */

	if (!modulePtr->prefixNodePtr) {
	    modulePtr->prefixNodePtr = objectPtr->nodePtr;
	} else {
	    for (i = 0; i < modulePtr->prefixNodePtr->oidlen; i++) {
		if (modulePtr->prefixNodePtr->oid[i] !=
		    objectPtr->nodePtr->oid[i]) {
		    modulePtr->prefixNodePtr =
			findNodeByOid(i, modulePtr->prefixNodePtr->oid);
		    break;
		}
	    }
	}
    }

    if (modulePtr->export.language == SMI_LANGUAGE_SMIV2) {
	for (objectPtr = modulePtr->firstObjectPtr;
	     objectPtr; objectPtr = objectPtr->nextPtr) {
	    
	    /*
	     * Check whether all objects and notifications are contained in at
	     * least one conformance group (RFC 2580 3.3 and 4.1).
	     */
	    
	    smiCheckGroupMembership(parserPtr, objectPtr);
	}
    }
}



static void
checkTypes(Parser *parserPtr, Module *modulePtr)
{
    Type *typePtr;
    
    for(typePtr = modulePtr->firstTypePtr;
	typePtr; typePtr = typePtr->nextPtr) {

	/*
	 * Check references to unknown types.
	 */
	
	if ((typePtr->flags & FLAG_INCOMPLETE)
	    && typePtr->export.name
	    && (typePtr->export.decl == SMI_DECL_UNKNOWN)) {
	    smiPrintErrorAtLine(parserPtr, ERR_UNKNOWN_TYPE,
				typePtr->line, typePtr->export.name);
	}

	/*
	 * Use TCs instead of type assignments in SMIv2.
	 */

	if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2
	    && typePtr->export.decl == SMI_DECL_TYPEASSIGNMENT
	    && typePtr->export.basetype != SMI_BASETYPE_UNKNOWN
	    && strcmp(thisModulePtr->export.name, "SNMPv2-SMI")) {
	    smiPrintErrorAtLine(parserPtr, ERR_SMIV2_TYPE_ASSIGNEMENT,
				typePtr->line, typePtr->export.name);
	}

	/*
	 * Complain about TCs derived from other TCs (RFC 2579 3.5).
	 */
	
	if (typePtr->parentPtr
	    && typePtr->export.decl == SMI_DECL_TEXTUALCONVENTION
	    && typePtr->export.decl == typePtr->parentPtr->export.decl) {
	    smiPrintErrorAtLine(parserPtr, ERR_SMIV2_NESTED_TEXTUAL_CONVENTION,
				typePtr->line,
				typePtr->export.name,
				typePtr->parentPtr->export.name);
	}

	/*
	 * Complain about empty description clauses.
	 */

	if (! parserPtr->flags & SMI_FLAG_NODESCR
	    && (! typePtr->export.description
		|| ! typePtr->export.description[0])) {
	    smiPrintErrorAtLine(parserPtr, ERR_EMPTY_DESCRIPTION,
				typePtr->line, typePtr->export.name);
	}

	smiCheckNamedNumberRedefinition(parserPtr, typePtr);
	smiCheckNamedNumberSubtyping(parserPtr, typePtr);
    }
}


				
static void
checkDefvals(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr, *object2Ptr;
    List *bitsListPtr, *valueListPtr, *p, *pp, *nextPtr, *listPtr;
    Import *importPtr;
    int i, nBits, bit;
    
    /*
     * Check unknown identifiers in OID DEFVALs.
     */

    for(objectPtr = modulePtr->firstObjectPtr;
	objectPtr; objectPtr = objectPtr->nextPtr) {

	if (objectPtr->export.value.basetype == SMI_BASETYPE_UNKNOWN)
	    continue;
	
	if (objectPtr->export.value.basetype ==
	    SMI_BASETYPE_OBJECTIDENTIFIER) {
	    /* a len of -1 indicates an unresolved label in ptr */
	    if (objectPtr->export.value.len == -1) {
		object2Ptr = findObjectByModuleAndName(
		    parserPtr->modulePtr,
		    (char *)objectPtr->export.value.value.ptr);
		if (!object2Ptr) {
		    importPtr = findImportByName(
			(char *)objectPtr->export.value.value.ptr, modulePtr);
		    if (importPtr) {		/* imported object */
			importPtr->use++;
			object2Ptr = findObjectByModulenameAndName(
			    importPtr->export.module,
			    importPtr->export.name);
		    }
		}
		if (!object2Ptr) {
		    smiPrintErrorAtLine(parserPtr, ERR_UNKNOWN_OIDLABEL,
					objectPtr->line,
				(char *)objectPtr->export.value.value.ptr);
		    smiFree(objectPtr->export.value.value.ptr);
		    objectPtr->export.value.value.ptr = NULL;
		    objectPtr->export.value.basetype = SMI_BASETYPE_UNKNOWN;
		} else {
		    objectPtr->export.value.len = object2Ptr->export.oidlen;
		    objectPtr->export.value.value.ptr =
			smiMalloc(object2Ptr->export.oidlen *
				  sizeof(SmiSubid));
		    memcpy(objectPtr->export.value.value.ptr,
			   object2Ptr->export.oid,
			   object2Ptr->export.oidlen * sizeof(SmiSubid));
		}
	    }
	} else if (objectPtr->export.value.basetype == SMI_BASETYPE_BITS) {
	    bitsListPtr = objectPtr->typePtr->listPtr;
	    valueListPtr = (void *)objectPtr->export.value.value.ptr;
	    for (nBits = 0, p = bitsListPtr; p; nBits++, p = p->nextPtr);
	    objectPtr->export.value.value.ptr = smiMalloc((nBits+7)/8);
	    memset(objectPtr->export.value.value.ptr, 0, (nBits+7)/8);
	    objectPtr->export.value.len = (nBits+7)/8;
	    for (i = 0, p = valueListPtr; p; i++) {
		for (bit = 0, pp = bitsListPtr; bit < nBits;
		     bit++, pp = pp->nextPtr) {
		    if (!strcmp(p->ptr,
				((NamedNumber *)(pp->ptr))->export.name))
			break;
		}
		if (bit < nBits) {
		    objectPtr->export.value.value.ptr[bit/8] |= 1 << bit%8;
		}
		smiFree(p->ptr);
		nextPtr = p->nextPtr;
		smiFree(p);
		p = nextPtr;
	    }
	} else if (objectPtr->export.value.basetype == SMI_BASETYPE_ENUM) {
	    /* a len of -1 indicates an unresolved enum label in ptr */
	    if (objectPtr->export.value.len == -1) {
		for (listPtr = objectPtr->typePtr->listPtr; listPtr;
		     listPtr = listPtr->nextPtr) {
		    if (!strcmp(((NamedNumber *)(listPtr->ptr))->export.name,
				objectPtr->export.value.value.ptr)) {
			smiFree(objectPtr->export.value.value.ptr);
			objectPtr->export.value.value.unsigned32 =
			    ((NamedNumber *)(listPtr->ptr))->
			    export.value.value.unsigned32;
			objectPtr->export.value.len = 1;
			break;
		    }
		}
	    }
	}
    }
}



static void
checkImportsUsage(Parser *parserPtr, Module *modulePtr)
{
    Import *importPtr;
    
    /*
     * Check usage of all imported identifiers.
     */

    if (strcmp(modulePtr->export.name, "SNMPv2-TC") &&
	strcmp(modulePtr->export.name, "SNMPv2-CONF") &&
	strcmp(modulePtr->export.name, "RFC-1212") &&
	strcmp(modulePtr->export.name, "RFC-1215")) {
	for(importPtr = modulePtr->firstImportPtr;
	    importPtr; importPtr = importPtr->nextPtr) {
	    if (! strcmp(importPtr->export.module, "SNMPv2-SMI")) {
		if (! strcmp(importPtr->export.name, "ExtUTCTime")
		    || !strcmp(importPtr->export.name, "ObjectName")
		    || !strcmp(importPtr->export.name, "NotificationName")) {
		    smiPrintErrorAtLine(parserPtr, ERR_ILLEGAL_IMPORT,
					importPtr->line,
					importPtr->export.name,
					importPtr->export.module);
		}
	    }
	    /* checkImports() handles KIND_NOTFOUND */
	    if (importPtr->use == 0 && importPtr->kind != KIND_NOTFOUND) {
		smiPrintErrorAtLine(parserPtr, ERR_UNUSED_IMPORT,
				    importPtr->line,
				    importPtr->export.name,
				    importPtr->export.module);
	    }
	}
    }
}



static time_t
checkDate(Parser *parserPtr, char *date)
{
    struct tm	tm;
    time_t	anytime;
    int		i, len;
    char	*p;
    
    memset(&tm, 0, sizeof(tm));
    anytime = 0;
    
    len = strlen(date);
    if (len == 11 || len == 13) {
	for (i = 0; i < len; i++) {
	    if ( (i < len-1 && ! isdigit((int) date[i]))
		 || (i == len-1 && date[len-1] != 'Z')) {
		smiPrintError(parserPtr, ERR_DATE_CHARACTER, date);
		anytime = (time_t) -1;
		break;
	    }
	}
    } else {
	smiPrintError(parserPtr, ERR_DATE_LENGTH, date);
	anytime = (time_t) -1;
    }

    if (anytime == 0) {
	for (i = 0, p = date, tm.tm_year = 0;
	     i < ((len == 11) ? 2 : 4); i++, p++) {
	    tm.tm_year = tm.tm_year * 10 + (*p - '0');
	}
	if (len == 11) {
	    tm.tm_year += 1900;
	    if (tm.tm_year < 1990)
		smiPrintError(parserPtr, ERR_DATE_YEAR_2DIGITS,
			      date, tm.tm_year);
	}
	tm.tm_mon  = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	tm.tm_mday = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	tm.tm_hour = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	tm.tm_min  = (p[0]-'0') * 10 + (p[1]-'0');
	
	if (tm.tm_mon < 1 || tm.tm_mon > 12) {
	    smiPrintError(parserPtr, ERR_DATE_MONTH, date);
	}
	if (tm.tm_mday < 1 || tm.tm_mday > 31) {
	    smiPrintError(parserPtr, ERR_DATE_DAY, date);
	}
	if (tm.tm_hour < 0 || tm.tm_hour > 23) {
	    smiPrintError(parserPtr, ERR_DATE_HOUR, date);
	}
	if (tm.tm_min < 0 || tm.tm_min > 59) {
	    smiPrintError(parserPtr, ERR_DATE_MINUTES, date);
	}
	
	tm.tm_year -= 1900;
	tm.tm_mon -= 1;
	tm.tm_isdst = 0;

	anytime = timegm(&tm);
	
	if (anytime == (time_t) -1) {
	    smiPrintError(parserPtr, ERR_DATE_VALUE, date);
	} else {
	    if (anytime < SMI_EPOCH) {
		smiPrintError(parserPtr, ERR_DATE_IN_PAST, date);
	    }
	    if (anytime > time(NULL)) {
		smiPrintError(parserPtr, ERR_DATE_IN_FUTURE, date);
	    }
	}
    }

    return (anytime == (time_t) -1) ? 0 : anytime;
}
    
%}

/*
 * The grammars start symbol.
 */
%start mibFile



/*
 * We call the parser from within the parser when IMPORTing modules,
 * hence we need reentrant parser code. This is a bison feature.
 */
%pure_parser



/*
 * The attributes.
 */
%union {
    char           *text;	  		/* scanned quoted text       */
    char           *id;				/* identifier name           */
    int            err;				/* actually just a dummy     */
    time_t	   date;			/* a date value              */
    Object         *objectPtr;			/* object identifier         */
    SmiStatus      status;			/* a STATUS value            */
    SmiAccess      access;			/* an ACCESS value           */
    Type           *typePtr;
    List           *listPtr;			/* SEQUENCE and INDEX lists  */
    NamedNumber    *namedNumberPtr;		/* BITS or enum item         */
    Range          *rangePtr;			/* type restricting range    */
    SmiValue	   *valuePtr;
    SmiUnsigned32  unsigned32;			/*                           */
    SmiInteger32   integer32;			/*                           */
    struct Compl   compl;
    struct Index   index;
    Module	   *modulePtr;
}



/*
 * Tokens and their attributes.
 */
%token DOT_DOT
%token COLON_COLON_EQUAL

%token <id>UPPERCASE_IDENTIFIER
%token <id>LOWERCASE_IDENTIFIER
%token <unsigned32>NUMBER
%token <integer32>NEGATIVENUMBER
%token <text>BIN_STRING
%token <text>HEX_STRING
%token <text>QUOTED_STRING

%token <id>ACCESS
%token <id>AGENT_CAPABILITIES
%token <id>APPLICATION
%token <id>AUGMENTS
%token <id>BEGIN_
%token <id>BITS
%token <id>CHOICE
%token <id>CONTACT_INFO
%token <id>CREATION_REQUIRES
%token <id>COUNTER32
%token <id>COUNTER64
%token <id>DEFINITIONS
%token <id>DEFVAL
%token <id>DESCRIPTION
%token <id>DISPLAY_HINT
%token <id>END
%token <id>ENTERPRISE
%token <id>EXPORTS
%token <id>FROM
%token <id>GROUP
%token <id>GAUGE32
%token <id>IDENTIFIER
%token <id>IMPLICIT
%token <id>IMPLIED
%token <id>IMPORTS
%token <id>INCLUDES
%token <id>INDEX
%token <id>INTEGER
%token <id>INTEGER32
%token <id>IPADDRESS
%token <id>LAST_UPDATED
%token <id>MACRO
%token <id>MANDATORY_GROUPS
%token <id>MAX_ACCESS
%token <id>MIN_ACCESS
%token <id>MODULE
%token <id>MODULE_COMPLIANCE
%token <id>MODULE_IDENTITY
%token <id>NOTIFICATIONS
%token <id>NOTIFICATION_GROUP
%token <id>NOTIFICATION_TYPE
%token <id>OBJECT
%token <id>OBJECT_GROUP
%token <id>OBJECT_IDENTITY
%token <id>OBJECT_TYPE
%token <id>OBJECTS
%token <id>OCTET
%token <id>OF
%token <id>ORGANIZATION
%token <id>OPAQUE
%token <id>PRODUCT_RELEASE
%token <id>REFERENCE
%token <id>REVISION
%token <id>SEQUENCE
%token <id>SIZE
%token <id>STATUS
%token <id>STRING
%token <id>SUPPORTS
%token <id>SYNTAX
%token <id>TEXTUAL_CONVENTION
%token <id>TIMETICKS
%token <id>TRAP_TYPE
%token <id>UNITS
%token <id>UNIVERSAL
%token <id>UNSIGNED32
%token <id>VARIABLES
%token <id>VARIATION
%token <id>WRITE_SYNTAX



/*
 * Types of non-terminal symbols.
 */
%type  <err>mibFile
%type  <err>modules
%type  <err>module
%type  <id>moduleName
%type  <id>importIdentifier
%type  <err>importIdentifiers
%type  <id>importedKeyword
%type  <err>linkagePart
%type  <err>linkageClause
%type  <err>importPart
%type  <err>imports
%type  <err>declarationPart
%type  <err>declarations
%type  <err>declaration
%type  <err>exportsClause
%type  <err>macroClause
%type  <id>macroName
%type  <typePtr>choiceClause
%type  <id>typeName
%type  <id>typeSMI
%type  <err>typeTag
%type  <id>fuzzy_lowercase_identifier
%type  <err>valueDeclaration
%type  <typePtr>conceptualTable
%type  <typePtr>row
%type  <typePtr>entryType
%type  <listPtr>sequenceItems
%type  <objectPtr>sequenceItem
%type  <typePtr>Syntax
%type  <typePtr>sequenceSyntax
%type  <listPtr>NamedBits
%type  <namedNumberPtr>NamedBit
%type  <err>objectIdentityClause
%type  <err>objectTypeClause
%type  <err>trapTypeClause
%type  <text>descriptionClause
%type  <listPtr>VarPart
%type  <listPtr>VarTypes
%type  <objectPtr>VarType
%type  <text>DescrPart
%type  <access>MaxAccessPart
%type  <err>notificationTypeClause
%type  <err>moduleIdentityClause
%type  <err>typeDeclaration
%type  <typePtr>typeDeclarationRHS
%type  <typePtr>ObjectSyntax
%type  <typePtr>sequenceObjectSyntax
%type  <valuePtr>valueofObjectSyntax
%type  <typePtr>SimpleSyntax
%type  <valuePtr>valueofSimpleSyntax
%type  <typePtr>sequenceSimpleSyntax
%type  <typePtr>ApplicationSyntax
%type  <typePtr>sequenceApplicationSyntax
%type  <listPtr>anySubType
%type  <listPtr>integerSubType
%type  <listPtr>octetStringSubType
%type  <listPtr>ranges
%type  <rangePtr>range
%type  <valuePtr>value
%type  <listPtr>enumSpec
%type  <listPtr>enumItems
%type  <namedNumberPtr>enumItem
%type  <valuePtr>enumNumber
%type  <status>Status
%type  <status>Status_Capabilities
%type  <text>DisplayPart
%type  <text>UnitsPart
%type  <access>Access
%type  <index>IndexPart
%type  <listPtr>IndexTypes
%type  <objectPtr>IndexType
%type  <objectPtr>Index
%type  <objectPtr>Entry
%type  <valuePtr>DefValPart
%type  <valuePtr>Value
%type  <listPtr>BitsValue
%type  <listPtr>BitNames
%type  <objectPtr>ObjectName
%type  <objectPtr>NotificationName
%type  <text>ReferPart
%type  <err>RevisionPart
%type  <err>Revisions
%type  <err>Revision
%type  <listPtr>ObjectsPart
%type  <listPtr>Objects
%type  <objectPtr>Object
%type  <listPtr>NotificationsPart
%type  <listPtr>Notifications
%type  <objectPtr>Notification
%type  <text>Text
%type  <date>ExtUTCTime
%type  <objectPtr>objectIdentifier
%type  <objectPtr>subidentifiers
%type  <objectPtr>subidentifier
%type  <text>objectIdentifier_defval
%type  <err>subidentifiers_defval
%type  <err>subidentifier_defval
%type  <err>objectGroupClause
%type  <err>notificationGroupClause
%type  <err>moduleComplianceClause
%type  <compl>ComplianceModulePart
%type  <compl>ComplianceModules
%type  <compl>ComplianceModule
%type  <modulePtr>ComplianceModuleName
%type  <listPtr>MandatoryPart
%type  <listPtr>MandatoryGroups
%type  <objectPtr>MandatoryGroup
%type  <compl>CompliancePart
%type  <compl>Compliances
%type  <compl>Compliance
%type  <listPtr>ComplianceGroup
%type  <listPtr>ComplianceObject
%type  <typePtr>SyntaxPart
%type  <typePtr>WriteSyntaxPart
%type  <typePtr>WriteSyntax
%type  <access>AccessPart
%type  <err>agentCapabilitiesClause
%type  <err>ModulePart_Capabilities
%type  <err>Modules_Capabilities
%type  <err>Module_Capabilities
%type  <modulePtr>ModuleName_Capabilities
%type  <listPtr>CapabilitiesGroups
%type  <listPtr>CapabilitiesGroup
%type  <err>VariationPart
%type  <err>Variations
%type  <err>Variation
%type  <access>VariationAccessPart
%type  <access>VariationAccess
%type  <err>CreationPart
%type  <err>Cells
%type  <err>Cell

%%

/*
 * Yacc rules.
 *
 */


/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
mibFile:
    modules
    {
        $$ = 0;
    }
| /* empty */
    {
	$$ = 0;
    }
;

modules:		module
			{ $$ = 0; }
	|		modules module
			{ $$ = 0; }
	;

/*
 * The general structure of a module is described at REF:RFC1902,3. .
 * An example is given at REF:RFC1902,5.7. .
 */
module:			moduleName
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    thisParserPtr->modulePtr = findModuleByName($1);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule($1,
					      smiStrdup(thisParserPtr->path),
					      0,
					      thisParserPtr);
				thisParserPtr->modulePtr->
				    numImportedIdentifiers = 0;
				thisParserPtr->modulePtr->
				    numStatements = 0;
				thisParserPtr->modulePtr->
				    numModuleIdentities = 0;
				if (!strcmp($1, "SNMPv2-SMI")) {
			            /*
				     * SNMPv2-SMI is an SMIv2 module
				     * that cannot be identified by
				     * importing from SNMPv2-SMI.
				     */
				    thisModulePtr->export.language =
					SMI_LANGUAGE_SMIV2;
				}
			    } else {
			        smiPrintError(thisParserPtr,
					      ERR_MODULE_ALREADY_LOADED,
					      $1);
				/*
				 * this aborts parsing the whole file,
				 * not only the current module.
				 */
				YYABORT;
			    }
			}
			DEFINITIONS COLON_COLON_EQUAL BEGIN_
			exportsClause
			linkagePart
			declarationPart
			END
			{
			    checkModuleIdentity(thisParserPtr, thisModulePtr);
			    checkObjects(thisParserPtr, thisModulePtr);
			    checkTypes(thisParserPtr, thisModulePtr);
			    checkDefvals(thisParserPtr, thisModulePtr);
			    checkImportsUsage(thisParserPtr, thisModulePtr);

			    capabilitiesModulePtr = NULL;

                            $$ = 0;
			}
	;

/*
 * REF:RFC1902,3.2.
 */
linkagePart:		linkageClause
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

linkageClause:		IMPORTS importPart ';'
			{
			    firstStatementLine = thisParserPtr->line;

			    if (thisModulePtr->export.language != SMI_LANGUAGE_SMIV2)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    $$ = 0;
			}
			    
        ;

exportsClause:		/* empty */
			{ $$ = 0; }
	|		EXPORTS
			{
			    firstStatementLine = thisParserPtr->line;

			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_EXPORTS);
			    }
			}
			/* the scanner skips until... */
			';'
			{ $$ = 0; }
	;

importPart:		imports
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
			/* TODO: ``IMPORTS ;'' allowed? refer ASN.1! */
	;

imports:		import
			{ $$ = 0; }
	|		imports import
			{ $$ = 0; }
	;

import:			importIdentifiers FROM moduleName
			/* TODO: multiple clauses with same moduleName
			 * allowed? I guess so. refer ASN.1! */
			{
			    Import      *importPtr;
			    Module      *modulePtr;
			    
			    /*
			     * Recursively call the parser to suffer
			     * the IMPORTS, if the module is not yet
			     * loaded.
			     */
			    modulePtr = findModuleByName($3);
			    if (!modulePtr) {
				modulePtr = loadModule($3);
			    }
			    checkImports(modulePtr, thisParserPtr);

			    if (modulePtr && !strcmp($3, "SNMPv2-SMI")) {
			        /*
				 * A module that imports a macro or
				 * type definition from SNMPv2-SMI
				 * seems to be SMIv2 style.
				 */
				for (importPtr =
					 thisModulePtr->firstImportPtr;
				     importPtr;
				     importPtr = importPtr->nextPtr) {
				    if ((!strcmp(importPtr->export.module,
						 $3)) &&
					((importPtr->kind == KIND_MACRO) ||
					 (importPtr->kind == KIND_TYPE))) {
					thisModulePtr->export.language =
					    SMI_LANGUAGE_SMIV2;
				    }
				}
			    }

			}
	;

importIdentifiers:	importIdentifier
			{ $$ = 0; }
	|		importIdentifiers ',' importIdentifier
			/* TODO: might this list list be empty? */
			{ $$ = 0; }
	;

/*
 * Note that some named types must not be imported, REF:RFC1902,590 .
 */
importIdentifier:	LOWERCASE_IDENTIFIER
			{
			    addImport($1, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    $$ = 0;
			}
	|		UPPERCASE_IDENTIFIER
			{
			    addImport($1, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    $$ = 0;
			}
	|		importedKeyword
			{
			    addImport(smiStrdup($1), thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    $$ = 0;
			}
	;

/*
 * These keywords are no real keywords. They have to be imported
 * from the SMI, TC, CONF MIBs.
 */
/*
 * TODO: Think! Shall we really leave these words as keywords or should
 * we prefer the symbol table appropriately??
 */
importedKeyword:	AGENT_CAPABILITIES
	|		BITS
	|		COUNTER32
	|		COUNTER64
	|		GAUGE32
	|		INTEGER32
	|		IPADDRESS
	|		MANDATORY_GROUPS
	|		MODULE_COMPLIANCE
	|		MODULE_IDENTITY
	|		NOTIFICATION_GROUP
	|		NOTIFICATION_TYPE
	|		OBJECT_GROUP
	|		OBJECT_IDENTITY
	|		OBJECT_TYPE
	|		OPAQUE
	|		TEXTUAL_CONVENTION
	|		TIMETICKS
	|		TRAP_TYPE
	|		UNSIGNED32
	;

moduleName:		UPPERCASE_IDENTIFIER
			{
			    checkNameLen(thisParserPtr, $1,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    $$ = $1;
			}
	;

/*
 * The paragraph at REF:RFC1902,490 lists roughly what's allowed
 * in the body of an information module.
 */
declarationPart:	declarations
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
			/* TODO: might this list really be emtpy? */
	;

declarations:		declaration
			{ $$ = 0; }
	|		declarations declaration
			{ $$ = 0; }
	;

declaration:		typeDeclaration
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		valueDeclaration
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		objectIdentityClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		objectTypeClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		trapTypeClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		notificationTypeClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		moduleIdentityClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		moduleComplianceClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		objectGroupClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		notificationGroupClause
			{
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		agentCapabilitiesClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		macroClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		error '}'
			{
			    smiPrintError(thisParserPtr,
					  ERR_FLUSH_DECLARATION);
			    yyerrok;
			    $$ = 1;
			}
	;

/*
 * Macro clauses. Its contents are not really parsed, but skipped by
 * the scanner until 'END' is read. This is just to make the SMI
 * documents readable.
 */
macroClause:		macroName
			MACRO
			{
			    firstStatementLine = thisParserPtr->line;

			    /*
			     * ASN.1 macros are known to be in these
			     * modules.
			     */
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC-1215") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_MACRO);
			    }
			}
			/* the scanner skips until... */
			END
			{
			    Macro *macroPtr;

			    macroPtr = addMacro($1, 0, thisParserPtr);
			    setMacroLine(macroPtr, firstStatementLine,
					 thisParserPtr);
			    smiFree($1);
			    $$ = 0;
                        }
	;

macroName:		MODULE_IDENTITY     { $$ = smiStrdup($1); }
	|		OBJECT_TYPE	    { $$ = smiStrdup($1); }
	|		TRAP_TYPE	    { $$ = smiStrdup($1); }
	|		NOTIFICATION_TYPE   { $$ = smiStrdup($1); }
	|		OBJECT_IDENTITY	    { $$ = smiStrdup($1); }
	|		TEXTUAL_CONVENTION  { $$ = smiStrdup($1); }
	|		OBJECT_GROUP	    { $$ = smiStrdup($1); }
	|		NOTIFICATION_GROUP  { $$ = smiStrdup($1); }
	|		MODULE_COMPLIANCE   { $$ = smiStrdup($1); }
	|		AGENT_CAPABILITIES  { $$ = smiStrdup($1); }
	;

choiceClause:		CHOICE
			{
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_CHOICE);
			    }
			}
			/* the scanner skips until... */
			'}'
			{
			    $$ = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			}
	;

/*
 * The only ASN.1 value declarations are for OIDs, REF:RFC1902,491 .
 */
fuzzy_lowercase_identifier:	LOWERCASE_IDENTIFIER
			{
			  $$ = $1;
			}
	|
			UPPERCASE_IDENTIFIER
			{
			    smiPrintError (thisParserPtr,
					   ERR_BAD_LOWER_IDENTIFIER_CASE,
					   $1);
			    /* xxx
			    if ((thisParserPtr->flags & SMI_FLAG_BE_LAX) == 0) {
			        YYERROR;
			    }
			    */
			  $$ = $1;
			}
	;

/* valueDeclaration:	LOWERCASE_IDENTIFIER */
valueDeclaration:	fuzzy_lowercase_identifier
			{
			    firstStatementLine = thisParserPtr->line;

			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        if (strchr($1, '-') &&
				    (strcmp($1, "mib-2") ||
				  strcmp(thisModulePtr->export.name, "SNMPv2-SMI"))) {
				    smiPrintError(thisParserPtr,
						  ERR_OIDNAME_INCLUDES_HYPHEN,
						  $1);
				}
			    }
			}
			OBJECT IDENTIFIER
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $7;
			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);
			    objectPtr = setObjectName(objectPtr, $1);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_VALUEASSIGNMENT);
			    $$ = 0;
			}
	;

/*
 * This is for simple ASN.1 style type assignments and textual conventions.
 */
typeDeclaration:	typeName
			{
			    firstStatementLine = thisParserPtr->line;

			    checkNameLen(thisParserPtr, $1,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			}
			COLON_COLON_EQUAL typeDeclarationRHS
			{
			    Type *typePtr;
			    
			    if (strlen($1)) {
				if ($4->export.basetype != SMI_BASETYPE_UNKNOWN) {
				    smiCheckTypeName(thisParserPtr,
						     thisModulePtr, $1,
						     firstStatementLine);
				}
				setTypeLine($4, firstStatementLine,
					    thisParserPtr);
				setTypeName($4, $1);
				$$ = 0;
			    } else {
				$$ = 0;
			    }

			    /*
			     * If we are in an SMI module, some type
			     * definitions derived from ASN.1 `INTEGER'
			     * must be modified to libsmi basetypes.
			     */
			    if (thisModulePtr &&
				!strcmp(thisModulePtr->export.name, "SNMPv2-SMI")) {
				if (!strcmp($1, "Counter32")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				    if ($4->listPtr) {
					((Range *)$4->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)$4->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp($1, "Gauge32")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				    if ($4->listPtr) {
					((Range *)$4->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)$4->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp($1, "Unsigned32")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				    if ($4->listPtr) {
					((Range *)$4->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)$4->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp($1, "TimeTicks")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				    if ($4->listPtr) {
					((Range *)$4->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)$4->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)$4->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp($1, "Counter64")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED64;
				    if ($4->listPtr) {
					((Range *)$4->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)$4->listPtr->ptr)->export.minValue.value.unsigned64 = 0;
					((Range *)$4->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)$4->listPtr->ptr)->export.maxValue.value.unsigned64 = LIBSMI_UINT64_MAX;
				    }
				    setTypeParent($4, typeUnsigned64Ptr);
				}
			    }
			    if (thisModulePtr &&
				!strcmp(thisModulePtr->export.name, "RFC1155-SMI")) {
				if (!strcmp($1, "Counter")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				} else if (!strcmp($1, "Gauge")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				} else if (!strcmp($1, "TimeTicks")) {
				    $4->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent($4, typeUnsigned32Ptr);
				} else if (!strcmp($1, "NetworkAddress")) {
				    setTypeName($4, smiStrdup("NetworkAddress"));
				    $4->export.basetype = SMI_BASETYPE_OCTETSTRING;
				    setTypeParent($4, findTypeByModuleAndName(
					                   thisModulePtr,
						           "IpAddress"));
				} else if (!strcmp($1, "IpAddress")) {
				    typePtr = findTypeByModuleAndName(
					thisModulePtr, smiStrdup("NetworkAddress"));
				    if (typePtr) 
					setTypeParent(typePtr, $4);
				}
			    }
			}
	;

typeName:		UPPERCASE_IDENTIFIER
			{
			    $$ = $1;
			}
	|		typeSMI
			{
			    $$ = smiStrdup($1);
			    /*
			     * well known types (keywords in this grammar)
			     * are known to be defined in these modules.
			     */
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_SMI, $1);
			    }
			}
	;

typeSMI:		INTEGER32
	|		IPADDRESS
	|		COUNTER32
	|		GAUGE32
	|		UNSIGNED32
	|		TIMETICKS
	|		OPAQUE
	|		COUNTER64
	;

typeDeclarationRHS:	Syntax
			{
			    if ($1->export.name) {
				/*
				 * If we found an already defined type,
				 * we have to inherit a new type structure.
				 * (Otherwise the `Syntax' rule created
				 * a new type for us.)
				 */
				$$ = duplicateType($1, 0, thisParserPtr);
				setTypeDecl($$, SMI_DECL_TYPEASSIGNMENT);
			    } else {
				$$ = $1;
				setTypeDecl($$, SMI_DECL_TYPEASSIGNMENT);
			    }
			}
	|		TEXTUAL_CONVENTION
			{
			    Import *importPtr;

			    if (strcmp(thisModulePtr->export.name, "SNMPv2-TC")) {
				importPtr =
				    findImportByName("TEXTUAL-CONVENTION",
						     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_MACRO_NOT_IMPORTED,
						  "TEXTUAL-CONVENTION",
						  "SNMPv2-TC");
				}
			    }
			}
			DisplayPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			SYNTAX Syntax
			{
			    if (($10) && !($10->export.name)) {
				/*
				 * If the Type we found has just been
				 * defined, we don't have to allocate
				 * a new one.
				 */
				$$ = $10;
			    } else {
				if (!($10))
				    smiPrintError(thisParserPtr, ERR_INTERNAL);
				/*
				 * Otherwise, we have to allocate a
				 * new Type struct, inherited from $10.
				 */
				$$ = duplicateType($10, 0, thisParserPtr);
			    }
			    setTypeDescription($$, $7, thisParserPtr);
			    if ($8) {
				setTypeReference($$, $8, thisParserPtr);
			    }
			    setTypeStatus($$, $5);
			    if ($3) {
				smiCheckFormat(thisParserPtr,
					       $$->export.basetype, $3,
					       firstStatementLine);
				setTypeFormat($$, $3);
			    }
			    setTypeDecl($$, SMI_DECL_TEXTUALCONVENTION);
			}
	|		choiceClause
			{
			    $$ = $1;
			    setTypeDecl($$, SMI_DECL_TYPEASSIGNMENT);
			}
	;

/* REF:RFC1902,7.1.12. */
conceptualTable:	SEQUENCE OF row
			{
			    if ($3) {
				$$ = addType(NULL,
					     SMI_BASETYPE_UNKNOWN, 0,
					     thisParserPtr);
				setTypeDecl($$, SMI_DECL_IMPL_SEQUENCEOF);
				setTypeParent($$, $3);
			    } else {
				$$ = NULL;
			    }
			}
	;

row:			UPPERCASE_IDENTIFIER
			/*
			 * In this case, we do NOT allow `Module.Type'.
			 * The identifier must be defined in the local
			 * module.
			 */
			{
			    Type *typePtr;
			    Import *importPtr;

			    $$ = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->export.name, $1);
			    if (! $$) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    typePtr = addType($1,
						      SMI_BASETYPE_UNKNOWN,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				    $$ = typePtr;
				} else {
				    /*
				     * imported type.
				     * TODO: is this allowed in a SEQUENCE? 
				     */
				    importPtr->use++;
				    $$ = findTypeByModulenameAndName(
					importPtr->export.module,
					importPtr->export.name);
				}
			    }
			}
		        /* TODO: this must be an entryType */
	;

/* REF:RFC1902,7.1.12. */
entryType:		SEQUENCE '{' sequenceItems '}'
			{
			    $$ = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList($$, $3);
			}
;

sequenceItems:		sequenceItem
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		sequenceItems ',' sequenceItem
			/* TODO: might this list be emtpy? */
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 * NOTE: REF:RFC1902,7.1.12. was less clear, it said:
 * `normally omitting the sub-typing information'
 */
sequenceItem:		LOWERCASE_IDENTIFIER sequenceSyntax
			{
			    Object *objectPtr;
			    Import *importPtr;
			    
			    objectPtr =
			        findObjectByModuleAndName(thisParserPtr->modulePtr,
							  $1);

			    if (!objectPtr) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    objectPtr = addObject($1, pendingNodePtr,
					                  0,
					                  FLAG_INCOMPLETE,
						          thisParserPtr);
				} else {
				    /*
				     * imported object.
				     */
				    importPtr->use++;
				    objectPtr = findObjectByModulenameAndName(
					importPtr->export.module, $1);
				}
			    }

			    $$ = objectPtr;
			}
	;

Syntax:			ObjectSyntax
			{
			    $$ = $1;
			    if ($$)
				defaultBasetype = $$->export.basetype;
			}
	|		BITS '{' NamedBits '}'
			/* TODO: standalone `BITS' ok? seen in RMON2-MIB */
			/* -> no, it's only allowed in a SEQUENCE {...} */
			{
			    Type *typePtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_BITS;
			    typePtr = addType(NULL, SMI_BASETYPE_BITS,
					      FLAG_INCOMPLETE,
					      thisParserPtr);
			    setTypeDecl(typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(typePtr, typeBitsPtr);
			    setTypeList(typePtr, $3);
			    for (p = $3; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = typePtr;
			    $$ = typePtr;
			}
	;

sequenceSyntax:		/* ObjectSyntax */
			sequenceObjectSyntax
			{
			    $$ = $1;
			}
	|		BITS
			{
			    /* TODO: */
			    $$ = typeOctetStringPtr;
			}
	|		UPPERCASE_IDENTIFIER anySubType
			{
			    Type *typePtr;
			    Import *importPtr;
			    
			    $$ = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->export.name, $1);
			    if (! $$) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    typePtr = addType($1, SMI_BASETYPE_UNKNOWN,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				    $$ = typePtr;
				} else {
				    /*
				     * imported type.
				     *
				     * We are in a SEQUENCE clause,
				     * where we do not have to create
				     * a new Type struct.
				     */
				    importPtr->use++;
				}
			    }
			}
	;

NamedBits:		NamedBit
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		NamedBits ',' NamedBit
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

NamedBit:		LOWERCASE_IDENTIFIER
			{
			    checkNameLen(thisParserPtr, $1,
					 ERR_BITNAME_32, ERR_BITNAME_64);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (strchr($1, '-')) {
				    smiPrintError(thisParserPtr,
						  ERR_NAMEDBIT_INCLUDES_HYPHEN,
						  $1);
				}
			    }
			}
			'(' NUMBER ')'
			{
			    $$ = smiMalloc(sizeof(NamedNumber));
			    $$->export.name = $1;
			    $$->export.value.basetype =
				                       SMI_BASETYPE_UNSIGNED32;
			    $$->export.value.value.unsigned32 = $4;
			    /* RFC 2578 7.1.4 */
			    if ($4 >= 65535*8) {
				smiPrintError(thisParserPtr,
					      ERR_BITS_NUMBER_TOO_LARGE,
					      $1, $4);
			    } else {
				if ($4 >= 128) {
				    smiPrintError(thisParserPtr,
						  ERR_BITS_NUMBER_LARGE,
						  $1, $4);
				}
			    }
			}
	;

objectIdentityClause:	LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			OBJECT_IDENTITY
			{
			    Import *importPtr;

			    if (strcmp(thisModulePtr->export.name, "SNMPv2-SMI")) {
				importPtr = findImportByName("OBJECT-IDENTITY",
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_MACRO_NOT_IMPORTED,
						  "OBJECT-IDENTITY",
						  "SNMPv2-SMI");
				}
			    }
			}
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $12;
			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTIDENTITY);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, $6);
			    setObjectDescription(objectPtr, $8, thisParserPtr);
			    if ($9) {
				setObjectReference(objectPtr, $9, thisParserPtr);
			    }
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    $$ = 0;
			}
	;

objectTypeClause:	LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			OBJECT_TYPE
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("OBJECT-TYPE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_MACRO_NOT_IMPORTED,
						  "OBJECT-TYPE", "SNMPv2-SMI");
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_MACRO_NOT_IMPORTED,
						  "OBJECT-TYPE", "RFC-1212");
				}
			    }
			}
			SYNTAX Syntax
		        UnitsPart
			MaxAccessPart
			STATUS Status
			descriptionClause
			ReferPart
			IndexPart
			DefValPart
			COLON_COLON_EQUAL '{' ObjectName '}'
			{
			    Object *objectPtr, *parentPtr;

			    objectPtr = $17;

			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectType(objectPtr, $6);
			    if (!($6->export.name)) {
				/*
				 * An inlined type.
				 */
#if 0 /* export implicitly defined types by the node's lowercase name */
				setTypeName($6, $1);
#endif
			    }
			    setObjectAccess(objectPtr, $8);
			    if (thisParserPtr->flags & FLAG_CREATABLE) {
				thisParserPtr->flags &= ~FLAG_CREATABLE;
				parentPtr =
				  objectPtr->nodePtr->parentPtr->lastObjectPtr;
				if (parentPtr &&
				    parentPtr->export.indexkind !=
				       SMI_INDEX_UNKNOWN) {
				    /*
				     * add objectPtr to the parent object's
				     * listPtr, which is the list of columns
				     * needed for row creation.
				     *
				     * Note, that this would clash, if the
				     * parent row object-type is not yet
				     * defined.
				     */
				    /*
				    newlistPtr = smiMalloc(sizeof(List));
				    newlistPtr->nextPtr = NULL;
				    newlistPtr->ptr = objectPtr;
				    */
				    /*
				     * Look up the parent object-type.
				     */
				    /*
				    if (parentPtr->listPtr) {
					for(listPtr = parentPtr->listPtr;
					    listPtr->nextPtr;
					    listPtr = listPtr->nextPtr);
					listPtr->nextPtr = newlistPtr;
				    } else {
					parentPtr->listPtr = newlistPtr;
				    }
				    */
				    addObjectFlags(parentPtr, FLAG_CREATABLE);
				    setObjectCreate(parentPtr, 1);
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_SCALAR_READCREATE);
				}
			    }
			    setObjectStatus(objectPtr, $10);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    if ($11) {
				setObjectDescription(objectPtr, $11, thisParserPtr);
			    }
			    if ($12) {
				setObjectReference(objectPtr, $12, thisParserPtr);
			    }
			    if ($13.indexkind != SMI_INDEX_UNKNOWN) {
				setObjectList(objectPtr, $13.listPtr);
				setObjectImplied(objectPtr, $13.implied);
				setObjectIndexkind(objectPtr, $13.indexkind);
				setObjectRelated(objectPtr, $13.rowPtr);
			    }
			    if ($14) {
				setObjectValue(objectPtr, $14);
			    }
			    $$ = 0;
			}
	;

descriptionClause:	/* empty */
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				smiPrintError(thisParserPtr,
					      ERR_MISSING_DESCRIPTION);
			    }
			    $$ = NULL;
			}
	|		DESCRIPTION Text
			{
			    $$ = $2;
			}
	;

trapTypeClause:		fuzzy_lowercase_identifier
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			TRAP_TYPE
			{
			    Import *importPtr;
			    
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        smiPrintError(thisParserPtr, ERR_TRAP_TYPE);
			    }

			    importPtr = findImportByName("TRAP-TYPE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "TRAP-TYPE", "RFC-1215");
			    }
			}
			ENTERPRISE objectIdentifier
			VarPart
			DescrPart
			ReferPart
			COLON_COLON_EQUAL NUMBER
			/* TODO: range of number? */
			{
			    Object *objectPtr;
			    Node *nodePtr;
			    
			    objectPtr = $6;
			    nodePtr = findNodeByParentAndSubid(
				objectPtr->nodePtr, 0);
			    if (nodePtr && nodePtr->lastObjectPtr &&
	       		(nodePtr->lastObjectPtr->modulePtr == thisModulePtr)) {
				/*
				 * hopefully, the last defined Object for
				 * this Node is the one we expect.
				 */
				objectPtr = nodePtr->lastObjectPtr;
			    } else {
				objectPtr = addObject(NULL,
						      objectPtr->nodePtr,
						      0,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
			    }
			    objectPtr = addObject(NULL,
						  objectPtr->nodePtr,
						  $11,
						  FLAG_INCOMPLETE,
						  thisParserPtr);
			    
			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_TRAPTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectList(objectPtr, $7);
			    setObjectStatus(objectPtr, SMI_STATUS_CURRENT); 
			    setObjectDescription(objectPtr, $8, thisParserPtr);
			    if ($9) {
				setObjectReference(objectPtr, $9, thisParserPtr);
			    }
			    $$ = 0;
			}
	;

VarPart:		VARIABLES '{' VarTypes '}'
			{
			    $$ = $3;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

VarTypes:		VarType
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		VarTypes ',' VarType
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

VarType:		ObjectName
			{
			    $$ = $1;
			}
	;

DescrPart:		DESCRIPTION Text
			{ $$ = $2; }
	|		/* empty */
			{ $$ = NULL; }
	;

MaxAccessPart:		MAX_ACCESS
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        smiPrintError(thisParserPtr,
					      ERR_MAX_ACCESS_IN_SMIV1);
			    }
			}
			Access
			{ $$ = $3; }
	|		ACCESS
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        smiPrintError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			}
			Access
			/* TODO: limited values in v1 */
			{ $$ = $3; }
	;

notificationTypeClause:	LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			NOTIFICATION_TYPE
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("NOTIFICATION-TYPE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "NOTIFICATION-TYPE",
					      "SNMPv2-SMI");
			    }
			}
			ObjectsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL
			'{' NotificationName '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $13;

			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectList(objectPtr, $5);
			    setObjectStatus(objectPtr, $7);
			    setObjectDescription(objectPtr, $9, thisParserPtr);
			    if ($10) {
				setObjectReference(objectPtr, $10, thisParserPtr);
			    }
			    $$ = 0;
			}
	;

moduleIdentityClause:	LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			MODULE_IDENTITY
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("MODULE-IDENTITY",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "MODULE-IDENTITY",
					      "SNMPv2-SMI");
			    }
			    
			    if (thisParserPtr->modulePtr->numModuleIdentities > 0)
			    {
			        smiPrintError(thisParserPtr,
					      ERR_TOO_MANY_MODULE_IDENTITIES);
			    }
			    if (thisParserPtr->modulePtr->numStatements > 0) {
			        smiPrintError(thisParserPtr,
					      ERR_MODULE_IDENTITY_NOT_FIRST);
			    }
			}
			LAST_UPDATED ExtUTCTime
			{
			    setModuleLastUpdated(thisParserPtr->modulePtr, $6);
			}
			ORGANIZATION Text
			CONTACT_INFO Text
			DESCRIPTION Text
			RevisionPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $17;

			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    thisParserPtr->modulePtr->numModuleIdentities++;

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_MODULEIDENTITY);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, SMI_STATUS_CURRENT);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setModuleIdentityObject(thisParserPtr->modulePtr,
						    objectPtr);
			    setModuleOrganization(thisParserPtr->modulePtr,
						  $9);
			    setModuleContactInfo(thisParserPtr->modulePtr,
						 $11);
			    setModuleDescription(thisParserPtr->modulePtr,
						 $13, thisParserPtr);
			    /* setObjectDescription(objectPtr, $13); */
			    $$ = 0;
			}
        ;

ObjectSyntax:		SimpleSyntax
			{
			    $$ = $1;
			}
	|		typeTag SimpleSyntax
			{
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_TAG, $1);
			    }
			    $$ = $2;
			}
	|		conceptualTable	     /* TODO: possible? row? entry? */
			{
			    /* TODO */
			    $$ = $1;
			}
	|		row		     /* the uppercase name of a row  */
			{
			    /* TODO */
			    $$ = $1;
			}
	|		entryType	     /* SEQUENCE { ... } phrase */
			{
			    /* TODO */
			    $$ = $1;
			}
	|		ApplicationSyntax
			{
			    Import *importPtr;

			    if ($1 && $1->export.name) {
				importPtr = findImportByName($1->export.name,
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				}
			    }

			    /* TODO */
			    $$ = $1;
			}
        ;

typeTag:		'[' APPLICATION NUMBER ']' IMPLICIT
			{ $$ = 0; /* TODO: check range */ }
	|		'[' UNIVERSAL NUMBER ']' IMPLICIT
			{ $$ = 0; /* TODO: check range */ }
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceObjectSyntax:	sequenceSimpleSyntax
			{ $$ = $1; }
/*	|		conceptualTable	     /* TODO: possible? row? entry? */
/*	|		row		     /* the uppercase name of a row  */
/*	|		entryType	     /* it's SEQUENCE { ... } phrase */
	|		sequenceApplicationSyntax
			{
			    Import *importPtr;

			    if ($1 && $1->export.name) {
				importPtr = findImportByName($1->export.name,
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				}
			    }

			    /* TODO */
			    $$ = $1;
			}
        ;

valueofObjectSyntax:	valueofSimpleSyntax
			{ $$ = $1; }
			/* conceptualTables and rows do not have DEFVALs
			 */
			/* valueofApplicationSyntax would not introduce any
			 * further syntax of ObjectSyntax values.
			 */
	;

SimpleSyntax:		INTEGER			/* (-2147483648..2147483647) */
			{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    $$ = typeInteger32Ptr;
			}
	|		INTEGER
			{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			}
			integerSubType
			{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    $$ = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList($$, $3);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		INTEGER
			{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			}
			enumSpec
			{
			    List *p;
			    
			    $$ = duplicateType(typeEnumPtr, 0,
					       thisParserPtr);
			    setTypeList($$, $3);
			    for (p = $3; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = $$;
			}
	|		INTEGER32		/* (-2147483648..2147483647) */
			{
			    Import *importPtr;

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Integer32");
			    }

			    /* TODO: any need to distinguish from INTEGER? */
			    $$ = typeInteger32Ptr;
			}
        |		INTEGER32
			{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			}
			integerSubType
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Integer32");
			    }

			    $$ = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList($$, $3);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		UPPERCASE_IDENTIFIER
			{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			}
			enumSpec
			{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModuleAndName(
			        thisParserPtr->modulePtr, $1);
			    if (!parentPtr) {
			        importPtr = findImportByName($1,
							     thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					importPtr->export.module, $1);
				}
			    }
			    if (parentPtr) {
				if ((parentPtr->export.basetype !=
				     SMI_BASETYPE_ENUM) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_BITS)) {
				    smiPrintError(thisParserPtr,
						  ERR_ILLEGAL_ENUM_FOR_PARENT_TYPE,
						  $1);
				    $$ = duplicateType(typeEnumPtr, 0,
						       thisParserPtr);
				} else {
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				/* 
				 * forward referenced type. create it,
				 * marked with FLAG_INCOMPLETE.
				 */
				parentPtr = addType($1,
						    SMI_BASETYPE_UNKNOWN,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList($$, $3);
			    for (p = $3; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = $$;
			}
	|		moduleName '.' UPPERCASE_IDENTIFIER enumSpec
			/* TODO: UPPERCASE_IDENTIFIER must be an INTEGER */
			{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
                            defaultBasetype = SMI_BASETYPE_ENUM;
                            parentPtr = findTypeByModulenameAndName($1, $3);
			    if (!parentPtr) {
				importPtr =
				    findImportByModulenameAndName($1,
							  $3, thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr =
					findTypeByModulenameAndName($1, $3);
				}
			    }
			    if (parentPtr) {
				if ((parentPtr->export.basetype !=
				     SMI_BASETYPE_ENUM) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_BITS)) {
				    smiPrintError(thisParserPtr,
						  ERR_ILLEGAL_ENUM_FOR_PARENT_TYPE,
						  $3);
				    $$ = duplicateType(typeEnumPtr, 0,
						       thisParserPtr);
				} else {
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, $3);
				$$ = duplicateType(typeEnumPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList($$, $4);
			    for (p = $4; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = $$;
			}
	|		UPPERCASE_IDENTIFIER integerSubType
			{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (!parentPtr) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					importPtr->export.module, $1);
				}
			    }
			    if (parentPtr) {
				if ((parentPtr->export.basetype !=
				    SMI_BASETYPE_INTEGER32) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_INTEGER64) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_UNSIGNED32) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_UNSIGNED64)) {
				    smiPrintError(thisParserPtr,
					     ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE,
						  $1);
				    $$ = duplicateType(typeInteger32Ptr, 0,
						       thisParserPtr);
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				} else {
				    defaultBasetype =
					parentPtr->export.basetype;
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				/* 
				 * forward referenced type. create it,
				 * marked with FLAG_INCOMPLETE.
				 */
				parentPtr = addType($1,
						    SMI_BASETYPE_UNKNOWN,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
				defaultBasetype = SMI_BASETYPE_INTEGER32;
			    }
			    setTypeList($$, $2);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		moduleName '.' UPPERCASE_IDENTIFIER integerSubType
			/* TODO: UPPERCASE_IDENTIFIER must be an INT/Int32. */
			{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByModulenameAndName($1, $3);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName($1,
							  $3, thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					$1, $3);
				}
			    }
			    if (parentPtr) {
				if ((parentPtr->export.basetype !=
				    SMI_BASETYPE_INTEGER32) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_INTEGER64) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_UNSIGNED32) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_UNSIGNED64)) {
				    smiPrintError(thisParserPtr,
					     ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE,
						  $3);
				    $$ = duplicateType(typeInteger32Ptr, 0,
						       thisParserPtr);
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				} else {
				    defaultBasetype =
					parentPtr->export.basetype;
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, $3);
				$$ = duplicateType(typeInteger32Ptr, 0,
						   thisParserPtr);
				defaultBasetype = SMI_BASETYPE_INTEGER32;
			    }
			    setTypeList($$, $4);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		OCTET STRING		/* (SIZE (0..65535))	     */
			{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    $$ = typeOctetStringPtr;
			}
	|		OCTET STRING
			{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			}
			octetStringSubType
			{
			    
			    $$ = duplicateType(typeOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeList($$, $4);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		UPPERCASE_IDENTIFIER octetStringSubType
			{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (!parentPtr) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					importPtr->export.module, $1);
				}
			    }
			    if (parentPtr) {
				if (parentPtr->export.basetype !=
				    SMI_BASETYPE_OCTETSTRING) {
				    smiPrintError(thisParserPtr,
					      ERR_ILLEGAL_SIZE_FOR_PARENT_TYPE,
						  $1);
				    $$ = duplicateType(typeOctetStringPtr, 0,
						       thisParserPtr);
				} else {
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				/* 
				 * forward referenced type. create it,
				 * marked with FLAG_INCOMPLETE.
				 */
				parentPtr = addType($1,
						    SMI_BASETYPE_UNKNOWN,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList($$, $2);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		moduleName '.' UPPERCASE_IDENTIFIER octetStringSubType
			/* TODO: UPPERCASE_IDENTIFIER must be an OCTET STR. */
			{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModulenameAndName($1, $3);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName($1,
							  $3, thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					$1, $3);
				}
			    }
			    if (parentPtr) {
				if (parentPtr->export.basetype !=
				    SMI_BASETYPE_OCTETSTRING) {
				    smiPrintError(thisParserPtr,
					      ERR_ILLEGAL_SIZE_FOR_PARENT_TYPE,
						  $3);
				    $$ = duplicateType(typeOctetStringPtr, 0,
						       thisParserPtr);
				} else {
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, $3);
				$$ = duplicateType(typeOctetStringPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList($$, $4);
			    smiCheckTypeRanges(thisParserPtr, $$);
			}
	|		OBJECT IDENTIFIER
			{
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    $$ = typeObjectIdentifierPtr;
			}
        ;

valueofSimpleSyntax:	NUMBER			/* 0..2147483647 */
			/* NOTE: Counter64 must not have a DEFVAL */
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->value.unsigned32 = $1;
			}
	|		NEGATIVENUMBER		/* -2147483648..0 */
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    $$->value.integer32 = $1;
			}
	|		BIN_STRING		/* number or OCTET STRING */
			{
			    char s[9];
			    int i, len, j;
			    
			    $$ = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				$$->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen($1);
				$$->value.ptr =
				    smiMalloc((len+7)/8+1);
				for (i = 0; i < len; i += 8) {
				    strncpy(s, &$1[i], 8);
				    for (j = 1; j < 8; j++) {
					if (!s[j]) s[j] = '0';
				    }
				    s[8] = 0;
				    $$->value.ptr[i/8] =
					(unsigned char)strtol(s, 0, 2);
				}
				$$->len = (len+7)/8;
			    } else {
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 2);
			    }
			}
	|		HEX_STRING		/* number or OCTET STRING */
			{
			    char s[3];
			    int i, len;
			    
			    $$ = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				$$->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen($1);
				$$->value.ptr = smiMalloc((len+1)/2+1);
				for (i = 0; i < len; i += 2) {
				    strncpy(s, &$1[i], 2);
				    if (!s[1]) s[1] = '0';
				    s[2] = 0;
				    $$->value.ptr[i/2] =
					(unsigned char)strtol(s, 0, 16);
				}
				$$->len = (len+1)/2;
			    } else {
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 16);
			    }
			}
	|		LOWERCASE_IDENTIFIER	/* enumeration or named oid */
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    if ((defaultBasetype != SMI_BASETYPE_ENUM) &&
				(defaultBasetype != SMI_BASETYPE_OBJECTIDENTIFIER)) {
				smiPrintError(thisParserPtr, ERR_DEFVAL_SYNTAX);
				$$->basetype = defaultBasetype;
				if (defaultBasetype == SMI_BASETYPE_ENUM) {
				    $$->len = 1;
				    $$->value.unsigned32 = 0;
				} else {
				    $$->len = 0;
				    $$->value.ptr = NULL;
				}
			    } else {
				$$->basetype = defaultBasetype;
				$$->len = -1;  /* indicates unresolved ptr */
				$$->value.ptr = $1;   /* JS: needs strdup? */
			    }
			}
	|		QUOTED_STRING		/* an OCTET STRING */
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_OCTETSTRING;
			    $$->value.ptr = smiStrdup($1);
			    $$->len = strlen($1);
			}
			/* NOTE: If the value is an OBJECT IDENTIFIER, then
			 *       it must be expressed as a single ASN.1
			 *	 identifier, and not as a collection of
			 *	 of sub-identifiers.
			 *	 REF: draft,p.34
			 *	 Anyway, we try to accept it. But it's only
			 *	 possible for numbered sub-identifiers, since
			 *	 other identifiers would make something like
			 *	 { gaga } indistiguishable from a BitsValue.
			 */
	|		'{' objectIdentifier_defval '}'
			/*
			 * This is only for some MIBs with invalid numerical
			 * OID notation for DEFVALs. We DO NOT parse them
			 * correctly. We just don't want to produce a
			 * parser error.
			 */
			{
			    /*
			     * SMIv1 allows something like { 0 0 } !
			     * SMIv2 does not!
			     */
			    /* TODO: make it work correctly for SMIv1 */
			    if (thisModulePtr->export.language ==
				SMI_LANGUAGE_SMIV2)
			    {
				smiPrintError(thisParserPtr,
					      ERR_OID_DEFVAL_TOO_LONG);
			    }
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    $$->len = 2;
			    $$->value.oid = smiMalloc(2 * sizeof(SmiSubid));
			    $$->value.oid[0] = 0;
			    $$->value.oid[1] = 0;
			}
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceSimpleSyntax:	INTEGER	anySubType
			{
			    $$ = typeInteger32Ptr;
			}
        |		INTEGER32 anySubType
			{
			    Import *importPtr;
			    
			    /* TODO: any need to distinguish from INTEGER? */
			    $$ = typeInteger32Ptr;

			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Integer32");
			    }

			}
	|		OCTET STRING anySubType
			{
			    $$ = typeOctetStringPtr;
			}
	|		OBJECT IDENTIFIER
			{
			    $$ = typeObjectIdentifierPtr;
			}
	;

ApplicationSyntax:	IPADDRESS
			{
			    $$ = findTypeByName("IpAddress");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			}
	|		COUNTER32		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Counter32");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			}
	|		GAUGE32			/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Gauge32");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			}
	|		GAUGE32 integerSubType
			{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByName("Gauge32");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
				$$ = NULL;
			    } else {
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeList($$, $2);
				smiCheckTypeRanges(thisParserPtr, $$);
			    }
			    importPtr = findImportByName("Gauge32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    }
			}
	|		UNSIGNED32		/* (0..4294967295)	     */
			{
			    Import *importPtr;

			    $$ = typeUnsigned32Ptr;
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Unsigned32");
			    }
			}
	|		UNSIGNED32 integerSubType
			{
			    Import *importPtr;
			    
			    $$ = duplicateType(typeUnsigned32Ptr, 0,
					       thisParserPtr);
			    setTypeList($$, $2);
			    smiCheckTypeRanges(thisParserPtr, $$);
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Unsigned32");
			    }
			}
	|		TIMETICKS		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("TimeTicks");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			}
	|		OPAQUE			/* IMPLICIT OCTET STRING     */
			{
			    $$ = findTypeByName("Opaque");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_OPAQUE_OBSOLETE);
			    }
			}
	|		OPAQUE octetStringSubType
			{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByName("Opaque");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
				$$ = NULL;
			    } else {
				    smiPrintError(thisParserPtr,
						  ERR_OPAQUE_OBSOLETE);
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				    setTypeList($$, $2);
				    smiCheckTypeRanges(thisParserPtr, $$);
			    }
			    importPtr = findImportByName("Opaque",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    }
			}
	|		COUNTER64	        /* (0..18446744073709551615) */
			{
			    $$ = findTypeByName("Counter64");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter64");
			    }
			}
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceApplicationSyntax: IPADDRESS
			{
			    $$ = findTypeByName("IpAddress");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			}
	|		COUNTER32		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Counter32");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			}
	|		GAUGE32	anySubType	/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Gauge32");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			}
	|		UNSIGNED32 anySubType /* (0..4294967295)	     */
			{
			    Import *importPtr;
			    
			    $$ = typeUnsigned32Ptr;
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Unsigned32");
			    }
			}
	|		TIMETICKS		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("TimeTicks");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			}
	|		OPAQUE			/* IMPLICIT OCTET STRING     */
			{
			    $$ = findTypeByName("Opaque");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_OPAQUE_OBSOLETE);
			    }
			}
	|		COUNTER64	        /* (0..18446744073709551615) */
			{
			    $$ = findTypeByName("Counter64");
			    if (! $$) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter64");
			    }
			}
	;

anySubType:		integerSubType
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    $$ = NULL;
			}
	|	        octetStringSubType
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    $$ = NULL;
			}
	|		enumSpec
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    $$ = NULL;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
        ;		      


/* REF: draft,p.46 */
integerSubType:		'(' ranges ')'		/* at least one range        */
			/*
			 * the specification mentions an alternative of an
			 * empty RHS here. this would lead to reduce/reduce
			 * conflicts. instead, we differentiate the parent
			 * rule(s) (SimpleSyntax).
			 */
			{ $$ = $2; }
	;

octetStringSubType:	'(' SIZE '(' ranges ')' ')'
			/*
			 * the specification mentions an alternative of an
			 * empty RHS here. this would lead to reduce/reduce
			 * conflicts. instead, we differentiate the parent
			 * rule(s) (SimpleSyntax).
			 */
			{
			    $$ = $4;
			}
	;

ranges:			range
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		ranges '|' range
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    
			    $$ = $1;
			}
	;

range:			value
			{
			    $$ = smiMalloc(sizeof(Range));
			    $$->export.minValue = *$1;
			    $$->export.maxValue = *$1;
			    smiFree($1);
			}
	|		value DOT_DOT value
			{
			    $$ = smiMalloc(sizeof(Range));
			    $$->export.minValue = *$1;
			    $$->export.maxValue = *$3;
			    smiFree($1);
			    smiFree($3);
			}
	;

value:			NEGATIVENUMBER
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    $$->value.integer32 = $1;
			}
	|		NUMBER
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->value.unsigned32 = $1;
			}
	|		HEX_STRING
			{
			    char s[3];
			    int i, len;
			    
			    $$ = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				$$->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen($1);
				$$->value.ptr = smiMalloc((len+1)/2+1);
				for (i = 0; i < len; i += 2) {
				    strncpy(s, &$1[i], 2);
				    if (!s[1]) s[1] = '0';
				    s[2] = 0;
				    $$->value.ptr[i/2] =
					(unsigned char)strtol(s, 0, 16);
				}
				$$->len = (len+1)/2;
			    } else {
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 16);
			    }
			}
	|		BIN_STRING
			{
			    char s[9];
			    int i, len, j;
			    
			    $$ = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				$$->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen($1);
				$$->value.ptr = smiMalloc((len+7)/8+1);
				for (i = 0; i < len; i += 8) {
				    strncpy(s, &$1[i], 8);
				    for (j = 1; j < 8; j++) {
					if (!s[j]) s[j] = '0';
				    }
				    s[8] = 0;
				    $$->value.ptr[i/8] =
					(unsigned char)strtol(s, 0, 2);
				}
				$$->len = (len+7)/8;
			    } else {
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 2);
			    }
			}
	;

enumSpec:		'{' enumItems '}'
			{
			    $$ = $2;
			}
	;

enumItems:		enumItem
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		enumItems ',' enumItem
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

enumItem:		LOWERCASE_IDENTIFIER
			{
			    checkNameLen(thisParserPtr, $1,
					 ERR_ENUMNAME_32, ERR_ENUMNAME_64);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (strchr($1, '-')) {
				    smiPrintError(thisParserPtr,
					  ERR_NAMEDNUMBER_INCLUDES_HYPHEN,
						  $1);
				}
			    }
			}
			'(' enumNumber ')'
			{
			    $$ = smiMalloc(sizeof(NamedNumber));
			    $$->export.name = $1;
			    $$->export.value = *$4;
			    smiFree($4);
			}
	;

enumNumber:		NUMBER
			{
			    if ($1 > MAX_INTEGER32) {
				smiPrintError(thisParserPtr,
					      ERR_INTEGER32_TOO_LARGE, $1);
			    }
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    $$->value.integer32 = $1;
			}
	|		NEGATIVENUMBER
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    $$->value.integer32 = $1;
			    /* TODO: non-negative is suggested */
			}
	;

Status:			LOWERCASE_IDENTIFIER
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (!strcmp($1, "current")) {
				    $$ = SMI_STATUS_CURRENT;
				} else if (!strcmp($1, "deprecated")) {
				    $$ = SMI_STATUS_DEPRECATED;
				} else if (!strcmp($1, "obsolete")) {
				    $$ = SMI_STATUS_OBSOLETE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV2_STATUS,
						  $1);
				    $$ = SMI_STATUS_UNKNOWN;
				}
			    } else {
				if (!strcmp($1, "mandatory")) {
				    $$ = SMI_STATUS_MANDATORY;
				} else if (!strcmp($1, "optional")) {
				    $$ = SMI_STATUS_OPTIONAL;
				} else if (!strcmp($1, "obsolete")) {
				    $$ = SMI_STATUS_OBSOLETE;
				} else if (!strcmp($1, "deprecated")) {
				    $$ = SMI_STATUS_OBSOLETE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV1_STATUS,
						  $1);
				    $$ = SMI_STATUS_UNKNOWN;
				}
			    }
			    smiFree($1);
			}
        ;		

Status_Capabilities:	LOWERCASE_IDENTIFIER
			{
			    if (!strcmp($1, "current")) {
				$$ = SMI_STATUS_CURRENT;
			    } else if (!strcmp($1, "obsolete")) {
				$$ = SMI_STATUS_OBSOLETE;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_INVALID_CAPABILITIES_STATUS,
					      $1);
				$$ = SMI_STATUS_UNKNOWN;
			    }
			    smiFree($1);
			}
        ;

DisplayPart:		DISPLAY_HINT Text
			{
			    $$ = $2;
			}
        |		/* empty */
			{
			    $$ = NULL;
			}
        ;

UnitsPart:		UNITS Text
			{
			    $$ = $2;
			}
        |		/* empty */
			{
			    $$ = NULL;
			}
        ;

Access:			LOWERCASE_IDENTIFIER
			{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (!strcmp($1, "not-accessible")) {
				    $$ = SMI_ACCESS_NOT_ACCESSIBLE;
				} else if (!strcmp($1,
						   "accessible-for-notify")) {
				    $$ = SMI_ACCESS_NOTIFY;
				} else if (!strcmp($1, "read-only")) {
				    $$ = SMI_ACCESS_READ_ONLY;
				} else if (!strcmp($1, "read-write")) {
				    $$ = SMI_ACCESS_READ_WRITE;
				} else if (!strcmp($1, "read-create")) {
				    $$ = SMI_ACCESS_READ_WRITE;
				    thisParserPtr->flags |= FLAG_CREATABLE;
				    /* TODO:remember it's really read-create */
				} else if (!strcmp($1, "write-only")) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_WRITE_ONLY);
				    $$ = SMI_ACCESS_READ_WRITE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV2_ACCESS,
						  $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				}
			    } else {
				if (!strcmp($1, "not-accessible")) {
				    $$ = SMI_ACCESS_NOT_ACCESSIBLE;
				} else if (!strcmp($1, "read-only")) {
				    $$ = SMI_ACCESS_READ_ONLY;
				} else if (!strcmp($1, "read-write")) {
				    $$ = SMI_ACCESS_READ_WRITE;
				} else if (!strcmp($1, "write-only")) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV1_WRITE_ONLY);
				    $$ = SMI_ACCESS_READ_WRITE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV1_ACCESS,
						  $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				}
			    }
			    smiFree($1);
			}
        ;

IndexPart:		INDEX
			{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			}
			'{' IndexTypes '}'
			{
			    $$.indexkind = SMI_INDEX_INDEX;
			    $$.implied   = impliedFlag;
			    $$.listPtr   = $4;
			    $$.rowPtr    = NULL;
			}
        |		AUGMENTS '{' Entry '}'
			/* TODO: no AUGMENTS clause in v1 */
			/* TODO: how to differ INDEX and AUGMENTS ? */
			{
			    $$.indexkind    = SMI_INDEX_AUGMENT;
			    $$.implied      = 0;
			    $$.listPtr      = NULL;
			    $$.rowPtr       = $3;
			}
        |		/* empty */
			{
			    $$.indexkind = SMI_INDEX_UNKNOWN;
			}
	;

IndexTypes:		IndexType
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
        |		IndexTypes ',' IndexType
			/* TODO: might this list be emtpy? */
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

IndexType:		IMPLIED Index
			{
			    impliedFlag = 1;
			    $$ = $2;
			}
	|		Index
			{
			    $$ = $1;
			}
	;

Index:			ObjectName
			/*
			 * TODO: use the SYNTAX value of the correspondent
			 *       OBJECT-TYPE invocation
			 */
			{
			    $$ = $1;
			}
        ;

Entry:			ObjectName
			{
			    $$ = $1;
			}
        ;

DefValPart:		DEFVAL '{' Value '}'
			{ $$ = $3; }
	|		/* empty */
			{ $$ = NULL; }
			/* TODO: different for DefValPart in AgentCaps ? */
	;

Value:			valueofObjectSyntax
			{ $$ = $1; }
	|		'{' BitsValue '}'
			{
			    $$ = smiMalloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_BITS;
			    $$->value.ptr = (void *)$2;
			}
	;

BitsValue:		BitNames
			{ $$ = $1; }
	|		/* empty */
			{ $$ = NULL; }
	;

BitNames:		LOWERCASE_IDENTIFIER
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		BitNames ',' LOWERCASE_IDENTIFIER
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

ObjectName:		objectIdentifier
			{
			    $$ = $1;
			}
	;

NotificationName:	objectIdentifier
			{
			    $$ = $1;
			}
	;

ReferPart:		REFERENCE Text
			{ $$ = $2; }
	|		/* empty */
			{ $$ = NULL; }
	;

RevisionPart:		Revisions
			{ $$ = 0; }
	|		/* empty */
			{
			    if (!thisModulePtr->firstRevisionPtr) {
				addRevision(thisModulePtr->lastUpdated,
					    smiStrdup(
	           "[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }
			    $$ = 0;
			}
	;

Revisions:		Revision
			{ $$ = 0; }
	|		Revisions Revision
			{ $$ = 0; }
	;

Revision:		REVISION ExtUTCTime
			DESCRIPTION Text
			{
			    /*
			     * If the first REVISION (which is the newest)
			     * has another date than the LAST-UPDATED clause,
			     * we add an implicit Revision structure.
			     */
			    if ((!thisModulePtr->firstRevisionPtr) &&
				($2 != thisModulePtr->lastUpdated)) {
				addRevision(thisModulePtr->lastUpdated,
					    smiStrdup(
	           "[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }
			    
			    if (addRevision($2, $4, thisParserPtr))
				$$ = 0;
			    else
				$$ = -1;
			}
	;

ObjectsPart:		OBJECTS '{' Objects '}'
			{
			    $$ = $3;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

Objects:		Object
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		Objects ',' Object
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

Object:			ObjectName
			{
			    $$ = $1;
			}
	;

NotificationsPart:	NOTIFICATIONS '{' Notifications '}'
			{
			    $$ = $3;
			}
	;

Notifications:		Notification
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		Notifications ',' Notification
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

Notification:		NotificationName
			{
			    $$ = $1;
			}
	;

Text:			QUOTED_STRING
			{
			    $$ = smiStrdup($1);
			}
	;

ExtUTCTime:		QUOTED_STRING
			{
			    $$ = checkDate(thisParserPtr, $1);
			}
	;

objectIdentifier:	{
			    parentNodePtr = rootNodePtr;
			}
			subidentifiers
			{
			    $$ = $2;
			    parentNodePtr = $2->nodePtr;
			}
	;

subidentifiers:
			subidentifier
			{
			    $$ = $1;
			}
	|		subidentifiers
			subidentifier
			{
			    $$ = $2;
			}
        ;

subidentifier:
			/* LOWERCASE_IDENTIFIER */
			fuzzy_lowercase_identifier
			{
			    Object *objectPtr;
			    Import *importPtr;
			    
			    if (parentNodePtr != rootNodePtr) {
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_NOT_FIRST, $1);
			    }
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (objectPtr) {
				$$ = objectPtr;
			    } else {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    /*
				     * If we are in a MODULE-COMPLIANCE
				     * statement with a given MODULE...
				     */
				    if (complianceModulePtr) {
					objectPtr =
					    findObjectByModuleAndName(
						complianceModulePtr, $1);
					if (objectPtr) {
					    importPtr = addImport(
						smiStrdup($1),
						thisParserPtr);
					    setImportModulename(importPtr,
								complianceModulePtr->export.name);
					    importPtr->use++;
					} else {
					    objectPtr = addObject($1,
								  pendingNodePtr, 0,
								  FLAG_INCOMPLETE,
								  thisParserPtr);
					    smiPrintError(thisParserPtr,
							  ERR_IDENTIFIER_NOT_IN_MODULE, $1,
							  complianceModulePtr->export.name);
					}
				    } else if (capabilitiesModulePtr) {
					objectPtr =
					    findObjectByModuleAndName(
						capabilitiesModulePtr, $1);
					if (objectPtr) {
					    importPtr = addImport(
						smiStrdup($1),
						thisParserPtr);
					    setImportModulename(importPtr,
								capabilitiesModulePtr->
								export.name);
					    importPtr->use++;
					} else {
					    objectPtr = addObject($1,
								  pendingNodePtr, 0,
								  FLAG_INCOMPLETE,
								  thisParserPtr);
					    smiPrintError(thisParserPtr,
							  ERR_IDENTIFIER_NOT_IN_MODULE, $1,
							  capabilitiesModulePtr->export.name);
					}
				    } else {
					/* 
					 * forward referenced node.
					 * create it,
					 * marked with FLAG_INCOMPLETE.
					 */
					objectPtr = addObject($1,
							      pendingNodePtr,
							      0,
							      FLAG_INCOMPLETE,
							      thisParserPtr);
				    }
				    $$ = objectPtr;
				} else {
				    /*
				     * imported object.
				     */
				    importPtr->use++;
				    $$ = findObjectByModulenameAndName(
					importPtr->export.module, $1);
				}
			    }
			    if ($$)
				parentNodePtr = $$->nodePtr;
			}
	|		moduleName '.' LOWERCASE_IDENTIFIER
			{
			    Object *objectPtr;
			    Import *importPtr;
			    char *md;
			    
			    if (parentNodePtr != rootNodePtr) {
				md = smiMalloc(sizeof(char) *
					       (strlen($1) + strlen($3) + 2));
				sprintf(md, "%s.%s", $1, $3);
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_NOT_FIRST, md);
				smiFree(md);
			    } else {
				objectPtr = findObjectByModulenameAndName(
				    $1, $3);
				if (objectPtr) {
				    $$ = objectPtr;
				} else {
				    importPtr = findImportByModulenameAndName(
					$1, $3, thisModulePtr);
				    if (!importPtr ||
					(importPtr->kind == KIND_NOTFOUND)) {
					/* TODO: check: $1 == thisModule ? */
					/*
					 * If we are in a MODULE-COMPLIANCE
					 * statement with a given MODULE...
					 */
					if (complianceModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    complianceModulePtr, $1);
					    if (objectPtr) {
						importPtr = addImport(
						    smiStrdup($1),
						    thisParserPtr);
						setImportModulename(importPtr,
						    complianceModulePtr->export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject($1,
						    pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, $1,
					     complianceModulePtr->export.name);
					    }
					} else if (capabilitiesModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    capabilitiesModulePtr, $1);
					    if (objectPtr) {
						importPtr = addImport(
						    smiStrdup($1),
						    thisParserPtr);
						setImportModulename(importPtr,
						        capabilitiesModulePtr->
								  export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject($1,
						    pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, $1,
					   capabilitiesModulePtr->export.name);
					    }
					} else {
					    /* 
					     * forward referenced node.
					     * create it,
					     * marked with FLAG_INCOMPLETE.
					     */
					    objectPtr = addObject($1,
							    pendingNodePtr,
							      0,
							      FLAG_INCOMPLETE,
							      thisParserPtr);
					}
					$$ = objectPtr;
				    } else {
					/*
					 * imported object.
					 */
					importPtr->use++;
					$$ = findObjectByModulenameAndName(
					    importPtr->export.module, $1);
				    }
				}
				if ($$)
				    parentNodePtr = $$->nodePtr;
			    }
			}
	|		NUMBER
			{
			    Node *nodePtr;
			    Object *objectPtr;

			    nodePtr = findNodeByParentAndSubid(parentNodePtr,
							       $1);
			    if (nodePtr && nodePtr->lastObjectPtr &&
	       		(nodePtr->lastObjectPtr->modulePtr == thisModulePtr)) {
				/*
				 * hopefully, the last defined Object for
				 * this Node is the one we expect.
				 */
				$$ = nodePtr->lastObjectPtr;
			    } else {
				objectPtr = addObject(NULL,
						      parentNodePtr,
						      $1,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				$$ = objectPtr;
			    }
			    parentNodePtr = $$->nodePtr;
			}
	|		LOWERCASE_IDENTIFIER '(' NUMBER ')'
			{
			    Object *objectPtr;
			    
			    /* TODO: search in local module and
			     *       in imported modules
			     */
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (objectPtr) {
				smiPrintError(thisParserPtr,
					      ERR_EXISTENT_OBJECT, $1);
				$$ = objectPtr;
				if ($$->nodePtr->subid != $3) {
				    smiPrintError(thisParserPtr,
					  ERR_SUBIDENTIFIER_VS_OIDLABEL,
						  $3, $1);
				}
			    } else {
				objectPtr = addObject($1, parentNodePtr,
						      $3, 0,
						      thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_VALUEASSIGNMENT);
				$$ = objectPtr;
			    }
			    if ($$) 
				parentNodePtr = $$->nodePtr;
			}
	|		moduleName '.' LOWERCASE_IDENTIFIER '(' NUMBER ')'
			{
			    Object *objectPtr;
			    char *md;

			    md = smiMalloc(sizeof(char) *
					   (strlen($1) + strlen($3) + 2));
			    sprintf(md, "%s.%s", $1, $3);
			    objectPtr = findObjectByModulenameAndName($1, $3);
			    if (objectPtr) {
				smiPrintError(thisParserPtr,
					      ERR_EXISTENT_OBJECT, $1);
				$$ = objectPtr;
				if ($$->nodePtr->subid != $5) {
				    smiPrintError(thisParserPtr,
					  ERR_SUBIDENTIFIER_VS_OIDLABEL,
						  $5, md);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_ILLEGALLY_QUALIFIED, md);
				objectPtr = addObject($3, parentNodePtr,
						   $5, 0,
						   thisParserPtr);
				$$ = objectPtr;
			    }
			    smiFree(md);
			    if ($$)
				parentNodePtr = $$->nodePtr;
			}
	;

objectIdentifier_defval: subidentifiers_defval
			{ $$ = NULL; }
        ;		/* TODO */

subidentifiers_defval:	subidentifier_defval
			{ $$ = 0; }
	|		subidentifiers_defval subidentifier_defval
			{ $$ = 0; }
        ;		/* TODO */

subidentifier_defval:	LOWERCASE_IDENTIFIER '(' NUMBER ')'
			{ $$ = 0; }
	|		NUMBER
			{ $$ = 0; }
	;		/* TODO */

objectGroupClause:	LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			OBJECT_GROUP
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("OBJECT-GROUP",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "OBJECT-GROUP", "SNMPv2-CONF");
			    }
			}
			ObjectsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $13;

			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTGROUP);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $7);
			    setObjectDescription(objectPtr, $9, thisParserPtr);
			    if ($10) {
				setObjectReference(objectPtr, $10, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, $5);
			    $$ = 0;
			}
	;

notificationGroupClause: LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			NOTIFICATION_GROUP
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("NOTIFICATION-GROUP",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "NOTIFICATION-GROUP",
					      "SNMPv2-CONF");
			    }
			}
			NotificationsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $13;

			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONGROUP);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $7);
			    setObjectDescription(objectPtr, $9, thisParserPtr);
			    if ($10) {
				setObjectReference(objectPtr, $10, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, $5);
			    $$ = 0;
			}
	;

moduleComplianceClause:	LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			MODULE_COMPLIANCE
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("MODULE-COMPLIANCE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "MODULE-COMPLIANCE",
					      "SNMPv2-CONF");
			    }
			}
			STATUS Status
			DESCRIPTION Text
			ReferPart
			ComplianceModulePart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    Option *optionPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    
			    objectPtr = $13;

			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $6);
			    setObjectDescription(objectPtr, $8, thisParserPtr);
			    if ($9) {
				setObjectReference(objectPtr, $9, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, $10.mandatorylistPtr);
			    objectPtr->optionlistPtr = $10.optionlistPtr;
			    objectPtr->refinementlistPtr =
				                          $10.refinementlistPtr;

			    if ($10.optionlistPtr) {
				for (listPtr = $10.optionlistPtr;
				     listPtr;
				     listPtr = listPtr->nextPtr) {
				    optionPtr = ((Option *)(listPtr->ptr));
				    optionPtr->compliancePtr = objectPtr;
				}
			    }

			    /*
			     * Dirty: Fake the types' names in the
			     * refinement list:
			     * ``<compliancename>+<objecttypename>+type''
			     * ``<compliancename>+<objecttypename>+writetype''
			     */
			    if ($10.refinementlistPtr) {
				for (listPtr = $10.refinementlistPtr;
				     listPtr;
				     listPtr = listPtr->nextPtr) {
				    refinementPtr =
					((Refinement *)(listPtr->ptr));
				    refinementPtr->compliancePtr = objectPtr;
				}
			    }

			    $$ = 0;
			}
	;

ComplianceModulePart:	ComplianceModules
			{
			    $$ = $1;
			}
	;

ComplianceModules:	ComplianceModule
			{
			    $$ = $1;
			}
	|		ComplianceModules ComplianceModule
			{
			    List *listPtr;
			    
			    /* concatenate lists in $1 and $2 */
			    if ($1.mandatorylistPtr) {
				for (listPtr = $1.mandatorylistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.mandatorylistPtr;
				$$.mandatorylistPtr = $1.mandatorylistPtr;
			    } else {
				$$.mandatorylistPtr = $2.mandatorylistPtr;
			    }
			    if ($1.optionlistPtr) {
				for (listPtr = $1.optionlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.optionlistPtr;
				$$.optionlistPtr = $1.optionlistPtr;
			    } else {
				$$.optionlistPtr = $2.optionlistPtr;
			    }
			    if ($1.refinementlistPtr) {
				for (listPtr = $1.refinementlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.refinementlistPtr;
				$$.refinementlistPtr = $1.refinementlistPtr;
			    } else {
				$$.refinementlistPtr = $2.refinementlistPtr;
			    }
			}
	;

ComplianceModule:	MODULE ComplianceModuleName
			{
			    /*
			     * Remember the module. SMIv2 is broken by
			     * design to allow subsequent clauses to
			     * refer identifiers that are not
			     * imported.  Although, SMIv2 does not
			     * require, we will fake it by inserting
			     * appropriate imports.
			     */
			    if ($2 == thisModulePtr)
				complianceModulePtr = NULL;
			    else
				complianceModulePtr = $2;
			}
			MandatoryPart
			CompliancePart
			{
			    $$.mandatorylistPtr = $4;
			    $$.optionlistPtr = $5.optionlistPtr;
			    $$.refinementlistPtr = $5.refinementlistPtr;
			    if (complianceModulePtr) {
				checkImports(complianceModulePtr,
					     thisParserPtr);
				complianceModulePtr = NULL;
			    }
			}
	;

ComplianceModuleName:	UPPERCASE_IDENTIFIER objectIdentifier
			{
			    $$ = findModuleByName($1);
			    /* TODO: handle objectIdentifier */
			    if (!$$) {
				$$ = loadModule($1);
			    }
			}
	|		UPPERCASE_IDENTIFIER
			{
			    $$ = findModuleByName($1);
			    if (!$$) {
				$$ = loadModule($1);
			    }
			}
	|		/* empty, only if contained in MIB module */
			/* TODO: RFC 1904 looks a bit different, is this ok? */
			{
			    $$ = thisModulePtr;
			}
	;

MandatoryPart:		MANDATORY_GROUPS '{' MandatoryGroups '}'
			{
			    $$ = $3;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

MandatoryGroups:	MandatoryGroup
			{
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		MandatoryGroups ',' MandatoryGroup
			{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

MandatoryGroup:		objectIdentifier
			{
			    /* TODO: check that objectIdentifier is
			       found, is defined in complianceModulePtr,
			       and is a group node. */
			    Import *importPtr;

			    $$ = $1;
			    if (complianceModulePtr) {
				$$ = findObjectByModuleAndName(
				                           complianceModulePtr,
							   $1->export.name);
			    }
			    if (complianceModulePtr && $1->export.name) {
				importPtr = findImportByModulenameAndName(
				    complianceModulePtr->export.name,
				    $1->export.name, thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }
			}
	;

CompliancePart:		Compliances
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = $1.optionlistPtr;
			    $$.refinementlistPtr = $1.refinementlistPtr;
			}
	|		/* empty */
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = NULL;
			    $$.refinementlistPtr = NULL;
			}
	;

Compliances:		Compliance
			{
			    $$ = $1;
			}
	|		Compliances Compliance
			{
			    List *listPtr;
			    int stop;
			    
			    $$.mandatorylistPtr = NULL;

			    /* check for duplicates in optionlist */
			    stop = 0;
			    if ($2.optionlistPtr) {
				for (listPtr = $1.optionlistPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    if (((Option *)listPtr->ptr)->objectPtr ==
					((Option *)$2.optionlistPtr->ptr)->objectPtr) {
					smiPrintError(thisParserPtr,
						      ERR_OPTIONALGROUP_ALREADY_EXISTS,
						      ((Option *)$2.optionlistPtr->ptr)->objectPtr->export.name);
					stop = 1;
					$$.optionlistPtr = $1.optionlistPtr;
				    }
				}
			    }
	
                            /* concatenate optionlists */
			    if ($1.optionlistPtr) {
				for (listPtr = $1.optionlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				if (!stop) {
				    listPtr->nextPtr = $2.optionlistPtr;
				}
				$$.optionlistPtr = $1.optionlistPtr;
			    } else {
				$$.optionlistPtr = $2.optionlistPtr;
			    }

			    /* check for duplicates in refinementlist */
			    stop = 0;
			    if ($2.refinementlistPtr) {
				for (listPtr = $1.refinementlistPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    if (((Refinement *)listPtr->ptr)->objectPtr ==
					((Refinement *)$2.refinementlistPtr->ptr)->objectPtr) {
					smiPrintError(thisParserPtr,
						      ERR_REFINEMENT_ALREADY_EXISTS,
						      ((Refinement *)$2.refinementlistPtr->ptr)->objectPtr->export.name);
					stop = 1;
					$$.refinementlistPtr = $1.refinementlistPtr;
				    }
				}
			    }
			    
                            /* concatenate refinementlists */
			    if ($1.refinementlistPtr) {
				for (listPtr = $1.refinementlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				if (!stop) {
				    listPtr->nextPtr = $2.refinementlistPtr;
				}
				$$.refinementlistPtr = $1.refinementlistPtr;
			    } else {
				$$.refinementlistPtr = $2.refinementlistPtr;
			    }
			}
	;

Compliance:		ComplianceGroup
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = $1;
			    $$.refinementlistPtr = NULL;
			}
	|		ComplianceObject
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = NULL;
			    $$.refinementlistPtr = $1;
			}
	;

ComplianceGroup:	GROUP objectIdentifier
			DESCRIPTION Text
			{
			    Import *importPtr;
			    
			    if (complianceModulePtr && $2->export.name) {
				importPtr = findImportByModulenameAndName(
						    complianceModulePtr->export.name,
						    $2->export.name,
						    thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }
			    
			    $$ = smiMalloc(sizeof(List));
			    $$->nextPtr = NULL;
			    $$->ptr = smiMalloc(sizeof(Option));
			    ((Option *)($$->ptr))->objectPtr = $2;
			    if (! (thisModulePtr->flags & SMI_FLAG_NODESCR)) {
				((Option *)($$->ptr))->export.description = $4;
			    }
			}
	;

ComplianceObject:	OBJECT ObjectName
			SyntaxPart
			WriteSyntaxPart
			AccessPart
			DESCRIPTION Text
			{
			    Import *importPtr;

			    if (complianceModulePtr && $2->export.name) {
				importPtr = findImportByModulenameAndName(
						    complianceModulePtr->export.name,
						    $2->export.name,
						    thisModulePtr);
				if (importPtr) 
				    importPtr->use++;
			    }
			    
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    $$ = smiMalloc(sizeof(List));
			    $$->nextPtr = NULL;
			    $$->ptr = smiMalloc(sizeof(Refinement));
			    ((Refinement *)($$->ptr))->objectPtr = $2;
			    ((Refinement *)($$->ptr))->typePtr = $3;
			    ((Refinement *)($$->ptr))->writetypePtr = $4;
			    ((Refinement *)($$->ptr))->export.access = $5;
			    if (! (thisParserPtr->flags & SMI_FLAG_NODESCR)) {
				((Refinement *)($$->ptr))->export.description = $7;
			    }
			}
	;

SyntaxPart:		SYNTAX Syntax
			{
			    if ($2->export.name) {
				$$ = duplicateType($2, 0, thisParserPtr);
			    } else {
				$$ = $2;
			    }
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

WriteSyntaxPart:	WRITE_SYNTAX WriteSyntax
			{
			    if ($2->export.name) {
				$$ = duplicateType($2, 0, thisParserPtr);
			    } else {
				$$ = $2;
			    }
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

WriteSyntax:		Syntax
			{
			    $$ = $1;
			}
	;

AccessPart:		MIN_ACCESS Access
			{
			    $$ = $2;
			}
	|		/* empty */
			{
			    $$ = SMI_ACCESS_UNKNOWN;
			}
	;

agentCapabilitiesClause: LOWERCASE_IDENTIFIER
			{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, $1,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, $1);
			}
			AGENT_CAPABILITIES
			{
			    Import *importPtr;
			    
			    importPtr = findImportByName("AGENT-CAPABILITIES",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "AGENT-CAPABILITIES",
					      "SNMPv2-CONF");
			    }
			}
			PRODUCT_RELEASE Text
			STATUS Status_Capabilities
			DESCRIPTION Text
			ReferPart
			ModulePart_Capabilities
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $15;
			    
			    smiCheckObjectReuse(thisParserPtr, $1, &objectPtr);

			    setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $8);
			    setObjectDescription(objectPtr, $10, thisParserPtr);
			    if ($11) {
				setObjectReference(objectPtr, $11, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
				/*
				 * TODO: PRODUCT_RELEASE Text
				 * TODO: ModulePart_Capabilities
				 */
			    $$ = 0;
			}
	;

ModulePart_Capabilities: Modules_Capabilities
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Modules_Capabilities:	Module_Capabilities
			{ $$ = 0; }
	|		Modules_Capabilities Module_Capabilities
			{ $$ = 0; }
	;

Module_Capabilities:	SUPPORTS ModuleName_Capabilities
			{
			    /*
			     * Remember the module. SMIv2 is broken by
			     * design to allow subsequent clauses to
			     * refer identifiers that are not
			     * imported.  Although, SMIv2 does not
			     * require, we will fake it by inserting
			     * appropriate imports.
			     */
			    if ($2 == thisModulePtr)
				capabilitiesModulePtr = NULL;
			    else
				capabilitiesModulePtr = $2;
			}
			INCLUDES '{' CapabilitiesGroups '}'
			VariationPart
			{
			    if (capabilitiesModulePtr) {
				checkImports(capabilitiesModulePtr,
					     thisParserPtr);
				capabilitiesModulePtr = NULL;
			    }
			    $$ = 0;
			}
	;

CapabilitiesGroups:	CapabilitiesGroup
			{
#if 0
			    $$ = smiMalloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
#else
			    $$ = NULL;
#endif
			}
	|		CapabilitiesGroups ',' CapabilitiesGroup
			{
#if 0
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
#else
			    $$ = NULL;
#endif
			}
	;

CapabilitiesGroup:	objectIdentifier
			{
			    $$ = NULL;
			}
	;

ModuleName_Capabilities: UPPERCASE_IDENTIFIER objectIdentifier
			{
			    $$ = findModuleByName($1);
			    /* TODO: handle objectIdentifier */
			    if (!$$) {
				$$ = loadModule($1);
			    }
			}
	|		UPPERCASE_IDENTIFIER
			{
			    $$ = findModuleByName($1);
			    if (!$$) {
				$$ = loadModule($1);
			    }
			}
	;

VariationPart:		Variations
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Variations:		Variation
			{ $$ = 0; }
	|		Variations Variation
			{ $$ = 0; }
	;

Variation:		VARIATION ObjectName
			{
			    if ($2) {
				variationkind = $2->export.nodekind;
			    } else {
				variationkind = SMI_NODEKIND_UNKNOWN;
			    }
			}
			SyntaxPart
			{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_SYNTAX);
			    }
			}
			WriteSyntaxPart
			{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_WRITESYNTAX);
			    }
			}
			VariationAccessPart
			CreationPart
			{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_CREATION);
			    }
			}
			DefValPart
			{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_DEFVAL);
			    }
			}
			DESCRIPTION Text
			{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    $$ = 0;
			    variationkind = SMI_NODEKIND_UNKNOWN;
			}
	;

VariationAccessPart:	ACCESS VariationAccess
			{ $$ = $2; }
	|		/* empty */
			{ $$ = 0; }
	;

VariationAccess:	LOWERCASE_IDENTIFIER
			{
			    if (!strcmp($1, "not-implemented")) {
				$$ = SMI_ACCESS_NOT_IMPLEMENTED;
			    } else if (!strcmp($1, "accessible-for-notify")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				} else {
				    $$ = SMI_ACCESS_NOTIFY;
				}
			    } else if (!strcmp($1, "read-only")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				} else {
				    $$ = SMI_ACCESS_READ_ONLY;
				}
			    } else if (!strcmp($1, "read-write")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				} else {
				    $$ = SMI_ACCESS_READ_WRITE;
				}
			    } else if (!strcmp($1, "read-create")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				} else {
				    $$ = SMI_ACCESS_READ_WRITE;
				}
			    } else if (!strcmp($1, "write-only")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				} else {
				    $$ = SMI_ACCESS_READ_WRITE; /* TODO */
				    smiPrintError(thisParserPtr,
					       ERR_SMIV2_WRITE_ONLY);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_INVALID_VARIATION_ACCESS,
					      $1);
				$$ = SMI_ACCESS_UNKNOWN;
			    }
			}
        ;

CreationPart:		CREATION_REQUIRES '{' Cells '}'
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Cells:			Cell
			{ $$ = 0; }
	|		Cells ',' Cell
			{ $$ = 0; }
	;

Cell:			ObjectName
			{ $$ = 0; }
	;

%%

#endif
