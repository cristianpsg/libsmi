
/*  A Bison parser, made from parser-smi.y
 by  GNU Bison version 1.27
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse smiparse
#define yylex smilex
#define yyerror smierror
#define yylval smilval
#define yychar smichar
#define yydebug smidebug
#define yynerrs sminerrs
#define	DOT_DOT	257
#define	COLON_COLON_EQUAL	258
#define	UPPERCASE_IDENTIFIER	259
#define	LOWERCASE_IDENTIFIER	260
#define	NUMBER	261
#define	NEGATIVENUMBER	262
#define	BIN_STRING	263
#define	HEX_STRING	264
#define	QUOTED_STRING	265
#define	ACCESS	266
#define	AGENT_CAPABILITIES	267
#define	APPLICATION	268
#define	AUGMENTS	269
#define	BEGIN_	270
#define	BITS	271
#define	CHOICE	272
#define	CONTACT_INFO	273
#define	CREATION_REQUIRES	274
#define	COUNTER32	275
#define	COUNTER64	276
#define	DEFINITIONS	277
#define	DEFVAL	278
#define	DESCRIPTION	279
#define	DISPLAY_HINT	280
#define	END	281
#define	ENTERPRISE	282
#define	EXPORTS	283
#define	FROM	284
#define	GROUP	285
#define	GAUGE32	286
#define	IDENTIFIER	287
#define	IMPLICIT	288
#define	IMPLIED	289
#define	IMPORTS	290
#define	INCLUDES	291
#define	INDEX	292
#define	INTEGER	293
#define	INTEGER32	294
#define	IPADDRESS	295
#define	LAST_UPDATED	296
#define	MACRO	297
#define	MANDATORY_GROUPS	298
#define	MAX_ACCESS	299
#define	MIN_ACCESS	300
#define	MODULE	301
#define	MODULE_COMPLIANCE	302
#define	MODULE_IDENTITY	303
#define	NOTIFICATIONS	304
#define	NOTIFICATION_GROUP	305
#define	NOTIFICATION_TYPE	306
#define	OBJECT	307
#define	OBJECT_GROUP	308
#define	OBJECT_IDENTITY	309
#define	OBJECT_TYPE	310
#define	OBJECTS	311
#define	OCTET	312
#define	OF	313
#define	ORGANIZATION	314
#define	OPAQUE	315
#define	PRODUCT_RELEASE	316
#define	REFERENCE	317
#define	REVISION	318
#define	SEQUENCE	319
#define	SIZE	320
#define	STATUS	321
#define	STRING	322
#define	SUPPORTS	323
#define	SYNTAX	324
#define	TEXTUAL_CONVENTION	325
#define	TIMETICKS	326
#define	TRAP_TYPE	327
#define	UNITS	328
#define	UNIVERSAL	329
#define	UNSIGNED32	330
#define	VARIABLES	331
#define	VARIATION	332
#define	WRITE_SYNTAX	333

#line 14 "parser-smi.y"


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

#include "smi.h"
#include "error.h"
#include "parser-smi.h"
#include "scanner-smi.h"
#include "data.h"
#include "check.h"
#include "util.h"
    
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif



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

 

static char *convertImportv2[] = {
    "RFC1155-SMI", "internet",	    "SNMPv2-SMI", "internet",
    "RFC1155-SMI", "directory",	    "SNMPv2-SMI", "directory",
    "RFC1155-SMI", "mgmt",	    "SNMPv2-SMI", "mgmt",
    "RFC1155-SMI", "experimental",  "SNMPv2-SMI", "experimental",
    "RFC1155-SMI", "private",	    "SNMPv2-SMI", "private",
    "RFC1155-SMI", "enterprises",   "SNMPv2-SMI", "enterprises",
    "RFC1155-SMI", "IpAddress",     "SNMPv2-SMI", "IpAddress",
    "RFC1155-SMI", "Counter",       "SNMPv2-SMI", "Counter32",
    "RFC1155-SMI", "Gauge",         "SNMPv2-SMI", "Gauge32",
    "RFC1155-SMI", "TimeTicks",     "SNMPv2-SMI", "TimeTicks",
    "RFC1155-SMI", "Opaque",        "SNMPv2-SMI", "Opaque",
    "RFC1065-SMI", "internet",	    "SNMPv2-SMI", "internet",
    "RFC1065-SMI", "directory",	    "SNMPv2-SMI", "directory",
    "RFC1065-SMI", "mgmt",	    "SNMPv2-SMI", "mgmt",
    "RFC1065-SMI", "experimental",  "SNMPv2-SMI", "experimental",
    "RFC1065-SMI", "private",	    "SNMPv2-SMI", "private",
    "RFC1065-SMI", "enterprises",   "SNMPv2-SMI", "enterprises",
    "RFC1065-SMI", "IpAddress",     "SNMPv2-SMI", "IpAddress",
    "RFC1065-SMI", "Counter",       "SNMPv2-SMI", "Counter32",
    "RFC1065-SMI", "Gauge",         "SNMPv2-SMI", "Gauge32",
    "RFC1065-SMI", "TimeTicks",     "SNMPv2-SMI", "TimeTicks",
    "RFC1065-SMI", "Opaque",        "SNMPv2-SMI", "Opaque",
    "RFC1213-MIB", "mib-2",         "SNMPv2-SMI", "mib-2",    
    "RFC1213-MIB", "DisplayString", "SNMPv2-TC",  "DisplayString",    
    NULL, NULL, NULL, NULL
};
 

 
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
	 * Check whether compliance statements contain out of date
	 * groups or objects.
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_COMPLIANCE) {
	    smiCheckComplianceStatus(parserPtr, objectPtr);
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
	    } else {
		Object *parentPtr;

		if (objectPtr->nodePtr->parentPtr &&
		    objectPtr->nodePtr->parentPtr->lastObjectPtr) {
		    parentPtr = objectPtr->nodePtr->parentPtr->lastObjectPtr;
		} else {
		    parentPtr = NULL;
		}

		/*
		 * This misreports some cases where the table name
		 * does not have the "*Table" suffix.  This is trying
		 * to allow Entry names of either fooTableEntry or
		 * fooEntry.
		 */
		if (parentPtr &&
		    !((strlen(parentPtr->export.name) == len ||
		       strlen(parentPtr->export.name) == len - 5) &&
		      !strncmp(objectPtr->export.name, parentPtr->export.name,
			len - 5))) {
		    smiPrintErrorAtLine(parserPtr, ERR_ROWNAME_TABLENAME,
					objectPtr->line,
					objectPtr->export.name,
					parentPtr->export.name);
		}
	    }
	}

	/*
	 * Check whether a row's SEQUENCE contains exactly the list
	 * of child nodes (columns).  An unknown SEQUENCE type
	 * is handled later.
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_ROW &&
	    ((objectPtr->typePtr->flags & FLAG_INCOMPLETE) == 0)) {
	    List *p;
	    Node *seqNodePtr, *childNodePtr;
	    Object *colPtr;
	    int i;
	    
	    /*
	     * Walk through the SEQUENCE elements and find those
	     * that are misordered or have no matching columnar object.
	     */
	    for (p = objectPtr->typePtr->listPtr, i = 1,
		     childNodePtr = objectPtr->nodePtr->firstChildPtr;
		 p && childNodePtr;
		 p = p->nextPtr, childNodePtr = childNodePtr->nextPtr, i++) {
		seqNodePtr = ((Object *)p->ptr)->nodePtr;
		
		/* unknown OIDs are handled later */
		if (childNodePtr->flags & FLAG_INCOMPLETE) {
		    continue;
		}

		if (seqNodePtr->parentPtr != childNodePtr->parentPtr) {
		    smiPrintErrorAtLine(parserPtr, ERR_SEQUENCE_NO_COLUMN,
					objectPtr->typePtr->line,
					i,
					((Object *)p->ptr)->export.name,
					objectPtr->export.name);
		    continue;
		}

		if (seqNodePtr != childNodePtr) {
		    smiPrintErrorAtLine(parserPtr, ERR_SEQUENCE_ORDER,
					objectPtr->typePtr->line,
					i,
					((Object *)p->ptr)->export.name,
					objectPtr->export.name);
		    break;
		}
	    }
	    if ((p != NULL) && (childNodePtr == NULL)) {
		smiPrintErrorAtLine(parserPtr, ERR_SEQUENCE_NO_COLUMN,
				    objectPtr->typePtr->line,
				    i, 
				    ((Object *)p->ptr)->export.name,
				    objectPtr->export.name);
	    }

	    /*
	     * Walk through all child objects and find those
	     * that were missing in the SEQUENCE.
	     */
	    for (childNodePtr = objectPtr->nodePtr->firstChildPtr;
		 childNodePtr; childNodePtr = childNodePtr->nextPtr) {
		colPtr = findObjectByModuleAndNode(modulePtr, childNodePtr);
		if (!colPtr) continue;
		for (p = objectPtr->typePtr->listPtr; p; p = p->nextPtr) {
		    if (((Object *)p->ptr)->nodePtr == colPtr->nodePtr)
			break;
		}
		if (!p) {
		    smiPrintErrorAtLine(parserPtr, ERR_SEQUENCE_MISSING_COLUMN,
					objectPtr->typePtr->line,
					objectPtr->typePtr->export.name,
					colPtr->export.name);
		}
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
		     nodePtr->parentPtr != smiHandle->pendingNodePtr &&
			 nodePtr->parentPtr != smiHandle->rootNodePtr &&
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
	    if (objectPtr->nodePtr->oidlen < modulePtr->prefixNodePtr->oidlen) {
		Node *nodePtr = findNodeByOid(objectPtr->nodePtr->oidlen,
					      modulePtr->prefixNodePtr->oid);
		if (nodePtr)
		    modulePtr->prefixNodePtr = nodePtr;
		else
		    smiPrintError(parserPtr, ERR_OTHER_ERROR,
				  "Failed to create complete object tree - "
				  "expect incorrect output");
	    }
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
		    smiFree(objectPtr->export.value.value.ptr);
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
				(char *)objectPtr->export.value.value.ptr)) {
			smiFree(objectPtr->export.value.value.ptr);
			objectPtr->export.value.value.integer32 =
			    ((NamedNumber *)(listPtr->ptr))->
			    export.value.value.integer32;
			objectPtr->export.value.len = 1;
			break;
		    }
		}
		if (objectPtr->export.value.len == -1) {
		    smiPrintErrorAtLine(parserPtr,
					ERR_DEFVAL_SYNTAX, objectPtr->line);
		}
	    }
	}

	smiCheckDefault(parserPtr, objectPtr);
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

	    if (modulePtr->export.language == SMI_LANGUAGE_SMIV2) {
		int j;
		for (j = 0; convertImportv2[j]; j += 4) {
		    if ((strcmp(convertImportv2[j],
				importPtr->export.module) == 0)
			&& (strcmp(convertImportv2[j+1],
				   importPtr->export.name) == 0)) {
			smiPrintErrorAtLine(parserPtr,
					    ERR_OBSOLETE_IMPORT,
					    importPtr->line,
					    importPtr->export.name,
					    convertImportv2[j+2],
					    importPtr->export.module);
		    }
		}
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
    

#line 855 "parser-smi.y"
typedef union {
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
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		602
#define	YYFLAG		-32768
#define	YYNTBASE	90

#define YYTRANSLATE(x) ((unsigned)(x) <= 333 ? yytranslate[x] : 278)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    84,
    85,     2,     2,    83,     2,    88,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    82,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    86,     2,    87,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    80,    89,    81,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    77,    78,    79
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     3,     5,     8,     9,    20,    24,    25,    27,
    28,    32,    33,    34,    38,    40,    41,    43,    46,    50,
    52,    56,    58,    60,    62,    64,    66,    68,    70,    72,
    74,    76,    78,    80,    82,    84,    86,    88,    90,    92,
    94,    96,    98,   100,   102,   104,   106,   107,   109,   112,
   114,   116,   118,   120,   122,   124,   126,   128,   130,   132,
   134,   136,   139,   140,   141,   147,   149,   151,   153,   155,
   157,   159,   161,   163,   165,   167,   168,   172,   174,   176,
   177,   186,   187,   192,   194,   196,   198,   200,   202,   204,
   206,   208,   210,   212,   214,   215,   216,   228,   230,   234,
   236,   241,   243,   247,   250,   252,   257,   259,   261,   264,
   266,   270,   271,   277,   278,   279,   280,   295,   296,   297,
   316,   317,   320,   321,   322,   334,   339,   340,   342,   346,
   348,   351,   352,   353,   357,   358,   362,   363,   364,   365,
   381,   382,   383,   384,   385,   386,   387,   409,   411,   414,
   416,   418,   420,   422,   428,   434,   436,   438,   440,   442,
   443,   447,   448,   452,   454,   455,   459,   460,   464,   469,
   472,   477,   480,   481,   486,   489,   494,   497,   499,   501,
   503,   505,   507,   509,   513,   516,   519,   523,   526,   528,
   530,   532,   535,   537,   540,   542,   544,   547,   549,   551,
   553,   556,   559,   561,   563,   565,   567,   569,   571,   572,
   576,   583,   585,   589,   591,   595,   597,   599,   601,   603,
   607,   609,   613,   614,   620,   622,   624,   626,   628,   631,
   632,   635,   636,   638,   639,   645,   650,   651,   653,   657,
   660,   662,   664,   666,   671,   672,   674,   678,   680,   681,
   683,   687,   689,   691,   694,   695,   697,   698,   700,   703,
   708,   713,   714,   716,   720,   722,   727,   729,   733,   735,
   737,   739,   740,   743,   745,   748,   750,   754,   756,   761,
   768,   770,   772,   775,   780,   782,   783,   784,   785,   801,
   802,   803,   804,   820,   821,   822,   823,   839,   841,   843,
   846,   847,   853,   856,   858,   859,   864,   865,   867,   871,
   873,   875,   876,   878,   881,   883,   885,   890,   898,   901,
   902,   905,   906,   908,   911,   912,   913,   914,   915,   933,
   935,   936,   938,   941,   942,   951,   953,   957,   959,   962,
   964,   966,   967,   969,   972,   973,   974,   975,   976,   977,
   992,   995,   996,   998,  1003,  1004,  1006,  1010
};

static const short yyrhs[] = {    91,
     0,     0,    92,     0,    91,    92,     0,     0,   105,    93,
    94,    23,     4,    16,    97,    95,   106,    27,     0,    80,
   218,    81,     0,     0,    96,     0,     0,    36,    99,    82,
     0,     0,     0,    29,    98,    82,     0,   100,     0,     0,
   101,     0,   100,   101,     0,   102,    30,   105,     0,   103,
     0,   102,    83,   103,     0,     6,     0,     5,     0,   104,
     0,    13,     0,    17,     0,    21,     0,    22,     0,    32,
     0,    40,     0,    41,     0,    44,     0,    48,     0,    49,
     0,    51,     0,    52,     0,    54,     0,    55,     0,    56,
     0,    61,     0,    71,     0,    72,     0,    73,     0,    76,
     0,     5,     0,   107,     0,     0,   108,     0,   107,   108,
     0,   118,     0,   116,     0,   135,     0,   139,     0,   143,
     0,   153,     0,   157,     0,   233,     0,   225,     0,   229,
     0,   254,     0,   109,     0,     1,    81,     0,     0,     0,
   112,   110,    43,   111,    27,     0,    49,     0,    56,     0,
    73,     0,    52,     0,    55,     0,    71,     0,    54,     0,
    51,     0,    48,     0,    13,     0,     0,    18,   114,    81,
     0,     6,     0,     5,     0,     0,   115,   117,    53,    33,
     4,    80,   218,    81,     0,     0,   120,   119,     4,   122,
     0,     5,     0,   121,     0,    41,     0,    72,     0,    61,
     0,    40,     0,    21,     0,    32,     0,    76,     0,    22,
     0,   130,     0,     0,     0,    71,   123,   191,    67,   189,
    25,   216,   124,   206,    70,   130,     0,   113,     0,    65,
    59,   126,     0,     5,     0,    65,    80,   128,    81,     0,
   129,     0,   128,    83,   129,     0,     6,   131,     0,   164,
     0,    17,    80,   132,    81,     0,   166,     0,    17,     0,
     5,   178,     0,   133,     0,   132,    83,   133,     0,     0,
     6,   134,    84,     7,    85,     0,     0,     0,     0,     6,
   136,    55,   137,    67,   189,    25,   216,   138,   206,     4,
    80,   218,    81,     0,     0,     0,     6,   140,    56,   141,
    70,   130,   192,   150,    67,   189,   142,   206,   194,   200,
     4,    80,   204,    81,     0,     0,    25,   216,     0,     0,
     0,   115,   144,    73,   145,    28,   218,   146,   149,   206,
     4,     7,     0,    77,    80,   147,    81,     0,     0,   148,
     0,   147,    83,   148,     0,   204,     0,    25,   216,     0,
     0,     0,    45,   151,   193,     0,     0,    12,   152,   193,
     0,     0,     0,     0,     6,   154,    52,   155,   210,    67,
   189,    25,   216,   156,   206,     4,    80,   205,    81,     0,
     0,     0,     0,     0,     0,     0,     6,   158,    49,   159,
    42,   217,   160,    60,   216,   161,    19,   216,   162,    25,
   216,   163,   207,     4,    80,   218,    81,     0,   168,     0,
   165,   168,     0,   125,     0,   126,     0,   127,     0,   176,
     0,    86,    14,     7,    87,    34,     0,    86,    75,     7,
    87,    34,     0,   175,     0,   177,     0,   174,     0,    39,
     0,     0,    39,   169,   179,     0,     0,    39,   170,   184,
     0,    40,     0,     0,    40,   171,   179,     0,     0,     5,
   172,   184,     0,   105,    88,     5,   184,     0,     5,   179,
     0,   105,    88,     5,   179,     0,    58,    68,     0,     0,
    58,    68,   173,   180,     0,     5,   180,     0,   105,    88,
     5,   180,     0,    53,    33,     0,     7,     0,     8,     0,
     9,     0,    10,     0,     6,     0,    11,     0,    80,   222,
    81,     0,    39,   178,     0,    40,   178,     0,    58,    68,
   178,     0,    53,    33,     0,    41,     0,    21,     0,    32,
     0,    32,   179,     0,    76,     0,    76,   179,     0,    72,
     0,    61,     0,    61,   180,     0,    22,     0,    41,     0,
    21,     0,    32,   178,     0,    76,   178,     0,    72,     0,
    61,     0,    22,     0,   179,     0,   180,     0,   184,     0,
     0,    84,   181,    85,     0,    84,    66,    84,   181,    85,
    85,     0,   182,     0,   181,    89,   182,     0,   183,     0,
   183,     3,   183,     0,     8,     0,     7,     0,    10,     0,
     9,     0,    80,   185,    81,     0,   186,     0,   185,    83,
   186,     0,     0,     6,   187,    84,   188,    85,     0,     7,
     0,     8,     0,     6,     0,     6,     0,    26,   216,     0,
     0,    74,   216,     0,     0,     6,     0,     0,    38,   195,
    80,   196,    81,     0,    15,    80,   199,    81,     0,     0,
   197,     0,   196,    83,   197,     0,    35,   198,     0,   198,
     0,   204,     0,   204,     0,    24,    80,   201,    81,     0,
     0,   167,     0,    80,   202,    81,     0,   203,     0,     0,
     6,     0,   203,    83,     6,     0,   218,     0,   218,     0,
    63,   216,     0,     0,   208,     0,     0,   209,     0,   208,
   209,     0,    64,   217,    25,   216,     0,    57,    80,   211,
    81,     0,     0,   212,     0,   211,    83,   212,     0,   204,
     0,    50,    80,   214,    81,     0,   215,     0,   214,    83,
   215,     0,   205,     0,    11,     0,    11,     0,     0,   219,
   220,     0,   221,     0,   220,   221,     0,   115,     0,   105,
    88,     6,     0,     7,     0,     6,    84,     7,    85,     0,
   105,    88,     6,    84,     7,    85,     0,   223,     0,   224,
     0,   223,   224,     0,     6,    84,     7,    85,     0,     7,
     0,     0,     0,     0,     6,   226,    54,   227,   210,    67,
   189,    25,   216,   228,   206,     4,    80,   218,    81,     0,
     0,     0,     0,     6,   230,    51,   231,   213,    67,   189,
    25,   216,   232,   206,     4,    80,   218,    81,     0,     0,
     0,     0,     6,   234,    48,   235,    67,   189,    25,   216,
   236,   206,   237,     4,    80,   218,    81,     0,   238,     0,
   239,     0,   238,   239,     0,     0,    47,   241,   240,   242,
   245,     0,     5,   218,     0,     5,     0,     0,    44,    80,
   243,    81,     0,     0,   244,     0,   243,    83,   244,     0,
   218,     0,   246,     0,     0,   247,     0,   246,   247,     0,
   248,     0,   249,     0,    31,   218,    25,   216,     0,    53,
   204,   250,   251,   253,    25,   216,     0,    70,   130,     0,
     0,    79,   252,     0,     0,   130,     0,    46,   193,     0,
     0,     0,     0,     0,     6,   255,    13,   256,    62,   216,
    67,   190,    25,   216,   257,   206,   258,     4,    80,   218,
    81,     0,   259,     0,     0,   260,     0,   259,   260,     0,
     0,    69,   264,   261,    37,    80,   262,    81,   265,     0,
   263,     0,   262,    83,   263,     0,   218,     0,     5,   218,
     0,     5,     0,   266,     0,     0,   267,     0,   266,   267,
     0,     0,     0,     0,     0,     0,    78,   204,   268,   250,
   269,   251,   270,   273,   275,   271,   200,   272,    25,   216,
     0,    12,   274,     0,     0,     6,     0,    20,    80,   276,
    81,     0,     0,   277,     0,   276,    83,   277,     0,   204,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
  1110,  1115,  1121,  1123,  1131,  1169,  1190,  1192,  1199,  1201,
  1205,  1217,  1219,  1232,  1235,  1237,  1242,  1244,  1248,  1290,
  1292,  1300,  1306,  1312,  1328,  1329,  1330,  1331,  1332,  1333,
  1334,  1335,  1336,  1337,  1338,  1339,  1340,  1341,  1342,  1343,
  1344,  1345,  1346,  1347,  1350,  1362,  1364,  1369,  1371,  1375,
  1380,  1385,  1390,  1395,  1400,  1405,  1410,  1415,  1420,  1425,
  1430,  1435,  1449,  1461,  1485,  1490,  1491,  1492,  1493,  1494,
  1495,  1496,  1497,  1498,  1499,  1502,  1521,  1530,  1534,  1550,
  1569,  1589,  1596,  1714,  1718,  1742,  1743,  1744,  1745,  1746,
  1747,  1748,  1749,  1752,  1768,  1790,  1799,  1830,  1838,  1852,
  1897,  1905,  1911,  1931,  1966,  1972,  1992,  1997,  2002,  2039,
  2045,  2058,  2071,  2093,  2103,  2119,  2128,  2151,  2161,  2181,
  2291,  2300,  2310,  2320,  2338,  2390,  2394,  2400,  2406,  2419,
  2425,  2433,  2437,  2446,  2447,  2456,  2459,  2469,  2484,  2493,
  2519,  2529,  2553,  2557,  2564,  2571,  2579,  2610,  2614,  2632,
  2637,  2642,  2647,  2664,  2666,  2674,  2679,  2696,  2705,  2717,
  2722,  2735,  2740,  2749,  2767,  2772,  2790,  2795,  2843,  2889,
  2944,  2994,  2999,  3004,  3011,  3057,  3098,  3105,  3112,  3118,
  3144,  3167,  3187,  3204,  3232,  3236,  3254,  3258,  3264,  3272,
  3280,  3288,  3310,  3325,  3343,  3351,  3362,  3386,  3412,  3420,
  3428,  3436,  3451,  3459,  3470,  3492,  3509,  3526,  3544,  3552,
  3562,  3574,  3580,  3594,  3601,  3611,  3617,  3623,  3646,  3673,
  3679,  3685,  3698,  3711,  3720,  3730,  3739,  3786,  3802,  3811,
  3817,  3826,  3832,  3881,  3889,  3896,  3905,  3911,  3917,  3931,
  3936,  3942,  3952,  3958,  3960,  3965,  3967,  3975,  3977,  3981,
  3987,  4000,  4006,  4012,  4021,  4025,  4027,  4039,  4041,  4045,
  4073,  4077,  4083,  4089,  4102,  4108,  4114,  4120,  4133,  4139,
  4145,  4151,  4155,  4161,  4166,  4173,  4265,  4365,  4389,  4419,
  4454,  4458,  4460,  4464,  4466,  4470,  4480,  4497,  4506,  4532,
  4542,  4560,  4569,  4596,  4606,  4622,  4631,  4691,  4697,  4701,
  4736,  4752,  4765,  4774,  4782,  4789,  4793,  4799,  4805,  4818,
  4841,  4847,  4855,  4859,  4926,  4932,  4940,  4971,  5009,  5017,
  5023,  5031,  5037,  5043,  5047,  5053,  5063,  5080,  5090,  5121,
  5123,  5127,  5129,  5133,  5148,  5160,  5170,  5187,  5193,  5202,
  5212,  5214,  5218,  5220,  5224,  5233,  5240,  5247,  5255,  5261,
  5275,  5277,  5281,  5346,  5348,  5352,  5354,  5358
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DOT_DOT",
"COLON_COLON_EQUAL","UPPERCASE_IDENTIFIER","LOWERCASE_IDENTIFIER","NUMBER","NEGATIVENUMBER",
"BIN_STRING","HEX_STRING","QUOTED_STRING","ACCESS","AGENT_CAPABILITIES","APPLICATION",
"AUGMENTS","BEGIN_","BITS","CHOICE","CONTACT_INFO","CREATION_REQUIRES","COUNTER32",
"COUNTER64","DEFINITIONS","DEFVAL","DESCRIPTION","DISPLAY_HINT","END","ENTERPRISE",
"EXPORTS","FROM","GROUP","GAUGE32","IDENTIFIER","IMPLICIT","IMPLIED","IMPORTS",
"INCLUDES","INDEX","INTEGER","INTEGER32","IPADDRESS","LAST_UPDATED","MACRO",
"MANDATORY_GROUPS","MAX_ACCESS","MIN_ACCESS","MODULE","MODULE_COMPLIANCE","MODULE_IDENTITY",
"NOTIFICATIONS","NOTIFICATION_GROUP","NOTIFICATION_TYPE","OBJECT","OBJECT_GROUP",
"OBJECT_IDENTITY","OBJECT_TYPE","OBJECTS","OCTET","OF","ORGANIZATION","OPAQUE",
"PRODUCT_RELEASE","REFERENCE","REVISION","SEQUENCE","SIZE","STATUS","STRING",
"SUPPORTS","SYNTAX","TEXTUAL_CONVENTION","TIMETICKS","TRAP_TYPE","UNITS","UNIVERSAL",
"UNSIGNED32","VARIABLES","VARIATION","WRITE_SYNTAX","'{'","'}'","';'","','",
"'('","')'","'['","']'","'.'","'|'","mibFile","modules","module","@1","moduleOid",
"linkagePart","linkageClause","exportsClause","@2","importPart","imports","import",
"importIdentifiers","importIdentifier","importedKeyword","moduleName","declarationPart",
"declarations","declaration","macroClause","@3","@4","macroName","choiceClause",
"@5","fuzzy_lowercase_identifier","valueDeclaration","@6","typeDeclaration",
"@7","typeName","typeSMI","typeDeclarationRHS","@8","@9","conceptualTable","row",
"entryType","sequenceItems","sequenceItem","Syntax","sequenceSyntax","NamedBits",
"NamedBit","@10","objectIdentityClause","@11","@12","@13","objectTypeClause",
"@14","@15","descriptionClause","trapTypeClause","@16","@17","VarPart","VarTypes",
"VarType","DescrPart","MaxAccessPart","@18","@19","notificationTypeClause","@20",
"@21","@22","moduleIdentityClause","@23","@24","@25","@26","@27","@28","ObjectSyntax",
"typeTag","sequenceObjectSyntax","valueofObjectSyntax","SimpleSyntax","@29",
"@30","@31","@32","@33","valueofSimpleSyntax","sequenceSimpleSyntax","ApplicationSyntax",
"sequenceApplicationSyntax","anySubType","integerSubType","octetStringSubType",
"ranges","range","value","enumSpec","enumItems","enumItem","@34","enumNumber",
"Status","Status_Capabilities","DisplayPart","UnitsPart","Access","IndexPart",
"@35","IndexTypes","IndexType","Index","Entry","DefValPart","Value","BitsValue",
"BitNames","ObjectName","NotificationName","ReferPart","RevisionPart","Revisions",
"Revision","ObjectsPart","Objects","Object","NotificationsPart","Notifications",
"Notification","Text","ExtUTCTime","objectIdentifier","@36","subidentifiers",
"subidentifier","objectIdentifier_defval","subidentifiers_defval","subidentifier_defval",
"objectGroupClause","@37","@38","@39","notificationGroupClause","@40","@41",
"@42","moduleComplianceClause","@43","@44","@45","ComplianceModulePart","ComplianceModules",
"ComplianceModule","@46","ComplianceModuleName","MandatoryPart","MandatoryGroups",
"MandatoryGroup","CompliancePart","Compliances","Compliance","ComplianceGroup",
"ComplianceObject","SyntaxPart","WriteSyntaxPart","WriteSyntax","AccessPart",
"agentCapabilitiesClause","@47","@48","@49","ModulePart_Capabilities","Modules_Capabilities",
"Module_Capabilities","@50","CapabilitiesGroups","CapabilitiesGroup","ModuleName_Capabilities",
"VariationPart","Variations","Variation","@51","@52","@53","@54","@55","VariationAccessPart",
"VariationAccess","CreationPart","Cells","Cell", NULL
};
#endif

static const short yyr1[] = {     0,
    90,    90,    91,    91,    93,    92,    94,    94,    95,    95,
    96,    97,    98,    97,    99,    99,   100,   100,   101,   102,
   102,   103,   103,   103,   104,   104,   104,   104,   104,   104,
   104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
   104,   104,   104,   104,   105,   106,   106,   107,   107,   108,
   108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
   108,   108,   110,   111,   109,   112,   112,   112,   112,   112,
   112,   112,   112,   112,   112,   114,   113,   115,   115,   117,
   116,   119,   118,   120,   120,   121,   121,   121,   121,   121,
   121,   121,   121,   122,   123,   124,   122,   122,   125,   126,
   127,   128,   128,   129,   130,   130,   131,   131,   131,   132,
   132,   134,   133,   136,   137,   138,   135,   140,   141,   139,
   142,   142,   144,   145,   143,   146,   146,   147,   147,   148,
   149,   149,   151,   150,   152,   150,   154,   155,   156,   153,
   158,   159,   160,   161,   162,   163,   157,   164,   164,   164,
   164,   164,   164,   165,   165,   166,   166,   167,   168,   169,
   168,   170,   168,   168,   171,   168,   172,   168,   168,   168,
   168,   168,   173,   168,   168,   168,   168,   174,   174,   174,
   174,   174,   174,   174,   175,   175,   175,   175,   176,   176,
   176,   176,   176,   176,   176,   176,   176,   176,   177,   177,
   177,   177,   177,   177,   177,   178,   178,   178,   178,   179,
   180,   181,   181,   182,   182,   183,   183,   183,   183,   184,
   185,   185,   187,   186,   188,   188,   189,   190,   191,   191,
   192,   192,   193,   195,   194,   194,   194,   196,   196,   197,
   197,   198,   199,   200,   200,   201,   201,   202,   202,   203,
   203,   204,   205,   206,   206,   207,   207,   208,   208,   209,
   210,   210,   211,   211,   212,   213,   214,   214,   215,   216,
   217,   219,   218,   220,   220,   221,   221,   221,   221,   221,
   222,   223,   223,   224,   224,   226,   227,   228,   225,   230,
   231,   232,   229,   234,   235,   236,   233,   237,   238,   238,
   240,   239,   241,   241,   241,   242,   242,   243,   243,   244,
   245,   245,   246,   246,   247,   247,   248,   249,   250,   250,
   251,   251,   252,   253,   253,   255,   256,   257,   254,   258,
   258,   259,   259,   261,   260,   262,   262,   263,   264,   264,
   265,   265,   266,   266,   268,   269,   270,   271,   272,   267,
   273,   273,   274,   275,   275,   276,   276,   277
};

static const short yyr2[] = {     0,
     1,     0,     1,     2,     0,    10,     3,     0,     1,     0,
     3,     0,     0,     3,     1,     0,     1,     2,     3,     1,
     3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     0,     1,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     0,     0,     5,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     0,     3,     1,     1,     0,
     8,     0,     4,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     0,     0,    11,     1,     3,     1,
     4,     1,     3,     2,     1,     4,     1,     1,     2,     1,
     3,     0,     5,     0,     0,     0,    14,     0,     0,    18,
     0,     2,     0,     0,    11,     4,     0,     1,     3,     1,
     2,     0,     0,     3,     0,     3,     0,     0,     0,    15,
     0,     0,     0,     0,     0,     0,    21,     1,     2,     1,
     1,     1,     1,     5,     5,     1,     1,     1,     1,     0,
     3,     0,     3,     1,     0,     3,     0,     3,     4,     2,
     4,     2,     0,     4,     2,     4,     2,     1,     1,     1,
     1,     1,     1,     3,     2,     2,     3,     2,     1,     1,
     1,     2,     1,     2,     1,     1,     2,     1,     1,     1,
     2,     2,     1,     1,     1,     1,     1,     1,     0,     3,
     6,     1,     3,     1,     3,     1,     1,     1,     1,     3,
     1,     3,     0,     5,     1,     1,     1,     1,     2,     0,
     2,     0,     1,     0,     5,     4,     0,     1,     3,     2,
     1,     1,     1,     4,     0,     1,     3,     1,     0,     1,
     3,     1,     1,     2,     0,     1,     0,     1,     2,     4,
     4,     0,     1,     3,     1,     4,     1,     3,     1,     1,
     1,     0,     2,     1,     2,     1,     3,     1,     4,     6,
     1,     1,     2,     4,     1,     0,     0,     0,    15,     0,
     0,     0,    15,     0,     0,     0,    15,     1,     1,     2,
     0,     5,     2,     1,     0,     4,     0,     1,     3,     1,
     1,     0,     1,     2,     1,     1,     4,     7,     2,     0,
     2,     0,     1,     2,     0,     0,     0,     0,    17,     1,
     0,     1,     2,     0,     8,     1,     3,     1,     2,     1,
     1,     0,     1,     2,     0,     0,     0,     0,     0,    14,
     2,     0,     1,     4,     0,     1,     3,     1
};

static const short yydefact[] = {     2,
    45,     1,     3,     5,     4,     8,   272,     0,     0,     0,
     0,     7,    79,    78,   278,     0,   276,   273,   274,     0,
     0,     0,   275,    12,     0,   277,    13,    10,   279,     0,
     0,    16,     0,     9,     0,    14,    23,    22,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,     0,    15,
    17,     0,    20,    24,     0,    79,    78,    75,    90,    93,
    91,    89,    86,    74,    66,    73,    69,    72,    70,    67,
    88,    71,    87,    68,    92,     0,     0,    48,    61,    63,
    80,    51,    50,    82,    85,    52,    53,    54,    55,    56,
    58,    59,    57,    60,   280,    11,    18,     0,     0,    62,
     0,     0,     0,     0,     0,     0,     0,     0,     6,    49,
     0,     0,     0,     0,    19,    21,   115,   119,   138,   142,
   287,   291,   295,   327,    64,     0,   124,     0,     0,     0,
   262,     0,   262,     0,     0,     0,     0,     0,     0,   100,
     0,    76,   190,   198,   191,   159,   164,   189,     0,     0,
   196,     0,    95,   195,   193,     0,     0,    98,    83,   150,
   151,   152,    94,   105,     0,   148,   153,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    65,     0,   272,
     0,     0,   170,   175,     0,     0,     0,   192,     0,     0,
     0,   177,   172,     0,   197,     0,     0,   230,   194,     0,
     0,     0,    45,   149,   227,     0,   232,   272,     0,   271,
   143,     0,   272,     0,     0,   270,     0,   272,   127,   217,
   216,   219,   218,     0,     0,   212,   214,     0,   168,   112,
     0,   110,    77,   161,   163,   166,     0,   100,    99,     0,
     0,   102,     0,     0,     0,     0,     0,     0,     0,     0,
   265,     0,   263,   252,     0,     0,     0,   269,     0,   267,
   253,     0,     0,     0,     0,     0,   132,     0,   210,     0,
     0,   223,     0,   221,     0,   106,     0,   174,   209,   108,
   200,   205,   209,   209,   209,   199,     0,     0,   204,   203,
   209,   104,   107,   156,   157,   101,     0,   229,     0,     0,
     0,   171,   176,   169,   116,   231,   135,   133,     0,   261,
   272,     0,     0,     0,   266,   272,     0,   296,   228,     0,
    81,   272,     0,   255,     0,   213,   215,     0,   220,     0,
     0,   111,   109,   206,   207,   208,   201,   185,   186,   188,
   209,   202,   103,     0,   154,   155,   255,     0,     0,     0,
   264,   139,   144,   288,   268,   292,   255,     0,     0,   128,
   130,   131,     0,     0,     0,     0,   222,     0,   187,     0,
     0,   233,   136,   134,   121,   255,     0,   255,   255,     0,
   328,   126,   272,   254,     0,   211,   225,   226,     0,   113,
    96,     0,     0,   255,     0,     0,     0,     0,   305,     0,
   298,   299,   255,   129,   125,   224,   255,   272,   122,   237,
     0,   145,     0,     0,   304,   301,     0,   300,   331,     0,
     0,     0,   234,   245,   272,     0,   272,   272,   303,   307,
   272,     0,     0,   330,   332,     0,   117,   272,     0,     0,
     0,     0,     0,     0,     0,     0,   312,     0,   272,   334,
     0,   333,    97,     0,   243,   272,     0,     0,   140,   146,
   289,   293,   272,   272,   272,   302,   311,   313,   315,   316,
   297,   339,     0,   272,   236,   272,     0,   238,   241,   242,
   182,   178,   179,   180,   181,   183,   249,   246,   158,     0,
   272,   257,   310,     0,   308,     0,   320,   314,     0,     0,
   240,   235,   272,   250,   285,     0,   248,     0,   281,   282,
   244,     0,     0,     0,   256,   258,   306,   272,     0,     0,
   322,   272,   329,   239,     0,   247,     0,   184,     0,   283,
   120,     0,     0,   259,   309,   317,   319,     0,   325,   338,
     0,   336,     0,   251,     0,   272,   323,   321,     0,     0,
   342,   272,   284,   260,     0,   324,     0,   272,   335,   341,
   343,   337,   147,   318,   345,   344,   320,   346,   322,   347,
   352,     0,   355,   353,   351,     0,   348,   272,   245,   358,
     0,   356,   349,   354,   272,     0,   357,     0,   350,     0,
     0,     0
};

static const short yydefgoto[] = {   600,
     2,     3,     6,     8,    33,    34,    28,    31,    59,    60,
    61,    62,    63,    64,   167,    86,    87,    88,    89,   121,
   147,    90,   168,   196,    17,    92,   122,    93,   124,    94,
    95,   169,   208,   417,   170,   171,   172,   251,   252,   173,
   302,   241,   242,   285,    96,   111,   139,   357,    97,   112,
   140,   404,    98,   123,   149,   277,   369,   370,   334,   319,
   359,   358,    99,   113,   141,   386,   100,   114,   142,   266,
   387,   436,   502,   174,   175,   303,   498,   176,   199,   200,
   201,   192,   247,   499,   304,   177,   305,   343,   344,   345,
   235,   236,   237,   346,   283,   284,   338,   399,   216,   330,
   254,   260,   383,   434,   449,   487,   488,   489,   464,   451,
   500,   516,   517,   490,   268,   374,   524,   525,   526,   181,
   262,   263,   185,   269,   270,   227,   221,   264,    10,    18,
    19,   518,   519,   520,   101,   115,   143,   388,   102,   116,
   144,   389,   103,   117,   145,   367,   410,   411,   412,   440,
   426,   457,   504,   505,   476,   477,   478,   479,   480,   531,
   549,   558,   560,   104,   118,   146,   413,   443,   444,   445,
   483,   551,   552,   460,   569,   570,   571,   577,   579,   581,
   589,   596,   583,   585,   587,   591,   592
};

static const short yypact[] = {     8,
-32768,     8,-32768,-32768,-32768,   -54,-32768,    16,   -36,    93,
    54,-32768,   -24,   -18,-32768,   -10,-32768,    93,-32768,    68,
    95,    89,-32768,    83,    58,    55,-32768,   118,-32768,   140,
    79,   392,   239,-32768,    73,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    86,   392,
-32768,     5,-32768,-32768,   105,   178,   290,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   160,   308,-32768,-32768,-32768,
   117,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,     8,   392,-32768,
   142,   144,   149,   154,   151,   158,   162,   198,-32768,-32768,
   169,   161,   145,   209,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   182,-32768,   109,   150,   152,
   166,   177,   166,   174,   159,   165,   202,   226,   205,     6,
   155,-32768,-32768,-32768,   153,   -21,   157,-32768,   201,   168,
   163,   -40,-32768,-32768,   153,     4,   175,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    22,-32768,-32768,   232,   167,   170,
   179,   231,   181,   176,   184,   232,   238,-32768,   185,-32768,
    41,   187,-32768,-32768,   248,   183,   156,-32768,   153,   187,
   153,-32768,   173,   189,-32768,   254,   256,   242,-32768,   262,
   265,   268,     1,-32768,-32768,   249,   203,-32768,   232,-32768,
-32768,   232,-32768,   232,   250,-32768,   214,-32768,   199,-32768,
-32768,-32768,-32768,   208,     2,-32768,   279,   277,-32768,-32768,
   -39,-32768,-32768,-32768,-32768,-32768,   163,-32768,-32768,    84,
    57,-32768,   238,   218,   210,   211,    13,   238,   238,    11,
-32768,    63,-32768,-32768,   261,   229,   271,-32768,    70,-32768,
-32768,   274,   238,   295,   221,   225,   281,   156,-32768,   156,
   156,-32768,    76,-32768,   223,-32768,   248,-32768,    13,-32768,
-32768,-32768,    13,    13,    13,-32768,   275,   252,-32768,-32768,
    13,-32768,-32768,-32768,-32768,-32768,   256,-32768,   232,   283,
   284,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   255,-32768,
-32768,   238,   238,   238,-32768,-32768,   238,-32768,-32768,   298,
-32768,-32768,   238,   263,    47,-32768,-32768,   240,-32768,   277,
   318,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    13,-32768,-32768,   302,-32768,-32768,   263,   322,   322,   232,
-32768,-32768,-32768,-32768,-32768,-32768,   263,   238,    88,-32768,
-32768,-32768,   238,   327,   247,   101,-32768,   251,-32768,   238,
   329,-32768,-32768,-32768,   309,   263,   324,   263,   263,   303,
-32768,-32768,-32768,-32768,   330,-32768,-32768,-32768,   267,-32768,
-32768,   273,   238,   263,   351,   238,   354,   357,   360,   362,
   303,-32768,   263,-32768,-32768,-32768,   263,-32768,-32768,     9,
   287,-32768,   288,   293,   128,-32768,   294,-32768,   301,   305,
   296,   306,-32768,   352,-32768,   358,-32768,-32768,-32768,   338,
-32768,   380,   383,   301,-32768,   167,-32768,-32768,   310,   311,
   384,   312,   238,   313,   314,   316,   -16,   319,   355,-32768,
   321,-32768,-32768,   323,-32768,   364,    23,   326,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   -16,-32768,-32768,-32768,
-32768,-32768,   365,-32768,-32768,-32768,    92,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   113,-32768,-32768,   331,
-32768,   325,-32768,   110,-32768,   378,   337,-32768,   328,   334,
-32768,-32768,   364,   332,-32768,   336,   339,   340,   171,-32768,
-32768,   342,   231,   406,   325,-32768,-32768,-32768,   238,   167,
   341,-32768,-32768,-32768,   412,-32768,   419,-32768,   332,-32768,
-32768,   401,   347,-32768,-32768,-32768,-32768,   167,   389,-32768,
   111,-32768,   344,-32768,   238,-32768,-32768,-32768,   322,   413,
   359,-32768,-32768,-32768,   361,-32768,   238,-32768,-32768,   359,
-32768,-32768,-32768,-32768,-32768,-32768,   337,-32768,   341,-32768,
   427,   439,   429,-32768,-32768,   370,-32768,-32768,   352,-32768,
   115,-32768,-32768,-32768,-32768,   426,-32768,   238,-32768,   454,
   455,-32768
};

static const short yypgoto[] = {-32768,
-32768,   456,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   396,-32768,   348,-32768,    10,-32768,-32768,   372,-32768,-32768,
-32768,-32768,-32768,-32768,   -17,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   264,-32768,-32768,   164,  -176,
-32768,-32768,   186,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    67,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   286,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -241,  -144,  -136,
   191,   192,   193,  -183,-32768,   122,-32768,-32768,  -181,-32768,
-32768,-32768,  -355,-32768,-32768,-32768,   -38,    -8,-32768,  -113,
-32768,-32768,-32768,  -217,    44,  -321,-32768,-32768,   -45,   343,
-32768,   172,-32768,-32768,   180,  -251,   -42,    -7,-32768,-32768,
   464,-32768,-32768,   -35,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    72,-32768,
-32768,-32768,-32768,   -43,-32768,-32768,    12,-32768,-32768,   -90,
   -91,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    46,
-32768,-32768,   -71,-32768,-32768,-32768,   -78,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -101
};


#define	YYLAST		555


static const short yytable[] = {     9,
   261,   308,   217,   384,   225,   193,   315,   316,   239,     4,
   198,     4,     1,   194,   474,    91,   245,   210,   206,    16,
   209,   328,   317,   432,   205,     7,   213,    16,   491,   492,
   493,   494,   495,   496,   108,   381,   475,   265,    11,   207,
   267,   286,   272,   287,    12,   390,   433,   230,   231,   232,
   233,   347,   348,   349,   244,   318,   246,    20,  -162,   352,
   156,   157,  -160,   -45,   405,    21,   407,   408,   193,    91,
   362,   363,   364,   314,   159,   366,   194,    22,   211,   160,
  -167,   372,   420,    24,   191,  -167,   279,   109,   289,   191,
   280,   429,   238,   -45,    26,   430,   191,    13,    14,    15,
   290,    25,   497,   261,   291,   292,   234,   397,   398,   379,
   288,    27,   312,   150,   371,   293,   391,   125,   514,   515,
   313,   394,   294,   295,   296,   151,   152,   354,   401,   153,
   154,   375,  -272,  -272,  -272,   280,   297,   306,    30,   307,
   155,   298,    29,   320,   299,   321,    35,   156,   157,   158,
   325,   419,   326,    32,   422,   300,   339,   105,   340,   301,
    36,   159,   230,   231,   232,   233,   160,   106,   392,   161,
   393,   150,   512,   162,   513,   371,   539,   515,   385,   163,
   164,   -84,   229,   151,   165,   110,   119,   153,   154,  -123,
   527,   561,   528,   562,   166,   594,   127,   595,   155,   128,
   129,   470,   130,   566,   131,   156,   157,   158,   132,   133,
   134,   135,   138,   136,   148,   271,   178,   137,   182,   159,
   275,   179,   180,   184,   160,   186,   187,   161,   188,   189,
   465,   162,   190,   202,   195,   203,   197,   215,   164,    65,
  -165,   220,   165,    66,    67,   219,   204,   222,   226,   218,
   224,    68,   166,   240,   234,   223,  -173,   507,   248,    69,
    70,   250,   212,   243,   228,   -47,   238,   253,   255,   463,
    71,   256,   257,   258,   273,   276,   259,   546,    72,    73,
   274,   281,   282,   522,   309,   322,    74,    75,   323,    76,
    77,   278,    78,    79,    80,   324,   310,   311,   327,    81,
   329,   331,  -326,   564,   332,   333,   341,   350,    65,    82,
    83,    84,    66,    67,    85,   574,   355,   356,   271,   351,
    68,   360,   368,   376,   378,   373,   380,   382,    69,    70,
   395,   396,   402,   403,   -46,   400,   415,  -294,  -141,    71,
  -290,  -137,   406,  -286,  -114,  -118,   599,    72,    73,   409,
   575,   416,   418,   547,   421,    74,    75,   423,    76,    77,
   424,    78,    79,    80,   425,   427,   435,   437,    81,   442,
   590,   557,   438,   441,   446,   450,   447,   590,    82,    83,
    84,   456,   453,    85,   459,   448,   461,   468,   523,   466,
   467,  -340,   469,   471,   472,   473,    37,    38,   486,   481,
   484,   509,   529,   485,    39,   501,   530,   532,    40,   543,
   431,   521,    41,    42,   533,   535,   536,   439,   553,   548,
   538,   537,   541,    43,   554,   555,   556,   271,   563,   454,
   455,    44,    45,   458,   559,    46,   568,   567,   582,    47,
    48,   573,    49,    50,   584,    51,    52,    53,   586,   588,
   598,   482,    54,   601,   602,   107,   126,     5,   120,   414,
   214,   377,    55,    56,    57,   503,   506,    58,   335,   249,
   353,   336,   342,   337,   534,   593,   510,   511,   452,   544,
   542,    23,   428,   540,   545,   183,   578,   580,   508,   462,
   572,   576,   361,   597,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   365,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   503,     0,     0,     0,   550,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   565,     0,
     0,     0,     0,     0,   550
};

static const short yycheck[] = {     7,
   218,   253,   179,   359,   186,   150,   258,   259,   192,     0,
   155,     2,     5,   150,    31,    33,   200,    14,    59,    10,
   165,   273,    12,    15,   161,    80,     5,    18,     6,     7,
     8,     9,    10,    11,    30,   357,    53,   219,    23,    80,
   222,    81,   224,    83,    81,   367,    38,     7,     8,     9,
    10,   293,   294,   295,   199,    45,   201,     4,    80,   301,
    39,    40,    84,    88,   386,    84,   388,   389,   213,    87,
   322,   323,   324,   257,    53,   327,   213,    88,    75,    58,
    80,   333,   404,    16,    84,    80,    85,    83,     5,    84,
    89,   413,    80,    88,     6,   417,    84,     5,     6,     7,
    17,     7,    80,   321,    21,    22,    66,     7,     8,   351,
   247,    29,   257,     5,   332,    32,   368,   108,     6,     7,
   257,   373,    39,    40,    41,    17,    18,   309,   380,    21,
    22,    85,     5,     6,     7,    89,    53,    81,    84,    83,
    32,    58,    85,    81,    61,    83,     7,    39,    40,    41,
    81,   403,    83,    36,   406,    72,    81,    85,    83,    76,
    82,    53,     7,     8,     9,    10,    58,    82,    81,    61,
    83,     5,    81,    65,    83,   393,     6,     7,   360,    71,
    72,     4,   190,    17,    76,    81,    27,    21,    22,    73,
    81,    81,    83,    83,    86,    81,    55,    83,    32,    56,
    52,   453,    49,   559,    54,    39,    40,    41,    51,    48,
    13,    43,     4,    53,    33,   223,    67,    73,    42,    53,
   228,    70,    57,    50,    58,    67,    62,    61,    27,     4,
   448,    65,    28,    33,    80,    68,    84,     6,    72,     1,
    84,    11,    76,     5,     6,    67,    84,    67,    11,    80,
    67,    13,    86,     6,    66,    80,    84,   475,     5,    21,
    22,     6,    88,    81,    80,    27,    80,    26,     7,   446,
    32,     7,     5,    25,    25,    77,    74,   529,    40,    41,
    67,     3,     6,   501,    67,    25,    48,    49,    60,    51,
    52,    84,    54,    55,    56,    25,    87,    87,    25,    61,
     6,    81,    13,   555,    80,    25,    84,    33,     1,    71,
    72,    73,     5,     6,    76,   567,    34,    34,   326,    68,
    13,    67,    25,    84,     7,    63,    25,     6,    21,    22,
     4,    85,     4,    25,    27,    85,     7,    48,    49,    32,
    51,    52,    19,    54,    55,    56,   598,    40,    41,    47,
   568,    85,    80,   530,     4,    48,    49,     4,    51,    52,
     4,    54,    55,    56,     5,     4,    80,    80,    61,    69,
   588,   548,    80,    80,    70,    24,    81,   595,    71,    72,
    73,    44,    25,    76,     5,    80,     4,     4,    64,    80,
    80,    37,    81,    81,    81,    80,     5,     6,    35,    81,
    80,    37,    25,    81,    13,    80,    70,    80,    17,     4,
   418,    81,    21,    22,    81,    84,    81,   425,     7,    79,
    81,    83,    81,    32,     6,    25,    80,   435,    85,   437,
   438,    40,    41,   441,    46,    44,    78,    25,    12,    48,
    49,    81,    51,    52,     6,    54,    55,    56,    20,    80,
    25,   459,    61,     0,     0,    60,   109,     2,    87,   393,
   175,   340,    71,    72,    73,   473,   474,    76,   278,   206,
   307,   280,   287,   281,   513,   589,   484,   486,   435,   525,
   523,    18,   411,   519,   528,   143,   577,   579,   477,   444,
   562,   570,   321,   595,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   326,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   528,    -1,    -1,    -1,   532,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   556,    -1,
    -1,    -1,    -1,    -1,   562
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.27.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 216 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse ();
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 1112 "parser-smi.y"
{
        yyval.err = 0;
    ;
    break;}
case 2:
#line 1116 "parser-smi.y"
{
	yyval.err = 0;
    ;
    break;}
case 3:
#line 1122 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 4:
#line 1124 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 5:
#line 1132 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    thisParserPtr->modulePtr = findModuleByName(yyvsp[0].id);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule(yyvsp[0].id,
					      smiStrdup(thisParserPtr->path),
					      0,
					      thisParserPtr);
				thisParserPtr->modulePtr->
				    numImportedIdentifiers = 0;
				thisParserPtr->modulePtr->
				    numStatements = 0;
				thisParserPtr->modulePtr->
				    numModuleIdentities = 0;
				if (!strcmp(yyvsp[0].id, "SNMPv2-SMI")) {
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
					      yyvsp[0].id);
				/*
				 * this aborts parsing the whole file,
				 * not only the current module.
				 */
				YYABORT;
			    }
			;
    break;}
