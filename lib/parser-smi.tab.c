
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
	    objectPtr->export.nodekind = SMI_NODEKIND_MODULE;
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

	if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2
	    && typePtr->export.decl == SMI_DECL_TYPEASSIGNMENT
	    && typePtr->export.basetype != SMI_BASETYPE_UNKNOWN) {
	    printErrorAtLine(parserPtr, ERR_SMIV2_TYPE_ASSIGNEMENT,
			     typePtr->line, typePtr->export.name);
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
		objectPtr->export.value.basetype == SMI_BASETYPE_UNKNOWN;
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
    

#line 463 "parser-smi.y"
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



#define	YYFINAL		543
#define	YYFLAG		-32768
#define	YYNTBASE	90

#define YYTRANSLATE(x) ((unsigned)(x) <= 333 ? yytranslate[x] : 236)

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
   172,   174,   176,   178,   180,   183,   184,   189,   191,   193,
   195,   197,   199,   201,   203,   205,   207,   209,   210,   214,
   215,   224,   225,   230,   232,   234,   236,   238,   240,   242,
   244,   246,   248,   250,   252,   253,   264,   266,   270,   272,
   277,   279,   283,   286,   288,   293,   295,   297,   300,   302,
   306,   307,   313,   314,   315,   329,   330,   331,   350,   351,
   354,   355,   356,   368,   373,   374,   376,   380,   382,   385,
   386,   387,   391,   392,   396,   397,   398,   413,   414,   415,
   416,   435,   437,   440,   442,   444,   446,   448,   454,   460,
   462,   464,   466,   468,   471,   474,   476,   479,   482,   487,
   490,   495,   498,   502,   505,   510,   513,   515,   517,   519,
   521,   523,   525,   529,   532,   535,   539,   542,   544,   546,
   548,   551,   553,   556,   558,   560,   563,   565,   567,   569,
   572,   575,   577,   579,   581,   583,   585,   587,   588,   592,
   599,   601,   605,   607,   611,   613,   615,   617,   619,   623,
   625,   629,   630,   636,   638,   640,   642,   644,   647,   648,
   651,   652,   654,   655,   661,   666,   667,   669,   673,   676,
   678,   680,   682,   687,   688,   690,   694,   696,   697,   699,
   703,   705,   707,   710,   711,   713,   714,   716,   719,   724,
   729,   730,   732,   736,   738,   743,   745,   749,   751,   753,
   755,   756,   759,   761,   764,   766,   770,   772,   777,   784,
   786,   788,   791,   796,   798,   799,   800,   815,   816,   817,
   832,   833,   834,   849,   851,   853,   856,   857,   863,   866,
   868,   869,   874,   875,   877,   881,   883,   885,   886,   888,
   891,   893,   895,   900,   908,   911,   912,   915,   916,   918,
   921,   922,   923,   924,   941,   943,   944,   946,   949,   957,
   959,   963,   965,   967,   969,   970,   972,   973,   975,   978,
   980,   982,   988,   998,  1003,  1004,  1006,  1010
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
     0,   216,     0,   210,     0,   213,     0,   108,     0,     1,
    82,     0,     0,   110,    43,   109,    27,     0,    49,     0,
    56,     0,    73,     0,    52,     0,    55,     0,    71,     0,
    54,     0,    51,     0,    48,     0,    13,     0,     0,    18,
   112,    82,     0,     0,     6,   114,    53,    33,     4,    83,
   203,    82,     0,     0,   117,   116,     4,   119,     0,     5,
     0,   118,     0,    40,     0,    41,     0,    21,     0,    32,
     0,    76,     0,    72,     0,    61,     0,    22,     0,   126,
     0,     0,    71,   120,   176,    67,   175,    25,   201,   191,
    70,   126,     0,   111,     0,    65,    59,   122,     0,     5,
     0,    65,    83,   124,    82,     0,   125,     0,   124,    81,
   125,     0,     6,   127,     0,   155,     0,    17,    83,   128,
    82,     0,   157,     0,    17,     0,     5,   164,     0,   129,
     0,   128,    81,   129,     0,     0,     6,   130,    84,     7,
    85,     0,     0,     0,     6,   132,    55,   133,    67,   175,
    25,   201,   191,     4,    83,   203,    82,     0,     0,     0,
     6,   135,    56,   136,    70,   126,   177,   145,    67,   175,
   137,   191,   179,   185,     4,    83,   189,    82,     0,     0,
    25,   201,     0,     0,     0,     6,   139,    73,   140,    28,
   203,   141,   144,   191,     4,     7,     0,    77,    83,   142,
    82,     0,     0,   143,     0,   142,    81,   143,     0,   189,
     0,    25,   201,     0,     0,     0,    45,   146,   178,     0,
     0,    12,   147,   178,     0,     0,     0,     6,   149,    52,
   150,   195,    67,   175,    25,   201,   191,     4,    83,   190,
    82,     0,     0,     0,     0,     6,   152,    49,   153,    42,
   202,   154,    60,   201,    19,   201,    25,   201,   192,     4,
    83,   203,    82,     0,   159,     0,   156,   159,     0,   121,
     0,   122,     0,   123,     0,   162,     0,    86,    14,     7,
    87,    34,     0,    86,    75,     7,    87,    34,     0,   161,
     0,   163,     0,   160,     0,    39,     0,    39,   165,     0,
    39,   170,     0,    40,     0,    40,   165,     0,     5,   170,
     0,   104,    88,     5,   170,     0,     5,   165,     0,   104,
    88,     5,   165,     0,    58,    68,     0,    58,    68,   166,
     0,     5,   166,     0,   104,    88,     5,   166,     0,    53,
    33,     0,     7,     0,     8,     0,     9,     0,    10,     0,
     6,     0,    11,     0,    83,   207,    82,     0,    39,   164,
     0,    40,   164,     0,    58,    68,   164,     0,    53,    33,
     0,    41,     0,    21,     0,    32,     0,    32,   165,     0,
    76,     0,    76,   165,     0,    72,     0,    61,     0,    61,
   166,     0,    22,     0,    41,     0,    21,     0,    32,   164,
     0,    76,   164,     0,    72,     0,    61,     0,    22,     0,
   165,     0,   166,     0,   170,     0,     0,    84,   167,    85,
     0,    84,    66,    84,   167,    85,    85,     0,   168,     0,
   167,    89,   168,     0,   169,     0,   169,     3,   169,     0,
     8,     0,     7,     0,    10,     0,     9,     0,    83,   171,
    82,     0,   172,     0,   171,    81,   172,     0,     0,     6,
   173,    84,   174,    85,     0,     7,     0,     8,     0,     6,
     0,     6,     0,    26,   201,     0,     0,    74,   201,     0,
     0,     6,     0,     0,    38,   180,    83,   181,    82,     0,
    15,    83,   184,    82,     0,     0,   182,     0,   181,    81,
   182,     0,    35,   183,     0,   183,     0,   189,     0,   189,
     0,    24,    83,   186,    82,     0,     0,   158,     0,    83,
   187,    82,     0,   188,     0,     0,     6,     0,   188,    81,
     6,     0,   203,     0,   203,     0,    63,   201,     0,     0,
   193,     0,     0,   194,     0,   193,   194,     0,    64,   202,
    25,   201,     0,    57,    83,   196,    82,     0,     0,   197,
     0,   196,    81,   197,     0,   189,     0,    50,    83,   199,
    82,     0,   200,     0,   199,    81,   200,     0,   190,     0,
    11,     0,    11,     0,     0,   204,   205,     0,   206,     0,
   205,   206,     0,     6,     0,   104,    88,     6,     0,     7,
     0,     6,    84,     7,    85,     0,   104,    88,     6,    84,
     7,    85,     0,   208,     0,   209,     0,   208,   209,     0,
     6,    84,     7,    85,     0,     7,     0,     0,     0,     6,
   211,    54,   212,   195,    67,   175,    25,   201,   191,     4,
    83,   203,    82,     0,     0,     0,     6,   214,    51,   215,
   198,    67,   175,    25,   201,   191,     4,    83,   203,    82,
     0,     0,     0,     6,   217,    48,   218,    67,   175,    25,
   201,   191,   219,     4,    83,   203,    82,     0,   220,     0,
   221,     0,   220,   221,     0,     0,    47,   223,   222,   224,
   227,     0,     5,   203,     0,     5,     0,     0,    44,    83,
   225,    82,     0,     0,   226,     0,   225,    81,   226,     0,
   203,     0,   228,     0,     0,   229,     0,   228,   229,     0,
   230,     0,   231,     0,    31,   203,    25,   201,     0,    53,
   189,   232,   233,   235,    25,   201,     0,    70,   126,     0,
     0,    79,   234,     0,     0,   126,     0,    46,   178,     0,
     0,     0,     0,     6,     0,    13,     0,    62,   201,    67,
     0,    25,   201,   191,     0,     4,    83,   203,    82,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    69,     0,
    37,    83,     0,    82,     0,     0,     0,     0,     0,    81,
     0,     0,   203,     0,   203,     0,     5,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    78,   189,   235,    25,   201,     0,    78,   189,   232,
   233,   235,     0,   185,    25,   201,     0,    20,    83,     0,
    82,     0,     0,     0,     0,     0,    81,     0,     0,   189,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   716,   721,   727,   729,   737,   781,   800,   802,   806,   816,
   818,   827,   830,   832,   837,   839,   843,   884,   886,   894,
   900,   906,   924,   925,   926,   927,   928,   929,   930,   931,
   932,   933,   934,   935,   936,   937,   938,   939,   940,   941,
   942,   943,   944,   945,   946,   947,   948,   949,   950,   951,
   952,   953,   954,   955,   956,   957,   958,   959,   960,   961,
   962,   963,   964,   965,   966,   967,   968,   969,   974,   986,
   988,   993,   995,   999,  1004,  1009,  1014,  1019,  1024,  1029,
  1035,  1040,  1045,  1050,  1055,  1068,  1092,  1100,  1101,  1102,
  1103,  1104,  1105,  1106,  1107,  1108,  1109,  1112,  1129,  1138,
  1154,  1176,  1181,  1249,  1253,  1274,  1275,  1276,  1277,  1278,
  1279,  1280,  1281,  1284,  1300,  1319,  1353,  1361,  1375,  1432,
  1440,  1447,  1467,  1513,  1517,  1537,  1542,  1547,  1580,  1587,
  1601,  1614,  1625,  1632,  1648,  1676,  1683,  1703,  1796,  1805,
  1811,  1818,  1836,  1892,  1896,  1902,  1909,  1923,  1929,  1931,
  1935,  1944,  1945,  1954,  1957,  1964,  1978,  2007,  2014,  2037,
  2041,  2075,  2079,  2095,  2101,  2107,  2113,  2130,  2132,  2140,
  2145,  2162,  2171,  2183,  2201,  2215,  2233,  2255,  2303,  2343,
  2393,  2435,  2440,  2453,  2501,  2540,  2547,  2555,  2562,  2588,
  2612,  2620,  2639,  2671,  2675,  2693,  2697,  2703,  2711,  2719,
  2727,  2750,  2770,  2797,  2805,  2815,  2839,  2853,  2861,  2869,
  2877,  2897,  2905,  2915,  2925,  2932,  2939,  2946,  2954,  2964,
  2974,  2981,  2995,  3003,  3014,  3021,  3028,  3052,  3080,  3086,
  3093,  3107,  3120,  3130,  3141,  3151,  3185,  3201,  3205,  3211,
  3215,  3221,  3264,  3272,  3279,  3288,  3294,  3301,  3315,  3320,
  3326,  3336,  3342,  3344,  3349,  3351,  3372,  3374,  3378,  3385,
  3399,  3405,  3411,  3413,  3417,  3419,  3431,  3433,  3437,  3460,
  3464,  3470,  3477,  3491,  3497,  3503,  3510,  3524,  3530,  3536,
  3542,  3546,  3552,  3557,  3564,  3644,  3721,  3747,  3778,  3811,
  3815,  3817,  3821,  3823,  3827,  3834,  3848,  3878,  3885,  3900,
  3931,  3938,  3952,  4032,  4038,  4042,  4077,  4093,  4106,  4114,
  4121,  4128,  4132,  4138,  4145,  4159,  4185,  4191,  4199,  4203,
  4270,  4276,  4284,  4306,  4335,  4344,  4350,  4359,  4365,  4371,
  4375,  4381,  4388,  4402,  4435,  4437,  4441,  4443,  4447,  4455,
  4462,  4476,  4482,  4484,  4486,  4492,  4494,  4498,  4500,  4504,
  4506,  4510,  4519,  4532,  4534,  4538,  4540,  4544
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
"valueofObjectSyntax","SimpleSyntax","valueofSimpleSyntax","sequenceSimpleSyntax",
"ApplicationSyntax","sequenceApplicationSyntax","anySubType","integerSubType",
"octetStringSubType","ranges","range","value","enumSpec","enumItems","enumItem",
"@22","enumNumber","Status","DisplayPart","UnitsPart","Access","IndexPart","@23",
"IndexTypes","IndexType","Index","Entry","DefValPart","Value","BitsValue","BitNames",
"ObjectName","NotificationName","ReferPart","RevisionPart","Revisions","Revision",
"ObjectsPart","Objects","Object","NotificationsPart","Notifications","Notification",
"Text","ExtUTCTime","objectIdentifier","@24","subidentifiers","subidentifier",
"objectIdentifier_defval","subidentifiers_defval","subidentifier_defval","objectGroupClause",
"@25","@26","notificationGroupClause","@27","@28","moduleComplianceClause","@29",
"@30","ComplianceModulePart","ComplianceModules","ComplianceModule","@31","ComplianceModuleName",
"MandatoryPart","MandatoryGroups","MandatoryGroup","CompliancePart","Compliances",
"Compliance","ComplianceGroup","ComplianceObject","SyntaxPart","WriteSyntaxPart",
"WriteSyntax","AccessPart", NULL
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
   107,   107,   107,   107,   107,   109,   108,   110,   110,   110,
   110,   110,   110,   110,   110,   110,   110,   112,   111,   114,
   113,   116,   115,   117,   117,   118,   118,   118,   118,   118,
   118,   118,   118,   119,   120,   119,   119,   121,   122,   123,
   124,   124,   125,   126,   126,   127,   127,   127,   128,   128,
   130,   129,   132,   133,   131,   135,   136,   134,   137,   137,
   139,   140,   138,   141,   141,   142,   142,   143,   144,   144,
   146,   145,   147,   145,   149,   150,   148,   152,   153,   154,
   151,   155,   155,   155,   155,   155,   155,   156,   156,   157,
   157,   158,   159,   159,   159,   159,   159,   159,   159,   159,
   159,   159,   159,   159,   159,   159,   160,   160,   160,   160,
   160,   160,   160,   161,   161,   161,   161,   162,   162,   162,
   162,   162,   162,   162,   162,   162,   162,   163,   163,   163,
   163,   163,   163,   163,   164,   164,   164,   164,   165,   166,
   167,   167,   168,   168,   169,   169,   169,   169,   170,   171,
   171,   173,   172,   174,   174,   175,    -1,   176,   176,   177,
   177,   178,   180,   179,   179,   179,   181,   181,   182,   182,
   183,   184,   185,   185,   186,   186,   187,   187,   188,   188,
   189,   190,   191,   191,   192,   192,   193,   193,   194,   195,
   195,   196,   196,   197,   198,   199,   199,   200,   201,   202,
   204,   203,   205,   205,   206,   206,   206,   206,   206,   207,
   208,   208,   209,   209,   211,   212,   210,   214,   215,   213,
   217,   218,   216,   219,   220,   220,   222,   221,   223,   223,
   223,   224,   224,   225,   225,   226,   227,   227,   228,   228,
   229,   229,   230,   231,   232,   232,   233,   233,   234,   235,
   235,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1
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
     1,     1,     1,     1,     2,     0,     4,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     0,     3,     0,
     8,     0,     4,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     0,    10,     1,     3,     1,     4,
     1,     3,     2,     1,     4,     1,     1,     2,     1,     3,
     0,     5,     0,     0,    13,     0,     0,    18,     0,     2,
     0,     0,    11,     4,     0,     1,     3,     1,     2,     0,
     0,     3,     0,     3,     0,     0,    14,     0,     0,     0,
    18,     1,     2,     1,     1,     1,     1,     5,     5,     1,
     1,     1,     1,     2,     2,     1,     2,     2,     4,     2,
     4,     2,     3,     2,     4,     2,     1,     1,     1,     1,
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
     0,     0,     0,    16,     1,     0,     1,     2,     7,     1,
     3,     1,     1,     1,     0,     1,     0,     1,     2,     1,
     1,     5,     9,     4,     0,     1,     3,     1
};

