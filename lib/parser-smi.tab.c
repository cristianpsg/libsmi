
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
#include <limits.h>
    
#include "smi.h"
#include "error.h"
#include "parser-smi.h"
#include "scanner-smi.h"
#include "data.h"
#include "util.h"
    


/*
 * These arguments are passed to yyparse() and yylex().
 */
#define YYPARSE_PARAM parserPtr
#define YYLEX_PARAM   parserPtr

    
    
#define thisParserPtr      ((Parser *)parserPtr)
#define thisModulePtr     (((Parser *)parserPtr)->modulePtr)


    
/*
 * NOTE: The argument lvalp ist not really a void pointer. Unfortunately,
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
	printError(parser, error_64, name);
    } else if (len > 32) {
	printError(parser, error_32, name);
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
	printError(parserPtr, ERR_NO_MODULE_IDENTITY);
    }
}


static void
checkObjects(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr, *parentPtr;
    Node *nodePtr;
    int i;
    
    /*
     * Set nodekinds of all newly defined objects.
     */
    
    for (objectPtr = modulePtr->firstObjectPtr;
	 objectPtr; objectPtr = objectPtr->nextPtr) {
	if (objectPtr->nodePtr->parentPtr &&
	    objectPtr->nodePtr->parentPtr->lastObjectPtr) {
	    parentPtr = objectPtr->nodePtr->parentPtr->lastObjectPtr;
	} else {
	    parentPtr = NULL;
	}
	if (objectPtr->export.decl == SMI_DECL_MODULEIDENTITY) {
	    objectPtr->export.nodekind = SMI_NODEKIND_NODE;
	} else if ((objectPtr->export.decl == SMI_DECL_VALUEASSIGNMENT) ||
		   (objectPtr->export.decl == SMI_DECL_OBJECTIDENTITY)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_NODE;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
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
		   (parentPtr->export.indexkind != SMI_INDEX_UNKNOWN)) {
	    objectPtr->export.nodekind = SMI_NODEKIND_COLUMN;
	} else if ((objectPtr->export.decl == SMI_DECL_OBJECTTYPE) &&
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
	    printErrorAtLine(parserPtr, ERR_BASETYPE_UNKNOWN, objectPtr->line,
			     objectPtr->typePtr->export.name, objectPtr->export.name);
	    if (objectPtr->nodePtr->parentPtr->firstObjectPtr->export.nodekind
		== SMI_NODEKIND_TABLE) {
		/* the parent node is a table node, so assume this is
		 *  a row node. this adjusts missing INDEXs in RFC 1158.
		 */
		objectPtr->export.nodekind = SMI_NODEKIND_ROW;
	    }
	}

	/*
	 * Check whether a row's subid is 1, see RFC 2578 7.10 (1).
	 */
	
	if (objectPtr->export.nodekind == SMI_NODEKIND_ROW) {
	    int len;
	    
	    if (objectPtr->nodePtr->subid != 1) {
		printErrorAtLine(parserPtr, ERR_ROW_SUBID_ONE,
				 objectPtr->line, objectPtr->export.name);
	    }

	    len = strlen(objectPtr->export.name);
	    if (len < 6 || strcmp(objectPtr->export.name+len-5,"Entry")) {
		printErrorAtLine(parserPtr, ERR_ROWNAME_ENTRY,
				 objectPtr->line, objectPtr->export.name);
	    }
	}

	if (objectPtr->export.nodekind == SMI_NODEKIND_TABLE) {
	    int len;

	    len = strlen(objectPtr->export.name);
	    if (len < 6 || strcmp(objectPtr->export.name+len-5,"Table")) {
		printErrorAtLine(parserPtr, ERR_TABLENAME_TABLE,
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
	    if (strlen(objectPtr->export.name)) {
		printErrorAtLine(parserPtr, ERR_UNKNOWN_OIDLABEL,
				 objectPtr->line, objectPtr->export.name);
	    } else {
		printErrorAtLine(parserPtr, ERR_IMPLICIT_NODE,
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
	 * Set the oidlen/oid values that are not yet correct.
	 */

	if (objectPtr->export.oidlen == 0) {
	    if (objectPtr->nodePtr->oidlen == 0) {
		for (nodePtr = objectPtr->nodePtr, i = 1;
		     nodePtr->parentPtr != pendingNodePtr &&
			 nodePtr->parentPtr != rootNodePtr;
		     nodePtr = nodePtr->parentPtr, i++);
		objectPtr->nodePtr->oid = calloc(i, sizeof(unsigned int));
		objectPtr->nodePtr->oidlen = i;
		for (nodePtr = objectPtr->nodePtr; i > 0; i--) {
		    objectPtr->nodePtr->oid[i-1] = nodePtr->subid;
		    nodePtr = nodePtr->parentPtr;
		}
	    }
	    objectPtr->export.oidlen = objectPtr->nodePtr->oidlen;
	    objectPtr->export.oid = objectPtr->nodePtr->oid;
	}
    }
}


static void
checkTypes(Parser *parserPtr, Module *modulePtr)
{
    Type *typePtr;
    
    /*
     * Check references to unknown types.
     */
    
    for(typePtr = modulePtr->firstTypePtr;
	typePtr; typePtr = typePtr->nextPtr) {
	if ((typePtr->flags & FLAG_INCOMPLETE)
	    && typePtr->export.name
	    && (typePtr->export.decl == SMI_DECL_UNKNOWN)) {
	    printErrorAtLine(parserPtr, ERR_UNKNOWN_TYPE,
			     typePtr->line, typePtr->export.name);
	}

	/*
	 * Use TCs instead of type assignments in SMIv2.
	 */

	if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2
	    && typePtr->export.decl == SMI_DECL_TYPEASSIGNMENT
	    && typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
	    printErrorAtLine(parserPtr, ERR_SMIV2_TYPE_ASSIGNEMENT,
			     typePtr->line, typePtr->export.name);
	}

	/*
	 * Complain about TCs derived from other TCs (RFC 2579 3.5).
	 */
	
	if (typePtr->parentPtr
	    && typePtr->export.decl == SMI_DECL_TEXTUALCONVENTION
	    && typePtr->export.decl == typePtr->parentPtr->export.decl) {
	    printErrorAtLine(parserPtr, ERR_SMIV2_NESTED_TEXTUAL_CONVENTION,
			     typePtr->line,
			     typePtr->export.name,
			     typePtr->parentPtr->export.name);
	}
    }
}

				
static void
checkDefvals(Parser *parserPtr, Module *modulePtr)
{
    Object *objectPtr, *object2Ptr;
    Import *importPtr;

    /*
     * Check unknown identifiers in OID DEFVALs.
     */

    for(objectPtr = modulePtr->firstObjectPtr;
	objectPtr; objectPtr = objectPtr->nextPtr) {

#if 0 /* TODO remove me */
	/*
	 * Set the basetype to the type's basetype if not done yet.
	 */
	    
	if (valuePtr->basetype == SMI_BASETYPE_UNKNOWN) {
	    valuePtr->basetype = objectPtr->typePtr->export.basetype;
	}
#endif
	if (objectPtr->export.value.basetype == SMI_BASETYPE_UNKNOWN) continue;
	
	if ((objectPtr->export.value.basetype == SMI_BASETYPE_OBJECTIDENTIFIER)
	    && (objectPtr->export.value.format == SMI_VALUEFORMAT_NAME)) {
	    object2Ptr = findObjectByModuleAndName(parserPtr->modulePtr,
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
		printErrorAtLine(parserPtr, ERR_UNKNOWN_OIDLABEL,
				 objectPtr->line,
				 (char *)objectPtr->export.value.value.ptr);
		objectPtr->export.value.value.ptr = NULL;
		objectPtr->export.value.basetype = SMI_BASETYPE_UNKNOWN;
	    } else {
		objectPtr->export.value.value.ptr = (void *)object2Ptr;
	    }
	    objectPtr->export.value.format = SMI_VALUEFORMAT_OID;
	} else if ((objectPtr->export.value.basetype == SMI_BASETYPE_OBJECTIDENTIFIER)
		   && (objectPtr->export.value.format == SMI_VALUEFORMAT_OID)) {
	    if ((objectPtr->export.value.len != 2) ||
		(objectPtr->export.value.value.oid[0] != 0) ||
		(objectPtr->export.value.value.oid[1] != 0)) {
		printErrorAtLine(parserPtr, ERR_ILLEGAL_OID_DEFVAL,
				 objectPtr->line, objectPtr->export.name);
	    }
	    if (!findModuleByName("SNMPv2-SMI")) {
		loadModule("SNMPv2-SMI");
	    }
	    object2Ptr = findObjectByModulenameAndName("SNMPv2-SMI",
						       "zeroDotZero");
	    objectPtr->export.value.format = SMI_VALUEFORMAT_OID;
	    objectPtr->export.value.value.ptr = (void *)object2Ptr;
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
		    printErrorAtLine(parserPtr, ERR_ILLEGAL_IMPORT,
				     importPtr->line, importPtr->export.name,
				     importPtr->export.module);
		}
	    }
	    if (importPtr->use == 0) {
		printErrorAtLine(parserPtr, ERR_UNUSED_IMPORT,
				 importPtr->line, importPtr->export.name,
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
		printError(parserPtr, ERR_DATE_CHARACTER, date);
		anytime = (time_t) -1;
		break;
	    }
	}
    } else {
	printError(parserPtr, ERR_DATE_LENGTH, date);
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
		printError(parserPtr, ERR_DATE_YEAR_2DIGITS, date, tm.tm_year);
	}
	tm.tm_mon  = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	tm.tm_mday = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	tm.tm_hour = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	tm.tm_min  = (p[0]-'0') * 10 + (p[1]-'0');
	
	if (tm.tm_mon < 1 || tm.tm_mon > 12) {
	    printError(parserPtr, ERR_DATE_MONTH, date);
	}
	if (tm.tm_mday < 1 || tm.tm_mday > 31) {
	    printError(parserPtr, ERR_DATE_DAY, date);
	}
	if (tm.tm_hour < 0 || tm.tm_hour > 23) {
	    printError(parserPtr, ERR_DATE_HOUR, date);
	}
	if (tm.tm_min < 0 || tm.tm_min > 59) {
	    printError(parserPtr, ERR_DATE_MINUTES, date);
	}
	
	tm.tm_year -= 1900;
	tm.tm_mon -= 1;
	tm.tm_isdst = 0;

	anytime = timegm(&tm);
	
	if (anytime == (time_t) -1) {
	    printError(parserPtr, ERR_DATE_VALUE, date);
	} else {
	    if (anytime < SMI_EPOCH) {
		printError(parserPtr, ERR_DATE_IN_PAST, date);
	    }
	    if (anytime > time(NULL)) {
		printError(parserPtr, ERR_DATE_IN_FUTURE, date);
	    }
	}
    }

    return (anytime == (time_t) -1) ? 0 : anytime;
}
    

#line 494 "parser-smi.y"
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



#define	YYFINAL		610
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
    92,    94,    96,    98,   100,   102,   104,   106,   108,   110,
   112,   114,   116,   118,   120,   122,   124,   126,   128,   130,
   132,   134,   136,   138,   140,   142,   144,   146,   148,   150,
   152,   153,   155,   158,   160,   162,   164,   166,   168,   170,
   172,   174,   176,   178,   180,   182,   185,   186,   191,   193,
   195,   197,   199,   201,   203,   205,   207,   209,   211,   212,
   216,   217,   226,   227,   232,   234,   236,   238,   240,   242,
   244,   246,   248,   250,   252,   254,   255,   266,   268,   272,
   274,   279,   281,   285,   288,   290,   295,   297,   299,   302,
   304,   308,   309,   315,   316,   317,   331,   332,   333,   352,
   353,   356,   357,   358,   370,   375,   376,   378,   382,   384,
   387,   388,   389,   393,   394,   398,   399,   400,   415,   416,
   417,   418,   437,   439,   442,   444,   446,   448,   450,   456,
   462,   464,   466,   468,   470,   471,   475,   476,   480,   482,
   483,   487,   488,   492,   497,   500,   505,   508,   509,   514,
   517,   522,   525,   527,   529,   531,   533,   535,   537,   541,
   544,   547,   551,   554,   556,   558,   560,   563,   565,   568,
   570,   572,   575,   577,   579,   581,   584,   587,   589,   591,
   593,   595,   597,   599,   600,   604,   611,   613,   617,   619,
   623,   625,   627,   629,   631,   635,   637,   641,   642,   648,
   650,   652,   654,   656,   659,   660,   663,   664,   666,   667,
   673,   678,   679,   681,   685,   688,   690,   692,   694,   699,
   700,   702,   706,   708,   709,   711,   715,   717,   719,   722,
   723,   725,   726,   728,   731,   736,   741,   742,   744,   748,
   750,   755,   757,   761,   763,   765,   767,   768,   771,   773,
   776,   778,   782,   784,   789,   796,   798,   800,   803,   808,
   810,   811,   812,   827,   828,   829,   844,   845,   846,   861,
   863,   865,   868,   869,   875,   878,   880,   881,   886,   887,
   889,   893,   895,   897,   898,   900,   903,   905,   907,   912,
   920,   923,   924,   927,   928,   930,   933,   934,   935,   936,
   953,   955,   956,   958,   961,   962,   971,   973,   977,   979,
   982,   984,   986,   987,   989,   992,   993,   994,   995,   996,
   997,  1012,  1015,  1016,  1018,  1023,  1024,  1026,  1030
};