case 6:
#line 1174 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    checkModuleIdentity(thisParserPtr, thisModulePtr);
			    checkObjects(thisParserPtr, thisModulePtr);
			    checkTypes(thisParserPtr, thisModulePtr);
			    checkDefvals(thisParserPtr, thisModulePtr);
			    checkImportsUsage(thisParserPtr, thisModulePtr);
			    smiCheckTypeUsage(thisParserPtr, thisModulePtr);
			    
			    capabilitiesModulePtr = NULL;

                            yyval.err = 0;
			;
    break;}
case 7:
#line 1191 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 8:
#line 1193 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 9:
#line 1200 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 10:
#line 1202 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 11:
#line 1206 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;

			    if (thisModulePtr->export.language != SMI_LANGUAGE_SMIV2)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    yyval.err = 0;
			;
    break;}
case 12:
#line 1218 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 13:
#line 1220 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;

			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1065-SMI")) {
			        smiPrintError(thisParserPtr, ERR_EXPORTS);
			    }
			;
    break;}
case 14:
#line 1232 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 15:
#line 1236 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 16:
#line 1238 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 17:
#line 1243 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 18:
#line 1245 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 19:
#line 1251 "parser-smi.y"
{
			    Import      *importPtr;
			    Module      *modulePtr;
			    
			    /*
			     * Recursively call the parser to suffer
			     * the IMPORTS, if the module is not yet
			     * loaded.
			     */
			    modulePtr = findModuleByName(yyvsp[0].id);
			    if (!modulePtr) {
				modulePtr = loadModule(yyvsp[0].id);
			    }
			    checkImports(modulePtr, thisParserPtr);

			    if (modulePtr && !strcmp(yyvsp[0].id, "SNMPv2-SMI")) {
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
						 yyvsp[0].id)) &&
					((importPtr->kind == KIND_MACRO) ||
					 (importPtr->kind == KIND_TYPE))) {
					thisModulePtr->export.language =
					    SMI_LANGUAGE_SMIV2;
				    }
				}
			    }

			    smiFree(yyvsp[0].id);
			;
    break;}
