typedef union {
    char           *text;	  		/* scanned quoted text       */
    char           *id;				/* identifier name           */
    int            err;				/* actually just a dummy     */
    Object         *objectPtr;			/* object identifier         */
    SmiStatus      status;			/* a STATUS value            */
    SmiAccess      access;			/* an ACCESS value           */
    Type           *typePtr;
    Index          *indexPtr;
    List           *listPtr;			/* SEQUENCE and INDEX lists  */
    NamedNumber    *namedNumberPtr;		/* BITS or enum item         */
    Range          *rangePtr;			/* type restricting range    */
    SmiValue	   *valuePtr;
    SmiUnsigned32  unsigned32;			/*                           */
    SmiInteger32   integer32;			/*                           */
    struct Compl   compl;
    Module	   *modulePtr;
} YYSTYPE;
#define	DOT_DOT	257
#define	COLON_COLON_EQUAL	258
#define	UPPERCASE_IDENTIFIER	259
#define	LOWERCASE_IDENTIFIER	260
#define	NUMBER	261
#define	BIN_STRING	262
#define	HEX_STRING	263
#define	QUOTED_STRING	264
#define	ACCESS	265
#define	AGENT_CAPABILITIES	266
#define	APPLICATION	267
#define	AUGMENTS	268
#define	BEGIN_	269
#define	BITS	270
#define	CHOICE	271
#define	CONTACT_INFO	272
#define	CREATION_REQUIRES	273
#define	COUNTER32	274
#define	COUNTER64	275
#define	DEFINITIONS	276
#define	DEFVAL	277
#define	DESCRIPTION	278
#define	DISPLAY_HINT	279
#define	END	280
#define	ENTERPRISE	281
#define	EXPORTS	282
#define	FROM	283
#define	GROUP	284
#define	GAUGE32	285
#define	IDENTIFIER	286
#define	IMPLICIT	287
#define	IMPLIED	288
#define	IMPORTS	289
#define	INCLUDES	290
#define	INDEX	291
#define	INTEGER	292
#define	INTEGER32	293
#define	IPADDRESS	294
#define	LAST_UPDATED	295
#define	MACRO	296
#define	MANDATORY_GROUPS	297
#define	MAX_ACCESS	298
#define	MIN_ACCESS	299
#define	MODULE	300
#define	MODULE_COMPLIANCE	301
#define	MODULE_IDENTITY	302
#define	NOTIFICATIONS	303
#define	NOTIFICATION_GROUP	304
#define	NOTIFICATION_TYPE	305
#define	OBJECT	306
#define	OBJECT_GROUP	307
#define	OBJECT_IDENTITY	308
#define	OBJECT_TYPE	309
#define	OBJECTS	310
#define	OCTET	311
#define	OF	312
#define	ORGANIZATION	313
#define	OPAQUE	314
#define	PRODUCT_RELEASE	315
#define	REFERENCE	316
#define	REVISION	317
#define	SEQUENCE	318
#define	SIZE	319
#define	STATUS	320
#define	STRING	321
#define	SUPPORTS	322
#define	SYNTAX	323
#define	TEXTUAL_CONVENTION	324
#define	TIMETICKS	325
#define	TRAP_TYPE	326
#define	UNITS	327
#define	UNIVERSAL	328
#define	UNSIGNED32	329
#define	VARIABLES	330
#define	VARIATION	331
#define	WRITE_SYNTAX	332

