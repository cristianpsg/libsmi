
/*  A Bison parser, made from parser-sming.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse smingparse
#define yylex sminglex
#define yyerror smingerror
#define yylval sminglval
#define yychar smingchar
#define yydebug smingdebug
#define yynerrs smingnerrs
#define	DOT_DOT	258
#define	COLON_COLON	259
#define	ucIdentifier	260
#define	lcIdentifier	261
#define	floatValue	262
#define	textSegment	263
#define	decimalNumber	264
#define	hexadecimalNumber	265
#define	moduleKeyword	266
#define	importKeyword	267
#define	revisionKeyword	268
#define	oidKeyword	269
#define	dateKeyword	270
#define	organizationKeyword	271
#define	contactKeyword	272
#define	descriptionKeyword	273
#define	referenceKeyword	274
#define	extensionKeyword	275
#define	typedefKeyword	276
#define	typeKeyword	277
#define	writetypeKeyword	278
#define	nodeKeyword	279
#define	scalarKeyword	280
#define	tableKeyword	281
#define	columnKeyword	282
#define	rowKeyword	283
#define	notificationKeyword	284
#define	groupKeyword	285
#define	complianceKeyword	286
#define	formatKeyword	287
#define	unitsKeyword	288
#define	statusKeyword	289
#define	accessKeyword	290
#define	defaultKeyword	291
#define	impliedKeyword	292
#define	indexKeyword	293
#define	augmentsKeyword	294
#define	reordersKeyword	295
#define	sparseKeyword	296
#define	expandsKeyword	297
#define	createKeyword	298
#define	membersKeyword	299
#define	objectsKeyword	300
#define	mandatoryKeyword	301
#define	optionalKeyword	302
#define	refineKeyword	303
#define	OctetStringKeyword	304
#define	ObjectIdentifierKeyword	305
#define	Integer32Keyword	306
#define	Unsigned32Keyword	307
#define	Integer64Keyword	308
#define	Unsigned64Keyword	309
#define	Float32Keyword	310
#define	Float64Keyword	311
#define	Float128Keyword	312
#define	BitsKeyword	313
#define	EnumerationKeyword	314
#define	currentKeyword	315
#define	deprecatedKeyword	316
#define	obsoleteKeyword	317
#define	noaccessKeyword	318
#define	notifyonlyKeyword	319
#define	readonlyKeyword	320
#define	readwriteKeyword	321
#define	readcreateKeyword	322

#line 14 "parser-sming.y"


#include <config.h>
    
#ifdef BACKEND_SMING

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
    
#include "smi.h"
#include "error.h"
#include "parser-sming.h"
#include "scanner-sming.h"
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



static char *typeIdentifier, *macroIdentifier, *nodeIdentifier,
	    *scalarIdentifier, *tableIdentifier, *rowIdentifier,
	    *columnIdentifier, *notificationIdentifier,
	    *groupIdentifier, *complianceIdentifier;
static char *importModulename = NULL;
static Object *moduleObjectPtr = NULL;
static Object *nodeObjectPtr = NULL;
static Object *scalarObjectPtr = NULL;
static Object *tableObjectPtr = NULL;
static Object *rowObjectPtr = NULL;
static Object *columnObjectPtr = NULL;
static Object *notificationObjectPtr = NULL;
static Object *groupObjectPtr = NULL;
static Object *complianceObjectPtr = NULL;
static Type *typePtr = NULL;
static Macro *macroPtr = NULL;
static SmiBasetype defaultBasetype = SMI_BASETYPE_UNKNOWN;


#define SMI_EPOCH	631152000	/* 01 Jan 1990 00:00:00 */ 
 

