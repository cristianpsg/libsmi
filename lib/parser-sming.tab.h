typedef union {
    char           *id;				/* identifier name           */
    int            rc;				/* >=0: ok, <0: error        */
    time_t	   date;			/* a date value		     */
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

