/*
 * scanner.l --
 *
 *      Lexical rules for scanning the MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner.l,v 1.26 1998/08/29 11:23:03 strauss Exp $
 */

%{
    
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "defs.h"    
#include "error.h"
#include "parser.h"
#include "parser-bison.h"
#include "scanner.h"


#define thisParser      (*(Parser *)parser)


/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)

char currentString[201];
int currentStringLength;
int lexDepth = 0;
YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];
 
int
enterLexRecursion(file)
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
leaveLexRecursion()
{    
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(yybuffer[--lexDepth]);
}


/*
 *----------------------------------------------------------------------
 *
 * trackInput --
 *
 *      track the input character stream to keep line number,
 *      coulmn, absolute offset and line buffer up to date.
 *
 * Results:
 *      TODO.
 *
 * Side effects:
 *      TODO.
 *
 *----------------------------------------------------------------------
 */

void
trackInput(nl, parser)
    int nl;
    Module *parser;
{
    if (nl) {
	thisParser.line += 1;
	thisParser.column = 1;
	thisParser.character += yyleng;
	thisParser.linebuf[0] = 0;
    } else {
	if ((yytext[0] != '\t') || (yyleng != 1)) {
	    thisParser.column += yyleng;
	    if (thisParser.column+1 < sizeof(thisParser.linebuf)) {
		strcat(thisParser.linebuf, yytext);
	    }
	} else {
	    thisParser.column += (8 - ((thisParser.column-1) % 8));
	    if (thisParser.column+1 < sizeof(thisParser.linebuf)) {
		strcat(thisParser.linebuf, yytext);
	    }
	}
	thisParser.character += yyleng;
    }
}


 
%}



/*
 * Lex pattern definitions.
 */
delim		([^a-zA-Z0-9-])|--
eol             ("\n"|"\n\015"|"\015\n")



/*
 * Lex state definitions.
 */
%s		Comment
%s              BinOrHex
%s              Hex
%s		String
%s		Macro
%s		Choice
%s		Exports



%%



 /*
  * Lex rules.
  */

 /*
  * Lex rules for comments.
  */

<INITIAL>MACRO {
    trackInput(0, parser);
    BEGIN(Macro);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MACRO;
}

<Macro>{eol} {
    trackInput(1, parser);
}

<Macro>. {
    trackInput(0, parser);
}

<Macro>END {
    trackInput(0, parser);
    BEGIN(INITIAL);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return END;
}

<INITIAL>EXPORTS {
    trackInput(0, parser);
    BEGIN(Exports);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return EXPORTS;
}

<Exports>{eol} {
    trackInput(1, parser);
}

<Exports>[^\;] {
    trackInput(0, parser);
}

<Exports>\; {
    trackInput(0, parser);
    BEGIN(INITIAL);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return yytext[0];
}

<INITIAL>CHOICE {
    trackInput(0, parser);
    BEGIN(Choice);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return CHOICE;
}

<Choice>{eol} {
    trackInput(1, parser);
}

<Choice>[^\}] {
    trackInput(0, parser);
}

<Choice>\} {
    trackInput(0, parser);
    BEGIN(INITIAL);
    return yytext[0];
}

<INITIAL>"--" {
    trackInput(0, parser);
    BEGIN(Comment);
}

<Comment>"--" {
    trackInput(0, parser);
    BEGIN(INITIAL);
}
    
<Comment>{eol} {
    trackInput(1, parser);
    BEGIN(INITIAL);
}

<Comment>. {
    trackInput(0, parser);
}

 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\[\]\{\}\(\)\:\;\,\-\.\|] {
    trackInput(0, parser);
    return yytext[0];
}
    
<INITIAL>".." {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return DOT_DOT;
}

<INITIAL>"::=" {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return COLON_COLON_EQUAL;
}

 /*
  * Lex rules for white space.
  */

<INITIAL>[ \t] {
    trackInput(0, parser);
}

<INITIAL>{eol} {
    trackInput(1, parser);
}

 /*
  * Lex rules for known keywords.
  */

<INITIAL>ACCESS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return ACCESS;
}

<INITIAL>AGENT-CAPABILITIES/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return AGENT_CAPABILITIES;
}

<INITIAL>APPLICATION/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return APPLICATION;
}

<INITIAL>AUGMENTS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return AUGMENTS;
}

<INITIAL>BEGIN/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return BEGIN_;
}

<INITIAL>BITS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return BITS;
}

<INITIAL>CONTACT-INFO/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return CONTACT_INFO;
}

<INITIAL>CREATION-REQUIRES/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return CREATION_REQUIRES;
}

<INITIAL>Counter32/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return COUNTER32;
}

<INITIAL>Counter64/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return COUNTER64;
}

<INITIAL>DEFINITIONS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return DEFINITIONS;
}

