/*
 * dump-xsd.c --
 *
 *      Operations to dump SMI module information as XML schema definitions.
 *
 * Copyright (c) 2001 J. Schoenwaelder, Technical University of Braunschweig.
 *           (c) 2002 T. Klie, Technical University of Braunschweig.
 *           (c) 2002 F. Strauss, Technical University of Braunschweig.
 *           (c) 2007 T. Klie, Technical University of Braunschweig.
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
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"
#include "smidump.h"


extern int smiAsprintf(char **strp, const char *format, ...);

#define  INDENT		2    /* indent factor */

static int ind = 0;

#ifndef MIN
#define MIN(a,b) ((a)) < ((b)) ? ((a)) : ((b))
#endif /* #ifndef MIN */

/* definition of commonly used display hints */
#define LIBSMI_DH_TP_IPV4Z_ADDR "1d.1d.1d.1d%4d:2d"
#define LIBSMI_DH_UDP_IPV4 "1d.1d.1d.1d/2d"
#define LIBSMI_DH_IPV4Z_ADDR "1d.1d.1d.1d%4d"
#define LIBSMI_DH_IPV4_ADDR "1d.1d.1d.1d"
#define LIBSMI_DH_LDP_ID "1d.1d.1d.1d:2d"
#define LIBSMI_DH_IPV6_ADDR "2x:2x:2x:2x:2x:2x:2x:2x"
#define LIBSMI_DH_IPV6Z_ADDR "2x:2x:2x:2x:2x:2x:2x:2x%4d"
#define LIBSMI_DH_TP_IPV6_ADDR "0a[2x:2x:2x:2x:2x:2x:2x:2x]0a:2d"
#define LIBSMI_DH_TP_IPV6Z_ADDR "0a[2x:2x:2x:2x:2x:2x:2x:2x%4d]0a:2d"
#define LIBSMI_DH_HEX "1x"
#define LIBSMI_DH_HEX_COLON "1x:"
#define LIBSMI_DH_DOUBLE_HEX_COLON "2x:"
#define LIBSMI_DH_ASCII "255a"
#define LIBSMI_DH_UTF8 "255t"
#define LIBSMI_DH_ASCII_CHAR "1a"
#define LIBSMI_DH_DATETIME "2d-1d-1d,1d.1d.1d,1a1d:1d"
#define LIBSMI_DH_DATETIME2 "2d-1d-1d,1d:1d:1d.1d,1a1d:1d"
#define LIBSMI_DH_IPX_ADDR "4x.1x:1x:1x:1x:1x:1x.2d"


/* list of parts of a display hint */
typedef struct DH {
    unsigned int number;
/*    int repeat;    //  repeat not yet supported */
    char type;
    char separator[3];
/*    char repTerm;  // repeat not yet supported */
    struct DH *next;
} DH;


static char *schemaLocation = "http://www.ibr.cs.tu-bs.de/projects/libsmi/xsd/";
static int container = 0;
static char *containerBasename = "container";
static int *nestAugmentedTables = 0;
static int *nestSubtables = 0;

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



/* some forward declarations */
static void fprintElement( FILE *f, SmiNode *smiNode, SmiNode *parentNode );
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
#if 0
static char
*getStringValue(SmiValue *valuePtr, SmiType *typePtr)
{
    static char    s[1024];
    char           ss[9];
    int		   n;
    unsigned int   i;
    SmiNamedNumber *nn;
    SmiNode        *nodePtr;
    
    s[0] = 0;
    
    switch (valuePtr->basetype) {
    case SMI_BASETYPE_UNSIGNED32:
	sprintf(s, "%lu", valuePtr->value.unsigned32);
	break;
    case SMI_BASETYPE_INTEGER32:
	sprintf(s, "%ld", valuePtr->value.integer32);
	break;
    case SMI_BASETYPE_UNSIGNED64:
	sprintf(s, UINT64_FORMAT, valuePtr->value.unsigned64);
	break;
    case SMI_BASETYPE_INTEGER64:
	sprintf(s, INT64_FORMAT, valuePtr->value.integer64);
	break;
    case SMI_BASETYPE_FLOAT32:
    case SMI_BASETYPE_FLOAT64:
    case SMI_BASETYPE_FLOAT128:
	break;
    case SMI_BASETYPE_ENUM:
	for (nn = smiGetFirstNamedNumber(typePtr); nn;
	     nn = smiGetNextNamedNumber(nn)) {
	    if (nn->value.value.unsigned32 == valuePtr->value.unsigned32)
		break;
	}
	if (nn) {
	    sprintf(s, "%s", nn->name);
	} else {
	    sprintf(s, "%ld", valuePtr->value.integer32);
	}
	break;
    case SMI_BASETYPE_OCTETSTRING:
	for (i = 0; i < valuePtr->len; i++) {
	    if (!isprint((int)valuePtr->value.ptr[i])) break;
	}
	if (i == valuePtr->len) {
	    sprintf(s, "\"%s\"", valuePtr->value.ptr);
	} else {
            sprintf(s, "0x%*s", 2 * valuePtr->len, "");
            for (i=0; i < valuePtr->len; i++) {
                sprintf(ss, "%02x", valuePtr->value.ptr[i]);
                strncpy(&s[2+2*i], ss, 2);
            }
	}
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	for (i = 0, n = 0; i < valuePtr->len * 8; i++) {
	    if (valuePtr->value.ptr[i/8] & (1 << (7-(i%8)))) {
		if (n)
		    sprintf(&s[strlen(s)], ", ");
		n++;
		for (nn = smiGetFirstNamedNumber(typePtr); nn;
		     nn = smiGetNextNamedNumber(nn)) {
		    if (nn->value.value.unsigned32 == i)
			break;
		}
		if (nn) {
		    sprintf(&s[strlen(s)], "%s", nn->name);
		} else {
		    sprintf(s, "%d", i);
		}
	    }
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	nodePtr = smiGetNodeByOID(valuePtr->len, valuePtr->value.oid);
	if (nodePtr) {
	    sprintf(s, "%s", nodePtr->name);
	} else {
	    strcpy(s, "");
	    for (i=0; i < valuePtr->len; i++) {
		if (i) strcat(s, ".");
		sprintf(&s[strlen(s)], "%u", valuePtr->value.oid[i]);
	    }
	}
	break;
    }

    return s;
}
#endif /* 0 */

static int smiPow( int base, unsigned int exponent )
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

static int getBracketLevel( char *bracketString )
{
  int level = 0;
  char *c = bracketString;
  while( level >= 0 && *c != '\0' ) {
    switch( *c ) {
    case '(':
      level++;
      break;
    case ')':
      level--;
      break;
    default:
      break;
    }
    c++;
  }

  return level;
}

/* initialize a display hint structre with zeros */
static void initDH( struct DH *dh )
{
    /* init with NULLs */
	dh->number = 0;
	/*dh->repeat = 0; repeat not yet supported */
	dh->type = 0;
	dh->separator[0] = '\0';
	dh->separator[1] = '\0';
	dh->separator[2] = '\0';
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
	case 't':
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
	      pos++;
	    }
	    else {
	      if( hint[ pos++ ] == '.' ) {
		iterDH->separator[0] = '\\';
		iterDH->separator[1] = '.';
	      }
	      else
		iterDH->separator[0] = hint[ pos ];
	    }
	    if( isdigit( hint[ pos ] ) || hint[ pos ] == '*' ) {
		iterDH = iterDH->next;
	    }
	    break;
	}	
    }
    return ret;
}

