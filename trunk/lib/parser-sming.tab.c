
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



#define	YYFINAL		740
#define	YYFLAG		-32768
#define	YYNTBASE	76

#define YYTRANSLATE(x) ((unsigned)(x) <= 321 ? yytranslate[x] : 301)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    70,
    71,     2,     2,    73,    74,    75,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    69,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    67,    72,    68,     2,     2,     2,     2,     2,
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
    66
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
  1062,  1064,  1066,  1068,  1070,  1073,  1075,  1077,  1078,  1080,
  1082,  1085,  1088,  1090,  1092,  1094,  1097,  1099,  1101,  1102,
  1103,  1104
};

static const short yyrhs[] = {   299,
    77,     0,     0,    78,     0,    79,     0,    78,    79,     0,
    80,   299,     0,     0,     0,     0,     0,     0,     0,     0,
    11,   298,     5,    81,   298,     6,    82,   299,    67,   300,
   180,   206,   300,    83,   208,   300,    84,   209,   300,    85,
   221,   300,    86,   222,    87,   186,    88,   100,   148,   158,
   168,    68,   299,    69,     0,     0,    89,     0,    90,     0,
    89,    90,     0,    91,   300,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    20,   298,     5,    92,   299,    67,
   300,   190,   300,    93,   218,    94,   210,    95,   212,    96,
   214,    97,   220,    98,   222,    99,    68,   299,    69,     0,
     0,   101,     0,   102,     0,   101,   102,     0,   103,   300,
     0,   104,     0,   110,     0,   121,     0,     0,     0,     0,
     0,     0,    23,   298,     6,   105,   299,    67,   300,   206,
   300,   106,   214,   107,   220,   108,   222,   109,    68,   299,
    69,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    24,   298,     6,   111,   299,    67,   300,   206,
   300,   112,   192,   300,   113,   217,   300,   114,   218,   115,
   210,   116,   212,   117,   214,   118,   221,   300,   119,   222,
   120,    68,   299,    69,     0,     0,     0,     0,     0,     0,
    25,   298,     6,   122,   299,    67,   300,   206,   300,   123,
   214,   124,   221,   300,   125,   222,   126,   127,   300,    68,
   299,    69,     0,     0,     0,     0,     0,     0,     0,     0,
    27,   298,     6,   128,   299,    67,   300,   206,   300,   129,
   195,   300,   130,   202,   131,   214,   132,   221,   300,   133,
   222,   134,   135,    68,   299,    69,     0,   136,     0,   135,
   136,     0,   137,   300,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    26,   298,     6,   138,   299,
    67,   300,   206,   300,   139,   192,   300,   140,   217,   300,
   141,   218,   142,   210,   143,   212,   144,   214,   145,   221,
   300,   146,   222,   147,    68,   299,    69,     0,     0,   149,
     0,   150,     0,   149,   150,     0,   151,   300,     0,     0,
     0,     0,     0,     0,     0,    28,   298,     6,   152,   299,
    67,   300,   206,   300,   153,   225,   154,   214,   155,   221,
   300,   156,   222,   157,    68,   299,    69,     0,     0,   159,
     0,   160,     0,   159,   160,     0,   161,   300,     0,     0,
     0,     0,     0,     0,     0,    29,   298,     6,   162,   299,
    67,   300,   206,   300,   163,   224,   300,   164,   214,   165,
   221,   300,   166,   222,   167,    68,   299,    69,     0,     0,
   169,     0,   170,     0,   169,   170,     0,   171,   300,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    30,   298,
     6,   172,   299,    67,   300,   206,   300,   173,   214,   174,
   221,   300,   175,   222,   176,   227,   177,   229,   178,   233,
   179,    68,   299,    69,     0,     0,   181,     0,   182,     0,
   181,   182,     0,   183,   300,     0,     0,     0,    12,   298,
     5,   184,   299,    70,   299,   264,   185,   299,    71,   299,
    69,     0,     0,   187,     0,   188,     0,   187,   188,     0,
   189,   300,     0,    13,   299,    67,   300,   207,   300,   221,
   300,    68,   299,    69,     0,    21,   298,   237,   299,    69,
     0,     0,   192,   300,     0,    21,   298,   237,   299,    69,
     0,     0,   194,   300,     0,    22,   298,   237,   299,    69,
     0,   196,     0,   197,     0,   198,     0,   199,     0,   200,
     0,    37,   201,   299,    70,   299,   268,   299,    71,   299,
    69,     0,    38,   298,   279,   299,    69,     0,    39,   298,
   279,   201,   299,    70,   299,   268,   299,    71,   299,    69,
     0,    40,   298,   279,   299,    69,     0,    41,   298,   279,
   201,   299,    70,   299,   268,   299,    71,   299,    69,     0,
     0,   298,    36,     0,     0,   203,   300,     0,    42,   204,
   299,    69,     0,     0,   299,   205,     0,    70,   299,   268,
   299,    71,     0,    14,   298,   288,   299,    69,     0,    15,
   298,   284,   299,    69,     0,    16,   298,   280,   299,    69,
     0,    17,   298,   280,   299,    69,     0,     0,   211,   300,
     0,    31,   298,   280,   299,    69,     0,     0,   213,   300,
     0,    32,   298,   280,   299,    69,     0,     0,   215,   300,
     0,    33,   298,   286,   299,    69,     0,     0,   217,   300,
     0,    34,   298,   287,   299,    69,     0,     0,   219,   300,
     0,    35,   298,   285,   299,    69,     0,     0,   221,   300,
     0,    18,   298,   280,   299,    69,     0,     0,   223,   300,
     0,    19,   298,   280,   299,    69,     0,     0,   224,   300,
     0,    43,   299,    70,   299,   268,   299,    71,   299,    69,
     0,     0,   226,   300,     0,    44,   299,    70,   299,   268,
   299,    71,   299,    69,     0,     0,   228,   300,     0,    45,
   299,    70,   299,   268,   299,    71,   299,    69,     0,     0,
   230,     0,   231,     0,   230,   231,     0,   232,   300,     0,
    46,   298,   279,   299,    67,   221,   300,    68,   299,    69,
     0,     0,   234,     0,   235,     0,   234,   235,     0,   236,
   300,     0,    47,   298,   279,   299,    67,   191,   193,   216,
   221,   300,    68,   299,    69,     0,   238,     0,   239,     0,
    48,   242,     0,    49,     0,    50,   242,     0,    51,   242,
     0,    52,   242,     0,    53,   242,     0,    54,   250,     0,
    55,   250,     0,    56,   250,     0,    58,   258,     0,    57,
   258,     0,   278,   240,     0,     0,   299,   241,     0,   243,
     0,   251,     0,     0,   299,   243,     0,    70,   299,   247,
   244,   299,    71,     0,     0,   245,     0,   246,     0,   245,
   246,     0,   299,    72,   299,   247,     0,   296,   248,     0,
     0,   249,     0,   299,     3,   299,   296,     0,     0,   299,
   251,     0,    70,   299,   255,   252,   299,    71,     0,     0,
   253,     0,   254,     0,   253,   254,     0,   299,    72,   299,
   255,     0,     7,   256,     0,     0,   257,     0,   299,     3,
   299,     7,     0,    70,   299,   259,   299,    71,     0,   263,
   260,   297,     0,     0,   261,     0,   262,     0,   261,   262,
     0,   299,    73,   299,   263,     0,     6,   299,    70,   299,
   294,   299,    71,     0,   277,   265,   297,     0,     0,   266,
     0,   267,     0,   266,   267,     0,   299,    73,   299,   277,
     0,     0,     0,   297,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   299,    73,   299,     0,     0,   279,   269,
   297,     0,     0,   270,     0,   271,     0,   270,   271,     0,
   299,    73,   299,   279,     0,    70,   299,   273,   299,    71,
     0,   297,     0,     6,   274,   297,     0,     0,   275,     0,
   276,     0,   275,   276,     0,   299,    73,   299,     6,     0,
     5,     0,     6,     0,   278,     0,   279,     0,     5,     4,
     5,     0,     5,     0,     5,     4,     6,     0,     6,     0,
     8,   281,     0,     0,   282,     0,   283,     0,   282,   283,
     0,   299,     8,     0,     8,     0,     8,     0,     8,     0,
   272,     0,     9,     0,    74,     9,     0,    10,     0,     7,
     0,   280,     0,   279,     0,   279,   291,     0,   293,    75,
   293,   291,     0,    59,     0,    60,     0,    61,     0,    62,
     0,    63,     0,    64,     0,    65,     0,    66,     0,   289,
   290,     0,   279,     0,   293,     0,     0,   291,     0,   292,
     0,   291,   292,     0,    75,   293,     0,     9,     0,    10,
     0,     9,     0,    74,     9,     0,   294,     0,   295,     0,
     0,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   425,   435,   439,   445,   449,   464,   478,   498,   502,   531,
   538,   545,   552,   558,   612,   616,   626,   630,   646,   660,
   664,   685,   691,   697,   703,   709,   715,   720,   728,   732,
   742,   746,   762,   776,   777,   778,   781,   785,   801,   807,
   820,   825,   833,   837,   851,   864,   871,   877,   883,   889,
   894,   901,   906,   915,   919,   936,   941,   948,   953,   962,
   966,   982,  1002,  1012,  1017,  1024,  1030,  1038,  1046,  1061,
  1075,  1079,  1093,  1106,  1113,  1119,  1125,  1131,  1136,  1143,
  1148,  1157,  1161,  1171,  1175,  1191,  1205,  1209,  1225,  1240,
  1245,  1253,  1258,  1266,  1270,  1280,  1284,  1299,  1313,  1317,
  1330,  1346,  1351,  1358,  1363,  1371,  1375,  1385,  1389,  1405,
  1419,  1423,  1439,  1444,  1451,  1457,  1472,  1476,  1504,  1512,
  1516,  1522,  1526,  1541,  1555,  1559,  1572,  1585,  1589,  1595,
  1599,  1614,  1628,  1637,  1645,  1649,  1655,  1662,  1666,  1672,
  1679,  1680,  1681,  1682,  1683,  1686,  1708,  1725,  1748,  1765,
  1788,  1792,  1798,  1802,  1808,  1817,  1821,  1827,  1833,  1839,
  1845,  1851,  1857,  1861,  1867,  1873,  1877,  1883,  1889,  1893,
  1899,  1905,  1909,  1915,  1921,  1925,  1931,  1937,  1941,  1947,
  1953,  1957,  1963,  1969,  1973,  1979,  1986,  1990,  1996,  2003,
  2007,  2013,  2020,  2024,  2030,  2036,  2050,  2056,  2067,  2071,
  2077,  2083,  2096,  2102,  2130,  2134,  2140,  2152,  2156,  2168,
  2180,  2192,  2204,  2216,  2228,  2240,  2252,  2266,  2285,  2289,
  2295,  2299,  2305,  2309,  2315,  2324,  2328,  2334,  2340,  2353,
  2359,  2371,  2375,  2381,  2387,  2391,  2397,  2406,  2410,  2416,
  2422,  2435,  2441,  2460,  2464,  2470,  2476,  2482,  2491,  2495,
  2501,  2507,  2522,  2528,  2536,  2544,  2548,  2554,  2560,  2573,
  2579,  2587,  2591,  2597,  2603,  2616,  2622,  2630,  2634,  2640,
  2646,  2659,  2665,  2671,  2675,  2683,  2687,  2693,  2699,  2712,
  2718,  2722,  2728,  2732,  2738,  2749,  2755,  2766,  2772,  2786,
  2790,  2796,  2800,  2810,  2816,  2823,  2829,  2842,  2867,  2906,
  2926,  2932,  2938,  2950,  2972,  2977,  2984,  2988,  2992,  2998,
  3002,  3006,  3010,  3014,  3020,  3049,  3076,  3082,  3086,  3093,
  3097,  3107,  3116,  3122,  3129,  3137,  3145,  3149,  3159,  3166,
  3172,  3178
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
"readonlyKeyword","readwriteKeyword","readcreateKeyword","'{'","'}'","';'","'('",
"')'","'|'","','","'-'","'.'","smingFile","moduleStatement_optsep_0n","moduleStatement_optsep_1n",
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
    76,    77,    77,    78,    78,    79,    81,    82,    83,    84,
    85,    86,    87,    80,    88,    88,    89,    89,    90,    92,
    93,    94,    95,    96,    97,    98,    99,    91,   100,   100,
   101,   101,   102,   103,   103,   103,   105,   106,   107,   108,
   109,   104,   111,   112,   113,   114,   115,   116,   117,   118,
   119,   120,   110,   122,   123,   124,   125,   126,   121,   128,
   129,   130,   131,   132,   133,   134,   127,   135,   135,   136,
   138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
   137,   148,   148,   149,   149,   150,   152,   153,   154,   155,
   156,   157,   151,   158,   158,   159,   159,   160,   162,   163,
   164,   165,   166,   167,   161,   168,   168,   169,   169,   170,
   172,   173,   174,   175,   176,   177,   178,   179,   171,   180,
   180,   181,   181,   182,   184,   185,   183,   186,   186,   187,
   187,   188,   189,   190,   191,   191,   192,   193,   193,   194,
   195,   195,   195,   195,   195,   196,   197,   198,   199,   200,
   201,   201,   202,   202,   203,   204,   204,   205,   206,   207,
   208,   209,   210,   210,   211,   212,   212,   213,   214,   214,
   215,   216,   216,   217,   218,   218,   219,   220,   220,   221,
   222,   222,   223,    -1,    -1,   224,   225,   225,   226,   227,
   227,   228,   229,   229,   230,   230,   231,   232,   233,   233,
   234,   234,   235,   236,   237,   237,   238,   238,   238,   238,
   238,   238,   238,   238,   238,   238,   238,   239,   240,   240,
   241,   241,   242,   242,   243,   244,   244,   245,   245,   246,
   247,   248,   248,   249,   250,   250,   251,   252,   252,   253,
   253,   254,   255,   256,   256,   257,   258,   259,   260,   260,
   261,   261,   262,   263,   264,   265,   265,   266,   266,   267,
    -1,    -1,    -1,    -1,    -1,    -1,   268,   269,   269,   270,
   270,   271,   272,   273,   273,   274,   274,   275,   275,   276,
   277,   277,    -1,    -1,   278,   278,   279,   279,   280,   281,
   281,   282,   282,   283,   284,    -1,    -1,   285,   285,   285,
   285,   285,   285,   285,   285,   285,   286,   286,   286,   287,
   287,   287,   287,   287,   288,   289,   289,   290,   290,   291,
   291,   292,   293,   294,   294,   295,   296,   296,   297,   298,
   299,   300
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
     1,     1,     1,     1,     2,     1,     1,     0,     1,     1,
     2,     2,     1,     1,     1,     2,     1,     1,     0,     0,
     0,     0
};

