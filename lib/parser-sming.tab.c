
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
#define	abnfKeyword	304
#define	OctetStringKeyword	305
#define	ObjectIdentifierKeyword	306
#define	Integer32Keyword	307
#define	Unsigned32Keyword	308
#define	Integer64Keyword	309
#define	Unsigned64Keyword	310
#define	Float32Keyword	311
#define	Float64Keyword	312
#define	Float128Keyword	313
#define	BitsKeyword	314
#define	EnumerationKeyword	315
#define	currentKeyword	316
#define	deprecatedKeyword	317
#define	obsoleteKeyword	318
#define	noaccessKeyword	319
#define	notifyonlyKeyword	320
#define	readonlyKeyword	321
#define	readwriteKeyword	322
#define	readcreateKeyword	323

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



#define	YYFINAL		766
#define	YYFLAG		-32768
#define	YYNTBASE	78

#define YYTRANSLATE(x) ((unsigned)(x) <= 323 ? yytranslate[x] : 314)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    72,
    73,     2,     2,    75,    76,    77,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    71,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    69,    74,    70,     2,     2,     2,     2,     2,
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
    66,    67,    68
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     4,     6,     8,    11,    14,    15,    16,    17,
    18,    19,    49,    50,    52,    54,    57,    60,    61,    62,
    63,    64,    65,    84,    85,    87,    89,    92,    95,    96,
    97,    98,    99,   100,   101,   102,   103,   129,   130,   132,
   134,   137,   140,   142,   144,   146,   147,   148,   149,   150,
   151,   171,   172,   173,   174,   175,   176,   177,   178,   179,
   180,   181,   214,   215,   216,   217,   218,   219,   242,   243,
   244,   245,   246,   247,   248,   249,   276,   278,   281,   284,
   285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
   327,   328,   330,   332,   335,   338,   339,   340,   341,   342,
   343,   344,   367,   368,   370,   372,   375,   378,   379,   380,
   381,   382,   383,   384,   408,   409,   411,   413,   416,   419,
   420,   421,   422,   423,   424,   425,   426,   427,   454,   455,
   457,   459,   462,   465,   466,   467,   481,   482,   484,   486,
   489,   492,   504,   510,   511,   514,   520,   521,   524,   530,
   532,   534,   536,   538,   540,   551,   557,   570,   576,   589,
   590,   593,   594,   597,   602,   603,   606,   612,   618,   624,
   630,   636,   637,   640,   646,   647,   650,   656,   657,   660,
   666,   667,   670,   676,   677,   680,   686,   687,   690,   696,
   697,   700,   706,   707,   710,   716,   726,   727,   730,   740,
   741,   744,   754,   755,   757,   759,   762,   765,   776,   777,
   779,   781,   784,   787,   801,   803,   805,   808,   810,   813,
   816,   819,   822,   825,   828,   831,   834,   837,   840,   841,
   844,   846,   848,   849,   852,   859,   860,   862,   864,   867,
   872,   875,   876,   878,   883,   884,   887,   894,   895,   897,
   899,   902,   907,   910,   911,   913,   918,   924,   928,   929,
   931,   933,   936,   941,   949,   953,   954,   956,   958,   961,
   966,   970,   971,   973,   975,   978,   983,   987,   988,   990,
   992,   995,  1000,  1006,  1008,  1012,  1013,  1015,  1017,  1020,
  1025,  1027,  1029,  1031,  1033,  1037,  1039,  1043,  1045,  1048,
  1049,  1051,  1053,  1056,  1059,  1061,  1063,  1065,  1067,  1069,
  1072,  1074,  1076,  1078,  1080,  1083,  1088,  1090,  1092,  1094,
  1096,  1098,  1100,  1102,  1105,  1107,  1109,  1110,  1112,  1114,
  1117,  1120,  1122,  1124,  1126,  1129,  1131,  1133,  1134,  1135,
  1136
};

