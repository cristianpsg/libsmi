
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
checkIndex(Parser *parserPtr, Object *objectPtr)
{

    /*
     * TODO: check that integers are positive
     */

    /*
     * TODO: check that index components are not-accessible
     */
}


static void
checkObjects(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr, *parentPtr;
    Node *nodePtr;
    int i;
    
    for (objectPtr = modulePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextPtr) {
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
	 * TODO: check whether the row is the only node below the
         * table node
	 */

	/*
	 * Check INDEX constraints for row objects.
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_ROW) {
	    checkIndex(parserPtr, objectPtr);
	}
	
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
#if 0
		    fprintf(stderr,
			    "** type refinement of %s derived from %s\n",
			    refinementPtr->objectPtr->export.name,
			    typePtr->parentPtr->export.name);
#endif
		}

		typePtr = refinementPtr->writetypePtr;
		if (typePtr) {
		    if (typePtr->export.status == SMI_STATUS_UNKNOWN) {
			typePtr->export.status = objectPtr->export.status;
		    }
#if 0
		    fprintf(stderr,
			    "** write type refinement of %s derived from %s\n",
			    refinementPtr->objectPtr->export.name,
			    typePtr->parentPtr->export.name);
#endif
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
			 nodePtr != nodePtr->parentPtr;
		     nodePtr = nodePtr->parentPtr, i++);
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
	 * Check named numbers lists for redefinitions of names or
	 * redefinitions of numbers.
	 */

	if (typePtr
	    && (typePtr->export.basetype == SMI_BASETYPE_ENUM
		|| typePtr->export.basetype == SMI_BASETYPE_BITS)) {
	    
	    List *list1Ptr, *list2Ptr;
	    NamedNumber *nn1Ptr, *nn2Ptr;

	    for (list1Ptr = typePtr->listPtr;
		 list1Ptr; list1Ptr = list1Ptr->nextPtr) {

		nn1Ptr = (NamedNumber *)(list1Ptr->ptr);

		for (list2Ptr = list1Ptr->nextPtr;
		     list2Ptr; list2Ptr = list2Ptr->nextPtr) {

		    nn2Ptr = (NamedNumber *)(list2Ptr->ptr);

		    if (typePtr->export.basetype == SMI_BASETYPE_ENUM) {
			if (!strcmp(nn1Ptr->export.name,
				    nn2Ptr->export.name)) {
			    smiPrintErrorAtLine(parserPtr,
						ERR_ENUM_NAME_REDEFINITION,
						typePtr->line,
						nn1Ptr->export.name);
			}
			if (nn1Ptr->export.value.value.integer32
			    == nn2Ptr->export.value.value.integer32) {
			    smiPrintErrorAtLine(parserPtr,
						ERR_ENUM_NUMBER_REDEFINITION,
						typePtr->line,
					nn1Ptr->export.value.value.integer32);
			}
		    }
		    if (typePtr->export.basetype == SMI_BASETYPE_BITS) {
			if (!strcmp(nn1Ptr->export.name,
				    nn2Ptr->export.name)) {
			    smiPrintErrorAtLine(parserPtr,
						ERR_BITS_NAME_REDEFINITION,
						typePtr->line,
						nn1Ptr->export.name);
			}
			if (nn1Ptr->export.value.value.unsigned32
			    == nn2Ptr->export.value.value.unsigned32) {
			    smiPrintErrorAtLine(parserPtr,
						ERR_BITS_NUMBER_REDEFINITION,
						typePtr->line,
					nn1Ptr->export.value.value.unsigned32);
			}
		    }
		}
	    }   
	}

	/*
	 * Check for "compatible" named numbers in sub-typed
	 * enumerations or named bits lists.
	 */

	if (typePtr->parentPtr
	    && typePtr->parentPtr->parentPtr
	    && (typePtr->export.basetype == SMI_BASETYPE_ENUM
		|| typePtr->export.basetype == SMI_BASETYPE_BITS)) {

	    List *list1Ptr, *list2Ptr;
	    NamedNumber *nn1Ptr, *nn2Ptr;

	    for (list1Ptr = typePtr->listPtr;
		 list1Ptr; list1Ptr = list1Ptr->nextPtr) {

		nn1Ptr = (NamedNumber *)(list1Ptr->ptr);

		for (list2Ptr = typePtr->parentPtr->listPtr;
		     list2Ptr; list2Ptr = list2Ptr->nextPtr) {

		    nn2Ptr = (NamedNumber *)(list2Ptr->ptr);

                    if (typePtr->export.basetype == SMI_BASETYPE_ENUM) {
			if (! strcmp(nn1Ptr->export.name, nn2Ptr->export.name)
			    && nn1Ptr->export.value.value.integer32
			    == nn2Ptr->export.value.value.integer32) {
			    break;
			}
		    }

		    if (typePtr->export.basetype == SMI_BASETYPE_BITS) {
			if (! strcmp(nn1Ptr->export.name, nn2Ptr->export.name)
			    && nn1Ptr->export.value.value.unsigned32
			    == nn2Ptr->export.value.value.unsigned32) {
			    break;
			}
		    }
		}

		if (! list2Ptr) {
		    if (typePtr->export.basetype == SMI_BASETYPE_ENUM) {
			smiPrintErrorAtLine(parserPtr, ERR_ENUM_SUBTYPE,
					    typePtr->line,
					    nn1Ptr->export.name,
					    typePtr->parentPtr->export.name);
		    }
		    if (typePtr->export.basetype == SMI_BASETYPE_BITS) {
			smiPrintErrorAtLine(parserPtr, ERR_BITS_SUBTYPE,
					    typePtr->line,
					    nn1Ptr->export.name,
					    typePtr->parentPtr->export.name);
		    }
		}
	    }
	}

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
	    if (importPtr->use == 0) {
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
	    if ( (i < len-1 && ! isdigit(date[i]))
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
    

#line 762 "parser-smi.y"
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



#define	YYFINAL		584
#define	YYFLAG		-32768
#define	YYNTBASE	90

#define YYTRANSLATE(x) ((unsigned)(x) <= 333 ? yytranslate[x] : 265)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    84,
    85,     2,     2,    81,     2,    88,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    80,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    86,     2,    87,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    83,    89,    82,     2,     2,     2,     2,     2,
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
     0,     2,     3,     5,     8,     9,    19,    21,    22,    26,
    27,    28,    32,    34,    35,    37,    40,    44,    46,    50,
    52,    54,    56,    58,    60,    62,    64,    66,    68,    70,
    72,    74,    76,    78,    80,    82,    84,    86,    88,    90,
    92,    94,    96,    98,   100,   101,   103,   106,   108,   110,
   112,   114,   116,   118,   120,   122,   124,   126,   128,   130,
   133,   134,   139,   141,   143,   145,   147,   149,   151,   153,
   155,   157,   159,   160,   164,   165,   174,   175,   180,   182,
   184,   186,   188,   190,   192,   194,   196,   198,   200,   202,
   203,   214,   216,   220,   222,   227,   229,   233,   236,   238,
   243,   245,   247,   250,   252,   256,   257,   263,   264,   265,
   279,   280,   281,   300,   301,   304,   305,   306,   318,   323,
   324,   326,   330,   332,   335,   336,   337,   341,   342,   346,
   347,   348,   363,   364,   365,   366,   385,   387,   390,   392,
   394,   396,   398,   404,   410,   412,   414,   416,   418,   419,
   423,   424,   428,   430,   431,   435,   436,   440,   445,   448,
   453,   456,   457,   462,   465,   470,   473,   475,   477,   479,
   481,   483,   485,   489,   492,   495,   499,   502,   504,   506,
   508,   511,   513,   516,   518,   520,   523,   525,   527,   529,
   532,   535,   537,   539,   541,   543,   545,   547,   548,   552,
   559,   561,   565,   567,   571,   573,   575,   577,   579,   583,
   585,   589,   590,   596,   598,   600,   602,   604,   607,   608,
   611,   612,   614,   615,   621,   626,   627,   629,   633,   636,
   638,   640,   642,   647,   648,   650,   654,   656,   657,   659,
   663,   665,   667,   670,   671,   673,   674,   676,   679,   684,
   689,   690,   692,   696,   698,   703,   705,   709,   711,   713,
   715,   716,   719,   721,   724,   726,   730,   732,   737,   744,
   746,   748,   751,   756,   758,   759,   760,   775,   776,   777,
   792,   793,   794,   809,   811,   813,   816,   817,   823,   826,
   828,   829,   834,   835,   837,   841,   843,   845,   846,   848,
   851,   853,   855,   860,   868,   871,   872,   875,   876,   878,
   881,   882,   883,   884,   901,   903,   904,   906,   909,   910,
   919,   921,   925,   927,   930,   932,   934,   935,   937,   940,
   941,   942,   943,   944,   945,   960,   963,   964,   966,   971,
   972,   974,   978
};

static const short yyrhs[] = {    91,
     0,     0,    92,     0,    91,    92,     0,     0,   104,    93,
    23,     4,    16,    96,    94,   105,    27,     0,    95,     0,
     0,    36,    98,    80,     0,     0,     0,    29,    97,    80,
     0,    99,     0,     0,   100,     0,    99,   100,     0,   101,
    30,   104,     0,   102,     0,   101,    81,   102,     0,     6,
     0,     5,     0,   103,     0,    13,     0,    17,     0,    21,
     0,    22,     0,    32,     0,    40,     0,    41,     0,    44,
     0,    48,     0,    49,     0,    51,     0,    52,     0,    54,
     0,    55,     0,    56,     0,    61,     0,    71,     0,    72,
     0,    73,     0,    76,     0,     5,     0,   106,     0,     0,
   107,     0,   106,   107,     0,   115,     0,   113,     0,   131,
     0,   134,     0,   138,     0,   148,     0,   151,     0,   222,
     0,   216,     0,   219,     0,   242,     0,   108,     0,     1,
    82,     0,     0,   110,    43,   109,    27,     0,    49,     0,
    56,     0,    73,     0,    52,     0,    55,     0,    71,     0,
    54,     0,    51,     0,    48,     0,    13,     0,     0,    18,
   112,    82,     0,     0,     6,   114,    53,    33,     4,    83,
   209,    82,     0,     0,   117,   116,     4,   119,     0,     5,
     0,   118,     0,    40,     0,    41,     0,    21,     0,    32,
     0,    76,     0,    72,     0,    61,     0,    22,     0,   126,
     0,     0,    71,   120,   182,    67,   180,    25,   207,   197,
    70,   126,     0,   111,     0,    65,    59,   122,     0,     5,
     0,    65,    83,   124,    82,     0,   125,     0,   124,    81,
   125,     0,     6,   127,     0,   155,     0,    17,    83,   128,
    82,     0,   157,     0,    17,     0,     5,   169,     0,   129,
     0,   128,    81,   129,     0,     0,     6,   130,    84,     7,
    85,     0,     0,     0,     6,   132,    55,   133,    67,   180,
    25,   207,   197,     4,    83,   209,    82,     0,     0,     0,
     6,   135,    56,   136,    70,   126,   183,   145,    67,   180,
   137,   197,   185,   191,     4,    83,   195,    82,     0,     0,
    25,   207,     0,     0,     0,     6,   139,    73,   140,    28,
   209,   141,   144,   197,     4,     7,     0,    77,    83,   142,
    82,     0,     0,   143,     0,   142,    81,   143,     0,   195,
     0,    25,   207,     0,     0,     0,    45,   146,   184,     0,
     0,    12,   147,   184,     0,     0,     0,     6,   149,    52,
   150,   201,    67,   180,    25,   207,   197,     4,    83,   196,
    82,     0,     0,     0,     0,     6,   152,    49,   153,    42,
   208,   154,    60,   207,    19,   207,    25,   207,   198,     4,
    83,   209,    82,     0,   159,     0,   156,   159,     0,   121,
     0,   122,     0,   123,     0,   167,     0,    86,    14,     7,
    87,    34,     0,    86,    75,     7,    87,    34,     0,   166,
     0,   168,     0,   165,     0,    39,     0,     0,    39,   160,
   170,     0,     0,    39,   161,   175,     0,    40,     0,     0,
    40,   162,   170,     0,     0,     5,   163,   175,     0,   104,
    88,     5,   175,     0,     5,   170,     0,   104,    88,     5,
   170,     0,    58,    68,     0,     0,    58,    68,   164,   171,
     0,     5,   171,     0,   104,    88,     5,   171,     0,    53,
    33,     0,     7,     0,     8,     0,     9,     0,    10,     0,
     6,     0,    11,     0,    83,   213,    82,     0,    39,   169,
     0,    40,   169,     0,    58,    68,   169,     0,    53,    33,
     0,    41,     0,    21,     0,    32,     0,    32,   170,     0,
    76,     0,    76,   170,     0,    72,     0,    61,     0,    61,
   171,     0,    22,     0,    41,     0,    21,     0,    32,   169,
     0,    76,   169,     0,    72,     0,    61,     0,    22,     0,
   170,     0,   171,     0,   175,     0,     0,    84,   172,    85,
     0,    84,    66,    84,   172,    85,    85,     0,   173,     0,
   172,    89,   173,     0,   174,     0,   174,     3,   174,     0,
     8,     0,     7,     0,    10,     0,     9,     0,    83,   176,
    82,     0,   177,     0,   176,    81,   177,     0,     0,     6,
   178,    84,   179,    85,     0,     7,     0,     8,     0,     6,
     0,     6,     0,    26,   207,     0,     0,    74,   207,     0,
     0,     6,     0,     0,    38,   186,    83,   187,    82,     0,
    15,    83,   190,    82,     0,     0,   188,     0,   187,    81,
   188,     0,    35,   189,     0,   189,     0,   195,     0,   195,
     0,    24,    83,   192,    82,     0,     0,   158,     0,    83,
   193,    82,     0,   194,     0,     0,     6,     0,   194,    81,
     6,     0,   209,     0,   209,     0,    63,   207,     0,     0,
   199,     0,     0,   200,     0,   199,   200,     0,    64,   208,
    25,   207,     0,    57,    83,   202,    82,     0,     0,   203,
     0,   202,    81,   203,     0,   195,     0,    50,    83,   205,
    82,     0,   206,     0,   205,    81,   206,     0,   196,     0,
    11,     0,    11,     0,     0,   210,   211,     0,   212,     0,
   211,   212,     0,     6,     0,   104,    88,     6,     0,     7,
     0,     6,    84,     7,    85,     0,   104,    88,     6,    84,
     7,    85,     0,   214,     0,   215,     0,   214,   215,     0,
     6,    84,     7,    85,     0,     7,     0,     0,     0,     6,
   217,    54,   218,   201,    67,   180,    25,   207,   197,     4,
    83,   209,    82,     0,     0,     0,     6,   220,    51,   221,
   204,    67,   180,    25,   207,   197,     4,    83,   209,    82,
     0,     0,     0,     6,   223,    48,   224,    67,   180,    25,
   207,   197,   225,     4,    83,   209,    82,     0,   226,     0,
   227,     0,   226,   227,     0,     0,    47,   229,   228,   230,
   233,     0,     5,   209,     0,     5,     0,     0,    44,    83,
   231,    82,     0,     0,   232,     0,   231,    81,   232,     0,
   209,     0,   234,     0,     0,   235,     0,   234,   235,     0,
   236,     0,   237,     0,    31,   209,    25,   207,     0,    53,
   195,   238,   239,   241,    25,   207,     0,    70,   126,     0,
     0,    79,   240,     0,     0,   126,     0,    46,   184,     0,
     0,     0,     0,     6,   243,    13,   244,    62,   207,    67,
   181,    25,   207,   197,   245,     4,    83,   209,    82,     0,
   246,     0,     0,   247,     0,   246,   247,     0,     0,    69,
   251,   248,    37,    83,   249,    82,   252,     0,   250,     0,
   249,    81,   250,     0,   209,     0,     5,   209,     0,     5,
     0,   253,     0,     0,   254,     0,   253,   254,     0,     0,
     0,     0,     0,     0,    78,   195,   255,   238,   256,   239,
   257,   260,   262,   258,   191,   259,    25,   207,     0,    12,
   261,     0,     0,     6,     0,    20,    83,   263,    82,     0,
     0,   264,     0,   263,    81,   264,     0,   195,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
  1015,  1020,  1026,  1028,  1036,  1071,  1090,  1092,  1096,  1106,
  1108,  1117,  1120,  1122,  1127,  1129,  1133,  1174,  1176,  1184,
  1190,  1196,  1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,
  1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,
  1230,  1231,  1234,  1246,  1248,  1253,  1255,  1259,  1264,  1269,
  1274,  1279,  1284,  1289,  1294,  1299,  1304,  1309,  1314,  1319,
  1333,  1357,  1364,  1365,  1366,  1367,  1368,  1369,  1370,  1371,
  1372,  1373,  1376,  1393,  1402,  1419,  1442,  1447,  1523,  1527,
  1549,  1550,  1551,  1552,  1553,  1554,  1555,  1556,  1559,  1575,
  1594,  1625,  1633,  1647,  1689,  1697,  1703,  1723,  1755,  1761,
  1781,  1786,  1791,  1825,  1831,  1844,  1857,  1879,  1887,  1903,
  1932,  1940,  1960,  2054,  2063,  2069,  2077,  2095,  2148,  2152,
  2158,  2164,  2177,  2183,  2185,  2189,  2198,  2199,  2208,  2211,
  2219,  2234,  2264,  2272,  2296,  2300,  2336,  2340,  2356,  2361,
  2366,  2371,  2388,  2390,  2398,  2403,  2420,  2429,  2441,  2446,
  2463,  2468,  2480,  2498,  2503,  2523,  2528,  2575,  2616,  2676,
  2728,  2733,  2738,  2749,  2798,  2838,  2845,  2852,  2858,  2883,
  2905,  2925,  2942,  2974,  2978,  2996,  3000,  3006,  3014,  3022,
  3030,  3053,  3073,  3100,  3108,  3119,  3143,  3157,  3165,  3173,
  3181,  3201,  3209,  3220,  3230,  3237,  3244,  3251,  3259,  3269,
  3279,  3285,  3298,  3305,  3315,  3321,  3327,  3349,  3375,  3381,
  3387,  3400,  3413,  3422,  3432,  3441,  3477,  3493,  3497,  3503,
  3507,  3513,  3562,  3570,  3577,  3586,  3592,  3598,  3612,  3617,
  3623,  3633,  3639,  3641,  3646,  3648,  3656,  3658,  3662,  3668,
  3681,  3687,  3693,  3695,  3699,  3701,  3713,  3715,  3719,  3742,
  3746,  3752,  3758,  3771,  3777,  3783,  3789,  3802,  3808,  3814,
  3820,  3824,  3830,  3835,  3842,  3931,  4026,  4050,  4079,  4111,
  4115,  4117,  4121,  4123,  4127,  4135,  4149,  4180,  4188,  4203,
  4235,  4243,  4257,  4322,  4328,  4332,  4367,  4383,  4396,  4404,
  4411,  4418,  4422,  4428,  4434,  4447,  4470,  4476,  4484,  4488,
  4555,  4561,  4569,  4593,  4624,  4633,  4639,  4648,  4654,  4660,
  4664,  4670,  4678,  4692,  4729,  4731,  4735,  4737,  4741,  4756,
  4768,  4778,  4795,  4801,  4809,  4818,  4820,  4824,  4826,  4830,
  4839,  4846,  4853,  4861,  4867,  4875,  4877,  4881,  4946,  4948,
  4952,  4954,  4958
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
"UNSIGNED32","VARIABLES","VARIATION","WRITE_SYNTAX","';'","','","'}'","'{'",
"'('","')'","'['","']'","'.'","'|'","mibFile","modules","module","@1","linkagePart",
"linkageClause","exportsClause","@2","importPart","imports","import","importIdentifiers",
"importIdentifier","importedKeyword","moduleName","declarationPart","declarations",
"declaration","macroClause","@3","macroName","choiceClause","@4","valueDeclaration",
"@5","typeDeclaration","@6","typeName","typeSMI","typeDeclarationRHS","@7","conceptualTable",
"row","entryType","sequenceItems","sequenceItem","Syntax","sequenceSyntax","NamedBits",
"NamedBit","@8","objectIdentityClause","@9","@10","objectTypeClause","@11","@12",
"descriptionClause","trapTypeClause","@13","@14","VarPart","VarTypes","VarType",
"DescrPart","MaxAccessPart","@15","@16","notificationTypeClause","@17","@18",
"moduleIdentityClause","@19","@20","@21","ObjectSyntax","typeTag","sequenceObjectSyntax",
"valueofObjectSyntax","SimpleSyntax","@22","@23","@24","@25","@26","valueofSimpleSyntax",
"sequenceSimpleSyntax","ApplicationSyntax","sequenceApplicationSyntax","anySubType",
"integerSubType","octetStringSubType","ranges","range","value","enumSpec","enumItems",
"enumItem","@27","enumNumber","Status","Status_Capabilities","DisplayPart","UnitsPart",
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
    90,    90,    91,    91,    93,    92,    94,    94,    95,    96,
    97,    96,    98,    98,    99,    99,   100,   101,   101,   102,
   102,   102,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   104,   105,   105,   106,   106,   107,   107,   107,
   107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
   109,   108,   110,   110,   110,   110,   110,   110,   110,   110,
   110,   110,   112,   111,   114,   113,   116,   115,   117,   117,
   118,   118,   118,   118,   118,   118,   118,   118,   119,   120,
   119,   119,   121,   122,   123,   124,   124,   125,   126,   126,
   127,   127,   127,   128,   128,   130,   129,   132,   133,   131,
   135,   136,   134,   137,   137,   139,   140,   138,   141,   141,
   142,   142,   143,   144,   144,   146,   145,   147,   145,   149,
   150,   148,   152,   153,   154,   151,   155,   155,   155,   155,
   155,   155,   156,   156,   157,   157,   158,   159,   160,   159,
   161,   159,   159,   162,   159,   163,   159,   159,   159,   159,
   159,   164,   159,   159,   159,   159,   165,   165,   165,   165,
   165,   165,   165,   166,   166,   166,   166,   167,   167,   167,
   167,   167,   167,   167,   167,   167,   167,   168,   168,   168,
   168,   168,   168,   168,   169,   169,   169,   169,   170,   171,
   172,   172,   173,   173,   174,   174,   174,   174,   175,   176,
   176,   178,   177,   179,   179,   180,   181,   182,   182,   183,
   183,   184,   186,   185,   185,   185,   187,   187,   188,   188,
   189,   190,   191,   191,   192,   192,   193,   193,   194,   194,
   195,   196,   197,   197,   198,   198,   199,   199,   200,   201,
   201,   202,   202,   203,   204,   205,   205,   206,   207,   208,
   210,   209,   211,   211,   212,   212,   212,   212,   212,   213,
   214,   214,   215,   215,   217,   218,   216,   220,   221,   219,
   223,   224,   222,   225,   226,   226,   228,   227,   229,   229,
   229,   230,   230,   231,   231,   232,   233,   233,   234,   234,
   235,   235,   236,   237,   238,   238,   239,   239,   240,   241,
   241,   243,   244,   242,   245,   245,   246,   246,   248,   247,
   249,   249,   250,   251,   251,   252,   252,   253,   253,   255,
   256,   257,   258,   259,   254,   260,   260,   261,   262,   262,
   263,   263,   264
};

static const short yyr2[] = {     0,
     1,     0,     1,     2,     0,     9,     1,     0,     3,     0,
     0,     3,     1,     0,     1,     2,     3,     1,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     0,     1,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
     0,     4,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     0,     3,     0,     8,     0,     4,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
    10,     1,     3,     1,     4,     1,     3,     2,     1,     4,
     1,     1,     2,     1,     3,     0,     5,     0,     0,    13,
     0,     0,    18,     0,     2,     0,     0,    11,     4,     0,
     1,     3,     1,     2,     0,     0,     3,     0,     3,     0,
     0,    14,     0,     0,     0,    18,     1,     2,     1,     1,
     1,     1,     5,     5,     1,     1,     1,     1,     0,     3,
     0,     3,     1,     0,     3,     0,     3,     4,     2,     4,
     2,     0,     4,     2,     4,     2,     1,     1,     1,     1,
     1,     1,     3,     2,     2,     3,     2,     1,     1,     1,
     2,     1,     2,     1,     1,     2,     1,     1,     1,     2,
     2,     1,     1,     1,     1,     1,     1,     0,     3,     6,
     1,     3,     1,     3,     1,     1,     1,     1,     3,     1,
     3,     0,     5,     1,     1,     1,     1,     2,     0,     2,
     0,     1,     0,     5,     4,     0,     1,     3,     2,     1,
     1,     1,     4,     0,     1,     3,     1,     0,     1,     3,
     1,     1,     2,     0,     1,     0,     1,     2,     4,     4,
     0,     1,     3,     1,     4,     1,     3,     1,     1,     1,
     0,     2,     1,     2,     1,     3,     1,     4,     6,     1,
     1,     2,     4,     1,     0,     0,    14,     0,     0,    14,
     0,     0,    14,     1,     1,     2,     0,     5,     2,     1,
     0,     4,     0,     1,     3,     1,     1,     0,     1,     2,
     1,     1,     4,     7,     2,     0,     2,     0,     1,     2,
     0,     0,     0,    16,     1,     0,     1,     2,     0,     8,
     1,     3,     1,     2,     1,     1,     0,     1,     2,     0,
     0,     0,     0,     0,    14,     2,     0,     1,     4,     0,
     1,     3,     1
};

static const short yydefact[] = {     2,
    43,     1,     3,     5,     4,     0,     0,     0,    10,    11,
     8,     0,    14,     0,     7,    12,    21,    20,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,     0,    13,
    15,     0,    18,    22,     0,    79,    75,    72,    83,    88,
    84,    81,    82,    71,    63,    70,    66,    69,    67,    64,
    87,    68,    86,    65,    85,     0,     0,    46,    59,     0,
    49,    48,    77,    80,    50,    51,    52,    53,    54,    56,
    57,    55,    58,     9,    16,     0,     0,    60,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     6,    47,
    61,     0,    17,    19,     0,   109,   112,   117,   131,   134,
   276,   279,   282,   313,     0,     0,     0,     0,     0,     0,
   251,     0,   251,     0,     0,     0,    62,    94,     0,    73,
   179,   187,   180,   148,   153,   178,     0,     0,   185,     0,
    90,   184,   182,     0,     0,    92,    78,   139,   140,   141,
    89,    99,     0,   137,   142,     0,     0,     0,   261,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   159,
   164,     0,     0,     0,   181,     0,     0,     0,   166,   161,
     0,   186,     0,     0,   219,   183,     0,     0,     0,    43,
   138,   261,   216,     0,   221,   120,     0,   261,     0,   260,
   135,     0,   261,     0,     0,   259,     0,   206,   205,   208,
   207,     0,     0,   201,   203,     0,   157,   106,     0,   104,
    74,   150,   152,   155,     0,    94,    93,     0,     0,    96,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   125,   265,   267,     0,   262,   263,   254,     0,   252,   241,
     0,     0,     0,   258,     0,   256,   242,     0,     0,     0,
     0,   199,     0,     0,   212,     0,   210,     0,     0,   100,
   163,   198,   102,   189,   194,   198,   198,   198,   188,     0,
     0,   193,   192,   198,    98,   101,   145,   146,     0,    95,
   218,     0,     0,     0,   160,   165,   158,    76,   244,   220,
   128,   126,     0,   261,     0,   244,     0,     0,   264,   261,
   250,     0,     0,     0,   261,   255,     0,   244,   217,     0,
     0,   202,   204,     0,     0,   209,     0,   105,   103,   195,
   196,   197,   190,   174,   175,   177,   198,   191,    97,     0,
   143,   144,     0,     0,     0,     0,     0,     0,   121,   123,
   124,     0,     0,   266,   253,   244,     0,   244,   257,   244,
     0,     0,     0,     0,   211,     0,   176,     0,   243,     0,
   222,   129,   127,   114,   261,   119,     0,   268,     0,     0,
     0,     0,     0,   291,     0,   284,   285,   244,   200,   214,
   215,     0,   107,   244,   261,     0,   244,   122,   118,     0,
     0,     0,     0,     0,   290,   287,     0,   286,   316,   213,
     0,     0,   115,   226,   269,   261,     0,   261,   261,   289,
   293,   261,     0,     0,   315,   317,     0,   110,     0,   223,
   234,     0,   246,     0,     0,     0,   298,     0,   261,   319,
     0,   318,    91,   261,     0,     0,     0,   132,     0,     0,
   245,   247,   277,   280,   261,   261,   261,   288,   297,   299,
   301,   302,   283,   324,     0,   261,     0,   232,   261,     0,
     0,     0,     0,   248,   296,     0,   294,     0,   306,   300,
     0,     0,   225,   261,     0,   227,   230,   231,   171,   167,
   168,   169,   170,   172,   238,   235,   147,     0,   261,     0,
   261,   261,   292,     0,     0,   308,   261,   314,   229,   261,
   224,   239,   274,     0,   237,     0,   270,   271,   233,     0,
   249,     0,   295,   303,   305,     0,   311,   323,     0,   321,
   228,     0,   236,     0,   173,     0,   272,   113,   136,   309,
   307,     0,     0,   261,   327,     0,   240,   310,     0,   322,
   261,   320,   326,   328,   273,   304,   330,   329,   306,   331,
   308,   332,   337,     0,   340,   338,   336,     0,   333,   261,
   234,   343,     0,   341,   334,   261,   339,     0,   342,     0,
   335,     0,     0,     0
};

static const short yydefgoto[] = {   582,
     2,     3,     6,    14,    15,    11,    12,    39,    40,    41,
    42,    43,    44,   145,    66,    67,    68,    69,   115,    70,
   146,   173,    71,    89,    72,   102,    73,    74,   147,   185,
   148,   149,   150,   229,   230,   151,   285,   219,   220,   268,
    75,    90,   118,    76,    91,   119,   397,    77,    92,   120,
   241,   348,   349,   306,   303,   346,   345,    78,    93,   121,
    79,    94,   122,   252,   152,   153,   286,   496,   154,   176,
   177,   178,   169,   225,   497,   287,   155,   288,   329,   330,
   331,   213,   214,   215,   332,   266,   267,   324,   392,   194,
   320,   232,   239,   372,   431,   445,   485,   486,   487,   467,
   447,   498,   514,   515,   488,   254,   344,   450,   451,   452,
   161,   248,   249,   165,   255,   256,   207,   201,   250,   197,
   245,   246,   516,   517,   518,    80,    95,   123,    81,    96,
   124,    82,    97,   125,   385,   386,   387,   421,   406,   437,
   476,   477,   458,   459,   460,   461,   462,   506,   527,   541,
   543,    83,    98,   126,   424,   425,   426,   465,   529,   530,
   440,   552,   553,   554,   559,   561,   563,   571,   578,   565,
   567,   569,   573,   574
};

static const short yypact[] = {    10,
-32768,    10,-32768,-32768,-32768,    24,    63,    56,    71,-32768,
    43,     4,   436,   346,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    29,   436,
-32768,   -21,-32768,-32768,    38,-32768,    70,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    77,   383,-32768,-32768,    80,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    10,   436,-32768,    89,   100,
   115,    95,   120,   125,   121,   127,   128,   167,-32768,-32768,
-32768,   177,-32768,-32768,   149,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   158,   112,   185,   119,   122,   163,
   137,   154,   137,   145,   133,   139,-32768,   -29,   123,-32768,
-32768,-32768,   124,    -7,   126,-32768,   172,   141,   129,   -18,
-32768,-32768,   124,     0,   130,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    41,-32768,-32768,   132,   201,   182,-32768,   134,
   144,   205,   152,   142,   157,   201,   209,    30,   143,-32768,
-32768,   206,   146,   138,-32768,   124,   143,   124,-32768,   147,
   161,-32768,   224,   226,   204,-32768,   227,   230,   228,     9,
-32768,-32768,-32768,   213,   165,   164,    15,-32768,   201,-32768,
-32768,   201,-32768,   201,   217,-32768,   178,-32768,-32768,-32768,
-32768,   160,   -36,-32768,   246,   245,-32768,-32768,     7,-32768,
-32768,-32768,-32768,-32768,   129,-32768,-32768,   273,    16,-32768,
   209,   186,   168,   169,    22,   170,   209,   209,     6,   179,
   239,   181,-32768,   183,    15,-32768,-32768,    46,-32768,-32768,
   241,   207,   244,-32768,    50,-32768,-32768,   249,   209,   264,
   138,-32768,   138,   138,-32768,    55,-32768,   191,   206,-32768,
-32768,    22,-32768,-32768,-32768,    22,    22,    22,-32768,   243,
   211,-32768,-32768,    22,-32768,-32768,-32768,-32768,   226,-32768,
-32768,   201,   247,   248,-32768,-32768,-32768,-32768,   220,-32768,
-32768,-32768,   218,-32768,   209,   220,   277,   280,-32768,-32768,
-32768,   209,   209,   209,-32768,-32768,   209,   220,-32768,   262,
   -28,-32768,-32768,   208,   245,-32768,   281,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    22,-32768,-32768,   266,
-32768,-32768,   209,   285,   287,   287,   201,    58,-32768,-32768,
-32768,   294,   214,   216,-32768,   220,   282,   220,-32768,   220,
   255,   209,   219,   104,-32768,   221,-32768,   209,-32768,   225,
-32768,-32768,-32768,   284,-32768,-32768,   303,-32768,   304,   311,
   209,   312,   313,   314,   316,   255,-32768,   220,-32768,-32768,
-32768,   233,-32768,   220,-32768,   209,   220,-32768,-32768,   236,
   240,   299,   242,   250,    57,-32768,   252,-32768,   258,-32768,
   259,   254,-32768,    28,-32768,-32768,   209,-32768,-32768,-32768,
   286,-32768,   323,   328,   258,-32768,   182,-32768,   256,-32768,
   317,   272,   274,   275,   276,   257,    21,   278,   300,-32768,
   261,-32768,-32768,-32768,   263,   279,   352,-32768,   205,   357,
   274,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    21,-32768,
-32768,-32768,-32768,-32768,   326,-32768,   283,-32768,   329,    19,
   289,   341,   292,-32768,-32768,    68,-32768,   344,   306,-32768,
   296,   288,-32768,-32768,    76,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   153,-32768,-32768,   295,-32768,   209,
-32768,-32768,-32768,   209,   182,   302,-32768,-32768,-32768,   329,
-32768,   298,-32768,   301,   309,   310,   155,-32768,-32768,   321,
-32768,   324,-32768,-32768,-32768,   182,   345,-32768,    82,-32768,
-32768,   386,-32768,   393,-32768,   298,-32768,-32768,-32768,-32768,
-32768,   287,   384,-32768,   330,   327,-32768,-32768,   209,-32768,
-32768,-32768,   330,-32768,-32768,-32768,-32768,-32768,   306,-32768,
   302,-32768,   399,   407,   394,-32768,-32768,   333,-32768,-32768,
   317,-32768,    85,-32768,-32768,-32768,-32768,   395,-32768,   209,
-32768,   421,   425,-32768
};

static const short yypgoto[] = {-32768,
-32768,   424,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   387,
-32768,   342,-32768,     5,-32768,-32768,   361,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   253,-32768,-32768,   151,  -155,-32768,-32768,   174,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    72,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   293,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -242,  -120,
  -122,   184,   187,   166,  -153,-32768,   108,-32768,-32768,  -154,
-32768,-32768,-32768,  -345,-32768,-32768,-32768,   -62,   -33,-32768,
  -119,-32768,-32768,-32768,  -196,    44,  -287,-32768,-32768,    11,
   338,-32768,   156,-32768,-32768,   148,  -227,    18,  -159,-32768,
-32768,   229,-32768,-32768,   -53,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    79,-32768,-32768,-32768,
-32768,   -32,-32768,-32768,    12,-32768,-32768,   -90,   -89,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    48,-32768,-32768,   -69,
-32768,-32768,-32768,   -75,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   -97
};


#define	YYLAST		512


static const short yytable[] = {   196,
   373,   247,   195,   291,     4,   171,     4,   170,    86,   299,
   300,   205,   175,   187,     1,   217,   182,   301,   352,     1,
   242,   243,   186,   223,   489,   490,   491,   492,   493,   494,
   361,   318,   236,   333,   334,   335,   208,   209,   210,   211,
   183,   338,   429,   257,   251,   190,     7,   253,   262,   258,
   302,   456,   263,  -156,   168,   222,   363,   224,   -43,    87,
   263,  -261,  -261,  -261,   184,   430,     8,   171,   380,   170,
   382,     9,   383,   457,   188,  -151,  -149,   351,    13,   134,
   135,   297,  -312,    16,   356,   357,   358,   269,   270,   360,
   103,  -156,   168,   137,   367,   212,   289,   290,   138,    10,
   409,   495,   271,    99,   216,   168,   411,   350,    84,   414,
   390,   391,   296,   247,   295,   369,   128,  -281,  -133,    88,
  -278,  -130,   101,  -275,  -108,  -111,   310,   311,   129,   130,
   315,   316,   131,   132,   388,   325,   326,   340,   375,   376,
   394,   105,  -116,   133,   208,   209,   210,   211,   502,   503,
   134,   135,   136,   402,   106,   257,   510,   511,   512,   513,
   536,   513,   544,   545,   137,   576,   577,   108,   413,   138,
   107,   109,   139,   110,   111,   113,   140,   112,   350,   114,
   116,   117,   141,   142,   127,   157,   128,   143,   156,   433,
   159,   158,   374,   160,   164,   162,   548,   144,   129,   166,
   167,   244,   131,   132,   179,   172,   193,   174,   180,  -154,
   199,   218,   181,   133,   192,   200,   198,   189,   202,   206,
   134,   135,   136,   204,   203,   216,   212,   221,   226,   231,
  -162,   228,   235,   233,   137,   412,   234,   237,   238,   138,
   240,   259,   139,   261,   260,   420,   140,   468,   264,   244,
   265,   298,   292,   142,   293,   294,   257,   143,   434,   435,
   479,   304,   438,   305,   307,   312,   313,   144,   314,   319,
   308,   443,   521,   317,   327,   336,   524,   272,   337,   464,
   341,   342,   343,   353,   347,   354,   362,   366,   370,   273,
   368,   364,   371,   274,   275,   475,   478,   377,   378,   379,
   381,   384,   520,   389,   276,   393,   482,   395,   396,   399,
   400,   277,   278,   279,   401,   403,   404,   410,   405,   407,
   415,   556,   416,   417,   418,   280,   423,   439,   427,   436,
   281,   441,   419,   282,   422,   428,  -325,   449,   444,   455,
   446,   522,   475,   466,   283,   469,    45,   528,   284,   525,
    46,    47,   581,   448,   557,   471,   453,   454,    48,   463,
   473,   470,   481,   484,   483,   500,    49,    50,   504,   508,
   540,   499,   -45,   572,   501,   505,   519,    51,   507,   572,
   526,   532,   533,    45,   528,    52,    53,    46,    47,   534,
   542,   535,   546,    54,    55,    48,    56,    57,   547,    58,
    59,    60,   538,    49,    50,   539,    61,   551,   549,   -44,
   564,   555,   566,   568,    51,   570,    62,    63,    64,   580,
   583,    65,    52,    53,   584,     5,    85,   100,   104,   323,
    54,    55,   365,    56,    57,   227,    58,    59,    60,   339,
    17,    18,   328,    61,   321,   191,   398,   531,    19,   322,
   509,   575,    20,    62,    63,    64,    21,    22,    65,   432,
   163,   474,   359,   537,   408,   355,   472,    23,   560,   523,
   480,   562,   442,   309,   550,    24,    25,   558,   579,    26,
     0,     0,     0,    27,    28,     0,    29,    30,     0,    31,
    32,    33,     0,     0,     0,     0,    34,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    35,    36,    37,     0,
     0,    38
};

static const short yycheck[] = {   159,
   346,   198,   158,   231,     0,   128,     2,   128,    30,   237,
   238,   166,   133,    14,     5,   169,   139,    12,   306,     5,
     6,     7,   143,   177,     6,     7,     8,     9,    10,    11,
   318,   259,   192,   276,   277,   278,     7,     8,     9,    10,
    59,   284,    15,   203,   199,     5,    23,   202,    85,   204,
    45,    31,    89,    83,    84,   176,    85,   178,    88,    81,
    89,     5,     6,     7,    83,    38,     4,   190,   356,   190,
   358,    16,   360,    53,    75,    83,    84,   305,    36,    39,
    40,   235,    13,    80,   312,   313,   314,    81,    82,   317,
    86,    83,    84,    53,   337,    66,    81,    82,    58,    29,
   388,    83,   225,    27,    83,    84,   394,   304,    80,   397,
     7,     8,   235,   310,   235,   343,     5,    48,    49,    82,
    51,    52,    43,    54,    55,    56,    81,    82,    17,    18,
    81,    82,    21,    22,   362,    81,    82,   292,    81,    82,
   368,    53,    73,    32,     7,     8,     9,    10,    81,    82,
    39,    40,    41,   381,    55,   315,    81,    82,     6,     7,
     6,     7,    81,    82,    53,    81,    82,    73,   396,    58,
    56,    52,    61,    49,    54,    48,    65,    51,   375,    13,
     4,    33,    71,    72,    27,    67,     5,    76,     4,   417,
    28,    70,   347,    57,    50,    42,   542,    86,    17,    67,
    62,   197,    21,    22,    33,    83,     6,    84,    68,    84,
    67,     6,    84,    32,    83,    11,    83,    88,    67,    11,
    39,    40,    41,    67,    83,    83,    66,    82,     5,    26,
    84,     6,     5,     7,    53,   395,     7,    25,    74,    58,
    77,    25,    61,    84,    67,   405,    65,   444,     3,   245,
     6,    82,    67,    72,    87,    87,   416,    76,   418,   419,
   457,    83,   422,    25,    84,    25,    60,    86,    25,     6,
    88,   427,   500,    25,    84,    33,   504,     5,    68,   439,
    34,    34,    63,     7,    67,     6,    25,     7,     4,    17,
    25,    84,     6,    21,    22,   455,   456,     4,    85,    84,
    19,    47,   499,    85,    32,    85,   466,    83,    25,     7,
     7,    39,    40,    41,     4,     4,     4,    85,     5,     4,
    85,   549,    83,    25,    83,    53,    69,     5,    70,    44,
    58,     4,    83,    61,    83,    82,    37,    64,    83,    83,
    24,   501,   502,    83,    72,    83,     1,   507,    76,   505,
     5,     6,   580,    82,   551,     4,    82,    82,    13,    82,
     4,    83,    37,    35,    82,    25,    21,    22,    25,    82,
   526,    83,    27,   570,    83,    70,    82,    32,    83,   576,
    79,    84,    82,     1,   544,    40,    41,     5,     6,    81,
    46,    82,     7,    48,    49,    13,    51,    52,     6,    54,
    55,    56,    82,    21,    22,    82,    61,    78,    25,    27,
    12,    85,     6,    20,    32,    83,    71,    72,    73,    25,
     0,    76,    40,    41,     0,     2,    40,    67,    87,   264,
    48,    49,   325,    51,    52,   183,    54,    55,    56,   289,
     5,     6,   269,    61,   261,   153,   375,   510,    13,   263,
   484,   571,    17,    71,    72,    73,    21,    22,    76,   416,
   123,   451,   315,   517,   386,   310,   449,    32,   559,   502,
   459,   561,   425,   245,   544,    40,    41,   553,   576,    44,
    -1,    -1,    -1,    48,    49,    -1,    51,    52,    -1,    54,
    55,    56,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,
    -1,    76
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
#line 1017 "parser-smi.y"
{
        yyval.err = 0;
    ;
    break;}
case 2:
#line 1021 "parser-smi.y"
{
	yyval.err = 0;
    ;
    break;}
case 3:
#line 1027 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 4:
#line 1029 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 5:
#line 1037 "parser-smi.y"
{
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
#line 1076 "parser-smi.y"
{
			    checkModuleIdentity(thisParserPtr, thisModulePtr);
			    checkObjects(thisParserPtr, thisModulePtr);
			    checkTypes(thisParserPtr, thisModulePtr);
			    checkDefvals(thisParserPtr, thisModulePtr);
			    checkImportsUsage(thisParserPtr, thisModulePtr);

                            yyval.err = 0;
			;
    break;}
case 7:
#line 1091 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 8:
#line 1093 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 9:
#line 1097 "parser-smi.y"
{
			    if (thisModulePtr->export.language != SMI_LANGUAGE_SMIV2)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    yyval.err = 0;
			;
    break;}
case 10:
#line 1107 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 11:
#line 1109 "parser-smi.y"
{
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_EXPORTS);
			    }
			;
    break;}
case 12:
#line 1117 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 13:
#line 1121 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 14:
#line 1123 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 15:
#line 1128 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 16:
#line 1130 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 17:
#line 1136 "parser-smi.y"
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
case 18:
#line 1175 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 19:
#line 1178 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 20:
#line 1185 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 21:
#line 1191 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 22:
#line 1197 "parser-smi.y"
{
			    addImport(smiStrdup(yyvsp[0].id), thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;
    break;}
case 43:
#line 1235 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 44:
#line 1247 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 45:
#line 1249 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 46:
#line 1254 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 47:
#line 1256 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 48:
#line 1260 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 49:
#line 1265 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 50:
#line 1270 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 51:
#line 1275 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 52:
#line 1280 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 53:
#line 1285 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 54:
#line 1290 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 55:
#line 1295 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 56:
#line 1300 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 57:
#line 1305 "parser-smi.y"
{
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 58:
#line 1310 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 59:
#line 1315 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 60:
#line 1320 "parser-smi.y"
{
			    smiPrintError(thisParserPtr,
					  ERR_FLUSH_DECLARATION);
			    yyerrok;
			    yyval.err = 1;
			;
    break;}
case 61:
#line 1335 "parser-smi.y"
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
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_MACRO);
			    }
			;
    break;}
case 62:
#line 1357 "parser-smi.y"
{
			    addMacro(yyvsp[-3].id, 0, thisParserPtr);
			    smiFree(yyvsp[-3].id);
			    yyval.err = 0;
                        ;
    break;}
case 63:
#line 1364 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 64:
#line 1365 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 65:
#line 1366 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 66:
#line 1367 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 67:
#line 1368 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 68:
#line 1369 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 69:
#line 1370 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 70:
#line 1371 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 71:
#line 1372 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 72:
#line 1373 "parser-smi.y"
{ yyval.id = smiStrdup(yyvsp[0].id); ;
    break;}
case 73:
#line 1377 "parser-smi.y"
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
			;
    break;}
case 74:
#line 1393 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			;
    break;}
