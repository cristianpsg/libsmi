
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
	    if (objectPtr->nodePtr->oidlen < modulePtr->prefixNodePtr->oidlen)
		modulePtr->prefixNodePtr =
		    findNodeByOid(objectPtr->nodePtr->oidlen,
				  modulePtr->prefixNodePtr->oid);
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
    

#line 872 "parser-smi.y"
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



#define	YYFINAL		591
#define	YYFLAG		-32768
#define	YYNTBASE	90

#define YYTRANSLATE(x) ((unsigned)(x) <= 333 ? yytranslate[x] : 267)

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
   134,   136,   139,   140,   145,   147,   149,   151,   153,   155,
   157,   159,   161,   163,   165,   166,   170,   172,   174,   175,
   184,   185,   190,   192,   194,   196,   198,   200,   202,   204,
   206,   208,   210,   212,   213,   224,   226,   230,   232,   237,
   239,   243,   246,   248,   253,   255,   257,   260,   262,   266,
   267,   273,   274,   275,   289,   290,   291,   310,   311,   314,
   315,   316,   328,   333,   334,   336,   340,   342,   345,   346,
   347,   351,   352,   356,   357,   358,   373,   374,   375,   376,
   395,   397,   400,   402,   404,   406,   408,   414,   420,   422,
   424,   426,   428,   429,   433,   434,   438,   440,   441,   445,
   446,   450,   455,   458,   463,   466,   467,   472,   475,   480,
   483,   485,   487,   489,   491,   493,   495,   499,   502,   505,
   509,   512,   514,   516,   518,   521,   523,   526,   528,   530,
   533,   535,   537,   539,   542,   545,   547,   549,   551,   553,
   555,   557,   558,   562,   569,   571,   575,   577,   581,   583,
   585,   587,   589,   593,   595,   599,   600,   606,   608,   610,
   612,   614,   617,   618,   621,   622,   624,   625,   631,   636,
   637,   639,   643,   646,   648,   650,   652,   657,   658,   660,
   664,   666,   667,   669,   673,   675,   677,   680,   681,   683,
   684,   686,   689,   694,   699,   700,   702,   706,   708,   713,
   715,   719,   721,   723,   725,   726,   729,   731,   734,   736,
   740,   742,   747,   754,   756,   758,   761,   766,   768,   769,
   770,   785,   786,   787,   802,   803,   804,   819,   821,   823,
   826,   827,   833,   836,   838,   839,   844,   845,   847,   851,
   853,   855,   856,   858,   861,   863,   865,   870,   878,   881,
   882,   885,   886,   888,   891,   892,   893,   894,   911,   913,
   914,   916,   919,   920,   929,   931,   935,   937,   940,   942,
   944,   945,   947,   950,   951,   952,   953,   954,   955,   970,
   973,   974,   976,   981,   982,   984,   988
};