<INITIAL>DEFVAL/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return DEFVAL;
}

<INITIAL>DESCRIPTION/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return DESCRIPTION;
}

<INITIAL>DISPLAY-HINT/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return DISPLAY_HINT;
}

<INITIAL>END/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return END;
}

<INITIAL>ENTERPRISE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return ENTERPRISE;
}

<INITIAL>FROM/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return FROM;
}

<INITIAL>GROUP/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return GROUP;
}

<INITIAL>Gauge32/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return GAUGE32;
}

<INITIAL>IDENTIFIER/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return IDENTIFIER;
}

<INITIAL>IMPLICIT/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return IMPLICIT;
}

<INITIAL>IMPLIED/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return IMPLIED;
}

<INITIAL>IMPORTS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return IMPORTS;
}

<INITIAL>INCLUDES/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return INCLUDES;
}

<INITIAL>INDEX/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return INDEX;
}

<INITIAL>INTEGER/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return INTEGER;
}

<INITIAL>Integer32/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return INTEGER32;
}

<INITIAL>IpAddress/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return IPADDRESS;
}

<INITIAL>LAST-UPDATED/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return LAST_UPDATED;
}

<INITIAL>MANDATORY-GROUPS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MANDATORY_GROUPS;
}

<INITIAL>MAX-ACCESS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MAX_ACCESS;
}

<INITIAL>MIN-ACCESS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MIN_ACCESS;
}

<INITIAL>MODULE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MODULE;
}

<INITIAL>MODULE-COMPLIANCE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MODULE_COMPLIANCE;
}

<INITIAL>MODULE-IDENTITY/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return MODULE_IDENTITY;
}

<INITIAL>NOTIFICATION-GROUP/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return NOTIFICATION_GROUP;
}

<INITIAL>NOTIFICATION-TYPE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return NOTIFICATION_TYPE;
}

<INITIAL>NOTIFICATIONS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return NOTIFICATIONS;
}

<INITIAL>OBJECT/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OBJECT;
}

<INITIAL>OBJECT-GROUP/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OBJECT_GROUP;
}

<INITIAL>OBJECT-IDENTITY/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OBJECT_IDENTITY;
}

<INITIAL>OBJECT-TYPE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OBJECT_TYPE;
}

<INITIAL>OBJECTS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OBJECTS;
}

<INITIAL>OCTET/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OCTET;
}

<INITIAL>OF/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OF;
}

<INITIAL>ORGANIZATION/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return ORGANIZATION;
}

<INITIAL>Opaque/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return OPAQUE;
}

<INITIAL>PRODUCT-RELEASE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return PRODUCT_RELEASE;
}

<INITIAL>REFERENCE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return REFERENCE;
}

<INITIAL>REVISION/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return REVISION;
}

<INITIAL>SEQUENCE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return SEQUENCE;
}

<INITIAL>SIZE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return SIZE;
}

<INITIAL>STATUS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return STATUS;
}

<INITIAL>STRING/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return STRING;
}

<INITIAL>SUPPORTS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return SUPPORTS;
}

<INITIAL>SYNTAX/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return SYNTAX;
}

<INITIAL>TEXTUAL-CONVENTION/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return TEXTUAL_CONVENTION;
}

<INITIAL>TimeTicks/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return TIMETICKS;
}

<INITIAL>TRAP-TYPE/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return TRAP_TYPE;
}

<INITIAL>UNITS/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return UNITS;
}

<INITIAL>UNIVERSAL/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return UNIVERSAL;
}

<INITIAL>Unsigned32/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return UNSIGNED32;
}

<INITIAL>VARIABLES/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return VARIABLES;
}

<INITIAL>VARIATION/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return VARIATION;
}

<INITIAL>WRITE-SYNTAX/{delim} {
    trackInput(0, parser);
    strncpy(yylval.id, yytext, sizeof(yylval.id)-1);
    return WRITE_SYNTAX;
}

 /*
  * Lex rules for forbidden keywords.
  */

<INITIAL>ABSENT|ANY|BIT|BOOLEAN|BY|COMPONENT|COMPONENTS|DEFAULT|DEFINED|ENUMERATED|EXPLICIT|EXTERNAL|FALSE|MAX|MIN|MINUS-INFINITY|NULL|OPTIONAL|PLUS-INFINITY|PRESENT|PRIVATE|REAL|SET|TAGS|TRUE|WITH/{delim} {
    trackInput(0, parser);
    printError(parser, ERR_ILLEGAL_KEYWORD, yytext);
}

 /*
  * Lex rules for descriptors.
  */

 /* e.g. module names: REF: draft,p.12-13 */
<INITIAL>[A-Z](-?[a-zA-Z0-9]+)*-? {
    trackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    strncpy(yylval.text.content, yytext, sizeof(yylval.text.content)-1);
    /* TODO: make length unlimited?! */
    return UPPERCASE_IDENTIFIER;
}

 /* same for lowercase names */