case 75:
#line 1403 "parser-smi.y"
{
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
case 76:
#line 1421 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr =
				    duplicateObject(objectPtr,
						    0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-7].id);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectDecl(objectPtr,
					  SMI_DECL_VALUEASSIGNMENT);
			    setObjectLine(objectPtr, 0, thisParserPtr);
			    yyval.err = 0;
			;
    break;}
case 77:
#line 1443 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			;
    break;}
case 78:
#line 1448 "parser-smi.y"
{
			    Type *typePtr;
			    
			    if (strlen(yyvsp[-3].id)) {
				if (yyvsp[0].typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
				    smiCheckTypeName(thisParserPtr,
						     thisModulePtr, yyvsp[-3].id);
				}
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
				!strcmp(thisModulePtr->export.name, "SNMPv2-SMI")) {
				if (!strcmp(yyvsp[-3].id, "Counter32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				} else if (!strcmp(yyvsp[-3].id, "Gauge32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				} else if (!strcmp(yyvsp[-3].id, "Unsigned32")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				} else if (!strcmp(yyvsp[-3].id, "TimeTicks")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				} else if (!strcmp(yyvsp[-3].id, "Counter64")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED64;
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned64 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED64;
#ifdef _WIN32
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned64 = 4294967295U;	/* XXX */
#else
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned64 = 18446744073709551615ULL;
#endif
				    }
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned64Ptr);
				}
			    }
			    if (thisModulePtr &&
				!strcmp(thisModulePtr->export.name, "RFC1155-SMI")) {
				if (!strcmp(yyvsp[-3].id, "Counter")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				} else if (!strcmp(yyvsp[-3].id, "Gauge")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
				} else if (!strcmp(yyvsp[-3].id, "TimeTicks")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED32;
				    setTypeParent(yyvsp[0].typePtr, typeUnsigned32Ptr);
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
case 79:
#line 1524 "parser-smi.y"
{
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 80:
#line 1528 "parser-smi.y"
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
				       "RFC1155-SMI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_SMI, yyvsp[0].id);
			    }
			;
    break;}
case 89:
#line 1560 "parser-smi.y"
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
case 90:
#line 1576 "parser-smi.y"
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
			;
    break;}
