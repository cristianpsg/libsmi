/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse smiparse
#define yylex   smilex
#define yyerror smierror
#define yylval  smilval
#define yychar  smichar
#define yydebug smidebug
#define yynerrs sminerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DOT_DOT = 258,
     COLON_COLON_EQUAL = 259,
     UPPERCASE_IDENTIFIER = 260,
     LOWERCASE_IDENTIFIER = 261,
     NUMBER = 262,
     NEGATIVENUMBER = 263,
     NUMBER64 = 264,
     NEGATIVENUMBER64 = 265,
     BIN_STRING = 266,
     HEX_STRING = 267,
     QUOTED_STRING = 268,
     ACCESS = 269,
     AGENT_CAPABILITIES = 270,
     APPLICATION = 271,
     AUGMENTS = 272,
     BEGIN_ = 273,
     BITS = 274,
     CHOICE = 275,
     CONTACT_INFO = 276,
     CREATION_REQUIRES = 277,
     COUNTER32 = 278,
     COUNTER64 = 279,
     DEFINITIONS = 280,
     DEFVAL = 281,
     DESCRIPTION = 282,
     DISPLAY_HINT = 283,
     END = 284,
     ENTERPRISE = 285,
     EXPORTS = 286,
     EXTENDS = 287,
     FROM = 288,
     GROUP = 289,
     GAUGE32 = 290,
     IDENTIFIER = 291,
     IMPLICIT = 292,
     IMPLIED = 293,
     IMPORTS = 294,
     INCLUDES = 295,
     INDEX = 296,
     INSTALL_ERRORS = 297,
     INTEGER = 298,
     INTEGER32 = 299,
     INTEGER64 = 300,
     IPADDRESS = 301,
     LAST_UPDATED = 302,
     MACRO = 303,
     MANDATORY_GROUPS = 304,
     MAX_ACCESS = 305,
     MIN_ACCESS = 306,
     MODULE = 307,
     MODULE_COMPLIANCE = 308,
     MODULE_IDENTITY = 309,
     NOT_ACCESSIBLE = 310,
     NOTIFICATIONS = 311,
     NOTIFICATION_GROUP = 312,
     NOTIFICATION_TYPE = 313,
     OBJECT = 314,
     OBJECT_GROUP = 315,
     OBJECT_IDENTITY = 316,
     OBJECT_TYPE = 317,
     OBJECTS = 318,
     OCTET = 319,
     OF = 320,
     ORGANIZATION = 321,
     OPAQUE = 322,
     PIB_ACCESS = 323,
     PIB_DEFINITIONS = 324,
     PIB_INDEX = 325,
     PIB_MIN_ACCESS = 326,
     PIB_REFERENCES = 327,
     PIB_TAG = 328,
     POLICY_ACCESS = 329,
     PRODUCT_RELEASE = 330,
     REFERENCE = 331,
     REVISION = 332,
     SEQUENCE = 333,
     SIZE = 334,
     STATUS = 335,
     STRING = 336,
     SUBJECT_CATEGORIES = 337,
     SUPPORTS = 338,
     SYNTAX = 339,
     TEXTUAL_CONVENTION = 340,
     TIMETICKS = 341,
     TRAP_TYPE = 342,
     UNIQUENESS = 343,
     UNITS = 344,
     UNIVERSAL = 345,
     UNSIGNED32 = 346,
     UNSIGNED64 = 347,
     VALUE = 348,
     VARIABLES = 349,
     VARIATION = 350,
     WRITE_SYNTAX = 351
   };
#endif
#define DOT_DOT 258
#define COLON_COLON_EQUAL 259
#define UPPERCASE_IDENTIFIER 260
#define LOWERCASE_IDENTIFIER 261
#define NUMBER 262
#define NEGATIVENUMBER 263
#define NUMBER64 264
#define NEGATIVENUMBER64 265
#define BIN_STRING 266
#define HEX_STRING 267
#define QUOTED_STRING 268
#define ACCESS 269
#define AGENT_CAPABILITIES 270
#define APPLICATION 271
#define AUGMENTS 272
#define BEGIN_ 273
#define BITS 274
#define CHOICE 275
#define CONTACT_INFO 276
#define CREATION_REQUIRES 277
#define COUNTER32 278
#define COUNTER64 279
#define DEFINITIONS 280
#define DEFVAL 281
#define DESCRIPTION 282
#define DISPLAY_HINT 283
#define END 284
#define ENTERPRISE 285
#define EXPORTS 286
#define EXTENDS 287
#define FROM 288
#define GROUP 289
#define GAUGE32 290
#define IDENTIFIER 291
#define IMPLICIT 292
#define IMPLIED 293
#define IMPORTS 294
#define INCLUDES 295
#define INDEX 296
#define INSTALL_ERRORS 297
#define INTEGER 298
#define INTEGER32 299
#define INTEGER64 300
#define IPADDRESS 301
#define LAST_UPDATED 302
#define MACRO 303
#define MANDATORY_GROUPS 304
#define MAX_ACCESS 305
#define MIN_ACCESS 306
#define MODULE 307
#define MODULE_COMPLIANCE 308
#define MODULE_IDENTITY 309
#define NOT_ACCESSIBLE 310
#define NOTIFICATIONS 311
#define NOTIFICATION_GROUP 312
#define NOTIFICATION_TYPE 313
#define OBJECT 314
#define OBJECT_GROUP 315
#define OBJECT_IDENTITY 316
#define OBJECT_TYPE 317
#define OBJECTS 318
#define OCTET 319
#define OF 320
#define ORGANIZATION 321
#define OPAQUE 322
#define PIB_ACCESS 323
#define PIB_DEFINITIONS 324
#define PIB_INDEX 325
#define PIB_MIN_ACCESS 326
#define PIB_REFERENCES 327
#define PIB_TAG 328
#define POLICY_ACCESS 329
#define PRODUCT_RELEASE 330
#define REFERENCE 331
#define REVISION 332
#define SEQUENCE 333
#define SIZE 334
#define STATUS 335
#define STRING 336
#define SUBJECT_CATEGORIES 337
#define SUPPORTS 338
#define SYNTAX 339
#define TEXTUAL_CONVENTION 340
#define TIMETICKS 341
#define TRAP_TYPE 342
#define UNIQUENESS 343
#define UNITS 344
#define UNIVERSAL 345
#define UNSIGNED32 346
#define UNSIGNED64 347
#define VALUE 348
#define VARIABLES 349
#define VARIATION 350
#define WRITE_SYNTAX 351




/* Copy the first part of user declarations.  */
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



static int	   impliedFlag;
static SmiNodekind variationkind;
static SmiBasetype defaultBasetype;
static int	   indexFlag;

/*
 * Values for the indexFlag variable
 */
#define INDEXFLAG_NONE     0
#define INDEXFLAG_PIBINDEX 1
#define INDEXFLAG_AUGMENTS 2
#define INDEXFLAG_EXTENDS  3
 
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
checkDescr(Parser *parser, char *descr)
{
    if (descr) {
	if (descr[0] == 0) {
	    smiPrintError(parser, ERR_EMPTY_DESCRIPTION);
	}
	/* we might want to add more checks since I have recently
	   seen things like DESCRIPTION "." to cirumvent warnings */
    }
}
 

 
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
checkModuleName(Parser *parserPtr, Module *modulePtr)
{
     static char *mib_ignore[] = {
	 "SNMPv2-SMI", "SNMPv2-TC", "SNMPv2-CONF", NULL
     };
     
     static char *pib_ignore[] = {
	 "COPS-PR-SPPI", "COPS-PR-SPPI-TC",
	 "SNMPv2-SMI", "SNMPv2-TC", "SNMPv2-CONF", NULL
     };

     const char *name = thisModulePtr->export.name;
     const int len = strlen(name);
     int i;

     switch (modulePtr->export.language) {
     case SMI_LANGUAGE_SMIV1:
     case SMI_LANGUAGE_SMIV2:
     case SMI_LANGUAGE_SMING:
	 for (i = 0; mib_ignore[i]; i++) {
	     if (strcmp(mib_ignore[i], name) == 0) {
		 return;
	     }
	 }
	 if (len > 3 && (strcmp(name + len - 4, "-MIB") != 0)) {
	     smiPrintError(parserPtr, ERR_MIB_MODULENAME_SUFFIX, name);
	     return;
	 }
	 break;
     case SMI_LANGUAGE_SPPI:
	 for (i = 0; pib_ignore[i]; i++) {
	     if (strcmp(pib_ignore[i], name) == 0) {
		 return;
	     }
	 }
	 if (len > 3 && (strcmp(name + len - 4, "-PIB") != 0)) {
	     smiPrintError(parserPtr, ERR_PIB_MODULENAME_SUFFIX, name);
	 }
	 break;
     case SMI_LANGUAGE_UNKNOWN:
	 break;
     }
}



static void
checkModuleIdentity(Parser *parserPtr, Module *modulePtr)
{
    if ((modulePtr->export.language == SMI_LANGUAGE_SMIV2)
	&& (modulePtr->numModuleIdentities < 1)
	&& strcmp(modulePtr->export.name, "SNMPv2-SMI")
	&& strcmp(modulePtr->export.name, "SNMPv2-CONF")
	&& strcmp(modulePtr->export.name, "SNMPv2-TC")
        && strcmp(modulePtr->export.name, "COPS-PR-SPPI")) {
	smiPrintError(parserPtr, ERR_NO_MODULE_IDENTITY);
    }
}