static Type *
findType(spec, parserPtr, modulePtr)
    char *spec;
    Parser *parserPtr;
    Module *modulePtr;
{
    Type *typePtr;
    Import *importPtr;
    char *module, *type;

    type = strstr(spec, "::");
    if (!type) {
	typePtr = findTypeByModuleAndName(modulePtr, spec);
	if (!typePtr) {
	    importPtr = findImportByName(spec, modulePtr);
	    if (importPtr) {
		typePtr = findTypeByModulenameAndName(importPtr->export.module,
						      spec);
	    }
	}
    } else {
	module = strtok(spec, ":");
	type = &type[2];
	typePtr = findTypeByModulenameAndName(module, type);
    }
    return typePtr;
}
 

			    
static Object *
findObject(spec, parserPtr, modulePtr)
    char *spec;
    Parser *parserPtr;
    Module *modulePtr;
{
    Object *objectPtr;
    Import *importPtr;
    char *module, *object;

    object = strstr(spec, "::");
    if (!object) {
	objectPtr = findObjectByModuleAndName(modulePtr, spec);
	if (!objectPtr) {
	    importPtr = findImportByName(spec, modulePtr);
	    if (importPtr) {
	     objectPtr = findObjectByModulenameAndName(importPtr->export.module,
							  spec);
	    }
	}
    } else {
	module = strtok(spec, ":");
	object = &object[2];
	objectPtr = findObjectByModulenameAndName(module, object);
    }
    return objectPtr;
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
    if (len == 10 || len == 16) {
	for (i = 0; i < len; i++) {
	    if (((i < 4 || i == 5 || i == 6 || i == 8 || i == 9 || i == 11
		  || i == 12 || i == 14 || i == 15) && ! isdigit(date[i]))
		|| ((i == 4 || i == 7) && date[i] != '-')
		|| (i == 10 && date[i] != ' ')
		|| (i == 13 && date[i] != ':')) {
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
	for (i = 0, p = date, tm.tm_year = 0; i < 4; i++, p++) {
	    tm.tm_year = tm.tm_year * 10 + (*p - '0');
	}
	p++;
	tm.tm_mon = (p[0]-'0') * 10 + (p[1]-'0');
	p += 3;
	tm.tm_mday = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	if (len == 16) {
	    p++;
	    tm.tm_hour = (p[0]-'0') * 10 + (p[1]-'0');
	    p += 3;
	    tm.tm_min = (p[0]-'0') * 10 + (p[1]-'0');
	}
	
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

			    

#line 239 "parser-sming.y"
typedef union {
    char           *id;				/* identifier name           */
    int            rc;				/* >=0: ok, <0: error        */
    time_t	   date;			/* a date value		     */
    char	   *text;
    Module	   *modulePtr;
    Node	   *nodePtr;
    Object	   *objectPtr;
    Macro	   *macroPtr;
    Type	   *typePtr;
    Index	   index;
    Option	   *optionPtr;
    Refinement	   *refinementPtr;
    SmiStatus	   status;
    SmiAccess	   access;
    NamedNumber    *namedNumberPtr;
    Range	   *rangePtr;
    SmiValue	   *valuePtr;
    List	   *listPtr;
    Revision	   *revisionPtr;
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



#define	YYFINAL		763
#define	YYFLAG		-32768
#define	YYNTBASE	77

#define YYTRANSLATE(x) ((unsigned)(x) <= 322 ? yytranslate[x] : 312)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    71,
    72,     2,     2,    74,    75,    76,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    70,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    68,    73,    69,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    66,    67
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     4,     6,     8,    11,    14,    15,    16,    17,
    18,    19,    20,    21,    57,    58,    60,    62,    65,    68,
    69,    70,    71,    72,    89,    90,    92,    94,    97,   100,
   101,   102,   103,   104,   105,   106,   107,   108,   134,   135,
   137,   139,   142,   145,   147,   149,   151,   152,   153,   154,
   155,   156,   176,   177,   178,   179,   180,   181,   182,   183,
   184,   185,   186,   219,   220,   221,   222,   223,   224,   247,
   248,   249,   250,   251,   252,   253,   254,   281,   283,   286,
   289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
   299,   332,   333,   335,   337,   340,   343,   344,   345,   346,
   347,   348,   349,   372,   373,   375,   377,   380,   383,   384,
   385,   386,   387,   388,   389,   413,   414,   416,   418,   421,
   424,   425,   426,   427,   428,   429,   430,   431,   432,   459,
   460,   462,   464,   467,   470,   471,   472,   486,   487,   489,
   491,   494,   497,   509,   515,   516,   519,   525,   526,   529,
   535,   537,   539,   541,   543,   545,   556,   562,   575,   581,
   594,   595,   598,   599,   602,   607,   608,   611,   617,   623,
   629,   635,   641,   642,   645,   651,   652,   655,   661,   662,
   665,   671,   672,   675,   681,   682,   685,   691,   692,   695,
   701,   702,   705,   711,   721,   722,   725,   735,   736,   739,
   749,   750,   752,   754,   757,   760,   771,   772,   774,   776,
   779,   782,   796,   798,   800,   803,   805,   808,   811,   814,
   817,   820,   823,   826,   829,   832,   835,   836,   839,   841,
   843,   844,   847,   854,   855,   857,   859,   862,   867,   870,
   871,   873,   878,   879,   882,   889,   890,   892,   894,   897,
   902,   905,   906,   908,   913,   919,   923,   924,   926,   928,
   931,   936,   944,   948,   949,   951,   953,   956,   961,   965,
   966,   968,   970,   973,   978,   982,   983,   985,   987,   990,
   995,  1001,  1003,  1007,  1008,  1010,  1012,  1015,  1020,  1022,
  1024,  1026,  1028,  1032,  1034,  1038,  1040,  1043,  1044,  1046,
  1048,  1051,  1054,  1056,  1058,  1060,  1062,  1064,  1067,  1069,
  1071,  1073,  1075,  1078,  1083,  1085,  1087,  1089,  1091,  1093,
  1095,  1097,  1100,  1102,  1104,  1105,  1107,  1109,  1112,  1115,
  1117,  1119,  1121,  1124,  1126,  1128,  1129,  1130,  1131
};

static const short yyrhs[] = {   310,
    78,     0,     0,    79,     0,    80,     0,    79,    80,     0,
    81,   310,     0,     0,     0,     0,     0,     0,     0,     0,
    11,   309,     5,    82,   309,     6,    83,   310,    68,   311,
   189,   215,   311,    84,   217,   311,    85,   218,   311,    86,
   230,   311,    87,   231,    88,   195,    89,    97,   109,   157,
   167,   177,    69,   310,    70,     0,     0,    90,     0,    91,
     0,    90,    91,     0,    92,   311,     0,     0,     0,     0,
     0,    20,   309,     6,    93,   310,    68,   311,   223,    94,
   229,    95,   231,    96,    69,   310,    70,     0,     0,    98,
     0,    99,     0,    98,    99,     0,   100,   311,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    21,   309,     5,
   101,   310,    68,   311,   199,   311,   102,   227,   103,   219,
   104,   221,   105,   223,   106,   229,   107,   231,   108,    69,
   310,    70,     0,     0,   110,     0,   111,     0,   110,   111,
     0,   112,   311,     0,   113,     0,   119,     0,   130,     0,
     0,     0,     0,     0,     0,    24,   309,     6,   114,   310,
    68,   311,   215,   311,   115,   223,   116,   229,   117,   231,
   118,    69,   310,    70,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    25,   309,     6,   120,   310,
    68,   311,   215,   311,   121,   201,   311,   122,   226,   311,
   123,   227,   124,   219,   125,   221,   126,   223,   127,   230,
   311,   128,   231,   129,    69,   310,    70,     0,     0,     0,
     0,     0,     0,    26,   309,     6,   131,   310,    68,   311,
   215,   311,   132,   223,   133,   230,   311,   134,   231,   135,
   136,   311,    69,   310,    70,     0,     0,     0,     0,     0,
     0,     0,     0,    28,   309,     6,   137,   310,    68,   311,
   215,   311,   138,   204,   311,   139,   211,   140,   223,   141,
   230,   311,   142,   231,   143,   144,    69,   310,    70,     0,
   145,     0,   144,   145,     0,   146,   311,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,   309,
     6,   147,   310,    68,   311,   215,   311,   148,   201,   311,
   149,   226,   311,   150,   227,   151,   219,   152,   221,   153,
   223,   154,   230,   311,   155,   231,   156,    69,   310,    70,
     0,     0,   158,     0,   159,     0,   158,   159,     0,   160,
   311,     0,     0,     0,     0,     0,     0,     0,    29,   309,
     6,   161,   310,    68,   311,   215,   311,   162,   234,   163,
   223,   164,   230,   311,   165,   231,   166,    69,   310,    70,
     0,     0,   168,     0,   169,     0,   168,   169,     0,   170,
   311,     0,     0,     0,     0,     0,     0,     0,    30,   309,
     6,   171,   310,    68,   311,   215,   311,   172,   233,   311,
   173,   223,   174,   230,   311,   175,   231,   176,    69,   310,
    70,     0,     0,   178,     0,   179,     0,   178,   179,     0,
   180,   311,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    31,   309,     6,   181,   310,    68,   311,   215,   311,
   182,   223,   183,   230,   311,   184,   231,   185,   236,   186,
   238,   187,   242,   188,    69,   310,    70,     0,     0,   190,
     0,   191,     0,   190,   191,     0,   192,   311,     0,     0,
     0,    12,   309,     5,   193,   310,    71,   310,   273,   194,
   310,    72,   310,    70,     0,     0,   196,     0,   197,     0,
   196,   197,     0,   198,   311,     0,    13,   310,    68,   311,
   216,   311,   230,   311,    69,   310,    70,     0,    22,   309,
   246,   310,    70,     0,     0,   201,   311,     0,    22,   309,
   246,   310,    70,     0,     0,   203,   311,     0,    23,   309,
   246,   310,    70,     0,   205,     0,   206,     0,   207,     0,
   208,     0,   209,     0,    38,   210,   310,    71,   310,   277,
   310,    72,   310,    70,     0,    39,   309,   288,   310,    70,
     0,    40,   309,   288,   210,   310,    71,   310,   277,   310,
    72,   310,    70,     0,    41,   309,   288,   310,    70,     0,
    42,   309,   288,   210,   310,    71,   310,   277,   310,    72,
   310,    70,     0,     0,   309,    37,     0,     0,   212,   311,
     0,    43,   213,   310,    70,     0,     0,   310,   214,     0,
    71,   310,   277,   310,    72,     0,    14,   309,   299,   310,
    70,     0,    15,   309,   293,   310,    70,     0,    16,   309,
   289,   310,    70,     0,    17,   309,   289,   310,    70,     0,
     0,   220,   311,     0,    32,   309,   294,   310,    70,     0,
     0,   222,   311,     0,    33,   309,   295,   310,    70,     0,
     0,   224,   311,     0,    34,   309,   297,   310,    70,     0,
     0,   226,   311,     0,    35,   309,   298,   310,    70,     0,
     0,   228,   311,     0,    36,   309,   296,   310,    70,     0,
     0,   230,   311,     0,    18,   309,   289,   310,    70,     0,
     0,   232,   311,     0,    19,   309,   289,   310,    70,     0,
    44,   310,    71,   310,   277,   310,    72,   310,    70,     0,
     0,   235,   311,     0,    45,   310,    71,   310,   277,   310,
    72,   310,    70,     0,     0,   237,   311,     0,    46,   310,
    71,   310,   277,   310,    72,   310,    70,     0,     0,   239,
     0,   240,     0,   239,   240,     0,   241,   311,     0,    47,
   309,   288,   310,    68,   230,   311,    69,   310,    70,     0,
     0,   243,     0,   244,     0,   243,   244,     0,   245,   311,
     0,    48,   309,   288,   310,    68,   200,   202,   225,   230,
   311,    69,   310,    70,     0,   247,     0,   248,     0,    49,
   251,     0,    50,     0,    51,   251,     0,    52,   251,     0,
    53,   251,     0,    54,   251,     0,    55,   259,     0,    56,
   259,     0,    57,   259,     0,    59,   267,     0,    58,   267,
     0,   287,   249,     0,     0,   310,   250,     0,   252,     0,
   260,     0,     0,   310,   252,     0,    71,   310,   256,   253,
   310,    72,     0,     0,   254,     0,   255,     0,   254,   255,
     0,   310,    73,   310,   256,     0,   307,   257,     0,     0,
   258,     0,   310,     3,   310,   307,     0,     0,   310,   260,
     0,    71,   310,   264,   261,   310,    72,     0,     0,   262,
     0,   263,     0,   262,   263,     0,   310,    73,   310,   264,
     0,     7,   265,     0,     0,   266,     0,   310,     3,   310,
     7,     0,    71,   310,   268,   310,    72,     0,   272,   269,
   308,     0,     0,   270,     0,   271,     0,   270,   271,     0,
   310,    74,   310,   272,     0,     6,   310,    71,   310,   305,
   310,    72,     0,   286,   274,   308,     0,     0,   275,     0,
   276,     0,   275,   276,     0,   310,    74,   310,   286,     0,
     0,     0,   308,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   310,    74,   310,     0,     0,   288,   278,   308,
     0,     0,   279,     0,   280,     0,   279,   280,     0,   310,
    74,   310,   288,     0,    71,   310,   282,   310,    72,     0,
   308,     0,     6,   283,   308,     0,     0,   284,     0,   285,
     0,   284,   285,     0,   310,    74,   310,     6,     0,     5,
     0,     6,     0,   287,     0,   288,     0,     5,     4,     5,
     0,     5,     0,     5,     4,     6,     0,     6,     0,     8,
   290,     0,     0,   291,     0,   292,     0,   291,   292,     0,
   310,     8,     0,     8,     0,     8,     0,     8,     0,   281,
     0,     9,     0,    75,     9,     0,    10,     0,     7,     0,
   289,     0,   288,     0,   288,   302,     0,   304,    76,   304,
   302,     0,    60,     0,    61,     0,    62,     0,    63,     0,
    64,     0,    65,     0,    66,     0,   300,   301,     0,   288,
     0,   304,     0,     0,   302,     0,   303,     0,   302,   303,
     0,    76,   304,     0,     9,     0,    10,     0,     9,     0,
    75,     9,     0,   305,     0,   306,     0,     0,     0,     0,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   515,   525,   529,   535,   539,   554,   568,   589,   593,   622,
   629,   636,   647,   657,   715,   719,   729,   733,   749,   763,
   772,   780,   786,   791,   798,   802,   812,   816,   832,   846,
   850,   871,   877,   887,   893,   899,   905,   910,   918,   922,
   932,   936,   952,   966,   967,   968,   971,   975,   991,   997,
  1010,  1015,  1023,  1027,  1041,  1056,  1063,  1069,  1079,  1085,
  1090,  1097,  1102,  1111,  1115,  1132,  1137,  1144,  1149,  1158,
  1162,  1178,  1203,  1214,  1219,  1226,  1232,  1240,  1248,  1263,
  1277,  1281,  1295,  1310,  1317,  1323,  1333,  1339,  1344,  1351,
  1356,  1365,  1369,  1379,  1383,  1399,  1413,  1417,  1433,  1449,
  1454,  1462,  1467,  1475,  1479,  1489,  1493,  1508,  1522,  1526,
  1539,  1556,  1561,  1568,  1573,  1581,  1585,  1595,  1599,  1615,
  1629,  1633,  1649,  1654,  1661,  1667,  1683,  1699,  1738,  1746,
  1750,  1756,  1760,  1775,  1789,  1793,  1806,  1821,  1825,  1831,
  1835,  1850,  1864,  1873,  1881,  1885,  1891,  1898,  1902,  1908,
  1915,  1916,  1917,  1918,  1919,  1922,  1943,  1959,  1981,  1997,
  2019,  2023,  2029,  2033,  2039,  2049,  2053,  2059,  2065,  2071,
  2077,  2083,  2089,  2093,  2099,  2105,  2109,  2115,  2121,  2125,
  2131,  2137,  2141,  2147,  2153,  2157,  2163,  2169,  2173,  2179,
  2185,  2189,  2195,  2201,  2208,  2212,  2218,  2225,  2229,  2235,
  2242,  2246,  2252,  2258,  2272,  2278,  2291,  2295,  2301,  2307,
  2320,  2326,  2356,  2360,  2366,  2381,  2385,  2400,  2415,  2430,
  2445,  2460,  2475,  2490,  2505,  2522,  2536,  2540,  2546,  2550,
  2556,  2560,  2566,  2575,  2579,  2585,  2591,  2604,  2610,  2624,
  2628,  2634,  2640,  2644,  2650,  2659,  2663,  2669,  2675,  2688,
  2694,  2713,  2717,  2723,  2729,  2735,  2744,  2748,  2754,  2760,
  2775,  2781,  2790,  2798,  2802,  2808,  2814,  2827,  2833,  2841,
  2845,  2851,  2857,  2870,  2876,  2884,  2888,  2894,  2900,  2913,
  2919,  2925,  2929,  2937,  2941,  2947,  2953,  2966,  2972,  2976,
  2982,  2986,  2992,  3003,  3009,  3020,  3026,  3040,  3044,  3050,
  3054,  3064,  3070,  3076,  3082,  3095,  3122,  3168,  3190,  3196,
  3202,  3215,  3239,  3244,  3251,  3255,  3259,  3265,  3269,  3273,
  3277,  3283,  3312,  3350,  3356,  3360,  3367,  3371,  3381,  3390,
  3396,  3404,  3413,  3422,  3426,  3436,  3443,  3449,  3455
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DOT_DOT",
"COLON_COLON","ucIdentifier","lcIdentifier","floatValue","textSegment","decimalNumber",
"hexadecimalNumber","moduleKeyword","importKeyword","revisionKeyword","oidKeyword",
"dateKeyword","organizationKeyword","contactKeyword","descriptionKeyword","referenceKeyword",
"extensionKeyword","typedefKeyword","typeKeyword","writetypeKeyword","nodeKeyword",
"scalarKeyword","tableKeyword","columnKeyword","rowKeyword","notificationKeyword",
"groupKeyword","complianceKeyword","formatKeyword","unitsKeyword","statusKeyword",
"accessKeyword","defaultKeyword","impliedKeyword","indexKeyword","augmentsKeyword",
"reordersKeyword","sparseKeyword","expandsKeyword","createKeyword","membersKeyword",
"objectsKeyword","mandatoryKeyword","optionalKeyword","refineKeyword","OctetStringKeyword",
"ObjectIdentifierKeyword","Integer32Keyword","Unsigned32Keyword","Integer64Keyword",
"Unsigned64Keyword","Float32Keyword","Float64Keyword","Float128Keyword","BitsKeyword",
"EnumerationKeyword","currentKeyword","deprecatedKeyword","obsoleteKeyword",
"noaccessKeyword","notifyonlyKeyword","readonlyKeyword","readwriteKeyword","readcreateKeyword",
"'{'","'}'","';'","'('","')'","'|'","','","'-'","'.'","smingFile","moduleStatement_optsep_0n",
"moduleStatement_optsep_1n","moduleStatement_optsep","moduleStatement","@1",
"@2","@3","@4","@5","@6","@7","extensionStatement_stmtsep_0n","extensionStatement_stmtsep_1n",
"extensionStatement_stmtsep","extensionStatement","@8","@9","@10","@11","typedefStatement_stmtsep_0n",
"typedefStatement_stmtsep_1n","typedefStatement_stmtsep","typedefStatement",
"@12","@13","@14","@15","@16","@17","@18","@19","anyObjectStatement_stmtsep_0n",
"anyObjectStatement_stmtsep_1n","anyObjectStatement_stmtsep","anyObjectStatement",
"nodeStatement","@20","@21","@22","@23","@24","scalarStatement","@25","@26",
"@27","@28","@29","@30","@31","@32","@33","@34","tableStatement","@35","@36",
"@37","@38","@39","rowStatement","@40","@41","@42","@43","@44","@45","@46","columnStatement_stmtsep_1n",
"columnStatement_stmtsep","columnStatement","@47","@48","@49","@50","@51","@52",
"@53","@54","@55","@56","notificationStatement_stmtsep_0n","notificationStatement_stmtsep_1n",
"notificationStatement_stmtsep","notificationStatement","@57","@58","@59","@60",
"@61","@62","groupStatement_stmtsep_0n","groupStatement_stmtsep_1n","groupStatement_stmtsep",
"groupStatement","@63","@64","@65","@66","@67","@68","complianceStatement_stmtsep_0n",
"complianceStatement_stmtsep_1n","complianceStatement_stmtsep","complianceStatement",
"@69","@70","@71","@72","@73","@74","@75","@76","importStatement_stmtsep_0n",
"importStatement_stmtsep_1n","importStatement_stmtsep","importStatement","@77",
"@78","revisionStatement_stmtsep_0n","revisionStatement_stmtsep_1n","revisionStatement_stmtsep",
"revisionStatement","typedefTypeStatement","typeStatement_stmtsep_01","typeStatement",
"writetypeStatement_stmtsep_01","writetypeStatement","anyIndexStatement","indexStatement",
"augmentsStatement","reordersStatement","sparseStatement","expandsStatement",
"sep_impliedKeyword_01","createStatement_stmtsep_01","createStatement","optsep_createColumns_01",
"createColumns","oidStatement","dateStatement","organizationStatement","contactStatement",
"formatStatement_stmtsep_01","formatStatement","unitsStatement_stmtsep_01","unitsStatement",
"statusStatement_stmtsep_01","statusStatement","accessStatement_stmtsep_01",
"accessStatement","defaultStatement_stmtsep_01","defaultStatement","descriptionStatement_stmtsep_01",
"descriptionStatement","referenceStatement_stmtsep_01","referenceStatement",
"membersStatement","objectsStatement_stmtsep_01","objectsStatement","mandatoryStatement_stmtsep_01",
"mandatoryStatement","optionalStatement_stmtsep_0n","optionalStatement_stmtsep_1n",
"optionalStatement_stmtsep","optionalStatement","refineStatement_stmtsep_0n",
"refineStatement_stmtsep_1n","refineStatement_stmtsep","refineStatement","refinedBaseType_refinedType",
"refinedBaseType","refinedType","optsep_anySpec_01","anySpec","optsep_numberSpec_01",
"numberSpec","furtherNumberElement_0n","furtherNumberElement_1n","furtherNumberElement",
"numberElement","numberUpperLimit_01","numberUpperLimit","optsep_floatSpec_01",
"floatSpec","furtherFloatElement_0n","furtherFloatElement_1n","furtherFloatElement",
"floatElement","floatUpperLimit_01","floatUpperLimit","bitsOrEnumerationSpec",
"bitsOrEnumerationList","furtherBitsOrEnumerationItem_0n","furtherBitsOrEnumerationItem_1n",
"furtherBitsOrEnumerationItem","bitsOrEnumerationItem","identifierList","furtherIdentifier_0n",
"furtherIdentifier_1n","furtherIdentifier","qlcIdentifierList","furtherQlcIdentifier_0n",
"furtherQlcIdentifier_1n","furtherQlcIdentifier","bitsValue","bitsList","furtherLcIdentifier_0n",
"furtherLcIdentifier_1n","furtherLcIdentifier","identifier","qucIdentifier",
"qlcIdentifier","text","optsep_textSegment_0n","optsep_textSegment_1n","optsep_textSegment",
"date","format","units","anyValue","status","access","objectIdentifier","qlcIdentifier_subid",
"dot_subid_0127","dot_subid_1n","dot_subid","subid","number","negativeNumber",
"signedNumber","optsep_comma_01","sep","optsep","stmtsep", NULL
};
#endif

static const short yyr1[] = {     0,
    77,    78,    78,    79,    79,    80,    82,    83,    84,    85,
    86,    87,    88,    81,    89,    89,    90,    90,    91,    93,
    94,    95,    96,    92,    97,    97,    98,    98,    99,   101,
   102,   103,   104,   105,   106,   107,   108,   100,   109,   109,
   110,   110,   111,   112,   112,   112,   114,   115,   116,   117,
   118,   113,   120,   121,   122,   123,   124,   125,   126,   127,
   128,   129,   119,   131,   132,   133,   134,   135,   130,   137,
   138,   139,   140,   141,   142,   143,   136,   144,   144,   145,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   146,   157,   157,   158,   158,   159,   161,   162,   163,   164,
   165,   166,   160,   167,   167,   168,   168,   169,   171,   172,
   173,   174,   175,   176,   170,   177,   177,   178,   178,   179,
   181,   182,   183,   184,   185,   186,   187,   188,   180,   189,
   189,   190,   190,   191,   193,   194,   192,   195,   195,   196,
   196,   197,   198,   199,   200,   200,   201,   202,   202,   203,
   204,   204,   204,   204,   204,   205,   206,   207,   208,   209,
   210,   210,   211,   211,   212,   213,   213,   214,   215,   216,
   217,   218,   219,   219,   220,   221,   221,   222,   223,   223,
   224,   225,   225,   226,   227,   227,   228,   229,   229,   230,
   231,   231,   232,   233,   234,   234,   235,   236,   236,   237,
   238,   238,   239,   239,   240,   241,   242,   242,   243,   243,
   244,   245,   246,   246,   247,   247,   247,   247,   247,   247,
   247,   247,   247,   247,   247,   248,   249,   249,   250,   250,
   251,   251,   252,   253,   253,   254,   254,   255,   256,   257,
   257,   258,   259,   259,   260,   261,   261,   262,   262,   263,
   264,   265,   265,   266,   267,   268,   269,   269,   270,   270,
   271,   272,   273,   274,   274,   275,   275,   276,    -1,    -1,
    -1,    -1,    -1,    -1,   277,   278,   278,   279,   279,   280,
   281,   282,   282,   283,   283,   284,   284,   285,   286,   286,
    -1,    -1,   287,   287,   288,   288,   289,   290,   290,   291,
   291,   292,   293,   294,   295,   296,   296,   296,   296,   296,
   296,   296,   296,   296,   297,   297,   297,   298,   298,   298,
   298,   299,   300,   300,   301,   301,   302,   302,   303,   304,
   305,   305,   306,   307,   307,   308,   309,   310,   311
};

static const short yyr2[] = {     0,
     2,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,     0,     0,    35,     0,     1,     1,     2,     2,     0,
     0,     0,     0,    16,     0,     1,     1,     2,     2,     0,
     0,     0,     0,     0,     0,     0,     0,    25,     0,     1,
     1,     2,     2,     1,     1,     1,     0,     0,     0,     0,
     0,    19,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    32,     0,     0,     0,     0,     0,    22,     0,
     0,     0,     0,     0,     0,     0,    26,     1,     2,     2,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    32,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,     0,    22,     0,     1,     1,     2,     2,     0,     0,
     0,     0,     0,     0,    23,     0,     1,     1,     2,     2,
     0,     0,     0,     0,     0,     0,     0,     0,    26,     0,
     1,     1,     2,     2,     0,     0,    13,     0,     1,     1,
     2,     2,    11,     5,     0,     2,     5,     0,     2,     5,
     1,     1,     1,     1,     1,    10,     5,    12,     5,    12,
     0,     2,     0,     2,     4,     0,     2,     5,     5,     5,
     5,     5,     0,     2,     5,     0,     2,     5,     0,     2,
     5,     0,     2,     5,     0,     2,     5,     0,     2,     5,
     0,     2,     5,     9,     0,     2,     9,     0,     2,     9,
     0,     1,     1,     2,     2,    10,     0,     1,     1,     2,
     2,    13,     1,     1,     2,     1,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     0,     2,     1,     1,
     0,     2,     6,     0,     1,     1,     2,     4,     2,     0,
     1,     4,     0,     2,     6,     0,     1,     1,     2,     4,
     2,     0,     1,     4,     5,     3,     0,     1,     1,     2,
     4,     7,     3,     0,     1,     1,     2,     4,     3,     0,
     1,     1,     2,     4,     3,     0,     1,     1,     2,     4,
     5,     1,     3,     0,     1,     1,     2,     4,     1,     1,
     1,     1,     3,     1,     3,     1,     2,     0,     1,     1,
     2,     2,     1,     1,     1,     1,     1,     2,     1,     1,
     1,     1,     2,     4,     1,     1,     1,     1,     1,     1,
     1,     2,     1,     1,     0,     1,     1,     2,     2,     1,
     1,     1,     2,     1,     1,     0,     0,     0,     0
};

static const short yydefact[] = {   338,
     2,   337,     1,     3,     4,   338,     0,     5,     6,     7,
   337,     0,     8,   338,     0,   339,   130,   337,     0,   131,
   132,   339,     0,   337,   339,   133,   134,   135,     0,     9,
   338,     0,   296,   330,   323,   338,   325,   324,     0,     0,
     0,     0,     0,   322,   326,   327,   337,   339,   338,   295,
   169,   329,   328,     0,    10,     0,   298,   338,     0,   289,
   290,   136,   264,   297,   299,   300,     0,     0,   337,   339,
   338,   336,   265,   266,     0,   301,   302,   171,     0,    11,
     0,   263,   267,   338,   338,     0,   338,     0,     0,   337,
   339,     0,   268,   172,     0,    12,   137,   338,   191,     0,
   337,    13,   339,   190,     0,   138,   192,   338,   338,    15,
   139,   140,   339,     0,     0,   337,    25,    16,    17,   339,
   141,   142,   193,   339,     0,   337,    39,    26,    27,   339,
    18,    19,     0,    20,     0,   337,   337,   337,    92,    40,
    41,   339,    44,    45,    46,    28,    29,   337,   339,   338,
    30,     0,     0,     0,   337,   104,    93,    94,   339,    42,
    43,     0,     0,     0,   338,    47,    53,    64,     0,   337,
   116,   105,   106,   339,    95,    96,   303,   338,   339,   339,
     0,   338,   338,   338,    97,     0,   337,     0,   117,   118,
   339,   107,   108,     0,     0,   179,   339,     0,     0,     0,
   338,   109,     0,   338,   119,   120,   170,   338,   337,    21,
   339,     0,   339,   339,   339,     0,   338,   121,     0,     0,
     0,   188,   180,   337,   339,     0,     0,     0,   339,     0,
   338,    14,   143,   315,   316,   317,   338,    22,   339,     0,
    31,   339,   339,   339,     0,   339,     0,     0,   191,   189,
   294,   231,   216,   231,   231,   231,   231,   243,   243,   243,
     0,     0,   338,   213,   214,   227,   185,    48,    54,    65,
   339,     0,   339,   181,    23,     0,   215,     0,   217,   218,
   219,   220,   221,     0,   222,   223,   338,   225,   224,     0,
   226,     0,   337,    32,   339,   179,     0,   179,    98,   339,
     0,     0,   293,   338,   232,   338,   244,     0,   144,   338,
   228,   229,   230,     0,   173,   186,    49,   337,   339,    66,
   195,   110,   339,   338,     0,     0,   338,   338,   257,     0,
   310,   307,   309,   338,     0,   306,   312,   311,   338,     0,
   337,    33,   339,   188,     0,    55,     0,   338,    99,   339,
     0,   122,     0,   332,   331,     0,   234,   334,   335,   240,
   252,   246,     0,     0,   336,   258,   259,     0,   336,   308,
   313,     0,     0,     0,   176,   174,    50,   338,     0,   339,
     0,   179,   196,   338,   339,   179,    24,   333,   338,   235,
   236,     0,   239,   241,     0,   251,   253,     0,   338,   247,
   248,     0,   338,   255,   256,   260,   338,   284,   338,   282,
   187,     0,   304,   338,   337,    34,   339,   191,     0,   337,
   339,    67,   338,   100,     0,   111,   123,     0,   237,   338,
   338,   338,     0,   249,   338,     0,     0,   336,   285,   286,
     0,     0,   314,     0,     0,   179,   177,    51,   147,     0,
    56,   191,     0,     0,   338,   179,     0,   233,     0,     0,
     0,   245,     0,   338,   261,   283,   287,   338,   281,   175,
   305,   338,    35,     0,   318,   319,   320,   321,   338,   185,
    68,   338,   276,   339,     0,   112,   339,   238,   242,   254,
   250,     0,     0,     0,   188,   338,     0,    57,     0,     0,
   336,   277,   278,     0,   101,   338,     0,   124,   262,   288,
   178,    36,     0,   184,   173,   337,   339,   338,   275,   279,
   338,   191,     0,   339,   191,   191,    52,    58,     0,     0,
     0,     0,   102,   338,   113,   125,    37,   176,    70,   338,
   197,   280,     0,     0,   191,   198,     0,    59,   338,     0,
   338,   194,   114,   338,   126,   339,   338,   179,     0,    69,
     0,     0,     0,   201,   199,     0,    60,   339,   103,   338,
   338,   337,   127,   202,   203,   339,    38,     0,     0,     0,
     0,     0,   207,   204,   205,   339,   339,   115,   338,   338,
   337,   128,   208,   209,   339,    61,    71,     0,     0,     0,
     0,   210,   211,   191,     0,   338,     0,   338,   338,    62,
   161,   337,   337,   337,   337,   339,   151,   152,   153,   154,
   155,     0,   339,     0,     0,     0,   338,     0,     0,     0,
     0,     0,    72,   200,     0,   145,   129,   338,     0,   162,
   338,   161,   338,   161,   163,   338,   148,   339,     0,   338,
     0,   338,     0,   338,   166,    73,   339,     0,   337,   182,
   339,   146,    63,     0,   157,     0,   159,     0,   338,     0,
   179,   164,   206,     0,     0,   339,   149,   338,   338,   338,
     0,   338,   167,    74,   338,   339,   183,     0,     0,     0,
   165,     0,     0,     0,     0,   338,   338,   338,   338,   339,
   150,   338,     0,     0,     0,     0,    75,     0,   156,   338,
   338,   168,   191,   212,     0,     0,    76,   158,   160,     0,
   337,     0,    78,   339,     0,   338,    79,    80,    81,     0,
   338,    77,     0,   339,     0,   339,    82,     0,   339,    83,
     0,   339,    84,   185,    85,   173,    86,   176,    87,   179,
    88,     0,   339,    89,   191,    90,     0,   338,     0,    91,
     0,     0,     0
};

static const short yydefgoto[] = {   761,
     3,     4,     5,     6,    11,    14,    39,    59,    86,    99,
   106,   117,   118,   119,   120,   150,   222,   249,   302,   127,
   128,   129,   130,   165,   267,   315,   375,   446,   495,   526,
   547,   139,   140,   141,   142,   143,   182,   296,   344,   418,
   474,   144,   183,   297,   379,   480,   515,   538,   558,   578,
   604,   626,   145,   184,   298,   347,   452,   499,   517,   549,
   605,   645,   671,   693,   713,   720,   722,   723,   724,   731,
   738,   741,   744,   746,   748,   750,   752,   755,   757,   156,
   157,   158,   159,   201,   321,   382,   454,   522,   543,   171,
   172,   173,   174,   217,   351,   456,   507,   545,   562,   188,
   189,   190,   191,   231,   386,   457,   525,   546,   564,   583,
   601,    19,    20,    21,    22,    31,    71,   110,   111,   112,
   113,   225,   647,   319,   660,   661,   616,   617,   618,   619,
   620,   621,   627,   656,   657,   669,   683,    25,   149,    48,
    70,   342,   343,   416,   417,   210,   211,   675,   421,   294,
   295,   238,   239,   102,   103,   385,   349,   350,   555,   556,
   573,   574,   575,   576,   592,   593,   594,   595,   263,   264,
   265,   291,   311,   277,   305,   389,   390,   391,   357,   393,
   394,   283,   307,   399,   400,   401,   362,   396,   397,   288,
   328,   365,   366,   367,   329,    62,    72,    73,    74,   482,
   501,   502,   503,   336,   409,   438,   439,   440,    63,   266,
   483,    58,    64,    65,    66,   178,   414,   472,   339,   237,
   479,    36,    37,    44,    45,    46,    38,   358,   359,   360,
    82,   628,   278,    17
};

static const short yypact[] = {-32768,
    21,-32768,-32768,    21,-32768,-32768,    30,-32768,-32768,-32768,
-32768,    77,-32768,-32768,    18,-32768,    78,-32768,    88,    78,
-32768,-32768,    87,-32768,-32768,-32768,-32768,-32768,    50,-32768,
-32768,    99,-32768,-32768,-32768,-32768,    29,-32768,    91,    40,
   106,    44,   107,-32768,    29,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   109,-32768,    33,   110,-32768,    98,-32768,
-32768,-32768,    45,-32768,   110,-32768,   112,    63,-32768,-32768,
-32768,-32768,    45,-32768,    60,-32768,-32768,-32768,   109,-32768,
    64,-32768,-32768,-32768,-32768,   117,-32768,    33,    67,-32768,
-32768,    68,-32768,-32768,   109,-32768,-32768,-32768,   120,    70,
-32768,-32768,-32768,-32768,   109,   128,-32768,-32768,-32768,   122,
   128,-32768,-32768,    74,    75,-32768,   124,   122,-32768,-32768,
-32768,-32768,-32768,-32768,   140,-32768,    36,   124,-32768,-32768,
-32768,-32768,   132,-32768,   143,-32768,-32768,-32768,   123,    36,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   145,   147,   149,-32768,   119,   123,-32768,-32768,-32768,
-32768,   148,   117,    90,-32768,-32768,-32768,-32768,   153,-32768,
   129,   119,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    93,-32768,-32768,-32768,-32768,   156,-32768,    94,   129,-32768,
-32768,-32768,-32768,   100,    95,   134,-32768,   101,   103,   104,
-32768,-32768,   167,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   152,-32768,-32768,-32768,   108,-32768,-32768,   105,   111,
    14,   117,-32768,-32768,-32768,    88,    88,    88,-32768,   118,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    73,
-32768,-32768,-32768,-32768,    88,-32768,   121,   125,   120,-32768,
   173,   116,-32768,   116,   116,   116,   116,   116,   116,   116,
   126,   126,-32768,-32768,-32768,   116,   144,-32768,-32768,-32768,
-32768,    88,-32768,-32768,-32768,   174,-32768,   127,-32768,-32768,
-32768,-32768,-32768,   131,-32768,-32768,-32768,-32768,-32768,   130,
-32768,   135,-32768,-32768,-32768,   134,   166,   134,-32768,-32768,
    88,   136,-32768,-32768,-32768,-32768,-32768,   184,-32768,-32768,
-32768,-32768,-32768,    35,   159,-32768,-32768,-32768,-32768,-32768,
   154,-32768,-32768,-32768,    24,   185,-32768,-32768,    45,    19,
-32768,   133,-32768,-32768,   194,-32768,    29,-32768,-32768,   137,
-32768,-32768,-32768,   117,    73,-32768,   117,-32768,-32768,-32768,
   150,-32768,   141,-32768,-32768,   198,   139,-32768,-32768,   190,
   190,   139,   155,   138,-32768,    45,-32768,   142,   208,-32768,
    29,   157,   107,   207,   187,-32768,-32768,-32768,   183,-32768,
   158,   134,-32768,-32768,-32768,   134,-32768,-32768,-32768,   139,
-32768,   160,-32768,-32768,   222,-32768,-32768,   225,-32768,   139,
-32768,   161,-32768,-32768,-32768,-32768,-32768,    45,-32768,-32768,
-32768,    29,-32768,-32768,-32768,-32768,-32768,   120,   162,-32768,
-32768,-32768,-32768,-32768,   164,-32768,-32768,   168,-32768,-32768,
-32768,-32768,   169,-32768,-32768,    37,   184,-32768,    45,-32768,
   165,   170,    29,   175,   230,   134,-32768,-32768,-32768,     4,
-32768,   120,    83,   117,-32768,   134,   117,-32768,    24,    24,
   223,-32768,   185,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   178,-32768,-32768,-32768,-32768,-32768,   144,
-32768,-32768,    45,-32768,    83,-32768,-32768,-32768,-32768,-32768,
-32768,   171,   238,   179,   117,-32768,   180,-32768,   220,   181,
-32768,    45,-32768,   177,-32768,-32768,   117,-32768,-32768,-32768,
-32768,-32768,   182,-32768,   159,-32768,-32768,-32768,-32768,-32768,
-32768,   120,   189,-32768,   120,   120,-32768,-32768,   248,   186,
   192,    83,-32768,-32768,-32768,-32768,-32768,   187,-32768,-32768,
-32768,-32768,   188,   195,   120,   210,   199,-32768,-32768,   197,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   134,   196,-32768,
   200,   202,   204,   226,-32768,   206,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   226,-32768,-32768,-32768,   117,    88,   209,
    83,    83,   229,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   229,-32768,-32768,-32768,-32768,   211,   212,    83,
   213,-32768,-32768,   120,    12,-32768,   117,-32768,-32768,-32768,
   241,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   214,-32768,   218,   221,   219,-32768,   244,    83,    83,
    83,    83,-32768,-32768,   227,   166,-32768,-32768,   224,-32768,
-32768,   241,-32768,   241,   247,-32768,   271,-32768,   228,-32768,
   231,-32768,   232,-32768,   116,-32768,-32768,   233,-32768,   183,
-32768,-32768,-32768,    83,-32768,   236,-32768,   237,-32768,   240,
   134,-32768,-32768,    73,   117,-32768,-32768,-32768,-32768,-32768,
   239,-32768,-32768,-32768,-32768,-32768,-32768,   242,    83,    83,
-32768,    83,   117,   243,   246,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   249,   245,   250,   251,-32768,   255,-32768,-32768,
-32768,-32768,   120,-32768,   256,   260,-32768,-32768,-32768,   270,
-32768,     3,-32768,-32768,   293,-32768,-32768,-32768,-32768,   262,
-32768,-32768,   252,-32768,    88,-32768,-32768,   166,-32768,-32768,
   183,-32768,-32768,   144,-32768,   159,-32768,   187,-32768,   134,
-32768,   117,-32768,-32768,   120,-32768,   264,-32768,   265,-32768,
   300,   316,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   314,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   203,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   215,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   201,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -386,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   193,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   172,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   151,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   322,-32768,-32768,-32768,-32768,-32768,   234,
-32768,-32768,-32768,  -617,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -578,-32768,-32768,-32768,-32768,  -224,-32768,-32768,
-32768,  -510,-32768,  -525,-32768,  -289,-32768,-32768,  -637,  -470,
-32768,  -329,   -62,  -233,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -228,-32768,-32768,-32768,  -246,-32768,  -337,-32768,
-32768,-32768,-32768,  -175,    57,-32768,-32768,   -39,  -107,-32768,
-32768,  -164,    61,-32768,-32768,   -46,  -105,-32768,-32768,    97,
-32768,-32768,-32768,   -10,   -74,-32768,-32768,-32768,   291,  -468,
-32768,-32768,  -137,-32768,-32768,-32768,-32768,   -72,   280,-32768,
    -9,   -68,-32768,-32768,   304,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -312,   -33,   -42,   -66,-32768,   -88,
  -347,   710,     0,   445
};


#define	YYLAST		1431


static const short yytable[] = {     1,
    52,   242,   243,   244,   528,     9,   317,   378,   320,   498,
    85,    53,   548,    15,   377,   275,   506,   405,   648,    35,
   271,   410,   676,    91,   371,   361,    98,   354,   355,   721,
    40,     2,   354,   355,    10,    42,   108,    60,    61,    32,
    33,   331,    57,   332,   333,   354,   355,   300,    56,   611,
   612,   613,   614,   615,    32,    33,    67,    68,    34,   136,
   137,   138,    75,   652,    67,   654,   475,   476,   477,   478,
    81,   726,    75,   234,   235,   236,   323,   251,   279,   280,
   281,   282,    13,    88,    89,    16,    92,    32,    33,    18,
   466,    28,   424,   356,   285,   286,   427,   100,   356,   443,
   179,    24,    41,   742,    43,   334,    47,   114,   115,   335,
    49,    50,   589,    51,    69,    34,    57,  -338,  -338,    77,
   739,   252,   253,   254,   255,   256,   257,   258,   259,   260,
   261,   262,    78,    84,    90,    87,    94,    97,   101,   104,
   109,   116,   124,   123,   126,   134,   148,   151,   170,   164,
   166,   155,   167,   519,   168,   177,   473,   180,   185,   187,
   197,   202,   204,   208,   181,   512,   486,   209,   213,   207,
   214,   215,   218,   224,   232,   229,   276,   194,   303,   293,
   233,   198,   199,   200,   448,   246,  -338,   318,   273,   327,
   341,   361,  -338,   384,   274,   678,   287,   304,   348,   309,
   216,   306,   370,   219,   324,   310,   388,   220,  -330,   404,
   387,  -338,   373,   408,   413,   407,   230,   420,   481,   415,
   697,   698,   749,   699,   431,   403,   411,   432,   423,   490,
   247,   449,   430,   435,   455,   747,   248,   471,   468,   458,
   462,   469,   509,   510,   470,   338,   496,   516,   511,   514,
   521,   527,   518,   539,   540,   554,   551,   284,   284,   284,
   534,   541,   290,   568,   552,   292,   560,   557,   567,   569,
   570,   340,   572,   745,   571,   577,   591,  -337,   588,   607,
   640,   609,   606,   634,   380,   636,   308,   638,   533,   655,
   637,   536,   537,   659,   650,   646,   721,   663,   729,   762,
   665,   667,   673,   325,   337,   326,   679,   680,   691,   330,
   682,   553,   701,   696,   702,   763,   710,     8,   709,   734,
   131,   711,   712,   353,   714,   718,   363,   364,   368,   719,
   412,   732,   758,   369,   760,   727,   685,    53,   372,   205,
   160,    26,   146,   192,   121,   584,   602,   381,   312,   175,
   429,   488,   313,   434,   587,   406,   392,   491,   289,   395,
   398,   402,   465,    83,   520,   368,   467,    93,    76,   464,
   610,   489,     0,     0,     0,     0,     0,   419,     0,     0,
     0,   684,     0,   425,     0,     0,     0,     0,   428,   392,
     0,   484,     0,     0,   487,     0,     0,     0,   433,   402,
     0,     0,   436,     0,     0,     0,   437,   441,   442,    53,
     0,     0,     0,   444,     0,     0,     0,     0,     0,     0,
     0,     0,   453,     0,     0,     0,     0,     0,     0,   459,
   460,   461,     0,     0,   463,     0,     0,     0,   441,     0,
     0,     0,     0,     0,   524,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   485,     0,     0,     0,     0,     0,
   751,     0,     0,   492,     0,     0,    27,   493,     0,    30,
     0,   494,     0,     0,     0,     0,     0,     0,   497,   717,
     0,   500,   504,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    55,     0,     0,   513,     0,     0,     0,     0,
     0,   504,     0,     0,     0,   523,     0,     0,     0,     0,
   736,     0,     0,     0,    80,   586,     0,   531,     0,     0,
   532,   756,   542,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   544,     0,    96,     0,     0,     0,   550,
     0,     0,     0,     0,   623,     0,     0,   107,   559,     0,
   561,     0,     0,   563,     0,     0,   566,   122,     0,     0,
     0,     0,     0,     0,   132,     0,     0,     0,   133,   580,
   581,     0,   590,     0,   147,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   161,     0,   598,   599,
   608,     0,     0,   163,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   176,     0,   622,     0,   624,   625,     0,
     0,     0,   686,     0,     0,     0,     0,     0,   193,   641,
   642,   643,   644,   195,   196,     0,   639,     0,     0,     0,
   700,     0,     0,     0,     0,   206,     0,   649,     0,     0,
   651,   212,   653,     0,     0,   658,     0,     0,     0,   664,
     0,   666,     0,   668,   670,   223,     0,   226,   227,   228,
     0,     0,     0,     0,     0,     0,     0,     0,   681,   241,
     0,     0,     0,   245,     0,     0,     0,   688,   689,   690,
     0,   692,     0,   250,   694,     0,   268,   269,   270,   753,
   272,     0,     0,     0,     0,   703,   704,   705,   706,     0,
     0,   708,     0,     0,     0,     0,     0,     0,     0,   715,
   716,     7,     0,     0,     0,   299,     0,   301,     0,     0,
    12,     0,     0,     0,     0,   730,     0,    23,     0,     0,
   733,     0,     0,    29,     0,     0,     0,     0,     0,   316,
     0,     0,     0,     0,   322,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    54,   759,     0,     0,
     0,     0,     0,   346,     0,     0,     0,   352,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    79,     0,
     0,     0,     0,     0,     0,     0,     0,   376,     0,     0,
     0,     0,     0,     0,   383,     0,     0,     0,     0,    95,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   422,   125,     0,     0,     0,   426,
     0,     0,     0,     0,     0,   135,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   152,   153,   154,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   162,     0,     0,
     0,   447,     0,     0,   169,   451,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   186,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   203,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   221,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   505,     0,
     0,   508,     0,   240,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   530,     0,     0,     0,     0,     0,     0,   535,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   565,     0,   314,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   579,     0,     0,     0,     0,     0,     0,     0,
   585,     0,     0,     0,     0,     0,     0,   345,     0,     0,
   596,   597,     0,     0,     0,     0,     0,     0,     0,   603,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   374,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   633,     0,     0,     0,     0,     0,     0,   635,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   662,     0,     0,     0,     0,     0,     0,     0,
     0,   672,     0,     0,     0,   677,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   687,     0,     0,     0,   445,     0,     0,     0,     0,   450,
   695,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   707,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   728,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   735,     0,
   737,     0,     0,   740,     0,     0,   743,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   754,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   529,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   582,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   600,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   629,   630,   631,   632,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   674,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   725
};

static const short yycheck[] = {     0,
    43,   226,   227,   228,   515,     6,   296,   345,   298,   480,
    79,    45,   538,    14,   344,   249,   485,   365,   636,    29,
   245,   369,   660,    86,   337,     7,    95,     9,    10,    27,
    31,    11,     9,    10,     5,    36,   105,     5,     6,     5,
     6,     7,     8,     9,    10,     9,    10,   272,    49,    38,
    39,    40,    41,    42,     5,     6,    57,    58,     9,    24,
    25,    26,    63,   642,    65,   644,    63,    64,    65,    66,
    71,    69,    73,    60,    61,    62,   301,     5,   254,   255,
   256,   257,     6,    84,    85,    68,    87,     5,     6,    12,
   438,     5,   382,    75,   259,   260,   386,    98,    75,   412,
   163,    14,     4,   741,    76,    71,    16,   108,   109,    75,
    71,     6,   581,    70,    17,     9,     8,     8,    74,     8,
   738,    49,    50,    51,    52,    53,    54,    55,    56,    57,
    58,    59,    70,    74,    18,    72,    70,    70,    19,    70,
    13,    20,    68,    70,    21,     6,    15,     5,    30,   150,
     6,    29,     6,   501,     6,     8,   446,    68,     6,    31,
    68,     6,    69,    69,   165,   495,   456,    34,    68,    70,
    68,    68,     6,    22,    70,    68,     4,   178,     5,    36,
    70,   182,   183,   184,   418,    68,    71,    22,    68,     6,
    32,     7,     3,    44,    70,   664,    71,    71,    45,    70,
   201,    71,     9,   204,    69,    71,     9,   208,    76,    72,
    70,    73,    76,     6,     8,    74,   217,    35,   452,    33,
   689,   690,   748,   692,     3,    71,    70,     3,    71,     7,
   231,    70,    73,    73,    71,   746,   237,     8,    74,    72,
    72,    72,    72,     6,    70,   314,    69,    28,    70,    70,
    74,    70,    72,     6,    69,    46,    69,   258,   259,   260,
    72,    70,   263,    68,    70,   266,    70,    69,   558,    70,
    69,   314,    47,   744,    71,    70,    48,    37,    70,    68,
    37,    69,    72,    70,   347,    68,   287,    69,   522,    43,
    70,   525,   526,    23,    71,    69,    27,    70,     6,     0,
    70,    70,    70,   304,   314,   306,    71,    71,    70,   310,
    71,   545,    70,    72,    69,     0,    72,     4,    70,    68,
   118,    72,    72,   324,    70,    70,   327,   328,   329,    70,
   373,    70,    69,   334,    70,   722,   674,   371,   339,   189,
   140,    20,   128,   172,   111,   574,   593,   348,   292,   157,
   390,   459,   292,   400,   579,   366,   357,   463,   262,   360,
   361,   362,   437,    73,   502,   366,   439,    88,    65,   436,
   604,   460,    -1,    -1,    -1,    -1,    -1,   378,    -1,    -1,
    -1,   671,    -1,   384,    -1,    -1,    -1,    -1,   389,   390,
    -1,   454,    -1,    -1,   457,    -1,    -1,    -1,   399,   400,
    -1,    -1,   403,    -1,    -1,    -1,   407,   408,   409,   443,
    -1,    -1,    -1,   414,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   423,    -1,    -1,    -1,    -1,    -1,    -1,   430,
   431,   432,    -1,    -1,   435,    -1,    -1,    -1,   439,    -1,
    -1,    -1,    -1,    -1,   507,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   455,    -1,    -1,    -1,    -1,    -1,
   750,    -1,    -1,   464,    -1,    -1,    22,   468,    -1,    25,
    -1,   472,    -1,    -1,    -1,    -1,    -1,    -1,   479,   713,
    -1,   482,   483,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    48,    -1,    -1,   496,    -1,    -1,    -1,    -1,
    -1,   502,    -1,    -1,    -1,   506,    -1,    -1,    -1,    -1,
   735,    -1,    -1,    -1,    70,   578,    -1,   518,    -1,    -1,
   521,   755,   532,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   534,    -1,    91,    -1,    -1,    -1,   540,
    -1,    -1,    -1,    -1,   607,    -1,    -1,   103,   549,    -1,
   551,    -1,    -1,   554,    -1,    -1,   557,   113,    -1,    -1,
    -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,   124,   570,
   571,    -1,   582,    -1,   130,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,   589,   590,
   600,    -1,    -1,   149,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   159,    -1,   606,    -1,   608,   609,    -1,
    -1,    -1,   675,    -1,    -1,    -1,    -1,    -1,   174,   629,
   630,   631,   632,   179,   180,    -1,   627,    -1,    -1,    -1,
   693,    -1,    -1,    -1,    -1,   191,    -1,   638,    -1,    -1,
   641,   197,   643,    -1,    -1,   646,    -1,    -1,    -1,   650,
    -1,   652,    -1,   654,   655,   211,    -1,   213,   214,   215,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   669,   225,
    -1,    -1,    -1,   229,    -1,    -1,    -1,   678,   679,   680,
    -1,   682,    -1,   239,   685,    -1,   242,   243,   244,   752,
   246,    -1,    -1,    -1,    -1,   696,   697,   698,   699,    -1,
    -1,   702,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   710,
   711,     2,    -1,    -1,    -1,   271,    -1,   273,    -1,    -1,
    11,    -1,    -1,    -1,    -1,   726,    -1,    18,    -1,    -1,
   731,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,   295,
    -1,    -1,    -1,    -1,   300,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    47,   758,    -1,    -1,
    -1,    -1,    -1,   319,    -1,    -1,    -1,   323,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   343,    -1,    -1,
    -1,    -1,    -1,    -1,   350,    -1,    -1,    -1,    -1,    90,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   380,   116,    -1,    -1,    -1,   385,
    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   136,   137,   138,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148,    -1,    -1,
    -1,   417,    -1,    -1,   155,   421,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   170,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   187,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   209,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   484,    -1,
    -1,   487,    -1,   224,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   517,    -1,    -1,    -1,    -1,    -1,    -1,   524,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   556,    -1,   293,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   568,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   576,    -1,    -1,    -1,    -1,    -1,    -1,   318,    -1,    -1,
   586,   587,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   595,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   341,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   616,    -1,    -1,    -1,    -1,    -1,    -1,   623,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   648,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   657,    -1,    -1,    -1,   661,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   676,    -1,    -1,    -1,   415,    -1,    -1,    -1,    -1,   420,
   686,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   700,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   724,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   734,    -1,
   736,    -1,    -1,   739,    -1,    -1,   742,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   753,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   516,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   572,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   591,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   612,   613,   614,   615,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   659,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   721
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/misc/bison.simple"

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
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

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifndef YYPARSE_PARAM
int yyparse ();
#endif
#endif

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
     int count;
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
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/share/misc/bison.simple"

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
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
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
#line 516 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 526 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 530 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 536 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 540 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * modules or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 6:
#line 555 "parser-sming.y"
{
			    /*
			     * If we got a (Module *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].modulePtr != NULL) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 7:
#line 569 "parser-sming.y"
{
			    thisParserPtr->modulePtr = findModuleByName(yyvsp[0].text);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule(yyvsp[0].text,
					      util_strdup(thisParserPtr->path),
					      thisParserPtr->character,
					      0,
					      thisParserPtr);
			    } else {
				free(yyvsp[0].text);
			    }
			    thisModulePtr->export.language = SMI_LANGUAGE_SMING;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			    thisParserPtr->firstIndexlabelPtr = NULL;
			    thisParserPtr->identityObjectName = NULL;
			;
    break;}
case 8:
#line 590 "parser-sming.y"
{
			    thisParserPtr->identityObjectName = yyvsp[0].text;
			;
    break;}
case 9:
#line 596 "parser-sming.y"
{
			    /*
			     * Note: Due to parsing imported modules
			     * the contents of moduleObjectPtr got
			     * invalid. Therefore, we created the
			     * module identity object above as
			     * incomplete, and now we add it's
			     * registration information.
			     */

			    if (yyvsp[-1].nodePtr) {
				moduleObjectPtr = addObject(
				             thisParserPtr->identityObjectName,
							    yyvsp[-1].nodePtr->parentPtr,
							    yyvsp[-1].nodePtr->subid,
							    0, thisParserPtr);
				setObjectDecl(moduleObjectPtr,
					      SMI_DECL_MODULE);
				setObjectNodekind(moduleObjectPtr,
						  SMI_NODEKIND_MODULE);
				addObjectFlags(moduleObjectPtr,
					       FLAG_REGISTERED);
				setModuleIdentityObject(
				    thisParserPtr->modulePtr, moduleObjectPtr);
			    }
			;
    break;}
case 10:
#line 623 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 11:
#line 630 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 12:
#line 637 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleDescription(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
				if (moduleObjectPtr) {
				    setObjectDescription(moduleObjectPtr, yyvsp[-1].text);
				}
			    }
			;
    break;}
case 13:
#line 647 "parser-sming.y"
{
			    if (yyvsp[0].text) {
				setModuleReference(thisParserPtr->modulePtr,
						   yyvsp[0].text);
				if (moduleObjectPtr) {
				    setObjectReference(moduleObjectPtr, yyvsp[0].text);
				}
			    }
			;
    break;}
case 14:
#line 664 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    /*
			     * Walk through the index structs of all table
			     * rows of this module and convert their
			     * labelstrings to (Object *). This is the
			     * case for index column lists
			     * (indexPtr->listPtr[]->ptr), index related
			     * rows (indexPtr->rowPtr) and create lists
			     * (listPtr[]->ptr).
			     */
			    while (thisParserPtr->firstIndexlabelPtr) {
				/* adjust indexPtr->listPtr elements */
				for (listPtr =
       ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr,
							   thisModulePtr);
				    listPtr->ptr = objectPtr;
				}
				/* adjust relatedPtr */
		     if (((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->
				    relatedPtr) {
				    objectPtr = findObject(
  		        ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->
					relatedPtr,
					thisParserPtr,
			                thisModulePtr);
			 ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->
				    relatedPtr = objectPtr;
				}
				/* adjust create list */
				for (listPtr =
		 ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr,
							   thisModulePtr);
				    listPtr->ptr = objectPtr;
				}
			  listPtr = thisParserPtr->firstIndexlabelPtr->nextPtr;
				free(thisParserPtr->firstIndexlabelPtr);
				thisParserPtr->firstIndexlabelPtr = listPtr;
			    }
			    yyval.modulePtr = thisModulePtr;
			    moduleObjectPtr = NULL;
			;
    break;}
case 15:
#line 716 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 16:
#line 720 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed extension statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 17:
#line 730 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 18:
#line 735 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * extensions or return -1, if at least one
			     * failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 19:
#line 750 "parser-sming.y"
{
			    /*
			     * If we got a (Type *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].macroPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 20:
#line 764 "parser-sming.y"
{
			    macroIdentifier = yyvsp[0].text;
			    macroPtr = addMacro(macroIdentifier,
						thisParserPtr->character,
						0,
						thisParserPtr);
			    setMacroDecl(macroPtr, SMI_DECL_EXTENSION);
			;
    break;}
case 21:
#line 774 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].status) {
				setMacroStatus(macroPtr, yyvsp[0].status);
			    }
			;
    break;}
case 22:
#line 780 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroDescription(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 23:
#line 786 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroReference(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 24:
#line 792 "parser-sming.y"
{
			    yyval.macroPtr = 0;
			    macroPtr = NULL;
			    free(macroIdentifier);
			;
    break;}
case 25:
#line 799 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 26:
#line 803 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 27:
#line 813 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 28:
#line 818 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * typedefs or return -1, if at least one
			     * failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 29:
#line 833 "parser-sming.y"
{
			    /*
			     * If we got a (Type *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].typePtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 30:
#line 847 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 31:
#line 852 "parser-sming.y"
{
			    if (yyvsp[-1].typePtr) {
				if (yyvsp[-1].typePtr->export.name) {
				    /*
				     * If the exact type has been found
				     * and no new Type structure has been
				     * created, we have to create a
				     * duplicate now.
				     */
				    typePtr = duplicateType(yyvsp[-1].typePtr, 0,
							    thisParserPtr);
				} else {
				    typePtr = yyvsp[-1].typePtr;
				}
				setTypeName(typePtr, typeIdentifier);
				setTypeDecl(typePtr, SMI_DECL_TYPEDEF);
			    }
			;
    break;}
