/*
 * dump-xsd.c --
 *
 *      Operations to dump SMI module information as XML schema definitions.
 *
 * Copyright (c) 2001 J. Schoenwaelder, Technical University of Braunschweig.
 *           (c) 2002 T. Klie, Technical University of Braunschweig.
 *           (c) 2002 F. Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-xsd.c,v 1.57 2002/12/11 22:04:08 strauss Exp $
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

extern int smiAsprintf(char **strp, const char *format, ...);

#define  INDENT		2    /* indent factor */

static int ind = 0;

#ifndef MIN
#define MIN(a,b) ((a)) < ((b)) ? ((a)) : ((b))
#endif /* #ifndef MIN */

static char *schemaLocation = "http://www.ibr.cs.tu-bs.de/projects/libsmi/xsd/";
static int container = 0;
static char *containerBasename = "container";

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

typedef struct TypePrefix {
    char *type;
    char *prefix;
    struct TypePrefix *next;
} TypePrefix;

static TypePrefix *typePrefixes = NULL;

/* part of a display hint */
typedef struct DH {
    unsigned int number;
/*    int repeat;    //  repeat not yet supported */
    char type;
    char separator;
/*    char repTerm;  // repeat not yet supported */
    struct DH *next;
} DH;



/* some forward declarations */
static void fprintElement( FILE *f, SmiNode *smiNode );
static char* getTypePrefix( char *typeName );

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

static char* getStringStatus(SmiStatus status)
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

static char* getStringAccess( SmiAccess smiAccess )
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

static int pow( int base, unsigned int exponent )
{
  unsigned int i;
  int ret = 1;
  
  if( exponent == 0 ) {
    return 1;
  }

  for( i = 0; i < exponent; i++ ) {
    ret *= base;
  }
  return ret;
}

static unsigned int numDigits( unsigned int val )
{
    int ret  = 1;

    for(; val / 10; val = val / 10 ) {
	ret++;
    }
    
    return ret;
}



static void fprintSegment(FILE *f, int relindent, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
     
    if ((ind == 0) || (ind + relindent == 0)) {
	ind += relindent;
    } else {
	if (relindent < 0) ind += relindent;
	fprintf(f, "%*c", ind * INDENT, ' ');
	if (relindent > 0) ind += relindent;
    }
    vfprintf(f, fmt, ap);

    va_end(ap);
}



static void fprintMultilineString(FILE *f, const char *s)
{
    int i, j, len;

    fprintSegment(f, 0, "");
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    for (j = 0; xmlEscapes[j].character; j++) {
		if (xmlEscapes[j].character == s[i]) break;
	    }
	    if (xmlEscapes[j].character) {
		fputs(xmlEscapes[j].escape, f);
	    } else {
		putc(s[i], f);
	    }
            if (s[i] == '\n') {
		fprintSegment(f, 0, "");
	    }
	}
    }
}



static void fprintDocumentation(FILE *f, const char *description)
{
    if (description) {
	fprintSegment(f, 1, "<xsd:documentation>\n");
	fprintMultilineString(f, description);
	fprintf(f, "\n");
	fprintSegment(f, -1, "</xsd:documentation>\n");
    }
}

static void fprintNamedNumber( FILE *f, SmiNamedNumber *nn )
{
    fprintSegment( f, 1, "<xsd:enumeration value=\"%s\">\n", nn->name );
    fprintSegment( f, 1, "<xsd:annotation>\n");
    fprintSegment( f, 1, "<xsd:appinfo>\n");
    fprintSegment( f, 0, "<intVal>%d</intVal>\n",
		   (int)nn->value.value.integer32 );
    fprintSegment( f, -1, "</xsd:appinfo>\n");
    fprintSegment( f, -1, "</xsd:annotation>\n");
    fprintSegment( f, -1, "</xsd:enumeration>\n");
}


static void fprintStdRestHead( FILE *f, SmiType *smiType )
{
    char *baseTypeName = getStringBasetype(smiType->basetype);
    char *prefix = getTypePrefix( baseTypeName );
    
    if( prefix ) {
	fprintSegment(f, 1, "<xsd:restriction base=\"%s:%s\">\n",
		      prefix, baseTypeName );
    }
    else {
	fprintSegment(f, 1, "<xsd:restriction base=\"%s\">\n", baseTypeName );
    }
}


static void fprintHexOrAsciiType( FILE *f, SmiType *parent,
				  SmiInteger32 minLength,
				  SmiInteger32 maxLength,
				  char *name, int hex )
{
    char *prefix = parent ? getTypePrefix( parent->name ) : NULL;
    char *typeFlag = hex ? "Hex" : "Ascii";
    
    if( name ) {
	fprintSegment( f, 1, "<xsd:simpleType name=\"%s%s\">\n",
		       name, typeFlag );
    } else {
	fprintSegment( f, 1, "<xsd:simpleType>\n");
    }
    if( prefix ) {
	fprintSegment( f, 1, "<xsd:restriction base=\"%s:%s%s\">\n",
		       prefix, parent->name, typeFlag );
    }
    else {
	fprintSegment( f, 1, "<xsd:restriction base=\"%s%s\">\n",
		       parent->name, typeFlag );
    }
   
    if( minLength > 0 ) {
	fprintSegment( f, 0, "<xsd:minLength value=\"%d\"/>\n",
		       (int)minLength );
    }
    if( maxLength > -1 ) {
	fprintSegment( f, 0, "<xsd:maxLength value=\"%d\"/>\n",
		       (int)maxLength );
    }

    fprintSegment( f, -1, "</xsd:restriction>\n");
    fprintSegment( f, -1, "</xsd:simpleType>\n");
}


static void initDH( struct DH *dh )
{
    	/* init with NULLs */
	dh->number = 0;
	/*dh->repeat = 0; repeat not yet supported */
	dh->type = 0;
	dh->separator = '\0';
	/* dh->repTerm = '\0'; repeat not yet supported */
	dh->next = NULL;
}

/* parse a display hint and create a list of DHs */
static struct DH *parseDH( const char *hint )
{
    struct DH *iterDH = (struct DH *)malloc( sizeof( struct DH ) );
    struct DH *ret = iterDH;
    struct DH *oldIterDH = iterDH;
    unsigned int pos = 0;
    
    if( !ret ) {
	return NULL;
    }