static const short yydefact[] = {   331,
     2,   330,     1,     3,     4,   331,     0,     5,     6,     7,
   330,     0,     8,   331,     0,   332,   120,   330,     0,   121,
   122,   332,     0,   330,   332,   123,   124,   125,     0,     9,
   331,     0,   288,   323,   316,   331,   318,   317,     0,     0,
     0,     0,     0,   315,   319,   320,   330,   332,   331,   287,
   159,   322,   321,     0,    10,     0,   290,   331,     0,   281,
   282,   126,   256,   289,   291,   292,     0,     0,   330,   332,
   331,   329,   257,   258,     0,   293,   294,   161,     0,    11,
     0,   255,   259,   331,   331,     0,   331,     0,     0,   330,
   332,     0,   260,   162,     0,    12,   127,   331,   181,     0,
   330,    13,   332,   180,     0,   128,   182,   331,   331,    15,
   129,   130,   332,     0,     0,   330,    29,    16,    17,   332,
   131,   132,   183,   332,     0,   330,   330,   330,    82,    30,
    31,   332,    34,    35,    36,    18,    19,     0,    20,     0,
     0,     0,   330,    94,    83,    84,   332,    32,    33,   330,
   332,   331,    37,    43,    54,     0,   330,   106,    95,    96,
   332,    85,    86,     0,     0,     0,   331,   331,   331,    87,
     0,   330,     0,   107,   108,   332,    97,    98,   295,   331,
   332,   332,     0,     0,     0,   331,    99,     0,   331,   109,
   110,     0,     0,     0,   332,   332,   332,     0,   331,   111,
     0,   160,   331,   330,   332,     0,     0,     0,   332,     0,
   331,    14,     0,     0,    21,   332,   332,   332,     0,   332,
     0,   133,   286,   223,   208,   223,   223,   223,   223,   235,
   235,   235,     0,     0,   331,   205,   206,   219,   175,    38,
    44,    55,   332,     0,   332,     0,   207,     0,   209,   210,
   211,   212,   213,     0,   214,   215,   331,   217,   216,     0,
   218,     0,   330,    22,   332,   169,     0,   169,    88,   332,
     0,   285,   331,   224,   331,   236,     0,   134,   331,   220,
   221,   222,     0,   163,   176,   330,    39,   332,   330,   332,
    56,   187,   100,   332,     0,     0,   331,   331,   249,     0,
   302,   299,   301,   331,     0,   298,   304,   303,   331,     0,
   330,    23,   332,     0,   178,   170,     0,    45,     0,   331,
    89,   332,     0,   112,   325,   324,     0,   226,   327,   328,
   232,   244,   238,     0,     0,   329,   250,   251,     0,   329,
   300,   305,     0,     0,     0,   166,   164,   307,   308,   309,
   331,    40,   332,   331,     0,   332,     0,   169,   188,   331,
   332,   169,   326,   331,   227,   228,     0,   231,   233,     0,
   243,   245,     0,   331,   239,   240,     0,   331,   247,   248,
   252,   331,   276,   331,   274,   177,     0,   331,   330,    24,
   332,     0,   181,   179,     0,   330,   332,    57,   331,    90,
     0,   101,   113,     0,   229,   331,   331,   331,     0,   241,
   331,     0,     0,   329,   277,   278,     0,     0,   306,     0,
     0,   169,   167,   171,    41,   137,     0,    46,   181,     0,
     0,   331,   169,     0,   225,     0,     0,     0,   237,     0,
   331,   253,   275,   279,   331,   273,   165,   331,    25,     0,
   310,   311,   312,   313,   314,   331,   175,    58,   331,   268,
   332,     0,   102,   332,   230,   234,   246,   242,     0,     0,
     0,   178,   331,     0,    47,     0,     0,   329,   269,   270,
     0,    91,   331,     0,   114,   254,   280,   168,    26,     0,
   174,   163,   330,   332,   331,   267,   271,   331,   181,     0,
   332,   181,   181,    42,    48,     0,     0,     0,     0,    92,
   331,   103,   115,    27,   166,    60,   331,   189,   272,     0,
     0,   181,   190,     0,    49,   331,     0,   331,   186,   104,
   331,   116,   332,   331,   169,     0,    59,     0,     0,     0,
   193,   191,     0,    50,   332,    93,   331,   331,   330,   117,
   194,   195,   332,    28,     0,     0,     0,     0,     0,   199,
   196,   197,   332,   332,   105,   331,   331,   330,   118,   200,
   201,   332,    51,    61,     0,     0,     0,     0,   202,   203,
   181,     0,   331,     0,   331,   331,    52,   151,   330,   330,
   330,   330,   332,   141,   142,   143,   144,   145,     0,   332,
     0,     0,     0,   331,     0,     0,     0,     0,     0,    62,
   192,     0,   135,   119,   331,     0,   152,   331,   151,   331,
   151,   153,   331,   138,   332,     0,   331,     0,   331,     0,
   331,   156,    63,   332,     0,   330,   172,   332,   136,    53,
     0,   147,     0,   149,     0,   331,     0,   169,   154,   198,
     0,     0,   332,   139,   331,   331,   331,     0,   331,   157,
    64,   331,   332,   173,     0,     0,     0,   155,     0,     0,
     0,     0,   331,   331,   331,   331,   332,   140,   331,     0,
     0,     0,     0,    65,     0,   146,   331,   331,   158,   181,
   204,     0,     0,    66,   148,   150,     0,   330,     0,    68,
   332,     0,   331,    69,    70,    71,     0,   331,    67,     0,
   332,     0,   332,    72,     0,   332,    73,     0,   332,    74,
   175,    75,   163,    76,   166,    77,   169,    78,     0,   332,
    79,   181,    80,     0,   331,     0,    81,     0,     0,     0
};