static const short yyrhs[] = {   312,
    79,     0,     0,    80,     0,    81,     0,    80,    81,     0,
    82,   312,     0,     0,     0,     0,     0,     0,    11,   311,
     5,    83,   312,    69,   313,   189,   217,   313,    84,   218,
   313,    85,   230,   313,    86,   231,    87,   195,    88,    97,
   109,   157,   167,   177,    70,   312,    71,     0,     0,    89,
     0,    90,     0,    89,    90,     0,    91,   313,     0,     0,
     0,     0,     0,     0,    20,   311,     6,    92,   312,    69,
   313,   223,    93,   229,    94,   231,    95,   233,    96,    70,
   312,    71,     0,     0,    98,     0,    99,     0,    98,    99,
     0,   100,   313,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    21,   311,     5,   101,   312,    69,   313,   199,
   313,   102,   227,   103,   219,   104,   221,   105,   223,   106,
   229,   107,   231,   108,    70,   312,    71,     0,     0,   110,
     0,   111,     0,   110,   111,     0,   112,   313,     0,   113,
     0,   119,     0,   130,     0,     0,     0,     0,     0,     0,
    24,   311,     6,   114,   312,    69,   313,   215,   313,   115,
   223,   116,   229,   117,   231,   118,    70,   312,    71,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    25,   311,     6,   120,   312,    69,   313,   215,   313,   121,
   201,   313,   122,   226,   313,   123,   227,   124,   219,   125,
   221,   126,   223,   127,   230,   313,   128,   231,   129,    70,
   312,    71,     0,     0,     0,     0,     0,     0,    26,   311,
     6,   131,   312,    69,   313,   215,   313,   132,   223,   133,
   230,   313,   134,   231,   135,   136,   313,    70,   312,    71,
     0,     0,     0,     0,     0,     0,     0,     0,    28,   311,
     6,   137,   312,    69,   313,   215,   313,   138,   204,   313,
   139,   211,   140,   223,   141,   230,   313,   142,   231,   143,
   144,    70,   312,    71,     0,   145,     0,   144,   145,     0,
   146,   313,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    27,   311,     6,   147,   312,    69,   313,
   215,   313,   148,   201,   313,   149,   226,   313,   150,   227,
   151,   219,   152,   221,   153,   223,   154,   230,   313,   155,
   231,   156,    70,   312,    71,     0,     0,   158,     0,   159,
     0,   158,   159,     0,   160,   313,     0,     0,     0,     0,
     0,     0,     0,    29,   311,     6,   161,   312,    69,   313,
   215,   313,   162,   236,   163,   223,   164,   230,   313,   165,
   231,   166,    70,   312,    71,     0,     0,   168,     0,   169,
     0,   168,   169,     0,   170,   313,     0,     0,     0,     0,
     0,     0,     0,    30,   311,     6,   171,   312,    69,   313,
   215,   313,   172,   235,   313,   173,   223,   174,   230,   313,
   175,   231,   176,    70,   312,    71,     0,     0,   178,     0,
   179,     0,   178,   179,     0,   180,   313,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    31,   311,     6,   181,
   312,    69,   313,   215,   313,   182,   223,   183,   230,   313,
   184,   231,   185,   238,   186,   240,   187,   244,   188,    70,
   312,    71,     0,     0,   190,     0,   191,     0,   190,   191,
     0,   192,   313,     0,     0,     0,    12,   311,     5,   193,
   312,    72,   312,   275,   194,   312,    73,   312,    71,     0,
     0,   196,     0,   197,     0,   196,   197,     0,   198,   313,
     0,    13,   312,    69,   313,   216,   313,   230,   313,    70,
   312,    71,     0,    22,   311,   248,   312,    71,     0,     0,
   201,   313,     0,    22,   311,   248,   312,    71,     0,     0,
   203,   313,     0,    23,   311,   248,   312,    71,     0,   205,
     0,   206,     0,   207,     0,   208,     0,   209,     0,    38,
   210,   312,    72,   312,   279,   312,    73,   312,    71,     0,
    39,   311,   290,   312,    71,     0,    40,   311,   290,   210,
   312,    72,   312,   279,   312,    73,   312,    71,     0,    41,
   311,   290,   312,    71,     0,    42,   311,   290,   210,   312,
    72,   312,   279,   312,    73,   312,    71,     0,     0,   311,
    37,     0,     0,   212,   313,     0,    43,   213,   312,    71,
     0,     0,   312,   214,     0,    72,   312,   279,   312,    73,
     0,    14,   311,   301,   312,    71,     0,    15,   311,   295,
   312,    71,     0,    16,   311,   291,   312,    71,     0,    17,
   311,   291,   312,    71,     0,     0,   220,   313,     0,    32,
   311,   296,   312,    71,     0,     0,   222,   313,     0,    33,
   311,   297,   312,    71,     0,     0,   224,   313,     0,    34,
   311,   299,   312,    71,     0,     0,   226,   313,     0,    35,
   311,   300,   312,    71,     0,     0,   228,   313,     0,    36,
   311,   298,   312,    71,     0,     0,   230,   313,     0,    18,
   311,   291,   312,    71,     0,     0,   232,   313,     0,    19,
   311,   291,   312,    71,     0,     0,   234,   313,     0,    49,
   311,   291,   312,    71,     0,    44,   312,    72,   312,   279,
   312,    73,   312,    71,     0,     0,   237,   313,     0,    45,
   312,    72,   312,   279,   312,    73,   312,    71,     0,     0,
   239,   313,     0,    46,   312,    72,   312,   279,   312,    73,
   312,    71,     0,     0,   241,     0,   242,     0,   241,   242,
     0,   243,   313,     0,    47,   311,   290,   312,    69,   230,
   313,    70,   312,    71,     0,     0,   245,     0,   246,     0,
   245,   246,     0,   247,   313,     0,    48,   311,   290,   312,
    69,   200,   202,   225,   230,   313,    70,   312,    71,     0,
   249,     0,   250,     0,    50,   253,     0,    51,     0,    52,
   253,     0,    53,   253,     0,    54,   253,     0,    55,   253,
     0,    56,   261,     0,    57,   261,     0,    58,   261,     0,
    60,   269,     0,    59,   269,     0,   289,   251,     0,     0,
   312,   252,     0,   254,     0,   262,     0,     0,   312,   254,
     0,    72,   312,   258,   255,   312,    73,     0,     0,   256,
     0,   257,     0,   256,   257,     0,   312,    74,   312,   258,
     0,   309,   259,     0,     0,   260,     0,   312,     3,   312,
   309,     0,     0,   312,   262,     0,    72,   312,   266,   263,
   312,    73,     0,     0,   264,     0,   265,     0,   264,   265,
     0,   312,    74,   312,   266,     0,     7,   267,     0,     0,
   268,     0,   312,     3,   312,     7,     0,    72,   312,   270,
   312,    73,     0,   274,   271,   310,     0,     0,   272,     0,
   273,     0,   272,   273,     0,   312,    75,   312,   274,     0,
     6,   312,    72,   312,   307,   312,    73,     0,   288,   276,
   310,     0,     0,   277,     0,   278,     0,   277,   278,     0,
   312,    75,   312,   288,     0,     0,     0,   310,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   312,    75,   312,
     0,     0,   290,   280,   310,     0,     0,   281,     0,   282,
     0,   281,   282,     0,   312,    75,   312,   290,     0,    72,
   312,   284,   312,    73,     0,   310,     0,     6,   285,   310,
     0,     0,   286,     0,   287,     0,   286,   287,     0,   312,
    75,   312,     6,     0,     5,     0,     6,     0,   289,     0,
   290,     0,     5,     4,     5,     0,     5,     0,     5,     4,
     6,     0,     6,     0,     8,   292,     0,     0,   293,     0,
   294,     0,   293,   294,     0,   312,     8,     0,     8,     0,
     8,     0,     8,     0,   283,     0,     9,     0,    76,     9,
     0,    10,     0,     7,     0,   291,     0,   290,     0,   290,
   304,     0,   306,    77,   306,   304,     0,    61,     0,    62,
     0,    63,     0,    64,     0,    65,     0,    66,     0,    67,
     0,   302,   303,     0,   290,     0,   306,     0,     0,   304,
     0,   305,     0,   304,   305,     0,    77,   306,     0,     9,
     0,    10,     0,     9,     0,    76,     9,     0,   307,     0,
   308,     0,     0,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   518,   528,   532,   538,   542,   557,   571,   592,   601,   608,
   619,   629,   687,   691,   701,   705,   721,   735,   744,   752,
   758,   764,   766,   773,   777,   787,   791,   807,   821,   825,
   846,   852,   862,   868,   874,   880,   885,   893,   897,   907,
   911,   927,   941,   942,   943,   946,   950,   966,   972,   985,
   990,  1007,  1011,  1025,  1040,  1047,  1053,  1063,  1069,  1074,
  1081,  1086,  1095,  1099,  1116,  1121,  1128,  1133,  1142,  1146,
  1162,  1187,  1198,  1203,  1210,  1216,  1224,  1232,  1247,  1261,
  1265,  1279,  1294,  1301,  1307,  1317,  1323,  1328,  1335,  1340,
  1349,  1353,  1363,  1367,  1383,  1397,  1401,  1417,  1433,  1438,
  1446,  1451,  1459,  1463,  1473,  1477,  1492,  1506,  1510,  1523,
  1540,  1545,  1552,  1557,  1565,  1569,  1579,  1583,  1599,  1613,
  1617,  1633,  1638,  1645,  1651,  1667,  1683,  1722,  1730,  1734,
  1740,  1744,  1759,  1773,  1777,  1790,  1805,  1809,  1815,  1819,
  1834,  1848,  1857,  1865,  1869,  1875,  1882,  1886,  1892,  1899,
  1900,  1901,  1902,  1903,  1906,  1927,  1943,  1965,  1981,  2003,
  2007,  2013,  2017,  2023,  2033,  2037,  2043,  2049,  2055,  2061,
  2067,  2073,  2077,  2083,  2089,  2093,  2099,  2105,  2109,  2115,
  2121,  2125,  2131,  2137,  2141,  2147,  2153,  2157,  2163,  2169,
  2173,  2179,  2185,  2189,  2195,  2201,  2208,  2212,  2218,  2225,
  2229,  2235,  2242,  2246,  2252,  2258,  2272,  2278,  2291,  2295,
  2301,  2307,  2320,  2326,  2356,  2360,  2366,  2381,  2385,  2400,
  2415,  2430,  2445,  2460,  2475,  2490,  2505,  2522,  2536,  2540,
  2546,  2550,  2556,  2560,  2566,  2575,  2579,  2585,  2591,  2604,
  2610,  2624,  2628,  2634,  2640,  2644,  2650,  2659,  2663,  2669,
  2675,  2688,  2694,  2713,  2717,  2723,  2729,  2735,  2744,  2748,
  2754,  2760,  2775,  2781,  2790,  2798,  2802,  2808,  2814,  2827,
  2833,  2841,  2845,  2851,  2857,  2870,  2876,  2884,  2888,  2894,
  2900,  2913,  2919,  2925,  2929,  2937,  2941,  2947,  2953,  2966,
  2972,  2976,  2982,  2986,  2992,  3003,  3009,  3020,  3026,  3040,
  3044,  3050,  3054,  3064,  3070,  3076,  3082,  3095,  3122,  3168,
  3190,  3196,  3202,  3215,  3239,  3244,  3251,  3255,  3259,  3265,
  3269,  3273,  3277,  3283,  3312,  3350,  3356,  3360,  3367,  3371,
  3381,  3390,  3396,  3404,  3413,  3422,  3426,  3436,  3443,  3449,
  3455
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
"objectsKeyword","mandatoryKeyword","optionalKeyword","refineKeyword","abnfKeyword",
"OctetStringKeyword","ObjectIdentifierKeyword","Integer32Keyword","Unsigned32Keyword",
"Integer64Keyword","Unsigned64Keyword","Float32Keyword","Float64Keyword","Float128Keyword",
"BitsKeyword","EnumerationKeyword","currentKeyword","deprecatedKeyword","obsoleteKeyword",
"noaccessKeyword","notifyonlyKeyword","readonlyKeyword","readwriteKeyword","readcreateKeyword",
"'{'","'}'","';'","'('","')'","'|'","','","'-'","'.'","smingFile","moduleStatement_optsep_0n",
"moduleStatement_optsep_1n","moduleStatement_optsep","moduleStatement","@1",
"@2","@3","@4","@5","extensionStatement_stmtsep_0n","extensionStatement_stmtsep_1n",
"extensionStatement_stmtsep","extensionStatement","@6","@7","@8","@9","@10",
"typedefStatement_stmtsep_0n","typedefStatement_stmtsep_1n","typedefStatement_stmtsep",
"typedefStatement","@11","@12","@13","@14","@15","@16","@17","@18","anyObjectStatement_stmtsep_0n",
"anyObjectStatement_stmtsep_1n","anyObjectStatement_stmtsep","anyObjectStatement",
"nodeStatement","@19","@20","@21","@22","@23","scalarStatement","@24","@25",
"@26","@27","@28","@29","@30","@31","@32","@33","tableStatement","@34","@35",
"@36","@37","@38","rowStatement","@39","@40","@41","@42","@43","@44","@45","columnStatement_stmtsep_1n",
"columnStatement_stmtsep","columnStatement","@46","@47","@48","@49","@50","@51",
"@52","@53","@54","@55","notificationStatement_stmtsep_0n","notificationStatement_stmtsep_1n",
"notificationStatement_stmtsep","notificationStatement","@56","@57","@58","@59",
"@60","@61","groupStatement_stmtsep_0n","groupStatement_stmtsep_1n","groupStatement_stmtsep",
"groupStatement","@62","@63","@64","@65","@66","@67","complianceStatement_stmtsep_0n",
"complianceStatement_stmtsep_1n","complianceStatement_stmtsep","complianceStatement",
"@68","@69","@70","@71","@72","@73","@74","@75","importStatement_stmtsep_0n",
"importStatement_stmtsep_1n","importStatement_stmtsep","importStatement","@76",
"@77","revisionStatement_stmtsep_0n","revisionStatement_stmtsep_1n","revisionStatement_stmtsep",
"revisionStatement","typedefTypeStatement","typeStatement_stmtsep_01","typeStatement",
"writetypeStatement_stmtsep_01","writetypeStatement","anyIndexStatement","indexStatement",
"augmentsStatement","reordersStatement","sparseStatement","expandsStatement",
"sep_impliedKeyword_01","createStatement_stmtsep_01","createStatement","optsep_createColumns_01",
"createColumns","oidStatement","dateStatement","organizationStatement","contactStatement",
"formatStatement_stmtsep_01","formatStatement","unitsStatement_stmtsep_01","unitsStatement",
"statusStatement_stmtsep_01","statusStatement","accessStatement_stmtsep_01",
"accessStatement","defaultStatement_stmtsep_01","defaultStatement","descriptionStatement_stmtsep_01",
"descriptionStatement","referenceStatement_stmtsep_01","referenceStatement",
"abnfStatement_stmtsep_01","abnfStatement","membersStatement","objectsStatement_stmtsep_01",
"objectsStatement","mandatoryStatement_stmtsep_01","mandatoryStatement","optionalStatement_stmtsep_0n",
"optionalStatement_stmtsep_1n","optionalStatement_stmtsep","optionalStatement",
"refineStatement_stmtsep_0n","refineStatement_stmtsep_1n","refineStatement_stmtsep",
"refineStatement","refinedBaseType_refinedType","refinedBaseType","refinedType",
"optsep_anySpec_01","anySpec","optsep_numberSpec_01","numberSpec","furtherNumberElement_0n",
"furtherNumberElement_1n","furtherNumberElement","numberElement","numberUpperLimit_01",
"numberUpperLimit","optsep_floatSpec_01","floatSpec","furtherFloatElement_0n",
"furtherFloatElement_1n","furtherFloatElement","floatElement","floatUpperLimit_01",
"floatUpperLimit","bitsOrEnumerationSpec","bitsOrEnumerationList","furtherBitsOrEnumerationItem_0n",
"furtherBitsOrEnumerationItem_1n","furtherBitsOrEnumerationItem","bitsOrEnumerationItem",
"identifierList","furtherIdentifier_0n","furtherIdentifier_1n","furtherIdentifier",
"qlcIdentifierList","furtherQlcIdentifier_0n","furtherQlcIdentifier_1n","furtherQlcIdentifier",
"bitsValue","bitsList","furtherLcIdentifier_0n","furtherLcIdentifier_1n","furtherLcIdentifier",
"identifier","qucIdentifier","qlcIdentifier","text","optsep_textSegment_0n",
"optsep_textSegment_1n","optsep_textSegment","date","format","units","anyValue",
"status","access","objectIdentifier","qlcIdentifier_subid","dot_subid_0127",
"dot_subid_1n","dot_subid","subid","number","negativeNumber","signedNumber",
"optsep_comma_01","sep","optsep","stmtsep", NULL
};
#endif

