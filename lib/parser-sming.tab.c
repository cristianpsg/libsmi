
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
#define	identityKeyword	269
#define	oidKeyword	270
#define	dateKeyword	271
#define	organizationKeyword	272
#define	contactKeyword	273
#define	descriptionKeyword	274
#define	referenceKeyword	275
#define	extensionKeyword	276
#define	typedefKeyword	277
#define	typeKeyword	278
#define	writetypeKeyword	279
#define	nodeKeyword	280
#define	scalarKeyword	281
#define	tableKeyword	282
#define	columnKeyword	283
#define	rowKeyword	284
#define	notificationKeyword	285
#define	groupKeyword	286
#define	complianceKeyword	287
#define	formatKeyword	288
#define	unitsKeyword	289
#define	statusKeyword	290
#define	accessKeyword	291
#define	defaultKeyword	292
#define	impliedKeyword	293
#define	indexKeyword	294
#define	augmentsKeyword	295
#define	reordersKeyword	296
#define	sparseKeyword	297
#define	expandsKeyword	298
#define	createKeyword	299
#define	membersKeyword	300
#define	objectsKeyword	301
#define	mandatoryKeyword	302
#define	optionalKeyword	303
#define	refineKeyword	304
#define	abnfKeyword	305
#define	OctetStringKeyword	306
#define	ObjectIdentifierKeyword	307
#define	Integer32Keyword	308
#define	Unsigned32Keyword	309
#define	Integer64Keyword	310
#define	Unsigned64Keyword	311
#define	Float32Keyword	312
#define	Float64Keyword	313
#define	Float128Keyword	314
#define	BitsKeyword	315
#define	EnumerationKeyword	316
#define	currentKeyword	317
#define	deprecatedKeyword	318
#define	obsoleteKeyword	319
#define	noaccessKeyword	320
#define	notifyonlyKeyword	321
#define	readonlyKeyword	322
#define	readwriteKeyword	323
#define	readcreateKeyword	324

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
#define	YYNTBASE	79

#define YYTRANSLATE(x) ((unsigned)(x) <= 324 ? yytranslate[x] : 315)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    73,
    74,     2,     2,    76,    77,    78,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    72,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    70,    75,    71,     2,     2,     2,     2,     2,
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
    66,    67,    68,    69
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     4,     6,     8,    11,    14,    15,    16,    17,
    18,    19,    50,    51,    53,    55,    58,    61,    62,    63,
    64,    65,    66,    85,    86,    88,    90,    93,    96,    97,
    98,    99,   100,   101,   102,   103,   104,   130,   131,   133,
   135,   138,   141,   143,   145,   147,   148,   149,   150,   151,
   152,   172,   173,   174,   175,   176,   177,   178,   179,   180,
   181,   182,   215,   216,   217,   218,   219,   220,   243,   244,
   245,   246,   247,   248,   249,   250,   277,   279,   282,   285,
   286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
   328,   329,   331,   333,   336,   339,   340,   341,   342,   343,
   344,   345,   368,   369,   371,   373,   376,   379,   380,   381,
   382,   383,   384,   385,   409,   410,   412,   414,   417,   420,
   421,   422,   423,   424,   425,   426,   427,   428,   455,   456,
   458,   460,   463,   466,   467,   468,   482,   483,   485,   487,
   490,   493,   505,   506,   509,   515,   521,   522,   525,   531,
   532,   535,   541,   543,   545,   547,   549,   551,   562,   568,
   581,   587,   600,   601,   604,   605,   608,   612,   618,   624,
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

