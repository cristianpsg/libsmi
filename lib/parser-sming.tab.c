
/*  A Bison parser, made from parser-sming.y
 by  GNU Bison version 1.27
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse smingparse
#define yylex sminglex
#define yyerror smingerror
#define yylval sminglval
#define yychar smingchar
#define yydebug smingdebug
#define yynerrs smingnerrs
#define	DOT_DOT	257
#define	COLON_COLON	258
#define	ucIdentifier	259
#define	lcIdentifier	260
#define	floatValue	261
#define	textSegment	262
#define	decimalNumber	263
#define	hexadecimalNumber	264
#define	moduleKeyword	265
#define	importKeyword	266
#define	revisionKeyword	267
#define	oidKeyword	268
#define	dateKeyword	269
#define	organizationKeyword	270
#define	contactKeyword	271
#define	descriptionKeyword	272
#define	referenceKeyword	273
#define	extensionKeyword	274
#define	typedefKeyword	275
#define	typeKeyword	276
#define	writetypeKeyword	277
#define	nodeKeyword	278
#define	scalarKeyword	279
#define	tableKeyword	280
#define	columnKeyword	281
#define	rowKeyword	282
#define	notificationKeyword	283
#define	groupKeyword	284
#define	complianceKeyword	285
#define	formatKeyword	286
#define	unitsKeyword	287
#define	statusKeyword	288
#define	accessKeyword	289
#define	defaultKeyword	290
#define	impliedKeyword	291
#define	indexKeyword	292
#define	augmentsKeyword	293
#define	reordersKeyword	294
#define	sparseKeyword	295
#define	expandsKeyword	296
#define	createKeyword	297
#define	membersKeyword	298
#define	objectsKeyword	299
#define	mandatoryKeyword	300
#define	optionalKeyword	301
#define	refineKeyword	302
#define	OctetStringKeyword	303
#define	ObjectIdentifierKeyword	304
#define	Integer32Keyword	305
#define	Unsigned32Keyword	306
#define	Integer64Keyword	307
#define	Unsigned64Keyword	308
#define	Float32Keyword	309
#define	Float64Keyword	310
#define	Float128Keyword	311
#define	BitsKeyword	312
#define	EnumerationKeyword	313
#define	currentKeyword	314
#define	deprecatedKeyword	315
#define	obsoleteKeyword	316
#define	noaccessKeyword	317
#define	notifyonlyKeyword	318
#define	readonlyKeyword	319
#define	readwriteKeyword	320
#define	readcreateKeyword	321

#line 14 "parser-sming.y"


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
    
    if (!strstr(spec, "::")) {
	typePtr = findTypeByModuleAndName(modulePtr, spec);
	if (!typePtr) {
	    importPtr = findImportByName(spec, modulePtr);
	    if (importPtr) {
		typePtr = findTypeByModulenameAndName(importPtr->export.module,
						      spec);
	    }
	}
    } else {
	module = smiModule(spec);
	type   = smiDescriptor(spec);
	typePtr = findTypeByModulenameAndName(module, type);
	util_free(module);
	util_free(type);
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
    
    if (!strstr(spec, "::")) {
	objectPtr = findObjectByModuleAndName(modulePtr, spec);
	if (!objectPtr) {
	    importPtr = findImportByName(spec, modulePtr);
	    if (importPtr) {
	     objectPtr = findObjectByModulenameAndName(importPtr->export.module,
							  spec);
	    }
	}
    } else {
	module = smiModule(spec);
	object = smiDescriptor(spec);
	objectPtr = findObjectByModulenameAndName(module, object);
	util_free(module);
	util_free(object);
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



#define	YYFINAL		761
#define	YYFLAG		-32768
#define	YYNTBASE	77

#define YYTRANSLATE(x) ((unsigned)(x) <= 321 ? yytranslate[x] : 310)

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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67
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
   701,   702,   705,   711,   712,   715,   725,   726,   729,   739,
   740,   743,   753,   754,   756,   758,   761,   764,   775,   776,
   778,   780,   783,   786,   800,   802,   804,   807,   809,   812,
   815,   818,   821,   824,   827,   830,   833,   836,   839,   840,
   843,   845,   847,   848,   851,   858,   859,   861,   863,   866,
   871,   874,   875,   877,   882,   883,   886,   893,   894,   896,
   898,   901,   906,   909,   910,   912,   917,   923,   927,   928,
   930,   932,   935,   940,   948,   952,   953,   955,   957,   960,
   965,   969,   970,   972,   974,   977,   982,   986,   987,   989,
   991,   994,   999,  1005,  1007,  1011,  1012,  1014,  1016,  1019,
  1024,  1026,  1028,  1030,  1032,  1036,  1038,  1042,  1044,  1047,
  1048,  1050,  1052,  1055,  1058,  1060,  1062,  1064,  1066,  1068,
  1071,  1073,  1075,  1077,  1079,  1082,  1087,  1089,  1091,  1093,
  1095,  1097,  1099,  1101,  1104,  1106,  1108,  1109,  1111,  1113,
  1116,  1119,  1121,  1123,  1125,  1128,  1130,  1132,  1133,  1134,
  1135
};

static const short yyrhs[] = {   308,
    78,     0,     0,    79,     0,    80,     0,    79,    80,     0,
    81,   308,     0,     0,     0,     0,     0,     0,     0,     0,
    11,   307,     5,    82,   307,     6,    83,   308,    68,   309,
   189,   215,   309,    84,   217,   309,    85,   218,   309,    86,
   230,   309,    87,   231,    88,   195,    89,    97,   109,   157,
   167,   177,    69,   308,    70,     0,     0,    90,     0,    91,
     0,    90,    91,     0,    92,   309,     0,     0,     0,     0,
     0,    20,   307,     6,    93,   308,    68,   309,   223,    94,
   229,    95,   231,    96,    69,   308,    70,     0,     0,    98,
     0,    99,     0,    98,    99,     0,   100,   309,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    21,   307,     5,
   101,   308,    68,   309,   199,   309,   102,   227,   103,   219,
   104,   221,   105,   223,   106,   229,   107,   231,   108,    69,
   308,    70,     0,     0,   110,     0,   111,     0,   110,   111,
     0,   112,   309,     0,   113,     0,   119,     0,   130,     0,
     0,     0,     0,     0,     0,    24,   307,     6,   114,   308,
    68,   309,   215,   309,   115,   223,   116,   229,   117,   231,
   118,    69,   308,    70,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    25,   307,     6,   120,   308,
    68,   309,   215,   309,   121,   201,   309,   122,   226,   309,
   123,   227,   124,   219,   125,   221,   126,   223,   127,   230,
   309,   128,   231,   129,    69,   308,    70,     0,     0,     0,
     0,     0,     0,    26,   307,     6,   131,   308,    68,   309,
   215,   309,   132,   223,   133,   230,   309,   134,   231,   135,
   136,   309,    69,   308,    70,     0,     0,     0,     0,     0,
     0,     0,     0,    28,   307,     6,   137,   308,    68,   309,
   215,   309,   138,   204,   309,   139,   211,   140,   223,   141,
   230,   309,   142,   231,   143,   144,    69,   308,    70,     0,
   145,     0,   144,   145,     0,   146,   309,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    27,   307,
     6,   147,   308,    68,   309,   215,   309,   148,   201,   309,
   149,   226,   309,   150,   227,   151,   219,   152,   221,   153,
   223,   154,   230,   309,   155,   231,   156,    69,   308,    70,
     0,     0,   158,     0,   159,     0,   158,   159,     0,   160,
   309,     0,     0,     0,     0,     0,     0,     0,    29,   307,
     6,   161,   308,    68,   309,   215,   309,   162,   234,   163,
   223,   164,   230,   309,   165,   231,   166,    69,   308,    70,
     0,     0,   168,     0,   169,     0,   168,   169,     0,   170,
   309,     0,     0,     0,     0,     0,     0,     0,    30,   307,
     6,   171,   308,    68,   309,   215,   309,   172,   233,   309,
   173,   223,   174,   230,   309,   175,   231,   176,    69,   308,
    70,     0,     0,   178,     0,   179,     0,   178,   179,     0,
   180,   309,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    31,   307,     6,   181,   308,    68,   309,   215,   309,
   182,   223,   183,   230,   309,   184,   231,   185,   236,   186,
   238,   187,   242,   188,    69,   308,    70,     0,     0,   190,
     0,   191,     0,   190,   191,     0,   192,   309,     0,     0,
     0,    12,   307,     5,   193,   308,    71,   308,   273,   194,
   308,    72,   308,    70,     0,     0,   196,     0,   197,     0,
   196,   197,     0,   198,   309,     0,    13,   308,    68,   309,
   216,   309,   230,   309,    69,   308,    70,     0,    22,   307,
   246,   308,    70,     0,     0,   201,   309,     0,    22,   307,
   246,   308,    70,     0,     0,   203,   309,     0,    23,   307,
   246,   308,    70,     0,   205,     0,   206,     0,   207,     0,
   208,     0,   209,     0,    38,   210,   308,    71,   308,   277,
   308,    72,   308,    70,     0,    39,   307,   288,   308,    70,
     0,    40,   307,   288,   210,   308,    71,   308,   277,   308,
    72,   308,    70,     0,    41,   307,   288,   308,    70,     0,
    42,   307,   288,   210,   308,    71,   308,   277,   308,    72,
   308,    70,     0,     0,   307,    37,     0,     0,   212,   309,
     0,    43,   213,   308,    70,     0,     0,   308,   214,     0,
    71,   308,   277,   308,    72,     0,    14,   307,   297,   308,
    70,     0,    15,   307,   293,   308,    70,     0,    16,   307,
   289,   308,    70,     0,    17,   307,   289,   308,    70,     0,
     0,   220,   309,     0,    32,   307,   289,   308,    70,     0,
     0,   222,   309,     0,    33,   307,   289,   308,    70,     0,
     0,   224,   309,     0,    34,   307,   295,   308,    70,     0,
     0,   226,   309,     0,    35,   307,   296,   308,    70,     0,
     0,   228,   309,     0,    36,   307,   294,   308,    70,     0,
     0,   230,   309,     0,    18,   307,   289,   308,    70,     0,
     0,   232,   309,     0,    19,   307,   289,   308,    70,     0,
     0,   233,   309,     0,    44,   308,    71,   308,   277,   308,
    72,   308,    70,     0,     0,   235,   309,     0,    45,   308,
    71,   308,   277,   308,    72,   308,    70,     0,     0,   237,
   309,     0,    46,   308,    71,   308,   277,   308,    72,   308,
    70,     0,     0,   239,     0,   240,     0,   239,   240,     0,
   241,   309,     0,    47,   307,   288,   308,    68,   230,   309,
    69,   308,    70,     0,     0,   243,     0,   244,     0,   243,
   244,     0,   245,   309,     0,    48,   307,   288,   308,    68,
   200,   202,   225,   230,   309,    69,   308,    70,     0,   247,
     0,   248,     0,    49,   251,     0,    50,     0,    51,   251,
     0,    52,   251,     0,    53,   251,     0,    54,   251,     0,
    55,   259,     0,    56,   259,     0,    57,   259,     0,    59,
   267,     0,    58,   267,     0,   287,   249,     0,     0,   308,
   250,     0,   252,     0,   260,     0,     0,   308,   252,     0,
    71,   308,   256,   253,   308,    72,     0,     0,   254,     0,
   255,     0,   254,   255,     0,   308,    73,   308,   256,     0,
   305,   257,     0,     0,   258,     0,   308,     3,   308,   305,
     0,     0,   308,   260,     0,    71,   308,   264,   261,   308,
    72,     0,     0,   262,     0,   263,     0,   262,   263,     0,
   308,    73,   308,   264,     0,     7,   265,     0,     0,   266,
     0,   308,     3,   308,     7,     0,    71,   308,   268,   308,
    72,     0,   272,   269,   306,     0,     0,   270,     0,   271,
     0,   270,   271,     0,   308,    74,   308,   272,     0,     6,
   308,    71,   308,   303,   308,    72,     0,   286,   274,   306,
     0,     0,   275,     0,   276,     0,   275,   276,     0,   308,
    74,   308,   286,     0,     0,     0,   306,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   308,    74,   308,     0,
     0,   288,   278,   306,     0,     0,   279,     0,   280,     0,
   279,   280,     0,   308,    74,   308,   288,     0,    71,   308,
   282,   308,    72,     0,   306,     0,     6,   283,   306,     0,
     0,   284,     0,   285,     0,   284,   285,     0,   308,    74,
   308,     6,     0,     5,     0,     6,     0,   287,     0,   288,
     0,     5,     4,     5,     0,     5,     0,     5,     4,     6,
     0,     6,     0,     8,   290,     0,     0,   291,     0,   292,
     0,   291,   292,     0,   308,     8,     0,     8,     0,     8,
     0,     8,     0,   281,     0,     9,     0,    75,     9,     0,
    10,     0,     7,     0,   289,     0,   288,     0,   288,   300,
     0,   302,    76,   302,   300,     0,    60,     0,    61,     0,
    62,     0,    63,     0,    64,     0,    65,     0,    66,     0,
   298,   299,     0,   288,     0,   302,     0,     0,   300,     0,
   301,     0,   300,   301,     0,    76,   302,     0,     9,     0,
    10,     0,     9,     0,    75,     9,     0,   303,     0,   304,
     0,     0,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   516,   526,   530,   536,   540,   555,   569,   590,   594,   623,
   630,   637,   648,   658,   716,   720,   730,   734,   750,   764,
   773,   781,   787,   792,   799,   803,   813,   817,   833,   847,
   851,   872,   878,   888,   894,   900,   906,   911,   919,   923,
   933,   937,   953,   967,   968,   969,   972,   976,   992,   998,
  1011,  1016,  1024,  1028,  1042,  1057,  1064,  1070,  1080,  1086,
  1091,  1098,  1103,  1112,  1116,  1133,  1138,  1145,  1150,  1159,
  1163,  1179,  1204,  1215,  1220,  1227,  1233,  1241,  1249,  1264,
  1278,  1282,  1296,  1311,  1318,  1324,  1334,  1340,  1345,  1352,
  1357,  1366,  1370,  1380,  1384,  1400,  1414,  1418,  1434,  1450,
  1455,  1463,  1468,  1476,  1480,  1490,  1494,  1509,  1523,  1527,
  1540,  1557,  1562,  1569,  1574,  1582,  1586,  1596,  1600,  1616,
  1630,  1634,  1650,  1655,  1662,  1668,  1684,  1700,  1733,  1741,
  1745,  1751,  1755,  1770,  1784,  1788,  1801,  1816,  1820,  1826,
  1830,  1845,  1859,  1868,  1876,  1880,  1886,  1893,  1897,  1903,
  1910,  1911,  1912,  1913,  1914,  1917,  1938,  1954,  1976,  1992,
  2014,  2018,  2024,  2028,  2034,  2044,  2048,  2054,  2060,  2066,
  2072,  2078,  2084,  2088,  2094,  2100,  2104,  2110,  2116,  2120,
  2126,  2132,  2136,  2142,  2148,  2152,  2158,  2164,  2168,  2174,
  2180,  2184,  2190,  2196,  2200,  2206,  2213,  2217,  2223,  2230,
  2234,  2240,  2247,  2251,  2257,  2263,  2277,  2283,  2296,  2300,
  2306,  2312,  2325,  2331,  2361,  2365,  2371,  2386,  2390,  2405,
  2420,  2435,  2450,  2465,  2480,  2495,  2510,  2527,  2541,  2545,
  2551,  2555,  2561,  2565,  2571,  2580,  2584,  2590,  2596,  2609,
  2615,  2629,  2633,  2639,  2645,  2649,  2655,  2664,  2668,  2674,
  2680,  2693,  2699,  2718,  2722,  2728,  2734,  2740,  2749,  2753,
  2759,  2765,  2780,  2786,  2795,  2803,  2807,  2813,  2819,  2832,
  2838,  2846,  2850,  2856,  2862,  2875,  2881,  2889,  2893,  2899,
  2905,  2918,  2924,  2930,  2934,  2942,  2946,  2952,  2958,  2971,
  2977,  2981,  2987,  2991,  2997,  3008,  3014,  3025,  3031,  3045,
  3049,  3055,  3059,  3069,  3075,  3081,  3087,  3100,  3127,  3173,
  3195,  3201,  3207,  3220,  3244,  3249,  3256,  3260,  3264,  3270,
  3274,  3278,  3282,  3288,  3317,  3355,  3361,  3365,  3372,  3376,
  3386,  3395,  3401,  3409,  3418,  3427,  3431,  3441,  3448,  3454,
  3460
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
"date","anyValue","status","access","objectIdentifier","qlcIdentifier_subid",
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
   231,   231,   232,    -1,    -1,   233,   234,   234,   235,   236,
   236,   237,   238,   238,   239,   239,   240,   241,   242,   242,
   243,   243,   244,   245,   246,   246,   247,   247,   247,   247,
   247,   247,   247,   247,   247,   247,   247,   248,   249,   249,
   250,   250,   251,   251,   252,   253,   253,   254,   254,   255,
   256,   257,   257,   258,   259,   259,   260,   261,   261,   262,
   262,   263,   264,   265,   265,   266,   267,   268,   269,   269,
   270,   270,   271,   272,   273,   274,   274,   275,   275,   276,
    -1,    -1,    -1,    -1,    -1,    -1,   277,   278,   278,   279,
   279,   280,   281,   282,   282,   283,   283,   284,   284,   285,
   286,   286,    -1,    -1,   287,   287,   288,   288,   289,   290,
   290,   291,   291,   292,   293,    -1,    -1,   294,   294,   294,
   294,   294,   294,   294,   294,   294,   295,   295,   295,   296,
   296,   296,   296,   297,   298,   298,   299,   299,   300,   300,
   301,   302,   303,   303,   304,   305,   305,   306,   307,   308,
   309
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
     0,     2,     5,     0,     2,     9,     0,     2,     9,     0,
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
   339,     0,     8,   340,     0,   341,   130,   339,     0,   131,
   132,   341,     0,   339,   341,   133,   134,   135,     0,     9,
   340,     0,   298,   332,   325,   340,   327,   326,     0,     0,
     0,     0,     0,   324,   328,   329,   339,   341,   340,   297,
   169,   331,   330,     0,    10,     0,   300,   340,     0,   291,
   292,   136,   266,   299,   301,   302,     0,     0,   339,   341,
   340,   338,   267,   268,     0,   303,   304,   171,     0,    11,
     0,   265,   269,   340,   340,     0,   340,     0,     0,   339,
   341,     0,   270,   172,     0,    12,   137,   340,   191,     0,
   339,    13,   341,   190,     0,   138,   192,   340,   340,    15,
   139,   140,   341,     0,     0,   339,    25,    16,    17,   341,
   141,   142,   193,   341,     0,   339,    39,    26,    27,   341,
    18,    19,     0,    20,     0,   339,   339,   339,    92,    40,
    41,   341,    44,    45,    46,    28,    29,   339,   341,   340,
    30,     0,     0,     0,   339,   104,    93,    94,   341,    42,
    43,     0,     0,     0,   340,    47,    53,    64,     0,   339,
   116,   105,   106,   341,    95,    96,   305,   340,   341,   341,
     0,   340,   340,   340,    97,     0,   339,     0,   117,   118,
   341,   107,   108,     0,     0,   179,   341,     0,     0,     0,
   340,   109,     0,   340,   119,   120,   170,   340,   339,    21,
   341,     0,   341,   341,   341,     0,   340,   121,     0,     0,
     0,   188,   180,   339,   341,     0,     0,     0,   341,     0,
   340,    14,   143,   317,   318,   319,   340,    22,   341,     0,
    31,   341,   341,   341,     0,   341,     0,     0,   191,   189,
   296,   233,   218,   233,   233,   233,   233,   245,   245,   245,
     0,     0,   340,   215,   216,   229,   185,    48,    54,    65,
   341,     0,   341,   181,    23,     0,   217,     0,   219,   220,
   221,   222,   223,     0,   224,   225,   340,   227,   226,     0,
   228,     0,   339,    32,   341,   179,     0,   179,    98,   341,
     0,     0,   295,   340,   234,   340,   246,     0,   144,   340,
   230,   231,   232,     0,   173,   186,    49,   339,   341,    66,
   197,   110,   341,   340,     0,     0,   340,   340,   259,     0,
   312,   309,   311,   340,     0,   308,   314,   313,   340,     0,
   339,    33,   341,   188,     0,    55,     0,   340,    99,   341,
     0,   122,     0,   334,   333,     0,   236,   336,   337,   242,
   254,   248,     0,     0,   338,   260,   261,     0,   338,   310,
   315,     0,     0,     0,   176,   174,    50,   340,     0,   341,
     0,   179,   198,   340,   341,   179,    24,   335,   340,   237,
   238,     0,   241,   243,     0,   253,   255,     0,   340,   249,
   250,     0,   340,   257,   258,   262,   340,   286,   340,   284,
   187,     0,   340,   339,    34,   341,   191,     0,   339,   341,
    67,   340,   100,     0,   111,   123,     0,   239,   340,   340,
   340,     0,   251,   340,     0,     0,   338,   287,   288,     0,
     0,   316,     0,     0,   179,   177,    51,   147,     0,    56,
   191,     0,     0,   340,   179,     0,   235,     0,     0,     0,
   247,     0,   340,   263,   285,   289,   340,   283,   175,   340,
    35,     0,   320,   321,   322,   323,   340,   185,    68,   340,
   278,   341,     0,   112,   341,   240,   244,   256,   252,     0,
     0,     0,   188,   340,     0,    57,     0,     0,   338,   279,
   280,     0,   101,   340,     0,   124,   264,   290,   178,    36,
     0,   184,   173,   339,   341,   340,   277,   281,   340,   191,
     0,   341,   191,   191,    52,    58,     0,     0,     0,     0,
   102,   340,   113,   125,    37,   176,    70,   340,   199,   282,
     0,     0,   191,   200,     0,    59,   340,     0,   340,   196,
   114,   340,   126,   341,   340,   179,     0,    69,     0,     0,
     0,   203,   201,     0,    60,   341,   103,   340,   340,   339,
   127,   204,   205,   341,    38,     0,     0,     0,     0,     0,
   209,   206,   207,   341,   341,   115,   340,   340,   339,   128,
   210,   211,   341,    61,    71,     0,     0,     0,     0,   212,
   213,   191,     0,   340,     0,   340,   340,    62,   161,   339,
   339,   339,   339,   341,   151,   152,   153,   154,   155,     0,
   341,     0,     0,     0,   340,     0,     0,     0,     0,     0,
    72,   202,     0,   145,   129,   340,     0,   162,   340,   161,
   340,   161,   163,   340,   148,   341,     0,   340,     0,   340,
     0,   340,   166,    73,   341,     0,   339,   182,   341,   146,
    63,     0,   157,     0,   159,     0,   340,     0,   179,   164,
   208,     0,     0,   341,   149,   340,   340,   340,     0,   340,
   167,    74,   340,   341,   183,     0,     0,     0,   165,     0,
     0,     0,     0,   340,   340,   340,   340,   341,   150,   340,
     0,     0,     0,     0,    75,     0,   156,   340,   340,   168,
   191,   214,     0,     0,    76,   158,   160,     0,   339,     0,
    78,   341,     0,   340,    79,    80,    81,     0,   340,    77,
     0,   341,     0,   341,    82,     0,   341,    83,     0,   341,
    84,   185,    85,   173,    86,   176,    87,   179,    88,     0,
   341,    89,   191,    90,     0,   340,     0,    91,     0,     0,
     0
};

static const short yydefgoto[] = {   759,
     3,     4,     5,     6,    11,    14,    39,    59,    86,    99,
   106,   117,   118,   119,   120,   150,   222,   249,   302,   127,
   128,   129,   130,   165,   267,   315,   375,   445,   493,   524,
   545,   139,   140,   141,   142,   143,   182,   296,   344,   417,
   472,   144,   183,   297,   379,   478,   513,   536,   556,   576,
   602,   624,   145,   184,   298,   347,   451,   497,   515,   547,
   603,   643,   669,   691,   711,   718,   720,   721,   722,   729,
   736,   739,   742,   744,   746,   748,   750,   753,   755,   156,
   157,   158,   159,   201,   321,   382,   453,   520,   541,   171,
   172,   173,   174,   217,   351,   455,   505,   543,   560,   188,
   189,   190,   191,   231,   386,   456,   523,   544,   562,   581,
   599,    19,    20,    21,    22,    31,    71,   110,   111,   112,
   113,   225,   645,   319,   658,   659,   614,   615,   616,   617,
   618,   619,   625,   654,   655,   667,   681,    25,   149,    48,
    70,   342,   343,   415,   416,   210,   211,   673,   420,   294,
   295,   238,   239,   102,   103,   385,   349,   350,   553,   554,
   571,   572,   573,   574,   590,   591,   592,   593,   263,   264,
   265,   291,   311,   277,   305,   389,   390,   391,   357,   393,
   394,   283,   307,   399,   400,   401,   362,   396,   397,   288,
   328,   365,   366,   367,   329,    62,    72,    73,    74,   480,
   499,   500,   501,   336,   409,   437,   438,   439,    63,   266,
   481,    58,    64,    65,    66,   178,   339,   237,   477,    36,
    37,    44,    45,    46,    38,   358,   359,   360,    82,   626,
   278,    17
};

static const short yypact[] = {-32768,
    37,-32768,-32768,    37,-32768,-32768,    47,-32768,-32768,-32768,
-32768,    50,-32768,-32768,     2,-32768,    52,-32768,    58,    52,
-32768,-32768,    95,-32768,-32768,-32768,-32768,-32768,    45,-32768,
-32768,    97,-32768,-32768,-32768,-32768,    31,-32768,    88,    35,
   104,    41,   105,-32768,    31,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   107,-32768,    69,   108,-32768,    96,-32768,
-32768,-32768,    43,-32768,   108,-32768,   111,    64,-32768,-32768,
-32768,-32768,    43,-32768,    46,-32768,-32768,-32768,   107,-32768,
    63,-32768,-32768,-32768,-32768,   118,-32768,    69,    67,-32768,
-32768,    70,-32768,-32768,   107,-32768,-32768,-32768,   119,    71,
-32768,-32768,-32768,-32768,   107,   126,-32768,-32768,-32768,   122,
   126,-32768,-32768,    73,    76,-32768,   124,   122,-32768,-32768,
-32768,-32768,-32768,-32768,   140,-32768,    56,   124,-32768,-32768,
-32768,-32768,   132,-32768,   143,-32768,-32768,-32768,   120,    56,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   145,   147,   148,-32768,   125,   120,-32768,-32768,-32768,
-32768,   149,   118,    90,-32768,-32768,-32768,-32768,   150,-32768,
   128,   125,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    92,-32768,-32768,-32768,-32768,   155,-32768,    93,   128,-32768,
-32768,-32768,-32768,    94,    98,   129,-32768,   100,   101,   102,
-32768,-32768,   165,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   151,-32768,-32768,-32768,   106,-32768,-32768,   110,   115,
    32,   118,-32768,-32768,-32768,    58,    58,    58,-32768,   109,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    74,
-32768,-32768,-32768,-32768,    58,-32768,   121,   116,   119,-32768,
   168,   117,-32768,   117,   117,   117,   117,   117,   117,   117,
   123,   123,-32768,-32768,-32768,   117,   139,-32768,-32768,-32768,
-32768,    58,-32768,-32768,-32768,   171,-32768,   127,-32768,-32768,
-32768,-32768,-32768,   131,-32768,-32768,-32768,-32768,-32768,   130,
-32768,   134,-32768,-32768,-32768,   129,   170,   129,-32768,-32768,
    58,   137,-32768,-32768,-32768,-32768,-32768,   181,-32768,-32768,
-32768,-32768,-32768,    20,   158,-32768,-32768,-32768,-32768,-32768,
   152,-32768,-32768,-32768,    30,   186,-32768,-32768,    43,    28,
-32768,   133,-32768,-32768,   187,-32768,    31,-32768,-32768,   135,
-32768,-32768,-32768,   118,    74,-32768,   118,-32768,-32768,-32768,
   159,-32768,   142,-32768,-32768,   190,   141,-32768,-32768,   204,
   204,   141,   144,   138,-32768,    43,-32768,   153,   210,-32768,
    31,   154,   105,   107,   185,-32768,-32768,-32768,   184,-32768,
   157,   129,-32768,-32768,-32768,   129,-32768,-32768,-32768,   141,
-32768,   156,-32768,-32768,   219,-32768,-32768,   223,-32768,   141,
-32768,   160,-32768,-32768,-32768,-32768,-32768,    43,-32768,-32768,
-32768,    31,-32768,-32768,-32768,-32768,   119,   162,-32768,-32768,
-32768,-32768,-32768,   163,-32768,-32768,   164,-32768,-32768,-32768,
-32768,   166,-32768,-32768,    68,   181,-32768,    43,-32768,   161,
   167,    31,   174,   107,   129,-32768,-32768,-32768,    -4,-32768,
   119,    83,   118,-32768,   129,   118,-32768,    30,    30,   233,
-32768,   186,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   172,-32768,-32768,-32768,-32768,-32768,   139,-32768,-32768,
    43,-32768,    83,-32768,-32768,-32768,-32768,-32768,-32768,   173,
   224,   176,   118,-32768,   177,-32768,   220,   178,-32768,    43,
-32768,   175,-32768,-32768,   118,-32768,-32768,-32768,-32768,-32768,
   182,-32768,   158,-32768,-32768,-32768,-32768,-32768,-32768,   119,
   179,-32768,   119,   119,-32768,-32768,   247,   188,   191,    83,
-32768,-32768,-32768,-32768,-32768,   185,-32768,-32768,-32768,-32768,
   193,   194,   119,   208,   196,-32768,-32768,   197,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   129,   200,-32768,   199,   202,
   203,   209,-32768,   205,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   209,-32768,-32768,-32768,   118,    58,   207,    83,    83,
   225,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   225,-32768,-32768,-32768,-32768,   183,   211,    83,   212,-32768,
-32768,   119,     4,-32768,   118,-32768,-32768,-32768,   241,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   213,
-32768,   216,   218,   221,-32768,   243,    83,    83,    83,    83,
-32768,-32768,   222,   170,-32768,-32768,   226,-32768,-32768,   241,
-32768,   241,   246,-32768,   270,-32768,   228,-32768,   229,-32768,
   230,-32768,   117,-32768,-32768,   231,-32768,   184,-32768,-32768,
-32768,    83,-32768,   232,-32768,   236,-32768,   237,   129,-32768,
-32768,    74,   118,-32768,-32768,-32768,-32768,-32768,   239,-32768,
-32768,-32768,-32768,-32768,-32768,   240,    83,    83,-32768,    83,
   118,   244,   227,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   245,   248,   249,   250,-32768,   253,-32768,-32768,-32768,-32768,
   119,-32768,   255,   256,-32768,-32768,-32768,   267,-32768,    14,
-32768,-32768,   305,-32768,-32768,-32768,-32768,   261,-32768,-32768,
   251,-32768,    58,-32768,-32768,   170,-32768,-32768,   184,-32768,
-32768,   139,-32768,   158,-32768,   185,-32768,   129,-32768,   118,
-32768,-32768,   119,-32768,   263,-32768,   266,-32768,   313,   316,
-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   314,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   215,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   189,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   198,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -383,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   192,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   169,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   180,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   322,-32768,-32768,-32768,-32768,-32768,   234,
-32768,-32768,-32768,  -615,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -587,-32768,-32768,-32768,-32768,  -225,-32768,-32768,
-32768,  -501,-32768,  -521,-32768,  -264,-32768,-32768,  -637,  -470,
-32768,  -327,   -77,  -238,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -229,-32768,-32768,-32768,  -247,-32768,  -332,-32768,
-32768,-32768,-32768,  -188,    54,-32768,-32768,   -43,  -108,-32768,
-32768,  -163,    59,-32768,-32768,   -47,  -107,-32768,-32768,   103,
-32768,-32768,-32768,   -12,   -80,-32768,-32768,-32768,   285,  -467,
-32768,-32768,  -141,-32768,-32768,-32768,-32768,   -75,   279,-32768,
   -19,   -72,-32768,-32768,   303,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,  -313,   -41,   -38,   -65,-32768,   -88,  -347,   728,
     0,   443
};


#define	YYLAST		1447


static const short yytable[] = {     1,
   242,   243,   244,    53,    52,     9,    85,   496,    91,    35,
   275,   526,   378,    15,   546,   504,   377,   405,   646,   271,
   674,   410,    98,   371,    32,    33,   331,    57,   332,   333,
    40,   317,   108,   320,   361,    42,   354,   355,   354,   355,
   719,   609,   610,   611,   612,   613,   300,     2,    56,    32,
    33,    10,   650,    34,   652,    13,    67,    68,   473,   474,
   475,   476,    75,    18,    67,   279,   280,   281,   282,    16,
    81,    24,    75,    60,    61,   323,   354,   355,   251,   136,
   137,   138,   724,    88,    89,   179,    92,    32,    33,   465,
   334,   234,   235,   236,   335,   285,   286,   100,   442,    28,
    41,   740,   356,    47,   356,    49,    43,   114,   115,    50,
    51,   587,    69,    34,    57,  -340,  -340,   423,    77,    84,
   737,   426,   252,   253,   254,   255,   256,   257,   258,   259,
   260,   261,   262,    78,    87,    90,    94,   101,   109,    97,
   104,   116,   123,   124,   126,   134,   148,   151,   155,   164,
   166,   517,   167,   168,   170,   185,   177,   180,   187,   197,
   202,   204,   209,   207,   181,   510,   208,   213,   214,   215,
   218,   276,   224,   229,   293,   303,   246,   194,   447,   232,
   471,   198,   199,   200,   233,   274,   327,  -340,   273,   341,
   484,   318,   361,   287,   676,   370,   348,   304,   388,   309,
   216,   306,   384,   219,   310,   324,  -340,   220,  -332,   404,
   373,   387,   479,  -340,   403,   408,   230,   414,   419,   695,
   696,   430,   697,   411,   747,   431,   407,   422,   429,   508,
   247,   448,   434,   454,   467,   457,   248,   461,   468,   488,
   494,   338,   745,   469,   507,   509,   512,   514,   519,   516,
   532,   525,   537,   552,   604,   570,   538,   284,   284,   284,
   539,   549,   290,   550,   555,   292,   558,   566,   567,   380,
   568,   743,   589,   569,   575,   340,   586,  -339,   605,   638,
   607,   531,   632,   634,   534,   535,   308,   635,   653,   636,
   644,   565,   657,   719,   337,   700,   648,   661,   663,   665,
   671,   413,   677,   325,   551,   326,   678,   680,   689,   330,
   727,   694,   760,   699,   707,   761,   146,     8,   732,   708,
   709,   710,   712,   353,   716,   717,   363,   364,   368,    53,
   730,   756,   131,   369,   412,   758,   725,   160,   372,   683,
   192,    26,   582,   600,   121,   312,   428,   381,   175,   486,
   313,   585,   433,   406,   489,   464,   392,    83,   518,   395,
   398,   402,   466,   608,   289,   368,    93,    76,   205,   463,
   487,   470,     0,     0,     0,   482,     0,   418,   485,     0,
     0,     0,     0,   424,     0,     0,     0,     0,   427,   392,
     0,     0,     0,     0,     0,     0,     0,     0,   432,   402,
    53,     0,   435,     0,   682,     0,   436,   440,   441,     0,
     0,     0,   443,     0,     0,     0,     0,     0,     0,     0,
     0,   452,     0,     0,     0,     0,     0,   522,   458,   459,
   460,     0,     0,   462,     0,     0,     0,   440,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   483,     0,     0,     0,     0,     0,     0,
     0,     0,   490,     0,    27,     0,   491,    30,     0,   492,
     0,     0,   715,     0,     0,     0,   495,     0,     0,   498,
   502,     0,     0,   749,     0,     0,     0,     0,     0,     0,
    55,     0,     0,   511,     0,     0,     0,     0,   584,   502,
     0,     0,     0,   521,     0,     0,     0,   734,     0,     0,
   540,     0,    80,     0,   754,   529,     0,     0,   530,     0,
     0,     0,     0,     0,     0,     0,     0,   621,     0,     0,
     0,   542,     0,    96,     0,     0,     0,   548,     0,     0,
     0,     0,     0,     0,     0,   107,   557,     0,   559,     0,
     0,   561,     0,     0,   564,   122,     0,     0,     0,     0,
   588,     0,   132,     0,     0,     0,   133,   578,   579,     0,
     0,     0,   147,     0,     0,     0,     0,     0,   606,     0,
     0,     0,     0,     0,   161,     0,   596,   597,     0,     0,
     0,   163,     0,     0,     0,   684,     0,     0,     0,     0,
     0,   176,     0,   620,     0,   622,   623,   639,   640,   641,
   642,     0,     0,   698,     0,     0,   193,     0,     0,     0,
     0,   195,   196,     0,   637,     0,     0,     0,     0,     0,
     0,     0,     0,   206,     0,   647,     0,     0,   649,   212,
   651,     0,     0,   656,     0,     0,     0,   662,     0,   664,
     0,   666,   668,   223,     0,   226,   227,   228,     0,     0,
     0,     0,     0,     0,     0,     0,   679,   241,     0,     0,
     0,   245,   751,     0,     0,   686,   687,   688,     0,   690,
     0,   250,   692,     0,   268,   269,   270,     0,   272,     0,
     0,     0,     0,   701,   702,   703,   704,     0,     0,   706,
     0,     0,     0,     0,     0,     0,     0,   713,   714,     0,
     0,     0,     0,   299,     0,   301,     0,     0,     0,     0,
     0,     0,     0,   728,     0,     0,     0,     0,   731,     7,
     0,     0,     0,     0,     0,     0,     0,   316,    12,     0,
     0,     0,   322,     0,     0,    23,     0,     0,     0,     0,
     0,    29,     0,     0,     0,   757,     0,     0,     0,     0,
     0,   346,     0,     0,     0,   352,     0,     0,     0,     0,
     0,     0,     0,     0,    54,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   376,     0,     0,     0,     0,
     0,     0,   383,     0,     0,     0,    79,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    95,     0,     0,
     0,     0,   421,     0,     0,     0,     0,   425,   105,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   125,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   135,     0,     0,     0,     0,   446,     0,
     0,     0,   450,   152,   153,   154,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   162,     0,     0,     0,     0,
     0,     0,   169,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   186,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   203,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   503,     0,     0,   506,     0,     0,
     0,     0,     0,     0,     0,     0,   221,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   240,     0,     0,     0,     0,     0,   528,     0,     0,
     0,     0,     0,     0,   533,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   563,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   577,     0,
     0,     0,     0,     0,     0,     0,   583,     0,     0,     0,
   314,     0,     0,     0,     0,     0,   594,   595,     0,     0,
     0,     0,     0,     0,     0,   601,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   345,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   631,     0,     0,     0,
     0,     0,     0,   633,     0,     0,     0,     0,   374,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   660,     0,
     0,     0,     0,     0,     0,     0,     0,   670,     0,     0,
     0,   675,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   685,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   693,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   705,   444,     0,     0,     0,     0,   449,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   726,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   733,     0,   735,     0,     0,   738,
     0,     0,   741,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   752,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   527,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   580,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   598,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   627,   628,   629,
   630,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   672,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   723
};

static const short yycheck[] = {     0,
   226,   227,   228,    45,    43,     6,    79,   478,    86,    29,
   249,   513,   345,    14,   536,   483,   344,   365,   634,   245,
   658,   369,    95,   337,     5,     6,     7,     8,     9,    10,
    31,   296,   105,   298,     7,    36,     9,    10,     9,    10,
    27,    38,    39,    40,    41,    42,   272,    11,    49,     5,
     6,     5,   640,     9,   642,     6,    57,    58,    63,    64,
    65,    66,    63,    12,    65,   254,   255,   256,   257,    68,
    71,    14,    73,     5,     6,   301,     9,    10,     5,    24,
    25,    26,    69,    84,    85,   163,    87,     5,     6,   437,
    71,    60,    61,    62,    75,   259,   260,    98,   412,     5,
     4,   739,    75,    16,    75,    71,    76,   108,   109,     6,
    70,   579,    17,     9,     8,     8,    74,   382,     8,    74,
   736,   386,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    70,    72,    18,    70,    19,    13,    70,
    70,    20,    70,    68,    21,     6,    15,     5,    29,   150,
     6,   499,     6,     6,    30,     6,     8,    68,    31,    68,
     6,    69,    34,    70,   165,   493,    69,    68,    68,    68,
     6,     4,    22,    68,    36,     5,    68,   178,   417,    70,
   445,   182,   183,   184,    70,    70,     6,    71,    68,    32,
   455,    22,     7,    71,   662,     9,    45,    71,     9,    70,
   201,    71,    44,   204,    71,    69,     3,   208,    76,    72,
    76,    70,   451,    73,    71,     6,   217,    33,    35,   687,
   688,     3,   690,    70,   746,     3,    74,    71,    73,     6,
   231,    70,    73,    71,    74,    72,   237,    72,    72,     7,
    69,   314,   744,    70,    72,    70,    70,    28,    74,    72,
    72,    70,     6,    46,    72,    47,    69,   258,   259,   260,
    70,    69,   263,    70,    69,   266,    70,    68,    70,   347,
    69,   742,    48,    71,    70,   314,    70,    37,    68,    37,
    69,   520,    70,    68,   523,   524,   287,    70,    43,    69,
    69,   556,    23,    27,   314,    69,    71,    70,    70,    70,
    70,   374,    71,   304,   543,   306,    71,    71,    70,   310,
     6,    72,     0,    70,    70,     0,   128,     4,    68,    72,
    72,    72,    70,   324,    70,    70,   327,   328,   329,   371,
    70,    69,   118,   334,   373,    70,   720,   140,   339,   672,
   172,    20,   572,   591,   111,   292,   390,   348,   157,   458,
   292,   577,   400,   366,   462,   436,   357,    73,   500,   360,
   361,   362,   438,   602,   262,   366,    88,    65,   189,   435,
   459,   444,    -1,    -1,    -1,   453,    -1,   378,   456,    -1,
    -1,    -1,    -1,   384,    -1,    -1,    -1,    -1,   389,   390,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   399,   400,
   442,    -1,   403,    -1,   669,    -1,   407,   408,   409,    -1,
    -1,    -1,   413,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   422,    -1,    -1,    -1,    -1,    -1,   505,   429,   430,
   431,    -1,    -1,   434,    -1,    -1,    -1,   438,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   454,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   463,    -1,    22,    -1,   467,    25,    -1,   470,
    -1,    -1,   711,    -1,    -1,    -1,   477,    -1,    -1,   480,
   481,    -1,    -1,   748,    -1,    -1,    -1,    -1,    -1,    -1,
    48,    -1,    -1,   494,    -1,    -1,    -1,    -1,   576,   500,
    -1,    -1,    -1,   504,    -1,    -1,    -1,   733,    -1,    -1,
   530,    -1,    70,    -1,   753,   516,    -1,    -1,   519,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   605,    -1,    -1,
    -1,   532,    -1,    91,    -1,    -1,    -1,   538,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   103,   547,    -1,   549,    -1,
    -1,   552,    -1,    -1,   555,   113,    -1,    -1,    -1,    -1,
   580,    -1,   120,    -1,    -1,    -1,   124,   568,   569,    -1,
    -1,    -1,   130,    -1,    -1,    -1,    -1,    -1,   598,    -1,
    -1,    -1,    -1,    -1,   142,    -1,   587,   588,    -1,    -1,
    -1,   149,    -1,    -1,    -1,   673,    -1,    -1,    -1,    -1,
    -1,   159,    -1,   604,    -1,   606,   607,   627,   628,   629,
   630,    -1,    -1,   691,    -1,    -1,   174,    -1,    -1,    -1,
    -1,   179,   180,    -1,   625,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   191,    -1,   636,    -1,    -1,   639,   197,
   641,    -1,    -1,   644,    -1,    -1,    -1,   648,    -1,   650,
    -1,   652,   653,   211,    -1,   213,   214,   215,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   667,   225,    -1,    -1,
    -1,   229,   750,    -1,    -1,   676,   677,   678,    -1,   680,
    -1,   239,   683,    -1,   242,   243,   244,    -1,   246,    -1,
    -1,    -1,    -1,   694,   695,   696,   697,    -1,    -1,   700,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   708,   709,    -1,
    -1,    -1,    -1,   271,    -1,   273,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   724,    -1,    -1,    -1,    -1,   729,     2,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   295,    11,    -1,
    -1,    -1,   300,    -1,    -1,    18,    -1,    -1,    -1,    -1,
    -1,    24,    -1,    -1,    -1,   756,    -1,    -1,    -1,    -1,
    -1,   319,    -1,    -1,    -1,   323,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   343,    -1,    -1,    -1,    -1,
    -1,    -1,   350,    -1,    -1,    -1,    69,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,
    -1,    -1,   380,    -1,    -1,    -1,    -1,   385,   101,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   116,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   126,    -1,    -1,    -1,    -1,   416,    -1,
    -1,    -1,   420,   136,   137,   138,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   148,    -1,    -1,    -1,    -1,
    -1,    -1,   155,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   170,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   187,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   482,    -1,    -1,   485,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   209,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   224,    -1,    -1,    -1,    -1,    -1,   515,    -1,    -1,
    -1,    -1,    -1,    -1,   522,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   554,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   566,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   574,    -1,    -1,    -1,
   293,    -1,    -1,    -1,    -1,    -1,   584,   585,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   593,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   318,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   614,    -1,    -1,    -1,
    -1,    -1,    -1,   621,    -1,    -1,    -1,    -1,   341,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   646,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   655,    -1,    -1,
    -1,   659,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   674,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   684,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   698,   414,    -1,    -1,    -1,    -1,   419,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   722,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   732,    -1,   734,    -1,    -1,   737,
    -1,    -1,   740,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   751,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   514,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   570,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   589,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   610,   611,   612,
   613,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   657,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   719
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
#line 517 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 527 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 531 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 537 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 541 "parser-sming.y"
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
#line 556 "parser-sming.y"
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
#line 570 "parser-sming.y"
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
#line 591 "parser-sming.y"
{
			    thisParserPtr->identityObjectName = yyvsp[0].text;
			;
    break;}
case 9:
#line 597 "parser-sming.y"
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
#line 624 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 11:
#line 631 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 12:
#line 638 "parser-sming.y"
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
#line 648 "parser-sming.y"
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
#line 665 "parser-sming.y"
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
#line 717 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 16:
#line 721 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed extension statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 17:
#line 731 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 18:
#line 736 "parser-sming.y"
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
#line 751 "parser-sming.y"
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
#line 765 "parser-sming.y"
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
#line 775 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].status) {
				setMacroStatus(macroPtr, yyvsp[0].status);
			    }
			;
    break;}
case 22:
#line 781 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroDescription(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 23:
#line 787 "parser-sming.y"
{
			    if (macroPtr && yyvsp[0].text) {
				setMacroReference(macroPtr, yyvsp[0].text);
			    }
			;
    break;}
case 24:
#line 793 "parser-sming.y"
{
			    yyval.macroPtr = 0;
			    macroPtr = NULL;
			    free(macroIdentifier);
			;
    break;}
case 25:
#line 800 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 26:
#line 804 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 27:
#line 814 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 28:
#line 819 "parser-sming.y"
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
#line 834 "parser-sming.y"
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
#line 848 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 31:
#line 853 "parser-sming.y"
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
#line 872 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 33:
#line 878 "parser-sming.y"
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
#line 888 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 35:
#line 894 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 36:
#line 900 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 37:
#line 906 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 38:
#line 912 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 39:
#line 920 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 40:
#line 924 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 41:
#line 934 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 42:
#line 939 "parser-sming.y"
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
#line 954 "parser-sming.y"
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
#line 973 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 48:
#line 978 "parser-sming.y"
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
#line 992 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 50:
#line 998 "parser-sming.y"
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
#line 1011 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 52:
#line 1017 "parser-sming.y"
{
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 53:
#line 1025 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 54:
#line 1030 "parser-sming.y"
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
#line 1043 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].typePtr) {
				setObjectType(scalarObjectPtr, yyvsp[-1].typePtr);
				setObjectBasetype(scalarObjectPtr,
						  yyvsp[-1].typePtr->export.basetype);
				defaultBasetype = yyvsp[-1].typePtr->export.basetype;
				if (!(yyvsp[-1].typePtr->export.name)) {
				    /*
				     * An inlined type.
				     */
				    setTypeName(yyvsp[-1].typePtr, scalarIdentifier);
				}
			    }
			;
    break;}