static const short yydefgoto[] = {   738,
     3,     4,     5,     6,    11,    14,    39,    59,    86,    99,
   106,   117,   118,   119,   120,   152,   239,   284,   346,   422,
   472,   503,   524,   129,   130,   131,   132,   133,   167,   266,
   315,   393,   450,   134,   168,   267,   355,   457,   492,   515,
   535,   555,   581,   603,   135,   169,   268,   319,   429,   476,
   494,   526,   582,   622,   648,   670,   690,   697,   699,   700,
   701,   708,   715,   718,   721,   723,   725,   727,   729,   732,
   734,   144,   145,   146,   147,   186,   292,   358,   431,   499,
   520,   158,   159,   160,   161,   199,   323,   433,   484,   522,
   539,   173,   174,   175,   176,   211,   362,   434,   502,   523,
   541,   560,   578,    19,    20,    21,    22,    31,    71,   110,
   111,   112,   113,   205,   624,   290,   637,   638,   593,   594,
   595,   596,   597,   598,   604,   633,   634,   646,   660,    25,
   151,    48,    70,   312,   313,   390,   391,   287,   288,   652,
   397,   264,   265,   352,   353,   102,   103,   361,   321,   322,
   532,   533,   550,   551,   552,   553,   569,   570,   571,   572,
   235,   236,   237,   261,   280,   247,   274,   364,   365,   366,
   328,   368,   369,   253,   276,   374,   375,   376,   333,   371,
   372,   258,   298,   336,   337,   338,   299,    62,    72,    73,
    74,   459,   478,   479,   480,   306,   384,   414,   415,   416,
    63,   238,   460,    58,    64,    65,    66,   180,   309,   351,
   456,    36,    37,    44,    45,    46,    38,   329,   330,   331,
    82,   605,   248,    17
};