static const short yyrhs[] = {    91,
     0,     0,    92,     0,    91,    92,     0,     0,   105,    93,
    94,    23,     4,    16,    97,    95,   106,    27,     0,    80,
   211,    81,     0,     0,    96,     0,     0,    36,    99,    82,
     0,     0,     0,    29,    98,    82,     0,   100,     0,     0,
   101,     0,   100,   101,     0,   102,    30,   105,     0,   103,
     0,   102,    83,   103,     0,     6,     0,     5,     0,   104,
     0,    13,     0,    17,     0,    21,     0,    22,     0,    32,
     0,    40,     0,    41,     0,    44,     0,    48,     0,    49,
     0,    51,     0,    52,     0,    54,     0,    55,     0,    56,
     0,    61,     0,    71,     0,    72,     0,    73,     0,    76,
     0,     5,     0,   107,     0,     0,   108,     0,   107,   108,
     0,   117,     0,   115,     0,   133,     0,   136,     0,   140,
     0,   150,     0,   153,     0,   224,     0,   218,     0,   221,
     0,   244,     0,   109,     0,     1,    81,     0,     0,   111,
    43,   110,    27,     0,    49,     0,    56,     0,    73,     0,
    52,     0,    55,     0,    71,     0,    54,     0,    51,     0,
    48,     0,    13,     0,     0,    18,   113,    81,     0,     6,
     0,     5,     0,     0,   114,   116,    53,    33,     4,    80,
   211,    81,     0,     0,   119,   118,     4,   121,     0,     5,
     0,   120,     0,    41,     0,    72,     0,    61,     0,    40,
     0,    21,     0,    32,     0,    76,     0,    22,     0,   128,
     0,     0,    71,   122,   184,    67,   182,    25,   209,   199,
    70,   128,     0,   112,     0,    65,    59,   124,     0,     5,
     0,    65,    80,   126,    81,     0,   127,     0,   126,    83,
   127,     0,     6,   129,     0,   157,     0,    17,    80,   130,
    81,     0,   159,     0,    17,     0,     5,   171,     0,   131,
     0,   130,    83,   131,     0,     0,     6,   132,    84,     7,
    85,     0,     0,     0,     6,   134,    55,   135,    67,   182,
    25,   209,   199,     4,    80,   211,    81,     0,     0,     0,
     6,   137,    56,   138,    70,   128,   185,   147,    67,   182,
   139,   199,   187,   193,     4,    80,   197,    81,     0,     0,
    25,   209,     0,     0,     0,   114,   141,    73,   142,    28,
   211,   143,   146,   199,     4,     7,     0,    77,    80,   144,
    81,     0,     0,   145,     0,   144,    83,   145,     0,   197,
     0,    25,   209,     0,     0,     0,    45,   148,   186,     0,
     0,    12,   149,   186,     0,     0,     0,     6,   151,    52,
   152,   203,    67,   182,    25,   209,   199,     4,    80,   198,
    81,     0,     0,     0,     0,     6,   154,    49,   155,    42,
   210,   156,    60,   209,    19,   209,    25,   209,   200,     4,
    80,   211,    81,     0,   161,     0,   158,   161,     0,   123,
     0,   124,     0,   125,     0,   169,     0,    86,    14,     7,
    87,    34,     0,    86,    75,     7,    87,    34,     0,   168,
     0,   170,     0,   167,     0,    39,     0,     0,    39,   162,
   172,     0,     0,    39,   163,   177,     0,    40,     0,     0,
    40,   164,   172,     0,     0,     5,   165,   177,     0,   105,
    88,     5,   177,     0,     5,   172,     0,   105,    88,     5,
   172,     0,    58,    68,     0,     0,    58,    68,   166,   173,
     0,     5,   173,     0,   105,    88,     5,   173,     0,    53,
    33,     0,     7,     0,     8,     0,     9,     0,    10,     0,
     6,     0,    11,     0,    80,   215,    81,     0,    39,   171,
     0,    40,   171,     0,    58,    68,   171,     0,    53,    33,
     0,    41,     0,    21,     0,    32,     0,    32,   172,     0,
    76,     0,    76,   172,     0,    72,     0,    61,     0,    61,
   173,     0,    22,     0,    41,     0,    21,     0,    32,   171,
     0,    76,   171,     0,    72,     0,    61,     0,    22,     0,
   172,     0,   173,     0,   177,     0,     0,    84,   174,    85,
     0,    84,    66,    84,   174,    85,    85,     0,   175,     0,
   174,    89,   175,     0,   176,     0,   176,     3,   176,     0,
     8,     0,     7,     0,    10,     0,     9,     0,    80,   178,
    81,     0,   179,     0,   178,    83,   179,     0,     0,     6,
   180,    84,   181,    85,     0,     7,     0,     8,     0,     6,
     0,     6,     0,    26,   209,     0,     0,    74,   209,     0,
     0,     6,     0,     0,    38,   188,    80,   189,    81,     0,
    15,    80,   192,    81,     0,     0,   190,     0,   189,    83,
   190,     0,    35,   191,     0,   191,     0,   197,     0,   197,
     0,    24,    80,   194,    81,     0,     0,   160,     0,    80,
   195,    81,     0,   196,     0,     0,     6,     0,   196,    83,
     6,     0,   211,     0,   211,     0,    63,   209,     0,     0,
   201,     0,     0,   202,     0,   201,   202,     0,    64,   210,
    25,   209,     0,    57,    80,   204,    81,     0,     0,   205,
     0,   204,    83,   205,     0,   197,     0,    50,    80,   207,
    81,     0,   208,     0,   207,    83,   208,     0,   198,     0,
    11,     0,    11,     0,     0,   212,   213,     0,   214,     0,
   213,   214,     0,   114,     0,   105,    88,     6,     0,     7,
     0,     6,    84,     7,    85,     0,   105,    88,     6,    84,
     7,    85,     0,   216,     0,   217,     0,   216,   217,     0,
     6,    84,     7,    85,     0,     7,     0,     0,     0,     6,
   219,    54,   220,   203,    67,   182,    25,   209,   199,     4,
    80,   211,    81,     0,     0,     0,     6,   222,    51,   223,
   206,    67,   182,    25,   209,   199,     4,    80,   211,    81,
     0,     0,     0,     6,   225,    48,   226,    67,   182,    25,
   209,   199,   227,     4,    80,   211,    81,     0,   228,     0,
   229,     0,   228,   229,     0,     0,    47,   231,   230,   232,
   235,     0,     5,   211,     0,     5,     0,     0,    44,    80,
   233,    81,     0,     0,   234,     0,   233,    83,   234,     0,
   211,     0,   236,     0,     0,   237,     0,   236,   237,     0,
   238,     0,   239,     0,    31,   211,    25,   209,     0,    53,
   197,   240,   241,   243,    25,   209,     0,    70,   128,     0,
     0,    79,   242,     0,     0,   128,     0,    46,   186,     0,
     0,     0,     0,     6,   245,    13,   246,    62,   209,    67,
   183,    25,   209,   199,   247,     4,    80,   211,    81,     0,
   248,     0,     0,   249,     0,   248,   249,     0,     0,    69,
   253,   250,    37,    80,   251,    81,   254,     0,   252,     0,
   251,    83,   252,     0,   211,     0,     5,   211,     0,     5,
     0,   255,     0,     0,   256,     0,   255,   256,     0,     0,
     0,     0,     0,     0,    78,   197,   257,   240,   258,   241,
   259,   262,   264,   260,   193,   261,    25,   209,     0,    12,
   263,     0,     0,     6,     0,    20,    80,   265,    81,     0,
     0,   266,     0,   265,    83,   266,     0,   197,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
  1127,  1132,  1138,  1140,  1148,  1186,  1207,  1209,  1216,  1218,
  1222,  1234,  1236,  1249,  1252,  1254,  1259,  1261,  1265,  1306,
  1308,  1316,  1322,  1328,  1344,  1345,  1346,  1347,  1348,  1349,
  1350,  1351,  1352,  1353,  1354,  1355,  1356,  1357,  1358,  1359,
  1360,  1361,  1362,  1363,  1366,  1378,  1380,  1385,  1387,  1391,
  1396,  1401,  1406,  1411,  1416,  1421,  1426,  1431,  1436,  1441,
  1446,  1451,  1465,  1493,  1504,  1505,  1506,  1507,  1508,  1509,
  1510,  1511,  1512,  1513,  1516,  1535,  1544,  1548,  1564,  1583,
  1603,  1610,  1728,  1732,  1756,  1757,  1758,  1759,  1760,  1761,
  1762,  1763,  1766,  1782,  1804,  1838,  1846,  1860,  1902,  1910,
  1916,  1936,  1968,  1974,  1994,  1999,  2004,  2038,  2044,  2057,
  2070,  2092,  2102,  2118,  2144,  2154,  2174,  2268,  2277,  2283,
  2293,  2311,  2363,  2367,  2373,  2379,  2392,  2398,  2400,  2404,
  2413,  2414,  2423,  2426,  2436,  2451,  2480,  2490,  2514,  2518,
  2553,  2557,  2575,  2580,  2585,  2590,  2607,  2609,  2617,  2622,
  2639,  2648,  2660,  2665,  2678,  2683,  2692,  2710,  2715,  2733,
  2738,  2785,  2829,  2883,  2931,  2936,  2941,  2948,  2993,  3032,
  3039,  3046,  3052,  3078,  3101,  3121,  3138,  3170,  3174,  3192,
  3196,  3202,  3210,  3218,  3226,  3248,  3263,  3281,  3289,  3300,
  3324,  3350,  3358,  3366,  3374,  3389,  3397,  3408,  3430,  3437,
  3444,  3451,  3459,  3469,  3481,  3487,  3501,  3508,  3518,  3524,
  3530,  3553,  3580,  3586,  3592,  3605,  3618,  3627,  3637,  3646,
  3682,  3698,  3702,  3708,  3712,  3718,  3767,  3775,  3782,  3791,
  3797,  3803,  3817,  3822,  3828,  3838,  3844,  3846,  3851,  3853,
  3861,  3863,  3867,  3873,  3886,  3892,  3898,  3900,  3904,  3906,
  3918,  3920,  3924,  3947,  3951,  3957,  3963,  3976,  3982,  3988,
  3994,  4007,  4013,  4019,  4025,  4029,  4035,  4040,  4047,  4137,
  4232,  4256,  4285,  4317,  4321,  4323,  4327,  4329,  4333,  4343,
  4360,  4389,  4399,  4417,  4447,  4457,  4473,  4536,  4542,  4546,
  4581,  4597,  4610,  4618,  4625,  4632,  4636,  4642,  4648,  4661,
  4684,  4690,  4698,  4702,  4769,  4775,  4783,  4807,  4838,  4846,
  4852,  4860,  4866,  4872,  4876,  4882,  4892,  4909,  4944,  4946,
  4950,  4952,  4956,  4971,  4983,  4993,  5010,  5016,  5024,  5033,
  5035,  5039,  5041,  5045,  5054,  5061,  5068,  5076,  5082,  5090,
  5092,  5096,  5161,  5163,  5167,  5169,  5173
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
"declarations","declaration","macroClause","@3","macroName","choiceClause","@4",
"fuzzy_lowercase_identifier","valueDeclaration","@5","typeDeclaration","@6",
"typeName","typeSMI","typeDeclarationRHS","@7","conceptualTable","row","entryType",
"sequenceItems","sequenceItem","Syntax","sequenceSyntax","NamedBits","NamedBit",
"@8","objectIdentityClause","@9","@10","objectTypeClause","@11","@12","descriptionClause",
"trapTypeClause","@13","@14","VarPart","VarTypes","VarType","DescrPart","MaxAccessPart",
"@15","@16","notificationTypeClause","@17","@18","moduleIdentityClause","@19",
"@20","@21","ObjectSyntax","typeTag","sequenceObjectSyntax","valueofObjectSyntax",
"SimpleSyntax","@22","@23","@24","@25","@26","valueofSimpleSyntax","sequenceSimpleSyntax",
"ApplicationSyntax","sequenceApplicationSyntax","anySubType","integerSubType",
"octetStringSubType","ranges","range","value","enumSpec","enumItems","enumItem",
"@27","enumNumber","Status","Status_Capabilities","DisplayPart","UnitsPart",
"Access","IndexPart","@28","IndexTypes","IndexType","Index","Entry","DefValPart",
"Value","BitsValue","BitNames","ObjectName","NotificationName","ReferPart","RevisionPart",
"Revisions","Revision","ObjectsPart","Objects","Object","NotificationsPart",
"Notifications","Notification","Text","ExtUTCTime","objectIdentifier","@29",
"subidentifiers","subidentifier","objectIdentifier_defval","subidentifiers_defval",
"subidentifier_defval","objectGroupClause","@30","@31","notificationGroupClause",
"@32","@33","moduleComplianceClause","@34","@35","ComplianceModulePart","ComplianceModules",
"ComplianceModule","@36","ComplianceModuleName","MandatoryPart","MandatoryGroups",
"MandatoryGroup","CompliancePart","Compliances","Compliance","ComplianceGroup",
"ComplianceObject","SyntaxPart","WriteSyntaxPart","WriteSyntax","AccessPart",
"agentCapabilitiesClause","@37","@38","ModulePart_Capabilities","Modules_Capabilities",
"Module_Capabilities","@39","CapabilitiesGroups","CapabilitiesGroup","ModuleName_Capabilities",
"VariationPart","Variations","Variation","@40","@41","@42","@43","@44","VariationAccessPart",
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
   108,   108,   110,   109,   111,   111,   111,   111,   111,   111,
   111,   111,   111,   111,   113,   112,   114,   114,   116,   115,
   118,   117,   119,   119,   120,   120,   120,   120,   120,   120,
   120,   120,   121,   122,   121,   121,   123,   124,   125,   126,
   126,   127,   128,   128,   129,   129,   129,   130,   130,   132,
   131,   134,   135,   133,   137,   138,   136,   139,   139,   141,
   142,   140,   143,   143,   144,   144,   145,   146,   146,   148,
   147,   149,   147,   151,   152,   150,   154,   155,   156,   153,
   157,   157,   157,   157,   157,   157,   158,   158,   159,   159,
   160,   161,   162,   161,   163,   161,   161,   164,   161,   165,
   161,   161,   161,   161,   161,   166,   161,   161,   161,   161,
   167,   167,   167,   167,   167,   167,   167,   168,   168,   168,
   168,   169,   169,   169,   169,   169,   169,   169,   169,   169,
   169,   170,   170,   170,   170,   170,   170,   170,   171,   171,
   171,   171,   172,   173,   174,   174,   175,   175,   176,   176,
   176,   176,   177,   178,   178,   180,   179,   181,   181,   182,
   183,   184,   184,   185,   185,   186,   188,   187,   187,   187,
   189,   189,   190,   190,   191,   192,   193,   193,   194,   194,
   195,   195,   196,   196,   197,   198,   199,   199,   200,   200,
   201,   201,   202,   203,   203,   204,   204,   205,   206,   207,
   207,   208,   209,   210,   212,   211,   213,   213,   214,   214,
   214,   214,   214,   215,   216,   216,   217,   217,   219,   220,
   218,   222,   223,   221,   225,   226,   224,   227,   228,   228,
   230,   229,   231,   231,   231,   232,   232,   233,   233,   234,
   235,   235,   236,   236,   237,   237,   238,   239,   240,   240,
   241,   241,   242,   243,   243,   245,   246,   244,   247,   247,
   248,   248,   250,   249,   251,   251,   252,   253,   253,   254,
   254,   255,   255,   257,   258,   259,   260,   261,   256,   262,
   262,   263,   264,   264,   265,   265,   266
};