static const short yyr1[] = {     0,
    78,    79,    79,    80,    80,    81,    83,    84,    85,    86,
    87,    82,    88,    88,    89,    89,    90,    92,    93,    94,
    95,    96,    91,    97,    97,    98,    98,    99,   101,   102,
   103,   104,   105,   106,   107,   108,   100,   109,   109,   110,
   110,   111,   112,   112,   112,   114,   115,   116,   117,   118,
   113,   120,   121,   122,   123,   124,   125,   126,   127,   128,
   129,   119,   131,   132,   133,   134,   135,   130,   137,   138,
   139,   140,   141,   142,   143,   136,   144,   144,   145,   147,
   148,   149,   150,   151,   152,   153,   154,   155,   156,   146,
   157,   157,   158,   158,   159,   161,   162,   163,   164,   165,
   166,   160,   167,   167,   168,   168,   169,   171,   172,   173,
   174,   175,   176,   170,   177,   177,   178,   178,   179,   181,
   182,   183,   184,   185,   186,   187,   188,   180,   189,   189,
   190,   190,   191,   193,   194,   192,   195,   195,   196,   196,
   197,   198,   199,   200,   200,   201,   202,   202,   203,   204,
   204,   204,   204,   204,   205,   206,   207,   208,   209,   210,
   210,   211,   211,   212,   213,   213,   214,   215,   216,   217,
   218,   219,   219,   220,   221,   221,   222,   223,   223,   224,
   225,   225,   226,   227,   227,   228,   229,   229,   230,   231,
   231,   232,   233,   233,   234,   235,   236,   236,   237,   238,
   238,   239,   240,   240,   241,   241,   242,   243,   244,   244,
   245,   245,   246,   247,   248,   248,   249,   249,   249,   249,
   249,   249,   249,   249,   249,   249,   249,   250,   251,   251,
   252,   252,   253,   253,   254,   255,   255,   256,   256,   257,
   258,   259,   259,   260,   261,   261,   262,   263,   263,   264,
   264,   265,   266,   267,   267,   268,   269,   270,   271,   271,
   272,   272,   273,   274,   275,   276,   276,   277,   277,   278,
    -1,    -1,    -1,    -1,    -1,    -1,   279,   280,   280,   281,
   281,   282,   283,   284,   284,   285,   285,   286,   286,   287,
   288,   288,    -1,    -1,   289,   289,   290,   290,   291,   292,
   292,   293,   293,   294,   295,   296,   297,   298,   298,   298,
   298,   298,   298,   298,   298,   298,   299,   299,   299,   300,
   300,   300,   300,   301,   302,   302,   303,   303,   304,   304,
   305,   306,   307,   307,   308,   309,   309,   310,   311,   312,
   313
};