static const short yydefact[] = {     2,
    69,     1,     3,     5,     4,     0,     0,     0,    10,    11,
     8,     0,    14,     0,     7,    12,    21,    20,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,     0,    13,    15,     0,    18,    22,
     0,   104,   100,    97,   108,   113,   109,   106,   107,    96,
    88,    95,    91,    94,    92,    89,   112,    93,   111,    90,
   110,     0,     0,    72,    84,     0,    75,    74,   102,   105,
    76,    77,    78,    79,    80,    82,    83,    81,     9,    16,
     0,     0,    85,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,    73,    86,     0,    17,    19,     0,   134,
   137,   142,   156,   159,   296,   299,   302,     0,     0,     0,
     0,     0,     0,   271,     0,   271,     0,     0,    87,   119,
     0,    98,   199,   207,   200,   173,   176,   198,     0,     0,
   205,     0,   115,   204,   202,     0,     0,   117,   103,   164,
   165,   166,   114,   124,     0,   162,   167,     0,     0,     0,
   281,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   180,   184,   178,     0,     0,     0,   201,   174,   175,   177,
   186,   182,     0,   206,     0,     0,   239,   203,     0,     0,
     0,    69,   163,   281,   236,     0,   241,   145,     0,   281,
     0,   280,   160,     0,   281,     0,     0,   232,     0,   230,
   226,   225,   228,   227,     0,     0,   221,   223,   131,     0,
   129,    99,   183,   119,   118,     0,     0,   121,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   150,   285,
   287,     0,   282,   283,   274,     0,   272,   261,     0,     0,
     0,   278,     0,   276,   262,     0,     0,     0,     0,   229,
     0,   219,     0,     0,     0,     0,   125,   218,   127,   209,
   214,   218,   218,   218,   208,     0,     0,   213,   212,   218,
   123,   126,   170,   171,     0,   120,   279,   238,     0,     0,
     0,   181,   185,   179,   101,   264,   240,   153,   151,     0,
   281,     0,   264,     0,     0,   284,   281,   270,     0,     0,
     0,   281,   275,     0,   264,     0,   231,     0,   222,   224,
     0,   130,   128,   215,   216,   217,   210,   194,   195,   197,
   218,   211,   122,     0,   168,   169,     0,     0,     0,     0,
     0,     0,   146,   148,   149,     0,     0,   286,   273,   264,
     0,   264,   277,   264,     0,   234,   235,     0,     0,     0,
   196,     0,   263,     0,   242,   154,   152,   139,   281,   144,
     0,   288,     0,     0,     0,     0,     0,   311,     0,   304,
   305,   233,   220,   132,   264,   281,     0,   264,   147,   143,
     0,     0,     0,     0,     0,   310,   307,     0,   306,     0,
     0,   140,   246,   289,   281,     0,   281,   281,   309,   313,
   281,     0,   135,     0,   243,   254,     0,   266,     0,     0,
     0,   318,     0,   116,   281,     0,     0,     0,   157,     0,
     0,   265,   267,   297,   300,   281,   281,   281,   308,   317,
   319,   321,   322,   303,     0,   252,   281,     0,     0,     0,
     0,   268,   316,     0,   314,     0,   326,   320,   245,   281,
     0,   247,   250,   251,   191,   187,   188,   189,   190,   192,
   258,   255,   172,     0,   281,     0,   281,   281,   312,     0,
     0,   328,   249,   281,   244,   259,   294,     0,   257,     0,
   290,   291,   253,     0,   269,     0,   315,   323,   325,     0,
   331,   248,     0,   256,     0,   193,     0,   292,   138,   161,
   329,   327,     0,     0,     0,   260,   330,     0,   293,   324,
     0,     0,     0
};