static const short yyr2[] = {     0,
     1,     0,     1,     2,     0,    10,     3,     0,     1,     0,
     3,     0,     0,     3,     1,     0,     1,     2,     3,     1,
     3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     0,     1,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     2,     0,     4,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     0,     3,     1,     1,     0,     8,
     0,     4,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     0,    10,     1,     3,     1,     4,     1,
     3,     2,     1,     4,     1,     1,     2,     1,     3,     0,
     5,     0,     0,    13,     0,     0,    18,     0,     2,     0,
     0,    11,     4,     0,     1,     3,     1,     2,     0,     0,
     3,     0,     3,     0,     0,    14,     0,     0,     0,    18,
     1,     2,     1,     1,     1,     1,     5,     5,     1,     1,
     1,     1,     0,     3,     0,     3,     1,     0,     3,     0,
     3,     4,     2,     4,     2,     0,     4,     2,     4,     2,
     1,     1,     1,     1,     1,     1,     3,     2,     2,     3,
     2,     1,     1,     1,     2,     1,     2,     1,     1,     2,
     1,     1,     1,     2,     2,     1,     1,     1,     1,     1,
     1,     0,     3,     6,     1,     3,     1,     3,     1,     1,
     1,     1,     3,     1,     3,     0,     5,     1,     1,     1,
     1,     2,     0,     2,     0,     1,     0,     5,     4,     0,
     1,     3,     2,     1,     1,     1,     4,     0,     1,     3,
     1,     0,     1,     3,     1,     1,     2,     0,     1,     0,
     1,     2,     4,     4,     0,     1,     3,     1,     4,     1,
     3,     1,     1,     1,     0,     2,     1,     2,     1,     3,
     1,     4,     6,     1,     1,     2,     4,     1,     0,     0,
    14,     0,     0,    14,     0,     0,    14,     1,     1,     2,
     0,     5,     2,     1,     0,     4,     0,     1,     3,     1,
     1,     0,     1,     2,     1,     1,     4,     7,     2,     0,
     2,     0,     1,     2,     0,     0,     0,    16,     1,     0,
     1,     2,     0,     8,     1,     3,     1,     2,     1,     1,
     0,     1,     2,     0,     0,     0,     0,     0,    14,     2,
     0,     1,     4,     0,     1,     3,     1
};

static const short yydefact[] = {     2,
    45,     1,     3,     5,     4,     8,   265,     0,     0,     0,
     0,     7,    78,    77,   271,     0,   269,   266,   267,     0,
     0,     0,   268,    12,     0,   270,    13,    10,   272,     0,
     0,    16,     0,     9,     0,    14,    23,    22,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,     0,    15,
    17,     0,    20,    24,     0,    78,    77,    74,    89,    92,
    90,    88,    85,    73,    65,    72,    68,    71,    69,    66,
    87,    70,    86,    67,    91,     0,     0,    48,    61,     0,
    79,    51,    50,    81,    84,    52,    53,    54,    55,    56,
    58,    59,    57,    60,   273,    11,    18,     0,     0,    62,
     0,     0,     0,     0,     0,     0,     0,     0,     6,    49,
    63,     0,     0,     0,    19,    21,   113,   116,   135,   138,
   280,   283,   286,   317,     0,     0,   121,     0,     0,     0,
   255,     0,   255,     0,     0,     0,    64,     0,     0,    98,
     0,    75,   183,   191,   184,   152,   157,   182,     0,     0,
   189,     0,    94,   188,   186,     0,     0,    96,    82,   143,
   144,   145,    93,   103,     0,   141,   146,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   265,     0,
     0,   163,   168,     0,     0,     0,   185,     0,     0,     0,
   170,   165,     0,   190,     0,     0,   223,   187,     0,     0,
     0,    45,   142,   220,     0,   225,   265,     0,   264,   139,
     0,   265,     0,     0,   263,     0,   265,   124,   210,   209,
   212,   211,     0,     0,   205,   207,     0,   161,   110,     0,
   108,    76,   154,   156,   159,     0,    98,    97,     0,     0,
   100,     0,     0,     0,     0,     0,     0,     0,     0,   258,
     0,   256,   245,     0,     0,     0,   262,     0,   260,   246,
     0,     0,     0,     0,     0,   129,     0,   203,     0,     0,
   216,     0,   214,     0,   104,     0,   167,   202,   106,   193,
   198,   202,   202,   202,   192,     0,     0,   197,   196,   202,
   102,   105,   149,   150,    99,     0,   222,     0,     0,     0,
   164,   169,   162,   248,   224,   132,   130,     0,   254,   265,
     0,     0,     0,   259,   265,     0,   248,   221,     0,    80,
   265,     0,   248,     0,   206,   208,     0,   213,     0,     0,
   109,   107,   199,   200,   201,   194,   178,   179,   181,   202,
   195,   101,     0,   147,   148,     0,     0,     0,     0,     0,
   257,   248,     0,   248,   261,   248,     0,     0,     0,   125,
   127,   128,     0,     0,     0,   215,     0,   180,     0,   247,
     0,   226,   133,   131,   118,     0,     0,     0,     0,   295,
     0,   288,   289,   248,   123,   265,     0,   204,   218,   219,
     0,   111,   248,   265,     0,   248,     0,     0,     0,     0,
   294,   291,     0,   290,   320,   126,   122,   217,     0,     0,
   119,   230,   265,     0,   265,   265,   293,   297,   265,     0,
     0,   319,   321,     0,   114,     0,   227,   238,     0,   250,
     0,     0,     0,   302,     0,   265,   323,     0,   322,    95,
   265,     0,     0,     0,   136,     0,     0,   249,   251,   281,
   284,   265,   265,   265,   292,   301,   303,   305,   306,   287,
   328,     0,   265,     0,   236,   265,     0,     0,     0,     0,
   252,   300,     0,   298,     0,   310,   304,     0,     0,   229,
   265,     0,   231,   234,   235,   175,   171,   172,   173,   174,
   176,   242,   239,   151,     0,   265,     0,   265,   296,   265,
     0,     0,   312,   265,   318,   233,   228,   265,   243,   278,
     0,   241,     0,   274,   275,   237,     0,   253,     0,   299,
   307,   309,     0,   315,   327,     0,   325,   232,     0,   240,
     0,   177,     0,   276,   117,   140,   313,   311,     0,     0,
   331,   265,     0,   244,   314,     0,   265,   324,   330,   332,
   326,   277,   308,   334,   333,   310,   335,   312,   336,   341,
     0,   344,   342,   340,     0,   337,   265,   238,   347,     0,
   345,   338,   343,   265,     0,   346,     0,   339,     0,     0,
     0
};