static const short yyrhs[] = {   313,
    80,     0,     0,    81,     0,    82,     0,    81,    82,     0,
    83,   313,     0,     0,     0,     0,     0,     0,    11,   312,
     5,    84,   313,    70,   314,   190,   218,   314,    85,   219,
   314,    86,   231,   314,    87,   232,    88,   196,   200,    89,
    98,   110,   158,   168,   178,    71,   313,    72,     0,     0,
    90,     0,    91,     0,    90,    91,     0,    92,   314,     0,
     0,     0,     0,     0,     0,    21,   312,     6,    93,   313,
    70,   314,   224,    94,   230,    95,   232,    96,   234,    97,
    71,   313,    72,     0,     0,    99,     0,   100,     0,    99,
   100,     0,   101,   314,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    22,   312,     5,   102,   313,    70,   314,
   202,   314,   103,   228,   104,   220,   105,   222,   106,   224,
   107,   230,   108,   232,   109,    71,   313,    72,     0,     0,
   111,     0,   112,     0,   111,   112,     0,   113,   314,     0,
   114,     0,   120,     0,   131,     0,     0,     0,     0,     0,
     0,    25,   312,     6,   115,   313,    70,   314,   216,   314,
   116,   224,   117,   230,   118,   232,   119,    71,   313,    72,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    26,   312,     6,   121,   313,    70,   314,   216,   314,
   122,   204,   314,   123,   227,   314,   124,   228,   125,   220,
   126,   222,   127,   224,   128,   231,   314,   129,   232,   130,
    71,   313,    72,     0,     0,     0,     0,     0,     0,    27,
   312,     6,   132,   313,    70,   314,   216,   314,   133,   224,
   134,   231,   314,   135,   232,   136,   137,   314,    71,   313,
    72,     0,     0,     0,     0,     0,     0,     0,     0,    29,
   312,     6,   138,   313,    70,   314,   216,   314,   139,   207,
   314,   140,   214,   141,   224,   142,   231,   314,   143,   232,
   144,   145,    71,   313,    72,     0,   146,     0,   145,   146,
     0,   147,   314,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    28,   312,     6,   148,   313,    70,
   314,   216,   314,   149,   204,   314,   150,   227,   314,   151,
   228,   152,   220,   153,   222,   154,   224,   155,   231,   314,
   156,   232,   157,    71,   313,    72,     0,     0,   159,     0,
   160,     0,   159,   160,     0,   161,   314,     0,     0,     0,
     0,     0,     0,     0,    30,   312,     6,   162,   313,    70,
   314,   216,   314,   163,   237,   164,   224,   165,   231,   314,
   166,   232,   167,    71,   313,    72,     0,     0,   169,     0,
   170,     0,   169,   170,     0,   171,   314,     0,     0,     0,
     0,     0,     0,     0,    31,   312,     6,   172,   313,    70,
   314,   216,   314,   173,   236,   314,   174,   224,   175,   231,
   314,   176,   232,   177,    71,   313,    72,     0,     0,   179,
     0,   180,     0,   179,   180,     0,   181,   314,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    32,   312,     6,
   182,   313,    70,   314,   216,   314,   183,   224,   184,   231,
   314,   185,   232,   186,   239,   187,   241,   188,   245,   189,
    71,   313,    72,     0,     0,   191,     0,   192,     0,   191,
   192,     0,   193,   314,     0,     0,     0,    12,   312,     5,
   194,   313,    73,   313,   276,   195,   313,    74,   313,    72,
     0,     0,   197,     0,   198,     0,   197,   198,     0,   199,
   314,     0,    13,   313,    70,   314,   217,   314,   231,   314,
    71,   313,    72,     0,     0,   201,   314,     0,    14,   312,
     6,   313,    72,     0,    23,   312,   249,   313,    72,     0,
     0,   204,   314,     0,    23,   312,   249,   313,    72,     0,
     0,   206,   314,     0,    24,   312,   249,   313,    72,     0,
   208,     0,   209,     0,   210,     0,   211,     0,   212,     0,
    39,   213,   313,    73,   313,   280,   313,    74,   313,    72,
     0,    40,   312,   291,   313,    72,     0,    41,   312,   291,
   213,   313,    73,   313,   280,   313,    74,   313,    72,     0,
    42,   312,   291,   313,    72,     0,    43,   312,   291,   213,
   313,    73,   313,   280,   313,    74,   313,    72,     0,     0,
   312,    38,     0,     0,   215,   314,     0,    44,   313,    72,
     0,    15,   312,   302,   313,    72,     0,    16,   312,   296,
   313,    72,     0,    17,   312,   292,   313,    72,     0,    18,
   312,   292,   313,    72,     0,     0,   221,   314,     0,    33,
   312,   297,   313,    72,     0,     0,   223,   314,     0,    34,
   312,   298,   313,    72,     0,     0,   225,   314,     0,    35,
   312,   300,   313,    72,     0,     0,   227,   314,     0,    36,
   312,   301,   313,    72,     0,     0,   229,   314,     0,    37,
   312,   299,   313,    72,     0,     0,   231,   314,     0,    19,
   312,   292,   313,    72,     0,     0,   233,   314,     0,    20,
   312,   292,   313,    72,     0,     0,   235,   314,     0,    50,
   312,   292,   313,    72,     0,    45,   313,    73,   313,   280,
   313,    74,   313,    72,     0,     0,   238,   314,     0,    46,
   313,    73,   313,   280,   313,    74,   313,    72,     0,     0,
   240,   314,     0,    47,   313,    73,   313,   280,   313,    74,
   313,    72,     0,     0,   242,     0,   243,     0,   242,   243,
     0,   244,   314,     0,    48,   312,   291,   313,    70,   231,
   314,    71,   313,    72,     0,     0,   246,     0,   247,     0,
   246,   247,     0,   248,   314,     0,    49,   312,   291,   313,
    70,   203,   205,   226,   231,   314,    71,   313,    72,     0,
   250,     0,   251,     0,    51,   254,     0,    52,     0,    53,
   254,     0,    54,   254,     0,    55,   254,     0,    56,   254,
     0,    57,   262,     0,    58,   262,     0,    59,   262,     0,
    61,   270,     0,    60,   270,     0,   290,   252,     0,     0,
   313,   253,     0,   255,     0,   263,     0,     0,   313,   255,
     0,    73,   313,   259,   256,   313,    74,     0,     0,   257,
     0,   258,     0,   257,   258,     0,   313,    75,   313,   259,
     0,   310,   260,     0,     0,   261,     0,   313,     3,   313,
   310,     0,     0,   313,   263,     0,    73,   313,   267,   264,
   313,    74,     0,     0,   265,     0,   266,     0,   265,   266,
     0,   313,    75,   313,   267,     0,     7,   268,     0,     0,
   269,     0,   313,     3,   313,     7,     0,    73,   313,   271,
   313,    74,     0,   275,   272,   311,     0,     0,   273,     0,
   274,     0,   273,   274,     0,   313,    76,   313,   275,     0,
     6,   313,    73,   313,   308,   313,    74,     0,   289,   277,
   311,     0,     0,   278,     0,   279,     0,   278,   279,     0,
   313,    76,   313,   289,     0,     0,     0,   311,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   313,    76,   313,
     0,     0,   291,   281,   311,     0,     0,   282,     0,   283,
     0,   282,   283,     0,   313,    76,   313,   291,     0,    73,
   313,   285,   313,    74,     0,   311,     0,     6,   286,   311,
     0,     0,   287,     0,   288,     0,   287,   288,     0,   313,
    76,   313,     6,     0,     5,     0,     6,     0,   290,     0,
   291,     0,     5,     4,     5,     0,     5,     0,     5,     4,
     6,     0,     6,     0,     8,   293,     0,     0,   294,     0,
   295,     0,   294,   295,     0,   313,     8,     0,     8,     0,
     8,     0,     8,     0,   284,     0,     9,     0,    77,     9,
     0,    10,     0,     7,     0,   292,     0,   291,     0,   291,
   305,     0,   307,    78,   307,   305,     0,    62,     0,    63,
     0,    64,     0,    65,     0,    66,     0,    67,     0,    68,
     0,   303,   304,     0,   291,     0,   307,     0,     0,   305,
     0,   306,     0,   305,   306,     0,    78,   307,     0,     9,
     0,    10,     0,     9,     0,    77,     9,     0,   308,     0,
   309,     0,     0,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   519,   529,   533,   539,   543,   558,   572,   601,   610,   617,
   628,   638,   734,   738,   748,   752,   768,   782,   791,   799,
   805,   811,   813,   820,   824,   834,   838,   854,   868,   872,
   893,   899,   909,   915,   921,   927,   932,   940,   944,   954,
   958,   974,   988,   989,   990,   993,   997,  1013,  1019,  1032,
  1037,  1053,  1057,  1071,  1086,  1093,  1099,  1109,  1115,  1120,
  1127,  1132,  1141,  1145,  1162,  1167,  1174,  1179,  1188,  1192,
  1208,  1233,  1243,  1248,  1255,  1261,  1269,  1277,  1292,  1306,
  1310,  1324,  1339,  1346,  1352,  1362,  1368,  1373,  1380,  1385,
  1394,  1398,  1408,  1412,  1428,  1442,  1446,  1462,  1478,  1483,
  1491,  1496,  1504,  1508,  1518,  1522,  1537,  1551,  1555,  1568,
  1585,  1590,  1597,  1602,  1610,  1614,  1624,  1628,  1644,  1658,
  1662,  1678,  1683,  1690,  1696,  1712,  1728,  1767,  1775,  1779,
  1785,  1789,  1804,  1818,  1822,  1835,  1850,  1854,  1860,  1864,
  1879,  1893,  1902,  1906,  1912,  1919,  1927,  1931,  1937,  1944,
  1948,  1954,  1961,  1962,  1963,  1964,  1965,  1968,  1989,  2005,
  2027,  2043,  2065,  2069,  2075,  2079,  2085,  2091,  2097,  2103,
  2109,  2115,  2119,  2125,  2131,  2135,  2141,  2147,  2151,  2157,
  2163,  2167,  2173,  2179,  2183,  2189,  2195,  2199,  2205,  2211,
  2215,  2221,  2227,  2231,  2237,  2243,  2250,  2254,  2260,  2267,
  2271,  2277,  2284,  2288,  2294,  2300,  2314,  2320,  2333,  2337,
  2343,  2349,  2362,  2368,  2398,  2402,  2408,  2423,  2427,  2442,
  2457,  2472,  2487,  2502,  2517,  2532,  2547,  2564,  2578,  2582,
  2588,  2592,  2598,  2602,  2608,  2617,  2621,  2627,  2633,  2646,
  2652,  2666,  2670,  2676,  2682,  2686,  2692,  2701,  2705,  2711,
  2717,  2730,  2736,  2755,  2759,  2765,  2771,  2777,  2786,  2790,
  2796,  2802,  2817,  2823,  2832,  2840,  2844,  2850,  2856,  2869,
  2875,  2883,  2887,  2893,  2899,  2912,  2918,  2926,  2930,  2936,
  2942,  2955,  2961,  2967,  2971,  2979,  2983,  2989,  2995,  3008,
  3014,  3018,  3024,  3028,  3034,  3045,  3051,  3062,  3068,  3082,
  3086,  3092,  3096,  3106,  3112,  3118,  3124,  3137,  3164,  3210,
  3232,  3238,  3244,  3257,  3281,  3286,  3293,  3297,  3301,  3307,
  3311,  3315,  3319,  3325,  3354,  3392,  3398,  3402,  3409,  3413,
  3423,  3432,  3438,  3446,  3455,  3464,  3468,  3478,  3485,  3491,
  3497
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DOT_DOT",
"COLON_COLON","ucIdentifier","lcIdentifier","floatValue","textSegment","decimalNumber",
"hexadecimalNumber","moduleKeyword","importKeyword","revisionKeyword","identityKeyword",
"oidKeyword","dateKeyword","organizationKeyword","contactKeyword","descriptionKeyword",
"referenceKeyword","extensionKeyword","typedefKeyword","typeKeyword","writetypeKeyword",
"nodeKeyword","scalarKeyword","tableKeyword","columnKeyword","rowKeyword","notificationKeyword",
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
"revisionStatement","identityStatement_stmtsep_01","identityStatement","typedefTypeStatement",
"typeStatement_stmtsep_01","typeStatement","writetypeStatement_stmtsep_01","writetypeStatement",
"anyIndexStatement","indexStatement","augmentsStatement","reordersStatement",
"sparseStatement","expandsStatement","sep_impliedKeyword_01","createStatement_stmtsep_01",
"createStatement","oidStatement","dateStatement","organizationStatement","contactStatement",
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
    79,    80,    80,    81,    81,    82,    84,    85,    86,    87,
    88,    83,    89,    89,    90,    90,    91,    93,    94,    95,
    96,    97,    92,    98,    98,    99,    99,   100,   102,   103,
   104,   105,   106,   107,   108,   109,   101,   110,   110,   111,
   111,   112,   113,   113,   113,   115,   116,   117,   118,   119,
   114,   121,   122,   123,   124,   125,   126,   127,   128,   129,
   130,   120,   132,   133,   134,   135,   136,   131,   138,   139,
   140,   141,   142,   143,   144,   137,   145,   145,   146,   148,
   149,   150,   151,   152,   153,   154,   155,   156,   157,   147,
   158,   158,   159,   159,   160,   162,   163,   164,   165,   166,
   167,   161,   168,   168,   169,   169,   170,   172,   173,   174,
   175,   176,   177,   171,   178,   178,   179,   179,   180,   182,
   183,   184,   185,   186,   187,   188,   189,   181,   190,   190,
   191,   191,   192,   194,   195,   193,   196,   196,   197,   197,
   198,   199,   200,   200,   201,   202,   203,   203,   204,   205,
   205,   206,   207,   207,   207,   207,   207,   208,   209,   210,
   211,   212,   213,   213,   214,   214,   215,   216,   217,   218,
   219,   220,   220,   221,   222,   222,   223,   224,   224,   225,
   226,   226,   227,   228,   228,   229,   230,   230,   231,   232,
   232,   233,   234,   234,   235,   236,   237,   237,   238,   239,
   239,   240,   241,   241,   242,   242,   243,   244,   245,   245,
   246,   246,   247,   248,   249,   249,   250,   250,   250,   250,
   250,   250,   250,   250,   250,   250,   250,   251,   252,   252,
   253,   253,   254,   254,   255,   256,   256,   257,   257,   258,
   259,   260,   260,   261,   262,   262,   263,   264,   264,   265,
   265,   266,   267,   268,   268,   269,   270,   271,   272,   272,
   273,   273,   274,   275,   276,   277,   277,   278,   278,   279,
    -1,    -1,    -1,    -1,    -1,    -1,   280,   281,   281,   282,
   282,   283,   284,   285,   285,   286,   286,   287,   287,   288,
   289,   289,    -1,    -1,   290,   290,   291,   291,   292,   293,
   293,   294,   294,   295,   296,   297,   298,   299,   299,   299,
   299,   299,   299,   299,   299,   299,   300,   300,   300,   301,
   301,   301,   301,   302,   303,   303,   304,   304,   305,   305,
   306,   307,   308,   308,   309,   310,   310,   311,   312,   313,
   314
};

