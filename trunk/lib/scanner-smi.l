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
 * @(#) $Id: scanner-smi.l,v 1.7 1999/05/31 11:58:35 strauss Exp $
 */

%option noyywrap

%{
    
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "error.h"
#include "parser-smi.h"
#include "parser-smi.tab.h"
#include "scanner.h"
#include "scanner-smi.h"



#define MAX_NUMBER		"18446744073709551615" /* max Counter64 */
#define MAX_BIN_STRING_LENGTH	128
#define MAX_HEX_STRING_LENGTH	128
#define MAX_QUOTED_STRING_LENGTH 65535



/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)



YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];


 
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



void
smiTrackInput(nl, parser)
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
	    if (thisParser.column+1 >= thisParser.linebufsize) {
		thisParser.linebufsize = thisParser.column+2;
		thisParser.linebuf = util_realloc(thisParser.linebuf,
						  thisParser.linebufsize);
	    }
	    strcat(thisParser.linebuf, yytext);
	} else {
	    thisParser.column += (8 - ((thisParser.column-1) % 8));
	    if (thisParser.column+1 >= thisParser.linebufsize) {
		thisParser.linebufsize = thisParser.column+2;
		thisParser.linebuf = util_realloc(thisParser.linebuf,
						  thisParser.linebufsize);
	    }
	    strcat(thisParser.linebuf, yytext);
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
  * Lex rules for skipping MACRO.
  */

<INITIAL>MACRO {
    smiTrackInput(0, parser);
    BEGIN(Macro);
    yylval.id = yytext;
    return MACRO;
}

<Macro>{eol} {
    smiTrackInput(1, parser);
}

<Macro>. {
    smiTrackInput(0, parser);
}

<Macro>END {
    smiTrackInput(0, parser);
    BEGIN(INITIAL);
    yylval.id = yytext;
    return END;
}

 /*
  * Lex rules for skipping EXPORTS.
  */

<INITIAL>EXPORTS {
    smiTrackInput(0, parser);
    BEGIN(Exports);
    yylval.id = yytext;
    return EXPORTS;
}

<Exports>{eol} {
    smiTrackInput(1, parser);
}

<Exports>[^\;] {
    smiTrackInput(0, parser);
}

<Exports>\; {
    smiTrackInput(0, parser);
    BEGIN(INITIAL);
    return yytext[0];
}

 /*
  * Lex rules for skipping CHOICE.
  */

<INITIAL>CHOICE {
    smiTrackInput(0, parser);
    BEGIN(Choice);
    yylval.id = yytext;
    return CHOICE;
}

<Choice>{eol} {
    smiTrackInput(1, parser);
}

<Choice>[^\}] {
    smiTrackInput(0, parser);
}

<Choice>\} {
    smiTrackInput(0, parser);
    BEGIN(INITIAL);
    return yytext[0];
}

 /*
  * Lex rules for comments.
  */

<INITIAL>"--" {
    smiTrackInput(0, parser);
    BEGIN(Comment);
}

<Comment>"--" {
    smiTrackInput(0, parser);
    BEGIN(INITIAL);
}
    
<Comment>{eol} {
    smiTrackInput(1, parser);
    BEGIN(INITIAL);
}

<Comment>. {
    smiTrackInput(0, parser);
}

 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\[\]\{\}\(\)\:\;\,\-\.\|] {
    smiTrackInput(0, parser);
    return yytext[0];
}
    
<INITIAL>".." {
    smiTrackInput(0, parser);
    return DOT_DOT;
}

<INITIAL>"::=" {
    smiTrackInput(0, parser);
    return COLON_COLON_EQUAL;
}

 /*
  * Lex rules for white space.
  */

<INITIAL>[ \t] {
    smiTrackInput(0, parser);
}

<INITIAL>{eol} {
    smiTrackInput(1, parser);
}

 /*
  * Lex rules for known keywords.
  */

<INITIAL>ACCESS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return ACCESS;
}

<INITIAL>AGENT-CAPABILITIES/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return AGENT_CAPABILITIES;
}

<INITIAL>APPLICATION/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return APPLICATION;
}

<INITIAL>AUGMENTS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return AUGMENTS;
}

<INITIAL>BEGIN/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return BEGIN_;
}

<INITIAL>BITS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return BITS;
}

<INITIAL>CONTACT-INFO/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return CONTACT_INFO;
}