static const short yydefgoto[] = {   589,
     2,     3,     6,     8,    33,    34,    28,    31,    59,    60,
    61,    62,    63,    64,   167,    86,    87,    88,    89,   135,
    90,   168,   195,    17,    92,   122,    93,   124,    94,    95,
   169,   207,   170,   171,   172,   250,   251,   173,   301,   240,
   241,   284,    96,   111,   139,    97,   112,   140,   406,    98,
   123,   149,   276,   369,   370,   333,   318,   359,   358,    99,
   113,   141,   100,   114,   142,   265,   174,   175,   302,   503,
   176,   198,   199,   200,   191,   246,   504,   303,   177,   304,
   342,   343,   344,   234,   235,   236,   345,   282,   283,   337,
   401,   215,   329,   253,   259,   383,   438,   452,   492,   493,
   494,   474,   454,   505,   521,   522,   495,   267,   357,   457,
   458,   459,   181,   261,   262,   185,   268,   269,   226,   220,
   263,    10,    18,    19,   523,   524,   525,   101,   115,   143,
   102,   116,   144,   103,   117,   145,   391,   392,   393,   428,
   412,   444,   483,   484,   465,   466,   467,   468,   469,   513,
   534,   548,   550,   104,   118,   146,   431,   432,   433,   472,
   536,   537,   447,   558,   559,   560,   566,   568,   570,   578,
   585,   572,   574,   576,   580,   581
};

static const short yypact[] = {    22,
-32768,    22,-32768,-32768,-32768,   -65,-32768,    -1,   -43,   129,
    53,-32768,   -47,   -37,-32768,   -18,-32768,   129,-32768,    60,
    72,    79,-32768,    55,    29,    16,-32768,    90,-32768,   122,
    49,   419,   224,-32768,    52,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    76,   419,
-32768,    -5,-32768,-32768,    62,   162,    42,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   146,   318,-32768,-32768,   135,
   104,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    22,   419,-32768,
   130,   139,   132,   142,   143,   145,   153,   189,-32768,-32768,
-32768,   150,   136,   204,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   183,   178,-32768,   128,   149,   147,
   155,   171,   155,   168,   154,   160,-32768,   219,   198,    37,
   148,-32768,-32768,-32768,   152,   -17,   156,-32768,   199,   165,
   157,   -29,-32768,-32768,   152,     0,   151,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    48,-32768,-32768,   228,   166,   163,
   177,   236,   182,   170,   186,   228,   243,   175,-32768,    36,
   179,-32768,-32768,   254,   180,   101,-32768,   152,   179,   152,
-32768,   184,   197,-32768,   261,   263,   241,-32768,   264,   267,
   265,     2,-32768,-32768,   256,   208,-32768,   228,-32768,-32768,
   228,-32768,   228,   258,-32768,   217,-32768,   209,-32768,-32768,
-32768,-32768,   203,    35,-32768,   285,   286,-32768,-32768,    61,
-32768,-32768,-32768,-32768,-32768,   157,-32768,-32768,   272,    70,
-32768,   243,   231,   212,   214,    15,   243,   243,    17,-32768,
    71,-32768,-32768,   266,   242,   278,-32768,    74,-32768,-32768,
   280,   243,   300,   227,   229,   289,   101,-32768,   101,   101,
-32768,    78,-32768,   226,-32768,   254,-32768,    15,-32768,-32768,
-32768,    15,    15,    15,-32768,   282,   248,-32768,-32768,    15,
-32768,-32768,-32768,-32768,-32768,   263,-32768,   228,   283,   287,
-32768,-32768,-32768,   257,-32768,-32768,-32768,   255,-32768,-32768,
   243,   243,   243,-32768,-32768,   243,   257,-32768,   301,-32768,
-32768,   243,   257,    43,-32768,-32768,   244,-32768,   286,   320,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    15,
-32768,-32768,   304,-32768,-32768,   243,   328,   330,   330,   228,
-32768,   257,   315,   257,-32768,   257,   290,   243,    81,-32768,
-32768,-32768,   338,   262,    57,-32768,   268,-32768,   243,-32768,
   269,-32768,-32768,-32768,   321,   339,   243,   347,   348,   349,
   351,   290,-32768,   257,-32768,-32768,   350,-32768,-32768,-32768,
   275,-32768,   257,-32768,   243,   257,   284,   337,   291,   295,
   134,-32768,   296,-32768,   294,-32768,-32768,-32768,   307,   297,
-32768,    11,-32768,   243,-32768,-32768,-32768,   336,-32768,   360,
   377,   294,-32768,   166,-32768,   302,-32768,   359,   303,   322,
   306,   311,   305,     8,   312,   358,-32768,   308,-32768,-32768,
-32768,   316,   319,   394,-32768,   236,   396,   322,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,     8,-32768,-32768,-32768,-32768,
-32768,   364,-32768,   324,-32768,   367,    25,   323,   381,   327,
-32768,-32768,    82,-32768,   383,   340,-32768,   329,   331,-32768,
-32768,    89,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   116,-32768,-32768,   332,-32768,   243,-32768,-32768,-32768,
   243,   166,   335,-32768,-32768,-32768,-32768,   367,   333,-32768,
   334,   343,   342,   141,-32768,-32768,   346,-32768,   352,-32768,
-32768,-32768,   166,   365,-32768,    93,-32768,-32768,   413,-32768,
   415,-32768,   333,-32768,-32768,-32768,-32768,-32768,   330,   403,
   353,-32768,   344,-32768,-32768,   243,-32768,-32768,   353,-32768,
-32768,-32768,-32768,-32768,-32768,   340,-32768,   335,-32768,   418,
   428,   417,-32768,-32768,   355,-32768,-32768,   359,-32768,   109,
-32768,-32768,-32768,-32768,   420,-32768,   243,-32768,   438,   442,
-32768
};

static const short yypgoto[] = {-32768,
-32768,   441,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   384,-32768,   341,-32768,    10,-32768,-32768,   361,-32768,-32768,
-32768,-32768,-32768,   -16,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   247,-32768,-32768,   140,  -177,-32768,-32768,
   161,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    58,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   274,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -234,  -144,  -143,   176,   185,   181,  -175,-32768,   118,-32768,
-32768,  -181,-32768,-32768,-32768,  -355,-32768,-32768,-32768,   -60,
   -26,-32768,  -116,-32768,-32768,-32768,  -216,    46,  -314,-32768,
-32768,    14,   345,-32768,   158,-32768,-32768,   159,  -249,    20,
    -7,-32768,-32768,   459,-32768,-32768,   -45,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    91,-32768,
-32768,-32768,-32768,   -28,-32768,-32768,    19,-32768,-32768,   -85,
   -82,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    64,-32768,
-32768,   -63,-32768,-32768,-32768,   -72,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   -91
};


#define	YYLAST		545


