
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

#define YYTRANSLATE(x) ((unsigned)(x) <= 320 ? yytranslate[x] : 300)

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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56,    57,    58,    59,    60,    61,    62,    63,    64,    65
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
   547,   554,   561,   567,   625,   629,   639,   643,   659,   673,
   677,   698,   704,   710,   716,   722,   728,   733,   741,   745,
   755,   759,   775,   789,   790,   791,   794,   798,   812,   818,
   831,   836,   844,   848,   860,   873,   880,   886,   892,   898,
   903,   910,   915,   924,   928,   943,   948,   955,   960,   969,
   973,   987,  1008,  1014,  1019,  1026,  1032,  1040,  1048,  1063,
  1077,  1081,  1093,  1106,  1113,  1119,  1125,  1131,  1136,  1143,
  1148,  1157,  1161,  1171,  1175,  1191,  1205,  1209,  1223,  1238,
  1243,  1251,  1256,  1264,  1268,  1278,  1282,  1297,  1311,  1315,
  1326,  1342,  1347,  1354,  1359,  1367,  1371,  1381,  1385,  1401,
  1415,  1419,  1433,  1438,  1445,  1451,  1466,  1470,  1498,  1506,
  1510,  1516,  1520,  1535,  1549,  1553,  1566,  1579,  1583,  1589,
  1593,  1608,  1622,  1631,  1639,  1643,  1649,  1656,  1660,  1666,
  1673,  1674,  1675,  1676,  1677,  1680,  1702,  1719,  1742,  1759,
  1782,  1786,  1792,  1796,  1802,  1808,  1812,  1818,  1824,  1830,
  1836,  1842,  1848,  1852,  1858,  1864,  1868,  1874,  1880,  1884,
  1890,  1896,  1900,  1906,  1912,  1916,  1922,  1928,  1932,  1938,
  1944,  1948,  1954,  1960,  1964,  1970,  1977,  1981,  1987,  1994,
  1998,  2004,  2011,  2015,  2021,  2027,  2041,  2047,  2058,  2062,
  2068,  2074,  2087,  2093,  2121,  2125,  2131,  2143,  2147,  2159,
  2171,  2183,  2195,  2207,  2219,  2231,  2243,  2257,  2276,  2280,
  2286,  2290,  2296,  2300,  2306,  2315,  2319,  2325,  2331,  2344,
  2350,  2362,  2366,  2372,  2378,  2382,  2388,  2397,  2401,  2407,
  2413,  2426,  2432,  2451,  2455,  2461,  2467,  2473,  2482,  2486,
  2492,  2498,  2513,  2519,  2527,  2535,  2539,  2545,  2551,  2564,
  2570,  2578,  2582,  2588,  2594,  2607,  2613,  2621,  2625,  2631,
  2637,  2650,  2656,  2662,  2666,  2674,  2678,  2684,  2690,  2703,
  2709,  2713,  2719,  2723,  2729,  2740,  2746,  2757,  2763,  2777,
  2781,  2787,  2791,  2801,  2807,  2814,  2820,  2833,  2858,  2894,
  2914,  2920,  2926,  2938,  2960,  2965,  2972,  2976,  2980,  2986,
  2990,  2994,  2998,  3004,  3033,  3060,  3066,  3070,  3077,  3081,
  3091,  3100,  3106,  3113,  3121,  3129,  3133,  3143,  3150,  3156,
  3162
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
			     * rows of this module with
			     * flag & FLAG_INDEXLABELS and convert their
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
				deleteObjectFlags(
			    (Object *)(thisParserPtr->firstIndexlabelPtr->ptr),
				    FLAG_INDEXLABELS);
			  listPtr = thisParserPtr->firstIndexlabelPtr->nextPtr;
				free(thisParserPtr->firstIndexlabelPtr);
				thisParserPtr->firstIndexlabelPtr = listPtr;
			    }
			    yyval.modulePtr = thisModulePtr;
			    moduleObjectPtr = NULL;
			;
    break;}
