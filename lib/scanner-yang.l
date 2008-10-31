/*
 * scanner-yang.l --
 *
 *      Lexical rules for scanning the Yang modules.
 *
 */

%option noyywrap

%{

#include <config.h>
    
#ifdef BACKEND_YANG

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#if defined(HAVE_WIN_H)
#include "win.h"
#endif

#include "error.h"
#include "util.h"
#include "parser-yang.h"
#include "parser-yang.tab.h"
#include "scanner-yang.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif



/* we need a reentrant parser, so yylex gets arguments */

#if 0
#define YY_DECL int yylex YY_PROTO((YYSTYPE *lvalp, void *parser))
#else
#define YY_DECL int yylex(YYSTYPE *lvalp, void *parser)
#endif



#define thisParser (*(Parser *) parser)



#define MAX_NUMBER		"18446744073709551615" /* max Counter64 */



/*
 * This makes the usual notation when referencing attributes also
 * work with our pure parser code.
 */
#define yylval (*lvalp)


static YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];

static int lexDepth = 0; 

 
int
yangEnterLexRecursion(file)
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
yangLeaveLexRecursion()
{    
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(yybuffer[--lexDepth]);
}

%}

%{

char* concatTmp;
int lineAdjust;

void cleanSQString()
{

char *s, *d;		/* the source and destination pointer */
    int column = 0;		/* the current column */
    int newlineflag = 0;	/* we have just passed a newline */
    int cutoffcolumn = 0;	/* cut off white space up to this column */
				/* (computed by caculating the indentation */
				/* of the first column) */

    lineAdjust = 0;
    yytext[yyleng-1] = '\0';
    for (d = yytext, s = yytext+1; s[0] ; s++, d++) {

	if ((s[0] == '\n' && s[1] == '\r')	/* newline sequence */
	    || (s[0] == '\r' && s[1] == '\n')) {
	    lineAdjust += 1;
	    d[0] = '\n';
	    s++;
	    newlineflag = 1;
	    column = 0;
	    if (cutoffcolumn < 0) cutoffcolumn = 0;
	} else if (s[0] == '\n') {		/* simple newline */
	    lineAdjust += 1;
	    d[0] = '\n';
	    newlineflag = 1;
	    column = 0;
	    if (cutoffcolumn < 0) cutoffcolumn = 0;
	} else {
	    if (newlineflag && isspace((int)(unsigned char)s[0])) {	/* space after newline */
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
		  /*  smiPrintError(parser, ERR_ILLEGAL_CHAR_IN_STRING,
				  s[0], (unsigned char) s[0]);*/
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
        //clean trailing spaces
    for (d = yytext, s = yytext; s[0] ; s++, d++) {
	if (isspace((int)(unsigned char)s[0]))
	{
		char *i = s;		
		while(isspace((int)(unsigned char)s[0]) && s[0] !=  '\n')
	 	{
		    s++;
		}
		if(s[0] != '\n')
		{
			while(i != s)
			{
				d[0] = i[0];
				d++;
				i++;
			}
		}
	}
	d[0] = s[0];
    }
    d[0] = '\0';
}

void cleanDQString()
{

char *s, *d;		/* the source and destination pointer */
    int column = 0;		/* the current column */
    int newlineflag = 0;	/* we have just passed a newline */
    int cutoffcolumn = 0;	/* cut off white space up to this column */
				/* (computed by caculating the indentation */
				/* of the first column) */

    lineAdjust = 0;
    yytext[yyleng-1] = '\0';
    for (d = yytext, s = yytext+1; s[0] ; s++, d++) {

	if ((s[0] == '\n' && s[1] == '\r')	/* newline sequence */
	    || (s[0] == '\r' && s[1] == '\n')) {
	    lineAdjust += 1;
	    d[0] = '\n';
	    s++;
	    newlineflag = 1;
	    column = 0;
	    if (cutoffcolumn < 0) cutoffcolumn = 0;
	} else if (s[0] == '\n') {		/* simple newline */
	    lineAdjust += 1;
	    d[0] = '\n';
	    newlineflag = 1;
	    column = 0;
	    if (cutoffcolumn < 0) cutoffcolumn = 0;
	} else {
	    if (newlineflag && isspace((int)(unsigned char)s[0])) {	/* space after newline */
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
		  /*  smiPrintError(parser, ERR_ILLEGAL_CHAR_IN_STRING,
				  s[0], (unsigned char) s[0]);*/
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
    //clean trailing spaces
    for (d = yytext, s = yytext; s[0] ; s++, d++) {
	if (isspace((int)(unsigned char)s[0]))
	{
		char *i = s;		
		while(isspace((int)(unsigned char)s[0]))
	 	{
		    s++;
		}
		if(s[0] != '\n')
		{
			while(i != s)
			{
				d[0] = i[0];
				d++;
				i++;
			}
		}
	}
	d[0] = s[0];
    }
    d[0] = '\0';
}
%}

/*
 * Lex pattern definitions.
 */

delim		([^a-zA-Z0-9\200-\377])
lineBreak       ("\n"|"\n\015"|"\015\n")
UTFCHAR 	([_a-zA-Z0-9\200-\377])
WSP		(" "|"\t")
VCHAR		([\041-\377])
HTAB		"\t"
SP		" "


/*
 * Lex state definitions.
 */
%s		Skipline
%x		ConcatString
%x		FindPlus

%%

 /*
  * Lex rules for comments. Do you use {lineBreak} here because it
  * introduces a trailing context which is (a) slow and (b) causes
  * REJECT to be used.
  */

<INITIAL>(\/){2}({WSP}|{VCHAR})*/"\n" {
//printf("comment1");
	thisParser.line++;
}

<INITIAL>(\/){2}({WSP}|{VCHAR})*/"\n\015" {
//printf("comment2");
	thisParser.line++;
}

<INITIAL>(\/){2}({WSP}|{VCHAR})*/"\015\n" {
//printf("comment3");
	thisParser.line++;
}

<INITIAL>"/*"([^\*]|(\*[^/]))*"*/"  {
	//printf("comment4");
}


 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\{\}\(\)\;\,\-\.\|] {
    return yytext[0];
}

	/*
  * Lex rules for separators.
  */

<INITIAL>{lineBreak} {
    thisParser.line++;
}

<INITIAL>{WSP} {
}

 /*
 *Lex rules for keywords
 */

<INITIAL>augment/{delim} {
	yylval.id = yytext;
	return augmentKeyword;
}

<INITIAL>belongs-to/{delim} {
	yylval.id = yytext;
	return belongs_toKeyword;
}

<INITIAL>choice/{delim} {
	yylval.id = yytext;
	return choiceKeyword;
}

<INITIAL>config/{delim} {
	yylval.id = yytext;
	return configKeyword;
}

<INITIAL>contact/{delim} {
	yylval.id = yytext;
	return contactKeyword;
}

<INITIAL>container/{delim} {
	yylval.id = yytext;
	return containerKeyword;
}

<INITIAL>default/{delim} {
	yylval.id = yytext;
	return defaultKeyword;
}

<INITIAL>description/{delim} {
	yylval.id = yytext;
	return descriptionKeyword;
}

<INITIAL>enum/{delim} {
	yylval.id = yytext;
	return enumKeyword;
}

<INITIAL>error-app-tag/{delim} {
	yylval.id = yytext;
	return error_app_tagKeyword;
}

<INITIAL>error-message/{delim} {
	yylval.id = yytext;
	return error_messageKeyword;
}

<INITIAL>extension/{delim} {
	yylval.id = yytext;
	return extensionKeyword;
}

<INITIAL>grouping/{delim} {
	yylval.id = yytext;
	return groupingKeyword;
}

<INITIAL>import/{delim} {
	yylval.id = yytext;
	return importKeyword;
}

<INITIAL>include/{delim} {
	yylval.id = yytext;
	return includeKeyword;
}

<INITIAL>key/{delim} {
	yylval.id = yytext;
	return keyKeyword;
}

<INITIAL>leaf/{delim} {
	yylval.id = yytext;
	return leafKeyword;
}

<INITIAL>leaf-list/{delim} {
	yylval.id = yytext;
	return leaf_listKeyword;
}

<INITIAL>length/{delim} {
	yylval.id = yytext;
	return lengthKeyword;
}

<INITIAL>list/{delim} {
	yylval.id = yytext;
	return listKeyword;
}

<INITIAL>mandatory/{delim} {
	yylval.id = yytext;
	return mandatoryKeyword;
}

<INITIAL>max-elements/{delim} {
	yylval.id = yytext;
	return max_elementsKeyword;
}

<INITIAL>min-elements/{delim} {
	yylval.id = yytext;
	return min_elementsKeyword;
}

<INITIAL>module/{delim} {
	yylval.id = yytext;
	return moduleKeyword;
}

<INITIAL>submodule/{delim} {
	yylval.id = yytext;
	return submoduleKeyword;
}

<INITIAL>must/{delim} {
	yylval.id = yytext;
	return mustKeyword;
}

<INITIAL>namespace/{delim} {
	yylval.id = yytext;
	return namespaceKeyword;
}

<INITIAL>ordered-by/{delim} {
	yylval.id = yytext;
	return ordered_byKeyword;
}

<INITIAL>organization/{delim} {
	yylval.id = yytext;
	return organizationKeyword;
}

<INITIAL>prefix/{delim} {
	yylval.id = yytext;
	return prefixKeyword;
}

<INITIAL>range/{delim} {
	yylval.id = yytext;
	return rangeKeyword;
}

<INITIAL>reference/{delim} {
	yylval.id = yytext;
	return referenceKeyword;
}

<INITIAL>pattern/{delim} {
	yylval.id = yytext;
	return patternKeyword;
}

<INITIAL>revision/{delim} {
	yylval.id = yytext;
	return revisionKeyword;
}

<INITIAL>status/{delim} {
	yylval.id = yytext;
	return statusKeyword;
}

<INITIAL>type/{delim} {
	yylval.id = yytext;
	return typeKeyword;
}

<INITIAL>typedef/{delim} {
	yylval.id = yytext;
	return typedefKeyword;
}

<INITIAL>unique/{delim} {
	yylval.id = yytext;
	return uniqueKeyword;
}

<INITIAL>units/{delim} {
	yylval.id = yytext;
	return unitsKeyword;
}

<INITIAL>uses/{delim} {
	yylval.id = yytext;
	return usesKeyword;
}

<INITIAL>value/{delim} {
	yylval.id = yytext;
	return valueKeyword;
}

<INITIAL>when/{delim} {
	yylval.id = yytext;
	return whenKeyword;
}

<INITIAL>bit/{delim} {
	yylval.id = yytext;
	return bitKeyword;
}

<INITIAL>path/{delim} {
	yylval.id = yytext;
	return pathKeyword;
}

<INITIAL>int8/{delim} {
	yylval.id = yytext;
	return int8Keyword;
}

<INITIAL>int16/{delim} {
	yylval.id = yytext;
	return int16Keyword;
}

<INITIAL>int32/{delim} {
	yylval.id = yytext;
	return int32Keyword;
}

<INITIAL>int64/{delim} {
	yylval.id = yytext;
	return int64Keyword;
}

<INITIAL>uint8/{delim} {
	yylval.id = yytext;
	return uint8Keyword;
}

<INITIAL>uint16/{delim} {
	yylval.id = yytext;
	return uint16Keyword;
}

<INITIAL>uint32/{delim} {
	yylval.id = yytext;
	return uint32Keyword;
}

<INITIAL>uint64/{delim} {
	yylval.id = yytext;
	return uint64Keyword;
}

<INITIAL>float32/{delim} {
	yylval.id = yytext;
	return float32Keyword;
}

<INITIAL>float64/{delim} {
	yylval.id = yytext;
	return float64Keyword;
}

<INITIAL>string/{delim} {
	yylval.id = yytext;
	return stringKeyword;
}

<INITIAL>boolean/{delim} {
	yylval.id = yytext;
	return booleanKeyword;
}

<INITIAL>enumeration/{delim} {
	yylval.id = yytext;
	return enumerationKeyword;
}

<INITIAL>bits/{delim} {
	yylval.id = yytext;
	return bitsKeyword;
}

<INITIAL>binary/{delim} {
	yylval.id = yytext;
	return binaryKeyword;
}

<INITIAL>keyref/{delim} {
	yylval.id = yytext;
	return keyrefKeyword;
}

<INITIAL>empty/{delim} {
	yylval.id = yytext;
	return emptyKeyword;
}

<INITIAL>anyxml/{delim} {
	yylval.id = yytext;
	return anyXMLKeyword;
}

<INITIAL>deprecated/{delim} {
	yylval.id = yytext;
	return deprecatedKeyword;
}

<INITIAL>current/{delim} {
	yylval.id = yytext;
	return currentKeyword;
}

<INITIAL>obsolete/{delim} {
	yylval.id = yytext;
	return obsoleteKeyword;
}

<INITIAL>true/{delim} {
	yylval.id = yytext;
	return trueKeyword;
}

<INITIAL>false/{delim} {
	yylval.id = yytext;
	return falseKeyword;
}

<INITIAL>case/{delim} {
	yylval.id = yytext;
	return caseKeyword;
}

<INITIAL>input/{delim} {
	yylval.id = yytext;
	return inputKeyword;
}

<INITIAL>output/{delim} {
	yylval.id = yytext;
	return outputKeyword;
}

<INITIAL>rpc/{delim} {
	yylval.id = yytext;
	return rpcKeyword;
}

<INITIAL>notification/{delim} {
	yylval.id = yytext;
	return notificationKeyword;
}

<INITIAL>argument/{delim} {
	yylval.id = yytext;
	return argumentKeyword;
}


 /*
  * Lex rules for textSegments.
  */

<INITIAL>\"([^\"]|(\\\"))*\" {
   cleanSQString();
   thisParser.line += lineAdjust;
   concatTmp = smiStrdup(yytext);
   BEGIN(FindPlus);
}

<INITIAL>\'([^\']|(\\\'))*\' {
    cleanSQString();
    thisParser.line += lineAdjust;
    concatTmp = smiStrdup(yytext);
    BEGIN(FindPlus);
}

<FindPlus>{WSP} {
}

<FindPlus>{lineBreak} {
}

<FindPlus>\+ {
	BEGIN(ConcatString);
}

<FindPlus>([^\+]|[^{WSP}]) {
	BEGIN(INITIAL);
	yyless(0);
	yylval.text = smiStrdup(concatTmp);
	smiFree(concatTmp);
	return qString;
}

<ConcatString>\"([^\"]|(\\\"))*\" {
    	cleanDQString();
	thisParser.line += lineAdjust;
	char *tmp = (char*)smiMalloc(strlen(concatTmp) + strlen(yytext) + 1);
	strcpy(tmp, concatTmp);
	smiFree(concatTmp);
	concatTmp = tmp;
	strcat(concatTmp,yytext);


	BEGIN(FindPlus);
}

<ConcatString>\'([^\']|(\\\'))*\' {
    	cleanSQString();
	thisParser.line += lineAdjust;
	char *tmp = (char*)smiMalloc(strlen(concatTmp) + strlen(yytext) + 2);
	strcpy(tmp, concatTmp);
	smiFree(concatTmp);
	concatTmp = tmp;
	strcat(concatTmp,"\n");
	strcat(concatTmp,yytext);

	BEGIN(FindPlus);
}

<ConcatString>{lineBreak} {
    thisParser.line++;
}

<ConcatString>{WSP} {
}


 /*
  * Lex rules for numbers.
  *
  * NOTE: `-' is a separate token. Hence, there are no negative numbers.
  */

<INITIAL>0+/[0-9] {
    printf("err leading zeroes");
}

<INITIAL>([1-9][0-9]*|0)/[^0-9] {
   yylval.text = smiStrdup(yytext);
   return decimalNumber;
}

<INITIAL>0x[0-9a-fA-F]+/[^0-9a-fA-F] {
     yylval.text = smiStrdup(yytext);
     return hexNumber;
}
 /*
  * Lex rules for floating point values.
  */

<INITIAL>([0-9]*\.[0-9]+([eE][+-]?[0-9]+)?)/[^0-9] {
   yylval.text = smiStrdup(yytext);
   return floatNumber;
}


 /*
  * Lex rules for identifiers.
  * note that these may be unquoted strings
  */

<INITIAL>([a-zA-Z0-9_:\-]+)*/{delim} {
    yylval.text = smiStrdup(yytext);
    return identifier;
}

 /*
  * Rule for unquoted string.
  * It is at the end because it potentially matches all the other tokens
  */
<INITIAL>{UTFCHAR}*/{delim} {
   	yylval.text = smiStrdup(yytext);
	return uqString;
}

%%

#endif