<INITIAL>CREATION-REQUIRES/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return CREATION_REQUIRES;
}

<INITIAL>Counter32/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return COUNTER32;
}

<INITIAL>Counter64/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return COUNTER64;
}

<INITIAL>DEFINITIONS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return DEFINITIONS;
}

<INITIAL>DEFVAL/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return DEFVAL;
}

<INITIAL>DESCRIPTION/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return DESCRIPTION;
}

<INITIAL>DISPLAY-HINT/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return DISPLAY_HINT;
}

<INITIAL>END/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return END;
}

<INITIAL>ENTERPRISE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return ENTERPRISE;
}

<INITIAL>FROM/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return FROM;
}

<INITIAL>GROUP/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return GROUP;
}

<INITIAL>Gauge32/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return GAUGE32;
}

<INITIAL>IDENTIFIER/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return IDENTIFIER;
}

<INITIAL>IMPLICIT/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return IMPLICIT;
}

<INITIAL>IMPLIED/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return IMPLIED;
}

<INITIAL>IMPORTS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return IMPORTS;
}

<INITIAL>INCLUDES/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return INCLUDES;
}

<INITIAL>INDEX/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return INDEX;
}

<INITIAL>INTEGER/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return INTEGER;
}

<INITIAL>Integer32/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return INTEGER32;
}

<INITIAL>IpAddress/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return IPADDRESS;
}

<INITIAL>LAST-UPDATED/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return LAST_UPDATED;
}

<INITIAL>MANDATORY-GROUPS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return MANDATORY_GROUPS;
}

<INITIAL>MAX-ACCESS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return MAX_ACCESS;
}

<INITIAL>MIN-ACCESS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return MIN_ACCESS;
}

<INITIAL>MODULE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return MODULE;
}

<INITIAL>MODULE-COMPLIANCE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return MODULE_COMPLIANCE;
}

<INITIAL>MODULE-IDENTITY/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return MODULE_IDENTITY;
}

<INITIAL>NOTIFICATION-GROUP/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return NOTIFICATION_GROUP;
}

<INITIAL>NOTIFICATION-TYPE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return NOTIFICATION_TYPE;
}

<INITIAL>NOTIFICATIONS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return NOTIFICATIONS;
}

<INITIAL>OBJECT/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OBJECT;
}

<INITIAL>OBJECT-GROUP/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OBJECT_GROUP;
}

<INITIAL>OBJECT-IDENTITY/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OBJECT_IDENTITY;
}

<INITIAL>OBJECT-TYPE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OBJECT_TYPE;
}

<INITIAL>OBJECTS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OBJECTS;
}

<INITIAL>OCTET/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OCTET;
}

<INITIAL>OF/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OF;
}

<INITIAL>ORGANIZATION/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return ORGANIZATION;
}

<INITIAL>Opaque/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return OPAQUE;
}

<INITIAL>PRODUCT-RELEASE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return PRODUCT_RELEASE;
}

<INITIAL>REFERENCE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return REFERENCE;
}

<INITIAL>REVISION/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return REVISION;
}

<INITIAL>SEQUENCE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return SEQUENCE;
}

<INITIAL>SIZE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return SIZE;
}

<INITIAL>STATUS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return STATUS;
}

<INITIAL>STRING/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return STRING;
}

<INITIAL>SUPPORTS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return SUPPORTS;
}

<INITIAL>SYNTAX/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return SYNTAX;
}

<INITIAL>TEXTUAL-CONVENTION/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return TEXTUAL_CONVENTION;
}

<INITIAL>TimeTicks/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return TIMETICKS;
}

<INITIAL>TRAP-TYPE/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return TRAP_TYPE;
}

<INITIAL>UNITS/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return UNITS;
}

<INITIAL>UNIVERSAL/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return UNIVERSAL;
}

<INITIAL>Unsigned32/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return UNSIGNED32;
}

<INITIAL>VARIABLES/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return VARIABLES;
}

<INITIAL>VARIATION/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return VARIATION;
}

<INITIAL>WRITE-SYNTAX/{delim} {
    smiTrackInput(0, parser);
    yylval.id = yytext;
    return WRITE_SYNTAX;
}

 /*
  * Lex rules for forbidden keywords.
  */