<INITIAL>[a-z](-?[a-zA-Z0-9]+)*-? {
    trackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    strncpy(yylval.text.content, yytext, sizeof(yylval.text.content)-1);
    /* TODO: make length unlimited?! */
    return LOWERCASE_IDENTIFIER;
}

 /*
  * Lex rules for numbers.
  *
  * NOTE: `-' is a separate Token. Hence, there are no negative NUMBERs.
  */

<INITIAL>0+/[0-9] {
    trackInput(0, parser);
    printError(parser, ERR_LEADING_ZEROS);
}

<INITIAL>([1-9][0-9]*|0)/[^0-9] {
    trackInput(0, parser);
    if ((strlen(yytext) > sizeof(MAX_NUMBER)-1) ||
	((strlen(yytext) == sizeof(MAX_NUMBER)-1) &&
	 (strcmp(yytext, MAX_NUMBER) > 0))) {
	printError(parser, ERR_NUMBER_TO_LARGE, yytext);
    }
    strncpy(yylval.text.content, yytext, sizeof(yylval.text.content)-1);
    return NUMBER;
}

 /*
  * Lex rules for binary, hexadecimal and quoted strings.
  */

<INITIAL>\' {
    trackInput(0, parser);
    currentStringLength = 0;
    currentString[0] = '\0';
    BEGIN(BinOrHex);
}

<BinOrHex>\'[bB] {
    trackInput(0, parser);
    BEGIN(INITIAL);
    strncpy(yylval.text.content, currentString, sizeof(yylval.text.content)-1);
    yylval.text.fileoffset = thisParser.character - currentStringLength - 3;
    yylval.text.length = currentStringLength;
    if (yylval.text.length % 8) {
	printError(parser, ERR_BIN_STRING_MUL8, yylval.text.content);
    }
    return BIN_STRING;
}

<Hex>\'[bB] {
    trackInput(0, parser);
    printError(parser, ERR_HEX_ENDS_IN_B);
    strncpy(yylval.text.content, currentString, sizeof(yylval.text.content)-1);
    yylval.text.fileoffset = thisParser.character - currentStringLength - 3;
    yylval.text.length = currentStringLength;
    if (yylval.text.length % 2) {
	printError(parser, ERR_HEX_STRING_MUL8, yylval.text.content);
    }
    BEGIN(INITIAL);
    return HEX_STRING;
}

<BinOrHex,Hex>\'[hH] {
    trackInput(0, parser);
    strncpy(yylval.text.content, currentString, sizeof(yylval.text.content)-1);
    yylval.text.fileoffset = thisParser.character - currentStringLength - 3;
    yylval.text.length = currentStringLength;
    if (yylval.text.length % 2) {
	printError(parser, ERR_HEX_STRING_MUL8, yylval.text.content);
    }
    BEGIN(INITIAL);
    return HEX_STRING;
}

<BinOrHex,Hex>[2-9A-Fa-f] {
    trackInput(0, parser);
    if (currentStringLength < sizeof(currentString)-1) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
    }
    currentStringLength++;
    BEGIN(Hex);
}

<BinOrHex,Hex>[01] {
    trackInput(0, parser);
    if (currentStringLength < sizeof(currentString)-1) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
    }
    currentStringLength++;
}

<INITIAL>\" {
    trackInput(0, parser);
    currentStringLength = 0;
    currentString[0] = '\0';
    BEGIN(String);
}

<String>\" {
    trackInput(0, parser);
    strncpy(yylval.text.content, currentString, sizeof(yylval.text.content)-1);
    yylval.text.fileoffset = thisParser.character - currentStringLength - 2;
    yylval.text.length = currentStringLength;
    BEGIN(INITIAL);
    return QUOTED_STRING;
}

 /*
  * any 7-bit displayable ASCII character
  * tab character
  * spaces, and
  * line terminator characters (\n or \r\n)
  */
<String>[ !#-\176\t] {
    trackInput(0, parser);
    if (currentStringLength < sizeof(currentString)-1) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
    }
    currentStringLength++;
}

<String>{eol} {
    trackInput(1, parser);
    if (currentStringLength < sizeof(currentString)-1) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
    }
    currentStringLength++;
}

<String>. {
    char a[20];
    trackInput(0, parser);
    sprintf(a, "code %u, column %d", (unsigned char)yytext[0],
	    thisParser.column-1);
    if (currentStringLength < sizeof(currentString)-1) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
    }
    currentStringLength++;
    /* TODO: or should we skip it? */ 
    printError(parser, ERR_ILLEGAL_CHAR_IN_STRING, a);
}

. {
    trackInput(0, parser);
    printError(parser, ERR_LEX_UNEXPECTED_CHAR);
}

%%