static const short yyr2[] = {     0,
     2,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,    30,     0,     1,     1,     2,     2,     0,     0,     0,
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
     2,    11,     0,     2,     5,     5,     0,     2,     5,     0,
     2,     5,     1,     1,     1,     1,     1,    10,     5,    12,
     5,    12,     0,     2,     0,     2,     3,     5,     5,     5,
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
   191,   136,   340,   340,   143,   138,   139,   341,     0,     0,
   339,    13,   341,   140,   141,   192,   341,     0,   339,    24,
    14,    15,   341,   144,     0,   340,     0,   339,    38,    25,
    26,   341,    16,    17,   339,   341,     0,    18,     0,   339,
   339,   339,    91,    39,    40,   341,    43,    44,    45,    27,
    28,     0,     0,   145,   340,    29,     0,     0,     0,   339,
   103,    92,    93,   341,    41,    42,   305,   340,   341,     0,
   340,    46,    52,    63,     0,   339,   115,   104,   105,   341,
    94,    95,     0,     0,   341,     0,   340,   340,   340,    96,
     0,   339,     0,   116,   117,   341,   106,   107,   169,   340,
   178,   341,     0,     0,     0,   340,   108,     0,   340,   118,
   119,     0,   339,    19,   341,     0,   341,   341,   341,     0,
   340,   120,     0,   142,     0,   187,   179,   339,   341,     0,
     0,     0,   341,     0,   340,    12,   317,   318,   319,   340,
    20,   341,     0,    30,   339,   341,   341,   341,     0,   341,
     0,     0,   190,   188,   296,   233,   218,   233,   233,   233,
   233,   245,   245,   245,     0,     0,   340,   215,   216,   229,
   184,     0,    47,    53,    64,   341,     0,   341,   180,    21,
     0,   217,     0,   219,   220,   221,   222,   223,     0,   224,
   225,   340,   227,   226,     0,   228,     0,   339,    31,   341,
     0,   298,   332,   325,   340,   327,   326,   178,     0,   178,
    97,   341,     0,   193,   295,   340,   234,   340,   246,     0,
   146,   340,   230,   231,   232,     0,   172,   185,     0,     0,
     0,   324,   328,   329,    48,   339,   341,    65,   197,   109,
   341,   339,    22,   341,     0,     0,   340,   340,   259,     0,
   312,   309,   311,   340,     0,   308,   314,   313,   340,     0,
   339,    32,   341,   297,   168,   331,   330,   187,     0,    54,
     0,   340,    98,   341,     0,   121,     0,     0,   194,   334,
   333,     0,   236,   336,   337,   242,   254,   248,     0,     0,
   338,   260,   261,     0,   338,   310,   315,     0,     0,     0,
   175,   173,    49,   340,     0,   341,     0,   178,   198,   340,
   341,   178,   340,   340,   335,   340,   237,   238,     0,   241,
   243,     0,   253,   255,     0,   340,   249,   250,     0,   340,
   257,   258,   262,   340,   286,   340,   284,   186,     0,   306,
   340,   339,    33,   341,   190,     0,   339,   341,    66,   340,
    99,     0,   110,   122,     0,     0,     0,   239,   340,   340,
   340,     0,   251,   340,     0,     0,   338,   287,   288,     0,
     0,   316,     0,     0,   178,   176,    50,   149,     0,    55,
   190,     0,     0,   340,   178,     0,   195,    23,   235,     0,
     0,     0,   247,     0,   340,   263,   285,   289,   340,   283,
   174,   307,   340,    34,     0,   320,   321,   322,   323,   340,
   184,    67,   340,   278,   341,     0,   111,   341,   240,   244,
   256,   252,     0,     0,     0,   187,   340,     0,    56,     0,
     0,   338,   279,   280,     0,   100,   340,     0,   123,   264,
   290,   177,    35,     0,   183,   172,   339,   341,   340,   277,
   281,   340,   190,     0,   341,   190,   190,    51,    57,     0,
     0,     0,     0,   101,   340,   112,   124,    36,   175,    69,
   340,   199,   282,     0,     0,   190,   200,     0,    58,   340,
     0,   340,   196,   113,   340,   125,   341,   340,   178,     0,
    68,     0,     0,     0,   203,   201,     0,    59,   341,   102,
   340,   340,   339,   126,   204,   205,   341,    37,     0,     0,
     0,     0,     0,   209,   206,   207,   341,   341,   114,   340,
   340,   339,   127,   210,   211,   341,    60,    70,     0,     0,
     0,     0,   212,   213,   190,     0,   340,     0,   340,   340,
    61,   163,   339,   339,   339,   339,   341,   153,   154,   155,
   156,   157,     0,   341,     0,     0,     0,   340,     0,     0,
     0,     0,     0,    71,   202,     0,   147,   128,   340,     0,
   164,   340,   163,   340,   163,   165,   340,   150,   341,     0,
   340,     0,   340,     0,   340,   340,    72,   341,     0,   339,
   181,   341,   148,    62,     0,   159,     0,   161,     0,     0,
   178,   166,   208,     0,     0,   341,   151,   340,   340,   340,
   167,    73,   340,   341,   182,     0,     0,     0,     0,     0,
     0,   340,   340,   340,   341,   152,   340,     0,     0,     0,
    74,     0,   158,   340,   340,   190,   214,     0,     0,    75,
   160,   162,     0,   339,     0,    77,   341,     0,   340,    78,
    79,    80,     0,   340,    76,     0,   341,     0,   341,    81,
     0,   341,    82,     0,   341,    83,   184,    84,   172,    85,
   175,    86,   178,    87,     0,   341,    88,   190,    89,     0,
   340,     0,    90,     0,     0,     0
};

