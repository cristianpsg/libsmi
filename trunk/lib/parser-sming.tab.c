
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



#define	YYFINAL		757
#define	YYFLAG		-32768
#define	YYNTBASE	77

#define YYTRANSLATE(x) ((unsigned)(x) <= 322 ? yytranslate[x] : 310)

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
    18,    19,    49,    50,    52,    54,    57,    60,    61,    62,
    63,    64,    81,    82,    84,    86,    89,    92,    93,    94,
    95,    96,    97,    98,    99,   100,   126,   127,   129,   131,
   134,   137,   139,   141,   143,   144,   145,   146,   147,   148,
   168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
   178,   211,   212,   213,   214,   215,   216,   239,   240,   241,
   242,   243,   244,   245,   246,   273,   275,   278,   281,   282,
   283,   284,   285,   286,   287,   288,   289,   290,   291,   324,
   325,   327,   329,   332,   335,   336,   337,   338,   339,   340,
   341,   364,   365,   367,   369,   372,   375,   376,   377,   378,
   379,   380,   381,   405,   406,   408,   410,   413,   416,   417,
   418,   419,   420,   421,   422,   423,   424,   451,   452,   454,
   456,   459,   462,   463,   464,   478,   479,   481,   483,   486,
   489,   501,   507,   508,   511,   517,   518,   521,   527,   529,
   531,   533,   535,   537,   548,   554,   567,   573,   586,   587,
   590,   591,   594,   599,   600,   603,   609,   615,   621,   627,
   633,   634,   637,   643,   644,   647,   653,   654,   657,   663,
   664,   667,   673,   674,   677,   683,   684,   687,   693,   694,
   697,   703,   713,   714,   717,   727,   728,   731,   741,   742,
   744,   746,   749,   752,   763,   764,   766,   768,   771,   774,
   788,   790,   792,   795,   797,   800,   803,   806,   809,   812,
   815,   818,   821,   824,   827,   828,   831,   833,   835,   836,
   839,   846,   847,   849,   851,   854,   859,   862,   863,   865,
   870,   871,   874,   881,   882,   884,   886,   889,   894,   897,
   898,   900,   905,   911,   915,   916,   918,   920,   923,   928,
   936,   940,   941,   943,   945,   948,   953,   957,   958,   960,
   962,   965,   970,   974,   975,   977,   979,   982,   987,   993,
   995,   999,  1000,  1002,  1004,  1007,  1012,  1014,  1016,  1018,
  1020,  1024,  1026,  1030,  1032,  1035,  1036,  1038,  1040,  1043,
  1046,  1048,  1050,  1052,  1054,  1056,  1059,  1061,  1063,  1065,
  1067,  1070,  1075,  1077,  1079,  1081,  1083,  1085,  1087,  1089,
  1092,  1094,  1096,  1097,  1099,  1101,  1104,  1107,  1109,  1111,
  1113,  1116,  1118,  1120,  1121,  1122,  1123
};