static const short yyr2[] = {     0,
     2,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,    29,     0,     1,     1,     2,     2,     0,     0,     0,
     0,     0,    18,     0,     1,     1,     2,     2,     0,     0,
     0,     0,     0,     0,     0,     0,    25,     0,     1,     1,
     2,     2,     1,     1,     1,     0,     0,     0,     0,     0,
    19,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    32,     0,     0,     0,     0,     0,    22,     0,     0,
     0,     0,     0,     0,     0,    26,     1,     2,     2,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
     0,     1,     1,     2,     2,     0,     0,     0,     0,     0,
     0,    22,     0,     1,     1,     2,     2,     0,     0,     0,
     0,     0,     0,    23,     0,     1,     1,     2,     2,     0,
     0,     0,     0,     0,     0,     0,     0,    26,     0,     1,
     1,     2,     2,     0,     0,    13,     0,     1,     1,     2,
     2,    11,     5,     0,     2,     5,     0,     2,     5,     1,
     1,     1,     1,     1,    10,     5,    12,     5,    12,     0,
     2,     0,     2,     4,     0,     2,     5,     5,     5,     5,
     5,     0,     2,     5,     0,     2,     5,     0,     2,     5,
     0,     2,     5,     0,     2,     5,     0,     2,     5,     0,
     2,     5,     0,     2,     5,     9,     0,     2,     9,     0,
     2,     9,     0,     1,     1,     2,     2,    10,     0,     1,
     1,     2,     2,    13,     1,     1,     2,     1,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     0,     2,
     1,     1,     0,     2,     6,     0,     1,     1,     2,     4,
     2,     0,     1,     4,     0,     2,     6,     0,     1,     1,
     2,     4,     2,     0,     1,     4,     5,     3,     0,     1,
     1,     2,     4,     7,     3,     0,     1,     1,     2,     4,
     3,     0,     1,     1,     2,     4,     3,     0,     1,     1,
     2,     4,     5,     1,     3,     0,     1,     1,     2,     4,
     1,     1,     1,     1,     3,     1,     3,     1,     2,     0,
     1,     1,     2,     2,     1,     1,     1,     1,     1,     2,
     1,     1,     1,     1,     2,     4,     1,     1,     1,     1,
     1,     1,     1,     2,     1,     1,     0,     1,     1,     2,
     2,     1,     1,     1,     2,     1,     1,     0,     0,     0,
     0
};

static const short yydefact[] = {   340,
     2,   339,     1,     3,     4,   340,     0,     5,     6,     7,
   340,     0,   341,   129,   339,     0,   130,   131,   341,     0,
   339,   341,   132,   133,   134,     0,     8,   340,   300,   340,
     0,     0,   299,   301,   302,     0,     0,   339,   341,   340,
   303,   304,   170,     0,     9,     0,   340,     0,   291,   292,
   135,   266,     0,   339,   341,   340,   338,   267,   268,     0,
   171,     0,    10,     0,   265,   269,   340,   340,   190,   340,
     0,     0,   339,    11,   341,     0,   270,   189,     0,   137,
   191,   136,   340,   340,    13,   138,   139,   341,     0,     0,
   339,    24,    14,    15,   341,   140,   141,   192,   341,     0,
   339,    38,    25,    26,   341,    16,    17,     0,    18,     0,
   339,   339,   339,    91,    39,    40,   341,    43,    44,    45,
    27,    28,   339,   341,   340,    29,     0,     0,     0,   339,
   103,    92,    93,   341,    41,    42,     0,     0,     0,   340,
    46,    52,    63,     0,   339,   115,   104,   105,   341,    94,
    95,   305,   340,   341,   341,     0,   340,   340,   340,    96,
     0,   339,     0,   116,   117,   341,   106,   107,     0,     0,
   178,   341,     0,     0,     0,   340,   108,     0,   340,   118,
   119,   169,   340,   339,    19,   341,     0,   341,   341,   341,
     0,   340,   120,     0,     0,     0,   187,   179,   339,   341,
     0,     0,     0,   341,     0,   340,    12,   142,   317,   318,
   319,   340,    20,   341,     0,    30,   339,   341,   341,   341,
     0,   341,     0,     0,   190,   188,   296,   233,   218,   233,
   233,   233,   233,   245,   245,   245,     0,     0,   340,   215,
   216,   229,   184,     0,    47,    53,    64,   341,     0,   341,
   180,    21,     0,   217,     0,   219,   220,   221,   222,   223,
     0,   224,   225,   340,   227,   226,     0,   228,     0,   339,
    31,   341,     0,   298,   332,   325,   340,   327,   326,   178,
     0,   178,    97,   341,     0,   193,   295,   340,   234,   340,
   246,     0,   143,   340,   230,   231,   232,     0,   172,   185,
     0,     0,     0,   324,   328,   329,    48,   339,   341,    65,
   197,   109,   341,   339,    22,   341,     0,     0,   340,   340,
   259,     0,   312,   309,   311,   340,     0,   308,   314,   313,
   340,     0,   339,    32,   341,   297,   168,   331,   330,   187,
     0,    54,     0,   340,    98,   341,     0,   121,     0,     0,
   194,   334,   333,     0,   236,   336,   337,   242,   254,   248,
     0,     0,   338,   260,   261,     0,   338,   310,   315,     0,
     0,     0,   175,   173,    49,   340,     0,   341,     0,   178,
   198,   340,   341,   178,   340,   340,   335,   340,   237,   238,
     0,   241,   243,     0,   253,   255,     0,   340,   249,   250,
     0,   340,   257,   258,   262,   340,   286,   340,   284,   186,
     0,   306,   340,   339,    33,   341,   190,     0,   339,   341,
    66,   340,    99,     0,   110,   122,     0,     0,     0,   239,
   340,   340,   340,     0,   251,   340,     0,     0,   338,   287,
   288,     0,     0,   316,     0,     0,   178,   176,    50,   146,
     0,    55,   190,     0,     0,   340,   178,     0,   195,    23,
   235,     0,     0,     0,   247,     0,   340,   263,   285,   289,
   340,   283,   174,   307,   340,    34,     0,   320,   321,   322,
   323,   340,   184,    67,   340,   278,   341,     0,   111,   341,
   240,   244,   256,   252,     0,     0,     0,   187,   340,     0,
    56,     0,     0,   338,   279,   280,     0,   100,   340,     0,
   123,   264,   290,   177,    35,     0,   183,   172,   339,   341,
   340,   277,   281,   340,   190,     0,   341,   190,   190,    51,
    57,     0,     0,     0,     0,   101,   340,   112,   124,    36,
   175,    69,   340,   199,   282,     0,     0,   190,   200,     0,
    58,   340,     0,   340,   196,   113,   340,   125,   341,   340,
   178,     0,    68,     0,     0,     0,   203,   201,     0,    59,
   341,   102,   340,   340,   339,   126,   204,   205,   341,    37,
     0,     0,     0,     0,     0,   209,   206,   207,   341,   341,
   114,   340,   340,   339,   127,   210,   211,   341,    60,    70,
     0,     0,     0,     0,   212,   213,   190,     0,   340,     0,
   340,   340,    61,   160,   339,   339,   339,   339,   341,   150,
   151,   152,   153,   154,     0,   341,     0,     0,     0,   340,
     0,     0,     0,     0,     0,    71,   202,     0,   144,   128,
   340,     0,   161,   340,   160,   340,   160,   162,   340,   147,
   341,     0,   340,     0,   340,     0,   340,   165,    72,   341,
     0,   339,   181,   341,   145,    62,     0,   156,     0,   158,
     0,   340,     0,   178,   163,   208,     0,     0,   341,   148,
   340,   340,   340,     0,   340,   166,    73,   340,   341,   182,
     0,     0,     0,   164,     0,     0,     0,     0,   340,   340,
   340,   340,   341,   149,   340,     0,     0,     0,     0,    74,
     0,   155,   340,   340,   167,   190,   214,     0,     0,    75,
   157,   159,     0,   339,     0,    77,   341,     0,   340,    78,
    79,    80,     0,   340,    76,     0,   341,     0,   341,    81,
     0,   341,    82,     0,   341,    83,   184,    84,   172,    85,
   175,    86,   178,    87,     0,   341,    88,   190,    89,     0,
   340,     0,    90,     0,     0,     0
};