case 32:
#line 871 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 33:
#line 877 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
                                if (!checkFormat(typePtr->export.basetype, yyvsp[0].text)) {
				    printError(thisParserPtr,
					       ERR_INVALID_FORMAT, yyvsp[0].text);
				}
				setTypeFormat(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 34:
#line 887 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 35:
#line 893 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 36:
#line 899 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 37:
#line 905 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 38:
#line 911 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 39:
#line 919 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 40:
#line 923 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 41:
#line 933 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 42:
#line 938 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * statements or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 43:
#line 953 "parser-sming.y"
{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].objectPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 47:
#line 972 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 48:
#line 977 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				nodeObjectPtr = addObject(nodeIdentifier,
							  yyvsp[-1].nodePtr->parentPtr,
							  yyvsp[-1].nodePtr->subid,
							  0, thisParserPtr);
				setObjectDecl(nodeObjectPtr, SMI_DECL_NODE);
				setObjectNodekind(nodeObjectPtr,
						  SMI_NODEKIND_NODE);
				setObjectAccess(nodeObjectPtr,
						SMI_ACCESS_NOT_ACCESSIBLE);
			    }
			;
    break;}
case 49:
#line 991 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 50:
#line 997 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectDescription(nodeObjectPtr, yyvsp[0].text);
				/*
				 * If the node has a description, it gets
				 * registered. This is used to distinguish
				 * between SMIv2 OBJECT-IDENTITY macros and
				 * non-registering ASN.1 value assignments.
				 */
				addObjectFlags(nodeObjectPtr, FLAG_REGISTERED);
			    }
			;
    break;}