static char *getSimpleCharFacet_XSD( char dhType, unsigned int number, 
							  unsigned int minL, unsigned int maxL ) {
    char *ret;
    char *baseRegExp;
    
    /* determine base regular expression, depending on display hint type */
    switch( dhType ) {
    	case 'a':
    		baseRegExp = "\\{isBasicLatin}";
    		break;
    	case 't':
    		baseRegExp = ".";
    		break;
    	default:
    		fprintf( stderr, 
    				"displayHint.c: Error: unknown display hint type\n" );
    		return NULL;
    } 
        
	smiAsprintf( &ret, "%s{%d,%d}", baseRegExp, minL, MIN( number, maxL ) );
	return ret;
}


/* check if the hint is known and (if so) return the XSD translation */
 static char *getKnownDisplayHint_XSD( const char *hint, 
 								SmiUnsigned32 *lengths, 
 								unsigned int numSubranges ) {
 	int i;
 	char *ret = "";
 	
 	if( ! strcmp( hint, LIBSMI_DH_TP_IPV4Z_ADDR ) )
 		/* this is a simplyfied restriction. 
 		 * We could restrict more, 
 		 * but then, the pattern will be very hard to read / understand */
 		return "(\\d{1,3}).){3}\\d{1,3})%\\d{1,10}:\\d{1,5}";
 	
 	if( ! strcmp( hint, LIBSMI_DH_UDP_IPV4 ) ) 
 		return "(\\d{1,3}).){3}\\d{1,3})/\\d{1,5}";
 	
 	if( ! strcmp( hint, LIBSMI_DH_IPV4Z_ADDR ) )
 		return "(\\d{1,3}).){3}\\d{1,3})%\\d{1,10}";
 	
 	if( ! strcmp( hint, LIBSMI_DH_IPV4_ADDR ) )
 		return "(\\d{1,3}).){3}\\d{1,3})";
 		
 	if( ! strcmp( hint, LIBSMI_DH_LDP_ID ) )
 		return "(\\d{1,3}).){3}\\d{1,3}):\\d{1,5}";
 		
 	if( ! strcmp( hint, LIBSMI_DH_IPV6_ADDR ) )
 		return "([\\dA-Fa-f]{2}:){7}[\\dA-Fa-f]{2}";
 		
 	if( ! strcmp( hint, LIBSMI_DH_IPV6Z_ADDR ) )
 		return "([\\dA-Fa-f]{2}:){7}[\\dA-Fa-f]{2}%\\d{1,10}";
 		
 	if( ! strcmp( hint, LIBSMI_DH_TP_IPV6_ADDR ) )
 		return "\\[([\\dA-Fa-f]{2}:){7}[\\dA-Fa-f]{2}\\]:\\d{1,5}";
 		
 	if( ! strcmp( hint, LIBSMI_DH_TP_IPV6Z_ADDR ) )
 		return "\\[([\\dA-Fa-f]{2}:){7}[\\dA-Fa-f]{2}%\\d{1,10}\\]:\\d{1,5}";
 	
 	if( ! strcmp( hint, LIBSMI_DH_HEX ) )
 		return "[0-9A-Fa-f]{2}";
 		
 	if( ! strcmp( hint, LIBSMI_DH_HEX_COLON ) )
 		return "[0-9A-Fa-f]{2}:";
 	
 	if( ! strcmp( hint, LIBSMI_DH_DOUBLE_HEX_COLON ) )
 		return "[0-9A-Fa-f]{4}:";
 		
 	if( ! strcmp( hint, LIBSMI_DH_ASCII ) ) {
 		switch( numSubranges ) { 		
 		case 0: 
 			return getSimpleCharFacet_XSD( 'a', 255, 0, 255 );
 		case 1: 			
 			return getSimpleCharFacet_XSD( 'a', 255, lengths[0], lengths[1] );
 		default:
 			ret = "("; 
 			for( i=0; i < numSubranges * 2; i+=2 ) {
 				if( i ) smiAsprintf( &ret, "%s)|(", ret );
 				smiAsprintf( &ret, "%s%s", ret,
 							getSimpleCharFacet_XSD( 'a', 255, 
 												    lengths[i], lengths[i+1]));
 									
 			}
 			smiAsprintf( &ret, "%s)", ret );
 			return ret;
 		}
 	}
 	 		
 	if( ! strcmp( hint, LIBSMI_DH_UTF8 ) ) {
 		switch( numSubranges ) { 		
 		case 0: 
 			return getSimpleCharFacet_XSD( 't' , 255, 0, 255 );
 		case 1: 			
 			return getSimpleCharFacet_XSD( 't' , 255, lengths[0], lengths[1]);
 		default:
 			ret = "("; 
 			for( i=0; i < numSubranges * 2; i+=2 ) {
 				if( i ) smiAsprintf( &ret, "%s)|(", ret );
 				smiAsprintf( &ret, "%s%s", 
 							 ret, getSimpleCharFacet_XSD( 't', 255,
 														  lengths[i], 
 														  lengths[i+1] ) ); 				
 			}
 			smiAsprintf( &ret, "%s)", ret );
 			return ret;
 		}
 	}
 		
 	if( ! strcmp( hint, LIBSMI_DH_ASCII_CHAR ))
 		return "\\p{IsBasicLatin}{0,1}";

	/* we assume here date-time-value, i.e. the number of digits are fixed */
	if( ! strcmp( hint, LIBSMI_DH_DATETIME ) )
		return "\\d{4}-\\d{2}-\\d{2},(\\d{2}.){2}\\d{2},\\p{IsBasicLatin}\\d{2}:\\d{2}";
		
	if( ! strcmp( hint, LIBSMI_DH_DATETIME2 ) ) {
		if( numSubranges >1 )
			/* there are more subranges, 
			 * assume: lengths = 8 | lengths = 11 
			 * thus: last 3 octets (time zone info) is optional */ 
			return "\\d{4}-\\d{2}-\\d{2},(\\d{2}:){2}\\d{2}(,[\\+\\-]\\d{2}:\\d{2}){0,1}";
		return "\\d{4}-\\d{2}-\\d{2},(\\d{2}:){2}\\d{2},[\\+\\-]\\d{2}:\\d{2}";
	}
	if( !strcmp( hint, LIBSMI_DH_IPX_ADDR ) )
		return "[0-9A-Fa-f]{8}.([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}.\\d{1,4}";
		
	else return NULL;		
 }



