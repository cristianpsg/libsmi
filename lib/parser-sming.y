/*
 * parser-sming.y --
 *
 *      Syntax rules for parsing the SMIng MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: parser-sming.y,v 1.1 1999/03/31 17:24:26 strauss Exp $
 */

%{

#ifdef BACKEND_SMING
    
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
    
#ifdef linux
#include <getopt.h>
#endif

#include "defs.h"
#include "smi.h"
#include "error.h"
#include "parser-sming.h"
#include "scanner-sming.h"
#include "data.h"
#include "util.h"
    


/*
 * These arguments are passed to yyparse() and yylex().
 */
#define YYPARSE_PARAM parserPtr
#define YYLEX_PARAM   parserPtr

    
    
#define thisParserPtr      ((Parser *)parserPtr)
#define thisModulePtr     (((Parser *)parserPtr)->modulePtr)


    
/*
 * NOTE: The argument lvalp ist not really a void pointer. Unfortunately,
 * we don't know it better at this point. bison generated C code declares
 * YYSTYPE just a few lines below based on the `%union' declaration.
 */
extern int yylex(void *lvalp, Parser *parserPtr);



%}

/*
 * The grammars start symbol.
 */
%start smingFile



/*
 * We call the parser from within the parser when IMPORTing modules,
 * hence we need reentrant parser code. This is a bison feature.
 */
%pure_parser



/*
 * The attributes.
 */
%union {
    char           *id;				/* identifier name           */
    int            err;				/* actually just a dummy     */
    char	   *text;
}



/*
 * Tokens and their attributes.
 */
%token DOT_DOT
%token COLON_COLON

%token ucIdentifier
%token lcIdentifier

%token floatValue
%token textSegment
%token decimalNumber
%token hexadecimalNumber

%token moduleKeyword
%token importKeyword
%token revisionKeyword
%token oidKeyword
%token dateKeyword
%token organizationKeyword
%token contactKeyword
%token descriptionKeyword
%token referenceKeyword
%token typedefKeyword
%token typeKeyword
%token writetypeKeyword
%token nodeKeyword
%token scalarKeyword
%token tableKeyword
%token columnKeyword
%token rowKeyword
%token notificationKeyword
%token groupKeyword
%token complianceKeyword
%token formatKeyword
%token unitsKeyword
%token statusKeyword
%token accessKeyword
%token defaultKeyword
%token impliedKeyword
%token indexKeyword
%token augmentsKeyword
%token reordersKeyword
%token sparseKeyword
%token expandsKeyword
%token createKeyword
%token objectsKeyword
%token notificationsKeyword
%token mandatoryKeyword
%token optionalKeyword
%token refineKeyword
%token OctetStringKeyword
%token ObjectIdentifierKeyword
%token Integer32Keyword
%token Unsigned32Keyword
%token Integer64Keyword
%token Unsigned64Keyword
%token Float32Keyword
%token Float64Keyword
%token Float128Keyword
%token BitsKeyword
%token EnumerationKeyword
%token currentKeyword
%token deprecatedKeyword
%token obsoleteKeyword
%token noaccessKeyword
%token notifyonlyKeyword
%token readonlyKeyword
%token readwriteKeyword


/*
 * Types of non-terminal symbols.
 */

/*
%type <err> smingFile
%type <err>moduleStatement_optsep_0n
%type <err>moduleStatement_optsep_1n
%type <err>moduleStatement_optsep
%type <err>moduleStatement
*/

%%

/*
 * Yacc rules.
 *
 */


/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
smingFile:		optsep moduleStatement_optsep_0n
	;

moduleStatement_optsep_0n:	/* empty */
	|		moduleStatement_optsep_1n
	;

moduleStatement_optsep_1n:	moduleStatement_optsep
	|		moduleStatement_optsep_1n moduleStatement_optsep
        ;

moduleStatement_optsep:	moduleStatement optsep
        ;

moduleStatement:	moduleKeyword sep ucIdentifier
			sep lcIdentifier optsep '{' stmtsep
			importStatement_stmtsep_0n
			oidStatement stmtsep
			organizationStatement stmtsep
			contactStatement stmtsep
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			revisionStatement_stmtsep_0n
			typedefStatement_stmtsep_0n
			anyObjectStatement_stmtsep_0n
			notificationStatement_stmtsep_0n
			groupStatement_stmtsep_0n
			complianceStatement_stmtsep_0n
			'}' optsep ';'
			{
			    printf("XXX sming module\n");
			}
	;

