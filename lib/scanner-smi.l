/*
 * scanner-smi.l --
 *
 *      Lexical rules for scanning the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner-smi.l,v 1.23 2000/06/14 13:15:18 strauss Exp $
 */

%option noyywrap

%{

#include <config.h>
    
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "error.h"
#include "util.h"
#include "parser-smi.h"
#include "parser-smi.tab.h"
#include "scanner-smi.h"

 
#if defined(_WIN32)
# undef YY_DECL
/* vc++ misses this; reason unclear */
# define YY_DECL int smilex   (YYSTYPE *lvalp, void *parser)   
#endif


#define thisParser (*(Parser *) parser)



#define MAX_UNSIGNED32		4294967295
#define MIN_UNSIGNED32		0
#define MAX_INTEGER32		2147483647
#define MIN_INTEGER32		-2147483648



/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)


static YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];

static int lexDepth = 0; 

 
int
smiEnterLexRecursion(file)
    FILE *file;
{
    if (lexDepth >= MAX_LEX_DEPTH) {
	return (-1);
    }
    yybuffer[lexDepth++] = YY_CURRENT_BUFFER;
    yy_switch_to_buffer(yy_create_buffer(file, YY_BUF_SIZE));
    return (lexDepth);
}


 
void
smiLeaveLexRecursion()
{    
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(yybuffer[--lexDepth]);
}



%}



/*
 * Lex pattern definitions.
 */
delim		([^a-zA-Z0-9-])|--
enddelim	([^a-zA-Z0-9-])|--|""
eol             ("\n"|"\n\015"|"\015\n")



/*
 * Lex state definitions.
 */
%s		Macro
%s		Choice
%s		Exports
%s		Comment



%%



 /*
  * Lex rules.
  */

 /*
  * Lex rules for skipping MACRO.
  */

<INITIAL>MACRO {
    BEGIN(Macro);
    yylval.id = yytext;
    return MACRO;
}

<Macro>{eol} {
    thisParser.line++;
}

<Macro>. {
}

<Macro>END {
    BEGIN(INITIAL);
    yylval.id = yytext;
    return END;
}

 /*
  * Lex rules for skipping EXPORTS.
  */

<INITIAL>EXPORTS {
    BEGIN(Exports);
    yylval.id = yytext;
    return EXPORTS;
}

<Exports>{eol} {
    thisParser.line++;
}

<Exports>[^\;]* {
}

<Exports>\; {
    BEGIN(INITIAL);
    return yytext[0];
}

 /*
  * Lex rules for skipping CHOICE.
  */

<INITIAL>CHOICE {
    BEGIN(Choice);
    yylval.id = yytext;
    return CHOICE;
}

<Choice>{eol} {
    thisParser.line++;
}

<Choice>[^\}]* {
}

<Choice>\} {
    BEGIN(INITIAL);
    return yytext[0];
}

 /*
  * Lex rules for comments.
  */

<INITIAL>"--" {
    BEGIN(Comment);
}

<Comment>"--" {
    BEGIN(INITIAL);
}
    
<Comment>{eol} {
    thisParser.line++;
    BEGIN(INITIAL);
}

<Comment>.* {
}

 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\[\]\{\}\(\)\:\;\,\-\.\|] {
    return yytext[0];
}
    
<INITIAL>".." {
    return DOT_DOT;
}

<INITIAL>"::=" {
    return COLON_COLON_EQUAL;
}

 /*
  * Lex rules for white space.
  */

<INITIAL>[ \t]* {
}

<INITIAL>{eol} {
    thisParser.line++;
}

 /*
  * Lex rules for known keywords.
  */

<INITIAL>ACCESS/{delim} {
    yylval.id = yytext;
    return ACCESS;
}

<INITIAL>AGENT-CAPABILITIES/{delim} {
    yylval.id = yytext;
    return AGENT_CAPABILITIES;
}

<INITIAL>APPLICATION/{delim} {
    yylval.id = yytext;
    return APPLICATION;
}

<INITIAL>AUGMENTS/{delim} {
    yylval.id = yytext;
    return AUGMENTS;
}

<INITIAL>BEGIN/{delim} {
    yylval.id = yytext;
    return BEGIN_;
}

<INITIAL>BITS/{delim} {
    yylval.id = yytext;
    return BITS;
}

