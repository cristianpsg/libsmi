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
#define	identityKeyword	269
#define	oidKeyword	270
#define	dateKeyword	271
#define	organizationKeyword	272
#define	contactKeyword	273
#define	descriptionKeyword	274
#define	referenceKeyword	275
#define	extensionKeyword	276
#define	typedefKeyword	277
#define	typeKeyword	278
#define	writetypeKeyword	279
#define	nodeKeyword	280
#define	scalarKeyword	281
#define	tableKeyword	282
#define	columnKeyword	283
#define	rowKeyword	284
#define	notificationKeyword	285
#define	groupKeyword	286
#define	complianceKeyword	287
#define	formatKeyword	288
#define	unitsKeyword	289
#define	statusKeyword	290
#define	accessKeyword	291
#define	defaultKeyword	292
#define	impliedKeyword	293
#define	indexKeyword	294
#define	augmentsKeyword	295
#define	reordersKeyword	296
#define	sparseKeyword	297
#define	expandsKeyword	298
#define	createKeyword	299
#define	membersKeyword	300
#define	objectsKeyword	301
#define	mandatoryKeyword	302
#define	optionalKeyword	303
#define	refineKeyword	304
#define	abnfKeyword	305
#define	OctetStringKeyword	306
#define	ObjectIdentifierKeyword	307
#define	Integer32Keyword	308
#define	Unsigned32Keyword	309
#define	Integer64Keyword	310
#define	Unsigned64Keyword	311
#define	Float32Keyword	312
#define	Float64Keyword	313
#define	Float128Keyword	314
#define	BitsKeyword	315
#define	EnumerationKeyword	316
#define	currentKeyword	317
#define	deprecatedKeyword	318
#define	obsoleteKeyword	319
#define	noaccessKeyword	320
#define	notifyonlyKeyword	321
#define	readonlyKeyword	322
#define	readwriteKeyword	323
#define	readcreateKeyword	324