/* build a regexp pattern from a display hint 
 * 
 * Some documentation: First of all, it is checked whether the display hint 
 * matches againts one of the well known display hints. If so, a direct mapping 
 * is applied. Note that most cases ignore any additional length information 
 * (except DATETIME and ASCII / UTF8).
 * If no match can be found, the display hint is parsed and seperated 
 * into a list of blocks (DH struct). A display hint block consists of 
 * o the number of octets to be used (unsigned int "number"),
 * o a charakter representing the type (char "type"),
 * o the separator (char[3] "separator"),
 * o a pointer to the next display hint block in the list.
 * 
 * Then, it is checked whether it is a "simple" display hint, 
 * i.e. is hint that consists only of a single block and 
 * ASCII / UTF-8 restriction (type 'a' or 't'; as in 100a, 127t).
 * (Maybe this can be extended for other simple hints.)
 * 
 * If all of the above matches fail, the hints blocks are converted 
 * step by step using the these Mappings:
 * o ASCII: \p{IsBasicLatin}
 * o UTF-8: .
 * o Binary: (0|1){8} (8 digits (0 or 1))
 * o Decimal: [0-9]{3} (3 decimal digits)
 * o Octal: [0-7]{3} (3 octal digits)
 * o Hexadecimal: [0-9A-Fa-f]{2} (2 hexadecimal digits)
 * The conversion is not straight forward, 
 * because there may be also (even several) length restrictions 
 * and we have to count (or try to estimate) the used octets. Thus, 
 * it is sometimes necessary to generate multiple patterns 
 * with different length specification.
 * 
 * Unfortunately, the conversion still is quite difficult to understand 
 * although I added some comments. The good news is, that all IETF MIBs 
 * (and all other MIBs coming with libsmi) can be converted "the easy way" 
 * (i.e. without the difficult conversion loop).*/