    initDH( iterDH );
    while( pos < strlen( hint ) ) {

	if( ! iterDH ) {
	    iterDH = (struct DH *)malloc( sizeof( struct DH ) );
	    if( ! iterDH ) return NULL;
	    initDH( iterDH );
	    oldIterDH->next = iterDH;
	    oldIterDH = iterDH;
	}

	switch( hint[ pos ] ) {
	    
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9': {
	    unsigned int endPos;
	    char *strNum;
	    
	    /* find end of number */
	    for( endPos = pos; isdigit( hint[ endPos ] ); endPos++ );
	    
	    /* parse number */
	    strNum = (char *)malloc( endPos - pos );
	    strncpy( strNum, &hint[ pos ], endPos - pos );
	    strNum[ endPos - pos ] = '\0';
	    iterDH->number = atoi( strNum );
	    free( strNum );
	    
	    /* forward the position pointer */
	    pos = endPos;
	    break;
	}
	
	case 'a':
	case 'b':
	case 'd':
	case 'o':
	case 'x':
	    iterDH->type = hint[ pos++ ];
	    if( isdigit( hint[ pos ] ) || hint[ pos ] == '*' ) {
		iterDH = iterDH->next;
	    }
	    break;
	    
	case '*':
	    /* repeat not supported */
	    return NULL;
	    
	default:
	    if( iterDH->separator ) {
		/* iterDH->repTerm = hint[ pos++ ]; // repeat not supported */
	    }
	    else {
		iterDH->separator = hint[ pos++ ];
	    }
	    if( isdigit( hint[ pos ] ) || hint[ pos ] == '*' ) {
		iterDH = iterDH->next;
	    }
	    break;
	}	
    }
    return ret;
}


/* build a regexp from this display hint */
static char* getStrDHType( char *hint,
			   SmiUnsigned32 *lengths, unsigned int numSubranges  )
{
    unsigned int i = 0;
    char *ret = lengths[ i ] ? "(" : "((";
    DH *dh = parseDH( hint );

    if(! dh ) return NULL;

    do {
	unsigned int octetsUsed = 0;
	DH *iterDH;
	
	for( iterDH = dh; iterDH; iterDH = iterDH->next ) {
	    char *baseRegexp = NULL;

	    switch( iterDH->type ) {

	    case 'a':
		/* ascii character */
		baseRegexp = "(#x00-#x7f)";
		break;

	    case 'b':
		/* binary number */
		baseRegexp = "((0|1){8})";
		break;

	    case 'd':
		/* decimal number */
		baseRegexp = "([0-9]{3})";
		break;

	    case 'o':
		/* octal number */
		baseRegexp = "([0-7]{3})";
		break;

	    case 'x':
		/* hexadecimal number */
		baseRegexp = "([0-9A-Fa-f]{2})";
		break;
	    }
	    

	    if( iterDH->number < lengths[ i ] ) {
		/* there are more octets to come */
		if( iterDH->type == 'd' ) {
		    /* decimal number needs to be treated differently */
		    smiAsprintf( &ret, "%s(0|[1-9](([0-9]){0,%d}))",
				 ret,
				 numDigits( pow( 255, iterDH->number ) ) - 1);

		    octetsUsed += iterDH->number;		    
		    if( octetsUsed >= lengths[ i + 1 ] ) {
			/* maximum number of octets used,
			   we must exit the loop */
			break;
		    }
		    
		    else if( iterDH->separator ) {
			smiAsprintf( &ret, "%s%c", ret, iterDH->separator );
		    }
		}
		else {
		    if( iterDH->next ){
			smiAsprintf( &ret, "%s(%s{%d})",
				     ret,
				     baseRegexp, iterDH->number );

			octetsUsed += iterDH->number;
			if( octetsUsed >= lengths[ i + 1 ] ) {
			/* maximum number of octets used,
			   we must exit the loop */			    
			    break;
			}

			if( iterDH->separator ) {
			    smiAsprintf( &ret, "%s%c", ret, iterDH->separator );
			}
		    }
		    else {			
			smiAsprintf( &ret, "%s(%s",
				  ret, baseRegexp );

			if( iterDH->separator ) {
			    smiAsprintf( &ret, "%s%c", ret, iterDH->separator );
			}

			smiAsprintf( &ret, "%s{%u,%u})%s",
				     ret, lengths[ i ] - 1, lengths[ i+1 ] - 1,
				     baseRegexp );
			
			octetsUsed += iterDH->number;
			if( octetsUsed >= lengths[ i + 1 ] ) {
			    /* maximum number of octets used,
			       we must exit the loop */			    
			    break;
			}
		    }
		}
	    }
	    else {
		/* might be the last one */

		if( iterDH->type == 'd' ) {
		    /* decimal number needs to be treated differently */
		    if( iterDH->number < lengths[ i+1 ] ) {
			smiAsprintf( &ret, "%s(0|[1-9]([0-9]{0,%d}))",
				     ret,
				     numDigits( pow( 255, iterDH->number ) ) );

			octetsUsed += lengths[ i ];
			if( octetsUsed >= lengths[ i + 1 ] ) {
			    /* the maximum number of octets have been reached,
			       we must exit the loop */
			    break;
			}

			if( iterDH->separator ) {
			    smiAsprintf( &ret, "%s%c", ret, iterDH->separator );
			}						
		    }
		    else {
			smiAsprintf( &ret, "%s(0|[1-9]([0-9]{0,%d})",
				     ret,
				     numDigits( pow( 255, lengths[ i+1 ] ) ) );
		    }
		}
		else {
		    smiAsprintf( &ret, "%s(%s",  ret, baseRegexp );
		    if( iterDH->next ) {
			if( iterDH->separator ) {
			    smiAsprintf( &ret, "%s%c", ret, iterDH->separator );
			}
			if( ! lengths[ i ] && lengths[ i+1 ] == 65535 ) {
			    smiAsprintf( &ret, "%s)*",ret );
			}
			else{
			    smiAsprintf( &ret, "%s){%u,%u}",ret, lengths[ i ],
					 MIN( iterDH->number,
					      lengths[ i + 1] ) - 1 );
			}
			octetsUsed += lengths[ i ];
			if( octetsUsed >= lengths[ i + 1 ] ) {
			    /* the maximum number of octets have been reached,
			       we must exit the loop */
			    break;
			}						
		    }
		    else {
			octetsUsed += lengths[ i ];
			if( iterDH->separator &&
			    octetsUsed < lengths[ i + 1 ] ) {

			    if( ! lengths[ i ] && lengths[ i+1 ] == 65535 ) {
				smiAsprintf( &ret, "%s%c)*%s",
					     ret, iterDH->separator, baseRegexp );
			    }
			    else {
				smiAsprintf( &ret, "%s%c){%u,%u}%s",
					     ret, iterDH->separator,
					     lengths[ i ], lengths[ i + 1] - 1,
					     baseRegexp );
			    }
			}
			else {
			    if( ! lengths[ i ] && lengths[ i+1 ] == 65535 ) {
				smiAsprintf( &ret, "%s)*%c",
					     ret, iterDH->separator );
			    }
			    else {
				smiAsprintf( &ret, "%s){%u,%u}%c",
					     ret, lengths[ i ],
					     lengths[ i + 1],
					     iterDH->separator );
 			    }			    
			}
		    }
		}
		
		if( octetsUsed >= lengths[ i + 1 ] ) {
		    /* the maximum number of octets have been reached,
		       we must exit the loop */
		    break;
		}
	    }
	}
	i += 2;

	if( i < numSubranges  * 2 ) {
	    smiAsprintf( &ret, "%s)|(", ret );
	}
	else {
	    smiAsprintf( &ret, "%s)", ret );
	    if( ! lengths[ i - 2 ] ) {
		smiAsprintf( &ret, "%s){0,1}", ret );
	    }
	}
    } while( i < numSubranges * 2 );
    
    return ret;
}