<INITIAL>CONTACT-INFO/{delim} {
    yylval.id = yytext;
    return CONTACT_INFO;
}

<INITIAL>CREATION-REQUIRES/{delim} {
    yylval.id = yytext;
    return CREATION_REQUIRES;
}

<INITIAL>Counter32/{delim} {
    yylval.id = yytext;
    return COUNTER32;
}

<INITIAL>Counter64/{delim} {
    yylval.id = yytext;
    return COUNTER64;
}

<INITIAL>DEFINITIONS/{delim} {
    yylval.id = yytext;
    return DEFINITIONS;
}

<INITIAL>DEFVAL/{delim} {
    yylval.id = yytext;
    return DEFVAL;
}

<INITIAL>DESCRIPTION/{delim} {
    yylval.id = yytext;
    return DESCRIPTION;
}

<INITIAL>DISPLAY-HINT/{delim} {
    yylval.id = yytext;
    return DISPLAY_HINT;
}

<INITIAL>END/{enddelim} {
    yylval.id = yytext;
    return END;
}

<INITIAL>ENTERPRISE/{delim} {
    yylval.id = yytext;
    return ENTERPRISE;
}

<INITIAL>FROM/{delim} {
    yylval.id = yytext;
    return FROM;
}

<INITIAL>GROUP/{delim} {
    yylval.id = yytext;
    return GROUP;
}

<INITIAL>Gauge32/{delim} {
    yylval.id = yytext;
    return GAUGE32;
}

<INITIAL>IDENTIFIER/{delim} {
    yylval.id = yytext;
    return IDENTIFIER;
}

<INITIAL>IMPLICIT/{delim} {
    yylval.id = yytext;
    return IMPLICIT;
}

<INITIAL>IMPLIED/{delim} {
    yylval.id = yytext;
    return IMPLIED;
}

<INITIAL>IMPORTS/{delim} {
    yylval.id = yytext;
    return IMPORTS;
}

<INITIAL>INCLUDES/{delim} {
    yylval.id = yytext;
    return INCLUDES;
}

<INITIAL>INDEX/{delim} {
    yylval.id = yytext;
    return INDEX;
}

<INITIAL>INTEGER/{delim} {
    yylval.id = yytext;
    return INTEGER;
}

<INITIAL>Integer32/{delim} {
    yylval.id = yytext;
    return INTEGER32;
}

<INITIAL>IpAddress/{delim} {
    yylval.id = yytext;
    return IPADDRESS;
}

<INITIAL>LAST-UPDATED/{delim} {
    yylval.id = yytext;
    return LAST_UPDATED;
}

<INITIAL>MANDATORY-GROUPS/{delim} {
    yylval.id = yytext;
    return MANDATORY_GROUPS;
}

<INITIAL>MAX-ACCESS/{delim} {
    yylval.id = yytext;
    return MAX_ACCESS;
}

<INITIAL>MIN-ACCESS/{delim} {
    yylval.id = yytext;
    return MIN_ACCESS;
}

<INITIAL>MODULE/{delim} {
    yylval.id = yytext;
    return MODULE;
}

<INITIAL>MODULE-COMPLIANCE/{delim} {
    yylval.id = yytext;
    return MODULE_COMPLIANCE;
}

<INITIAL>MODULE-IDENTITY/{delim} {
    yylval.id = yytext;
    return MODULE_IDENTITY;
}

<INITIAL>NOTIFICATION-GROUP/{delim} {
    yylval.id = yytext;
    return NOTIFICATION_GROUP;
}

<INITIAL>NOTIFICATION-TYPE/{delim} {
    yylval.id = yytext;
    return NOTIFICATION_TYPE;
}

<INITIAL>NOTIFICATIONS/{delim} {
    yylval.id = yytext;
    return NOTIFICATIONS;
}

<INITIAL>OBJECT/{delim} {
    yylval.id = yytext;
    return OBJECT;
}

<INITIAL>OBJECT-GROUP/{delim} {
    yylval.id = yytext;
    return OBJECT_GROUP;
}

<INITIAL>OBJECT-IDENTITY/{delim} {
    yylval.id = yytext;
    return OBJECT_IDENTITY;
}

<INITIAL>OBJECT-TYPE/{delim} {
    yylval.id = yytext;
    return OBJECT_TYPE;
}