case 51:
#line 1010 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 52:
#line 1016 "parser-sming.y"
{
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 53:
#line 1024 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 54:
#line 1029 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				scalarObjectPtr = addObject(scalarIdentifier,
							    yyvsp[-1].nodePtr->parentPtr,
							    yyvsp[-1].nodePtr->subid,
							    0, thisParserPtr);
				setObjectDecl(scalarObjectPtr,
					      SMI_DECL_SCALAR);
				setObjectNodekind(scalarObjectPtr,
						  SMI_NODEKIND_SCALAR);
			    }
			;
    break;}
case 55:
#line 1042 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].typePtr) {
				setObjectType(scalarObjectPtr, yyvsp[-1].typePtr);
				defaultBasetype = yyvsp[-1].typePtr->export.basetype;
				if (!(yyvsp[-1].typePtr->export.name)) {
				    /*
				     * An inlined type.
				     */
#if 0 /* export implicitly defined types by the node's lowercase name */
				    setTypeName(yyvsp[-1].typePtr, scalarIdentifier);
#endif
				}
			    }
			;
    break;}
case 56:
#line 1057 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 57:
#line 1063 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 58:
#line 1069 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				if (!checkFormat(yyvsp[-8].typePtr->export.basetype, yyvsp[0].text)) {
				    printError(thisParserPtr,
					       ERR_INVALID_FORMAT, yyvsp[0].text);
				}
				setObjectFormat(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 1079 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 60:
#line 1085 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 61:
#line 1091 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 62:
#line 1097 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 63:
#line 1103 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 64:
#line 1112 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 65:
#line 1117 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				tableObjectPtr = addObject(tableIdentifier,
							   yyvsp[-1].nodePtr->parentPtr,
							   yyvsp[-1].nodePtr->subid,
							   0, thisParserPtr);
				setObjectDecl(tableObjectPtr,
					      SMI_DECL_TABLE);
				setObjectNodekind(tableObjectPtr,
						  SMI_NODEKIND_TABLE);
				setObjectAccess(tableObjectPtr,
						SMI_ACCESS_NOT_ACCESSIBLE);
			    }
			;
    break;}
