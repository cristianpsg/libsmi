/*
 * scanner-smi.l --
 *
 *      Lexical rules for scanning the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner-smi.l,v 1.2 1999/03/12 18:43:44 strauss Exp $
 */

%{
    
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "defs.h"    
#include "error.h"
#include "parser-smi.h"
#include "parser-smi.tab.h"
#include "scanner-smi.h"


#define thisParser      (*(Parser *)parser)


/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)

char *currentString = NULL;
int currentStringLength = 0;
int currentStringMaxLength = 0;
int currentStringColumn = 0;
int currentStringFetchLine = 0; 
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
 *      coulmn, and line buffer up to date.
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
    yylval.id = yytext;
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
    yylval.id = yytext;
    return END;
}

<INITIAL>EXPORTS {
    trackInput(0, parser);
    BEGIN(Exports);
    yylval.id = yytext;
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
    return yytext[0];
}

<INITIAL>CHOICE {
    trackInput(0, parser);
    BEGIN(Choice);
    yylval.id = yytext;
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
    return DOT_DOT;
}

<INITIAL>"::=" {
    trackInput(0, parser);
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
    yylval.id = yytext;
    return ACCESS;
}

<INITIAL>AGENT-CAPABILITIES/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return AGENT_CAPABILITIES;
}

<INITIAL>APPLICATION/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return APPLICATION;
}

<INITIAL>AUGMENTS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return AUGMENTS;
}

<INITIAL>BEGIN/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return BEGIN_;
}

<INITIAL>BITS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return BITS;
}

<INITIAL>CONTACT-INFO/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return CONTACT_INFO;
}

<INITIAL>CREATION-REQUIRES/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return CREATION_REQUIRES;
}

<INITIAL>Counter32/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return COUNTER32;
}

<INITIAL>Counter64/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return COUNTER64;
}

<INITIAL>DEFINITIONS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return DEFINITIONS;
}

<INITIAL>DEFVAL/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return DEFVAL;
}

<INITIAL>DESCRIPTION/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return DESCRIPTION;
}

<INITIAL>DISPLAY-HINT/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return DISPLAY_HINT;
}

<INITIAL>END/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return END;
}

<INITIAL>ENTERPRISE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return ENTERPRISE;
}

<INITIAL>FROM/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return FROM;
}

<INITIAL>GROUP/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return GROUP;
}

<INITIAL>Gauge32/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return GAUGE32;
}

<INITIAL>IDENTIFIER/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return IDENTIFIER;
}

<INITIAL>IMPLICIT/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return IMPLICIT;
}

<INITIAL>IMPLIED/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return IMPLIED;
}

<INITIAL>IMPORTS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return IMPORTS;
}

<INITIAL>INCLUDES/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return INCLUDES;
}

<INITIAL>INDEX/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return INDEX;
}

<INITIAL>INTEGER/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return INTEGER;
}

<INITIAL>Integer32/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return INTEGER32;
}

<INITIAL>IpAddress/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return IPADDRESS;
}

<INITIAL>LAST-UPDATED/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return LAST_UPDATED;
}

<INITIAL>MANDATORY-GROUPS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return MANDATORY_GROUPS;
}

<INITIAL>MAX-ACCESS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return MAX_ACCESS;
}

<INITIAL>MIN-ACCESS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return MIN_ACCESS;
}

<INITIAL>MODULE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return MODULE;
}

<INITIAL>MODULE-COMPLIANCE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return MODULE_COMPLIANCE;
}

<INITIAL>MODULE-IDENTITY/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return MODULE_IDENTITY;
}

<INITIAL>NOTIFICATION-GROUP/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return NOTIFICATION_GROUP;
}

<INITIAL>NOTIFICATION-TYPE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return NOTIFICATION_TYPE;
}

<INITIAL>NOTIFICATIONS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return NOTIFICATIONS;
}

<INITIAL>OBJECT/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OBJECT;
}

<INITIAL>OBJECT-GROUP/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OBJECT_GROUP;
}

<INITIAL>OBJECT-IDENTITY/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OBJECT_IDENTITY;
}

<INITIAL>OBJECT-TYPE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OBJECT_TYPE;
}

<INITIAL>OBJECTS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OBJECTS;
}

<INITIAL>OCTET/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OCTET;
}

<INITIAL>OF/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OF;
}

<INITIAL>ORGANIZATION/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return ORGANIZATION;
}

<INITIAL>Opaque/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return OPAQUE;
}

<INITIAL>PRODUCT-RELEASE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return PRODUCT_RELEASE;
}

<INITIAL>REFERENCE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return REFERENCE;
}

<INITIAL>REVISION/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return REVISION;
}

<INITIAL>SEQUENCE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return SEQUENCE;
}

<INITIAL>SIZE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return SIZE;
}

<INITIAL>STATUS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return STATUS;
}

<INITIAL>STRING/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return STRING;
}

<INITIAL>SUPPORTS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return SUPPORTS;
}

<INITIAL>SYNTAX/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return SYNTAX;
}

<INITIAL>TEXTUAL-CONVENTION/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return TEXTUAL_CONVENTION;
}

<INITIAL>TimeTicks/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return TIMETICKS;
}

<INITIAL>TRAP-TYPE/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return TRAP_TYPE;
}

<INITIAL>UNITS/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return UNITS;
}

<INITIAL>UNIVERSAL/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return UNIVERSAL;
}

<INITIAL>Unsigned32/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return UNSIGNED32;
}

<INITIAL>VARIABLES/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return VARIABLES;
}

<INITIAL>VARIATION/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
    return VARIATION;
}

<INITIAL>WRITE-SYNTAX/{delim} {
    trackInput(0, parser);
    yylval.id = yytext;
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
    yylval.id = strdup(yytext);
    return UPPERCASE_IDENTIFIER;
}

 /* same for lowercase names */
<INITIAL>[a-z](-?[a-zA-Z0-9]+)*-? {
    trackInput(0, parser);
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
    yylval.id = strdup(yytext);
    return NUMBER;
}

 /*
  * Lex rules for binary, hexadecimal and quoted strings.
  */

<INITIAL>\' {
    trackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentStringLength = 0;
    currentString[0] = '\0';
    BEGIN(BinOrHex);
}

<BinOrHex>\'[bB] {
    trackInput(0, parser);
    BEGIN(INITIAL);
    yylval.text = currentString;
    if (strlen(yylval.text) % 8) {
	printError(parser, ERR_BIN_STRING_MUL8, currentString);
    }
    return BIN_STRING;
}

<Hex>\'[bB] {
    trackInput(0, parser);
    printError(parser, ERR_HEX_ENDS_IN_B);
    yylval.text = currentString;
    if (strlen(yylval.text) % 2) {
	printError(parser, ERR_HEX_STRING_MUL8, currentString);
    }
    BEGIN(INITIAL);
    return HEX_STRING;
}

<BinOrHex,Hex>\'[hH] {
    trackInput(0, parser);
    yylval.text = currentString;
    if (strlen(yylval.text) % 2) {
	printError(parser, ERR_HEX_STRING_MUL8, currentString);
    }
    BEGIN(INITIAL);
    return HEX_STRING;
}

<BinOrHex,Hex>[2-9A-Fa-f] {
    trackInput(0, parser);
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
    trackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
}

<INITIAL>\" {
    trackInput(0, parser);
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
    trackInput(0, parser);
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
    trackInput(0, parser);
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
    trackInput(0, parser);
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
    trackInput(1, parser);
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

    trackInput(0, parser);
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
    trackInput(0, parser);
    printError(parser, ERR_LEX_UNEXPECTED_CHAR);
}

%%