case 56:
#line 1058 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 57:
#line 1064 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 58:
#line 1070 "parser-sming.y"
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
#line 1080 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 60:
#line 1086 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 61:
#line 1092 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 62:
#line 1098 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 63:
#line 1104 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 64:
#line 1113 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 65:
#line 1118 "parser-sming.y"
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
#line 1133 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 67:
#line 1139 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 68:
#line 1145 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 69:
#line 1152 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 70:
#line 1160 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 71:
#line 1165 "parser-sming.y"
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
#line 1180 "parser-sming.y"
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
#line 1204 "parser-sming.y"
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
#line 1215 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 75:
#line 1221 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 76:
#line 1227 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 77:
#line 1234 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 78:
#line 1242 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 79:
#line 1250 "parser-sming.y"
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
#line 1265 "parser-sming.y"
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
#line 1279 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 82:
#line 1284 "parser-sming.y"
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
#line 1297 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].typePtr) {
				setObjectType(columnObjectPtr, yyvsp[-1].typePtr);
				setObjectBasetype(columnObjectPtr,
						  yyvsp[-1].typePtr->export.basetype);
				defaultBasetype = yyvsp[-1].typePtr->export.basetype;
				if (!(yyvsp[-1].typePtr->export.name)) {
				    /*
				     * An inlined type.
				     */
				    setTypeName(yyvsp[-1].typePtr, columnIdentifier);
				}
			    }
			;
    break;}
