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
    Value	   *valuePtr;
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
#define	readcreateKeyword	320

