typedef union {
    char           *text;	  		/* scanned quoted text       */
    char           *id;				/* identifier name           */
    int            err;				/* actually just a dummy     */
    time_t	   date;			/* a date value              */
    Object         *objectPtr;			/* object identifier         */
    SmiStatus      status;			/* a STATUS value            */
    SmiAccess      access;			/* an ACCESS value           */
    Type           *typePtr;
    List           *listPtr;			/* SEQUENCE and INDEX lists  */
    NamedNumber    *namedNumberPtr;		/* BITS or enum item         */
    Range          *rangePtr;			/* type restricting range    */
    SmiValue	   *valuePtr;
    SmiUnsigned32  unsigned32;			/*                           */
    SmiInteger32   integer32;			/*                           */
    struct Compl   compl;
    struct Index   index;
    Module	   *modulePtr;
} YYSTYPE;
#define	DOT_DOT	258
#define	COLON_COLON_EQUAL	259
#define	UPPERCASE_IDENTIFIER	260
#define	LOWERCASE_IDENTIFIER	261
#define	NUMBER	262
#define	NEGATIVENUMBER	263
#define	BIN_STRING	264
#define	HEX_STRING	265
#define	QUOTED_STRING	266
#define	ACCESS	267
#define	AGENT_CAPABILITIES	268
#define	APPLICATION	269
#define	AUGMENTS	270
#define	BEGIN_	271
#define	BITS	272
#define	CHOICE	273
#define	CONTACT_INFO	274
#define	CREATION_REQUIRES	275
#define	COUNTER32	276
#define	COUNTER64	277
#define	DEFINITIONS	278
#define	DEFVAL	279
#define	DESCRIPTION	280
#define	DISPLAY_HINT	281
#define	END	282
#define	ENTERPRISE	283
#define	EXPORTS	284
#define	FROM	285
#define	GROUP	286
#define	GAUGE32	287
#define	IDENTIFIER	288
#define	IMPLICIT	289
#define	IMPLIED	290
#define	IMPORTS	291
#define	INCLUDES	292
#define	INDEX	293
#define	INTEGER	294
#define	INTEGER32	295
#define	IPADDRESS	296
#define	LAST_UPDATED	297
#define	MACRO	298
#define	MANDATORY_GROUPS	299
#define	MAX_ACCESS	300
#define	MIN_ACCESS	301
#define	MODULE	302
#define	MODULE_COMPLIANCE	303
#define	MODULE_IDENTITY	304
#define	NOTIFICATIONS	305
#define	NOTIFICATION_GROUP	306
#define	NOTIFICATION_TYPE	307
#define	OBJECT	308
#define	OBJECT_GROUP	309
#define	OBJECT_IDENTITY	310
#define	OBJECT_TYPE	311
#define	OBJECTS	312
#define	OCTET	313
#define	OF	314
#define	ORGANIZATION	315
#define	OPAQUE	316
#define	PRODUCT_RELEASE	317
#define	REFERENCE	318
#define	REVISION	319
#define	SEQUENCE	320
#define	SIZE	321
#define	STATUS	322
#define	STRING	323
#define	SUPPORTS	324
#define	SYNTAX	325
#define	TEXTUAL_CONVENTION	326
#define	TIMETICKS	327
#define	TRAP_TYPE	328
#define	UNITS	329
#define	UNIVERSAL	330
#define	UNSIGNED32	331
#define	VARIABLES	332
#define	VARIATION	333
#define	WRITE_SYNTAX	334

