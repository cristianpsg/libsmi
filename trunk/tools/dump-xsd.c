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
 * @(#) $Id: dump-xsd.c,v 1.12 2002/02/26 14:34:08 tklie Exp $
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

typedef struct TypePrefix {
    char *type;
    char *prefix;
} TypePrefix;

static TypePrefix *typePrefixes;
static int numTypes;

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
	fprintSegment(f, indent, "<xsd:documentation>\n", 0);
	fprintMultilineString(f, indent, description);
	fprint(f, "\n");
	fprintSegment(f, indent, "</xsd:documentation>\n", 0);
    }
}

static void fprintNamedNumber( FILE *f, int indent, SmiNamedNumber *nn )
{
    fprintSegment( f, indent, "<xsd:enumeration ", 0 );
    fprint( f, "value=\"%s\">\n", nn->name );
    fprintSegment( f, indent + INDENT, "<xsd:annotation>\n", 0 );
    fprintSegment( f, indent + 2 * INDENT, "<xsd:appinfo>\n", 0 );
    fprintSegment( f, indent + 3 * INDENT, "<intVal>", 0 );
    fprint( f, "%d</intVal>\n", nn->value.value.integer32 );
    fprintSegment( f, indent + 2 * INDENT, "</xsd:appinfo>\n", 0 );
    fprintSegment( f, indent + INDENT, "</xsd:annotation>\n", 0 );
    fprintSegment( f, indent, "</xsd:enumeration>\n", 0 );
}

static void fprintRestriction(FILE *f, int indent, SmiType *smiType)
{
    SmiRange *smiRange;

    /* print ranges etc. */
    switch( smiType->basetype ) {

    case SMI_BASETYPE_INTEGER32:
    {
	SmiInteger32 min, max;

	min = -2147483647;
	max = 2147483646;

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n", 
	       getStringBasetype(smiType->basetype));
	
	smiRange = smiGetFirstRange( smiType );
	while( smiRange ) {
	    if( min == -2147483647 ||
		smiRange->minValue.value.integer32 < min ) {
		min = smiRange->minValue.value.integer32;
	    }
	    if( max == 2147483646 ||
		smiRange->maxValue.value.integer32 > max ) {
		max = smiRange->maxValue.value.integer32;
	    }
	    smiRange = smiGetNextRange( smiRange );
	}
	
	fprintSegment( f, indent + INDENT, "<xsd:minInclusive", 0 );
	fprint( f, " value=\"%d\"/>\n", min );

	fprintSegment( f, indent + INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%d\"/>\n",	max );
	
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	break;
    }

    case SMI_BASETYPE_OCTETSTRING:
    {
	SmiInteger32  minLength, maxLength;

	minLength = 0;
	maxLength = -1;

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n",
	       getStringBasetype(smiType->basetype));

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
	    fprintSegment( f, indent + INDENT, "<xsd:minLength", 0 );
	    fprint( f, " value=\"%d\"/>\n", minLength );
	}
	if( maxLength > -1 ) {
	    fprintSegment( f, indent + INDENT, "<xsd:maxLength", 0 );
	    fprint( f, " value=\"%d\"/>\n", maxLength );
	}

	fprintSegment( f, indent, "</xsd:restriction>\n", 0 );
	
	break;
    }

    case SMI_BASETYPE_FLOAT128:
    {
/*	SmiFloat128 min, max; */

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n",
	       getStringBasetype(smiType->basetype));

	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_FLOAT64:
    {
/*	SmiFloat64 min,max;*/

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n",
	       getStringBasetype(smiType->basetype));
	
	/* xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_FLOAT32:
    {
/*	SmiFloat32 min,max;*/

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n",
	       getStringBasetype(smiType->basetype));

	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_INTEGER64:
    {
/*	SmiInteger64 min,max;*/

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n",
	       getStringBasetype(smiType->basetype));

	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_UNSIGNED64:
    {
	SmiUnsigned64 min, max;

	min = 0;
	max = 18446744073709551615;

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n", 
	       getStringBasetype(smiType->basetype));

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
	fprintSegment( f, indent + INDENT, "<xsd:minInclusive", 0 );
	fprint( f, " value=\"%lu\"/>\n", min );

	fprintSegment( f, indent + INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%lu\"/>\n",max );
	
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	
	break;
    }

    case SMI_BASETYPE_UNSIGNED32:
    {
	SmiUnsigned32 min, max;

	fprintSegment(f, indent, "<xsd:restriction", 0);
	fprint(f, " base=\"%s\">\n",
	       getStringBasetype(smiType->basetype));
	
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
	fprintSegment( f, indent + INDENT, "<xsd:minInclusive", 0 );
	fprint( f, " value=\"%d\"/>\n", min );

	fprintSegment( f, indent + INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%d\"/>\n",max );
	
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	break;
    }

    case SMI_BASETYPE_ENUM:
    case SMI_BASETYPE_BITS:
    {
	SmiNamedNumber *nn;
	
	fprintSegment(f, indent, "<xsd:restriction base=\"xsd:NMTOKEN\">\n",
		      0);

	/* iterate named numbers */
	for( nn = smiGetFirstNamedNumber( smiType );
	     nn;
	     nn = smiGetNextNamedNumber( nn ) ) {
	    fprintNamedNumber( f, indent + INDENT, nn );
	}
	fprintSegment(f, indent, "</xsd:restriction>\n", 0);
	break;
    }

    case SMI_BASETYPE_OBJECTIDENTIFIER:
	/* xxx */
    case SMI_BASETYPE_UNKNOWN:
	/* should not occur */
    }
}


