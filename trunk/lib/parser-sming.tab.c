
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

#line 25 "parser-sming.y"


#ifdef BACKEND_SMING
    
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
    
#include "defs.h"
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
static List *firstIndexlabelPtr = NULL;
 
 

Type *
findType(spec, parserPtr)
    char *spec;
    Parser *parserPtr; 
{
    Type *typePtr;
    Import *importPtr;
    char *module, *type;
    
    if (!strstr(spec, SMI_NAMESPACE_OPERATOR)) {
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
    
    if (!strstr(spec, SMI_NAMESPACE_OPERATOR)) {
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
 

			    

#line 168 "parser-sming.y"
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



#define	YYFINAL		730
#define	YYFLAG		-32768
#define	YYNTBASE	74

#define YYTRANSLATE(x) ((unsigned)(x) <= 319 ? yytranslate[x] : 298)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    68,
    69,     2,     2,    71,    72,    73,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    67,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    65,    70,    66,     2,     2,     2,     2,     2,
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
    56,    57,    58,    59,    60,    61,    62,    63,    64
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
   707,   709,   711,   714,   717,   728,   729,   731,   733,   736,
   739,   753,   755,   757,   760,   762,   765,   768,   771,   774,
   777,   780,   783,   786,   789,   792,   793,   796,   798,   800,
   801,   804,   811,   812,   814,   816,   819,   824,   827,   828,
   830,   835,   836,   839,   846,   847,   849,   851,   854,   859,
   862,   863,   865,   870,   876,   880,   881,   883,   885,   888,
   893,   901,   905,   906,   908,   910,   913,   918,   922,   923,
   925,   927,   930,   935,   939,   940,   942,   944,   947,   952,
   958,   960,   964,   965,   967,   969,   972,   977,   979,   981,
   983,   985,   989,   991,   995,   997,  1000,  1001,  1003,  1005,
  1008,  1011,  1013,  1015,  1017,  1019,  1021,  1024,  1026,  1028,
  1030,  1032,  1035,  1040,  1042,  1044,  1046,  1048,  1050,  1052,
  1054,  1057,  1059,  1061,  1062,  1064,  1066,  1069,  1072,  1074,
  1076,  1078,  1081,  1083,  1085,  1086,  1087,  1088
};

static const short yyrhs[] = {   296,
    75,     0,     0,    76,     0,    77,     0,    76,    77,     0,
    78,   296,     0,     0,     0,     0,     0,     0,     0,     0,
    11,   295,     5,    79,   295,     6,    80,   296,    65,   297,
   178,   204,   297,    81,   206,   297,    82,   207,   297,    83,
   219,   297,    84,   220,    85,   184,    86,    98,   146,   156,
   166,    66,   296,    67,     0,     0,    87,     0,    88,     0,
    87,    88,     0,    89,   297,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    20,   295,     5,    90,   296,    65,
   297,   188,   297,    91,   216,    92,   208,    93,   210,    94,
   212,    95,   218,    96,   220,    97,    66,   296,    67,     0,
     0,    99,     0,   100,     0,    99,   100,     0,   101,   297,
     0,   102,     0,   108,     0,   119,     0,     0,     0,     0,
     0,     0,    23,   295,     6,   103,   296,    65,   297,   204,
   297,   104,   212,   105,   218,   106,   220,   107,    66,   296,
    67,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    24,   295,     6,   109,   296,    65,   297,   204,
   297,   110,   190,   297,   111,   215,   297,   112,   216,   113,
   208,   114,   210,   115,   212,   116,   219,   297,   117,   220,
   118,    66,   296,    67,     0,     0,     0,     0,     0,     0,
    25,   295,     6,   120,   296,    65,   297,   204,   297,   121,
   212,   122,   219,   297,   123,   220,   124,   125,   297,    66,
   296,    67,     0,     0,     0,     0,     0,     0,     0,     0,
    27,   295,     6,   126,   296,    65,   297,   204,   297,   127,
   193,   297,   128,   200,   129,   212,   130,   219,   297,   131,
   220,   132,   133,    66,   296,    67,     0,   134,     0,   133,
   134,     0,   135,   297,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    26,   295,     6,   136,   296,
    65,   297,   204,   297,   137,   190,   297,   138,   215,   297,
   139,   216,   140,   208,   141,   210,   142,   212,   143,   219,
   297,   144,   220,   145,    66,   296,    67,     0,     0,   147,
     0,   148,     0,   147,   148,     0,   149,   297,     0,     0,
     0,     0,     0,     0,     0,    28,   295,     6,   150,   296,
    65,   297,   204,   297,   151,   222,   152,   212,   153,   219,
   297,   154,   220,   155,    66,   296,    67,     0,     0,   157,
     0,   158,     0,   157,   158,     0,   159,   297,     0,     0,
     0,     0,     0,     0,     0,    29,   295,     6,   160,   296,
    65,   297,   204,   297,   161,   223,   297,   162,   212,   163,
   219,   297,   164,   220,   165,    66,   296,    67,     0,     0,
   167,     0,   168,     0,   167,   168,     0,   169,   297,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    30,   295,
     6,   170,   296,    65,   297,   204,   297,   171,   212,   172,
   219,   297,   173,   220,   174,   224,   175,   226,   176,   230,
   177,    66,   296,    67,     0,     0,   179,     0,   180,     0,
   179,   180,     0,   181,   297,     0,     0,     0,    12,   295,
     5,   182,   296,    68,   296,   261,   183,   296,    69,   296,
    67,     0,     0,   185,     0,   186,     0,   185,   186,     0,
   187,   297,     0,    13,   296,    65,   297,   205,   297,   219,
   297,    66,   296,    67,     0,    21,   295,   234,   296,    67,
     0,     0,   190,   297,     0,    21,   295,   234,   296,    67,
     0,     0,   192,   297,     0,    22,   295,   234,   296,    67,
     0,   194,     0,   195,     0,   196,     0,   197,     0,   198,
     0,    37,   199,   296,    68,   296,   265,   296,    69,   296,
    67,     0,    38,   295,   276,   296,    67,     0,    39,   295,
   276,   199,   296,    68,   296,   265,   296,    69,   296,    67,
     0,    40,   295,   276,   296,    67,     0,    41,   295,   276,
   199,   296,    68,   296,   265,   296,    69,   296,    67,     0,
     0,   295,    36,     0,     0,   201,   297,     0,    42,   202,
   296,    67,     0,     0,   296,   203,     0,    68,   296,   265,
   296,    69,     0,    14,   295,   285,   296,    67,     0,    15,
   295,   281,   296,    67,     0,    16,   295,   277,   296,    67,
     0,    17,   295,   277,   296,    67,     0,     0,   209,   297,
     0,    31,   295,   277,   296,    67,     0,     0,   211,   297,
     0,    32,   295,   277,   296,    67,     0,     0,   213,   297,
     0,    33,   295,   283,   296,    67,     0,     0,   215,   297,
     0,    34,   295,   284,   296,    67,     0,     0,   217,   297,
     0,    35,   295,   282,   296,    67,     0,     0,   219,   297,
     0,    18,   295,   277,   296,    67,     0,     0,   221,   297,
     0,    19,   295,   277,   296,    67,     0,     0,   223,   297,
     0,    43,   296,    68,   296,   265,   296,    69,   296,    67,
     0,     0,   225,   297,     0,    44,   296,    68,   296,   265,
   296,    69,   296,    67,     0,     0,   227,     0,   228,     0,
   227,   228,     0,   229,   297,     0,    45,   295,   276,   296,
    65,   219,   297,    66,   296,    67,     0,     0,   231,     0,
   232,     0,   231,   232,     0,   233,   297,     0,    46,   295,
   276,   296,    65,   189,   191,   214,   219,   297,    66,   296,
    67,     0,   235,     0,   236,     0,    47,   239,     0,    48,
     0,    49,   239,     0,    50,   239,     0,    51,   239,     0,
    52,   239,     0,    53,   247,     0,    54,   247,     0,    55,
   247,     0,    57,   255,     0,    56,   255,     0,   275,   237,
     0,     0,   296,   238,     0,   240,     0,   248,     0,     0,
   296,   240,     0,    68,   296,   244,   241,   296,    69,     0,
     0,   242,     0,   243,     0,   242,   243,     0,   296,    70,
   296,   244,     0,   293,   245,     0,     0,   246,     0,   296,
     3,   296,   293,     0,     0,   296,   248,     0,    68,   296,
   252,   249,   296,    69,     0,     0,   250,     0,   251,     0,
   250,   251,     0,   296,    70,   296,   252,     0,     7,   253,
     0,     0,   254,     0,   296,     3,   296,     7,     0,    68,
   296,   256,   296,    69,     0,   260,   257,   294,     0,     0,
   258,     0,   259,     0,   258,   259,     0,   296,    71,   296,
   260,     0,     6,   296,    68,   296,   291,   296,    69,     0,
   274,   262,   294,     0,     0,   263,     0,   264,     0,   263,
   264,     0,   296,    71,   296,   274,     0,     0,     0,   294,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   296,
    71,   296,     0,     0,   276,   266,   294,     0,     0,   267,
     0,   268,     0,   267,   268,     0,   296,    71,   296,   276,
     0,    68,   296,   270,   296,    69,     0,   294,     0,     6,
   271,   294,     0,     0,   272,     0,   273,     0,   272,   273,
     0,   296,    71,   296,     6,     0,     5,     0,     6,     0,
   275,     0,   276,     0,     5,     4,     5,     0,     5,     0,
     5,     4,     6,     0,     6,     0,     8,   278,     0,     0,
   279,     0,   280,     0,   279,   280,     0,   296,     8,     0,
     8,     0,     8,     0,     8,     0,   269,     0,     9,     0,
    72,     9,     0,    10,     0,     7,     0,   277,     0,   276,
     0,   276,   288,     0,   290,    73,   290,   288,     0,    58,
     0,    59,     0,    60,     0,    61,     0,    62,     0,    63,
     0,    64,     0,   286,   287,     0,   276,     0,   290,     0,
     0,   288,     0,   289,     0,   288,   289,     0,    73,   290,
     0,     9,     0,    10,     0,     9,     0,    72,     9,     0,
   291,     0,   292,     0,     0,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   434,   444,   448,   454,   458,   473,   487,   507,   519,   550,
   557,   564,   571,   577,   631,   635,   645,   649,   665,   679,
   683,   704,   710,   716,   722,   728,   734,   739,   747,   751,
   761,   765,   781,   795,   796,   797,   800,   804,   818,   824,
   837,   842,   850,   854,   866,   879,   886,   892,   898,   904,
   909,   916,   921,   930,   934,   949,   954,   961,   966,   975,
   979,   993,  1014,  1020,  1025,  1032,  1038,  1046,  1054,  1069,
  1083,  1087,  1099,  1112,  1119,  1125,  1131,  1137,  1142,  1149,
  1154,  1163,  1167,  1177,  1181,  1197,  1211,  1215,  1229,  1244,
  1249,  1257,  1262,  1270,  1274,  1284,  1288,  1303,  1317,  1321,
  1332,  1348,  1353,  1360,  1365,  1373,  1377,  1387,  1391,  1407,
  1421,  1425,  1439,  1444,  1451,  1457,  1472,  1476,  1504,  1512,
  1516,  1522,  1526,  1541,  1555,  1559,  1572,  1585,  1589,  1595,
  1599,  1614,  1628,  1637,  1645,  1649,  1655,  1662,  1666,  1672,
  1679,  1680,  1681,  1682,  1683,  1686,  1708,  1725,  1748,  1765,
  1788,  1792,  1798,  1802,  1808,  1814,  1818,  1824,  1830,  1836,
  1842,  1848,  1854,  1858,  1864,  1870,  1874,  1880,  1886,  1890,
  1896,  1902,  1906,  1912,  1918,  1922,  1928,  1934,  1938,  1944,
  1950,  1954,  1960,  1966,  1970,  1976,  1983,  1987,  1993,  2000,
  2004,  2010,  2016,  2030,  2036,  2047,  2051,  2057,  2063,  2076,
  2082,  2110,  2114,  2120,  2132,  2136,  2148,  2160,  2172,  2184,
  2196,  2208,  2220,  2232,  2246,  2266,  2270,  2276,  2280,  2286,
  2290,  2296,  2305,  2309,  2315,  2321,  2334,  2340,  2352,  2356,
  2362,  2368,  2372,  2378,  2387,  2391,  2397,  2403,  2416,  2422,
  2441,  2445,  2451,  2457,  2463,  2472,  2476,  2482,  2488,  2503,
  2509,  2517,  2525,  2529,  2535,  2541,  2554,  2560,  2568,  2572,
  2578,  2584,  2597,  2603,  2611,  2615,  2621,  2627,  2640,  2646,
  2652,  2656,  2664,  2668,  2674,  2680,  2693,  2699,  2703,  2709,
  2713,  2719,  2732,  2738,  2751,  2757,  2771,  2775,  2781,  2785,
  2795,  2801,  2808,  2814,  2827,  2852,  2888,  2908,  2914,  2920,
  2932,  2954,  2959,  2966,  2970,  2974,  2980,  2984,  2988,  2992,
  2998,  3027,  3057,  3063,  3067,  3074,  3078,  3088,  3097,  3103,
  3110,  3118,  3126,  3130,  3140,  3147,  3153,  3159
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
"sparseKeyword","expandsKeyword","createKeyword","objectsKeyword","mandatoryKeyword",
"optionalKeyword","refineKeyword","OctetStringKeyword","ObjectIdentifierKeyword",
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
"objectsStatement_stmtsep_01","objectsStatement","mandatoryStatement_stmtsep_01",
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
    74,    75,    75,    76,    76,    77,    79,    80,    81,    82,
    83,    84,    85,    78,    86,    86,    87,    87,    88,    90,
    91,    92,    93,    94,    95,    96,    97,    89,    98,    98,
    99,    99,   100,   101,   101,   101,   103,   104,   105,   106,
   107,   102,   109,   110,   111,   112,   113,   114,   115,   116,
   117,   118,   108,   120,   121,   122,   123,   124,   119,   126,
   127,   128,   129,   130,   131,   132,   125,   133,   133,   134,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   135,   146,   146,   147,   147,   148,   150,   151,   152,   153,
   154,   155,   149,   156,   156,   157,   157,   158,   160,   161,
   162,   163,   164,   165,   159,   166,   166,   167,   167,   168,
   170,   171,   172,   173,   174,   175,   176,   177,   169,   178,
   178,   179,   179,   180,   182,   183,   181,   184,   184,   185,
   185,   186,   187,   188,   189,   189,   190,   191,   191,   192,
   193,   193,   193,   193,   193,   194,   195,   196,   197,   198,
   199,   199,   200,   200,   201,   202,   202,   203,   204,   205,
   206,   207,   208,   208,   209,   210,   210,   211,   212,   212,
   213,   214,   214,   215,   216,   216,   217,   218,   218,   219,
   220,   220,   221,   222,   222,   223,   224,   224,   225,   226,
   226,   227,   227,   228,   229,   230,   230,   231,   231,   232,
   233,   234,   234,   235,   235,   235,   235,   235,   235,   235,
   235,   235,   235,   235,   236,   237,   237,   238,   238,   239,
   239,   240,   241,   241,   242,   242,   243,   244,   245,   245,
   246,   247,   247,   248,   249,   249,   250,   250,   251,   252,
   253,   253,   254,   255,   256,   257,   257,   258,   258,   259,
   260,   261,   262,   262,   263,   263,   264,    -1,    -1,    -1,
    -1,    -1,    -1,   265,   266,   266,   267,   267,   268,   269,
   270,   270,   271,   271,   272,   272,   273,   274,   274,    -1,
    -1,   275,   275,   276,   276,   277,   278,   278,   279,   279,
   280,   281,    -1,    -1,   282,   282,   282,   282,   282,   282,
   282,   282,   282,   283,   283,   283,   284,   284,   284,   284,
   285,   286,   286,   287,   287,   288,   288,   289,   290,   291,
   291,   292,   293,   293,   294,   295,   296,   297
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
     1,     1,     2,     2,    10,     0,     1,     1,     2,     2,
    13,     1,     1,     2,     1,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     0,     2,     1,     1,     0,
     2,     6,     0,     1,     1,     2,     4,     2,     0,     1,
     4,     0,     2,     6,     0,     1,     1,     2,     4,     2,
     0,     1,     4,     5,     3,     0,     1,     1,     2,     4,
     7,     3,     0,     1,     1,     2,     4,     3,     0,     1,
     1,     2,     4,     3,     0,     1,     1,     2,     4,     5,
     1,     3,     0,     1,     1,     2,     4,     1,     1,     1,
     1,     3,     1,     3,     1,     2,     0,     1,     1,     2,
     2,     1,     1,     1,     1,     1,     2,     1,     1,     1,
     1,     2,     4,     1,     1,     1,     1,     1,     1,     1,
     2,     1,     1,     0,     1,     1,     2,     2,     1,     1,
     1,     2,     1,     1,     0,     0,     0,     0
};

static const short yydefact[] = {   327,
     2,   326,     1,     3,     4,   327,     0,     5,     6,     7,
   326,     0,     8,   327,     0,   328,   120,   326,     0,   121,
   122,   328,     0,   326,   328,   123,   124,   125,     0,     9,
   327,     0,   285,   319,   312,   327,   314,   313,     0,     0,
     0,     0,     0,   311,   315,   316,   326,   328,   327,   284,
   159,   318,   317,     0,    10,     0,   287,   327,     0,   278,
   279,   126,   253,   286,   288,   289,     0,     0,   326,   328,
   327,   325,   254,   255,     0,   290,   291,   161,     0,    11,
     0,   252,   256,   327,   327,     0,   327,     0,     0,   326,
   328,     0,   257,   162,     0,    12,   127,   327,   181,     0,
   326,    13,   328,   180,     0,   128,   182,   327,   327,    15,
   129,   130,   328,     0,     0,   326,    29,    16,    17,   328,
   131,   132,   183,   328,     0,   326,   326,   326,    82,    30,
    31,   328,    34,    35,    36,    18,    19,     0,    20,     0,
     0,     0,   326,    94,    83,    84,   328,    32,    33,   326,
   328,   327,    37,    43,    54,     0,   326,   106,    95,    96,
   328,    85,    86,     0,     0,     0,   327,   327,   327,    87,
     0,   326,     0,   107,   108,   328,    97,    98,   292,   327,
   328,   328,     0,     0,     0,   327,    99,     0,   327,   109,
   110,     0,     0,     0,   328,   328,   328,     0,   327,   111,
     0,   160,   327,   326,   328,     0,     0,     0,   328,     0,
   327,    14,     0,     0,    21,   328,   328,   328,     0,   328,
     0,   133,   283,   220,   205,   220,   220,   220,   220,   232,
   232,   232,     0,     0,   327,   202,   203,   216,   175,    38,
    44,    55,   328,     0,   328,     0,   204,     0,   206,   207,
   208,   209,   210,     0,   211,   212,   327,   214,   213,     0,
   215,     0,   326,    22,   328,   169,     0,   169,    88,   328,
     0,   282,   327,   221,   327,   233,     0,   134,   327,   217,
   218,   219,     0,   163,   176,   326,    39,   328,   326,   328,
    56,   184,   100,   328,     0,     0,   327,   327,   246,     0,
   299,   296,   298,   327,     0,   295,   301,   300,   327,     0,
   326,    23,   328,     0,   178,   170,     0,    45,     0,   327,
    89,   328,     0,   112,   321,   320,     0,   223,   323,   324,
   229,   241,   235,     0,     0,   325,   247,   248,     0,   325,
   297,   302,     0,     0,     0,   166,   164,   304,   305,   306,
   327,    40,   328,   327,     0,   328,     0,   169,   185,   328,
   169,   322,   327,   224,   225,     0,   228,   230,     0,   240,
   242,     0,   327,   236,   237,     0,   327,   244,   245,   249,
   327,   273,   327,   271,   177,     0,   327,   326,    24,   328,
     0,   181,   179,     0,   326,   328,    57,   327,    90,   101,
   113,     0,   226,   327,   327,   327,     0,   238,   327,     0,
     0,   325,   274,   275,     0,     0,   303,     0,     0,   169,
   167,   171,    41,   137,     0,    46,   181,     0,     0,   169,
     0,   222,     0,     0,     0,   234,     0,   327,   250,   272,
   276,   327,   270,   165,   327,    25,     0,   307,   308,   309,
   310,   327,   175,    58,   327,   265,   328,   102,   328,   227,
   231,   243,   239,     0,     0,     0,   178,   327,     0,    47,
     0,     0,   325,   266,   267,     0,    91,     0,   114,   251,
   277,   168,    26,     0,   174,   163,   326,   328,   327,   264,
   268,   327,   181,   328,   181,   181,    42,    48,     0,     0,
     0,     0,    92,   103,   115,    27,   166,    60,   327,   186,
   269,     0,   181,   187,     0,    49,   327,     0,   327,   104,
   327,   116,   328,   327,   169,     0,    59,     0,     0,     0,
   190,   188,     0,    50,   328,    93,   327,   327,   326,   117,
   191,   192,   328,    28,     0,     0,     0,     0,     0,   196,
   193,   194,   328,   328,   105,   327,   327,   326,   118,   197,
   198,   328,    51,    61,     0,     0,     0,     0,   199,   200,
   181,     0,   327,     0,   327,   327,    52,   151,   326,   326,
   326,   326,   328,   141,   142,   143,   144,   145,     0,   328,
     0,     0,     0,   327,     0,     0,     0,     0,     0,    62,
   189,     0,   135,   119,   327,     0,   152,   327,   151,   327,
   151,   153,   327,   138,   328,     0,   327,     0,   327,     0,
   327,   156,    63,   328,     0,   326,   172,   328,   136,    53,
     0,   147,     0,   149,     0,   327,     0,   169,   154,   195,
     0,     0,   328,   139,   327,   327,   327,     0,   327,   157,
    64,   327,   328,   173,     0,     0,     0,   155,     0,     0,
     0,     0,   327,   327,   327,   327,   328,   140,   327,     0,
     0,     0,     0,    65,     0,   146,   327,   327,   158,   181,
   201,     0,     0,    66,   148,   150,     0,   326,     0,    68,
   328,     0,   327,    69,    70,    71,     0,   327,    67,     0,
   328,     0,   328,    72,     0,   328,    73,     0,   328,    74,
   175,    75,   163,    76,   166,    77,   169,    78,     0,   328,
    79,   181,    80,     0,   327,     0,    81,     0,     0,     0
};

static const short yydefgoto[] = {   728,
     3,     4,     5,     6,    11,    14,    39,    59,    86,    99,
   106,   117,   118,   119,   120,   152,   239,   284,   346,   420,
   467,   496,   515,   129,   130,   131,   132,   133,   167,   266,
   315,   392,   447,   134,   168,   267,   355,   453,   486,   507,
   525,   545,   571,   593,   135,   169,   268,   319,   427,   471,
   488,   517,   572,   612,   638,   660,   680,   687,   689,   690,
   691,   698,   705,   708,   711,   713,   715,   717,   719,   722,
   724,   144,   145,   146,   147,   186,   292,   358,   429,   493,
   512,   158,   159,   160,   161,   199,   323,   430,   478,   513,
   529,   173,   174,   175,   176,   211,   361,   431,   495,   514,
   531,   550,   568,    19,    20,    21,    22,    31,    71,   110,
   111,   112,   113,   205,   614,   290,   627,   628,   583,   584,
   585,   586,   587,   588,   594,   623,   624,   636,   650,    25,
   151,    48,    70,   312,   313,   389,   390,   287,   288,   642,
   396,   264,   265,   352,   353,   102,   103,   321,   322,   522,
   523,   540,   541,   542,   543,   559,   560,   561,   562,   235,
   236,   237,   261,   280,   247,   274,   363,   364,   365,   328,
   367,   368,   253,   276,   373,   374,   375,   333,   370,   371,
   258,   298,   336,   337,   338,   299,    62,    72,    73,    74,
   455,   473,   474,   475,   306,   383,   412,   413,   414,    63,
   238,   456,    58,    64,    65,    66,   180,   309,   351,   452,
    36,    37,    44,    45,    46,    38,   329,   330,   331,    82,
   595,   248,    17
};

static const short yypact[] = {-32768,
    22,-32768,-32768,    22,-32768,-32768,    20,-32768,-32768,-32768,
-32768,    31,-32768,-32768,   -25,-32768,    39,-32768,    50,    39,
-32768,-32768,    65,-32768,-32768,-32768,-32768,-32768,    33,-32768,
-32768,    68,-32768,-32768,-32768,-32768,    28,-32768,    72,    45,
    87,    40,   106,-32768,    28,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   109,-32768,    61,   112,-32768,    97,-32768,
-32768,-32768,    51,-32768,   112,-32768,   115,    57,-32768,-32768,
-32768,-32768,    51,-32768,    54,-32768,-32768,-32768,   109,-32768,
    63,-32768,-32768,-32768,-32768,   108,-32768,    61,    66,-32768,
-32768,    67,-32768,-32768,   109,-32768,-32768,-32768,   116,    70,
-32768,-32768,-32768,-32768,   109,   136,-32768,-32768,-32768,   130,
   136,-32768,-32768,    84,    92,-32768,    55,   130,-32768,-32768,
-32768,-32768,-32768,-32768,   151,-32768,-32768,-32768,   131,    55,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   143,-32768,   154,
   155,   156,-32768,   134,   131,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   159,-32768,   140,   134,-32768,
-32768,-32768,-32768,   158,   108,   107,-32768,-32768,-32768,-32768,
   165,-32768,   110,   140,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   113,   114,   117,-32768,-32768,   167,-32768,-32768,
-32768,   118,   111,   153,-32768,-32768,-32768,   119,-32768,-32768,
   120,-32768,-32768,-32768,-32768,    50,    50,    50,-32768,   123,
-32768,-32768,   124,    91,-32768,-32768,-32768,-32768,    50,-32768,
   125,-32768,   177,   127,-32768,   127,   127,   127,   127,   127,
   127,   127,   129,   129,-32768,-32768,-32768,   127,   148,-32768,
-32768,-32768,-32768,    50,-32768,   187,-32768,   132,-32768,-32768,
-32768,-32768,-32768,   133,-32768,-32768,-32768,-32768,-32768,   126,
-32768,   138,-32768,-32768,-32768,   169,   183,   169,-32768,-32768,
    50,-32768,-32768,-32768,-32768,-32768,   192,-32768,-32768,-32768,
-32768,-32768,    38,   176,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   166,-32768,-32768,    14,   203,-32768,-32768,    51,    25,
-32768,   139,-32768,-32768,   204,-32768,    28,-32768,-32768,   141,
-32768,-32768,-32768,    23,   108,-32768,    91,-32768,   108,-32768,
-32768,-32768,   166,-32768,-32768,-32768,   206,   146,-32768,-32768,
   214,   214,   146,   152,   150,-32768,    51,-32768,   157,   216,
-32768,    28,   160,   106,   109,   191,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   190,-32768,   171,   169,-32768,-32768,
   169,-32768,-32768,   146,-32768,   163,-32768,-32768,   223,-32768,
-32768,   231,-32768,   146,-32768,   170,-32768,-32768,-32768,-32768,
-32768,    51,-32768,-32768,-32768,    28,-32768,-32768,-32768,-32768,
   174,   116,-32768,   175,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   179,-32768,-32768,-32768,-32768,   180,-32768,-32768,    81,
   192,-32768,    51,-32768,   173,   181,    28,   178,   109,   169,
-32768,-32768,-32768,-32768,    -2,-32768,   116,    94,   108,   169,
   108,-32768,    14,    14,   239,-32768,   203,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   185,-32768,-32768,-32768,
-32768,-32768,   148,-32768,-32768,    51,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   184,   248,   188,   108,-32768,   193,-32768,
   234,   189,-32768,    51,-32768,   194,-32768,   108,-32768,-32768,
-32768,-32768,-32768,   195,-32768,   176,-32768,-32768,-32768,-32768,
-32768,-32768,   116,-32768,   116,   116,-32768,-32768,   257,   198,
   201,    94,-32768,-32768,-32768,-32768,   191,-32768,-32768,-32768,
-32768,   205,   116,   225,   208,-32768,-32768,   209,-32768,-32768,
-32768,-32768,-32768,-32768,   169,   207,-32768,   210,   212,   215,
   235,-32768,   217,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   235,-32768,-32768,-32768,   108,    50,   218,    94,    94,   236,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   236,
-32768,-32768,-32768,-32768,   219,   224,    94,   226,-32768,-32768,
   116,    15,-32768,   108,-32768,-32768,-32768,   254,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   227,-32768,
   228,   229,   240,-32768,   255,    94,    94,    94,    94,-32768,
-32768,   241,   183,-32768,-32768,   243,-32768,-32768,   254,-32768,
   254,   253,-32768,   290,-32768,   246,-32768,   249,-32768,   250,
-32768,   127,-32768,-32768,   251,-32768,   190,-32768,-32768,-32768,
    94,-32768,   247,-32768,   256,-32768,   258,   169,-32768,-32768,
    91,   108,-32768,-32768,-32768,-32768,-32768,   252,-32768,-32768,
-32768,-32768,-32768,-32768,   260,    94,    94,-32768,    94,   108,
   263,   261,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   267,
   269,   270,   271,-32768,   275,-32768,-32768,-32768,-32768,   116,
-32768,   277,   279,-32768,-32768,-32768,   310,-32768,     3,-32768,
-32768,   341,-32768,-32768,-32768,-32768,   282,-32768,-32768,   287,
-32768,    50,-32768,-32768,   183,-32768,-32768,   190,-32768,-32768,
   148,-32768,   176,-32768,   191,-32768,   169,-32768,   108,-32768,
-32768,   116,-32768,   291,-32768,   289,-32768,   359,   360,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   357,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   244,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   237,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -324,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   221,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   220,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   197,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   348,-32768,-32768,-32768,-32768,
-32768,   265,-32768,-32768,-32768,  -584,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -581,-32768,-32768,-32768,-32768,  -203,
-32768,-32768,-32768,  -477,-32768,  -494,-32768,  -266,-32768,-32768,
  -606,  -441,-32768,   -98,   -76,  -377,-32768,-32768,    62,-32768,
-32768,-32768,-32768,  -163,-32768,-32768,-32768,  -176,-32768,  -306,
-32768,-32768,-32768,-32768,  -152,   128,-32768,-32768,    24,   -47,
-32768,-32768,  -120,   135,-32768,-32768,    17,   -48,-32768,-32768,
   162,-32768,-32768,-32768,    64,   -19,-32768,-32768,-32768,   321,
  -528,-32768,-32768,   -74,-32768,-32768,-32768,-32768,   -10,   319,
-32768,    75,   -78,-32768,-32768,   343,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -281,   -37,   -36,     1,-32768,   -24,  -318,
   511,     0,   105
};


#define	YYLAST		1199


static const short yytable[] = {     1,
    85,   291,   216,   217,   218,     9,    52,    53,   498,    91,
   354,   470,   516,    15,   423,   243,    98,   379,   615,   556,
   643,   384,   325,   326,    10,   342,   108,   619,   688,   621,
    40,   332,     2,   325,   326,    42,    13,    32,    33,    16,
   270,    34,    32,    33,   301,    57,   302,   303,    56,   454,
    18,   578,   579,   580,   581,   582,    67,    68,   448,   449,
   450,   451,    75,    24,    67,    60,    61,   294,   693,    28,
    81,    41,    75,   249,   250,   251,   252,   126,   127,   128,
   348,   349,   350,    88,    89,   327,    92,    47,   181,   325,
   326,   399,    50,   440,   401,   223,   327,   100,    32,    33,
    43,   709,   645,    35,   417,   304,    51,   114,   115,   305,
   255,   256,    49,    69,    34,   503,    57,   505,   506,  -327,
   706,  -327,    77,    78,    84,    90,    27,   664,   665,    30,
   666,    87,    94,    97,   101,   520,   104,   224,   225,   226,
   227,   228,   229,   230,   231,   232,   233,   234,   109,   116,
   123,   166,    55,   446,   490,   139,   124,   150,   143,   153,
   154,   155,   157,   458,   170,   179,   183,   184,   185,   172,
   187,   182,   200,   204,    80,   189,   203,   195,   196,   192,
   246,   197,   263,   209,   202,   198,   212,   220,   201,   245,
   222,   272,   278,   577,  -327,    96,   257,   297,   210,   273,
   275,   286,   213,   289,   308,   279,   311,   107,   320,   332,
   221,  -319,   341,   344,   362,  -327,  -327,   122,   378,   377,
   716,   382,   388,   395,   137,   405,   385,   381,   138,   254,
   254,   254,   404,   406,   260,   714,   149,   262,   398,   409,
   422,   424,   356,   442,   444,   462,   310,   432,   436,   443,
   468,   163,   480,   481,   482,   165,   277,   489,   534,   485,
   487,   497,   508,   509,   492,   178,   387,   510,   521,   712,
   519,   535,   295,   524,   296,   527,   536,   537,   300,   539,
   191,   558,   538,   544,   555,   193,   194,   573,   574,  -326,
   607,   576,   603,   601,   622,   604,   334,   335,   339,   206,
   207,   208,   684,   340,    53,   605,   613,   386,   343,   215,
   617,   626,   630,   219,   646,   632,   634,   640,   658,   357,
   240,   241,   242,   647,   244,   649,   669,   366,   663,   668,
   369,   372,   376,   676,   652,   688,   339,   677,   678,   679,
   445,   681,   554,   685,   723,   686,   696,   269,   699,   271,
   391,   701,   457,   394,   459,   727,   725,   307,   729,   730,
     8,   136,   402,   366,   694,   162,   148,    26,   483,   285,
   190,   651,   407,   376,   293,   121,   410,   551,   177,    53,
   411,   415,   416,   569,   360,   460,   418,   403,   463,   281,
   408,   439,   316,    83,   318,   259,   282,   428,   324,   491,
   380,   494,   441,   433,   434,   435,    93,    76,   437,   461,
   438,     0,   415,     0,     0,     0,     0,   347,     0,     0,
     0,     0,     0,     0,     0,     0,   359,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   464,     0,     0,
     0,   465,     0,     0,   466,     0,     0,     0,     0,     0,
   718,   469,     0,     0,   472,   476,     0,   393,     0,     0,
   397,     0,     0,     0,   400,     0,     0,   484,   553,     0,
     0,     0,     0,   476,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   501,     0,
     0,   502,     0,     0,   421,     0,     0,   590,   703,     0,
   426,     0,     0,     0,     0,     0,     0,     0,   518,     0,
     0,     0,     7,     0,     0,     0,   526,     0,   528,     0,
   530,    12,     0,   533,     0,     0,     0,     0,    23,     0,
     0,     0,     0,     0,    29,     0,   547,   548,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   565,   566,    54,     0,     0,
     0,   477,     0,   479,     0,   653,     0,     0,     0,     0,
     0,     0,   589,     0,   591,   592,   511,     0,     0,    79,
     0,     0,     0,   667,     0,     0,     0,     0,     0,     0,
     0,     0,   500,   606,     0,     0,     0,     0,   504,     0,
    95,     0,     0,     0,   616,     0,     0,   618,     0,   620,
     0,   105,   625,     0,     0,     0,   631,     0,   633,     0,
   635,   637,     0,   557,     0,     0,   125,   532,     0,     0,
     0,     0,     0,     0,     0,   648,   140,   141,   142,   546,
     0,   575,   720,     0,   655,   656,   657,   552,   659,     0,
     0,   661,     0,   156,     0,     0,     0,   563,   564,     0,
   164,     0,   670,   671,   672,   673,   570,   171,   675,     0,
   608,   609,   610,   611,     0,     0,   682,   683,     0,     0,
     0,     0,   188,     0,     0,     0,     0,   600,     0,     0,
     0,     0,   697,     0,   602,     0,     0,   700,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   214,     0,     0,     0,     0,   629,
     0,     0,     0,     0,   726,     0,     0,     0,   639,     0,
     0,     0,   644,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   654,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   662,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   674,     0,   283,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   695,   314,     0,     0,   317,
     0,     0,     0,     0,     0,   702,     0,   704,     0,     0,
   707,     0,     0,   710,     0,     0,     0,     0,     0,     0,
     0,   345,     0,     0,   721,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   419,     0,
     0,     0,     0,     0,     0,   425,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   499,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   549,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   567,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   596,
   597,   598,   599,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   641,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   692
};

static const short yycheck[] = {     0,
    79,   268,   206,   207,   208,     6,    43,    45,   486,    86,
   317,   453,   507,    14,   392,   219,    95,   336,   603,   548,
   627,   340,     9,    10,     5,   307,   105,   609,    26,   611,
    31,     7,    11,     9,    10,    36,     6,     5,     6,    65,
   244,     9,     5,     6,     7,     8,     9,    10,    49,   427,
    12,    37,    38,    39,    40,    41,    57,    58,    61,    62,
    63,    64,    63,    14,    65,     5,     6,   271,    66,     5,
    71,     4,    73,   226,   227,   228,   229,    23,    24,    25,
    58,    59,    60,    84,    85,    72,    87,    16,   165,     9,
    10,   358,     6,   412,   361,     5,    72,    98,     5,     6,
    73,   708,   631,    29,   386,    68,    67,   108,   109,    72,
   231,   232,    68,    17,     9,   493,     8,   495,   496,     8,
   705,    71,     8,    67,    71,    18,    22,   656,   657,    25,
   659,    69,    67,    67,    19,   513,    67,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    56,    57,    13,    20,
    67,   152,    48,   420,   473,     5,    65,    15,    28,     6,
     6,     6,    29,   430,     6,     8,   167,   168,   169,    30,
     6,    65,     6,    21,    70,    66,    66,    65,    65,   180,
     4,    65,    35,    65,    67,   186,    67,    65,   189,    65,
    67,     5,    67,   571,    68,    91,    68,     6,   199,    68,
    68,    33,   203,    21,   283,    68,    31,   103,    43,     7,
   211,    73,     9,    73,     9,    70,     3,   113,    69,    68,
   715,     6,    32,    34,   120,     3,    67,    71,   124,   230,
   231,   232,    70,     3,   235,   713,   132,   238,    68,    70,
    67,    67,   319,    71,    67,     7,   283,    69,    69,    69,
    66,   147,    69,     6,    67,   151,   257,    69,   525,    67,
    27,    67,     6,    66,    71,   161,   345,    67,    44,   711,
    66,    65,   273,    66,   275,    67,    67,    66,   279,    45,
   176,    46,    68,    67,    67,   181,   182,    69,    65,    36,
    36,    66,    65,    67,    42,    67,   297,   298,   299,   195,
   196,   197,   680,   304,   342,    66,    66,   344,   309,   205,
    68,    22,    67,   209,    68,    67,    67,    67,    67,   320,
   216,   217,   218,    68,   220,    68,    66,   328,    69,    67,
   331,   332,   333,    67,   641,    26,   337,    69,    69,    69,
   419,    67,   546,    67,   722,    67,     6,   243,    67,   245,
   351,    65,   429,   354,   431,    67,    66,   283,     0,     0,
     4,   118,   363,   364,   689,   145,   130,    20,   467,   265,
   174,   638,   373,   374,   270,   111,   377,   541,   159,   417,
   381,   382,   383,   560,   323,   433,   387,   364,   437,   262,
   374,   411,   288,    73,   290,   234,   262,   398,   294,   474,
   337,   478,   413,   404,   405,   406,    88,    65,   409,   434,
   410,    -1,   413,    -1,    -1,    -1,    -1,   313,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   322,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   438,    -1,    -1,
    -1,   442,    -1,    -1,   445,    -1,    -1,    -1,    -1,    -1,
   717,   452,    -1,    -1,   455,   456,    -1,   353,    -1,    -1,
   356,    -1,    -1,    -1,   360,    -1,    -1,   468,   545,    -1,
    -1,    -1,    -1,   474,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   489,    -1,
    -1,   492,    -1,    -1,   390,    -1,    -1,   574,   702,    -1,
   396,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   509,    -1,
    -1,    -1,     2,    -1,    -1,    -1,   517,    -1,   519,    -1,
   521,    11,    -1,   524,    -1,    -1,    -1,    -1,    18,    -1,
    -1,    -1,    -1,    -1,    24,    -1,   537,   538,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   556,   557,    47,    -1,    -1,
    -1,   457,    -1,   459,    -1,   642,    -1,    -1,    -1,    -1,
    -1,    -1,   573,    -1,   575,   576,   502,    -1,    -1,    69,
    -1,    -1,    -1,   660,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   488,   594,    -1,    -1,    -1,    -1,   494,    -1,
    90,    -1,    -1,    -1,   605,    -1,    -1,   608,    -1,   610,
    -1,   101,   613,    -1,    -1,    -1,   617,    -1,   619,    -1,
   621,   622,    -1,   549,    -1,    -1,   116,   523,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   636,   126,   127,   128,   535,
    -1,   567,   719,    -1,   645,   646,   647,   543,   649,    -1,
    -1,   652,    -1,   143,    -1,    -1,    -1,   553,   554,    -1,
   150,    -1,   663,   664,   665,   666,   562,   157,   669,    -1,
   596,   597,   598,   599,    -1,    -1,   677,   678,    -1,    -1,
    -1,    -1,   172,    -1,    -1,    -1,    -1,   583,    -1,    -1,
    -1,    -1,   693,    -1,   590,    -1,    -1,   698,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,   615,
    -1,    -1,    -1,    -1,   725,    -1,    -1,    -1,   624,    -1,
    -1,    -1,   628,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   643,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   653,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   667,    -1,   263,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   691,   286,    -1,    -1,   289,
    -1,    -1,    -1,    -1,    -1,   701,    -1,   703,    -1,    -1,
   706,    -1,    -1,   709,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   311,    -1,    -1,   720,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   388,    -1,
    -1,    -1,    -1,    -1,    -1,   395,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   487,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   539,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   558,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   579,
   580,   581,   582,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   626,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   688
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
#line 435 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 445 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 449 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 455 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 459 "parser-sming.y"
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
#line 474 "parser-sming.y"
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
#line 488 "parser-sming.y"
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
			    firstIndexlabelPtr = NULL;
			;
    break;}
case 8:
#line 508 "parser-sming.y"
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
#line 522 "parser-sming.y"
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
#line 551 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 11:
#line 558 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 12:
#line 565 "parser-sming.y"
{
			    if (moduleObjectPtr && yyvsp[-1].text) {
				setObjectDescription(moduleObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 13:
#line 571 "parser-sming.y"
{
			    if (moduleObjectPtr && yyvsp[0].text) {
				setObjectReference(moduleObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 14:
#line 583 "parser-sming.y"
{
			    List *listPtr;
			    Object *objectPtr;
			    /*
			     * Walk through rows of this module with
			     * flag & FLAG_INDEXLABELS and convert their
			     * labelstrings to (Object *). This is the
			     * case for index column lists
			     * (indexPtr->listPtr[]->ptr), index related
			     * rows (indexPtr->rowPtr) and create lists
			     * (listPtr[]->ptr).
			     */
			    while (firstIndexlabelPtr) {
				for (listPtr =
		      ((Object *)(firstIndexlabelPtr->ptr))->indexPtr->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				if (((Object *)(firstIndexlabelPtr->ptr))->
				    indexPtr->rowPtr) {
				    objectPtr = findObject(
					((Object *)(firstIndexlabelPtr->ptr))->
					indexPtr->rowPtr,
					thisParserPtr);
				    ((Object *)(firstIndexlabelPtr->ptr))->
				    indexPtr->rowPtr = objectPtr;
				}
				for (listPtr =
		                ((Object *)(firstIndexlabelPtr->ptr))->listPtr;
				     listPtr; listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				deleteObjectFlags(
				    (Object *)(firstIndexlabelPtr->ptr),
				    FLAG_INDEXLABELS);
				listPtr = firstIndexlabelPtr->nextPtr;
				free(firstIndexlabelPtr);
				firstIndexlabelPtr = listPtr;
			    }
			    yyval.modulePtr = thisModulePtr;
			    moduleObjectPtr = NULL;
			;
    break;}
case 15:
#line 632 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 16:
#line 636 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 17:
#line 646 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 18:
#line 651 "parser-sming.y"
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
#line 666 "parser-sming.y"
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
#line 680 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 21:
#line 685 "parser-sming.y"
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
#line 704 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 23:
#line 710 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeFormat(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 24:
#line 716 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 25:
#line 722 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 26:
#line 728 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 27:
#line 734 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 28:
#line 740 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 29:
#line 748 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 30:
#line 752 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 31:
#line 762 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 32:
#line 767 "parser-sming.y"
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
#line 782 "parser-sming.y"
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
#line 801 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 38:
#line 806 "parser-sming.y"
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
#line 818 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 40:
#line 824 "parser-sming.y"
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
#line 837 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 42:
#line 843 "parser-sming.y"
{
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 43:
#line 851 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 44:
#line 856 "parser-sming.y"
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
#line 867 "parser-sming.y"
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
#line 880 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 47:
#line 886 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 48:
#line 892 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectFormat(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 49:
#line 898 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 50:
#line 904 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 51:
#line 910 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 52:
#line 916 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 53:
#line 922 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 54:
#line 931 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 55:
#line 936 "parser-sming.y"
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
#line 949 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 57:
#line 955 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 58:
#line 961 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 968 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 60:
#line 976 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 61:
#line 981 "parser-sming.y"
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
#line 994 "parser-sming.y"
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
				listPtr->nextPtr = firstIndexlabelPtr;
				firstIndexlabelPtr = listPtr;
			    }
			;
    break;}
case 63:
#line 1014 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectList(rowObjectPtr, yyvsp[0].listPtr);
			    }
			;
    break;}
case 64:
#line 1020 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 65:
#line 1026 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 66:
#line 1032 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 67:
#line 1039 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 68:
#line 1047 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 69:
#line 1055 "parser-sming.y"
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
#line 1070 "parser-sming.y"
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
#line 1084 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 72:
#line 1089 "parser-sming.y"
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
#line 1100 "parser-sming.y"
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
#line 1113 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 75:
#line 1119 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(columnObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 76:
#line 1125 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectFormat(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 77:
#line 1131 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectUnits(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 78:
#line 1137 "parser-sming.y"
{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 79:
#line 1143 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[-1].text) {
				setObjectDescription(columnObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 80:
#line 1149 "parser-sming.y"
{
			    if (columnObjectPtr && yyvsp[0].text) {
				setObjectReference(columnObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 81:
#line 1155 "parser-sming.y"
{
			    yyval.objectPtr = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 82:
#line 1164 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 83:
#line 1168 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 84:
#line 1178 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 85:
#line 1183 "parser-sming.y"
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
#line 1198 "parser-sming.y"
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
#line 1212 "parser-sming.y"
{
			    notificationIdentifier = yyvsp[0].text;
			;
    break;}
case 88:
#line 1217 "parser-sming.y"
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
#line 1229 "parser-sming.y"
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
#line 1244 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 91:
#line 1250 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 92:
#line 1257 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 93:
#line 1263 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 94:
#line 1271 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 95:
#line 1275 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 96:
#line 1285 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 97:
#line 1289 "parser-sming.y"
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
#line 1304 "parser-sming.y"
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
#line 1318 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 100:
#line 1323 "parser-sming.y"
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
#line 1333 "parser-sming.y"
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
#line 1348 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 103:
#line 1354 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 104:
#line 1360 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 105:
#line 1366 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 106:
#line 1374 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 107:
#line 1378 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 108:
#line 1388 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 109:
#line 1393 "parser-sming.y"
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
#line 1408 "parser-sming.y"
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
#line 1422 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 112:
#line 1427 "parser-sming.y"
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
#line 1439 "parser-sming.y"
{
			    if (complianceObjectPtr) {
				setObjectStatus(complianceObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 114:
#line 1445 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[-1].text) {
				setObjectDescription(complianceObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 115:
#line 1451 "parser-sming.y"
{
			    if (complianceObjectPtr && yyvsp[0].text) {
				setObjectReference(complianceObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 116:
#line 1457 "parser-sming.y"
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
#line 1472 "parser-sming.y"
{
			    complianceObjectPtr->optionlistPtr = yyvsp[0].listPtr;
			;
    break;}
case 118:
#line 1476 "parser-sming.y"
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
#line 1505 "parser-sming.y"
{
			    yyval.objectPtr = complianceObjectPtr;
			    complianceObjectPtr = NULL;
			    free(complianceIdentifier);
			;
    break;}
case 120:
#line 1513 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 121:
#line 1517 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 122:
#line 1523 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 123:
#line 1527 "parser-sming.y"
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
#line 1542 "parser-sming.y"
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
#line 1556 "parser-sming.y"
{
			    importModulename = util_strdup(yyvsp[0].text);
			;
    break;}
case 126:
#line 1561 "parser-sming.y"
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
#line 1573 "parser-sming.y"
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
#line 1586 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 129:
#line 1590 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 130:
#line 1596 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 131:
#line 1600 "parser-sming.y"
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
#line 1615 "parser-sming.y"
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
#line 1632 "parser-sming.y"
{
			    yyval.revisionPtr = addRevision(smiMkTime(yyvsp[-6].text), yyvsp[-4].text, thisParserPtr);
			;
    break;}
case 134:
#line 1640 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 135:
#line 1646 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 136:
#line 1650 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 137:
#line 1657 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 138:
#line 1663 "parser-sming.y"
{
			    yyval.typePtr = NULL;
			;
    break;}
case 139:
#line 1667 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-1].typePtr;
			;
    break;}
case 140:
#line 1674 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[-2].typePtr;
			;
    break;}
case 146:
#line 1688 "parser-sming.y"
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
#line 1709 "parser-sming.y"
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
#line 1728 "parser-sming.y"
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
#line 1749 "parser-sming.y"
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
#line 1768 "parser-sming.y"
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
#line 1789 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 152:
#line 1793 "parser-sming.y"
{
			    yyval.rc = 1;
			;
    break;}
case 153:
#line 1799 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 154:
#line 1803 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 155:
#line 1809 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 156:
#line 1815 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 157:
#line 1819 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 158:
#line 1825 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 159:
#line 1831 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 160:
#line 1837 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 161:
#line 1843 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 162:
#line 1849 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 163:
#line 1855 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 164:
#line 1859 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 165:
#line 1865 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 166:
#line 1871 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 167:
#line 1875 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 168:
#line 1881 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 169:
#line 1887 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 170:
#line 1891 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 171:
#line 1897 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 172:
#line 1903 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 173:
#line 1907 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 174:
#line 1913 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 175:
#line 1919 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 176:
#line 1923 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 177:
#line 1929 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 178:
#line 1935 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 179:
#line 1939 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 180:
#line 1945 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 181:
#line 1951 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 182:
#line 1955 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 183:
#line 1961 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 184:
#line 1967 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 185:
#line 1971 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 186:
#line 1978 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 187:
#line 1984 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 188:
#line 1988 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 189:
#line 1995 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 190:
#line 2001 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 191:
#line 2005 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 192:
#line 2011 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 193:
#line 2018 "parser-sming.y"
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
case 194:
#line 2031 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 195:
#line 2040 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text, thisParserPtr);
			    yyval.optionPtr->description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 196:
#line 2048 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 197:
#line 2052 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 198:
#line 2058 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 199:
#line 2064 "parser-sming.y"
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
case 200:
#line 2077 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 201:
#line 2088 "parser-sming.y"
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
case 202:
#line 2111 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 203:
#line 2115 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 204:
#line 2121 "parser-sming.y"
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
case 205:
#line 2133 "parser-sming.y"
{
			    yyval.typePtr = typeSmingObjectIdentifierPtr;
			;
    break;}
case 206:
#line 2137 "parser-sming.y"
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
case 207:
#line 2149 "parser-sming.y"
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
case 208:
#line 2161 "parser-sming.y"
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
case 209:
#line 2173 "parser-sming.y"
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
case 210:
#line 2185 "parser-sming.y"
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
case 211:
#line 2197 "parser-sming.y"
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
case 212:
#line 2209 "parser-sming.y"
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
case 213:
#line 2221 "parser-sming.y"
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
case 214:
#line 2233 "parser-sming.y"
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
case 215:
#line 2247 "parser-sming.y"
{
			    char s[SMI_MAX_FULLNAME];

			    typePtr = findType(yyvsp[-1].text, thisParserPtr);
			    if (typePtr && yyvsp[0].listPtr) {
				sprintf(s, "%s%s%s",
					typePtr->modulePtr->name,
					SMI_NAMESPACE_OPERATOR,
					typePtr->name);
				typePtr = duplicateType(typePtr, 0,
							thisParserPtr);
				setTypeParent(typePtr, s);
				setTypeList(typePtr, yyvsp[0].listPtr);
			    }

			    yyval.typePtr = typePtr;
			;
    break;}
case 216:
#line 2267 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 217:
#line 2271 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 218:
#line 2277 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 219:
#line 2281 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 220:
#line 2287 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 221:
#line 2291 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 222:
#line 2298 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 223:
#line 2306 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 224:
#line 2310 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 225:
#line 2316 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 226:
#line 2322 "parser-sming.y"
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
case 227:
#line 2335 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 228:
#line 2341 "parser-sming.y"
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
case 229:
#line 2353 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 230:
#line 2357 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 231:
#line 2363 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 232:
#line 2369 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 233:
#line 2373 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 234:
#line 2380 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 235:
#line 2388 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 236:
#line 2392 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 237:
#line 2398 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 238:
#line 2404 "parser-sming.y"
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
case 239:
#line 2417 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 240:
#line 2423 "parser-sming.y"
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
case 241:
#line 2442 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 242:
#line 2446 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 243:
#line 2452 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 244:
#line 2458 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 245:
#line 2465 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 246:
#line 2473 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 247:
#line 2477 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 248:
#line 2483 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 249:
#line 2490 "parser-sming.y"
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
case 250:
#line 2504 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 251:
#line 2510 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->name = yyvsp[-6].text;
			    yyval.namedNumberPtr->valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 252:
#line 2518 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 253:
#line 2526 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 254:
#line 2530 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 255:
#line 2536 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 256:
#line 2542 "parser-sming.y"
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
case 257:
#line 2555 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 258:
#line 2561 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 259:
#line 2569 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 260:
#line 2573 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 261:
#line 2579 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 262:
#line 2585 "parser-sming.y"
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
case 263:
#line 2598 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 264:
#line 2604 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 265:
#line 2612 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 266:
#line 2616 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 267:
#line 2622 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 268:
#line 2628 "parser-sming.y"
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
case 269:
#line 2641 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 270:
#line 2647 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 271:
#line 2653 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 272:
#line 2657 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 273:
#line 2665 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 274:
#line 2669 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 275:
#line 2675 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 276:
#line 2681 "parser-sming.y"
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
case 277:
#line 2694 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 278:
#line 2700 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 279:
#line 2704 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 280:
#line 2710 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 281:
#line 2714 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 282:
#line 2720 "parser-sming.y"
{
			    char *s;

			    s = util_malloc(strlen(yyvsp[-2].text) +
					    strlen(SMI_NAMESPACE_OPERATOR) +
					    strlen(yyvsp[0].text) + 1);
			    sprintf(s, "%s%s%s",
				    yyvsp[-2].text, SMI_NAMESPACE_OPERATOR, yyvsp[0].text);
			    yyval.text = s;
			    free(yyvsp[-2].text);
			    free(yyvsp[0].text);
			;
    break;}
case 283:
#line 2733 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 284:
#line 2739 "parser-sming.y"
{
			    char *s;

			    s = util_malloc(strlen(yyvsp[-2].text) +
					    strlen(SMI_NAMESPACE_OPERATOR) +
					    strlen(yyvsp[0].text) + 1);
			    sprintf(s, "%s%s%s",
				    yyvsp[-2].text, SMI_NAMESPACE_OPERATOR, yyvsp[0].text);
			    yyval.text = s;
			    free(yyvsp[-2].text);
			    free(yyvsp[0].text);
			;
    break;}
case 285:
#line 2752 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 286:
#line 2758 "parser-sming.y"
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
case 287:
#line 2772 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 288:
#line 2776 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 289:
#line 2782 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 290:
#line 2786 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 291:
#line 2796 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 292:
#line 2803 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 293:
#line 2809 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 294:
#line 2815 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 295:
#line 2828 "parser-sming.y"
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
case 296:
#line 2853 "parser-sming.y"
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
case 297:
#line 2889 "parser-sming.y"
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
case 298:
#line 2909 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 299:
#line 2915 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 300:
#line 2921 "parser-sming.y"
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
case 301:
#line 2933 "parser-sming.y"
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
case 302:
#line 2955 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 303:
#line 2960 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 304:
#line 2967 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 305:
#line 2971 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 306:
#line 2975 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 307:
#line 2981 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 308:
#line 2985 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 309:
#line 2989 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 310:
#line 2993 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 311:
#line 2999 "parser-sming.y"
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
case 312:
#line 3028 "parser-sming.y"
{
			    SmiNode *smiNodePtr;
			    Import *importPtr;

			    /* TODO: findObject(), then createOid */
			    if (strstr(yyvsp[0].text, SMI_NAMESPACE_OPERATOR)) {
				smiNodePtr = smiGetNode(yyvsp[0].text, "");
			    } else {
				smiNodePtr = smiGetNode(yyvsp[0].text,
							thisModulePtr->name);
				if (!smiNodePtr) {
				    importPtr =
					findImportByName(yyvsp[0].text, thisModulePtr);
				    if (importPtr) {
					smiNodePtr =
					    smiGetNode(yyvsp[0].text,
						      importPtr->importmodule);
				    }
				}
			    }
			    if (smiNodePtr) {
				yyval.text = util_strdup(smiNodePtr->oid);
				smiFreeNode(smiNodePtr);
			    } else {
				printError(thisParserPtr,
					   ERR_UNKNOWN_OIDLABEL, yyvsp[0].text);
				yyval.text = NULL;
			    }
			;
    break;}
case 313:
#line 3058 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 314:
#line 3064 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 315:
#line 3068 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 316:
#line 3075 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 317:
#line 3079 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 318:
#line 3089 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 319:
#line 3098 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 320:
#line 3104 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = 42;
			;
    break;}
case 321:
#line 3111 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 322:
#line 3119 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 323:
#line 3127 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 324:
#line 3131 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 325:
#line 3141 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 326:
#line 3148 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 327:
#line 3154 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 328:
#line 3160 "parser-sming.y"
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
#line 3165 "parser-sming.y"


#endif
			