static const short yytable[] = {     9,
   260,   216,   307,   384,   224,   192,   193,   314,   315,     4,
   197,     4,   367,   209,     7,   238,    91,   204,   373,    16,
   208,    11,   327,   244,   108,   436,     1,    16,   316,   205,
   496,   497,   498,   499,   500,   501,   264,    12,   463,   266,
   -45,   271,   229,   230,   231,   232,    21,   386,   437,   388,
   206,   389,   212,   243,  -316,   245,    20,   346,   347,   348,
   464,   317,  -155,   399,   400,   351,  -153,   192,   193,    22,
    91,   362,   363,   364,   210,    24,   366,   109,    25,   415,
   313,  -160,   372,    27,    26,   190,   156,   157,   419,  -285,
  -137,   422,  -282,  -134,   237,  -279,  -112,  -115,   190,    30,
   159,   233,   287,   260,   502,   160,   380,   229,   230,   231,
   232,   311,   312,    29,   371,   378,  -160,   125,   394,   278,
   190,   519,   520,   279,   -45,    32,   353,   374,    35,   403,
    36,   279,   150,    13,    14,    15,   105,   408,  -265,  -265,
  -265,   285,   110,   286,   151,   152,   543,   520,   153,   154,
   305,   319,   306,   320,   324,   421,   325,   106,   338,   155,
   339,   395,   509,   396,   510,   -83,   156,   157,   158,   517,
   150,   518,   119,   551,   440,   552,  -120,   121,   385,   371,
   159,   228,   151,   129,   127,   160,   153,   154,   161,   583,
   130,   584,   162,   555,   128,   132,   131,   155,   163,   164,
   133,   134,   136,   165,   156,   157,   158,   138,   137,   147,
   148,   180,   182,   166,   270,   178,   179,   184,   159,   274,
   186,   187,   188,   160,    65,   189,   161,   194,    66,    67,
   162,   201,   202,   214,   475,   196,    68,   164,   211,  -158,
   203,   165,   217,   218,    69,    70,   219,   486,   221,   222,
   -47,   166,   223,   225,   227,    71,   450,   528,   237,   239,
   242,   531,   233,    72,    73,   247,   252,  -166,   249,   256,
   254,    74,    75,   255,    76,    77,   288,    78,    79,    80,
   257,   258,   272,   273,    81,   275,   277,   280,   289,   527,
   321,   281,   290,   291,    82,    83,    84,   308,   309,    85,
   310,   322,   323,   292,   326,   328,   563,   330,   331,   340,
   293,   294,   295,   332,   349,   350,   354,   270,    65,   356,
   355,   360,    66,    67,   296,   368,   377,   375,   379,   297,
    68,   381,   298,   387,   532,   382,   390,   588,    69,    70,
   564,   397,   407,   299,   -46,   405,   398,   300,   404,    71,
   409,   410,   402,   411,   413,   547,   417,    72,    73,   418,
   579,   424,   430,   423,   446,    74,    75,   579,    76,    77,
   425,    78,    79,    80,   426,   429,   434,   435,    81,   443,
   448,   451,   453,   455,   462,   456,   460,   473,    82,    83,
    84,   461,   470,    85,  -329,   476,   420,   478,   477,   480,
   488,   491,   506,   427,   490,   507,   508,   511,   514,   512,
   549,   515,   526,   533,   540,   270,   539,   441,   442,   553,
   554,   445,   542,    37,    38,   541,   545,   556,   562,   571,
   557,    39,   546,   573,   577,    40,   575,   590,   471,    41,
    42,   591,     5,   107,   587,   352,   341,   120,   213,   126,
    43,   248,   334,   416,   482,   485,   376,   538,    44,    45,
   336,   582,    46,   335,   516,   489,    47,    48,   439,    49,
    50,   481,    51,    52,    53,   479,    23,   361,   544,    54,
   567,   530,   414,   365,   487,   569,   565,   183,   561,    55,
    56,    57,   586,     0,    58,   449,     0,     0,     0,     0,
   529,     0,   482,     0,     0,     0,   535,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   535
};

static const short yycheck[] = {     7,
   217,   179,   252,   359,   186,   150,   150,   257,   258,     0,
   155,     2,   327,    14,    80,   191,    33,   161,   333,    10,
   165,    23,   272,   199,    30,    15,     5,    18,    12,    59,
     6,     7,     8,     9,    10,    11,   218,    81,    31,   221,
    88,   223,     7,     8,     9,    10,    84,   362,    38,   364,
    80,   366,     5,   198,    13,   200,     4,   292,   293,   294,
    53,    45,    80,     7,     8,   300,    84,   212,   212,    88,
    87,   321,   322,   323,    75,    16,   326,    83,     7,   394,
   256,    80,   332,    29,     6,    84,    39,    40,   403,    48,
    49,   406,    51,    52,    80,    54,    55,    56,    84,    84,
    53,    66,   246,   320,    80,    58,   356,     7,     8,     9,
    10,   256,   256,    85,   331,   350,    80,   108,   368,    85,
    84,     6,     7,    89,    88,    36,   308,    85,     7,   379,
    82,    89,     5,     5,     6,     7,    85,   387,     5,     6,
     7,    81,    81,    83,    17,    18,     6,     7,    21,    22,
    81,    81,    83,    83,    81,   405,    83,    82,    81,    32,
    83,    81,    81,    83,    83,     4,    39,    40,    41,    81,
     5,    83,    27,    81,   424,    83,    73,    43,   360,   396,
    53,   189,    17,    52,    55,    58,    21,    22,    61,    81,
    49,    83,    65,   549,    56,    51,    54,    32,    71,    72,
    48,    13,    53,    76,    39,    40,    41,     4,    73,    27,
    33,    57,    42,    86,   222,    67,    70,    50,    53,   227,
    67,    62,     4,    58,     1,    28,    61,    80,     5,     6,
    65,    33,    68,     6,   451,    84,    13,    72,    88,    84,
    84,    76,    80,    67,    21,    22,    11,   464,    67,    80,
    27,    86,    67,    11,    80,    32,   434,   507,    80,     6,
    81,   511,    66,    40,    41,     5,    26,    84,     6,     5,
     7,    48,    49,     7,    51,    52,     5,    54,    55,    56,
    25,    74,    25,    67,    61,    77,    84,     3,    17,   506,
    25,     6,    21,    22,    71,    72,    73,    67,    87,    76,
    87,    60,    25,    32,    25,     6,   556,    81,    80,    84,
    39,    40,    41,    25,    33,    68,    34,   325,     1,    63,
    34,    67,     5,     6,    53,    25,     7,    84,    25,    58,
    13,     4,    61,    19,   512,     6,    47,   587,    21,    22,
   557,     4,     4,    72,    27,    25,    85,    76,    80,    32,
     4,     4,    85,     5,     4,   533,     7,    40,    41,    85,
   577,    25,    69,    80,     5,    48,    49,   584,    51,    52,
    80,    54,    55,    56,    80,    80,    70,    81,    61,    44,
     4,    80,    24,    81,    80,    64,    81,    80,    71,    72,
    73,    81,    81,    76,    37,    80,   404,     4,    80,     4,
    37,    35,    80,   411,    81,    25,    80,    25,    80,    70,
    46,    81,    81,    79,    81,   423,    84,   425,   426,     7,
     6,   429,    81,     5,     6,    83,    81,    25,    85,    12,
    78,    13,    81,     6,    80,    17,    20,     0,   446,    21,
    22,     0,     2,    60,    25,   306,   286,    87,   175,   109,
    32,   205,   277,   396,   462,   463,   339,   518,    40,    41,
   280,   578,    44,   279,   491,   473,    48,    49,   423,    51,
    52,   458,    54,    55,    56,   456,    18,   320,   524,    61,
   566,   510,   392,   325,   466,   568,   559,   143,   552,    71,
    72,    73,   584,    -1,    76,   432,    -1,    -1,    -1,    -1,
   508,    -1,   510,    -1,    -1,    -1,   514,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   552
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
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

#line 216 "/usr/lib/bison.simple"

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
#line 1129 "parser-smi.y"
{
        yyval.err = 0;
    ;
    break;}
case 2:
#line 1133 "parser-smi.y"
{
	yyval.err = 0;
    ;
    break;}
case 3:
#line 1139 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 4:
#line 1141 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 5:
#line 1149 "parser-smi.y"
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
#line 1191 "parser-smi.y"
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
#line 1208 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 8:
#line 1210 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 9:
#line 1217 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 10:
#line 1219 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 11:
#line 1223 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;

			    if (thisModulePtr->export.language != SMI_LANGUAGE_SMIV2)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    yyval.err = 0;
			;
    break;}
case 12:
#line 1235 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 13:
#line 1237 "parser-smi.y"
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
#line 1249 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 15:
#line 1253 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 16:
#line 1255 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 17:
#line 1260 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 18:
#line 1262 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 19:
#line 1268 "parser-smi.y"
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

			;
    break;}