static void
checkObjects(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr;
    Node *nodePtr;
    int i;
    Type *counterTypePtr, *counter32TypePtr, *counter64TypePtr;
    
    counterTypePtr = findTypeByName("Counter");
    counter32TypePtr = findTypeByModulenameAndName("SNMPv2-SMI", "Counter32");
    counter64TypePtr = findTypeByModulenameAndName("SNMPv2-SMI", "Counter64");
    
    for (objectPtr = modulePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextPtr) {

	Object *parentPtr;
	
	if ((objectPtr->export.decl != SMI_DECL_UNKNOWN) &&
	    objectPtr->nodePtr->parentPtr &&
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

	Object *parentPtr;
	
	if (objectPtr->nodePtr->parentPtr &&
	    objectPtr->nodePtr->parentPtr->lastObjectPtr) {
	    parentPtr = objectPtr->nodePtr->parentPtr->lastObjectPtr;
	} else {
	    parentPtr = NULL;
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
	 * Mark types that are referenced in this module.
	 */

	if (objectPtr->typePtr
	    && (objectPtr->export.nodekind == SMI_NODEKIND_COLUMN
		|| objectPtr->export.nodekind == SMI_NODEKIND_SCALAR)
	    && (objectPtr->typePtr->export.decl == SMI_DECL_TYPEDEF
		|| objectPtr->typePtr->export.decl == SMI_DECL_TEXTUALCONVENTION
		|| objectPtr->typePtr->export.decl == SMI_DECL_IMPLICIT_TYPE)) {
	    addTypeFlags(objectPtr->typePtr, FLAG_INSYNTAX);
	    if (objectPtr->typePtr->export.decl == SMI_DECL_IMPLICIT_TYPE) {
		addTypeFlags(objectPtr->typePtr->parentPtr, FLAG_INSYNTAX);
	    }
	}
	
	/*
	 * Check whether the status of the associated type matches the
	 * status of the object.
	 */

	if (objectPtr->typePtr
	    && (objectPtr->export.nodekind == SMI_NODEKIND_COLUMN
		|| objectPtr->export.nodekind == SMI_NODEKIND_SCALAR)
	    && (objectPtr->export.status < objectPtr->typePtr->export.status)) {
	    if (objectPtr->typePtr->export.status == SMI_STATUS_DEPRECATED) {
		smiPrintErrorAtLine(parserPtr, ERR_TYPE_STATUS_DEPRECATED,
				    objectPtr->line,
				    objectPtr->typePtr->export.name,
				    objectPtr->export.name);
	    }
	    if (objectPtr->typePtr->export.status == SMI_STATUS_OBSOLETE) {
		smiPrintErrorAtLine(parserPtr, ERR_TYPE_STATUS_OBSOLETE,
				    objectPtr->line,
				    objectPtr->typePtr->export.name,
				    objectPtr->export.name);
	    }
	}

	/*
	 * Check the nodekind of the parent node.
	 */

	if (parentPtr) {
	    switch (objectPtr->export.nodekind) {
	    case SMI_NODEKIND_COLUMN:
		if (parentPtr->export.nodekind != SMI_NODEKIND_ROW) {
		    smiPrintErrorAtLine(parserPtr, ERR_COLUMN_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_ROW:
		if (parentPtr->export.nodekind != SMI_NODEKIND_TABLE) {
		    smiPrintErrorAtLine(parserPtr, ERR_ROW_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		if (parentPtr->typePtr && parentPtr->typePtr->parentPtr &&
		    strcmp(parentPtr->typePtr->parentPtr->export.name,
			   objectPtr->typePtr->export.name)) {
		    smiPrintErrorAtLine(parserPtr, ERR_TABLE_ROW_TYPE_MISMATCH,
					objectPtr->line,
					objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_TABLE:
		if ((parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_NODE)) {
		    smiPrintErrorAtLine(parserPtr, ERR_TABLE_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_SCALAR:
		if ((parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_NODE)) {
		    smiPrintErrorAtLine(parserPtr, ERR_SCALAR_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_NOTIFICATION:
		if ((parentPtr->export.nodekind != SMI_NODEKIND_NODE) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN)) {
		    smiPrintErrorAtLine(parserPtr, ERR_NOTIFICATION_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		if (parserPtr && parentPtr->nodePtr->parentPtr &&
		    parentPtr->nodePtr->parentPtr->lastObjectPtr) {
		    Object *parent2Ptr = parentPtr->nodePtr->parentPtr->lastObjectPtr;
		    if ((parent2Ptr->export.nodekind != SMI_NODEKIND_NODE) &&
			(parent2Ptr->export.nodekind != SMI_NODEKIND_UNKNOWN)) {
			smiPrintErrorAtLine(parserPtr,
					    ERR_NOTIFICATION_PARENT_TYPE,
					    objectPtr->line,
					    objectPtr->export.name);
		    }
		}
		break;
	    case SMI_NODEKIND_NODE:
		/* Node defined by OBJECT IDENTIFIER assignments can have
		   arbitrary parent node. */
		if ((parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_NODE) &&
		    (objectPtr->export.decl != SMI_DECL_VALUEASSIGNMENT)) {
		    smiPrintErrorAtLine(parserPtr, ERR_NODE_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_GROUP:
		if ((parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_NODE)) {
		    smiPrintErrorAtLine(parserPtr, ERR_GROUP_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_COMPLIANCE:
		if ((parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_NODE)) {
		    smiPrintErrorAtLine(parserPtr, ERR_COMPLIANCE_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
	    case SMI_NODEKIND_CAPABILITIES:
		if ((parentPtr->export.nodekind != SMI_NODEKIND_UNKNOWN) &&
		    (parentPtr->export.nodekind != SMI_NODEKIND_NODE)) {
		    smiPrintErrorAtLine(parserPtr, ERR_CAPABILITIES_PARENT_TYPE,
					objectPtr->line, objectPtr->export.name);
		}
		break;
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
	 * Check whether notification statements contain useful
	 * objects.
	 */

	if (objectPtr->export.nodekind == SMI_NODEKIND_NOTIFICATION) {
	    smiCheckNotificationOid(parserPtr, modulePtr, objectPtr);
	    smiCheckNotificationMembers(parserPtr, objectPtr);
	}

        if (modulePtr->export.language != SMI_LANGUAGE_SPPI) {
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
	     * Check whether counter objects are read-only or
	     * accessible-for-notify (RFC 2578, 7.1.6).
	     */
	    if (((objectPtr->export.nodekind == SMI_NODEKIND_SCALAR) ||
		 (objectPtr->export.nodekind == SMI_NODEKIND_COLUMN)) &&
	        (objectPtr->export.access != SMI_ACCESS_NOTIFY) &&
		(objectPtr->export.access != SMI_ACCESS_READ_ONLY) &&
		(smiTypeDerivedFrom(objectPtr->typePtr, counterTypePtr) ||
		 smiTypeDerivedFrom(objectPtr->typePtr, counter32TypePtr) ||
		 smiTypeDerivedFrom(objectPtr->typePtr, counter64TypePtr))) {
	        smiPrintErrorAtLine(parserPtr, ERR_COUNTER_ACCESS,
				    objectPtr->line, objectPtr->export.name);
	    }
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

		/*
		 * This misreports some cases where the table name
		 * does not have the "*Table" suffix.  This is trying
		 * to allow Entry names of either fooTableEntry or
		 * fooEntry.
		 */
		if (parentPtr &&
		    !(((int)strlen(parentPtr->export.name) == len ||
		       (int)strlen(parentPtr->export.name) == len - 5) &&
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
		    if (colPtr->export.name) {
			/*
			 * Don't complain, if it's an implcitly defined
			 * unnamed node (could happen for parent node of
			 * TRAP-TYPE definitions).
			 */
			smiPrintErrorAtLine(parserPtr,
					    ERR_SEQUENCE_MISSING_COLUMN,
					    objectPtr->typePtr->line,
					    objectPtr->typePtr->export.name,
					    colPtr->export.name);
		    }
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

	if ((objectPtr->flags & FLAG_INCOMPLETE) &&
	    (objectPtr->export.decl != SMI_DECL_IMPL_OBJECT)) {
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
		     nodePtr->parentPtr != thisParserPtr->pendingNodePtr &&
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

	if ((objectPtr->export.decl != SMI_DECL_UNKNOWN)
	    && (objectPtr->export.nodekind != SMI_NODEKIND_NODE)
	    && objectPtr->export.name
	    && objectPtr->export.oid[objectPtr->export.oidlen-1] == 0
	    && objectPtr->export.oidlen != 2 && objectPtr->export.oid[0] != 0) {
	    smiPrintErrorAtLine(parserPtr, ERR_OID_ADMIN_ZERO,
				objectPtr->line,
				objectPtr->export.name);
	}

	/*
	 * Check whether the module identity is registered in a well
	 * known controlled location. 
	 */

	if (objectPtr->export.decl == SMI_DECL_MODULEIDENTITY) {
	    smiCheckModuleIdentityRegistration(parserPtr, objectPtr);
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
            case SMI_INDEX_SPARSE:
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

    if (modulePtr->export.language == SMI_LANGUAGE_SPPI) {
        Object *parentPtr;
        
        for (objectPtr = modulePtr->firstObjectPtr;
             objectPtr; objectPtr = objectPtr->nextPtr) {
            /*
             * All checks for SPPI constructs
             */
            if (objectPtr->nodePtr->parentPtr)
                parentPtr = objectPtr->nodePtr->parentPtr->lastObjectPtr;
            else
                parentPtr = NULL;
            
            /*
             * Do all rows contain a PIB-INDEX/AUGMENTS/EXTENDS ?
             * See RFC 3159 7.5, 7.7, 7.8
             */
            if (parentPtr  &&
                (parentPtr->export.nodekind == SMI_NODEKIND_TABLE) &&
                (objectPtr->export.indexkind != SMI_INDEX_INDEX) &&
                (objectPtr->export.indexkind != SMI_INDEX_AUGMENT) &&
                (objectPtr->export.indexkind != SMI_INDEX_SPARSE))
                smiPrintErrorAtLine(parserPtr, ERR_ROW_LACKS_PIB_INDEX,
                                    objectPtr->line);

            /*
             * Does any non row contain a PIB-INDEX/AUGMENTS/EXTENDS ?
             * See RFC 3159 7.5, 7.7, 7.8
             */
            if ((objectPtr->export.nodekind != SMI_NODEKIND_ROW) &&
                (objectPtr->export.indexkind != SMI_INDEX_UNKNOWN))
                smiPrintErrorAtLine(parserPtr, ERR_PIB_INDEX_FOR_NON_ROW_TYPE,
                                    objectPtr->line);

            /*
             * Check the PIB-INDEX and other indices
             */
            if ((objectPtr->export.nodekind == SMI_NODEKIND_ROW) &&
                (objectPtr->export.indexkind == SMI_INDEX_INDEX)) {

                /*
                 * Only the first element (PIB-INDEX) has to be an InstanceId.
                 * See RFC 3159 7.5
                 */
                if (objectPtr->listPtr && objectPtr->listPtr->ptr) {
                    Object *pibindex = (Object *)objectPtr->listPtr->ptr;
                    if (pibindex->typePtr && pibindex->typePtr->export.name &&
                        strcmp(pibindex->typePtr->export.name, "InstanceId"))
                        smiPrintErrorAtLine(thisParserPtr, ERR_PIB_INDEX_NOT_INSTANCEID,
                                            pibindex->line, pibindex->export.name);
                }
            }
            
            /*
             * Do all tables contain a PIB-ACCESS clause?
             * See RFC 3159 7.3
             */
            if ((objectPtr->export.nodekind == SMI_NODEKIND_TABLE) &&
                (objectPtr->export.access == SMI_ACCESS_UNKNOWN))
                smiPrintErrorAtLine(parserPtr, ERR_TABLE_LACKS_PIB_ACCESS,
                                    objectPtr->line);

            /*
             * Does any non table types contain a PIB-ACCESS clause?
             * See RFC 3159 7.3
             */
            if (((objectPtr->export.nodekind == SMI_NODEKIND_NODE) ||
                 (objectPtr->export.nodekind == SMI_NODEKIND_ROW) ||
                 (objectPtr->export.nodekind == SMI_NODEKIND_SCALAR)) &&
                (objectPtr->export.access != SMI_ACCESS_UNKNOWN))
                smiPrintErrorAtLine(parserPtr, ERR_PIB_ACCESS_FOR_NON_TABLE,
                                    objectPtr->line);

            /*
             * Check the UNIQUENESS clause and its entries
             * See RFC 3159 7.9
             */
            if (objectPtr->uniquenessPtr) {
                if (objectPtr->export.nodekind != SMI_NODEKIND_ROW)
                    smiPrintErrorAtLine(parserPtr, ERR_UNIQUENESS_FOR_NON_ROW,
                                        objectPtr->line);
                else
                    smiCheckUniqueness(parserPtr, objectPtr);
            }
            
            /*
             * Does the PIB-REFERENCES object point to a PRC (table)?
             * See RFC 3159 7.10
             */
            if (objectPtr->typePtr && objectPtr->typePtr->export.name &&
                !strcmp(objectPtr->typePtr->export.name, "ReferenceId") &&
                objectPtr->relatedPtr &&
                (objectPtr->relatedPtr->export.nodekind != SMI_NODEKIND_ROW))
                smiPrintErrorAtLine(parserPtr, ERR_PIB_REFERENCES_NOT_ROW,
                                    objectPtr->line);

            /*
             * Do all PIB-TAGs point to objects with a SYNTAX of TagId?
             * See RFC 3159 7.12
             */
            if (objectPtr->typePtr && objectPtr->typePtr->export.name &&
                !strcmp(objectPtr->typePtr->export.name, "TagReferenceId") &&
                objectPtr->relatedPtr && objectPtr->relatedPtr->typePtr &&
                objectPtr->relatedPtr->typePtr->export.name &&
                strcmp(objectPtr->relatedPtr->typePtr->export.name, "TagId"))
                smiPrintErrorAtLine(parserPtr, ERR_PIB_TAG_TYPE, objectPtr->line);
            
            /*
             * Is the attribute member of at least one compliance group?
             * See RFC 3159 9.1
             */
            if (objectPtr->export.nodekind & SMI_NODEKIND_COLUMN) {
                Object *group;
                int found = 0;
                
                for (group = modulePtr->firstObjectPtr; group;
                     group = group->nextPtr) {
                    if ((group->export.nodekind == SMI_NODEKIND_GROUP) &&
                        group->listPtr) {
                        List *l;
                        
                        for (l = group->listPtr; l; l = l->nextPtr)
                            if (((Object *)l->ptr)->export.name &&
                                !strcmp(((Object *)l->ptr)->export.name,
                                        objectPtr->export.name)) {
                                found = 1;
                                break;
                            }
                    }
                    if (found)
                        break;
                }
                if (!found)
                    smiPrintErrorAtLine(parserPtr, ERR_ATTRIBUTE_NOT_IN_GROUP,
                                        objectPtr->line, objectPtr->export.name);
            }
        }
    }
}



static void
checkTypes(Parser *parserPtr, Module *modulePtr)
{
    Type *typePtr;
    
    for (typePtr = modulePtr->firstTypePtr;
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
	 * Check whether we have types that are not used in this
	 * module.
	 */

	if ((typePtr->export.decl == SMI_DECL_TYPEDEF
	     || typePtr->export.decl == SMI_DECL_TEXTUALCONVENTION)
	    && ! (typePtr->flags & FLAG_INSYNTAX)) {
	    static char *status[] = { "Unknown", "current", "deprecated",
				      "mandatory", "optional", "obsolete" };
	    smiPrintErrorAtLine(parserPtr, ERR_TYPE_UNREF,
				typePtr->line,
				status[typePtr->export.status],
				typePtr->export.name);
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

	    if (typePtr->export.status < typePtr->parentPtr->export.status) {
		if (typePtr->parentPtr->export.status == SMI_STATUS_DEPRECATED) {
		    smiPrintErrorAtLine(parserPtr, ERR_TYPE_STATUS_DEPRECATED,
					typePtr->line,
					typePtr->parentPtr->export.name,
					typePtr->export.name);
		}
		if (typePtr->parentPtr->export.status == SMI_STATUS_OBSOLETE) {
		    smiPrintErrorAtLine(parserPtr, ERR_TYPE_STATUS_OBSOLETE,
					typePtr->line,
					typePtr->parentPtr->export.name,
					typePtr->export.name);
		}
	    }
	}

	smiCheckTypeFormat(parserPtr, typePtr);
	smiCheckNamedNumberRedefinition(parserPtr, typePtr);
	smiCheckNamedNumberSubtyping(parserPtr, typePtr);
    }
}


				
static void
adjustDefval(Parser *parserPtr, SmiValue *valuePtr, Type *typePtr, int line)
{
    Object *object2Ptr;
    List *bitsListPtr, *valueListPtr, *p, *pp, *nextPtr, *listPtr;
    Import *importPtr;
    int nBits, bit;
    
    if (valuePtr->basetype == SMI_BASETYPE_UNKNOWN)
	return;
	
    if (valuePtr->basetype == SMI_BASETYPE_OBJECTIDENTIFIER) {
	/* a len of -1 indicates an unresolved label in ptr */
	if (valuePtr->len == -1) {
	    object2Ptr = findObjectByModuleAndName(parserPtr->modulePtr,
						  (char *)valuePtr->value.ptr);
	    if (!object2Ptr) {
		importPtr = findImportByName(
		    (char *)valuePtr->value.ptr, parserPtr->modulePtr);
		if (importPtr) {		/* imported object */
		    importPtr->use++;
		    object2Ptr = findObjectByModulenameAndName(
			importPtr->export.module,
			importPtr->export.name);
		}
	    }
	    if (!object2Ptr) {
		smiPrintErrorAtLine(parserPtr, ERR_UNKNOWN_OIDLABEL,
				    line,
				    (char *)valuePtr->value.ptr);
		smiFree(valuePtr->value.ptr);
		valuePtr->value.ptr = NULL;
		valuePtr->basetype = SMI_BASETYPE_UNKNOWN;
	    } else {
		smiFree(valuePtr->value.ptr);
		valuePtr->len = object2Ptr->export.oidlen;
		valuePtr->value.ptr = smiMalloc(object2Ptr->export.oidlen *
						sizeof(SmiSubid));
		memcpy(valuePtr->value.ptr,
		       object2Ptr->export.oid,
		       object2Ptr->export.oidlen * sizeof(SmiSubid));
	    }
	}
    } else if (valuePtr->basetype == SMI_BASETYPE_BITS) {
	bitsListPtr = typePtr->listPtr;
	valueListPtr = (void *)valuePtr->value.ptr;
	for (nBits = 0, p = bitsListPtr; p; p = p->nextPtr) {
	    if (nBits < 1+((NamedNumber *)(p->ptr))->export.value.value.integer32) {
		nBits = 1+((NamedNumber *)(p->ptr))->export.value.value.integer32;
	    }
	}
	valuePtr->value.ptr = smiMalloc((nBits+7)/8);
	memset(valuePtr->value.ptr, 0, (nBits+7)/8);
	valuePtr->len = (nBits+7)/8;
	for (p = valueListPtr; p;) {
	    for (pp = bitsListPtr; pp; pp = pp->nextPtr) {
		if (!strcmp(p->ptr,
			    ((NamedNumber *)(pp->ptr))->export.name)) {
		    bit = ((NamedNumber *)(pp->ptr))->export.value.value.integer32;
		    valuePtr->value.ptr[bit/8] |=
			1 << (7-(bit%8));
		}
	    }
	    smiFree(p->ptr);
	    nextPtr = p->nextPtr;
	    smiFree(p);
	    p = nextPtr;
	}
    } else if (valuePtr->basetype == SMI_BASETYPE_ENUM) {
	/* a len of -1 indicates an unresolved enum label in ptr */
	if (valuePtr->len == -1) {
	    for (listPtr = typePtr->listPtr; listPtr;
		 listPtr = listPtr->nextPtr) {
		if (!strcmp(((NamedNumber *)(listPtr->ptr))->export.name,
			    (char *)valuePtr->value.ptr)) {
		    smiFree(valuePtr->value.ptr);
		    valuePtr->value.integer32 =
			((NamedNumber *)(listPtr->ptr))->
			export.value.value.integer32;
		    valuePtr->len = 1;
		    break;
		}
	    }
	}
    }
}



static void
checkDefvals(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr;
    
    /*
     * Check unknown identifiers in OID DEFVALs.
     */

    for(objectPtr = modulePtr->firstObjectPtr;
	objectPtr; objectPtr = objectPtr->nextPtr) {

	adjustDefval(parserPtr, &objectPtr->export.value,
		     objectPtr->typePtr, objectPtr->line);
	
	if (objectPtr->export.value.len == -1) {
	    smiPrintErrorAtLine(parserPtr,
				ERR_DEFVAL_SYNTAX, objectPtr->line);
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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 1292 "parser-smi.y"
typedef union YYSTYPE {
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
    SmiUnsigned64  unsigned64;			/*                           */
    SmiInteger64   integer64;			/*                           */
    struct Compl   compl;
    struct Index   index;
    Module	   *modulePtr;
    SubjectCategories *subjectCategoriesPtr;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 1560 "parser-smi.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 1572 "parser-smi.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   664

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  107
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  224
/* YYNRULES -- Number of rules. */
#define YYNRULES  427
/* YYNRULES -- Number of states. */
#define YYNSTATES  712

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   351

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     101,   102,     2,     2,   100,     2,   105,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    99,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   103,     2,   104,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    97,   106,    98,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     6,     8,    11,    12,    23,    27,
      28,    30,    32,    34,    35,    39,    40,    41,    45,    47,
      48,    50,    53,    57,    59,    63,    65,    67,    69,    71,
      73,    75,    77,    79,    81,    83,    85,    87,    89,    91,
      93,    95,    97,    99,   101,   103,   105,   107,   109,   111,
     113,   115,   117,   119,   121,   122,   124,   127,   129,   131,
     133,   135,   137,   139,   141,   143,   145,   147,   149,   151,
     154,   155,   156,   162,   164,   166,   168,   170,   172,   174,
     176,   178,   180,   182,   183,   187,   189,   191,   192,   201,
     202,   207,   209,   211,   213,   215,   217,   219,   221,   223,
     225,   227,   229,   231,   233,   235,   237,   239,   240,   241,
     253,   255,   259,   261,   266,   268,   272,   275,   277,   282,
     284,   286,   289,   291,   295,   296,   302,   303,   304,   305,
     320,   321,   322,   346,   347,   350,   351,   352,   364,   369,
     370,   372,   376,   378,   381,   382,   384,   386,   387,   390,
     392,   394,   395,   401,   402,   403,   409,   410,   411,   417,
     418,   420,   421,   423,   427,   429,   430,   436,   437,   439,
     443,   448,   449,   453,   454,   458,   459,   460,   461,   477,
     478,   479,   480,   481,   482,   483,   484,   485,   510,   515,
     516,   518,   520,   524,   526,   531,   533,   536,   538,   540,
     542,   544,   550,   556,   558,   560,   562,   564,   565,   569,
     570,   574,   576,   577,   581,   582,   586,   591,   594,   599,
     602,   603,   608,   611,   616,   620,   622,   624,   626,   628,
     630,   632,   634,   636,   640,   643,   646,   650,   654,   657,
     660,   663,   665,   668,   670,   673,   676,   678,   681,   684,
     687,   689,   692,   694,   697,   700,   703,   706,   709,   712,
     714,   717,   719,   721,   723,   725,   727,   728,   732,   739,
     741,   745,   747,   751,   753,   755,   757,   759,   761,   763,
     767,   769,   773,   774,   780,   782,   784,   786,   788,   791,
     792,   795,   796,   798,   799,   805,   810,   811,   817,   818,
     819,   825,   826,   828,   832,   835,   837,   839,   841,   846,
     847,   849,   853,   855,   856,   858,   862,   864,   866,   869,
     870,   872,   873,   875,   878,   879,   885,   890,   891,   896,
     898,   902,   904,   909,   911,   915,   917,   919,   921,   922,
     925,   927,   930,   932,   936,   938,   943,   950,   952,   954,
     957,   962,   964,   965,   966,   967,   983,   984,   985,   986,
    1002,  1003,  1004,  1005,  1021,  1023,  1025,  1028,  1029,  1035,
    1038,  1040,  1041,  1046,  1047,  1049,  1053,  1055,  1057,  1058,
    1060,  1063,  1065,  1067,  1068,  1074,  1075,  1084,  1087,  1088,
    1091,  1092,  1094,  1097,  1100,  1101,  1102,  1103,  1104,  1122,
    1124,  1125,  1127,  1130,  1131,  1140,  1142,  1146,  1148,  1151,
    1153,  1155,  1156,  1158,  1161,  1162,  1163,  1164,  1165,  1166,
    1181,  1184,  1185,  1187,  1192,  1193,  1195,  1199
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     108,     0,    -1,   109,    -1,    -1,   110,    -1,   109,   110,
      -1,    -1,   126,   111,   112,   113,     4,    18,   116,   114,
     127,    29,    -1,    97,   269,    98,    -1,    -1,    25,    -1,
      69,    -1,   115,    -1,    -1,    39,   118,    99,    -1,    -1,
      -1,    31,   117,    99,    -1,   119,    -1,    -1,   120,    -1,
     119,   120,    -1,   121,    33,   126,    -1,   122,    -1,   121,
     100,   122,    -1,     6,    -1,     5,    -1,   123,    -1,   124,
      -1,   125,    -1,    19,    -1,    44,    -1,    46,    -1,    49,
      -1,    53,    -1,    54,    -1,    60,    -1,    61,    -1,    62,
      -1,    67,    -1,    85,    -1,    86,    -1,    91,    -1,    15,
      -1,    23,    -1,    24,    -1,    35,    -1,    57,    -1,    58,
      -1,    87,    -1,    45,    -1,    92,    -1,     5,    -1,   128,
      -1,    -1,   129,    -1,   128,   129,    -1,   139,    -1,   137,
      -1,   159,    -1,   163,    -1,   167,    -1,   193,    -1,   197,
      -1,   284,    -1,   276,    -1,   280,    -1,   307,    -1,   130,
      -1,     1,    98,    -1,    -1,    -1,   133,   131,    48,   132,
      29,    -1,    54,    -1,    62,    -1,    87,    -1,    58,    -1,
      61,    -1,    85,    -1,    60,    -1,    57,    -1,    53,    -1,
      15,    -1,    -1,    20,   135,    98,    -1,     6,    -1,     5,
      -1,    -1,   136,   138,    59,    36,     4,    97,   269,    98,
      -1,    -1,   141,   140,     4,   146,    -1,     5,    -1,   142,
      -1,   145,    -1,   143,    -1,   144,    -1,    46,    -1,    86,
      -1,    67,    -1,    44,    -1,    91,    -1,    23,    -1,    35,
      -1,    24,    -1,    45,    -1,    92,    -1,   154,    -1,    -1,
      -1,    85,   147,   237,    80,   235,    27,   267,   148,   255,
      84,   154,    -1,   134,    -1,    78,    65,   150,    -1,     5,
      -1,    78,    97,   152,    98,    -1,   153,    -1,   152,   100,
     153,    -1,     6,   155,    -1,   210,    -1,    19,    97,   156,
      98,    -1,   212,    -1,    19,    -1,     5,   224,    -1,   157,
      -1,   156,   100,   157,    -1,    -1,     6,   158,   101,     7,
     102,    -1,    -1,    -1,    -1,     6,   160,    61,   161,    80,
     235,    27,   267,   162,   255,     4,    97,   269,    98,    -1,
      -1,    -1,     6,   164,    62,   165,    84,   154,   238,   174,
     177,   179,    80,   235,   166,   186,   255,   240,   243,   181,
     249,     4,    97,   253,    98,    -1,    -1,    27,   267,    -1,
      -1,    -1,   136,   168,    87,   169,    30,   269,   170,   173,
     255,     4,     7,    -1,    94,    97,   171,    98,    -1,    -1,
     172,    -1,   171,   100,   172,    -1,   253,    -1,    27,   267,
      -1,    -1,   190,    -1,   175,    -1,    -1,   176,   239,    -1,
      74,    -1,    68,    -1,    -1,    72,   178,    97,   248,    98,
      -1,    -1,    -1,    73,   180,    97,   253,    98,    -1,    -1,
      -1,    88,   182,    97,   183,    98,    -1,    -1,   184,    -1,
      -1,   185,    -1,   184,   100,   185,    -1,   253,    -1,    -1,
      42,   187,    97,   188,    98,    -1,    -1,   189,    -1,   188,
     100,   189,    -1,     6,   101,     7,   102,    -1,    -1,    50,
     191,   239,    -1,    -1,    14,   192,   239,    -1,    -1,    -1,
      -1,     6,   194,    58,   195,   260,    80,   235,    27,   267,
     196,   255,     4,    97,   254,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     6,   198,    54,   199,   206,
     200,    47,   268,   201,    66,   267,   202,    21,   267,   203,
      27,   267,   204,   256,   205,     4,    97,   269,    98,    -1,
      82,    97,   207,    98,    -1,    -1,   208,    -1,   209,    -1,
     208,   100,   209,    -1,     6,    -1,     6,   101,     7,   102,
      -1,   214,    -1,   211,   214,    -1,   149,    -1,   150,    -1,
     151,    -1,   222,    -1,   103,    16,     7,   104,    37,    -1,
     103,    90,     7,   104,    37,    -1,   221,    -1,   223,    -1,
     220,    -1,    43,    -1,    -1,    43,   215,   225,    -1,    -1,
      43,   216,   230,    -1,    44,    -1,    -1,    44,   217,   225,
      -1,    -1,     5,   218,   230,    -1,   126,   105,     5,   230,
      -1,     5,   225,    -1,   126,   105,     5,   225,    -1,    64,
      81,    -1,    -1,    64,    81,   219,   226,    -1,     5,   226,
      -1,   126,   105,     5,   226,    -1,    59,    36,   224,    -1,
       7,    -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,
      12,    -1,     6,    -1,    13,    -1,    97,   273,    98,    -1,
      43,   224,    -1,    44,   224,    -1,    64,    81,   224,    -1,
      59,    36,   224,    -1,    46,   224,    -1,    23,   224,    -1,
      23,   225,    -1,    35,    -1,    35,   225,    -1,    91,    -1,
      91,   225,    -1,    86,   224,    -1,    67,    -1,    67,   226,
      -1,    24,   224,    -1,    24,   225,    -1,    45,    -1,    45,
     225,    -1,    92,    -1,    92,   225,    -1,    46,   224,    -1,
      23,   224,    -1,    35,   224,    -1,    91,   224,    -1,    86,
     224,    -1,    67,    -1,    24,   224,    -1,    45,    -1,    92,
      -1,   225,    -1,   226,    -1,   230,    -1,    -1,   101,   227,
     102,    -1,   101,    79,   101,   227,   102,   102,    -1,   228,
      -1,   227,   106,   228,    -1,   229,    -1,   229,     3,   229,
      -1,     8,    -1,     7,    -1,    10,    -1,     9,    -1,    12,
      -1,    11,    -1,    97,   231,    98,    -1,   232,    -1,   231,
     100,   232,    -1,    -1,     6,   233,   101,   234,   102,    -1,
       7,    -1,     8,    -1,     6,    -1,     6,    -1,    28,   267,
      -1,    -1,    89,   267,    -1,    -1,     6,    -1,    -1,    70,
     241,    97,   248,    98,    -1,    17,    97,   248,    98,    -1,
      -1,    32,   242,    97,   248,    98,    -1,    -1,    -1,    41,
     244,    97,   245,    98,    -1,    -1,   246,    -1,   245,   100,
     246,    -1,    38,   247,    -1,   247,    -1,   253,    -1,   253,
      -1,    26,    97,   250,    98,    -1,    -1,   213,    -1,    97,
     251,    98,    -1,   252,    -1,    -1,     6,    -1,   252,   100,
       6,    -1,   269,    -1,   269,    -1,    76,   267,    -1,    -1,
     257,    -1,    -1,   258,    -1,   257,   258,    -1,    -1,    77,
     268,   259,    27,   267,    -1,    63,    97,   262,    98,    -1,
      -1,    63,    97,   262,    98,    -1,   263,    -1,   262,   100,
     263,    -1,   253,    -1,    56,    97,   265,    98,    -1,   266,
      -1,   265,   100,   266,    -1,   254,    -1,    13,    -1,    13,
      -1,    -1,   270,   271,    -1,   272,    -1,   271,   272,    -1,
     136,    -1,   126,   105,     6,    -1,     7,    -1,     6,   101,
       7,   102,    -1,   126,   105,     6,   101,     7,   102,    -1,
     274,    -1,   275,    -1,   274,   275,    -1,     6,   101,     7,
     102,    -1,     7,    -1,    -1,    -1,    -1,     6,   277,    60,
     278,   261,    80,   235,    27,   267,   279,   255,     4,    97,
     269,    98,    -1,    -1,    -1,    -1,     6,   281,    57,   282,
     264,    80,   235,    27,   267,   283,   255,     4,    97,   269,
      98,    -1,    -1,    -1,    -1,     6,   285,    53,   286,    80,
     235,    27,   267,   287,   255,   288,     4,    97,   269,    98,
      -1,   289,    -1,   290,    -1,   289,   290,    -1,    -1,    52,
     292,   291,   293,   296,    -1,     5,   269,    -1,     5,    -1,
      -1,    49,    97,   294,    98,    -1,    -1,   295,    -1,   294,
     100,   295,    -1,   269,    -1,   297,    -1,    -1,   298,    -1,
     297,   298,    -1,   299,    -1,   301,    -1,    -1,    34,   300,
     269,    27,   267,    -1,    -1,    59,   302,   253,   303,   304,
     306,    27,   267,    -1,    84,   154,    -1,    -1,    96,   305,
      -1,    -1,   154,    -1,    51,   239,    -1,    71,   239,    -1,
      -1,    -1,    -1,    -1,     6,   308,    15,   309,    75,   267,
      80,   236,    27,   267,   310,   255,   311,     4,    97,   269,
      98,    -1,   312,    -1,    -1,   313,    -1,   312,   313,    -1,
      -1,    83,   317,   314,    40,    97,   315,    98,   318,    -1,
     316,    -1,   315,   100,   316,    -1,   269,    -1,     5,   269,
      -1,     5,    -1,   319,    -1,    -1,   320,    -1,   319,   320,
      -1,    -1,    -1,    -1,    -1,    -1,    95,   253,   321,   303,
     322,   304,   323,   326,   328,   324,   249,   325,    27,   267,
      -1,    14,   327,    -1,    -1,     6,    -1,    22,    97,   329,
      98,    -1,    -1,   330,    -1,   329,   100,   330,    -1,   253,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,  1590,  1590,  1595,  1600,  1602,  1611,  1610,  1672,  1675,
    1678,  1680,  1689,  1692,  1695,  1710,  1712,  1711,  1732,  1735,
    1739,  1741,  1745,  1790,  1792,  1800,  1806,  1812,  1828,  1838,
    1844,  1845,  1846,  1847,  1848,  1849,  1850,  1851,  1852,  1853,
    1854,  1855,  1856,  1859,  1860,  1861,  1862,  1863,  1864,  1865,
    1868,  1869,  1872,  1884,  1887,  1891,  1893,  1897,  1902,  1907,
    1912,  1917,  1922,  1927,  1932,  1937,  1942,  1947,  1952,  1957,
    1972,  1984,  1971,  2017,  2018,  2019,  2020,  2021,  2022,  2023,
    2024,  2025,  2026,  2030,  2029,  2059,  2064,  2080,  2079,  2120,
    2119,  2285,  2289,  2313,  2326,  2327,  2336,  2337,  2338,  2339,
    2340,  2343,  2344,  2345,  2348,  2349,  2352,  2369,  2400,  2368,
    2436,  2444,  2458,  2503,  2511,  2517,  2537,  2620,  2626,  2648,
    2652,  2657,  2690,  2696,  2710,  2709,  2745,  2755,  2780,  2744,
    2808,  2818,  2807,  3114,  3123,  3131,  3141,  3130,  3213,  3218,
    3223,  3233,  3255,  3261,  3267,  3270,  3274,  3283,  3286,  3290,
    3294,  3299,  3298,  3306,  3310,  3309,  3317,  3322,  3321,  3329,
    3332,  3335,  3338,  3344,  3358,  3363,  3362,  3370,  3373,  3379,
    3393,  3415,  3414,  3426,  3425,  3439,  3449,  3469,  3438,  3500,
    3510,  3535,  3539,  3543,  3550,  3557,  3561,  3499,  3607,  3614,
    3621,  3628,  3634,  3649,  3670,  3693,  3697,  3717,  3722,  3727,
    3732,  3749,  3751,  3759,  3764,  3781,  3790,  3804,  3803,  3823,
    3822,  3837,  3864,  3863,  3895,  3894,  3948,  3995,  4050,  4100,
    4106,  4105,  4117,  4163,  4204,  4211,  4218,  4224,  4231,  4238,
    4264,  4287,  4307,  4324,  4357,  4361,  4386,  4390,  4396,  4424,
    4450,  4487,  4513,  4546,  4570,  4597,  4625,  4663,  4709,  4735,
    4772,  4797,  4829,  4854,  4892,  4920,  4946,  4972,  4996,  5024,
    5064,  5090,  5115,  5142,  5159,  5176,  5195,  5202,  5212,  5224,
    5230,  5244,  5251,  5261,  5267,  5273,  5279,  5285,  5308,  5335,
    5341,  5347,  5361,  5360,  5382,  5397,  5406,  5465,  5481,  5491,
    5496,  5506,  5511,  5578,  5577,  5595,  5606,  5605,  5619,  5625,
    5624,  5658,  5663,  5669,  5683,  5688,  5694,  5704,  5710,  5713,
    5717,  5719,  5727,  5730,  5733,  5739,  5752,  5758,  5764,  5774,
    5777,  5780,  5783,  5785,  5790,  5789,  5820,  5825,  5830,  5836,
    5842,  5855,  5867,  5873,  5879,  5892,  5903,  5914,  5920,  5920,
    5935,  5939,  5948,  6042,  6146,  6170,  6229,  6298,  6302,  6304,
    6308,  6310,  6315,  6325,  6344,  6314,  6375,  6385,  6407,  6374,
    6439,  6449,  6467,  6438,  6531,  6537,  6541,  6577,  6576,  6605,
    6614,  6624,  6629,  6634,  6639,  6645,  6658,  6681,  6688,  6695,
    6699,  6766,  6772,  6781,  6780,  6814,  6813,  6860,  6869,  6874,
    6886,  6891,  6897,  6903,  6912,  6918,  6928,  6950,  6917,  6985,
    6988,  6991,  6993,  6998,  6997,  7024,  7034,  7051,  7057,  7066,
    7076,  7079,  7082,  7084,  7089,  7097,  7104,  7112,  7119,  7088,
    7142,  7145,  7148,  7213,  7216,  7219,  7221,  7225
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOT_DOT", "COLON_COLON_EQUAL",
  "UPPERCASE_IDENTIFIER", "LOWERCASE_IDENTIFIER", "NUMBER",
  "NEGATIVENUMBER", "NUMBER64", "NEGATIVENUMBER64", "BIN_STRING",
  "HEX_STRING", "QUOTED_STRING", "ACCESS", "AGENT_CAPABILITIES",
  "APPLICATION", "AUGMENTS", "BEGIN_", "BITS", "CHOICE", "CONTACT_INFO",
  "CREATION_REQUIRES", "COUNTER32", "COUNTER64", "DEFINITIONS", "DEFVAL",
  "DESCRIPTION", "DISPLAY_HINT", "END", "ENTERPRISE", "EXPORTS", "EXTENDS",
  "FROM", "GROUP", "GAUGE32", "IDENTIFIER", "IMPLICIT", "IMPLIED",
  "IMPORTS", "INCLUDES", "INDEX", "INSTALL_ERRORS", "INTEGER", "INTEGER32",
  "INTEGER64", "IPADDRESS", "LAST_UPDATED", "MACRO", "MANDATORY_GROUPS",
  "MAX_ACCESS", "MIN_ACCESS", "MODULE", "MODULE_COMPLIANCE",
  "MODULE_IDENTITY", "NOT_ACCESSIBLE", "NOTIFICATIONS",
  "NOTIFICATION_GROUP", "NOTIFICATION_TYPE", "OBJECT", "OBJECT_GROUP",
  "OBJECT_IDENTITY", "OBJECT_TYPE", "OBJECTS", "OCTET", "OF",
  "ORGANIZATION", "OPAQUE", "PIB_ACCESS", "PIB_DEFINITIONS", "PIB_INDEX",
  "PIB_MIN_ACCESS", "PIB_REFERENCES", "PIB_TAG", "POLICY_ACCESS",
  "PRODUCT_RELEASE", "REFERENCE", "REVISION", "SEQUENCE", "SIZE", "STATUS",
  "STRING", "SUBJECT_CATEGORIES", "SUPPORTS", "SYNTAX",
  "TEXTUAL_CONVENTION", "TIMETICKS", "TRAP_TYPE", "UNIQUENESS", "UNITS",
  "UNIVERSAL", "UNSIGNED32", "UNSIGNED64", "VALUE", "VARIABLES",
  "VARIATION", "WRITE_SYNTAX", "'{'", "'}'", "';'", "','", "'('", "')'",
  "'['", "']'", "'.'", "'|'", "$accept", "mibFile", "modules", "module",
  "@1", "moduleOid", "definitions", "linkagePart", "linkageClause",
  "exportsClause", "@2", "importPart", "imports", "import",
  "importIdentifiers", "importIdentifier", "importedKeyword",
  "importedSMIKeyword", "importedSPPIKeyword", "moduleName",
  "declarationPart", "declarations", "declaration", "macroClause", "@3",
  "@4", "macroName", "choiceClause", "@5", "fuzzy_lowercase_identifier",
  "valueDeclaration", "@6", "typeDeclaration", "@7", "typeName", "typeSMI",
  "typeSMIandSPPI", "typeSMIonly", "typeSPPIonly", "typeDeclarationRHS",
  "@8", "@9", "conceptualTable", "row", "entryType", "sequenceItems",
  "sequenceItem", "Syntax", "sequenceSyntax", "NamedBits", "NamedBit",
  "@10", "objectIdentityClause", "@11", "@12", "@13", "objectTypeClause",
  "@14", "@15", "descriptionClause", "trapTypeClause", "@16", "@17",
  "VarPart", "VarTypes", "VarType", "DescrPart", "MaxOrPIBAccessPart",
  "PibAccessPart", "PibAccess", "SPPIPibReferencesPart", "@18",
  "SPPIPibTagPart", "@19", "SPPIUniquePart", "@20", "UniqueTypesPart",
  "UniqueTypes", "UniqueType", "SPPIErrorsPart", "@21", "Errors", "Error",
  "MaxAccessPart", "@22", "@23", "notificationTypeClause", "@24", "@25",
  "@26", "moduleIdentityClause", "@27", "@28", "@29", "@30", "@31", "@32",
  "@33", "@34", "SubjectCategoriesPart", "SubjectCategories",
  "CategoryIDs", "CategoryID", "ObjectSyntax", "typeTag",
  "sequenceObjectSyntax", "valueofObjectSyntax", "SimpleSyntax", "@35",
  "@36", "@37", "@38", "@39", "valueofSimpleSyntax",
  "sequenceSimpleSyntax", "ApplicationSyntax", "sequenceApplicationSyntax",
  "anySubType", "integerSubType", "octetStringSubType", "ranges", "range",
  "value", "enumSpec", "enumItems", "enumItem", "@40", "enumNumber",
  "Status", "Status_Capabilities", "DisplayPart", "UnitsPart", "Access",
  "IndexPart", "@41", "@42", "MibIndex", "@43", "IndexTypes", "IndexType",
  "Index", "Entry", "DefValPart", "Value", "BitsValue", "BitNames",
  "ObjectName", "NotificationName", "ReferPart", "RevisionPart",
  "Revisions", "Revision", "@44", "NotificationObjectsPart",
  "ObjectGroupObjectsPart", "Objects", "Object", "NotificationsPart",
  "Notifications", "Notification", "Text", "ExtUTCTime",
  "objectIdentifier", "@45", "subidentifiers", "subidentifier",
  "objectIdentifier_defval", "subidentifiers_defval",
  "subidentifier_defval", "objectGroupClause", "@46", "@47", "@48",
  "notificationGroupClause", "@49", "@50", "@51", "moduleComplianceClause",
  "@52", "@53", "@54", "ComplianceModulePart", "ComplianceModules",
  "ComplianceModule", "@55", "ComplianceModuleName", "MandatoryPart",
  "MandatoryGroups", "MandatoryGroup", "CompliancePart", "Compliances",
  "Compliance", "ComplianceGroup", "@56", "ComplianceObject", "@57",
  "SyntaxPart", "WriteSyntaxPart", "WriteSyntax", "AccessPart",
  "agentCapabilitiesClause", "@58", "@59", "@60",
  "ModulePart_Capabilities", "Modules_Capabilities", "Module_Capabilities",
  "@61", "CapabilitiesGroups", "CapabilitiesGroup",
  "ModuleName_Capabilities", "VariationPart", "Variations", "Variation",
  "@62", "@63", "@64", "@65", "@66", "VariationAccessPart",
  "VariationAccess", "CreationPart", "Cells", "Cell", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   123,   125,    59,
      44,    40,    41,    91,    93,    46,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned short int yyr1[] =
{
       0,   107,   108,   108,   109,   109,   111,   110,   112,   112,
     113,   113,   114,   114,   115,   116,   117,   116,   118,   118,
     119,   119,   120,   121,   121,   122,   122,   122,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   124,   124,   124,   124,   124,   124,   124,
     125,   125,   126,   127,   127,   128,   128,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     131,   132,   130,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   135,   134,   136,   136,   138,   137,   140,
     139,   141,   141,   141,   142,   142,   143,   143,   143,   143,
     143,   144,   144,   144,   145,   145,   146,   147,   148,   146,
     146,   149,   150,   151,   152,   152,   153,   154,   154,   155,
     155,   155,   156,   156,   158,   157,   160,   161,   162,   159,
     164,   165,   163,   166,   166,   168,   169,   167,   170,   170,
     171,   171,   172,   173,   173,   174,   174,   174,   175,   176,
     176,   178,   177,   177,   180,   179,   179,   182,   181,   181,
     183,   183,   184,   184,   185,   187,   186,   186,   188,   188,
     189,   191,   190,   192,   190,   194,   195,   196,   193,   198,
     199,   200,   201,   202,   203,   204,   205,   197,   206,   206,
     207,   208,   208,   209,   209,   210,   210,   210,   210,   210,
     210,   211,   211,   212,   212,   213,   214,   215,   214,   216,
     214,   214,   217,   214,   218,   214,   214,   214,   214,   214,
     219,   214,   214,   214,   214,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   221,   221,   221,   221,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   224,   224,   224,   224,   225,   226,   227,
     227,   228,   228,   229,   229,   229,   229,   229,   229,   230,
     231,   231,   233,   232,   234,   234,   235,   236,   237,   237,
     238,   238,   239,   241,   240,   240,   242,   240,   240,   244,
     243,   243,   245,   245,   246,   246,   247,   248,   249,   249,
     250,   250,   251,   251,   252,   252,   253,   254,   255,   255,
     256,   256,   257,   257,   259,   258,   260,   260,   261,   262,
     262,   263,   264,   265,   265,   266,   267,   268,   270,   269,
     271,   271,   272,   272,   272,   272,   272,   273,   274,   274,
     275,   275,   277,   278,   279,   276,   281,   282,   283,   280,
     285,   286,   287,   284,   288,   289,   289,   291,   290,   292,
     292,   292,   293,   293,   294,   294,   295,   296,   296,   297,
     297,   298,   298,   300,   299,   302,   301,   303,   303,   304,
     304,   305,   306,   306,   306,   308,   309,   310,   307,   311,
     311,   312,   312,   314,   313,   315,   315,   316,   317,   317,
     318,   318,   319,   319,   321,   322,   323,   324,   325,   320,
     326,   326,   327,   328,   328,   329,   329,   330
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     1,     2,     0,    10,     3,     0,
       1,     1,     1,     0,     3,     0,     0,     3,     1,     0,
       1,     2,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       0,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     1,     1,     0,     8,     0,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     0,    11,
       1,     3,     1,     4,     1,     3,     2,     1,     4,     1,
       1,     2,     1,     3,     0,     5,     0,     0,     0,    14,
       0,     0,    23,     0,     2,     0,     0,    11,     4,     0,
       1,     3,     1,     2,     0,     1,     1,     0,     2,     1,
       1,     0,     5,     0,     0,     5,     0,     0,     5,     0,
       1,     0,     1,     3,     1,     0,     5,     0,     1,     3,
       4,     0,     3,     0,     3,     0,     0,     0,    15,     0,
       0,     0,     0,     0,     0,     0,     0,    24,     4,     0,
       1,     1,     3,     1,     4,     1,     2,     1,     1,     1,
       1,     5,     5,     1,     1,     1,     1,     0,     3,     0,
       3,     1,     0,     3,     0,     3,     4,     2,     4,     2,
       0,     4,     2,     4,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     2,     2,     3,     3,     2,     2,
       2,     1,     2,     1,     2,     2,     1,     2,     2,     2,
       1,     2,     1,     2,     2,     2,     2,     2,     2,     1,
       2,     1,     1,     1,     1,     1,     0,     3,     6,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     3,
       1,     3,     0,     5,     1,     1,     1,     1,     2,     0,
       2,     0,     1,     0,     5,     4,     0,     5,     0,     0,
       5,     0,     1,     3,     2,     1,     1,     1,     4,     0,
       1,     3,     1,     0,     1,     3,     1,     1,     2,     0,
       1,     0,     1,     2,     0,     5,     4,     0,     4,     1,
       3,     1,     4,     1,     3,     1,     1,     1,     0,     2,
       1,     2,     1,     3,     1,     4,     6,     1,     1,     2,
       4,     1,     0,     0,     0,    15,     0,     0,     0,    15,
       0,     0,     0,    15,     1,     1,     2,     0,     5,     2,
       1,     0,     4,     0,     1,     3,     1,     1,     0,     1,
       2,     1,     1,     0,     5,     0,     8,     2,     0,     2,
       0,     1,     2,     2,     0,     0,     0,     0,    17,     1,
       0,     1,     2,     0,     8,     1,     3,     1,     2,     1,
       1,     0,     1,     2,     0,     0,     0,     0,     0,    14,
       2,     0,     1,     4,     0,     1,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned short int yydefact[] =
{
       3,    52,     0,     2,     4,     6,     1,     5,     9,   338,
       0,     0,     0,    10,    11,     0,     8,    86,    85,   344,
       0,   342,   339,   340,     0,     0,     0,   341,    15,     0,
     343,    16,    13,   345,     0,     0,    19,     0,    12,     0,
      17,    26,    25,    43,    30,    44,    45,    46,    31,    50,
      32,    33,    34,    35,    47,    48,    36,    37,    38,    39,
      40,    41,    49,    42,    51,     0,    18,    20,     0,    23,
      27,    28,    29,     0,    86,    85,    82,   101,   103,   102,
      99,   104,    96,    81,    73,    80,    76,    79,    77,    74,
      98,    78,    97,    75,   100,   105,     0,     0,    55,    68,
      70,    87,    58,    57,    89,    92,    94,    95,    93,    59,
      60,    61,    62,    63,    65,    66,    64,    67,   346,    14,
      21,     0,     0,    69,     0,     0,     0,     0,     0,     0,
       0,     0,     7,    56,     0,     0,     0,     0,    22,    24,
     127,   131,   176,   180,   353,   357,   361,   396,    71,     0,
     136,     0,     0,     0,   327,   189,     0,     0,     0,     0,
       0,     0,     0,   112,     0,    83,   266,   266,   241,   206,
     211,   250,   266,     0,     0,   246,     0,   107,   266,   243,
     252,     0,     0,   110,    90,   197,   198,   199,   106,   117,
       0,   195,   200,     0,     0,     0,     0,     0,   181,     0,
       0,     0,     0,     0,     0,    72,     0,   338,     0,     0,
     217,   222,     0,     0,     0,   239,   240,   264,   265,   248,
     249,     0,   242,     0,     0,     0,   251,   238,   263,   266,
     219,     0,   247,     0,     0,   289,   245,   244,   253,     0,
       0,     0,    52,   196,   286,     0,   291,   338,     0,     0,
       0,   338,     0,   338,     0,     0,   336,     0,   338,   139,
     274,   273,   276,   275,   278,   277,     0,     0,   269,   271,
     215,   124,     0,   122,    84,   282,     0,   280,   208,   210,
     213,   224,     0,   112,   111,     0,     0,   114,     0,     0,
       0,     0,     0,     0,     0,   147,   331,     0,   329,   316,
       0,   193,     0,   190,   191,     0,     0,     0,   335,     0,
     333,   317,     0,     0,     0,     0,     0,   144,     0,   267,
       0,     0,     0,   118,     0,     0,   279,     0,   221,   266,
     120,   266,   266,   266,   266,   266,   261,   266,     0,     0,
     259,   266,   266,   262,   116,   119,   203,   204,   113,     0,
     288,     0,     0,     0,   218,   223,   216,   128,   290,   173,
     171,   150,   149,   153,   146,     0,   145,   326,   338,     0,
       0,   188,     0,   337,   182,   328,     0,   332,   338,     0,
     362,   287,     0,    88,   338,     0,   319,     0,   270,   272,
       0,   123,     0,   281,   121,   255,   260,   256,   234,   235,
     254,   266,   266,   258,   257,   115,     0,   201,   202,   319,
       0,     0,   151,   156,   292,   148,   330,   177,     0,   192,
       0,   354,   334,   358,   319,     0,     0,   140,   142,   143,
       0,     0,     0,     0,   284,   285,     0,   237,   236,     0,
       0,   174,   172,     0,   154,     0,   319,   194,     0,   319,
     319,     0,   397,   138,   338,   318,     0,   268,   125,   283,
     108,     0,   338,     0,     0,     0,   183,     0,     0,   371,
       0,   364,   365,   319,   141,   137,   319,   338,     0,   307,
     338,   133,     0,     0,     0,     0,   370,   367,     0,   366,
     400,     0,     0,   152,     0,     0,   167,   338,     0,   338,
     338,   369,   373,   338,     0,     0,   399,   401,     0,   129,
     155,   134,   165,   319,     0,   184,     0,     0,     0,   378,
       0,   338,   403,     0,   402,   109,     0,   298,   178,     0,
     355,   359,   338,   383,   385,   368,   377,   379,   381,   382,
     363,   408,     0,   338,     0,     0,   296,   293,   301,     0,
     376,     0,   374,   338,   338,   380,     0,     0,     0,     0,
     168,   338,     0,     0,   299,   159,   185,   372,   338,     0,
     388,   338,   398,     0,   166,     0,     0,   338,   338,     0,
     157,   309,   321,   375,     0,     0,   390,   407,     0,   405,
       0,   169,   295,     0,     0,   338,     0,     0,     0,     0,
     186,   320,   322,   384,   387,     0,   394,   411,   338,   170,
     297,   294,   338,     0,   302,   305,   306,   338,     0,     0,
     324,     0,   323,   391,   389,     0,     0,     0,   338,   404,
     410,   412,   406,   304,   300,   338,     0,   160,   162,   164,
     231,   225,   226,   227,   228,   229,   230,   232,   313,   310,
     205,     0,   338,     0,     0,   392,   393,     0,   414,   413,
     303,   158,   338,   314,   351,     0,   312,     0,   347,   348,
     308,     0,     0,   338,   386,   388,   163,     0,   311,     0,
     233,     0,   349,   132,   325,     0,   415,     0,   315,   187,
     390,   350,   416,   421,     0,   424,   422,   420,     0,   417,
     338,   309,   427,     0,   425,   418,   423,   338,     0,   426,
       0,   419
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,     2,     3,     4,     8,    10,    15,    37,    38,    32,
      35,    65,    66,    67,    68,    69,    70,    71,    72,   182,
      96,    97,    98,    99,   134,   160,   100,   183,   213,    21,
     102,   135,   103,   137,   104,   105,   106,   107,   108,   184,
     235,   476,   185,   186,   187,   286,   287,   188,   344,   272,
     273,   322,   109,   124,   152,   409,   110,   125,   153,   496,
     111,   136,   162,   317,   426,   427,   386,   363,   364,   365,
     413,   443,   445,   463,   581,   596,   636,   637,   638,   513,
     526,   559,   560,   366,   411,   410,   112,   126,   154,   446,
     113,   127,   155,   250,   420,   483,   529,   582,   621,   198,
     302,   303,   304,   189,   190,   345,   649,   191,   223,   224,
     225,   209,   282,   650,   346,   192,   347,   215,   228,   217,
     267,   268,   269,   218,   276,   277,   325,   436,   245,   382,
     289,   295,   415,   548,   563,   562,   565,   579,   613,   614,
     615,   478,   598,   651,   665,   666,   479,   308,   431,   600,
     601,   602,   653,   196,   200,   297,   298,   202,   309,   310,
     257,   374,   299,    12,    22,    23,   667,   668,   669,   114,
     128,   156,   449,   115,   129,   157,   450,   116,   130,   158,
     424,   470,   471,   472,   502,   487,   519,   551,   552,   535,
     536,   537,   538,   553,   539,   554,   586,   606,   624,   627,
     117,   131,   159,   473,   505,   506,   507,   542,   588,   589,
     522,   629,   630,   631,   675,   690,   693,   701,   708,   695,
     697,   699,   703,   704
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -461
static const short int yypact[] =
{
      19,  -461,    36,    19,  -461,  -461,  -461,  -461,   -58,  -461,
      -5,   -31,    54,  -461,  -461,    78,  -461,    -1,     9,  -461,
      38,  -461,    54,  -461,   107,   154,   160,  -461,   138,    88,
      90,  -461,   153,  -461,   186,    95,   561,   350,  -461,    93,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,    97,   561,  -461,   -23,  -461,
    -461,  -461,  -461,    99,   195,    70,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,   171,   496,  -461,  -461,
    -461,   115,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,    19,   561,  -461,   140,   141,   146,   151,   149,   155,
     158,   191,  -461,  -461,   166,   156,   129,   213,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,   183,
    -461,   202,   143,   134,   157,   142,   167,   173,   152,   161,
     204,   227,   205,     2,   145,  -461,   -47,   -47,   150,   -18,
     159,   150,   -47,   203,   162,   169,   -49,  -461,   -47,   150,
     150,    -2,   148,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
      52,  -461,  -461,   244,   233,   165,   174,   170,  -461,   176,
     178,   177,   179,   244,   242,  -461,   184,  -461,    35,   185,
    -461,  -461,   257,   187,   265,  -461,  -461,  -461,  -461,  -461,
    -461,   175,  -461,   150,   185,   150,  -461,  -461,  -461,   -47,
     188,   193,  -461,   270,   277,   256,  -461,  -461,  -461,   279,
     283,   286,     5,  -461,  -461,   268,   207,  -461,   244,   292,
     254,  -461,   244,  -461,   244,   275,  -461,   223,  -461,   210,
    -461,  -461,  -461,  -461,  -461,  -461,   206,     3,  -461,   303,
    -461,  -461,    15,  -461,  -461,  -461,    37,  -461,  -461,  -461,
    -461,  -461,   169,  -461,  -461,   441,    41,  -461,   242,   229,
     208,   209,   -47,   242,   242,    13,  -461,    44,  -461,  -461,
     287,   214,   212,   217,  -461,   305,    48,   293,  -461,    51,
    -461,  -461,   294,   242,   316,   225,   230,   299,   175,  -461,
     175,   175,   228,  -461,   257,   231,  -461,   265,  -461,   -47,
    -461,   -47,   -47,   -47,   -47,   -47,  -461,   -47,   295,   247,
    -461,   -47,   -47,  -461,  -461,  -461,  -461,  -461,  -461,   277,
    -461,   244,   296,   297,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,   258,  -461,   329,  -461,  -461,  -461,   242,
     330,  -461,   292,  -461,  -461,  -461,   242,  -461,  -461,   242,
    -461,  -461,   311,  -461,  -461,   242,   263,     6,  -461,  -461,
     333,  -461,   126,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,   -47,   -47,  -461,  -461,  -461,   314,  -461,  -461,   263,
     329,   329,  -461,   269,  -461,  -461,  -461,  -461,   241,  -461,
     278,  -461,  -461,  -461,   263,   242,    55,  -461,  -461,  -461,
     242,   341,   245,   246,  -461,  -461,   248,  -461,  -461,   242,
     342,  -461,  -461,   255,  -461,   273,   263,  -461,   242,   263,
     263,   302,  -461,  -461,  -461,  -461,   351,  -461,  -461,  -461,
    -461,   260,  -461,   262,   244,   356,  -461,   357,   358,   359,
     363,   302,  -461,   263,  -461,  -461,   263,  -461,   272,  -461,
    -461,   336,   271,   354,   280,   281,   114,  -461,   284,  -461,
     300,   304,   282,  -461,   288,   242,   334,  -461,   242,  -461,
    -461,  -461,   335,  -461,   385,   387,   300,  -461,   233,  -461,
    -461,  -461,  -461,   263,   301,  -461,   307,   317,   312,    -3,
     320,   352,  -461,   322,  -461,  -461,   323,    20,  -461,   370,
    -461,  -461,  -461,  -461,  -461,  -461,    -3,  -461,  -461,  -461,
    -461,  -461,   360,  -461,   392,   324,  -461,  -461,   361,   242,
    -461,    58,  -461,  -461,  -461,  -461,   325,   326,   313,    59,
    -461,  -461,   331,   332,  -461,   338,  -461,  -461,  -461,   374,
     339,  -461,  -461,   420,  -461,   392,   340,  -461,  -461,   337,
    -461,   404,   355,  -461,   242,   233,   343,  -461,    64,  -461,
     345,  -461,  -461,   346,   347,   393,   353,   362,   429,   305,
    -461,   355,  -461,  -461,  -461,   233,   -13,   348,  -461,  -461,
    -461,  -461,  -461,    67,  -461,  -461,  -461,   364,    63,   366,
    -461,   436,  -461,  -461,  -461,   329,   329,   421,  -461,  -461,
     348,  -461,  -461,  -461,  -461,   393,   368,   349,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,   172,  -461,
    -461,   369,  -461,   424,   372,  -461,  -461,   242,  -461,  -461,
    -461,  -461,  -461,   371,  -461,   373,   375,   376,   182,  -461,
    -461,   380,   242,  -461,  -461,   339,  -461,   445,  -461,   447,
    -461,   371,  -461,  -461,  -461,   381,  -461,   378,  -461,  -461,
     343,  -461,  -461,   442,   449,   435,  -461,  -461,   384,  -461,
    -461,   404,  -461,    77,  -461,  -461,  -461,  -461,   431,  -461,
     242,  -461
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -461,  -461,  -461,   467,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,   407,  -461,   367,  -461,  -461,  -461,    29,
    -461,  -461,   386,  -461,  -461,  -461,  -461,  -461,  -461,    -4,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,   249,  -461,  -461,   144,  -192,  -461,  -461,
     168,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,    42,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -167,  -461,
    -461,  -461,   -77,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,   127,  -461,  -461,  -461,  -461,   319,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -161,  -145,  -156,
     189,   190,   192,  -194,  -461,   194,  -461,  -461,  -199,  -461,
    -461,  -461,  -398,  -461,  -461,  -461,  -461,  -461,  -461,  -132,
    -108,  -460,  -195,  -461,  -461,  -461,  -246,    17,  -384,  -461,
    -461,   -86,  -461,  -461,  -461,   266,   180,  -461,  -461,   196,
    -285,   -83,    -9,  -461,  -461,   500,  -461,  -461,  -150,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,    53,  -461,  -461,  -461,  -461,   -42,  -461,
    -461,    -8,  -461,  -461,  -461,  -461,  -146,  -160,  -461,  -461,
    -461,  -461,  -461,  -461,  -461,  -461,    30,  -461,  -461,   -73,
    -461,  -461,  -461,   -93,  -461,  -461,  -461,  -461,  -461,  -461,
    -461,  -461,  -461,  -169
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -410
static const short int yytable[] =
{
      11,   296,   246,   350,   255,   296,   219,   211,   357,   358,
     121,   227,   441,   442,   239,   270,   233,   236,   210,   232,
      13,   216,   220,   222,     1,   440,   226,   359,   380,     5,
     279,   533,     5,   101,   237,   238,     6,   545,   625,     9,
     451,    20,   260,   261,   262,   263,   264,   265,   234,   300,
     214,    20,   546,   307,   208,   312,   534,   242,   626,    17,
      18,    19,   465,   360,    14,   467,   468,    16,   281,   640,
     641,   642,   643,   644,   645,   646,   647,   122,   278,  -209,
     280,   361,    24,  -207,   417,  -395,   211,   362,   240,   490,
     547,   421,   491,   101,   423,   169,   170,   210,   356,  -214,
     429,   576,  -214,   208,   -52,   319,   208,   -52,   432,   320,
      25,   173,   320,   323,   266,   324,   174,   593,   594,  -338,
    -338,  -338,   296,  -360,  -179,    28,   328,  -356,  -175,   527,
    -352,  -126,  -130,   434,   435,   326,   355,   327,   428,   348,
     452,   349,   367,    26,   368,   455,   375,   354,   368,   377,
     138,   378,   406,   453,   460,   454,   567,   574,   568,   575,
     648,    29,   607,   466,   608,   634,    30,   635,   394,    31,
     395,   396,   397,   398,   399,   706,   400,   707,   663,   664,
     403,   404,   260,   261,   262,   263,   264,   265,   681,   664,
      33,    34,    36,    39,    40,   118,   119,   123,   259,   -91,
     132,   140,  -135,   141,   142,   143,   147,   163,   428,   144,
     511,   146,   145,   515,   148,   149,   150,   151,   194,   161,
     195,   164,   165,   193,   197,   166,   167,   655,   656,   201,
     199,   206,   203,   205,   494,   207,   204,   168,   163,   229,
     437,   438,   212,   230,   311,   169,   170,   171,   172,   315,
     244,   221,   164,   241,   248,   256,   166,   167,   252,   254,
    -212,   173,   247,   271,   566,   481,   174,   249,   168,   175,
     231,   275,   266,   251,   253,   283,   169,   170,   171,   172,
     176,   258,   214,   285,   288,   274,   290,   177,   178,  -220,
     291,   292,   173,   179,   180,   293,   294,   174,   301,   603,
     175,   305,   313,   314,   316,   181,   321,   318,   570,   351,
     371,   176,   352,   353,   369,   370,   525,   372,   373,   178,
     376,   379,   381,   383,   179,   180,   385,   384,   402,   390,
     412,   401,   392,   407,   408,   414,   181,   418,   425,   430,
     433,   439,   444,   447,   448,   456,   461,   457,   458,   616,
     459,    73,   462,   464,   469,    74,    75,   477,   475,   480,
     482,   484,   485,   495,   486,    76,   616,   488,   497,   311,
     493,   639,   674,    77,    78,   498,   512,   499,   500,   -54,
     509,   503,   658,   504,   518,    79,   510,   684,   508,   616,
     521,   523,  -409,   604,    80,    81,    82,   549,   558,   528,
     556,   584,   564,    83,    84,   530,   671,    85,    86,   532,
      87,    88,    89,   623,   573,   531,   639,    90,   540,   543,
     544,   561,   571,   585,   572,   711,   580,   590,   577,   578,
     597,   612,   599,   619,   595,    91,    92,    93,   592,   605,
     654,    94,    95,   628,   610,   611,   329,   609,   657,   662,
     617,   672,   687,   688,   702,   696,   694,   698,   710,   618,
     330,   702,  -161,   652,   331,   332,   661,   670,   492,   673,
       7,   678,   677,   120,   680,   679,   333,   501,   683,   689,
     691,   700,   284,   133,   334,   335,   336,   337,   311,   139,
     516,   517,   391,   405,   520,   676,   474,    73,   591,   419,
     338,    74,    75,   660,   633,   339,   705,   387,   340,   243,
     388,    76,   541,   389,   514,   622,   620,   306,   682,    77,
      78,   393,    27,   550,   489,   -53,   583,   341,   555,   686,
     692,    79,   342,   343,   557,   632,   524,   659,   709,     0,
      80,    81,    82,     0,   569,     0,     0,     0,   416,    83,
      84,     0,     0,    85,    86,     0,    87,    88,    89,   550,
       0,     0,   587,    90,     0,     0,    41,    42,     0,     0,
       0,     0,     0,     0,   422,     0,    43,     0,     0,     0,
      44,    91,    92,    93,    45,    46,     0,    94,    95,     0,
       0,     0,     0,     0,     0,     0,    47,     0,     0,   587,
       0,     0,     0,     0,     0,    48,    49,    50,     0,     0,
      51,     0,     0,     0,    52,    53,     0,     0,    54,    55,
       0,    56,    57,    58,     0,     0,     0,     0,    59,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    60,    61,    62,     0,
       0,     0,    63,    64,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   685
};

static const short int yycheck[] =
{
       9,   247,   194,   288,   203,   251,   167,   163,   293,   294,
      33,   172,   410,   411,    16,   209,    65,   178,   163,   175,
      25,   166,   167,   168,     5,   409,   171,    14,   313,     0,
     224,    34,     3,    37,   179,   180,     0,    17,    51,    97,
     424,    12,     7,     8,     9,    10,    11,    12,    97,   248,
      97,    22,    32,   252,   101,   254,    59,     5,    71,     5,
       6,     7,   446,    50,    69,   449,   450,    98,   229,     6,
       7,     8,     9,    10,    11,    12,    13,   100,   223,    97,
     225,    68,     4,   101,   369,    15,   242,    74,    90,   473,
      70,   376,   476,    97,   379,    43,    44,   242,   292,    97,
     385,   561,    97,   101,   105,   102,   101,   105,   102,   106,
     101,    59,   106,    98,    79,   100,    64,   577,   578,     5,
       6,     7,   368,    53,    54,    18,   282,    57,    58,   513,
      60,    61,    62,     7,     8,    98,   292,   100,   384,    98,
     425,   100,    98,   105,   100,   430,    98,   292,   100,    98,
     121,   100,   351,    98,   439,   100,    98,    98,   100,   100,
      97,     7,    98,   448,   100,    98,     6,   100,   329,    31,
     331,   332,   333,   334,   335,    98,   337,   100,     6,     7,
     341,   342,     7,     8,     9,    10,    11,    12,     6,     7,
     102,   101,    39,     7,    99,   102,    99,    98,   207,     4,
      29,    61,    87,    62,    58,    54,    15,     5,   454,    60,
     495,    53,    57,   498,    48,    59,    87,     4,    84,    36,
      63,    19,    20,    80,    82,    23,    24,   625,   626,    56,
      63,     4,    80,    29,   480,    30,    75,    35,     5,    36,
     401,   402,    97,    81,   253,    43,    44,    45,    46,   258,
       6,   101,    19,   105,    80,    13,    23,    24,    80,    80,
     101,    59,    97,     6,   549,   464,    64,    97,    35,    67,
     101,     6,    79,    97,    97,     5,    43,    44,    45,    46,
      78,    97,    97,     6,    28,    98,     7,    85,    86,   101,
       7,     5,    59,    91,    92,    27,    89,    64,     6,   584,
      67,    47,    27,    80,    94,   103,     3,   101,   554,    80,
      98,    78,   104,   104,    27,   101,   508,   100,    13,    86,
      27,    27,     6,    98,    91,    92,    27,    97,    81,   101,
      72,    36,   101,    37,    37,     6,   103,     7,    27,    76,
       7,    27,    73,   102,    66,     4,     4,   102,   102,   595,
     102,     1,    97,    80,    52,     5,     6,    97,     7,    97,
       4,     4,     4,    27,     5,    15,   612,     4,    97,   378,
      98,   617,   657,    23,    24,    21,    42,    97,    97,    29,
      98,    97,   628,    83,    49,    35,    98,   672,    84,   635,
       5,     4,    40,   585,    44,    45,    46,    27,     6,    98,
      40,    27,    41,    53,    54,    98,   652,    57,    58,    97,
      60,    61,    62,   605,   101,    98,   662,    67,    98,    97,
      97,    97,    97,    84,    98,   710,    88,     7,    97,    97,
      26,    38,    77,     4,    97,    85,    86,    87,    98,    96,
       4,    91,    92,    95,    98,    98,     5,   102,    27,   100,
      97,    27,     7,     6,   700,     6,    14,    22,    27,    97,
      19,   707,    98,    97,    23,    24,    98,    98,   477,    97,
       3,    98,   101,    66,    98,   100,    35,   486,    98,    98,
     102,    97,   233,    97,    43,    44,    45,    46,   497,   122,
     499,   500,   324,   349,   503,   662,   454,     1,   575,   372,
      59,     5,     6,   635,   612,    64,   701,   318,    67,   190,
     320,    15,   521,   321,   497,   601,   599,   251,   668,    23,
      24,   327,    22,   532,   471,    29,   568,    86,   536,   675,
     690,    35,    91,    92,   543,   608,   506,   630,   707,    -1,
      44,    45,    46,    -1,   553,    -1,    -1,    -1,   368,    53,
      54,    -1,    -1,    57,    58,    -1,    60,    61,    62,   568,
      -1,    -1,   571,    67,    -1,    -1,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,   378,    -1,    15,    -1,    -1,    -1,
      19,    85,    86,    87,    23,    24,    -1,    91,    92,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,   608,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    -1,    -1,
      49,    -1,    -1,    -1,    53,    54,    -1,    -1,    57,    58,
      -1,    60,    61,    62,    -1,    -1,    -1,    -1,    67,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    85,    86,    87,    -1,
      -1,    -1,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   673
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned short int yystos[] =
{
       0,     5,   108,   109,   110,   126,     0,   110,   111,    97,
     112,   269,   270,    25,    69,   113,    98,     5,     6,     7,
     126,   136,   271,   272,     4,   101,   105,   272,    18,     7,
       6,    31,   116,   102,   101,   117,    39,   114,   115,     7,
      99,     5,     6,    15,    19,    23,    24,    35,    44,    45,
      46,    49,    53,    54,    57,    58,    60,    61,    62,    67,
      85,    86,    87,    91,    92,   118,   119,   120,   121,   122,
     123,   124,   125,     1,     5,     6,    15,    23,    24,    35,
      44,    45,    46,    53,    54,    57,    58,    60,    61,    62,
      67,    85,    86,    87,    91,    92,   127,   128,   129,   130,
     133,   136,   137,   139,   141,   142,   143,   144,   145,   159,
     163,   167,   193,   197,   276,   280,   284,   307,   102,    99,
     120,    33,   100,    98,   160,   164,   194,   198,   277,   281,
     285,   308,    29,   129,   131,   138,   168,   140,   126,   122,
      61,    62,    58,    54,    60,    57,    53,    15,    48,    59,
      87,     4,   161,   165,   195,   199,   278,   282,   286,   309,
     132,    36,   169,     5,    19,    20,    23,    24,    35,    43,
      44,    45,    46,    59,    64,    67,    78,    85,    86,    91,
      92,   103,   126,   134,   146,   149,   150,   151,   154,   210,
     211,   214,   222,    80,    84,    63,   260,    82,   206,    63,
     261,    56,   264,    80,    75,    29,     4,    30,   101,   218,
     225,   226,    97,   135,    97,   224,   225,   226,   230,   224,
     225,   101,   225,   215,   216,   217,   225,   224,   225,    36,
      81,   101,   226,    65,    97,   147,   224,   225,   225,    16,
      90,   105,     5,   214,     6,   235,   154,    97,    80,    97,
     200,    97,    80,    97,    80,   235,    13,   267,    97,   269,
       7,     8,     9,    10,    11,    12,    79,   227,   228,   229,
     230,     6,   156,   157,    98,     6,   231,   232,   225,   230,
     225,   224,   219,     5,   150,     6,   152,   153,    28,   237,
       7,     7,     5,    27,    89,   238,   253,   262,   263,   269,
     235,     6,   207,   208,   209,    47,   262,   235,   254,   265,
     266,   269,   235,    27,    80,   269,    94,   170,   101,   102,
     106,     3,   158,    98,   100,   233,    98,   100,   226,     5,
      19,    23,    24,    35,    43,    44,    45,    46,    59,    64,
      67,    86,    91,    92,   155,   212,   221,   223,    98,   100,
     267,    80,   104,   104,   225,   226,   230,   267,   267,    14,
      50,    68,    74,   174,   175,   176,   190,    98,   100,    27,
     101,    98,   100,    13,   268,    98,    27,    98,   100,    27,
     267,     6,   236,    98,    97,    27,   173,   227,   228,   229,
     101,   157,   101,   232,   224,   224,   224,   224,   224,   224,
     224,    36,    81,   224,   224,   153,   235,    37,    37,   162,
     192,   191,    72,   177,     6,   239,   263,   267,     7,   209,
     201,   267,   266,   267,   287,    27,   171,   172,   253,   267,
      76,   255,   102,     7,     7,     8,   234,   224,   224,    27,
     255,   239,   239,   178,    73,   179,   196,   102,    66,   279,
     283,   255,   267,    98,   100,   267,     4,   102,   102,   102,
     267,     4,    97,   180,    80,   255,   267,   255,   255,    52,
     288,   289,   290,   310,   172,     7,   148,    97,   248,   253,
      97,   235,     4,   202,     4,     4,     5,   292,     4,   290,
     255,   255,   269,    98,   253,    27,   166,    97,    21,    97,
      97,   269,   291,    97,    83,   311,   312,   313,    84,    98,
      98,   267,    42,   186,   254,   267,   269,   269,    49,   293,
     269,     5,   317,     4,   313,   154,   187,   255,    98,   203,
      98,    98,    97,    34,    59,   296,   297,   298,   299,   301,
      98,   269,   314,    97,    97,    17,    32,    70,   240,    27,
     269,   294,   295,   300,   302,   298,    40,   269,     6,   188,
     189,    97,   242,   241,    41,   243,   267,    98,   100,   269,
     253,    97,    98,   101,    98,   100,   248,    97,    97,   244,
      88,   181,   204,   295,    27,    84,   303,   269,   315,   316,
       7,   189,    98,   248,   248,    97,   182,    26,   249,    77,
     256,   257,   258,   267,   154,    96,   304,    98,   100,   102,
      98,    98,    38,   245,   246,   247,   253,    97,    97,     4,
     268,   205,   258,   154,   305,    51,    71,   306,    95,   318,
     319,   320,   316,   247,    98,   100,   183,   184,   185,   253,
       6,     7,     8,     9,    10,    11,    12,    13,    97,   213,
     220,   250,    97,   259,     4,   239,   239,    27,   253,   320,
     246,    98,   100,     6,     7,   251,   252,   273,   274,   275,
      98,   253,    27,    97,   267,   321,   185,   101,    98,   100,
      98,     6,   275,    98,   267,   269,   303,     7,     6,    98,
     322,   102,   304,   323,    14,   326,     6,   327,    22,   328,
      97,   324,   253,   329,   330,   249,    98,   100,   325,   330,
      27,   267
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 1591 "parser-smi.y"
    {
        yyval.err = 0;
    ;}
    break;

  case 3:
#line 1595 "parser-smi.y"
    {
	yyval.err = 0;
    ;}
    break;

  case 4:
#line 1601 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 5:
#line 1603 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 6:
#line 1611 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_MODULE;
			    
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
			;}
    break;

  case 7:
#line 1655 "parser-smi.y"
    {
			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    checkModuleName(thisParserPtr, thisModulePtr);
			    checkModuleIdentity(thisParserPtr, thisModulePtr);
			    checkObjects(thisParserPtr, thisModulePtr);
			    checkTypes(thisParserPtr, thisModulePtr);
			    checkDefvals(thisParserPtr, thisModulePtr);
			    checkImportsUsage(thisParserPtr, thisModulePtr);
			    smiCheckTypeUsage(thisParserPtr, thisModulePtr);
			    
			    thisParserPtr->capabilitiesModulePtr = NULL;

                            yyval.err = 0;
			;}
    break;

  case 8:
#line 1673 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 9:
#line 1675 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 10:
#line 1679 "parser-smi.y"
    { ;}
    break;

  case 11:
#line 1681 "parser-smi.y"
    {
                            thisModulePtr->export.language = SMI_LANGUAGE_SPPI;
                        ;}
    break;

  case 12:
#line 1690 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 13:
#line 1692 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 14:
#line 1696 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_MODULE;

			    if ((thisModulePtr->export.language != SMI_LANGUAGE_SMIV2) &&
                                (thisModulePtr->export.language != SMI_LANGUAGE_SPPI))
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    yyval.err = 0;
			;}
    break;

  case 15:
#line 1710 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 16:
#line 1712 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language ==
                                 SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "EXPORTS");
                            
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_MODULE;

			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1065-SMI")) {
			        smiPrintError(thisParserPtr, ERR_EXPORTS);
			    }
			;}
    break;

  case 17:
#line 1729 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 18:
#line 1733 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 19:
#line 1735 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 20:
#line 1740 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 21:
#line 1742 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 22:
#line 1748 "parser-smi.y"
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
				modulePtr = loadModule(yyvsp[0].id, thisParserPtr);
			    }
			    checkImports(modulePtr, thisParserPtr);

			    if (modulePtr && !strcmp(yyvsp[0].id, "SNMPv2-SMI")) {
			        /*
				 * A module that imports a macro or
				 * type definition from SNMPv2-SMI
				 * seems to be SMIv2 style - but only if
                                 * it is not SPPI yet.
				 */
                                if (thisModulePtr->export.language != SMI_LANGUAGE_SPPI) {
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
			    }

			    smiFree(yyvsp[0].id);
			;}
    break;

  case 23:
#line 1791 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 24:
#line 1794 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 25:
#line 1801 "parser-smi.y"
    {
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;}
    break;

  case 26:
#line 1807 "parser-smi.y"
    {
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;}
    break;

  case 27:
#line 1813 "parser-smi.y"
    {
			    addImport(smiStrdup(yyvsp[0].id), thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = 0;
			;}
    break;

  case 28:
#line 1829 "parser-smi.y"
    {
                            /*
                             * There are PIBs that import e.g. Counter64 - so
                             * don't complain here about SMI keywords.
                             */
                           /* if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                              smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, $1);*/
                            yyval.id = yyvsp[0].id;
                        ;}
    break;

  case 29:
#line 1839 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                              smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, yyvsp[0].id);
                            yyval.id = yyvsp[0].id;
                        ;}
    break;

  case 52:
#line 1873 "parser-smi.y"
    {
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    yyval.id = yyvsp[0].id;
			;}
    break;

  case 53:
#line 1885 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 54:
#line 1887 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 55:
#line 1892 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 56:
#line 1894 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 57:
#line 1898 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 58:
#line 1903 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 59:
#line 1908 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 60:
#line 1913 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 61:
#line 1918 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 62:
#line 1923 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 63:
#line 1928 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 64:
#line 1933 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 65:
#line 1938 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 66:
#line 1943 "parser-smi.y"
    {
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 67:
#line 1948 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 68:
#line 1953 "parser-smi.y"
    { 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;}
    break;

  case 69:
#line 1958 "parser-smi.y"
    {
			    smiPrintError(thisParserPtr,
					  ERR_FLUSH_DECLARATION);
			    yyerrok;
			    yyval.err = 1;
			;}
    break;

  case 70:
#line 1972 "parser-smi.y"
    {
			    Macro *macroPtr;

			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_MACRO;

			    macroPtr = addMacro(smiStrdup(yyvsp[0].id),
						0, thisParserPtr);
			    setMacroLine(macroPtr, thisParserPtr->firstStatementLine,
					 thisParserPtr);
			;}
    break;

  case 71:
#line 1984 "parser-smi.y"
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
				       "RFC1155-SMI") &&
                                strcmp(thisParserPtr->modulePtr->export.name,
                                       "COPS-PR-SPPI") &&
                                strcmp(thisParserPtr->modulePtr->export.name,
                                       "COPS-PR-SPPI-TC")) {
			        smiPrintError(thisParserPtr, ERR_MACRO);
			    }
			;}
    break;

  case 72:
#line 2012 "parser-smi.y"
    {
			    yyval.err = 0;
                        ;}
    break;

  case 73:
#line 2017 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 74:
#line 2018 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 75:
#line 2019 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 76:
#line 2020 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 77:
#line 2021 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 78:
#line 2022 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 79:
#line 2023 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 80:
#line 2024 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 81:
#line 2025 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 82:
#line 2026 "parser-smi.y"
    { yyval.id = yyvsp[0].id; ;}
    break;

  case 83:
#line 2030 "parser-smi.y"
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
				       "RFC1155-SMI") && 
                                strcmp(thisParserPtr->modulePtr->export.name,
                                       "COPS-PR-SPPI")) {
			        smiPrintError(thisParserPtr, ERR_CHOICE);
			    }
			;}
    break;

  case 84:
#line 2050 "parser-smi.y"
    {
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			;}
    break;

  case 85:
#line 2060 "parser-smi.y"
    {
			  yyval.id = yyvsp[0].id;
			;}
    break;

  case 86:
#line 2065 "parser-smi.y"
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
			;}
    break;

  case 87:
#line 2080 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_VALUEASSIGNMENT;

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
			;}
    break;

  case 88:
#line 2101 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-7].id, &objectPtr);
			    objectPtr = setObjectName(objectPtr, yyvsp[-7].id, thisParserPtr);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
					  thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_VALUEASSIGNMENT);
			    yyval.err = 0;
			;}
    break;

  case 89:
#line 2120 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_TYPEASSIGNMENT;

			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			;}
    break;

  case 90:
#line 2128 "parser-smi.y"
    {
			    Type *typePtr;
			    
			    if (strlen(yyvsp[-3].id)) {
				if (yyvsp[0].typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
				    smiCheckTypeName(thisParserPtr,
						     thisModulePtr, yyvsp[-3].id,
						     thisParserPtr->firstStatementLine);
				}
				setTypeLine(yyvsp[0].typePtr, thisParserPtr->firstStatementLine,
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
			    if (thisModulePtr &&
				(!strcmp(thisModulePtr->export.name, "COPS-PR-SPPI"))) {
				if (!strcmp(yyvsp[-3].id, "Unsigned32")) {
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
				} else if (!strcmp(yyvsp[-3].id, "Unsigned64")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_UNSIGNED64;
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned64 = 0;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned64 = LIBSMI_UINT64_MAX;
				    }
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeUnsigned64Ptr);
				} else if (!strcmp(yyvsp[-3].id, "Integer64")) {
				    yyvsp[0].typePtr->export.basetype = SMI_BASETYPE_INTEGER64;
				    if (yyvsp[0].typePtr->listPtr) {
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.basetype = SMI_BASETYPE_INTEGER64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.integer64 = LIBSMI_INT64_MIN;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_INTEGER64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.integer64 = LIBSMI_INT64_MAX;
				    }
				    setTypeParent(yyvsp[0].typePtr, smiHandle->typeInteger64Ptr);
				}
			    }
			;}
    break;

  case 91:
#line 2286 "parser-smi.y"
    {
			    yyval.id = yyvsp[0].id;
			;}
    break;

  case 92:
#line 2290 "parser-smi.y"
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
				       "RFC1155-SMI") &&
				strcmp(thisParserPtr->modulePtr->export.name,
				       "COPS-PR-SPPI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_SMI, yyvsp[0].id);
			    }
			;}
    break;

  case 93:
#line 2314 "parser-smi.y"
    {
			    yyval.id = smiStrdup(yyvsp[0].id);
			    /*
			     * well known types (keywords in this grammar)
			     * are known to be defined in these modules.
			     */
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "COPS-PR-SPPI"))
			        smiPrintError(thisParserPtr, ERR_TYPE_SMI, yyvsp[0].id);
                        ;}
    break;

  case 95:
#line 2328 "parser-smi.y"
    {
                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName(yyvsp[0].id, thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, yyvsp[0].id);
                            yyval.id = yyvsp[0].id;
                        ;}
    break;

  case 106:
#line 2353 "parser-smi.y"
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
			;}
    break;

  case 107:
#line 2369 "parser-smi.y"
    {
			    Import *importPtr;

			    thisParserPtr->currentDecl = SMI_DECL_TEXTUALCONVENTION;

			    if (thisModulePtr->export.language == SMI_LANGUAGE_UNKNOWN)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV2;

			    if (strcmp(thisModulePtr->export.name, "SNMPv2-TC")) {
				importPtr =
				    findImportByName("TEXTUAL-CONVENTION",
						     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				} else {
                                    if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
				        smiPrintError(thisParserPtr,
						      ERR_MACRO_NOT_IMPORTED,
						      "TEXTUAL-CONVENTION",
						      "SNMPv2-TC");
                                    else
				        smiPrintError(thisParserPtr,
						      ERR_MACRO_NOT_IMPORTED,
						      "TEXTUAL-CONVENTION",
						      "COPS-PR-SPPI");
				}
			    }
			;}
    break;

  case 108:
#line 2400 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 109:
#line 2405 "parser-smi.y"
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
				if (smiCheckFormat(thisParserPtr,
						   yyval.typePtr->export.basetype, yyvsp[-8].text,
						   thisParserPtr->firstStatementLine)) {
				    setTypeFormat(yyval.typePtr, yyvsp[-8].text);
				}
			    }
			    setTypeDecl(yyval.typePtr, SMI_DECL_TEXTUALCONVENTION);
			;}
    break;

  case 110:
#line 2437 "parser-smi.y"
    {
			    yyval.typePtr = yyvsp[0].typePtr;
			    setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			;}
    break;

  case 111:
#line 2445 "parser-smi.y"
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
			;}
    break;

  case 112:
#line 2464 "parser-smi.y"
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
			;}
    break;

  case 113:
#line 2504 "parser-smi.y"
    {
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[-1].listPtr);
			;}
    break;

  case 114:
#line 2512 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 115:
#line 2519 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 116:
#line 2538 "parser-smi.y"
    {
			    Object *objectPtr;
			    Import *importPtr;
			    Type *typePtr;
			    
			    objectPtr =
			        findObjectByModuleAndName(thisParserPtr->modulePtr,
							  yyvsp[-1].id);

			    if (!objectPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr ||
				    (importPtr->kind == KIND_NOTFOUND)) {
				    objectPtr = addObject(yyvsp[-1].id, thisParserPtr->pendingNodePtr,
					                  0,
					                  FLAG_INCOMPLETE |
							  FLAG_SEQTYPE,
						          thisParserPtr);
				    setObjectType(objectPtr, yyvsp[0].typePtr);
				} else {
				    /*
				     * imported object.
				     */
				    importPtr->use++;
				    objectPtr = findObjectByModulenameAndName(
					importPtr->export.module, yyvsp[-1].id);
				    smiFree(yyvsp[-1].id);

				    if (objectPtr->typePtr->export.name) {
					typePtr = objectPtr->typePtr;
				    } else {
					typePtr = objectPtr->typePtr->parentPtr;
				    }
				    if ((yyvsp[0].typePtr != typePtr) &&
					((yyvsp[0].typePtr->export.basetype !=
					  SMI_BASETYPE_INTEGER32) ||
					 (typePtr->export.basetype !=
					  SMI_BASETYPE_ENUM)) &&
					((yyvsp[0].typePtr->export.basetype !=
					  SMI_BASETYPE_OCTETSTRING) ||
					 (typePtr->export.basetype !=
					  SMI_BASETYPE_BITS))) {
					smiPrintError(thisParserPtr,
						      ERR_SEQUENCE_TYPE_MISMATCH,
						      objectPtr->export.name);
				    }
				}
			    } else {
				smiFree(yyvsp[-1].id);
				if (objectPtr->typePtr) {

				    if (objectPtr->typePtr->export.name) {
					typePtr = objectPtr->typePtr;
				    } else {
					typePtr = objectPtr->typePtr->parentPtr;
				    }
				    if ((yyvsp[0].typePtr != typePtr) &&
					((yyvsp[0].typePtr->export.basetype !=
					  SMI_BASETYPE_INTEGER32) ||
					 (typePtr->export.basetype !=
					  SMI_BASETYPE_ENUM)) &&
					((yyvsp[0].typePtr->export.basetype !=
					  SMI_BASETYPE_OCTETSTRING) ||
					 (typePtr->export.basetype !=
					  SMI_BASETYPE_BITS))) {
					smiPrintError(thisParserPtr,
						      ERR_SEQUENCE_TYPE_MISMATCH,
						      objectPtr->export.name);
				    }

				} else {
				    setObjectType(objectPtr, yyvsp[0].typePtr);
				    addObjectFlags(objectPtr,
						   FLAG_SEQTYPE);
				}
			    }

			    yyval.objectPtr = objectPtr;
			;}
    break;

  case 117:
#line 2621 "parser-smi.y"
    {
			    yyval.typePtr = yyvsp[0].typePtr;
			    if (yyval.typePtr)
				defaultBasetype = yyval.typePtr->export.basetype;
			;}
    break;

  case 118:
#line 2629 "parser-smi.y"
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
			    smiCheckNamedNumbersOrder(parserPtr, typePtr);
			    yyval.typePtr = typePtr;
			;}
    break;

  case 119:
#line 2649 "parser-smi.y"
    {
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 120:
#line 2653 "parser-smi.y"
    {
			    /* TODO: */
			    yyval.typePtr = smiHandle->typeOctetStringPtr;
			;}
    break;

  case 121:
#line 2658 "parser-smi.y"
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
				    importPtr->use++;
				    yyval.typePtr = findTypeByModulenameAndName(
					importPtr->export.module,
					importPtr->export.name);
				    smiFree(yyvsp[-1].id);
				}
			    } else {
				smiFree(yyvsp[-1].id);
			    }
			;}
    break;

  case 122:
#line 2691 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 123:
#line 2697 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 124:
#line 2710 "parser-smi.y"
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
			;}
    break;

  case 125:
#line 2723 "parser-smi.y"
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
			;}
    break;

  case 126:
#line 2745 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_OBJECTIDENTITY;

			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 127:
#line 2755 "parser-smi.y"
    {
			    Import *importPtr;

			    if (strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
                                strcmp(thisModulePtr->export.name, "COPS-PR-SPPI")) {
				importPtr = findImportByName("OBJECT-IDENTITY",
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				} else {
                                    if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
				        smiPrintError(thisParserPtr,
						      ERR_MACRO_NOT_IMPORTED,
						      "OBJECT-IDENTITY",
						      "SNMPv2-SMI");
                                    else
				        smiPrintError(thisParserPtr,
						      ERR_MACRO_NOT_IMPORTED,
						      "OBJECT-IDENTITY",
						      "COPS-PR-SPPI");
				}
			    }
			;}
    break;

  case 128:
#line 2780 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 129:
#line 2786 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-13].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-13].id, thisParserPtr);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTIDENTITY);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text, thisParserPtr);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text, thisParserPtr);
			    }
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    yyval.err = 0;
			;}
    break;

  case 130:
#line 2808 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_OBJECTTYPE;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 131:
#line 2818 "parser-smi.y"
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
				} else if (thisModulePtr->export.language ==
                                    SMI_LANGUAGE_SPPI) {
                                    smiPrintError(thisParserPtr,
                                                  ERR_MACRO_NOT_IMPORTED,
                                                  "OBJECT-TYPE", "COPS-PR-SPPI");
                                } else {
				    smiPrintError(thisParserPtr,
						  ERR_MACRO_NOT_IMPORTED,
						  "OBJECT-TYPE", "RFC-1212");
				}
			    }
                            indexFlag = 0;
			;}
    break;

  case 132:
#line 2858 "parser-smi.y"
    {
			    Object *objectPtr, *parentPtr;
			    Type *typePtr = NULL;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-22].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-22].id, thisParserPtr);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTTYPE);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
					  thisParserPtr);
			    if (checkObjectFlags(objectPtr, FLAG_SEQTYPE)) {
				deleteObjectFlags(objectPtr, FLAG_SEQTYPE);
				if (yyvsp[-17].typePtr) {
				    if (yyvsp[-17].typePtr->export.name) {
					typePtr = yyvsp[-17].typePtr;
                                        /*
                                         * According to RFC 3159 7.1.3. Opaque must not be used
                                         * in a SYNTAX clause.
                                         */
                                        if ((thisModulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                            !strcmp(typePtr->export.name, "Opaque"))
                                            smiPrintError(thisParserPtr, ERR_OPAQUE_IN_SYNTAX);
                                        /*
                                         * According to RFC 3159 7.1.4. IpAddress must not be used
                                         * in a SYNTAX clause.
                                         */
                                        if ((thisModulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                            !strcmp(typePtr->export.name, "IpAddress"))
                                            smiPrintError(thisParserPtr, ERR_IPADDRESS_IN_SYNTAX);
				    } else {
					typePtr = yyvsp[-17].typePtr->parentPtr;
				    }
				    if ((objectPtr->typePtr != typePtr) &&
					((objectPtr->typePtr->export.basetype !=
					  SMI_BASETYPE_INTEGER32) ||
					 (typePtr->export.basetype !=
					  SMI_BASETYPE_ENUM)) &&
					((objectPtr->typePtr->export.basetype !=
					  SMI_BASETYPE_OCTETSTRING) ||
					 (typePtr->export.basetype !=
					  SMI_BASETYPE_BITS))) {
					smiPrintError(thisParserPtr,
						      ERR_SEQUENCE_TYPE_MISMATCH,
						      objectPtr->export.name);
				    }
				}
			    }
			    setObjectType(objectPtr, yyvsp[-17].typePtr);
			    if (!(yyvsp[-17].typePtr->export.name)) {
				/*
				 * An inlined type.
				 */
#if 0 /* export implicitly defined types by the node's lowercase name */
				setTypeName(yyvsp[-17].typePtr, yyvsp[-22].id);
#endif
			    }
			    setObjectUnits(objectPtr, yyvsp[-16].text);
			    setObjectAccess(objectPtr, yyvsp[-15].access);
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
			    setObjectStatus(objectPtr, yyvsp[-11].status);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    if (yyvsp[-10].text) {
				setObjectDescription(objectPtr, yyvsp[-10].text, thisParserPtr);
			    }
			    if (yyvsp[-8].text) {
				setObjectReference(objectPtr, yyvsp[-8].text, thisParserPtr);
			    }
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI) {
                                /*
                                 * For SMI documents either $16 (IndexPart) or $17 (MibIndex)
                                 * are used, but not both. This is signalled via the indexFlag
                                 * which is 1 if IndexPart has been used.
                                 */
                                if (indexFlag == INDEXFLAG_AUGMENTS) { /* IndexPart was used */
			            if (yyvsp[-7].index.indexkind != SMI_INDEX_UNKNOWN) {
				        setObjectList(objectPtr, yyvsp[-7].index.listPtr);
				        setObjectImplied(objectPtr, yyvsp[-7].index.implied);
				        setObjectIndexkind(objectPtr, yyvsp[-7].index.indexkind);
				        setObjectRelated(objectPtr, yyvsp[-7].index.rowPtr);
			            }
                                } else {
			            if (yyvsp[-6].index.indexkind != SMI_INDEX_UNKNOWN) {
				        setObjectList(objectPtr, yyvsp[-6].index.listPtr);
				        setObjectImplied(objectPtr, yyvsp[-6].index.implied);
				        setObjectIndexkind(objectPtr, yyvsp[-6].index.indexkind);
				        setObjectRelated(objectPtr, yyvsp[-6].index.rowPtr);
			            }
                                }
                            } else {
                                /*
                                 * PIBs contain either PIB-INDEX or AUGMENTS or EXTENDS - 
                                 * but only with one Index entry. A PIB-INDEX may be
                                 * followed by a full INDEX. We get the indexkind
                                 * from the first.
                                 * Note that PIB-INDEX/AUGMENTS/EXTENS is always
                                 * the first element in objectPtr->listPtr.
                                 * If an optional INDEX exists then it is
                                 * appended to this list.
                                 */
			        if (yyvsp[-7].index.indexkind != SMI_INDEX_UNKNOWN) {
                                    setObjectList(objectPtr, yyvsp[-7].index.listPtr);
				    setObjectIndexkind(objectPtr, yyvsp[-7].index.indexkind);
				    setObjectRelated(objectPtr, yyvsp[-7].index.rowPtr);
			        }
			        if (yyvsp[-6].index.indexkind != SMI_INDEX_UNKNOWN) {
                                    if (objectPtr->listPtr) {
                                        List *p;
                                        for (p = objectPtr->listPtr; p->nextPtr;
                                             p = p->nextPtr);
                                        p->nextPtr = yyvsp[-6].index.listPtr;
                                    }
				    setObjectImplied(objectPtr, yyvsp[-6].index.implied);
			        }
                            }
                            if (yyvsp[-5].listPtr) {
                                setObjectUniqueness(objectPtr, yyvsp[-5].listPtr);
                            }
			    if (yyvsp[-4].valuePtr) {
				if (objectPtr->typePtr
				    && (((objectPtr->typePtr->export.basetype == SMI_BASETYPE_OCTETSTRING) &&
					 (yyvsp[-4].valuePtr->basetype != SMI_BASETYPE_OCTETSTRING))
					|| ((objectPtr->typePtr->export.basetype == SMI_BASETYPE_OBJECTIDENTIFIER) &&
					    (yyvsp[-4].valuePtr->basetype != SMI_BASETYPE_OBJECTIDENTIFIER)))) {
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
                            if (yyvsp[-14].objectPtr) {
                                if (objectPtr->relatedPtr)
                                    smiPrintError(thisParserPtr, ERR_OBJECTPTR_ELEMENT_IN_USE,
                                                  "relatedPtr", "PIB-REFERENCES");
                                /*
                                 * PIB-REFERENCES clauses are only allowed for
                                 * objects with a SYNTAX of 'ReferenceId'.
                                 * See RFC 3159 7.10
                                 */
                                if (objectPtr->typePtr && objectPtr->typePtr->export.name &&
                                    strcmp(objectPtr->typePtr->export.name, "ReferenceId"))
                                    smiPrintErrorAtLine(parserPtr, ERR_PIB_REFERENCES_WRONG_TYPE,
                                                        objectPtr->line);
                                else
                                    setObjectRelated(objectPtr, yyvsp[-14].objectPtr);
                            } else {
                               /*
                                * Does this object have a SYNTAX of 'ReferenceId'
                                * and a PIB-REFERENCES clause?
                                * See RFC 3159 7.10
                                */
                               if ((thisModulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                   objectPtr->typePtr && objectPtr->typePtr->export.name &&
                                   !strcmp(objectPtr->typePtr->export.name, "ReferenceId"))
                                   smiPrintErrorAtLine(parserPtr, ERR_LACKING_PIB_REFERENCES,
                                                       objectPtr->line);
                            }
                            if (yyvsp[-13].objectPtr) {
                                if (objectPtr->relatedPtr)
                                    smiPrintError(thisParserPtr, ERR_OBJECTPTR_ELEMENT_IN_USE,
                                                  "relatedPtr", "PIB-TAG");
                                /*
                                 * PIB-TAG clauses are only allowed for
                                 * objects with a SYNTAX of 'TagReferenceId'.
                                 * See RFC 3159 7.11
                                 */
                                if (objectPtr->typePtr && objectPtr->typePtr->export.name &&
                                    strcmp(objectPtr->typePtr->export.name, "TagReferenceId"))
                                    smiPrintErrorAtLine(parserPtr, ERR_PIB_TAG_WRONG_TYPE,
                                                        objectPtr->line);
                                else
                                    setObjectRelated(objectPtr, yyvsp[-13].objectPtr);
                            } else {
                                /*
                                 * Does this object have a SYNTAX of 'TagReferenceId'
                                 * and a PIB-TAG clause?
                                 * See RFC 3159 7.11
                                 */
                                if ((thisModulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                    objectPtr->typePtr && objectPtr->typePtr->export.name &&
                                    !strcmp(objectPtr->typePtr->export.name, "TagReferenceId"))
                                    smiPrintErrorAtLine(parserPtr, ERR_LACKING_PIB_TAG,
                                                        objectPtr->line);

                            }
                            if (yyvsp[-9].listPtr) {
                                if (objectPtr->listPtr)
                                    smiPrintError(thisParserPtr, ERR_OBJECTPTR_ELEMENT_IN_USE,
                                                  "listPtr", "INSTALL-ERRORS");
                                /*
                                 * Are INSTALL-ERRORS only used with tables?
                                 * See RFC 3159 7.4
                                 */
                                if (!((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
                                    (objectPtr->typePtr) &&
                                    (objectPtr->typePtr->export.decl == SMI_DECL_IMPL_SEQUENCEOF)))
                                    smiPrintErrorAtLine(parserPtr, ERR_INSTALL_ERRORS_FOR_NON_TABLE,
                                                        objectPtr->line);
                                else
                                    setObjectList(objectPtr, yyvsp[-9].listPtr);
                            }
			    yyval.err = 0;
			;}
    break;

  case 133:
#line 3114 "parser-smi.y"
    {
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2) ||
                                (thisModulePtr->export.language == SMI_LANGUAGE_SPPI))
			    {
				smiPrintError(thisParserPtr,
					      ERR_MISSING_DESCRIPTION);
			    }
			    yyval.text = NULL;
			;}
    break;

  case 134:
#line 3124 "parser-smi.y"
    {
			    yyval.text = yyvsp[0].text;
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 135:
#line 3131 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_TRAPTYPE;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 136:
#line 3141 "parser-smi.y"
    {
			    Import *importPtr;
                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "TRAP-TYPE");
			    
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
			;}
    break;

  case 137:
#line 3167 "parser-smi.y"
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

			    objectPtr = setObjectName(objectPtr, yyvsp[-10].id, thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_TRAPTYPE);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
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
			;}
    break;

  case 138:
#line 3214 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-1].listPtr;
			;}
    break;

  case 139:
#line 3218 "parser-smi.y"
    {
			    yyval.listPtr = NULL;
			;}
    break;

  case 140:
#line 3224 "parser-smi.y"
    {
			    if (yyvsp[0].objectPtr) {
				yyval.listPtr = smiMalloc(sizeof(List));
				yyval.listPtr->ptr = yyvsp[0].objectPtr;
				yyval.listPtr->nextPtr = NULL;
			    } else {
				yyval.listPtr = NULL;
			    }
			;}
    break;

  case 141:
#line 3234 "parser-smi.y"
    {
			    List *p, *pp;

			    if (yyvsp[0].objectPtr) {
				p = smiMalloc(sizeof(List));
				p->ptr = yyvsp[0].objectPtr;
				p->nextPtr = NULL;
				if (yyvsp[-2].listPtr) {
				    for (pp = yyvsp[-2].listPtr; pp->nextPtr;
					 pp = pp->nextPtr);
				    pp->nextPtr = p;
				    yyval.listPtr = yyvsp[-2].listPtr;
				} else {
				    yyval.listPtr = p;
				}
			    } else {
				yyval.listPtr = yyvsp[-2].listPtr;
			    }
			;}
    break;

  case 142:
#line 3256 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 143:
#line 3262 "parser-smi.y"
    {
			    yyval.text = yyvsp[0].text;
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 144:
#line 3267 "parser-smi.y"
    { yyval.text = NULL; ;}
    break;

  case 145:
#line 3271 "parser-smi.y"
    {
                            yyval.access = yyvsp[0].access;
                        ;}
    break;

  case 146:
#line 3275 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "MAX-ACCESS");
                            if (yyvsp[0].access == SMI_ACCESS_NOT_ACCESSIBLE)
                                smiPrintError(thisParserPtr, ERR_NOT_ACCESSIBLE_IN_PIB_ACCESS);
                            yyval.access = yyvsp[0].access;
                        ;}
    break;

  case 147:
#line 3283 "parser-smi.y"
    { yyval.access = 0;  ;}
    break;

  case 148:
#line 3287 "parser-smi.y"
    { yyval.access = yyvsp[0].access; ;}
    break;

  case 149:
#line 3291 "parser-smi.y"
    { 
                            smiPrintError(thisParserPtr, ERR_POLICY_ACCESS_IN_PIB);
                        ;}
    break;

  case 150:
#line 3295 "parser-smi.y"
    { ;}
    break;

  case 151:
#line 3299 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "PIB-REFERENCES");
                        ;}
    break;

  case 152:
#line 3304 "parser-smi.y"
    { yyval.objectPtr = yyvsp[-1].objectPtr; ;}
    break;

  case 153:
#line 3306 "parser-smi.y"
    { yyval.objectPtr = 0; ;}
    break;

  case 154:
#line 3310 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "PIB-TAG");
                        ;}
    break;

  case 155:
#line 3315 "parser-smi.y"
    { yyval.objectPtr = yyvsp[-1].objectPtr; ;}
    break;

  case 156:
#line 3317 "parser-smi.y"
    { yyval.objectPtr = 0; ;}
    break;

  case 157:
#line 3322 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "UNIQUENESS");
                        ;}
    break;

  case 158:
#line 3327 "parser-smi.y"
    { yyval.listPtr = yyvsp[-1].listPtr; ;}
    break;

  case 159:
#line 3329 "parser-smi.y"
    { yyval.listPtr = NULL; ;}
    break;

  case 160:
#line 3333 "parser-smi.y"
    { yyval.listPtr = yyvsp[0].listPtr; ;}
    break;

  case 161:
#line 3335 "parser-smi.y"
    { yyval.listPtr = NULL; ;}
    break;

  case 162:
#line 3339 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 163:
#line 3346 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
                        ;}
    break;

  case 164:
#line 3359 "parser-smi.y"
    { yyval.objectPtr = yyvsp[0].objectPtr; ;}
    break;

  case 165:
#line 3363 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "INSTALL-ERRORS");
                        ;}
    break;

  case 166:
#line 3368 "parser-smi.y"
    { yyval.listPtr = yyvsp[-1].listPtr; ;}
    break;

  case 167:
#line 3370 "parser-smi.y"
    { yyval.listPtr = NULL; ;}
    break;

  case 168:
#line 3374 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 169:
#line 3381 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
                        ;}
    break;

  case 170:
#line 3394 "parser-smi.y"
    {
			    Object *objectPtr;
			    
                            if ((yyvsp[-1].unsigned32 < 1) || (yyvsp[-1].unsigned32 > 65536))
                                smiPrintError(thisParserPtr, ERR_ERROR_NUMBER_RANGE, yyvsp[-1].unsigned32);
                            /*
                             * This is not a regular object that will be added vid
                             * 'addObject' as error identifier have no other
                             * meaning in PIBs. They are just used for
                             * a direct mapping to the actual protocol fields.
                             */
                            objectPtr = smiMalloc(sizeof(Object));
                            objectPtr->export.name = yyvsp[-3].id;
                            objectPtr->export.oidlen = 1;
                            objectPtr->export.oid = (void *)yyvsp[-1].unsigned32;
			    yyval.objectPtr = objectPtr;
			;}
    break;

  case 171:
#line 3415 "parser-smi.y"
    {
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        smiPrintError(thisParserPtr,
					      ERR_MAX_ACCESS_IN_SMIV1);
			    } else if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "MAX-ACCESS");
			;}
    break;

  case 172:
#line 3424 "parser-smi.y"
    { yyval.access = yyvsp[0].access; ;}
    break;

  case 173:
#line 3426 "parser-smi.y"
    {
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        smiPrintError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    } else if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "ACCESS");
			;}
    break;

  case 174:
#line 3435 "parser-smi.y"
    { yyval.access = yyvsp[0].access; ;}
    break;

  case 175:
#line 3439 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_NOTIFICATIONTYPE;

			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 176:
#line 3449 "parser-smi.y"
    {
			    Import *importPtr;

                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "NOTIFICATION-TYPE");
                                
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
			;}
    break;

  case 177:
#line 3469 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 178:
#line 3475 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-14].id, thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONTYPE);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
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
			;}
    break;

  case 179:
#line 3500 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_MODULEIDENTITY;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 180:
#line 3510 "parser-smi.y"
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
			;}
    break;

  case 181:
#line 3535 "parser-smi.y"
    {
                          /* do nothing at the moment */
                        ;}
    break;

  case 182:
#line 3539 "parser-smi.y"
    {
			    setModuleLastUpdated(thisParserPtr->modulePtr, yyvsp[0].date);
			;}
    break;

  case 183:
#line 3543 "parser-smi.y"
    {
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_ORGANIZATION);
			    }
			;}
    break;

  case 184:
#line 3550 "parser-smi.y"
    {
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_CONTACT);
			    }
			;}
    break;

  case 185:
#line 3557 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 186:
#line 3561 "parser-smi.y"
    {
			    if ((!thisModulePtr->firstRevisionPtr) ||
				(thisModulePtr->firstRevisionPtr->export.date !=
				 thisModulePtr->lastUpdated)) {
				smiPrintError(thisParserPtr,
					      ERR_REVISION_MISSING);
				addRevision(thisModulePtr->lastUpdated,
					    smiStrdup(
						"[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }
			;}
    break;

  case 187:
#line 3575 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-23].id, &objectPtr);

			    thisParserPtr->modulePtr->numModuleIdentities++;

			    objectPtr = setObjectName(objectPtr, yyvsp[-23].id, thisParserPtr);
			    setObjectDecl(objectPtr, SMI_DECL_MODULEIDENTITY);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
					  thisParserPtr);
			    setObjectStatus(objectPtr, SMI_STATUS_CURRENT);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setModuleIdentityObject(thisParserPtr->modulePtr,
						    objectPtr);
			    setModuleOrganization(thisParserPtr->modulePtr,
						  yyvsp[-13].text);
			    setModuleContactInfo(thisParserPtr->modulePtr,
						 yyvsp[-10].text);
			    setModuleDescription(thisParserPtr->modulePtr,
						 yyvsp[-7].text, thisParserPtr);
                            if (yyvsp[-19].subjectCategoriesPtr != NULL) {
                                setObjectList(objectPtr, yyvsp[-19].subjectCategoriesPtr->categories);
                                smiFree(yyvsp[-19].subjectCategoriesPtr);
                            }
			    /* setObjectDescription(objectPtr, $15); */
			    yyval.err = 0;
			;}
    break;

  case 188:
#line 3608 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "SUBJECT-CATEGORIES");
                            yyval.subjectCategoriesPtr = yyvsp[-1].subjectCategoriesPtr;
                        ;}
    break;

  case 189:
#line 3614 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SUBJECT_CATEGORIES_MISSING);
                            yyval.subjectCategoriesPtr = NULL;
                        ;}
    break;

  case 190:
#line 3622 "parser-smi.y"
    {
                            yyval.subjectCategoriesPtr = smiMalloc(sizeof(SubjectCategories));
                            yyval.subjectCategoriesPtr->categories    = yyvsp[0].listPtr;
                        ;}
    break;

  case 191:
#line 3629 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 192:
#line 3635 "parser-smi.y"
    {
			    List *p, *pp;
			    
                            if (yyvsp[-2].listPtr->ptr == NULL)
                                smiPrintError(thisParserPtr, ERR_SUBJECT_CATEGORIES_ALL);
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 193:
#line 3650 "parser-smi.y"
    {
                            Object *objectPtr;
                            
                            if (strcmp(yyvsp[0].id, "all"))
                                smiPrintError(thisParserPtr, ERR_SUBJECT_CATEGORIES_MISSING_SUBID);
                            else {
                                /*
                                 * This is not a regular object that will be added via
                                 * 'addObject' as subject category dentifier have no
                                 * other meaning in PIBs. They are just used for
                                 * a direct mapping to the actual protocol fields.
                                 */
                                objectPtr = smiMalloc(sizeof(Object));
                                objectPtr->export.name = "all";
                                objectPtr->export.oidlen = 0;
                                objectPtr->export.oid = 0;
                                yyval.objectPtr = objectPtr;
                            }
                            smiFree(yyvsp[0].id);
                        ;}
    break;

  case 194:
#line 3671 "parser-smi.y"
    {
			    Object *objectPtr;
			    
                            if (!strcmp(yyvsp[-3].id, "all")) {
                                smiPrintError(thisParserPtr, ERR_SUBJECT_CATEGORIES_ALL_WITH_SUBID);
                                yyval.objectPtr = NULL;
                            } else {
                                /*
                                 * This is not a regular object that will be added via
                                 * 'addObject' as subject category dentifier have no
                                 * other meaning in PIBs. They are just used for
                                 * a direct mapping to the actual protocol fields.
                                 */
                                objectPtr = smiMalloc(sizeof(Object));
                                objectPtr->export.name = yyvsp[-3].id;
                                objectPtr->export.oidlen = 1;
                                objectPtr->export.oid = (void *)yyvsp[-1].unsigned32;
                                yyval.objectPtr = objectPtr;
                            }
			;}
    break;

  case 195:
#line 3694 "parser-smi.y"
    {
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 196:
#line 3698 "parser-smi.y"
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
				       "RFC1155-SMI") &&
                                strcmp(thisParserPtr->modulePtr->export.name,
                                       "COPS-PR-SPPI")) {
			        smiPrintError(thisParserPtr, ERR_TYPE_TAG, yyvsp[-1].err);
			    }
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 197:
#line 3718 "parser-smi.y"
    {
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 198:
#line 3723 "parser-smi.y"
    {
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 199:
#line 3728 "parser-smi.y"
    {
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 200:
#line 3733 "parser-smi.y"
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
			;}
    break;

  case 201:
#line 3750 "parser-smi.y"
    { yyval.err = 0; /* TODO: check range */ ;}
    break;

  case 202:
#line 3752 "parser-smi.y"
    { yyval.err = 0; /* TODO: check range */ ;}
    break;

  case 203:
#line 3760 "parser-smi.y"
    { yyval.typePtr = yyvsp[0].typePtr; ;}
    break;

  case 204:
#line 3765 "parser-smi.y"
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
			;}
    break;

  case 205:
#line 3782 "parser-smi.y"
    { yyval.valuePtr = yyvsp[0].valuePtr; ;}
    break;

  case 206:
#line 3791 "parser-smi.y"
    {
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC") &&
                                 strcmp(thisModulePtr->export.name, "COPS-PR-SPPI")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    yyval.typePtr = smiHandle->typeInteger32Ptr;
			;}
    break;

  case 207:
#line 3804 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;}
    break;

  case 208:
#line 3808 "parser-smi.y"
    {
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC") &&
                                 strcmp(thisModulePtr->export.name, "COPS-PR-SPPI")))
				smiPrintError(thisParserPtr,
					      ERR_INTEGER_IN_SMIV2);

			    yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;}
    break;

  case 209:
#line 3823 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;}
    break;

  case 210:
#line 3827 "parser-smi.y"
    {
			    List *p;
			    
			    yyval.typePtr = duplicateType(smiHandle->typeEnumPtr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			    smiCheckNamedNumbersOrder(parserPtr, yyval.typePtr);
			;}
    break;

  case 211:
#line 3838 "parser-smi.y"
    {
			    Import *importPtr;

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Integer32");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Integer32");
				}
			    }

			    /* TODO: any need to distinguish from INTEGER? */
			    yyval.typePtr = smiHandle->typeInteger32Ptr;
			;}
    break;

  case 212:
#line 3864 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;}
    break;

  case 213:
#line 3868 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Integer32");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Integer32");
				}
			    }

			    yyval.typePtr = duplicateType(smiHandle->typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;}
    break;

  case 214:
#line 3895 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;}
    break;

  case 215:
#line 3899 "parser-smi.y"
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
			    smiCheckNamedNumbersOrder(parserPtr, yyval.typePtr);
			;}
    break;

  case 216:
#line 3950 "parser-smi.y"
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
			    smiCheckNamedNumbersOrder(parserPtr, yyval.typePtr);
			    smiFree(yyvsp[-3].id);
			    smiFree(yyvsp[-1].id);
			;}
    break;

  case 217:
#line 3996 "parser-smi.y"
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
			;}
    break;

  case 218:
#line 4052 "parser-smi.y"
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
			;}
    break;

  case 219:
#line 4101 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = smiHandle->typeOctetStringPtr;
			;}
    break;

  case 220:
#line 4106 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			;}
    break;

  case 221:
#line 4110 "parser-smi.y"
    {
			    
			    yyval.typePtr = duplicateType(smiHandle->typeOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			;}
    break;

  case 222:
#line 4118 "parser-smi.y"
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
			;}
    break;

  case 223:
#line 4165 "parser-smi.y"
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
			;}
    break;

  case 224:
#line 4205 "parser-smi.y"
    {
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.typePtr = smiHandle->typeObjectIdentifierPtr;
			;}
    break;

  case 225:
#line 4213 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;}
    break;

  case 226:
#line 4219 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;}
    break;

  case 227:
#line 4225 "parser-smi.y"
    {   
                            /* The scanner already checks for the language */
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = yyvsp[0].unsigned64;
			;}
    break;

  case 228:
#line 4232 "parser-smi.y"
    {
                            /* The scanner already checks for the language */
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->value.integer64 = yyvsp[0].integer64;
			;}
    break;

  case 229:
#line 4239 "parser-smi.y"
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
			;}
    break;

  case 230:
#line 4265 "parser-smi.y"
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
			;}
    break;

  case 231:
#line 4288 "parser-smi.y"
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
			;}
    break;

  case 232:
#line 4308 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.valuePtr->value.ptr = (unsigned char *)smiStrdup(yyvsp[0].text);
			    yyval.valuePtr->len = strlen(yyvsp[0].text);
			;}
    break;

  case 233:
#line 4331 "parser-smi.y"
    {
			    /*
			     * SMIv1 allows something like { 0 0 } !
			     * SMIv2 does not!
			     */
			    /* TODO: make it work correctly for SMIv1 */
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2) {
				smiPrintError(thisParserPtr,
					      ERR_OID_DEFVAL_TOO_LONG_SMIV2);
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_OID_DEFVAL_TOO_LONG_SMIV1);
			    }
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.valuePtr->len = 2;
			    yyval.valuePtr->value.oid = smiMalloc(2 * sizeof(SmiSubid));
			    yyval.valuePtr->value.oid[0] = 0;
			    yyval.valuePtr->value.oid[1] = 0;
			;}
    break;

  case 234:
#line 4358 "parser-smi.y"
    {
			    yyval.typePtr = smiHandle->typeInteger32Ptr;
			;}
    break;

  case 235:
#line 4362 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    /* TODO: any need to distinguish from INTEGER? */
			    yyval.typePtr = smiHandle->typeInteger32Ptr;

			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Integer32");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Integer32");
				}
			    }
			;}
    break;

  case 236:
#line 4387 "parser-smi.y"
    {
			    yyval.typePtr = smiHandle->typeOctetStringPtr;
			;}
    break;

  case 237:
#line 4391 "parser-smi.y"
    {
			    yyval.typePtr = smiHandle->typeObjectIdentifierPtr;
			;}
    break;

  case 238:
#line 4397 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }
			    
			    importPtr = findImportByName("IpAddress",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "IpAddress");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "IpAddress");
				}
			    }
			;}
    break;

  case 239:
#line 4425 "parser-smi.y"
    {
			    Import *importPtr;

                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Counter32", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "COUNTER32");
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			    
			    importPtr = findImportByName("Counter32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Counter32");
				}
			    }
			;}
    break;

  case 240:
#line 4451 "parser-smi.y"
    {
			    Import *importPtr;
			    List *listPtr, *nextListPtr;
			    
			    smiPrintError(thisParserPtr,
					  ERR_ILLEGAL_RANGE_FOR_COUNTER,
					  "Counter32");
			    for (listPtr = yyvsp[0].listPtr; listPtr;
				 listPtr = nextListPtr) {
				nextListPtr = listPtr->nextPtr;
				smiFree((Range *)listPtr->ptr);
				smiFree(listPtr);
			    }
			    
                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Counter32", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Counter32");
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }
			    
			    importPtr = findImportByName("Counter32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Counter32");
				}
			    }
			;}
    break;

  case 241:
#line 4488 "parser-smi.y"
    {
			    Import *importPtr;

                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Gauge32", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Gauge32");
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }
			    
			    importPtr = findImportByName("Gauge32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Gauge32");
				}
			    }
			;}
    break;

  case 242:
#line 4514 "parser-smi.y"
    {
			    Type *parentPtr;
			    Import *importPtr;
			    
                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Gauge32", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Gauge32");
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
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Gauge32");
				}
			    }
			;}
    break;

  case 243:
#line 4547 "parser-smi.y"
    {
			    Import *importPtr;

			    yyval.typePtr = smiHandle->typeUnsigned32Ptr;
			    
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Unsigned32");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Unsigned32");
				}
			    }
			;}
    break;

  case 244:
#line 4571 "parser-smi.y"
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
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Unsigned32");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Unsigned32");
				}
			    }
			;}
    break;

  case 245:
#line 4598 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }
			    
			    importPtr = findImportByName("TimeTicks",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "TimeTicks");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "TimeTicks");
				}
			    }
			;}
    break;

  case 246:
#line 4626 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("Opaque");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_OPAQUE_OBSOLETE);
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_OPAQUE_OBSOLETE);
				}
			    }
			    
			    importPtr = findImportByName("Opaque",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Opaque");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Opaque");
				}
			    }
			;}
    break;

  case 247:
#line 4664 "parser-smi.y"
    {
			    Type *parentPtr;
			    Import *importPtr;
			    
			    parentPtr = findTypeByName("Opaque");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
				yyval.typePtr = NULL;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_OPAQUE_OBSOLETE,
						  "Opaque");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_OPAQUE_OBSOLETE,
						  "Opaque");
				}
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    }

			    importPtr = findImportByName("Opaque",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Opaque");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Opaque");
				}
			    }
			;}
    break;

  case 248:
#line 4710 "parser-smi.y"
    {
			    Import *importPtr;

                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Counter64", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Counter64");
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
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Counter64");
				}
			    }
			;}
    break;

  case 249:
#line 4736 "parser-smi.y"
    {
			    Import *importPtr;
			    List *listPtr, *nextListPtr;
			    
			    smiPrintError(thisParserPtr,
					  ERR_ILLEGAL_RANGE_FOR_COUNTER,
					  "Counter64");
			    for (listPtr = yyvsp[0].listPtr; listPtr;
				 listPtr = nextListPtr) {
				nextListPtr = listPtr->nextPtr;
				smiFree((Range *)listPtr->ptr);
				smiFree(listPtr);
			    }
			    
                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Counter64", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Counter64");
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
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Counter64");
				}
			    }
			;}
    break;

  case 250:
#line 4773 "parser-smi.y"
    {
			    Import *importPtr;

                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "Integer64");
			    yyval.typePtr = findTypeByName("Integer64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Integer64");
			    }

			    importPtr = findImportByName("Integer64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Integer64");
				}
			    }
			;}
    break;

  case 251:
#line 4798 "parser-smi.y"
    {
			    Type *parentPtr;
			    Import *importPtr;
			    
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "Integer64");
			    parentPtr = findTypeByName("Integer64");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Integer64");
				yyval.typePtr = NULL;
			    } else {
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    }

			    importPtr = findImportByName("Integer64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Integer64");
				}
			    }
			;}
    break;

  case 252:
#line 4830 "parser-smi.y"
    {
			    Import *importPtr;

                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "Unsigned64");
			    yyval.typePtr = findTypeByName("Unsigned64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Unsigned64");
			    }

			    importPtr = findImportByName("Unsigned64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Unsigned64");
				}
			    }
			;}
    break;

  case 253:
#line 4855 "parser-smi.y"
    {
			    Type *parentPtr;
			    Import *importPtr;
			    
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "Unsigned64");
			    parentPtr = findTypeByName("Unsigned64");
			    if (! parentPtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Unsigned64");
				yyval.typePtr = NULL;
			    } else {
				yyval.typePtr = duplicateType(parentPtr, 0,
						   thisParserPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				smiCheckTypeRanges(thisParserPtr, yyval.typePtr);
			    }

			    importPtr = findImportByName("Unsigned64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Unsigned64");
				}
			    }
			;}
    break;

  case 254:
#line 4893 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "IpAddress");
			    }

			    importPtr = findImportByName("IpAddress",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "IpAddress");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "IpAddress");
				}
			    }
			;}
    break;

  case 255:
#line 4921 "parser-smi.y"
    {
			    Import *importPtr;
			    
                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Counter32", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Counter32");
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Counter32");
			    }

			    importPtr = findImportByName("Counter32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Counter32");
				}
			    }
			;}
    break;

  case 256:
#line 4947 "parser-smi.y"
    {
			    Import *importPtr;
			    
                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Gauge32", thisParserPtr->modulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Gauge32");
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Gauge32");
			    }

			    importPtr = findImportByName("Gauge32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Gauge32");
				}
			    }
			;}
    break;

  case 257:
#line 4973 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = smiHandle->typeUnsigned32Ptr;

			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Unsigned32");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Unsigned32");
				}
			    }
			;}
    break;

  case 258:
#line 4997 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "TimeTicks");
			    }

			    importPtr = findImportByName("TimeTicks",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "TimeTicks");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "TimeTicks");
				}
			    }
			;}
    break;

  case 259:
#line 5025 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("Opaque");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Opaque");
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_OPAQUE_OBSOLETE,
						  "Opaque");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_OPAQUE_OBSOLETE,
						  "Opaque");
				}
			    }

			    importPtr = findImportByName("Opaque",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Opaque");
				} else if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Opaque");
				}
			    }
			;}
    break;

  case 260:
#line 5065 "parser-smi.y"
    {
			    Import *importPtr;

                            if ((thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI) &&
                                !findImportByName("Counter64", thisModulePtr))
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "Counter64");
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
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    smiPrintError(thisParserPtr,
						  ERR_SMIV2_BASETYPE_NOT_IMPORTED,
						  "Counter64");
				}
			    }
			;}
    break;

  case 261:
#line 5091 "parser-smi.y"
    {
			    Import *importPtr;

                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "Integer64");
			    yyval.typePtr = findTypeByName("Integer64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Integer64");
			    }

			    importPtr = findImportByName("Integer64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Integer64");
				}
			    }
			;}
    break;

  case 262:
#line 5116 "parser-smi.y"
    {
			    Import *importPtr;

                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "Unsigned64");
			    yyval.typePtr = findTypeByName("Unsigned64");
			    if (! yyval.typePtr) {
				smiPrintError(thisParserPtr, ERR_UNKNOWN_TYPE,
					      "Unsigned64");
			    }

			    importPtr = findImportByName("Unsigned64",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
					   SMI_LANGUAGE_SPPI) {
				    smiPrintError(thisParserPtr,
						  ERR_SPPI_BASETYPE_NOT_IMPORTED,
						  "Unsigned64");
				}
			    }
			;}
    break;

  case 263:
#line 5143 "parser-smi.y"
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
			;}
    break;

  case 264:
#line 5160 "parser-smi.y"
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
			;}
    break;

  case 265:
#line 5177 "parser-smi.y"
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
			;}
    break;

  case 266:
#line 5195 "parser-smi.y"
    {
			    yyval.listPtr = NULL;
			;}
    break;

  case 267:
#line 5209 "parser-smi.y"
    { yyval.listPtr = yyvsp[-1].listPtr; ;}
    break;

  case 268:
#line 5219 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 269:
#line 5225 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 270:
#line 5231 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 271:
#line 5245 "parser-smi.y"
    {
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[0].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[0].valuePtr);
			;}
    break;

  case 272:
#line 5252 "parser-smi.y"
    {
			    yyval.rangePtr = smiMalloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[-2].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    smiFree(yyvsp[-2].valuePtr);
			    smiFree(yyvsp[0].valuePtr);
			;}
    break;

  case 273:
#line 5262 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			;}
    break;

  case 274:
#line 5268 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			;}
    break;

  case 275:
#line 5274 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->value.integer64 = yyvsp[0].integer64;
			;}
    break;

  case 276:
#line 5280 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = yyvsp[0].unsigned64;
			;}
    break;

  case 277:
#line 5286 "parser-smi.y"
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
			;}
    break;

  case 278:
#line 5309 "parser-smi.y"
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
			;}
    break;

  case 279:
#line 5336 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-1].listPtr;
			;}
    break;

  case 280:
#line 5342 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 281:
#line 5348 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 282:
#line 5361 "parser-smi.y"
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
			;}
    break;

  case 283:
#line 5374 "parser-smi.y"
    {
			    yyval.namedNumberPtr = smiMalloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id;
			    yyval.namedNumberPtr->export.value = *yyvsp[-1].valuePtr;
			    smiFree(yyvsp[-1].valuePtr);
			;}
    break;

  case 284:
#line 5383 "parser-smi.y"
    {
			    if (yyvsp[0].unsigned32 > MAX_INTEGER32) {
				smiPrintError(thisParserPtr,
					      ERR_INTEGER32_TOO_LARGE, yyvsp[0].unsigned32);
			    }
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV1) &&
				(yyvsp[0].unsigned32 == 0)) {
				smiPrintError(thisParserPtr,
					      ERR_ENUM_ZERO);
			    }
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].unsigned32;
			;}
    break;

  case 285:
#line 5398 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    /* TODO: non-negative is suggested */
			;}
    break;

  case 286:
#line 5407 "parser-smi.y"
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
			    } else if (thisModulePtr->export.language != SMI_LANGUAGE_SPPI) {
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
			    } else { /* it is SPPI */
			        if (!strcmp(yyvsp[0].id, "current")) {
				    yyval.status = SMI_STATUS_CURRENT;
			        } else if (!strcmp(yyvsp[0].id, "obsolete")) {
				    yyval.status = SMI_STATUS_OBSOLETE;
			        } else if (!strcmp(yyvsp[0].id, "deprecated")) {
				    yyval.status = SMI_STATUS_OBSOLETE;
                                } else {
                                    smiPrintError(thisParserPtr,
                                                  ERR_INVALID_SPPI_STATUS, yyvsp[0].id);
                                    yyval.status = SMI_STATUS_UNKNOWN;
                                }
                            }
			    smiFree(yyvsp[0].id);
			;}
    break;

  case 287:
#line 5466 "parser-smi.y"
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
			;}
    break;

  case 288:
#line 5482 "parser-smi.y"
    {
			    yyval.text = yyvsp[0].text;
			    
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_FORMAT);
			    }
			;}
    break;

  case 289:
#line 5491 "parser-smi.y"
    {
			    yyval.text = NULL;
			;}
    break;

  case 290:
#line 5497 "parser-smi.y"
    {
			    yyval.text = yyvsp[0].text;
			    
			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_UNITS);
			    }
			;}
    break;

  case 291:
#line 5506 "parser-smi.y"
    {
			    yyval.text = NULL;
			;}
    break;

  case 292:
#line 5512 "parser-smi.y"
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
			    } else if (thisModulePtr->export.language != SMI_LANGUAGE_SPPI) {
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
			    } else {
			        if (!strcmp(yyvsp[0].id, "install")) {
				    yyval.access = SMI_ACCESS_INSTALL;
			        } else if (!strcmp(yyvsp[0].id, "install-notify")) {
				    yyval.access = SMI_ACCESS_INSTALL_NOTIFY;
			        } else if (!strcmp(yyvsp[0].id, "notify")) {
				    yyval.access = SMI_ACCESS_NOTIFY;
			        } else if (!strcmp(yyvsp[0].id, "report-only")) {
				    yyval.access = SMI_ACCESS_REPORT_ONLY;
			        } else if (!strcmp(yyvsp[0].id, "not-accessible")) {
				    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
                                } else {
				    smiPrintError(thisParserPtr,
					          ERR_INVALID_SPPI_ACCESS,
					          yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
			        }
                            }
			    smiFree(yyvsp[0].id);
			;}
    break;

  case 293:
#line 5578 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "PIB-INDEX");
                        ;}
    break;

  case 294:
#line 5583 "parser-smi.y"
    {
                            List *p = smiMalloc(sizeof(List));
                            
                            p->ptr       = yyvsp[-1].objectPtr;
                            p->nextPtr   = NULL;
                            
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = p;
			    yyval.index.rowPtr    = NULL;
                            indexFlag    = INDEXFLAG_PIBINDEX;
			;}
    break;

  case 295:
#line 5598 "parser-smi.y"
    {
			    yyval.index.indexkind    = SMI_INDEX_AUGMENT;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
                            indexFlag       = INDEXFLAG_AUGMENTS;
			;}
    break;

  case 296:
#line 5606 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "EXTENDS");
                        ;}
    break;

  case 297:
#line 5611 "parser-smi.y"
    {
			    yyval.index.indexkind    = SMI_INDEX_SPARSE;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
                            indexFlag       = INDEXFLAG_EXTENDS;
			;}
    break;

  case 298:
#line 5619 "parser-smi.y"
    {
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;}
    break;

  case 299:
#line 5625 "parser-smi.y"
    {
                            /* 
                             * To avoid ambiguity caused by merging
                             * the SMI and SPPI parser we use a flag.
                             */
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI) {
                                /*
                                 * Only INDEX or AUGMENTS are allowed for SMI
                                 */
                                if (indexFlag != INDEXFLAG_NONE)
                                    smiPrintError(thisParserPtr, ERR_INDEX_AND_AUGMENTS_USED);
                            } else {
                                /*
                                 * INDEX may only be used if PIB_INDEX was used
                                 */
                                if (indexFlag != INDEXFLAG_PIBINDEX)
                                    smiPrintError(thisParserPtr, ERR_INDEX_WITHOUT_PIB_INDEX);
                            }
                            
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			;}
    break;

  case 300:
#line 5651 "parser-smi.y"
    {
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = yyvsp[-1].listPtr;
			    yyval.index.rowPtr    = NULL;
                        ;}
    break;

  case 301:
#line 5658 "parser-smi.y"
    {
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;}
    break;

  case 302:
#line 5664 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 303:
#line 5671 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 304:
#line 5684 "parser-smi.y"
    {
			    impliedFlag = 1;
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 305:
#line 5689 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 306:
#line 5699 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 307:
#line 5705 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 308:
#line 5711 "parser-smi.y"
    { yyval.valuePtr = yyvsp[-1].valuePtr; ;}
    break;

  case 309:
#line 5713 "parser-smi.y"
    { yyval.valuePtr = NULL; ;}
    break;

  case 310:
#line 5718 "parser-smi.y"
    { yyval.valuePtr = yyvsp[0].valuePtr; ;}
    break;

  case 311:
#line 5720 "parser-smi.y"
    {
			    yyval.valuePtr = smiMalloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
			    yyval.valuePtr->value.ptr = (void *)yyvsp[-1].listPtr;
			;}
    break;

  case 312:
#line 5728 "parser-smi.y"
    { yyval.listPtr = yyvsp[0].listPtr; ;}
    break;

  case 313:
#line 5730 "parser-smi.y"
    { yyval.listPtr = NULL; ;}
    break;

  case 314:
#line 5734 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].id;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 315:
#line 5740 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].id;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 316:
#line 5753 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 317:
#line 5759 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 318:
#line 5765 "parser-smi.y"
    {
			    yyval.text = yyvsp[0].text;

			    if (yyvsp[0].text && !strlen(yyvsp[0].text)) {
				smiPrintError(thisParserPtr,
					      ERR_EMPTY_REFERENCE);
			    }
			;}
    break;

  case 319:
#line 5774 "parser-smi.y"
    { yyval.text = NULL; ;}
    break;

  case 320:
#line 5778 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 321:
#line 5780 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 322:
#line 5784 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 323:
#line 5786 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 324:
#line 5790 "parser-smi.y"
    {
			    thisParserPtr->firstRevisionLine = thisParserPtr->line;

			    if (thisParserPtr->modulePtr->lastRevisionPtr &&
				(yyvsp[0].date >= thisParserPtr->modulePtr->lastRevisionPtr->export.date)) {
				smiPrintError(thisParserPtr,
					      ERR_REVISION_NOT_DESCENDING);
			    }

			    if (yyvsp[0].date > thisParserPtr->modulePtr->lastUpdated) {
				smiPrintError(thisParserPtr,
					      ERR_REVISION_AFTER_LAST_UPDATE);
			    }
			;}
    break;

  case 325:
#line 5805 "parser-smi.y"
    {
			    Revision *revisionPtr;

			    checkDescr(thisParserPtr, yyvsp[0].text);

			    revisionPtr = addRevision(yyvsp[-3].date, yyvsp[0].text, thisParserPtr);
			    if (revisionPtr) {
				setRevisionLine(revisionPtr,
						thisParserPtr->firstRevisionLine,
						thisParserPtr);
			    }
			    yyval.err = revisionPtr ? 0 : -1;
			;}
    break;

  case 326:
#line 5821 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-1].listPtr;
			;}
    break;

  case 327:
#line 5825 "parser-smi.y"
    {
			    yyval.listPtr = NULL;
			;}
    break;

  case 328:
#line 5831 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-1].listPtr;
			;}
    break;

  case 329:
#line 5837 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 330:
#line 5843 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 331:
#line 5856 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    if ((thisParserPtr->currentDecl == SMI_DECL_OBJECTGROUP) &&
				yyval.objectPtr->modulePtr != thisParserPtr->modulePtr) {
                                smiPrintError(thisParserPtr,
					      ERR_COMPLIANCE_MEMBER_NOT_LOCAL,
					      yyval.objectPtr->export.name);
			    }
			;}
    break;

  case 332:
#line 5868 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-1].listPtr;
			;}
    break;

  case 333:
#line 5874 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 334:
#line 5880 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 335:
#line 5893 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    if (yyval.objectPtr->modulePtr != thisParserPtr->modulePtr) {
                                smiPrintError(thisParserPtr,
					      ERR_COMPLIANCE_MEMBER_NOT_LOCAL,
					      yyval.objectPtr->export.name);
			    }
			;}
    break;

  case 336:
#line 5904 "parser-smi.y"
    {
			    int len;
			    yyval.text = smiStrdup(yyvsp[0].text);
			    len = strlen(yyval.text);
			    while (len > 0 && yyval.text[len-1] == '\n') {
				yyval.text[--len] = 0;
			    }
			;}
    break;

  case 337:
#line 5915 "parser-smi.y"
    {
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 338:
#line 5920 "parser-smi.y"
    {
			    thisParserPtr->parentNodePtr = smiHandle->rootNodePtr;
			;}
    break;

  case 339:
#line 5924 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    if (yyval.objectPtr) {
				thisParserPtr->parentNodePtr = yyvsp[0].objectPtr->nodePtr;
			    } else {
				thisParserPtr->parentNodePtr = NULL;
			    }
			;}
    break;

  case 340:
#line 5936 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 341:
#line 5941 "parser-smi.y"
    {
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;}
    break;

  case 342:
#line 5949 "parser-smi.y"
    {
			    Object *objectPtr;
			    Import *importPtr;
			    
			    if (thisParserPtr->parentNodePtr != smiHandle->rootNodePtr) {
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
				    if (thisParserPtr->complianceModulePtr) {
					objectPtr =
					    findObjectByModuleAndName(
						thisParserPtr->complianceModulePtr, yyvsp[0].id);
					if (objectPtr) {
					    importPtr = addImport(
						yyvsp[0].id,
						thisParserPtr);
					    setImportModulename(importPtr,
								thisParserPtr->complianceModulePtr->export.name);
					    addImportFlags(importPtr,
							   FLAG_INCOMPLIANCE);
					    importPtr->use++;
					} else {
					    objectPtr = addObject(yyvsp[0].id,
								  thisParserPtr->pendingNodePtr, 0,
								  FLAG_INCOMPLETE,
								  thisParserPtr);
					    smiPrintError(thisParserPtr,
							  ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[0].id,
							  thisParserPtr->complianceModulePtr->export.name);
					}
				    } else if (thisParserPtr->capabilitiesModulePtr) {
					objectPtr =
					    findObjectByModuleAndName(
						thisParserPtr->capabilitiesModulePtr, yyvsp[0].id);
					if (objectPtr) {
					    importPtr = addImport(
						yyvsp[0].id,
						thisParserPtr);
					    setImportModulename(importPtr,
								thisParserPtr->capabilitiesModulePtr->
								export.name);
					    addImportFlags(importPtr,
							   FLAG_INCOMPLIANCE);
					    importPtr->use++;
					} else {
					    objectPtr = addObject(yyvsp[0].id,
								  thisParserPtr->pendingNodePtr, 0,
								  FLAG_INCOMPLETE,
								  thisParserPtr);
					    smiPrintError(thisParserPtr,
							  ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[0].id,
							  thisParserPtr->capabilitiesModulePtr->export.name);
					}
				    } else {
					/* 
					 * forward referenced node.
					 * create it,
					 * marked with FLAG_INCOMPLETE.
					 */
					objectPtr = addObject(yyvsp[0].id,
							      thisParserPtr->pendingNodePtr,
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
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			;}
    break;

  case 343:
#line 6043 "parser-smi.y"
    {
			    Object *objectPtr;
			    Import *importPtr;
			    char *md;
			    
			    if (thisParserPtr->parentNodePtr != smiHandle->rootNodePtr) {
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
					if (thisParserPtr->complianceModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    thisParserPtr->complianceModulePtr, yyvsp[-2].id);
					    if (objectPtr) {
						importPtr = addImport(
						    yyvsp[-2].id,
						    thisParserPtr);
						setImportModulename(importPtr,
						    thisParserPtr->complianceModulePtr->export.name);
						addImportFlags(importPtr,
							       FLAG_INCOMPLIANCE);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    thisParserPtr->pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[-2].id,
					     thisParserPtr->complianceModulePtr->export.name);
					    }
					} else if (thisParserPtr->capabilitiesModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    thisParserPtr->capabilitiesModulePtr, yyvsp[-2].id);
					    if (objectPtr) {
						importPtr = addImport(
						    yyvsp[-2].id,
						    thisParserPtr);
						setImportModulename(importPtr,
						        thisParserPtr->capabilitiesModulePtr->
								  export.name);
						addImportFlags(importPtr,
							       FLAG_INCOMPLIANCE);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    thisParserPtr->pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[-2].id,
					   thisParserPtr->capabilitiesModulePtr->export.name);
					    }
					} else {
					    /* 
					     * forward referenced node.
					     * create it,
					     * marked with FLAG_INCOMPLETE.
					     */
					    objectPtr = addObject(yyvsp[0].id,
							    thisParserPtr->pendingNodePtr,
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
				    thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			;}
    break;

  case 344:
#line 6147 "parser-smi.y"
    {
			    Node *nodePtr;
			    Object *objectPtr;

			    nodePtr = findNodeByParentAndSubid(thisParserPtr->parentNodePtr,
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
						      thisParserPtr->parentNodePtr,
						      yyvsp[0].unsigned32,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				yyval.objectPtr = objectPtr;
			    }
			    thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			;}
    break;

  case 345:
#line 6171 "parser-smi.y"
    {
			    Object *objectPtr = NULL;
			    Object *oldObjectPtr = NULL;
			    Node *oldNodePtr = NULL;
			    
			    /* TODO: search in local module and
			     *       in imported modules
			     */

			    oldNodePtr = findNodeByParentAndSubid(
				thisParserPtr->parentNodePtr, yyvsp[-1].unsigned32);
			    oldObjectPtr = findObjectByModuleAndName(
                                thisParserPtr->modulePtr, yyvsp[-3].id);

			    if (oldObjectPtr &&
				((oldObjectPtr->nodePtr->subid != yyvsp[-1].unsigned32) ||
				 (oldObjectPtr->nodePtr->parentPtr != thisParserPtr->parentNodePtr))) {
				smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_OID_CHANGED,
					      yyvsp[-3].id);
				smiPrintErrorAtLine(thisParserPtr,
						    ERR_PREVIOUS_DEFINITION,
						    oldObjectPtr->line,
						    oldObjectPtr->export.name);
				objectPtr = addObject(yyvsp[-3].id,
						      thisParserPtr->parentNodePtr,
						      yyvsp[-1].unsigned32, 0, thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_IMPL_OBJECT);
				yyval.objectPtr = objectPtr;
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    } else if (oldNodePtr &&
				       oldNodePtr->lastObjectPtr &&
				       strcmp(oldNodePtr->lastObjectPtr->export.name, yyvsp[-3].id)) {
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_CHANGED,
					      yyvsp[-3].id, oldNodePtr->lastObjectPtr->export.name);
				smiPrintErrorAtLine(thisParserPtr,
						    ERR_PREVIOUS_DEFINITION,
						    oldNodePtr->lastObjectPtr->line,
						    oldNodePtr->lastObjectPtr->export.name);
				objectPtr = addObject(yyvsp[-3].id,
						      thisParserPtr->parentNodePtr,
						      yyvsp[-1].unsigned32, 0, thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_IMPL_OBJECT);
				yyval.objectPtr = objectPtr;
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    } else {
				objectPtr = addObject(yyvsp[-3].id, thisParserPtr->parentNodePtr,
						      yyvsp[-1].unsigned32, 0,
						      thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_IMPL_OBJECT);
				yyval.objectPtr = objectPtr;
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			;}
    break;

  case 346:
#line 6230 "parser-smi.y"
    {
			    Object *objectPtr = NULL;
			    Object *oldObjectPtr = NULL;
			    Node *oldNodePtr = NULL;
			    char *md;

			    md = smiMalloc(sizeof(char) *
					   (strlen(yyvsp[-5].id) + strlen(yyvsp[-3].id) + 2));
			    sprintf(md, "%s.%s", yyvsp[-5].id, yyvsp[-3].id);

			    oldNodePtr = findNodeByParentAndSubid(
				thisParserPtr->parentNodePtr, yyvsp[-1].unsigned32);
			    oldObjectPtr = findObjectByModulenameAndName(
                                yyvsp[-5].id, yyvsp[-3].id);

			    if (oldObjectPtr &&
				((oldObjectPtr->nodePtr->subid != yyvsp[-1].unsigned32) ||
				 (oldObjectPtr->nodePtr->parentPtr != thisParserPtr->parentNodePtr))) {
				smiPrintError(thisParserPtr,
					      ERR_ILLEGALLY_QUALIFIED, md);
				smiPrintError(thisParserPtr,
					      ERR_IDENTIFIER_OID_CHANGED,
					      yyvsp[-3].id);
				smiPrintErrorAtLine(thisParserPtr,
						    ERR_PREVIOUS_DEFINITION,
						    oldObjectPtr->line,
						    oldObjectPtr->export.name);
				objectPtr = addObject(yyvsp[-3].id,
						      thisParserPtr->parentNodePtr,
						      yyvsp[-1].unsigned32, 0, thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_IMPL_OBJECT);
				yyval.objectPtr = objectPtr;
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    } else if (oldNodePtr &&
				       oldNodePtr->lastObjectPtr &&
				       strcmp(oldNodePtr->lastObjectPtr->export.name, yyvsp[-3].id)) {
				smiPrintError(thisParserPtr,
					      ERR_ILLEGALLY_QUALIFIED, md);
				smiPrintError(thisParserPtr,
					      ERR_OIDLABEL_CHANGED,
					      yyvsp[-3].id, oldNodePtr->lastObjectPtr->export.name);
				smiPrintErrorAtLine(thisParserPtr,
						    ERR_PREVIOUS_DEFINITION,
						    oldNodePtr->lastObjectPtr->line,
						    oldNodePtr->lastObjectPtr->export.name);
				objectPtr = addObject(yyvsp[-3].id,
						      thisParserPtr->parentNodePtr,
						      yyvsp[-1].unsigned32, 0, thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_IMPL_OBJECT);
				yyval.objectPtr = objectPtr;
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    } else {
				smiPrintError(thisParserPtr,
					      ERR_ILLEGALLY_QUALIFIED, md);
				objectPtr = addObject(yyvsp[-3].id, thisParserPtr->parentNodePtr,
						      yyvsp[-1].unsigned32, 0,
						      thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_IMPL_OBJECT);
				yyval.objectPtr = objectPtr;
				thisParserPtr->parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			    smiFree(md);
			;}
    break;

  case 347:
#line 6299 "parser-smi.y"
    { yyval.text = NULL; ;}
    break;

  case 348:
#line 6303 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 349:
#line 6305 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 350:
#line 6309 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 351:
#line 6311 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 352:
#line 6315 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_OBJECTGROUP;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 353:
#line 6325 "parser-smi.y"
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
			;}
    break;

  case 354:
#line 6344 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 355:
#line 6349 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-14].id, thisParserPtr);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTGROUP);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
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
			;}
    break;

  case 356:
#line 6375 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_NOTIFICATIONGROUP;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 357:
#line 6385 "parser-smi.y"
    {
			    Import *importPtr;
                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "NOTIFICATION-GROUP");
			    
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
			;}
    break;

  case 358:
#line 6407 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 359:
#line 6412 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    objectPtr = setObjectName(objectPtr, yyvsp[-14].id, thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONGROUP);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
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
			;}
    break;

  case 360:
#line 6439 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_MODULECOMPLIANCE;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 361:
#line 6449 "parser-smi.y"
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
			;}
    break;

  case 362:
#line 6467 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 363:
#line 6473 "parser-smi.y"
    {
			    Object *objectPtr;
			    Option *optionPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;

			    smiCheckObjectReuse(thisParserPtr, yyvsp[-14].id, &objectPtr);

			    setObjectName(objectPtr, yyvsp[-14].id, thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
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
			;}
    break;

  case 364:
#line 6532 "parser-smi.y"
    {
			    yyval.compl = yyvsp[0].compl;
			;}
    break;

  case 365:
#line 6538 "parser-smi.y"
    {
			    yyval.compl = yyvsp[0].compl;
			;}
    break;

  case 366:
#line 6542 "parser-smi.y"
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
			;}
    break;

  case 367:
#line 6577 "parser-smi.y"
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
				thisParserPtr->complianceModulePtr = NULL;
			    else
				thisParserPtr->complianceModulePtr = yyvsp[0].modulePtr;
			;}
    break;

  case 368:
#line 6593 "parser-smi.y"
    {
			    yyval.compl.mandatorylistPtr = yyvsp[-1].listPtr;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			    if (thisParserPtr->complianceModulePtr) {
				checkImports(thisParserPtr->complianceModulePtr,
					     thisParserPtr);
				thisParserPtr->complianceModulePtr = NULL;
			    }
			;}
    break;

  case 369:
#line 6606 "parser-smi.y"
    {
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id, thisParserPtr);
			    }
			    smiFree(yyvsp[-1].id);
			;}
    break;

  case 370:
#line 6615 "parser-smi.y"
    {
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id, thisParserPtr);
			    }
			    smiFree(yyvsp[0].id);
			;}
    break;

  case 371:
#line 6624 "parser-smi.y"
    {
			    yyval.modulePtr = thisModulePtr;
			;}
    break;

  case 372:
#line 6630 "parser-smi.y"
    {
			    yyval.listPtr = yyvsp[-1].listPtr;
			;}
    break;

  case 373:
#line 6634 "parser-smi.y"
    {
			    yyval.listPtr = NULL;
			;}
    break;

  case 374:
#line 6640 "parser-smi.y"
    {
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;}
    break;

  case 375:
#line 6646 "parser-smi.y"
    {
			    List *p, *pp;
			    
			    p = smiMalloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;}
    break;

  case 376:
#line 6659 "parser-smi.y"
    {
			    /* TODO: check that objectIdentifier is
			       found, is defined in thisParserPtr->complianceModulePtr,
			       and is a group node. */
			    Import *importPtr;

			    yyval.objectPtr = yyvsp[0].objectPtr;
			    if (thisParserPtr->complianceModulePtr) {
				yyval.objectPtr = findObjectByModuleAndName(
				                           thisParserPtr->complianceModulePtr,
							   yyvsp[0].objectPtr->export.name);
			    }
			    if (thisParserPtr->complianceModulePtr && yyvsp[0].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
				    thisParserPtr->complianceModulePtr->export.name,
				    yyvsp[0].objectPtr->export.name, thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }
			;}
    break;

  case 377:
#line 6682 "parser-smi.y"
    {
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			;}
    break;

  case 378:
#line 6688 "parser-smi.y"
    {
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = NULL;
			;}
    break;

  case 379:
#line 6696 "parser-smi.y"
    {
			    yyval.compl = yyvsp[0].compl;
			;}
    break;

  case 380:
#line 6700 "parser-smi.y"
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
			;}
    break;

  case 381:
#line 6767 "parser-smi.y"
    {
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].listPtr;
			    yyval.compl.refinementlistPtr = NULL;
			;}
    break;

  case 382:
#line 6773 "parser-smi.y"
    {
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = yyvsp[0].listPtr;
			;}
    break;

  case 383:
#line 6781 "parser-smi.y"
    {
			    thisParserPtr->firstNestedStatementLine = thisParserPtr->line;
			;}
    break;

  case 384:
#line 6786 "parser-smi.y"
    {
			    Import *importPtr;
			    
			    if (thisParserPtr->complianceModulePtr && yyvsp[-2].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
						    thisParserPtr->complianceModulePtr->export.name,
						    yyvsp[-2].objectPtr->export.name,
						    thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }

			    checkDescr(thisParserPtr, yyvsp[0].text);
			    
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = smiMalloc(sizeof(Option));
			    ((Option *)(yyval.listPtr->ptr))->line = thisParserPtr->firstNestedStatementLine;
			    ((Option *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-2].objectPtr;
			    if (! (thisModulePtr->flags & SMI_FLAG_NODESCR)) {
				((Option *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			    } else {
				smiFree(yyvsp[0].text);
			    }
			;}
    break;

  case 385:
#line 6814 "parser-smi.y"
    {
			    thisParserPtr->firstNestedStatementLine = thisParserPtr->line;
			;}
    break;

  case 386:
#line 6822 "parser-smi.y"
    {
			    Import *importPtr;

			    if (thisParserPtr->complianceModulePtr && yyvsp[-5].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
						    thisParserPtr->complianceModulePtr->export.name,
						    yyvsp[-5].objectPtr->export.name,
						    thisModulePtr);
				if (importPtr) 
				    importPtr->use++;
			    }

			    checkDescr(thisParserPtr, yyvsp[0].text);
			    
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = smiMalloc(sizeof(Refinement));
			    ((Refinement *)(yyval.listPtr->ptr))->line =
				thisParserPtr->firstNestedStatementLine;
			    ((Refinement *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-5].objectPtr;
			    ((Refinement *)(yyval.listPtr->ptr))->typePtr = yyvsp[-4].typePtr;
			    if (yyvsp[-4].typePtr) {
				yyvsp[-4].typePtr->parentPtr = yyvsp[-5].objectPtr->typePtr;
			    }
			    ((Refinement *)(yyval.listPtr->ptr))->writetypePtr = yyvsp[-3].typePtr;
			    if (yyvsp[-3].typePtr) {
				yyvsp[-3].typePtr->parentPtr = yyvsp[-5].objectPtr->typePtr;
			    }
			    ((Refinement *)(yyval.listPtr->ptr))->export.access = yyvsp[-2].access;
			    if (! (thisParserPtr->flags & SMI_FLAG_NODESCR)) {
				((Refinement *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			    } else {
				smiFree(yyvsp[0].text);
			    }
			;}
    break;

  case 387:
#line 6861 "parser-smi.y"
    {
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;}
    break;

  case 388:
#line 6869 "parser-smi.y"
    {
			    yyval.typePtr = NULL;
			;}
    break;

  case 389:
#line 6875 "parser-smi.y"
    {
                            /* must not be present in PIBs */
                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "WRITE-SYNTAX");
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;}
    break;

  case 390:
#line 6886 "parser-smi.y"
    {
			    yyval.typePtr = NULL;
			;}
    break;

  case 391:
#line 6892 "parser-smi.y"
    {
			    yyval.typePtr = yyvsp[0].typePtr;
			;}
    break;

  case 392:
#line 6898 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "MIN-ACCESS");
			    yyval.access = yyvsp[0].access;
			;}
    break;

  case 393:
#line 6904 "parser-smi.y"
    {
                            if (thisParserPtr->modulePtr->export.language != SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SPPI_CONSTRUCT_IN_MIB, "PIB-MIN-ACCESS");
                            if (yyvsp[0].access == SMI_ACCESS_REPORT_ONLY)
                                smiPrintError(thisParserPtr, ERR_REPORT_ONLY_IN_PIB_MIN_ACCESS);
                            yyval.access = yyvsp[0].access;
                        ;}
    break;

  case 394:
#line 6912 "parser-smi.y"
    {
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;}
    break;

  case 395:
#line 6918 "parser-smi.y"
    {
			    thisParserPtr->firstStatementLine = thisParserPtr->line;
			    thisParserPtr->currentDecl = SMI_DECL_AGENTCAPABILITIES;
			    
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    smiCheckObjectName(thisParserPtr,
					       thisModulePtr, yyvsp[0].id);
			;}
    break;

  case 396:
#line 6928 "parser-smi.y"
    {
			    Import *importPtr;
                            if (thisParserPtr->modulePtr->export.language == SMI_LANGUAGE_SPPI)
                                smiPrintError(thisParserPtr, ERR_SMI_CONSTRUCT_IN_PIB, "AGENT-CAAPABILITIES");
			    
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
			;}
    break;

  case 397:
#line 6950 "parser-smi.y"
    {
			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 398:
#line 6956 "parser-smi.y"
    {
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    smiCheckObjectReuse(thisParserPtr, yyvsp[-16].id, &objectPtr);

			    setObjectName(objectPtr, yyvsp[-16].id, thisParserPtr);
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    setObjectLine(objectPtr, thisParserPtr->firstStatementLine,
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
			;}
    break;

  case 399:
#line 6986 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 400:
#line 6988 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 401:
#line 6992 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 402:
#line 6994 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 403:
#line 6998 "parser-smi.y"
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
				thisParserPtr->capabilitiesModulePtr = NULL;
			    else
				thisParserPtr->capabilitiesModulePtr = yyvsp[0].modulePtr;
			;}
    break;

  case 404:
#line 7014 "parser-smi.y"
    {
			    if (thisParserPtr->capabilitiesModulePtr) {
				checkImports(thisParserPtr->capabilitiesModulePtr,
					     thisParserPtr);
				thisParserPtr->capabilitiesModulePtr = NULL;
			    }
			    yyval.err = 0;
			;}
    break;

  case 405:
#line 7025 "parser-smi.y"
    {
#if 0
			    yyval.listPtr = smiMalloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].listPtr;
			    yyval.listPtr->nextPtr = NULL;
#else
			    yyval.listPtr = NULL;
#endif
			;}
    break;

  case 406:
#line 7035 "parser-smi.y"
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
			;}
    break;

  case 407:
#line 7052 "parser-smi.y"
    {
			    yyval.listPtr = NULL;
			;}
    break;

  case 408:
#line 7058 "parser-smi.y"
    {
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id, thisParserPtr);
			    }
			    smiFree(yyvsp[-1].id);
			;}
    break;

  case 409:
#line 7067 "parser-smi.y"
    {
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id, thisParserPtr);
			    }
			    smiFree(yyvsp[0].id);
			;}
    break;

  case 410:
#line 7077 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 411:
#line 7079 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 412:
#line 7083 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 413:
#line 7085 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 414:
#line 7089 "parser-smi.y"
    {
			    if (yyvsp[0].objectPtr) {
				variationkind = yyvsp[0].objectPtr->export.nodekind;
			    } else {
				variationkind = SMI_NODEKIND_UNKNOWN;
			    }
			;}
    break;

  case 415:
#line 7097 "parser-smi.y"
    {
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_SYNTAX);
			    }
			;}
    break;

  case 416:
#line 7104 "parser-smi.y"
    {
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_WRITESYNTAX);
			    }
			;}
    break;

  case 417:
#line 7112 "parser-smi.y"
    {
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_CREATION);
			    }
			;}
    break;

  case 418:
#line 7119 "parser-smi.y"
    {
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				smiPrintError(thisParserPtr,
				      ERR_NOTIFICATION_VARIATION_DEFVAL);
			    } else if (yyvsp[0].valuePtr) {
				adjustDefval(thisParserPtr,
					     yyvsp[0].valuePtr, yyvsp[-9].objectPtr->typePtr,
					     thisParserPtr->line);
				smiCheckValueType(thisParserPtr,
						  yyvsp[0].valuePtr, yyvsp[-9].objectPtr->typePtr,
						  thisParserPtr->line);
			    }
			;}
    break;

  case 419:
#line 7133 "parser-smi.y"
    {
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			    variationkind = SMI_NODEKIND_UNKNOWN;

			    checkDescr(thisParserPtr, yyvsp[0].text);
			;}
    break;

  case 420:
#line 7143 "parser-smi.y"
    { yyval.access = yyvsp[0].access; ;}
    break;

  case 421:
#line 7145 "parser-smi.y"
    { yyval.access = 0; ;}
    break;

  case 422:
#line 7149 "parser-smi.y"
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
			;}
    break;

  case 423:
#line 7214 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 424:
#line 7216 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 425:
#line 7220 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 426:
#line 7222 "parser-smi.y"
    { yyval.err = 0; ;}
    break;

  case 427:
#line 7226 "parser-smi.y"
    { yyval.err = 0; ;}
    break;


    }

/* Line 1010 of yacc.c.  */
#line 9496 "parser-smi.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 7229 "parser-smi.y"


#endif