case 66:
#line 1132 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 67:
#line 1138 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 68:
#line 1144 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 69:
#line 1151 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 70:
#line 1159 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 71:
#line 1164 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				rowObjectPtr = addObject(rowIdentifier,
							 yyvsp[-1].nodePtr->parentPtr,
							 yyvsp[-1].nodePtr->subid,
							 0, thisParserPtr);
				setObjectDecl(rowObjectPtr,
					      SMI_DECL_ROW);
				setObjectNodekind(rowObjectPtr,
						  SMI_NODEKIND_ROW);
				setObjectAccess(rowObjectPtr,
						SMI_ACCESS_NOT_ACCESSIBLE);
			    }
			;
    break;}
case 72:
#line 1179 "parser-sming.y"
{
			    List *listPtr;
			    
			    if (rowObjectPtr &&
				(yyvsp[-1].index.indexkind != SMI_INDEX_UNKNOWN)) {
				setObjectIndexkind(rowObjectPtr,
						   yyvsp[-1].index.indexkind);
				setObjectImplied(rowObjectPtr, yyvsp[-1].index.implied);
				setObjectRelated(rowObjectPtr, yyvsp[-1].index.rowPtr);
				setObjectList(rowObjectPtr, yyvsp[-1].index.listPtr);

				/*
				 * Add this row object to the list of rows
				 * that have to be converted when the whole
				 * module has been parsed. See the end of
				 * the moduleStatement rule above.
				 */
				listPtr = util_malloc(sizeof(List));
				listPtr->ptr = rowObjectPtr;
			  listPtr->nextPtr = thisParserPtr->firstIndexlabelPtr;
				thisParserPtr->firstIndexlabelPtr = listPtr;
			    }
			;
    break;}