static const short yydefgoto[] = {   541,
     2,     3,     6,    14,    15,    11,    12,    65,    66,    67,
    68,    69,    70,   167,    92,    93,    94,    95,   138,    96,
   168,   195,    97,   114,    98,   126,    99,   100,   169,   207,
   170,   171,   172,   247,   248,   173,   301,   240,   241,   285,
   101,   115,   141,   102,   116,   142,   408,   103,   117,   143,
   259,   362,   363,   323,   320,   360,   359,   104,   118,   144,
   105,   119,   145,   270,   174,   175,   302,   492,   176,   493,
   303,   177,   304,   343,   344,   345,   236,   237,   238,   346,
   229,   230,   278,   378,   216,   250,   257,   386,   436,   446,
   481,   482,   483,   465,   448,   494,   508,   509,   484,   272,
   358,   451,   452,   453,   183,   266,   267,   187,   273,   274,
   308,   223,   268,   219,   263,   264,   510,   511,   512,   106,
   120,   146,   107,   121,   147,   108,   122,   148,   399,   400,
   401,   430,   417,   442,   474,   475,   459,   460,   461,   462,
   463,   502,   521,   532,   534
};

static const short yypact[] = {    16,
-32768,    16,-32768,-32768,-32768,   -14,    10,    11,    32,-32768,
    52,     5,   374,   255,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    13,   374,-32768,   -19,-32768,-32768,
    17,-32768,    35,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    68,   312,-32768,-32768,    71,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    16,   374,-32768,    74,    85,    92,    51,   100,   106,   110,
   112,   118,-32768,-32768,-32768,   161,-32768,-32768,   134,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   141,   117,   165,
   104,   103,   148,   120,   137,   120,   131,   116,-32768,   -31,
   107,-32768,-32768,-32768,   102,    18,   102,-32768,   154,   123,
   108,   -24,-32768,-32768,   102,    -1,   111,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    39,-32768,-32768,   114,   188,   179,
-32768,   115,   128,   191,   138,   121,   139,   188,   201,    30,
-32768,-32768,-32768,   202,   127,   109,-32768,-32768,-32768,-32768,
-32768,   108,   144,-32768,   207,   208,   187,-32768,   209,   210,
   216,    26,-32768,-32768,-32768,   190,   150,   145,    49,-32768,
   188,-32768,-32768,   188,-32768,   188,   200,-32768,    44,-32768,
-32768,-32768,-32768,-32768,   142,   -22,-32768,   224,-32768,    48,
-32768,-32768,-32768,-32768,-32768,   434,    50,-32768,   218,   164,
   146,   147,    26,   157,   218,   218,     6,   152,   211,   170,
-32768,   160,    49,-32768,-32768,    55,-32768,-32768,   220,   181,
   232,-32768,    61,-32768,-32768,   233,   218,   175,   201,-32768,
   109,-32768,   109,   109,   178,   202,-32768,    26,-32768,-32768,
-32768,    26,    26,    26,-32768,   230,   196,-32768,-32768,    26,
-32768,-32768,-32768,-32768,   208,-32768,-32768,-32768,   188,   235,
   236,-32768,-32768,-32768,-32768,   203,-32768,-32768,-32768,   204,
-32768,   218,   203,   265,   267,-32768,-32768,-32768,   218,   218,
   218,-32768,-32768,   218,   203,   113,-32768,     9,-32768,-32768,
   268,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    26,-32768,-32768,   249,-32768,-32768,   218,   277,   279,   279,
   188,    63,-32768,-32768,-32768,   282,   198,   205,-32768,   203,
   269,   203,-32768,   203,   243,-32768,-32768,   206,   212,   213,
-32768,   218,-32768,   217,-32768,-32768,-32768,   274,-32768,-32768,
   285,-32768,   286,   290,   218,   297,   298,   300,   304,   243,
-32768,-32768,-32768,-32768,   203,-32768,   218,   203,-32768,-32768,
   227,   231,   299,   239,   240,    59,-32768,   246,-32768,   245,
   248,-32768,    43,-32768,-32768,   218,-32768,-32768,-32768,   288,
-32768,   179,-32768,   252,-32768,   313,   254,   278,   256,   259,
   260,    -7,   263,-32768,-32768,   264,   266,   342,-32768,   191,
   344,   278,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -7,
-32768,-32768,-32768,-32768,   272,-32768,   315,    22,   273,   326,
   275,-32768,-32768,    65,-32768,   330,   287,-32768,-32768,-32768,
    69,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   153,-32768,-32768,   280,-32768,   218,-32768,-32768,-32768,   218,
   179,   291,-32768,   315,-32768,   281,-32768,   289,   293,   294,
   155,-32768,-32768,   295,-32768,   296,-32768,-32768,-32768,   179,
   323,-32768,   352,-32768,   366,-32768,   281,-32768,-32768,-32768,
-32768,-32768,   279,   350,   305,-32768,-32768,   218,-32768,-32768,
   381,   382,-32768
};