static const short yypact[] = {-32768,
    33,-32768,-32768,    33,-32768,-32768,    32,-32768,-32768,-32768,
-32768,    34,-32768,-32768,    -3,-32768,    49,-32768,    56,    49,
-32768,-32768,    67,-32768,-32768,-32768,-32768,-32768,    37,-32768,
-32768,    70,-32768,-32768,-32768,-32768,    11,-32768,    78,    25,
    93,    47,    97,-32768,    11,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   109,-32768,    42,   111,-32768,   103,-32768,
-32768,-32768,    48,-32768,   111,-32768,   119,    59,-32768,-32768,
-32768,-32768,    48,-32768,    57,-32768,-32768,-32768,   109,-32768,
    58,-32768,-32768,-32768,-32768,   113,-32768,    42,    66,-32768,
-32768,    68,-32768,-32768,   109,-32768,-32768,-32768,   117,    69,
-32768,-32768,-32768,-32768,   109,   127,-32768,-32768,-32768,   124,
   127,-32768,-32768,    77,    80,-32768,    27,   124,-32768,-32768,
-32768,-32768,-32768,-32768,   143,-32768,-32768,-32768,   122,    27,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   136,-32768,   147,
   148,   149,-32768,   128,   122,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   152,-32768,   129,   128,-32768,
-32768,-32768,-32768,   153,   113,    99,-32768,-32768,-32768,-32768,
   154,-32768,   102,   129,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   104,   105,   106,-32768,-32768,   168,-32768,-32768,
-32768,   107,   114,   156,-32768,-32768,-32768,   112,-32768,-32768,
   115,-32768,-32768,-32768,-32768,    56,    56,    56,-32768,   116,
-32768,-32768,   118,   210,-32768,-32768,-32768,-32768,    56,-32768,
   121,-32768,   177,   120,-32768,   120,   120,   120,   120,   120,
   120,   120,   123,   123,-32768,-32768,-32768,   120,   150,-32768,
-32768,-32768,-32768,    56,-32768,   186,-32768,   125,-32768,-32768,
-32768,-32768,-32768,   126,-32768,-32768,-32768,-32768,-32768,   131,
-32768,   132,-32768,-32768,-32768,   159,   173,   159,-32768,-32768,
    56,-32768,-32768,-32768,-32768,-32768,   191,-32768,-32768,-32768,
-32768,-32768,    71,   167,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   160,-32768,-32768,    29,   198,-32768,-32768,    48,    19,
-32768,   133,-32768,-32768,   197,-32768,    11,-32768,-32768,   134,
-32768,-32768,-32768,    -5,   113,-32768,   210,-32768,   113,-32768,
-32768,-32768,   164,-32768,-32768,-32768,   203,   141,-32768,-32768,
   211,   211,   141,   146,   151,-32768,    48,-32768,   144,   212,
-32768,    11,   155,    97,   109,   187,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   194,-32768,   163,   159,-32768,-32768,
-32768,   159,-32768,-32768,   141,-32768,   162,-32768,-32768,   217,
-32768,-32768,   233,-32768,   141,-32768,   165,-32768,-32768,-32768,
-32768,-32768,    48,-32768,-32768,-32768,    11,-32768,-32768,-32768,
-32768,   170,   117,-32768,   172,-32768,-32768,-32768,-32768,-32768,
   174,-32768,-32768,   171,-32768,-32768,-32768,-32768,   175,-32768,
-32768,    50,   191,-32768,    48,-32768,   176,   179,    11,   178,
   109,   159,-32768,-32768,-32768,-32768,    26,-32768,   117,    61,
   113,-32768,   159,   113,-32768,    29,    29,   236,-32768,   198,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   180,
-32768,-32768,-32768,-32768,-32768,-32768,   150,-32768,-32768,    48,
-32768,    61,-32768,-32768,-32768,-32768,-32768,-32768,   182,   239,
   185,   113,-32768,   201,-32768,   228,   205,-32768,    48,-32768,
   199,-32768,-32768,   113,-32768,-32768,-32768,-32768,-32768,   209,
-32768,   167,-32768,-32768,-32768,-32768,-32768,-32768,   117,   213,
-32768,   117,   117,-32768,-32768,   268,   214,   218,    61,-32768,
-32768,-32768,-32768,-32768,   187,-32768,-32768,-32768,-32768,   215,
   219,   117,   235,   221,-32768,-32768,   222,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   159,   223,-32768,   224,   226,   216,
   250,-32768,   231,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   250,-32768,-32768,-32768,   113,    56,   232,    61,    61,   234,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   234,
-32768,-32768,-32768,-32768,   240,   238,    61,   242,-32768,-32768,
   117,    73,-32768,   113,-32768,-32768,-32768,   266,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   237,-32768,
   245,   246,   248,-32768,   267,    61,    61,    61,    61,-32768,
-32768,   251,   173,-32768,-32768,   244,-32768,-32768,   266,-32768,
   266,   265,-32768,   296,-32768,   252,-32768,   253,-32768,   254,
-32768,   120,-32768,-32768,   255,-32768,   194,-32768,-32768,-32768,
    61,-32768,   256,-32768,   257,-32768,   259,   159,-32768,-32768,
   210,   113,-32768,-32768,-32768,-32768,-32768,   269,-32768,-32768,
-32768,-32768,-32768,-32768,   263,    61,    61,-32768,    61,   113,
   270,   272,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   273,
   274,   275,   276,-32768,   279,-32768,-32768,-32768,-32768,   117,
-32768,   280,   281,-32768,-32768,-32768,   299,-32768,    -6,-32768,
-32768,   330,-32768,-32768,-32768,-32768,   283,-32768,-32768,   277,
-32768,    56,-32768,-32768,   173,-32768,-32768,   194,-32768,-32768,
   150,-32768,   167,-32768,   187,-32768,   159,-32768,   113,-32768,
-32768,   117,-32768,   287,-32768,   288,-32768,   341,   343,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   352,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   241,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   243,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -341,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   225,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   208,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,   188,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   348,-32768,-32768,-32768,-32768,
-32768,   258,-32768,-32768,-32768,  -600,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -587,-32768,-32768,-32768,-32768,  -203,
-32768,-32768,-32768,  -483,-32768,  -500,-32768,  -258,-32768,-32768,
  -616,  -450,-32768,  -101,   -68,  -360,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -175,-32768,-32768,-32768,  -193,-32768,
  -316,-32768,-32768,-32768,-32768,  -103,   130,-32768,-32768,    14,
   -56,-32768,-32768,  -149,   135,-32768,-32768,    12,   -59,-32768,
-32768,   157,-32768,-32768,-32768,    52,   -20,-32768,-32768,-32768,
   313,  -440,-32768,-32768,   -85,-32768,-32768,-32768,-32768,   -19,
   310,-32768,   -27,   -60,-32768,-32768,   335,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,  -282,   -34,   -31,   -11,-32768,   -33,
  -313,     6,     0,   569
};


#define	YYLAST		1299