static const short yyrhs[] = {   308,
    78,     0,     0,    79,     0,    80,     0,    79,    80,     0,
    81,   308,     0,     0,     0,     0,     0,     0,    11,   307,
     5,    82,   308,    68,   309,   187,   215,   309,    83,   216,
   309,    84,   228,   309,    85,   229,    86,   193,    87,    95,
   107,   155,   165,   175,    69,   308,    70,     0,     0,    88,
     0,    89,     0,    88,    89,     0,    90,   309,     0,     0,
     0,     0,     0,    20,   307,     6,    91,   308,    68,   309,
   221,    92,   227,    93,   229,    94,    69,   308,    70,     0,
     0,    96,     0,    97,     0,    96,    97,     0,    98,   309,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
   307,     5,    99,   308,    68,   309,   197,   309,   100,   225,
   101,   217,   102,   219,   103,   221,   104,   227,   105,   229,
   106,    69,   308,    70,     0,     0,   108,     0,   109,     0,
   108,   109,     0,   110,   309,     0,   111,     0,   117,     0,
   128,     0,     0,     0,     0,     0,     0,    24,   307,     6,
   112,   308,    68,   309,   213,   309,   113,   221,   114,   227,
   115,   229,   116,    69,   308,    70,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    25,   307,     6,
   118,   308,    68,   309,   213,   309,   119,   199,   309,   120,
   224,   309,   121,   225,   122,   217,   123,   219,   124,   221,
   125,   228,   309,   126,   229,   127,    69,   308,    70,     0,
     0,     0,     0,     0,     0,    26,   307,     6,   129,   308,
    68,   309,   213,   309,   130,   221,   131,   228,   309,   132,
   229,   133,   134,   309,    69,   308,    70,     0,     0,     0,
     0,     0,     0,     0,     0,    28,   307,     6,   135,   308,
    68,   309,   213,   309,   136,   202,   309,   137,   209,   138,
   221,   139,   228,   309,   140,   229,   141,   142,    69,   308,
    70,     0,   143,     0,   142,   143,     0,   144,   309,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    27,   307,     6,   145,   308,    68,   309,   213,   309,   146,
   199,   309,   147,   224,   309,   148,   225,   149,   217,   150,
   219,   151,   221,   152,   228,   309,   153,   229,   154,    69,
   308,    70,     0,     0,   156,     0,   157,     0,   156,   157,
     0,   158,   309,     0,     0,     0,     0,     0,     0,     0,
    29,   307,     6,   159,   308,    68,   309,   213,   309,   160,
   232,   161,   221,   162,   228,   309,   163,   229,   164,    69,
   308,    70,     0,     0,   166,     0,   167,     0,   166,   167,
     0,   168,   309,     0,     0,     0,     0,     0,     0,     0,
    30,   307,     6,   169,   308,    68,   309,   213,   309,   170,
   231,   309,   171,   221,   172,   228,   309,   173,   229,   174,
    69,   308,    70,     0,     0,   176,     0,   177,     0,   176,
   177,     0,   178,   309,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    31,   307,     6,   179,   308,    68,   309,
   213,   309,   180,   221,   181,   228,   309,   182,   229,   183,
   234,   184,   236,   185,   240,   186,    69,   308,    70,     0,
     0,   188,     0,   189,     0,   188,   189,     0,   190,   309,
     0,     0,     0,    12,   307,     5,   191,   308,    71,   308,
   271,   192,   308,    72,   308,    70,     0,     0,   194,     0,
   195,     0,   194,   195,     0,   196,   309,     0,    13,   308,
    68,   309,   214,   309,   228,   309,    69,   308,    70,     0,
    22,   307,   244,   308,    70,     0,     0,   199,   309,     0,
    22,   307,   244,   308,    70,     0,     0,   201,   309,     0,
    23,   307,   244,   308,    70,     0,   203,     0,   204,     0,
   205,     0,   206,     0,   207,     0,    38,   208,   308,    71,
   308,   275,   308,    72,   308,    70,     0,    39,   307,   286,
   308,    70,     0,    40,   307,   286,   208,   308,    71,   308,
   275,   308,    72,   308,    70,     0,    41,   307,   286,   308,
    70,     0,    42,   307,   286,   208,   308,    71,   308,   275,
   308,    72,   308,    70,     0,     0,   307,    37,     0,     0,
   210,   309,     0,    43,   211,   308,    70,     0,     0,   308,
   212,     0,    71,   308,   275,   308,    72,     0,    14,   307,
   297,   308,    70,     0,    15,   307,   291,   308,    70,     0,
    16,   307,   287,   308,    70,     0,    17,   307,   287,   308,
    70,     0,     0,   218,   309,     0,    32,   307,   292,   308,
    70,     0,     0,   220,   309,     0,    33,   307,   293,   308,
    70,     0,     0,   222,   309,     0,    34,   307,   295,   308,
    70,     0,     0,   224,   309,     0,    35,   307,   296,   308,
    70,     0,     0,   226,   309,     0,    36,   307,   294,   308,
    70,     0,     0,   228,   309,     0,    18,   307,   287,   308,
    70,     0,     0,   230,   309,     0,    19,   307,   287,   308,
    70,     0,    44,   308,    71,   308,   275,   308,    72,   308,
    70,     0,     0,   233,   309,     0,    45,   308,    71,   308,
   275,   308,    72,   308,    70,     0,     0,   235,   309,     0,
    46,   308,    71,   308,   275,   308,    72,   308,    70,     0,
     0,   237,     0,   238,     0,   237,   238,     0,   239,   309,
     0,    47,   307,   286,   308,    68,   228,   309,    69,   308,
    70,     0,     0,   241,     0,   242,     0,   241,   242,     0,
   243,   309,     0,    48,   307,   286,   308,    68,   198,   200,
   223,   228,   309,    69,   308,    70,     0,   245,     0,   246,
     0,    49,   249,     0,    50,     0,    51,   249,     0,    52,
   249,     0,    53,   249,     0,    54,   249,     0,    55,   257,
     0,    56,   257,     0,    57,   257,     0,    59,   265,     0,
    58,   265,     0,   285,   247,     0,     0,   308,   248,     0,
   250,     0,   258,     0,     0,   308,   250,     0,    71,   308,
   254,   251,   308,    72,     0,     0,   252,     0,   253,     0,
   252,   253,     0,   308,    73,   308,   254,     0,   305,   255,
     0,     0,   256,     0,   308,     3,   308,   305,     0,     0,
   308,   258,     0,    71,   308,   262,   259,   308,    72,     0,
     0,   260,     0,   261,     0,   260,   261,     0,   308,    73,
   308,   262,     0,     7,   263,     0,     0,   264,     0,   308,
     3,   308,     7,     0,    71,   308,   266,   308,    72,     0,
   270,   267,   306,     0,     0,   268,     0,   269,     0,   268,
   269,     0,   308,    74,   308,   270,     0,     6,   308,    71,
   308,   303,   308,    72,     0,   284,   272,   306,     0,     0,
   273,     0,   274,     0,   273,   274,     0,   308,    74,   308,
   284,     0,     0,     0,   306,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   308,    74,   308,     0,     0,   286,
   276,   306,     0,     0,   277,     0,   278,     0,   277,   278,
     0,   308,    74,   308,   286,     0,    71,   308,   280,   308,
    72,     0,   306,     0,     6,   281,   306,     0,     0,   282,
     0,   283,     0,   282,   283,     0,   308,    74,   308,     6,
     0,     5,     0,     6,     0,   285,     0,   286,     0,     5,
     4,     5,     0,     5,     0,     5,     4,     6,     0,     6,
     0,     8,   288,     0,     0,   289,     0,   290,     0,   289,
   290,     0,   308,     8,     0,     8,     0,     8,     0,     8,
     0,   279,     0,     9,     0,    75,     9,     0,    10,     0,
     7,     0,   287,     0,   286,     0,   286,   300,     0,   302,
    76,   302,   300,     0,    60,     0,    61,     0,    62,     0,
    63,     0,    64,     0,    65,     0,    66,     0,   298,   299,
     0,   286,     0,   302,     0,     0,   300,     0,   301,     0,
   300,   301,     0,    76,   302,     0,     9,     0,    10,     0,
     9,     0,    75,     9,     0,   303,     0,   304,     0,     0,
     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   515,   525,   529,   535,   539,   554,   568,   589,   598,   605,
   616,   626,   684,   688,   698,   702,   718,   732,   741,   749,
   755,   760,   767,   771,   781,   785,   801,   815,   819,   840,
   846,   856,   862,   868,   874,   879,   887,   891,   901,   905,
   921,   935,   936,   937,   940,   944,   960,   966,   979,   984,
  1001,  1005,  1019,  1034,  1041,  1047,  1057,  1063,  1068,  1075,
  1080,  1089,  1093,  1110,  1115,  1122,  1127,  1136,  1140,  1156,
  1181,  1192,  1197,  1204,  1210,  1218,  1226,  1241,  1255,  1259,
  1273,  1288,  1295,  1301,  1311,  1317,  1322,  1329,  1334,  1343,
  1347,  1357,  1361,  1377,  1391,  1395,  1411,  1427,  1432,  1440,
  1445,  1453,  1457,  1467,  1471,  1486,  1500,  1504,  1517,  1534,
  1539,  1546,  1551,  1559,  1563,  1573,  1577,  1593,  1607,  1611,
  1627,  1632,  1639,  1645,  1661,  1677,  1716,  1724,  1728,  1734,
  1738,  1753,  1767,  1771,  1784,  1799,  1803,  1809,  1813,  1828,
  1842,  1851,  1859,  1863,  1869,  1876,  1880,  1886,  1893,  1894,
  1895,  1896,  1897,  1900,  1921,  1937,  1959,  1975,  1997,  2001,
  2007,  2011,  2017,  2027,  2031,  2037,  2043,  2049,  2055,  2061,
  2067,  2071,  2077,  2083,  2087,  2093,  2099,  2103,  2109,  2115,
  2119,  2125,  2131,  2135,  2141,  2147,  2151,  2157,  2163,  2167,
  2173,  2179,  2186,  2190,  2196,  2203,  2207,  2213,  2220,  2224,
  2230,  2236,  2250,  2256,  2269,  2273,  2279,  2285,  2298,  2304,
  2334,  2338,  2344,  2359,  2363,  2378,  2393,  2408,  2423,  2438,
  2453,  2468,  2483,  2500,  2514,  2518,  2524,  2528,  2534,  2538,
  2544,  2553,  2557,  2563,  2569,  2582,  2588,  2602,  2606,  2612,
  2618,  2622,  2628,  2637,  2641,  2647,  2653,  2666,  2672,  2691,
  2695,  2701,  2707,  2713,  2722,  2726,  2732,  2738,  2753,  2759,
  2768,  2776,  2780,  2786,  2792,  2805,  2811,  2819,  2823,  2829,
  2835,  2848,  2854,  2862,  2866,  2872,  2878,  2891,  2897,  2903,
  2907,  2915,  2919,  2925,  2931,  2944,  2950,  2954,  2960,  2964,
  2970,  2981,  2987,  2998,  3004,  3018,  3022,  3028,  3032,  3042,
  3048,  3054,  3060,  3073,  3100,  3146,  3168,  3174,  3180,  3193,
  3217,  3222,  3229,  3233,  3237,  3243,  3247,  3251,  3255,  3261,
  3290,  3328,  3334,  3338,  3345,  3349,  3359,  3368,  3374,  3382,
  3391,  3400,  3404,  3414,  3421,  3427,  3433
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
"@2","@3","@4","@5","extensionStatement_stmtsep_0n","extensionStatement_stmtsep_1n",
"extensionStatement_stmtsep","extensionStatement","@6","@7","@8","@9","typedefStatement_stmtsep_0n",
"typedefStatement_stmtsep_1n","typedefStatement_stmtsep","typedefStatement",
"@10","@11","@12","@13","@14","@15","@16","@17","anyObjectStatement_stmtsep_0n",
"anyObjectStatement_stmtsep_1n","anyObjectStatement_stmtsep","anyObjectStatement",
"nodeStatement","@18","@19","@20","@21","@22","scalarStatement","@23","@24",
"@25","@26","@27","@28","@29","@30","@31","@32","tableStatement","@33","@34",
"@35","@36","@37","rowStatement","@38","@39","@40","@41","@42","@43","@44","columnStatement_stmtsep_1n",
"columnStatement_stmtsep","columnStatement","@45","@46","@47","@48","@49","@50",
"@51","@52","@53","@54","notificationStatement_stmtsep_0n","notificationStatement_stmtsep_1n",
"notificationStatement_stmtsep","notificationStatement","@55","@56","@57","@58",
"@59","@60","groupStatement_stmtsep_0n","groupStatement_stmtsep_1n","groupStatement_stmtsep",
"groupStatement","@61","@62","@63","@64","@65","@66","complianceStatement_stmtsep_0n",
"complianceStatement_stmtsep_1n","complianceStatement_stmtsep","complianceStatement",
"@67","@68","@69","@70","@71","@72","@73","@74","importStatement_stmtsep_0n",
"importStatement_stmtsep_1n","importStatement_stmtsep","importStatement","@75",
"@76","revisionStatement_stmtsep_0n","revisionStatement_stmtsep_1n","revisionStatement_stmtsep",
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
    86,    81,    87,    87,    88,    88,    89,    91,    92,    93,
    94,    90,    95,    95,    96,    96,    97,    99,   100,   101,
   102,   103,   104,   105,   106,    98,   107,   107,   108,   108,
   109,   110,   110,   110,   112,   113,   114,   115,   116,   111,
   118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
   117,   129,   130,   131,   132,   133,   128,   135,   136,   137,
   138,   139,   140,   141,   134,   142,   142,   143,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   144,   155,
   155,   156,   156,   157,   159,   160,   161,   162,   163,   164,
   158,   165,   165,   166,   166,   167,   169,   170,   171,   172,
   173,   174,   168,   175,   175,   176,   176,   177,   179,   180,
   181,   182,   183,   184,   185,   186,   178,   187,   187,   188,
   188,   189,   191,   192,   190,   193,   193,   194,   194,   195,
   196,   197,   198,   198,   199,   200,   200,   201,   202,   202,
   202,   202,   202,   203,   204,   205,   206,   207,   208,   208,
   209,   209,   210,   211,   211,   212,   213,   214,   215,   216,
   217,   217,   218,   219,   219,   220,   221,   221,   222,   223,
   223,   224,   225,   225,   226,   227,   227,   228,   229,   229,
   230,   231,   232,   232,   233,   234,   234,   235,   236,   236,
   237,   237,   238,   239,   240,   240,   241,   241,   242,   243,
   244,   244,   245,   245,   245,   245,   245,   245,   245,   245,
   245,   245,   245,   246,   247,   247,   248,   248,   249,   249,
   250,   251,   251,   252,   252,   253,   254,   255,   255,   256,
   257,   257,   258,   259,   259,   260,   260,   261,   262,   263,
   263,   264,   265,   266,   267,   267,   268,   268,   269,   270,
   271,   272,   272,   273,   273,   274,    -1,    -1,    -1,    -1,
    -1,    -1,   275,   276,   276,   277,   277,   278,   279,   280,
   280,   281,   281,   282,   282,   283,   284,   284,    -1,    -1,
   285,   285,   286,   286,   287,   288,   288,   289,   289,   290,
   291,   292,   293,   294,   294,   294,   294,   294,   294,   294,
   294,   294,   295,   295,   295,   296,   296,   296,   296,   297,
   298,   298,   299,   299,   300,   300,   301,   302,   303,   303,
   304,   305,   305,   306,   307,   308,   309
};