static void fprintBitList( FILE *f, int indent, SmiType *smiType,
			   const char *name )
{
    fprintSegment( f, indent, "<xsd:simpleType", 0 );
    fprint( f, " name=\"%sBitList\">\n", name );
    fprintSegment( f, indent + INDENT, "<xsd:list", 0 );
    fprint( f, " itemType=\"%sBit\"/>\n", name );
    fprintSegment( f, indent, "</xsd:simpleType>\n\n", 0 );
    
    fprintSegment( f, indent, "<xsd:simpleType", 0 );
    fprint( f, " name=\"%sBits\">\n", name );
    fprintSegment( f, indent + INDENT, "<xsd:restriction", 0 );
    fprint( f, " base=\"%sBitList\">\n", name );
    fprintSegment( f, indent + 2 * INDENT, "<xsd:maxlength", 0 );
    fprint( f, " value=\"%d\"/>\n", smiType->value.len );
    fprintSegment( f, indent + INDENT, "</xsd:restriction>\n", 0 );
    fprintSegment( f, indent, "</xsd:simpleType>\n", 0 );
}


static void fprintTypedef(FILE *f, int indent, SmiType *smiType,
			  const char *name)
{
    fprintSegment(f, indent, "<xsd:simpleType", 0);
    if (smiType->name) {
	if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprint(f, " name=\"%sBit\"", name);
	}
	else {
	    fprint(f, " name=\"%s\"", name);
	}
    }
    fprint(f, ">\n");

    if( smiType->description ) {
	fprintSegment( f, indent + INDENT, "<xsd:annotation>\n", 0 );
	fprintDocumentation(f, indent + 2 * INDENT, smiType->description);
	fprintSegment( f, indent + INDENT, "</xsd:annotation>\n", 0 );
    }

    fprintRestriction(f, indent + INDENT, smiType);
  

    
    fprintSegment(f, indent, "</xsd:simpleType>\n", 0);

    if( smiType->basetype == SMI_BASETYPE_BITS ) {
	/* if basetype is BITS we have to do something more */
	fprintBitList( f, indent, smiType, name );
    }
}

