/*
 * mibdump-xml.c --
 *
 *      Operations to dump SMIng module information as XML schemas.
 *
 * Copyright (c) 2002 T. Klie, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "smi.h"
#include "mibdump.h"
#include "table.h"

#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>

#undef DEMO_USE_SNMP_VERSION_3

#ifdef DEMO_USE_SNMP_VERSION_3
#include "ucd-snmp/transform_oids.h"
const char *our_v3_passphrase = "The UCD Demo Password";
#endif

#define  INDENT		2    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	4    /* column to start multiline texts */
#define  INDENTMAX	64   /* max column to fill, break lines otherwise */

/* xxx let the user configure this with command-line args */
#define DXSD_MIBPREFIX "if"

static char *schemaLocation = "http://www.ibr.cs.tu-bs.de/~tklie/";
static char *peername = "henkell.ibr.cs.tu-bs.de";
static char *community = "public";

static struct snmp_session *ss;

#if 0
typedef struct XmlEscape {
    char character;
    char *escape;
} XmlEscape;

static XmlEscape xmlEscapes [] = {
    { '<',	"&lt;" },
    { '>',	"&gt;" },
    { '&',	"&amp;" },
    { 0,	NULL }
};
#endif /* #if 0 */

static int current_column = 0;
    
/* some forward declarations */
static void fprintElement( FILE *f, int indent,
			   SmiNode *smiNode, oid *suffix, int suflen);


static int
numDigits( int val )
{
    int ret  = 0;

    for( val; val / 10; val = val / 10 ) {
	ret++;
    }
    
    return ret;
}


static int
numOIDDigits( oid *anOID, int oid_len )
{
    int ret = 1;
    int pos;

    for( pos = 0; pos < oid_len; pos++ ) {
	ret += numDigits( anOID[ pos ] ) + 1;
    }

    return ret;
}

static char
*oidToStr( oid *anOID, int oid_len )
{
    char * ret;
    int pos;
    
    ret = xmalloc( numOIDDigits( anOID, oid_len ) );
    for( pos = 0; pos < oid_len; pos++ ) {
	sprintf( ret, "%s.%ld", ret, anOID[ pos ] );
    }

    return ret;
}



static char 
*getOctStr( struct variable_list *vars )
{
     int i;
     char *ret;

     ret = xmalloc( 3 * vars->val_len );
     for( i = 0; i < vars->val_len; i++ ) {
	 if( ! i ) {
	     sprintf( ret, "%x", vars->val.string[0] );
	 }
	 else {
	     sprintf( ret, "%s%x", ret, vars->val.string[i] );
	 }
     }
     return ret;
}



static char
*getAsciiStr( struct variable_list *vars )
{
    char *ret;

    ret = xmalloc( vars->val_len + 1 );
    strncpy( ret, vars->val.string, vars->val_len );
    ret[ vars->val_len ] = '\0';

    return ret;
}



static int
isAsciiString( struct variable_list *vars )
{
    int i;

    for( i = 0; i < vars->val_len; i++ ) {
	if( vars->val.string[i] < 32 ) {
	    /* value is non-printable */
	    return 0;
	}
    }

    return 1;
}


static char*
getStringVal( struct variable_list *vars )
{
    char* ret;

    switch( vars->type & 0x0F ) {

    case ASN_INTEGER:
    case ASN_BOOLEAN:
	ret = xmalloc( numDigits( *(vars->val.integer) ) );
	sprintf( ret, "%ld", *(vars->val.integer) );
	return ret;

    case ASN_OCTET_STR:
    case ASN_BIT_STR:
	if( vars->val_len ) {

	    ret = isAsciiString( vars ) ?
		getAsciiStr( vars ) :
		getOctStr( vars );
	   
	}
	else {
	    ret = NULL;
	}
	return ret;

    case ASN_OBJECT_ID:
	return oidToStr( vars->val.objid, vars->val_len  );

    default:
	ret = xmalloc( 27 + numDigits( vars->type ) );
	sprintf( ret, "type not yet implemented: %d", vars->type );
	return ret;
    }
    
}