static const short yydefgoto[] = {   764,
     3,     4,     5,     6,    11,    31,    48,    69,    80,    92,
    93,    94,    95,   125,   197,   225,   286,   350,   102,   103,
   104,   105,   140,   243,   299,   373,   447,   498,   529,   550,
   114,   115,   116,   117,   118,   157,   280,   340,   417,   477,
   119,   158,   281,   377,   483,   518,   541,   561,   581,   607,
   629,   120,   159,   282,   343,   453,   502,   520,   552,   608,
   648,   674,   696,   716,   723,   725,   726,   727,   734,   741,
   744,   747,   749,   751,   753,   755,   758,   760,   131,   132,
   133,   134,   176,   311,   380,   455,   525,   546,   146,   147,
   148,   149,   192,   347,   457,   510,   548,   565,   163,   164,
   165,   166,   206,   384,   458,   528,   549,   567,   586,   604,
    16,    17,    18,    19,    28,    56,    85,    86,    87,    88,
   200,   650,   309,   663,   664,   619,   620,   621,   622,   623,
   624,   630,   659,   660,   672,   686,   218,   124,    22,    39,
   334,   335,   415,   416,   185,   186,   678,   420,   271,   272,
   213,   214,    74,    75,   315,   316,   383,   345,   346,   558,
   559,   576,   577,   578,   579,   595,   596,   597,   598,   239,
   240,   241,   268,   295,   254,   289,   388,   389,   390,   355,
   392,   393,   260,   291,   398,   399,   400,   360,   395,   396,
   265,   320,   363,   364,   365,   321,    51,    57,    58,    59,
   485,   504,   505,   506,   328,   408,   439,   440,   441,    52,
   242,   486,    30,    33,    34,    35,   153,   413,   475,   331,
   212,   482,   277,   278,   304,   305,   306,   279,   356,   357,
   358,    65,   631,   255,    14
};

static const short yypact[] = {-32768,
    13,-32768,-32768,    13,-32768,-32768,    34,-32768,-32768,-32768,
-32768,   -36,-32768,    43,-32768,    53,    43,-32768,-32768,    70,
-32768,-32768,-32768,-32768,-32768,    69,-32768,-32768,    77,-32768,
    65,    27,-32768,    77,-32768,    99,    44,-32768,-32768,-32768,
-32768,-32768,-32768,    69,-32768,    74,-32768,    93,-32768,-32768,
-32768,    39,    46,-32768,-32768,-32768,-32768,    39,-32768,    45,
-32768,    69,-32768,    50,-32768,-32768,-32768,-32768,   102,-32768,
    74,    66,-32768,-32768,-32768,    67,-32768,-32768,    69,   106,
-32768,-32768,-32768,-32768,   104,   106,-32768,-32768,    68,    72,
-32768,   121,   104,-32768,-32768,-32768,-32768,-32768,-32768,   137,
-32768,    40,   121,-32768,-32768,-32768,-32768,   129,-32768,   140,
-32768,-32768,-32768,   117,    40,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   141,   142,   143,-32768,
   120,   117,-32768,-32768,-32768,-32768,   146,    93,    86,-32768,
-32768,-32768,-32768,   154,-32768,   130,   120,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    94,-32768,-32768,-32768,-32768,
   156,-32768,    96,   130,-32768,-32768,-32768,-32768,    98,    97,
   136,-32768,    95,   103,   105,-32768,-32768,   167,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   153,-32768,-32768,-32768,
   108,-32768,-32768,   107,   109,    26,    93,-32768,-32768,-32768,
   168,   168,   168,-32768,   112,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    76,-32768,-32768,-32768,-32768,-32768,
   168,-32768,   115,   116,   102,-32768,   182,   118,-32768,   118,
   118,   118,   118,   118,   118,   118,   119,   119,-32768,-32768,
-32768,   118,   157,    48,-32768,-32768,-32768,-32768,   168,-32768,
-32768,-32768,   184,-32768,   125,-32768,-32768,-32768,-32768,-32768,
   126,-32768,-32768,-32768,-32768,-32768,   128,-32768,   131,-32768,
-32768,-32768,   196,-32768,-32768,-32768,-32768,   124,-32768,   136,
   180,   136,-32768,-32768,   168,   158,-32768,-32768,-32768,-32768,
-32768,   199,-32768,-32768,-32768,-32768,-32768,    36,   176,-32768,
   203,   139,   202,-32768,   124,-32768,-32768,-32768,-32768,-32768,
   169,-32768,-32768,-32768,-32768,-32768,    28,   206,-32768,-32768,
    39,    16,-32768,   138,-32768,-32768,   207,-32768,   124,-32768,
-32768,   148,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    93,
    76,-32768,    93,-32768,-32768,-32768,   173,-32768,    69,   159,
-32768,-32768,-32768,   209,   152,-32768,-32768,   217,   217,   152,
   155,   160,-32768,    39,-32768,   162,   222,-32768,   124,   161,
   202,   223,   210,-32768,-32768,-32768,   205,-32768,   172,   136,
-32768,-32768,-32768,   136,-32768,-32768,-32768,-32768,   152,-32768,
   171,-32768,-32768,   243,-32768,-32768,   244,-32768,   152,-32768,
   174,-32768,-32768,-32768,-32768,-32768,    39,-32768,-32768,-32768,
   124,-32768,-32768,-32768,-32768,-32768,   102,   178,-32768,-32768,
-32768,-32768,-32768,   181,-32768,-32768,   179,   183,   185,-32768,
-32768,-32768,-32768,   186,-32768,-32768,    92,   199,-32768,    39,
-32768,   177,   187,   124,   194,   247,   136,-32768,-32768,-32768,
     7,-32768,   102,   100,    93,-32768,   136,    93,-32768,-32768,
-32768,    28,    28,   249,-32768,   206,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   198,-32768,-32768,-32768,
-32768,-32768,   157,-32768,-32768,    39,-32768,   100,-32768,-32768,
-32768,-32768,-32768,-32768,   193,   251,   200,    93,-32768,   201,
-32768,   242,   208,-32768,    39,-32768,   204,-32768,-32768,    93,
-32768,-32768,-32768,-32768,-32768,   211,-32768,   176,-32768,-32768,
-32768,-32768,-32768,-32768,   102,   212,-32768,   102,   102,-32768,
-32768,   268,   213,   215,   100,-32768,-32768,-32768,-32768,-32768,
   210,-32768,-32768,-32768,-32768,   214,   216,   102,   229,   219,
-32768,-32768,   220,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   136,   224,-32768,   221,   226,   225,   231,-32768,   227,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   231,-32768,-32768,-32768,
    93,   168,   230,   100,   100,   228,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   228,-32768,-32768,-32768,-32768,
   234,   235,   100,   238,-32768,-32768,   102,    21,-32768,    93,
-32768,-32768,-32768,   263,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   239,-32768,   240,   245,   241,-32768,
   276,   100,   100,   100,   100,-32768,-32768,   252,   180,-32768,
-32768,   256,-32768,-32768,   263,-32768,   263,   271,-32768,   292,
-32768,   246,-32768,   253,-32768,   258,-32768,   118,-32768,-32768,
   259,-32768,   205,-32768,-32768,-32768,   100,-32768,   260,-32768,
   261,-32768,   264,   136,-32768,-32768,    76,    93,-32768,-32768,
-32768,-32768,-32768,   267,-32768,-32768,-32768,-32768,-32768,-32768,
   250,   100,   100,-32768,   100,    93,   269,   265,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   270,   266,   272,   273,-32768,
   277,-32768,-32768,-32768,-32768,   102,-32768,   279,   280,-32768,
-32768,-32768,   315,-32768,     8,-32768,-32768,   337,-32768,-32768,
-32768,-32768,   281,-32768,-32768,   278,-32768,   168,-32768,-32768,
   180,-32768,-32768,   205,-32768,-32768,   157,-32768,   176,-32768,
   210,-32768,   136,-32768,    93,-32768,-32768,   102,-32768,   283,
-32768,   285,-32768,   354,   357,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   358,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   275,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   262,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   248,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -364,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   237,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   232,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   233,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   349,-32768,-32768,-32768,-32768,-32768,   284,-32768,
-32768,-32768,  -625,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,  -596,-32768,-32768,-32768,-32768,  -199,-32768,-32768,-32768,
  -511,-32768,  -532,-32768,  -262,-32768,-32768,  -647,  -478,-32768,
  -330,   -40,  -223,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -206,-32768,-32768,-32768,  -224,-32768,  -340,
-32768,-32768,-32768,-32768,  -137,   111,-32768,-32768,   -16,   -88,
-32768,-32768,  -126,   122,-32768,-32768,   -24,   -85,-32768,-32768,
   149,-32768,-32768,-32768,    14,   -48,-32768,-32768,-32768,   334,
  -471,-32768,-32768,  -112,-32768,-32768,-32768,-32768,   -46,   325,
-32768,  -208,   -31,-32768,-32768,   366,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -308,  -293,  -271,   -34,-32768,
   -62,  -348,   150,     0,   473
};