case 20:
#line 1307 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 21:
#line 1310 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 22:
#line 1317 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 23:
#line 1323 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 24:
#line 1329 "parser-smi.y"
{
			    addImport(smiStrdup(yyvsp[0].id), thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 45:
#line 1367 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 46:
#line 1379 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 47:
#line 1381 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 48:
#line 1386 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 49:
#line 1388 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 50:
#line 1392 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 51:
#line 1397 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 52:
#line 1402 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 53:
#line 1407 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 54:
#line 1412 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 55:
#line 1417 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 56:
#line 1422 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 57:
#line 1427 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 58:
#line 1432 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 59:
#line 1437 "parser-smi.y"
{
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 60:
#line 1442 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 61:
#line 1447 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 62:
#line 1452 "parser-smi.y"
{
			    smiPrintError(thisParserPtr,
					  ERR_FLUSH_DECLARATION);
			    yyerrok;
			    yyval.err = 1;
			;
    break;}
case 63:
#line 1467 "parser-smi.y"
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
				       "RFC1065-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_MACRO);
			    }
			;
    break;}
case 64:
#line 1493 "parser-smi.y"
{
			    Macro *macroPtr;

			    macroPtr = addMacro(yyvsp[-3].id, 0, thisParserPtr);
			    setMacroLine(macroPtr, firstStatementLine,
					 thisParserPtr);
			    smiFree(yyvsp[-3].id);
			    yyval.err = 0;
                        ;
    break;}
case 65:
#line 1504 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 66:
#line 1505 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 67:
#line 1506 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 68:
#line 1507 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 69:
#line 1508 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 70:
#line 1509 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 71:
#line 1510 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 72:
#line 1511 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 73:
#line 1512 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 74:
#line 1513 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 75:
#line 1517 "parser-smi.y"
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
case 76:
#line 1535 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			;
    break;}
case 77:
#line 1545 "parser-smi.y"
{
			  yyval.id = yyvsp[0].id;
			;
    break;}
case 78:
#line 1550 "parser-smi.y"
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
case 79:
#line 1565 "parser-smi.y"
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
case 80:
#line 1585 "parser-smi.y"
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
case 81:
#line 1604 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;

			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			;
    break;}
case 82:
#line 1611 "parser-smi.y"
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
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Gauge32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Unsigned32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "TimeTicks")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
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
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned64Ptr);
				}
			    }
			    if (thisModulePtr &&
				(!strcmp(thisModulePtr->export.name, "RFC1155-SMI") ||
				 !strcmp(thisModulePtr->export.name, "RFC1065-SMI"))) {
				if (!strcmp(yyvsp[-3].id, "Counter")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "Gauge")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned32 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED32;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned32 = 4294967295U;
				    }
				} else if (!strcmp(yyvsp[-3].id, "TimeTicks")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
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
					thisModulePtr, smiStrdup("NetworkAddress"));
				    if (typePtr) 
					setTypeParent(typePtr, yyvsp[0].typePtr);
				}
			    }
			;
    break;}
case 83:
#line 1729 "parser-smi.y"
{
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 84:
#line 1733 "parser-smi.y"
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
case 93:
#line 1767 "parser-smi.y"
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
case 94:
#line 1783 "parser-smi.y"
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
case 95:
#line 1808 "parser-smi.y"
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
			    setTypeDescription(yyval.typePtr, yyvsp[-3].text, thisParserPtr);
			    if (yyvsp[-2].text) {
				setTypeReference(yyval.typePtr, yyvsp[-2].text, thisParserPtr);
			    }
			    setTypeStatus(yyval.typePtr, yyvsp[-5].status);
			    if (yyvsp[-7].text) {
				smiCheckFormat(thisParserPtr,
					       yyval.typePtr->export.basetype, yyvsp[-7].text,
					       firstStatementLine);
				setTypeFormat(yyval.typePtr, yyvsp[-7].text);
			    }
			    setTypeDecl(yyval.typePtr, SMI_DECL_TEXTUALCONVENTION);
			;
    break;}
case 96:
#line 1839 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			;
    break;}
case 97:
#line 1847 "parser-smi.y"
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
case 98:
#line 1866 "parser-smi.y"
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
				}
			    }
			;
    break;}
case 99:
#line 1903 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[-1].listPtr);
			;
    break;}
case 100:
#line 1911 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 101:
#line 1918 "parser-smi.y"
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
case 102:
#line 1937 "parser-smi.y"
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
				    objectPtr = addObject(yyvsp[-1].id, pendingNodePtr,
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
				}
			    }

			    yyval.objectPtr = objectPtr;
			;
    break;}
case 103:
#line 1969 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    if (yyval.typePtr)
				defaultBasetype = yyval.typePtr->export.basetype;
			;
    break;}
case 104:
#line 1977 "parser-smi.y"
{
			    Type *typePtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_BITS;
			    typePtr = addType(NULL, SMI_BASETYPE_BITS,
					      FLAG_INCOMPLETE,
					      thisParserPtr);
			    setTypeDecl(typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(typePtr, typeBitsPtr);
			    setTypeList(typePtr, yyvsp[-1].listPtr);
			    for (p = yyvsp[-1].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = typePtr;
			    yyval.typePtr = typePtr;
			;
    break;}
case 105:
#line 1996 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 106:
#line 2000 "parser-smi.y"
{
			    /* TODO: */
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 107:
#line 2005 "parser-smi.y"
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
				}
			    }
			;
    break;}
case 108:
#line 2039 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 109:
#line 2045 "parser-smi.y"
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
case 110:
#line 2058 "parser-smi.y"
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
case 111:
#line 2071 "parser-smi.y"
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
case 112:
#line 2093 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 113:
#line 2102 "parser-smi.y"
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
case 114:
#line 2123 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-12].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-12].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTIDENTITY);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, yyvsp[-7].status);
			    setObjectDescription(objectPtr, yyvsp[-5].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    yyval.err = 0;
			;
    break;}
case 115:
#line 2145 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 116:
#line 2154 "parser-smi.y"
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
case 117:
#line 2183 "parser-smi.y"
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
				setObjectValue(objectPtr, yyvsp[-4].valuePtr);
			    }
			    yyval.err = 0;
			;
    break;}
case 118:
#line 2269 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				smiPrintError(thisParserPtr,
					      ERR_MISSING_DESCRIPTION);
			    }
			    yyval.text = NULL;
			;
    break;}
case 119:
#line 2278 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 120:
#line 2284 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 121:
#line 2293 "parser-smi.y"
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
case 122:
#line 2317 "parser-smi.y"
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
case 123:
#line 2364 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 124:
#line 2368 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 125:
#line 2374 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 126:
#line 2380 "parser-smi.y"
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
case 127:
#line 2393 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 128:
#line 2399 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 129:
#line 2401 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 130:
#line 2405 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        smiPrintError(thisParserPtr,
					      ERR_MAX_ACCESS_IN_SMIV1);
			    }
			;
    break;}
case 131:
#line 2413 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 132:
#line 2415 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        smiPrintError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			;
    break;}
case 133:
#line 2423 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 134:
#line 2427 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 135:
#line 2436 "parser-smi.y"
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
case 136:
#line 2456 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-13].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONTYPE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectList(objectPtr, yyvsp[-9].listPtr);
			    setObjectStatus(objectPtr, yyvsp[-7].status);
			    setObjectDescription(objectPtr, yyvsp[-5].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    yyval.err = 0;
			;
    break;}
case 137:
#line 2481 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 138:
#line 2490 "parser-smi.y"
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
case 139:
#line 2515 "parser-smi.y"
{
			    setModuleLastUpdated(thisParserPtr->modulePtr, yyvsp[0].date);
			;
    break;}
case 140:
#line 2524 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-17].id, &objectPtr);

			    thisParserPtr->modulePtr->numModuleIdentities++;

			    objectPtr = setObjectName(objectPtr, yyvsp[-17].id);
			    setObjectDecl(objectPtr, SMI_DECL_MODULEIDENTITY);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, SMI_STATUS_CURRENT);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setModuleIdentityObject(thisParserPtr->modulePtr,
						    objectPtr);
			    setModuleOrganization(thisParserPtr->modulePtr,
						  yyvsp[-9].text);
			    setModuleContactInfo(thisParserPtr->modulePtr,
						 yyvsp[-7].text);
			    setModuleDescription(thisParserPtr->modulePtr,
						 yyvsp[-5].text, thisParserPtr);
			    /* setObjectDescription(objectPtr, $13); */
			    yyval.err = 0;
			;
    break;}