typedefStatement_stmtsep_0n: /* empty */
        |		typedefStatement_stmtsep_1n
	;

typedefStatement_stmtsep_1n: typedefStatement_stmtsep
        |		     typedefStatement_stmtsep_1n
			     typedefStatement_stmtsep
        ;

typedefStatement_stmtsep: typedefStatement stmtsep
        ;

typedefStatement:	typedefKeyword sep ucIdentifier optsep '{' stmtsep
			typedefTypeStatement stmtsep
			defaultStatement_stmtsep_01
			formatStatement_stmtsep_01
			unitsStatement_stmtsep_01
			statusStatement_stmtsep_01
			descriptionStatement_stmtsep_01
			referenceStatement_stmtsep_01
			'}' optsep ';'

anyObjectStatement_stmtsep_0n: /* empty */
        |		anyObjectStatement_stmtsep_1n
	;

anyObjectStatement_stmtsep_1n: anyObjectStatement_stmtsep
        |		anyObjectStatement_stmtsep_1n
			anyObjectStatement_stmtsep
        ;

anyObjectStatement_stmtsep: anyObjectStatement stmtsep
        ;

anyObjectStatement:	nodeStatement
        |		scalarStatement
	|		tableStatement
	;

nodeStatement:		nodeKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			descriptionStatement_stmtsep_01
			referenceStatement_stmtsep_01
			'}' optsep ';'
        ;

scalarStatement:	scalarKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			typeStatement stmtsep
			accessStatement stmtsep
			defaultStatement_stmtsep_01
			formatStatement_stmtsep_01
			unitsStatement_stmtsep_01
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			'}' optsep ';'
        ;

tableStatement:		tableKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			rowStatement stmtsep
			'}' optsep ';'
        ;

rowStatement:		rowKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			anyIndexStatement stmtsep
			createStatement_stmtsep_01
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			columnStatement_stmtsep_1n
			'}' optsep ';'
        ;

columnStatement_stmtsep_1n: columnStatement_stmtsep
        |		columnStatement_stmtsep_1n columnStatement_stmtsep
	;

columnStatement_stmtsep: columnStatement stmtsep
        ;

columnStatement:	columnKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			typeStatement stmtsep
			accessStatement stmtsep
			defaultStatement_stmtsep_01
			formatStatement_stmtsep_01
			unitsStatement_stmtsep_01
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			'}' optsep ';'
        ;

notificationStatement_stmtsep_0n: /* empty */
        |		notificationStatement_stmtsep_1n
	;

notificationStatement_stmtsep_1n: notificationStatement_stmtsep
        |		notificationStatement_stmtsep_1n
			notificationStatement_stmtsep
        ;

notificationStatement_stmtsep: notificationStatement stmtsep
        ;

notificationStatement:	notificationKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			objectsStatement_stmtsep_01
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			'}' optsep ';'
        ;

groupStatement_stmtsep_0n: /* empty */
                        groupStatement_stmtsep_1n
        ;

groupStatement_stmtsep_1n: groupStatement_stmtsep
        |               groupStatement_stmtsep_1n groupStatement_stmtsep
        ;

groupStatement_stmtsep: groupStatement stmtsep
        ;

groupStatement:		groupKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			objectsStatement_notificationsStatement stmtsep
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			'}' optsep ';'
        ;

complianceStatement_stmtsep_0n: /* empty */
        |               complianceStatement_stmtsep_1n
	;

complianceStatement_stmtsep_1n: complianceStatement_stmtsep
        |               complianceStatement_stmtsep_1n
			complianceStatement_stmtsep
        ;

complianceStatement_stmtsep: complianceStatement stmtsep
        ;

complianceStatement:	complianceKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			mandatoryStatement_stmtsep_01
			optionalStatement_stmtsep_0n
			refineStatement_stmtsep_0n
			'}' optsep ';'
        ;

importStatement_stmtsep_0n: /* empty */
			importStatement_stmtsep_1n
        ;