static const short yyr2[] = {     0,
     2,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,    29,     0,     1,     1,     2,     2,     0,     0,     0,
     0,    16,     0,     1,     1,     2,     2,     0,     0,     0,
     0,     0,     0,     0,     0,    25,     0,     1,     1,     2,
     2,     1,     1,     1,     0,     0,     0,     0,     0,    19,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    32,     0,     0,     0,     0,     0,    22,     0,     0,     0,
     0,     0,     0,     0,    26,     1,     2,     2,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    32,     0,
     1,     1,     2,     2,     0,     0,     0,     0,     0,     0,
    22,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,     0,    23,     0,     1,     1,     2,     2,     0,     0,
     0,     0,     0,     0,     0,     0,    26,     0,     1,     1,
     2,     2,     0,     0,    13,     0,     1,     1,     2,     2,
    11,     5,     0,     2,     5,     0,     2,     5,     1,     1,
     1,     1,     1,    10,     5,    12,     5,    12,     0,     2,
     0,     2,     4,     0,     2,     5,     5,     5,     5,     5,
     0,     2,     5,     0,     2,     5,     0,     2,     5,     0,
     2,     5,     0,     2,     5,     0,     2,     5,     0,     2,
     5,     9,     0,     2,     9,     0,     2,     9,     0,     1,
     1,     2,     2,    10,     0,     1,     1,     2,     2,    13,
     1,     1,     2,     1,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     0,     2,     1,     1,     0,     2,
     6,     0,     1,     1,     2,     4,     2,     0,     1,     4,
     0,     2,     6,     0,     1,     1,     2,     4,     2,     0,
     1,     4,     5,     3,     0,     1,     1,     2,     4,     7,
     3,     0,     1,     1,     2,     4,     3,     0,     1,     1,
     2,     4,     3,     0,     1,     1,     2,     4,     5,     1,
     3,     0,     1,     1,     2,     4,     1,     1,     1,     1,
     3,     1,     3,     1,     2,     0,     1,     1,     2,     2,
     1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
     2,     4,     1,     1,     1,     1,     1,     1,     1,     2,
     1,     1,     0,     1,     1,     2,     2,     1,     1,     1,
     2,     1,     1,     0,     0,     0,     0
};

