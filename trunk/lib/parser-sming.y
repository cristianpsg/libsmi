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
 * @(#) $Id: parser-sming.y,v 1.2 1999/04/01 13:05:43 strauss Exp $
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
    int            rc;				/* >=0: ok, <0: error        */
    char	   *text;
}



/*
 * Tokens and their attributes.
 */
%token DOT_DOT
%token COLON_COLON

%token <text>ucIdentifier
%token <text>lcIdentifier

%token <text>floatValue
%token <text>textSegment
%token <text>decimalNumber
%token <text>hexadecimalNumber

%token <id>moduleKeyword
%token <id>importKeyword
%token <id>revisionKeyword
%token <id>oidKeyword
%token <id>dateKeyword
%token <id>organizationKeyword
%token <id>contactKeyword
%token <id>descriptionKeyword
%token <id>referenceKeyword
%token <id>typedefKeyword
%token <id>typeKeyword
%token <id>writetypeKeyword
%token <id>nodeKeyword
%token <id>scalarKeyword
%token <id>tableKeyword
%token <id>columnKeyword
%token <id>rowKeyword
%token <id>notificationKeyword
%token <id>groupKeyword
%token <id>complianceKeyword
%token <id>formatKeyword
%token <id>unitsKeyword
%token <id>statusKeyword
%token <id>accessKeyword
%token <id>defaultKeyword
%token <id>impliedKeyword
%token <id>indexKeyword
%token <id>augmentsKeyword
%token <id>reordersKeyword
%token <id>sparseKeyword
%token <id>expandsKeyword
%token <id>createKeyword
%token <id>objectsKeyword
%token <id>notificationsKeyword
%token <id>mandatoryKeyword
%token <id>optionalKeyword
%token <id>refineKeyword
%token <id>OctetStringKeyword
%token <id>ObjectIdentifierKeyword
%token <id>Integer32Keyword
%token <id>Unsigned32Keyword
%token <id>Integer64Keyword
%token <id>Unsigned64Keyword
%token <id>Float32Keyword
%token <id>Float64Keyword
%token <id>Float128Keyword
%token <id>BitsKeyword
%token <id>EnumerationKeyword
%token <id>currentKeyword
%token <id>deprecatedKeyword
%token <id>obsoleteKeyword
%token <id>noaccessKeyword
%token <id>notifyonlyKeyword
%token <id>readonlyKeyword
%token <id>readwriteKeyword


/*
 * Types of non-terminal symbols.
 */