static int dhInParent( SmiType *smiType )
{
    SmiType *parent = smiGetParentType( smiType );

    if( smiType->format && parent->format ) {
	return ! strcmp( smiType->format, parent->format );
    }
    return 0;
}

#define MD_DH_INT_NORMAL   1
#define MD_DH_INT_DECIMAL  2
#define MD_DH_INT_BIN      3
#define MD_DH_INT_OCT      4
#define MD_DH_INT_HEX      5

/* parse a (integer) display hint and specify the offset, if used */
static int getIntDHType( char *hint, int *offset )
{
    switch( hint[ 0 ] ) {

    case 'd':
	if( hint[1] ) {
	    *offset = 0;
	    *offset = atoi( &hint[2] );
	    return MD_DH_INT_DECIMAL;
	}
	return MD_DH_INT_NORMAL;

    case 'b':
	/* binary value */
	return MD_DH_INT_BIN;
    case 'o':
	/* octet value */
	return MD_DH_INT_OCT;
    case 'x':
	/* hex value */
	return MD_DH_INT_HEX;
    default:
	/* should not occur */
	return 0;
    }
}


static void fprintRestriction(FILE *f, SmiType *smiType)
{
    SmiRange *smiRange;
    
    /* print ranges etc. */
    switch( smiType->basetype ) {

    case SMI_BASETYPE_INTEGER32:
    {
	SmiInteger32 min = -2147483647, max = 2147483646;
	int offset, useDecPoint = 0;

	if( smiType->format ) {
	  /* we have a display hint here, so check if we have to use
	     a decimal point */
	  useDecPoint =  
	    getIntDHType( smiType->format, &offset ) == MD_DH_INT_DECIMAL; 
	  /* xxx: other display hint types (binary, oct, hex) */
	}

	if( useDecPoint ) {
	  fprintSegment( f, 1, "<xsd:restriction base=\"xsd:decimal\">\n");
	  fprintSegment( f, 0, "<xsd:fractionDigits=\"%d\"/>\n", offset );
	}
	else {
	  fprintStdRestHead( f, smiType );
	}

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
	
	/* print minimu value */
	if( useDecPoint ) {
	    fprintSegment( f, 0, "<xsd:minInclusive value=\"%d.%d\"/>\n", 
			   (int)min / pow( 10, offset ), 
			   abs( (int)min % pow( 10, offset ) ) );
	} else {
	    fprintSegment( f, 0, "<xsd:minInclusive value=\"%d\"/>\n",
			   (int)min );
	}

	/* print maximum value */
	if( useDecPoint ) {
	    fprintSegment( f, 0, "<xsd:maxInclusive value=\"%d.%d\"/>\n", 
			   (int)max / pow( 10, offset ), 
			   abs( (int)max % pow( 10, offset ) ) );
	} else {
	    fprintSegment( f, 0, "<xsd:maxInclusive value=\"%d\"/>\n",
			   (int)max );
	}
	
	fprintSegment(f, -1, "</xsd:restriction>\n");
	break;
    }
    
    case SMI_BASETYPE_OCTETSTRING:
    {
	SmiInteger32  minLength, maxLength;
	unsigned int numSubRanges = 0;
	
	minLength = 0;
	maxLength = -1;

	/* get range details */
	for( smiRange = smiGetFirstRange( smiType );
	     smiRange;
	     smiRange = smiGetNextRange( smiRange ) ) {
	    if( minLength == 0 ||
		smiRange->minValue.value.integer32 < minLength ) {
		minLength = smiRange->minValue.value.integer32;	     
	    }
	    if( smiRange->maxValue.value.integer32 > maxLength ) {
		maxLength = smiRange->maxValue.value.integer32;
	    }
	    numSubRanges++;
	}

	

	if( smiType->format &&
	    ( smiType->decl == SMI_DECL_IMPLICIT_TYPE ||
	      smiType->decl == SMI_DECL_TEXTUALCONVENTION ) &&
	    ! dhInParent( smiType ) ) {
	    /*
	    fprintStringUnion( f, indent, smiType,
			       minLength, maxLength, 0, NULL );
	    */
	    SmiUnsigned32 *lengths;
	    char *dh;
	    if( numSubRanges ) {
		unsigned int lp = 0;
		SmiRange *smiRange;
		lengths = xmalloc( 2 * sizeof( SmiUnsigned32 ) *
				   numSubRanges );
				
		/* write subtype lengths to the array */
		for( smiRange = smiGetFirstRange( smiType );
		     smiRange;
		     smiRange = smiGetNextRange( smiRange ) ) {
		    lengths[ lp++ ] = smiRange->minValue.value.unsigned32;
		    lengths[ lp++ ] = smiRange->maxValue.value.unsigned32;
		}
	    }
	    else {
		lengths = xmalloc( 2 * sizeof( SmiUnsigned32 ) );
		lengths[0] = 0;
		lengths[1] = 65535;
	    }
	    
	    fprintSegment( f, 1, "<xsd:restriction base=\"xsd:string\">\n" );

	    /* create regexp */
	    dh = getStrDHType( smiType->format, lengths, numSubRanges );

	    if( dh ) {
		fprintSegment( f, 0, "<xsd:pattern value=\"%s\"/>\n", dh );
	    }
	    else {
		fprintf( f, "<!-- Warning: repeat in display hint. "
		            "This feature is not supported. -->\n" );
	    }
	    fprintSegment( f, -1, "</xsd:restriction>\n");
	    
	    xfree( lengths );
	}
	else {
	    SmiType *parent = smiGetParentType( smiType );
	    /*
	    fprintStringUnion( f, indent, smiType,
			       minLength, maxLength, secondTime,
			       smiType->name );
	    */
	    if( parent ) {
		if( ! parent->format ) {
		    char *prefix = getTypePrefix( parent->name );

		    if( prefix ) {
			fprintSegment( f, 1, "<xsd:restriction base=\"%s:%s\">\n",
				       prefix, parent->name );
		    } else {
			fprintSegment( f, 1, "<xsd:restriction base=\"%s\">\n",
				       parent->name );
		    }

		    /* print length restriction */
		    fprintSegment( f, 0, "<xsd:minLength value=\"%d\"/>\n",
				   (int)minLength );
		    fprintSegment( f, 0, "<xsd:maxLength value=\"%d\"/>\n",
				   (int)maxLength );
		    
		    fprintSegment( f, -1, "</xsd:restriction>\n");
		} else {
		    SmiUnsigned32 *lengths =
			xmalloc( 2 * sizeof( SmiUnsigned32 ) * numSubRanges );
		    unsigned int lp = 0;
		    SmiRange *smiRange;
		    
		    /* write subtype lengths to the array */
		    for( smiRange = smiGetFirstRange( smiType );
			 smiRange;
			 smiRange = smiGetNextRange( smiRange ) ) {
			lengths[ lp++ ] = smiRange->minValue.value.unsigned32;
			lengths[ lp++ ] = smiRange->maxValue.value.unsigned32;
		    }
		    
		    fprintSegment( f, 1, "<xsd:restriction base=\"xsd:string\">\n" );
		    fprintSegment( f, 0, "<xsd:pattern value=\"%s\"/>\n",
				   getStrDHType( parent->format,
						 lengths, numSubRanges ) );
		    fprintSegment( f, -1, "</xsd:restriction>\n");
		    xfree( lengths );
		}
		
	    }
	}
	break;
    }

    case SMI_BASETYPE_FLOAT128:
    {
/*	SmiFloat128 min, max; */
	fprintStdRestHead( f, smiType );
	
	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_FLOAT64:
    {
/*	SmiFloat64 min,max;*/
	fprintStdRestHead( f, smiType );

	/* xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_FLOAT32:
    {
/*	SmiFloat32 min,max;*/
	fprintStdRestHead( f, smiType );
	
	/* xxx, only SMIng */
	break;
    }
    
    case SMI_BASETYPE_INTEGER64:
    {
/*	SmiInteger64 min,max;*/
	fprintStdRestHead( f, smiType );
	
	/* xxx, only SMIng */
	break;
    }

    case SMI_BASETYPE_UNSIGNED64:
    {
	SmiUnsigned64 min, max;

	min = 0;
	max = 18446744073709551615U;

	fprintStdRestHead( f, smiType );
	
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
	fprintSegment( f, 0, "<xsd:minInclusive value=\"%lu\"/>\n",
		       (unsigned long)min );

	fprintSegment( f, 0, "<xsd:maxInclusive value=\"%lu\"/>\n",
		       (unsigned long)max );
	
	fprintSegment(f, -1, "</xsd:restriction>\n");
	
	break;
    }

    case SMI_BASETYPE_UNSIGNED32:
    {
	SmiUnsigned32 min, max;

	min = 0;
	max = 4294967295UL;

	fprintStdRestHead( f, smiType );
	
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
	fprintSegment( f, 0, "<xsd:minInclusive value=\"%u\"/>\n",
		       (unsigned int)min );

	fprintSegment( f, 0, "<xsd:maxInclusive value=\"%u\"/>\n",
		       (unsigned int)max );
	
	fprintSegment(f, -1, "</xsd:restriction>\n");
	break;
    }

    case SMI_BASETYPE_ENUM:
    case SMI_BASETYPE_BITS:
    {
	SmiNamedNumber *nn;
	
	fprintSegment(f, 1, "<xsd:restriction base=\"xsd:NMTOKEN\">\n");

	/* iterate named numbers */
	for( nn = smiGetFirstNamedNumber( smiType );
	     nn;
	     nn = smiGetNextNamedNumber( nn ) ) {
	    fprintNamedNumber( f, nn );
	}
	fprintSegment(f, -1, "</xsd:restriction>\n");
	break;
    }

    case SMI_BASETYPE_OBJECTIDENTIFIER:
	fprintSegment( f, 0,
		       "<xsd:restriction base=\"smi:ObjectIdentifier\"/>\n");
	break;
    case SMI_BASETYPE_UNKNOWN:
	/* should not occur */
	break;
    }
}
    

static unsigned int getNamedNumberCount( SmiType *smiType )
{
    SmiNamedNumber *nn;
    unsigned int ret = 0;

    for( nn = smiGetFirstNamedNumber( smiType );
	 nn;
	 nn = smiGetNextNamedNumber( nn ) ) {
	ret++;
    }

    return ret;
}


static void fprintBitList( FILE *f, SmiType *smiType, const char *name )
{
    fprintSegment( f, 1, "<xsd:simpleType name=\"%sBitList\">\n", name );
    fprintSegment( f, 0, "<xsd:list itemType=\"%sBit\"/>\n", name );
    fprintSegment( f, -1, "</xsd:simpleType>\n\n");
    
    fprintSegment( f, 1, "<xsd:simpleType name=\"%sBits\">\n", name );
    fprintSegment( f, 1, "<xsd:restriction base=\"%sBitList\">\n", name );
    fprintSegment( f, 0, "<xsd:maxLength value=\"%d\"/>\n",
		   getNamedNumberCount( smiType ) );
    fprintSegment( f, -1, "</xsd:restriction>\n");
    fprintSegment( f, -1, "</xsd:simpleType>\n\n");
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


static void fprintSubRangeType( FILE *f,
				SmiRange *smiRange, SmiType *smiType )
{
    
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
	
	fprintSegment( f, 1, "<xsd:simpleType>\n");
	fprintStdRestHead( f, smiType );

	fprintSegment( f, 0, "<xsd:minInclusive value=\"%u\"/>\n",
		       (unsigned int)min );
	
	fprintSegment( f, 0, "<xsd:maxInclusive value=\"%u\"/>\n",
		       (unsigned int)max );

	fprintSegment(f, -1, "</xsd:restriction>\n");
	fprintSegment(f, -1, "</xsd:simpleType>\n");
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

	fprintSegment( f, 1, "<xsd:simpleType>\n");
    	fprintStdRestHead( f, smiType );
	
	fprintSegment( f, 0, "<xsd:minInclusive value=\"%d\"/>\n", (int)min );

	fprintSegment( f, 0, "<xsd:maxInclusive value=\"%d\"/>\n", (int)max );

	fprintSegment(f, -1, "</xsd:restriction>\n");	
	fprintSegment(f, -1, "</xsd:simpleType>\n");
	break;
	
    }

    case SMI_BASETYPE_OCTETSTRING: {
	SmiInteger32  minLength, maxLength;
	
	minLength = 0;
	maxLength = -1;
	
	if( smiRange->minValue.value.integer32 < minLength ) {
	    minLength = smiRange->minValue.value.integer32;	     
	}
	if( smiRange->maxValue.value.integer32 > maxLength ) {
	    maxLength = smiRange->maxValue.value.integer32;
	}
	fprintHexOrAsciiType( f, smiType,
			      minLength, maxLength, NULL, 1 );
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
	
	fprintSegment( f, 1, "<xsd:simpleType>\n");
	fprintStdRestHead( f, smiType );

	fprintSegment( f, 0, "<xsd:minInclusive value=\"%lu\"/>\n",
		       (unsigned long)min );

	fprintSegment( f, 0, "<xsd:maxInclusive value=\"%lu\"/>\n",
		       (unsigned long)max );

	fprintSegment(f, -1, "</xsd:restriction>\n");
	fprintSegment(f, -1, "</xsd:simpleType>\n");
	break;
    }

    case SMI_BASETYPE_ENUM:
    case SMI_BASETYPE_BITS:
    case SMI_BASETYPE_OBJECTIDENTIFIER:
    case SMI_BASETYPE_UNKNOWN:
	/* should not occur */
	break;
	
    }
}