static const short yydefact[] = {   336,
     2,   335,     1,     3,     4,   336,     0,     5,     6,     7,
   336,     0,   337,   128,   335,     0,   129,   130,   337,     0,
   335,   337,   131,   132,   133,     0,     8,   336,   296,   336,
     0,     0,   295,   297,   298,     0,     0,   335,   337,   336,
   299,   300,   169,     0,     9,     0,   336,     0,   287,   288,
   134,   262,     0,   335,   337,   336,   334,   263,   264,     0,
   170,     0,    10,     0,   261,   265,   336,   336,   189,   336,
     0,     0,   335,    11,   337,     0,   266,   188,     0,   136,
   190,   135,   336,   336,    13,   137,   138,   337,     0,     0,
   335,    23,    14,    15,   337,   139,   140,   191,   337,     0,
   335,    37,    24,    25,   337,    16,    17,     0,    18,     0,
   335,   335,   335,    90,    38,    39,   337,    42,    43,    44,
    26,    27,   335,   337,   336,    28,     0,     0,     0,   335,
   102,    91,    92,   337,    40,    41,     0,     0,     0,   336,
    45,    51,    62,     0,   335,   114,   103,   104,   337,    93,
    94,   301,   336,   337,   337,     0,   336,   336,   336,    95,
     0,   335,     0,   115,   116,   337,   105,   106,     0,     0,
   177,   337,     0,     0,     0,   336,   107,     0,   336,   117,
   118,   168,   336,   335,    19,   337,     0,   337,   337,   337,
     0,   336,   119,     0,     0,     0,   186,   178,   335,   337,
     0,     0,     0,   337,     0,   336,    12,   141,   313,   314,
   315,   336,    20,   337,     0,    29,   335,   337,   337,   337,
     0,   337,     0,     0,   189,   187,   292,   229,   214,   229,
   229,   229,   229,   241,   241,   241,     0,     0,   336,   211,
   212,   225,   183,     0,    46,    52,    63,   337,     0,   337,
   179,    21,     0,   213,     0,   215,   216,   217,   218,   219,
     0,   220,   221,   336,   223,   222,     0,   224,     0,   335,
    30,   337,     0,   294,   328,   321,   336,   323,   322,   177,
     0,   177,    96,   337,     0,     0,   291,   336,   230,   336,
   242,     0,   142,   336,   226,   227,   228,     0,   171,   184,
     0,     0,     0,   320,   324,   325,    47,   335,   337,    64,
   193,   108,   337,   336,     0,     0,   336,   336,   255,     0,
   308,   305,   307,   336,     0,   304,   310,   309,   336,     0,
   335,    31,   337,   293,   167,   327,   326,   186,     0,    53,
     0,   336,    97,   337,     0,   120,     0,   330,   329,     0,
   232,   332,   333,   238,   250,   244,     0,     0,   334,   256,
   257,     0,   334,   306,   311,     0,     0,     0,   174,   172,
    48,   336,     0,   337,     0,   177,   194,   336,   337,   177,
    22,   331,   336,   233,   234,     0,   237,   239,     0,   249,
   251,     0,   336,   245,   246,     0,   336,   253,   254,   258,
   336,   282,   336,   280,   185,     0,   302,   336,   335,    32,
   337,   189,     0,   335,   337,    65,   336,    98,     0,   109,
   121,     0,   235,   336,   336,   336,     0,   247,   336,     0,
     0,   334,   283,   284,     0,     0,   312,     0,     0,   177,
   175,    49,   145,     0,    54,   189,     0,     0,   336,   177,
     0,   231,     0,     0,     0,   243,     0,   336,   259,   281,
   285,   336,   279,   173,   303,   336,    33,     0,   316,   317,
   318,   319,   336,   183,    66,   336,   274,   337,     0,   110,
   337,   236,   240,   252,   248,     0,     0,     0,   186,   336,
     0,    55,     0,     0,   334,   275,   276,     0,    99,   336,
     0,   122,   260,   286,   176,    34,     0,   182,   171,   335,
   337,   336,   273,   277,   336,   189,     0,   337,   189,   189,
    50,    56,     0,     0,     0,     0,   100,   336,   111,   123,
    35,   174,    68,   336,   195,   278,     0,     0,   189,   196,
     0,    57,   336,     0,   336,   192,   112,   336,   124,   337,
   336,   177,     0,    67,     0,     0,     0,   199,   197,     0,
    58,   337,   101,   336,   336,   335,   125,   200,   201,   337,
    36,     0,     0,     0,     0,     0,   205,   202,   203,   337,
   337,   113,   336,   336,   335,   126,   206,   207,   337,    59,
    69,     0,     0,     0,     0,   208,   209,   189,     0,   336,
     0,   336,   336,    60,   159,   335,   335,   335,   335,   337,
   149,   150,   151,   152,   153,     0,   337,     0,     0,     0,
   336,     0,     0,     0,     0,     0,    70,   198,     0,   143,
   127,   336,     0,   160,   336,   159,   336,   159,   161,   336,
   146,   337,     0,   336,     0,   336,     0,   336,   164,    71,
   337,     0,   335,   180,   337,   144,    61,     0,   155,     0,
   157,     0,   336,     0,   177,   162,   204,     0,     0,   337,
   147,   336,   336,   336,     0,   336,   165,    72,   336,   337,
   181,     0,     0,     0,   163,     0,     0,     0,     0,   336,
   336,   336,   336,   337,   148,   336,     0,     0,     0,     0,
    73,     0,   154,   336,   336,   166,   189,   210,     0,     0,
    74,   156,   158,     0,   335,     0,    76,   337,     0,   336,
    77,    78,    79,     0,   336,    75,     0,   337,     0,   337,
    80,     0,   337,    81,     0,   337,    82,   183,    83,   171,
    84,   174,    85,   177,    86,     0,   337,    87,   189,    88,
     0,   336,     0,    89,     0,     0,     0
};

static const short yydefgoto[] = {   755,
     3,     4,     5,     6,    11,    31,    48,    69,    80,    92,
    93,    94,    95,   125,   197,   225,   286,   102,   103,   104,
   105,   140,   243,   299,   369,   440,   489,   520,   541,   114,
   115,   116,   117,   118,   157,   280,   338,   412,   468,   119,
   158,   281,   373,   474,   509,   532,   552,   572,   598,   620,
   120,   159,   282,   341,   446,   493,   511,   543,   599,   639,
   665,   687,   707,   714,   716,   717,   718,   725,   732,   735,
   738,   740,   742,   744,   746,   749,   751,   131,   132,   133,
   134,   176,   311,   376,   448,   516,   537,   146,   147,   148,
   149,   192,   345,   450,   501,   539,   556,   163,   164,   165,
   166,   206,   380,   451,   519,   540,   558,   577,   595,    16,
    17,    18,    19,    28,    56,    85,    86,    87,    88,   200,
   641,   309,   654,   655,   610,   611,   612,   613,   614,   615,
   621,   650,   651,   663,   677,   218,   124,    22,    39,   332,
   333,   410,   411,   185,   186,   669,   415,   271,   272,   213,
   214,    74,    75,   379,   343,   344,   549,   550,   567,   568,
   569,   570,   586,   587,   588,   589,   239,   240,   241,   268,
   295,   254,   289,   383,   384,   385,   351,   387,   388,   260,
   291,   393,   394,   395,   356,   390,   391,   265,   318,   359,
   360,   361,   319,    51,    57,    58,    59,   476,   495,   496,
   497,   326,   403,   432,   433,   434,    52,   242,   477,    30,
    33,    34,    35,   153,   408,   466,   329,   212,   473,   277,
   278,   304,   305,   306,   279,   352,   353,   354,    65,   622,
   255,    14
};