#if 0
static void fprintImportedTypes( FILE *f, SmiModule *smiModule )
{
    SmiImport *iterImp;
    SmiType *impType;
    SmiModule *impModule; 
    
    fprintSegment( f, INDENT, "<!-- imported type(s) -->\n", 0 );

    /* iterate imports */
    for( iterImp = smiGetFirstImport( smiModule );
	 iterImp;
	 iterImp = smiGetNextImport( iterImp ) ) {
	if( iterImp->name && iterImp->module ) {

	    /* ignore standard imports */
	    if( strcmp( iterImp->module , "SNMPv2-SMI" ) &&
		strcmp( iterImp->module , "SNMPv2-TC" ) &&
		strcmp( iterImp->module , "SNMPv2-CONF" ) &&
		strcmp( iterImp->module , "SNMPv2-MIB" ) ) {

		/* try to get imported type */
		impModule = smiGetModule( iterImp->module );
		if( impModule ) {
		    impType = smiGetType( impModule, iterImp->name );
		    if( impType && impType->name) {
			fprintTypedef( f, INDENT, impType, impType->name );
			fprintf( f, "\n" );
		    }
		}
	    }
	}
    }
    /* xxx */
}
#endif /* 0 */

static char* getTypePrefix( SmiType *smiType )
{
    int i;

    if( !smiType->name ) {
	return NULL;
    }

    for( i = 0; i < numTypes; i++ ) {
	if( !strcmp( typePrefixes[ i ].type, smiType->name ) ) {
	    return typePrefixes[ i ].prefix;
	}
    }
    return NULL;
}

static void fprintAnnotationElem( FILE *f, int indent, SmiNode *smiNode ) {
      
    fprintSegment( f, indent, "<xsd:annotation>\n", 0 );
    fprintSegment( f, indent + INDENT, "<xsd:appinfo>\n", 0 );

    fprintSegment( f, indent + 2 * INDENT, "<maxAccess>", 0 );
    fprint( f, "%s</maxAccess>\n", getStringAccess( smiNode->access ) );

    fprintSegment( f, indent + 2 * INDENT, "<status>", 0 );
    fprint( f, "%s</status>\n",  getStringStatus( smiNode->status ) );
  
    fprintSegment( f, indent +  INDENT, "</xsd:appinfo>\n", 0 );
    fprintDocumentation( f, indent + INDENT, smiNode->description );
    fprintSegment( f, indent, "</xsd:annotation>\n", 0 );
    
}


static void fprintElement( FILE *f, int indent,
			   SmiNode *smiNode, const char *prefix )
{
    switch( smiNode->nodekind ) {
	SmiType *smiType;

    case SMI_NODEKIND_NODE :
	fprintSegment( f, indent, "<xsd:element", 0 );
	fprint( f, " name=\"%s\" type=\"%sType\"/>\n",
		smiNode->name, smiNode->name );
	break;

    case SMI_NODEKIND_TABLE :
    {
	SmiNode *iterNode;
	
	/* ignore tables and just include their entries */
	for( iterNode = smiGetFirstChildNode( smiNode );
	     iterNode;
	     iterNode = smiGetNextChildNode( iterNode ) ) {
	    fprintElement( f, indent, iterNode, DXSD_MIBPREFIX );
	}
	break;
    }

    case SMI_NODEKIND_ROW :
	fprintSegment( f, indent, "<xsd:element", 0 );
	fprint( f, " name=\"%s\"",  smiNode->name );
	fprint( f, " type=\"%sType\"", smiNode->name );
	fprint( f, " minOccurs=\"0\" maxOccurs=\"unbounded\">\n" );

	fprintAnnotationElem( f, indent + INDENT, smiNode );
	fprintSegment( f, indent, "</xsd:element>\n", 0 );
	break;
	
    case SMI_NODEKIND_SCALAR :
    case SMI_NODEKIND_COLUMN:
    {
	SmiElement *iterElem;
	char *prefix;
	char *typeName;
	
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
	prefix = getTypePrefix( smiType );
	
	fprintSegment( f, indent, "<xsd:element", 0 );
	fprint( f, " name=\"%s\"", smiNode->name );

	if( smiType->name ) {
	    typeName = smiType->name;
	}
	else {
	    typeName = getStringBasetype( smiType->basetype );
	}

	if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprint( f, " type=\"%%s%s\"",
		    smiNode->name,
		    getStringBasetype( smiType->basetype ) );
	    fprint( f, " minOccurs=\"0\">\n" );
	}
	
	else if( smiType->basetype == SMI_BASETYPE_ENUM &&
	    ! smiType->name ) {
	    fprint( f, "minOccurs=\"0>\n" );
	    fprintTypedef( f, indent + INDENT, smiType, NULL );
	}
	
	else {
	    SmiModule *typeModule = smiGetTypeModule( smiType );
	    if( prefix ) {
		fprint( f, " type=\"%s:%s\"", prefix, typeName );
	    }
	    else {
		fprint( f, " type=\"%s\"", typeName );
	    }
	    fprint( f, " minOccurs=\"0\">\n" );
	}
	
	
	fprintAnnotationElem( f, indent + INDENT, smiNode );
	fprintSegment( f, indent, "</xsd:element>\n", 0 ); 
	break;
    }

    default:
	fprint( f, "<!-- Warning! Unhandled Element! No details available!\n" );
	fprint( f, "      Nodekind: %#4x -->\n\n" );
	
    }
}