case 73:
#line 1203 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectList(rowObjectPtr, yyvsp[0].listPtr);
				if (yyvsp[0].listPtr) {
				    addObjectFlags(rowObjectPtr,
						   FLAG_CREATABLE);
				    setObjectCreate(rowObjectPtr, 1);
				}
			    }
			;
    break;}
case 74:
#line 1214 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 75:
#line 1220 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 76:
#line 1226 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 77:
#line 1233 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 78:
#line 1241 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 79:
#line 1249 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * columns or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 80:
#line 1264 "parser-sming.y"
{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].objectPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 81:
#line 1278 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 82:
#line 1283 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				columnObjectPtr = addObject(columnIdentifier,
							    yyvsp[-1].nodePtr->parentPtr,
							    yyvsp[-1].nodePtr->subid,
							    0, thisParserPtr);
				setObjectDecl(columnObjectPtr,
					      SMI_DECL_COLUMN);
				setObjectNodekind(columnObjectPtr,
						  SMI_NODEKIND_COLUMN);
			    }
			;
    break;}
case 83:
#line 1296 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].typePtr) {
				setObjectType(columnObjectPtr, yyvsp[-1].typePtr);
				defaultBasetype = yyvsp[-1].typePtr->export.basetype;
				if (!(yyvsp[-1].typePtr->export.name)) {
				    /*
				     * An inlined type.
				     */
#if 0 /* export implicitly defined types by the node's lowercase name */
				    setTypeName(yyvsp[-1].typePtr, columnIdentifier);
#endif
				}
			    }
			;
    break;}