static const short yyrhs[] = {    91,
     0,     0,    92,     0,    91,    92,     0,     0,   104,    93,
    23,     4,    16,    96,    94,   105,    27,     0,    95,     0,
     0,    36,    98,    80,     0,     0,     0,    29,    97,    80,
     0,    99,     0,     0,   100,     0,    99,   100,     0,   101,
    30,   104,     0,   102,     0,   101,    81,   102,     0,     6,
     0,     5,     0,   103,     0,    12,     0,    13,     0,    15,
     0,    17,     0,    19,     0,    20,     0,    21,     0,    22,
     0,    24,     0,    25,     0,    26,     0,    31,     0,    32,
     0,    35,     0,    38,     0,    40,     0,    41,     0,    42,
     0,    44,     0,    45,     0,    46,     0,    47,     0,    48,
     0,    49,     0,    50,     0,    51,     0,    52,     0,    54,
     0,    55,     0,    56,     0,    57,     0,    60,     0,    61,
     0,    62,     0,    63,     0,    64,     0,    67,     0,    69,
     0,    70,     0,    71,     0,    72,     0,    73,     0,    74,
     0,    76,     0,    78,     0,    79,     0,     5,     0,   106,
     0,     0,   107,     0,   106,   107,     0,   115,     0,   113,
     0,   131,     0,   134,     0,   138,     0,   148,     0,   151,
     0,   222,     0,   216,     0,   219,     0,   242,     0,   108,
     0,     1,    82,     0,     0,   110,    43,   109,    27,     0,
    49,     0,    56,     0,    73,     0,    52,     0,    55,     0,
    71,     0,    54,     0,    51,     0,    48,     0,    13,     0,
     0,    18,   112,    82,     0,     0,     6,   114,    53,    33,
     4,    83,   209,    82,     0,     0,   117,   116,     4,   119,
     0,     5,     0,   118,     0,    40,     0,    41,     0,    21,
     0,    32,     0,    76,     0,    72,     0,    61,     0,    22,
     0,   126,     0,     0,    71,   120,   182,    67,   180,    25,
   207,   197,    70,   126,     0,   111,     0,    65,    59,   122,
     0,     5,     0,    65,    83,   124,    82,     0,   125,     0,
   124,    81,   125,     0,     6,   127,     0,   155,     0,    17,
    83,   128,    82,     0,   157,     0,    17,     0,     5,   169,
     0,   129,     0,   128,    81,   129,     0,     0,     6,   130,
    84,     7,    85,     0,     0,     0,     6,   132,    55,   133,
    67,   180,    25,   207,   197,     4,    83,   209,    82,     0,
     0,     0,     6,   135,    56,   136,    70,   126,   183,   145,
    67,   180,   137,   197,   185,   191,     4,    83,   195,    82,
     0,     0,    25,   207,     0,     0,     0,     6,   139,    73,
   140,    28,   209,   141,   144,   197,     4,     7,     0,    77,
    83,   142,    82,     0,     0,   143,     0,   142,    81,   143,
     0,   195,     0,    25,   207,     0,     0,     0,    45,   146,
   184,     0,     0,    12,   147,   184,     0,     0,     0,     6,
   149,    52,   150,   201,    67,   180,    25,   207,   197,     4,
    83,   196,    82,     0,     0,     0,     0,     6,   152,    49,
   153,    42,   208,   154,    60,   207,    19,   207,    25,   207,
   198,     4,    83,   209,    82,     0,   159,     0,   156,   159,
     0,   121,     0,   122,     0,   123,     0,   167,     0,    86,
    14,     7,    87,    34,     0,    86,    75,     7,    87,    34,
     0,   166,     0,   168,     0,   165,     0,    39,     0,     0,
    39,   160,   170,     0,     0,    39,   161,   175,     0,    40,
     0,     0,    40,   162,   170,     0,     0,     5,   163,   175,
     0,   104,    88,     5,   175,     0,     5,   170,     0,   104,
    88,     5,   170,     0,    58,    68,     0,     0,    58,    68,
   164,   171,     0,     5,   171,     0,   104,    88,     5,   171,
     0,    53,    33,     0,     7,     0,     8,     0,     9,     0,
    10,     0,     6,     0,    11,     0,    83,   213,    82,     0,
    39,   169,     0,    40,   169,     0,    58,    68,   169,     0,
    53,    33,     0,    41,     0,    21,     0,    32,     0,    32,
   170,     0,    76,     0,    76,   170,     0,    72,     0,    61,
     0,    61,   171,     0,    22,     0,    41,     0,    21,     0,
    32,   169,     0,    76,   169,     0,    72,     0,    61,     0,
    22,     0,   170,     0,   171,     0,   175,     0,     0,    84,
   172,    85,     0,    84,    66,    84,   172,    85,    85,     0,
   173,     0,   172,    89,   173,     0,   174,     0,   174,     3,
   174,     0,     8,     0,     7,     0,    10,     0,     9,     0,
    83,   176,    82,     0,   177,     0,   176,    81,   177,     0,
     0,     6,   178,    84,   179,    85,     0,     7,     0,     8,
     0,     6,     0,     6,     0,    26,   207,     0,     0,    74,
   207,     0,     0,     6,     0,     0,    38,   186,    83,   187,
    82,     0,    15,    83,   190,    82,     0,     0,   188,     0,
   187,    81,   188,     0,    35,   189,     0,   189,     0,   195,
     0,   195,     0,    24,    83,   192,    82,     0,     0,   158,
     0,    83,   193,    82,     0,   194,     0,     0,     6,     0,
   194,    81,     6,     0,   209,     0,   209,     0,    63,   207,
     0,     0,   199,     0,     0,   200,     0,   199,   200,     0,
    64,   208,    25,   207,     0,    57,    83,   202,    82,     0,
     0,   203,     0,   202,    81,   203,     0,   195,     0,    50,
    83,   205,    82,     0,   206,     0,   205,    81,   206,     0,
   196,     0,    11,     0,    11,     0,     0,   210,   211,     0,
   212,     0,   211,   212,     0,     6,     0,   104,    88,     6,
     0,     7,     0,     6,    84,     7,    85,     0,   104,    88,
     6,    84,     7,    85,     0,   214,     0,   215,     0,   214,
   215,     0,     6,    84,     7,    85,     0,     7,     0,     0,
     0,     6,   217,    54,   218,   201,    67,   180,    25,   207,
   197,     4,    83,   209,    82,     0,     0,     0,     6,   220,
    51,   221,   204,    67,   180,    25,   207,   197,     4,    83,
   209,    82,     0,     0,     0,     6,   223,    48,   224,    67,
   180,    25,   207,   197,   225,     4,    83,   209,    82,     0,
   226,     0,   227,     0,   226,   227,     0,     0,    47,   229,
   228,   230,   233,     0,     5,   209,     0,     5,     0,     0,
    44,    83,   231,    82,     0,     0,   232,     0,   231,    81,
   232,     0,   209,     0,   234,     0,     0,   235,     0,   234,
   235,     0,   236,     0,   237,     0,    31,   209,    25,   207,
     0,    53,   195,   238,   239,   241,    25,   207,     0,    70,
   126,     0,     0,    79,   240,     0,     0,   126,     0,    46,
   184,     0,     0,     0,     0,     6,   243,    13,   244,    62,
   207,    67,   181,    25,   207,   197,   245,     4,    83,   209,
    82,     0,   246,     0,     0,   247,     0,   246,   247,     0,
     0,    69,   251,   248,    37,    83,   249,    82,   252,     0,
   250,     0,   249,    81,   250,     0,   209,     0,     5,   209,
     0,     5,     0,   253,     0,     0,   254,     0,   253,   254,
     0,     0,     0,     0,     0,     0,    78,   195,   255,   238,
   256,   239,   257,   260,   262,   258,   191,   259,    25,   207,
     0,    12,   261,     0,     0,     6,     0,    20,    83,   263,
    82,     0,     0,   264,     0,   263,    81,   264,     0,   195,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   747,   752,   758,   760,   768,   803,   822,   824,   828,   838,
   840,   849,   852,   854,   859,   861,   865,   906,   908,   916,
   922,   928,   946,   947,   948,   949,   950,   951,   952,   953,
   954,   955,   956,   957,   958,   959,   960,   961,   962,   963,
   964,   965,   966,   967,   968,   969,   970,   971,   972,   973,
   974,   975,   976,   977,   978,   979,   980,   981,   982,   983,
   984,   985,   986,   987,   988,   989,   990,   991,   996,  1008,
  1010,  1015,  1017,  1021,  1026,  1031,  1036,  1041,  1046,  1051,
  1056,  1061,  1066,  1071,  1076,  1081,  1094,  1118,  1125,  1126,
  1127,  1128,  1129,  1130,  1131,  1132,  1133,  1134,  1137,  1154,
  1163,  1179,  1201,  1206,  1274,  1278,  1299,  1300,  1301,  1302,
  1303,  1304,  1305,  1306,  1309,  1325,  1344,  1377,  1385,  1399,
  1440,  1448,  1455,  1475,  1506,  1510,  1530,  1535,  1540,  1573,
  1580,  1594,  1607,  1618,  1625,  1641,  1669,  1676,  1696,  1790,
  1799,  1805,  1812,  1830,  1882,  1886,  1892,  1899,  1913,  1919,
  1921,  1925,  1934,  1935,  1944,  1947,  1954,  1968,  1997,  2004,
  2027,  2031,  2066,  2070,  2086,  2092,  2098,  2104,  2121,  2123,
  2131,  2136,  2153,  2162,  2174,  2179,  2195,  2200,  2212,  2230,
  2235,  2255,  2260,  2306,  2346,  2396,  2438,  2443,  2448,  2459,
  2507,  2546,  2553,  2561,  2568,  2595,  2619,  2627,  2646,  2678,
  2682,  2700,  2704,  2710,  2718,  2726,  2734,  2757,  2777,  2804,
  2812,  2822,  2846,  2860,  2868,  2876,  2884,  2904,  2912,  2922,
  2932,  2939,  2946,  2953,  2961,  2971,  2981,  2988,  3002,  3010,
  3021,  3028,  3035,  3059,  3087,  3093,  3100,  3114,  3127,  3137,
  3148,  3158,  3192,  3208,  3212,  3218,  3222,  3228,  3276,  3284,
  3291,  3300,  3306,  3313,  3327,  3332,  3338,  3348,  3354,  3356,
  3361,  3363,  3384,  3386,  3390,  3397,  3411,  3417,  3423,  3425,
  3429,  3431,  3443,  3445,  3449,  3472,  3476,  3482,  3489,  3503,
  3509,  3515,  3522,  3536,  3542,  3548,  3554,  3558,  3564,  3569,
  3576,  3664,  3757,  3781,  3810,  3842,  3846,  3848,  3852,  3854,
  3858,  3865,  3879,  3909,  3916,  3931,  3962,  3969,  3983,  4065,
  4071,  4075,  4110,  4126,  4139,  4147,  4154,  4161,  4165,  4171,
  4178,  4192,  4215,  4221,  4229,  4233,  4300,  4306,  4314,  4336,
  4365,  4374,  4380,  4389,  4395,  4401,  4405,  4411,  4418,  4432,
  4468,  4470,  4474,  4476,  4480,  4495,  4507,  4514,  4528,  4534,
  4542,  4551,  4553,  4557,  4559,  4563,  4572,  4579,  4586,  4594,
  4600,  4608,  4610,  4614,  4679,  4681,  4685,  4687,  4691
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
   103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   103,   103,   103,   103,   103,   103,   104,   105,
   105,   106,   106,   107,   107,   107,   107,   107,   107,   107,
   107,   107,   107,   107,   107,   107,   109,   108,   110,   110,
   110,   110,   110,   110,   110,   110,   110,   110,   112,   111,
   114,   113,   116,   115,   117,   117,   118,   118,   118,   118,
   118,   118,   118,   118,   119,   120,   119,   119,   121,   122,
   123,   124,   124,   125,   126,   126,   127,   127,   127,   128,
   128,   130,   129,   132,   133,   131,   135,   136,   134,   137,
   137,   139,   140,   138,   141,   141,   142,   142,   143,   144,
   144,   146,   145,   147,   145,   149,   150,   148,   152,   153,
   154,   151,   155,   155,   155,   155,   155,   155,   156,   156,
   157,   157,   158,   159,   160,   159,   161,   159,   159,   162,
   159,   163,   159,   159,   159,   159,   159,   164,   159,   159,
   159,   159,   165,   165,   165,   165,   165,   165,   165,   166,
   166,   166,   166,   167,   167,   167,   167,   167,   167,   167,
   167,   167,   167,   168,   168,   168,   168,   168,   168,   168,
   169,   169,   169,   169,   170,   171,   172,   172,   173,   173,
   174,   174,   174,   174,   175,   176,   176,   178,   177,   179,
   179,   180,   181,   182,   182,   183,   183,   184,   186,   185,
   185,   185,   187,   187,   188,   188,   189,   190,   191,   191,
   192,   192,   193,   193,   194,   194,   195,   196,   197,   197,
   198,   198,   199,   199,   200,   201,   201,   202,   202,   203,
   204,   205,   205,   206,   207,   208,   210,   209,   211,   211,
   212,   212,   212,   212,   212,   213,   214,   214,   215,   215,
   217,   218,   216,   220,   221,   219,   223,   224,   222,   225,
   226,   226,   228,   227,   229,   229,   229,   230,   230,   231,
   231,   232,   233,   233,   234,   234,   235,   235,   236,   237,
   238,   238,   239,   239,   240,   241,   241,   243,   244,   242,
   245,   245,   246,   246,   248,   247,   249,   249,   250,   251,
   251,   252,   252,   253,   253,   255,   256,   257,   258,   259,
   254,   260,   260,   261,   262,   262,   263,   263,   264
};