static const short yydefgoto[] = {   764,
     3,     4,     5,     6,    11,    31,    48,    69,    80,   100,
   101,   102,   103,   135,   206,   233,   294,   358,   109,   110,
   111,   112,   151,   251,   307,   381,   455,   506,   537,   558,
   123,   124,   125,   126,   127,   167,   288,   348,   425,   485,
   128,   168,   289,   385,   491,   526,   549,   569,   589,   615,
   637,   129,   169,   290,   351,   461,   510,   528,   560,   616,
   656,   681,   699,   716,   723,   725,   726,   727,   734,   741,
   744,   747,   749,   751,   753,   755,   758,   760,   141,   142,
   143,   144,   186,   319,   388,   463,   533,   554,   157,   158,
   159,   160,   201,   355,   465,   518,   556,   573,   173,   174,
   175,   176,   215,   392,   466,   536,   557,   575,   594,   612,
    16,    17,    18,    19,    28,    56,    85,    86,    87,    88,
    92,    93,   209,   658,   317,   671,   672,   627,   628,   629,
   630,   631,   632,   638,   667,   668,   226,   116,    22,    39,
   342,   343,   423,   424,   194,   195,   685,   428,   279,   280,
   221,   222,    74,    75,   323,   324,   391,   353,   354,   566,
   567,   584,   585,   586,   587,   603,   604,   605,   606,   247,
   248,   249,   276,   303,   262,   297,   396,   397,   398,   363,
   400,   401,   268,   299,   406,   407,   408,   368,   403,   404,
   273,   328,   371,   372,   373,   329,    51,    57,    58,    59,
   493,   512,   513,   514,   336,   416,   447,   448,   449,    52,
   250,   494,    30,    33,    34,    35,   148,   421,   483,   339,
   220,   490,   285,   286,   312,   313,   314,   287,   364,   365,
   366,    65,   639,   263,    14
};

static const short yypact[] = {-32768,
    24,-32768,-32768,    24,-32768,-32768,    18,-32768,-32768,-32768,
-32768,   -29,-32768,    31,-32768,    38,    31,-32768,-32768,    43,
-32768,-32768,-32768,-32768,-32768,    57,-32768,-32768,    96,-32768,
    28,    37,-32768,    96,-32768,   105,    25,-32768,-32768,-32768,
-32768,-32768,-32768,    57,-32768,    71,-32768,    95,-32768,-32768,
-32768,    39,    45,-32768,-32768,-32768,-32768,    39,-32768,    44,
-32768,    57,-32768,    47,-32768,-32768,-32768,-32768,    99,-32768,
    71,    50,-32768,-32768,-32768,    51,-32768,-32768,    57,   111,
-32768,-32768,-32768,-32768,   112,   111,-32768,-32768,    53,    58,
-32768,   108,-32768,-32768,-32768,-32768,-32768,   124,-32768,   110,
   108,-32768,-32768,-32768,   117,-32768,   128,-32768,    12,   110,
-32768,-32768,-32768,-32768,-32768,-32768,    75,-32768,   144,-32768,
-32768,-32768,   120,    12,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   146,    95,-32768,-32768,-32768,   149,   150,   151,-32768,
   121,   120,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    88,
-32768,-32768,-32768,-32768,   153,-32768,   130,   121,-32768,-32768,
-32768,-32768,    91,    93,-32768,    90,-32768,-32768,-32768,-32768,
   159,-32768,   100,   130,-32768,-32768,-32768,-32768,-32768,-32768,
   131,-32768,   103,   106,   107,-32768,-32768,   164,-32768,-32768,
-32768,   109,-32768,-32768,-32768,   152,-32768,-32768,-32768,   113,
-32768,-32768,   115,-32768,    29,    95,-32768,-32768,-32768,   163,
   163,   163,-32768,   114,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    85,-32768,-32768,-32768,-32768,-32768,   163,-32768,
   118,   119,    99,-32768,   175,   122,-32768,   122,   122,   122,
   122,   122,   122,   122,   123,   123,-32768,-32768,-32768,   122,
   148,    48,-32768,-32768,-32768,-32768,   163,-32768,-32768,-32768,
   185,-32768,   125,-32768,-32768,-32768,-32768,-32768,   126,-32768,
-32768,-32768,-32768,-32768,   132,-32768,   127,-32768,-32768,-32768,
   188,-32768,-32768,-32768,-32768,   129,-32768,   131,   170,   131,
-32768,-32768,   163,   147,-32768,-32768,-32768,-32768,-32768,   196,
-32768,-32768,-32768,-32768,-32768,    54,   173,-32768,   197,   136,
   200,-32768,   129,-32768,-32768,-32768,-32768,-32768,   165,-32768,
-32768,-32768,-32768,-32768,    41,   203,-32768,-32768,    39,    35,
-32768,   135,-32768,-32768,   205,-32768,   129,-32768,-32768,   140,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    95,    85,-32768,
    95,-32768,-32768,-32768,   174,-32768,    57,   154,-32768,-32768,
-32768,   212,   155,-32768,-32768,   219,   219,   155,   156,   157,
-32768,    39,-32768,   158,   217,-32768,   129,   161,   200,   216,
   192,-32768,-32768,-32768,   191,-32768,   162,   131,-32768,-32768,
-32768,   131,-32768,-32768,-32768,-32768,   155,-32768,   171,-32768,
-32768,   225,-32768,-32768,   233,-32768,   155,-32768,   176,-32768,
-32768,-32768,-32768,-32768,    39,-32768,-32768,-32768,   129,-32768,
-32768,-32768,-32768,-32768,    99,   166,-32768,-32768,-32768,-32768,
-32768,   167,-32768,-32768,   177,   180,   179,-32768,-32768,-32768,
-32768,   181,-32768,-32768,    92,   196,-32768,    39,-32768,   169,
   182,   129,   186,   229,   131,-32768,-32768,-32768,    14,-32768,
    99,    89,    95,-32768,   131,    95,-32768,-32768,-32768,    41,
    41,   232,-32768,   203,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   183,-32768,-32768,-32768,-32768,-32768,
   148,-32768,-32768,    39,-32768,    89,-32768,-32768,-32768,-32768,
-32768,-32768,   187,   242,   190,    95,-32768,   193,-32768,   228,
   194,-32768,    39,-32768,   184,-32768,-32768,    95,-32768,-32768,
-32768,-32768,-32768,   195,-32768,   173,-32768,-32768,-32768,-32768,
-32768,-32768,    99,   199,-32768,    99,    99,-32768,-32768,   253,
   198,   202,    89,-32768,-32768,-32768,-32768,-32768,   192,-32768,
-32768,-32768,-32768,   204,   206,    99,   223,   208,-32768,-32768,
   209,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   131,   201,
-32768,   210,   213,   207,   218,-32768,   211,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   218,-32768,-32768,-32768,    95,   163,
   215,    89,    89,   227,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   227,-32768,-32768,-32768,-32768,   214,   220,
    89,   221,-32768,-32768,    99,    32,-32768,    95,-32768,-32768,
-32768,   239,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   222,-32768,   230,   234,   224,-32768,   251,    89,
    89,    89,    89,-32768,-32768,   226,   170,-32768,-32768,   231,
-32768,-32768,   239,-32768,   239,   247,-32768,   269,-32768,   235,
-32768,   236,-32768,   237,-32768,-32768,-32768,-32768,   238,-32768,
   191,-32768,-32768,-32768,    89,-32768,   245,-32768,   246,   240,
   131,-32768,-32768,    85,    95,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   241,    89,    89,    95,   248,
   252,-32768,-32768,-32768,-32768,-32768,-32768,   250,   256,   257,
-32768,   254,-32768,-32768,-32768,    99,-32768,   260,   261,-32768,
-32768,-32768,   271,-32768,    -2,-32768,-32768,   295,-32768,-32768,
-32768,-32768,   263,-32768,-32768,   255,-32768,   163,-32768,-32768,
   170,-32768,-32768,   191,-32768,-32768,   148,-32768,   173,-32768,
   192,-32768,   131,-32768,    95,-32768,-32768,    99,-32768,   267,
-32768,   268,-32768,   305,   311,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   320,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   243,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   244,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   249,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -384,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   262,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   189,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   168,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   326,-32768,-32768,-32768,-32768,-32768,   259,-32768,
-32768,-32768,-32768,-32768,  -625,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -622,-32768,-32768,  -208,-32768,-32768,-32768,
  -517,-32768,  -539,-32768,  -283,-32768,-32768,  -646,  -483,-32768,
  -332,   -30,  -220,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -239,-32768,-32768,-32768,  -256,-32768,  -347,
-32768,-32768,-32768,-32768,  -152,    72,-32768,-32768,   -47,  -119,
-32768,-32768,  -136,    78,-32768,-32768,   -51,  -117,-32768,-32768,
   116,-32768,-32768,-32768,   -14,   -87,-32768,-32768,-32768,   302,
  -481,-32768,-32768,  -149,-32768,-32768,-32768,-32768,   -83,   290,
-32768,  -240,   -43,-32768,-32768,   335,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -320,  -299,  -279,   -75,-32768,
   -97,  -351,   435,     0,   584
};