static const short yypact[] = {-32768,
    30,-32768,-32768,    30,-32768,-32768,    46,-32768,-32768,-32768,
-32768,   -37,-32768,    45,-32768,    50,    45,-32768,-32768,    64,
-32768,-32768,-32768,-32768,-32768,    70,-32768,-32768,    99,-32768,
    72,    38,-32768,    99,-32768,   103,    43,-32768,-32768,-32768,
-32768,-32768,-32768,    70,-32768,    59,-32768,    96,-32768,-32768,
-32768,    41,    47,-32768,-32768,-32768,-32768,    41,-32768,    42,
-32768,    70,-32768,    48,-32768,-32768,-32768,-32768,   100,-32768,
    59,    51,-32768,-32768,-32768,    54,-32768,-32768,    70,   105,
-32768,-32768,-32768,-32768,   118,   105,-32768,-32768,    69,    55,
-32768,   121,   118,-32768,-32768,-32768,-32768,-32768,-32768,   137,
-32768,    20,   121,-32768,-32768,-32768,-32768,   129,-32768,   140,
-32768,-32768,-32768,   117,    20,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   141,   142,   144,-32768,
   122,   117,-32768,-32768,-32768,-32768,   146,    96,    87,-32768,
-32768,-32768,-32768,   150,-32768,   130,   122,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    92,-32768,-32768,-32768,-32768,
   156,-32768,    94,   130,-32768,-32768,-32768,-32768,    97,    95,
   132,-32768,   101,   104,   106,-32768,-32768,   162,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   149,-32768,-32768,-32768,
   107,-32768,-32768,   108,   111,    -7,    96,-32768,-32768,-32768,
   159,   159,   159,-32768,   109,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    77,-32768,-32768,-32768,-32768,-32768,
   159,-32768,   114,   115,   100,-32768,   180,   116,-32768,   116,
   116,   116,   116,   116,   116,   116,   119,   119,-32768,-32768,
-32768,   116,   152,    33,-32768,-32768,-32768,-32768,   159,-32768,
-32768,-32768,   181,-32768,   120,-32768,-32768,-32768,-32768,-32768,
   125,-32768,-32768,-32768,-32768,-32768,   123,-32768,   126,-32768,
-32768,-32768,   185,-32768,-32768,-32768,-32768,   124,-32768,   132,
   176,   132,-32768,-32768,   159,   133,-32768,-32768,-32768,-32768,
-32768,   193,-32768,-32768,-32768,-32768,-32768,    66,   169,-32768,
   197,   134,   196,-32768,   124,-32768,-32768,-32768,-32768,-32768,
   163,-32768,-32768,-32768,    15,   200,-32768,-32768,    41,    26,
-32768,   135,-32768,-32768,   201,-32768,   124,-32768,-32768,   138,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    96,    77,-32768,
    96,-32768,-32768,-32768,   165,-32768,   143,-32768,-32768,   207,
   145,-32768,-32768,   214,   214,   145,   148,   153,-32768,    41,
-32768,   155,   216,-32768,   124,   154,   196,   218,   194,-32768,
-32768,-32768,   195,-32768,   160,   132,-32768,-32768,-32768,   132,
-32768,-32768,-32768,   145,-32768,   164,-32768,-32768,   229,-32768,
-32768,   230,-32768,   145,-32768,   167,-32768,-32768,-32768,-32768,
-32768,    41,-32768,-32768,-32768,   124,-32768,-32768,-32768,-32768,
-32768,   100,   168,-32768,-32768,-32768,-32768,-32768,   170,-32768,
-32768,   171,-32768,-32768,-32768,-32768,   173,-32768,-32768,    71,
   193,-32768,    41,-32768,   172,   175,   124,   178,   241,   132,
-32768,-32768,-32768,    -4,-32768,   100,    81,    96,-32768,   132,
    96,-32768,    15,    15,   243,-32768,   200,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   182,-32768,-32768,
-32768,-32768,-32768,   152,-32768,-32768,    41,-32768,    81,-32768,
-32768,-32768,-32768,-32768,-32768,   183,   246,   184,    96,-32768,
   186,-32768,   225,   187,-32768,    41,-32768,   189,-32768,-32768,
    96,-32768,-32768,-32768,-32768,-32768,   188,-32768,   169,-32768,
-32768,-32768,-32768,-32768,-32768,   100,   198,-32768,   100,   100,
-32768,-32768,   251,   191,   199,    81,-32768,-32768,-32768,-32768,
-32768,   194,-32768,-32768,-32768,-32768,   192,   202,   100,   219,
   204,-32768,-32768,   205,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   132,   203,-32768,   206,   209,   208,   220,-32768,   210,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   220,-32768,-32768,
-32768,    96,   159,   211,    81,    81,   226,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   226,-32768,-32768,-32768,
-32768,   212,   215,    81,   217,-32768,-32768,   100,    53,-32768,
    96,-32768,-32768,-32768,   248,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   222,-32768,   221,   223,   227,
-32768,   250,    81,    81,    81,    81,-32768,-32768,   228,   176,
-32768,-32768,   224,-32768,-32768,   248,-32768,   248,   256,-32768,
   277,-32768,   233,-32768,   234,-32768,   235,-32768,   116,-32768,
-32768,   236,-32768,   195,-32768,-32768,-32768,    81,-32768,   237,
-32768,   238,-32768,   239,   132,-32768,-32768,    77,    96,-32768,
-32768,-32768,-32768,-32768,   242,-32768,-32768,-32768,-32768,-32768,
-32768,   244,    81,    81,-32768,    81,    96,   245,   253,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   255,   254,   259,   260,
-32768,   257,-32768,-32768,-32768,-32768,   100,-32768,   258,   263,
-32768,-32768,-32768,   280,-32768,    10,-32768,-32768,   307,-32768,
-32768,-32768,-32768,   264,-32768,-32768,   252,-32768,   159,-32768,
-32768,   176,-32768,-32768,   195,-32768,-32768,   152,-32768,   169,
-32768,   194,-32768,   132,-32768,    96,-32768,-32768,   100,-32768,
   266,-32768,   267,-32768,   323,   336,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   262,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   247,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   240,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   231,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -378,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   213,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   232,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   249,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   322,-32768,-32768,-32768,-32768,-32768,   261,-32768,-32768,
-32768,  -610,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -588,-32768,-32768,-32768,-32768,  -200,-32768,-32768,-32768,  -496,
-32768,  -527,-32768,  -270,-32768,-32768,  -632,  -470,-32768,  -324,
   -30,  -218,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -224,-32768,-32768,-32768,  -239,-32768,  -338,-32768,-32768,-32768,
-32768,  -133,    80,-32768,-32768,   -34,  -101,-32768,-32768,  -131,
    84,-32768,-32768,   -33,  -100,-32768,-32768,   127,-32768,-32768,
-32768,    -2,   -69,-32768,-32768,-32768,   305,  -463,-32768,-32768,
  -132,-32768,-32768,-32768,-32768,   -67,   296,-32768,  -235,   -36,
-32768,-32768,   334,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -304,  -288,  -271,   -61,-32768,   -84,  -344,   656,
     0,   464
};


#define	YYLAST		1371