case 20:
#line 1291 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 21:
#line 1294 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 22:
#line 1301 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 23:
#line 1307 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 24:
#line 1313 "parser-smi.y"
{
			    addImport(smiStrdup(yyvsp[0].id), thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 45:
#line 1351 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 46:
#line 1363 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 47:
#line 1365 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 48:
#line 1370 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 49:
#line 1372 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 50:
#line 1376 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 51:
#line 1381 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 52:
#line 1386 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 53:
#line 1391 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 54:
#line 1396 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 55:
#line 1401 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 56:
#line 1406 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 57:
#line 1411 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 58:
#line 1416 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 59:
#line 1421 "parser-smi.y"
{
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 60:
#line 1426 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 61:
#line 1431 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 62:
#line 1436 "parser-smi.y"
{
			    smiPrintError(thisParserPtr,
					  ERR_FLUSH_DECLARATION);
			    yyerrok;
			    yyval.err = 1;
			;
    break;}
case 63:
#line 1450 "parser-smi.y"
{
			    Macro *macroPtr;

			    firstStatementLine = thisParserPtr->line;

			    macroPtr = addMacro(smiStrdup(yyvsp[0].id),
						0, thisParserPtr);
			    setMacroLine(macroPtr, firstStatementLine,
					 thisParserPtr);
			;
    break;}
case 64:
#line 1461 "parser-smi.y"
{
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
				       "RFC1065-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_MACRO);
			    }
			;
    break;}
case 65:
#line 1485 "parser-smi.y"
{
			    yyval.err = 0;
                        ;
    break;}
case 66:
#line 1490 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 67:
#line 1491 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 68:
#line 1492 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 69:
#line 1493 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 70:
#line 1494 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 71:
#line 1495 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 72:
#line 1496 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 73:
#line 1497 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 74:
#line 1498 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 75:
#line 1499 "parser-smi.y"
{ yyval.id = yyvsp[0].id; ;
    break;}
case 76:
#line 1503 "parser-smi.y"
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
				       "RFC1065-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_CHOICE);
			    }
			;
    break;}