#define	YYLAST		1340


static const short yytable[] = {     1,
    47,   384,   227,   228,   315,     9,   318,   509,   539,   559,
    12,   284,   260,   347,   517,   383,   377,    55,    68,   412,
   256,   659,    10,   417,   686,   724,   340,    32,    36,    37,
   663,   346,   665,    36,     2,    83,   120,   121,   122,    46,
    13,   367,    15,   360,   361,    38,    53,    25,   292,   360,
   361,    60,   281,   282,    21,    64,   283,    60,   281,   282,
   331,    29,   332,   333,    29,   337,    71,    72,   729,    76,
   622,   623,   624,   625,   626,    49,    50,   347,   486,   487,
   488,   489,    89,    90,   321,   264,   265,   266,   267,   235,
   217,   218,   219,   281,   282,   477,    43,   745,   452,   419,
   360,   361,   149,  -340,   431,   117,   270,   271,   434,    40,
   600,   362,    42,    54,  -340,   742,    61,   362,    73,    67,
    70,    78,    82,    84,    96,    91,   334,    97,    99,   106,
   335,   108,   115,   118,   150,   236,   237,   238,   239,   240,
   241,   242,   243,   244,   245,   246,   134,   163,   136,   140,
   166,   156,   347,   147,   152,   153,   154,   165,   170,   182,
   530,   172,   179,   180,   187,   193,   183,   184,   185,   202,
   189,   484,   197,   523,   208,   198,   199,   225,   261,   192,
   204,   497,   213,   230,   278,   200,   216,   258,   203,   295,
   259,   309,   316,   688,  -340,   272,   322,   296,   298,   302,
   214,   327,   344,   301,   457,   341,   311,   345,   283,   367,
   352,   752,  -332,   376,   231,   703,   704,   379,   390,   232,
   395,  -340,   415,   420,   394,   422,   427,   440,   410,  -340,
   411,   750,   418,   414,   430,   441,   482,   458,   501,   464,
   492,   269,   269,   269,   479,   439,   275,   521,   467,   277,
   444,   468,   469,   507,   473,   480,   527,   481,   550,   532,
   520,   522,   338,   748,   525,   583,   538,   529,   551,   565,
   579,   300,   545,   552,   562,   602,  -339,   563,   568,   582,
   571,   580,   588,   581,   310,   578,   599,   617,   651,   618,
   666,   620,   670,   645,   649,   325,   657,   326,   724,   647,
   732,   330,   553,   661,   765,   648,   674,   676,   678,   683,
   766,   691,   544,   393,   702,   547,   548,   689,   690,   706,
   386,   713,   707,     8,   737,   717,   369,   370,   374,   714,
   715,   721,   722,   375,   735,   564,   693,   761,   378,   763,
   730,   190,    23,   113,    94,   595,   177,   613,   304,   438,
   499,   387,   601,   130,   305,   443,   502,   413,   476,    66,
    77,   274,   399,   531,   478,   402,   405,   409,    41,   475,
   619,   374,   145,   500,     0,     0,     0,     0,     0,     0,
     0,   598,     0,   426,     0,     0,     0,     0,     0,   432,
     0,     0,   435,   436,   621,   437,   399,   692,     0,   652,
   653,   654,   655,   161,     0,   442,   409,     0,     0,   445,
     0,     0,     0,   446,   450,   451,     0,     0,     0,     0,
   453,     0,     0,     0,     0,     0,     0,     0,     0,   462,
     0,     0,   495,     0,     0,   498,     7,     0,   470,   471,
   472,     0,     0,   474,     0,     0,     0,   450,     0,    20,
     0,     0,     0,     0,     0,    26,     0,     0,     0,     0,
     0,     0,     0,   496,     0,     0,     0,     0,     0,   754,
     0,     0,    44,     0,   503,     0,     0,     0,   504,     0,
     0,     0,   505,     0,     0,     0,     0,   535,    62,   508,
     0,     0,   511,   515,     0,   720,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   524,    79,     0,     0,
     0,     0,   515,     0,     0,     0,   534,     0,     0,     0,
     0,     0,     0,     0,     0,    98,     0,     0,   542,   739,
     0,   543,     0,   107,     0,     0,     0,   759,     0,     0,
     0,     0,   119,     0,   555,     0,     0,     0,     0,   132,
   561,     0,     0,     0,   137,   138,   139,     0,   597,   570,
     0,   572,     0,     0,   574,     0,     0,   577,     0,     0,
     0,     0,     0,     0,   155,     0,     0,     0,     0,     0,
   591,   592,     0,     0,     0,     0,     0,   634,     0,     0,
   171,     0,     0,     0,     0,     0,     0,     0,     0,   609,
   610,     0,    24,     0,     0,    27,   188,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   633,     0,   635,   636,
     0,     0,    45,     0,     0,     0,     0,   205,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   650,    63,     0,
     0,     0,   223,     0,     0,     0,     0,     0,   660,     0,
     0,   662,     0,   664,   694,     0,   669,     0,    81,   252,
   675,     0,   677,     0,   679,   680,     0,     0,   705,     0,
     0,    95,     0,     0,     0,     0,   104,     0,     0,     0,
   105,     0,     0,     0,     0,     0,   114,   696,   697,   698,
     0,     0,   700,     0,     0,   131,     0,     0,     0,   133,
     0,   708,   709,   710,     0,     0,   712,     0,     0,   146,
     0,     0,   306,   718,   719,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   756,     0,     0,   162,   733,     0,
     0,     0,   164,   736,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   178,     0,     0,     0,     0,   181,     0,
   349,     0,     0,     0,     0,     0,   357,     0,     0,   191,
   762,     0,     0,     0,     0,   196,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   380,     0,     0,   207,     0,
   210,   211,   212,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   224,     0,     0,     0,   229,     0,     0,     0,
     0,     0,     0,     0,     0,   234,     0,     0,     0,   253,
   254,   255,     0,   257,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   291,
     0,   293,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   454,     0,     0,     0,
     0,   459,     0,   308,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   320,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   350,     0,     0,     0,   356,     0,     0,   359,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   382,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   389,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   540,     0,     0,     0,     0,     0,     0,     0,   429,
     0,     0,     0,     0,   433,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   456,     0,     0,
     0,   460,     0,     0,     0,     0,     0,   593,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   611,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   640,   641,   642,
   643,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   516,     0,
     0,   519,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   684,     0,     0,     0,     0,     0,
     0,   541,     0,     0,     0,     0,     0,     0,   546,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   576,     0,     0,     0,     0,     0,     0,     0,   728,     0,
     0,     0,   590,     0,     0,     0,     0,     0,     0,     0,
   596,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   607,   608,     0,     0,     0,     0,     0,     0,     0,   614,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   644,     0,     0,     0,     0,     0,     0,   646,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   673,     0,     0,     0,     0,     0,     0,     0,
     0,   682,     0,     0,     0,   687,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   695,
     0,     0,     0,     0,     0,     0,     0,   701,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   711,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   731,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   738,     0,   740,     0,     0,   743,     0,     0,   746,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   757
};