static const short yypgoto[] = {-32768,
-32768,   390,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   331,
-32768,   292,-32768,     4,-32768,-32768,   308,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   197,-32768,-32768,    98,  -180,-32768,-32768,   122,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    21,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   238,-32768,
-32768,-32768,-32768,  -251,  -140,  -142,   126,   149,   133,  -130,
-32768,   132,-32768,-32768,  -176,-32768,-32768,  -353,-32768,-32768,
-32768,   -77,   -47,-32768,-32768,-32768,-32768,-32768,  -215,    15,
  -301,-32768,-32768,   -10,   303,-32768,   130,-32768,-32768,   129,
  -254,     8,  -178,-32768,-32768,   199,-32768,-32768,   -57,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    60,-32768,-32768,-32768,-32768,   -39,-32768,-32768,     3,-32768,
-32768,-32768,-32768,-32768,-32768
};


#define	YYLAST		510


static const short yytable[] = {   217,
   316,   317,   218,     4,   265,     4,   387,   192,     7,   191,
   111,   227,   209,     8,   197,   198,   200,   318,   204,   193,
     1,   366,   335,   457,   208,   199,     9,   485,   486,   487,
   488,   489,   490,   375,   205,   254,   231,   232,   233,   234,
   347,   348,   349,   212,   269,   458,   275,   271,   352,   276,
   319,   189,   190,     1,   260,   261,   -69,   434,   206,   243,
    10,   112,   282,  -281,  -281,  -281,   283,   365,   394,   192,
   396,   191,   397,   210,   370,   371,   372,   156,   157,   374,
   435,   193,  -301,  -158,    16,  -298,  -155,    13,  -295,  -133,
  -136,   159,   109,   379,   123,   235,   160,   283,   113,   381,
   189,   196,   383,   420,   491,   364,   423,  -141,   189,   190,
   313,   265,   312,   125,   127,   231,   232,   233,   234,   376,
   377,   150,   314,   132,   279,   280,   129,   405,   286,   287,
   305,   306,   354,   151,   152,   327,   328,   153,   154,   130,
   413,   332,   333,   389,   390,   498,   499,   131,   155,   504,
   505,   133,   422,   275,   134,   156,   157,   158,   506,   507,
   527,   507,   136,   135,   139,   137,   140,   149,   178,   159,
   179,   438,   180,   364,   160,   181,   182,   161,   184,   537,
   186,   162,   188,   150,   388,   196,   201,   163,   164,   194,
   202,   203,   165,   215,   221,   151,   214,   220,   211,   153,
   154,   222,   166,   225,   224,   226,   228,   239,   242,   235,
   155,   244,   249,   246,   255,   251,   252,   156,   157,   158,
   253,   258,   262,   256,   277,   281,   284,   421,   307,   466,
   309,   159,   310,   311,   321,   322,   160,   429,   315,   161,
   330,   515,   477,   162,   329,   518,   275,   325,   439,   440,
   164,   444,   443,   324,   165,    71,   331,   334,   336,    72,
    73,   341,   350,   351,   166,   357,   262,    74,   355,   356,
   361,   367,   368,   382,   380,    75,    76,   473,   476,   514,
   384,   -71,   392,   540,   385,   391,    77,   395,   393,   398,
   402,   410,   411,   412,    78,    79,   403,   404,   407,   406,
   414,   415,    80,    81,   416,    82,    83,   418,    84,    85,
    86,   424,    71,   425,   432,    87,    72,    73,   516,   473,
   519,   427,   428,   426,    74,    88,    89,    90,   431,   433,
    91,   441,    75,    76,   445,   449,   447,   454,   -70,   531,
   455,   450,   456,    77,   464,   469,   467,   471,   468,   480,
   496,    78,    79,   479,   500,   495,   501,   497,   535,    80,
    81,   513,    82,    83,   523,    84,    85,    86,   533,   520,
   524,   536,    87,   525,   538,   526,   529,   530,    17,    18,
   542,   543,    88,    89,    90,    19,    20,    91,    21,   539,
    22,     5,    23,    24,    25,    26,   110,    27,    28,    29,
   124,   245,   353,   128,    30,    31,   338,   342,    32,   409,
   337,    33,   213,    34,    35,    36,   340,    37,    38,    39,
    40,    41,    42,    43,    44,    45,   522,    46,    47,    48,
    49,   339,   503,    50,    51,    52,    53,    54,   288,   437,
    55,   472,    56,    57,    58,    59,    60,    61,   185,    62,
   289,    63,    64,   528,   290,   291,   369,   470,   517,   419,
   373,   326,   478,     0,     0,   292,     0,     0,     0,     0,
     0,     0,   293,   294,   295,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   296,     0,     0,     0,
     0,   297,     0,     0,   298,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   299,     0,     0,     0,   300
};