static void fprintIndex( FILE *f, int indent,
			 SmiNode *smiNode, const char *prefix )
{
    SmiNode *iterNode;
    SmiElement *iterElem;
    SmiType *smiType;

    for( iterElem = smiGetFirstElement( smiNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	iterNode = smiGetElementNode( iterElem );
	smiType = smiGetNodeType( iterNode );

	fprintSegment( f, indent, "<xsd:attribute", 0 );
	fprint( f, " name=\"%s\"", iterNode->name );
	fprint( f, " type=\"%s\">\n",
		getStringBasetype( smiType->basetype ) );
	
	fprintAnnotationElem( f, indent + INDENT, smiNode );
	fprintSegment( f, indent, "</xsd:attribute>\n", 0 ); 
	break;
    }

    
}

static void fprintComplexType( FILE *f, int indent,
			       SmiNode *smiNode, const char *prefix )
{
    SmiNode *iterNode;
    
    fprintSegment( f, indent, "<xsd:complexType", 0 );
    fprint( f, " name=\"%sType\">\n", smiNode->name );
    
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

#if 0
static void fprintNode( FILE *f, SmiNode *smiNode, int indent )
{
    fprintElement( f, indent, smiNode, DXSD_MIBPREFIX );
    fprint( f, "\n" );
    fprintComplexType( f, indent, smiNode, DXSD_MIBPREFIX );
}
#endif /* 0 */

static void fprintNodes(FILE *f, SmiModule *smiModule)
{
    SmiNode *iterNode;
    int oidlen = 99999999;

    /* print root node */
    fprintSegment( f, INDENT, "<xsd:element ", 0 );
    fprint( f, "name=\"%s\">\n", smiModule->name );
    fprintSegment( f, 2 * INDENT, "<xsd:complexType>\n", 0 );
    fprintSegment( f, 3* INDENT, "<xsd:sequence>\n", 0 );
    
    /* print child elements */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( oidlen >= iterNode->oidlen ) {
	    /* skip child nodes here */
	    oidlen = iterNode->oidlen;
	    fprintElement( f, 4 * INDENT, iterNode, DXSD_MIBPREFIX );
	}
    }
    
    fprintSegment( f, 3 * INDENT, "</xsd:sequence>\n", 0 );
    fprintSegment( f, 2 * INDENT, "</xsd:complexType>\n", 0 );
    fprintSegment( f, INDENT, "</xsd:element>\n\n", 0 );

    /* print typedefs for child elements */
    oidlen = 99999999;
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( oidlen >= iterNode->oidlen ) {
	    /* skip child nodes here */
	    oidlen = iterNode->oidlen;
	    fprintComplexType( f, INDENT, iterNode, DXSD_MIBPREFIX );
	}
    }
}

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


static void fprintRows( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;

    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    /* skip groups */
	    fprintComplexType( f, INDENT, iterNode, DXSD_MIBPREFIX );
	}
    }
}