#if 0
static int numIndex( SmiNode *smiNode )
{
    SmiElement *iterElem;
    int ret = 0;

    for( iterElem = smiGetFirstElement( smiNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	ret++;
    }
    return ret;
}
#endif /* #if 0 */



static struct snmp_session *initSnmp()
{
    struct snmp_session session, *ss;
    /* initialize SNMP library */
    init_snmp( "mibdump" );

    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    snmp_sess_init( &session );                   /* set up defaults */
    session.peername = "henkell";/*peername;*/

    /* set up the authentication parameters for talking to the server */
    
#ifdef DEMO_USE_SNMP_VERSION_3
    
    /* Use SNMPv3 to talk to the experimental server */
    
    /* set the SNMP version number */
    session.version=SNMP_VERSION_3;
    
    /* set the SNMPv3 user name */
    session.securityName = strdup("MD5User");
    session.securityNameLen = strlen(session.securityName);
    
    /* set the security level to authenticated, but not encrypted */
    session.securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
    
    /* set the authentication method to MD5 */
    session.securityAuthProto = usmHMACMD5AuthProtocol;
    session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;
    
    /* set the authentication key to a MD5 hashed version of our
       passphrase "The UCD Demo Password" (which must be at least 8
       characters long) */
    if (generate_Ku(session.securityAuthProto,
                    session.securityAuthProtoLen,
                    (u_char *) our_v3_passphrase, strlen(our_v3_passphrase),
                    session.securityAuthKey,
                    &session.securityAuthKeyLen) != SNMPERR_SUCCESS) {
        snmp_perror(argv[0]);
        snmp_log(LOG_ERR,
                 "Error generating Ku from authentication pass phrase. \n");
        exit(1);
    }
    
#else /* we'll use the insecure (but simplier) SNMPv1 */
    
    /* set the SNMP version number */
    session.version = SNMP_VERSION_1;
    
    /* set the SNMPv1 community name used for authentication */
    session.community = "public"; /*community;*/
    session.community_len = strlen(session.community);
    
#endif /* SNMPv1 */

    /* windows32 specific initialization (is a noop on unix) */
    SOCK_STARTUP;
    
    /*
     * Open the session
     */
    ss = snmp_open(&session);                     /* establish the session */

    if (!ss) {
        snmp_perror("ack");
        snmp_log(LOG_ERR, "something horrible happened!!!\n");
	
	fprintf( stderr, "host: %s, community: %s\n", peername, community );
        exit(2);
    }
    
    return ss;
}


static void fprint(FILE *f, char *fmt, ...)
{
    va_list ap;
    char    s[200];
    char    *p;

    va_start(ap, fmt);
#ifdef HAVE_VSNPRINTF
    current_column += vsnprintf(s, sizeof(s), fmt, ap);
#else
    current_column += vsprintf(s, fmt, ap);	/* buffer overwrite */
#endif
    va_end(ap);

    fputs(s, f);

    if ((p = strrchr(s, '\n'))) {
	current_column = strlen(p) - 1;
    }
}



static void fprintSegment(FILE *f, int column, char *string, int length)
{
    fprint(f, "%*c%s", column, ' ', string);
    if (length) {
	fprint(f, "%*c", length - strlen(string) - column, ' ');
    }
}


#if 0
static void fprintMultilineString(FILE *f, int column, const char *s)
{
    int i, j, len;

#ifdef INDENTTEXTS
    fprintSegment(f, column + INDENTTEXTS, "", 0);
#endif /* #ifdef INDENTTEXTS */
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    for (j = 0; xmlEscapes[j].character; j++) {
		if (xmlEscapes[j].character == s[i]) break;
	    }
	    if (xmlEscapes[j].character) {
		fputs(xmlEscapes[j].escape, f);
		current_column += strlen(xmlEscapes[j].escape);
	    } else {
		putc(s[i], f);
		current_column++;
	    }
	    if (s[i] == '\n') {
		current_column = 0;
#ifdef INDENTTEXTS
		fprintSegment(f, column + INDENTTEXTS, "", 0);
#endif /* #ifdef INDENTTEXTS */
	    }
	}
    }
    current_column++;
}
#endif /* #if 0 */