static const short yytable[] = {     1,
   372,   219,   220,   492,   542,     9,   252,    47,   276,   307,
    12,   310,   522,   371,   399,   500,   337,    55,   404,   642,
   248,   670,   365,   348,   349,    68,   330,    32,    36,    37,
    13,   336,   355,    36,   348,   349,   715,   273,   274,    46,
     2,   275,    83,   111,   112,   113,    53,   646,   284,   648,
    10,    60,   209,   210,   211,    64,    15,    60,   469,   470,
   471,   472,   327,    49,    50,    21,    71,    72,    25,    76,
   273,   274,   321,    29,   322,   323,   337,    29,   720,   348,
   349,   227,    89,    90,   313,   273,   274,   460,    38,   350,
   605,   606,   607,   608,   609,   406,   256,   257,   258,   259,
   350,   437,   736,   262,   263,   418,  -336,   154,    40,   421,
    42,   583,    43,    54,  -336,    67,    61,    84,    73,    70,
    78,   733,    99,    82,   139,   228,   229,   230,   231,   232,
   233,   234,   235,   236,   237,   238,   324,    91,    98,   156,
   325,   101,   109,   123,   126,   130,   141,   142,   337,   143,
   513,   145,   169,   152,   155,   160,   173,   174,   175,   172,
   162,   177,   179,   183,   506,   184,   182,   193,   188,   467,
   199,   189,   217,   190,   204,   191,   222,   207,   194,   480,
   208,   250,   195,   253,   251,   287,  -336,   270,   301,   264,
   288,   205,   293,   442,   672,   290,   294,   308,   317,   303,
   331,   314,   334,   335,   275,   223,   355,   342,   378,   364,
  -328,   224,   381,   367,   743,   382,  -336,  -336,   397,   691,
   692,   402,   693,   405,   398,   407,   409,   475,   401,   414,
   417,   425,   426,   261,   261,   261,   424,   443,   267,   429,
   449,   269,   452,   741,   456,   462,   463,   464,   465,   484,
   490,   504,   510,   505,   503,   508,   533,   521,   512,   534,
   545,   328,   515,   292,   548,     8,   566,   739,   535,   528,
   562,   546,   551,   585,   554,   563,   302,   564,   565,   571,
   582,   561,   601,   600,  -335,   603,   634,   315,   630,   316,
   536,   628,   631,   320,   644,   632,   640,   527,   649,   653,
   530,   531,   657,   659,   661,   667,   715,   673,   674,   676,
   374,   685,   723,   347,   695,   690,   357,   358,   362,   728,
   547,   696,   756,   363,   703,   704,   708,   712,   366,   679,
   705,   706,   713,   726,   752,   757,   754,   721,    23,   106,
   584,   375,   121,   578,   150,   135,    96,   596,   296,   423,
   386,   482,   297,   389,   392,   396,   485,   400,   602,   362,
   428,   459,    66,   514,   266,   461,    77,    41,   458,   483,
     0,   413,   581,     0,     0,     0,     0,   419,   167,   604,
     0,     0,   422,   386,     0,     0,     0,   635,   636,   637,
   638,     0,   427,   396,   678,     0,   430,     0,     0,     0,
   431,   435,   436,     0,     0,     0,     0,   438,     0,     0,
     0,     0,   180,     0,     0,     0,   447,   478,     0,     0,
   481,     0,     0,   453,   454,   455,     0,     0,   457,     0,
     0,     0,   435,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   479,     0,
     0,     0,     0,     0,     0,     0,     0,   486,     0,     0,
     0,   487,     0,     0,     0,   488,     0,     0,     0,     0,
   518,     0,   491,   745,     0,   494,   498,     0,     0,     0,
     0,     0,    24,     0,     0,    27,     0,     0,   711,   507,
     0,     0,     0,     0,     0,   498,     0,     0,     0,   517,
     0,     0,    45,     0,     0,     0,     0,     0,     0,     0,
     0,   525,     0,     0,   526,     0,     0,     0,    63,     0,
     0,     0,     0,     0,     0,     0,     0,   538,   730,     0,
   750,     0,     0,   544,     0,     0,     0,     0,    81,     0,
     0,   580,   553,     0,   555,     0,     0,   557,     0,     0,
   560,    97,     0,     0,     0,     0,     0,     0,   107,     0,
     0,     0,   108,   574,   575,     0,     0,     0,   122,     0,
   617,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   136,     0,   592,   593,     0,     0,     0,   138,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   151,     0,   616,
     0,   618,   619,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   168,     0,     0,     0,     0,   170,   171,     0,
   633,     0,     0,     0,     0,     0,     0,     0,     0,   181,
     0,   643,     0,     0,   645,   187,   647,     0,   680,   652,
     0,     0,     0,   658,     0,   660,     0,   662,   664,   198,
     0,   201,   202,   203,     0,     0,   694,     7,     0,     0,
     0,     0,   675,   216,     0,     0,     0,   221,     0,     0,
    20,   682,   683,   684,     0,   686,    26,   226,   688,     0,
     0,   245,   246,   247,     0,   249,     0,     0,     0,   697,
   698,   699,   700,    44,     0,   702,     0,     0,     0,     0,
     0,     0,     0,   709,   710,     0,     0,     0,     0,    62,
     0,   283,     0,   285,     0,   747,     0,     0,     0,   724,
     0,     0,     0,     0,   727,     0,     0,     0,    79,     0,
     0,     0,     0,     0,     0,   300,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   100,   312,     0,     0,
     0,   753,     0,     0,     0,     0,   110,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,   128,   129,     0,
     0,     0,   340,     0,     0,     0,   346,     0,   137,     0,
     0,     0,     0,     0,     0,   144,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   370,     0,     0,     0,
   161,     0,     0,     0,     0,     0,     0,   377,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   178,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   416,     0,   196,
     0,     0,   420,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   215,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   244,     0,   441,     0,     0,     0,   445,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   298,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   499,     0,     0,   502,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   339,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   524,     0,     0,     0,     0,     0,
     0,   529,     0,     0,     0,     0,   368,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   559,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   573,     0,     0,     0,     0,
     0,     0,     0,   579,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   590,   591,     0,     0,     0,     0,     0,
     0,     0,   597,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   439,     0,     0,     0,     0,   444,
     0,     0,     0,   627,     0,     0,     0,     0,     0,     0,
   629,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   656,     0,     0,     0,     0,
     0,     0,     0,     0,   666,     0,     0,     0,   671,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   681,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   689,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   701,     0,     0,
     0,     0,     0,     0,     0,   523,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   722,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   729,     0,   731,     0,     0,   734,     0,     0,   737,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   748,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   576,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   594,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   623,   624,   625,   626,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   668,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   719
};