static void fprintDisplayHint( FILE *f, char *format )
{
    fprintSegment( f, 0, "<displayHint>%s</displayHint>\n", format );
}




static void fprintTypedef(FILE *f, SmiType *smiType, const char *name)
{
    SmiRange *smiRange;
    unsigned int numSubRanges = getNumSubRanges( smiType );
    
    if ( name ) {
	if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprintSegment(f, 1, "<xsd:simpleType name=\"%sBit\">\n", name);
	}
/*	else if( smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {
	    fprintSegment(f, indent, "<xsd:simpleType");
	    fprintf( f, " name=\"%sType\">\n", name );
	    }*/
	else {
	    fprintSegment(f, 1, "<xsd:simpleType name=\"%s\">\n", name);
	}
    }

    else {
	/* unnamed simple type */
	fprintSegment(f, 1, "<xsd:simpleType>\n");
    }

    if( smiType->description ) {
	fprintSegment( f, 1, "<xsd:annotation>\n");
	fprintDocumentation(f, smiType->description);
	if( smiType->format ) {
	    fprintSegment( f, 1, "<xsd:appinfo>\n");
	    fprintDisplayHint( f, smiType->format );
	    fprintSegment( f, -1, "</xsd:appinfo>\n");
	}
	fprintSegment( f, -1, "</xsd:annotation>\n");
    }
    
    if( smiType->format && smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {
	fprintRestriction( f, smiType );
	fprintSegment(f, -1, "</xsd:simpleType>\n");
    }
    
    else if( ( numSubRanges > 1 ) &&
	     ( smiType->basetype != SMI_BASETYPE_OCTETSTRING ) ) {
	
	fprintSegment( f, 1, "<xsd:union>\n");
	
	for( smiRange = smiGetFirstRange( smiType );
	     smiRange;
	     smiRange = smiGetNextRange( smiRange ) ) {
	    fprintSubRangeType( f, smiRange, smiType );
	}
	
	fprintSegment( f, -1, "</xsd:union>\n");
	fprintSegment(f, -1, "</xsd:simpleType>\n");
    }
    else {
	fprintRestriction(f, smiType );
	fprintSegment(f, -1, "</xsd:simpleType>\n");
    }

    /* print an empty line after global types */
    if( smiType->decl != SMI_DECL_IMPLICIT_TYPE && name ) {
	fprintf( f, "\n" );
    }
  

    if( smiType->basetype == SMI_BASETYPE_BITS ) {
	/* if basetype is BITS we have to do something more */
	fprintBitList( f, smiType, name );
    }
}