static char* getStrDHType( char *hint,
			   SmiUnsigned32 *lengths, unsigned int numSubranges  )
{
    unsigned int i = 0;
    char *ret = lengths[ i ] ? "(" : "((";
    char *r2 = getKnownDisplayHint_XSD( hint, lengths, numSubranges );
  
    if( r2 ) {
    /*	if( lengths[ i ] && lengths[i+1] < 65535 ) {    		
    		fputs( "LENGTH: ", stdout );
    		fprintf( stdout, "min: %d, max: %d ; ", lengths[i], lengths[i+1] );
    		fprintf( stdout, "hint was: %s\n", hint );
    	} 
    	if( numSubranges >  1 ) fputs( "MORE_SUBRANGES\n", stdout );*/
    	return r2;	
    }
    DH *dh = parseDH( hint );
    int bl = 0;

    if(! dh ) return NULL;
    
    /* check if we have a "simple" display hint (e.g. "100a") */
    if( !dh->next && ( dh->type == 'a' || dh->type == 't' ) ) {
    	switch( numSubranges ) { 		
 		case 0: 
 			return getSimpleCharFacet_XSD( dh->type , dh->number, 0, dh->number );
 		case 1: 			
 			return getSimpleCharFacet_XSD( dh->type , dh->number, lengths[0], lengths[1]);
 		default:
 			ret = "("; 
 			for( i=0; i < numSubranges * 2; i+=2 ) {
 				if( i ) smiAsprintf( &ret, "%s)|(", ret );
 				smiAsprintf( &ret, "%s%s", 
 							 ret, getSimpleCharFacet_XSD( dh->type, dh->number,
 														  lengths[i], 
 														  lengths[i+1] ) ); 				
 			}
 			smiAsprintf( &ret, "%s)", ret );
 			return ret;
 		}   		
    }

	/* no "easy match was possible, so start the "hard" loop */
    do {
		unsigned int octetsUsed = 0;
		DH *iterDH;
	
		for( iterDH = dh; iterDH; iterDH = iterDH->next ) {
	    	char *baseRegexp = NULL;

	    	switch( iterDH->type ) {

	    	case 'a':
				/* ascii character */
				baseRegexp = "(\\p{IsBasicLatin})";
				break;
				
			case 't':
				/* utf-8 character */
				baseRegexp = ".";
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

	    	default:
				fputs( "smidump: Warning: unknown type of display-hint",
		       			stderr );
	    	}	   
	    

	    	if( iterDH->number < lengths[ i ] ) {
				/* there are more octets to come */
				if( iterDH->type == 'd' ) {
		    		/* decimal number needs to be treated differently */
		    		if( iterDH->next ){
		    			/* we still have another diplay hint block coming */
		   				smiAsprintf( &ret, "%s(0|[1-9](([0-9]){0,%d}))",
					    			 ret,
				    	 			 numDigits(smiPow(255,
					    						      iterDH->number ))-1 );
					    						      
						/* adjust number of used digits */
						octetsUsed += iterDH->number;
						
						if( octetsUsed >= lengths[ i + 1 ] ) {
							/* maximum number of octets used,
			   	   			   we must exit the loop */			    
			    			break;
						}
						
						/* add separator char */
						if( iterDH->separator ) {
			    			smiAsprintf( &ret, "%s%s",ret, iterDH->separator );
						}
		 			}
		    		else {
		    			/* no orther display hint coming up. 
		    	 		 * we are at the last iteration */		
						smiAsprintf( &ret, "%s((0|[1-9](([0-9]){0,%d})",
					    			 ret,
				    	 			 numDigits( smiPow( 255,
					     						iterDH->number ) ) - 1 );			
						/* add separator char */
						if( iterDH->separator ) {
			  				smiAsprintf( &ret, "%s%s",
										 ret, iterDH->separator );
						}
						if( lengths[ i+1 ] - 1 - octetsUsed ) {
							/* not all digits for maximum string length (length[i+1 ])
				 			 * have been used, so we have to add some digits */
		    				smiAsprintf(&ret,
				 						"%s){%u,%u})(0|[1-9](([0-9]){0,%d}))",
				 						ret, lengths[ i ] - 1 - octetsUsed,
				 						lengths[ i+1 ] - 1 - octetsUsed,
				 						numDigits(
				     					smiPow( 255, iterDH->number ))- 1 );							
						}
						else {
							/* maximum number of digets have been used,
				 			 * so let's terminate the pattern for this round*/
		    				smiAsprintf( &ret, "%s)", ret );
						}
			
						/* adjust the used digit counter */
						octetsUsed += iterDH->number;
			
						if( octetsUsed >= lengths[ i + 1 ] ) {
		  					/* maximum number of octets used, we must exit the loop */
		    				break;
						}
	    			}
				}
				else {
					/* type other than decimal */
	    			if( iterDH->next ){
	    				/* there will be another display hint block */
						smiAsprintf( &ret, "%s(%s{%d})",
				     				 ret,
				     				 baseRegexp, iterDH->number );

						/* adjust number of used octets */
						octetsUsed += iterDH->number;
						if( octetsUsed >= lengths[ i + 1 ] ) {
							/* maximum number of octets used,
			   				   we must exit the loop */			    
			    			break;
						}

						/* add separator char */
						if( iterDH->separator ) {
			    			smiAsprintf( &ret, "%s%s", ret, iterDH->separator );
						}
		    		}
		    		else {
		    			/* we are the last display hint block */			
						smiAsprintf( &ret, "%s(%s",
				  					 ret, baseRegexp );

						/* add separator char */
						if( iterDH->separator ) {
			    			smiAsprintf( &ret, "%s%s", ret, iterDH->separator );
						}

						smiAsprintf( &ret, "(%s){%u,%u})%s",
				     				 ret, lengths[ i ] - 1, lengths[ i+1 ] - 1,
				     				 baseRegexp );
			
						/* adjust the number of used octets */
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
			    		/* we are not using the maximun number of octets */
						smiAsprintf( &ret, "%s(0|[1-9]([0-9]{0,%d}))",
				    			 	 ret,
				     			 	 numDigits( smiPow( 255, iterDH->number ) ) );

						/* adjust the number of used octets */
						octetsUsed += lengths[ i ];
						if( octetsUsed >= lengths[ i + 1 ] ) {
				    		/* the maximum number of octets have been reached,
			    	   	   	   we must exit the loop */
			    			break;
						}

						/* add separator char */
						if( iterDH->separator ) {
					    	smiAsprintf( &ret, "%s%s", ret, iterDH->separator );
						}						
					}
					else {
						/* we have used the maximum number of octets */
						smiAsprintf( &ret, "%s(0|[1-9]([0-9]{0,%d})",
								     ret,
								     numDigits( smiPow( 255, lengths[ i+1 ] ) ) );
					}
				}
				else {
					/* type is not decimal */
		    		smiAsprintf( &ret, "%s(%s",  ret, baseRegexp );
		    		if( iterDH->next ) {
			    		/* there will be another display hint block */
						if( iterDH->separator ) {
				    		smiAsprintf( &ret, "%s%s", ret, iterDH->separator );
						}
						if( ! lengths[ i ] && lengths[ i+1 ] == 65535 ) {
							/* minLength = 0, maxLength = 65535, 
							 * i.e. no restriction at all */
			    			smiAsprintf( &ret, "%s)*",ret );
						}
						else{
							/* we have a different length restriction */
				    		smiAsprintf( &ret, "%s){%u,%u}",ret, lengths[ i ],
						 				 MIN( iterDH->number,
						      			 	  lengths[ i + 1] ) - 1 );
						}
					
						/* adjust the number of used octets */
						octetsUsed += lengths[ i ];
						if( octetsUsed >= lengths[ i + 1 ] ) {
			    			/* the maximum number of octets have been reached,
			       			   we must exit the loop */
			    			break;
						}						
		    		}
		    		else {
		    			/* we are the ast display hint block */
						octetsUsed += lengths[ i ];
						if( iterDH->separator &&
			    			octetsUsed < lengths[ i + 1 ] ) {
							/* we have a separator char and 
						 	 * still not reached the maximum number of octets */
			    			if( ! lengths[ i ] && lengths[ i+1 ] == 65535 ) {
			    				/* we have no length restriction */
								smiAsprintf( &ret, "%s%s)*%s",
					     					 ret, iterDH->separator, baseRegexp );
			    			}
			    			else {
				    			/* we have a length restriction */
								smiAsprintf( &ret, "%s%s){%u,%u}%s",
					     				 	 ret, iterDH->separator,
					     				 	 lengths[ i ], lengths[ i + 1] - 1,
					     				 	 baseRegexp );
			    			}
						}
						else {
							/* we don't have a separator char or
						 	 * have used the maximum number of octets */
			    			if( ! lengths[ i ] && lengths[ i+1 ] == 65535 ) {
			    				/* no lengths restriction */
								smiAsprintf( &ret, "%s)*%s",
					     				 	 ret, iterDH->separator ); 
					     				 	// TBD: what, if there is no separator ???
			    			}
			    			else {
			    				/* we have a length restriction */
								smiAsprintf( &ret, "%s){%u,%u}%s",
					     					 ret, lengths[ i ],
					     					 lengths[ i + 1],
					     				 	 iterDH->separator );
					     				 	// TBD: what, if there is no separator ???
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
		/* adjust the "pointer" for the lenghts array */
		i += 2;

		if( i < numSubranges  * 2 ) {
			/* we are not the last subrange, so we have to extend the pattern */
	    	smiAsprintf( &ret, "%s)|(", ret );
		}
		else {
			/* we are the last subrange */
	    	smiAsprintf( &ret, "%s)", ret );
	    	if( ! lengths[ 0 ] ) {
				smiAsprintf( &ret, "%s){0,1}", ret );
	    	}
		}
	} while( i < numSubranges * 2 );
    
    /* check if all brackets have been closed */
    if( getBracketLevel( ret ) ) {
  		bl = getBracketLevel( ret );
      	fprintf( stderr, "%d\n", bl );
      	if( bl > 0 ) {
			// TODO: add a warning that brackets have been added
			for( bl; bl; bl--) {
	  			smiAsprintf( &ret, "%s)", ret );
			}
      	}
      	else {
			// TODO: some error handling
      	}
    }
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
	  fprintSegment( f, 0, "<xsd:fractionDigits value=\"%d\"/>\n", offset );
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
			   (int)min / smiPow( 10, offset ), 
			   abs( (int)min % smiPow( 10, offset ) ) );
	} else {
	    fprintSegment( f, 0, "<xsd:minInclusive value=\"%d\"/>\n",
			   (int)min );
	}

	/* print maximum value */
	if( useDecPoint ) {
	    fprintSegment( f, 0, "<xsd:maxInclusive value=\"%d.%d\"/>\n", 
			   (int)max / smiPow( 10, offset ), 
			   abs( (int)max % smiPow( 10, offset ) ) );
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
		if(  parent->format ) {

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
		
		
		else if( smiType->name &&
			 ! strcmp( smiType->name, "IpAddress" ) ) {
		    SmiUnsigned32 lengths[] = {4, 4};
		    lengths[0] = 4; lengths[1] = 4;
		    fprintSegment( f, 1, "<xsd:restriction base=\"xsd:string\">\n" );
		    fprintSegment( f, 0, "<xsd:pattern "
				   "value=\"(0|[1-9](([0-9]){0,2}))."
				   "(0|[1-9](([0-9]){0,2}))."
				   "(0|[1-9](([0-9]){0,2}))."
				   "(0|[1-9](([0-9]){0,2}))\"/>\n" );
		    fprintSegment( f, -1, "</xsd:restriction>\n");
		}
		
		else {
		    
		    
		    char *prefix = getTypePrefix( parent->name );

		    if( prefix ) {
			fprintSegment( f, 1, "<xsd:restriction base=\"%s:%s\">\n",
				       prefix, parent->name );
		    } else {
			fprintSegment( f, 1, "<xsd:restriction base=\"%s\">\n",
				       parent->name );
		    }

		    /* print length restriction */
		    if( minLength > 0 )
			fprintSegment( f, 0, "<xsd:minLength value=\"%d\"/>\n",
				       (int)minLength );
		    if( maxLength > -1 )
			fprintSegment( f, 0, "<xsd:maxLength value=\"%d\"/>\n",
				       (int)maxLength );		    
		    fprintSegment( f, -1, "</xsd:restriction>\n");
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
	max = LIBSMI_UINT64_MAX;

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


static void fprintBitList( FILE *f, SmiType *smiType )
{
    fprintSegment( f, 1, "<xsd:restriction>\n" );
    fprintSegment( f, 1, "<xsd:simpleType>\n" );
    fprintSegment( f, 1, "<xsd:list>\n" );  
    fprintSegment( f, 1, "<xsd:simpleType>\n" );
    fprintRestriction( f, smiType );
    fprintSegment( f, -1, "</xsd:simpleType>\n" );
    fprintSegment( f, -1, "</xsd:list>\n" );
    fprintSegment( f, -1, "</xsd:simpleType>\n");
    fprintSegment( f, 0, "<xsd:maxLength value=\"%d\"/>\n",
		   getNamedNumberCount( smiType ) );
    fprintSegment( f, -1, "</xsd:restriction>\n");
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
	max = LIBSMI_UINT64_MAX;

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

static void fprintLengths( FILE *f, SmiType *smiType )
{
    SmiRange *smiRange = smiGetFirstRange( smiType );
    unsigned int numSubRanges = getNumSubRanges( smiType ),
	lp = 0;
    SmiUnsigned32 *lengths = xmalloc(  2 * numSubRanges * 4 );
    
    /* write lengths to the array */
    for( smiRange = smiGetFirstRange( smiType );
	 smiRange;
	 smiRange = smiGetNextRange( smiRange ) ) {
	lengths[ lp++ ] = smiRange->minValue.value.unsigned32;
	lengths[ lp++ ] = smiRange->maxValue.value.unsigned32;
    }
    
    if( numSubRanges ) {
	fprintSegment( f, 1, "<lengths>\n" );
    }
    for( lp = 0; lp < numSubRanges * 2; lp = lp + 2 ) {
	fprintSegment( f, 0, "<length min=\"%u\" max=\"%u\"/>\n",
		       lengths[ lp ], lengths[ lp + 1 ] );
    } 
    if( numSubRanges ) {
	fprintSegment( f, -1, "</lengths>\n" );
	xfree( lengths );
    }
}


static void fprintTypedef(FILE *f, SmiType *smiType, const char *name)
{
    SmiRange *smiRange;
    unsigned int numSubRanges = getNumSubRanges( smiType );
    
    if ( name ) {
	fprintSegment(f, 1, "<xsd:simpleType name=\"%s\">\n", name);
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
	    if( smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {
	      fprintLengths( f, smiType );
	    }
	    fprintSegment( f, -1, "</xsd:appinfo>\n");
	}
	fprintSegment( f, -1, "</xsd:annotation>\n");
    }
        
    if( ( numSubRanges > 1 ) &&
	     ( smiType->basetype != SMI_BASETYPE_OCTETSTRING ) ) {
	
	fprintSegment( f, 1, "<xsd:union>\n");
	
	for( smiRange = smiGetFirstRange( smiType );
	     smiRange;
	     smiRange = smiGetNextRange( smiRange ) ) {
	    fprintSubRangeType( f, smiRange, smiType );
	}
	
	fprintSegment( f, -1, "</xsd:union>\n");
    }
    else if( smiType->basetype == SMI_BASETYPE_BITS ) {
	fprintBitList( f, smiType );
    }
    else {
	fprintRestriction(f, smiType );
    }
    fprintSegment(f, -1, "</xsd:simpleType>\n");

    /* print an empty line after global types */
    if( smiType->decl != SMI_DECL_IMPLICIT_TYPE && name ) {
	fprintf( f, "\n" );
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

    if( smiNode->nodekind == SMI_NODEKIND_ROW &&
	( smiNode->implied || smiNode->create ) ) {
	fprintSegment( f, 0, "<flags" );
	if( smiNode->implied ) {
	    fprintf( f, " implied=\"yes\"" );
	}
	if( smiNode->create ) {
	    fprintf( f, " create=\"yes\"" );
	}
	fprintf( f, "/>\n" );
    }
    
    fprintSegment( f, 0, "<maxAccess>%s</maxAccess>\n",
		   getStringAccess( smiNode->access ) );
    fprintSegment( f, 0, "<oid>");
    for (i = 0; i < smiNode->oidlen; i++) {
	fprintf(f, i ? ".%u" : "%u", smiNode->oid[i]);
    }
    fprintf( f, "</oid>\n" );

    fprintSegment( f, 0, "<status>%s</status>\n",
		   getStringStatus( smiNode->status ) );
    if( smiNode->value.basetype != SMI_BASETYPE_UNKNOWN ) {
	char *defval = smiRenderValue( &smiNode->value,
				       smiGetNodeType( smiNode ),
				       SMI_RENDER_FORMAT | SMI_RENDER_NAME );
	fprintSegment( f, 0, "<default>%s</default>\n", defval );

    }


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

static void
fprintTypeWithHint( FILE *f, SmiNode *smiNode, SmiType *smiType, char *hint )
{
    unsigned int numSubRanges = getNumSubRanges( smiType ),
	lp = 0;
    SmiRange *smiRange;
    SmiUnsigned32 *lengths = xmalloc(  2 * numSubRanges * 4 );
    
/*    fprintAnnotationElem( f, smiNode );*/
    
    /* write subtype lengths to the array */
    for( smiRange = smiGetFirstRange( smiType );
	 smiRange;
	 smiRange = smiGetNextRange( smiRange ) ) {
	lengths[ lp++ ] = smiRange->minValue.value.unsigned32;
	lengths[ lp++ ] = smiRange->maxValue.value.unsigned32;
    }		    
    fprintSegment( f, 1, "<xsd:simpleType>\n");
    fprintSegment( f, 1, "<xsd:annotation>\n");
    fprintSegment( f, 1, "<xsd:appinfo>\n");
    fprintDisplayHint( f, hint );
    fprintSegment( f, -1, "</xsd:appinfo>\n");
    fprintSegment( f, -1, "</xsd:annotation>\n");
    fprintSegment( f, 1, "<xsd:restriction base=\"xsd:string\">\n");
    fprintSegment( f, 0, "<xsd:pattern value=\"%s\"/>\n",
		   getStrDHType( hint, lengths, numSubRanges));
    fprintSegment( f, -1, "</xsd:restriction>\n");
    fprintSegment( f, -1, "</xsd:simpleType>\n");
    xfree( lengths );
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
    

    if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprintSegment( f, 1, "<xsd:attribute name=\"%s\" type=\"%s%s\" "
			   "use=\"required\">\n",
			   smiNode->name,
			   smiNode->name,
			   getStringBasetype( smiType->basetype ) );
	    fprintAnnotationElem( f, smiNode );
    }

    else if( smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {	    
	
	if( smiType->decl == SMI_DECL_IMPLICIT_TYPE ) {
	    char *hint = getParentDisplayHint( smiType );
	    
	    fprintSegment( f, 1, "<xsd:attribute name=\"%s\" "
			   "use=\"required\">\n", smiNode->name );
	    if( ! hint ) {
		fprintAnnotationElem( f, smiNode );
		fprintTypedef( f, smiType, NULL );
	    }
	    else {
		fprintTypeWithHint( f, smiNode, smiType, hint );
	    }
	}
	
	else {
	    if( prefix ) {
		fprintSegment( f, 1, "<xsd:attribute name=\"%s\" "
			       "type=\"%s:%s\" use=\"required\">\n",
			       smiNode->name, prefix, typeName );
	    }
	    else {		    
		fprintSegment( f, 1, "<xsd:attribute name=\"%s\" "
			       "type=\"%s\" use=\"required\">\n",
			       smiNode->name, typeName );
	    }
	    fprintAnnotationElem( f, smiNode );
	}
    }
    
    /* check for other (implicit) types */
    else if( smiType->decl == SMI_DECL_IMPLICIT_TYPE ) {
	fprintSegment( f, 1, "<xsd:attribute name=\"%s\" "
		       "use=\"required\">\n",
		       smiNode->name );
	fprintAnnotationElem( f, smiNode );
	fprintTypedef( f, smiType, NULL );
    }
    
    else {	
	if( prefix ) {
	    fprintSegment( f, 1,"<xsd:attribute name=\"%s\" type=\"%s:%s\" ",
			   smiNode->name, prefix, typeName );
	    fprintf( f, "use=\"required\">\n" );
	}
	else {
	    fprintSegment( f, 1, "<xsd:attribute name=\"%s\" type=\"%s\" ",
			   smiNode->name, typeName );
	    fprintf( f, "use=\"required\">\n" );
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
    }
    fprintSegment( f, -1, "</xsd:attribute>\n"); 
}

static int containsIndex( SmiNode *parentNode, SmiNode *idxNode )
{
    SmiElement *iterElement;


    for( iterElement = smiGetFirstElement( parentNode );
	 iterElement;
	 iterElement = smiGetNextElement( iterElement ) ) {
	SmiNode *iterNode = smiGetElementNode( iterElement );	
	if( iterNode == idxNode )
	    return 1;
    }
    return 0;
}

static void fprintIndex( FILE *f,
			 SmiNode *smiNode, SmiNode *augments, SmiNode *parent )
{
    SmiNode *iterNode;
    SmiElement *iterElem;
  
    /* iterate INDEX columns */
    for( iterElem = smiGetFirstElement( smiNode );
	 iterElem;
	 iterElem = smiGetNextElement( iterElem ) ) {
	iterNode = smiGetElementNode( iterElem );
	if( ! parent || (parent && !containsIndex( parent, iterNode ) ) ) {
	    fprintIndexAttr( f, iterNode, augments );
	}
    }

    /* print AUGMENTS-clause */
    iterNode = smiGetRelatedNode( smiNode );
    if( iterNode ) {
	fprintIndex( f, iterNode, iterNode, NULL );
    }
}

/* counts index elements of a table row node */
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


/* checks if the second node is a subtable of the first node */
static int
isSubTable( SmiNode *smiNode, SmiNode *subNode )
{
    SmiElement *iterElement;
    unsigned int numIdx = numIndex( smiNode ), numSubIdx = numIndex( subNode );

    /* compare number of index elements */
    if( numSubIdx <= numIdx ) {
	/* does not have more index elements --> no subtable */
	return 0;
    }

    /* compare all index elements */
    for( iterElement = smiGetFirstElement( smiNode );
	 iterElement;
	 iterElement = smiGetNextElement( iterElement ) ) {
	SmiElement *iterSubElement = smiGetFirstElement( subNode );
	SmiNode *iterSubNode;
	SmiNode *idxNode = smiGetElementNode( iterElement );
	
	for( iterSubElement = smiGetFirstElement( subNode );
	     iterSubElement;
	     iterSubElement = smiGetNextElement( iterSubElement ) ) {
	    
	    iterSubNode = smiGetElementNode( iterSubElement );
	    if( idxNode == iterSubNode ){
		return 1;
	    }
	}
    }
    return 0;
}


static void fprintComplexType( FILE *f, SmiNode *smiNode, const char *name,
			       SmiNode *parent )
{
    SmiNode *iterNode;
    int numChildren;
    
    if( name ) {
	fprintSegment( f, 1, "<xsd:complexType name=\"%sType\">\n",
		       smiNode->name );
    } else {
	fprintSegment( f, 1, "<xsd:complexType>\n" );
    }

/*    fprintAnnotationElem( f, smiNode ); */

    numChildren = hasChildren( smiNode, SMI_NODEKIND_ANY );

    fprintSegment( f, 1, "<xsd:sequence>\n");

    /* print child elements */
    for( iterNode = smiGetFirstChildNode( smiNode );
	 iterNode;
	 iterNode = smiGetNextChildNode( iterNode ) ) {
	
	fprintElement( f, iterNode, NULL );
	
    }

    /* print augmentations */
    if( nestAugmentedTables ) {
	for( iterNode = smiGetFirstNode( smiGetNodeModule( smiNode ),
					 SMI_NODEKIND_ROW );
	     iterNode;
	     iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_ROW ) ) {
	    SmiNode *augmNode = smiGetRelatedNode( iterNode );
	    if( augmNode == smiNode ) {
		SmiNode *augIterNode;
		for( augIterNode = smiGetFirstChildNode( iterNode );
		     augIterNode;
		     augIterNode = smiGetNextChildNode( augIterNode ) ) {
		    
		    fprintElement( f, augIterNode, NULL );
		}
	    }
	}
    }
    
    /* print subtables */
    if( nestSubtables ) {
	for( iterNode = smiGetFirstNode( smiGetNodeModule( smiNode ),
					 SMI_NODEKIND_ROW );
	     iterNode;
	     iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_ROW ) ) {
	    if( isSubTable( smiNode, iterNode ) ) {
/*	    fputs( "<!-- Here BEGIN subtable entry -->\n", f );*/
		fprintElement( f, iterNode, smiNode );
/*	    fputs( "<!-- Here END subtable entry -->\n", f );*/
	    }
	}
    }
    
    fprintSegment( f, -1, "</xsd:sequence>\n");
    fprintIndex( f, smiNode, NULL, parent );
    
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
	    fprintComplexType( f, iterNode, iterNode->name, NULL );
	}
    }

}    


static void fprintElement( FILE *f, SmiNode *smiNode, SmiNode *parentNode )
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
		    fprintElement( f, iterNode, NULL );
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
	    fprintElement( f, iterNode, NULL );
	}
	break;
    }

    case SMI_NODEKIND_ROW:
       
	fprintSegment( f, 1, "<xsd:element name=\"%s\" "
		       "minOccurs=\"0\" maxOccurs=\"unbounded\">\n",
		       smiNode->name );
	
	fprintAnnotationElem( f, smiNode );
	
	fprintComplexType( f, smiNode, NULL, parentNode );
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

#if 0
	if( smiType->basetype == SMI_BASETYPE_BITS ) {
	    fprintSegment( f, 1, "<xsd:element name=\"%s\" type=\"%s%s\" "
			   "minOccurs=\"0\">\n",
			   smiNode->name,
			   smiNode->name,
			   getStringBasetype( smiType->basetype ) );
	    fprintAnnotationElem( f, smiNode );
	}
	
//	else if( smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {
#endif /* 0 */

	if( smiType->basetype == SMI_BASETYPE_OCTETSTRING ) {
	    if( smiType->decl == SMI_DECL_IMPLICIT_TYPE ) {

		char *hint = getParentDisplayHint( smiType );

		fprintSegment( f, 1, "<xsd:element name=\"%s\" "
			       "minOccurs=\"0\">\n", smiNode->name );
		fprintAnnotationElem( f, smiNode );
		if( ! hint ) {
		    fprintTypedef( f, smiType, NULL );
		}
		else {
		    fprintTypeWithHint( f, smiNode, smiType, hint );
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


#if 0
static void fprintRows( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;

    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    fprintElement( f, iterNode, NULL );
	}
    }
}
#endif

static void fprintImports( FILE *f, SmiModule *smiModule )
{
    SmiImport *iterImp;
    char *lastModName = "";
    
    fprintSegment( f, 0, "<xsd:import namespace=\"%ssmi\" schemaLocation=\"%ssmi.xsd\"/>\n", schemaLocation, schemaLocation );
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


/*
 * Check if given table io a sub table of another table.
 * If so, its parent table is returned (NULL otherwise).
 */
static SmiNode *isASubTable( SmiNode *smiNode, SmiModule *smiModule )
{
    SmiNode *iterNode;
    int numIdxDiff = -1;
    SmiNode *retNode = NULL;
    
    for( iterNode = smiGetFirstNode( smiModule,
				     SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,
				    SMI_NODEKIND_ROW ) ) {
	
	if( isSubTable( iterNode, smiNode ) ) {

	    if( (numIdxDiff == -1) ||
		((numIndex( smiNode ) - numIndex( iterNode )) < numIdxDiff) ) {
		retNode = iterNode;
		numIdxDiff = numIndex( smiNode ) - numIndex( iterNode );
	    }
	    
	}
    }
    return retNode;
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
		    fprintElement( f, iterNode2, NULL );
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
	if( hasChildren( iterNode,
			 SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){	   

	    /* skip nested subtables here */
	    if( nestSubtables ){
		if( isASubTable( iterNode, smiModule ) != NULL ) {
		    continue;
		}
	    }

	    /* skip table augmentations here */
	    if( nestAugmentedTables ) {
		if( iterNode->indexkind == SMI_INDEX_AUGMENT ) {
		    continue;
		}
	    }
	    
	    fprintComplexType( f, iterNode, iterNode->name, NULL );
	    
	}
    }   
}


#if 0
static void fprintNotifications( FILE *f, SmiModule *smiModule )
{
    SmiNode *iterNode;
    
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NOTIFICATION );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NOTIFICATION ) ) {
	fprintElement( f, iterNode, NULL );
    }
}
#endif


static void fprintModuleHead(FILE *f, SmiModule *smiModule)
{
    if( smiModule->description ) {
	fprintSegment(f, 1, "<xsd:annotation>\n");
	fprintDocumentation(f, smiModule->description);
	fprintSegment(f, -1, "</xsd:annotation>\n\n");
    }
    
}


static void fprintKey( FILE *f, SmiNode *smiNode )
{
    SmiNode *relNode;
    SmiElement *iterElem;
    
    switch( smiNode->indexkind ) {
	
    case SMI_INDEX_INDEX:
	
	/* print key */
/*	fprintSegment( f, 1, "<xsd:key " );
	fprintf( f, "name=\"%sKey\">\n", smiNode->name );
	fprintSegment( f, 0, "<xsd:selector ");
	fprintf( f, "xpath=\"%s\"/>\n", smiNode->name );
	for( iterElem = smiGetFirstElement( smiNode );
	     iterElem;
	     iterElem = smiGetNextElement( iterElem ) ) {
	    SmiNode *indexNode = smiGetElementNode( iterElem );
	    fprintSegment( f, 0, "<xsd:field ");
	    fprintf( f, "xpath=\"@%s\"/>\n", indexNode->name );
	}
	fprintSegment( f, -1, "</xsd:key>\n\n");*/
	break;
	
    case SMI_INDEX_AUGMENT:
	
	/* print keyref */
	fprintSegment( f, 1, "<xsd:keyref " );
	relNode = smiGetRelatedNode( smiNode );
	fprintf( f, "name=\"%sKeyRef\" ", smiNode->name );
	fprintf( f, "refer=\"%sKey\">\n", relNode->name );
	fprintSegment( f, 0, "<xsd:selector ");
	fprintf( f, "xpath=\"%s\"/>\n", smiNode->name );
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
	fprintf( f, "name=\"%sKeyRefUnique\">\n", smiNode->name );
	fprintSegment( f, 0, "<xsd:selector ");
	fprintf( f, "xpath=\"%s\"/>\n", smiNode->name );
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


static void fprintGroupElements(FILE *f, SmiModule *smiModule)
{
    SmiNode *iterNode;

    /* scalar groups */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_NODE );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_NODE ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_SCALAR ) ) {
	    
	    if (container) {
		fprintSegment(f, 1, "<xsd:element name=\"%s\" "
			      "type=\"%s:%sType\" minOccurs=\"0\">\n",
			      iterNode->name,
			      smiModule->name, iterNode->name);
	    } else {
		fprintSegment(f, 1, "<xsd:element name=\"%s\" "
			      "type=\"%sType\" minOccurs=\"0\">\n",
			      iterNode->name, iterNode->name);
	    }
	    fprintAnnotationElem( f, iterNode );
	    fprintSegment( f, -1, "</xsd:element>\n" );
	}
    }   

    /* rows */
    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode,  SMI_NODEKIND_ROW ) ) {
	if( hasChildren( iterNode, SMI_NODEKIND_COLUMN | SMI_NODEKIND_TABLE ) ){
	    /* skip nested subtables here */
	    if( nestSubtables ){
		if( isASubTable( iterNode, smiModule ) != NULL ) {
		    continue;
		}
	    }

	    /* skip table augmentations here */
	    if( nestAugmentedTables ) {
		if( iterNode->indexkind == SMI_INDEX_AUGMENT ) {
		    continue;
		}
	    }

	    if (container) {
		fprintSegment(f, 1, "<xsd:element name=\"%s\" "
			      "type=\"%s:%sType\" minOccurs=\"0\" "
			      "maxOccurs=\"unbounded\">\n",
			      iterNode->name,
			      smiModule->name, iterNode->name);
		fprintKey( f, iterNode );
	    } else {
		fprintSegment(f, 1, "<xsd:element name=\"%s\" "
			      "type=\"%sType\" minOccurs=\"0\" "
			      "maxOccurs=\"unbounded\">\n",
			      iterNode->name, iterNode->name);
	    }
	    fprintAnnotationElem( f, iterNode );
	    fprintSegment( f, -1, "</xsd:element>\n" );
	}
    }   
}


static char *getSubTableXPath( SmiNode *smiNode, SmiModule *smiModule )
{
    char *ret;
    SmiNode *parentTable = isASubTable( smiNode, smiModule );

    if( parentTable ) {
	smiAsprintf( &ret, "%s/%s",
		     getSubTableXPath( parentTable, smiModule ),
		     smiNode->name );
    }
    else {
	smiAsprintf( &ret, "%s", smiNode->name );
    }
    return ret;   
}


static void fprintKeys( FILE *f, SmiModule *smiModule )
{

    SmiNode *iterNode;

    for( iterNode = smiGetFirstNode( smiModule, SMI_NODEKIND_ROW );
	 iterNode;
	 iterNode = smiGetNextNode( iterNode, SMI_NODEKIND_ROW ) ) {

	SmiElement *iterElem;

	/* print only keys for base tables */
	if( iterNode->indexkind != SMI_INDEX_INDEX ) {
	    continue;
	}
	
	/* print key */
	fprintSegment( f, 1, "<xsd:key name=\"%sKey\">\n", iterNode->name );
	fprintSegment( f, 0, "<xsd:selector ");
	fprintf( f, "xpath=\"%s\"/>\n",
		 nestSubtables ?
		 getSubTableXPath( iterNode, smiModule ) : iterNode->name );
	    
	for( iterElem = smiGetFirstElement( iterNode );
	     iterElem;
	     iterElem = smiGetNextElement( iterElem ) ) {
	    SmiNode *indexNode = smiGetElementNode( iterElem );
	    fprintSegment( f, 0, "<xsd:field ");
	    fprintf( f, "xpath=\"@%s\"/>\n", indexNode->name );
	}
	fprintSegment( f, -1, "</xsd:key>\n\n");
    }
    

}


static void fprintContextHead(FILE *f)
{
    fprintSegment( f, 1, "<xsd:element name=\"snmp-data\">\n");
    fprintSegment( f, 1, "<xsd:complexType>\n");
    fprintSegment( f, 1, "<xsd:sequence>\n");
    fprintSegment( f, 1, "<xsd:element name=\"context\" "
		   "minOccurs=\"0\" maxOccurs=\"unbounded\">\n");
    fprintSegment( f, 1, "<xsd:complexType>\n");
    fprintSegment( f, 1, "<xsd:sequence>\n");
}


static void fprintContextFoot(FILE *f, SmiModule **modv, int modc)
{
    int i;
    
    fprintSegment( f, -1, "</xsd:sequence>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"ipaddr\" type=\"xsd:NMTOKEN\" use=\"required\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"hostname\" type=\"xsd:NMTOKEN\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"port\" type=\"xsd:unsignedInt\" use=\"required\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"community\" type=\"xsd:NMTOKEN\" use=\"required\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"caching\" type=\"xsd:NMTOKEN\"/>\n");
    fprintSegment( f, 0, "<xsd:attribute name=\"time\" type=\"xsd:dateTime\" use=\"required\"/>\n");
    fprintSegment( f, -1, "</xsd:complexType>\n");
    fprintSegment( f, -1, "</xsd:element>\n");
    fprintSegment( f, -1, "</xsd:sequence>\n");
    fprintSegment( f, -1, "</xsd:complexType>\n");

    for( i=0; i < modc; i++ ) {
	fprintKeys( f, modv[ i ] );
    }
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
	fprintContextHead(f);
	fprintGroupElements(f, modv[i]);
	fprintContextFoot(f, modv, 0);
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

    /* imports */
    for (i = 0; i < modc; i++) {
	fprintSegment( f, 0, "<xsd:import ");
	fprintf( f, "namespace=\"%s%s\" schemaLocation=\"%s%s.xsd\"/>\n",
		 schemaLocation, modv[i]->name,
		 schemaLocation, modv[i]->name);
    }
    fprintf( f, "\n");
    
    /* context */
    fprintContextHead(f);
    for (i = 0; i < modc; i++) {
	/* per module elements */
	fprintGroupElements(f, modv[i]);
    }
    fprintContextFoot(f, modv, modc);

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
	smiAsprintf( &schemaLocation, "%s%c", schemaLocation, '/');
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
	{ "nest-augments", OPT_FLAG, &nestAugmentedTables, 0,
	  "Nest rows of augmented tables in the base tables" },
	{ "nest-subtables", OPT_FLAG, &nestSubtables, 0,
	  "Nest subtables in the base tables" },
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