case 91:
#line 1598 "parser-smi.y"
{
			    if ((yyvsp[0].typePtr) && !(yyvsp[0].typePtr->export.name)) {
				/*
				 * If the Type we found has just been
				 * defined, we don't have to allocate
				 * a new one.
				 */
				yyval.typePtr = yyvsp[0].typePtr;
			    } else {
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
					       yyval.typePtr->export.basetype, yyvsp[-7].text);
				setTypeFormat(yyval.typePtr, yyvsp[-7].text);
			    }
			    setTypeDecl(yyval.typePtr, SMI_DECL_TEXTUALCONVENTION);
			;
    break;}
case 92:
#line 1626 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			;
    break;}
case 93:
#line 1634 "parser-smi.y"
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
case 94:
#line 1653 "parser-smi.y"
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
case 95:
#line 1690 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[-1].listPtr);
			;
    break;}
case 96:
#line 1698 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 97:
#line 1705 "parser-smi.y"
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
case 98:
#line 1724 "parser-smi.y"
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
case 99:
#line 1756 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    if (yyval.typePtr)
				defaultBasetype = yyval.typePtr->export.basetype;
			;
    break;}
case 100:
#line 1764 "parser-smi.y"
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
case 101:
#line 1783 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 102:
#line 1787 "parser-smi.y"
{
			    /* TODO: */
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 103:
#line 1792 "parser-smi.y"
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
case 104:
#line 1826 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 105:
#line 1832 "parser-smi.y"
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
case 106:
#line 1845 "parser-smi.y"
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
case 107:
#line 1858 "parser-smi.y"
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
case 108:
#line 1880 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 109:
#line 1887 "parser-smi.y"
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
case 110:
#line 1908 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr,
				                            0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-12].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTIDENTITY);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 111:
#line 1933 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 112:
#line 1940 "parser-smi.y"
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
case 113:
#line 1969 "parser-smi.y"
{
			    Object *objectPtr, *parentPtr;

			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr,
				                            0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-17].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTTYPE);
			    setObjectLine(objectPtr, 0, thisParserPtr);
			    setObjectType(objectPtr, yyvsp[-12].typePtr);
			    if (!(yyvsp[-12].typePtr->export.name)) {
				/*
				 * An inlined type.
				 */
#if 0 /* export implicitly defined types by the node's lowercase name */
				setTypeName(yyvsp[-12].typePtr, yyvsp[-17].id);
#endif
			    }
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
case 114:
#line 2055 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				smiPrintError(thisParserPtr,
					      ERR_MISSING_DESCRIPTION);
			    }
			    yyval.text = NULL;
			;
    break;}
case 115:
#line 2064 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 116:
#line 2070 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 117:
#line 2077 "parser-smi.y"
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
case 118:
#line 2101 "parser-smi.y"
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
			    
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-10].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_TRAPTYPE);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 119:
#line 2149 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 120:
#line 2153 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 121:
#line 2159 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 122:
#line 2165 "parser-smi.y"
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
case 123:
#line 2178 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 124:
#line 2184 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 125:
#line 2186 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 126:
#line 2190 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        smiPrintError(thisParserPtr,
					      ERR_MAX_ACCESS_IN_SMIV1);
			    }
			;
    break;}