case 77:
#line 1521 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			;
    break;}
case 78:
#line 1531 "parser-smi.y"
{
			  yyval.id = yyvsp[0].id;
			;
    break;}
case 79:
#line 1536 "parser-smi.y"
{
			    smiPrintError (thisParserPtr,
					   ERR_BAD_LOWER_IDENTIFIER_CASE,
					   yyvsp[0].id);
			    /* xxx
			    if ((thisParserPtr->flags & SMI_FLAG_BE_LAX) == 0) {
			        YYERROR;
			    }
			    */
			  yyval.id = yyvsp[0].id;
			;
    break;}
case 80:
#line 1551 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;

			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        if (strchr(yyvsp[0].id, '-') &&
				    (strcmp(yyvsp[0].id, "mib-2") ||
				  strcmp(thisModulePtr->export.name, "SNMPv2-SMI"))) {
				    smiPrintError(thisParserPtr,
						  ERR_OIDNAME_INCLUDES_HYPHEN,
						  yyvsp[0].id);
				}
			    }
			;
    break;}
case 81:
#line 1571 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-7].id, &objectPtr);
			    objectPtr = setObjectName(objectPtr, yyvsp[-7].id);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_VALUEASSIGNMENT);
			    yyval.err = 0;
			;
    break;}
case 82:
#line 1590 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;

			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			;
    break;}
