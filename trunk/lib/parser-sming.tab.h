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
#define	objectsKeyword	297
#define	mandatoryKeyword	298
#define	optionalKeyword	299
#define	refineKeyword	300
#define	OctetStringKeyword	301
#define	ObjectIdentifierKeyword	302
#define	Integer32Keyword	303
#define	Unsigned32Keyword	304
#define	Integer64Keyword	305
#define	Unsigned64Keyword	306
#define	Float32Keyword	307
#define	Float64Keyword	308
#define	Float128Keyword	309
#define	BitsKeyword	310
#define	EnumerationKeyword	311
#define	currentKeyword	312
#define	deprecatedKeyword	313
#define	obsoleteKeyword	314
#define	noaccessKeyword	315
#define	notifyonlyKeyword	316
#define	readonlyKeyword	317
#define	readwriteKeyword	318