%type <rc>smingFile
%type <rc>moduleStatement_optsep_0n
%type <rc>moduleStatement_optsep_1n
%type <rc>moduleStatement_optsep
%type <rc>moduleStatement
%type <rc>typedefStatement_stmtsep_0n
%type <rc>typedefStatement_stmtsep_1n
%type <rc>typedefStatement_stmtsep
%type <rc>typedefStatement
%type <rc>anyObjectStatement_stmtsep_0n
%type <rc>anyObjectStatement_stmtsep_1n
%type <rc>anyObjectStatement_stmtsep
%type <rc>anyObjectStatement
%type <rc>nodeStatement
%type <rc>scalarStatement
%type <rc>tableStatement
%type <rc>rowStatement
%type <rc>columnStatement_stmtsep_1n
%type <rc>columnStatement_stmtsep
%type <rc>columnStatement
%type <rc>notificationStatement_stmtsep_0n
%type <rc>notificationStatement_stmtsep_1n
%type <rc>notificationStatement_stmtsep
%type <rc>notificationStatement
%type <rc>groupStatement_stmtsep_0n
%type <rc>groupStatement_stmtsep_1n
%type <rc>groupStatement_stmtsep
%type <rc>groupStatement
%type <rc>complianceStatement_stmtsep_0n
%type <rc>complianceStatement_stmtsep_1n
%type <rc>complianceStatement_stmtsep
%type <rc>complianceStatement
%type <rc>importStatement_stmtsep_0n
%type <rc>importStatement_stmtsep_1n
%type <rc>importStatement_stmtsep
%type <rc>importStatement
%type <rc>revisionStatement_stmtsep_0n
%type <rc>revisionStatement_stmtsep_1n
%type <rc>revisionStatement_stmtsep
%type <rc>revisionStatement
%type <rc>typedefTypeStatement
%type <rc>typeStatement_stmtsep_01
%type <rc>typeStatement
%type <rc>writetypeStatement_stmtsep_01
%type <rc>writetypeStatement
%type <rc>anyIndexStatement
%type <rc>indexStatement
%type <rc>augmentsStatement
%type <rc>reordersStatement
%type <rc>sparseStatement
%type <rc>expandsStatement
%type <rc>sep_impliedKeyword_01
%type <rc>createStatement_stmtsep_01
%type <rc>createStatement
%type <rc>optsep_createColumns_01
%type <rc>createColumns
%type <rc>oidStatement
%type <rc>dateStatement
%type <rc>organizationStatement
%type <rc>contactStatement
%type <rc>formatStatement_stmtsep_01
%type <rc>formatStatement
%type <rc>unitsStatement_stmtsep_01
%type <rc>unitsStatement
%type <rc>statusStatement_stmtsep_01
%type <rc>statusStatement
%type <rc>accessStatement_stmtsep_01
%type <rc>accessStatement
%type <rc>defaultStatement_stmtsep_01
%type <rc>defaultStatement
%type <rc>descriptionStatement_stmtsep_01
%type <rc>descriptionStatement
%type <rc>referenceStatement_stmtsep_01
%type <rc>referenceStatement
%type <rc>objectsStatement_notificationsStatement
%type <rc>objectsStatement_stmtsep_01
%type <rc>objectsStatement
%type <rc>notificationsStatement
%type <rc>mandatoryStatement_stmtsep_01
%type <rc>mandatoryStatement
%type <rc>optionalStatement_stmtsep_0n
%type <rc>optionalStatement_stmtsep_1n
%type <rc>optionalStatement_stmtsep
%type <rc>optionalStatement
%type <rc>refineStatement_stmtsep_0n
%type <rc>refineStatement_stmtsep_1n
%type <rc>refineStatement_stmtsep
%type <rc>refineStatement
%type <rc>refinedBaseType_refinedType
%type <rc>refinedBaseType
%type <rc>refinedType
%type <rc>optsep_anySpec_01
%type <rc>anySpec
%type <rc>optsep_numberSpec_01
%type <rc>numberSpec
%type <rc>furtherNumberElement_0n
%type <rc>furtherNumberElement_1n
%type <rc>furtherNumberElement
%type <rc>numberElement
%type <rc>numberUpperLimit_01
%type <rc>numberUpperLimit
%type <rc>optsep_floatSpec_01
%type <rc>floatSpec
%type <rc>furtherFloatElement_0n
%type <rc>furtherFloatElement_1n
%type <rc>furtherFloatElement
%type <rc>floatElement
%type <rc>floatUpperLimit_01
%type <rc>floatUpperLimit
%type <rc>bitsOrEnumerationSpec
%type <rc>bitsOrEnumerationList
%type <rc>furtherBitsOrEnumerationItem_0n
%type <rc>furtherBitsOrEnumerationItem_1n
%type <rc>furtherBitsOrEnumerationItem
%type <rc>bitsOrEnumerationItem
%type <rc>identifierList
%type <rc>furtherIdentifier_0n
%type <rc>furtherIdentifier_1n
%type <rc>furtherIdentifier
%type <rc>qIdentifierList
%type <rc>furtherQIdentifier_0n
%type <rc>furtherQIdentifier_1n
%type <rc>furtherQIdentifier
%type <rc>qlcIdentifierList
%type <rc>furtherQlcIdentifier_0n
%type <rc>furtherQlcIdentifier_1n
%type <rc>furtherQlcIdentifier
%type <rc>bitsValue
%type <rc>bitsList
%type <rc>furtherLcIdentifier_0n
%type <rc>furtherLcIdentifier_1n
%type <rc>furtherLcIdentifier
%type <rc>identifier
%type <rc>qIdentifier
%type <rc>qucIdentifier
%type <rc>qlcIdentifier
%type <rc>text
%type <rc>optsep_textSegment_0n
%type <rc>optsep_textSegment_1n
%type <rc>optsep_textSegment
%type <rc>date
%type <rc>format
%type <rc>units
%type <rc>anyValue
%type <rc>status
%type <rc>access
%type <rc>objectIdentifier
%type <rc>qlcIdentifier_subid
%type <rc>dot_subid_0127
%type <rc>dot_subid_1n
%type <rc>dot_subid
%type <rc>subid
%type <rc>number
%type <rc>negativeNumber
%type <rc>signedNumber
%type <rc>optsep_comma_01
%type <rc>sep
%type <rc>optsep
%type <rc>stmtsep

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
			{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    $$ = $2;
			}
	;