static const short yyr2[] = {     0,
     1,     0,     1,     2,     0,     9,     1,     0,     3,     0,
     0,     3,     1,     0,     1,     2,     3,     1,     3,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     0,     1,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     0,     4,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     0,     3,
     0,     8,     0,     4,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     0,    10,     1,     3,     1,
     4,     1,     3,     2,     1,     4,     1,     1,     2,     1,
     3,     0,     5,     0,     0,    13,     0,     0,    18,     0,
     2,     0,     0,    11,     4,     0,     1,     3,     1,     2,
     0,     0,     3,     0,     3,     0,     0,    14,     0,     0,
     0,    18,     1,     2,     1,     1,     1,     1,     5,     5,
     1,     1,     1,     1,     0,     3,     0,     3,     1,     0,
     3,     0,     3,     4,     2,     4,     2,     0,     4,     2,
     4,     2,     1,     1,     1,     1,     1,     1,     3,     2,
     2,     3,     2,     1,     1,     1,     2,     1,     2,     1,
     1,     2,     1,     1,     1,     2,     2,     1,     1,     1,
     1,     1,     1,     0,     3,     6,     1,     3,     1,     3,
     1,     1,     1,     1,     3,     1,     3,     0,     5,     1,
     1,     1,     1,     2,     0,     2,     0,     1,     0,     5,
     4,     0,     1,     3,     2,     1,     1,     1,     4,     0,
     1,     3,     1,     0,     1,     3,     1,     1,     2,     0,
     1,     0,     1,     2,     4,     4,     0,     1,     3,     1,
     4,     1,     3,     1,     1,     1,     0,     2,     1,     2,
     1,     3,     1,     4,     6,     1,     1,     2,     4,     1,
     0,     0,    14,     0,     0,    14,     0,     0,    14,     1,
     1,     2,     0,     5,     2,     1,     0,     4,     0,     1,
     3,     1,     1,     0,     1,     2,     1,     1,     4,     7,
     2,     0,     2,     0,     1,     2,     0,     0,     0,    16,
     1,     0,     1,     2,     0,     8,     1,     3,     1,     2,
     1,     1,     0,     1,     2,     0,     0,     0,     0,     0,
    14,     2,     0,     1,     4,     0,     1,     3,     1
};

static const short yydefact[] = {     2,
    69,     1,     3,     5,     4,     0,     0,     0,    10,    11,
     8,     0,    14,     0,     7,    12,    21,    20,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,     0,    13,    15,     0,    18,    22,
     0,   105,   101,    98,   109,   114,   110,   107,   108,    97,
    89,    96,    92,    95,    93,    90,   113,    94,   112,    91,
   111,     0,     0,    72,    85,     0,    75,    74,   103,   106,
    76,    77,    78,    79,    80,    82,    83,    81,    84,     9,
    16,     0,     0,    86,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     6,    73,    87,     0,    17,    19,
     0,   135,   138,   143,   157,   160,   302,   305,   308,   339,
     0,     0,     0,     0,     0,     0,   277,     0,   277,     0,
     0,     0,    88,   120,     0,    99,   205,   213,   206,   174,
   179,   204,     0,     0,   211,     0,   116,   210,   208,     0,
     0,   118,   104,   165,   166,   167,   115,   125,     0,   163,
   168,     0,     0,     0,   287,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   185,   190,     0,     0,     0,
   207,     0,     0,     0,   192,   187,     0,   212,     0,     0,
   245,   209,     0,     0,     0,    69,   164,   287,   242,     0,
   247,   146,     0,   287,     0,   286,   161,     0,   287,     0,
     0,   285,     0,   232,   231,   234,   233,     0,     0,   227,
   229,     0,   183,   132,     0,   130,   100,   176,   178,   181,
     0,   120,   119,     0,     0,   122,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   151,   291,   293,     0,
   288,   289,   280,     0,   278,   267,     0,     0,     0,   284,
     0,   282,   268,     0,     0,     0,     0,   225,     0,     0,
   238,     0,   236,     0,     0,   126,   189,   224,   128,   215,
   220,   224,   224,   224,   214,     0,     0,   219,   218,   224,
   124,   127,   171,   172,     0,   121,   244,     0,     0,     0,
   186,   191,   184,   102,   270,   246,   154,   152,     0,   287,
     0,   270,     0,     0,   290,   287,   276,     0,     0,     0,
   287,   281,     0,   270,   243,     0,     0,   228,   230,     0,
     0,   235,     0,   131,   129,   221,   222,   223,   216,   200,
   201,   203,   224,   217,   123,     0,   169,   170,     0,     0,
     0,     0,     0,     0,   147,   149,   150,     0,     0,   292,
   279,   270,     0,   270,   283,   270,     0,     0,     0,     0,
   237,     0,   202,     0,   269,     0,   248,   155,   153,   140,
   287,   145,     0,   294,     0,     0,     0,     0,     0,   317,
     0,   310,   311,   270,   226,   240,   241,     0,   133,   270,
   287,     0,   270,   148,   144,     0,     0,     0,     0,     0,
   316,   313,     0,   312,   342,   239,     0,     0,   141,   252,
   295,   287,     0,   287,   287,   315,   319,   287,     0,     0,
   341,   343,     0,   136,     0,   249,   260,     0,   272,     0,
     0,     0,   324,     0,   287,   345,     0,   344,   117,   287,
     0,     0,     0,   158,     0,     0,   271,   273,   303,   306,
   287,   287,   287,   314,   323,   325,   327,   328,   309,   350,
     0,   287,     0,   258,   287,     0,     0,     0,     0,   274,
   322,     0,   320,     0,   332,   326,     0,     0,   251,   287,
     0,   253,   256,   257,   197,   193,   194,   195,   196,   198,
   264,   261,   173,     0,   287,     0,   287,   287,   318,     0,
     0,   334,   287,   340,   255,   287,   250,   265,   300,     0,
   263,     0,   296,   297,   259,     0,   275,     0,   321,   329,
   331,     0,   337,   349,     0,   347,   254,     0,   262,     0,
   199,     0,   298,   139,   162,   335,   333,     0,     0,   287,
   353,     0,   266,   336,     0,   348,   287,   346,   352,   354,
   299,   330,   356,   355,   332,   357,   334,   358,   363,     0,
   366,   364,   362,     0,   359,   287,   260,   369,     0,   367,
   360,   287,   365,     0,   368,     0,   361,     0,     0,     0
};