case 141:
#line 2554 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 142:
#line 2558 "parser-smi.y"
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
case 143:
#line 2576 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 144:
#line 2581 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 145:
#line 2586 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 146:
#line 2591 "parser-smi.y"
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
case 147:
#line 2608 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 148:
#line 2610 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 149:
#line 2618 "parser-smi.y"
{ yyval.typePtr = yyvsp[0].typePtr; ;
    break;}
case 150:
#line 2623 "parser-smi.y"
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
case 151:
#line 2640 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 152:
#line 2649 "parser-smi.y"
{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 153:
#line 2661 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 154:
#line 2665 "parser-smi.y"
{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 155:
#line 2679 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 156:
#line 2683 "parser-smi.y"
{
			    List *p;
			    
			    yyval.typePtr = duplicateType(typeEnumPtr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 157:
#line 2693 "parser-smi.y"
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
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 158:
#line 2711 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 159:
#line 2715 "parser-smi.y"
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

			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 160:
#line 2734 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 161:
#line 2738 "parser-smi.y"
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
				    yyval.typePtr = duplicateType(typeEnumPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
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
case 162:
#line 2787 "parser-smi.y"
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
				    yyval.typePtr = duplicateType(typeEnumPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				yyval.typePtr = duplicateType(typeEnumPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 163:
#line 2830 "parser-smi.y"
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
				    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
						       thisParserPtr);
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				} else {
				    defaultBasetype =
					parentPtr->export.basetype;
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
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
case 164:
#line 2885 "parser-smi.y"
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
				    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
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
				yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
						   thisParserPtr);
				defaultBasetype = SMI_BASETYPE_INTEGER32;
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 165:
#line 2932 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 166:
#line 2937 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			;
    break;}
case 167:
#line 2941 "parser-smi.y"
{
			    
			    yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 168:
#line 2949 "parser-smi.y"
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
				    yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
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
case 169:
#line 2995 "parser-smi.y"
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
				    yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
						       thisParserPtr);
				} else {
				    yyval.typePtr = duplicateType(parentPtr, 0,
						       thisParserPtr);
				}
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
						   thisParserPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;
    break;}
case 170:
#line 3033 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 171:
#line 3041 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;
    break;}
case 172:
#line 3047 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;
    break;}
case 173:
#line 3053 "parser-smi.y"
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
case 174:
#line 3079 "parser-smi.y"
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
case 175:
#line 3102 "parser-smi.y"
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
				yyval.valuePtr->value.ptr = yyvsp[0].id;   /* JS: needs strdup? */
			    }
			;
    break;}
case 176:
#line 3122 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.valuePtr->value.ptr = smiStrdup(yyvsp[0].text);
			    yyval.valuePtr->len = strlen(yyvsp[0].text);
			;
    break;}
case 177:
#line 3145 "parser-smi.y"
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
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.valuePtr->len = 2;
			    yyval.valuePtr->value.oid = smiMalloc(2 * sizeof(SmiSubid));
			    yyval.valuePtr->value.oid[0] = 0;
			    yyval.valuePtr->value.oid[1] = 0;
			;
    break;}
case 178:
#line 3171 "parser-smi.y"
{
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 179:
#line 3175 "parser-smi.y"
{
			    Import *importPtr;
			    
			    /* TODO: any need to distinguish from INTEGER? */
			    yyval.typePtr = typeInteger32Ptr;

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
case 180:
#line 3193 "parser-smi.y"
{
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 181:
#line 3197 "parser-smi.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 182:
#line 3203 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			;
    break;}
case 183:
#line 3211 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			;
    break;}
case 184:
#line 3219 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			;
    break;}
case 185:
#line 3227 "parser-smi.y"
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
case 186:
#line 3249 "parser-smi.y"
{
			    Import *importPtr;

			    yyval.typePtr = typeUnsigned32Ptr;
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
case 187:
#line 3264 "parser-smi.y"
{
			    Import *importPtr;
			    
			    yyval.typePtr = duplicateType(typeUnsigned32Ptr, 0,
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
case 188:
#line 3282 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			;
    break;}
case 189:
#line 3290 "parser-smi.y"
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
case 190:
#line 3301 "parser-smi.y"
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
case 191:
#line 3325 "parser-smi.y"
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
case 192:
#line 3351 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			;
    break;}
case 193:
#line 3359 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			;
    break;}
case 194:
#line 3367 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			;
    break;}
case 195:
#line 3375 "parser-smi.y"
{
			    Import *importPtr;
			    
			    yyval.typePtr = typeUnsigned32Ptr;
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
case 196:
#line 3390 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			;
    break;}
case 197:
#line 3398 "parser-smi.y"
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
case 198:
#line 3409 "parser-smi.y"
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
#line 3431 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 200:
#line 3438 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 201:
#line 3445 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 202:
#line 3452 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 203:
#line 3466 "parser-smi.y"
{ yyval.listPtr = yyvsp[-1].listPtr; ;
    break;}
case 204:
#line 3476 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 205:
#line 3482 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 206:
#line 3488 "parser-smi.y"
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
case 207:
#line 3502 "parser-smi.y"
{
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[0].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[0].valuePtr);
			;
    break;}
case 208:
#line 3509 "parser-smi.y"
{
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[-2].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[-2].valuePtr);
			    smiFree(yyvsp[0].valuePtr);
			;
    break;}
case 209:
#line 3519 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;
    break;}
case 210:
#line 3525 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;
    break;}
case 211:
#line 3531 "parser-smi.y"
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
case 212:
#line 3554 "parser-smi.y"
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
case 213:
#line 3581 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 214:
#line 3587 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 215:
#line 3593 "parser-smi.y"
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
case 216:
#line 3606 "parser-smi.y"
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
case 217:
#line 3619 "parser-smi.y"
{
			    yyval.namedNumberPtr = smiMalloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id;
			    yyval.namedNumberPtr->export.value = *yyvsp[-1].valuePtr;
			    smiFree(yyvsp[-1].valuePtr);
			;
    break;}
case 218:
#line 3628 "parser-smi.y"
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
case 219:
#line 3638 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    /* TODO: non-negative is suggested */
			;
    break;}
case 220:
#line 3647 "parser-smi.y"
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
				    yyval.status = SMI_STATUS_UNKNOWN;
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
				    yyval.status = SMI_STATUS_UNKNOWN;
				}
			    }
			    smiFree(yyvsp[0].id);
			;
    break;}
case 221:
#line 3683 "parser-smi.y"
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
case 222:
#line 3699 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 223:
#line 3703 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 224:
#line 3709 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 225:
#line 3713 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 226:
#line 3719 "parser-smi.y"
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
case 227:
#line 3768 "parser-smi.y"
{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			;
    break;}
case 228:
#line 3776 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = yyvsp[-1].listPtr;
			    yyval.index.rowPtr    = NULL;
			;
    break;}
case 229:
#line 3785 "parser-smi.y"
{
			    yyval.index.indexkind    = SMI_INDEX_AUGMENT;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
			;
    break;}
case 230:
#line 3792 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;
    break;}
case 231:
#line 3798 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 232:
#line 3805 "parser-smi.y"
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
case 233:
#line 3818 "parser-smi.y"
{
			    impliedFlag = 1;
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 234:
#line 3823 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 235:
#line 3833 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 236:
#line 3839 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 237:
#line 3845 "parser-smi.y"
{ yyval.valuePtr = yyvsp[-1].valuePtr; ;
    break;}
case 238:
#line 3847 "parser-smi.y"
{ yyval.valuePtr = NULL; ;
    break;}
case 239:
#line 3852 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 240:
#line 3854 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
			    yyval.valuePtr->value.ptr = (void *)yyvsp[-1].listPtr;
			;
    break;}
case 241:
#line 3862 "parser-smi.y"
{ yyval.listPtr = yyvsp[0].listPtr; ;
    break;}
case 242:
#line 3864 "parser-smi.y"
{ yyval.listPtr = NULL; ;
    break;}