case 83:
#line 1597 "parser-smi.y"
{
			    Type *typePtr;
			    
			    if (strlen(yyvsp[-3].id)) {
				if (yyvsp[0].typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
				    smiCheckTypeName(thisParserPtr,
						     thisModulePtr, yyvsp[-3].id,
						     firstStatementLine);
				}
				setTypeLine(yyvsp[0].typePtr, firstStatementLine,
					    thisParserPtr);
				setTypeName(yyvsp[0].typePtr, yyvsp[-3].id);
				yyval.err = 0;
			    } else {
				yyval.err = 0;
			    }

			    /*
			     * If we are in an SMI module, some type
			     * definitions derived from ASN.1 `INTEGER'
			     * must be modified to libsmi basetypes.
			     */
			    if (thisModulePtr &&
				(!strcmp(thisModulePtr->export.name, "SNMPv2-SMI"))) {
				if (!strcmp(yyvsp[-3].id, "Counter32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Gauge32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Unsigned32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "TimeTicks")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Counter64")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED64;
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned64 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned64 = LIBSMI_UINT64_MAX;
				    }
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned64Ptr);
				}
			    }
			    if (thisModulePtr &&
				(!strcmp(thisModulePtr->export.name, "RFC1155-SMI") ||
				 !strcmp(thisModulePtr->export.name, "RFC1065-SMI"))) {
				if (!strcmp(yyvsp[-3].id, "Counter")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Gauge")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "TimeTicks")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "NetworkAddress")) {
				    setTypeName(yyvsp[0].typePtr, smiStrdup("NetworkAddress"));
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_OCTETSTRING;
				    setTypeParent(yyvsp[0].typePtr, findTypeByModuleAndName(
					                   thisModulePtr,
						           "IpAddress"));
				} else if (!strcmp(yyvsp[-3].id, "IpAddress")) {
				    typePtr = findTypeByModuleAndName(
					thisModulePtr, "NetworkAddress");
				    if (typePtr) 
					setTypeParent(typePtr, yyvsp[0].typePtr);
				}
			    }
			;
    break;}
case 84:
#line 1715 "parser-smi.y"
{
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 85:
#line 1719 "parser-smi.y"
{
			    yyval.id = smiStrdup(yyvsp[0].id);
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
				       "RFC1065-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_SMI, yyvsp[0].id);
			    }
			;
    break;}