static const short yydefgoto[] = {   608,
     2,     3,     6,    14,    15,    11,    12,    65,    66,    67,
    68,    69,    70,   171,    92,    93,    94,    95,   141,    96,
   172,   199,    97,   115,    98,   128,    99,   100,   173,   211,
   174,   175,   176,   255,   256,   177,   311,   245,   246,   294,
   101,   116,   144,   102,   117,   145,   423,   103,   118,   146,
   267,   374,   375,   332,   329,   372,   371,   104,   119,   147,
   105,   120,   148,   278,   178,   179,   312,   522,   180,   202,
   203,   204,   195,   251,   523,   313,   181,   314,   355,   356,
   357,   239,   240,   241,   358,   292,   293,   350,   418,   220,
   346,   258,   265,   398,   457,   471,   511,   512,   513,   493,
   473,   524,   540,   541,   514,   280,   370,   476,   477,   478,
   187,   274,   275,   191,   281,   282,   233,   227,   276,   223,
   271,   272,   542,   543,   544,   106,   121,   149,   107,   122,
   150,   108,   123,   151,   411,   412,   413,   447,   432,   463,
   502,   503,   484,   485,   486,   487,   488,   532,   553,   567,
   569,   109,   124,   152,   450,   451,   452,   491,   555,   556,
   466,   578,   579,   580,   585,   587,   589,   597,   604,   591,
   593,   595,   599,   600
};

static const short yypact[] = {    15,
-32768,    15,-32768,-32768,-32768,     8,    33,    23,    28,-32768,
    31,    -8,   445,   346,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    16,   445,-32768,   -15,-32768,-32768,
     7,-32768,    25,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    72,   383,-32768,-32768,    59,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    15,   445,-32768,    51,    52,    53,    39,    73,    69,
    82,    83,    99,   136,-32768,-32768,-32768,   151,-32768,-32768,
   126,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   130,   105,   174,   113,   119,   164,   145,   158,   145,   153,
   140,   149,-32768,   -37,   129,-32768,-32768,-32768,   132,    37,
   133,-32768,   180,   146,   137,   -40,-32768,-32768,   132,     0,
   134,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    66,-32768,
-32768,   141,   214,   143,-32768,   142,   156,   215,   160,   148,
   165,   214,   222,    45,   154,-32768,-32768,   232,   157,   121,
-32768,   132,   154,   132,-32768,   159,   175,-32768,   235,   236,
   218,-32768,   240,   242,   245,    49,-32768,-32768,-32768,   226,
   181,   179,    77,-32768,   214,-32768,-32768,   214,-32768,   214,
   233,-32768,   195,-32768,-32768,-32768,-32768,   183,   -25,-32768,
   261,   259,-32768,-32768,    58,-32768,-32768,-32768,-32768,-32768,
   137,-32768,-32768,   213,    61,-32768,   222,   201,   182,   191,
    67,   193,   222,   222,     4,   187,   254,   197,-32768,   194,
    77,-32768,-32768,    71,-32768,-32768,   258,   224,   262,-32768,
    80,-32768,-32768,   263,   222,   280,   121,-32768,   121,   121,
-32768,    86,-32768,   206,   232,-32768,-32768,    67,-32768,-32768,
-32768,    67,    67,    67,-32768,   260,   223,-32768,-32768,    67,
-32768,-32768,-32768,-32768,   236,-32768,-32768,   214,   264,   265,
-32768,-32768,-32768,-32768,   229,-32768,-32768,-32768,   227,-32768,
   222,   229,   288,   294,-32768,-32768,-32768,   222,   222,   222,
-32768,-32768,   222,   229,-32768,   276,   -20,-32768,-32768,   220,
   259,-32768,   295,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    67,-32768,-32768,   281,-32768,-32768,   222,   301,
   302,   302,   214,    90,-32768,-32768,-32768,   305,   225,   228,
-32768,   229,   292,   229,-32768,   229,   266,   222,   230,   166,
-32768,   231,-32768,   222,-32768,   234,-32768,-32768,-32768,   289,
-32768,-32768,   311,-32768,   312,   316,   222,   317,   319,   320,
   322,   266,-32768,   229,-32768,-32768,-32768,   239,-32768,   229,
-32768,   222,   229,-32768,-32768,   243,   244,   304,   247,   248,
    87,-32768,   249,-32768,   267,-32768,   268,   251,-32768,     3,
-32768,-32768,   222,-32768,-32768,-32768,   290,-32768,   330,   333,
   267,-32768,   143,-32768,   256,-32768,   321,   272,   277,   274,
   275,   257,     9,   278,   307,-32768,   279,-32768,-32768,-32768,
   282,   283,   342,-32768,   215,   345,   277,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,     9,-32768,-32768,-32768,-32768,-32768,
   324,-32768,   287,-32768,   323,    18,   293,   338,   296,-32768,
-32768,   104,-32768,   339,   300,-32768,   298,   308,-32768,-32768,
   106,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   188,-32768,-32768,   309,-32768,   222,-32768,-32768,-32768,   222,
   143,   303,-32768,-32768,-32768,   323,-32768,   291,-32768,   310,
   318,   326,   192,-32768,-32768,   327,-32768,   329,-32768,-32768,
-32768,   143,   331,-32768,   124,-32768,-32768,   365,-32768,   377,
-32768,   291,-32768,-32768,-32768,-32768,-32768,   302,   368,-32768,
   325,   328,-32768,-32768,   222,-32768,-32768,-32768,   325,-32768,
-32768,-32768,-32768,-32768,   300,-32768,   303,-32768,   394,   406,
   396,-32768,-32768,   337,-32768,-32768,   321,-32768,   128,-32768,
-32768,-32768,-32768,   389,-32768,   222,-32768,   421,   425,-32768
};

static const short yypgoto[] = {-32768,
-32768,   424,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   361,
-32768,   315,-32768,     5,-32768,-32768,   336,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   221,-32768,-32768,   118,  -181,-32768,-32768,   147,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    35,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   269,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -268,  -146,
  -148,   162,   152,   150,  -173,-32768,    92,-32768,-32768,  -180,
-32768,-32768,-32768,  -371,-32768,-32768,-32768,   -91,   -64,-32768,
  -150,-32768,-32768,-32768,  -222,    10,  -323,-32768,-32768,   -24,
   314,-32768,   125,-32768,-32768,   127,  -253,    -3,  -185,-32768,
-32768,   202,-32768,-32768,   -69,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    63,-32768,-32768,-32768,
-32768,   -50,-32768,-32768,    -6,-32768,-32768,  -104,  -105,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,    47,-32768,-32768,   -86,
-32768,-32768,-32768,   -76,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -114
};


#define	YYLAST		524


static const short yytable[] = {   222,
   399,   273,   221,   317,     4,   197,     4,   196,   378,   325,
   326,   231,   201,   213,   112,   327,   208,   455,   209,     1,
   387,   243,   212,   515,   516,   517,   518,   519,   520,   249,
     7,   344,   262,   359,   360,   361,     8,  -338,     9,   482,
   456,   364,   210,   283,   277,  -182,   194,   279,   328,   284,
   -69,   234,   235,   236,   237,   248,    10,   250,   406,   288,
   408,   483,   409,   289,   389,   113,    13,   197,   289,   196,
   216,    16,  -307,  -159,   214,  -304,  -156,   377,  -301,  -134,
  -137,     1,   268,   269,   382,   383,   384,   323,   114,   386,
   435,  -287,  -287,  -287,   393,   110,   437,  -142,   125,   440,
   521,   127,   297,   131,   160,   161,   132,   376,   133,   154,
   238,   134,   322,   273,   321,   395,   129,   136,   163,  -177,
  -175,   155,   156,   164,   135,   157,   158,   234,   235,   236,
   237,  -182,   194,   138,   414,   137,   159,   366,   295,   296,
   420,   315,   316,   160,   161,   162,   139,   154,   140,   242,
   194,   336,   337,   428,   142,   283,   153,   163,   143,   155,
   341,   342,   164,   157,   158,   165,   351,   352,   439,   166,
   401,   402,   416,   417,   159,   167,   168,   182,   376,   183,
   169,   160,   161,   162,   528,   529,   536,   537,   184,   459,
   170,   185,   400,   538,   539,   163,   574,   562,   539,   188,
   164,   186,   190,   165,   570,   571,   192,   166,   602,   603,
   193,   198,   205,   206,   168,   200,  -180,   298,   169,   219,
   207,   215,   225,   218,   224,   226,   228,   270,   170,   299,
   229,   230,   232,   300,   301,   438,   242,   244,   247,   252,
   238,   254,  -188,   257,   302,   446,   259,   494,   260,   261,
   263,   303,   304,   305,   264,   266,   283,   285,   460,   461,
   505,   286,   464,   290,   291,   306,   287,   318,   319,   330,
   307,   469,   547,   308,   324,   270,   550,   320,   331,   490,
   333,   334,   338,   339,   309,   345,   340,   343,   310,   353,
   363,   369,   362,   373,   379,   501,   504,   367,   368,   380,
   388,   392,   546,   390,   396,   394,   508,   397,   403,   404,
   407,   405,   410,   422,   415,   419,   421,   425,   426,   427,
   429,   582,   430,   436,   431,   433,   442,   441,   443,   444,
   445,   448,   454,   462,   465,   449,   467,   453,   470,   481,
   475,   548,   501,  -351,   472,   497,    71,   554,   499,   551,
    72,    73,   607,   474,   583,   479,   480,   510,    74,   489,
   507,   492,   526,   530,   495,   496,    75,    76,   509,   531,
   566,   572,   -71,   598,   558,   525,   568,    77,   527,   598,
   533,   552,   573,    71,   554,    78,    79,    72,    73,   534,
   545,   559,   575,    80,    81,    74,    82,    83,   560,    84,
    85,    86,   577,    75,    76,   590,    87,   561,   564,   -70,
   565,   592,   581,   606,    77,   594,    88,    89,    90,   596,
   609,    91,    78,    79,   610,     5,   111,   130,   126,   253,
    80,    81,   365,    82,    83,   424,    84,    85,    86,   349,
   348,   354,   391,    87,   557,   535,   601,   217,   347,    17,
    18,   458,   500,    88,    89,    90,    19,    20,    91,    21,
   381,    22,   189,    23,    24,    25,    26,   385,    27,    28,
    29,   498,   335,   563,   434,    30,    31,   549,   506,    32,
   586,   588,    33,   576,    34,    35,    36,   605,    37,    38,
    39,    40,    41,    42,    43,    44,    45,   468,    46,    47,
    48,    49,   584,     0,    50,    51,    52,    53,    54,     0,
     0,    55,     0,    56,    57,    58,    59,    60,    61,     0,
    62,     0,    63,    64
};

