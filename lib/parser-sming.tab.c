
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
#define	typedefKeyword	274
#define	typeKeyword	275
#define	writetypeKeyword	276
#define	nodeKeyword	277
#define	scalarKeyword	278
#define	tableKeyword	279
#define	columnKeyword	280
#define	rowKeyword	281
#define	notificationKeyword	282
#define	groupKeyword	283
#define	complianceKeyword	284
#define	formatKeyword	285
#define	unitsKeyword	286
#define	statusKeyword	287
#define	accessKeyword	288
#define	defaultKeyword	289
#define	impliedKeyword	290
#define	indexKeyword	291
#define	augmentsKeyword	292
#define	reordersKeyword	293
#define	sparseKeyword	294
#define	expandsKeyword	295
#define	createKeyword	296
#define	membersKeyword	297
#define	objectsKeyword	298
#define	mandatoryKeyword	299
#define	optionalKeyword	300
#define	refineKeyword	301
#define	OctetStringKeyword	302
#define	ObjectIdentifierKeyword	303
#define	Integer32Keyword	304
#define	Unsigned32Keyword	305
#define	Integer64Keyword	306
#define	Unsigned64Keyword	307
#define	Float32Keyword	308
#define	Float64Keyword	309
#define	Float128Keyword	310
#define	BitsKeyword	311
#define	EnumerationKeyword	312
#define	currentKeyword	313
#define	deprecatedKeyword	314
#define	obsoleteKeyword	315
#define	noaccessKeyword	316
#define	notifyonlyKeyword	317
#define	readonlyKeyword	318
#define	readwriteKeyword	319

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



static char *typeIdentifier, *nodeIdentifier,
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
static SmiBasetype defaultBasetype = SMI_BASETYPE_UNKNOWN;
 
 