static int hasChildren( SmiNode *smiNode, SmiNodekind nodekind )
{
    SmiNode *iterNode;
    int childNodeCount = 0; 

    for( iterNode = smiGetFirstChildNode( smiNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ){
	if( nodekind & iterNode->nodekind ) {
	    childNodeCount++;
	}
    }
    return childNodeCount;
}


static oid
*oidcpy( SmiNode *smiNode )
{
    oid *anOID;
    int j;
    
    anOID = xmalloc( smiNode->oidlen * sizeof( oid ) );
    for( j = 0; j < smiNode->oidlen; j++ ) {
	anOID[j] = (oid)smiNode->oid[j];
    }
    return anOID;
}

static oid
*oidadd( oid *anOID, int oidlen, oid *suffix, int suflen, int *newlen )
{
    int j;
    
    anOID = xrealloc( anOID, ( oidlen + suflen ) * sizeof( oid ) );
    for( j = oidlen; j - oidlen < suflen; j++ ) {
	anOID[ j ] = suffix[ j - oidlen ];
    }
    *newlen = oidlen + suflen;
    return anOID;
}

static int
snmpGet( oid *anOID,
	 int oidlen,
	 u_char method,
	 struct snmp_pdu **response )
{
    struct snmp_pdu *pdu = NULL;
    int status;

    if( ! ss ) {
	ss = initSnmp();
    }
    
    pdu = snmp_pdu_create( method );
    
    snmp_add_null_var( pdu, anOID, oidlen );
    status = snmp_synch_response( ss, pdu, response );

    if( status == STAT_SUCCESS && (*response)->errstat == SNMP_ERR_NOERROR) {
	return 1;
    }
    else {
	int i;
	
	fprintf( stderr, "NOT READY:\n" );
	for( i = 0; i < oidlen; i++ ) {
	    fprintf( stderr, ".%lu",anOID[i] );
	}
	fprintf( stderr, "\n" );
	if (status == STAT_SUCCESS)
	    fprintf(stderr, "Error in packet\nReason: %s\n",
		    snmp_errstring( (*response)->errstat));
	else
	    snmp_sess_perror("snmpget", ss);
	return 0;
    }
}

static char
*getIndexStr( SmiNode *tableNode, MDRow *row )
{
    char *ret = NULL;
    SmiElement *iterElem;

    for( iterElem = smiGetFirstElement( tableNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	SmiNode *iterNode = smiGetElementNode( iterElem );
	MDCol *iterCol = md_findColumn( row,
				       iterNode->oid[ iterNode->oidlen - 1 ] );
	if( ret ) {
	    ret = xrealloc( ret,
			    5 + strlen ( iterNode->name ) +
			    strlen( iterCol->value ) );
	    sprintf( "%s %s=\"%s\"", ret, iterNode->name, iterCol->value );
	}
	else {
	    ret = xmalloc( 4 + strlen ( iterNode->name ) +
			   strlen( iterCol->value ) );
	    sprintf( ret, "%s=\"%s\"", iterNode->name, iterCol->value );
	}
    }

/*    printf( ret ); */
    return ret;
}


static SmiNode
*getColumnNode( SmiNode *tableNode, oid col )
{
    SmiNode *iterNode;

    for( iterNode = smiGetFirstChildNode( tableNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ) {
	if( iterNode->oid[ iterNode->oidlen - 1 ] == (SmiSubid)col ) {
	    return iterNode;
	}
    }
    return NULL;
}


static void fprintTable( FILE *f, int indent,
			 MDTable *table, SmiNode *tableNode )
{
    MDRow *iterRow;
    MDCol *iterCol;

    for( iterRow = table->rows; iterRow; iterRow = iterRow->next ) {
	char *indexStr = getIndexStr( tableNode, iterRow );

	/* print row with index */
	fprintSegment( f, indent, "<", 0 );
	fprint( f, "%s %s>\n", table->name, indexStr );
	xfree( indexStr );

	/* print all columns */
	for( iterCol = iterRow->columns; iterCol; iterCol = iterCol->next ) {
	    SmiNode *colNode = getColumnNode( tableNode, iterCol->col  );
	    SmiElement *iterElem;
	    int cont = 1;
	    
	    /* check if we are index column */
	    for( iterElem = smiGetFirstElement( smiGetParentNode( colNode ) ) ;
		 iterElem;
		 iterElem = smiGetNextElement( iterElem ) ) {
		if( colNode == smiGetElementNode( iterElem ) ) {
		    /* we are index coulumn ==> do not print element */
		    cont = 0;
		}
	    }
	    
	    if( colNode && cont ) {
		fprintSegment( f, indent + INDENT, "<", 0 );
		fprint( f, "%s value=\"%s\"/>\n",
			colNode->name, iterCol->value );
	    }
	}
	fprintSegment( f, indent, "</", 0 );
	fprint( f, "%s>\n", table->name );
    }
}

static void fprintElement( FILE *f, int indent,
			   SmiNode *smiNode, oid *suffix, int suflen )
{
    struct snmp_pdu *response;
    oid *anOID;
    int oidlen;
    
    switch( smiNode->nodekind ) {
	SmiType *smiType;

    case SMI_NODEKIND_NODE :
	{
	    SmiNode *iterNode;
	   
	    fprintSegment( f, indent, "<", 0 );
	    fprint( f, "%s>\n", smiNode->name );
	   
	    for( iterNode = smiGetFirstChildNode( smiNode );
		 iterNode;
		 iterNode = smiGetNextChildNode( iterNode ) ) {
		if( iterNode->nodekind == SMI_NODEKIND_SCALAR ) {
		    fprintElement( f, indent + INDENT,
				   iterNode, suffix, suflen );
		}
	    }
	  
	    fprintSegment( f, indent, "</", 0 );
	    fprint( f, "%s>\n", smiNode->name );
	}
	break;

    case SMI_NODEKIND_TABLE :
    {
	SmiNode *iterNode;
	
	/* ignore tables and just include their entries */
	for( iterNode = smiGetFirstChildNode( smiNode );
	     iterNode;
	     iterNode = smiGetNextChildNode( iterNode ) ) {
	    fprintElement( f, indent, iterNode, suffix, suflen );
	}
	break;
    }

    case SMI_NODEKIND_ROW :
    {
	MDTable *table;
	MDRow *row;
	oid *prefix;
	int prefix_len;
	oid col;
	
	prefix = oidcpy( smiNode );
	prefix_len = smiNode->oidlen;
	table = md_createTable( smiNode->name, prefix, prefix_len );
	
	if( snmpGet( prefix, prefix_len, 
		     SNMP_MSG_GETNEXT, &response ) ) {
	    while( ! memcmp( response->variables->name, prefix,
			     MIN( response->variables->name_length,
				  prefix_len ) * sizeof( oid ) ) ) {
		oid newOID[MAX_OID_LEN];
		int newOID_len;
		oid *idx;

		col = response->variables->name[ prefix_len ];
		idx = &(response->variables->name[prefix_len + 1]);
		row = md_findRow( table, idx,
				  response->variables->name_length -
				  prefix_len - 1 );
		if( !row ) {
		    /* row does not exist */
		    row = md_addRow( table, idx,
				     response->variables->name_length -
				     prefix_len - 1 );
		}
		md_addColumn( row, col, getStringVal( response->variables ) );
		
		/* get next value */
		memcpy( newOID, response->variables->name,
			response->variables->name_length * sizeof( oid ) );
		newOID_len = response->variables->name_length;
		if( ! snmpGet( newOID, newOID_len,
			       SNMP_MSG_GETNEXT, &response ) ) {
		    break; /* if we cannot get more values let's exit loop */
		}
	    }
	}
	fprintTable( f, indent, table, smiNode );
	md_dropTable( table );
	
	break;
    }
    
    case SMI_NODEKIND_SCALAR :
    {
	smiType = smiGetNodeType( smiNode );

	/* get values from agent*/
	anOID = oidcpy( smiNode );
	anOID = oidadd( anOID, smiNode->oidlen, suffix, suflen, &oidlen );
	
	if( snmpGet( anOID, oidlen, SNMP_MSG_GET, &response ) ) {
	    fprintSegment( f, indent, "<", 0 );
	    fprint( f, "%s value=\"%s\"/>\n", smiNode->name,
		    getStringVal( response->variables ) );
	}
	xfree( anOID );
	break;
    }

    case SMI_NODEKIND_NOTIFICATION:
	/* this should not occur here */
	break;

    default:
	fprint( f, "<!-- Warning! Unhandled Element! No details available!\n" );
	fprint( f, "      Nodekind: %#4x -->\n\n" );
	
    }
}


static void fprintRows( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;

    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    fprintElement( f, INDENT, iterNode, NULL, 0 );
	}
    }
}


static void fprintNodes( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;
    oid *suffix = xmalloc( sizeof( oid ) );

    suffix[0] = 0;
    
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_SCALAR ) ) {
	    fprintElement( f, INDENT, iterNode, suffix, 1 );
	}
    }


    
    xfree( suffix );
}