static char* getTypePrefix( char *typeName )
{
    TypePrefix *iterTPr;

    if( !typeName ) {
	return NULL;
    }

    for( iterTPr = typePrefixes; iterTPr; iterTPr = iterTPr->next ) {
	if( ! strcmp( iterTPr->type, typeName ) ) {
	    return iterTPr->prefix;
	}
    }

    return NULL;
}


static void fprintAnnotationElem( FILE *f, SmiNode *smiNode ) {
    int i;
    
    fprintSegment( f, 1, "<xsd:annotation>\n");
    fprintSegment( f, 1, "<xsd:appinfo>\n");

    fprintSegment( f, 0, "<maxAccess>%s</maxAccess>\n",
		   getStringAccess( smiNode->access ) );

    fprintSegment( f, 0, "<status>%s</status>\n",
		   getStringStatus( smiNode->status ) );

    fprintSegment( f, 0, "<oid>");
    for (i = 0; i < smiNode->oidlen; i++) {
	fprintf(f, i ? ".%u" : "%u", smiNode->oid[i]);
    }
    fprintf( f, "</oid>\n" );

    if( smiNode->format ) {
	fprintDisplayHint( f, smiNode->format );
    }

    if( smiNode->units ) {
	fprintSegment( f, 0, "<units>%s</units>\n", smiNode->units );
    }
  
    fprintSegment( f, -1, "</xsd:appinfo>\n");
    fprintDocumentation( f, smiNode->description );
    fprintSegment( f, -1, "</xsd:annotation>\n");
    
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

static void fprintIndexAttr( FILE *f, SmiNode *smiNode, SmiNode *augments )
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
    
    if( prefix ) {
	fprintSegment( f, 1, "<xsd:attribute name=\"%s\" type=\"%s:%s\">\n",
		       smiNode->name, prefix, typeName );
    }
    else {
	fprintSegment( f, 1, "<xsd:attribute name=\"%s\" type=\"%s\">\n",
		       smiNode->name, typeName );
    }
        
    if( augments ) {
	fprintSegment( f, 1, "<xsd:annotation>\n");
	fprintSegment( f, 1, "<xsd:appinfo>\n");
	fprintSegment( f, 0, "<augments>%s</augments>\n", augments->name );
	fprintSegment( f, -1, "</xsd:appinfo>\n");
	fprintSegment( f, -1, "</xsd:annotation>\n");
    }
    else {
	fprintAnnotationElem( f, smiNode );
    }
    fprintSegment( f, -1, "</xsd:attribute>\n"); 
}



