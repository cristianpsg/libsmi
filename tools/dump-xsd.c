/*
 * dump-xsd.c --
 *
 *      Operations to dump SMIng module information as XML schemas.
 *
 * Copyright (c) 2001 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-xsd.c,v 1.5 2002/01/29 19:01:28 tklie Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "smi.h"
#include "smidump.h"



#define  INDENT		2    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	4    /* column to start multiline texts */
#define  INDENTMAX	64   /* max column to fill, break lines otherwise */

/* xxx let the user configure this with command-line args */
#define DXSD_MIBPREFIX "if"
#define DXSD_SCHEMALOCATION "http://www.ibr.cs.tu-bs.de/~tklie/"

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


static int current_column = 0;



static char *getStringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<UNKNOWN>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "OctetString" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectIdentifier" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "Unsigned32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "Integer32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "Unsigned64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "Integer64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "Float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "Float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "Float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "Enumeration" :
        (basetype == SMI_BASETYPE_BITS)              ? "Bits" :
                                                   "<unknown>";
}

static char*
getStringStatus(SmiStatus status)
{
    char *statStr;
    
    switch( status ) {
    case SMI_STATUS_CURRENT:
	statStr = "current";
	break;
    case SMI_STATUS_DEPRECATED:
	statStr = "deprecated";
	break;
    case SMI_STATUS_OBSOLETE:
	statStr = "obsolete";
	break;
    case SMI_STATUS_MANDATORY:
	statStr = "mandatory";
	break;
    case SMI_STATUS_OPTIONAL:
	statStr = "optional";
	break;
    case SMI_STATUS_UNKNOWN:
    default:
	statStr = "unknown";
	break;
    }
    return statStr;
}

static char*
getStringAccess( SmiAccess smiAccess )
{
    switch( smiAccess ) {
    case SMI_ACCESS_NOT_IMPLEMENTED: return "not-implemented";
    case SMI_ACCESS_NOT_ACCESSIBLE : return "not-accessible";
    case SMI_ACCESS_NOTIFY         : return "notify";
    case SMI_ACCESS_READ_ONLY      : return "read-only";
    case SMI_ACCESS_READ_WRITE     : return "read-write";
    case SMI_ACCESS_UNKNOWN:
    default: return "unknown";
    }
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



static void fprintMultilineString(FILE *f, int column, const char *s)
{
    int i, j, len;

#ifdef INDENTTEXTS
    fprintSegment(f, column + INDENTTEXTS, "", 0);
#endif
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
#endif
	    }
	}
    }
    current_column++;
}



static void fprintDocumentation(FILE *f, int indent, const char *description)
{
    if (description) {
	fprintSegment(f, indent, "<xsd:documentation xml:lang=\"en\">\n", 0);
	fprintMultilineString(f, indent, description);
	fprint(f, "\n");
	fprintSegment(f, indent, "</xsd:documentation>\n", 0);
    }
}

static void fprintImportedTypes( FILE *f, SmiModule *smiModule )
{
    /* xxx */
}

static void fprintAnnotationElem( FILE *f, int indent, SmiNode *smiNode ) {
    char *buf;
    
    fprintSegment( f, indent, "<xsd:annotation>\n", 0 );
    fprintSegment( f, indent + INDENT, "<xsd:appinfo>\n", 0 );
    
    buf = malloc( 25 + strlen( getStringAccess( smiNode->access ) ) );
    if( buf ) {
	sprintf( buf, "<maxAccess>%s</maxAccess>\n",
		 getStringAccess( smiNode->access ) );
	fprintSegment( f, indent + 2 * INDENT, buf, 0 );
	free( buf );
    }
    
    buf = malloc( 20 +
		  strlen( getStringStatus( smiNode->status ) ) );
    if( buf ) {
	sprintf( buf, "<status>%s</status>\n",
		 getStringStatus( smiNode->status ) );
	fprintSegment( f, indent + 2 * INDENT, buf,  0 );
	free( buf );
    }
    
    fprintSegment( f, indent +  INDENT, "</xsd:appinfo>\n", 0 );
    fprintDocumentation( f, indent + INDENT, smiNode->description );
    fprintSegment( f, indent, "</xsd:annotation>\n", 0 );
    
}