static const short yycheck[] = {     0,
   339,   202,   203,   474,   532,     6,   225,    44,   244,   280,
    11,   282,   509,   338,   359,   479,   305,    48,   363,   630,
   221,   654,   327,     9,    10,    62,   298,    28,    29,    30,
    68,   303,     7,    34,     9,    10,    27,     5,     6,    40,
    11,     9,    79,    24,    25,    26,    47,   636,   249,   638,
     5,    52,    60,    61,    62,    56,    12,    58,    63,    64,
    65,    66,   298,     5,     6,    16,    67,    68,     5,    70,
     5,     6,     7,     8,     9,    10,   365,     8,    69,     9,
    10,     5,    83,    84,   285,     5,     6,   432,    17,    75,
    38,    39,    40,    41,    42,   367,   230,   231,   232,   233,
    75,   406,   735,   235,   236,   376,     8,   138,    71,   380,
     8,   575,    70,    18,    74,    74,    70,    13,    19,    72,
    70,   732,    68,    70,   125,    49,    50,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    71,    20,    70,   140,
    75,    21,     6,    15,     5,    29,     6,     6,   437,     6,
   495,    30,   153,     8,    68,     6,   157,   158,   159,    68,
    31,     6,    69,    69,   489,    34,    70,     6,    68,   440,
    22,    68,    14,    68,    68,   176,    68,    70,   179,   450,
    70,    68,   183,     4,    70,     5,    71,    36,     4,    71,
    71,   192,    70,   412,   658,    71,    71,    22,     6,    76,
    32,    69,     6,    70,     9,   206,     7,    45,    44,     9,
    76,   212,    70,    76,   742,     9,     3,    73,    71,   683,
   684,     6,   686,    70,    72,     8,    33,   446,    74,    35,
    71,     3,     3,   234,   235,   236,    73,    70,   239,    73,
    71,   242,    72,   740,    72,    74,    72,    70,     8,     7,
    69,     6,    28,    70,    72,    70,     6,    70,    72,    69,
    69,   298,    74,   264,    46,     4,    47,   738,    70,    72,
    68,    70,    69,    48,    70,    70,   277,    69,    71,    70,
    70,   552,    68,    72,    37,    69,    37,   288,    68,   290,
   526,    70,    70,   294,    71,    69,    69,   516,    43,    23,
   519,   520,    70,    70,    70,    70,    27,    71,    71,    71,
   341,    70,     6,   314,    70,    72,   317,   318,   319,    68,
   539,    69,     0,   324,    70,    72,    70,    70,   329,   668,
    72,    72,    70,    70,    69,     0,    70,   716,    17,    93,
   576,   342,   103,   568,   132,   115,    86,   587,   269,   384,
   351,   453,   269,   354,   355,   356,   457,   360,   594,   360,
   394,   431,    58,   496,   238,   433,    71,    34,   430,   454,
    -1,   372,   573,    -1,    -1,    -1,    -1,   378,   147,   598,
    -1,    -1,   383,   384,    -1,    -1,    -1,   623,   624,   625,
   626,    -1,   393,   394,   665,    -1,   397,    -1,    -1,    -1,
   401,   402,   403,    -1,    -1,    -1,    -1,   408,    -1,    -1,
    -1,    -1,   164,    -1,    -1,    -1,   417,   448,    -1,    -1,
   451,    -1,    -1,   424,   425,   426,    -1,    -1,   429,    -1,
    -1,    -1,   433,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   449,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   458,    -1,    -1,
    -1,   462,    -1,    -1,    -1,   466,    -1,    -1,    -1,    -1,
   501,    -1,   473,   744,    -1,   476,   477,    -1,    -1,    -1,
    -1,    -1,    19,    -1,    -1,    22,    -1,    -1,   707,   490,
    -1,    -1,    -1,    -1,    -1,   496,    -1,    -1,    -1,   500,
    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   512,    -1,    -1,   515,    -1,    -1,    -1,    55,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   528,   729,    -1,
   749,    -1,    -1,   534,    -1,    -1,    -1,    -1,    75,    -1,
    -1,   572,   543,    -1,   545,    -1,    -1,   548,    -1,    -1,
   551,    88,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,
    -1,    -1,    99,   564,   565,    -1,    -1,    -1,   105,    -1,
   601,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   117,    -1,   583,   584,    -1,    -1,    -1,   124,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   134,    -1,   600,
    -1,   602,   603,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   149,    -1,    -1,    -1,    -1,   154,   155,    -1,
   621,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   166,
    -1,   632,    -1,    -1,   635,   172,   637,    -1,   669,   640,
    -1,    -1,    -1,   644,    -1,   646,    -1,   648,   649,   186,
    -1,   188,   189,   190,    -1,    -1,   687,     2,    -1,    -1,
    -1,    -1,   663,   200,    -1,    -1,    -1,   204,    -1,    -1,
    15,   672,   673,   674,    -1,   676,    21,   214,   679,    -1,
    -1,   218,   219,   220,    -1,   222,    -1,    -1,    -1,   690,
   691,   692,   693,    38,    -1,   696,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   704,   705,    -1,    -1,    -1,    -1,    54,
    -1,   248,    -1,   250,    -1,   746,    -1,    -1,    -1,   720,
    -1,    -1,    -1,    -1,   725,    -1,    -1,    -1,    73,    -1,
    -1,    -1,    -1,    -1,    -1,   272,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    91,   284,    -1,    -1,
    -1,   752,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,    -1,
    -1,    -1,   309,    -1,    -1,    -1,   313,    -1,   123,    -1,
    -1,    -1,    -1,    -1,    -1,   130,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   333,    -1,    -1,    -1,
   145,    -1,    -1,    -1,    -1,    -1,    -1,   344,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,    -1,   184,
    -1,    -1,   379,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   199,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   217,    -1,   411,    -1,    -1,    -1,   415,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   270,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   478,    -1,    -1,   481,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   308,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   511,    -1,    -1,    -1,    -1,    -1,
    -1,   518,    -1,    -1,    -1,    -1,   331,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   550,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   562,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   570,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   580,   581,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   589,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   409,    -1,    -1,    -1,    -1,   414,
    -1,    -1,    -1,   610,    -1,    -1,    -1,    -1,    -1,    -1,
   617,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   642,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   651,    -1,    -1,    -1,   655,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   670,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   680,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   694,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   510,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   718,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   728,    -1,   730,    -1,    -1,   733,    -1,    -1,   736,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   747,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   566,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   585,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   606,   607,   608,   609,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   653,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   715
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
#line 592 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 9:
#line 599 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 10:
#line 606 "parser-sming.y"
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
#line 616 "parser-sming.y"
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
#line 633 "parser-sming.y"
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
#line 685 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 14:
#line 689 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed extension statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 15:
#line 699 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 16:
#line 704 "parser-sming.y"
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
#line 719 "parser-sming.y"
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
#line 733 "parser-sming.y"
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
#line 743 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].status) {
				setMacroStatus(macroPtr, yyvsp[0].status);
			    }
			;
    break;}
case 20:
#line 749 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroDescription(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 21:
#line 755 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroReference(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 22:
#line 761 "parser-sming.y"
{
			    yyval.macroPtr = 0;
			    macroPtr = NULL;
			    free(macroIdentifier);
			;
    break;}
case 23:
#line 768 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 24:
#line 772 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 25:
#line 782 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 26:
#line 787 "parser-sming.y"
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
case 27:
#line 802 "parser-sming.y"
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
case 28:
#line 816 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 29:
#line 821 "parser-sming.y"
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
case 30:
#line 840 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 31:
#line 846 "parser-sming.y"
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
case 32:
#line 856 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 33:
#line 862 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 34:
#line 868 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 35:
#line 874 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 36:
#line 880 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 37:
#line 888 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 38:
#line 892 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 39:
#line 902 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 40:
#line 907 "parser-sming.y"
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
case 41:
#line 922 "parser-sming.y"
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
case 45:
#line 941 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 46:
#line 946 "parser-sming.y"
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
case 47:
#line 960 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 48:
#line 966 "parser-sming.y"
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
case 49:
#line 979 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 50:
#line 985 "parser-sming.y"
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
case 51:
#line 1002 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 52:
#line 1007 "parser-sming.y"
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
case 53:
#line 1020 "parser-sming.y"
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
case 54:
#line 1035 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 55:
#line 1041 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 56:
#line 1047 "parser-sming.y"
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
case 57:
#line 1057 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 58:
#line 1063 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 59:
#line 1069 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 60:
#line 1075 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 61:
#line 1081 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 62:
#line 1090 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 63:
#line 1095 "parser-sming.y"
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
case 64:
#line 1110 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 65:
#line 1116 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 66:
#line 1122 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 67:
#line 1129 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 68:
#line 1137 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 69:
#line 1142 "parser-sming.y"
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
case 70:
#line 1157 "parser-sming.y"
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
case 71:
#line 1181 "parser-sming.y"
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
case 72:
#line 1192 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 73:
#line 1198 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 74:
#line 1204 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 75:
#line 1211 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 76:
#line 1219 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 77:
#line 1227 "parser-sming.y"
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
case 78:
#line 1242 "parser-sming.y"
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
case 79:
#line 1256 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 80:
#line 1261 "parser-sming.y"
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
case 81:
#line 1274 "parser-sming.y"
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
case 82:
#line 1289 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 83:
#line 1295 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 84:
#line 1301 "parser-sming.y"
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
case 85:
#line 1311 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 86:
#line 1317 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 87:
#line 1323 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 88:
#line 1329 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 89:
#line 1335 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 90:
#line 1344 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 91:
#line 1348 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 92:
#line 1358 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 93:
#line 1363 "parser-sming.y"
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
case 94:
#line 1378 "parser-sming.y"
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
case 95:
#line 1392 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 96:
#line 1397 "parser-sming.y"
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
case 97:
#line 1411 "parser-sming.y"
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
case 98:
#line 1427 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 99:
#line 1433 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 100:
#line 1440 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 101:
#line 1446 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 102:
#line 1454 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 103:
#line 1458 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 104:
#line 1468 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 105:
#line 1472 "parser-sming.y"
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
case 106:
#line 1487 "parser-sming.y"
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
case 107:
#line 1501 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 108:
#line 1506 "parser-sming.y"
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
case 109:
#line 1518 "parser-sming.y"
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
case 110:
#line 1534 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 111:
#line 1540 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 112:
#line 1546 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 113:
#line 1552 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 114:
#line 1560 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 115:
#line 1564 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 116:
#line 1574 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 117:
#line 1579 "parser-sming.y"
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
case 118:
#line 1594 "parser-sming.y"
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
case 119:
#line 1608 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 120:
#line 1613 "parser-sming.y"
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
case 121:
#line 1627 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 122:
#line 1633 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 123:
#line 1639 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 124:
#line 1645 "parser-sming.y"
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
case 125:
#line 1661 "parser-sming.y"
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
case 126:
#line 1677 "parser-sming.y"
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
case 127:
#line 1717 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 128:
#line 1725 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 129:
#line 1729 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 130:
#line 1735 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 131:
#line 1739 "parser-sming.y"
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
case 132:
#line 1754 "parser-sming.y"
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
case 133:
#line 1768 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 134:
#line 1773 "parser-sming.y"
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
case 135:
#line 1785 "parser-sming.y"
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
case 136:
#line 1800 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 137:
#line 1804 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 138:
#line 1810 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 139:
#line 1814 "parser-sming.y"
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
case 140:
#line 1829 "parser-sming.y"
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
case 141:
#line 1846 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(yyvsp[-6].date, yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 142:
#line 1854 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 143:
#line 1860 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 144:
#line 1864 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 145:
#line 1871 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 146:
#line 1877 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 147:
#line 1881 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 148:
#line 1888 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 154:
#line 1902 "parser-sming.y"
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
case 155:
#line 1922 "parser-sming.y"
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
case 156:
#line 1940 "parser-sming.y"
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
case 157:
#line 1960 "parser-sming.y"
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
case 158:
#line 1978 "parser-sming.y"
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
case 159:
#line 1998 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 160:
#line 2002 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 161:
#line 2008 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 162:
#line 2012 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 163:
#line 2018 "parser-sming.y"
{
			    if (rowObjectPtr) {
				addObjectFlags(rowObjectPtr, FLAG_CREATABLE);
				setObjectCreate(rowObjectPtr, 1);
			    }
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 164:
#line 2028 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 165:
#line 2032 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 166:
#line 2038 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 167:
#line 2044 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 168:
#line 2050 "parser-sming.y"
{
			    yyval.date = yyvsp[-2].date;
			;
    break;}
case 169:
#line 2056 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
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
			    yyval.text = NULL;
			;
    break;}