case 243:
#line 3868 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].id;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 244:
#line 3874 "parser-smi.y"
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
case 245:
#line 3887 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 246:
#line 3893 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 247:
#line 3899 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 248:
#line 3901 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 249:
#line 3905 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 250:
#line 3907 "parser-smi.y"
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
case 251:
#line 3919 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 252:
#line 3921 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 253:
#line 3926 "parser-smi.y"
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
			    
			    if (addRevision(yyvsp[-2].date, yyvsp[0].text, thisParserPtr))
				yyval.err = 0;
			    else
				yyval.err = -1;
			;
    break;}
case 254:
#line 3948 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 255:
#line 3952 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 256:
#line 3958 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 257:
#line 3964 "parser-smi.y"
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
case 258:
#line 3977 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 259:
#line 3983 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 260:
#line 3989 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 261:
#line 3995 "parser-smi.y"
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
case 262:
#line 4008 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 263:
#line 4014 "parser-smi.y"
{
			    yyval.text = smiStrdup(yyvsp[0].text);
			;
    break;}
case 264:
#line 4020 "parser-smi.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 265:
#line 4025 "parser-smi.y"
{
			    parentNodePtr = rootNodePtr;
			;
    break;}
case 266:
#line 4029 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    parentNodePtr = yyvsp[0].objectPtr->nodePtr;
			;
    break;}
case 267:
#line 4037 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 268:
#line 4042 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 269:
#line 4050 "parser-smi.y"
{
			    Object *objectPtr;
			    Import *importPtr;
			    
			    if (parentNodePtr != rootNodePtr) {
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_NOT_FIRST, yyvsp[0].id);
			    }
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[0].id);
			    if (objectPtr) {
				yyval.objectPtr = objectPtr;
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
						smiStrdup(yyvsp[0].id),
						thisParserPtr);
					    setImportModulename(importPtr,
								complianceModulePtr->export.name);
					    importPtr->use++;
					} else {
					    objectPtr = addObject(yyvsp[0].id,
								  pendingNodePtr, 0,
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
						smiStrdup(yyvsp[0].id),
						thisParserPtr);
					    setImportModulename(importPtr,
								capabilitiesModulePtr->
								export.name);
					    importPtr->use++;
					} else {
					    objectPtr = addObject(yyvsp[0].id,
								  pendingNodePtr, 0,
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
							      pendingNodePtr,
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
				}
			    }
			    if (yyval.objectPtr)
				parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 270:
#line 4138 "parser-smi.y"
{
			    Object *objectPtr;
			    Import *importPtr;
			    char *md;
			    
			    if (parentNodePtr != rootNodePtr) {
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
						    smiStrdup(yyvsp[-2].id),
						    thisParserPtr);
						setImportModulename(importPtr,
						    complianceModulePtr->export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    pendingNodePtr, 0,
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
						    smiStrdup(yyvsp[-2].id),
						    thisParserPtr);
						setImportModulename(importPtr,
						        capabilitiesModulePtr->
								  export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    pendingNodePtr, 0,
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
					    objectPtr = addObject(yyvsp[-2].id,
							    pendingNodePtr,
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
					    importPtr->export.module, yyvsp[-2].id);
				    }
				}
				if (yyval.objectPtr)
				    parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			;
    break;}
case 271:
#line 4233 "parser-smi.y"
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
case 272:
#line 4257 "parser-smi.y"
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
case 273:
#line 4286 "parser-smi.y"
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
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_ILLEGALLY_QUALIFIED, md);
				objectPtr = addObject(yyvsp[-3].id, parentNodePtr,
						   yyvsp[-1].unsigned32, 0,
						   thisParserPtr);
				yyval.objectPtr = objectPtr;
			    }
			    smiFree(md);
			    if (yyval.objectPtr)
				parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 274:
#line 4318 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 275:
#line 4322 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 276:
#line 4324 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 277:
#line 4328 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 278:
#line 4330 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 279:
#line 4334 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 280:
#line 4343 "parser-smi.y"
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
case 281:
#line 4364 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-13].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTGROUP);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-7].status);
			    setObjectDescription(objectPtr, yyvsp[-5].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-9].listPtr);
			    yyval.err = 0;
			;
    break;}
case 282:
#line 4390 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 283:
#line 4399 "parser-smi.y"
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
case 284:
#line 4421 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-13].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONGROUP);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-7].status);
			    setObjectDescription(objectPtr, yyvsp[-5].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-9].listPtr);
			    yyval.err = 0;
			;
    break;}
case 285:
#line 4448 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 286:
#line 4457 "parser-smi.y"
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
case 287:
#line 4478 "parser-smi.y"
{
			    Object *objectPtr;
			    Option *optionPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-13].id, &objectPtr);

			    setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
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
case 288:
#line 4537 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 289:
#line 4543 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 290:
#line 4547 "parser-smi.y"
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
case 291:
#line 4582 "parser-smi.y"
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
case 292:
#line 4598 "parser-smi.y"
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
case 293:
#line 4611 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 294:
#line 4619 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 295:
#line 4627 "parser-smi.y"
{
			    yyval.modulePtr = thisModulePtr;
			;
    break;}
case 296:
#line 4633 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 297:
#line 4637 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 298:
#line 4643 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 299:
#line 4649 "parser-smi.y"
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
case 300:
#line 4662 "parser-smi.y"
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
case 301:
#line 4685 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			;
    break;}
case 302:
#line 4691 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 303:
#line 4699 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 304:
#line 4703 "parser-smi.y"
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
case 305:
#line 4770 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].listPtr;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 306:
#line 4776 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 307:
#line 4785 "parser-smi.y"
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
			    
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = smiMalloc(sizeof(Option));
			    ((Option *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-2].objectPtr;
			    if (! (thisModulePtr->flags & SMI_FLAG_NODESCR)) {
				((Option *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			    }
			;
    break;}
case 308:
#line 4812 "parser-smi.y"
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
			    }
			;
    break;}
case 309:
#line 4839 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 310:
#line 4847 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 311:
#line 4853 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 312:
#line 4861 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 313:
#line 4867 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 314:
#line 4873 "parser-smi.y"
{
			    yyval.access = yyvsp[0].access;
			;
    break;}
case 315:
#line 4877 "parser-smi.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 316:
#line 4883 "parser-smi.y"
{
			    firstStatementLine = thisParserPtr->line;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 317:
#line 4892 "parser-smi.y"
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
case 318:
#line 4915 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-15].id, &objectPtr);

			    setObjectName(objectPtr, yyvsp[-15].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    setObjectLine(objectPtr, firstStatementLine,
					  thisParserPtr);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
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
case 319:
#line 4945 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 320:
#line 4947 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 321:
#line 4951 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 322:
#line 4953 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 323:
#line 4957 "parser-smi.y"
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
case 324:
#line 4973 "parser-smi.y"
{
			    if (capabilitiesModulePtr) {
				checkImports(capabilitiesModulePtr,
					     thisParserPtr);
				capabilitiesModulePtr = NULL;
			    }
			    yyval.err = 0;
			;
    break;}
case 325:
#line 4984 "parser-smi.y"
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
case 326:
#line 4994 "parser-smi.y"
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
case 327:
#line 5011 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 328:
#line 5017 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 329:
#line 5025 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 330:
#line 5034 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 331:
#line 5036 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 332:
#line 5040 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 333:
#line 5042 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 334:
#line 5046 "parser-smi.y"
{
			    if (yyvsp[0].objectPtr) {
				variationkind = yyvsp[0].objectPtr->export.nodekind;
			    } else {
				variationkind = SMI_NODEKIND_UNKNOWN;
			    }
			;
    break;}
case 335:
#line 5054 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_SYNTAX);
			    }
			;
    break;}
case 336:
#line 5061 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_WRITESYNTAX);
			    }
			;
    break;}
case 337:
#line 5069 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_CREATION);
			    }
			;
    break;}
case 338:
#line 5076 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_DEFVAL);
			    }
			;
    break;}
case 339:
#line 5083 "parser-smi.y"
{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			    variationkind = SMI_NODEKIND_UNKNOWN;
			;
    break;}
case 340:
#line 5091 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 341:
#line 5093 "parser-smi.y"
{ yyval.access = 0; ;
    break;}
case 342:
#line 5097 "parser-smi.y"
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
case 343:
#line 5162 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 344:
#line 5164 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 345:
#line 5168 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 346:
#line 5170 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 347:
#line 5174 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 542 "/usr/lib/bison.simple"

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
#line 5177 "parser-smi.y"


#endif