static void fprintElement( FILE *f, int indent,
			   SmiNode *smiNode, const char *prefix )
{
    switch( smiNode->nodekind ) {
	char * buf;
	SmiNode *iterNode;
	SmiType *smiType;
	SmiElement *iterElem;

    case SMI_NODEKIND_NODE :
	buf = malloc( 37 + 2 * strlen( smiNode->name ) +
			    strlen( prefix ) );
	if( buf ) {
	    sprintf( buf, "<xsd:element name=\"%s\" type=\"%s:%sType\"/>\n",
		     smiNode->name, prefix, smiNode->name );
	    fprintSegment( f, indent, buf, 0 );
	    free( buf );
	}
	break;

    case SMI_NODEKIND_TABLE :
	/* ignore tables and just include their entries */
	for( iterNode = smiGetFirstChildNode( smiNode );
	     iterNode;
	     iterNode = smiGetNextChildNode( iterNode ) ) {
	    fprintElement( f, indent, iterNode, DXSD_MIBPREFIX );
	}
	break;

    case SMI_NODEKIND_ROW :
	
	buf = malloc( 71 + 2 *strlen( smiNode->name ) + strlen( prefix ) );
	if( buf ) {
	    sprintf( buf,
		     "<xsd:element name=\"%s\" type=\"%s:%sType\" minOccurs=\"0\" maxOccurs=\"unbounded\">\n",
		     smiNode->name, prefix, smiNode->name );
	    fprintSegment( f, indent, buf, 0 );
	    free( buf );
	}
	fprintAnnotationElem( f, indent + INDENT, smiNode );
	fprintSegment( f, indent, "</xsd:element>\n", 0 );
	break;
	
    case SMI_NODEKIND_SCALAR :
    case SMI_NODEKIND_COLUMN:
	/* check if we are index column */
	for( iterElem = smiGetFirstElement( smiGetParentNode( smiNode ) ) ;
	     iterElem;
	     iterElem = smiGetNextElement( iterElem ) ) {
	    if( smiNode == smiGetElementNode( iterElem ) ) {
		/* we are index coulumn ==> do not print element */
		return;
	    }
	}
	smiType = smiGetNodeType( smiNode );
	buf = malloc( 47 + strlen( smiNode->name ) + strlen( prefix ) +
		      strlen( getStringBasetype( smiType->basetype ) ) );
	if( buf ) {
	    sprintf( buf,
		     "<xsd:element name=\"%s\" type=\"%s:%s\" minOccurs=\"0\">\n",
		     smiNode->name, prefix,
		     getStringBasetype( smiType->basetype ) );
	    fprintSegment( f, indent, buf, 0 );
	    free( buf );
	}
	fprintAnnotationElem( f, indent + INDENT, smiNode );
	fprintSegment( f, indent, "</xsd:element>\n", 0 ); 
	break;
	
    }
}


static void fprintIndex( FILE *f, int indent,
			 SmiNode *smiNode, const char *prefix )
{
    SmiNode *iterNode;
    SmiElement *iterElem;
    char *buf;
    SmiType *smiType;

    for( iterElem = smiGetFirstElement( smiNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	iterNode = smiGetElementNode( iterElem );
	smiType = smiGetNodeType( iterNode );
	buf = malloc( 47 + strlen( iterNode->name ) + strlen( prefix ) +
		      strlen( getStringBasetype( smiType->basetype ) ) );
	if( buf ) {
	    sprintf( buf,
		     "<xsd:attribute  name=\"%s\" type=\"%s:%s\">\n",
		     iterNode->name, prefix,
		     getStringBasetype( smiType->basetype ) );
	    fprintSegment( f, indent, buf, 0 );
	    free( buf );
	}
	fprintAnnotationElem( f, indent + INDENT, smiNode );
	fprintSegment( f, indent, "</xsd:attribute>\n", 0 ); 
	break;
    }

    
}

static void fprintComplexType( FILE *f, int indent,
			       SmiNode *smiNode, const char *prefix )
{
    char *buf = malloc( 31 + strlen( smiNode->name ) );

    if( buf ) {
	SmiNode *iterNode;
		
	sprintf( buf, "<xsd:complexType name=\"%sType\">\n", smiNode->name );
	fprintSegment( f, indent, buf, 0 );
	free( buf );
	
	
	fprintSegment( f, indent + INDENT, "<xsd:sequence>\n", 0 );
	
	/* print child elements */
	for( iterNode = smiGetFirstChildNode( smiNode );
	     iterNode;
	     iterNode = smiGetNextChildNode( iterNode ) ) {

	    fprintElement( f, indent + 2 * INDENT, iterNode, prefix );
	    
	}
	fprintSegment( f, indent + INDENT, "</xsd:sequence>\n", 0 );
	fprintIndex( f, indent + INDENT, smiNode, prefix );
	
	
	fprintSegment( f, indent, "</xsd:complexType>\n\n", 0 );
	
    }
}

static void fprintNode( FILE *f, SmiNode *smiNode )
{

    fprintElement( f, INDENT, smiNode, DXSD_MIBPREFIX );
    fprint( f, "\n" );
    fprintComplexType( f, INDENT, smiNode, DXSD_MIBPREFIX );
    
}

static void fprintNodes(FILE *f, SmiModule *smiModule)
{
    SmiNode *iterNode;

    fprintElement(f, INDENT, smiModule, DXSD_MIBPREFIX);

    
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	fprintNode( f, iterNode );
    }
    
}