case 94:
#line 1753 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				/*
				 * If we found an already defined type,
				 * we have to inherit a new type structure.
				 * (Otherwise the `Syntax' rule created
				 * a new type for us.)
				 */
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
				setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			    }
			;
    break;}
case 95:
#line 1769 "parser-smi.y"
{
			    Import *importPtr;

			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV2;

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
			;
    break;}
case 96:
#line 1792 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 97:
#line 1800 "parser-smi.y"
{
			    if ((yyvsp[0].typePtr) && !(yyvsp[0].typePtr->export.name)) {
				/*
				 * If the Type we found has just been
				 * defined, we don't have to allocate
				 * a new one.
				 */
				yyval.typePtr = yyvsp[0].typePtr;
			    } else {
				if (!(yyvsp[0].typePtr))
				    smiPrintError(thisParserPtr, ERR_INTERNAL);
				/*
				 * Otherwise, we have to allocate a
				 * new Type struct, inherited from $10.
				 */
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    }
			    setTypeDescription(yyval.typePtr, yyvsp[-4].text, thisParserPtr);
			    if (yyvsp[-2].text) {
				setTypeReference(yyval.typePtr, yyvsp[-2].text, thisParserPtr);
			    }
			    setTypeStatus(yyval.typePtr, yyvsp[-6].status);
			    if (yyvsp[-8].text) {
				smiCheckFormat(thisParserPtr,
					       yyval.typePtr->export.basetype, yyvsp[-8].text,
					       firstStatementLine);
				setTypeFormat(yyval.typePtr, yyvsp[-8].text);
			    }
			    setTypeDecl(yyval.typePtr, SMI_DECL_TEXTUALCONVENTION);
			;
    break;}
case 98:
#line 1831 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			;
    break;}
case 99:
#line 1839 "parser-smi.y"
{
			    if (yyvsp[0].typePtr) {
				yyval.typePtr = addType(NULL,
					     SMI_BASETYPE_UNKNOWN, 0,
					     thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPL_SEQUENCEOF);
				setTypeParent(yyval.typePtr, yyvsp[0].typePtr);
			    } else {
				yyval.typePtr = NULL;
			    }
			;
    break;}
case 100:
#line 1858 "parser-smi.y"
{
			    Type *typePtr;
			    Import *importPtr;

			    yyval.typePtr = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->export.name, yyvsp[0].id);
			    if (! yyval.typePtr) {
				importPtr = findImportByName(yyvsp[0].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    typePtr = addType(yyvsp[0].id,
						      SMI_BASETYPE_UNKNOWN,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				    yyval.typePtr = typePtr;
				} else {
				    /*
				     * imported type.
				     * TODO: is this allowed in a SEQUENCE? 
				     */
				    importPtr->use++;
				    yyval.typePtr = findTypeByModulenameAndName(
					importPtr->export.module,
					importPtr->export.name);
				    smiFree(yyvsp[0].id);
				}
			    } else {
				smiFree(yyvsp[0].id);
			    }
			;
    break;}
case 101:
#line 1898 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[-1].listPtr);
			;
    break;}
case 102:
#line 1906 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 103:
#line 1913 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 104:
#line 1932 "parser-smi.y"
{
			    Object *objectPtr;
			    Import *importPtr;
			    
			    objectPtr =
			        findObjectByModuleAndName(thisParserPtr->modulePtr,
							  yyvsp[-1].id);

			    if (!objectPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    objectPtr = addObject(yyvsp[-1].id, smiHandle->pendingNodePtr,
					                  0,
					                  FLAG_INCOMPLETE,
						          thisParserPtr);
				} else {
				    /*
				     * imported object.
				     */
				    importPtr->use++;
				    objectPtr = findObjectByModulenameAndName(
					importPtr->export.module, yyvsp[-1].id);
				    smiFree(yyvsp[-1].id);
				}
			    } else {
				smiFree(yyvsp[-1].id);
			    }

			    yyval.objectPtr = objectPtr;
			;
    break;}
case 105:
#line 1967 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    if (yyval.typePtr)
				defaultBasetype = yyval.typePtr->export.basetype;
			;
    break;}
case 106:
#line 1975 "parser-smi.y"
{
			    Type *typePtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_BITS;
			    typePtr = addType(NULL, SMI_BASETYPE_BITS,
					      FLAG_INCOMPLETE,
					      thisParserPtr);
			    setTypeDecl(typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(typePtr, smiHandle->typeBitsPtr);
			    setTypeList(typePtr, yyvsp[-1].listPtr);
			    for (p = yyvsp[-1].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = typePtr;
			    yyval.typePtr = typePtr;
			;
    break;}
case 107:
#line 1994 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 108:
#line 1998 "parser-smi.y"
{
			    /* TODO: */
			    yyval.typePtr = smiHandle->typeOctetStringPtr;
			;
    break;}
case 109:
#line 2003 "parser-smi.y"
{
			    Type *typePtr;
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->export.name, yyvsp[-1].id);
			    if (! yyval.typePtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    typePtr = addType(yyvsp[-1].id, SMI_BASETYPE_UNKNOWN,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				    yyval.typePtr = typePtr;
				} else {
				    /*
				     * imported type.
				     *
				     * We are in a SEQUENCE clause,
				     * where we do not have to create
				     * a new Type struct.
				     */
				    importPtr->use++;
				    smiFree(yyvsp[-1].id);
				}
			    } else {
				smiFree(yyvsp[-1].id);
			    }
			;
    break;}
case 110:
#line 2040 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 111:
#line 2046 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 112:
#line 2059 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_BITNAME_32, ERR_BITNAME_64);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (strchr(yyvsp[0].id, '-')) {
				    smiPrintError(thisParserPtr,
						  ERR_NAMEDBIT_INCLUDES_HYPHEN,
						  yyvsp[0].id);
				}
			    }
			;
    break;}
case 113:
#line 2072 "parser-smi.y"
{
			    yyval.namedNumberPtr = smiMalloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id;
			    yyval.namedNumberPtr->export.value.basetype =
				                       SMI_BASETYPE_UNSIGNED32;
			    yyval.namedNumberPtr->export.value.value.unsigned32 = yyvsp[-1].unsigned32;
			    /* RFC 2578 7.1.4 */
			    if (yyvsp[-1].unsigned32 >= 65535*8) {
				smiPrintError(thisParserPtr,
					      ERR_BITS_NUMBER_TOO_LARGE,
					      yyvsp[-4].id, yyvsp[-1].unsigned32);
			    } else {
				if (yyvsp[-1].unsigned32 >= 128) {
				    smiPrintError(thisParserPtr,
						  ERR_BITS_NUMBER_LARGE,
						  yyvsp[-4].id, yyvsp[-1].unsigned32);
				}
			    }
			;
    break;}
case 114:
#line 2094 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 115:
#line 2103 "parser-smi.y"
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
			;
    break;}
case 116:
#line 2121 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 117:
#line 2130 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-13].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTIDENTITY);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    yyval.err = 0;
			;
    break;}
case 118:
#line 2152 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 119:
#line 2161 "parser-smi.y"
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
			;
    break;}
case 120:
#line 2190 "parser-smi.y"
{
			    Object *objectPtr, *parentPtr;

			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-17].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-17].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectType(objectPtr, yyvsp[-12].typePtr);
			    if (!(yyvsp[-12].typePtr->export.name)) {
				/*
				 * An inlined type.
				 */
#if 0 /* export implicitly defined types by the node's lowercase name */
				setTypeName(yyvsp[-12].typePtr, yyvsp[-17].id);
#endif
			    }
			    setObjectUnits(objectPtr, yyvsp[-11].text);
			    setObjectAccess(objectPtr, yyvsp[-10].access);
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
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    if (yyvsp[-7].text) {
				setObjectDescription(objectPtr, yyvsp[-7].text, thisParserPtr);
			    }
			    if (yyvsp[-6].text) {
				setObjectReference(objectPtr, yyvsp[-6].text, thisParserPtr);
			    }
			    if (yyvsp[-5].index.indexkind != SMI_INDEX_UNKNOWN) {
				setObjectList(objectPtr, yyvsp[-5].index.listPtr);
				setObjectImplied(objectPtr, yyvsp[-5].index.implied);
				setObjectIndexkind(objectPtr, yyvsp[-5].index.indexkind);
				setObjectRelated(objectPtr, yyvsp[-5].index.rowPtr);
			    }
			    if (yyvsp[-4].valuePtr) {
				if (((objectPtr->typePtr->export.basetype == SMI_BASETYPE_OCTETSTRING) &&
				     (yyvsp[-4].valuePtr->basetype != SMI_BASETYPE_OCTETSTRING)) ||
				    ((objectPtr->typePtr->export.basetype == SMI_BASETYPE_OBJECTIDENTIFIER) &&
				     (yyvsp[-4].valuePtr->basetype != SMI_BASETYPE_OBJECTIDENTIFIER))) {
				    smiPrintError(thisParserPtr,
						  ERR_DEFVAL_SYNTAX);
				    if (yyvsp[-4].valuePtr->basetype == SMI_BASETYPE_OBJECTIDENTIFIER) {
					smiFree(yyvsp[-4].valuePtr->value.oid);
				    }
				    if ((yyvsp[-4].valuePtr->basetype == SMI_BASETYPE_BITS) ||
					(yyvsp[-4].valuePtr->basetype == SMI_BASETYPE_OCTETSTRING)) {
					smiFree(yyvsp[-4].valuePtr->value.ptr);
				    }
				    smiFree(yyvsp[-4].valuePtr);
				} else {
				    setObjectValue(objectPtr, yyvsp[-4].valuePtr);
				}
			    }
			    yyval.err = 0;
			;
    break;}
case 121:
#line 2292 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				smiPrintError(thisParserPtr,
					      ERR_MISSING_DESCRIPTION);
			    }
			    yyval.text = NULL;
			;
    break;}
case 122:
#line 2301 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 123:
#line 2311 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 124:
#line 2320 "parser-smi.y"
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
			;
    break;}
case 125:
#line 2344 "parser-smi.y"
{
			    Object *objectPtr;
			    Node *nodePtr;
			    
			    objectPtr = yyvsp[-5].objectPtr;
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
						  yyvsp[0].unsigned32,
						  FLAG_INCOMPLETE,
						  thisParserPtr);
			    
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-10].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-10].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_TRAPTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectList(objectPtr, yyvsp[-4].listPtr);
			    setObjectStatus(objectPtr, SMI_STATUS_CURRENT); 
			    setObjectDescription(objectPtr, yyvsp[-3].text, thisParserPtr);
			    if (yyvsp[-2].text) {
				setObjectReference(objectPtr, yyvsp[-2].text, thisParserPtr);
			    }
			    yyval.err = 0;
			;
    break;}
case 126:
#line 2391 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 127:
#line 2395 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 128:
#line 2401 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 129:
#line 2407 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 130:
#line 2420 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 131:
#line 2426 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 132:
#line 2434 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 133:
#line 2438 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        smiPrintError(thisParserPtr,
					      ERR_MAX_ACCESS_IN_SMIV1);
			    }
			;
    break;}
case 134:
#line 2446 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 135:
#line 2448 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        smiPrintError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			;
    break;}
case 136:
#line 2456 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 137:
#line 2460 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 138:
#line 2469 "parser-smi.y"
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
			;
    break;}
case 139:
#line 2486 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 140:
#line 2495 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-14].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectList(objectPtr, yyvsp[-10].listPtr);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    yyval.err = 0;
			;
    break;}
case 141:
#line 2520 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 142:
#line 2529 "parser-smi.y"
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
			;
    break;}
case 143:
#line 2554 "parser-smi.y"
{
			    setModuleLastUpdated(thisParserPtr->modulePtr, yyvsp[0].date);
			;
    break;}
case 144:
#line 2558 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_ORGANIZATION);
			    }
			;
    break;}
case 145:
#line 2565 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_CONTACT);
			    }
			;
    break;}
case 146:
#line 2572 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 147:
#line 2581 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-20].id, &objectPtr);

			    thisParserPtr->modulePtr->numModuleIdentities++;

			    objectPtr = setObjectName(objectPtr, yyvsp[-20].id);
			    setObjectDecl(objectPtr, SMI_DECL_MODULEIDENTITY);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, SMI_STATUS_CURRENT);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setModuleIdentityObject(thisParserPtr->modulePtr,
						    objectPtr);
			    setModuleOrganization(thisParserPtr->modulePtr,
						  yyvsp[-12].text);
			    setModuleContactInfo(thisParserPtr->modulePtr,
						 yyvsp[-9].text);
			    setModuleDescription(thisParserPtr->modulePtr,
						 yyvsp[-6].text, thisParserPtr);
			    /* setObjectDescription(objectPtr, $13); */
			    yyval.err = 0;
			;
    break;}
case 148:
#line 2611 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 149:
#line 2615 "parser-smi.y"
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
				       "RFC1065-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_TAG, yyvsp[-1].err);
			    }
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 150:
#line 2633 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 151:
#line 2638 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 152:
#line 2643 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 153:
#line 2648 "parser-smi.y"
{
			    Import *importPtr;

			    if (yyvsp[0].typePtr && yyvsp[0].typePtr->export.name) {
				importPtr = findImportByName(yyvsp[0].typePtr->export.name,
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				}
			    }

			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 154:
#line 2665 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 155:
#line 2667 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 156:
#line 2675 "parser-smi.y"
{ yyval.typePtr = yyvsp[0].typePtr; ;
    break;}
case 157:
#line 2680 "parser-smi.y"
{
			    Import *importPtr;

			    if (yyvsp[0].typePtr && yyvsp[0].typePtr->export.name) {
				importPtr = findImportByName(yyvsp[0].typePtr->export.name,
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				}
			    }

			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 158:
#line 2697 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 159:
#line 2706 "parser-smi.y"
{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    yyval.typePtr = smiHandle->typeInteger32Ptr;
			;
    break;}
case 160:
#line 2718 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 161:
#line 2722 "parser-smi.y"
{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 162:
#line 2736 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 163:
#line 2740 "parser-smi.y"
{
			    List *p;
			    
			    yyval.typePtr = duplicateType(smiHandle->typeEnumPtr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 164:
#line 2750 "parser-smi.y"
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
			    yyval.typePtr = smiHandle->typeInteger32Ptr;
			;
    break;}
case 165:
#line 2768 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 166:
#line 2772 "parser-smi.y"
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

			    yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 167:
#line 2791 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 168:
#line 2795 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModuleAndName(
			        thisParserPtr->modulePtr, yyvsp[-2].id);
			    if (!parentPtr) {
			        importPtr = findImportByName(yyvsp[-2].id,
							     thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					importPtr->export.module, yyvsp[-2].id);
				}
			    }
			    if (parentPtr) {
				if ((parentPtr->export.basetype !=
				     SMI_BASETYPE_ENUM) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_BITS)) {
				    smiPrintError(thisParserPtr,
						  ERR_ILLEGAL_ENUM_FOR_PARENT_TYPE,
						  yyvsp[-2].id);
				    yyval.typePtr = duplicateType(smiHandle->typeEnumPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
				smiFree(yyvsp[-2].id);
			    } else {
				/* 
				 * forward referenced type. create it,
				 * marked with FLAG_INCOMPLETE.
				 */
				parentPtr = addType(yyvsp[-2].id,
						    SMI_BASETYPE_UNKNOWN,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 169:
#line 2845 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
                            defaultBasetype = SMI_BASETYPE_ENUM;
                            parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr =
				    findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr =
					findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
				}
			    }
			    if (parentPtr) {
				if ((parentPtr->export.basetype !=
				     SMI_BASETYPE_ENUM) &&
				    (parentPtr->export.basetype !=
				     SMI_BASETYPE_BITS)) {
				    smiPrintError(thisParserPtr,
						  ERR_ILLEGAL_ENUM_FOR_PARENT_TYPE,
						  yyvsp[-1].id);
				    yyval.typePtr = duplicateType(smiHandle->typeEnumPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				yyval.typePtr = duplicateType(smiHandle->typeEnumPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			    smiFree(yyvsp[-3].id);
			    smiFree(yyvsp[-1].id);
			;
    break;}
case 170:
#line 2890 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					importPtr->export.module, yyvsp[-1].id);
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
						  yyvsp[-1].id);
				    yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
						       thisParserPtr);
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				} else {
				    defaultBasetype =
					parentPtr->export.basetype;
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
				smiFree(yyvsp[-1].id);
			    } else {
				/* 
				 * forward referenced type. create it,
				 * marked with FLAG_INCOMPLETE.
				 */
				parentPtr = addType(yyvsp[-1].id,
						    SMI_BASETYPE_UNKNOWN,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				defaultBasetype = SMI_BASETYPE_INTEGER32;
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 171:
#line 2946 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					yyvsp[-3].id, yyvsp[-1].id);
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
						  yyvsp[-1].id);
				    yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
						       thisParserPtr);
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				} else {
				    defaultBasetype =
					parentPtr->export.basetype;
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
						   thisParserPtr);
				defaultBasetype = SMI_BASETYPE_INTEGER32;
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    smiFree(yyvsp[-3].id);
			    smiFree(yyvsp[-1].id);
			;
    break;}
case 172:
#line 2995 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = smiHandle->typeOctetStringPtr;
			;
    break;}
case 173:
#line 3000 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			;
    break;}
case 174:
#line 3004 "parser-smi.y"
{
			    
			    yyval.typePtr = duplicateType(smiHandle->typeOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 175:
#line 3012 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					importPtr->export.module, yyvsp[-1].id);
				}
			    }
			    if (parentPtr) {
				if (parentPtr->export.basetype !=
				    SMI_BASETYPE_OCTETSTRING) {
				    smiPrintError(thisParserPtr,
					      ERR_ILLEGAL_SIZE_FOR_PARENT_TYPE,
						  yyvsp[-1].id);
				    yyval.typePtr = duplicateType(smiHandle->typeOctetStringPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
				smiFree(yyvsp[-1].id);
			    } else {
				/* 
				 * forward referenced type. create it,
				 * marked with FLAG_INCOMPLETE.
				 */
				parentPtr = addType(yyvsp[-1].id,
						    SMI_BASETYPE_UNKNOWN,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 176:
#line 3059 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (importPtr &&
				    importPtr->kind == KIND_TYPE) {
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					yyvsp[-3].id, yyvsp[-1].id);
				}
			    }
			    if (parentPtr) {
				if (parentPtr->export.basetype !=
				    SMI_BASETYPE_OCTETSTRING) {
				    smiPrintError(thisParserPtr,
					      ERR_ILLEGAL_SIZE_FOR_PARENT_TYPE,
						  yyvsp[-1].id);
				    yyval.typePtr = duplicateType(smiHandle->typeOctetStringPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				yyval.typePtr = duplicateType(smiHandle->typeOctetStringPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    smiFree(yyvsp[-3].id);
			    smiFree(yyvsp[-1].id);
			;
    break;}
case 177:
#line 3099 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.typePtr = smiHandle->typeObjectIdentifierPtr;
			;
    break;}
case 178:
#line 3107 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;
    break;}
case 179:
#line 3113 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;
    break;}
case 180:
#line 3119 "parser-smi.y"
{
			    char s[9];
			    int i, len, j;
			    
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr =
				    smiMalloc((len+7)/8+1);
				for (i = 0; i < len; i += 8) {
				    strncpy(s, &yyvsp[0].text[i], 8);
				    for (j = 1; j < 8; j++) {
					if (!s[j]) s[j] = '0';
				    }
				    s[8] = 0;
				    yyval.valuePtr->value.ptr[i/8] =
					(unsigned char)strtol(s, 0, 2);
				}
				yyval.valuePtr->len = (len+7)/8;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 2);
			    }
			;
    break;}
case 181:
#line 3145 "parser-smi.y"
{
			    char s[3];
			    int i, len;
			    
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr = smiMalloc((len+1)/2+1);
				for (i = 0; i < len; i += 2) {
				    strncpy(s, &yyvsp[0].text[i], 2);
				    if (!s[1]) s[1] = '0';
				    s[2] = 0;
				    yyval.valuePtr->value.ptr[i/2] =
					(unsigned char)strtol(s, 0, 16);
				}
				yyval.valuePtr->len = (len+1)/2;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 16);
			    }
			;
    break;}
case 182:
#line 3168 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    if ((defaultBasetype != SMI_BASETYPE_ENUM) &&
				(defaultBasetype != SMI_BASETYPE_OBJECTIDENTIFIER)) {
				smiPrintError(thisParserPtr, ERR_DEFVAL_SYNTAX);
				yyval.valuePtr->basetype = defaultBasetype;
				if (defaultBasetype == SMI_BASETYPE_ENUM) {
				    yyval.valuePtr->len = 1;
				    yyval.valuePtr->value.unsigned32 = 0;
				} else {
				    yyval.valuePtr->len = 0;
				    yyval.valuePtr->value.ptr = NULL;
				}
			    } else {
				yyval.valuePtr->basetype = defaultBasetype;
				yyval.valuePtr->len = -1;  /* indicates unresolved ptr */
				yyval.valuePtr->value.ptr = (unsigned char *)yyvsp[0].id; /* JS: needs strdup? */
			    }
			;
    break;}
case 183:
#line 3188 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.valuePtr->value.ptr = (unsigned char *)smiStrdup(yyvsp[0].text);
			    yyval.valuePtr->len = strlen(yyvsp[0].text);
			;
    break;}
case 184:
#line 3211 "parser-smi.y"
{
			    /*
			     * SMIv1 allows something like { 0 0 } !
			     * SMIv2 does not!
			     */
			    /* TODO: make it work correctly for SMIv1 */
			    smiPrintError(thisParserPtr,
					  ERR_OID_DEFVAL_TOO_LONG);
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.valuePtr->len = 2;
			    yyval.valuePtr->value.oid = smiMalloc(2 * sizeof(SmiSubid));
			    yyval.valuePtr->value.oid[0] = 0;
			    yyval.valuePtr->value.oid[1] = 0;
			;
    break;}
case 185:
#line 3233 "parser-smi.y"
{
			    yyval.typePtr = smiHandle->typeInteger32Ptr;
			;
    break;}
case 186:
#line 3237 "parser-smi.y"
{
			    Import *importPtr;
			    
			    /* TODO: any need to distinguish from INTEGER? */
			    yyval.typePtr = smiHandle->typeInteger32Ptr;

			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Integer32");
			    }

			;
    break;}
case 187:
#line 3255 "parser-smi.y"
{
			    yyval.typePtr = smiHandle->typeOctetStringPtr;
			;
    break;}
case 188:
#line 3259 "parser-smi.y"
{
			    yyval.typePtr = smiHandle->typeObjectIdentifierPtr;
			;
    break;}
case 189:
#line 3265 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			;
    break;}
case 190:
#line 3273 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			;
    break;}
case 191:
#line 3281 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			;
    break;}
case 192:
#line 3289 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByName("Gauge32");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
				yyval.typePtr = NULL;
			    } else {
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    }
			    importPtr = findImportByName("Gauge32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    }
			;
    break;}
case 193:
#line 3311 "parser-smi.y"
{
			    Import *importPtr;

			    yyval.typePtr = smiHandle->typeUnsigned32Ptr;
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Unsigned32");
			    }
			;
    break;}
case 194:
#line 3326 "parser-smi.y"
{
			    Import *importPtr;
			    
			    yyval.typePtr = duplicateType(smiHandle->typeUnsigned32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Unsigned32");
			    }
			;
    break;}
case 195:
#line 3344 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			;
    break;}
case 196:
#line 3352 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Opaque");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_OPAQUE_OBSOLETE);
			    }
			;
    break;}
case 197:
#line 3363 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByName("Opaque");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
				yyval.typePtr = NULL;
			    } else {
				    smiPrintError(thisParserPtr,
						  ERR_OPAQUE_OBSOLETE);
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    }
			    importPtr = findImportByName("Opaque",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    }
			;
    break;}
case 198:
#line 3387 "parser-smi.y"
{
			    Import *importPtr;

			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter64");
			    }

			    importPtr = findImportByName("Counter64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Counter64");
			    }
			;
    break;}
case 199:
#line 3413 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			;
    break;}
case 200:
#line 3421 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			;
    break;}
case 201:
#line 3429 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			;
    break;}
case 202:
#line 3437 "parser-smi.y"
{
			    Import *importPtr;
			    
			    yyval.typePtr = smiHandle->typeUnsigned32Ptr;
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Unsigned32");
			    }
			;
    break;}
case 203:
#line 3452 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			;
    break;}
case 204:
#line 3460 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Opaque");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_OPAQUE_OBSOLETE);
			    }
			;
    break;}
case 205:
#line 3471 "parser-smi.y"
{
			    Import *importPtr;

			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter64");
			    }

			    importPtr = findImportByName("Counter64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Counter64");
			    }
			;
    break;}
case 206:
#line 3493 "parser-smi.y"
{
			    List *listPtr, *nextListPtr;

			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);

			    for (listPtr = yyvsp[0].listPtr; listPtr;
				 listPtr = nextListPtr) {
				nextListPtr = listPtr->nextPtr;
				smiFree((Range *)(listPtr->ptr));
				smiFree(listPtr);
			    }

			    yyval.listPtr = NULL;
			;
    break;}
case 207:
#line 3510 "parser-smi.y"
{
			    List *listPtr, *nextListPtr;

			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);

			    for (listPtr = yyvsp[0].listPtr; listPtr;
				 listPtr = nextListPtr) {
				nextListPtr = listPtr->nextPtr;
				smiFree((Range *)(listPtr->ptr));
				smiFree(listPtr);
			    }

			    yyval.listPtr = NULL;
			;
    break;}
case 208:
#line 3527 "parser-smi.y"
{
			    List *listPtr, *nextListPtr;

			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);

			    for (listPtr = yyvsp[0].listPtr; listPtr;
				 listPtr = nextListPtr) {
				nextListPtr = listPtr->nextPtr;
				smiFree(((NamedNumber *)(listPtr->ptr))->export.name);
				smiFree((NamedNumber *)(listPtr->ptr));
				smiFree(listPtr);
			    }

			    yyval.listPtr = NULL;
			;
    break;}
case 209:
#line 3545 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 210:
#line 3559 "parser-smi.y"
{ yyval.listPtr = yyvsp[-1].listPtr; ;
    break;}
case 211:
#line 3569 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 212:
#line 3575 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 213:
#line 3581 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 214:
#line 3595 "parser-smi.y"
{
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[0].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[0].valuePtr);
			;
    break;}
case 215:
#line 3602 "parser-smi.y"
{
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[-2].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[-2].valuePtr);
			    smiFree(yyvsp[0].valuePtr);
			;
    break;}
case 216:
#line 3612 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;
    break;}
case 217:
#line 3618 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;
    break;}
case 218:
#line 3624 "parser-smi.y"
{
			    char s[3];
			    int i, len;
			    
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr = smiMalloc((len+1)/2+1);
				for (i = 0; i < len; i += 2) {
				    strncpy(s, &yyvsp[0].text[i], 2);
				    if (!s[1]) s[1] = '0';
				    s[2] = 0;
				    yyval.valuePtr->value.ptr[i/2] =
					(unsigned char)strtol(s, 0, 16);
				}
				yyval.valuePtr->len = (len+1)/2;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 16);
			    }
			;
    break;}
case 219:
#line 3647 "parser-smi.y"
{
			    char s[9];
			    int i, len, j;
			    
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr = smiMalloc((len+7)/8+1);
				for (i = 0; i < len; i += 8) {
				    strncpy(s, &yyvsp[0].text[i], 8);
				    for (j = 1; j < 8; j++) {
					if (!s[j]) s[j] = '0';
				    }
				    s[8] = 0;
				    yyval.valuePtr->value.ptr[i/8] =
					(unsigned char)strtol(s, 0, 2);
				}
				yyval.valuePtr->len = (len+7)/8;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 2);
			    }
			;
    break;}
case 220:
#line 3674 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 221:
#line 3680 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 222:
#line 3686 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 223:
#line 3699 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_ENUMNAME_32, ERR_ENUMNAME_64);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (strchr(yyvsp[0].id, '-')) {
				    smiPrintError(thisParserPtr,
					  ERR_NAMEDNUMBER_INCLUDES_HYPHEN,
						  yyvsp[0].id);
				}
			    }
			;
    break;}
case 224:
#line 3712 "parser-smi.y"
{
			    yyval.namedNumberPtr = smiMalloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id;
			    yyval.namedNumberPtr->export.value = *yyvsp[-1].valuePtr;
			    smiFree(yyvsp[-1].valuePtr);
			;
    break;}
case 225:
#line 3721 "parser-smi.y"
{
			    if (yyvsp[0].unsigned32 > MAX_INTEGER32) {
				smiPrintError(thisParserPtr,
					      ERR_INTEGER32_TOO_LARGE, yyvsp[0].unsigned32);
			    }
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].unsigned32;
			;
    break;}
case 226:
#line 3731 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    /* TODO: non-negative is suggested */
			;
    break;}
case 227:
#line 3740 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (!strcmp(yyvsp[0].id, "current")) {
				    yyval.status = SMI_STATUS_CURRENT;
				} else if (!strcmp(yyvsp[0].id, "deprecated")) {
				    yyval.status = SMI_STATUS_DEPRECATED;
				} else if (!strcmp(yyvsp[0].id, "obsolete")) {
				    yyval.status = SMI_STATUS_OBSOLETE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV2_STATUS,
						  yyvsp[0].id);
				    if (!strcmp(yyvsp[0].id, "mandatory")
					|| !strcmp(yyvsp[0].id, "optional")) {
					/* best guess */
					yyval.status = SMI_STATUS_CURRENT;
				    } else {
					yyval.status = SMI_STATUS_UNKNOWN;
				    }
				}
			    } else {
				if (!strcmp(yyvsp[0].id, "mandatory")) {
				    yyval.status = SMI_STATUS_MANDATORY;
				} else if (!strcmp(yyvsp[0].id, "optional")) {
				    yyval.status = SMI_STATUS_OPTIONAL;
				} else if (!strcmp(yyvsp[0].id, "obsolete")) {
				    yyval.status = SMI_STATUS_OBSOLETE;
				} else if (!strcmp(yyvsp[0].id, "deprecated")) {
				    yyval.status = SMI_STATUS_OBSOLETE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV1_STATUS,
						  yyvsp[0].id);
				    if (!strcmp(yyvsp[0].id, "current")) {
					/* best guess */
					yyval.status = SMI_STATUS_MANDATORY; 
				    } else {
					yyval.status = SMI_STATUS_UNKNOWN;
				    }
				}
			    }
			    smiFree(yyvsp[0].id);
			;
    break;}
case 228:
#line 3787 "parser-smi.y"
{
			    if (!strcmp(yyvsp[0].id, "current")) {
				yyval.status = SMI_STATUS_CURRENT;
			    } else if (!strcmp(yyvsp[0].id, "obsolete")) {
				yyval.status = SMI_STATUS_OBSOLETE;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_INVALID_CAPABILITIES_STATUS,
					      yyvsp[0].id);
				yyval.status = SMI_STATUS_UNKNOWN;
			    }
			    smiFree(yyvsp[0].id);
			;
    break;}
case 229:
#line 3803 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			    
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_FORMAT);
			    }
			;
    break;}
case 230:
#line 3812 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 231:
#line 3818 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			    
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_UNITS);
			    }
			;
    break;}
case 232:
#line 3827 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 233:
#line 3833 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (!strcmp(yyvsp[0].id, "not-accessible")) {
				    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
				} else if (!strcmp(yyvsp[0].id,
						   "accessible-for-notify")) {
				    yyval.access = SMI_ACCESS_NOTIFY;
				} else if (!strcmp(yyvsp[0].id, "read-only")) {
				    yyval.access = SMI_ACCESS_READ_ONLY;
				} else if (!strcmp(yyvsp[0].id, "read-write")) {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				} else if (!strcmp(yyvsp[0].id, "read-create")) {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				    thisParserPtr->flags |= FLAG_CREATABLE;
				    /* TODO:remember it's really read-create */
				} else if (!strcmp(yyvsp[0].id, "write-only")) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_WRITE_ONLY);
				    yyval.access = SMI_ACCESS_READ_WRITE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV2_ACCESS,
						  yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				}
			    } else {
				if (!strcmp(yyvsp[0].id, "not-accessible")) {
				    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
				} else if (!strcmp(yyvsp[0].id, "read-only")) {
				    yyval.access = SMI_ACCESS_READ_ONLY;
				} else if (!strcmp(yyvsp[0].id, "read-write")) {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				} else if (!strcmp(yyvsp[0].id, "write-only")) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV1_WRITE_ONLY);
				    yyval.access = SMI_ACCESS_READ_WRITE;
				} else {
				    smiPrintError(thisParserPtr,
						  ERR_INVALID_SMIV1_ACCESS,
						  yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				}
			    }
			    smiFree(yyvsp[0].id);
			;
    break;}
case 234:
#line 3882 "parser-smi.y"
{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			;
    break;}
case 235:
#line 3890 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = yyvsp[-1].listPtr;
			    yyval.index.rowPtr    = NULL;
			;
    break;}
case 236:
#line 3899 "parser-smi.y"
{
			    yyval.index.indexkind    = SMI_INDEX_AUGMENT;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
			;
    break;}
case 237:
#line 3906 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;
    break;}
case 238:
#line 3912 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 239:
#line 3919 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 240:
#line 3932 "parser-smi.y"
{
			    impliedFlag = 1;
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 241:
#line 3937 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 242:
#line 3947 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 243:
#line 3953 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 244:
#line 3959 "parser-smi.y"
{ yyval.valuePtr = yyvsp[-1].valuePtr; ;
    break;}
case 245:
#line 3961 "parser-smi.y"
{ yyval.valuePtr = NULL; ;
    break;}
case 246:
#line 3966 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 247:
#line 3968 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
			    yyval.valuePtr->value.ptr = (void *)yyvsp[-1].listPtr;
			;
    break;}
case 248:
#line 3976 "parser-smi.y"
{ yyval.listPtr = yyvsp[0].listPtr; ;
    break;}
case 249:
#line 3978 "parser-smi.y"
{ yyval.listPtr = NULL; ;
    break;}
case 250:
#line 3982 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].id;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 251:
#line 3988 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].id;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 252:
#line 4001 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 253:
#line 4007 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 254:
#line 4013 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;

			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_REFERENCE);
			    }
			;
    break;}
case 255:
#line 4022 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 256:
#line 4026 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 257:
#line 4028 "parser-smi.y"
{
			    if (!thisModulePtr->firstRevisionPtr) {
				addRevision(thisModulePtr->lastUpdated,
					    smiStrdup(
	           "[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }
			    yyval.err = 0;
			;
    break;}
case 258:
#line 4040 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 259:
#line 4042 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 260:
#line 4047 "parser-smi.y"
{
			    /*
			     * If the first REVISION (which is the newest)
			     * has another date than the LAST-UPDATED clause,
			     * we add an implicit Revision structure.
			     */
			    if ((!thisModulePtr->firstRevisionPtr) &&
				(yyvsp[-2].date != thisModulePtr->lastUpdated)) {
				addRevision(thisModulePtr->lastUpdated,
					    smiStrdup(
	           "[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }

			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			    
			    if (addRevision(yyvsp[-2].date, yyvsp[0].text, thisParserPtr))
				yyval.err = 0;
			    else
				yyval.err = -1;
			;
    break;}
case 261:
#line 4074 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 262:
#line 4078 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 263:
#line 4084 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 264:
#line 4090 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 265:
#line 4103 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 266:
#line 4109 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 267:
#line 4115 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 268:
#line 4121 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 269:
#line 4134 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 270:
#line 4140 "parser-smi.y"
{
			    yyval.text = smiStrdup(yyvsp[0].text);
			;
    break;}
case 271:
#line 4146 "parser-smi.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 272:
#line 4151 "parser-smi.y"
{
			    parentNodePtr = smiHandle->rootNodePtr;
			;
    break;}
case 273:
#line 4155 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    parentNodePtr = yyvsp[0].objectPtr->nodePtr;
			;
    break;}
case 274:
#line 4163 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 275:
#line 4168 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 276:
#line 4176 "parser-smi.y"
{
			    Object *objectPtr;
			    Import *importPtr;
			    
			    if (parentNodePtr != smiHandle->rootNodePtr) {
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_NOT_FIRST, yyvsp[0].id);
			    }
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[0].id);
			    if (objectPtr) {
				yyval.objectPtr = objectPtr;
				smiFree(yyvsp[0].id);
			    } else {
				importPtr = findImportByName(yyvsp[0].id,
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
						complianceModulePtr, yyvsp[0].id);
					if (objectPtr) {
					    importPtr = addImport(
						yyvsp[0].id,
						thisParserPtr);
					    setImportModulename(importPtr,
								complianceModulePtr->export.name);
					    importPtr->use++;
					} else {
					    objectPtr = addObject(yyvsp[0].id,
								  smiHandle->pendingNodePtr, 0,
								  FLAG_INCOMPLETE,
								  thisParserPtr);
					    smiPrintError(thisParserPtr,
							  ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[0].id,
							  complianceModulePtr->export.name);
					}
				    } else if (capabilitiesModulePtr) {
					objectPtr =
					    findObjectByModuleAndName(
						capabilitiesModulePtr, yyvsp[0].id);
					if (objectPtr) {
					    importPtr = addImport(
						yyvsp[0].id,
						thisParserPtr);
					    setImportModulename(importPtr,
								capabilitiesModulePtr->
								export.name);
					    importPtr->use++;
					} else {
					    objectPtr = addObject(yyvsp[0].id,
								  smiHandle->pendingNodePtr, 0,
								  FLAG_INCOMPLETE,
								  thisParserPtr);
					    smiPrintError(thisParserPtr,
							  ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[0].id,
							  capabilitiesModulePtr->export.name);
					}
				    } else {
					/* 
					 * forward referenced node.
					 * create it,
					 * marked with FLAG_INCOMPLETE.
					 */
					objectPtr = addObject(yyvsp[0].id,
							      smiHandle->pendingNodePtr,
							      0,
							      FLAG_INCOMPLETE,
							      thisParserPtr);
				    }
				    yyval.objectPtr = objectPtr;
				} else {
				    /*
				     * imported object.
				     */
				    importPtr->use++;
				    yyval.objectPtr = findObjectByModulenameAndName(
					importPtr->export.module, yyvsp[0].id);
				    smiFree(yyvsp[0].id);
				}
			    }
			    if (yyval.objectPtr)
				parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 277:
#line 4266 "parser-smi.y"
{
			    Object *objectPtr;
			    Import *importPtr;
			    char *md;
			    
			    if (parentNodePtr != smiHandle->rootNodePtr) {
				md = smiMalloc(sizeof(char) *
					       (strlen(yyvsp[-2].id) + strlen(yyvsp[0].id) + 2));
				sprintf(md, "%s.%s", yyvsp[-2].id, yyvsp[0].id);
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_NOT_FIRST, md);
				smiFree(md);
			    } else {
				objectPtr = findObjectByModulenameAndName(
				    yyvsp[-2].id, yyvsp[0].id);
				if (objectPtr) {
				    yyval.objectPtr = objectPtr;
				    smiFree(yyvsp[-2].id);
				    smiFree(yyvsp[0].id);
				} else {
				    importPtr = findImportByModulenameAndName(
					yyvsp[-2].id, yyvsp[0].id, thisModulePtr);
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
						    complianceModulePtr, yyvsp[-2].id);
					    if (objectPtr) {
						importPtr = addImport(
						    yyvsp[-2].id,
						    thisParserPtr);
						setImportModulename(importPtr,
						    complianceModulePtr->export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    smiHandle->pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[-2].id,
					     complianceModulePtr->export.name);
					    }
					} else if (capabilitiesModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    capabilitiesModulePtr, yyvsp[-2].id);
					    if (objectPtr) {
						importPtr = addImport(
						    yyvsp[-2].id,
						    thisParserPtr);
						setImportModulename(importPtr,
						        capabilitiesModulePtr->
								  export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    smiHandle->pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[-2].id,
					   capabilitiesModulePtr->export.name);
					    }
					} else {
					    /* 
					     * forward referenced node.
					     * create it,
					     * marked with FLAG_INCOMPLETE.
					     */
					    objectPtr = addObject(yyvsp[0].id,
							    smiHandle->pendingNodePtr,
							      0,
							      FLAG_INCOMPLETE,
							      thisParserPtr);
					    smiFree(yyvsp[-2].id);
					}
					yyval.objectPtr = objectPtr;
				    } else {
					/*
					 * imported object.
					 */
					importPtr->use++;
					yyval.objectPtr = findObjectByModulenameAndName(
					    importPtr->export.module, yyvsp[0].id);
					smiFree(yyvsp[-2].id);
					smiFree(yyvsp[0].id);
				    }
				}
				if (yyval.objectPtr)
				    parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			;
    break;}
case 278:
#line 4366 "parser-smi.y"
{
			    Node *nodePtr;
			    Object *objectPtr;

			    nodePtr = findNodeByParentAndSubid(parentNodePtr,
							       yyvsp[0].unsigned32);
			    if (nodePtr && nodePtr->lastObjectPtr &&
	       		(nodePtr->lastObjectPtr->modulePtr == thisModulePtr)) {
				/*
				 * hopefully, the last defined Object for
				 * this Node is the one we expect.
				 */
				yyval.objectPtr = nodePtr->lastObjectPtr;
			    } else {
				objectPtr = addObject(NULL,
						      parentNodePtr,
						      yyvsp[0].unsigned32,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				yyval.objectPtr = objectPtr;
			    }
			    parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 279:
#line 4390 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    /* TODO: search in local module and
			     *       in imported modules
			     */
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-3].id);
			    if (objectPtr) {
				smiPrintError(thisParserPtr,
					      ERR_EXISTENT_OBJECT, yyvsp[-3].id);
				yyval.objectPtr = objectPtr;
				if (yyval.objectPtr->nodePtr->subid != yyvsp[-1].unsigned32) {
				    smiPrintError(thisParserPtr,
					  ERR_SUBIDENTIFIER_VS_OIDLABEL,
						  yyvsp[-1].unsigned32, yyvsp[-3].id);
				}
				smiFree(yyvsp[-3].id);
			    } else {
				objectPtr = addObject(yyvsp[-3].id, parentNodePtr,
						      yyvsp[-1].unsigned32, 0,
						      thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_VALUEASSIGNMENT);
				yyval.objectPtr = objectPtr;
			    }
			    if (yyval.objectPtr) 
				parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 280:
#line 4420 "parser-smi.y"
{
			    Object *objectPtr;
			    char *md;

			    md = smiMalloc(sizeof(char) *
					   (strlen(yyvsp[-5].id) + strlen(yyvsp[-3].id) + 2));
			    sprintf(md, "%s.%s", yyvsp[-5].id, yyvsp[-3].id);
			    objectPtr = findObjectByModulenameAndName(yyvsp[-5].id, yyvsp[-3].id);
			    if (objectPtr) {
				smiPrintError(thisParserPtr,
					      ERR_EXISTENT_OBJECT, yyvsp[-5].id);
				yyval.objectPtr = objectPtr;
				if (yyval.objectPtr->nodePtr->subid != yyvsp[-1].unsigned32) {
				    smiPrintError(thisParserPtr,
					  ERR_SUBIDENTIFIER_VS_OIDLABEL,
						  yyvsp[-1].unsigned32, md);
				}
				smiFree(yyvsp[-5].id);
				smiFree(yyvsp[-3].id);
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_ILLEGALLY_QUALIFIED, md);
				objectPtr = addObject(yyvsp[-3].id, parentNodePtr,
						   yyvsp[-1].unsigned32, 0,
						   thisParserPtr);
				yyval.objectPtr = objectPtr;
				smiFree(yyvsp[-5].id);
			    }
			    smiFree(md);
			    if (yyval.objectPtr)
				parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 281:
#line 4455 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 282:
#line 4459 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 283:
#line 4461 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 284:
#line 4465 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 285:
#line 4467 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 286:
#line 4471 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 287:
#line 4480 "parser-smi.y"
{
			    Import *importPtr;
			    
			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV2;

			    importPtr = findImportByName("OBJECT-GROUP",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_MACRO_NOT_IMPORTED,
					      "OBJECT-GROUP", "SNMPv2-CONF");
			    }
			;
    break;}
case 288:
#line 4499 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 289:
#line 4507 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-14].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTGROUP);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-10].listPtr);
			    yyval.err = 0;
			;
    break;}
case 290:
#line 4533 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 291:
#line 4542 "parser-smi.y"
{
			    Import *importPtr;
			    
			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV2;

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
			;
    break;}
case 292:
#line 4562 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 293:
#line 4570 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-14].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONGROUP);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-10].listPtr);
			    yyval.err = 0;
			;
    break;}
case 294:
#line 4597 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 295:
#line 4606 "parser-smi.y"
{
			    Import *importPtr;
			    
			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV2;
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
			;
    break;}
case 296:
#line 4624 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 297:
#line 4633 "parser-smi.y"
{
			    Object *objectPtr;
			    Option *optionPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    setObjectName(objectPtr, yyvsp[-14].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-9].status);
			    setObjectDescription(objectPtr, yyvsp[-7].text, thisParserPtr);
			    if (yyvsp[-5].text) {
				setObjectReference(objectPtr, yyvsp[-5].text, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-4].compl.mandatorylistPtr);
			    objectPtr->optionlistPtr = yyvsp[-4].compl.optionlistPtr;
			    objectPtr->refinementlistPtr =
				                          yyvsp[-4].compl.refinementlistPtr;

			    if (yyvsp[-4].compl.optionlistPtr) {
				for (listPtr = yyvsp[-4].compl.optionlistPtr;
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
			    if (yyvsp[-4].compl.refinementlistPtr) {
				for (listPtr = yyvsp[-4].compl.refinementlistPtr;
				     listPtr;
				     listPtr = listPtr->nextPtr) {
				    refinementPtr =
					((Refinement *)(listPtr->ptr));
				    refinementPtr->compliancePtr = objectPtr;
				}
			    }

			    yyval.err = 0;
			;
    break;}
case 298:
#line 4692 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 299:
#line 4698 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 300:
#line 4702 "parser-smi.y"
{
			    List *listPtr;
			    
			    /* concatenate lists in $1 and $2 */
			    if (yyvsp[-1].compl.mandatorylistPtr) {
				for (listPtr = yyvsp[-1].compl.mandatorylistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = yyvsp[0].compl.mandatorylistPtr;
				yyval.compl.mandatorylistPtr = yyvsp[-1].compl.mandatorylistPtr;
			    } else {
				yyval.compl.mandatorylistPtr = yyvsp[0].compl.mandatorylistPtr;
			    }
			    if (yyvsp[-1].compl.optionlistPtr) {
				for (listPtr = yyvsp[-1].compl.optionlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = yyvsp[0].compl.optionlistPtr;
				yyval.compl.optionlistPtr = yyvsp[-1].compl.optionlistPtr;
			    } else {
				yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    }
			    if (yyvsp[-1].compl.refinementlistPtr) {
				for (listPtr = yyvsp[-1].compl.refinementlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = yyvsp[0].compl.refinementlistPtr;
				yyval.compl.refinementlistPtr = yyvsp[-1].compl.refinementlistPtr;
			    } else {
				yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			    }
			;
    break;}
case 301:
#line 4737 "parser-smi.y"
{
			    /*
			     * Remember the module. SMIv2 is broken by
			     * design to allow subsequent clauses to
			     * refer identifiers that are not
			     * imported.  Although, SMIv2 does not
			     * require, we will fake it by inserting
			     * appropriate imports.
			     */
			    if (yyvsp[0].modulePtr == thisModulePtr)
				complianceModulePtr = NULL;
			    else
				complianceModulePtr = yyvsp[0].modulePtr;
			;
    break;}
case 302:
#line 4753 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = yyvsp[-1].listPtr;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			    if (complianceModulePtr) {
				checkImports(complianceModulePtr,
					     thisParserPtr);
				complianceModulePtr = NULL;
			    }
			;
    break;}
case 303:
#line 4766 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			    smiFree(yyvsp[-1].id);
			;
    break;}
case 304:
#line 4775 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			    smiFree(yyvsp[0].id);
			;
    break;}
case 305:
#line 4784 "parser-smi.y"
{
			    yyval.modulePtr = thisModulePtr;
			;
    break;}
case 306:
#line 4790 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 307:
#line 4794 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 308:
#line 4800 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 309:
#line 4806 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 310:
#line 4819 "parser-smi.y"
{
			    /* TODO: check that objectIdentifier is
			       found, is defined in complianceModulePtr,
			       and is a group node. */
			    Import *importPtr;

			    yyval.objectPtr = yyvsp[0].objectPtr;
			    if (complianceModulePtr) {
				yyval.objectPtr = findObjectByModuleAndName(
				                           complianceModulePtr,
							   yyvsp[0].objectPtr->export.name);
			    }
			    if (complianceModulePtr && yyvsp[0].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
				    complianceModulePtr->export.name,
				    yyvsp[0].objectPtr->export.name, thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }
			;
    break;}
case 311:
#line 4842 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			;
    break;}
case 312:
#line 4848 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 313:
#line 4856 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 314:
#line 4860 "parser-smi.y"
{
			    List *listPtr;
			    int stop;
			    
			    yyval.compl.mandatorylistPtr = NULL;

			    /* check for duplicates in optionlist */
			    stop = 0;
			    if (yyvsp[0].compl.optionlistPtr) {
				for (listPtr = yyvsp[-1].compl.optionlistPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    if (((Option *)listPtr->ptr)->objectPtr ==
					((Option *)yyvsp[0].compl.optionlistPtr->ptr)->objectPtr) {
					smiPrintError(thisParserPtr,
						      ERR_OPTIONALGROUP_ALREADY_EXISTS,
						      ((Option *)yyvsp[0].compl.optionlistPtr->ptr)->objectPtr->export.name);
					stop = 1;
					yyval.compl.optionlistPtr = yyvsp[-1].compl.optionlistPtr;
				    }
				}
			    }
	
                            /* concatenate optionlists */
			    if (yyvsp[-1].compl.optionlistPtr) {
				for (listPtr = yyvsp[-1].compl.optionlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				if (!stop) {
				    listPtr->nextPtr = yyvsp[0].compl.optionlistPtr;
				}
				yyval.compl.optionlistPtr = yyvsp[-1].compl.optionlistPtr;
			    } else {
				yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    }

			    /* check for duplicates in refinementlist */
			    stop = 0;
			    if (yyvsp[0].compl.refinementlistPtr) {
				for (listPtr = yyvsp[-1].compl.refinementlistPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    if (((Refinement *)listPtr->ptr)->objectPtr ==
					((Refinement *)yyvsp[0].compl.refinementlistPtr->ptr)->objectPtr) {
					smiPrintError(thisParserPtr,
						      ERR_REFINEMENT_ALREADY_EXISTS,
						      ((Refinement *)yyvsp[0].compl.refinementlistPtr->ptr)->objectPtr->export.name);
					stop = 1;
					yyval.compl.refinementlistPtr = yyvsp[-1].compl.refinementlistPtr;
				    }
				}
			    }
			    
                            /* concatenate refinementlists */
			    if (yyvsp[-1].compl.refinementlistPtr) {
				for (listPtr = yyvsp[-1].compl.refinementlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				if (!stop) {
				    listPtr->nextPtr = yyvsp[0].compl.refinementlistPtr;
				}
				yyval.compl.refinementlistPtr = yyvsp[-1].compl.refinementlistPtr;
			    } else {
				yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			    }
			;
    break;}
case 315:
#line 4927 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].listPtr;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 316:
#line 4933 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 317:
#line 4942 "parser-smi.y"
{
			    Import *importPtr;
			    
			    if (complianceModulePtr && yyvsp[-2].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
						    complianceModulePtr->export.name,
						    yyvsp[-2].objectPtr->export.name,
						    thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }
			    
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			    
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = smiMalloc(sizeof(Option));
			    ((Option *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-2].objectPtr;
			    if (! (thisModulePtr->flags & SMI_FLAG_NODESCR)) {
				((Option *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			    } else {
				smiFree(yyvsp[0].text);
			    }
			;
    break;}
case 318:
#line 4976 "parser-smi.y"
{
			    Import *importPtr;

			    if (complianceModulePtr && yyvsp[-5].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
						    complianceModulePtr->export.name,
						    yyvsp[-5].objectPtr->export.name,
						    thisModulePtr);
				if (importPtr) 
				    importPtr->use++;
			    }
			    
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			    
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = smiMalloc(sizeof(Refinement));
			    ((Refinement *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-5].objectPtr;
			    ((Refinement *)(yyval.listPtr->ptr))->typePtr = yyvsp[-4].typePtr;
			    ((Refinement *)(yyval.listPtr->ptr))->writetypePtr = yyvsp[-3].typePtr;
			    ((Refinement *)(yyval.listPtr->ptr))->export.access = yyvsp[-2].access;
			    if (! (thisParserPtr->flags & SMI_FLAG_NODESCR)) {
				((Refinement *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			    } else {
				smiFree(yyvsp[0].text);
			    }
			;
    break;}
case 319:
#line 5010 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 320:
#line 5018 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 321:
#line 5024 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 322:
#line 5032 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 323:
#line 5038 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 324:
#line 5044 "parser-smi.y"
{
			    yyval.access = yyvsp[0].access;
			;
    break;}
case 325:
#line 5048 "parser-smi.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 326:
#line 5054 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 327:
#line 5063 "parser-smi.y"
{
			    Import *importPtr;
			    
			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV2;

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
			;
    break;}
case 328:
#line 5083 "parser-smi.y"
{
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			;
    break;}
case 329:
#line 5092 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-16].id, &objectPtr);

			    setObjectName(objectPtr, yyvsp[-16].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-9].status);
			    setObjectDescription(objectPtr, yyvsp[-7].text, thisParserPtr);
			    if (yyvsp[-5].text) {
				setObjectReference(objectPtr, yyvsp[-5].text, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
				/*
				 * TODO: PRODUCT_RELEASE Text
				 * TODO: ModulePart_Capabilities
				 */
			    yyval.err = 0;
			;
    break;}
case 330:
#line 5122 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 331:
#line 5124 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 332:
#line 5128 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 333:
#line 5130 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 334:
#line 5134 "parser-smi.y"
{
			    /*
			     * Remember the module. SMIv2 is broken by
			     * design to allow subsequent clauses to
			     * refer identifiers that are not
			     * imported.  Although, SMIv2 does not
			     * require, we will fake it by inserting
			     * appropriate imports.
			     */
			    if (yyvsp[0].modulePtr == thisModulePtr)
				capabilitiesModulePtr = NULL;
			    else
				capabilitiesModulePtr = yyvsp[0].modulePtr;
			;
    break;}
case 335:
#line 5150 "parser-smi.y"
{
			    if (capabilitiesModulePtr) {
				checkImports(capabilitiesModulePtr,
					     thisParserPtr);
				capabilitiesModulePtr = NULL;
			    }
			    yyval.err = 0;
			;
    break;}
case 336:
#line 5161 "parser-smi.y"
{
#if 0
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].listPtr;
			    yyval.listPtr->nextPtr = NULL;
#else
			    yyval.listPtr = NULL;
#endif
			;
    break;}
case 337:
#line 5171 "parser-smi.y"
{
#if 0
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].listPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
#else
			    yyval.listPtr = NULL;
#endif
			;
    break;}
case 338:
#line 5188 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 339:
#line 5194 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			    smiFree(yyvsp[-1].id);
			;
    break;}
case 340:
#line 5203 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			    smiFree(yyvsp[0].id);
			;
    break;}
case 341:
#line 5213 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 342:
#line 5215 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 343:
#line 5219 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 344:
#line 5221 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 345:
#line 5225 "parser-smi.y"
{
			    if (yyvsp[0].objectPtr) {
				variationkind = yyvsp[0].objectPtr->export.nodekind;
			    } else {
				variationkind = SMI_NODEKIND_UNKNOWN;
			    }
			;
    break;}
case 346:
#line 5233 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_SYNTAX);
			    }
			;
    break;}
case 347:
#line 5240 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_WRITESYNTAX);
			    }
			;
    break;}
case 348:
#line 5248 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_CREATION);
			    }
			;
    break;}
case 349:
#line 5255 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_DEFVAL);
			    }
			;
    break;}
case 350:
#line 5262 "parser-smi.y"
{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			    variationkind = SMI_NODEKIND_UNKNOWN;

			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_DESCRIPTION);
			    }
			    
			;
    break;}
case 351:
#line 5276 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 352:
#line 5278 "parser-smi.y"
{ yyval.access = 0; ;
    break;}
case 353:
#line 5282 "parser-smi.y"
{
			    if (!strcmp(yyvsp[0].id, "not-implemented")) {
				yyval.access = SMI_ACCESS_NOT_IMPLEMENTED;
			    } else if (!strcmp(yyvsp[0].id, "accessible-for-notify")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_NOTIFY;
				}
			    } else if (!strcmp(yyvsp[0].id, "read-only")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_ONLY;
				}
			    } else if (!strcmp(yyvsp[0].id, "read-write")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				}
			    } else if (!strcmp(yyvsp[0].id, "read-create")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				}
			    } else if (!strcmp(yyvsp[0].id, "write-only")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    smiPrintError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_WRITE; /* TODO */
				    smiPrintError(thisParserPtr,
					       ERR_SMIV2_WRITE_ONLY);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_INVALID_VARIATION_ACCESS,
					      yyvsp[0].id);
				yyval.access = SMI_ACCESS_UNKNOWN;
			    }
			;
    break;}
case 354:
#line 5347 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 355:
#line 5349 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 356:
#line 5353 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 357:
#line 5355 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 358:
#line 5359 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 542 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 5362 "parser-smi.y"


#endif