#define	YYLAST		1229


static const short yytable[] = {     1,
   376,   252,   219,   220,   501,     9,   531,    55,   551,   375,
    12,   339,    47,   651,   404,   679,   509,   307,   409,   310,
   369,   248,   359,     2,   352,   353,   332,    32,    36,    37,
    68,   338,    13,    36,   724,   276,   352,   353,    10,    46,
   273,   274,   323,    29,   324,   325,    53,    83,   655,   284,
   657,    60,   273,   274,    15,    64,   275,    60,   614,   615,
   616,   617,   618,   111,   112,   113,    71,    72,    21,    76,
   478,   479,   480,   481,    25,   339,    29,   729,    49,    50,
   227,    38,    89,    90,  -340,   313,   209,   210,   211,   329,
   469,   354,   256,   257,   258,   259,   745,   154,    40,   411,
   352,   353,   444,   354,   273,   274,    42,   326,   262,   263,
    54,   327,   592,  -340,    43,   742,    61,   423,    84,    67,
    73,   426,    70,    91,   139,   228,   229,   230,   231,   232,
   233,   234,   235,   236,   237,   238,    78,    82,    98,   156,
    99,   101,   109,   123,   126,   130,   141,   142,   143,   145,
   339,     7,   169,   152,   155,   522,   173,   174,   175,   160,
   162,   177,   172,   188,    20,   179,   183,   515,   182,   184,
    26,   189,   193,   190,   199,   191,   204,   207,   194,   208,
   222,   217,   195,   250,   476,   253,   251,    44,   287,  -340,
   264,   205,   270,   449,   489,   681,   288,   290,   293,   301,
   303,   308,   294,    62,   319,   223,   314,   333,   336,   337,
   275,   224,   359,   344,  -332,   368,   382,   387,   752,  -340,
   700,   701,    79,   702,   371,  -340,   402,   407,   386,   484,
   412,   410,   403,   261,   261,   261,   406,   750,   267,   419,
   100,   269,   414,   422,   431,   432,   433,   436,   450,   459,
   110,   471,   456,   460,   474,   493,   513,   461,   465,   472,
   127,   128,   129,   292,   473,   512,   330,   499,   748,   519,
   514,   517,   137,   542,   557,   594,   302,   575,   524,   144,
   521,   530,   543,   554,   537,   544,   555,   317,   560,   318,
   563,   572,   571,   322,   161,   573,   574,   580,   570,  -339,
   591,   536,   378,   610,   539,   540,   609,   612,   639,   637,
   641,   178,   643,   658,   662,   640,   666,   385,   361,   362,
   366,   649,   699,   668,   556,   367,   545,   653,   670,   676,
   370,   682,   683,   196,   705,   685,   688,   694,   713,   704,
   712,   724,   732,   379,   714,   715,   737,   717,   215,   721,
   722,   735,   761,   765,   391,   763,   766,   394,   397,   401,
   730,     8,   135,   366,   121,    23,   244,   106,   150,    96,
   587,   605,   430,   491,   435,   418,   593,   405,   167,   296,
   494,   424,   590,   613,   427,   428,   266,   429,   391,   468,
   297,    66,   523,   470,   611,    77,   180,   434,   401,    41,
   492,   437,   467,     0,     0,   438,   442,   443,     0,     0,
     0,   687,   445,     0,   487,     0,     0,   490,     0,   298,
     0,   454,     0,   644,   645,   646,   647,     0,     0,     0,
   462,   463,   464,     0,     0,   466,     0,     0,     0,   442,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   488,     0,   341,     0,     0,
     0,     0,     0,   349,     0,     0,   495,     0,     0,   527,
   496,     0,     0,     0,   497,     0,     0,     0,     0,     0,
     0,   500,   372,     0,   503,   507,     0,     0,     0,     0,
   754,    24,   720,     0,    27,     0,     0,     0,   516,     0,
     0,     0,     0,     0,   507,     0,     0,     0,   526,     0,
     0,    45,     0,     0,     0,     0,     0,     0,     0,     0,
   534,     0,     0,   535,     0,     0,     0,    63,     0,     0,
     0,     0,     0,     0,   759,     0,   547,     0,   739,     0,
   589,     0,   553,     0,     0,     0,     0,    81,     0,     0,
     0,   562,     0,   564,     0,     0,   566,     0,     0,   569,
    97,     0,     0,   446,     0,     0,     0,   107,   451,   626,
     0,   108,   583,   584,     0,     0,     0,   122,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   136,
     0,   601,   602,     0,     0,     0,   138,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   151,     0,   625,     0,
   627,   628,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   168,     0,     0,     0,     0,   170,   171,     0,   642,
     0,     0,     0,     0,     0,     0,     0,   689,   181,     0,
   652,     0,     0,   654,   187,   656,     0,     0,   661,     0,
     0,     0,   667,     0,   669,   703,   671,   673,   198,     0,
   201,   202,   203,     0,     0,     0,     0,     0,   532,     0,
     0,   684,   216,     0,     0,     0,   221,     0,     0,     0,
   691,   692,   693,     0,   695,     0,   226,   697,     0,     0,
   245,   246,   247,     0,   249,     0,     0,     0,   706,   707,
   708,   709,     0,     0,   711,     0,     0,     0,     0,     0,
     0,     0,   718,   719,   756,     0,     0,     0,     0,     0,
   283,     0,   285,     0,   585,     0,     0,     0,   733,     0,
     0,     0,     0,   736,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   603,   300,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   312,     0,     0,     0,
   762,     0,     0,     0,   632,   633,   634,   635,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   342,     0,     0,     0,   348,     0,     0,   351,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   374,     0,     0,
     0,   677,     0,     0,     0,     0,     0,     0,   381,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   421,     0,     0,     0,     0,   425,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   728,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   448,     0,
     0,     0,   452,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   508,
     0,     0,   511,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   533,     0,     0,     0,     0,     0,     0,   538,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   568,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   582,     0,     0,     0,     0,     0,     0,
     0,   588,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   599,   600,     0,     0,     0,     0,     0,     0,     0,
   606,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   636,     0,     0,     0,     0,     0,     0,   638,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   665,     0,     0,     0,     0,     0,     0,
     0,     0,   675,     0,     0,     0,   680,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   690,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   698,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   710,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   731,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   738,
     0,   740,     0,     0,   743,     0,     0,   746,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   757
};

