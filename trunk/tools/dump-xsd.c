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
 * @(#) $Id: dump-xsd.c,v 1.22 2002/04/08 17:29:07 tklie Exp $
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

static char *schemaLocation = "http://www.ibr.cs.tu-bs.de/~tklie/";

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
    struct TypePrefix *next;
} TypePrefix;

static TypePrefix *typePrefixes = NULL;


/* some forward declarations */
static void fprintElement( FILE *f, int indent,
			   SmiNode *smiNode, const char *prefix );

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


static void fprintStdRestHead( FILE *f, int indent, SmiType *smiType )
{
    char *baseTypeName = getStringBasetype(smiType->basetype);
    char *prefix = getTypePrefix( baseTypeName );
    
    fprintSegment(f, indent, "<xsd:restriction", 0);
    if( prefix ) {
	fprint(f, " base=\"%s:%s\">\n", prefix, baseTypeName );
    }
    else {
	fprint(f, " base=\"%s\">\n", baseTypeName );
    }
}


static void fprintRestriction(FILE *f, int indent, SmiType *smiType)
{
    SmiRange *smiRange;
    char *prefix = getTypePrefix( smiType->name );
    
    /* print ranges etc. */
    switch( smiType->basetype ) {

    case SMI_BASETYPE_INTEGER32:
    {
	SmiInteger32 min = -2147483647, max = -2147483647;
	
	fprintStdRestHead( f, indent, smiType );
	
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

	fprintStdRestHead( f, indent, smiType );
	
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
	fprintStdRestHead( f, indent, smiType );
	
	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_FLOAT64:
    {
/*	SmiFloat64 min,max;*/
	fprintStdRestHead( f, indent, smiType );

	/* xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_FLOAT32:
    {
/*	SmiFloat32 min,max;*/
	fprintStdRestHead( f, indent,  smiType );
	
	/* xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_INTEGER64:
    {
/*	SmiInteger64 min,max;*/
	fprintStdRestHead( f, indent, smiType );
	
	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_UNSIGNED64:
    {
	SmiUnsigned64 min, max;

	min = 0;
	max = 18446744073709551615U;

	fprintStdRestHead( f, indent, smiType );
	
	smiRange = smiGetFirstRange( smiType );
	while( smiRange ) {
	    if( smiRange->minValue.value.unsigned64 < min ) {
		min = smiRange->minValue.value.unsigned64;
	    }
	    if( smiRange->maxValue.value.unsigned64 > max ) {
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

	min = 0;
	max = 4294967295UL;

	fprintStdRestHead( f, indent, smiType );
	
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
	fprint( f, " value=\"%u\"/>\n", min );

	fprintSegment( f, indent + INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%u\"/>\n",max );
	
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
	fprintSegment( f, indent,
		       "<xsd:restriction base=\"smi:ObjectIdentifier\"/>\n", 0 );
	break;
    case SMI_BASETYPE_UNKNOWN:
	/* should not occur */
	break;
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

static int getNumSubRanges( SmiType *smiType )
{
    SmiRange *smiRange;
    int num = 0;

    for( smiRange = smiGetFirstRange( smiType );
	 smiRange;
	 smiRange = smiGetNextRange( smiRange ) ) {
	num++;
    }
    
    return num;
}

static void fprintSubRangeType( FILE *f, int indent,
				SmiRange *smiRange, SmiType *smiType, int num )
{
    fprintSegment( f, indent, "<xsd:simpleType ", 0 );
    fprint( f, "name=\"%sRange%dType\">\n", smiType->name, num );

    fprintStdRestHead( f, indent + INDENT, smiType );

    switch( smiType->basetype ) {

    case SMI_BASETYPE_UNSIGNED32: {
	SmiUnsigned32 min, max;

	min = 0;
	max = 4294967295UL;

	if( smiRange->minValue.value.unsigned32 < min ) {
	    min = smiRange->minValue.value.unsigned32;
	}
	if( smiRange->maxValue.value.unsigned32 > max ) {
	    max = smiRange->maxValue.value.unsigned32;
	}
	
	fprintSegment( f, indent + 2 * INDENT, "<xsd:minInclusive", 0 );
	fprint( f, " value=\"%u\"/>\n", min );
	
	fprintSegment( f, indent + 2 * INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%u\"/>\n",max );

	break;
    }

    case SMI_BASETYPE_INTEGER32: {
	SmiInteger32 min, max;

	min = -2147483647;
	max = 2147483646;

	if( min == -2147483647 ||
	    smiRange->minValue.value.integer32 < min ) {
	    min = smiRange->minValue.value.integer32;
	}
	if( max == 2147483646 ||
	    smiRange->maxValue.value.integer32 > max ) {
	    max = smiRange->maxValue.value.integer32;
	}
	smiRange = smiGetNextRange( smiRange );
	
	fprintSegment( f, indent + 2 * INDENT, "<xsd:minInclusive", 0 );
	fprint( f, " value=\"%d\"/>\n", min );

	fprintSegment( f, indent + 2 * INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%d\"/>\n",	max );
	
	break;
	
    }

    case SMI_BASETYPE_OCTETSTRING: {
	SmiInteger32  minLength, maxLength;
	
	minLength = 0;
	maxLength = -1;
	
	if( minLength == 0 ||
	    smiRange->minValue.value.integer32 < minLength ) {
	    minLength = smiRange->minValue.value.integer32;	     
	}
	if( smiRange->maxValue.value.integer32 > maxLength ) {
	    maxLength = smiRange->maxValue.value.integer32;
	}
	
	if( minLength > 0 ) {
	    fprintSegment( f, indent + 2 * INDENT, "<xsd:minLength", 0 );
	    fprint( f, " value=\"%d\"/>\n", minLength );
	}
	if( maxLength > -1 ) {
	    fprintSegment( f, indent + 2 * INDENT, "<xsd:maxLength", 0 );
	    fprint( f, " value=\"%d\"/>\n", maxLength );
	}
	
	break;
    }

    case SMI_BASETYPE_FLOAT128:
    {
/*	SmiFloat128 min, max; 
	xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_FLOAT64:
    {
/*	SmiFloat64 min,max;
	xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_FLOAT32:
    {
/*	SmiFloat32 min,max;
	xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_INTEGER64:
    {
/*	SmiInteger64 min,max;
	 xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_UNSIGNED64:
    {
	SmiUnsigned64 min, max;

	min = 0;
	max = 18446744073709551615U;

	if( smiRange->minValue.value.unsigned64 < min ) {
	    min = smiRange->minValue.value.unsigned64;
	}
	if( smiRange->maxValue.value.unsigned32 > max ) {
	    max = smiRange->maxValue.value.unsigned64;
	}
	
	fprintSegment( f, indent + 2 * INDENT, "<xsd:minInclusive", 0 );
	fprint( f, " value=\"%lu\"/>\n", min );

	fprintSegment( f, indent + 2 * INDENT, "<xsd:maxInclusive", 0 );
	fprint( f, " value=\"%lu\"/>\n",max );
	
	break;
    }

    case SMI_BASETYPE_ENUM:
    case SMI_BASETYPE_BITS:
    case SMI_BASETYPE_OBJECTIDENTIFIER:
    case SMI_BASETYPE_UNKNOWN:
	/* should not occur */
	break;
	
    }
    
    fprintSegment(f, indent + INDENT, "</xsd:restriction>\n", 0);
    fprintSegment(f, indent, "</xsd:simpleType>\n\n", 0 );
}

static void fprintDisplayHint( FILE *f, int indent, char *format )
{
    fprintSegment( f, indent, "<displayHint>", 0 );
    fprint( f, "%s</displayHint>\n", format );
}

static void fprintTypedef(FILE *f, int indent, SmiType *smiType,
			  const char *name)
{
    int i;
    SmiRange *smiRange;
    int numSubRanges = getNumSubRanges( smiType );
    char *prefix = getTypePrefix( name );

    if( numSubRanges > 1 ) {
	/* print out simple types for all subranges */
	i = 0;
	for( smiRange = smiGetFirstRange( smiType );
	     smiRange;
	     smiRange = smiGetNextRange( smiRange ) ) {
	    fprintSubRangeType( f, indent, smiRange, smiType, i++ );
	}
    }
   
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
	if( smiType->format ) {
	    fprintSegment( f, indent + 2 * INDENT, "<xsd:appinfo>\n", 0 );
	    fprintDisplayHint( f, indent + 3 * INDENT, smiType->format );
	    fprintSegment( f, indent + 2 * INDENT, "</xsd:appinfo>\n", 0 );
	}
	fprintSegment( f, indent + INDENT, "</xsd:annotation>\n", 0 );
    }

    if( numSubRanges > 1 ) {
	fprintSegment( f, indent + INDENT, "<xsd:union membertypes=\" ", 0 );
	i = 0;
	for( smiRange = smiGetFirstRange( smiType );
	     smiRange;
	     smiRange = smiGetNextRange( smiRange ) ) {
	    fprint( f, "%sRange%dType ", smiType->name, i++ ); 
	}
	fprint( f, "\"/>\n" );
    }
    else {
	/* we do not have more than one subrange */
	fprintRestriction(f, indent + INDENT, smiType);
    }
  
    fprintSegment(f, indent, "</xsd:simpleType>\n", 0);

    if( smiType->basetype == SMI_BASETYPE_BITS ) {
	/* if basetype is BITS we have to do something more */
	fprintBitList( f, indent, smiType, name );
    }
}

static char* getTypePrefix( char *typeName )
{
    int i;
    TypePrefix *iterTPr;

    if( !typeName ) {
	return NULL;
    }

    for( iterTPr = typePrefixes; iterTPr; iterTPr = iterTPr->next ) {
	if( ! strcmp( iterTPr->type, typeName ) ) {
	    return iterTPr->prefix;
	}
    }
    /*
    for( i = 0; i < numTypes; i++ ) {
	if( !strcmp( typePrefixes[ i ].type, typeName ) ) {
	    return typePrefixes[ i ].prefix;
	}
    }
    */
    return NULL;
}


static void fprintAnnotationElem( FILE *f, int indent, SmiNode *smiNode ) {
    int i;
    
    fprintSegment( f, indent, "<xsd:annotation>\n", 0 );
    fprintSegment( f, indent + INDENT, "<xsd:appinfo>\n", 0 );

    fprintSegment( f, indent + 2 * INDENT, "<maxAccess>", 0 );
    fprint( f, "%s</maxAccess>\n", getStringAccess( smiNode->access ) );

    fprintSegment( f, indent + 2 * INDENT, "<status>", 0 );
    fprint( f, "%s</status>\n",  getStringStatus( smiNode->status ) );

    fprintSegment( f, indent + 2 * INDENT, "<oid>", 0 );
    for (i = 0; i < smiNode->oidlen; i++) {
	fprint(f, i ? ".%u" : "%u", smiNode->oid[i]);
    }
    fprint( f, "</oid>\n" );

    if( smiNode->format ) {
	fprintDisplayHint( f, indent + 2 * INDENT, smiNode->format );
    }
  
    fprintSegment( f, indent +  INDENT, "</xsd:appinfo>\n", 0 );
    fprintDocumentation( f, indent + INDENT, smiNode->description );
    fprintSegment( f, indent, "</xsd:annotation>\n", 0 );
    
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

static void fprintIndexAttr( FILE *f, int indent, SmiNode *smiNode,
			     SmiNode *augments )
{
    char *typeName, *prefix;
    SmiType *smiType;

    smiType = smiGetNodeType( smiNode );
    if( !smiType ) {
/*	fprint( f, "<!-- error: no type in %s -->\n", smiNode->name );*/
	return;
    }
    
    typeName = smiType->name ?
	smiType->name :
	getStringBasetype( smiType->basetype );
    prefix = getTypePrefix( typeName );
    
    fprintSegment( f, indent, "<xsd:attribute", 0 );
    fprint( f, " name=\"%s\"", smiNode->name );
    if( prefix ) {
	fprint( f, " type=\"%s:%s\">\n", prefix, typeName );
    }
    else {
	fprint( f, " type=\"%s\">\n", typeName );
    }
        
    if( augments ) {
	fprintSegment( f, indent + INDENT, "<appinfo>\n", 0 );
	fprintSegment( f, indent + 2 * INDENT, "<augments>", 0 );
	fprint( f, "%s</augments>\n", augments->name );
	fprintSegment( f, indent + INDENT, "</appinfo>", 0 );
    }
    else {
	fprintAnnotationElem( f, indent + INDENT, smiNode );
    }
    fprintSegment( f, indent, "</xsd:attribute>\n", 0 ); 
}



static void fprintIndex( FILE *f, int indent, SmiNode *smiNode,
			 SmiNode *augments )
{
    SmiNode *iterNode;
    SmiElement *iterElem;
  
    /* iterate INDEX columns */
    for( iterElem = smiGetFirstElement( smiNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	iterNode = smiGetElementNode( iterElem );
	fprintIndexAttr( f, indent, iterNode, augments );
    }

    /* print AUGMENTS-clause */
    iterNode = smiGetRelatedNode( smiNode );
    if( iterNode ) {
	fprintIndex( f, indent, iterNode, iterNode );
    }
}


static void fprintComplexType( FILE *f, int indent,
			       SmiNode *smiNode,
			       const char *prefix, const char *name )
{
    SmiNode *iterNode;
    int numChildren;
    
    fprintSegment( f, indent, "<xsd:complexType", 0 );
    if( name ) {
	fprint( f, " name=\"%sType\">\n", smiNode->name );
    }
    else {
	fprint( f, ">\n" );
    }

    numChildren = hasChildren( smiNode, SMI_NODEKIND_ANY );
    if( numChildren ) {
	fprintSegment( f, indent + INDENT, "<xsd:sequence>\n", 0 );
    }
    /* print child elements */
    for( iterNode = smiGetFirstChildNode( smiNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ) {
	
	fprintElement( f, indent + 2 * INDENT, iterNode, prefix );
	
    }
    if( numChildren ) {
	fprintSegment( f, indent + INDENT, "</xsd:sequence>\n", 0 );
    }
    fprintIndex( f, indent + INDENT, smiNode, NULL );
    
    fprintSegment( f, indent, "</xsd:complexType>\n", 0 );
    if( name ) {
	/* we are printing out a global type,
	   so let's leave a blank line after it. */
	fprint( f, "\n" );
    }

    for( iterNode = smiGetFirstChildNode( smiNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ) {
	if( iterNode->nodekind == SMI_NODEKIND_NODE ) {
	    fprintComplexType( f, indent, iterNode, NULL, iterNode->name );
	}
    }
}


static void fprintElement( FILE *f, int indent,
			   SmiNode *smiNode, const char *prefix )
{
    switch( smiNode->nodekind ) {
	SmiType *smiType;

    case SMI_NODEKIND_NODE :
	{
	    SmiNode *iterNode;
	    int moreIndent;
	    int numScalars = hasChildren( smiNode, SMI_NODEKIND_SCALAR );

	    moreIndent = numScalars > 1 ? INDENT : 0;
	    fprintSegment( f, indent, "<xsd:element", 0 );
	    fprint( f, " name=\"%s\">\n", smiNode->name );
	    fprintSegment( f, indent + INDENT, "<xsd:complexType>\n", 0 );
	    if( numScalars > 1 ) {
		fprintSegment( f, indent + 2 * INDENT, "<xsd:sequence>\n", 0 );
	    }
	    for( iterNode = smiGetFirstChildNode( smiNode );
		 iterNode;
		 iterNode = smiGetNextChildNode( iterNode ) ) {
		if( iterNode->nodekind == SMI_NODEKIND_SCALAR ) {
		    fprintElement( f, indent + moreIndent + 2 * INDENT,
				   iterNode, DXSD_MIBPREFIX );
		}
	    }
	    if( numScalars > 1 ) {
		fprintSegment( f, indent + 2 * INDENT, "</xsd:sequence>\n",
			       0 );
	    }
	    fprintSegment( f, indent + INDENT, "</xsd:complexType>\n", 0 );
	    fprintSegment( f, indent, "</xsd:element>\n", 0 );
	}
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
	fprint( f, " minOccurs=\"0\" maxOccurs=\"unbounded\">\n" );

	fprintAnnotationElem( f, indent + INDENT, smiNode );

	fprintComplexType( f, indent + INDENT, smiNode,
			   DXSD_MIBPREFIX, NULL );
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

	
	fprintSegment( f, indent, "<xsd:element", 0 );
	fprint( f, " name=\"%s\"", smiNode->name );

	if( smiType->name ) {
	    typeName = smiType->name;
	}
	else {
	    typeName = getStringBasetype( smiType->basetype );
	}
	prefix = getTypePrefix( typeName );

	if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprint( f, " type=\"%s%s\"",
		    smiNode->name,
		    getStringBasetype( smiType->basetype ) );
	    fprint( f, " minOccurs=\"0\">\n" );
	    fprintAnnotationElem( f, indent + INDENT, smiNode );
	}
	
	else if( smiType->basetype == SMI_BASETYPE_ENUM &&
	    ! smiType->name ) {
	    fprint( f, " minOccurs=\"0\">\n" );
	    fprintAnnotationElem( f, indent + INDENT, smiNode );
	    fprintTypedef( f, indent + INDENT, smiType, NULL );
	}
	
	else {
	    if( prefix ) {
		fprint( f, " type=\"%s:%s\"", prefix, typeName );
	    }
	    else {
		fprint( f, " type=\"%s\"", typeName );
	    }
	    fprint( f, " minOccurs=\"0\">\n" );
	    fprintAnnotationElem( f, indent + INDENT, smiNode );
	}
	
	fprintSegment( f, indent, "</xsd:element>\n", 0 ); 
	break;
    }

    case SMI_NODEKIND_NOTIFICATION:
	fprintSegment( f, indent, "<xsd:element ", 0 );
	fprint( f, "name=\"%s\"/>\n", smiNode->name );
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
	    fprintElement( f, 5 * INDENT, iterNode, DXSD_MIBPREFIX );
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
		( ! getTypePrefix( smiType->name ) ) ) {
		fprintTypedef( f, INDENT, smiType, iterNode->name );
	    }
	}
    }
    
}

static void fprintImports( FILE *f, int indent, SmiModule *smiModule )
{
    SmiImport *iterImp;
    char *lastModName = "lastModName";
    
    for( iterImp = smiGetFirstImport( smiModule );
	 iterImp;
	 iterImp = smiGetNextImport( iterImp ) ) {
	/* assume imports to be ordered by module names */
	if( strcmp( iterImp->module, lastModName ) ) {
	    fprintSegment( f, indent, "<xsd:import ", 0 );
	    fprint( f, "namespace=\"%s%s\" schemaLocation=\"%s%s.xsd\"/>\n",
		    schemaLocation, iterImp->module,
		    schemaLocation, iterImp->module );
	}
	lastModName = iterImp->module;
    }
    fprintSegment( f, indent, "<xsd:import namespace=\"http://www.ibr.cs.tu-bs.de/~tklie/smi\" schemaLocation=\"http://www.ibr.cs.tu-bs.de/~tklie/smi.xsd\"/>\n\n", 0 );
}


static void fprintNodes( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;
    
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_SCALAR ) ) {
	    fprintElement( f, 5 * INDENT, iterNode, DXSD_MIBPREFIX );
	}
    }   
}

static void fprintNotifications( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;
    
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NOTIFICATION );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NOTIFICATION ) ) {
	fprintElement( f, 5 * INDENT, iterNode, DXSD_MIBPREFIX );
    }
}


static void fprintModule(FILE *f, SmiModule *smiModule)
{
    if( smiModule->description ) {
	fprintSegment(f, INDENT, "<xsd:annotation>\n", 0);
	fprintDocumentation(f, 2 * INDENT, smiModule->description);
	fprintSegment(f, INDENT, "</xsd:annotation>\n\n", 0);
    }
    
    fprintImports(f, INDENT, smiModule);

    /* print root node */
    fprintSegment( f, INDENT, "<xsd:element ", 0 );
    fprint( f, "name=\"%s\">\n", smiModule->name );
    fprintSegment( f, 2 * INDENT, "<xsd:complexType>\n", 0 );
    fprintSegment( f, 3 *INDENT, "<xsd:choice>\n", 0 );
    fprintSegment( f, 4 * INDENT, "<xsd:sequence>\n", 0 );
    fprintNodes( f, smiModule );
    fprintRows( f, smiModule );
    fprintSegment( f, 4 * INDENT, "</xsd:sequence>\n", 0 );
    fprintSegment( f, 4 * INDENT, "<xsd:sequence>\n", 0 );
    fprintNotifications( f, smiModule );
    fprintSegment( f, 4 * INDENT, "</xsd:sequence>\n", 0 );
    fprintSegment( f, 3 * INDENT, "</xsd:choice>\n", 0 );
    fprintSegment( f, 2 * INDENT, "</xsd:complexType>\n", 0 );
    fprintSegment( f, INDENT, "</xsd:element>\n\n", 0 );

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

static void registerType( char *type, char *module )
{
    TypePrefix *oldTPr = NULL, *iterTPr = NULL;

    for( iterTPr = typePrefixes; iterTPr; iterTPr = iterTPr->next ) {
	oldTPr = iterTPr;
    }
    if( ! oldTPr ) {
	/* type prefixes do not exist yet */
	typePrefixes = xmalloc( sizeof( TypePrefix ) );
	typePrefixes->type = type;
	typePrefixes->prefix = module;
	typePrefixes->next = NULL;
    }
    else {
	/* create new TypePrefix */
	oldTPr->next = xmalloc( sizeof( TypePrefix ) );
	oldTPr->next->type = type;
	oldTPr->next->prefix = module;
	oldTPr->next->next = NULL;
    }
}


static void fprintSchemaDef( FILE *f, SmiModule *smiModule )
{
    SmiImport *iterImp;
    char *lastModName = "lastModName";
    /* There is no mib called "lastModName", is there?
       So let's use this to initialize variable. */
    
    fprint(f,
	   "<xsd:schema targetNamespace=\"%s%s\"\n",
	   schemaLocation, smiModule->name);
    
    fprint(f, "            xmlns=\"%s%s\"\n",
	   schemaLocation, smiModule->name);
    fprint(f, "            xmlns:xml=\"http://www.w3.org/XML/1998/namespace\"\n");
    fprint(f, "            xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\n");
    fprint(f, "            xmlns:smi=\"http://www.ibr.cs.tu-bs.de/~tklie/smi\"\n");
    
    for( iterImp = smiGetFirstImport( smiModule );
	 iterImp;
	 iterImp = smiGetNextImport( iterImp ) ) {
	registerType( iterImp->name, iterImp->module );
	/* assume imports to be ordered by module names */
	if( strcmp( iterImp->module, lastModName ) ) {
	    fprint( f, "            xmlns:%s=\"%s%s\"\n",
		    iterImp->module, schemaLocation, iterImp->module );
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

	/* register smi basetypes */
	registerType( "Integer32","smi" );
	registerType( "ObjectIdentifier", "smi" );
	registerType( "OctetString", "smi" );

	/* make sure url ends with '/' */
	if( schemaLocation[ strlen( schemaLocation ) - 1 ] != '/' ) {
	    strcat( schemaLocation, "/" );
	}
	
	fprint(f, "<?xml version=\"1.0\"?>\n"); 
	fprint(f, "<!-- This module has been generated by smidump "
	       SMI_VERSION_STRING ". Do not edit. -->\n");
	fprint(f, "\n");

	fprintSchemaDef(f, modv[i]);	

	fprintModule(f, modv[i]);
	fprintTypedefs(f, modv[i]);
	
	fprint(f, "\n</xsd:schema>\n");

	/* delete type-prefix-mapping */
	free( typePrefixes );
    }

    if (output) {
	fclose(f);
    }
}



void initXsd()
{

    static SmidumpDriverOption opt[] = {
	{ "schema-url", OPT_STRING, &schemaLocation, 0,
	  "The URL where the imported MIB modules reside" },
	{ 0, OPT_END, 0, 0 }
    };
    
    static SmidumpDriver driver = {
	"xsd",
	dumpXsd,
	0,
	SMIDUMP_DRIVER_CANT_UNITE,
	"XML schema definitions",
	opt,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}