case 127:
#line 2198 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 128:
#line 2200 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        smiPrintError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			;
    break;}
case 129:
#line 2208 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 130:
#line 2212 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 131:
#line 2219 "parser-smi.y"
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
case 132:
#line 2239 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
				
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONTYPE);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 133:
#line 2265 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 134:
#line 2272 "parser-smi.y"
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
case 135:
#line 2297 "parser-smi.y"
{
			    setModuleLastUpdated(thisParserPtr->modulePtr, yyvsp[0].date);
			;
    break;}
case 136:
#line 2306 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    thisParserPtr->modulePtr->numModuleIdentities++;
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-17].id);
			    setObjectDecl(objectPtr, SMI_DECL_MODULEIDENTITY);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 137:
#line 2337 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 138:
#line 2341 "parser-smi.y"
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
			        smiPrintError(thisParserPtr, ERR_TYPE_TAG, yyvsp[-1].err);
			    }
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 139:
#line 2357 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 140:
#line 2362 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 141:
#line 2367 "parser-smi.y"
{
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 142:
#line 2372 "parser-smi.y"
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
case 143:
#line 2389 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 144:
#line 2391 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 145:
#line 2399 "parser-smi.y"
{ yyval.typePtr = yyvsp[0].typePtr; ;
    break;}
case 146:
#line 2404 "parser-smi.y"
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
#line 2421 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 148:
#line 2430 "parser-smi.y"
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
case 149:
#line 2442 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 150:
#line 2446 "parser-smi.y"
{
			    List *p;
			    
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 151:
#line 2464 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 152:
#line 2468 "parser-smi.y"
{
			    List *p;
			    
			    yyval.typePtr = duplicateType(typeEnumPtr, 0,
					       thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(yyval.typePtr, typeEnumPtr);
			    setTypeBasetype(yyval.typePtr, SMI_BASETYPE_ENUM);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 153:
#line 2481 "parser-smi.y"
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
case 154:
#line 2499 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 155:
#line 2503 "parser-smi.y"
{
			    Import *importPtr;
			    List *p;
			    
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_BASETYPE_NOT_IMPORTED,
					      "Integer32");
			    }

			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0, thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 156:
#line 2524 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 157:
#line 2528 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModuleAndName(
			        thisParserPtr->modulePtr, yyvsp[-2].id);
			    if (!parentPtr) {
			        importPtr = findImportByName(yyvsp[-2].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
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
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				} else {
				    /*
				     * imported type.
				     */
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					  importPtr->export.module, yyvsp[-2].id);
				    yyval.typePtr = addType(NULL,
						 parentPtr->export.basetype, 0,
						 thisParserPtr);
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				}
			    } else {
			        yyval.typePtr = duplicateType(parentPtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				setTypeParent(yyval.typePtr, parentPtr);
			    }
			    setTypeBasetype(yyval.typePtr, SMI_BASETYPE_ENUM);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 158:
#line 2577 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_ENUM;
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    smiPrintError(thisParserPtr,
						  ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				    yyval.typePtr = NULL;
				} else {
				    /*
				     * imported type.
				     */
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					                               yyvsp[-3].id, yyvsp[-1].id);
				    /* TODO: success? */
				    yyval.typePtr = addType(NULL,
						 parentPtr->export.basetype, 0,
						 thisParserPtr);
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				}
			    } else {
			        yyval.typePtr = duplicateType(parentPtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				setTypeParent(yyval.typePtr, parentPtr);
			    }
			    setTypeBasetype(yyval.typePtr, SMI_BASETYPE_ENUM);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 159:
#line 2617 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
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
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				} else {
				    /*
				     * imported type.
				     */
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
						 importPtr->export.module, yyvsp[-1].id);
				    yyval.typePtr = addType(NULL,
						 parentPtr->export.basetype, 0,
						 thisParserPtr);
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				    defaultBasetype = parentPtr->export.basetype;
				}
			    } else {
				defaultBasetype = parentPtr->export.basetype;
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				setTypeParent(yyval.typePtr, parentPtr);
			    }
			    if ((defaultBasetype == SMI_BASETYPE_INTEGER32) ||
				(defaultBasetype == SMI_BASETYPE_INTEGER64) ||
				(defaultBasetype == SMI_BASETYPE_UNSIGNED32) ||
				(defaultBasetype == SMI_BASETYPE_UNSIGNED64)) {
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    } else {
				smiPrintError(thisParserPtr,
				      ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE,
					      yyvsp[-1].id);
			    }
			;
    break;}