case 84:
#line 1311 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 85:
#line 1317 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 86:
#line 1323 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
                                if (!checkFormat(yyvsp[-8].typePtr->export.basetype, yyvsp[0].text)) {
				    printError(thisParserPtr,
					       ERR_INVALID_FORMAT, yyvsp[0].text);
				}
				setObjectFormat(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 87:
#line 1333 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 88:
#line 1339 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 89:
#line 1345 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 90:
#line 1351 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 91:
#line 1357 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 92:
#line 1366 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 93:
#line 1370 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 94:
#line 1380 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 95:
#line 1385 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * notifications or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 96:
#line 1400 "parser-sming.y"
{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].objectPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 97:
#line 1414 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 98:
#line 1419 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				notificationObjectPtr =
				    addObject(notificationIdentifier,
					      yyvsp[-1].nodePtr->parentPtr,
					      yyvsp[-1].nodePtr->subid,
					      0, thisParserPtr);
				setObjectDecl(notificationObjectPtr,
					      SMI_DECL_NOTIFICATION);
				setObjectNodekind(notificationObjectPtr,
						  SMI_NODEKIND_NOTIFICATION);
			    }
			;
    break;}
case 99:
#line 1433 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (notificationObjectPtr && yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr,
							   thisModulePtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(notificationObjectPtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 100:
#line 1449 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 101:
#line 1455 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 102:
#line 1462 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 103:
#line 1468 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 104:
#line 1476 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 105:
#line 1480 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 106:
#line 1490 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 107:
#line 1494 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * groups or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 108:
#line 1509 "parser-sming.y"
{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].objectPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 109:
#line 1523 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 110:
#line 1528 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				groupObjectPtr = addObject(groupIdentifier,
							   yyvsp[-1].nodePtr->parentPtr,
							   yyvsp[-1].nodePtr->subid,
							   0, thisParserPtr);
				setObjectDecl(groupObjectPtr, SMI_DECL_GROUP);
				setObjectNodekind(groupObjectPtr,
						  SMI_NODEKIND_GROUP);
			    }
			;
    break;}
case 111:
#line 1540 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (groupObjectPtr && yyvsp[-1].listPtr) {
				for (listPtr = yyvsp[-1].listPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr,
							   thisModulePtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(groupObjectPtr, yyvsp[-1].listPtr);
			    }
			;
    break;}
case 112:
#line 1556 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 113:
#line 1562 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 114:
#line 1568 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 115:
#line 1574 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 116:
#line 1582 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 117:
#line 1586 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 118:
#line 1596 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 119:
#line 1601 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * compliances or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 120:
#line 1616 "parser-sming.y"
{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].objectPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 121:
#line 1630 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 122:
#line 1635 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				complianceObjectPtr =
				    addObject(complianceIdentifier,
					      yyvsp[-1].nodePtr->parentPtr,
					      yyvsp[-1].nodePtr->subid,
					      0, thisParserPtr);
				setObjectDecl(complianceObjectPtr,
					      SMI_DECL_COMPLIANCE);
				setObjectNodekind(complianceObjectPtr,
						  SMI_NODEKIND_COMPLIANCE);
			    }
			;
    break;}
case 123:
#line 1649 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 124:
#line 1655 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 125:
#line 1661 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 126:
#line 1667 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (complianceObjectPtr && yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr,
							   thisModulePtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(complianceObjectPtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 127:
#line 1683 "parser-sming.y"
{
			    Option *optionPtr;
			    List *listPtr;
			    
			    complianceObjectPtr->optionlistPtr = yyvsp[0].listPtr;
			    if (yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr;
				     listPtr;
				     listPtr = listPtr->nextPtr) {
				    optionPtr = ((Option *)(listPtr->ptr));
				    optionPtr->compliancePtr =
					complianceObjectPtr;
				}
			    }
			;
    break;}
case 128:
#line 1699 "parser-sming.y"
{
			    Refinement *refinementPtr;
			    List *listPtr;
			    char *s;
			    
			    complianceObjectPtr->refinementlistPtr = yyvsp[0].listPtr;
			    if (yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr;
				     listPtr;
				     listPtr = listPtr->nextPtr) {
				    refinementPtr =
					((Refinement *)(listPtr->ptr));
				    refinementPtr->compliancePtr =
					complianceObjectPtr;
#if 0 /* export implicitly defined types by the node's lowercase name */
				    s = util_malloc(strlen(refinementPtr->
					    objectPtr->export.name) +
					    strlen(complianceIdentifier) + 13);
				    if (refinementPtr->typePtr) {
					sprintf(s, "%s+%s+type",
						complianceIdentifier,
						refinementPtr->objectPtr->
						                  export.name);
					setTypeName(refinementPtr->typePtr, s);
					
				    }
				    if (refinementPtr->writetypePtr) {
					sprintf(s, "%s+%s+writetype",
						complianceIdentifier,
						refinementPtr->objectPtr->
						                  export.name);
					setTypeName(refinementPtr->
						              writetypePtr, s);
				    }
				    util_free(s);
#endif
				}
			    }
			;
    break;}