<INITIAL>ABSENT|ANY|BIT|BOOLEAN|BY|COMPONENT|COMPONENTS|DEFAULT|DEFINED|ENUMERATED|EXPLICIT|EXTERNAL|FALSE|MAX|MIN|MINUS-INFINITY|NULL|OPTIONAL|PLUS-INFINITY|PRESENT|PRIVATE|REAL|SET|TAGS|TRUE|WITH/{delim} {
    smiTrackInput(0, parser);
    printError(parser, ERR_ILLEGAL_KEYWORD, yytext);
}

 /*
  * Lex rules for descriptors.
  */

 /* e.g. module names: REF: draft,p.12-13 */
<INITIAL>[A-Z](-?[a-zA-Z0-9]+)*-? {
    smiTrackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    yylval.id = strdup(yytext);
    return UPPERCASE_IDENTIFIER;
}

 /* same for lowercase names */
<INITIAL>[a-z](-?[a-zA-Z0-9]+)*-? {
    smiTrackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    yylval.id = strdup(yytext);
    return LOWERCASE_IDENTIFIER;
}

 /*
  * Lex rules for numbers.
  *
  * NOTE: `-' is a separate Token. Hence, there are no negative NUMBERs.
  */

<INITIAL>0+/[0-9] {
    smiTrackInput(0, parser);
    printError(parser, ERR_LEADING_ZEROS);
}

<INITIAL>([1-9][0-9]*|0)/[^0-9] {
    smiTrackInput(0, parser);
    if ((strlen(yytext) > sizeof(MAX_NUMBER)-1) ||
	((strlen(yytext) == sizeof(MAX_NUMBER)-1) &&
	 (strcmp(yytext, MAX_NUMBER) > 0))) {
	printError(parser, ERR_NUMBER_TO_LARGE, yytext);
    }
    yylval.id = strdup(yytext);
    return NUMBER;
}

 /*
  * Lex rules for binary, hexadecimal and quoted strings.
  */

<INITIAL>\' {
    smiTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentStringLength = 0;
    currentString[0] = '\0';
    BEGIN(BinOrHex);
}

<BinOrHex>\'[bB] {
    smiTrackInput(0, parser);
    BEGIN(INITIAL);
    yylval.text = currentString;
    if (strlen(yylval.text) % 8) {
	printError(parser, ERR_BIN_STRING_MUL8, currentString);
    }
    return BIN_STRING;
}

<Hex>\'[bB] {
    smiTrackInput(0, parser);
    printError(parser, ERR_HEX_ENDS_IN_B);
    yylval.text = currentString;
    if (strlen(yylval.text) % 2) {
	printError(parser, ERR_HEX_STRING_MUL2, currentString);
    }
    BEGIN(INITIAL);
    return HEX_STRING;
}

<BinOrHex,Hex>\'[hH] {
    smiTrackInput(0, parser);
    yylval.text = currentString;
    if (strlen(yylval.text) % 2) {
	printError(parser, ERR_HEX_STRING_MUL2, currentString);
    }
    BEGIN(INITIAL);
    return HEX_STRING;
}

<BinOrHex,Hex>[2-9A-Fa-f] {
    smiTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    BEGIN(Hex);
}

<BinOrHex,Hex>[01] {
    smiTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
}

<INITIAL>\" {
    smiTrackInput(0, parser);
    currentStringColumn = thisParser.column+1;
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentStringLength = 0;
    currentString[0] = '\0';
    BEGIN(String);
}

<String>\" {
    smiTrackInput(0, parser);
    yylval.text = currentString;
    BEGIN(INITIAL);

    /* TODO: beautify the string */
    
    return QUOTED_STRING;
}

 /*
  * any 7-bit displayable ASCII character
  * tab character
  * spaces, and
  * line terminator characters (\n or \r\n)
  */
<String>[!#-\176] {
    smiTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    currentStringFetchLine = 1;
}

<String>[ \t] {
    smiTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    if (currentStringFetchLine || (thisParser.column >= currentStringColumn)) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
	currentStringLength++;
	currentStringFetchLine = 1;
    }
}

<String>{eol} {
    smiTrackInput(1, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = '\n';
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    currentStringFetchLine = 0;
}

<String>. {
    char a[20];

    smiTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    sprintf(a, "code %u, column %d", (unsigned char)yytext[0],
	    thisParser.column-1);
    printError(parser, ERR_ILLEGAL_CHAR_IN_STRING, a);
}

. {
    smiTrackInput(0, parser);
    printError(parser, ERR_LEX_UNEXPECTED_CHAR);
}

%%