case 160:
#line 2678 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    defaultBasetype = SMI_BASETYPE_UNKNOWN;
				    smiPrintError(thisParserPtr,
						  ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				    yyval.typePtr = NULL;
				} else {
				    /*
				     * imported type.
				     */
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
					                               yyvsp[-3].id, yyvsp[-1].id);
				    /* TODO: success? */
				    yyval.typePtr = addType(NULL,
						 parentPtr->export.basetype, 0,
						 thisParserPtr);
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				    defaultBasetype = parentPtr->export.basetype;
				}
			    } else {
				defaultBasetype = parentPtr->export.basetype;
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				setTypeParent(yyval.typePtr, parentPtr);
			    }
			    if ((defaultBasetype == SMI_BASETYPE_INTEGER32) ||
				(defaultBasetype == SMI_BASETYPE_INTEGER64) ||
				(defaultBasetype == SMI_BASETYPE_UNSIGNED32) ||
				(defaultBasetype == SMI_BASETYPE_UNSIGNED64)) {
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    } else {
				smiPrintError(thisParserPtr,
				      ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE,
					      yyvsp[-1].id);
			    }
			;
    break;}
case 161:
#line 2729 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 162:
#line 2734 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			;
    break;}
case 163:
#line 2738 "parser-smi.y"
{
			    List *p;
			    
			    yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(yyval.typePtr, typeOctetStringPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 164:
#line 2750 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
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
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				} else {
				    /*
				     * imported type.
				     */
				    importPtr->use++;
				    parentPtr = findTypeByModulenameAndName(
						 importPtr->export.module, yyvsp[-1].id);
				    yyval.typePtr = addType(NULL,
						 parentPtr->export.basetype, 0,
						 thisParserPtr);
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				}
			    } else {
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				setTypeParent(yyval.typePtr, parentPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 165:
#line 2800 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    smiPrintError(thisParserPtr,
						  ERR_UNKNOWN_TYPE, yyvsp[-1].id);
				    yyval.typePtr = NULL;
				} else {
				    /*
				     * imported type.
				     */
				    importPtr->use++;
				    parentPtr =
					findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
				    /* TODO: success? */
				    yyval.typePtr = addType(NULL,
						 parentPtr->export.basetype, 0,
						 thisParserPtr);
				    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				    setTypeParent(yyval.typePtr, parentPtr);
				}
			    } else {
			        yyval.typePtr = duplicateType(parentPtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
				setTypeParent(yyval.typePtr, parentPtr);
			    }
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 166:
#line 2839 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 167:
#line 2847 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;
    break;}