static const short yycheck[] = {   180,
   255,   256,   181,     0,   220,     2,   360,   150,    23,   150,
    30,   188,    14,     4,   155,   156,   157,    12,   161,   150,
     5,   323,   277,    31,   165,   156,    16,     6,     7,     8,
     9,    10,    11,   335,    59,   214,     7,     8,     9,    10,
   292,   293,   294,     5,   221,    53,   225,   224,   300,   226,
    45,    83,    84,     5,     6,     7,    88,    15,    83,   202,
    29,    81,    85,     5,     6,     7,    89,   322,   370,   212,
   372,   212,   374,    75,   329,   330,   331,    39,    40,   334,
    38,   212,    48,    49,    80,    51,    52,    36,    54,    55,
    56,    53,    80,    85,    27,    66,    58,    89,    82,   351,
    83,    84,   357,   405,    83,   321,   408,    73,    83,    84,
   253,   327,   253,    43,   111,     7,     8,     9,    10,     7,
     8,     5,   253,    73,    81,    82,    53,   382,    81,    82,
    81,    82,   309,    17,    18,    81,    82,    21,    22,    55,
   395,    81,    82,    81,    82,    81,    82,    56,    32,    81,
    82,    52,   407,   332,    49,    39,    40,    41,     6,     7,
     6,     7,    51,    54,     4,    48,    33,    27,     4,    53,
    67,   426,    70,   389,    58,    28,    57,    61,    42,   533,
    50,    65,    67,     5,   361,    84,    33,    71,    72,    83,
    68,    84,    76,     6,    67,    17,    83,    83,    88,    21,
    22,    11,    86,    83,    67,    67,     6,     6,    82,    66,
    32,     5,    26,     6,    25,     7,     7,    39,    40,    41,
     5,    77,   219,    74,    25,    84,     3,   406,    11,   445,
    67,    53,    87,    87,    83,    25,    58,   416,    82,    61,
    60,   496,   458,    65,    25,   500,   425,    88,   427,   428,
    72,   432,   431,    84,    76,     1,    25,    25,    84,     5,
     6,    84,    33,    68,    86,    63,   263,    13,    34,    34,
    67,     7,     6,    25,     7,    21,    22,   456,   457,   495,
     4,    27,    85,   538,     6,     4,    32,    19,    84,    47,
    85,     7,     7,     4,    40,    41,    85,    85,    25,    83,
     4,     4,    48,    49,     5,    51,    52,     4,    54,    55,
    56,    85,     1,    83,    70,    61,     5,     6,   497,   498,
   501,    83,    83,    25,    13,    71,    72,    73,    83,    82,
    76,    44,    21,    22,    83,    82,    24,    82,    27,   520,
    82,    64,    83,    32,    82,     4,    83,     4,    83,    35,
    25,    40,    41,    82,    25,    83,    70,    83,     7,    48,
    49,    82,    51,    52,    84,    54,    55,    56,    46,    79,
    82,     6,    61,    81,    25,    82,    82,    82,     5,     6,
     0,     0,    71,    72,    73,    12,    13,    76,    15,    85,
    17,     2,    19,    20,    21,    22,    66,    24,    25,    26,
    93,   205,   305,   112,    31,    32,   281,   286,    35,   389,
   279,    38,   175,    40,    41,    42,   284,    44,    45,    46,
    47,    48,    49,    50,    51,    52,   504,    54,    55,    56,
    57,   283,   480,    60,    61,    62,    63,    64,     5,   425,
    67,   452,    69,    70,    71,    72,    73,    74,   146,    76,
    17,    78,    79,   511,    21,    22,   327,   450,   498,   400,
   332,   263,   460,    -1,    -1,    32,    -1,    -1,    -1,    -1,
    -1,    -1,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,
    -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    72,    -1,    -1,    -1,    76
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
#line 718 "parser-smi.y"
{
        yyval.err = 0;
    ;
    break;}
case 2:
#line 722 "parser-smi.y"
{
	yyval.err = 0;
    ;
    break;}
case 3:
#line 728 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 4:
#line 730 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 5:
#line 738 "parser-smi.y"
{
			    /*
			     * This module is to be parsed, either if we
			     * want the whole file or if its name matches
			     * the module name we are looking for.
			     * In fact, we always parse it, but we just
			     * remember its contents if needed.
			     */
			    thisParserPtr->modulePtr = findModuleByName(yyvsp[0].id);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule(yyvsp[0].id,
					      util_strdup(thisParserPtr->path),
					      thisParserPtr->character,
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
#line 786 "parser-smi.y"
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
#line 801 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 8:
#line 803 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 9:
#line 807 "parser-smi.y"
{
			    if (thisModulePtr->export.language != SMI_LANGUAGE_SMIV2)
				thisModulePtr->export.language = SMI_LANGUAGE_SMIV1;
			    
			    yyval.err = 0;
			;
    break;}
case 10:
#line 817 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 11:
#line 819 "parser-smi.y"
{
			    if (strcmp(thisParserPtr->modulePtr->export.name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_EXPORTS);
			    }
			;
    break;}
case 12:
#line 827 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 13:
#line 831 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 14:
#line 833 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 15:
#line 838 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 16:
#line 840 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 17:
#line 846 "parser-smi.y"
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
#line 885 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 19:
#line 888 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 20:
#line 895 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 21:
#line 901 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 22:
#line 909 "parser-smi.y"
{
			    addImport(yyvsp[0].id, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 69:
#line 975 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_MODULENAME_32, ERR_MODULENAME_64);
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 70:
#line 987 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 71:
#line 989 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 72:
#line 994 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 73:
#line 996 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 74:
#line 1000 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 75:
#line 1005 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 76:
#line 1010 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 77:
#line 1015 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 78:
#line 1020 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 79:
#line 1025 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 80:
#line 1030 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 81:
#line 1036 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 82:
#line 1041 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 83:
#line 1046 "parser-smi.y"
{
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 84:
#line 1051 "parser-smi.y"
{ 
			    thisParserPtr->modulePtr->numStatements++;
			    yyval.err = 0;
			;
    break;}
case 85:
#line 1056 "parser-smi.y"
{
			    printError(thisParserPtr, ERR_FLUSH_DECLARATION);
			    yyerrok;
			    yyval.err = 1;
			;
    break;}
case 86:
#line 1070 "parser-smi.y"
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
case 87:
#line 1092 "parser-smi.y"
{
			    addMacro(yyvsp[-3].id, thisParserPtr->character, 0,
				     thisParserPtr);
			    util_free(yyvsp[-3].id);
			    yyval.err = 0;
                        ;
    break;}
case 88:
#line 1100 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 89:
#line 1101 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 90:
#line 1102 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 91:
#line 1103 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 92:
#line 1104 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 93:
#line 1105 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 94:
#line 1106 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 95:
#line 1107 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 96:
#line 1108 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 97:
#line 1109 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 98:
#line 1113 "parser-smi.y"
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
case 99:
#line 1129 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			;
    break;}
case 100:
#line 1139 "parser-smi.y"
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
case 101:
#line 1156 "parser-smi.y"
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
case 102:
#line 1177 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_TYPENAME_32, ERR_TYPENAME_64);
			;
    break;}
case 103:
#line 1182 "parser-smi.y"
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
			     * must modified to libsmi basetypes.
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
case 104:
#line 1250 "parser-smi.y"
{
			    yyval.id = yyvsp[0].id;
			;
    break;}
case 105:
#line 1254 "parser-smi.y"
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
case 106:
#line 1274 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 107:
#line 1275 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 108:
#line 1276 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 109:
#line 1277 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 110:
#line 1278 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 111:
#line 1279 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 112:
#line 1280 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 113:
#line 1281 "parser-smi.y"
{ yyval.id = util_strdup(yyvsp[0].id); ;
    break;}
case 114:
#line 1285 "parser-smi.y"
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
case 115:
#line 1301 "parser-smi.y"
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
case 116:
#line 1323 "parser-smi.y"
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
				deleteTypeFlags(yyval.typePtr, FLAG_IMPORTED);
			    }
			    setTypeDescription(yyval.typePtr, yyvsp[-3].text);
			    if (yyvsp[-2].text) {
				setTypeReference(yyval.typePtr, yyvsp[-2].text);
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
case 117:
#line 1354 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			    setTypeDecl(yyval.typePtr, SMI_DECL_TYPEASSIGNMENT);
			;
    break;}
case 118:
#line 1362 "parser-smi.y"
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
case 119:
#line 1381 "parser-smi.y"
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
#if 0
				    XXX
				    stypePtr = smiGetType(
						  importPtr->export.module, yyvsp[0].id);
				    if (stypePtr) {
					yyval.typePtr = addType(yyvsp[0].id, stypePtr->basetype,
						     FLAG_IMPORTED,
						     thisParserPtr);
				    } else {
					yyval.typePtr = addType(yyvsp[0].id, SMI_BASETYPE_UNKNOWN,
					       FLAG_INCOMPLETE | FLAG_IMPORTED,
						     thisParserPtr);
				    }
				    smiFreeType(stypePtr);
#else
				    yyval.typePtr = findTypeByModulenameAndName(
					importPtr->export.module,
					importPtr->export.name);
#endif
				}
			    }
			;
    break;}
case 120:
#line 1433 "parser-smi.y"
{
			    yyval.typePtr = addType(NULL, SMI_BASETYPE_UNKNOWN, 0,
					 thisParserPtr);
			    setTypeList(yyval.typePtr, yyvsp[-1].listPtr);
			;
    break;}
case 121:
#line 1441 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 122:
#line 1449 "parser-smi.y"
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
case 123:
#line 1468 "parser-smi.y"
{
			    Object *objectPtr;
			    SmiNode *snodePtr;
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
				    setObjectFileOffset(objectPtr,
						        thisParserPtr->character);
				} else {
				    /*
				     * imported object.
				     */
				    importPtr->use++;
#if 0
				    snodePtr = smiGetNode(
						  importPtr->export.module, yyvsp[-1].id);
				    objectPtr = addObject(yyvsp[-1].id,
					getParentNode(
				             createNodes(snodePtr->oidlen,
							 snodePtr->oid)),
					snodePtr->oid[snodePtr->oidlen-1],
					FLAG_IMPORTED, thisParserPtr);
				    smiFreeNode(snodePtr);
#else
				    objectPtr = findObjectByModulenameAndName(
					importPtr->export.module, yyvsp[-1].id);
#endif
				}
			    }

			    yyval.objectPtr = objectPtr;
			;
    break;}
case 124:
#line 1514 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 125:
#line 1520 "parser-smi.y"
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
case 126:
#line 1539 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 127:
#line 1543 "parser-smi.y"
{
			    /* TODO: */
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 128:
#line 1548 "parser-smi.y"
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
case 129:
#line 1581 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 130:
#line 1588 "parser-smi.y"
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
case 131:
#line 1602 "parser-smi.y"
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
case 132:
#line 1615 "parser-smi.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id; /* JS: needs strdup? */
			    yyval.namedNumberPtr->export.value.basetype =
				                       SMI_BASETYPE_UNSIGNED32;
			    yyval.namedNumberPtr->export.value.value.unsigned32 = yyvsp[-1].unsigned32;
			    yyval.namedNumberPtr->export.value.format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 133:
#line 1626 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 134:
#line 1632 "parser-smi.y"
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
case 135:
#line 1653 "parser-smi.y"
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
			    setObjectDescription(objectPtr, yyvsp[-5].text);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text);
			    }
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    yyval.err = 0;
			;
    break;}
case 136:
#line 1677 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 137:
#line 1683 "parser-smi.y"
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
case 138:
#line 1712 "parser-smi.y"
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
			    setObjectBasetype(objectPtr, yyvsp[-12].typePtr->export.basetype);
			    if (!(yyvsp[-12].typePtr->export.name)) {
				/*
				 * An inlined type.
				 */
				setTypeName(yyvsp[-12].typePtr, yyvsp[-17].id);
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
				setObjectDescription(objectPtr, yyvsp[-7].text);
			    }
			    if (yyvsp[-6].text) {
				setObjectReference(objectPtr, yyvsp[-6].text);
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
case 139:
#line 1797 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
				printError(thisParserPtr,
					   ERR_MISSING_DESCRIPTION);
			    }
			    yyval.text = NULL;
			;
    break;}