case 84:
#line 1312 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 85:
#line 1318 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 86:
#line 1324 "parser-sming.y"
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
#line 1334 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 88:
#line 1340 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 89:
#line 1346 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 90:
#line 1352 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 91:
#line 1358 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 92:
#line 1367 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 93:
#line 1371 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 94:
#line 1381 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 95:
#line 1386 "parser-sming.y"
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
#line 1401 "parser-sming.y"
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
#line 1415 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 98:
#line 1420 "parser-sming.y"
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
#line 1434 "parser-sming.y"
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
#line 1450 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 101:
#line 1456 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 102:
#line 1463 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 103:
#line 1469 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 104:
#line 1477 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 105:
#line 1481 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 106:
#line 1491 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 107:
#line 1495 "parser-sming.y"
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
#line 1510 "parser-sming.y"
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
#line 1524 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 110:
#line 1529 "parser-sming.y"
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
#line 1541 "parser-sming.y"
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
#line 1557 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 113:
#line 1563 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 114:
#line 1569 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 115:
#line 1575 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 116:
#line 1583 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 117:
#line 1587 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 118:
#line 1597 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 119:
#line 1602 "parser-sming.y"
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
#line 1617 "parser-sming.y"
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
#line 1631 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 122:
#line 1636 "parser-sming.y"
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
#line 1650 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 124:
#line 1656 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 125:
#line 1662 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 126:
#line 1668 "parser-sming.y"
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
#line 1684 "parser-sming.y"
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
#line 1700 "parser-sming.y"
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
		    s = util_malloc(strlen(refinementPtr->objectPtr->export.name) +
				    strlen(complianceIdentifier) + 13);
				    if (refinementPtr->typePtr) {
					sprintf(s, "%s+%s+type",
						complianceIdentifier,
					       refinementPtr->objectPtr->export.name);
					setTypeName(refinementPtr->typePtr, s);
					
				    }
				    if (refinementPtr->writetypePtr) {
					sprintf(s, "%s+%s+writetype",
						complianceIdentifier,
					       refinementPtr->objectPtr->export.name);
				   setTypeName(refinementPtr->writetypePtr, s);
				    }
				    util_free(s);
				}
			    }
			;
    break;}