static void fprintIndex( FILE *f, SmiNode *smiNode, SmiNode *augments )
{
    SmiNode *iterNode;
    SmiElement *iterElem;
  
    /* iterate INDEX columns */
    for( iterElem = smiGetFirstElement( smiNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	iterNode = smiGetElementNode( iterElem );
	fprintIndexAttr( f, iterNode, augments );
    }

    /* print AUGMENTS-clause */
    iterNode = smiGetRelatedNode( smiNode );
    if( iterNode ) {
	fprintIndex( f, iterNode, iterNode );
    }
}


static void fprintComplexType( FILE *f, SmiNode *smiNode, const char *name )
{
    SmiNode *iterNode;
    int numChildren;
    
    if( name ) {
	fprintSegment( f, 1, "<xsd:complexType name=\"%sType\">\n",
		       smiNode->name );
    } else {
	fprintSegment( f, 1, "<xsd:complexType>\n" );
    }

    fprintAnnotationElem( f, smiNode );

    numChildren = hasChildren( smiNode, SMI_NODEKIND_ANY );
    if( numChildren ) {
	fprintSegment( f, 1, "<xsd:sequence>\n");
    }
    /* print child elements */
    for( iterNode = smiGetFirstChildNode( smiNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ) {
	
	fprintElement( f, iterNode );
	
    }
    if( numChildren ) {
	fprintSegment( f, -1, "</xsd:sequence>\n");
    }
    fprintIndex( f, smiNode, NULL );
    
    fprintSegment( f, -1, "</xsd:complexType>\n");
    if( name ) {
	/* we are printing out a global type,
	   so let's leave a blank line after it. */
	fprintf( f, "\n" );
    }

    for( iterNode = smiGetFirstChildNode( smiNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ) {
	if( iterNode->nodekind == SMI_NODEKIND_NODE ) {
	    fprintComplexType( f, iterNode, iterNode->name );
	}
    }
}

static char *getParentDisplayHint( SmiType *smiType )
{
    SmiType *iterType;

    for( iterType = smiGetParentType( smiType );
	 iterType;
	 iterType = smiGetParentType( iterType ) ) {
	if( iterType->format ) {
	    return iterType->format;
	}	
    }
    return NULL;
}
    



static void fprintElement( FILE *f, SmiNode *smiNode )
{
    switch( smiNode->nodekind ) {
	SmiType *smiType;

    case SMI_NODEKIND_NODE :
	{
	    SmiNode *iterNode;

	    fprintSegment( f, 1, "<xsd:element name=\"%s\">\n", smiNode->name);
	    fprintSegment( f, 1, "<xsd:complexType>\n");
	    fprintSegment( f, 1, "<xsd:sequence>\n");
	    for( iterNode = smiGetFirstChildNode( smiNode );
		 iterNode;
		 iterNode = smiGetNextChildNode( iterNode ) ) {
		if( iterNode->nodekind == SMI_NODEKIND_SCALAR ) {
		    fprintElement( f, iterNode );
		}
	    }
	    fprintSegment( f, -1, "</xsd:sequence>\n");
	    fprintSegment( f, -1, "</xsd:complexType>\n");
	    fprintSegment( f, -1, "</xsd:element>\n");
	}
	break;
	
    case SMI_NODEKIND_TABLE :
    {
	SmiNode *iterNode;
	
	/* ignore tables and just include their entries */
	for( iterNode = smiGetFirstChildNode( smiNode );
	     iterNode;
	     iterNode = smiGetNextChildNode( iterNode ) ) {
	    fprintElement( f, iterNode );
	}
	break;
    }

    case SMI_NODEKIND_ROW:
	fprintSegment( f, 1, "<xsd:element name=\"%s\" "
		       "minOccurs=\"0\" maxOccurs=\"unbounded\">\n",
		       smiNode->name );

	fprintAnnotationElem( f, smiNode );

	fprintComplexType( f, smiNode, NULL );
	fprintSegment( f, -1, "</xsd:element>\n");
	break;
	
    case SMI_NODEKIND_SCALAR:
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
	
	if( smiNode->access < SMI_ACCESS_READ_ONLY ) {
	    /* only print accessible nodes */
	    return;
	}
	
	smiType = smiGetNodeType( smiNode );

	if( smiType->name ) {
	    typeName = smiType->name;
	}
	else {
	    typeName = getStringBasetype( smiType->basetype );
	}
	prefix = getTypePrefix( typeName );

	if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprintSegment( f, 1, "<xsd:element name=\"%s\" type=\"%s%s\" "
			   "minOccurs=\"0\">\n",
			   smiNode->name,
			   smiNode->name,
			   getStringBasetype( smiType->basetype ) );
	    fprintAnnotationElem( f, smiNode );
	}
	
	else if( smiType->basetype == SMI_BASETYPE_ENUM && ! smiType->name ) {
	    fprintSegment( f, 1, "<xsd:element name=\"%s\" minOccurs=\"0\">\n",
			   smiNode->name );
	    fprintAnnotationElem( f, smiNode );
	    fprintTypedef( f, smiType, NULL );
	}

	else if( smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {	    
	    
	    if( smiType->decl == SMI_DECL_IMPLICIT_TYPE ) {

		char *hint = getParentDisplayHint( smiType );

		fprintSegment( f, 1, "<xsd:element name=\"%s\" "
			       "minOccurs=\"0\">\n", smiNode->name );
		if( ! hint ) {
		    fprintAnnotationElem( f, smiNode );
		    fprintTypedef( f, smiType, NULL );
		}
		else {
		    unsigned int numSubRanges = getNumSubRanges( smiType ),
			lp = 0;
		    SmiRange *smiRange;
		    SmiUnsigned32 *lengths = xmalloc(  2 * numSubRanges * 4 );

		    fprintAnnotationElem( f, smiNode );
		    
		    /* write subtype lengths to the array */
		    for( smiRange = smiGetFirstRange( smiType );
			 smiRange;
			 smiRange = smiGetNextRange( smiRange ) ) {
			lengths[ lp++ ] = smiRange->minValue.value.unsigned32;
			lengths[ lp++ ] = smiRange->maxValue.value.unsigned32;
		    }		    
		    fprintSegment( f, 1, "<xsd:simpleType>\n");
		    fprintSegment( f, 1, "<xsd:restriction base=\"xsd:string\">\n");
		    fprintSegment( f, 0, "<xsd:pattern value=\"%s\"/>\n",
				   getStrDHType( hint, lengths, numSubRanges));
		    fprintSegment( f, -1, "</xsd:restriction>\n");
		    fprintSegment( f, -1, "</xsd:simpleType>\n");
		    xfree( lengths );
		}
	    }

	    else {
		if( prefix ) {
		    fprintSegment( f, 1, "<xsd:element name=\"%s\" "
				   "type=\"%s:%s\" minOccurs=\"0\">\n",
				   smiNode->name, prefix, typeName );
		}
		else {		    
		    fprintSegment( f, 1, "<xsd:element name=\"%s\" "
				   "type=\"%s\" minOccurs=\"0\">\n",
				   smiNode->name, typeName );
		}
		fprintAnnotationElem( f, smiNode );
	    }	   	   
#if 0
	    fprintSegment( f, 1, "<xsd:complexType>\n");
	    fprintSegment( f, 1, "<xsd:simpleContent>\n");
	    fprintSegment( f, indent + 3 * INDENT, "<xsd:extension");
	    if( smiType->decl == SMI_DECL_IMPLICIT_TYPE  ) {
		fprintf( f, " base=\"%sType\">\n",
			 smiNode->name ); 
	    }
	    else {
		
		if( prefix ) {
		    fprintf( f, " base=\"%s:%s\">\n", prefix, typeName );
		}
		else {
		    fprintf( f, " base=\"%s\">\n", typeName );
		}
	    }
	    
	    fprintSegment( f, indent + 4 * INDENT,
			   "<xsd:attribute name=\"enc\" " , 0 );
	    fprintf( f, "type=\"smi:EncAttrType\"/>\n" );
	    fprintSegment( f, indent + 3 * INDENT, "</xsd:extension>\n");
	    
	    fprintSegment(f, indent + 2 * INDENT, "</xsd:simpleContent>\n");
	    fprintSegment( f, indent + INDENT, "</xsd:complexType>\n");
#endif /* 0 */
	}

	else if( smiType->decl == SMI_DECL_IMPLICIT_TYPE ) {
	    fprintSegment( f, 1, "<xsd:element name=\"%s\" minOccurs=\"0\">\n",
			   smiNode->name );
	    fprintAnnotationElem( f, smiNode );
	    fprintTypedef( f, smiType, NULL );
	}
	
	else {
	    if( prefix ) {
		fprintSegment( f, 1, "<xsd:element name=\"%s\" type=\"%s:%s\" "
			       "minOccurs=\"0\">\n",
			       smiNode->name, prefix, typeName );
	    }
	    else {
		fprintSegment( f, 1, "<xsd:element name=\"%s\" type=\"%s\" "
			       "minOccurs=\"0\">\n",
			       smiNode->name, typeName );
	    }
	    fprintAnnotationElem( f, smiNode );
	}
	fprintSegment( f, -1, "</xsd:element>\n"); 
	break;
    }

    case SMI_NODEKIND_NOTIFICATION:
	fprintSegment( f, 0, "<xsd:element name=\"%s\"/>\n", smiNode->name );
	break;

    default:
	fprintf( f, "<!-- Warning! Unhandled Element! No details available!\n");
	fprintf( f, "      Nodekind: %#4x -->\n\n", smiNode->nodekind );
	
    }
}


static void fprintRows( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;

    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    fprintElement( f, iterNode );
	}
    }
}