importStatement_stmtsep_1n: importStatement_stmtsep
        |		importStatement_stmtsep_1n importStatement_stmtsep
        ;

importStatement_stmtsep: importStatement stmtsep
        ;

importStatement:	importKeyword sep ucIdentifier optsep '(' optsep
			identifierList optsep ')' optsep ';'
        ;

revisionStatement_stmtsep_0n: /* empty */
        |		revisionStatement_stmtsep_1n
        ;

revisionStatement_stmtsep_1n: revisionStatement_stmtsep
        |		revisionStatement_stmtsep_1n revisionStatement_stmtsep
        ;

revisionStatement_stmtsep: revisionStatement stmtsep
        ;

revisionStatement:	revisionKeyword optsep '{' stmtsep
			dateStatement stmtsep
			descriptionStatement stmtsep
			'}' optsep ';'
        ;

typedefTypeStatement:	typeKeyword sep refinedBaseType optsep ';'
	;

typeStatement_stmtsep_01: /* empty */
        |		typeStatement stmtsep
	;

typeStatement:		typeKeyword sep refinedBaseType_refinedType
			optsep ';'
        ;

writetypeStatement_stmtsep_01: /* empty */
        |	        writetypeStatement stmtsep
	;

writetypeStatement:	writetypeKeyword sep refinedBaseType_refinedType
			optsep ';'
        ;

anyIndexStatement:	indexStatement
        |		augmentsStatement
	|		reordersStatement
	|		sparseStatement
	|		expandsStatement
	;

indexStatement:		indexKeyword sep_impliedKeyword_01 optsep
			'(' optsep qlcIdentifierList optsep ')' optsep ';'
        ;

augmentsStatement:	augmentsKeyword sep qlcIdentifier optsep ';'
        ;

reordersStatement:	reordersKeyword sep qlcIdentifier
			sep_impliedKeyword_01 optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

sparseStatement:	sparseKeyword sep qlcIdentifier optsep ';'
	;

expandsStatement:	expandsKeyword sep qlcIdentifier
			sep_impliedKeyword_01 optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

sep_impliedKeyword_01:	/* empty */
	|		sep impliedKeyword
	;

createStatement_stmtsep_01: /* empty */
        |               createStatement stmtsep
	;

createStatement:	createKeyword optsep_createColumns_01 optsep ';'
        ;

optsep_createColumns_01: /* empty */
        |		optsep createColumns
        ;

createColumns:		'(' optsep qlcIdentifierList optsep ')'
        ;

oidStatement:		oidKeyword sep objectIdentifier optsep ';'
        ;

dateStatement:		dateKeyword sep date optsep ';'
        ;

organizationStatement:	organizationKeyword sep text optsep ';'
        ;

contactStatement:	contactKeyword sep text optsep ';'
        ;

formatStatement_stmtsep_01: /* empty */
	|		formatStatement stmtsep
	;

formatStatement:	formatKeyword sep text optsep ';'
        ;

unitsStatement_stmtsep_01: /* empty */
        |		unitsStatement stmtsep
	;

unitsStatement:		unitsKeyword sep text optsep ';'
        ;

statusStatement_stmtsep_01: /* empty */
        |               statusStatement stmtsep
	;

statusStatement:	statusKeyword sep status optsep ';'
        ;

accessStatement_stmtsep_01: /* empty */
        |		accessStatement stmtsep
	;

accessStatement:	accessKeyword sep access optsep ';'
        ;

defaultStatement_stmtsep_01: /* empty */
        |		defaultStatement stmtsep
	;

defaultStatement:	defaultKeyword sep anyValue optsep ';'
        ;

descriptionStatement_stmtsep_01: /* empty */
        |               descriptionStatement stmtsep
	;

descriptionStatement:	descriptionKeyword sep text optsep ';'
        ;

referenceStatement_stmtsep_01: /* empty */
        |		referenceStatement stmtsep
	;

referenceStatement:	referenceKeyword sep text optsep ';'
        ;

objectsStatement_notificationsStatement: objectsStatement
        |		notificationsStatement
	;

objectsStatement_stmtsep_01: /* empty */
        |		objectsStatement stmtsep
	;

objectsStatement:	objectsKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

notificationsStatement:	notificationsKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