case 129:
#line 1734 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 130:
#line 1742 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 131:
#line 1746 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 132:
#line 1752 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 133:
#line 1756 "parser-sming.y"
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
#line 1771 "parser-sming.y"
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
#line 1785 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 136:
#line 1790 "parser-sming.y"
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
#line 1802 "parser-sming.y"
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
#line 1817 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 139:
#line 1821 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 140:
#line 1827 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 141:
#line 1831 "parser-sming.y"
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
#line 1846 "parser-sming.y"
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
#line 1863 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(yyvsp[-6].date, yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 144:
#line 1871 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 145:
#line 1877 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 146:
#line 1881 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 147:
#line 1888 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 148:
#line 1894 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 149:
#line 1898 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 150:
#line 1905 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 156:
#line 1919 "parser-sming.y"
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
#line 1939 "parser-sming.y"
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
#line 1957 "parser-sming.y"
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
#line 1977 "parser-sming.y"
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
#line 1995 "parser-sming.y"
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
#line 2015 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 162:
#line 2019 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 163:
#line 2025 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 164:
#line 2029 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 165:
#line 2035 "parser-sming.y"
{
			    if (rowObjectPtr) {
				addObjectFlags(rowObjectPtr, FLAG_CREATABLE);
				setObjectCreate(rowObjectPtr, 1);
			    }
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 166:
#line 2045 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 167:
#line 2049 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 168:
#line 2055 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 169:
#line 2061 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 170:
#line 2067 "parser-sming.y"
{
			    yyval.date = yyvsp[-2].date;
			;
    break;}
case 171:
#line 2073 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 172:
#line 2079 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 173:
#line 2085 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 174:
#line 2089 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 175:
#line 2095 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 176:
#line 2101 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 177:
#line 2105 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 178:
#line 2111 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 179:
#line 2117 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 180:
#line 2121 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 181:
#line 2127 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 182:
#line 2133 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 183:
#line 2137 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 184:
#line 2143 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 185:
#line 2149 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 186:
#line 2153 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 187:
#line 2159 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 188:
#line 2165 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 189:
#line 2169 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 190:
#line 2175 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 191:
#line 2181 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 192:
#line 2185 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 193:
#line 2191 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 194:
#line 2197 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 195:
#line 2201 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 196:
#line 2208 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 197:
#line 2214 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 198:
#line 2218 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 199:
#line 2225 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 200:
#line 2231 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 201:
#line 2235 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 202:
#line 2242 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 203:
#line 2248 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 204:
#line 2252 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 205:
#line 2258 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 206:
#line 2265 "parser-sming.y"
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
#line 2278 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 208:
#line 2287 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text,
						       thisParserPtr,
						       thisModulePtr);
			    yyval.optionPtr->export.description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 209:
#line 2297 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 210:
#line 2301 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 211:
#line 2307 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 212:
#line 2313 "parser-sming.y"
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
#line 2326 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 214:
#line 2337 "parser-sming.y"
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
#line 2362 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 216:
#line 2366 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 217:
#line 2372 "parser-sming.y"
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
#line 2387 "parser-sming.y"
{
			    yyval.typePtr = typeObjectIdentifierPtr;
			;
    break;}
case 219:
#line 2391 "parser-sming.y"
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
#line 2406 "parser-sming.y"
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
#line 2421 "parser-sming.y"
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
#line 2436 "parser-sming.y"
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
#line 2451 "parser-sming.y"
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
#line 2466 "parser-sming.y"
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
#line 2481 "parser-sming.y"
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
#line 2496 "parser-sming.y"
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
#line 2511 "parser-sming.y"
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
#line 2528 "parser-sming.y"
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
#line 2542 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 230:
#line 2546 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 231:
#line 2552 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 232:
#line 2556 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 233:
#line 2562 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 234:
#line 2566 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 235:
#line 2573 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 236:
#line 2581 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 237:
#line 2585 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 238:
#line 2591 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 239:
#line 2597 "parser-sming.y"
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
#line 2610 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 241:
#line 2616 "parser-sming.y"
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
#line 2630 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 243:
#line 2634 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 244:
#line 2640 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 245:
#line 2646 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 246:
#line 2650 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 247:
#line 2657 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 248:
#line 2665 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 249:
#line 2669 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 250:
#line 2675 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 251:
#line 2681 "parser-sming.y"
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
#line 2694 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 253:
#line 2700 "parser-sming.y"
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
#line 2719 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 255:
#line 2723 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 256:
#line 2729 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 257:
#line 2735 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 258:
#line 2742 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 259:
#line 2750 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 260:
#line 2754 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 261:
#line 2760 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 262:
#line 2767 "parser-sming.y"
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
#line 2781 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 264:
#line 2787 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->export.name = yyvsp[-6].text;
			    yyval.namedNumberPtr->export.value = *yyvsp[-2].valuePtr;
			    util_free(yyvsp[-2].valuePtr);
			;
    break;}