static const short yycheck[] = {     0,
   341,   225,   202,   203,   483,     6,   518,    48,   541,   340,
    11,   305,    44,   639,   363,   663,   488,   280,   367,   282,
   329,   221,     7,    11,     9,    10,   298,    28,    29,    30,
    62,   303,    69,    34,    27,   244,     9,    10,     5,    40,
     5,     6,     7,     8,     9,    10,    47,    79,   645,   249,
   647,    52,     5,     6,    12,    56,     9,    58,    38,    39,
    40,    41,    42,    24,    25,    26,    67,    68,    16,    70,
    64,    65,    66,    67,     5,   369,     8,    70,     5,     6,
     5,    17,    83,    84,     8,   285,    61,    62,    63,   298,
   439,    76,   230,   231,   232,   233,   744,   138,    72,   371,
     9,    10,   411,    76,     5,     6,     8,    72,   235,   236,
    18,    76,   584,    75,    71,   741,    71,   380,    13,    75,
    19,   384,    73,    20,   125,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    71,    71,    71,   140,
    69,    21,     6,    15,     5,    29,     6,     6,     6,    30,
   444,     2,   153,     8,    69,   504,   157,   158,   159,     6,
    31,     6,    69,    69,    15,    70,    70,   498,    71,    34,
    21,    69,     6,    69,    22,   176,    69,    71,   179,    71,
    69,    14,   183,    69,   447,     4,    71,    38,     5,    72,
    72,   192,    36,   417,   457,   667,    72,    72,    71,     4,
    77,    22,    72,    54,     6,   206,    49,    32,     6,    71,
     9,   212,     7,    45,    77,     9,    44,     9,   751,     3,
   692,   693,    73,   695,    77,    74,    72,     6,    70,   453,
     8,    71,    73,   234,   235,   236,    75,   749,   239,    35,
    91,   242,    33,    72,    74,     3,     3,    74,    71,    71,
   101,    75,    72,    71,     8,     7,     6,    73,    73,    73,
   111,   112,   113,   264,    71,    73,   298,    70,   747,    28,
    71,    71,   123,     6,    46,    48,   277,    47,    75,   130,
    73,    71,    70,    70,    73,    71,    71,   288,    70,   290,
    71,    71,    69,   294,   145,    70,    72,    71,   561,    37,
    71,   525,   343,    69,   528,   529,    73,    70,    69,    71,
    70,   162,    37,    43,    23,    71,    71,   349,   319,   320,
   321,    70,    73,    71,   548,   326,   535,    72,    71,    71,
   331,    72,    72,   184,    70,    72,   677,    71,    73,    71,
    71,    27,     6,   344,    73,    73,    69,    71,   199,    71,
    71,    71,    70,     0,   355,    71,     0,   358,   359,   360,
   725,     4,   115,   364,   103,    17,   217,    93,   132,    86,
   577,   596,   389,   462,   399,   376,   585,   364,   147,   269,
   466,   382,   582,   607,   385,   386,   238,   388,   389,   438,
   269,    58,   505,   440,   603,    71,   164,   398,   399,    34,
   463,   402,   437,    -1,    -1,   406,   407,   408,    -1,    -1,
    -1,   674,   413,    -1,   455,    -1,    -1,   458,    -1,   270,
    -1,   422,    -1,   632,   633,   634,   635,    -1,    -1,    -1,
   431,   432,   433,    -1,    -1,   436,    -1,    -1,    -1,   440,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   456,    -1,   308,    -1,    -1,
    -1,    -1,    -1,   314,    -1,    -1,   467,    -1,    -1,   510,
   471,    -1,    -1,    -1,   475,    -1,    -1,    -1,    -1,    -1,
    -1,   482,   333,    -1,   485,   486,    -1,    -1,    -1,    -1,
   753,    19,   716,    -1,    22,    -1,    -1,    -1,   499,    -1,
    -1,    -1,    -1,    -1,   505,    -1,    -1,    -1,   509,    -1,
    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   521,    -1,    -1,   524,    -1,    -1,    -1,    55,    -1,    -1,
    -1,    -1,    -1,    -1,   758,    -1,   537,    -1,   738,    -1,
   581,    -1,   543,    -1,    -1,    -1,    -1,    75,    -1,    -1,
    -1,   552,    -1,   554,    -1,    -1,   557,    -1,    -1,   560,
    88,    -1,    -1,   414,    -1,    -1,    -1,    95,   419,   610,
    -1,    99,   573,   574,    -1,    -1,    -1,   105,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
    -1,   592,   593,    -1,    -1,    -1,   124,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   134,    -1,   609,    -1,
   611,   612,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   149,    -1,    -1,    -1,    -1,   154,   155,    -1,   630,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   678,   166,    -1,
   641,    -1,    -1,   644,   172,   646,    -1,    -1,   649,    -1,
    -1,    -1,   653,    -1,   655,   696,   657,   658,   186,    -1,
   188,   189,   190,    -1,    -1,    -1,    -1,    -1,   519,    -1,
    -1,   672,   200,    -1,    -1,    -1,   204,    -1,    -1,    -1,
   681,   682,   683,    -1,   685,    -1,   214,   688,    -1,    -1,
   218,   219,   220,    -1,   222,    -1,    -1,    -1,   699,   700,
   701,   702,    -1,    -1,   705,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   713,   714,   755,    -1,    -1,    -1,    -1,    -1,
   248,    -1,   250,    -1,   575,    -1,    -1,    -1,   729,    -1,
    -1,    -1,    -1,   734,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   594,   272,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   284,    -1,    -1,    -1,
   761,    -1,    -1,    -1,   615,   616,   617,   618,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   309,    -1,    -1,    -1,   313,    -1,    -1,   316,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   335,    -1,    -1,
    -1,   662,    -1,    -1,    -1,    -1,    -1,    -1,   346,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   378,    -1,    -1,    -1,    -1,   383,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   724,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   416,    -1,
    -1,    -1,   420,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   487,
    -1,    -1,   490,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   520,    -1,    -1,    -1,    -1,    -1,    -1,   527,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   559,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   571,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   579,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   589,   590,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   598,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   619,    -1,    -1,    -1,    -1,    -1,    -1,   626,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   651,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   660,    -1,    -1,    -1,   664,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   679,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   689,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   703,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   727,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   737,
    -1,   739,    -1,    -1,   742,    -1,    -1,   745,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   756
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
#line 519 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 529 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 533 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 539 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 543 "parser-sming.y"
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
#line 558 "parser-sming.y"
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
#line 572 "parser-sming.y"
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
#line 595 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 9:
#line 602 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 10:
#line 609 "parser-sming.y"
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
case 11:
#line 619 "parser-sming.y"
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
case 12:
#line 636 "parser-sming.y"
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
case 13:
#line 688 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 14:
#line 692 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed extension statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 15:
#line 702 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 16:
#line 707 "parser-sming.y"
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
case 17:
#line 722 "parser-sming.y"
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
case 18:
#line 736 "parser-sming.y"
{
			    macroIdentifier = yyvsp[0].text;
			    macroPtr = addMacro(macroIdentifier,
						thisParserPtr->character,
						0,
						thisParserPtr);
			    setMacroDecl(macroPtr, SMI_DECL_EXTENSION);
			;
    break;}
case 19:
#line 746 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].status) {
				setMacroStatus(macroPtr, yyvsp[0].status);
			    }
			;
    break;}
case 20:
#line 752 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroDescription(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 21:
#line 758 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroReference(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 22:
#line 764 "parser-sming.y"
{
			;
    break;}
case 23:
#line 767 "parser-sming.y"
{
			    yyval.macroPtr = 0;
			    macroPtr = NULL;
			    free(macroIdentifier);
			;
    break;}
