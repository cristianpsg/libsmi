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
#define	DOT_DOT	257
#define	COLON_COLON_EQUAL	258
#define	UPPERCASE_IDENTIFIER	259
#define	LOWERCASE_IDENTIFIER	260
#define	NUMBER	261
#define	NEGATIVENUMBER	262
#define	BIN_STRING	263
#define	HEX_STRING	264
#define	QUOTED_STRING	265
#define	ACCESS	266
#define	AGENT_CAPABILITIES	267
#define	APPLICATION	268
#define	AUGMENTS	269
#define	BEGIN_	270
#define	BITS	271
#define	CHOICE	272
#define	CONTACT_INFO	273
#define	CREATION_REQUIRES	274
#define	COUNTER32	275
#define	COUNTER64	276
#define	DEFINITIONS	277
#define	DEFVAL	278
#define	DESCRIPTION	279
#define	DISPLAY_HINT	280
#define	END	281
#define	ENTERPRISE	282
#define	EXPORTS	283
#define	FROM	284
#define	GROUP	285
#define	GAUGE32	286
#define	IDENTIFIER	287
#define	IMPLICIT	288
#define	IMPLIED	289
#define	IMPORTS	290
#define	INCLUDES	291
#define	INDEX	292
#define	INTEGER	293
#define	INTEGER32	294
#define	IPADDRESS	295
#define	LAST_UPDATED	296
#define	MACRO	297
#define	MANDATORY_GROUPS	298
#define	MAX_ACCESS	299
#define	MIN_ACCESS	300
#define	MODULE	301
#define	MODULE_COMPLIANCE	302
#define	MODULE_IDENTITY	303
#define	NOTIFICATIONS	304
#define	NOTIFICATION_GROUP	305
#define	NOTIFICATION_TYPE	306
#define	OBJECT	307
#define	OBJECT_GROUP	308
#define	OBJECT_IDENTITY	309
#define	OBJECT_TYPE	310
#define	OBJECTS	311
#define	OCTET	312
#define	OF	313
#define	ORGANIZATION	314
#define	OPAQUE	315
#define	PRODUCT_RELEASE	316
#define	REFERENCE	317
#define	REVISION	318
#define	SEQUENCE	319
#define	SIZE	320
#define	STATUS	321
#define	STRING	322
#define	SUPPORTS	323
#define	SYNTAX	324
#define	TEXTUAL_CONVENTION	325
#define	TIMETICKS	326
#define	TRAP_TYPE	327
#define	UNITS	328
#define	UNIVERSAL	329
#define	UNSIGNED32	330
#define	VARIABLES	331
#define	VARIATION	332
#define	WRITE_SYNTAX	333