mandatoryStatement_stmtsep_01: /* empty */
        |               mandatoryStatement stmtsep
	;

mandatoryStatement:	mandatoryKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

optionalStatement_stmtsep_0n: /* empty */
        |               optionalStatement_stmtsep_1n
	;

optionalStatement_stmtsep_1n: optionalStatement_stmtsep
        |		optionalStatement_stmtsep_1n
			optionalStatement_stmtsep
	;

optionalStatement_stmtsep: optionalStatement stmtsep
        ;

optionalStatement:	optionalKeyword sep qlcIdentifier optsep '{'
			descriptionStatement stmtsep '}' optsep ';'
        ;

refineStatement_stmtsep_0n: /* empty */
        |               refineStatement_stmtsep_1n
	;

refineStatement_stmtsep_1n: refineStatement_stmtsep
        |		refineStatement_stmtsep_1n refineStatement_stmtsep
	;

refineStatement_stmtsep: refineStatement stmtsep
        ;

refineStatement:	refineKeyword sep qlcIdentifier optsep '{'
			typeStatement_stmtsep_01
			writetypeStatement_stmtsep_01
			accessStatement_stmtsep_01
			descriptionStatement stmtsep '}' optsep ';'
        ;

refinedBaseType_refinedType: refinedBaseType
        |		refinedType
	;

refinedBaseType:	OctetStringKeyword optsep_numberSpec_01
        |		ObjectIdentifierKeyword
	|		Integer32Keyword optsep_numberSpec_01
	|		Unsigned32Keyword optsep_numberSpec_01
	|		Integer64Keyword optsep_numberSpec_01
	|		Unsigned64Keyword optsep_numberSpec_01
	|		Float32Keyword optsep_floatSpec_01
	|		Float64Keyword optsep_floatSpec_01
	|		Float128Keyword optsep_floatSpec_01
	|		EnumerationKeyword bitsOrEnumerationSpec
	|		BitsKeyword bitsOrEnumerationSpec
	;

refinedType:		qucIdentifier optsep_anySpec_01
	;

optsep_anySpec_01:	/* empty */
	|		optsep anySpec
	;

anySpec:		numberSpec
	|		floatSpec
	;

optsep_numberSpec_01:	/* empty */
	|		optsep numberSpec
	;

numberSpec:		'(' optsep numberElement furtherNumberElement_0n
			optsep ')'

furtherNumberElement_0n:	/* empty */
        |		furtherNumberElement_1n
        ;

furtherNumberElement_1n:	furtherNumberElement
        |		furtherNumberElement_1n furtherNumberElement
        ;

furtherNumberElement:	optsep '|' optsep numberElement
	;

numberElement:		number numberUpperLimit_01
	;

numberUpperLimit_01:	/* empty */
        |		numberUpperLimit
	;

numberUpperLimit:		optsep DOT_DOT optsep number
        ;

optsep_floatSpec_01:	/* empty */
        |		optsep floatSpec
	;

floatSpec:		'(' optsep floatElement furtherFloatElement_0n
			optsep ')'

furtherFloatElement_0n:	/* empty */
        |		furtherFloatElement_1n
        ;

furtherFloatElement_1n:	furtherFloatElement
        |		furtherFloatElement_1n furtherFloatElement
        ;

furtherFloatElement:	optsep '|' optsep floatElement
	;

floatElement:		floatValue floatUpperLimit_01
	;

floatUpperLimit_01:	/* empty */
        |		floatUpperLimit
	;

floatUpperLimit:	optsep DOT_DOT optsep floatValue
        ;

bitsOrEnumerationSpec:	'(' optsep bitsOrEnumerationList optsep ')'
        ;

bitsOrEnumerationList:	bitsOrEnumerationItem furtherBitsOrEnumerationItem_0n
			optsep_comma_01
        ;

furtherBitsOrEnumerationItem_0n: /* empty */
        |		 furtherBitsOrEnumerationItem_1n
        ;

furtherBitsOrEnumerationItem_1n: furtherBitsOrEnumerationItem
        |		furtherBitsOrEnumerationItem_1n
			furtherBitsOrEnumerationItem
        ;

	
furtherBitsOrEnumerationItem: optsep ',' optsep bitsOrEnumerationItem
        ;