static void fprintImplicitTypes( FILE *f, SmiModule *smiModule )
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
		switch( smiType->basetype ) {
		    
		case SMI_BASETYPE_BITS:
		    if( ! getTypePrefix( smiType->name ) ) {
			fprintTypedef( f, smiType, iterNode->name );
			break;
		    }

		case SMI_BASETYPE_OCTETSTRING:
#if 0
		    if( smiType->decl == SMI_DECL_IMPLICIT_TYPE ) {
			fprintTypedef( f, INDENT, smiType, iterNode->name );
		    }
#endif /* 0 */
		    break;
		default:
		    break;
		}
	}
    }
}


static void fprintKeys( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode, *relNode;
    SmiElement *iterElem;
        
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_ROW ) ) {

	switch( iterNode->indexkind ) {

	case SMI_INDEX_INDEX:

	    /* print key */
	    fprintSegment( f, 1, "<xsd:key " );
	    fprintf( f, "name=\"%sKey\">\n", iterNode->name );
	    fprintSegment( f, 0, "<xsd:selector ");
	    fprintf( f, "xpath=\"%s\"/>\n", iterNode->name );
	    for( iterElem = smiGetFirstElement( iterNode );
		 iterElem;
		 iterElem = smiGetNextElement( iterElem ) ) {
		SmiNode *indexNode = smiGetElementNode( iterElem );
		fprintSegment( f, 0, "<xsd:field ");
		fprintf( f, "xpath=\"@%s\"/>\n", indexNode->name );
	    }
	    fprintSegment( f, -1, "</xsd:key>\n\n");
	    break;

	case SMI_INDEX_AUGMENT:

	    /* print keyref */
	    fprintSegment( f, 1, "<xsd:keyref " );
	    relNode = smiGetRelatedNode( iterNode );
	    fprintf( f, "name=\"%sKeyRef\" ", iterNode->name );
	    fprintf( f, "refer=\"%sKey\">\n", relNode->name );
	    fprintSegment( f, 0, "<xsd:selector ");
	    fprintf( f, "xpath=\"%s\"/>\n", iterNode->name );
	    for( iterElem = smiGetFirstElement( relNode );
		 iterElem;
		 iterElem = smiGetNextElement( iterElem ) ) {
		SmiNode *indexNode = smiGetElementNode( iterElem );
		fprintSegment( f, 0, "<xsd:field ");
		fprintf( f, "xpath=\"@%s\"/>\n", indexNode->name );
	    }
	    fprintSegment( f, -1, "</xsd:keyref>\n");

	    /* print unique clause */
	    fprintSegment( f, 1, "<xsd:unique " );
	    fprintf( f, "name=\"%sKeyRefUnique\">\n", iterNode->name );
	    fprintSegment( f, 0, "<xsd:selector ");
	    fprintf( f, "xpath=\"%s\"/>\n", iterNode->name );
	    for( iterElem = smiGetFirstElement( relNode );
		 iterElem;
		 iterElem = smiGetNextElement( iterElem ) ) {
		SmiNode *indexNode = smiGetElementNode( iterElem );
		fprintSegment( f, 0, "<xsd:field ");
		fprintf( f, "xpath=\"@%s\"/>\n", indexNode->name );
	    }
	    fprintSegment( f, -1, "</xsd:unique>\n\n");
	    break;

	case SMI_INDEX_REORDER:
	case SMI_INDEX_SPARSE:
	case SMI_INDEX_EXPAND:
	    /* SMIng, not implemented yet */
	    break;

	default:
	    fprintf( f, "<!-- Error: Unknown index type -->\n" );
	    break;
	}
    }
}


static void fprintImports( FILE *f, SmiModule *smiModule )
{
    SmiImport *iterImp;
    char *lastModName = "";
    
    fprintSegment( f, 0, "<xsd:import namespace=\"http://www.ibr.cs.tu-bs.de/projects/libsmi/xsd/smi\" schemaLocation=\"http://www.ibr.cs.tu-bs.de/projects/libsmi/xsd/smi.xsd\"/>\n");
    for( iterImp = smiGetFirstImport( smiModule );
	 iterImp;
	 iterImp = smiGetNextImport( iterImp ) ) {
	/* assume imports to be ordered by module names */
	if( strcmp( iterImp->module, lastModName ) ) {
	    fprintSegment( f, 0, "<xsd:import ");
	    fprintf( f, "namespace=\"%s%s\" schemaLocation=\"%s%s.xsd\"/>\n",
		    schemaLocation, iterImp->module,
		    schemaLocation, iterImp->module );
	}
	lastModName = iterImp->module;
    }
    fprintf( f, "\n");
   
}


static void fprintGroupTypes( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode, *iterNode2;

    /* scalar groups */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_SCALAR ) ) {
	    fprintSegment(f, 1, "<xsd:complexType name=\"%sType\">\n",
			  iterNode->name);
	    fprintSegment( f, 1, "<xsd:sequence>\n");
	    for( iterNode2 = smiGetFirstChildNode( iterNode );
		 iterNode2;
		 iterNode2 = smiGetNextChildNode( iterNode2 ) ) {
		if( iterNode2->nodekind == SMI_NODEKIND_SCALAR ) {
		    fprintElement( f, iterNode2 );
		}
	    }
	    fprintSegment( f, -1, "</xsd:sequence>\n");
	    fprintSegment(f, -1, "</xsd:complexType>\n");
	}
    }   

    /* rows */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    fprintComplexType( f, iterNode, iterNode->name );
	    
	}
    }   
}

static void fprintNotifications( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;
    
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NOTIFICATION );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NOTIFICATION ) ) {
	fprintElement( f, iterNode );
    }
}


static void fprintModuleHead(FILE *f, SmiModule *smiModule)
{
    if( smiModule->description ) {
	fprintSegment(f, 1, "<xsd:annotation>\n");
	fprintDocumentation(f, smiModule->description);
	fprintSegment(f, -1, "</xsd:annotation>\n\n");
    }
    
}