case 265:
#line 2796 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 266:
#line 2804 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 267:
#line 2808 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 268:
#line 2814 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 269:
#line 2820 "parser-sming.y"
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
#line 2833 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 271:
#line 2839 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 272:
#line 2847 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 273:
#line 2851 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 274:
#line 2857 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 275:
#line 2863 "parser-sming.y"
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
#line 2876 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 277:
#line 2882 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 278:
#line 2890 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 279:
#line 2894 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 280:
#line 2900 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 281:
#line 2906 "parser-sming.y"
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
#line 2919 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 283:
#line 2925 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 284:
#line 2931 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 285:
#line 2935 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 286:
#line 2943 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 287:
#line 2947 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 288:
#line 2953 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 289:
#line 2959 "parser-sming.y"
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
#line 2972 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 291:
#line 2978 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
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
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 294:
#line 2992 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 295:
#line 2998 "parser-sming.y"
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
#line 3009 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 297:
#line 3015 "parser-sming.y"
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
#line 3026 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 299:
#line 3032 "parser-sming.y"
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
#line 3046 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 301:
#line 3050 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 302:
#line 3056 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 303:
#line 3060 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 304:
#line 3070 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 305:
#line 3076 "parser-sming.y"
{
			    yyval.date = checkDate(thisParserPtr, yyvsp[0].text);
			;
    break;}
