/*
 * scanner-sming.l --
 *
 *      Lexical rules for scanning the SMIng MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: scanner-sming.l,v 1.14 2000/01/03 17:07:40 strauss Exp $
 */

%option noyywrap

%{
    
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "error.h"
#include "util.h"
#include "parser-sming.h"
#include "parser-sming.tab.h"
#include "scanner-sming.h"


static char            *currentString;
static int	       currentStringLength;
static int	       currentStringMaxLength;
static int	       currentStringColumn;
static int	       currentStringFetchLine; 


#define thisParser (*(Parser *) parser)



#define MAX_NUMBER		"18446744073709551615" /* max Counter64 */
#define MAX_QUOTED_STRING_LENGTH 65535



/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)


#define MAX_LEX_DEPTH            20
 
static YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];

static int lexDepth = 0; 

 
int
smingEnterLexRecursion(file)
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
smingLeaveLexRecursion()
{    
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(yybuffer[--lexDepth]);
}



void
smingTrackInput(nl, parser)
    int nl;
    Parser *parser;
{
    if (nl) {
	parser->line += 1;
	parser->column = 1;
	parser->character += yyleng;
    } else {
	if ((yytext[0] != '\t') || (yyleng != 1)) {
	    parser->column += yyleng;
	} else {
	    parser->column += (8 - ((parser->column-1) % 8));
	}
	parser->character += yyleng;
    }
}


 
%}



/*
 * Lex pattern definitions.
 */
delim		([^a-zA-Z0-9-])
lineBreak       ("\n\015"|"\n")
WSP		(" "|"\t")
VCHAR		([\041-\176])
textVChar	[!#-\176]
HTAB		"\t"
SP		" "
/*
 * Lex state definitions.
 */
%s		Comment
%s		String



%%



 /*
  * Lex rules.
  */

<INITIAL>"//" {
    smingTrackInput(0, parser);
    BEGIN(Comment);
}

<Comment>{lineBreak} {
    smingTrackInput(1, parser);
    BEGIN(INITIAL);
}

<Comment>{WSP}|{VCHAR} {
    smingTrackInput(0, parser);
}

 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\{\}\(\)\;\,\-\.\|] {
    smingTrackInput(0, parser);
    return yytext[0];
}
    
<INITIAL>".." {
    smingTrackInput(0, parser);
    return DOT_DOT;
}

<INITIAL>"::" {
    smingTrackInput(0, parser);
    return COLON_COLON;
}

 /*
  * Lex rules for separators.
  */

<INITIAL>{lineBreak} {
    smingTrackInput(1, parser);
}

<INITIAL>{WSP} {
    smingTrackInput(0, parser);
}

 /*
  * Lex rules for known keywords.
  */

<INITIAL>module/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return moduleKeyword;
}

<INITIAL>import/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return importKeyword;
}

<INITIAL>revision/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return revisionKeyword;
}

<INITIAL>oid/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return oidKeyword;
}

<INITIAL>date/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return dateKeyword;
}

<INITIAL>organization/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return organizationKeyword;
}

<INITIAL>contact/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return contactKeyword;
}

<INITIAL>description/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return descriptionKeyword;
}

<INITIAL>reference/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return referenceKeyword;
}

<INITIAL>extension/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return extensionKeyword;
}

<INITIAL>typedef/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return typedefKeyword;
}

<INITIAL>type/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return typeKeyword;
}

<INITIAL>writetype/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return writetypeKeyword;
}

<INITIAL>node/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return nodeKeyword;
}

<INITIAL>scalar/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return scalarKeyword;
}

<INITIAL>table/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return tableKeyword;
}

<INITIAL>column/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return columnKeyword;
}

<INITIAL>row/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return rowKeyword;
}

<INITIAL>notification/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return notificationKeyword;
}

<INITIAL>group/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return groupKeyword;
}

<INITIAL>compliance/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return complianceKeyword;
}

<INITIAL>format/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return formatKeyword;
}

<INITIAL>units/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return unitsKeyword;
}

<INITIAL>status/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return statusKeyword;
}

<INITIAL>access/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return accessKeyword;
}

<INITIAL>default/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return defaultKeyword;
}

<INITIAL>implied/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return impliedKeyword;
}

<INITIAL>index/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return indexKeyword;
}

<INITIAL>augments/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return augmentsKeyword;
}

<INITIAL>reorders/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return reordersKeyword;
}

<INITIAL>sparse/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return sparseKeyword;
}

<INITIAL>expands/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return expandsKeyword;
}

<INITIAL>create/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return createKeyword;
}

<INITIAL>members/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return membersKeyword;
}

<INITIAL>objects/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return objectsKeyword;
}

<INITIAL>mandatory/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return mandatoryKeyword;
}

<INITIAL>optional/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return optionalKeyword;
}

<INITIAL>refine/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return refineKeyword;
}

<INITIAL>OctetString/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return OctetStringKeyword;
}

<INITIAL>ObjectIdentifier/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return ObjectIdentifierKeyword;
}

<INITIAL>Integer32/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Integer32Keyword;
}

<INITIAL>Unsigned32/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Unsigned32Keyword;
}