static const short yycheck[] = {     0,
    44,   349,   211,   212,   288,     6,   290,   491,   526,   549,
    11,   252,   233,   313,   496,   348,   337,    48,    62,   371,
   229,   647,     5,   375,   671,    28,   306,    28,    29,    30,
   653,   311,   655,    34,    11,    79,    25,    26,    27,    40,
    70,     7,    12,     9,    10,    18,    47,     5,   257,     9,
    10,    52,     5,     6,    17,    56,     9,    58,     5,     6,
     7,     8,     9,    10,     8,   306,    67,    68,    71,    70,
    39,    40,    41,    42,    43,     5,     6,   377,    65,    66,
    67,    68,    83,    84,   293,   238,   239,   240,   241,     5,
    62,    63,    64,     5,     6,   447,    72,   744,   419,   379,
     9,    10,   133,     8,   388,   106,   243,   244,   392,    73,
   592,    77,     8,    19,    76,   741,    72,    77,    20,    76,
    74,    72,    72,    13,    72,    14,    73,    70,    21,     6,
    77,    22,    16,     6,   135,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    72,   148,     5,    30,
   151,    31,   452,     8,     6,     6,     6,    70,     6,    70,
   512,    32,    72,    71,     6,    35,   167,   168,   169,     6,
    71,   455,    70,   506,    23,    70,    70,    15,     4,   180,
    72,   465,    70,    70,    37,   186,    72,    70,   189,     5,
    72,     4,    23,   675,    73,    73,    50,    73,    73,    73,
   201,     6,     6,    72,   425,    33,    78,    72,     9,     7,
    46,   751,    78,     9,   215,   697,   698,    78,    45,   220,
     9,     3,     6,     8,    71,    34,    36,     3,    73,    75,
    74,   749,    72,    76,    73,     3,     8,    72,     7,    73,
   461,   242,   243,   244,    76,    75,   247,     6,    72,   250,
    75,    72,    74,    71,    74,    74,    29,    72,     6,    76,
    74,    72,   306,   747,    72,    48,    72,    74,    71,    47,
    70,   272,    74,    72,    71,    49,    38,    72,    71,    73,
    72,    72,    72,    71,   285,   569,    72,    74,    38,    70,
    44,    71,    24,    72,    71,   296,    71,   298,    28,    70,
     6,   302,   543,    73,     0,    72,    72,    72,    72,    72,
     0,    72,   533,   357,    74,   536,   537,    73,    73,    72,
   351,    72,    71,     4,    70,    72,   327,   328,   329,    74,
    74,    72,    72,   334,    72,   556,   684,    71,   339,    72,
   725,   174,    17,   101,    86,   585,   158,   604,   277,   397,
   470,   352,   593,   110,   277,   407,   474,   372,   446,    58,
    71,   246,   363,   513,   448,   366,   367,   368,    34,   445,
   611,   372,   124,   471,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   590,    -1,   384,    -1,    -1,    -1,    -1,    -1,   390,
    -1,    -1,   393,   394,   615,   396,   397,   681,    -1,   640,
   641,   642,   643,   142,    -1,   406,   407,    -1,    -1,   410,
    -1,    -1,    -1,   414,   415,   416,    -1,    -1,    -1,    -1,
   421,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   430,
    -1,    -1,   463,    -1,    -1,   466,     2,    -1,   439,   440,
   441,    -1,    -1,   444,    -1,    -1,    -1,   448,    -1,    15,
    -1,    -1,    -1,    -1,    -1,    21,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   464,    -1,    -1,    -1,    -1,    -1,   753,
    -1,    -1,    38,    -1,   475,    -1,    -1,    -1,   479,    -1,
    -1,    -1,   483,    -1,    -1,    -1,    -1,   518,    54,   490,
    -1,    -1,   493,   494,    -1,   716,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   507,    73,    -1,    -1,
    -1,    -1,   513,    -1,    -1,    -1,   517,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    91,    -1,    -1,   529,   738,
    -1,   532,    -1,    99,    -1,    -1,    -1,   758,    -1,    -1,
    -1,    -1,   108,    -1,   545,    -1,    -1,    -1,    -1,   115,
   551,    -1,    -1,    -1,   120,   121,   122,    -1,   589,   560,
    -1,   562,    -1,    -1,   565,    -1,    -1,   568,    -1,    -1,
    -1,    -1,    -1,    -1,   140,    -1,    -1,    -1,    -1,    -1,
   581,   582,    -1,    -1,    -1,    -1,    -1,   618,    -1,    -1,
   156,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   600,
   601,    -1,    19,    -1,    -1,    22,   172,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   617,    -1,   619,   620,
    -1,    -1,    39,    -1,    -1,    -1,    -1,   193,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   638,    55,    -1,
    -1,    -1,   208,    -1,    -1,    -1,    -1,    -1,   649,    -1,
    -1,   652,    -1,   654,   685,    -1,   657,    -1,    75,   225,
   661,    -1,   663,    -1,   665,   666,    -1,    -1,   699,    -1,
    -1,    88,    -1,    -1,    -1,    -1,    93,    -1,    -1,    -1,
    97,    -1,    -1,    -1,    -1,    -1,   103,   688,   689,   690,
    -1,    -1,   693,    -1,    -1,   112,    -1,    -1,    -1,   116,
    -1,   702,   703,   704,    -1,    -1,   707,    -1,    -1,   126,
    -1,    -1,   278,   714,   715,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   755,    -1,    -1,   144,   729,    -1,
    -1,    -1,   149,   734,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   160,    -1,    -1,    -1,    -1,   165,    -1,
   316,    -1,    -1,    -1,    -1,    -1,   322,    -1,    -1,   176,
   761,    -1,    -1,    -1,    -1,   182,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   341,    -1,    -1,   195,    -1,
   197,   198,   199,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   209,    -1,    -1,    -1,   213,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   222,    -1,    -1,    -1,   226,
   227,   228,    -1,   230,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   256,
    -1,   258,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   422,    -1,    -1,    -1,
    -1,   427,    -1,   280,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   292,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   317,    -1,    -1,    -1,   321,    -1,    -1,   324,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   343,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   354,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   527,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   386,
    -1,    -1,    -1,    -1,   391,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   424,    -1,    -1,
    -1,   428,    -1,    -1,    -1,    -1,    -1,   583,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   602,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   623,   624,   625,
   626,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   495,    -1,
    -1,   498,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   670,    -1,    -1,    -1,    -1,    -1,
    -1,   528,    -1,    -1,    -1,    -1,    -1,    -1,   535,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   567,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   724,    -1,
    -1,    -1,   579,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   587,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   597,   598,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   606,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   627,    -1,    -1,    -1,    -1,    -1,    -1,   634,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   659,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   668,    -1,    -1,    -1,   672,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   686,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   694,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   705,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   727,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   737,    -1,   739,    -1,    -1,   742,    -1,    -1,   745,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   756
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
#line 520 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 530 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 534 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 540 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 544 "parser-sming.y"
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
#line 559 "parser-sming.y"
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
#line 573 "parser-sming.y"
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
			        printError(thisParserPtr,
					   ERR_MODULE_ALREADY_LOADED,
					   yyvsp[0].text);
				free(yyvsp[0].text);
				/*
				 * this aborts parsing the whole file,
				 * not only the current module.
				 */
				YYABORT;
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
#line 604 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 9:
#line 611 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 10:
#line 618 "parser-sming.y"
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
#line 628 "parser-sming.y"
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
#line 646 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    Node *nodePtr;
			    int i;
			    
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
					 ((Object *)(thisParserPtr->
						     firstIndexlabelPtr->
						     ptr))->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr,
							   thisModulePtr);
				    listPtr->ptr = objectPtr;
				}
				/* adjust relatedPtr */
				if (((Object *)
				    (thisParserPtr->firstIndexlabelPtr->ptr))->
				    relatedPtr) {
				    objectPtr = findObject(
					((Object *)(thisParserPtr->
						    firstIndexlabelPtr->ptr))->
					relatedPtr,
					thisParserPtr,
			                thisModulePtr);
				    ((Object *)(thisParserPtr->
						firstIndexlabelPtr->ptr))->
					relatedPtr = objectPtr;
				}
				listPtr =
				    thisParserPtr->firstIndexlabelPtr->nextPtr;
				free(thisParserPtr->firstIndexlabelPtr);
				thisParserPtr->firstIndexlabelPtr = listPtr;
			    }

			    /*
			     * Is there a node that matches the `identity'
			     * statement?
			     */

			    
			    /*
			     * Set the oidlen/oid values that are not
			     * yet correct.
			     */
			    for (objectPtr = thisModulePtr->firstObjectPtr;
				 objectPtr; objectPtr = objectPtr->nextPtr) {
				if (objectPtr->export.oidlen == 0) {
				    if (objectPtr->nodePtr->oidlen == 0) {
					for (nodePtr = objectPtr->nodePtr,
						 i = 1;
					     nodePtr->parentPtr != rootNodePtr;
					     nodePtr = nodePtr->parentPtr,
						 i++);
					objectPtr->nodePtr->oid =
					    calloc(i, sizeof(unsigned int));
					objectPtr->nodePtr->oidlen = i;
					for (nodePtr = objectPtr->nodePtr;
					     i > 0; i--) {
					    objectPtr->nodePtr->oid[i-1] =
						nodePtr->subid;
					    nodePtr = nodePtr->parentPtr;
					}
				    }
				    objectPtr->export.oidlen =
					objectPtr->nodePtr->oidlen;
				    objectPtr->export.oid =
					objectPtr->nodePtr->oid;
				}
			    }
			    
			    yyval.modulePtr = thisModulePtr;
			    moduleObjectPtr = NULL;
			;
    break;}