static const short yytable[] = {     1,
   354,    35,   216,   217,   218,     9,   475,     7,   505,   291,
    53,    52,   625,    15,   525,   243,    12,    91,    85,   698,
   653,   483,   380,    23,   342,   332,   385,   325,   326,    29,
    40,   629,   425,   631,    98,    42,    10,   325,   326,    13,
   270,    32,    33,     2,   108,    34,    60,    61,    56,   126,
   127,   128,    54,   348,   349,   350,    67,    68,   325,   326,
    18,   703,    75,    16,    67,    32,    33,   294,   458,    24,
    81,    28,    75,    41,    79,    32,    33,   301,    57,   302,
   303,   255,   256,    88,    89,    43,    92,   451,   452,   453,
   454,   455,   327,    47,    49,    95,   181,   100,    50,   400,
   443,   719,   327,   403,   419,    34,   105,   114,   115,   588,
   589,   590,   591,   592,   716,    51,    57,   566,  -331,    69,
  -331,   125,   249,   250,   251,   252,    77,    78,    87,    84,
    90,   140,   141,   142,    94,   101,    97,   104,   510,   109,
   304,   513,   514,   116,   305,   123,   124,   139,   156,   143,
   150,   166,   153,   154,   155,   164,   157,   170,   172,   187,
   179,   530,   171,   449,   496,   182,   183,   184,   185,   189,
   195,   196,   197,   200,   463,   202,   204,   188,   209,   192,
   246,   203,   220,   212,   263,   198,   222,   245,   201,  -331,
   272,   286,   257,   289,   273,   275,   297,   311,   210,   278,
   655,   279,   213,   320,   332,   341,   360,  -323,   344,   214,
   221,   363,  -331,  -331,   223,   378,   382,   383,   389,   407,
   587,   379,   308,   386,   726,   674,   675,   396,   676,   254,
   254,   254,   399,   406,   260,   408,   411,   262,   424,   724,
   426,   435,   467,   432,   487,   439,   447,   473,   445,   446,
   356,   310,   486,   488,   493,   307,   277,   224,   225,   226,
   227,   228,   229,   230,   231,   232,   233,   234,   283,   491,
   722,   498,   295,   516,   296,   495,   544,   504,   300,   531,
   568,   517,   528,   511,   388,   548,   518,   529,   534,   545,
   537,   314,   546,   547,   317,   549,   334,   335,   339,   554,
   565,  -330,   617,   340,   584,   611,   632,    53,   343,   586,
   583,   613,   387,   627,   614,   615,   345,   636,   623,   357,
   640,   642,   644,   650,   698,   656,   657,   367,   659,   694,
   370,   373,   377,   673,   662,   706,   339,   668,   678,   679,
   739,   686,   740,   711,   687,   688,   689,   691,   695,   696,
   392,   709,   564,   395,   735,     8,   737,   704,   136,   401,
   448,   190,   461,   404,   367,   464,   177,    26,   121,   162,
   489,   733,   148,   409,   377,   561,   579,   412,   405,   465,
   468,   413,   417,   418,    53,    83,   410,   420,   381,   661,
   259,   281,   442,   497,   421,   444,   282,    93,   430,    76,
   441,   427,     0,   466,     0,   436,   437,   438,     0,     0,
   440,     0,     0,     0,   417,   501,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   462,     0,     0,     0,     0,     0,     0,     0,     0,
   469,     0,     0,     0,   470,     0,     0,   471,     0,     0,
     0,     0,     0,     0,     0,   474,     0,     0,   477,   481,
     0,     0,     0,     0,     0,     0,     0,     0,   728,     0,
     0,     0,   490,     0,     0,     0,     0,     0,   481,     0,
     0,   519,   500,     0,     0,     0,   563,     0,     0,     0,
     0,     0,     0,     0,   508,     0,     0,   509,   506,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   713,     0,
   521,     0,     0,     0,     0,   600,   527,     0,     0,     0,
     0,     0,     0,     0,     0,   536,     0,   538,     0,     0,
   540,   567,     0,   543,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   557,   558,     0,   585,
     0,     0,     0,     0,   559,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   575,   576,     0,     0,     0,
     0,     0,     0,   577,     0,     0,     0,     0,   618,   619,
   620,   621,   599,   663,   601,   602,     0,     0,     0,     0,
    27,     0,     0,    30,   606,   607,   608,   609,     0,     0,
     0,   677,     0,   616,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   626,     0,    55,   628,     0,   630,
     0,     0,   635,     0,     0,     0,   641,     0,   643,     0,
   645,   647,     0,     0,     0,     0,     0,     0,    80,     0,
     0,   651,     0,     0,     0,   658,     0,     0,     0,     0,
     0,     0,     0,     0,   665,   666,   667,     0,   669,    96,
   730,   671,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   107,   680,   681,   682,   683,     0,     0,   685,     0,
     0,   122,     0,     0,     0,     0,   692,   693,   137,     0,
     0,     0,   138,     0,     0,     0,     0,     0,     0,     0,
   149,     0,   707,   702,     0,     0,     0,   710,     0,     0,
     0,     0,     0,     0,     0,   163,     0,     0,     0,   165,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   178,
     0,     0,     0,     0,   736,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   191,     0,     0,     0,     0,   193,
   194,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   206,   207,   208,     0,     0,     0,     0,
     0,     0,     0,   215,     0,     0,     0,   219,     0,     0,
     0,     0,     0,     0,   240,   241,   242,     0,   244,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   269,     0,   271,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   285,     0,     0,     0,     0,   293,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   316,     0,   318,     0,
     0,     0,   324,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   347,     0,     0,     0,     0,     0,     0,     0,     0,
   359,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   394,     0,     0,   398,     0,     0,     0,     0,   402,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   423,
     0,     0,     0,     0,     0,   428,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   482,
     0,     0,   485,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   507,     0,     0,     0,     0,     0,     0,   512,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   542,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   556,     0,     0,     0,     0,     0,     0,
     0,   562,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   573,   574,     0,     0,     0,     0,     0,     0,     0,
   580,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   610,     0,     0,     0,     0,     0,     0,   612,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   639,     0,     0,     0,     0,     0,     0,
     0,     0,   649,     0,     0,     0,   654,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   664,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   672,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   684,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   705,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   712,
     0,   714,     0,     0,   717,     0,     0,   720,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   731
};