static void fprintContext(FILE *f, SmiModule *smiModule)
{
    SmiNode *iterNode;

    fprintSegment( f, 1, "<xsd:element name=\"snmp-data\">\n");
    fprintSegment( f, 1, "<xsd:complexType>\n");
    fprintSegment( f, 1, "<xsd:sequence>\n");
    fprintSegment( f, 1, "<xsd:element name=\"context\" "
		   "minOccurs=\"0\" maxOccurs=\"unbounded\">\n");
    fprintSegment( f, 1, "<xsd:complexType>\n");
    fprintSegment( f, 1, "<xsd:sequence>\n");

    /* scalar groups */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_SCALAR ) ) {
	    fprintSegment(f, 0, "<xsd:element name=\"%s\" type=\"%sType\" minOccurs=\"0\"/>\n",
			  iterNode->name, iterNode->name);
	}
    }   

    /* rows */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    fprintSegment(f, 0, "<xsd:element name=\"%s\" type=\"%sType\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>\n",
			  iterNode->name, iterNode->name);
	}
    }   



    
    fprintSegment( f, -1, "</xsd:sequence>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"agent\" type=\"xsd:NMTOKEN\" use=\"required\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"community\" type=\"xsd:NMTOKEN\" use=\"required\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"port\" type=\"xsd:unsignedInt\" use=\"required\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"time\" type=\"xsd:dateTime\" use=\"required\"/>\n");
    fprintSegment( f, -1, "</xsd:complexType>\n");
    fprintSegment( f, -1, "</xsd:element>\n");
    fprintSegment( f, -1, "</xsd:sequence>\n");
    fprintSegment( f, -1, "</xsd:complexType>\n");
    fprintSegment( f, -1, "</xsd:element>\n\n");
}


static void fprintTypedefs(FILE *f, SmiModule *smiModule)
{
    int		 i;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(smiModule);
	smiType;
	i++, smiType = smiGetNextType(smiType)) {
	fprintf(f, "\n");
	fprintTypedef(f, smiType, smiType->name);
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



static void dumpXsdModules(int modc, SmiModule **modv, int flags, char *output)
{
    int  i;
    FILE *f = stdout;
    SmiImport *iterImp;
    char *lastModName = "";

    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

    for (i = 0; i < modc; i++) {

	fprintf(f, "<?xml version=\"1.0\"?>\n"); 
	fprintf(f, "<!-- This module has been generated by smidump "
	       SMI_VERSION_STRING ". Do not edit. -->\n");
	
	fputs( "<!-- WARNING: files located at ", f );
	fprintf(f, "%s ", schemaLocation);
	fputs( "are subject to changes. -->\n\n", f );

	fprintSegment(f, 1, "<xsd:schema ");
	fprintf(f, "targetNamespace=\"%s%s\"\n",
		schemaLocation, modv[i]->name);
    
	fprintf(f, "            xmlns=\"%s%s\"\n",
		schemaLocation, modv[i]->name);
/*      fprintf(f, "            xmlns:xmn=\"http://www.w3.org/XML/1998/namespace\"\n"); */
	fprintf(f, "            xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\n");
	fprintf(f, "            xmlns:smi=\"%ssmi\"\n", schemaLocation);
    
	for( iterImp = smiGetFirstImport( modv[i] );
	     iterImp;
	     iterImp = smiGetNextImport( iterImp ) ) {
	    registerType( iterImp->name, iterImp->module );
	    /* assume imports to be ordered by module names */
	    if( strcmp( iterImp->module, lastModName ) ) {
		fprintf( f, "            xmlns:%s=\"%s%s\"\n",
			 iterImp->module, schemaLocation, iterImp->module );
	    }
	    lastModName = iterImp->module;
	}
  
	fprintf(f, "            xml:lang=\"en\"\n");
	fprintf(f, "            elementFormDefault=\"qualified\"\n");
	fprintf(f, "            attributeFormDefault=\"unqualified\">\n\n");

	fprintModuleHead(f, modv[i]);
	fprintImports(f, modv[i]);
	fprintContext(f, modv[i]);
	fprintGroupTypes(f, modv[i]);
	fprintImplicitTypes(f, modv[i]);
	fprintTypedefs(f, modv[i]);

	fprintSegment(f, -1, "</xsd:schema>\n");
    }

    if (fflush(f) || ferror(f)) {
	perror("smidump: write error");
	exit(1);
    }

    if (output) {
	fclose(f);
    }
}



static void dumpXsdContainer(int modc, SmiModule **modv, int flags,
			     char *output)
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

    fprintf(f, "<?xml version=\"1.0\"?>\n"); 
    fprintf(f, "<!-- This module has been generated by smidump "
	    SMI_VERSION_STRING ". Do not edit. -->\n");
	
    fputs( "<!-- WARNING: files located at ", f );
    fprintf(f, "%s ", schemaLocation);
    fputs( "are subject to changes. -->\n\n", f );
    
    fprintSegment(f, 1, "<xsd:schema ");
    fprintf(f, "targetNamespace=\"%s%s\"\n", schemaLocation, containerBasename);
    fprintf(f, "            xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\n");
    fprintf(f, "            xmlns:smi=\"%ssmi\"\n", schemaLocation);
    for (i = 0; i < modc; i++) {
	fprintf(f, "            xmlns:%s=\"%s%s\"\n",
		modv[i]->name, schemaLocation, modv[i]->name);
    }
    
    fprintf(f, "            xml:lang=\"en\"\n");
    fprintf(f, "            elementFormDefault=\"qualified\"\n");
    fprintf(f, "            attributeFormDefault=\"unqualified\">\n\n");

    for (i = 0; i < modc; i++) {
	
	
    }

    fprintSegment(f, -1, "</xsd:schema>\n");

    if (fflush(f) || ferror(f)) {
	perror("smidump: write error");
	exit(1);
    }

    if (output) {
	fclose(f);
    }
}



static void dumpXsd(int modc, SmiModule **modv, int flags, char *output)
{
    /* register smi basetypes */
    registerType( "Integer32", "smi" );
    registerType( "ObjectIdentifier", "smi" );
    registerType( "OctetString", "smi" );
    registerType( "Unsigned32", "smi" );
    registerType( "Unsigned64", "smi" );

    /* make sure url ends with '/' */
    if( schemaLocation[ strlen( schemaLocation ) - 1 ] != '/' ) {
	schemaLocation =
	    realloc(schemaLocation, strlen(schemaLocation)+2);
	strcat( schemaLocation, "/" );
    }
	
    if (container) {
	dumpXsdContainer(modc, modv, flags, output);
    } else {
	dumpXsdModules(modc, modv, flags, output);
    }

    /* delete type-prefix-mapping */
    free( typePrefixes ); /* XXX: TODO: free all malloced types in a loop */
}



void initXsd()
{

    static SmidumpDriverOption opt[] = {
	{ "schema-url", OPT_STRING, &schemaLocation, 0,
	  "URI prefix for schema definitions and namespaces" },
	{ "container", OPT_FLAG, &container, 0,
	  "generate a container schema" },
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