static void fprintBits( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;
    SmiType *smiType;

    for(iterNode = smiGetFirstNode(smiModule,
				   SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN);
	iterNode;
	iterNode = smiGetNextNode(iterNode,
				  SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN)) {
	smiType = smiGetNodeType( iterNode );
	if( smiType ) {
	    if( ( smiType->basetype == SMI_BASETYPE_BITS ) &&
		( ! getTypePrefix( smiType ) ) ) {
		fprintTypedef( f, INDENT, smiType, iterNode->name );
	    }
	}
    }
    
}

    
static void fprintModule(FILE *f, SmiModule *smiModule)
{
    fprintSegment(f, INDENT, "<xsd:annotation>\n", 0);
    fprintDocumentation(f, 2 * INDENT, smiModule->description);
    fprintSegment(f, INDENT, "</xsd:annotation>\n\n", 0);
    
    /*fprintImportedTypes( f, smiModule );*/
    fprintNodes(f, smiModule);
    fprintRows(f, smiModule);
    fprintBits(f, smiModule);
}


static void fprintTypedefs(FILE *f, SmiModule *smiModule)
{
    int		 i;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(smiModule);
	smiType;
	i++, smiType = smiGetNextType(smiType)) {
	fprint(f, "\n");
	fprintTypedef(f, INDENT, smiType, smiType->name);
    }
}
#if 0
static void fprintImports( FILE *f, smiModule *smiModule )
{
    
}
#endif 0

static void registerType( char *type, char *module )
{
    if( typePrefixes ) {
	typePrefixes = realloc( typePrefixes,
				++numTypes * sizeof( TypePrefix ) );
    }
    else {
	/* typelist empty -->  create it */
	typePrefixes = malloc( sizeof( TypePrefix ) );
	numTypes = 1;
    }
    if( typePrefixes ) {
	typePrefixes[ numTypes - 1 ].type = type;
	typePrefixes[ numTypes - 1 ].prefix = module;
    }
}


static void fprintSchemaDef( FILE *f, SmiModule *smiModule )
{
    SmiImport *iterImp;
    SmiType *impType;
    SmiModule *impModule;
    char *lastModName;
    
    fprint(f,
	   "<xsd:schema targetNamespace=\"%s%s.xsd\"\n",
	   DXSD_SCHEMALOCATION, smiModule->name);
    
    fprint(f, "            xmlns=\"%s%s.xsd\"\n",
	   DXSD_SCHEMALOCATION, smiModule->name);
    fprint(f, "            xmlns:xml=\"http://www.w3.org/XML/1998/namespace\"\n");
    fprint(f, "            xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\n");
    
    for( iterImp = smiGetFirstImport( smiModule );
	 iterImp;
	 iterImp = smiGetNextImport( iterImp ) ) {
	registerType( iterImp->name, iterImp->module );
	/* assume imports to be ordered by module names */
	if( strcmp( iterImp->module, lastModName ) ) {
	    fprint( f, "            xmlns:%s=\"%s%s.xsd\"\n",
		    iterImp->module, DXSD_SCHEMALOCATION, iterImp->module );
	}
	lastModName = iterImp->module;
    }
  
    fprint(f, "            xml:lang=\"en\"\n");
    fprint(f, "            xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
    fprint(f, "            xsi:schemaLocation=\"http://www.w3.org/2001/XMLSchema http://www.w3.org/2001/XMLSchema.xsd\"\n");
    fprint(f, "            elementFormDefault=\"qualified\"\n");
    fprint(f, "            attributeFormDefault=\"unqualified\">\n\n");
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
	fprint(f, "<?xml version=\"1.0\"?>\n"); 
	fprint(f, "<!-- This module has been generated by smidump "
	       SMI_VERSION_STRING ". Do not edit. -->\n");
	fprint(f, "\n");

	fprintSchemaDef(f, modv[i]);	

        /*
	fprintSegment(f, INDENT, "<xsd:include", 0 );
	fprint( f, " id=\"smi\" schemaLocation=\"%ssmi.xsd\"/>\n\n",
		DXSD_SCHEMALOCATION );
	*/
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