static const short yycheck[] = {   185,
   372,   224,   184,   257,     0,   154,     2,   154,   332,   263,
   264,   192,   159,    14,    30,    12,   165,    15,    59,     5,
   344,   195,   169,     6,     7,     8,     9,    10,    11,   203,
    23,   285,   218,   302,   303,   304,     4,    13,    16,    31,
    38,   310,    83,   229,   225,    83,    84,   228,    45,   230,
    88,     7,     8,     9,    10,   202,    29,   204,   382,    85,
   384,    53,   386,    89,    85,    81,    36,   216,    89,   216,
     5,    80,    48,    49,    75,    51,    52,   331,    54,    55,
    56,     5,     6,     7,   338,   339,   340,   261,    82,   343,
   414,     5,     6,     7,   363,    80,   420,    73,    27,   423,
    83,    43,   251,    53,    39,    40,    55,   330,    56,     5,
    66,    73,   261,   336,   261,   369,   112,    49,    53,    83,
    84,    17,    18,    58,    52,    21,    22,     7,     8,     9,
    10,    83,    84,    51,   388,    54,    32,   318,    81,    82,
   394,    81,    82,    39,    40,    41,    48,     5,    13,    83,
    84,    81,    82,   407,     4,   341,    27,    53,    33,    17,
    81,    82,    58,    21,    22,    61,    81,    82,   422,    65,
    81,    82,     7,     8,    32,    71,    72,     4,   401,    67,
    76,    39,    40,    41,    81,    82,    81,    82,    70,   443,
    86,    28,   373,     6,     7,    53,   568,     6,     7,    42,
    58,    57,    50,    61,    81,    82,    67,    65,    81,    82,
    62,    83,    33,    68,    72,    84,    84,     5,    76,     6,
    84,    88,    67,    83,    83,    11,    67,   223,    86,    17,
    83,    67,    11,    21,    22,   421,    83,     6,    82,     5,
    66,     6,    84,    26,    32,   431,     7,   470,     7,     5,
    25,    39,    40,    41,    74,    77,   442,    25,   444,   445,
   483,    67,   448,     3,     6,    53,    84,    67,    87,    83,
    58,   453,   526,    61,    82,   271,   530,    87,    25,   465,
    84,    88,    25,    60,    72,     6,    25,    25,    76,    84,
    68,    63,    33,    67,     7,   481,   482,    34,    34,     6,
    25,     7,   525,    84,     4,    25,   492,     6,     4,    85,
    19,    84,    47,    25,    85,    85,    83,     7,     7,     4,
     4,   575,     4,    85,     5,     4,    83,    85,    25,    83,
    83,    83,    82,    44,     5,    69,     4,    70,    83,    83,
    64,   527,   528,    37,    24,     4,     1,   533,     4,   531,
     5,     6,   606,    82,   577,    82,    82,    35,    13,    82,
    37,    83,    25,    25,    83,    83,    21,    22,    82,    70,
   552,     7,    27,   596,    84,    83,    46,    32,    83,   602,
    83,    79,     6,     1,   570,    40,    41,     5,     6,    82,
    82,    82,    25,    48,    49,    13,    51,    52,    81,    54,
    55,    56,    78,    21,    22,    12,    61,    82,    82,    27,
    82,     6,    85,    25,    32,    20,    71,    72,    73,    83,
     0,    76,    40,    41,     0,     2,    66,   113,    93,   209,
    48,    49,   315,    51,    52,   401,    54,    55,    56,   290,
   289,   295,   351,    61,   536,   510,   597,   179,   287,     5,
     6,   442,   477,    71,    72,    73,    12,    13,    76,    15,
   336,    17,   149,    19,    20,    21,    22,   341,    24,    25,
    26,   475,   271,   543,   412,    31,    32,   528,   485,    35,
   585,   587,    38,   570,    40,    41,    42,   602,    44,    45,
    46,    47,    48,    49,    50,    51,    52,   451,    54,    55,
    56,    57,   579,    -1,    60,    61,    62,    63,    64,    -1,
    -1,    67,    -1,    69,    70,    71,    72,    73,    74,    -1,
    76,    -1,    78,    79
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
#line 749 "parser-smi.y"
{
        yyval.err = 0;
    ;
    break;}
case 2:
#line 753 "parser-smi.y"
{
	yyval.err = 0;
    ;
    break;}
case 3:
#line 759 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 4:
#line 761 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 5:
#line 769 "parser-smi.y"
{
			    thisParserPtr->modulePtr = findModuleByName(yyvsp[0].id);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule(yyvsp[0].id,
					      util_strdup(thisParserPtr->path),
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
			        printError(thisParserPtr,
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
#line 808 "parser-smi.y"
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
#line 823 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 8:
#line 825 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 9:
#line 829 "parser-smi.y"
{
			    if (thisModulePtr->export.language != SMI_LANGUAGE_SMIV2)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    yyval.err = 0;
			;
    break;}
case 10:
#line 839 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 11:
#line 841 "parser-smi.y"
{
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_EXPORTS);
			    }
			;
    break;}
case 12:
#line 849 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 13:
#line 853 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 14:
#line 855 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 15:
#line 860 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 16:
#line 862 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 17:
#line 868 "parser-smi.y"
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

			    if (!strcmp(yyvsp[0].id, "SNMPv2-SMI")) {
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
#line 907 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 19:
#line 910 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 20:
#line 917 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 21:
#line 923 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 22:
#line 931 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 69:
#line 997 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 70:
#line 1009 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 71:
#line 1011 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 72:
#line 1016 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 73:
#line 1018 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 74:
#line 1022 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 75:
#line 1027 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 76:
#line 1032 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 77:
#line 1037 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 78:
#line 1042 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 79:
#line 1047 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 80:
#line 1052 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 81:
#line 1057 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 82:
#line 1062 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 83:
#line 1067 "parser-smi.y"
{
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 84:
#line 1072 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 85:
#line 1077 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 86:
#line 1082 "parser-smi.y"
{
			    printError(thisParserPtr, ERR_FLUSH_DECLARATION);
			    yyerrok;
			    yyval.err = 1;
			;
    break;}
case 87:
#line 1096 "parser-smi.y"
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
			        printError(thisParserPtr, ERR_MACRO);
			    }
			;
    break;}
case 88:
#line 1118 "parser-smi.y"
{
			    addMacro(yyvsp[-3].id, 0, thisParserPtr);
			    util_free(yyvsp[-3].id);
			    yyval.err = 0;
                        ;
    break;}
case 89:
#line 1125 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 90:
#line 1126 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 91:
#line 1127 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 92:
#line 1128 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 93:
#line 1129 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 94:
#line 1130 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 95:
#line 1131 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 96:
#line 1132 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 97:
#line 1133 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 98:
#line 1134 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 99:
#line 1138 "parser-smi.y"
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
			        printError(thisParserPtr, ERR_CHOICE);
			    }
			;
    break;}
case 100:
#line 1154 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			;
    break;}
case 101:
#line 1164 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        if (strchr(yyvsp[0].id, '-') &&
				    (strcmp(yyvsp[0].id, "mib-2") ||
				  strcmp(thisModulePtr->export.name, "SNMPv2-SMI"))) {
				    printError(thisParserPtr,
					       ERR_OIDNAME_INCLUDES_HYPHEN,
					       yyvsp[0].id);
				}
			    }
			;
    break;}
case 102:
#line 1181 "parser-smi.y"
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
			    yyval.err = 0;
			;
    break;}
case 103:
#line 1202 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			;
    break;}
case 104:
#line 1207 "parser-smi.y"
{
			    Type *typePtr;
			    
			    if (strlen(yyvsp[-3].id)) {
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
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.minValue.value.unsigned64 = 0ULL;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.basetype = SMI_BASETYPE_UNSIGNED64;
					((Range *)yyvsp[0].typePtr->listPtr->ptr)->export.maxValue.value.unsigned64 = 18446744073709551615ULL;
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
				    setTypeName(yyvsp[0].typePtr, "NetworkAddress");
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
case 105:
#line 1275 "parser-smi.y"
{
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 106:
#line 1279 "parser-smi.y"
{
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
			        printError(thisParserPtr, ERR_TYPE_SMI, yyvsp[0].id);
			    }
			;
    break;}
case 107:
#line 1299 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 108:
#line 1300 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 109:
#line 1301 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 110:
#line 1302 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 111:
#line 1303 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 112:
#line 1304 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 113:
#line 1305 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 114:
#line 1306 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 115:
#line 1310 "parser-smi.y"
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
case 116:
#line 1326 "parser-smi.y"
{
			    Import *importPtr;

			    if (strcmp(thisModulePtr->export.name, "SNMPv2-TC")) {
				importPtr =
				    findImportByName("TEXTUAL-CONVENTION",
						     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				} else {
				    printError(thisParserPtr,
					       ERR_MACRO_NOT_IMPORTED,
					       "TEXTUAL-CONVENTION",
					       "SNMPv2-TC");
				}
			    }
			;
    break;}
case 117:
#line 1348 "parser-smi.y"
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
				if (!checkFormat(yyval.typePtr->export.basetype, yyvsp[-7].text)) {
				    printError(thisParserPtr,
					       ERR_INVALID_FORMAT, yyvsp[-7].text);
				}
				setTypeFormat(yyval.typePtr, yyvsp[-7].text);
			    }
			    setTypeDecl(yyval.typePtr, SMI_DECL_TEXTUALCONVENTION);
			;
    break;}
case 118:
#line 1378 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			;
    break;}