case 129:
#line 1739 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 130:
#line 1747 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 131:
#line 1751 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 132:
#line 1757 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 133:
#line 1761 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * imports or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 134:
#line 1776 "parser-sming.y"
{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if (yyvsp[-1].listPtr) {
				yyval.rc = 1;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 135:
#line 1790 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 136:
#line 1795 "parser-sming.y"
{
			    List *listPtr, *nextPtr;
			    
			    for (listPtr = yyvsp[0].listPtr; listPtr; listPtr = nextPtr) {
				addImport(listPtr->ptr, thisParserPtr);
				thisParserPtr->modulePtr->
				                      numImportedIdentifiers++;
				nextPtr = listPtr->nextPtr;
				free(listPtr);
			    }
			;
    break;}
case 137:
#line 1807 "parser-sming.y"
{
			    Module *modulePtr;
			    char *s = importModulename;

			    modulePtr = findModuleByName(s);
			    if (!modulePtr) {
				modulePtr = loadModule(s);
			    }
			    checkImports(modulePtr, thisParserPtr);
			    free(s);
			    yyval.listPtr = NULL;
			;
    break;}
case 138:
#line 1822 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 139:
#line 1826 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 140:
#line 1832 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 141:
#line 1836 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * revisions or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 142:
#line 1851 "parser-sming.y"
{
			       /*
				* If we got a (Revision *) return rc == 1,
				* otherwise parsing failed (rc == -1).
				*/
			       if (yyvsp[-1].revisionPtr) {
				yyval.rc = 1;
			       } else {
				   yyval.rc = -1;
			       }
			   ;
    break;}
case 143:
#line 1868 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(yyvsp[-6].date, yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 144:
#line 1876 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 145:
#line 1882 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 146:
#line 1886 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 147:
#line 1893 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 148:
#line 1899 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 149:
#line 1903 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 150:
#line 1910 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 156:
#line 1924 "parser-sming.y"
{
			    if (yyvsp[-8].rc) {
				yyval.index.implied = 1;
			    } else {
				yyval.index.implied = 0;
			    }
			    yyval.index.indexkind = SMI_INDEX_INDEX;
			    yyval.index.listPtr = yyvsp[-4].listPtr;
			    yyval.index.rowPtr = NULL;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			;
    break;}
case 157:
#line 1944 "parser-sming.y"
{
			    yyval.index.implied = 0;
			    yyval.index.indexkind = SMI_INDEX_AUGMENT;
			    yyval.index.listPtr = NULL;
			    yyval.index.rowPtr = (void *)yyvsp[-2].text;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			;
    break;}
case 158:
#line 1962 "parser-sming.y"
{
			    if (yyvsp[-8].rc) {
				yyval.index.implied = 1;
			    } else {
				yyval.index.implied = 0;
			    }
			    yyval.index.indexkind = SMI_INDEX_REORDER;
			    yyval.index.listPtr = yyvsp[-4].listPtr;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			    yyval.index.rowPtr = (void *)yyvsp[-9].text;
			;
    break;}
case 159:
#line 1982 "parser-sming.y"
{
			    yyval.index.implied = 0;
			    yyval.index.indexkind = SMI_INDEX_SPARSE;
			    yyval.index.listPtr = NULL;
			    yyval.index.rowPtr = (void *)yyvsp[-2].text;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			;
    break;}
case 160:
#line 2000 "parser-sming.y"
{
			    if (yyvsp[-8].rc) {
				yyval.index.implied = 1;
			    } else {
				yyval.index.implied = 0;
			    }
			    yyval.index.indexkind = SMI_INDEX_EXPAND;
			    yyval.index.listPtr = yyvsp[-4].listPtr;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			    yyval.index.rowPtr = (void *)yyvsp[-9].text;
			;
    break;}
case 161:
#line 2020 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 162:
#line 2024 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 163:
#line 2030 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 164:
#line 2034 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 165:
#line 2040 "parser-sming.y"
{
			    if (rowObjectPtr) {
				addObjectFlags(rowObjectPtr, FLAG_CREATABLE);
				setObjectCreate(rowObjectPtr, 1);
			    }
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 166:
#line 2050 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 167:
#line 2054 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 168:
#line 2060 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 169:
#line 2066 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 170:
#line 2072 "parser-sming.y"
{
			    yyval.date = yyvsp[-2].date;
			;
    break;}
case 171:
#line 2078 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 172:
#line 2084 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 173:
#line 2090 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 174:
#line 2094 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 175:
#line 2100 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 176:
#line 2106 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 177:
#line 2110 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 178:
#line 2116 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 179:
#line 2122 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 180:
#line 2126 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 181:
#line 2132 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 182:
#line 2138 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 183:
#line 2142 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 184:
#line 2148 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 185:
#line 2154 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 186:
#line 2158 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 187:
#line 2164 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 188:
#line 2170 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 189:
#line 2174 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 190:
#line 2180 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 191:
#line 2186 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 192:
#line 2190 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 193:
#line 2196 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 194:
#line 2203 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 195:
#line 2209 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 196:
#line 2213 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 197:
#line 2220 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 198:
#line 2226 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 199:
#line 2230 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 200:
#line 2237 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 201:
#line 2243 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 202:
#line 2247 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 203:
#line 2253 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 204:
#line 2260 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].optionPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 205:
#line 2273 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 206:
#line 2282 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text,
						       thisParserPtr,
						       thisModulePtr);
			    yyval.optionPtr->export.description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 207:
#line 2292 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 208:
#line 2296 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 209:
#line 2302 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 210:
#line 2308 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].refinementPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 211:
#line 2321 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 212:
#line 2332 "parser-sming.y"
{
			    yyval.refinementPtr = util_malloc(sizeof(Refinement));
			    yyval.refinementPtr->objectPtr = findObject(yyvsp[-10].text,
						       thisParserPtr,
						       thisModulePtr);
			    if (yyvsp[-7].typePtr) {
				yyval.refinementPtr->typePtr = duplicateType(yyvsp[-7].typePtr, 0,
							    thisParserPtr);
				yyval.refinementPtr->typePtr->listPtr = yyvsp[-7].typePtr->listPtr;
			    } else {
				yyval.refinementPtr->typePtr = NULL;
			    }
			    if (yyvsp[-6].typePtr) {
				yyval.refinementPtr->writetypePtr =
				    duplicateType(yyvsp[-6].typePtr, 0, thisParserPtr);
				yyval.refinementPtr->writetypePtr->listPtr = yyvsp[-6].typePtr->listPtr;
			    } else {
				yyval.refinementPtr->writetypePtr = NULL;
			    }
			    yyval.refinementPtr->export.access = yyvsp[-5].access;
			    yyval.refinementPtr->export.description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 213:
#line 2357 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 214:
#line 2361 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 215:
#line 2367 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeOctetStringPtr;
			    } else {
				yyval.typePtr = duplicateType(typeOctetStringPtr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeOctetStringPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 216:
#line 2382 "parser-sming.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 217:
#line 2386 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeInteger32Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeInteger32Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeInteger32Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 218:
#line 2401 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeUnsigned32Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeUnsigned32Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeUnsigned32Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 219:
#line 2416 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeInteger64Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeInteger64Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeInteger64Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 220:
#line 2431 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeUnsigned64Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeUnsigned64Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeUnsigned64Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 221:
#line 2446 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeFloat32Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeFloat32Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeFloat32Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 222:
#line 2461 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeFloat64Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeFloat64Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeFloat64Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 223:
#line 2476 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeFloat128Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeFloat128Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeFloat128Ptr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((Range *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 224:
#line 2491 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeEnumPtr;
			    } else {
				yyval.typePtr = duplicateType(typeEnumPtr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeEnumPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 225:
#line 2506 "parser-sming.y"
{
			    List *p;
			    
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeBitsPtr;
			    } else {
				yyval.typePtr = duplicateType(typeBitsPtr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr, typeBitsPtr);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
				for (p = yyvsp[0].listPtr; p; p = p->nextPtr)
				    ((NamedNumber *)p->ptr)->typePtr = yyval.typePtr;
			    }
			;
    break;}
case 226:
#line 2523 "parser-sming.y"
{
			    typePtr = findType(yyvsp[-1].text, thisParserPtr,
					       thisModulePtr);
			    if (typePtr && yyvsp[0].listPtr) {
				typePtr = duplicateType(typePtr, 0,
							thisParserPtr);
				setTypeList(typePtr, yyvsp[0].listPtr);
			    }

			    yyval.typePtr = typePtr;
			;
    break;}
case 227:
#line 2537 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 228:
#line 2541 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 229:
#line 2547 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 230:
#line 2551 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 231:
#line 2557 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 232:
#line 2561 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 233:
#line 2568 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 234:
#line 2576 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 235:
#line 2580 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 236:
#line 2586 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 237:
#line 2592 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 238:
#line 2605 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 239:
#line 2611 "parser-sming.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    yyval.rangePtr->export.minValue = *yyvsp[-1].valuePtr;
			    if (yyvsp[0].valuePtr) {
				yyval.rangePtr->export.maxValue = *yyvsp[0].valuePtr;
				util_free(yyvsp[0].valuePtr);
			    } else {
				yyval.rangePtr->export.maxValue = *yyvsp[-1].valuePtr;
			    }
			    util_free(yyvsp[-1].valuePtr);
			;
    break;}
case 240:
#line 2625 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 241:
#line 2629 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 242:
#line 2635 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 243:
#line 2641 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 244:
#line 2645 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 245:
#line 2652 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 246:
#line 2660 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 247:
#line 2664 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 248:
#line 2670 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 249:
#line 2676 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 250:
#line 2689 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 251:
#line 2695 "parser-sming.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    yyval.rangePtr->export.minValue.basetype = SMI_BASETYPE_FLOAT64;
			    yyval.rangePtr->export.minValue.format = SMI_VALUEFORMAT_NATIVE;
			    yyval.rangePtr->export.minValue.value.float64 = strtod(yyvsp[-1].text, NULL);
			    if (yyvsp[0].text) {
				yyval.rangePtr->export.maxValue.basetype =
				                          SMI_BASETYPE_FLOAT64;
				yyval.rangePtr->export.maxValue.format =
							SMI_VALUEFORMAT_NATIVE;
				yyval.rangePtr->export.maxValue.value.float64 =
				                              strtod(yyvsp[0].text, NULL);
			    } else {
				yyval.rangePtr->export.maxValue = yyval.rangePtr->export.minValue;
			    }
			;
    break;}
case 252:
#line 2714 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 253:
#line 2718 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 254:
#line 2724 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 255:
#line 2730 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 256:
#line 2737 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 257:
#line 2745 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 258:
#line 2749 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 259:
#line 2755 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 260:
#line 2762 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 261:
#line 2776 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 262:
#line 2782 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-6].text;
			    yyval.namedNumberPtr->export.value = *yyvsp[-2].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			;
    break;}
case 263:
#line 2791 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 264:
#line 2799 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 265:
#line 2803 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 266:
#line 2809 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 267:
#line 2815 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 268:
#line 2828 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 269:
#line 2834 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 270:
#line 2842 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 271:
#line 2846 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 272:
#line 2852 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 273:
#line 2858 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 274:
#line 2871 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 275:
#line 2877 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 276:
#line 2885 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 277:
#line 2889 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 278:
#line 2895 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 279:
#line 2901 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 280:
#line 2914 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 281:
#line 2920 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 282:
#line 2926 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 283:
#line 2930 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 284:
#line 2938 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 285:
#line 2942 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 286:
#line 2948 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 287:
#line 2954 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 288:
#line 2967 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 289:
#line 2973 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 290:
#line 2977 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 291:
#line 2983 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 292:
#line 2987 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 2993 "parser-sming.y"
{
			    char *s;

			    s = util_malloc(strlen(yyvsp[-2].text) +
					    strlen(yyvsp[0].text) + 3);
			    sprintf(s, "%s::%s", yyvsp[-2].text, yyvsp[0].text);
			    yyval.text = s;
			    free(yyvsp[-2].text);
			    free(yyvsp[0].text);
			;
    break;}
case 294:
#line 3004 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 295:
#line 3010 "parser-sming.y"
{
			    char *s;

			    s = util_malloc(strlen(yyvsp[-2].text) +
					    strlen(yyvsp[0].text) + 3);
			    sprintf(s, "%s::%s", yyvsp[-2].text, yyvsp[0].text);
			    yyval.text = s;
			    free(yyvsp[-2].text);
			    free(yyvsp[0].text);
			;
    break;}
case 296:
#line 3021 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 297:
#line 3027 "parser-sming.y"
{
			    if (yyvsp[0].text) {
				yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
				strcpy(yyval.text, yyvsp[-1].text);
				strcat(yyval.text, yyvsp[0].text);
				free(yyvsp[-1].text);
				free(yyvsp[0].text);
			    } else {
				yyval.text = util_strdup(yyvsp[-1].text);
			    }
			;
    break;}
case 298:
#line 3041 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 299:
#line 3045 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 300:
#line 3051 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 301:
#line 3055 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 302:
#line 3065 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 303:
#line 3071 "parser-sming.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 304:
#line 3077 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 305:
#line 3083 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 306:
#line 3096 "parser-sming.y"
{
			    int i;
			    List *listPtr, *nextPtr;
			    
			    if (defaultBasetype == SMI_BASETYPE_BITS) {
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
				yyval.valuePtr->format =
				    SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.bits = NULL;
				for (i = 0, listPtr = yyvsp[0].listPtr; listPtr;
				     i++, listPtr = nextPtr) {
				    nextPtr = listPtr->nextPtr;
				    yyval.valuePtr->value.bits =
					util_realloc(yyval.valuePtr->value.bits,
						     sizeof(char *) * (i+2));
				    yyval.valuePtr->value.bits[i] = listPtr->ptr;
				    yyval.valuePtr->value.bits[i+1] = NULL;
				    free(listPtr);
				}
			    } else {
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
			    }
			;
    break;}
case 307:
#line 3123 "parser-sming.y"
{
			    /* Note: might also be an OID or signed */
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_UNSIGNED32:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_UNSIGNED64:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER32:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.integer32 = strtol(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER64:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.integer64 = strtoll(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_OBJECTIDENTIFIER:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
				yyval.valuePtr->format = SMI_VALUEFORMAT_OID;
				yyval.valuePtr->len = 2;
				yyval.valuePtr->value.oid =
				    util_malloc(2 * sizeof(SmiSubid));
				yyval.valuePtr->value.oid[0] = 0;
				yyval.valuePtr->value.oid[1] = 0;
				/* TODO */
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
				break;
			    }
			;
    break;}
case 308:
#line 3169 "parser-sming.y"
{
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_INTEGER32:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.integer32 = - strtoul(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER64:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
				yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
				break;
			    }
			;
    break;}
case 309:
#line 3191 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 310:
#line 3197 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 311:
#line 3203 "parser-sming.y"
{
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				yyval.valuePtr->format = SMI_VALUEFORMAT_TEXT;
				yyval.valuePtr->value.ptr = yyvsp[0].text;
			    } else {
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
			    }
			;
    break;}
case 312:
#line 3216 "parser-sming.y"
{
			    /* Note: might be an Enumeration item or OID */
			    /* TODO: convert if it's an oid? */
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_ENUM:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_ENUM;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NAME;
				yyval.valuePtr->value.ptr = yyvsp[0].text;
				break;
			    case SMI_BASETYPE_OBJECTIDENTIFIER:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
				yyval.valuePtr->format = SMI_VALUEFORMAT_NAME;
				yyval.valuePtr->value.ptr = yyvsp[0].text;
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
				break;
			    }
			;
    break;}
case 313:
#line 3240 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 314:
#line 3245 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 315:
#line 3252 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 316:
#line 3256 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 317:
#line 3260 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 318:
#line 3266 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 319:
#line 3270 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 320:
#line 3274 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 321:
#line 3278 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 322:
#line 3284 "parser-sming.y"
{
			    char *oid = NULL;
			    Node *nodePtr;

			    if (yyvsp[-1].text && yyvsp[0].text) {
				oid = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
				strcpy(oid, yyvsp[-1].text);
				strcat(oid, yyvsp[0].text);
				free(yyvsp[-1].text);
				free(yyvsp[0].text);
			    } else if (yyvsp[-1].text) {
				oid = util_malloc(strlen(yyvsp[-1].text) + 1);
				strcpy(oid, yyvsp[-1].text);
				free(yyvsp[-1].text);
			    }
			    
			    if (oid) {
				nodePtr = findNodeByOidString(oid);
				if (!nodePtr) {
				    nodePtr = createNodesByOidString(oid);
				}
				yyval.nodePtr = nodePtr;
			    } else {
				yyval.nodePtr = NULL;
			    }
			;
    break;}
case 323:
#line 3313 "parser-sming.y"
{
			    Object *objectPtr;
			    Node *nodePtr;
			    char *s;
			    char ss[20];
			    
			    /* TODO: $1 might be numeric !? */
			    
			    objectPtr = findObject(yyvsp[0].text,
						   thisParserPtr,
						   thisModulePtr);

			    if (objectPtr) {
				/* create OID string */
				nodePtr = objectPtr->nodePtr;
				s = util_malloc(100);
				sprintf(s, "%u", nodePtr->subid);
				while ((nodePtr->parentPtr) &&
				       (nodePtr->parentPtr != rootNodePtr)) {
				    nodePtr = nodePtr->parentPtr;

				    sprintf(ss, "%u", nodePtr->subid);
				    if (strlen(s) > 80)
					s = util_realloc(s,
						     strlen(s)+strlen(ss)+2);
				    memmove(&s[strlen(ss)+1], s, strlen(s)+1);
				    strncpy(s, ss, strlen(ss));
				    s[strlen(ss)] = '.';
				}
				yyval.text = util_strdup(s);
				util_free(s);
			    } else {
				printError(thisParserPtr,
					   ERR_UNKNOWN_OIDLABEL, yyvsp[0].text);
				yyval.text = NULL;
			    }
			;
    break;}
case 324:
#line 3351 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 325:
#line 3357 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 326:
#line 3361 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 327:
#line 3368 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 328:
#line 3372 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 329:
#line 3382 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 330:
#line 3391 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 331:
#line 3397 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 0);
			;
    break;}
case 332:
#line 3405 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 333:
#line 3414 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 334:
#line 3423 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 335:
#line 3427 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 336:
#line 3437 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 337:
#line 3444 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 338:
#line 3450 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 339:
#line 3456 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/share/misc/bison.simple"

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
}
#line 3461 "parser-sming.y"


#endif
			