bitsOrEnumerationItem:	lcIdentifier optsep '(' optsep number optsep ')'
        ;

identifierList:		identifier furtherIdentifier_0n optsep_comma_01
        ;

furtherIdentifier_0n:	/* empty */
        |		 furtherIdentifier_1n
        ;

furtherIdentifier_1n:	furtherIdentifier
        |		furtherIdentifier_1n furtherIdentifier
        ;

furtherIdentifier:	optsep ',' optsep identifier
        ;

qIdentifierList:	qIdentifier furtherQIdentifier_0n optsep_comma_01
        ;

furtherQIdentifier_0n:	/* empty */
        |		 furtherQIdentifier_1n
        ;

furtherQIdentifier_1n:	furtherQIdentifier
        |		furtherQIdentifier_1n furtherQIdentifier
        ;

furtherQIdentifier:	optsep ',' optsep qIdentifier
        ;

qlcIdentifierList:	qlcIdentifier furtherQlcIdentifier_0n optsep_comma_01
        ;

furtherQlcIdentifier_0n: /* empty */
        |		 furtherQlcIdentifier_1n
        ;

furtherQlcIdentifier_1n:	furtherQlcIdentifier
        |		furtherQlcIdentifier_1n furtherQlcIdentifier
        ;

furtherQlcIdentifier:	optsep ',' optsep qlcIdentifier
        ;

bitsValue:		'(' optsep bitsList optsep ')'
        ;

bitsList:		optsep_comma_01
        |		lcIdentifier furtherLcIdentifier_0n optsep_comma_01
	;

furtherLcIdentifier_0n:	/* empty */
        |		furtherLcIdentifier_1n
	;

furtherLcIdentifier_1n:	furtherLcIdentifier
        |		furtherLcIdentifier_1n furtherLcIdentifier
	;

furtherLcIdentifier:	optsep ',' optsep lcIdentifier
        ;

identifier:		ucIdentifier
        |		lcIdentifier
	;

qIdentifier:		qucIdentifier
        |		qlcIdentifier
	;

qucIdentifier:		ucIdentifier COLON_COLON ucIdentifier
        |		ucIdentifier
	;

qlcIdentifier:		ucIdentifier COLON_COLON lcIdentifier
        |		lcIdentifier
	;

text:			textSegment optsep_textSegment_0n
        ;

optsep_textSegment_0n:	/* empty */
        |		optsep_textSegment_1n
	;

optsep_textSegment_1n:	optsep_textSegment
        |		optsep_textSegment_1n optsep_textSegment
	;

optsep_textSegment:	optsep textSegment
        ;

date:			textSegment
			/* TODO: check "YYYY-MM-DD HH:MM" or "YYYY-MM-DD" */
        ;

format:			textSegment
        ;

units:			textSegment
        ;

anyValue:		bitsValue
	|		negativeNumber
	|		hexadecimalNumber
	|		floatValue
	|		text
	|		objectIdentifier
			/* also label in case of enum */
			/* also number */
	;

status:			currentKeyword
        |		deprecatedKeyword
	|		obsoleteKeyword
	;

access:			noaccessKeyword
        |		notifyonlyKeyword
	|		readonlyKeyword
	|		readwriteKeyword
	;

objectIdentifier:	qlcIdentifier_subid dot_subid_0127
	;

qlcIdentifier_subid:	qlcIdentifier
        |		subid
	;

dot_subid_0127:		/* empty */
        |		dot_subid_1n
	;

dot_subid_1n:		dot_subid
        |		dot_subid_1n dot_subid
	;

dot_subid:		'.' subid
        ;

subid:			decimalNumber
        ;

number:			hexadecimalNumber
        |		decimalNumber
	;

negativeNumber:		'-' decimalNumber
        ;

signedNumber:		decimalNumber
        |		negativeNumber
	;

/* decimalNumber, hexadecimalNumber, floatValue */

/* unknown... */

optsep_comma_01:	/* empty */
/*	|		optsep ',' */
	;

sep:			/* empty, skipped by lexer */
        ;			

optsep:			/* empty, skipped by lexer */
        ;			

stmtsep:		/* empty, skipped by lexer */
        ;			

%%

#endif
			