case 140:
#line 1806 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 141:
#line 1812 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 142:
#line 1818 "parser-smi.y"
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
case 143:
#line 1842 "parser-smi.y"
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
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
			    }
			    objectPtr = addObject("",
						  objectPtr->nodePtr,
						  yyvsp[0].unsigned32,
						  FLAG_INCOMPLETE,
						  thisParserPtr);
			    setObjectFileOffset(objectPtr,
						thisParserPtr->character);
			    
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
			    setObjectDescription(objectPtr, yyvsp[-3].text);
			    if (yyvsp[-2].text) {
				setObjectReference(objectPtr, yyvsp[-2].text);
			    }
			    yyval.err = 0;
			;
    break;}
case 144:
#line 1893 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 145:
#line 1897 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 146:
#line 1903 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 147:
#line 1910 "parser-smi.y"
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
case 148:
#line 1924 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 149:
#line 1930 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 150:
#line 1932 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 151:
#line 1936 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV1)
			    {
			        printError(thisParserPtr,
					   ERR_MAX_ACCESS_IN_SMIV1);
			    }
			;
    break;}
case 152:
#line 1944 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 153:
#line 1946 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
			    {
			        printError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			;
    break;}
case 154:
#line 1954 "parser-smi.y"
{ yyval.access = yyvsp[0].access; ;
    break;}
case 155:
#line 1958 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 156:
#line 1964 "parser-smi.y"
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
case 157:
#line 1983 "parser-smi.y"
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
			    setObjectDescription(objectPtr, yyvsp[-5].text);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text);
			    }
			    yyval.err = 0;
			;
    break;}
case 158:
#line 2008 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 159:
#line 2014 "parser-smi.y"
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
case 160:
#line 2038 "parser-smi.y"
{
			    setModuleLastUpdated(thisParserPtr->modulePtr, yyvsp[0].date);
			;
    break;}
case 161:
#line 2047 "parser-smi.y"
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
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setModuleIdentityObject(thisParserPtr->modulePtr,
						    objectPtr);
			    setModuleOrganization(thisParserPtr->modulePtr,
						  yyvsp[-9].text);
			    setModuleContactInfo(thisParserPtr->modulePtr,
						 yyvsp[-7].text);
			    setModuleDescription(thisParserPtr->modulePtr,
						 yyvsp[-5].text);
			    setObjectDescription(objectPtr, yyvsp[-5].text);
			    yyval.err = 0;
			;
    break;}
case 162:
#line 2076 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 163:
#line 2080 "parser-smi.y"
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
case 164:
#line 2096 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 165:
#line 2102 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 166:
#line 2108 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			    /* TODO */
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 167:
#line 2114 "parser-smi.y"
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
case 168:
#line 2131 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 169:
#line 2133 "parser-smi.y"
{ yyval.err = 0; /* TODO: check range */ ;
    break;}
case 170:
#line 2141 "parser-smi.y"
{ yyval.typePtr = yyvsp[0].typePtr; ;
    break;}
case 171:
#line 2146 "parser-smi.y"
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
case 172:
#line 2163 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 173:
#line 2172 "parser-smi.y"
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
case 174:
#line 2184 "parser-smi.y"
{
			    List *p;
			    
			    if ((thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				&&
				(strcmp(thisModulePtr->export.name, "SNMPv2-SMI") &&
				 strcmp(thisModulePtr->export.name, "SNMPv2-TC")))
				printError(thisParserPtr,ERR_INTEGER_IN_SMIV2);

			    defaultBasetype = SMI_BASETYPE_INTEGER32;
			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
					       thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 175:
#line 2202 "parser-smi.y"
{
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_ENUM;
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
case 176:
#line 2216 "parser-smi.y"
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
case 177:
#line 2234 "parser-smi.y"
{
			    Import *importPtr;
			    List *p;
			    
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

			    yyval.typePtr = duplicateType(typeInteger32Ptr, 0, thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 178:
#line 2256 "parser-smi.y"
{
			    Type *parentPtr;
			    Import *importPtr;
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_ENUM;
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
case 179:
#line 2305 "parser-smi.y"
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
case 180:
#line 2344 "parser-smi.y"
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
case 181:
#line 2395 "parser-smi.y"
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
case 182:
#line 2436 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 183:
#line 2441 "parser-smi.y"
{
			    List *p;
			    
			    defaultBasetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    setTypeParent(yyval.typePtr, typeOctetStringPtr);
			    setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				((Range *)p->ptr)->typePtr = yyval.typePtr;
			;
    break;}
case 184:
#line 2454 "parser-smi.y"
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
case 185:
#line 2503 "parser-smi.y"
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
case 186:
#line 2541 "parser-smi.y"
{
			    defaultBasetype = SMI_BASETYPE_OBJECTIDENTIFIER;
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 187:
#line 2549 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 188:
#line 2556 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 189:
#line 2563 "parser-smi.y"
{
			    char s[9];
			    int i, j;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_BINSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				yyval.valuePtr->value.ptr =
				    util_malloc((strlen(yyvsp[0].text)+7)/8+1);
				for (i = 0; i < strlen(yyvsp[0].text); i += 8) {
				    strncpy(s, &yyvsp[0].text[i], 8);
				    for (j = 1; j < 8; j++) {
					if (!s[j]) s[j] = '0';
				    }
				    s[8] = 0;
				    yyval.valuePtr->value.ptr[i/8] = strtol(s, 0, 2);
				}
				yyval.valuePtr->len = (strlen(yyvsp[0].text)+7)/8;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 2);
			    }
			;
    break;}
case 190:
#line 2589 "parser-smi.y"
{
			    char s[3];
			    int i;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_HEXSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				yyval.valuePtr->value.ptr =
				    util_malloc((strlen(yyvsp[0].text)+1)/2+1);
				for (i = 0; i < strlen(yyvsp[0].text); i += 2) {
				    strncpy(s, &yyvsp[0].text[i], 2);
				    if (!s[1]) s[1] = '0';
				    s[2] = 0;
				    yyval.valuePtr->value.ptr[i/2] = strtol(s, 0, 16);
				}
				yyval.valuePtr->len = (strlen(yyvsp[0].text)+1)/2;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 16);
			    }
			;
    break;}
case 191:
#line 2613 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->basetype = defaultBasetype;
			    yyval.valuePtr->value.ptr = yyvsp[0].id;	/* JS: needs strdup? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NAME;
			;
    break;}
case 192:
#line 2621 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
			    yyval.valuePtr->value.ptr = util_strdup(yyvsp[0].text);
			    yyval.valuePtr->format = SMI_VALUEFORMAT_TEXT;
			    yyval.valuePtr->len = strlen(yyvsp[0].text);
			;
    break;}
case 193:
#line 2646 "parser-smi.y"
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
case 194:
#line 2672 "parser-smi.y"
{
			    yyval.typePtr = typeInteger32Ptr;
			;
    break;}
case 195:
#line 2676 "parser-smi.y"
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
case 196:
#line 2694 "parser-smi.y"
{
			    yyval.typePtr = typeOctetStringPtr;
			;
    break;}
case 197:
#line 2698 "parser-smi.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 198:
#line 2704 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "IpAddress");
			    }
			;
    break;}
case 199:
#line 2712 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter32");
			    }
			;
    break;}
case 200:
#line 2720 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			;
    break;}
case 201:
#line 2728 "parser-smi.y"
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
case 202:
#line 2751 "parser-smi.y"
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
case 203:
#line 2771 "parser-smi.y"
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
case 204:
#line 2798 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "TimeTicks");
			    }
			;
    break;}
case 205:
#line 2806 "parser-smi.y"
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
case 206:
#line 2816 "parser-smi.y"
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
case 207:
#line 2840 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter64");
			    }
			;
    break;}
case 208:
#line 2854 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("IpAddress");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "IpAddress");
			    }
			;
    break;}
case 209:
#line 2862 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter32");
			    }
			;
    break;}
case 210:
#line 2870 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Gauge32");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			;
    break;}
case 211:
#line 2878 "parser-smi.y"
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
case 212:
#line 2898 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("TimeTicks");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "TimeTicks");
			    }
			;
    break;}
case 213:
#line 2906 "parser-smi.y"
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
case 214:
#line 2916 "parser-smi.y"
{
			    yyval.typePtr = findTypeByName("Counter64");
			    if (! yyval.typePtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter64");
			    }
			;
    break;}
case 215:
#line 2926 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				printError(thisParserPtr,
					   ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 216:
#line 2933 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				printError(thisParserPtr,
					   ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 217:
#line 2940 "parser-smi.y"
{
			    if (thisModulePtr->export.language == SMI_LANGUAGE_SMIV2)
				printError(thisParserPtr,
					   ERR_UNEXPECTED_TYPE_RESTRICTION);
			    yyval.listPtr = NULL;
			;
    break;}
case 218:
#line 2947 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 219:
#line 2961 "parser-smi.y"
{ yyval.listPtr = yyvsp[-1].listPtr; ;
    break;}
case 220:
#line 2971 "parser-smi.y"
{ yyval.listPtr = yyvsp[-2].listPtr; ;
    break;}
case 221:
#line 2975 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 222:
#line 2982 "parser-smi.y"
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
case 223:
#line 2996 "parser-smi.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    /* TODO: success? */
			    yyval.rangePtr->export.minValue = *yyvsp[0].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    util_free(yyvsp[0].valuePtr);
			;
    break;}
case 224:
#line 3004 "parser-smi.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    /* TODO: success? */
			    yyval.rangePtr->export.minValue = *yyvsp[-2].valuePtr;
			    yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			    util_free(yyvsp[0].valuePtr);
			;
    break;}
case 225:
#line 3015 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 226:
#line 3022 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    yyval.valuePtr->value.unsigned32 = yyvsp[0].unsigned32;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 227:
#line 3029 "parser-smi.y"
{
			    char s[3];
			    int i;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_HEXSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				yyval.valuePtr->value.ptr =
				    util_malloc((strlen(yyvsp[0].text)+1)/2+1);
				for (i = 0; i < strlen(yyvsp[0].text); i += 2) {
				    strncpy(s, &yyvsp[0].text[i], 2);
				    if (!s[1]) s[1] = '0';
				    s[2] = 0;
				    yyval.valuePtr->value.ptr[i/2] = strtol(s, 0, 16);
				}
				yyval.valuePtr->len = (strlen(yyvsp[0].text)+1)/2;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 16);
			    }
			;
    break;}
