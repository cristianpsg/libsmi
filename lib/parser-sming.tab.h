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