Type *
findType(spec, parserPtr)
    char *spec;
    Parser *parserPtr; 
{
    Type *typePtr;
    Import *importPtr;
    char *module, *type;
    
    if (!strstr(spec, "::")) {
	typePtr = findTypeByModuleAndName(thisModulePtr, spec);
	if (!typePtr) {
	    importPtr = findImportByName(spec, thisModulePtr);
	    if (importPtr) {
		typePtr = findTypeByModulenameAndName(importPtr->importmodule,
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
 

			    
Object *
findObject(spec, parserPtr)
    char *spec;
    Parser *parserPtr; 
{
    Object *objectPtr;
    Import *importPtr;
    char *module, *object;
    
    if (!strstr(spec, "::")) {
	objectPtr = findObjectByModuleAndName(thisModulePtr, spec);
	if (!objectPtr) {
	    importPtr = findImportByName(spec, thisModulePtr);
	    if (importPtr) {
	     objectPtr = findObjectByModulenameAndName(importPtr->importmodule,
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
 

			    

#line 155 "parser-sming.y"
typedef union {
    char           *id;				/* identifier name           */
    int            rc;				/* >=0: ok, <0: error        */
    char	   *text;
    Module	   *modulePtr;
    Node	   *nodePtr;
    Object	   *objectPtr;
    Type	   *typePtr;
    Index	   *indexPtr;
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



#define	YYFINAL		739
#define	YYFLAG		-32768
#define	YYNTBASE	75

#define YYTRANSLATE(x) ((unsigned)(x) <= 319 ? yytranslate[x] : 300)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    69,
    70,     2,     2,    72,    73,    74,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    68,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    66,    71,    67,     2,     2,     2,     2,     2,
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
    57,    58,    59,    60,    61,    62,    63,    64,    65
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     4,     6,     8,    11,    14,    15,    16,    17,
    18,    19,    20,    21,    56,    57,    59,    61,    64,    67,
    68,    69,    70,    71,    72,    73,    74,    75,   101,   102,
   104,   106,   109,   112,   114,   116,   118,   119,   120,   121,
   122,   123,   143,   144,   145,   146,   147,   148,   149,   150,
   151,   152,   153,   186,   187,   188,   189,   190,   191,   214,
   215,   216,   217,   218,   219,   220,   221,   248,   250,   253,
   256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
   266,   299,   300,   302,   304,   307,   310,   311,   312,   313,
   314,   315,   316,   339,   340,   342,   344,   347,   350,   351,
   352,   353,   354,   355,   356,   380,   381,   383,   385,   388,
   391,   392,   393,   394,   395,   396,   397,   398,   399,   426,
   427,   429,   431,   434,   437,   438,   439,   453,   454,   456,
   458,   461,   464,   476,   482,   483,   486,   492,   493,   496,
   502,   504,   506,   508,   510,   512,   523,   529,   542,   548,
   561,   562,   565,   566,   569,   574,   575,   578,   584,   590,
   596,   602,   608,   609,   612,   618,   619,   622,   628,   629,
   632,   638,   639,   642,   648,   649,   652,   658,   659,   662,
   668,   669,   672,   678,   679,   682,   692,   693,   696,   706,
   707,   710,   720,   721,   723,   725,   728,   731,   742,   743,
   745,   747,   750,   753,   767,   769,   771,   774,   776,   779,
   782,   785,   788,   791,   794,   797,   800,   803,   806,   807,
   810,   812,   814,   815,   818,   825,   826,   828,   830,   833,
   838,   841,   842,   844,   849,   850,   853,   860,   861,   863,
   865,   868,   873,   876,   877,   879,   884,   890,   894,   895,
   897,   899,   902,   907,   915,   919,   920,   922,   924,   927,
   932,   936,   937,   939,   941,   944,   949,   953,   954,   956,
   958,   961,   966,   972,   974,   978,   979,   981,   983,   986,
   991,   993,   995,   997,   999,  1003,  1005,  1009,  1011,  1014,
  1015,  1017,  1019,  1022,  1025,  1027,  1029,  1031,  1033,  1035,
  1038,  1040,  1042,  1044,  1046,  1049,  1054,  1056,  1058,  1060,
  1062,  1064,  1066,  1068,  1071,  1073,  1075,  1076,  1078,  1080,
  1083,  1086,  1088,  1090,  1092,  1095,  1097,  1099,  1100,  1101,
  1102
};

static const short yyrhs[] = {   298,
    76,     0,     0,    77,     0,    78,     0,    77,    78,     0,
    79,   298,     0,     0,     0,     0,     0,     0,     0,     0,
    11,   297,     5,    80,   297,     6,    81,   298,    66,   299,
   179,   205,   299,    82,   207,   299,    83,   208,   299,    84,
   220,   299,    85,   221,    86,   185,    87,    99,   147,   157,
   167,    67,   298,    68,     0,     0,    88,     0,    89,     0,
    88,    89,     0,    90,   299,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    20,   297,     5,    91,   298,    66,
   299,   189,   299,    92,   217,    93,   209,    94,   211,    95,
   213,    96,   219,    97,   221,    98,    67,   298,    68,     0,
     0,   100,     0,   101,     0,   100,   101,     0,   102,   299,
     0,   103,     0,   109,     0,   120,     0,     0,     0,     0,
     0,     0,    23,   297,     6,   104,   298,    66,   299,   205,
   299,   105,   213,   106,   219,   107,   221,   108,    67,   298,
    68,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    24,   297,     6,   110,   298,    66,   299,   205,
   299,   111,   191,   299,   112,   216,   299,   113,   217,   114,
   209,   115,   211,   116,   213,   117,   220,   299,   118,   221,
   119,    67,   298,    68,     0,     0,     0,     0,     0,     0,
    25,   297,     6,   121,   298,    66,   299,   205,   299,   122,
   213,   123,   220,   299,   124,   221,   125,   126,   299,    67,
   298,    68,     0,     0,     0,     0,     0,     0,     0,     0,
    27,   297,     6,   127,   298,    66,   299,   205,   299,   128,
   194,   299,   129,   201,   130,   213,   131,   220,   299,   132,
   221,   133,   134,    67,   298,    68,     0,   135,     0,   134,
   135,     0,   136,   299,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    26,   297,     6,   137,   298,
    66,   299,   205,   299,   138,   191,   299,   139,   216,   299,
   140,   217,   141,   209,   142,   211,   143,   213,   144,   220,
   299,   145,   221,   146,    67,   298,    68,     0,     0,   148,
     0,   149,     0,   148,   149,     0,   150,   299,     0,     0,
     0,     0,     0,     0,     0,    28,   297,     6,   151,   298,
    66,   299,   205,   299,   152,   224,   153,   213,   154,   220,
   299,   155,   221,   156,    67,   298,    68,     0,     0,   158,
     0,   159,     0,   158,   159,     0,   160,   299,     0,     0,
     0,     0,     0,     0,     0,    29,   297,     6,   161,   298,
    66,   299,   205,   299,   162,   223,   299,   163,   213,   164,
   220,   299,   165,   221,   166,    67,   298,    68,     0,     0,
   168,     0,   169,     0,   168,   169,     0,   170,   299,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    30,   297,
     6,   171,   298,    66,   299,   205,   299,   172,   213,   173,
   220,   299,   174,   221,   175,   226,   176,   228,   177,   232,
   178,    67,   298,    68,     0,     0,   180,     0,   181,     0,
   180,   181,     0,   182,   299,     0,     0,     0,    12,   297,
     5,   183,   298,    69,   298,   263,   184,   298,    70,   298,
    68,     0,     0,   186,     0,   187,     0,   186,   187,     0,
   188,   299,     0,    13,   298,    66,   299,   206,   299,   220,
   299,    67,   298,    68,     0,    21,   297,   236,   298,    68,
     0,     0,   191,   299,     0,    21,   297,   236,   298,    68,
     0,     0,   193,   299,     0,    22,   297,   236,   298,    68,
     0,   195,     0,   196,     0,   197,     0,   198,     0,   199,
     0,    37,   200,   298,    69,   298,   267,   298,    70,   298,
    68,     0,    38,   297,   278,   298,    68,     0,    39,   297,
   278,   200,   298,    69,   298,   267,   298,    70,   298,    68,
     0,    40,   297,   278,   298,    68,     0,    41,   297,   278,
   200,   298,    69,   298,   267,   298,    70,   298,    68,     0,
     0,   297,    36,     0,     0,   202,   299,     0,    42,   203,
   298,    68,     0,     0,   298,   204,     0,    69,   298,   267,
   298,    70,     0,    14,   297,   287,   298,    68,     0,    15,
   297,   283,   298,    68,     0,    16,   297,   279,   298,    68,
     0,    17,   297,   279,   298,    68,     0,     0,   210,   299,
     0,    31,   297,   279,   298,    68,     0,     0,   212,   299,
     0,    32,   297,   279,   298,    68,     0,     0,   214,   299,
     0,    33,   297,   285,   298,    68,     0,     0,   216,   299,
     0,    34,   297,   286,   298,    68,     0,     0,   218,   299,
     0,    35,   297,   284,   298,    68,     0,     0,   220,   299,
     0,    18,   297,   279,   298,    68,     0,     0,   222,   299,
     0,    19,   297,   279,   298,    68,     0,     0,   223,   299,
     0,    43,   298,    69,   298,   267,   298,    70,   298,    68,
     0,     0,   225,   299,     0,    44,   298,    69,   298,   267,
   298,    70,   298,    68,     0,     0,   227,   299,     0,    45,
   298,    69,   298,   267,   298,    70,   298,    68,     0,     0,
   229,     0,   230,     0,   229,   230,     0,   231,   299,     0,
    46,   297,   278,   298,    66,   220,   299,    67,   298,    68,
     0,     0,   233,     0,   234,     0,   233,   234,     0,   235,
   299,     0,    47,   297,   278,   298,    66,   190,   192,   215,
   220,   299,    67,   298,    68,     0,   237,     0,   238,     0,
    48,   241,     0,    49,     0,    50,   241,     0,    51,   241,
     0,    52,   241,     0,    53,   241,     0,    54,   249,     0,
    55,   249,     0,    56,   249,     0,    58,   257,     0,    57,
   257,     0,   277,   239,     0,     0,   298,   240,     0,   242,
     0,   250,     0,     0,   298,   242,     0,    69,   298,   246,
   243,   298,    70,     0,     0,   244,     0,   245,     0,   244,
   245,     0,   298,    71,   298,   246,     0,   295,   247,     0,
     0,   248,     0,   298,     3,   298,   295,     0,     0,   298,
   250,     0,    69,   298,   254,   251,   298,    70,     0,     0,
   252,     0,   253,     0,   252,   253,     0,   298,    71,   298,
   254,     0,     7,   255,     0,     0,   256,     0,   298,     3,
   298,     7,     0,    69,   298,   258,   298,    70,     0,   262,
   259,   296,     0,     0,   260,     0,   261,     0,   260,   261,
     0,   298,    72,   298,   262,     0,     6,   298,    69,   298,
   293,   298,    70,     0,   276,   264,   296,     0,     0,   265,
     0,   266,     0,   265,   266,     0,   298,    72,   298,   276,
     0,     0,     0,   296,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   298,    72,   298,     0,     0,   278,   268,
   296,     0,     0,   269,     0,   270,     0,   269,   270,     0,
   298,    72,   298,   278,     0,    69,   298,   272,   298,    70,
     0,   296,     0,     6,   273,   296,     0,     0,   274,     0,
   275,     0,   274,   275,     0,   298,    72,   298,     6,     0,
     5,     0,     6,     0,   277,     0,   278,     0,     5,     4,
     5,     0,     5,     0,     5,     4,     6,     0,     6,     0,
     8,   280,     0,     0,   281,     0,   282,     0,   281,   282,
     0,   298,     8,     0,     8,     0,     8,     0,     8,     0,
   271,     0,     9,     0,    73,     9,     0,    10,     0,     7,
     0,   279,     0,   278,     0,   278,   290,     0,   292,    74,
   292,   290,     0,    59,     0,    60,     0,    61,     0,    62,
     0,    63,     0,    64,     0,    65,     0,   288,   289,     0,
   278,     0,   292,     0,     0,   290,     0,   291,     0,   290,
   291,     0,    74,   292,     0,     9,     0,    10,     0,     9,
     0,    73,     9,     0,   293,     0,   294,     0,     0,     0,
     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   424,   434,   438,   444,   448,   463,   477,   497,   509,   540,
   547,   554,   561,   567,   621,   625,   635,   639,   655,   669,
   673,   694,   700,   706,   712,   718,   724,   729,   737,   741,
   751,   755,   771,   785,   786,   787,   790,   794,   808,   814,
   827,   832,   840,   844,   856,   869,   876,   882,   888,   894,
   899,   906,   911,   920,   924,   939,   944,   951,   956,   965,
   969,   983,  1003,  1013,  1018,  1025,  1031,  1039,  1047,  1062,
  1076,  1080,  1092,  1105,  1112,  1118,  1124,  1130,  1135,  1142,
  1147,  1156,  1160,  1170,  1174,  1190,  1204,  1208,  1222,  1237,
  1242,  1250,  1255,  1263,  1267,  1277,  1281,  1296,  1310,  1314,
  1325,  1341,  1346,  1353,  1358,  1366,  1370,  1380,  1384,  1400,
  1414,  1418,  1432,  1437,  1444,  1450,  1465,  1469,  1497,  1505,
  1509,  1515,  1519,  1534,  1548,  1552,  1565,  1578,  1582,  1588,
  1592,  1607,  1621,  1630,  1638,  1642,  1648,  1655,  1659,  1665,
  1672,  1673,  1674,  1675,  1676,  1679,  1701,  1718,  1741,  1758,
  1781,  1785,  1791,  1795,  1801,  1810,  1814,  1820,  1826,  1832,
  1838,  1844,  1850,  1854,  1860,  1866,  1870,  1876,  1882,  1886,
  1892,  1898,  1902,  1908,  1914,  1918,  1924,  1930,  1934,  1940,
  1946,  1950,  1956,  1962,  1966,  1972,  1979,  1983,  1989,  1996,
  2000,  2006,  2013,  2017,  2023,  2029,  2043,  2049,  2060,  2064,
  2070,  2076,  2089,  2095,  2123,  2127,  2133,  2145,  2149,  2161,
  2173,  2185,  2197,  2209,  2221,  2233,  2245,  2259,  2278,  2282,
  2288,  2292,  2298,  2302,  2308,  2317,  2321,  2327,  2333,  2346,
  2352,  2364,  2368,  2374,  2380,  2384,  2390,  2399,  2403,  2409,
  2415,  2428,  2434,  2453,  2457,  2463,  2469,  2475,  2484,  2488,
  2494,  2500,  2515,  2521,  2529,  2537,  2541,  2547,  2553,  2566,
  2572,  2580,  2584,  2590,  2596,  2609,  2615,  2623,  2627,  2633,
  2639,  2652,  2658,  2664,  2668,  2676,  2680,  2686,  2692,  2705,
  2711,  2715,  2721,  2725,  2731,  2742,  2748,  2759,  2765,  2779,
  2783,  2789,  2793,  2803,  2809,  2816,  2822,  2835,  2860,  2896,
  2916,  2922,  2928,  2940,  2962,  2967,  2974,  2978,  2982,  2988,
  2992,  2996,  3000,  3006,  3035,  3062,  3068,  3072,  3079,  3083,
  3093,  3102,  3108,  3115,  3123,  3131,  3135,  3145,  3152,  3158,
  3164
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","DOT_DOT",
"COLON_COLON","ucIdentifier","lcIdentifier","floatValue","textSegment","decimalNumber",
"hexadecimalNumber","moduleKeyword","importKeyword","revisionKeyword","oidKeyword",
"dateKeyword","organizationKeyword","contactKeyword","descriptionKeyword","referenceKeyword",
"typedefKeyword","typeKeyword","writetypeKeyword","nodeKeyword","scalarKeyword",
"tableKeyword","columnKeyword","rowKeyword","notificationKeyword","groupKeyword",
"complianceKeyword","formatKeyword","unitsKeyword","statusKeyword","accessKeyword",
"defaultKeyword","impliedKeyword","indexKeyword","augmentsKeyword","reordersKeyword",
"sparseKeyword","expandsKeyword","createKeyword","membersKeyword","objectsKeyword",
"mandatoryKeyword","optionalKeyword","refineKeyword","OctetStringKeyword","ObjectIdentifierKeyword",
"Integer32Keyword","Unsigned32Keyword","Integer64Keyword","Unsigned64Keyword",
"Float32Keyword","Float64Keyword","Float128Keyword","BitsKeyword","EnumerationKeyword",
"currentKeyword","deprecatedKeyword","obsoleteKeyword","noaccessKeyword","notifyonlyKeyword",
"readonlyKeyword","readwriteKeyword","'{'","'}'","';'","'('","')'","'|'","','",
"'-'","'.'","smingFile","moduleStatement_optsep_0n","moduleStatement_optsep_1n",
"moduleStatement_optsep","moduleStatement","@1","@2","@3","@4","@5","@6","@7",
"typedefStatement_stmtsep_0n","typedefStatement_stmtsep_1n","typedefStatement_stmtsep",
"typedefStatement","@8","@9","@10","@11","@12","@13","@14","@15","anyObjectStatement_stmtsep_0n",
"anyObjectStatement_stmtsep_1n","anyObjectStatement_stmtsep","anyObjectStatement",
"nodeStatement","@16","@17","@18","@19","@20","scalarStatement","@21","@22",
"@23","@24","@25","@26","@27","@28","@29","@30","tableStatement","@31","@32",
"@33","@34","@35","rowStatement","@36","@37","@38","@39","@40","@41","@42","columnStatement_stmtsep_1n",
"columnStatement_stmtsep","columnStatement","@43","@44","@45","@46","@47","@48",
"@49","@50","@51","@52","notificationStatement_stmtsep_0n","notificationStatement_stmtsep_1n",
"notificationStatement_stmtsep","notificationStatement","@53","@54","@55","@56",
"@57","@58","groupStatement_stmtsep_0n","groupStatement_stmtsep_1n","groupStatement_stmtsep",
"groupStatement","@59","@60","@61","@62","@63","@64","complianceStatement_stmtsep_0n",
"complianceStatement_stmtsep_1n","complianceStatement_stmtsep","complianceStatement",
"@65","@66","@67","@68","@69","@70","@71","@72","importStatement_stmtsep_0n",
"importStatement_stmtsep_1n","importStatement_stmtsep","importStatement","@73",
"@74","revisionStatement_stmtsep_0n","revisionStatement_stmtsep_1n","revisionStatement_stmtsep",
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
    75,    76,    76,    77,    77,    78,    80,    81,    82,    83,
    84,    85,    86,    79,    87,    87,    88,    88,    89,    91,
    92,    93,    94,    95,    96,    97,    98,    90,    99,    99,
   100,   100,   101,   102,   102,   102,   104,   105,   106,   107,
   108,   103,   110,   111,   112,   113,   114,   115,   116,   117,
   118,   119,   109,   121,   122,   123,   124,   125,   120,   127,
   128,   129,   130,   131,   132,   133,   126,   134,   134,   135,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   136,   147,   147,   148,   148,   149,   151,   152,   153,   154,
   155,   156,   150,   157,   157,   158,   158,   159,   161,   162,
   163,   164,   165,   166,   160,   167,   167,   168,   168,   169,
   171,   172,   173,   174,   175,   176,   177,   178,   170,   179,
   179,   180,   180,   181,   183,   184,   182,   185,   185,   186,
   186,   187,   188,   189,   190,   190,   191,   192,   192,   193,
   194,   194,   194,   194,   194,   195,   196,   197,   198,   199,
   200,   200,   201,   201,   202,   203,   203,   204,   205,   206,
   207,   208,   209,   209,   210,   211,   211,   212,   213,   213,
   214,   215,   215,   216,   217,   217,   218,   219,   219,   220,
   221,   221,   222,    -1,    -1,   223,   224,   224,   225,   226,
   226,   227,   228,   228,   229,   229,   230,   231,   232,   232,
   233,   233,   234,   235,   236,   236,   237,   237,   237,   237,
   237,   237,   237,   237,   237,   237,   237,   238,   239,   239,
   240,   240,   241,   241,   242,   243,   243,   244,   244,   245,
   246,   247,   247,   248,   249,   249,   250,   251,   251,   252,
   252,   253,   254,   255,   255,   256,   257,   258,   259,   259,
   260,   260,   261,   262,   263,   264,   264,   265,   265,   266,
    -1,    -1,    -1,    -1,    -1,    -1,   267,   268,   268,   269,
   269,   270,   271,   272,   272,   273,   273,   274,   274,   275,
   276,   276,    -1,    -1,   277,   277,   278,   278,   279,   280,
   280,   281,   281,   282,   283,    -1,    -1,   284,   284,   284,
   284,   284,   284,   284,   284,   284,   285,   285,   285,   286,
   286,   286,   286,   287,   288,   288,   289,   289,   290,   290,
   291,   292,   293,   293,   294,   295,   295,   296,   297,   298,
   299
};

static const short yyr2[] = {     0,
     2,     0,     1,     1,     2,     2,     0,     0,     0,     0,
     0,     0,     0,    34,     0,     1,     1,     2,     2,     0,
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

static const short yydefact[] = {   330,
     2,   329,     1,     3,     4,   330,     0,     5,     6,     7,
   329,     0,     8,   330,     0,   331,   120,   329,     0,   121,
   122,   331,     0,   329,   331,   123,   124,   125,     0,     9,
   330,     0,   288,   322,   315,   330,   317,   316,     0,     0,
     0,     0,     0,   314,   318,   319,   329,   331,   330,   287,
   159,   321,   320,     0,    10,     0,   290,   330,     0,   281,
   282,   126,   256,   289,   291,   292,     0,     0,   329,   331,
   330,   328,   257,   258,     0,   293,   294,   161,     0,    11,
     0,   255,   259,   330,   330,     0,   330,     0,     0,   329,
   331,     0,   260,   162,     0,    12,   127,   330,   181,     0,
   329,    13,   331,   180,     0,   128,   182,   330,   330,    15,
   129,   130,   331,     0,     0,   329,    29,    16,    17,   331,
   131,   132,   183,   331,     0,   329,   329,   329,    82,    30,
    31,   331,    34,    35,    36,    18,    19,     0,    20,     0,
     0,     0,   329,    94,    83,    84,   331,    32,    33,   329,
   331,   330,    37,    43,    54,     0,   329,   106,    95,    96,
   331,    85,    86,     0,     0,     0,   330,   330,   330,    87,
     0,   329,     0,   107,   108,   331,    97,    98,   295,   330,
   331,   331,     0,     0,     0,   330,    99,     0,   330,   109,
   110,     0,     0,     0,   331,   331,   331,     0,   330,   111,
     0,   160,   330,   329,   331,     0,     0,     0,   331,     0,
   330,    14,     0,     0,    21,   331,   331,   331,     0,   331,
     0,   133,   286,   223,   208,   223,   223,   223,   223,   235,
   235,   235,     0,     0,   330,   205,   206,   219,   175,    38,
    44,    55,   331,     0,   331,     0,   207,     0,   209,   210,
   211,   212,   213,     0,   214,   215,   330,   217,   216,     0,
   218,     0,   329,    22,   331,   169,     0,   169,    88,   331,
     0,   285,   330,   224,   330,   236,     0,   134,   330,   220,
   221,   222,     0,   163,   176,   329,    39,   331,   329,   331,
    56,   187,   100,   331,     0,     0,   330,   330,   249,     0,
   302,   299,   301,   330,     0,   298,   304,   303,   330,     0,
   329,    23,   331,     0,   178,   170,     0,    45,     0,   330,
    89,   331,     0,   112,   324,   323,     0,   226,   326,   327,
   232,   244,   238,     0,     0,   328,   250,   251,     0,   328,
   300,   305,     0,     0,     0,   166,   164,   307,   308,   309,
   330,    40,   331,   330,     0,   331,     0,   169,   188,   330,
   331,   169,   325,   330,   227,   228,     0,   231,   233,     0,
   243,   245,     0,   330,   239,   240,     0,   330,   247,   248,
   252,   330,   276,   330,   274,   177,     0,   330,   329,    24,
   331,     0,   181,   179,     0,   329,   331,    57,   330,    90,
     0,   101,   113,     0,   229,   330,   330,   330,     0,   241,
   330,     0,     0,   328,   277,   278,     0,     0,   306,     0,
     0,   169,   167,   171,    41,   137,     0,    46,   181,     0,
     0,   330,   169,     0,   225,     0,     0,     0,   237,     0,
   330,   253,   275,   279,   330,   273,   165,   330,    25,     0,
   310,   311,   312,   313,   330,   175,    58,   330,   268,   331,
     0,   102,   331,   230,   234,   246,   242,     0,     0,     0,
   178,   330,     0,    47,     0,     0,   328,   269,   270,     0,
    91,   330,     0,   114,   254,   280,   168,    26,     0,   174,
   163,   329,   331,   330,   267,   271,   330,   181,     0,   331,
   181,   181,    42,    48,     0,     0,     0,     0,    92,   330,
   103,   115,    27,   166,    60,   330,   189,   272,     0,     0,
   181,   190,     0,    49,   330,     0,   330,   186,   104,   330,
   116,   331,   330,   169,     0,    59,     0,     0,     0,   193,
   191,     0,    50,   331,    93,   330,   330,   329,   117,   194,
   195,   331,    28,     0,     0,     0,     0,     0,   199,   196,
   197,   331,   331,   105,   330,   330,   329,   118,   200,   201,
   331,    51,    61,     0,     0,     0,     0,   202,   203,   181,
     0,   330,     0,   330,   330,    52,   151,   329,   329,   329,
   329,   331,   141,   142,   143,   144,   145,     0,   331,     0,
     0,     0,   330,     0,     0,     0,     0,     0,    62,   192,
     0,   135,   119,   330,     0,   152,   330,   151,   330,   151,
   153,   330,   138,   331,     0,   330,     0,   330,     0,   330,
   156,    63,   331,     0,   329,   172,   331,   136,    53,     0,
   147,     0,   149,     0,   330,     0,   169,   154,   198,     0,
     0,   331,   139,   330,   330,   330,     0,   330,   157,    64,
   330,   331,   173,     0,     0,     0,   155,     0,     0,     0,
     0,   330,   330,   330,   330,   331,   140,   330,     0,     0,
     0,     0,    65,     0,   146,   330,   330,   158,   181,   204,
     0,     0,    66,   148,   150,     0,   329,     0,    68,   331,
     0,   330,    69,    70,    71,     0,   330,    67,     0,   331,
     0,   331,    72,     0,   331,    73,     0,   331,    74,   175,
    75,   163,    76,   166,    77,   169,    78,     0,   331,    79,
   181,    80,     0,   330,     0,    81,     0,     0,     0
};

static const short yydefgoto[] = {   737,
     3,     4,     5,     6,    11,    14,    39,    59,    86,    99,
   106,   117,   118,   119,   120,   152,   239,   284,   346,   422,
   471,   502,   523,   129,   130,   131,   132,   133,   167,   266,
   315,   393,   450,   134,   168,   267,   355,   456,   491,   514,
   534,   554,   580,   602,   135,   169,   268,   319,   429,   475,
   493,   525,   581,   621,   647,   669,   689,   696,   698,   699,
   700,   707,   714,   717,   720,   722,   724,   726,   728,   731,
   733,   144,   145,   146,   147,   186,   292,   358,   431,   498,
   519,   158,   159,   160,   161,   199,   323,   433,   483,   521,
   538,   173,   174,   175,   176,   211,   362,   434,   501,   522,
   540,   559,   577,    19,    20,    21,    22,    31,    71,   110,
   111,   112,   113,   205,   623,   290,   636,   637,   592,   593,
   594,   595,   596,   597,   603,   632,   633,   645,   659,    25,
   151,    48,    70,   312,   313,   390,   391,   287,   288,   651,
   397,   264,   265,   352,   353,   102,   103,   361,   321,   322,
   531,   532,   549,   550,   551,   552,   568,   569,   570,   571,
   235,   236,   237,   261,   280,   247,   274,   364,   365,   366,
   328,   368,   369,   253,   276,   374,   375,   376,   333,   371,
   372,   258,   298,   336,   337,   338,   299,    62,    72,    73,
    74,   458,   477,   478,   479,   306,   384,   414,   415,   416,
    63,   238,   459,    58,    64,    65,    66,   180,   309,   351,
   455,    36,    37,    44,    45,    46,    38,   329,   330,   331,
    82,   604,   248,    17
};

static const short yypact[] = {-32768,
    21,-32768,-32768,    21,-32768,-32768,    43,-32768,-32768,-32768,
-32768,    36,-32768,-32768,   -11,-32768,    50,-32768,    58,    50,
-32768,-32768,    59,-32768,-32768,-32768,-32768,-32768,    28,-32768,
-32768,    70,-32768,-32768,-32768,-32768,    12,-32768,    72,    20,
    84,    29,    90,-32768,    12,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    94,-32768,    54,   103,-32768,    96,-32768,
-32768,-32768,    42,-32768,   103,-32768,   109,    51,-32768,-32768,
-32768,-32768,    42,-32768,    49,-32768,-32768,-32768,    94,-32768,
    66,-32768,-32768,-32768,-32768,   119,-32768,    54,    73,-32768,
-32768,    74,-32768,-32768,    94,-32768,-32768,-32768,   120,    75,
-32768,-32768,-32768,-32768,    94,   125,-32768,-32768,-32768,   126,
   125,-32768,-32768,    77,    81,-32768,    15,   126,-32768,-32768,
-32768,-32768,-32768,-32768,   143,-32768,-32768,-32768,   122,    15,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   136,-32768,   147,
   148,   149,-32768,   128,   122,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   152,-32768,   129,   128,-32768,
-32768,-32768,-32768,   153,   119,    98,-32768,-32768,-32768,-32768,
   154,-32768,    95,   129,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   104,   105,   106,-32768,-32768,   167,-32768,-32768,
-32768,   107,   110,   158,-32768,-32768,-32768,   108,-32768,-32768,
   113,-32768,-32768,-32768,-32768,    58,    58,    58,-32768,   116,
-32768,-32768,   115,   210,-32768,-32768,-32768,-32768,    58,-32768,
   118,-32768,   181,   121,-32768,   121,   121,   121,   121,   121,
   121,   121,   123,   123,-32768,-32768,-32768,   121,   156,-32768,
-32768,-32768,-32768,    58,-32768,   182,-32768,   124,-32768,-32768,
-32768,-32768,-32768,   127,-32768,-32768,-32768,-32768,-32768,   130,
-32768,   131,-32768,-32768,-32768,   155,   174,   155,-32768,-32768,
    58,-32768,-32768,-32768,-32768,-32768,   191,-32768,-32768,-32768,
-32768,-32768,    71,   171,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   160,-32768,-32768,    18,   198,-32768,-32768,    42,    37,
-32768,   132,-32768,-32768,   199,-32768,    12,-32768,-32768,   133,
-32768,-32768,-32768,    -9,   119,-32768,   210,-32768,   119,-32768,
-32768,-32768,   166,-32768,-32768,-32768,   203,   142,-32768,-32768,
   211,   211,   142,   150,   146,-32768,    42,-32768,   145,   212,
-32768,    12,   165,    90,    94,   188,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   187,-32768,   159,   155,-32768,-32768,
-32768,   155,-32768,-32768,   142,-32768,   151,-32768,-32768,   221,
-32768,-32768,   231,-32768,   142,-32768,   168,-32768,-32768,-32768,
-32768,-32768,    42,-32768,-32768,-32768,    12,-32768,-32768,-32768,
-32768,   169,   120,-32768,   173,-32768,-32768,-32768,-32768,-32768,
   175,-32768,-32768,   172,-32768,-32768,-32768,-32768,   176,-32768,
-32768,    57,   191,-32768,    42,-32768,   164,   177,    12,   180,
    94,   155,-32768,-32768,-32768,-32768,    30,-32768,   120,    64,
   119,-32768,   155,   119,-32768,    18,    18,   236,-32768,   198,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   178,
-32768,-32768,-32768,-32768,-32768,   156,-32768,-32768,    42,-32768,
    64,-32768,-32768,-32768,-32768,-32768,-32768,   179,   245,   184,
   119,-32768,   185,-32768,   228,   200,-32768,    42,-32768,   202,
-32768,-32768,   119,-32768,-32768,-32768,-32768,-32768,   204,-32768,
   171,-32768,-32768,-32768,-32768,-32768,-32768,   120,   201,-32768,
   120,   120,-32768,-32768,   272,   213,   214,    64,-32768,-32768,
-32768,-32768,-32768,   188,-32768,-32768,-32768,-32768,   216,   218,
   120,   239,   220,-32768,-32768,   222,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   155,   215,-32768,   223,   226,   219,   243,
-32768,   232,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   243,
-32768,-32768,-32768,   119,    58,   233,    64,    64,   247,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   247,-32768,
-32768,-32768,-32768,   235,   230,    64,   240,-32768,-32768,   120,
    86,-32768,   119,-32768,-32768,-32768,   266,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   238,-32768,   242,
   244,   246,-32768,   278,    64,    64,    64,    64,-32768,-32768,
   248,   174,-32768,-32768,   249,-32768,-32768,   266,-32768,   266,
   274,-32768,   297,-32768,   253,-32768,   254,-32768,   255,-32768,
   121,-32768,-32768,   256,-32768,   187,-32768,-32768,-32768,    64,
-32768,   257,-32768,   258,-32768,   260,   155,-32768,-32768,   210,
   119,-32768,-32768,-32768,-32768,-32768,   262,-32768,-32768,-32768,
-32768,-32768,-32768,   265,    64,    64,-32768,    64,   119,   268,
   271,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   273,   269,
   270,   276,-32768,   275,-32768,-32768,-32768,-32768,   120,-32768,
   279,   280,-32768,-32768,-32768,   299,-32768,    -6,-32768,-32768,
   336,-32768,-32768,-32768,-32768,   281,-32768,-32768,   284,-32768,
    58,-32768,-32768,   174,-32768,-32768,   187,-32768,-32768,   156,
-32768,   171,-32768,   188,-32768,   155,-32768,   119,-32768,-32768,
   120,-32768,   277,-32768,   285,-32768,   355,   356,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   353,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   241,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   237,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -340,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   217,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   209,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   189,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   350,-32768,-32768,-32768,-32768,
-32768,   261,-32768,-32768,-32768,  -594,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -564,-32768,-32768,-32768,-32768,  -203,
-32768,-32768,-32768,  -482,-32768,  -499,-32768,  -257,-32768,-32768,
  -613,  -444,-32768,  -100,   -65,  -386,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -177,-32768,-32768,-32768,  -193,-32768,
  -316,-32768,-32768,-32768,-32768,   -98,   117,-32768,-32768,    16,
   -59,-32768,-32768,  -149,   134,-32768,-32768,     5,   -55,-32768,
-32768,   157,-32768,-32768,-32768,    52,   -21,-32768,-32768,-32768,
   313,  -439,-32768,-32768,   -85,-32768,-32768,-32768,-32768,   -18,
   306,-32768,   -27,   -60,-32768,-32768,   333,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -281,   -32,   -33,   -12,-32768,   -36,
  -311,     6,     0,   568
};


#define	YYLAST		1297


static const short yytable[] = {     1,
   354,    35,   216,   217,   218,     9,   425,     7,   504,    52,
   291,   474,    53,    15,   524,   243,    12,   624,    85,   697,
    91,   482,   652,    23,   380,   342,   325,   326,   385,    29,
    40,     2,    32,    33,    98,    42,    34,   126,   127,   128,
   270,    13,   457,   332,   108,   325,   326,    10,    56,   348,
   349,   350,    54,   628,    16,   630,    67,    68,    60,    61,
   702,    18,    75,    28,    67,   325,   326,   294,    32,    33,
    81,    24,    75,    41,    79,    32,    33,   301,    57,   302,
   303,   255,   256,    88,    89,    43,    92,    47,    49,    50,
   327,   451,   452,   453,   454,    95,    51,   100,    34,   181,
   400,    57,   443,   718,   403,   419,   105,   114,   115,   327,
  -330,   509,    69,  -330,   512,   513,    77,   565,    78,   715,
    84,   125,   587,   588,   589,   590,   591,   249,   250,   251,
   252,   140,   141,   142,   529,    87,    90,   109,   101,   304,
    94,    97,   104,   305,   123,   116,   124,   139,   156,   143,
   150,   166,   153,   154,   155,   164,   157,   170,   172,   187,
   179,   189,   171,   182,   449,   495,   183,   184,   185,   195,
   196,   197,   200,   209,   202,   462,   203,   188,   204,   192,
   212,   220,   222,   245,   246,   198,   272,   286,   201,  -330,
   263,   257,   273,   586,   289,   275,   297,   278,   210,   279,
   654,   311,   213,   320,   332,  -322,   344,   341,   360,   214,
   221,   363,  -330,  -330,   223,   379,   382,   383,   378,   389,
   396,   406,   308,   407,   725,   673,   674,   399,   675,   254,
   254,   254,   386,   408,   260,   445,   424,   262,   411,   723,
   426,   435,   466,   432,   472,   439,   446,   447,   485,   310,
   486,   487,   490,   356,   492,   307,   277,   224,   225,   226,
   227,   228,   229,   230,   231,   232,   233,   234,   283,   494,
   510,   503,   295,   497,   296,   721,   543,   515,   300,   516,
   544,   517,   527,   530,   388,   528,   533,   547,   548,   536,
   545,   314,   546,   567,   317,   583,   334,   335,   339,   553,
   564,  -329,   693,   340,   582,   610,   585,   612,   343,    53,
   387,   613,   614,   616,   622,   631,   345,   626,   635,   357,
   639,   641,   643,   649,   697,   655,   656,   367,   658,   667,
   370,   373,   377,   661,   672,   677,   339,   678,   686,   687,
   685,   705,   690,   734,   732,   688,   694,   695,   708,   710,
   392,   563,   736,   395,   738,   739,     8,   703,   136,   401,
   448,   162,   190,   404,   367,   460,   148,   177,   463,    26,
   488,   121,   560,   409,   377,   578,   464,   412,   281,   410,
   405,   413,   417,   418,   467,    83,    53,   420,   381,   660,
   259,   442,   496,    93,   421,   282,   444,    76,   430,   441,
   465,   427,     0,     0,     0,   436,   437,   438,     0,     0,
   440,     0,     0,     0,   417,     0,     0,   500,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   461,     0,     0,     0,     0,     0,     0,     0,     0,
   468,     0,     0,     0,   469,     0,     0,   470,     0,     0,
     0,     0,     0,     0,   473,     0,     0,   476,   480,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   727,     0,
     0,   489,     0,     0,     0,     0,     0,   480,     0,     0,
   518,   499,     0,     0,     0,     0,     0,     0,   562,     0,
     0,     0,     0,   507,     0,     0,   508,   505,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   712,     0,   520,
     0,     0,     0,     0,     0,   526,     0,   599,     0,     0,
     0,     0,     0,     0,   535,     0,   537,     0,     0,   539,
   566,     0,   542,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   556,   557,     0,   584,     0,
     0,     0,     0,   558,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   574,   575,     0,     0,     0,     0,
     0,     0,   576,     0,     0,     0,     0,   617,   618,   619,
   620,   598,     0,   600,   601,   662,     0,     0,     0,    27,
     0,     0,    30,   605,   606,   607,   608,     0,     0,     0,
     0,     0,   615,   676,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   625,     0,    55,   627,     0,   629,     0,
     0,   634,     0,     0,     0,   640,     0,   642,     0,   644,
   646,     0,     0,     0,     0,     0,     0,    80,     0,     0,
   650,     0,     0,     0,   657,     0,     0,     0,     0,     0,
     0,     0,     0,   664,   665,   666,     0,   668,    96,     0,
   670,     0,   729,     0,     0,     0,     0,     0,     0,     0,
   107,   679,   680,   681,   682,     0,     0,   684,     0,     0,
   122,     0,     0,     0,     0,   691,   692,   137,     0,     0,
     0,   138,     0,     0,     0,     0,     0,     0,     0,   149,
     0,   706,   701,     0,     0,     0,   709,     0,     0,     0,
     0,     0,     0,     0,   163,     0,     0,     0,   165,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   178,     0,
     0,     0,     0,   735,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   191,     0,     0,     0,     0,   193,   194,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   206,   207,   208,     0,     0,     0,     0,     0,
     0,     0,   215,     0,     0,     0,   219,     0,     0,     0,
     0,     0,     0,   240,   241,   242,     0,   244,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   269,     0,   271,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   285,     0,     0,     0,     0,   293,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   316,     0,   318,     0,     0,
     0,   324,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   347,     0,     0,     0,     0,     0,     0,     0,     0,   359,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   394,     0,     0,   398,     0,     0,     0,     0,   402,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   423,     0,
     0,     0,     0,     0,   428,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   481,     0,     0,
   484,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   506,     0,     0,     0,     0,     0,     0,   511,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   541,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   555,     0,     0,     0,     0,     0,     0,     0,   561,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   572,
   573,     0,     0,     0,     0,     0,     0,     0,   579,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   609,
     0,     0,     0,     0,     0,     0,   611,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   638,     0,     0,     0,     0,     0,     0,     0,     0,
   648,     0,     0,     0,   653,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   663,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   671,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   683,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   704,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   711,     0,   713,
     0,     0,   716,     0,     0,   719,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   730
};

static const short yycheck[] = {     0,
   317,    29,   206,   207,   208,     6,   393,     2,   491,    43,
   268,   456,    45,    14,   514,   219,    11,   612,    79,    26,
    86,   461,   636,    18,   336,   307,     9,    10,   340,    24,
    31,    11,     5,     6,    95,    36,     9,    23,    24,    25,
   244,     6,   429,     7,   105,     9,    10,     5,    49,    59,
    60,    61,    47,   618,    66,   620,    57,    58,     5,     6,
    67,    12,    63,     5,    65,     9,    10,   271,     5,     6,
    71,    14,    73,     4,    69,     5,     6,     7,     8,     9,
    10,   231,   232,    84,    85,    74,    87,    16,    69,     6,
    73,    62,    63,    64,    65,    90,    68,    98,     9,   165,
   358,     8,   414,   717,   362,   387,   101,   108,   109,    73,
     8,   498,    17,    72,   501,   502,     8,   557,    68,   714,
    72,   116,    37,    38,    39,    40,    41,   226,   227,   228,
   229,   126,   127,   128,   521,    70,    18,    13,    19,    69,
    68,    68,    68,    73,    68,    20,    66,     5,   143,    28,
    15,   152,     6,     6,     6,   150,    29,     6,    30,     6,
     8,    67,   157,    66,   422,   477,   167,   168,   169,    66,
    66,    66,     6,    66,    68,   433,    67,   172,    21,   180,
    68,    66,    68,    66,     4,   186,     5,    33,   189,    69,
    35,    69,    69,   580,    21,    69,     6,    68,   199,    69,
   640,    31,   203,    44,     7,    74,    74,     9,    43,   204,
   211,     9,    71,     3,     5,    70,    72,     6,    69,    32,
    34,    71,   283,     3,   724,   665,   666,    69,   668,   230,
   231,   232,    68,     3,   235,    72,    68,   238,    71,   722,
    68,    70,     7,    69,    67,    70,    70,    68,    70,   283,
     6,    68,    68,   319,    27,   283,   257,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,   263,    70,
    70,    68,   273,    72,   275,   720,   534,     6,   279,    67,
    66,    68,    67,    45,   345,    68,    67,    69,    46,    68,
    68,   286,    67,    47,   289,    66,   297,   298,   299,    68,
    68,    36,   689,   304,    70,    68,    67,    66,   309,   342,
   344,    68,    67,    36,    67,    42,   311,    69,    22,   320,
    68,    68,    68,    68,    26,    69,    69,   328,    69,    68,
   331,   332,   333,   650,    70,    68,   337,    67,    70,    70,
    68,     6,    68,    67,   731,    70,    68,    68,    68,    66,
   351,   555,    68,   354,     0,     0,     4,   698,   118,   360,
   421,   145,   174,   364,   365,   431,   130,   159,   434,    20,
   471,   111,   550,   374,   375,   569,   436,   378,   262,   375,
   365,   382,   383,   384,   440,    73,   419,   388,   337,   647,
   234,   413,   478,    88,   389,   262,   415,    65,   399,   412,
   437,   396,    -1,    -1,    -1,   406,   407,   408,    -1,    -1,
   411,    -1,    -1,    -1,   415,    -1,    -1,   483,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   432,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   441,    -1,    -1,    -1,   445,    -1,    -1,   448,    -1,    -1,
    -1,    -1,    -1,    -1,   455,    -1,    -1,   458,   459,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   726,    -1,
    -1,   472,    -1,    -1,    -1,    -1,    -1,   478,    -1,    -1,
   508,   482,    -1,    -1,    -1,    -1,    -1,    -1,   554,    -1,
    -1,    -1,    -1,   494,    -1,    -1,   497,   492,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   711,    -1,   510,
    -1,    -1,    -1,    -1,    -1,   516,    -1,   583,    -1,    -1,
    -1,    -1,    -1,    -1,   525,    -1,   527,    -1,    -1,   530,
   558,    -1,   533,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   546,   547,    -1,   576,    -1,
    -1,    -1,    -1,   548,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   565,   566,    -1,    -1,    -1,    -1,
    -1,    -1,   567,    -1,    -1,    -1,    -1,   605,   606,   607,
   608,   582,    -1,   584,   585,   651,    -1,    -1,    -1,    22,
    -1,    -1,    25,   588,   589,   590,   591,    -1,    -1,    -1,
    -1,    -1,   603,   669,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   614,    -1,    48,   617,    -1,   619,    -1,
    -1,   622,    -1,    -1,    -1,   626,    -1,   628,    -1,   630,
   631,    -1,    -1,    -1,    -1,    -1,    -1,    70,    -1,    -1,
   635,    -1,    -1,    -1,   645,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   654,   655,   656,    -1,   658,    91,    -1,
   661,    -1,   728,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   103,   672,   673,   674,   675,    -1,    -1,   678,    -1,    -1,
   113,    -1,    -1,    -1,    -1,   686,   687,   120,    -1,    -1,
    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   132,
    -1,   702,   697,    -1,    -1,    -1,   707,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   147,    -1,    -1,    -1,   151,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   161,    -1,
    -1,    -1,    -1,   734,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   176,    -1,    -1,    -1,    -1,   181,   182,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   195,   196,   197,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   205,    -1,    -1,    -1,   209,    -1,    -1,    -1,
    -1,    -1,    -1,   216,   217,   218,    -1,   220,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   243,    -1,   245,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   265,    -1,    -1,    -1,    -1,   270,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   288,    -1,   290,    -1,    -1,
    -1,   294,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   313,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   322,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   353,    -1,    -1,   356,    -1,    -1,    -1,    -1,   361,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   391,    -1,
    -1,    -1,    -1,    -1,   397,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   460,    -1,    -1,
   463,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   493,    -1,    -1,    -1,    -1,    -1,    -1,   500,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   532,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   544,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   552,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   562,
   563,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   571,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   592,
    -1,    -1,    -1,    -1,    -1,    -1,   599,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   624,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   633,    -1,    -1,    -1,   637,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   652,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   662,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   676,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   700,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   710,    -1,   712,
    -1,    -1,   715,    -1,    -1,   718,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   729
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
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

#line 216 "/usr/local/share/bison.simple"

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
#line 425 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 435 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 439 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 445 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 449 "parser-sming.y"
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
#line 464 "parser-sming.y"
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
#line 478 "parser-sming.y"
{
			    thisParserPtr->modulePtr = findModuleByName(yyvsp[0].text);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule(yyvsp[0].text,
					      thisParserPtr->path,
					      thisParserPtr->character,
					      0,
					      thisParserPtr);
			    }
			    thisParserPtr->modulePtr->flags &= ~FLAG_SMIV2;
			    thisParserPtr->modulePtr->flags |= FLAG_SMING;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			    free(yyvsp[0].text);
			    thisParserPtr->firstIndexlabelPtr = NULL;
			;
    break;}
case 8:
#line 498 "parser-sming.y"
{
			    moduleObjectPtr = addObject(yyvsp[0].text,
							pendingNodePtr, 0,
							FLAG_INCOMPLETE,
							thisParserPtr);
			    setObjectDecl(moduleObjectPtr,
					  SMI_DECL_MODULE);
			    setModuleIdentityObject(
				thisParserPtr->modulePtr, moduleObjectPtr);
			    free(yyvsp[0].text);
			;
    break;}
case 9:
#line 512 "parser-sming.y"
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
				moduleObjectPtr = addObject("",
							    yyvsp[-1].nodePtr->parentPtr,
							    yyvsp[-1].nodePtr->subid,
							    0, thisParserPtr);
				moduleObjectPtr =
				    setObjectName(moduleObjectPtr,
					       thisModulePtr->objectPtr->name);
				setObjectDecl(moduleObjectPtr,
					      SMI_DECL_MODULE);
				deleteObjectFlags(moduleObjectPtr,
					       FLAG_INCOMPLETE);
				addObjectFlags(moduleObjectPtr,
					       FLAG_REGISTERED);
				setModuleIdentityObject(
				    thisParserPtr->modulePtr, moduleObjectPtr);
			    }
			;
    break;}
case 10:
#line 541 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 11:
#line 548 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 12:
#line 555 "parser-sming.y"
{
			    if (moduleObjectPtr && yyvsp[-1].text) {
				setObjectDescription(moduleObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 13:
#line 561 "parser-sming.y"
{
			    if (moduleObjectPtr && yyvsp[0].text) {
				setObjectReference(moduleObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 14:
#line 573 "parser-sming.y"
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
       ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->indexPtr->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				/* adjust indexPtr->rowPtr */
		     if (((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->
				    indexPtr->rowPtr) {
				    objectPtr = findObject(
  		        ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->
					indexPtr->rowPtr,
					thisParserPtr);
			 ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->
				    indexPtr->rowPtr = objectPtr;
				}
				/* adjust create list */
				for (listPtr =
		 ((Object *)(thisParserPtr->firstIndexlabelPtr->ptr))->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
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
#line 622 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 16:
#line 626 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 17:
#line 636 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 18:
#line 641 "parser-sming.y"
{
			    /*
			     * Sum up the number of successfully parsed
			     * typedefs or return -1, if at least one
			     * module failed.
			     */
			    if ((yyvsp[-1].rc >= 0) && (yyvsp[0].rc >= 0)) {
				yyval.rc = yyvsp[-1].rc + yyvsp[0].rc;
			    } else {
				yyval.rc = -1;
			    }
			;
    break;}
case 19:
#line 656 "parser-sming.y"
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
case 20:
#line 670 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 21:
#line 675 "parser-sming.y"
{
			    if (yyvsp[-1].typePtr) {
				if (yyvsp[-1].typePtr->name) {
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
case 22:
#line 694 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 23:
#line 700 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeFormat(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 24:
#line 706 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 25:
#line 712 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 26:
#line 718 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 27:
#line 724 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 28:
#line 730 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 29:
#line 738 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 30:
#line 742 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 31:
#line 752 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 32:
#line 757 "parser-sming.y"
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
case 33:
#line 772 "parser-sming.y"
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
case 37:
#line 791 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 38:
#line 796 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				nodeObjectPtr = addObject(nodeIdentifier,
							  yyvsp[-1].nodePtr->parentPtr,
							  yyvsp[-1].nodePtr->subid,
							  0, thisParserPtr);
				setObjectDecl(nodeObjectPtr, SMI_DECL_NODE);
				setObjectAccess(nodeObjectPtr,
						SMI_ACCESS_NOT_ACCESSIBLE);
			    }
			;
    break;}
case 39:
#line 808 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 40:
#line 814 "parser-sming.y"
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
case 41:
#line 827 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 42:
#line 833 "parser-sming.y"
{
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 43:
#line 841 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 44:
#line 846 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				scalarObjectPtr = addObject(scalarIdentifier,
							    yyvsp[-1].nodePtr->parentPtr,
							    yyvsp[-1].nodePtr->subid,
							    0, thisParserPtr);
				setObjectDecl(scalarObjectPtr,
					      SMI_DECL_SCALAR);
			    }
			;
    break;}
case 45:
#line 857 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].typePtr) {
				setObjectType(scalarObjectPtr, yyvsp[-1].typePtr);
				defaultBasetype = yyvsp[-1].typePtr->basetype;
				if (!(yyvsp[-1].typePtr->name)) {
				    /*
				     * An inlined type.
				     */
				    setTypeName(yyvsp[-1].typePtr, scalarIdentifier);
				}
			    }
			;
    break;}
case 46:
#line 870 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 47:
#line 876 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 48:
#line 882 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectFormat(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 49:
#line 888 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 50:
#line 894 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 51:
#line 900 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 52:
#line 906 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 53:
#line 912 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 54:
#line 921 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 55:
#line 926 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				tableObjectPtr = addObject(tableIdentifier,
							   yyvsp[-1].nodePtr->parentPtr,
							   yyvsp[-1].nodePtr->subid,
							   0, thisParserPtr);
				setObjectDecl(tableObjectPtr,
					      SMI_DECL_TABLE);
				setObjectAccess(tableObjectPtr,
						SMI_ACCESS_NOT_ACCESSIBLE);
			    }
			;
    break;}
case 56:
#line 939 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 57:
#line 945 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 58:
#line 951 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 958 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 60:
#line 966 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 61:
#line 971 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				rowObjectPtr = addObject(rowIdentifier,
							 yyvsp[-1].nodePtr->parentPtr,
							 yyvsp[-1].nodePtr->subid,
							 0, thisParserPtr);
				setObjectDecl(rowObjectPtr,
					      SMI_DECL_ROW);
				setObjectAccess(rowObjectPtr,
						SMI_ACCESS_NOT_ACCESSIBLE);
			    }
			;
    break;}
case 62:
#line 984 "parser-sming.y"
{
			    List *listPtr;
			    
			    if (rowObjectPtr && yyvsp[-1].indexPtr) {
				setObjectIndex(rowObjectPtr, yyvsp[-1].indexPtr);

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
case 63:
#line 1003 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectList(rowObjectPtr, yyvsp[0].listPtr);
				if (yyvsp[0].listPtr) {
				    addObjectFlags(rowObjectPtr,
						   FLAG_CREATABLE);
				}
			    }
			;
    break;}
case 64:
#line 1013 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 65:
#line 1019 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 66:
#line 1025 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 67:
#line 1032 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 68:
#line 1040 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 69:
#line 1048 "parser-sming.y"
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
case 70:
#line 1063 "parser-sming.y"
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
case 71:
#line 1077 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 72:
#line 1082 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				columnObjectPtr = addObject(columnIdentifier,
							    yyvsp[-1].nodePtr->parentPtr,
							    yyvsp[-1].nodePtr->subid,
							    0, thisParserPtr);
				setObjectDecl(columnObjectPtr,
					      SMI_DECL_COLUMN);
			    }
			;
    break;}
case 73:
#line 1093 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].typePtr) {
				setObjectType(columnObjectPtr, yyvsp[-1].typePtr);
				defaultBasetype = yyvsp[-1].typePtr->basetype;
				if (!(yyvsp[-1].typePtr->name)) {
				    /*
				     * An inlined type.
				     */
				    setTypeName(yyvsp[-1].typePtr, columnIdentifier);
				}
			    }
			;
    break;}
case 74:
#line 1106 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 75:
#line 1112 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 76:
#line 1118 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectFormat(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 77:
#line 1124 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 78:
#line 1130 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 79:
#line 1136 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 80:
#line 1142 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 81:
#line 1148 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 82:
#line 1157 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 83:
#line 1161 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 84:
#line 1171 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 85:
#line 1176 "parser-sming.y"
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
case 86:
#line 1191 "parser-sming.y"
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
case 87:
#line 1205 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 88:
#line 1210 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				notificationObjectPtr =
				    addObject(notificationIdentifier,
					      yyvsp[-1].nodePtr->parentPtr,
					      yyvsp[-1].nodePtr->subid,
					      0, thisParserPtr);
				setObjectDecl(notificationObjectPtr,
					      SMI_DECL_NOTIFICATION);
			    }
			;
    break;}
case 89:
#line 1222 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (notificationObjectPtr && yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(notificationObjectPtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 90:
#line 1237 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 91:
#line 1243 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 92:
#line 1250 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 93:
#line 1256 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 94:
#line 1264 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 95:
#line 1268 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 96:
#line 1278 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 97:
#line 1282 "parser-sming.y"
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
case 98:
#line 1297 "parser-sming.y"
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
case 99:
#line 1311 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 100:
#line 1316 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				groupObjectPtr = addObject(groupIdentifier,
							   yyvsp[-1].nodePtr->parentPtr,
							   yyvsp[-1].nodePtr->subid,
							   0, thisParserPtr);
				setObjectDecl(groupObjectPtr, SMI_DECL_GROUP);
			    }
			;
    break;}