case 228:
#line 3053 "parser-smi.y"
{
			    char s[9];
			    int i, j;
			    
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_BINSTRING;
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				yyval.valuePtr->value.ptr =
				    util_malloc((strlen(yyvsp[0].text)+7)/8+1);
				for (i = 0; i < strlen(yyvsp[0].text); i += 8) {
				    strncpy(s, &yyvsp[0].text[i], 8);
				    for (j = 1; j < 8; j++) {
					if (!s[j]) s[j] = '0';
				    }
				    s[8] = 0;
				    yyval.valuePtr->value.ptr[i/8] = strtol(s, 0, 2);
				}
				yyval.valuePtr->len = (strlen(yyvsp[0].text)+7)/8;
			    } else {
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 2);
			    }
			;
    break;}
case 229:
#line 3081 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 230:
#line 3087 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 231:
#line 3094 "parser-smi.y"
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
case 232:
#line 3108 "parser-smi.y"
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
case 233:
#line 3121 "parser-smi.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    /* TODO: success? */
			    yyval.namedNumberPtr->export.name = yyvsp[-4].id; /* JS: needs strdup? */
			    yyval.namedNumberPtr->export.value = *yyvsp[-1].valuePtr;
			    util_free(yyvsp[-1].valuePtr);
			;
    break;}
case 234:
#line 3131 "parser-smi.y"
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
case 235:
#line 3142 "parser-smi.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
			    yyval.valuePtr->value.integer32 = yyvsp[0].integer32;
			    /* TODO: non-negative is suggested */
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			;
    break;}
case 236:
#line 3152 "parser-smi.y"
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
case 237:
#line 3186 "parser-smi.y"
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
case 238:
#line 3202 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 239:
#line 3206 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 240:
#line 3212 "parser-smi.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 241:
#line 3216 "parser-smi.y"
{
			    yyval.text = NULL;
			;
    break;}
case 242:
#line 3222 "parser-smi.y"
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
				    yyval.access = SMI_ACCESS_READ_WRITE;
				    /* TODO: remember it's really write-only */
				} else {
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV1_ACCESS, yyvsp[0].id);
				    yyval.access = SMI_ACCESS_UNKNOWN;
				}
			    }
			    util_free(yyvsp[0].id);
			;
    break;}
case 243:
#line 3265 "parser-smi.y"
{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			;
    break;}
case 244:
#line 3273 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.implied   = impliedFlag;
			    yyval.index.listPtr   = yyvsp[-1].listPtr;
			    yyval.index.rowPtr    = NULL;
			;
    break;}
case 245:
#line 3282 "parser-smi.y"
{
			    yyval.index.indexkind    = SMI_INDEX_AUGMENT;
			    yyval.index.implied      = 0;
			    yyval.index.listPtr      = NULL;
			    yyval.index.rowPtr       = yyvsp[-1].objectPtr;
			;
    break;}
case 246:
#line 3289 "parser-smi.y"
{
			    yyval.index.indexkind = SMI_INDEX_UNKNOWN;
			;
    break;}
case 247:
#line 3295 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 248:
#line 3303 "parser-smi.y"
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
case 249:
#line 3316 "parser-smi.y"
{
			    impliedFlag = 1;
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 250:
#line 3321 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 251:
#line 3331 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 252:
#line 3337 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 253:
#line 3343 "parser-smi.y"
{ yyval.valuePtr = yyvsp[-1].valuePtr; ;
    break;}
case 254:
#line 3345 "parser-smi.y"
{ yyval.valuePtr = NULL; ;
    break;}
case 255:
#line 3350 "parser-smi.y"
{ yyval.valuePtr = yyvsp[0].valuePtr; ;
    break;}
case 256:
#line 3352 "parser-smi.y"
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
case 257:
#line 3373 "parser-smi.y"
{ yyval.listPtr = yyvsp[0].listPtr; ;
    break;}
case 258:
#line 3375 "parser-smi.y"
{ yyval.listPtr = NULL; ;
    break;}
case 259:
#line 3379 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].id; /* JS: needs strdup? */
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 260:
#line 3386 "parser-smi.y"
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
case 261:
#line 3400 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 262:
#line 3406 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 263:
#line 3412 "parser-smi.y"
{ yyval.text = yyvsp[0].text; ;
    break;}
case 264:
#line 3414 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 265:
#line 3418 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 266:
#line 3420 "parser-smi.y"
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
case 267:
#line 3432 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 268:
#line 3434 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 269:
#line 3439 "parser-smi.y"
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
case 270:
#line 3461 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 271:
#line 3465 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 272:
#line 3471 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 273:
#line 3478 "parser-smi.y"
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
case 274:
#line 3492 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 275:
#line 3498 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 276:
#line 3504 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 277:
#line 3511 "parser-smi.y"
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
case 278:
#line 3525 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 279:
#line 3531 "parser-smi.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 280:
#line 3537 "parser-smi.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 281:
#line 3542 "parser-smi.y"
{
			    parentNodePtr = rootNodePtr;
			;
    break;}
case 282:
#line 3546 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    parentNodePtr = yyvsp[0].objectPtr->nodePtr;
			;
    break;}
case 283:
#line 3554 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 284:
#line 3559 "parser-smi.y"
{
			    yyval.objectPtr = yyvsp[0].objectPtr;
			;
    break;}
case 285:
#line 3566 "parser-smi.y"
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
					    setObjectFileOffset(objectPtr,
						     thisParserPtr->character);
					}
					yyval.objectPtr = objectPtr;
				    } else {
					/*
					 * imported object.
					 */
					importPtr->use++;
#if 0
					snodePtr = smiGetNode(
						  importPtr->export.module, yyvsp[0].id);
					if (snodePtr) {
					    yyval.objectPtr = addObject(yyvsp[0].id, 
							   getParentNode(
					          createNodes(snodePtr->oidlen,
						 	      snodePtr->oid)),
					    snodePtr->oid[snodePtr->oidlen-1],
							   FLAG_IMPORTED,
							   thisParserPtr);
					    smiFreeNode(snodePtr);
					} else {
					    yyval.objectPtr = addObject(yyvsp[0].id, pendingNodePtr,
							   0,
					       FLAG_IMPORTED | FLAG_INCOMPLETE,
							   thisParserPtr);
					}
#else
					yyval.objectPtr = findObjectByModulenameAndName(
					    importPtr->export.module, yyvsp[0].id);
#endif
				    }
				}
				parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			;
    break;}