case 306:
#line 3082 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 307:
#line 3088 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 308:
#line 3101 "parser-sming.y"
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
#line 3128 "parser-sming.y"
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
#line 3174 "parser-sming.y"
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
#line 3196 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 312:
#line 3202 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 313:
#line 3208 "parser-sming.y"
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
#line 3221 "parser-sming.y"
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
#line 3245 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 316:
#line 3250 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 317:
#line 3257 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 318:
#line 3261 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 319:
#line 3265 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 320:
#line 3271 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 321:
#line 3275 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 322:
#line 3279 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 323:
#line 3283 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 324:
#line 3289 "parser-sming.y"
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
#line 3318 "parser-sming.y"
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
#line 3356 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 327:
#line 3362 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 328:
#line 3366 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 329:
#line 3373 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 330:
#line 3377 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 331:
#line 3387 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 332:
#line 3396 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 333:
#line 3402 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 0);
			;
    break;}
case 334:
#line 3410 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 335:
#line 3419 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->format = SMI_VALUEFORMAT_NATIVE;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 336:
#line 3428 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 337:
#line 3432 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 338:
#line 3442 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 339:
#line 3449 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 340:
#line 3455 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 341:
#line 3461 "parser-sming.y"
{
			    yyval.rc = 0;
			;
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
#line 3466 "parser-sming.y"


#endif
			