case 172:
#line 2072 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 173:
#line 2078 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 174:
#line 2084 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 175:
#line 2088 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 176:
#line 2094 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 177:
#line 2100 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 178:
#line 2104 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 179:
#line 2110 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 180:
#line 2116 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 181:
#line 2120 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 182:
#line 2126 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 183:
#line 2132 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 184:
#line 2136 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 185:
#line 2142 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 186:
#line 2148 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 187:
#line 2152 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 188:
#line 2158 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 189:
#line 2164 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 190:
#line 2168 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 191:
#line 2174 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 192:
#line 2181 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 193:
#line 2187 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 194:
#line 2191 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 195:
#line 2198 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 196:
#line 2204 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 197:
#line 2208 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 198:
#line 2215 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 199:
#line 2221 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 200:
#line 2225 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 201:
#line 2231 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 202:
#line 2238 "parser-sming.y"
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
case 203:
#line 2251 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 204:
#line 2260 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text,
						       thisParserPtr,
						       thisModulePtr);
			    yyval.optionPtr->export.description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 205:
#line 2270 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 206:
#line 2274 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 207:
#line 2280 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 208:
#line 2286 "parser-sming.y"
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
case 209:
#line 2299 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 210:
#line 2310 "parser-sming.y"
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
case 211:
#line 2335 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 212:
#line 2339 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 213:
#line 2345 "parser-sming.y"
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
case 214:
#line 2360 "parser-sming.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 215:
#line 2364 "parser-sming.y"
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
case 216:
#line 2379 "parser-sming.y"
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
case 217:
#line 2394 "parser-sming.y"
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
case 218:
#line 2409 "parser-sming.y"
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
case 219:
#line 2424 "parser-sming.y"
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
case 220:
#line 2439 "parser-sming.y"
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
case 221:
#line 2454 "parser-sming.y"
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
case 222:
#line 2469 "parser-sming.y"
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
case 223:
#line 2484 "parser-sming.y"
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
case 224:
#line 2501 "parser-sming.y"
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
case 225:
#line 2515 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 226:
#line 2519 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 227:
#line 2525 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 228:
#line 2529 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 229:
#line 2535 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 230:
#line 2539 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 231:
#line 2546 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 232:
#line 2554 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 233:
#line 2558 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 234:
#line 2564 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 235:
#line 2570 "parser-sming.y"
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
case 236:
#line 2583 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 237:
#line 2589 "parser-sming.y"
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
case 238:
#line 2603 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 239:
#line 2607 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 240:
#line 2613 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 241:
#line 2619 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 242:
#line 2623 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 243:
#line 2630 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 244:
#line 2638 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 245:
#line 2642 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 246:
#line 2648 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 247:
#line 2654 "parser-sming.y"
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
case 248:
#line 2667 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 249:
#line 2673 "parser-sming.y"
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
case 250:
#line 2692 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 251:
#line 2696 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 252:
#line 2702 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 253:
#line 2708 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 254:
#line 2715 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 255:
#line 2723 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 256:
#line 2727 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 257:
#line 2733 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 258:
#line 2740 "parser-sming.y"
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
case 259:
#line 2754 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 260:
#line 2760 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-6].text;
			    yyval.namedNumberPtr->export.value = *yyvsp[-2].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			;
    break;}
case 261:
#line 2769 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 262:
#line 2777 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 263:
#line 2781 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 264:
#line 2787 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 265:
#line 2793 "parser-sming.y"
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
case 266:
#line 2806 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 267:
#line 2812 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 268:
#line 2820 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 269:
#line 2824 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 270:
#line 2830 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 271:
#line 2836 "parser-sming.y"
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
case 272:
#line 2849 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 273:
#line 2855 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 274:
#line 2863 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 275:
#line 2867 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 276:
#line 2873 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 277:
#line 2879 "parser-sming.y"
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
case 278:
#line 2892 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 279:
#line 2898 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 280:
#line 2904 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 281:
#line 2908 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 282:
#line 2916 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 283:
#line 2920 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 284:
#line 2926 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 285:
#line 2932 "parser-sming.y"
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
case 286:
#line 2945 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 287:
#line 2951 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 288:
#line 2955 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 289:
#line 2961 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 290:
#line 2965 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 291:
#line 2971 "parser-sming.y"
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
case 292:
#line 2982 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 2988 "parser-sming.y"
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
#line 2999 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 295:
#line 3005 "parser-sming.y"
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
case 296:
#line 3019 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 297:
#line 3023 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 298:
#line 3029 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 299:
#line 3033 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 300:
#line 3043 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 301:
#line 3049 "parser-sming.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 302:
#line 3055 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 303:
#line 3061 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 304:
#line 3074 "parser-sming.y"
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
case 305:
#line 3101 "parser-sming.y"
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
case 306:
#line 3147 "parser-sming.y"
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
case 307:
#line 3169 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 308:
#line 3175 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 309:
#line 3181 "parser-sming.y"
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
case 310:
#line 3194 "parser-sming.y"
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
case 311:
#line 3218 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 312:
#line 3223 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 313:
#line 3230 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 314:
#line 3234 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 315:
#line 3238 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 316:
#line 3244 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 317:
#line 3248 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 318:
#line 3252 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 319:
#line 3256 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 320:
#line 3262 "parser-sming.y"
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
case 321:
#line 3291 "parser-sming.y"
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
case 322:
#line 3329 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 323:
#line 3335 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 324:
#line 3339 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 325:
#line 3346 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 326:
#line 3350 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 327:
#line 3360 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 328:
#line 3369 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 329:
#line 3375 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 0);
			;
    break;}
case 330:
#line 3383 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 331:
#line 3392 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 332:
#line 3401 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 333:
#line 3405 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 334:
#line 3415 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 335:
#line 3422 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 336:
#line 3428 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 337:
#line 3434 "parser-sming.y"
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
#line 3439 "parser-sming.y"


#endif
			