case 13:
#line 735 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 14:
#line 739 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed extension statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 15:
#line 749 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 16:
#line 754 "parser-sming.y"
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
#line 769 "parser-sming.y"
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
#line 783 "parser-sming.y"
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
#line 793 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].status) {
				setMacroStatus(macroPtr, yyvsp[0].status);
			    }
			;
    break;}
case 20:
#line 799 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroDescription(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 21:
#line 805 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroReference(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 22:
#line 811 "parser-sming.y"
{
			;
    break;}
case 23:
#line 814 "parser-sming.y"
{
			    yyval.macroPtr = 0;
			    macroPtr = NULL;
			    free(macroIdentifier);
			;
    break;}
case 24:
#line 821 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 25:
#line 825 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 26:
#line 835 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 27:
#line 840 "parser-sming.y"
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
#line 855 "parser-sming.y"
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
#line 869 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 30:
#line 874 "parser-sming.y"
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
#line 893 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 32:
#line 899 "parser-sming.y"
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
#line 909 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 34:
#line 915 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 35:
#line 921 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 36:
#line 927 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 37:
#line 933 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 38:
#line 941 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 39:
#line 945 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 40:
#line 955 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 41:
#line 960 "parser-sming.y"
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
#line 975 "parser-sming.y"
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
#line 994 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 47:
#line 999 "parser-sming.y"
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
#line 1013 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 49:
#line 1019 "parser-sming.y"
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
#line 1032 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 51:
#line 1038 "parser-sming.y"
{
			    if (thisParserPtr->identityObjectName &&
				!strcmp(thisParserPtr->identityObjectName,
					nodeIdentifier)) {
				setModuleIdentityObject(
				    thisParserPtr->modulePtr, nodeObjectPtr);
				thisParserPtr->identityObjectName = NULL;
			    }
			    
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 52:
#line 1054 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 53:
#line 1059 "parser-sming.y"
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
#line 1072 "parser-sming.y"
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
#line 1087 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 56:
#line 1093 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 57:
#line 1099 "parser-sming.y"
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
#line 1109 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 1115 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 60:
#line 1121 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 61:
#line 1127 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 62:
#line 1133 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 63:
#line 1142 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 64:
#line 1147 "parser-sming.y"
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
#line 1162 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 66:
#line 1168 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 67:
#line 1174 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 68:
#line 1181 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 69:
#line 1189 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 70:
#line 1194 "parser-sming.y"
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
#line 1209 "parser-sming.y"
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
#line 1233 "parser-sming.y"
{
			    if (rowObjectPtr) {
				if (yyvsp[0].rc) {
				    addObjectFlags(rowObjectPtr,
						   FLAG_CREATABLE);
				    setObjectCreate(rowObjectPtr, 1);
				}
			    }
			;
    break;}
case 73:
#line 1243 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 74:
#line 1249 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 75:
#line 1255 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 76:
#line 1262 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 77:
#line 1270 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 78:
#line 1278 "parser-sming.y"
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
#line 1293 "parser-sming.y"
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
#line 1307 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 81:
#line 1312 "parser-sming.y"
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
#line 1325 "parser-sming.y"
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
#line 1340 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 84:
#line 1346 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 85:
#line 1352 "parser-sming.y"
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
#line 1362 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 87:
#line 1368 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 88:
#line 1374 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 89:
#line 1380 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 90:
#line 1386 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 91:
#line 1395 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 92:
#line 1399 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 93:
#line 1409 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 94:
#line 1414 "parser-sming.y"
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
#line 1429 "parser-sming.y"
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
#line 1443 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 97:
#line 1448 "parser-sming.y"
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
#line 1462 "parser-sming.y"
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
#line 1478 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 100:
#line 1484 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 101:
#line 1491 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 102:
#line 1497 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 103:
#line 1505 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 104:
#line 1509 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 105:
#line 1519 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 106:
#line 1523 "parser-sming.y"
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
#line 1538 "parser-sming.y"
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
#line 1552 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 109:
#line 1557 "parser-sming.y"
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
#line 1569 "parser-sming.y"
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
#line 1585 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 112:
#line 1591 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 113:
#line 1597 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 114:
#line 1603 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 115:
#line 1611 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 116:
#line 1615 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 117:
#line 1625 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 118:
#line 1630 "parser-sming.y"
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
#line 1645 "parser-sming.y"
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
#line 1659 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 121:
#line 1664 "parser-sming.y"
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
#line 1678 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 123:
#line 1684 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 124:
#line 1690 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 125:
#line 1696 "parser-sming.y"
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
#line 1712 "parser-sming.y"
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
#line 1728 "parser-sming.y"
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
#line 1768 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 129:
#line 1776 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 130:
#line 1780 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 131:
#line 1786 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 132:
#line 1790 "parser-sming.y"
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
#line 1805 "parser-sming.y"
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
#line 1819 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 135:
#line 1824 "parser-sming.y"
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
#line 1836 "parser-sming.y"
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
#line 1851 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 138:
#line 1855 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 139:
#line 1861 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 140:
#line 1865 "parser-sming.y"
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
#line 1880 "parser-sming.y"
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
#line 1897 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(yyvsp[-6].date, yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 143:
#line 1903 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 144:
#line 1907 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 145:
#line 1913 "parser-sming.y"
{
			    thisParserPtr->identityObjectName = yyvsp[-2].text;
			    yyval.rc = 1;
			;
    break;}
case 146:
#line 1922 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 147:
#line 1928 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 148:
#line 1932 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 149:
#line 1939 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 150:
#line 1945 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 151:
#line 1949 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 152:
#line 1956 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 158:
#line 1970 "parser-sming.y"
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
case 159:
#line 1990 "parser-sming.y"
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
case 160:
#line 2008 "parser-sming.y"
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
case 161:
#line 2028 "parser-sming.y"
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
case 162:
#line 2046 "parser-sming.y"
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
case 163:
#line 2066 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 164:
#line 2070 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 165:
#line 2076 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 166:
#line 2080 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 167:
#line 2086 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 168:
#line 2092 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 169:
#line 2098 "parser-sming.y"
{
			    yyval.date = yyvsp[-2].date;
			;
    break;}
case 170:
#line 2104 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 171:
#line 2110 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 172:
#line 2116 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 173:
#line 2120 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 174:
#line 2126 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 175:
#line 2132 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 176:
#line 2136 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 177:
#line 2142 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 178:
#line 2148 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 179:
#line 2152 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 180:
#line 2158 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 181:
#line 2164 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 182:
#line 2168 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 183:
#line 2174 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 184:
#line 2180 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 185:
#line 2184 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 186:
#line 2190 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 187:
#line 2196 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 188:
#line 2200 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 189:
#line 2206 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 190:
#line 2212 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 191:
#line 2216 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 192:
#line 2222 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 193:
#line 2228 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 194:
#line 2232 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 195:
#line 2238 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 196:
#line 2245 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 197:
#line 2251 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 198:
#line 2255 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 199:
#line 2262 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 200:
#line 2268 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 201:
#line 2272 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 202:
#line 2279 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 203:
#line 2285 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 204:
#line 2289 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 205:
#line 2295 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 206:
#line 2302 "parser-sming.y"
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
#line 2315 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 208:
#line 2324 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text,
						       thisParserPtr,
						       thisModulePtr);
			    yyval.optionPtr->export.description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 209:
#line 2334 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 210:
#line 2338 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 211:
#line 2344 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 212:
#line 2350 "parser-sming.y"
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
#line 2363 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 214:
#line 2374 "parser-sming.y"
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
#line 2399 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 216:
#line 2403 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 217:
#line 2409 "parser-sming.y"
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
#line 2424 "parser-sming.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 219:
#line 2428 "parser-sming.y"
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
#line 2443 "parser-sming.y"
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
#line 2458 "parser-sming.y"
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
#line 2473 "parser-sming.y"
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
#line 2488 "parser-sming.y"
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
#line 2503 "parser-sming.y"
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
#line 2518 "parser-sming.y"
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
#line 2533 "parser-sming.y"
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
#line 2548 "parser-sming.y"
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
#line 2565 "parser-sming.y"
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
#line 2579 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 230:
#line 2583 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 231:
#line 2589 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 232:
#line 2593 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 233:
#line 2599 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 234:
#line 2603 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 235:
#line 2610 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 236:
#line 2618 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 237:
#line 2622 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 238:
#line 2628 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 239:
#line 2634 "parser-sming.y"
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
#line 2647 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 241:
#line 2653 "parser-sming.y"
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
#line 2667 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 243:
#line 2671 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 244:
#line 2677 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 245:
#line 2683 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 246:
#line 2687 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 247:
#line 2694 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 248:
#line 2702 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 249:
#line 2706 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 250:
#line 2712 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 251:
#line 2718 "parser-sming.y"
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
#line 2731 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 253:
#line 2737 "parser-sming.y"
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
#line 2756 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 255:
#line 2760 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 256:
#line 2766 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 257:
#line 2772 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 258:
#line 2779 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 259:
#line 2787 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 260:
#line 2791 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 261:
#line 2797 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 262:
#line 2804 "parser-sming.y"
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
#line 2818 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 264:
#line 2824 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-6].text;
			    yyval.namedNumberPtr->export.value = *yyvsp[-2].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			;
    break;}
case 265:
#line 2833 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 266:
#line 2841 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 267:
#line 2845 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 268:
#line 2851 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 269:
#line 2857 "parser-sming.y"
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
#line 2870 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 271:
#line 2876 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 272:
#line 2884 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 273:
#line 2888 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 274:
#line 2894 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 275:
#line 2900 "parser-sming.y"
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
#line 2913 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 277:
#line 2919 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 278:
#line 2927 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 279:
#line 2931 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 280:
#line 2937 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 281:
#line 2943 "parser-sming.y"
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
#line 2956 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 283:
#line 2962 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 284:
#line 2968 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 285:
#line 2972 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 286:
#line 2980 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 287:
#line 2984 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 288:
#line 2990 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 289:
#line 2996 "parser-sming.y"
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
#line 3009 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 291:
#line 3015 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 292:
#line 3019 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 3025 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 294:
#line 3029 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 295:
#line 3035 "parser-sming.y"
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
#line 3046 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 297:
#line 3052 "parser-sming.y"
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
#line 3063 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 299:
#line 3069 "parser-sming.y"
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
#line 3083 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 301:
#line 3087 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 302:
#line 3093 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 303:
#line 3097 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 304:
#line 3107 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 305:
#line 3113 "parser-sming.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 306:
#line 3119 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 307:
#line 3125 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 308:
#line 3138 "parser-sming.y"
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
#line 3165 "parser-sming.y"
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
#line 3211 "parser-sming.y"
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
#line 3233 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 312:
#line 3239 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 313:
#line 3245 "parser-sming.y"
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
#line 3258 "parser-sming.y"
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
#line 3282 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 316:
#line 3287 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 317:
#line 3294 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 318:
#line 3298 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 319:
#line 3302 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 320:
#line 3308 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 321:
#line 3312 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 322:
#line 3316 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 323:
#line 3320 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 324:
#line 3326 "parser-sming.y"
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
#line 3355 "parser-sming.y"
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
#line 3393 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 327:
#line 3399 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 328:
#line 3403 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 329:
#line 3410 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 330:
#line 3414 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 331:
#line 3424 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 332:
#line 3433 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 333:
#line 3439 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 0);
			;
    break;}
case 334:
#line 3447 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 335:
#line 3456 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 336:
#line 3465 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 337:
#line 3469 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 338:
#line 3479 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 339:
#line 3486 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 340:
#line 3492 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 341:
#line 3498 "parser-sming.y"
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
#line 3503 "parser-sming.y"


#endif
			