case 101:
#line 1326 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (groupObjectPtr && yyvsp[-1].listPtr) {
				for (listPtr = yyvsp[-1].listPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(groupObjectPtr, yyvsp[-1].listPtr);
			    }
			;
    break;}
case 102:
#line 1341 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 103:
#line 1347 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 104:
#line 1353 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 105:
#line 1359 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 106:
#line 1367 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 107:
#line 1371 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 108:
#line 1381 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 109:
#line 1386 "parser-sming.y"
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
case 110:
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
case 111:
#line 1415 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 112:
#line 1420 "parser-sming.y"
{
			    if (yyvsp[-1].nodePtr) {
				complianceObjectPtr =
				    addObject(complianceIdentifier,
					      yyvsp[-1].nodePtr->parentPtr,
					      yyvsp[-1].nodePtr->subid,
					      0, thisParserPtr);
				setObjectDecl(complianceObjectPtr,
					      SMI_DECL_COMPLIANCE);
			    }
			;
    break;}
case 113:
#line 1432 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 114:
#line 1438 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 115:
#line 1444 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 116:
#line 1450 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (complianceObjectPtr && yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(complianceObjectPtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 117:
#line 1465 "parser-sming.y"
{
			    complianceObjectPtr->optionlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 118:
#line 1469 "parser-sming.y"
{
			    Refinement *refinementPtr;
			    List *listPtr;
			    char s[SMI_MAX_DESCRIPTOR * 2 + 15];
			    
			    complianceObjectPtr->refinementlistPtr = yyvsp[0].listPtr;
			    if (yyvsp[0].listPtr) {
				for (listPtr = yyvsp[0].listPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr) {
				    refinementPtr =
					((Refinement *)(listPtr->ptr));
				    if (refinementPtr->typePtr) {
					sprintf(s, "%s+%s+type",
						complianceIdentifier,
					       refinementPtr->objectPtr->name);
					setTypeName(refinementPtr->typePtr, s);
					
				    }
				    if (refinementPtr->writetypePtr) {
					sprintf(s, "%s+%s+writetype",
						complianceIdentifier,
					       refinementPtr->objectPtr->name);
				   setTypeName(refinementPtr->writetypePtr, s);
				    }
				}
			    }
			;
    break;}
case 119:
#line 1498 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 120:
#line 1506 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 121:
#line 1510 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 122:
#line 1516 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 123:
#line 1520 "parser-sming.y"
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
case 124:
#line 1535 "parser-sming.y"
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
case 125:
#line 1549 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 126:
#line 1554 "parser-sming.y"
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
case 127:
#line 1566 "parser-sming.y"
{
			    char *s = importModulename;
			    
			    if (!findModuleByName(s)) {
				smiLoadModule(s);
			    }
			    checkImports(s, thisParserPtr);
			    free(s);
			    yyval.listPtr = NULL;
			;
    break;}
case 128:
#line 1579 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 129:
#line 1583 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 130:
#line 1589 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 131:
#line 1593 "parser-sming.y"
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
case 132:
#line 1608 "parser-sming.y"
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
case 133:
#line 1625 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(smiMkTime(yyvsp[-6].text), yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 134:
#line 1633 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 135:
#line 1639 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 136:
#line 1643 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 137:
#line 1650 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 138:
#line 1656 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 139:
#line 1660 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 140:
#line 1667 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 146:
#line 1681 "parser-sming.y"
{
			    yyval.indexPtr = util_malloc(sizeof(Index));
			    if (yyvsp[-8].rc) {
				yyval.indexPtr->implied = 1;
			    } else {
				yyval.indexPtr->implied = 0;
			    }
			    yyval.indexPtr->indexkind = SMI_INDEX_INDEX;
			    yyval.indexPtr->listPtr = yyvsp[-4].listPtr;
			    yyval.indexPtr->rowPtr = NULL;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			;
    break;}
case 147:
#line 1702 "parser-sming.y"
{
			    yyval.indexPtr = util_malloc(sizeof(Index));
			    yyval.indexPtr->implied = 0;
			    yyval.indexPtr->indexkind = SMI_INDEX_AUGMENT;
			    yyval.indexPtr->listPtr = NULL;
			    yyval.indexPtr->rowPtr = (void *)yyvsp[-2].text;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			;
    break;}
case 148:
#line 1721 "parser-sming.y"
{
			    yyval.indexPtr = util_malloc(sizeof(Index));
			    if (yyvsp[-8].rc) {
				yyval.indexPtr->implied = 1;
			    } else {
				yyval.indexPtr->implied = 0;
			    }
			    yyval.indexPtr->indexkind = SMI_INDEX_REORDER;
			    yyval.indexPtr->listPtr = yyvsp[-4].listPtr;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			    yyval.indexPtr->rowPtr = (void *)yyvsp[-9].text;
			;
    break;}
case 149:
#line 1742 "parser-sming.y"
{
			    yyval.indexPtr = util_malloc(sizeof(Index));
			    yyval.indexPtr->implied = 0;
			    yyval.indexPtr->indexkind = SMI_INDEX_SPARSE;
			    yyval.indexPtr->listPtr = NULL;
			    yyval.indexPtr->rowPtr = (void *)yyvsp[-2].text;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			;
    break;}
case 150:
#line 1761 "parser-sming.y"
{
			    yyval.indexPtr = util_malloc(sizeof(Index));
			    if (yyvsp[-8].rc) {
				yyval.indexPtr->implied = 1;
			    } else {
				yyval.indexPtr->implied = 0;
			    }
			    yyval.indexPtr->indexkind = SMI_INDEX_EXPAND;
			    yyval.indexPtr->listPtr = yyvsp[-4].listPtr;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			    yyval.indexPtr->rowPtr = (void *)yyvsp[-9].text;
			;
    break;}
case 151:
#line 1782 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 152:
#line 1786 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 153:
#line 1792 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 154:
#line 1796 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 155:
#line 1802 "parser-sming.y"
{
			    if (rowObjectPtr) {
				addObjectFlags(rowObjectPtr, FLAG_CREATABLE);
			    }
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 156:
#line 1811 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 157:
#line 1815 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 158:
#line 1821 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 159:
#line 1827 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 160:
#line 1833 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 161:
#line 1839 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 162:
#line 1845 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 163:
#line 1851 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 164:
#line 1855 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 165:
#line 1861 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 166:
#line 1867 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 167:
#line 1871 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 168:
#line 1877 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 169:
#line 1883 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 170:
#line 1887 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 171:
#line 1893 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 172:
#line 1899 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 173:
#line 1903 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 174:
#line 1909 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 175:
#line 1915 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 176:
#line 1919 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 177:
#line 1925 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 178:
#line 1931 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 179:
#line 1935 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 180:
#line 1941 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 181:
#line 1947 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 182:
#line 1951 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 183:
#line 1957 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 184:
#line 1963 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 185:
#line 1967 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 186:
#line 1974 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 187:
#line 1980 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 188:
#line 1984 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 189:
#line 1991 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 190:
#line 1997 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 191:
#line 2001 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 192:
#line 2008 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 193:
#line 2014 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 194:
#line 2018 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 195:
#line 2024 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 196:
#line 2031 "parser-sming.y"
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
case 197:
#line 2044 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 198:
#line 2053 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text, thisParserPtr);
			    yyval.optionPtr->description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 199:
#line 2061 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 200:
#line 2065 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 201:
#line 2071 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 202:
#line 2077 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].refinementPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 203:
#line 2090 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 204:
#line 2101 "parser-sming.y"
{
			    yyval.refinementPtr = util_malloc(sizeof(Refinement));
			    yyval.refinementPtr->objectPtr = findObject(yyvsp[-10].text, thisParserPtr);
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
			    yyval.refinementPtr->access = yyvsp[-5].access;
			    yyval.refinementPtr->description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 205:
#line 2124 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 206:
#line 2128 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 207:
#line 2134 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingOctetStringPtr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingOctetStringPtr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingOctetStringPtr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 208:
#line 2146 "parser-sming.y"
{
			    yyval.typePtr = typeSmingObjectIdentifierPtr;
			;
    break;}
case 209:
#line 2150 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingInteger32Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingInteger32Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingInteger32Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 210:
#line 2162 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingUnsigned32Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingUnsigned32Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingUnsigned32Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 211:
#line 2174 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingInteger64Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingInteger64Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingInteger64Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 212:
#line 2186 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingUnsigned64Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingUnsigned64Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingUnsigned64Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 213:
#line 2198 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingFloat32Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingFloat32Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingFloat32Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 214:
#line 2210 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingFloat64Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingFloat64Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingFloat64Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 215:
#line 2222 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingFloat128Ptr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingFloat128Ptr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingFloat128Ptr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 216:
#line 2234 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingEnumPtr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingEnumPtr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingEnumPtr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 217:
#line 2246 "parser-sming.y"
{
			    if (!yyvsp[0].listPtr) {
				yyval.typePtr = typeSmingBitsPtr;
			    } else {
				yyval.typePtr = duplicateType(typeSmingBitsPtr, 0,
						   thisParserPtr);
				setTypeParent(yyval.typePtr,
					      typeSmingBitsPtr->name);
				setTypeList(yyval.typePtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 218:
#line 2260 "parser-sming.y"
{
			    char s[SMI_MAX_FULLNAME];

			    typePtr = findType(yyvsp[-1].text, thisParserPtr);
			    if (typePtr && yyvsp[0].listPtr) {
				sprintf(s, "%s::%s",
					typePtr->modulePtr->name,
					typePtr->name);
				typePtr = duplicateType(typePtr, 0,
							thisParserPtr);
				setTypeParent(typePtr, s);
				setTypeList(typePtr, yyvsp[0].listPtr);
			    }

			    yyval.typePtr = typePtr;
			;
    break;}
case 219:
#line 2279 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 220:
#line 2283 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 221:
#line 2289 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 222:
#line 2293 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 223:
#line 2299 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 224:
#line 2303 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 225:
#line 2310 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 226:
#line 2318 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 227:
#line 2322 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 228:
#line 2328 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 229:
#line 2334 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 230:
#line 2347 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 231:
#line 2353 "parser-sming.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    yyval.rangePtr->minValuePtr = yyvsp[-1].valuePtr;
			    if (yyvsp[0].valuePtr) {
				yyval.rangePtr->maxValuePtr = yyvsp[0].valuePtr;
			    } else {
				yyval.rangePtr->maxValuePtr = yyvsp[-1].valuePtr;
			    }
			;
    break;}
case 232:
#line 2365 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 233:
#line 2369 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 234:
#line 2375 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 235:
#line 2381 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 236:
#line 2385 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 237:
#line 2392 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 238:
#line 2400 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 239:
#line 2404 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 240:
#line 2410 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 241:
#line 2416 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].rangePtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 242:
#line 2429 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 243:
#line 2435 "parser-sming.y"
{
			    yyval.rangePtr = util_malloc(sizeof(Range));
			    yyval.rangePtr->minValuePtr = util_malloc(sizeof(SmiValue));
			    yyval.rangePtr->minValuePtr->basetype = SMI_BASETYPE_FLOAT64;
			    yyval.rangePtr->minValuePtr->value.float64 = strtod(yyvsp[-1].text, NULL);
			    if (yyvsp[0].text) {
				yyval.rangePtr->maxValuePtr =
				                 util_malloc(sizeof(SmiValue));
				yyval.rangePtr->maxValuePtr->basetype =
				                          SMI_BASETYPE_FLOAT64;
				yyval.rangePtr->maxValuePtr->value.float64 =
				                              strtod(yyvsp[0].text, NULL);
			    } else {
				yyval.rangePtr->maxValuePtr = yyval.rangePtr->minValuePtr;
			    }
			;
    break;}
case 244:
#line 2454 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 245:
#line 2458 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 246:
#line 2464 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 247:
#line 2470 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 248:
#line 2477 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 249:
#line 2485 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 250:
#line 2489 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 251:
#line 2495 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 252:
#line 2502 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].namedNumberPtr;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 253:
#line 2516 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 254:
#line 2522 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->name = yyvsp[-6].text;
			    yyval.namedNumberPtr->valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 255:
#line 2530 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 256:
#line 2538 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 257:
#line 2542 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 258:
#line 2548 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 259:
#line 2554 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 260:
#line 2567 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 261:
#line 2573 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 262:
#line 2581 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 263:
#line 2585 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 264:
#line 2591 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 265:
#line 2597 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 266:
#line 2610 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 267:
#line 2616 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 268:
#line 2624 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 269:
#line 2628 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 270:
#line 2634 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 271:
#line 2640 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 272:
#line 2653 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 273:
#line 2659 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 274:
#line 2665 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 275:
#line 2669 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 276:
#line 2677 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 277:
#line 2681 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 278:
#line 2687 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 279:
#line 2693 "parser-sming.y"
{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)yyvsp[0].text;
			    p->nextPtr = NULL;
			    for (pp = yyvsp[-1].listPtr; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 280:
#line 2706 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 281:
#line 2712 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 282:
#line 2716 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 283:
#line 2722 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 284:
#line 2726 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 285:
#line 2732 "parser-sming.y"
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
case 286:
#line 2743 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 287:
#line 2749 "parser-sming.y"
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
case 288:
#line 2760 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 289:
#line 2766 "parser-sming.y"
{
			    if (yyvsp[0].text) {
				yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
				strcpy(yyval.text, yyvsp[-1].text);
				strcat(yyval.text, yyvsp[0].text);
				free(yyvsp[-1].text);
				free(yyvsp[0].text);
			    } else {
				yyval.text = yyvsp[-1].text;
			    }
			;
    break;}
case 290:
#line 2780 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 291:
#line 2784 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 292:
#line 2790 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 2794 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 294:
#line 2804 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 295:
#line 2811 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 296:
#line 2817 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 297:
#line 2823 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 298:
#line 2836 "parser-sming.y"
{
			    int i;
			    List *listPtr, *nextPtr;
			    
			    if (defaultBasetype == SMI_BASETYPE_BITS) {
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_BITS;
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
case 299:
#line 2861 "parser-sming.y"
{
			    /* Note: might also be an OID or signed */
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_UNSIGNED32:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
				yyval.valuePtr->value.unsigned32 = strtoul(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_UNSIGNED64:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
				yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER32:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
				yyval.valuePtr->value.integer32 = strtol(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER64:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
				yyval.valuePtr->value.integer64 = strtoll(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_OBJECTIDENTIFIER:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
				yyval.valuePtr->value.oid = util_strdup(yyvsp[0].text);
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
				break;
			    }
			;
    break;}
case 300:
#line 2897 "parser-sming.y"
{
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_INTEGER32:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER32;
				yyval.valuePtr->value.integer32 = - strtoul(yyvsp[0].text, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER64:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
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
case 301:
#line 2917 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 302:
#line 2923 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 303:
#line 2929 "parser-sming.y"
{
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_OCTETSTRING;
				yyval.valuePtr->value.ptr = yyvsp[0].text;
			    } else {
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				yyval.valuePtr = NULL;
			    }
			;
    break;}
case 304:
#line 2941 "parser-sming.y"
{
			    /* Note: might be an Enumeration item or OID */
			    /* TODO: convert if it's an oid? */
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_ENUM:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_ENUM;
				yyval.valuePtr->value.ptr = yyvsp[0].text;
				break;
			    case SMI_BASETYPE_OBJECTIDENTIFIER:
				yyval.valuePtr = util_malloc(sizeof(SmiValue));
				yyval.valuePtr->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
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
case 305:
#line 2963 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 306:
#line 2968 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 307:
#line 2975 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 308:
#line 2979 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 309:
#line 2983 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 310:
#line 2989 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 311:
#line 2993 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 312:
#line 2997 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 313:
#line 3001 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 314:
#line 3007 "parser-sming.y"
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
				    nodePtr = createNodes(oid);
				}
				yyval.nodePtr = nodePtr;
			    } else {
				yyval.nodePtr = NULL;
			    }
			;
    break;}
case 315:
#line 3036 "parser-sming.y"
{
			    Object *objectPtr;
			    Node *nodePtr;
			    char s1[SMI_MAX_OID+1], s2[SMI_MAX_OID+1];
			    
			    /* TODO XXX: $1 might be numeric !? */
			    
			    objectPtr = findObject(yyvsp[0].text, thisParserPtr);

			    if (objectPtr) {
				/* create OID string */
				nodePtr = objectPtr->nodePtr;
				sprintf(s1, "%u", nodePtr->subid);
				while ((nodePtr->parentPtr) &&
				       (nodePtr->parentPtr != rootNodePtr)) {
				    nodePtr = nodePtr->parentPtr;
				    sprintf(s2, "%u.%s", nodePtr->subid, s1);
				    strcpy(s1, s2);
				}
				yyval.text = util_strdup(s1);
			    } else {
				printError(thisParserPtr,
					   ERR_UNKNOWN_OIDLABEL, yyvsp[0].text);
				yyval.text = NULL;
			    }
			;
    break;}
case 316:
#line 3063 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 317:
#line 3069 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 318:
#line 3073 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 319:
#line 3080 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 320:
#line 3084 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 321:
#line 3094 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 322:
#line 3103 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 323:
#line 3109 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = 42;
			;
    break;}
case 324:
#line 3116 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 325:
#line 3124 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 326:
#line 3132 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 327:
#line 3136 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 328:
#line 3146 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 329:
#line 3153 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 330:
#line 3159 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 331:
#line 3165 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 542 "/usr/local/share/bison.simple"

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
#line 3170 "parser-sming.y"


#endif
			
