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