case 15:
#line 626 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 16:
#line 630 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 17:
#line 640 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 18:
#line 645 "parser-sming.y"
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
#line 660 "parser-sming.y"
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
#line 674 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 21:
#line 679 "parser-sming.y"
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
#line 698 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 23:
#line 704 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeFormat(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 24:
#line 710 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 25:
#line 716 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 26:
#line 722 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 27:
#line 728 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 28:
#line 734 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 29:
#line 742 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 30:
#line 746 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 31:
#line 756 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 32:
#line 761 "parser-sming.y"
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
#line 776 "parser-sming.y"
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
#line 795 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 38:
#line 800 "parser-sming.y"
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
#line 812 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 40:
#line 818 "parser-sming.y"
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
#line 831 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 42:
#line 837 "parser-sming.y"
{
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 43:
#line 845 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 44:
#line 850 "parser-sming.y"
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
#line 861 "parser-sming.y"
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
#line 874 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 47:
#line 880 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 48:
#line 886 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectFormat(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 49:
#line 892 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 50:
#line 898 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 51:
#line 904 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 52:
#line 910 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 53:
#line 916 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 54:
#line 925 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 55:
#line 930 "parser-sming.y"
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
#line 943 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 57:
#line 949 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 58:
#line 955 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 962 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 60:
#line 970 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 61:
#line 975 "parser-sming.y"
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
#line 988 "parser-sming.y"
{
			    List *listPtr;
			    
			    if (rowObjectPtr && yyvsp[-1].indexPtr) {
				setObjectIndex(rowObjectPtr, yyvsp[-1].indexPtr);
				addObjectFlags(rowObjectPtr, FLAG_INDEXLABELS);

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
#line 1008 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectList(rowObjectPtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 64:
#line 1014 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 65:
#line 1020 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 66:
#line 1026 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 67:
#line 1033 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 68:
#line 1041 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 69:
#line 1049 "parser-sming.y"
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
#line 1064 "parser-sming.y"
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
#line 1078 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 72:
#line 1083 "parser-sming.y"
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
#line 1094 "parser-sming.y"
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
#line 1107 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 75:
#line 1113 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 76:
#line 1119 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectFormat(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 77:
#line 1125 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 78:
#line 1131 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 79:
#line 1137 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 80:
#line 1143 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 81:
#line 1149 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 82:
#line 1158 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 83:
#line 1162 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 84:
#line 1172 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 85:
#line 1177 "parser-sming.y"
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
#line 1192 "parser-sming.y"
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
#line 1206 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 88:
#line 1211 "parser-sming.y"
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
#line 1223 "parser-sming.y"
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
#line 1238 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 91:
#line 1244 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 92:
#line 1251 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 93:
#line 1257 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 94:
#line 1265 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 95:
#line 1269 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 96:
#line 1279 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 97:
#line 1283 "parser-sming.y"
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
#line 1298 "parser-sming.y"
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
#line 1312 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 100:
#line 1317 "parser-sming.y"
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
#line 1327 "parser-sming.y"
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
#line 1342 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 103:
#line 1348 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 104:
#line 1354 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 105:
#line 1360 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 106:
#line 1368 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 107:
#line 1372 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 108:
#line 1382 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 109:
#line 1387 "parser-sming.y"
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
#line 1402 "parser-sming.y"
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
#line 1416 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 112:
#line 1421 "parser-sming.y"
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
#line 1433 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 114:
#line 1439 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 115:
#line 1445 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 116:
#line 1451 "parser-sming.y"
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
#line 1466 "parser-sming.y"
{
			    complianceObjectPtr->optionlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 118:
#line 1470 "parser-sming.y"
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
#line 1499 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 120:
#line 1507 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 121:
#line 1511 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 122:
#line 1517 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 123:
#line 1521 "parser-sming.y"
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
#line 1536 "parser-sming.y"
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
#line 1550 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 126:
#line 1555 "parser-sming.y"
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
#line 1567 "parser-sming.y"
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
#line 1580 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 129:
#line 1584 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 130:
#line 1590 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 131:
#line 1594 "parser-sming.y"
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
#line 1609 "parser-sming.y"
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
#line 1626 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(smiMkTime(yyvsp[-6].text), yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 134:
#line 1634 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 135:
#line 1640 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 136:
#line 1644 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 137:
#line 1651 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 138:
#line 1657 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 139:
#line 1661 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 140:
#line 1668 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 146:
#line 1682 "parser-sming.y"
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
#line 1703 "parser-sming.y"
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
#line 1722 "parser-sming.y"
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
#line 1743 "parser-sming.y"
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
#line 1762 "parser-sming.y"
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
#line 1783 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 152:
#line 1787 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 153:
#line 1793 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 154:
#line 1797 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 155:
#line 1803 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 156:
#line 1809 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 157:
#line 1813 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 158:
#line 1819 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 159:
#line 1825 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 160:
#line 1831 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 161:
#line 1837 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 162:
#line 1843 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 163:
#line 1849 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 164:
#line 1853 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 165:
#line 1859 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 166:
#line 1865 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 167:
#line 1869 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 168:
#line 1875 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 169:
#line 1881 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 170:
#line 1885 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 171:
#line 1891 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 172:
#line 1897 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 173:
#line 1901 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 174:
#line 1907 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 175:
#line 1913 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 176:
#line 1917 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 177:
#line 1923 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 178:
#line 1929 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 179:
#line 1933 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 180:
#line 1939 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 181:
#line 1945 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 182:
#line 1949 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 183:
#line 1955 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 184:
#line 1961 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 185:
#line 1965 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 186:
#line 1972 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 187:
#line 1978 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 188:
#line 1982 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 189:
#line 1989 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 190:
#line 1995 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 191:
#line 1999 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 192:
#line 2006 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 193:
#line 2012 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 194:
#line 2016 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 195:
#line 2022 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 196:
#line 2029 "parser-sming.y"
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
#line 2042 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 198:
#line 2051 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text, thisParserPtr);
			    yyval.optionPtr->description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 199:
#line 2059 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 200:
#line 2063 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 201:
#line 2069 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 202:
#line 2075 "parser-sming.y"
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
#line 2088 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 204:
#line 2099 "parser-sming.y"
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
#line 2122 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 206:
#line 2126 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 207:
#line 2132 "parser-sming.y"
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
#line 2144 "parser-sming.y"
{
			    yyval.typePtr = typeSmingObjectIdentifierPtr;
			;
    break;}
case 209:
#line 2148 "parser-sming.y"
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
#line 2160 "parser-sming.y"
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
#line 2172 "parser-sming.y"
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
#line 2184 "parser-sming.y"
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
#line 2196 "parser-sming.y"
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
#line 2208 "parser-sming.y"
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
#line 2220 "parser-sming.y"
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
#line 2232 "parser-sming.y"
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
#line 2244 "parser-sming.y"
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
#line 2258 "parser-sming.y"
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
#line 2277 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 220:
#line 2281 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 221:
#line 2287 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 222:
#line 2291 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 223:
#line 2297 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 224:
#line 2301 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 225:
#line 2308 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 226:
#line 2316 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 227:
#line 2320 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 228:
#line 2326 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 229:
#line 2332 "parser-sming.y"
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
#line 2345 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 231:
#line 2351 "parser-sming.y"
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
#line 2363 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 233:
#line 2367 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 234:
#line 2373 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 235:
#line 2379 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 236:
#line 2383 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 237:
#line 2390 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 238:
#line 2398 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 239:
#line 2402 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 240:
#line 2408 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 241:
#line 2414 "parser-sming.y"
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
#line 2427 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 243:
#line 2433 "parser-sming.y"
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
#line 2452 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 245:
#line 2456 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 246:
#line 2462 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 247:
#line 2468 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 248:
#line 2475 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 249:
#line 2483 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 250:
#line 2487 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 251:
#line 2493 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 252:
#line 2500 "parser-sming.y"
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
#line 2514 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 254:
#line 2520 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->name = yyvsp[-6].text;
			    yyval.namedNumberPtr->valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 255:
#line 2528 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 256:
#line 2536 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 257:
#line 2540 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 258:
#line 2546 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 259:
#line 2552 "parser-sming.y"
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
#line 2565 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 261:
#line 2571 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 262:
#line 2579 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 263:
#line 2583 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 264:
#line 2589 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 265:
#line 2595 "parser-sming.y"
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
#line 2608 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 267:
#line 2614 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 268:
#line 2622 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 269:
#line 2626 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 270:
#line 2632 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 271:
#line 2638 "parser-sming.y"
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
#line 2651 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 273:
#line 2657 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 274:
#line 2663 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 275:
#line 2667 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 276:
#line 2675 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 277:
#line 2679 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 278:
#line 2685 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 279:
#line 2691 "parser-sming.y"
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
#line 2704 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 281:
#line 2710 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 282:
#line 2714 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 283:
#line 2720 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 284:
#line 2724 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 285:
#line 2730 "parser-sming.y"
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
#line 2741 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 287:
#line 2747 "parser-sming.y"
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
#line 2758 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 289:
#line 2764 "parser-sming.y"
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
#line 2778 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 291:
#line 2782 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 292:
#line 2788 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 2792 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 294:
#line 2802 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 295:
#line 2809 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 296:
#line 2815 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 297:
#line 2821 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 298:
#line 2834 "parser-sming.y"
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
#line 2859 "parser-sming.y"
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
#line 2895 "parser-sming.y"
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
#line 2915 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 302:
#line 2921 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 303:
#line 2927 "parser-sming.y"
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
#line 2939 "parser-sming.y"
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
#line 2961 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 306:
#line 2966 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 307:
#line 2973 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 308:
#line 2977 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 309:
#line 2981 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 310:
#line 2987 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 311:
#line 2991 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 312:
#line 2995 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 313:
#line 2999 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 314:
#line 3005 "parser-sming.y"
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
#line 3034 "parser-sming.y"
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
#line 3061 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 317:
#line 3067 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 318:
#line 3071 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 319:
#line 3078 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 320:
#line 3082 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 321:
#line 3092 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 322:
#line 3101 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 323:
#line 3107 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = 42;
			;
    break;}
case 324:
#line 3114 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 325:
#line 3122 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 326:
#line 3130 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 327:
#line 3134 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 328:
#line 3144 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 329:
#line 3151 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 330:
#line 3157 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 331:
#line 3163 "parser-sming.y"
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
#line 3168 "parser-sming.y"


#endif
			