case 286:
#line 3645 "parser-smi.y"
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
					    setObjectFileOffset(objectPtr,
						     thisParserPtr->character);
					}
					yyval.objectPtr = objectPtr;
				    } else {
					/*
					 * imported object.
					 */
					importPtr->use++;
#if 0
					snodePtr = smiGetNode(yyvsp[-2].id, yyvsp[0].id);
					yyval.objectPtr = addObject(yyvsp[0].id, 
					  getParentNode(
					      createNodes(snodePtr->oidlen,
						          snodePtr->oid)),
					  snodePtr->oid[snodePtr->oidlen-1],
					  FLAG_IMPORTED,
					  thisParserPtr);
					smiFreeNode(snodePtr);
#else
					yyval.objectPtr = findObjectByModulenameAndName(
					    importPtr->export.module, yyvsp[-2].id);
#endif
				    }
				}
				parentNodePtr = yyval.objectPtr->nodePtr;
			    }
			;
    break;}
case 287:
#line 3722 "parser-smi.y"
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
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
			    }
			    parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 288:
#line 3748 "parser-smi.y"
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
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
				yyval.objectPtr = objectPtr;
			    }
			    
			    parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 289:
#line 3779 "parser-smi.y"
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
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
				yyval.objectPtr = objectPtr;
			    }
			    util_free(md);
			    parentNodePtr = yyval.objectPtr->nodePtr;
			;
    break;}
case 290:
#line 3812 "parser-smi.y"
{ yyval.text = NULL; ;
    break;}
case 291:
#line 3816 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 292:
#line 3818 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 293:
#line 3822 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 294:
#line 3824 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 295:
#line 3828 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 296:
#line 3834 "parser-smi.y"
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
case 297:
#line 3852 "parser-smi.y"
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
			    setObjectDescription(objectPtr, yyvsp[-5].text);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-9].listPtr);
			    yyval.err = 0;
			;
    break;}
case 298:
#line 3879 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 299:
#line 3885 "parser-smi.y"
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
case 300:
#line 3904 "parser-smi.y"
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
			    setObjectDescription(objectPtr, yyvsp[-5].text);
			    if (yyvsp[-4].text) {
				setObjectReference(objectPtr, yyvsp[-4].text);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, yyvsp[-9].listPtr);
			    yyval.err = 0;
			;
    break;}
case 301:
#line 3932 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 302:
#line 3938 "parser-smi.y"
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
case 303:
#line 3957 "parser-smi.y"
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
			    setObjectDescription(objectPtr, yyvsp[-6].text);
			    if (yyvsp[-5].text) {
				setObjectReference(objectPtr, yyvsp[-5].text);
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
				    
		    s = util_malloc(strlen(refinementPtr->objectPtr->export.name) +
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
				}
			    }

			    yyval.err = 0;
			;
    break;}
case 304:
#line 4033 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 305:
#line 4039 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 306:
#line 4043 "parser-smi.y"
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
case 307:
#line 4078 "parser-smi.y"
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
case 308:
#line 4094 "parser-smi.y"
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
case 309:
#line 4107 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[-1].id);
			    /* TODO: handle objectIdentifier */
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[-1].id);
			    }
			;
    break;}
case 310:
#line 4115 "parser-smi.y"
{
			    yyval.modulePtr = findModuleByName(yyvsp[0].id);
			    if (!yyval.modulePtr) {
				yyval.modulePtr = loadModule(yyvsp[0].id);
			    }
			;
    break;}
case 311:
#line 4123 "parser-smi.y"
{
			    yyval.modulePtr = thisModulePtr;
			;
    break;}
case 312:
#line 4129 "parser-smi.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 313:
#line 4133 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 314:
#line 4139 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].objectPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 315:
#line 4146 "parser-smi.y"
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
case 316:
#line 4160 "parser-smi.y"
{
			    Import *importPtr;

			    /*
			     * The object found may be an implicitly
			     * created object with flags & FLAG_IMPORTED.
			     * We want to get the original definition:
			     */
			    yyval.objectPtr = yyvsp[0].objectPtr;
			    if (complianceModulePtr) {
				yyval.objectPtr = findObjectByModuleAndName(
				                           complianceModulePtr,
							   yyvsp[0].objectPtr->export.name);
			    }
			    if (complianceModulePtr && yyvsp[0].objectPtr->export.name) {
				importPtr = findImportByModulenameAndName(
						    complianceModulePtr->export.name,
						    yyvsp[0].objectPtr->export.name,
						    thisModulePtr);
				if (importPtr)
				    importPtr->use++;
			    }
			;
    break;}
case 317:
#line 4186 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].compl.optionlistPtr;
			    yyval.compl.refinementlistPtr = yyvsp[0].compl.refinementlistPtr;
			;
    break;}
case 318:
#line 4192 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 319:
#line 4200 "parser-smi.y"
{
			    yyval.compl = yyvsp[0].compl;
			;
    break;}
case 320:
#line 4204 "parser-smi.y"
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
case 321:
#line 4271 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = yyvsp[0].listPtr;
			    yyval.compl.refinementlistPtr = NULL;
			;
    break;}
case 322:
#line 4277 "parser-smi.y"
{
			    yyval.compl.mandatorylistPtr = NULL;
			    yyval.compl.optionlistPtr = NULL;
			    yyval.compl.refinementlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 323:
#line 4286 "parser-smi.y"
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
case 324:
#line 4311 "parser-smi.y"
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
case 325:
#line 4336 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 326:
#line 4345 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 327:
#line 4351 "parser-smi.y"
{
			    if (yyvsp[0].typePtr->export.name) {
				yyval.typePtr = duplicateType(yyvsp[0].typePtr, 0, thisParserPtr);
				setTypeDecl(yyval.typePtr, SMI_DECL_IMPLICIT_TYPE);
			    } else {
				yyval.typePtr = yyvsp[0].typePtr;
			    }
			;
    break;}
case 328:
#line 4360 "parser-smi.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 329:
#line 4366 "parser-smi.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 330:
#line 4372 "parser-smi.y"
{
			    yyval.access = yyvsp[0].access;
			;
    break;}
case 331:
#line 4376 "parser-smi.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 332:
#line 4382 "parser-smi.y"
{
			    checkNameLen(thisParserPtr, yyvsp[0].id,
					 ERR_OIDNAME_32, ERR_OIDNAME_64);
			    checkObjectName(thisModulePtr, yyvsp[0].id, thisParserPtr);
			;
    break;}
case 333:
#line 4388 "parser-smi.y"
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
case 334:
#line 4408 "parser-smi.y"
{
			    Object *objectPtr;
			    
			    objectPtr = yyvsp[-1].objectPtr;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, yyvsp[-8].status);
			    setObjectDescription(objectPtr, yyvsp[-6].text);
			    if (yyvsp[-5].text) {
				setObjectReference(objectPtr, yyvsp[-5].text);
			    }
				/*
				 * TODO: PRODUCT_RELEASE Text
				 * TODO: ModulePart_Capabilities
				 */
			    yyval.err = 0;
			;
    break;}
case 335:
#line 4436 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 336:
#line 4438 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 337:
#line 4442 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 338:
#line 4444 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 339:
#line 4452 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 340:
#line 4456 "parser-smi.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    /* TODO: success? */
			    yyval.listPtr->ptr = yyvsp[0].listPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 341:
#line 4463 "parser-smi.y"
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
case 342:
#line 4477 "parser-smi.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 343:
#line 4483 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 344:
#line 4485 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 345:
#line 4489 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 346:
#line 4493 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 347:
#line 4495 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 348:
#line 4499 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 349:
#line 4501 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 350:
#line 4505 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 351:
#line 4507 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 352:
#line 4513 "parser-smi.y"
{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			;
    break;}
case 353:
#line 4526 "parser-smi.y"
{
			    thisParserPtr->flags &= ~FLAG_CREATABLE;
			    yyval.err = 0;
			;
    break;}
case 354:
#line 4533 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 355:
#line 4535 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 356:
#line 4539 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 357:
#line 4541 "parser-smi.y"
{ yyval.err = 0; ;
    break;}
case 358:
#line 4545 "parser-smi.y"
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
#line 4548 "parser-smi.y"


#endif /*  */
