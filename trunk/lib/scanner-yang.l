/*
 * scanner-yang.l --
 *
 *      Lexical rules for scanning the Yang modules.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *  Authors: Kaloyan Kanev, Siarhei Kuryla
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

//#define DEBUG

#ifdef DEBUG 
#define debug(args...) fprintf (stderr, "[%s=%s]", args);
#define _debug(args...) fprintf (stderr, "[%s=%s]", args, yylval.text);
#else
#define debug
#define _debug
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

int getNumberOfCR() {
   int j, count = 0;
   for (j = 0; j <  yyleng; j++) {
        if (*(yytext + j) == '\n') {
            count++;
        }
   }
   return count;
}

%}

/* RFC 4234 core rules. */
ALPHA           [A-Za-z]
CR              \015 
CRLF            {CR}{LF}

DIGIT           [0-9]
NONZERODIGIT    [1-9]
POSDECIMAL      {NONZERODIGIT}{DIGIT}*
NONNEGDECIMAL   (0|{POSDECIMAL})
DECIMAL         ('-'{NONNEGDECIMAL}|{NONNEGDECIMAL})

HTAB            "\t"
LF              \012
SP              " "
VCHAR           ([\041-\377])
WSP             ({SP}|{HTAB})

/*
 * Lex pattern definitions.
 */
yangVersion     1
date            {DIGIT}{4}"-"{DIGIT}{2}"-"{DIGIT}{2}
identifier      ({ALPHA}|"_")({ALPHA}|{DIGIT}|"_"|"-"|".")*
lineBreak       ({CRLF}|{LF})
unquotedString  ([^ \n\t;{}/*]|(\*[^/])|(\/[^*/]))+