case 24:
#line 774 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 25:
#line 778 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 26:
#line 788 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 27:
#line 793 "parser-sming.y"
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
case 28:
#line 808 "parser-sming.y"
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
case 29:
#line 822 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 30:
#line 827 "parser-sming.y"
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
case 31:
#line 846 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 32:
#line 852 "parser-sming.y"
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
case 33:
#line 862 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 34:
#line 868 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 35:
#line 874 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 36:
#line 880 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 37:
#line 886 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 38:
#line 894 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 39:
#line 898 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 40:
#line 908 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 41:
#line 913 "parser-sming.y"
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
case 42:
#line 928 "parser-sming.y"
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
case 46:
#line 947 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 47:
#line 952 "parser-sming.y"
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
case 48:
#line 966 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 49:
#line 972 "parser-sming.y"
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
case 50:
#line 985 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 51:
#line 991 "parser-sming.y"
{
			    /*
			     * The first node definition is registered
			     * as the module identity node.
			     */
			    if (!thisParserPtr->modulePtr->objectPtr) {
				setModuleIdentityObject(
				    thisParserPtr->modulePtr, nodeObjectPtr);
			    }
			    
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 52:
#line 1008 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 53:
#line 1013 "parser-sming.y"
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
case 54:
#line 1026 "parser-sming.y"
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
case 55:
#line 1041 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 56:
#line 1047 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 57:
#line 1053 "parser-sming.y"
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
case 58:
#line 1063 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 1069 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 60:
#line 1075 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 61:
#line 1081 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 62:
#line 1087 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 63:
#line 1096 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 64:
#line 1101 "parser-sming.y"
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
case 65:
#line 1116 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 66:
#line 1122 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 67:
#line 1128 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 68:
#line 1135 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 69:
#line 1143 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 70:
#line 1148 "parser-sming.y"
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
case 71:
#line 1163 "parser-sming.y"
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
case 72:
#line 1187 "parser-sming.y"
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
case 73:
#line 1198 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 74:
#line 1204 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 75:
#line 1210 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 76:
#line 1217 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 77:
#line 1225 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 78:
#line 1233 "parser-sming.y"
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
case 79:
#line 1248 "parser-sming.y"
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
case 80:
#line 1262 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 81:
#line 1267 "parser-sming.y"
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
case 82:
#line 1280 "parser-sming.y"
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
case 83:
#line 1295 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 84:
#line 1301 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 85:
#line 1307 "parser-sming.y"
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
case 86:
#line 1317 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 87:
#line 1323 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 88:
#line 1329 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 89:
#line 1335 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 90:
#line 1341 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 91:
#line 1350 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 92:
#line 1354 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 93:
#line 1364 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 94:
#line 1369 "parser-sming.y"
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
case 95:
#line 1384 "parser-sming.y"
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
case 96:
#line 1398 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 97:
#line 1403 "parser-sming.y"
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
case 98:
#line 1417 "parser-sming.y"
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
case 99:
#line 1433 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 100:
#line 1439 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 101:
#line 1446 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 102:
#line 1452 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 103:
#line 1460 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 104:
#line 1464 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 105:
#line 1474 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 106:
#line 1478 "parser-sming.y"
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
case 107:
#line 1493 "parser-sming.y"
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
case 108:
#line 1507 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 109:
#line 1512 "parser-sming.y"
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
case 110:
#line 1524 "parser-sming.y"
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
case 111:
#line 1540 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 112:
#line 1546 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 113:
#line 1552 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 114:
#line 1558 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 115:
#line 1566 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 116:
#line 1570 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 117:
#line 1580 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 118:
#line 1585 "parser-sming.y"
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
case 119:
#line 1600 "parser-sming.y"
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
case 120:
#line 1614 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 121:
#line 1619 "parser-sming.y"
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
case 122:
#line 1633 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 123:
#line 1639 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 124:
#line 1645 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 125:
#line 1651 "parser-sming.y"
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
case 126:
#line 1667 "parser-sming.y"
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
case 127:
#line 1683 "parser-sming.y"
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
case 128:
#line 1723 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 129:
#line 1731 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 130:
#line 1735 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 131:
#line 1741 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 132:
#line 1745 "parser-sming.y"
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
case 133:
#line 1760 "parser-sming.y"
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
case 134:
#line 1774 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 135:
#line 1779 "parser-sming.y"
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
case 136:
#line 1791 "parser-sming.y"
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
case 137:
#line 1806 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 138:
#line 1810 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 139:
#line 1816 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 140:
#line 1820 "parser-sming.y"
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
case 141:
#line 1835 "parser-sming.y"
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
case 142:
#line 1852 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(yyvsp[-6].date, yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 143:
#line 1860 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 144:
#line 1866 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 145:
#line 1870 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 146:
#line 1877 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 147:
#line 1883 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 148:
#line 1887 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 149:
#line 1894 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 155:
#line 1908 "parser-sming.y"
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
case 156:
#line 1928 "parser-sming.y"
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
case 157:
#line 1946 "parser-sming.y"
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
case 158:
#line 1966 "parser-sming.y"
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
case 159:
#line 1984 "parser-sming.y"
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
case 160:
#line 2004 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 161:
#line 2008 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 162:
#line 2014 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 163:
#line 2018 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 164:
#line 2024 "parser-sming.y"
{
			    if (rowObjectPtr) {
				addObjectFlags(rowObjectPtr, FLAG_CREATABLE);
				setObjectCreate(rowObjectPtr, 1);
			    }
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 165:
#line 2034 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 166:
#line 2038 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 167:
#line 2044 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 168:
#line 2050 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 169:
#line 2056 "parser-sming.y"
{
			    yyval.date = yyvsp[-2].date;
			;
    break;}
case 170:
#line 2062 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 171:
#line 2068 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 172:
#line 2074 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 173:
#line 2078 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 174:
#line 2084 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 175:
#line 2090 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 176:
#line 2094 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 177:
#line 2100 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 178:
#line 2106 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 179:
#line 2110 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 180:
#line 2116 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 181:
#line 2122 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 182:
#line 2126 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 183:
#line 2132 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 184:
#line 2138 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 185:
#line 2142 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 186:
#line 2148 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 187:
#line 2154 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 188:
#line 2158 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 189:
#line 2164 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 190:
#line 2170 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 191:
#line 2174 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 192:
#line 2180 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 193:
#line 2186 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 194:
#line 2190 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 195:
#line 2196 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 196:
#line 2203 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 197:
#line 2209 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 198:
#line 2213 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 199:
#line 2220 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 200:
#line 2226 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 201:
#line 2230 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 202:
#line 2237 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 203:
#line 2243 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 204:
#line 2247 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 205:
#line 2253 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 206:
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
case 207:
#line 2273 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 208:
#line 2282 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text,
						       thisParserPtr,
						       thisModulePtr);
			    yyval.optionPtr->export.description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 209:
#line 2292 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 210:
#line 2296 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 211:
#line 2302 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 212:
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
case 213:
#line 2321 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 214:
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
case 215:
#line 2357 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 216:
#line 2361 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 217:
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
case 218:
#line 2382 "parser-sming.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 219:
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
case 220:
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
case 221:
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
case 222:
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
case 223:
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
case 224:
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
case 225:
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
case 226:
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
case 227:
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
case 228:
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
case 229:
#line 2537 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 230:
#line 2541 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 231:
#line 2547 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 232:
#line 2551 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 233:
#line 2557 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 234:
#line 2561 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 235:
#line 2568 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 236:
#line 2576 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 237:
#line 2580 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 238:
#line 2586 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 239:
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
case 240:
#line 2605 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 241:
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
case 242:
#line 2625 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 243:
#line 2629 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 244:
#line 2635 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 245:
#line 2641 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 246:
#line 2645 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 247:
#line 2652 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 248:
#line 2660 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 249:
#line 2664 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 250:
#line 2670 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 251:
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
case 252:
#line 2689 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 253:
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
case 254:
#line 2714 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 255:
#line 2718 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 256:
#line 2724 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 257:
#line 2730 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 258:
#line 2737 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 259:
#line 2745 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 260:
#line 2749 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 261:
#line 2755 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 262:
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
case 263:
#line 2776 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 264:
#line 2782 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-6].text;
			    yyval.namedNumberPtr->export.value = *yyvsp[-2].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			;
    break;}
case 265:
#line 2791 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 266:
#line 2799 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 267:
#line 2803 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 268:
#line 2809 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 269:
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
case 270:
#line 2828 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 271:
#line 2834 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 272:
#line 2842 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 273:
#line 2846 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 274:
#line 2852 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 275:
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
case 276:
#line 2871 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 277:
#line 2877 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 278:
#line 2885 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 279:
#line 2889 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 280:
#line 2895 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 281:
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
case 282:
#line 2914 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 283:
#line 2920 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 284:
#line 2926 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 285:
#line 2930 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 286:
#line 2938 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 287:
#line 2942 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 288:
#line 2948 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 289:
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
case 290:
#line 2967 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 291:
#line 2973 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 292:
#line 2977 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 2983 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 294:
#line 2987 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 295:
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
case 296:
#line 3004 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 297:
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
case 298:
#line 3021 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 299:
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
case 300:
#line 3041 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 301:
#line 3045 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 302:
#line 3051 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 303:
#line 3055 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 304:
#line 3065 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 305:
#line 3071 "parser-sming.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 306:
#line 3077 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 307:
#line 3083 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 308:
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
case 309:
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
case 310:
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
case 311:
#line 3191 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 312:
#line 3197 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 313:
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
case 314:
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
case 315:
#line 3240 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 316:
#line 3245 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 317:
#line 3252 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 318:
#line 3256 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 319:
#line 3260 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 320:
#line 3266 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 321:
#line 3270 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 322:
#line 3274 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 323:
#line 3278 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 324:
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
case 325:
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
case 326:
#line 3351 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 327:
#line 3357 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 328:
#line 3361 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 329:
#line 3368 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 330:
#line 3372 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 331:
#line 3382 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 332:
#line 3391 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 333:
#line 3397 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 0);
			;
    break;}
case 334:
#line 3405 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 335:
#line 3414 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 336:
#line 3423 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 337:
#line 3427 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 338:
#line 3437 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 339:
#line 3444 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 340:
#line 3450 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 341:
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
			