case 119:
#line 1386 "parser-smi.y"
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
case 120:
#line 1405 "parser-smi.y"
{
			    Type *typePtr;
			    Import *importPtr;

			    yyval.typePtr = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->export.name, yyvsp[0].id);
			    if (! yyval.typePtr) {
				importPtr = findImportByName(yyvsp[0].id,
							     thisModulePtr);
				if (!importPtr) {
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
case 121:
#line 1441 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[-1].listPtr);
			;
    break;}
case 122:
#line 1449 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 123:
#line 1457 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 124:
#line 1476 "parser-smi.y"
{
			    Object *objectPtr;
			    Import *importPtr;
			    
			    objectPtr =
			        findObjectByModuleAndName(thisParserPtr->modulePtr,
							  yyvsp[-1].id);

			    if (!objectPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr) {
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
case 125:
#line 1507 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 126:
#line 1513 "parser-smi.y"
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
case 127:
#line 1532 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 128:
#line 1536 "parser-smi.y"
{
			    /* TODO: */
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 129:
#line 1541 "parser-smi.y"
{
			    Type *typePtr;
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->export.name, yyvsp[-1].id);
			    if (! yyval.typePtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr) {
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
case 130:
#line 1574 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 131:
#line 1581 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 132:
#line 1595 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_BITNAME_32, ERR_BITNAME_64);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (strchr(yyvsp[0].id, '-')) {
				    printError(thisParserPtr,
					       ERR_NAMEDBIT_INCLUDES_HYPHEN,
					       yyvsp[0].id);
				}
			    }
			;
    break;}
case 133:
#line 1608 "parser-smi.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id; /* JS: needs strdup? */
			    yyval.namedNumberPtr->export.value.basetype =
				                       SMI_BASETYPE_UNSIGNED32;
			    yyval.namedNumberPtr->export.value.value.unsigned32 = yyvsp[-1].unsigned32;
			    yyval.namedNumberPtr->export.value.format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 134:
#line 1619 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 135:
#line 1625 "parser-smi.y"
{
			    Import *importPtr;

			    if (strcmp(thisModulePtr->export.name, "SNMPv2-SMI")) {
				importPtr = findImportByName("OBJECT-IDENTITY",
							     thisModulePtr);
				if (importPtr) {
				    importPtr->use++;
				} else {
				    printError(thisParserPtr,
					       ERR_MACRO_NOT_IMPORTED,
					       "OBJECT-IDENTITY",
					       "SNMPv2-SMI");
				}
			    }
			;
    break;}
case 136:
#line 1646 "parser-smi.y"
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
case 137:
#line 1670 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 138:
#line 1676 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("OBJECT-TYPE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				if (thisModulePtr->export.language ==
				    SMI_LANGUAGE_SMIV2) {
				    printError(thisParserPtr,
					       ERR_MACRO_NOT_IMPORTED,
					       "OBJECT-TYPE", "SNMPv2-SMI");
				} else {
				    printError(thisParserPtr,
					       ERR_MACRO_NOT_IMPORTED,
					       "OBJECT-TYPE", "RFC-1212");
				}
			    }
			;
    break;}
case 139:
#line 1705 "parser-smi.y"
{
			    Object *objectPtr, *parentPtr;
			    List *listPtr, *newlistPtr;

			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr,
				                            0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, yyvsp[-17].id);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTTYPE);
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
				    newlistPtr = util_malloc(sizeof(List));
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
				    printError(thisParserPtr,
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
case 140:
#line 1791 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				printError(thisParserPtr,
					   ERR_MISSING_DESCRIPTION);
			    }
			    yyval.text = NULL;
			;
    break;}
case 141:
#line 1800 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 142:
#line 1806 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 143:
#line 1812 "parser-smi.y"
{
			    Import *importPtr;
			    
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        printError(thisParserPtr, ERR_TRAP_TYPE);
			    }

			    importPtr = findImportByName("TRAP-TYPE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "TRAP-TYPE", "RFC-1215");
			    }
			;
    break;}
case 144:
#line 1836 "parser-smi.y"
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
				objectPtr = addObject("",
						      objectPtr->nodePtr,
						      0,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
			    }
			    objectPtr = addObject("",
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
case 145:
#line 1883 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 146:
#line 1887 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 147:
#line 1893 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 148:
#line 1900 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 149:
#line 1914 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 150:
#line 1920 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 151:
#line 1922 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 152:
#line 1926 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        printError(thisParserPtr,
					   ERR_MAX_ACCESS_IN_SMIV1);
			    }
			;
    break;}
case 153:
#line 1934 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 154:
#line 1936 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        printError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			;
    break;}
case 155:
#line 1944 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 156:
#line 1948 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 157:
#line 1954 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("NOTIFICATION-TYPE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "NOTIFICATION-TYPE", "SNMPv2-SMI");
			    }
			;
    break;}
case 158:
#line 1973 "parser-smi.y"
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
case 159:
#line 1998 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 160:
#line 2004 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("MODULE-IDENTITY",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "MODULE-IDENTITY", "SNMPv2-SMI");
			    }
			    
			    if (thisParserPtr->modulePtr->numModuleIdentities > 0)
			    {
			        printError(thisParserPtr,
					   ERR_TOO_MANY_MODULE_IDENTITIES);
			    }
			    if (thisParserPtr->modulePtr->numStatements > 0) {
			        printError(thisParserPtr,
					   ERR_MODULE_IDENTITY_NOT_FIRST);
			    }
			;
    break;}
case 161:
#line 2028 "parser-smi.y"
{
			    setModuleLastUpdated(thisParserPtr->modulePtr, yyvsp[0].date);
			;
    break;}
case 162:
#line 2037 "parser-smi.y"
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
case 163:
#line 2067 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 164:
#line 2071 "parser-smi.y"
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
			        printError(thisParserPtr, ERR_TYPE_TAG, yyvsp[-1].err);
			    }
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 165:
#line 2087 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 166:
#line 2093 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 167:
#line 2099 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 168:
#line 2105 "parser-smi.y"
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
case 169:
#line 2122 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 170:
#line 2124 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 171:
#line 2132 "parser-smi.y"
{ yyval.typePtr = yyvsp[0].typePtr; ;
    break;}
case 172:
#line 2137 "parser-smi.y"
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
case 173:
#line 2154 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 174:
#line 2163 "parser-smi.y"
{
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				printError(thisParserPtr,
					   ERR_INTEGER_IN_SMIV2);

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 175:
#line 2175 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 176:
#line 2179 "parser-smi.y"
{
			    List *p;
			    
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				printError(thisParserPtr,ERR_INTEGER_IN_SMIV2);

			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 177:
#line 2196 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 178:
#line 2200 "parser-smi.y"
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
case 179:
#line 2213 "parser-smi.y"
{
			    Import *importPtr;

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_BASETYPE_NOT_IMPORTED,
					   "Integer32");
			    }

			    /* TODO: any need to distinguish from INTEGER? */
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 180:
#line 2231 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			;
    break;}
case 181:
#line 2235 "parser-smi.y"
{
			    Import *importPtr;
			    List *p;
			    
			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
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
case 182:
#line 2256 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_ENUM;
			;
    break;}
case 183:
#line 2260 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModuleAndName(
			        thisParserPtr->modulePtr, yyvsp[-2].id);
			    if (!parentPtr) {
			        importPtr = findImportByName(yyvsp[-2].id,
							     thisModulePtr);
				if (!importPtr) {
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
case 184:
#line 2308 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_ENUM;
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (!importPtr) {
				    printError(thisParserPtr,
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
case 185:
#line 2347 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByName(yyvsp[-1].id,
							     thisModulePtr);
				if (!importPtr) {
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
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				}
			    } else {
				defaultBasetype = parentPtr->export.basetype;
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
case 186:
#line 2398 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (!importPtr) {
				    defaultBasetype = SMI_BASETYPE_UNKNOWN;
				    printError(thisParserPtr,
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
				    defaultBasetype = SMI_BASETYPE_INTEGER32;
				}
			    } else {
				defaultBasetype = parentPtr->export.basetype;
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
case 187:
#line 2439 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 188:
#line 2444 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			;
    break;}
case 189:
#line 2448 "parser-smi.y"
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
case 190:
#line 2460 "parser-smi.y"
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
				if (!importPtr) {
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
case 191:
#line 2509 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    parentPtr = findTypeByModulenameAndName(yyvsp[-3].id, yyvsp[-1].id);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName(yyvsp[-3].id,
							  yyvsp[-1].id, thisModulePtr);
				if (!importPtr) {
				    printError(thisParserPtr,
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
case 192:
#line 2547 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 193:
#line 2555 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 194:
#line 2562 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 195:
#line 2569 "parser-smi.y"
{
			    char s[9];
			    int i, len, j;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_BINSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr =
				    util_malloc((len+7)/8+1);
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
case 196:
#line 2596 "parser-smi.y"
{
			    char s[3];
			    int i, len;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_HEXSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr = util_malloc((len+1)/2+1);
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
case 197:
#line 2620 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->basetype = defaultBasetype;
			    yyval.valuePtr->value.ptr = yyvsp[0].id;	/* JS: needs strdup? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NAME;
			;
    break;}
case 198:
#line 2628 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.valuePtr->value.ptr = util_strdup(yyvsp[0].text);
			    yyval.valuePtr->format = SMI_VALUEFORMAT_TEXT;
			    yyval.valuePtr->len = strlen(yyvsp[0].text);
			;
    break;}
case 199:
#line 2653 "parser-smi.y"
{
			    /*
			     * SMIv1 allows something like { 0 0 } !
			     * SMIv2 does not!
			     */
			    /* TODO: make it work correctly for SMIv1 */
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				printError(thisParserPtr,
					   ERR_OID_DEFVAL_TOO_LONG);
			    }
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_OID;
			    yyval.valuePtr->len = 2;
			    yyval.valuePtr->value.oid = util_malloc(2 * sizeof(SmiSubid));
			    yyval.valuePtr->value.oid[0] = 0;
			    yyval.valuePtr->value.oid[1] = 0;
			;
    break;}
case 200:
#line 2679 "parser-smi.y"
{
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 201:
#line 2683 "parser-smi.y"
{
			    Import *importPtr;
			    
			    /* TODO: any need to distinguish from INTEGER? */
			    yyval.typePtr = typeInteger32Ptr;

			    importPtr = findImportByName("Integer32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_BASETYPE_NOT_IMPORTED,
					   "Integer32");
			    }

			;
    break;}
case 202:
#line 2701 "parser-smi.y"
{
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 203:
#line 2705 "parser-smi.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 204:
#line 2711 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "IpAddress");
			    }
			;
    break;}
case 205:
#line 2719 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter32");
			    }
			;
    break;}
case 206:
#line 2727 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			;
    break;}
case 207:
#line 2735 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByName("Gauge32");
			    if (! parentPtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
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
case 208:
#line 2758 "parser-smi.y"
{
			    Import *importPtr;

			    yyval.typePtr = findTypeByName("Unsigned32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Unsigned32");
			    }
			    
			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_BASETYPE_NOT_IMPORTED,
					   "Unsigned32");
			    }
			;
    break;}
case 209:
#line 2778 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByName("Unsigned32");
			    if (! parentPtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
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
				printError(thisParserPtr,
					   ERR_BASETYPE_NOT_IMPORTED,
					   "Unsigned32");
			    }
			;
    break;}
case 210:
#line 2805 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "TimeTicks");
			    }
			;
    break;}
case 211:
#line 2813 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Opaque");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Opaque");
			    } else {
				printError(thisParserPtr, ERR_OPAQUE_OBSOLETE);
			    }
			;
    break;}
case 212:
#line 2823 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    parentPtr = findTypeByName("Opaque");
			    if (! parentPtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Opaque");
			    }
			    printError(thisParserPtr, ERR_OPAQUE_OBSOLETE);
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
case 213:
#line 2847 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter64");
			    }
			;
    break;}