delim		([{; \n\t\015\012/])
UTFCHAR 	([_a-zA-Z0-9\200-\377])

currentKeyword      current
deprecatedKeyword   deprecated
obsoleteKeyword     obsolete
falseKeyword        false
trueKeyword         true
notSupported        not-supported
addKeyword          add
deleteKeyword       delete
replaceKeyword      replace

/*
 * Lex state definitions.
 */
%x		ConcatString
%x		FindPlus

%%

 /*
  * Lex rules for comments. We don't use {lineBreak} here because it
  * introduces a trailing context which is slow. To avoid it we add two rules for single line comments.
  */
<INITIAL>(\/){2}({WSP}|{VCHAR})*/{LF} {
}

<FindPlus>(\/){2}({WSP}|{VCHAR})*/{LF} {
}

<ConcatString>(\/){2}({WSP}|{VCHAR})*/{LF} {
}

<INITIAL>(\/){2}({WSP}|{VCHAR})*/{CRLF} {
}

<FindPlus>(\/){2}({WSP}|{VCHAR})*/{CRLF} {
}

<ConcatString>(\/){2}({WSP}|{VCHAR})*/{CRLF} {
}

<INITIAL>"/*"([^\*]|(\*[^/]))*"*/"  {
        thisParser.line += getNumberOfCR();
}

<FindPlus>"/*"([^\*]|(\*[^/]))*"*/"  {
        thisParser.line += getNumberOfCR();
}

<ConcatString>"/*"([^\*]|(\*[^/]))*"*/"  {
        thisParser.line += getNumberOfCR();
}

 /*
  * Lex rules for some special tokens.
  */

<INITIAL>[\{\}\;] {
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
  * Lex rules for statment keywords 
  */

<INITIAL>anyxml/{delim} {
	yylval.id = yytext;
	return anyXMLKeyword;
}

<INITIAL>argument/{delim} {
	yylval.id = yytext;
	return argumentKeyword;
}

<INITIAL>augment/{delim} {
	yylval.id = yytext;
	return augmentKeyword;
}

<INITIAL>base/{delim} {
	yylval.id = yytext;
	return baseKeyword;
}

<INITIAL>belongs-to/{delim} {
	yylval.id = yytext;
	return belongs_toKeyword;
}

<INITIAL>bit/{delim} {
	yylval.id = yytext;
	return bitKeyword;
}

<INITIAL>case/{delim} {
	yylval.id = yytext;
	return caseKeyword;
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

<INITIAL>deviation/{delim} {
	yylval.id = yytext;
	return deviationKeyword;
}

<INITIAL>deviate/{delim} {
	yylval.id = yytext;
	return deviateKeyword;
}

<INITIAL>feature/{delim} {
	yylval.id = yytext;
	return featureKeyword;
}

<INITIAL>grouping/{delim} {
	yylval.id = yytext;
	return groupingKeyword;
}

<INITIAL>identity/{delim} {
	yylval.id = yytext;
	return identityKeyword;
}

<INITIAL>if-feature/{delim} {
	yylval.id = yytext;
	return ifFeatureKeyword;
}

<INITIAL>import/{delim} {
	yylval.id = yytext;
	return importKeyword;
}

<INITIAL>include/{delim} {
	yylval.id = yytext;
	return includeKeyword;
}

<INITIAL>input/{delim} {
	yylval.id = yytext;
	return inputKeyword;
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

<INITIAL>must/{delim} {
	yylval.id = yytext;
	return mustKeyword;
}

<INITIAL>namespace/{delim} {
	yylval.id = yytext;
	return namespaceKeyword;
}

<INITIAL>notification/{delim} {
	yylval.id = yytext;
	return notificationKeyword;
}

<INITIAL>ordered-by/{delim} {
	yylval.id = yytext;
	return ordered_byKeyword;
}

<INITIAL>organization/{delim} {
	yylval.id = yytext;
	return organizationKeyword;
}

<INITIAL>output/{delim} {
	yylval.id = yytext;
	return outputKeyword;
}

<INITIAL>path/{delim} {
	yylval.id = yytext;
	return pathKeyword;
}

<INITIAL>pattern/{delim} {
	yylval.id = yytext;
	return patternKeyword;
}

<INITIAL>position/{delim} {
	yylval.id = yytext;
	return positionKeyword;
}

<INITIAL>prefix/{delim} {
	yylval.id = yytext;
	return prefixKeyword;
}

<INITIAL>presence/{delim} {
	yylval.id = yytext;
	return presenceKeyword;
}

<INITIAL>range/{delim} {
	yylval.id = yytext;
	return rangeKeyword;
}

<INITIAL>reference/{delim} {
	yylval.id = yytext;
	return referenceKeyword;
}

<INITIAL>refine/{delim} {
	yylval.id = yytext;
	return refineKeyword;
}

<INITIAL>require-instance/{delim} {
	yylval.id = yytext;
	return requireInstanceKeyword;
}

<INITIAL>revision/{delim} {
	yylval.id = yytext;
	return revisionKeyword;
}

<INITIAL>rpc/{delim} {
	yylval.id = yytext;
	return rpcKeyword;
}

<INITIAL>status/{delim} {
	yylval.id = yytext;
	return statusKeyword;
}

<INITIAL>submodule/{delim} {
	yylval.id = yytext;
	return submoduleKeyword;
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

<INITIAL>yang-version/{delim} {
	yylval.id = yytext;
	return yangversionKeyword;
}

<INITIAL>yin-element/{delim} {
	yylval.id = yytext;
	return yinElementKeyword;
}

 /*
  * Lex rules for other keywords
  */

<INITIAL>({currentKeyword}|\"{currentKeyword}\"|\'{currentKeyword}\')/{delim} {
	yylval.id = yytext;
	return currentKeyword;
}

<INITIAL>({deprecatedKeyword}|\"{deprecatedKeyword}\"|\'{deprecatedKeyword}\')/{delim} {
	yylval.id = yytext;
	return deprecatedKeyword;
}

<INITIAL>({falseKeyword}|\"{falseKeyword}\"|\'{falseKeyword}\')/{delim} {
	yylval.id = yytext;
	return falseKeyword;
}

<INITIAL>({obsoleteKeyword}|\"{obsoleteKeyword}\"|\'{obsoleteKeyword}\')/{delim} {
	yylval.id = yytext;
	return obsoleteKeyword;
}

<INITIAL>({trueKeyword}|\"{trueKeyword}\"|\'{trueKeyword}\')/{delim} {
	yylval.id = yytext;
	return trueKeyword;
}

<INITIAL>({notSupported}|\"{notSupported}\"|\'{notSupported}\')/{delim} {
	yylval.id = yytext;
	return notSupportedKeyword;
}

<INITIAL>({addKeyword}|\"{addKeyword}\"|\'{addKeyword}\')/{delim} {
	yylval.id = yytext;
	return addKeyword;
}

<INITIAL>({deleteKeyword}|\"{deleteKeyword}\"|\'{deleteKeyword}\')/{delim} {
	yylval.id = yytext;
	return deleteKeyword;
}

<INITIAL>({replaceKeyword}|\"{replaceKeyword}\"|\'{replaceKeyword}\')/{delim} {
	yylval.id = yytext;
	return replaceKeyword;
}

 /*
  * Lex rules for identifiers.
  * Note that these may be either single and double quoted or unquoted strings
  */
<INITIAL>{identifier}/{delim} {
    yylval.text = smiStrdup(yytext);
    _debug("uIdentifier");
    return identifier;
}

<INITIAL>(\"{identifier}\"|\'{identifier}\')/{delim} {
    yylval.text = smiStrndup((yytext + 1), yyleng - 2);
    _debug("qIdentifier");
    return identifier;
}

 /*
  * Lex rules for identifier-ref-arg-str.
  * Note that these may be either single and double quoted or unquoted strings
  */
<INITIAL>{identifier}:{identifier}/{delim} {
    yylval.text = smiStrdup(yytext);
    _debug("uIdentifierRefArgStr");
    return identifierRefArg;
}

<INITIAL>(\"{identifier}:{identifier}\"|\'{identifier}:{identifier}\')/{delim} {
    yylval.text = smiStrndup((yytext + 1), yyleng - 2);
    _debug("qIdentifierRefArgStr");
    return identifierRefArgStr;
}

 /*
  * Lex rules for dates.
  * Note that these may be either single and double quoted or unquoted strings
  */
<INITIAL>{date}/{delim} {    
    yylval.text = smiStrdup(yytext);
    _debug("uDate");
    return dateString;
}


<INITIAL>(\"{date}\"|\'{date}\')/{delim} {
    yylval.text = smiStrndup((yytext + 1), yyleng - 2);
    _debug("qDate");
    return dateString;
}

 /*
  * Lex rules for dates.
  * Note that these may be either single and double quoted or unquoted strings
  */
<INITIAL>{yangVersion}/{delim} {    
    yylval.text = smiStrdup(yytext);
    _debug("uYangVersion");
    return yangVersion;
}


<INITIAL>(\"{yangVersion}\"|\'{yangVersion}\')/{delim} {
    yylval.text = smiStrndup((yytext + 1), yyleng - 2);
    _debug("qYangVersion");
    return yangVersion;
}

 /*
  * Lex rules for textSegments.
  */

<INITIAL>\"([^\"]|(\\\"))*\"/{delim} {
   cleanSQString();
   thisParser.line += lineAdjust;
   concatTmp = smiStrdup(yytext);
   BEGIN(FindPlus);
}

<INITIAL>\'([^\'])*\'/{delim} {
    cleanSQString();
    thisParser.line += lineAdjust;
    concatTmp = smiStrdup(yytext);
    BEGIN(FindPlus);
}

<FindPlus>{WSP} {
}

<FindPlus>{lineBreak} {
    thisParser.line++;
}

<FindPlus>\+ {
	BEGIN(ConcatString);
}

 /* TODO: '+' is added temporarily, actually here might be any character,
  * but it's not an error because there is a separate rule for '+' above.
  */
<FindPlus>([^+]) {
	BEGIN(INITIAL);
	yyless(0); // returns the character of the current token back to the input stream
	yylval.text = smiStrdup(concatTmp);
	smiFree(concatTmp);
        _debug("qSring");
	return qString;
}

<ConcatString>\"([^\"]|(\\\"))*\" {
    	cleanDQString();
	thisParser.line += lineAdjust;
	char *tmp = (char*)smiMalloc(strlen(concatTmp) + strlen(yytext) + 1);
	strcpy(tmp, concatTmp);
	smiFree(concatTmp);
	concatTmp = tmp;
	strcat(concatTmp, yytext);
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

<INITIAL>{unquotedString}/{delim} {
        yylval.text = smiStrdup(yytext);
        _debug("uString");
        return uqString;
}
%%


#endif