<INITIAL>OBJECTS/{delim} {
    yylval.id = yytext;
    return OBJECTS;
}

<INITIAL>OCTET/{delim} {
    yylval.id = yytext;
    return OCTET;
}

<INITIAL>OF/{delim} {
    yylval.id = yytext;
    return OF;
}

<INITIAL>ORGANIZATION/{delim} {
    yylval.id = yytext;
    return ORGANIZATION;
}

<INITIAL>Opaque/{delim} {
    yylval.id = yytext;
    return OPAQUE;
}

<INITIAL>PRODUCT-RELEASE/{delim} {
    yylval.id = yytext;
    return PRODUCT_RELEASE;
}

<INITIAL>REFERENCE/{delim} {
    yylval.id = yytext;
    return REFERENCE;
}

<INITIAL>REVISION/{delim} {
    yylval.id = yytext;
    return REVISION;
}

<INITIAL>SEQUENCE/{delim} {
    yylval.id = yytext;
    return SEQUENCE;
}

<INITIAL>SIZE/{delim} {
    yylval.id = yytext;
    return SIZE;
}

<INITIAL>STATUS/{delim} {
    yylval.id = yytext;
    return STATUS;
}

<INITIAL>STRING/{delim} {
    yylval.id = yytext;
    return STRING;
}

<INITIAL>SUPPORTS/{delim} {
    yylval.id = yytext;
    return SUPPORTS;
}

<INITIAL>SYNTAX/{delim} {
    yylval.id = yytext;
    return SYNTAX;
}

<INITIAL>TEXTUAL-CONVENTION/{delim} {
    yylval.id = yytext;
    return TEXTUAL_CONVENTION;
}

<INITIAL>TimeTicks/{delim} {
    yylval.id = yytext;
    return TIMETICKS;
}

<INITIAL>TRAP-TYPE/{delim} {
    yylval.id = yytext;
    return TRAP_TYPE;
}

<INITIAL>UNITS/{delim} {
    yylval.id = yytext;
    return UNITS;
}

<INITIAL>UNIVERSAL/{delim} {
    yylval.id = yytext;
    return UNIVERSAL;
}

<INITIAL>Unsigned32/{delim} {
    yylval.id = yytext;
    return UNSIGNED32;
}

<INITIAL>VARIABLES/{delim} {
    yylval.id = yytext;
    return VARIABLES;
}

<INITIAL>VARIATION/{delim} {
    yylval.id = yytext;
    return VARIATION;
}

<INITIAL>WRITE-SYNTAX/{delim} {
    yylval.id = yytext;
    return WRITE_SYNTAX;
}

 /*
  * Lex rules for forbidden keywords. Do you use {delim} here because it
  * introduces a trailing context which is (a) slow and (b) causes
  * REJECT to be used.
  */

<INITIAL>ABSENT|ANY|BIT|BOOLEAN|BY|COMPONENT|COMPONENTS|DEFAULT|DEFINED|ENUMERATED|EXPLICIT|EXTERNAL|FALSE|MAX|MIN|MINUS-INFINITY|NULL|OPTIONAL|PLUS-INFINITY|PRESENT|PRIVATE|REAL|SET|TAGS|TRUE|WITH/[^a-zA-Z0-9-] {
    smiPrintError(parser, ERR_ILLEGAL_KEYWORD, yytext);
}

<INITIAL>ABSENT|ANY|BIT|BOOLEAN|BY|COMPONENT|COMPONENTS|DEFAULT|DEFINED|ENUMERATED|EXPLICIT|EXTERNAL|FALSE|MAX|MIN|MINUS-INFINITY|NULL|OPTIONAL|PLUS-INFINITY|PRESENT|PRIVATE|REAL|SET|TAGS|TRUE|WITH/-- {
    smiPrintError(parser, ERR_ILLEGAL_KEYWORD, yytext);
}

 /*
  * Lex rules for descriptors.
  */

 /* e.g. module names: REF: draft,p.12-13 */