static void fprintRows( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;

    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_ROW ) ) {
	fprintComplexType( f, INDENT, iterNode, DXSD_MIBPREFIX );
    }
}

    
static void fprintModule(FILE *f, SmiModule *smiModule)
{
    fprintNodes(f, smiModule);
    fprintRows(f, smiModule);
    fprintSegment(f, INDENT, "<xsd:annotation>\n", 0);
    fprintDocumentation(f, 2 * INDENT, smiModule->description);
    fprintSegment(f, INDENT, "</xsd:annotation>\n", 0);
}


static void fprintRestriction(FILE *f, int indent, SmiType *smiType)
{
    SmiRange *smiRange;
    SmiValue *minRange, *maxRange;
    char *buf, *val;
    
    fprintSegment(f, indent, "<xsd:restriction", 0);
    fprint(f, " base=\"%s%s\">\n", DXSD_MIBPREFIX,
	   getStringBasetype(smiType->basetype));

    /* print ranges etc. */
    switch( smiType->basetype ) {

    case SMI_BASETYPE_INTEGER32:
    {
	int min, max;

	min = -2147483648;
	max = 2147483647;
	
	smiRange = smiGetFirstRange( smiType );
	while( smiRange ) {
	    if( min == -2147483648 ||
		smiRange->minValue.value.integer32 < min ) {
		min = smiRange->minValue.value.integer32;
	    }
	    if( max == 2147483647 ||
		smiRange->maxValue.value.integer32 > max ) {
		max = smiRange->maxValue.value.integer32;
	    }
	    smiRange = smiGetNextRange( smiRange );
	}
	
	buf = malloc( 42 );
	if( buf ) {
	    sprintf( buf, "<xsd:minInclusive value=\"%d\"/>\n", min );
	    fprintSegment( f, indent + INDENT, buf, 0 );
	    free( buf );
	}
	buf = malloc( 42 );
	if( buf ) {
	    sprintf( buf, "<xsd:maxInclusive value=\"%d\"/>\n",	max );
	    fprintSegment( f, indent + INDENT, buf, 0 );
	    free( buf );
	}
	
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	break;
    }

    case SMI_BASETYPE_OCTETSTRING:
    {
	SmiInteger32  minLength, maxLength;

	minLength = 0;
	maxLength = -1;

	smiRange = smiGetFirstRange( smiType );
	while( smiRange ) {
	    if( minLength == 0 ||
		smiRange->minValue.value.integer32 < minLength ) {
		minLength = smiRange->minValue.value.integer32;	     
	    }
	    if( smiRange->maxValue.value.integer32 > maxLength ) {
		maxLength = smiRange->maxValue.value.integer32;
	    }
	    smiRange = smiGetNextRange( smiRange );
	}

	if( minLength > 0 ) {
	    buf = malloc( 40 );
	    if( buf ) {
		sprintf( buf, "<xsd:minLength value=\"%d\"/>\n>", minLength );
		fprintSegment( f, indent + INDENT, buf, 0 );
		free( buf );
	    }
	}
	if( maxLength > -1 ) {
	    buf = malloc( 40 );
	    if( buf ) {
		sprintf( buf, "<xsd:maxLength value=\"%d\"/>\n>", maxLength );
		fprintSegment( f, indent + INDENT, buf, 0 );
		free( buf );
	    }
	}
    }

    case SMI_BASETYPE_FLOAT128:
    {
	SmiFloat128 min, max;

	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_FLOAT64:
    {
	SmiFloat64 min,max;

	/* xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_FLOAT32:
    {
	SmiFloat32 min,max;

	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_INTEGER64:
    {
	SmiInteger64 min,max;

	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_UNSIGNED64:
    {
	SmiUnsigned64 min, max;

	min = 0;
	max = 18446744073709551615;

	smiRange = smiGetFirstRange( smiType );
	while( smiRange ) {
	    if( smiRange->minValue.value.unsigned64 < min ) {
		min = smiRange->minValue.value.unsigned64;
	    }
	    if( smiRange->maxValue.value.unsigned32 > max ) {
		max = smiRange->maxValue.value.unsigned64;
	    }
	    smiRange = smiGetNextRange( smiRange );
	}
	
	buf = malloc( 42 );
	if( buf ) {
	    sprintf( buf, "<xsd:minInclusive value=\"%lu\"/>\n", min );
	    fprintSegment( f, indent + INDENT, buf, 0 );
	    free( buf );
	}
	buf = malloc( 42 );
	if( buf ) {
	    sprintf( buf, "<xsd:maxInclusive value=\"%lu\"/>\n",max );
	    fprintSegment( f, indent + INDENT, buf, 0 );
	    free( buf );
	}
	
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	
	break;
    }

    case SMI_BASETYPE_UNSIGNED32:
    {
	SmiUnsigned32 min, max;

	min = 0;
	max = 4294967295;

	smiRange = smiGetFirstRange( smiType );
	while( smiRange ) {
	    if( smiRange->minValue.value.unsigned32 < min ) {
		min = smiRange->minValue.value.unsigned32;
	    }
	    if( smiRange->maxValue.value.unsigned32 > max ) {
		max = smiRange->maxValue.value.unsigned32;
	    }
	    smiRange = smiGetNextRange( smiRange );
	}
	
	buf = malloc( 42 );
	if( buf ) {
	    sprintf( buf, "<xsd:minInclusive value=\"%d\"/>\n", min );
	    fprintSegment( f, indent + INDENT, buf, 0 );
	    free( buf );
	}
	buf = malloc( 42 );
	if( buf ) {
	    sprintf( buf, "<xsd:maxInclusive value=\"%d\"/>\n",max );
	    fprintSegment( f, indent + INDENT, buf, 0 );
	    free( buf );
	}
	
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	break;
    }
    }
}



static void fprintTypedef(FILE *f, int indent, SmiType *smiType)
{
    fprintSegment(f, indent, "<xsd:simpleType", 0);
    if (smiType->name) {
	fprint(f, " name=\"%s\"", smiType->name);
    }
    fprint(f, ">\n");

    fprintRestriction(f, indent + INDENT, smiType);
    fprintDocumentation(f, indent + INDENT, smiType->description);
    
    fprintSegment(f, indent, "</xsd:simpleType>\n", 0);
}



static void fprintTypedefs(FILE *f, SmiModule *smiModule)
{
    int		 i;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(smiModule);
	smiType;
	i++, smiType = smiGetNextType(smiType)) {
	fprint(f, "\n");
	fprintTypedef(f, INDENT, smiType);
    }
}



static void dumpXsd(int modc, SmiModule **modv, int flags, char *output)
{
    int  i;
    FILE *f = stdout;

    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

    for (i = 0; i < modc; i++) {
	char *buf;

	fprint(f, "<!-- This module has been generated by smidump "
	       SMI_VERSION_STRING ". Do not edit. -->\n");
	fprint(f, "\n");
	
	fprint(f, "<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\n");

	fprint(f, "            xmlns:%s=\"%s%s.xsd\"\n",
	       DXSD_MIBPREFIX, DXSD_SCHEMALOCATION,
	       modv[i]->name);
	fprint(f, "            targetNamespace=\"%s%s.xsd\">\n\n",
	       DXSD_SCHEMALOCATION, modv[i]->name);

	buf = malloc( 52 + strlen( DXSD_SCHEMALOCATION ) );
	if( buf ) {
	    sprintf( buf,
		     "<xsd:include id=\"smi\" schemaLocation=\"%ssmi.xsd\"/>\n\n",
		     DXSD_SCHEMALOCATION );
	    fprintSegment(f, INDENT, buf, 0);
	    free( buf );
	}
	
	fprintModule(f, modv[i]);
	fprintTypedefs(f, modv[i]);
	
	fprint(f, "\n</xsd:schema>\n");
    }

    if (output) {
	fclose(f);
    }
}



void initXsd()
{
    
    static SmidumpDriver driver = {
	"xsd",
	dumpXsd,
	0,
	SMIDUMP_DRIVER_CANT_UNITE,
	"XML schema definitions",
	NULL,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}