moduleStatement_optsep_0n:	/* empty */
			{
			    $$ = 0;
			}
	|		moduleStatement_optsep_1n
			{
			    $$ = $1;
			}
	;

moduleStatement_optsep_1n:	moduleStatement_optsep
			{
			    $$ = $1;
			}
	|		moduleStatement_optsep_1n moduleStatement_optsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * modules or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

moduleStatement_optsep:	moduleStatement optsep
			{
			    /*
			     * If we got a (Module *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
        ;

moduleStatement:	moduleKeyword sep ucIdentifier
			{
			    thisParserPtr->modulePtr = findModuleByName($3);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule($3,
					      thisParserPtr->path,
					      thisParserPtr->locationPtr,
					      thisParserPtr->character,
					      0,
					      thisParserPtr);
			    }
			    thisParserPtr->modulePtr->flags &= ~FLAG_SMIV2;
			    thisParserPtr->modulePtr->flags |= FLAG_SMING;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			}
			sep lcIdentifier
			{
			    ;
			}
			optsep '{' stmtsep
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
			    Module *modulePtr = NULL;
			    
			    printf("XXX sming module %s\n", $3);
			    return modulePtr;
			}
	;

typedefStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}

anyObjectStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
        |		typeStatement stmtsep
	;

typeStatement:		typeKeyword sep refinedBaseType_refinedType
			optsep ';'
        ;

writetypeStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
	|		sep impliedKeyword
	;

createStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |               createStatement stmtsep
	;

createStatement:	createKeyword optsep_createColumns_01 optsep ';'
        ;

optsep_createColumns_01: /* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
	|		formatStatement stmtsep
	;

formatStatement:	formatKeyword sep text optsep ';'
        ;

unitsStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |		unitsStatement stmtsep
	;

unitsStatement:		unitsKeyword sep text optsep ';'
        ;

statusStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |               statusStatement stmtsep
	;

statusStatement:	statusKeyword sep status optsep ';'
        ;

accessStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |		accessStatement stmtsep
	;

accessStatement:	accessKeyword sep access optsep ';'
        ;

defaultStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |		defaultStatement stmtsep
	;

defaultStatement:	defaultKeyword sep anyValue optsep ';'
        ;

descriptionStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |               descriptionStatement stmtsep
	;

descriptionStatement:	descriptionKeyword sep text optsep ';'
        ;

referenceStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |		referenceStatement stmtsep
	;

referenceStatement:	referenceKeyword sep text optsep ';'
        ;

objectsStatement_notificationsStatement: objectsStatement
        |		notificationsStatement
	;

objectsStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |		objectsStatement stmtsep
	;

objectsStatement:	objectsKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

notificationsStatement:	notificationsKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

mandatoryStatement_stmtsep_01: /* empty */
			{
			    $$ = 0;
			}
        |               mandatoryStatement stmtsep
	;

mandatoryStatement:	mandatoryKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
        ;

optionalStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
	|		optsep anySpec
	;

anySpec:		numberSpec
	|		floatSpec
	;

optsep_numberSpec_01:	/* empty */
			{
			    $$ = 0;
			}
	|		optsep numberSpec
	;

numberSpec:		'(' optsep numberElement furtherNumberElement_0n
			optsep ')'

furtherNumberElement_0n:	/* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
        |		numberUpperLimit
	;

numberUpperLimit:		optsep DOT_DOT optsep number
        ;

optsep_floatSpec_01:	/* empty */
			{
			    $$ = 0;
			}
        |		optsep floatSpec
	;

floatSpec:		'(' optsep floatElement furtherFloatElement_0n
			optsep ')'

furtherFloatElement_0n:	/* empty */
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
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
			{
			    $$ = 0;
			}
/*	|		optsep ',' */
	;

sep:			/* empty, skipped by lexer */
			{
			    $$ = 0;
			}
        ;			

optsep:			/* empty, skipped by lexer */
			{
			    $$ = 0;
			}
        ;			

stmtsep:		/* empty, skipped by lexer */
			{
			    $$ = 0;
			}
        ;			

%%

#endif
			