case 168:
#line 2853 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;
    break;}
case 169:
#line 2859 "parser-smi.y"
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
				    yyval.valuePtr->value.ptr[i/8] = strtol(s, 0, 2);
				}
				yyval.valuePtr->len = (len+7)/8;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 2);
			    }
			;
    break;}
case 170:
#line 2884 "parser-smi.y"
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
				    yyval.valuePtr->value.ptr[i/2] = strtol(s, 0, 16);
				}
				yyval.valuePtr->len = (len+1)/2;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 16);
			    }
			;
    break;}
case 171:
#line 2906 "parser-smi.y"
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
case 172:
#line 2926 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.valuePtr->value.ptr = smiStrdup(yyvsp[0].text);
			    yyval.valuePtr->len = strlen(yyvsp[0].text);
			;
    break;}
case 173:
#line 2949 "parser-smi.y"
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
case 174:
#line 2975 "parser-smi.y"
{
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 175:
#line 2979 "parser-smi.y"
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
case 176:
#line 2997 "parser-smi.y"
{
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 177:
#line 3001 "parser-smi.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 178:
#line 3007 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			;
    break;}
case 179:
#line 3015 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			;
    break;}
case 180:
#line 3023 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			;
    break;}
case 181:
#line 3031 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByName("Gauge32");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			    yyval.typePtr = duplicateType(parentPtr, 0, thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(yyval.typePtr, parentPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			    importPtr = findImportByName("Gauge32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    }
			;
    break;}
case 182:
#line 3054 "parser-smi.y"
{
			    Import *importPtr;

			    yyval.typePtr = findTypeByName("Unsigned32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Unsigned32");
			    }
			    
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
case 183:
#line 3074 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByName("Unsigned32");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Unsigned32");
			    }
			    yyval.typePtr = duplicateType(parentPtr, 0, thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(yyval.typePtr, parentPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
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
case 184:
#line 3101 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			;
    break;}
case 185:
#line 3109 "parser-smi.y"
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
case 186:
#line 3120 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByName("Opaque");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    }
			    smiPrintError(thisParserPtr, ERR_OPAQUE_OBSOLETE);
			    yyval.typePtr = duplicateType(parentPtr, 0, thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(yyval.typePtr, parentPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			    importPtr = findImportByName("Opaque",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    }
			;
    break;}
case 187:
#line 3144 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter64");
			    }
			;
    break;}
case 188:
#line 3158 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			;
    break;}
case 189:
#line 3166 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			;
    break;}
case 190:
#line 3174 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			;
    break;}
case 191:
#line 3182 "parser-smi.y"
{
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("Unsigned32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Unsigned32");
			    }

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
case 192:
#line 3202 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			;
    break;}
case 193:
#line 3210 "parser-smi.y"
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
case 194:
#line 3221 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter64");
			    }
			;
    break;}
case 195:
#line 3231 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 196:
#line 3238 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 197:
#line 3245 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				smiPrintError(thisParserPtr,
					      ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 198:
#line 3252 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 199:
#line 3266 "parser-smi.y"
{ yyval.listPtr = yyvsp[-1].listPtr; ;
    break;}