case 214:
#line 2861 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "IpAddress");
			    }
			;
    break;}
case 215:
#line 2869 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter32");
			    }
			;
    break;}
case 216:
#line 2877 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			;
    break;}
case 217:
#line 2885 "parser-smi.y"
{
			    Import *importPtr;
			    
			    yyval.typePtr = findTypeByName("Unsigned32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Unsigned32");
			    }

			    importPtr = findImportByName("Unsigned32",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_BASETYPE_NOT_IMPORTED,
					   "Unsigned32");
			    }
			;
    break;}
case 218:
#line 2905 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "TimeTicks");
			    }
			;
    break;}
case 219:
#line 2913 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Opaque");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Opaque");
			    } else {
				printError(thisParserPtr, ERR_OPAQUE_OBSOLETE);
			    }
			;
    break;}
case 220:
#line 2923 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter64");
			    }
			;
    break;}
case 221:
#line 2933 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				printError(thisParserPtr,
					   ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 222:
#line 2940 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				printError(thisParserPtr,
					   ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 223:
#line 2947 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				printError(thisParserPtr,
					   ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 224:
#line 2954 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 225:
#line 2968 "parser-smi.y"
{ yyval.listPtr = yyvsp[-1].listPtr; ;
    break;}
case 226:
#line 2978 "parser-smi.y"
{ yyval.listPtr = yyvsp[-2].listPtr; ;
    break;}
case 227:
#line 2982 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 228:
#line 2989 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = (void *)yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 229:
#line 3003 "parser-smi.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    /* TODO: success? */
			    yyval.rangePtr->export.minValue = *yyvsp[0].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    util_free(yyvsp[0].valuePtr);
			;
    break;}
case 230:
#line 3011 "parser-smi.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    /* TODO: success? */
			    yyval.rangePtr->export.minValue = *yyvsp[-2].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			    util_free(yyvsp[0].valuePtr);
			;
    break;}
case 231:
#line 3022 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 232:
#line 3029 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 233:
#line 3036 "parser-smi.y"
{
			    char s[3];
			    int i, len;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_HEXSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr = util_malloc((len+1)/2+1);
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
case 234:
#line 3060 "parser-smi.y"
{
			    char s[9];
			    int i, len, j;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_BINSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				len = strlen(yyvsp[0].text);
				yyval.valuePtr->value.ptr = util_malloc((len+7)/8+1);
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
case 235:
#line 3088 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 236:
#line 3094 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 237:
#line 3101 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 238:
#line 3115 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_ENUMNAME_32, ERR_ENUMNAME_64);
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				if (strchr(yyvsp[0].id, '-')) {
				    printError(thisParserPtr,
					       ERR_NAMEDNUMBER_INCLUDES_HYPHEN,
					       yyvsp[0].id);
				}
			    }
			;
    break;}
case 239:
#line 3128 "parser-smi.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    /* TODO: success? */
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id; /* JS: needs strdup? */
			    yyval.namedNumberPtr->export.value = *yyvsp[-1].valuePtr;
			    util_free(yyvsp[-1].valuePtr);
			;
    break;}
case 240:
#line 3138 "parser-smi.y"
{
			    if (yyvsp[0].unsigned32 > MAX_INTEGER32) {
				printError(thisParserPtr,
					   ERR_INTEGER32_TOO_LARGE, yyvsp[0].unsigned32);
			    }
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].unsigned32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 241:
#line 3149 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    /* TODO: non-negative is suggested */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 242:
#line 3159 "parser-smi.y"
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
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV2_STATUS, yyvsp[0].id);
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
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV1_STATUS, yyvsp[0].id);
				    yyval.status = SMI_STATUS_UNKNOWN;
				}
			    }
			    util_free(yyvsp[0].id);
			;
    break;}
case 243:
#line 3193 "parser-smi.y"
{
			    if (!strcmp(yyvsp[0].id, "current")) {
				yyval.status = SMI_STATUS_CURRENT;
			    } else if (!strcmp(yyvsp[0].id, "obsolete")) {
				yyval.status = SMI_STATUS_OBSOLETE;
			    } else {
				printError(thisParserPtr,
					   ERR_INVALID_CAPABILITIES_STATUS,
					   yyvsp[0].id);
				yyval.status = SMI_STATUS_UNKNOWN;
			    }
			    util_free(yyvsp[0].id);
			;
    break;}
case 244:
#line 3209 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 245:
#line 3213 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 246:
#line 3219 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 247:
#line 3223 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 248:
#line 3229 "parser-smi.y"
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
				    printError(thisParserPtr,
					       ERR_SMIV2_WRITE_ONLY);
				    yyval.access = SMI_ACCESS_READ_WRITE;
				} else {
				    printError(thisParserPtr,
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
				    printError(thisParserPtr,
					       ERR_SMIV1_WRITE_ONLY);
				    yyval.access = SMI_ACCESS_READ_WRITE;
				} else {
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV1_ACCESS, yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				}
			    }
			    util_free(yyvsp[0].id);
			;
    break;}
case 249:
#line 3277 "parser-smi.y"
{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			;
    break;}
case 250:
#line 3285 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = yyvsp[-1].listPtr;
			    yyval.index.rowPtr    = NULL;
			;
    break;}
case 251:
#line 3294 "parser-smi.y"
{
			    yyval.index.indexkind    = SMI_INDEX_AUGMENT;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
			;
    break;}
case 252:
#line 3301 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;
    break;}