static void fprintModule(FILE *f, SmiModule *smiModule)
{
    /* print root node */
    fprint( f, "<%s>\n", smiModule->name );

    /* print other nodes (scalars and scalar groups) */
    fprintNodes( f, smiModule );

    /* print tables */
    fprintRows( f, smiModule );
    
    fprint( f, "</%s>\n", smiModule->name );
}

static void dumpXml(int modc, SmiModule **modv, int flags, char *output)
{
    int  i;
    FILE *f = stdout;

    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "mibdump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

       for (i = 0; i < modc; i++) {
	   
	/* make sure url ends with '/' */
	if( schemaLocation[ strlen( schemaLocation ) - 1 ] != '/' ) {
	    strcat( schemaLocation, "/" );
	}
	
	fprint(f, "<?xml version=\"1.0\"?>\n"); 
	fprint(f, "<!-- This module has been generated by mibdump "
	       SMI_VERSION_STRING ". Do not edit. -->\n");
	fprint(f, "\n");

	fprintModule(f, modv[i]);

	/* delete type-prefix-mapping */
    }

    if (output) {
	fclose(f);
    }
}



void initXml()
{

    static MibdumpDriverOption opt[] = {
	{ "schema-url", OPT_STRING, &schemaLocation, 0,
	  "The URL where the imported MIB modules reside" },
	{ "host", OPT_STRING, &peername, 0,
	  "The name of the host to get the data from" },
	{ "community", OPT_STRING, &community, 0,
	  "The snmp community string" },
	{ 0, OPT_END, 0, 0 }
    };
    
    static MibdumpDriver driver = {
	"xml",
	dumpXml,
	0,
	MIBDUMP_DRIVER_CANT_UNITE,
	"XML schema definitions",
	opt,
	NULL
    };
    
    mibdumpRegisterDriver(&driver);
}