case 200:
#line 3276 "parser-smi.y"
{ yyval.listPtr = yyvsp[-2].listPtr; ;
    break;}
case 201:
#line 3280 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 202:
#line 3286 "parser-smi.y"
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
case 203:
#line 3299 "parser-smi.y"
{
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[0].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[0].valuePtr);
			;
    break;}
case 204:
#line 3306 "parser-smi.y"
{
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[-2].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[-2].valuePtr);
			    smiFree(yyvsp[0].valuePtr);
			;
    break;}
case 205:
#line 3316 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;
    break;}
case 206:
#line 3322 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;
    break;}
case 207:
#line 3328 "parser-smi.y"
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
				    yyval.valuePtr->value.ptr[i/2] = strtol(s, 0, 16);
				}
				yyval.valuePtr->len = (len+1)/2;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 16);
			    }
			;
    break;}
case 208:
#line 3350 "parser-smi.y"
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
				    yyval.valuePtr->value.ptr[i/8] = strtol(s, 0, 2);
				}
				yyval.valuePtr->len = (len+7)/8;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 2);
			    }
			;
    break;}
case 209:
#line 3376 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 210:
#line 3382 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 211:
#line 3388 "parser-smi.y"
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
case 212:
#line 3401 "parser-smi.y"
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
case 213:
#line 3414 "parser-smi.y"
{
			    yyval.namedNumberPtr = smiMalloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id;
			    yyval.namedNumberPtr->export.value = *yyvsp[-1].valuePtr;
			    smiFree(yyvsp[-1].valuePtr);
			;
    break;}
case 214:
#line 3423 "parser-smi.y"
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
case 215:
#line 3433 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    /* TODO: non-negative is suggested */
			;
    break;}
case 216:
#line 3442 "parser-smi.y"
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
case 217:
#line 3478 "parser-smi.y"
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
case 218:
#line 3494 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 219:
#line 3498 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 220:
#line 3504 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 221:
#line 3508 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 222:
#line 3514 "parser-smi.y"
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
case 223:
#line 3563 "parser-smi.y"
{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			;
    break;}
case 224:
#line 3571 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = yyvsp[-1].listPtr;
			    yyval.index.rowPtr    = NULL;
			;
    break;}
case 225:
#line 3580 "parser-smi.y"
{
			    yyval.index.indexkind    = SMI_INDEX_AUGMENT;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
			;
    break;}
case 226:
#line 3587 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;
    break;}
case 227:
#line 3593 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 228:
#line 3600 "parser-smi.y"
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
case 229:
#line 3613 "parser-smi.y"
{
			    impliedFlag = 1;
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 230:
#line 3618 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 231:
#line 3628 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 232:
#line 3634 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 233:
#line 3640 "parser-smi.y"
{ yyval.valuePtr = yyvsp[-1].valuePtr; ;
    break;}
case 234:
#line 3642 "parser-smi.y"
{ yyval.valuePtr = NULL; ;
    break;}
case 235:
#line 3647 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 236:
#line 3649 "parser-smi.y"
{
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
			    yyval.valuePtr->value.ptr = (void *)yyvsp[-1].listPtr;
			;
    break;}
case 237:
#line 3657 "parser-smi.y"
{ yyval.listPtr = yyvsp[0].listPtr; ;
    break;}
case 238:
#line 3659 "parser-smi.y"
{ yyval.listPtr = NULL; ;
    break;}
case 239:
#line 3663 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].id;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 240:
#line 3669 "parser-smi.y"
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
case 241:
#line 3682 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 242:
#line 3688 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 243:
#line 3694 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 244:
#line 3696 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 245:
#line 3700 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 246:
#line 3702 "parser-smi.y"
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
case 247:
#line 3714 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 248:
#line 3716 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 249:
#line 3721 "parser-smi.y"
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
case 250:
#line 3743 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 251:
#line 3747 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 252:
#line 3753 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 253:
#line 3759 "parser-smi.y"
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
case 254:
#line 3772 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 255:
#line 3778 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 256:
#line 3784 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 257:
#line 3790 "parser-smi.y"
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
#line 3803 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 259:
#line 3809 "parser-smi.y"
{
			    yyval.text = smiStrdup(yyvsp[0].text);
			;
    break;}
case 260:
#line 3815 "parser-smi.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 261:
#line 3820 "parser-smi.y"
{
			    parentNodePtr = rootNodePtr;
			;
    break;}
case 262:
#line 3824 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    parentNodePtr = yyvsp[0].objectPtr->nodePtr;
			;
    break;}
case 263:
#line 3832 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 264:
#line 3837 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 265:
#line 3844 "parser-smi.y"
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
case 266:
#line 3932 "parser-smi.y"
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
case 267:
#line 4027 "parser-smi.y"
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
case 268:
#line 4051 "parser-smi.y"
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
case 269:
#line 4080 "parser-smi.y"
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
case 270:
#line 4112 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 271:
#line 4116 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 272:
#line 4118 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 273:
#line 4122 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 274:
#line 4124 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 275:
#line 4128 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 276:
#line 4135 "parser-smi.y"
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
			;
    break;}
case 277:
#line 4153 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTGROUP);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 278:
#line 4181 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 279:
#line 4188 "parser-smi.y"
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
			;
    break;}
case 280:
#line 4207 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONGROUP);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 281:
#line 4236 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 282:
#line 4243 "parser-smi.y"
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
			;
    break;}
case 283:
#line 4262 "parser-smi.y"
{
			    Object *objectPtr;
			    Option *optionPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 284:
#line 4323 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 285:
#line 4329 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 286:
#line 4333 "parser-smi.y"
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
case 287:
#line 4368 "parser-smi.y"
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
case 288:
#line 4384 "parser-smi.y"
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
case 289:
#line 4397 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 290:
#line 4405 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 291:
#line 4413 "parser-smi.y"
{
			    yyval.modulePtr = thisModulePtr;
			;
    break;}
case 292:
#line 4419 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 293:
#line 4423 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 294:
#line 4429 "parser-smi.y"
{
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 295:
#line 4435 "parser-smi.y"
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
case 296:
#line 4448 "parser-smi.y"
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
case 297:
#line 4471 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			;
    break;}
case 298:
#line 4477 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 299:
#line 4485 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 300:
#line 4489 "parser-smi.y"
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
case 301:
#line 4556 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].listPtr;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 302:
#line 4562 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 303:
#line 4571 "parser-smi.y"
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
case 304:
#line 4598 "parser-smi.y"
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
case 305:
#line 4625 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 306:
#line 4634 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 307:
#line 4640 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 308:
#line 4649 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 309:
#line 4655 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 310:
#line 4661 "parser-smi.y"
{
			    yyval.access = yyvsp[0].access;
			;
    break;}
case 311:
#line 4665 "parser-smi.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 312:
#line 4671 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;
    break;}
case 313:
#line 4678 "parser-smi.y"
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
			;
    break;}
case 314:
#line 4698 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    setObjectName(objectPtr, yyvsp[-15].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    setObjectLine(objectPtr, 0, thisParserPtr);
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
case 315:
#line 4730 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 316:
#line 4732 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 317:
#line 4736 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 318:
#line 4738 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 319:
#line 4742 "parser-smi.y"
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
case 320:
#line 4758 "parser-smi.y"
{
			    if (capabilitiesModulePtr) {
				checkImports(capabilitiesModulePtr,
					     thisParserPtr);
				capabilitiesModulePtr = NULL;
			    }
			    yyval.err = 0;
			;
    break;}
case 321:
#line 4769 "parser-smi.y"
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
case 322:
#line 4779 "parser-smi.y"
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
case 323:
#line 4796 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 324:
#line 4802 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 325:
#line 4810 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 326:
#line 4819 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 327:
#line 4821 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 328:
#line 4825 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 329:
#line 4827 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 330:
#line 4831 "parser-smi.y"
{
			    if (yyvsp[0].objectPtr) {
				variationkind = yyvsp[0].objectPtr->export.nodekind;
			    } else {
				variationkind = SMI_NODEKIND_UNKNOWN;
			    }
			;
    break;}
case 331:
#line 4839 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_SYNTAX);
			    }
			;
    break;}
case 332:
#line 4846 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_WRITESYNTAX);
			    }
			;
    break;}
case 333:
#line 4854 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_CREATION);
			    }
			;
    break;}
case 334:
#line 4861 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_DEFVAL);
			    }
			;
    break;}
case 335:
#line 4868 "parser-smi.y"
{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			    variationkind = SMI_NODEKIND_UNKNOWN;
			;
    break;}
case 336:
#line 4876 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 337:
#line 4878 "parser-smi.y"
{ yyval.access = 0; ;
    break;}
case 338:
#line 4882 "parser-smi.y"
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
case 339:
#line 4947 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 340:
#line 4949 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 341:
#line 4953 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 342:
#line 4955 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 343:
#line 4959 "parser-smi.y"
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
#line 4962 "parser-smi.y"


#endif