case 253:
#line 3307 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 254:
#line 3315 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 255:
#line 3328 "parser-smi.y"
{
			    impliedFlag = 1;
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 256:
#line 3333 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 257:
#line 3343 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 258:
#line 3349 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 259:
#line 3355 "parser-smi.y"
{ yyval.valuePtr = yyvsp[-1].valuePtr; ;
    break;}
case 260:
#line 3357 "parser-smi.y"
{ yyval.valuePtr = NULL; ;
    break;}
case 261:
#line 3362 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 262:
#line 3364 "parser-smi.y"
{
			    int i = 0;
			    List *listPtr;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
			    yyval.valuePtr->value.bits = NULL;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    for (i = 0, listPtr = yyvsp[-1].listPtr; listPtr;
				 i++, listPtr = listPtr->nextPtr) {
				yyval.valuePtr->value.bits = util_realloc(yyval.valuePtr->value.bits,
						       sizeof(char *) * (i+2));
				yyval.valuePtr->value.bits[i] = listPtr->ptr;
				yyval.valuePtr->value.bits[i+1] = NULL;
				/* XXX util_free(listPtr); */
			    }
			;
    break;}
case 263:
#line 3385 "parser-smi.y"
{ yyval.listPtr = yyvsp[0].listPtr; ;
    break;}
case 264:
#line 3387 "parser-smi.y"
{ yyval.listPtr = NULL; ;
    break;}
case 265:
#line 3391 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].id; /* JS: needs strdup? */
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 266:
#line 3398 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = yyvsp[0].id; /* JS: needs strdup? */
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 267:
#line 3412 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 268:
#line 3418 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 269:
#line 3424 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 270:
#line 3426 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 271:
#line 3430 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 272:
#line 3432 "parser-smi.y"
{
			    if (!thisModulePtr->firstRevisionPtr) {
				addRevision(thisModulePtr->lastUpdated,
					    util_strdup(
	           "[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }
			    yyval.err = 0;
			;
    break;}
case 273:
#line 3444 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 274:
#line 3446 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 275:
#line 3451 "parser-smi.y"
{
			    /*
			     * If the first REVISION (which is the newest)
			     * has another date than the LAST-UPDATED clause,
			     * we add an implicit Revision structure.
			     */
			    if ((!thisModulePtr->firstRevisionPtr) &&
				(yyvsp[-2].date != thisModulePtr->lastUpdated)) {
				addRevision(thisModulePtr->lastUpdated,
					    util_strdup(
	           "[Revision added by libsmi due to a LAST-UPDATED clause.]"),
					    thisParserPtr);
			    }
			    
			    if (addRevision(yyvsp[-2].date, yyvsp[0].text, thisParserPtr))
				yyval.err = 0;
			    else
				yyval.err = -1;
			;
    break;}
case 276:
#line 3473 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 277:
#line 3477 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 278:
#line 3483 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 279:
#line 3490 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 280:
#line 3504 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 281:
#line 3510 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 282:
#line 3516 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 283:
#line 3523 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 284:
#line 3537 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 285:
#line 3543 "parser-smi.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 286:
#line 3549 "parser-smi.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 287:
#line 3554 "parser-smi.y"
{
			    parentNodePtr = rootNodePtr;
			;
    break;}
case 288:
#line 3558 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    parentNodePtr = yyvsp[0].objectPtr->nodePtr;
			;
    break;}
case 289:
#line 3566 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 290:
#line 3571 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 291:
#line 3578 "parser-smi.y"
{
			    Object *objectPtr;
			    SmiNode *snodePtr;
			    Import *importPtr;
			    
			    if (parentNodePtr != rootNodePtr) {
				printError(thisParserPtr,
					   ERR_OIDLABEL_NOT_FIRST, yyvsp[0].id);
			    } else {
				objectPtr = findObjectByModuleAndName(
				    thisParserPtr->modulePtr, yyvsp[0].id);
				if (objectPtr) {
				    yyval.objectPtr = objectPtr;
				} else {
				    importPtr = findImportByName(yyvsp[0].id,
							       thisModulePtr);
				    if (!importPtr) {
					/*
					 * If we are in a MODULE-COMPLIANCE
					 * statement with a given MODULE...
					 */
					if (complianceModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    complianceModulePtr, yyvsp[0].id);
					    if (objectPtr) {
						importPtr = addImport(yyvsp[0].id,
								thisParserPtr);
						setImportModulename(importPtr,
						    complianceModulePtr->export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[0].id,
						    pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						printError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[0].id,
					     complianceModulePtr->export.name);
					    }
					} else if (capabilitiesModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    capabilitiesModulePtr, yyvsp[0].id);
					    if (objectPtr) {
						importPtr = addImport(yyvsp[0].id,
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
						printError(thisParserPtr,
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
			    }
			;
    break;}
case 292:
#line 3665 "parser-smi.y"
{
			    Object *objectPtr;
			    SmiNode *snodePtr;
			    Import *importPtr;
			    char *md;
			    
			    if (parentNodePtr != rootNodePtr) {
				md = util_malloc(sizeof(char) *
					        (strlen(yyvsp[-2].id) + strlen(yyvsp[0].id) + 2));
				sprintf(md, "%s.%s", yyvsp[-2].id, yyvsp[0].id);
				printError(thisParserPtr,
					   ERR_OIDLABEL_NOT_FIRST, md);
				util_free(md);
			    } else {
				objectPtr = findObjectByModulenameAndName(
				    yyvsp[-2].id, yyvsp[0].id);
				if (objectPtr) {
				    yyval.objectPtr = objectPtr;
				} else {
				    importPtr = findImportByModulenameAndName(
					yyvsp[-2].id, yyvsp[0].id, thisModulePtr);
				    if (!importPtr) {
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
						importPtr = addImport(yyvsp[-2].id,
								thisParserPtr);
						setImportModulename(importPtr,
						    complianceModulePtr->export.name);
						importPtr->use++;
					    } else {
						objectPtr = addObject(yyvsp[-2].id,
						    pendingNodePtr, 0,
						    FLAG_INCOMPLETE,
						    thisParserPtr);
						printError(thisParserPtr,
					      ERR_IDENTIFIER_NOT_IN_MODULE, yyvsp[-2].id,
					     complianceModulePtr->export.name);
					    }
					} else if (capabilitiesModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    capabilitiesModulePtr, yyvsp[-2].id);
					    if (objectPtr) {
						importPtr = addImport(yyvsp[-2].id,
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
						printError(thisParserPtr,
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
case 293:
#line 3758 "parser-smi.y"
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
				objectPtr = addObject("",
						      parentNodePtr,
						      yyvsp[0].unsigned32,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				yyval.objectPtr = objectPtr;
			    }
			    parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 294:
#line 3782 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    /* TODO: search in local module and
			     *       in imported modules
			     */
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, yyvsp[-3].id);
			    if (objectPtr) {
				printError(thisParserPtr,
					   ERR_EXISTENT_OBJECT, yyvsp[-3].id);
				yyval.objectPtr = objectPtr;
				if (yyval.objectPtr->nodePtr->subid != yyvsp[-1].unsigned32) {
				    printError(thisParserPtr,
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
case 295:
#line 3811 "parser-smi.y"
{
			    Object *objectPtr;
			    char *md;

			    md = util_malloc(sizeof(char) *
					     (strlen(yyvsp[-5].id) + strlen(yyvsp[-3].id) + 2));
			    sprintf(md, "%s.%s", yyvsp[-5].id, yyvsp[-3].id);
			    objectPtr = findObjectByModulenameAndName(yyvsp[-5].id, yyvsp[-3].id);
			    if (objectPtr) {
				printError(thisParserPtr, ERR_EXISTENT_OBJECT,
					   yyvsp[-5].id);
				yyval.objectPtr = objectPtr;
				if (yyval.objectPtr->nodePtr->subid != yyvsp[-1].unsigned32) {
				    printError(thisParserPtr,
					       ERR_SUBIDENTIFIER_VS_OIDLABEL,
					       yyvsp[-1].unsigned32, md);
				}
			    } else {
				printError(thisParserPtr,
					   ERR_ILLEGALLY_QUALIFIED, md);
				objectPtr = addObject(yyvsp[-3].id, parentNodePtr,
						   yyvsp[-1].unsigned32, 0,
						   thisParserPtr);
				yyval.objectPtr = objectPtr;
			    }
			    util_free(md);
			    if (yyval.objectPtr)
				parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 296:
#line 3843 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 297:
#line 3847 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 298:
#line 3849 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 299:
#line 3853 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 300:
#line 3855 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 301:
#line 3859 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 302:
#line 3865 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("OBJECT-GROUP",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "OBJECT-GROUP", "SNMPv2-CONF");
			    }
			;
    break;}
case 303:
#line 3883 "parser-smi.y"
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
case 304:
#line 3910 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 305:
#line 3916 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("NOTIFICATION-GROUP",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "NOTIFICATION-GROUP",
					   "SNMPv2-CONF");
			    }
			;
    break;}
case 306:
#line 3935 "parser-smi.y"
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
case 307:
#line 3963 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 308:
#line 3969 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("MODULE-COMPLIANCE",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "MODULE-COMPLIANCE",
					   "SNMPv2-CONF");
			    }
			;
    break;}
case 309:
#line 3988 "parser-smi.y"
{
			    Object *objectPtr;
			    Option *optionPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    char *s;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    setObjectName(objectPtr, yyvsp[-13].id);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
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
#if 0 /* export implicitly defined types by the node's lowercase name */
				    s = util_malloc(strlen(refinementPtr->
						      objectPtr->export.name) +
   				                              strlen(yyvsp[-13].id) + 13);
				    if (refinementPtr->typePtr) {
					sprintf(s, "%s+%s+type", yyvsp[-13].id,
					refinementPtr->objectPtr->export.name);
					setTypeName(refinementPtr->typePtr, s);
				    }
				    if (refinementPtr->writetypePtr) {
					sprintf(s, "%s+%s+writetype", yyvsp[-13].id,
					refinementPtr->objectPtr->export.name);
				   setTypeName(refinementPtr->writetypePtr, s);
				    }
				    util_free(s);
#endif
				}
			    }

			    yyval.err = 0;
			;
    break;}
case 310:
#line 4066 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 311:
#line 4072 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 312:
#line 4076 "parser-smi.y"
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
case 313:
#line 4111 "parser-smi.y"
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
case 314:
#line 4127 "parser-smi.y"
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
case 315:
#line 4140 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 316:
#line 4148 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 317:
#line 4156 "parser-smi.y"
{
			    yyval.modulePtr = thisModulePtr;
			;
    break;}
case 318:
#line 4162 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 319:
#line 4166 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 320:
#line 4172 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 321:
#line 4179 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = yyvsp[0].objectPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 322:
#line 4193 "parser-smi.y"
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
case 323:
#line 4216 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			;
    break;}
case 324:
#line 4222 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 325:
#line 4230 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 326:
#line 4234 "parser-smi.y"
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
					printError(thisParserPtr,
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
					printError(thisParserPtr,
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
case 327:
#line 4301 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].listPtr;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 328:
#line 4307 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 329:
#line 4316 "parser-smi.y"
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
			    
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = util_malloc(sizeof(Option));
			    ((Option *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-2].objectPtr;
			    ((Option *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			;
    break;}
case 330:
#line 4341 "parser-smi.y"
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
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->nextPtr = NULL;
			    yyval.listPtr->ptr = util_malloc(sizeof(Refinement));
			    ((Refinement *)(yyval.listPtr->ptr))->objectPtr = yyvsp[-5].objectPtr;
			    ((Refinement *)(yyval.listPtr->ptr))->typePtr = yyvsp[-4].typePtr;
			    ((Refinement *)(yyval.listPtr->ptr))->writetypePtr = yyvsp[-3].typePtr;
			    ((Refinement *)(yyval.listPtr->ptr))->export.access = yyvsp[-2].access;
			    ((Refinement *)(yyval.listPtr->ptr))->export.description = yyvsp[0].text;
			;
    break;}
case 331:
#line 4366 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 332:
#line 4375 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 333:
#line 4381 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 334:
#line 4390 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 335:
#line 4396 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 336:
#line 4402 "parser-smi.y"
{
			    yyval.access = yyvsp[0].access;
			;
    break;}
case 337:
#line 4406 "parser-smi.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 338:
#line 4412 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 339:
#line 4418 "parser-smi.y"
{
			    Import *importPtr;
			    
			    importPtr = findImportByName("AGENT-CAPABILITIES",
							 thisModulePtr);
			    if (importPtr) {
				importPtr->use++;
			    } else {
				printError(thisParserPtr,
					   ERR_MACRO_NOT_IMPORTED,
					   "AGENT-CAPABILITIES",
					   "SNMPv2-CONF");
			    }
			;
    break;}
case 340:
#line 4438 "parser-smi.y"
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
case 341:
#line 4469 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 342:
#line 4471 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 343:
#line 4475 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 344:
#line 4477 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 345:
#line 4481 "parser-smi.y"
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
case 346:
#line 4497 "parser-smi.y"
{
			    if (capabilitiesModulePtr) {
				checkImports(capabilitiesModulePtr,
					     thisParserPtr);
				capabilitiesModulePtr = NULL;
			    }
			    yyval.err = 0;
			;
    break;}
case 347:
#line 4508 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].listPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 348:
#line 4515 "parser-smi.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = yyvsp[0].listPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-2].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 349:
#line 4529 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 350:
#line 4535 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 351:
#line 4543 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 352:
#line 4552 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 353:
#line 4554 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 354:
#line 4558 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 355:
#line 4560 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 356:
#line 4564 "parser-smi.y"
{
			    if (yyvsp[0].objectPtr) {
				variationkind = yyvsp[0].objectPtr->export.nodekind;
			    } else {
				variationkind = SMI_NODEKIND_UNKNOWN;
			    }
			;
    break;}
case 357:
#line 4572 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				printError(thisParserPtr,
					   ERR_NOTIFICATION_VARIATION_SYNTAX);
			    }
			;
    break;}
case 358:
#line 4579 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				printError(thisParserPtr,
				       ERR_NOTIFICATION_VARIATION_WRITESYNTAX);
			    }
			;
    break;}
case 359:
#line 4587 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				printError(thisParserPtr,
					  ERR_NOTIFICATION_VARIATION_CREATION);
			    }
			;
    break;}
case 360:
#line 4594 "parser-smi.y"
{
			    if (variationkind == SMI_NODEKIND_NOTIFICATION) {
				printError(thisParserPtr,
					   ERR_NOTIFICATION_VARIATION_DEFVAL);
			    }
			;
    break;}
case 361:
#line 4601 "parser-smi.y"
{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			    variationkind = SMI_NODEKIND_UNKNOWN;
			;
    break;}
case 362:
#line 4609 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 363:
#line 4611 "parser-smi.y"
{ yyval.access = 0; ;
    break;}
case 364:
#line 4615 "parser-smi.y"
{
			    if (!strcmp(yyvsp[0].id, "not-implemented")) {
				yyval.access = SMI_ACCESS_NOT_IMPLEMENTED;
			    } else if (!strcmp(yyvsp[0].id, "accessible-for-notify")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    printError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_NOTIFY;
				}
			    } else if (!strcmp(yyvsp[0].id, "read-only")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    printError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_ONLY;
				}
			    } else if (!strcmp(yyvsp[0].id, "read-write")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    printError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				}
			    } else if (!strcmp(yyvsp[0].id, "read-create")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    printError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_WRITE;
				}
			    } else if (!strcmp(yyvsp[0].id, "write-only")) {
				if (variationkind ==
				    SMI_NODEKIND_NOTIFICATION) {
				    printError(thisParserPtr,
				     ERR_INVALID_NOTIFICATION_VARIATION_ACCESS,
					       yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				} else {
				    yyval.access = SMI_ACCESS_READ_WRITE; /* TODO */
				    printError(thisParserPtr,
					       ERR_SMIV2_WRITE_ONLY);
				}
			    } else {
				printError(thisParserPtr,
				           ERR_INVALID_VARIATION_ACCESS,
					   yyvsp[0].id);
				yyval.access = SMI_ACCESS_UNKNOWN;
			    }
			;
    break;}
case 365:
#line 4680 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 366:
#line 4682 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 367:
#line 4686 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 368:
#line 4688 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 369:
#line 4692 "parser-smi.y"
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
#line 4695 "parser-smi.y"


#endif