static const short yycheck[] = {     0,
   317,    29,   206,   207,   208,     6,   457,     2,   492,   268,
    45,    43,   613,    14,   515,   219,    11,    86,    79,    26,
   637,   462,   336,    18,   307,     7,   340,     9,    10,    24,
    31,   619,   393,   621,    95,    36,     5,     9,    10,     6,
   244,     5,     6,    11,   105,     9,     5,     6,    49,    23,
    24,    25,    47,    59,    60,    61,    57,    58,     9,    10,
    12,    68,    63,    67,    65,     5,     6,   271,   429,    14,
    71,     5,    73,     4,    69,     5,     6,     7,     8,     9,
    10,   231,   232,    84,    85,    75,    87,    62,    63,    64,
    65,    66,    74,    16,    70,    90,   165,    98,     6,   358,
   414,   718,    74,   362,   387,     9,   101,   108,   109,    37,
    38,    39,    40,    41,   715,    69,     8,   558,     8,    17,
    73,   116,   226,   227,   228,   229,     8,    69,    71,    73,
    18,   126,   127,   128,    69,    19,    69,    69,   499,    13,
    70,   502,   503,    20,    74,    69,    67,     5,   143,    28,
    15,   152,     6,     6,     6,   150,    29,     6,    30,     6,
     8,   522,   157,   422,   478,    67,   167,   168,   169,    68,
    67,    67,    67,     6,   433,    69,    21,   172,    67,   180,
     4,    68,    67,    69,    35,   186,    69,    67,   189,    70,
     5,    33,    70,    21,    70,    70,     6,    31,   199,    69,
   641,    70,   203,    44,     7,     9,    43,    75,    75,   204,
   211,     9,    72,     3,     5,    70,    73,     6,    32,     3,
   581,    71,   283,    69,   725,   666,   667,    34,   669,   230,
   231,   232,    70,    72,   235,     3,    72,   238,    69,   723,
    69,    71,     7,    70,     6,    71,    69,    68,    73,    71,
   319,   283,    71,    69,    27,   283,   257,    48,    49,    50,
    51,    52,    53,    54,    55,    56,    57,    58,   263,    69,
   721,    73,   273,     6,   275,    71,   535,    69,   279,    45,
    47,    68,    68,    71,   345,    70,    69,    69,    68,    67,
    69,   286,    69,    68,   289,    46,   297,   298,   299,    69,
    69,    36,    36,   304,    67,    69,    42,   342,   309,    68,
    71,    67,   344,    70,    69,    68,   311,    22,    68,   320,
    69,    69,    69,    69,    26,    70,    70,   328,    70,   690,
   331,   332,   333,    71,   651,     6,   337,    69,    69,    68,
     0,    69,     0,    67,    71,    71,    71,    69,    69,    69,
   351,    69,   556,   354,    68,     4,    69,   699,   118,   360,
   421,   174,   431,   364,   365,   434,   159,    20,   111,   145,
   472,   732,   130,   374,   375,   551,   570,   378,   365,   436,
   440,   382,   383,   384,   419,    73,   375,   388,   337,   648,
   234,   262,   413,   479,   389,   415,   262,    88,   399,    65,
   412,   396,    -1,   437,    -1,   406,   407,   408,    -1,    -1,
   411,    -1,    -1,    -1,   415,   484,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   432,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   441,    -1,    -1,    -1,   445,    -1,    -1,   448,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   456,    -1,    -1,   459,   460,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   727,    -1,
    -1,    -1,   473,    -1,    -1,    -1,    -1,    -1,   479,    -1,
    -1,   509,   483,    -1,    -1,    -1,   555,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   495,    -1,    -1,   498,   493,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   712,    -1,
   511,    -1,    -1,    -1,    -1,   584,   517,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   526,    -1,   528,    -1,    -1,
   531,   559,    -1,   534,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   547,   548,    -1,   577,
    -1,    -1,    -1,    -1,   549,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   566,   567,    -1,    -1,    -1,
    -1,    -1,    -1,   568,    -1,    -1,    -1,    -1,   606,   607,
   608,   609,   583,   652,   585,   586,    -1,    -1,    -1,    -1,
    22,    -1,    -1,    25,   589,   590,   591,   592,    -1,    -1,
    -1,   670,    -1,   604,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   615,    -1,    48,   618,    -1,   620,
    -1,    -1,   623,    -1,    -1,    -1,   627,    -1,   629,    -1,
   631,   632,    -1,    -1,    -1,    -1,    -1,    -1,    70,    -1,
    -1,   636,    -1,    -1,    -1,   646,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   655,   656,   657,    -1,   659,    91,
   729,   662,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   103,   673,   674,   675,   676,    -1,    -1,   679,    -1,
    -1,   113,    -1,    -1,    -1,    -1,   687,   688,   120,    -1,
    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   132,    -1,   703,   698,    -1,    -1,    -1,   708,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   147,    -1,    -1,    -1,   151,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   161,
    -1,    -1,    -1,    -1,   735,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,   176,    -1,    -1,    -1,    -1,   181,
   182,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   195,   196,   197,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   205,    -1,    -1,    -1,   209,    -1,    -1,
    -1,    -1,    -1,    -1,   216,   217,   218,    -1,   220,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   243,    -1,   245,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   265,    -1,    -1,    -1,    -1,   270,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,   288,    -1,   290,    -1,
    -1,    -1,   294,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   313,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   322,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   353,    -1,    -1,   356,    -1,    -1,    -1,    -1,   361,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   391,
    -1,    -1,    -1,    -1,    -1,   397,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   461,
    -1,    -1,   464,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   494,    -1,    -1,    -1,    -1,    -1,    -1,   501,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   533,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   545,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   553,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   563,   564,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   572,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   593,    -1,    -1,    -1,    -1,    -1,    -1,   600,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   625,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   634,    -1,    -1,    -1,   638,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   653,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   663,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,   677,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   701,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   711,
    -1,   713,    -1,    -1,   716,    -1,    -1,   719,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   730
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
#line 426 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 2:
#line 436 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 3:
#line 440 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 4:
#line 446 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 5:
#line 450 "parser-sming.y"
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
#line 465 "parser-sming.y"
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
#line 479 "parser-sming.y"
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
			    thisModulePtr->language = SMI_LANGUAGE_SMING;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			    free(yyvsp[0].text);
			    thisParserPtr->firstIndexlabelPtr = NULL;
			    thisParserPtr->identityObjectName = NULL;
			;
    break;}
case 8:
#line 499 "parser-sming.y"
{
			    thisParserPtr->identityObjectName = yyvsp[0].text;
			;
    break;}
case 9:
#line 505 "parser-sming.y"
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
#line 532 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      yyvsp[-1].text);
			    }
			;
    break;}