<INITIAL>Integer64/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Integer64Keyword;
}

<INITIAL>Unsigned64/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Unsigned64Keyword;
}

<INITIAL>Float32/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Float32Keyword;
}

<INITIAL>Float64/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Float64Keyword;
}

<INITIAL>Float128/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return Float128Keyword;
}

<INITIAL>Bits/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return BitsKeyword;
}

<INITIAL>Enumeration/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return EnumerationKeyword;
}

<INITIAL>current/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return currentKeyword;
}

<INITIAL>deprecated/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return deprecatedKeyword;
}

<INITIAL>obsolete/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return obsoleteKeyword;
}

<INITIAL>noaccess/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return noaccessKeyword;
}

<INITIAL>notifyonly/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return notifyonlyKeyword;
}

<INITIAL>readonly/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return readonlyKeyword;
}

<INITIAL>readwrite/{delim} {
    smingTrackInput(0, parser);
    yylval.id = yytext;
    return readwriteKeyword;
}

 /*
  * Lex rules for identifiers.
  */

 /* e.g. module names: REF: draft,p.12-13 */
<INITIAL>[A-Z](-?[a-zA-Z0-9]+)*-? {
    smingTrackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    if (strlen(yytext) > 64) {
	printError(parser, ERR_UCIDENTIFIER_64, yytext);
    }
    yylval.text = util_strdup(yytext);
    return ucIdentifier;
}

 /* same for lowercase names */
<INITIAL>[a-z](-?[a-zA-Z0-9]+)*-? {
    smingTrackInput(0, parser);
    if (yytext[yyleng-1] == '-') {
	printError(parser, ERR_ID_ENDS_IN_HYPHEN, yytext);
    }
    if (strlen(yytext) > 64) {
	printError(parser, ERR_LCIDENTIFIER_64, yytext);
    }
    yylval.text = util_strdup(yytext);
    return lcIdentifier;
}

 /*
  * Lex rules for numbers.
  *
  * NOTE: `-' is a separate token. Hence, there are no negative numbers.
  */

<INITIAL>0+/[0-9] {
    smingTrackInput(0, parser);
    printError(parser, ERR_LEADING_ZEROS);
}

<INITIAL>([1-9][0-9]*|0)/[^0-9] {
    smingTrackInput(0, parser);
    if ((strlen(yytext) > sizeof(MAX_NUMBER)-1) ||
	((strlen(yytext) == sizeof(MAX_NUMBER)-1) &&
	 (strcmp(yytext, MAX_NUMBER) > 0))) {
	printError(parser, ERR_NUMBER_TOO_LARGE, yytext);
    }
    yylval.text = yytext;
    return decimalNumber;
}

<INITIAL>0x[0-9a-fA-F]+/[^0-9a-fA-F] {
    smingTrackInput(0, parser);
    if (strlen(yytext) % 2) {
	printError(parser, ERR_HEX_STRING_MUL2, yytext);
    }
    yylval.text = yytext;
    return hexadecimalNumber;
}
 /*
  * Lex rules for floating point values.
  */

<INITIAL>0.00/{delim} {
    /* TODO */
    smingTrackInput(0, parser);
    yylval.text = yytext;
    return floatValue;
}

 /*
  * Lex rules for textSegments.
  */

<INITIAL>\" {
    smingTrackInput(0, parser);
    currentStringColumn = thisParser.column+1;
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = util_realloc(currentString, currentStringMaxLength);
    }
    currentStringLength = 0;
    currentString[0] = '\0';
    BEGIN(String);
}

<String>\" {
    smingTrackInput(0, parser);
    yylval.text = currentString;
    BEGIN(INITIAL);
    return textSegment;
}

 /*
  * any 7-bit displayable ASCII character
  * tab character
  * spaces, and
  * line terminator characters (\n or \r\n)
  */
<String>{textVChar} {
    smingTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = util_realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    currentStringFetchLine = 1;
}

<String>{HTAB}|{SP} {
    smingTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = util_realloc(currentString, currentStringMaxLength);
    }
    if (currentStringFetchLine || (thisParser.column >= currentStringColumn)) {
	currentString[currentStringLength] = yytext[0];
	currentString[currentStringLength+1] = '\0';
	currentStringLength++;
	currentStringFetchLine = 1;
    }
}

<String>{lineBreak} {
    smingTrackInput(1, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = util_realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = '\n';
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    currentStringFetchLine = 0;
}

<String>. {
    char a[20];

    smingTrackInput(0, parser);
    if (currentStringMaxLength <= (currentStringLength+1)) {
        currentStringMaxLength += 4096;
        currentString = util_realloc(currentString, currentStringMaxLength);
    }
    currentString[currentStringLength] = yytext[0];
    currentString[currentStringLength+1] = '\0';
    currentStringLength++;
    sprintf(a, "code %u, column %d", (unsigned char)yytext[0],
	    thisParser.column-1);
    printError(parser, ERR_ILLEGAL_CHAR_IN_STRING, a);
}

 /*
  *
  */

. {
    smingTrackInput(0, parser);
    printError(parser, ERR_LEX_UNEXPECTED_CHAR);
}

%%