<INITIAL>[A-Z](-?[a-zA-Z0-9]+)*-? {
    if (yytext[yyleng-1] == '-') {
	smiPrintError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    yylval.id = smiStrdup(yytext);
    return UPPERCASE_IDENTIFIER;
}

 /* same for lowercase names */
<INITIAL>[a-z](-?[a-zA-Z0-9]+)*-? {
    if (yytext[yyleng-1] == '-') {
	smiPrintError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    yylval.id = smiStrdup(yytext);
    return LOWERCASE_IDENTIFIER;
}

 /*
  * Lex rules for numbers.
  */

<INITIAL>0+/[0-9] {
    smiPrintError(parser, ERR_LEADING_ZEROS);
}

<INITIAL>([1-9][0-9]*|0)/[^0-9] {
    errno = 0;
    yylval.unsigned32 = strtoul(yytext, NULL, 10);
    if (errno == ERANGE) {
	/* We don't want to see this error for the SNMPv2-SMI module. */
	if (strcmp(thisParser.modulePtr->export.name, "SNMPv2-SMI")) {
	    smiPrintError(parser, ERR_SMIV2_UNSIGNED_NUMBER_RANGE, yytext);
	}
    }
    return NUMBER;
}

<INITIAL>-([1-9][0-9]*|0)/[^0-9] {
    errno = 0;
    yylval.integer32 = strtol(yytext, NULL, 10);
    if (errno == ERANGE) {
	smiPrintError(parser, ERR_SMIV2_SIGNED_NUMBER_RANGE, yytext);
    }
    return NEGATIVENUMBER;
}

 /*
  * Lex rules for binary, hexadecimal and quoted strings (RFC 2578 3.1.1)
  */

<INITIAL>\'[01]*\'[bB] {
    yylval.text = yytext + 1;
    yytext[yyleng-2] = '\0';
    if ((yyleng-3) % 8) {
	smiPrintError(parser, ERR_BIN_STRING_MUL8, yylval.text);
    }
    return BIN_STRING;
}

<INITIAL>\'[0123456789AaBbCcDdEeFf]*\'[hH] {
    yylval.text = yytext + 1;
    yytext[yyleng-2] = '\0';
    if ((yyleng-3) % 2) {
	smiPrintError(parser, ERR_HEX_STRING_MUL2, yylval.text);
    }
    return HEX_STRING;
}

<INITIAL>\"[^\"]*\" {
    char *s, *d;		/* the source and destination pointer */
    int column = 0;		/* the current column */
    int newlineflag = 0;	/* we have just passed a newline */
    int cutoffcolumn = 0;	/* cut off white space up to this column */
				/* (computed by caculating the indentation */
				/* of the first column) */

    yytext[yyleng-1] = '\0';
    for (d = yytext, s = yytext+1; s[0]; s++, d++) {

	if ((s[0] == '\n' && s[1] == '\r')	/* newline sequence */
	    || (s[0] == '\r' && s[1] == '\n')) {
	    thisParser.line++;
	    d[0] = '\n';
	    s++;
	    newlineflag = 1;
	    column = 0;
	    if (cutoffcolumn < 0) cutoffcolumn = 0;
	} else if (s[0] == '\n') {		/* simple newline */
	    thisParser.line++;
	    d[0] = '\n';
	    newlineflag = 1;
	    column = 0;
	    if (cutoffcolumn < 0) cutoffcolumn = 0;
	} else {
	    if (newlineflag && isspace(s[0])) {	/* space after newline */
		if (cutoffcolumn <= 0) {
		    cutoffcolumn -= (s[0] == '\t') ? (8-((column-1) % 8)) : 1;
		}
		column += (s[0] == '\t') ? (8-((column-1) % 8)) : 1;
		if (cutoffcolumn <= 0 || column <= cutoffcolumn) {
		    d--;
		} else {
		    d[0] = s[0];
		    newlineflag = 0;
		}
	    } else {				/* everything else */
		if (! isascii(s[0])) {
		    smiPrintError(parser, ERR_ILLEGAL_CHAR_IN_STRING,
				  s[0], (unsigned char) s[0]);
		}
		d[0] = s[0];
		newlineflag = 0;
		if (cutoffcolumn < 0) {
		    cutoffcolumn *= -1;
		}
	    }
	}
    }
    d[0] = '\0';

    yylval.text = yytext;
    return QUOTED_STRING;
}

 /*
  * Everything else...
  */

. {
    smiPrintError(parser, ERR_LEX_UNEXPECTED_CHAR);
}

%%