case 11:
#line 539 "parser-sming.y"
{
			    if (yyvsp[-1].text) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 12:
#line 546 "parser-sming.y"
{
			    if (moduleObjectPtr && yyvsp[-1].text) {
				setObjectDescription(moduleObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 13:
#line 552 "parser-sming.y"
{
			    if (moduleObjectPtr && yyvsp[0].text) {
				setObjectReference(moduleObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 14:
#line 564 "parser-sming.y"
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
#line 613 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 16:
#line 617 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 17:
#line 627 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 18:
#line 632 "parser-sming.y"
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
#line 647 "parser-sming.y"
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
#line 661 "parser-sming.y"
{
			    typeIdentifier = yyvsp[0].text;
			;
    break;}
case 21:
#line 666 "parser-sming.y"
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
#line 685 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].valuePtr) {
				setTypeValue(typePtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 23:
#line 691 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeFormat(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 24:
#line 697 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeUnits(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 25:
#line 703 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].status) {
				setTypeStatus(typePtr, yyvsp[0].status);
			    }
			;
    break;}
case 26:
#line 709 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeDescription(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 27:
#line 715 "parser-sming.y"
{
			    if (typePtr && yyvsp[0].text) {
				setTypeReference(typePtr, yyvsp[0].text);
			    }
			;
    break;}
case 28:
#line 721 "parser-sming.y"
{
			    yyval.typePtr = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 29:
#line 729 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 30:
#line 733 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 31:
#line 743 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 32:
#line 748 "parser-sming.y"
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
#line 763 "parser-sming.y"
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
#line 782 "parser-sming.y"
{
			    nodeIdentifier = yyvsp[0].text;
			;
    break;}
case 38:
#line 787 "parser-sming.y"
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
case 39:
#line 801 "parser-sming.y"
{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 40:
#line 807 "parser-sming.y"
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
#line 820 "parser-sming.y"
{
			    if (nodeObjectPtr && yyvsp[0].text) {
				setObjectReference(nodeObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 42:
#line 826 "parser-sming.y"
{
			    yyval.objectPtr = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			;
    break;}
case 43:
#line 834 "parser-sming.y"
{
			    scalarIdentifier = yyvsp[0].text;
			;
    break;}
case 44:
#line 839 "parser-sming.y"
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
case 45:
#line 852 "parser-sming.y"
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
#line 865 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, yyvsp[-1].access);
			    }
			;
    break;}
case 47:
#line 871 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].valuePtr) {
				setObjectValue(scalarObjectPtr, yyvsp[0].valuePtr);
			    }
			;
    break;}
case 48:
#line 877 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectFormat(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 49:
#line 883 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectUnits(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 50:
#line 889 "parser-sming.y"
{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 51:
#line 895 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[-1].text) {
				setObjectDescription(scalarObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 52:
#line 901 "parser-sming.y"
{
			    if (scalarObjectPtr && yyvsp[0].text) {
				setObjectReference(scalarObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 53:
#line 907 "parser-sming.y"
{
			    yyval.objectPtr = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			;
    break;}
case 54:
#line 916 "parser-sming.y"
{
			    tableIdentifier = yyvsp[0].text;
			;
    break;}
case 55:
#line 921 "parser-sming.y"
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
case 56:
#line 936 "parser-sming.y"
{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 57:
#line 942 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[-1].text) {
				setObjectDescription(tableObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 58:
#line 948 "parser-sming.y"
{
			    if (tableObjectPtr && yyvsp[0].text) {
				setObjectReference(tableObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 59:
#line 955 "parser-sming.y"
{
			    yyval.objectPtr = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			;
    break;}
case 60:
#line 963 "parser-sming.y"
{
			    rowIdentifier = yyvsp[0].text;
			;
    break;}
case 61:
#line 968 "parser-sming.y"
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
case 62:
#line 983 "parser-sming.y"
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
#line 1002 "parser-sming.y"
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
#line 1012 "parser-sming.y"
{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 65:
#line 1018 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[-1].text) {
				setObjectDescription(rowObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 66:
#line 1024 "parser-sming.y"
{
			    if (rowObjectPtr && yyvsp[0].text) {
				setObjectReference(rowObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 67:
#line 1031 "parser-sming.y"
{
			    yyval.objectPtr = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
			;
    break;}
case 68:
#line 1039 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 69:
#line 1047 "parser-sming.y"
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
#line 1062 "parser-sming.y"
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
#line 1076 "parser-sming.y"
{
			    columnIdentifier = yyvsp[0].text;
			;
    break;}
case 72:
#line 1081 "parser-sming.y"
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
				setObjectNodekind(notificationObjectPtr,
						  SMI_NODEKIND_NOTIFICATION);
			    }
			;
    break;}
case 89:
#line 1225 "parser-sming.y"
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
#line 1240 "parser-sming.y"
{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 91:
#line 1246 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[-1].text) {
				setObjectDescription(notificationObjectPtr,
						     yyvsp[-1].text);
			    }
			;
    break;}
case 92:
#line 1253 "parser-sming.y"
{
			    if (notificationObjectPtr && yyvsp[0].text) {
				setObjectReference(notificationObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 93:
#line 1259 "parser-sming.y"
{
			    yyval.objectPtr = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
			;
    break;}
case 94:
#line 1267 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 95:
#line 1271 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 96:
#line 1281 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 97:
#line 1285 "parser-sming.y"
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
#line 1300 "parser-sming.y"
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
#line 1314 "parser-sming.y"
{
			    groupIdentifier = yyvsp[0].text;
			;
    break;}
case 100:
#line 1319 "parser-sming.y"
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
case 101:
#line 1331 "parser-sming.y"
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
#line 1346 "parser-sming.y"
{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, yyvsp[0].status);
			    }
			;
    break;}
case 103:
#line 1352 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[-1].text) {
				setObjectDescription(groupObjectPtr, yyvsp[-1].text);
			    }
			;
    break;}
case 104:
#line 1358 "parser-sming.y"
{
			    if (groupObjectPtr && yyvsp[0].text) {
				setObjectReference(groupObjectPtr, yyvsp[0].text);
			    }
			;
    break;}
case 105:
#line 1364 "parser-sming.y"
{
			    yyval.objectPtr = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
			;
    break;}
case 106:
#line 1372 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 107:
#line 1376 "parser-sming.y"
{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 108:
#line 1386 "parser-sming.y"
{
			    yyval.rc = yyvsp[0].rc;
			;
    break;}
case 109:
#line 1391 "parser-sming.y"
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
#line 1406 "parser-sming.y"
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
#line 1420 "parser-sming.y"
{
			    complianceIdentifier = yyvsp[0].text;
			;
    break;}
case 112:
#line 1425 "parser-sming.y"
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
				     listPtr;
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
			    if (rowObjectPtr) {
				addObjectFlags(rowObjectPtr, FLAG_CREATABLE);
			    }
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 156:
#line 1818 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 157:
#line 1822 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 158:
#line 1828 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 159:
#line 1834 "parser-sming.y"
{
			    yyval.nodePtr = yyvsp[-2].nodePtr;
			;
    break;}
case 160:
#line 1840 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 161:
#line 1846 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 162:
#line 1852 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 163:
#line 1858 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 164:
#line 1862 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 165:
#line 1868 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 166:
#line 1874 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 167:
#line 1878 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 168:
#line 1884 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 169:
#line 1890 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 170:
#line 1894 "parser-sming.y"
{
			    yyval.status = yyvsp[-1].status;
			;
    break;}
case 171:
#line 1900 "parser-sming.y"
{
			    yyval.status = yyvsp[-2].status;
			;
    break;}
case 172:
#line 1906 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_UNKNOWN;
			;
    break;}
case 173:
#line 1910 "parser-sming.y"
{
			    yyval.access = yyvsp[-1].access;
			;
    break;}
case 174:
#line 1916 "parser-sming.y"
{
			    yyval.access = yyvsp[-2].access;
			;
    break;}
case 175:
#line 1922 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 176:
#line 1926 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-1].valuePtr;
			;
    break;}
case 177:
#line 1932 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 178:
#line 1938 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 179:
#line 1942 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 180:
#line 1948 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 181:
#line 1954 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 182:
#line 1958 "parser-sming.y"
{
			    yyval.text = yyvsp[-1].text;
			;
    break;}
case 183:
#line 1964 "parser-sming.y"
{
			    yyval.text = yyvsp[-2].text;
			;
    break;}
case 184:
#line 1970 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 185:
#line 1974 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 186:
#line 1981 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 187:
#line 1987 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 188:
#line 1991 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 189:
#line 1998 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 190:
#line 2004 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 191:
#line 2008 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-1].listPtr;
			;
    break;}
case 192:
#line 2015 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-4].listPtr;
			;
    break;}
case 193:
#line 2021 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 194:
#line 2025 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 195:
#line 2031 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].optionPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 196:
#line 2038 "parser-sming.y"
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
#line 2051 "parser-sming.y"
{
			       yyval.optionPtr = yyvsp[-1].optionPtr;
			   ;
    break;}
case 198:
#line 2060 "parser-sming.y"
{
			    yyval.optionPtr = util_malloc(sizeof(Option));
			    yyval.optionPtr->objectPtr = findObject(yyvsp[-7].text, thisParserPtr);
			    yyval.optionPtr->description = util_strdup(yyvsp[-4].text);
			;
    break;}
case 199:
#line 2068 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 200:
#line 2072 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 201:
#line 2078 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[0].refinementPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 202:
#line 2084 "parser-sming.y"
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
#line 2097 "parser-sming.y"
{
			    yyval.refinementPtr = yyvsp[-1].refinementPtr;
			;
    break;}
case 204:
#line 2108 "parser-sming.y"
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
#line 2131 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 206:
#line 2135 "parser-sming.y"
{
			    yyval.typePtr = yyvsp[0].typePtr;
			;
    break;}
case 207:
#line 2141 "parser-sming.y"
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
#line 2153 "parser-sming.y"
{
			    yyval.typePtr = typeSmingObjectIdentifierPtr;
			;
    break;}
case 209:
#line 2157 "parser-sming.y"
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
#line 2169 "parser-sming.y"
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
#line 2181 "parser-sming.y"
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
#line 2193 "parser-sming.y"
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
#line 2205 "parser-sming.y"
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
#line 2217 "parser-sming.y"
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
#line 2229 "parser-sming.y"
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
#line 2241 "parser-sming.y"
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
#line 2253 "parser-sming.y"
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
#line 2267 "parser-sming.y"
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
#line 2286 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 220:
#line 2290 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 221:
#line 2296 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 222:
#line 2300 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
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
#line 2317 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 226:
#line 2325 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 227:
#line 2329 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 228:
#line 2335 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 229:
#line 2341 "parser-sming.y"
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
#line 2354 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 231:
#line 2360 "parser-sming.y"
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
#line 2372 "parser-sming.y"
{
			    yyval.valuePtr = NULL;
			;
    break;}
case 233:
#line 2376 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 234:
#line 2382 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
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
#line 2399 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-3].rangePtr;
			    yyval.listPtr->nextPtr = yyvsp[-2].listPtr;
			;
    break;}
case 238:
#line 2407 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 239:
#line 2411 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 240:
#line 2417 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].rangePtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 241:
#line 2423 "parser-sming.y"
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
#line 2436 "parser-sming.y"
{
			    yyval.rangePtr = yyvsp[0].rangePtr;
			;
    break;}
case 243:
#line 2442 "parser-sming.y"
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
#line 2461 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 245:
#line 2465 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 246:
#line 2471 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 247:
#line 2477 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 248:
#line 2484 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].namedNumberPtr;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 249:
#line 2492 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 250:
#line 2496 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 251:
#line 2502 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].namedNumberPtr;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 252:
#line 2509 "parser-sming.y"
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
#line 2523 "parser-sming.y"
{
			    yyval.namedNumberPtr = yyvsp[0].namedNumberPtr;
			;
    break;}
case 254:
#line 2529 "parser-sming.y"
{
			    yyval.namedNumberPtr = util_malloc(sizeof(NamedNumber));
			    yyval.namedNumberPtr->name = yyvsp[-6].text;
			    yyval.namedNumberPtr->valuePtr = yyvsp[-2].valuePtr;
			;
    break;}
case 255:
#line 2537 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 256:
#line 2545 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 257:
#line 2549 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 258:
#line 2555 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 259:
#line 2561 "parser-sming.y"
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
#line 2574 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 261:
#line 2580 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 262:
#line 2588 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 263:
#line 2592 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 264:
#line 2598 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 265:
#line 2604 "parser-sming.y"
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
#line 2617 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 267:
#line 2623 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 268:
#line 2631 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 269:
#line 2635 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 270:
#line 2641 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 271:
#line 2647 "parser-sming.y"
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
#line 2660 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 273:
#line 2666 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[-2].listPtr;
			;
    break;}
case 274:
#line 2672 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 275:
#line 2676 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = yyvsp[-2].text;
			    yyval.listPtr->nextPtr = yyvsp[-1].listPtr;
			;
    break;}
case 276:
#line 2684 "parser-sming.y"
{
			    yyval.listPtr = NULL;
			;
    break;}
case 277:
#line 2688 "parser-sming.y"
{
			    yyval.listPtr = yyvsp[0].listPtr;
			;
    break;}
case 278:
#line 2694 "parser-sming.y"
{
			    yyval.listPtr = util_malloc(sizeof(List));
			    yyval.listPtr->ptr = (void *)yyvsp[0].text;
			    yyval.listPtr->nextPtr = NULL;
			;
    break;}
case 279:
#line 2700 "parser-sming.y"
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
#line 2713 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 281:
#line 2719 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 282:
#line 2723 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 283:
#line 2729 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 284:
#line 2733 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 285:
#line 2739 "parser-sming.y"
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
#line 2750 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 287:
#line 2756 "parser-sming.y"
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
#line 2767 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 289:
#line 2773 "parser-sming.y"
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
#line 2787 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 291:
#line 2791 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 292:
#line 2797 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 293:
#line 2801 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 294:
#line 2811 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 295:
#line 2818 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 296:
#line 2824 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 297:
#line 2830 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 298:
#line 2843 "parser-sming.y"
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
#line 2868 "parser-sming.y"
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
				/* TODO */
				yyval.valuePtr->value.oidlen = 2;
				yyval.valuePtr->value.oid[0] = 0;
				yyval.valuePtr->value.oid[1] = 0;
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
#line 2907 "parser-sming.y"
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
#line 2927 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an octet string */
			    yyval.valuePtr = NULL;
			;
    break;}
case 302:
#line 2933 "parser-sming.y"
{
			    /* TODO */
			    /* Note: might also be an OID */
			    yyval.valuePtr = NULL;
			;
    break;}
case 303:
#line 2939 "parser-sming.y"
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
#line 2951 "parser-sming.y"
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
#line 2973 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 306:
#line 2978 "parser-sming.y"
{
			    /* TODO */
			    yyval.valuePtr = NULL;
			;
    break;}
case 307:
#line 2985 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_CURRENT;
			;
    break;}
case 308:
#line 2989 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_DEPRECATED;
			;
    break;}
case 309:
#line 2993 "parser-sming.y"
{
			    yyval.status = SMI_STATUS_OBSOLETE;
			;
    break;}
case 310:
#line 2999 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOT_ACCESSIBLE;
			;
    break;}
case 311:
#line 3003 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_NOTIFY;
			;
    break;}
case 312:
#line 3007 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_ONLY;
			;
    break;}
case 313:
#line 3011 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_WRITE;
			;
    break;}
case 314:
#line 3015 "parser-sming.y"
{
			    yyval.access = SMI_ACCESS_READ_CREATE;
			;
    break;}
case 315:
#line 3021 "parser-sming.y"
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
case 316:
#line 3050 "parser-sming.y"
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
case 317:
#line 3077 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 318:
#line 3083 "parser-sming.y"
{
			    yyval.text = NULL;
			;
    break;}
case 319:
#line 3087 "parser-sming.y"
{
			    /* TODO: check upper limit of 127 subids */ 
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 320:
#line 3094 "parser-sming.y"
{
			    yyval.text = yyvsp[0].text;
			;
    break;}
case 321:
#line 3098 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[-1].text) + strlen(yyvsp[0].text) + 1);
			    strcpy(yyval.text, yyvsp[-1].text);
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[-1].text);
			    free(yyvsp[0].text);
			;
    break;}
case 322:
#line 3108 "parser-sming.y"
{
			    yyval.text = util_malloc(strlen(yyvsp[0].text) + 1 + 1);
			    strcpy(yyval.text, ".");
			    strcat(yyval.text, yyvsp[0].text);
			    free(yyvsp[0].text);
			;
    break;}
case 323:
#line 3117 "parser-sming.y"
{
			    yyval.text = util_strdup(yyvsp[0].text);
			;
    break;}
case 324:
#line 3123 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = 42;
			;
    break;}
case 325:
#line 3130 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_UNSIGNED64;
			    yyval.valuePtr->value.unsigned64 = strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 326:
#line 3138 "parser-sming.y"
{
			    yyval.valuePtr = util_malloc(sizeof(SmiValue));
			    yyval.valuePtr->basetype = SMI_BASETYPE_INTEGER64;
			    yyval.valuePtr->value.integer64 = - strtoull(yyvsp[0].text, NULL, 10);
			;
    break;}
case 327:
#line 3146 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 328:
#line 3150 "parser-sming.y"
{
			    yyval.valuePtr = yyvsp[0].valuePtr;
			;
    break;}
case 329:
#line 3160 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 330:
#line 3167 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 331:
#line 3173 "parser-sming.y"
{
			    yyval.rc = 0;
			;
    break;}
case 332:
#line 3179 "parser-sming.y"
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
#line 3184 "parser-sming.y"


#endif
			
