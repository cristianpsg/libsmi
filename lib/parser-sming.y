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
 * @(#) $Id: parser-sming.y,v 1.4 1999/04/06 16:23:23 strauss Exp $
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



static char *identifier;
static Object *objectPtr;
 


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
    Module	   *modulePtr;
    Node	   *nodePtr;
    Object	   *objectPtr;
    Type	   *typePtr;
    Index	   *indexPtr;
    Option	   *optionPtr;
    Refinement	   *refinementPtr;
    SmiStatus	   status;
    SmiAccess	   access;
    SmiNamedNumber *namedNumberPtr;
    SmiRange	   *rangePtr;
    SmiValue	   *valuePtr;
    List	   *listPtr;
    Revision	   *revisionPtr;
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

%token <rc>moduleKeyword
%token <rc>importKeyword
%token <rc>revisionKeyword
%token <rc>oidKeyword
%token <rc>dateKeyword
%token <rc>organizationKeyword
%token <rc>contactKeyword
%token <rc>descriptionKeyword
%token <rc>referenceKeyword
%token <rc>typedefKeyword
%token <rc>typeKeyword
%token <rc>writetypeKeyword
%token <rc>nodeKeyword
%token <rc>scalarKeyword
%token <rc>tableKeyword
%token <rc>columnKeyword
%token <rc>rowKeyword
%token <rc>notificationKeyword
%token <rc>groupKeyword
%token <rc>complianceKeyword
%token <rc>formatKeyword
%token <rc>unitsKeyword
%token <rc>statusKeyword
%token <rc>accessKeyword
%token <rc>defaultKeyword
%token <rc>impliedKeyword
%token <rc>indexKeyword
%token <rc>augmentsKeyword
%token <rc>reordersKeyword
%token <rc>sparseKeyword
%token <rc>expandsKeyword
%token <rc>createKeyword
%token <rc>objectsKeyword
%token <rc>notificationsKeyword
%token <rc>mandatoryKeyword
%token <rc>optionalKeyword
%token <rc>refineKeyword
%token <rc>OctetStringKeyword
%token <rc>ObjectIdentifierKeyword
%token <rc>Integer32Keyword
%token <rc>Unsigned32Keyword
%token <rc>Integer64Keyword
%token <rc>Unsigned64Keyword
%token <rc>Float32Keyword
%token <rc>Float64Keyword
%token <rc>Float128Keyword
%token <rc>BitsKeyword
%token <rc>EnumerationKeyword
%token <rc>currentKeyword
%token <rc>deprecatedKeyword
%token <rc>obsoleteKeyword
%token <rc>noaccessKeyword
%token <rc>notifyonlyKeyword
%token <rc>readonlyKeyword
%token <rc>readwriteKeyword


/*
 * Types of non-terminal symbols.
 */

%type <rc>smingFile
%type <rc>moduleStatement_optsep_0n
%type <rc>moduleStatement_optsep_1n
%type <rc>moduleStatement_optsep
%type <modulePtr>moduleStatement
%type <rc>typedefStatement_stmtsep_0n
%type <rc>typedefStatement_stmtsep_1n
%type <rc>typedefStatement_stmtsep
%type <typePtr>typedefStatement
%type <rc>anyObjectStatement_stmtsep_0n
%type <rc>anyObjectStatement_stmtsep_1n
%type <rc>anyObjectStatement_stmtsep
%type <objectPtr>anyObjectStatement
%type <objectPtr>nodeStatement
%type <objectPtr>scalarStatement
%type <objectPtr>tableStatement
%type <objectPtr>rowStatement
%type <rc>columnStatement_stmtsep_1n
%type <rc>columnStatement_stmtsep
%type <objectPtr>columnStatement
%type <rc>notificationStatement_stmtsep_0n
%type <rc>notificationStatement_stmtsep_1n
%type <rc>notificationStatement_stmtsep
%type <objectPtr>notificationStatement
%type <rc>groupStatement_stmtsep_0n
%type <rc>groupStatement_stmtsep_1n
%type <rc>groupStatement_stmtsep
%type <objectPtr>groupStatement
%type <rc>complianceStatement_stmtsep_0n
%type <rc>complianceStatement_stmtsep_1n
%type <rc>complianceStatement_stmtsep
%type <objectPtr>complianceStatement
%type <rc>importStatement_stmtsep_0n
%type <rc>importStatement_stmtsep_1n
%type <rc>importStatement_stmtsep
%type <listPtr>importStatement
%type <rc>revisionStatement_stmtsep_0n
%type <rc>revisionStatement_stmtsep_1n
%type <rc>revisionStatement_stmtsep
%type <revisionPtr>revisionStatement
%type <typePtr>typedefTypeStatement
%type <typePtr>typeStatement_stmtsep_01
%type <typePtr>typeStatement
%type <typePtr>writetypeStatement_stmtsep_01
%type <typePtr>writetypeStatement
%type <indexPtr>anyIndexStatement
%type <indexPtr>indexStatement
%type <indexPtr>augmentsStatement
%type <indexPtr>reordersStatement
%type <indexPtr>sparseStatement
%type <indexPtr>expandsStatement
%type <rc>sep_impliedKeyword_01
%type <listPtr>createStatement_stmtsep_01
%type <listPtr>createStatement
%type <listPtr>optsep_createColumns_01
%type <listPtr>createColumns
%type <nodePtr>oidStatement
%type <text>dateStatement
%type <text>organizationStatement
%type <text>contactStatement
%type <text>formatStatement_stmtsep_01
%type <text>formatStatement
%type <text>unitsStatement_stmtsep_01
%type <text>unitsStatement
%type <status>statusStatement_stmtsep_01
%type <status>statusStatement
%type <access>accessStatement_stmtsep_01
%type <access>accessStatement
%type <valuePtr>defaultStatement_stmtsep_01
%type <valuePtr>defaultStatement
%type <text>descriptionStatement_stmtsep_01
%type <text>descriptionStatement
%type <text>referenceStatement_stmtsep_01
%type <text>referenceStatement
%type <listPtr>objectsStatement_notificationsStatement
%type <listPtr>objectsStatement_stmtsep_01
%type <listPtr>objectsStatement
%type <listPtr>notificationsStatement
%type <listPtr>mandatoryStatement_stmtsep_01
%type <listPtr>mandatoryStatement
%type <listPtr>optionalStatement_stmtsep_0n
%type <listPtr>optionalStatement_stmtsep_1n
%type <optionPtr>optionalStatement_stmtsep
%type <optionPtr>optionalStatement
%type <listPtr>refineStatement_stmtsep_0n
%type <listPtr>refineStatement_stmtsep_1n
%type <refinementPtr>refineStatement_stmtsep
%type <refinementPtr>refineStatement
%type <typePtr>refinedBaseType_refinedType
%type <typePtr>refinedBaseType
%type <typePtr>refinedType
%type <listPtr>optsep_anySpec_01
%type <listPtr>anySpec
%type <listPtr>optsep_numberSpec_01
%type <listPtr>numberSpec
%type <listPtr>furtherNumberElement_0n
%type <listPtr>furtherNumberElement_1n
%type <rangePtr>furtherNumberElement
%type <rangePtr>numberElement
%type <valuePtr>numberUpperLimit_01
%type <valuePtr>numberUpperLimit
%type <listPtr>optsep_floatSpec_01
%type <listPtr>floatSpec
%type <listPtr>furtherFloatElement_0n
%type <listPtr>furtherFloatElement_1n
%type <rangePtr>furtherFloatElement
%type <rangePtr>floatElement
%type <text>floatUpperLimit_01
%type <text>floatUpperLimit
%type <listPtr>bitsOrEnumerationSpec
%type <listPtr>bitsOrEnumerationList
%type <listPtr>furtherBitsOrEnumerationItem_0n
%type <listPtr>furtherBitsOrEnumerationItem_1n
%type <namedNumberPtr>furtherBitsOrEnumerationItem
%type <namedNumberPtr>bitsOrEnumerationItem
%type <listPtr>identifierList
%type <listPtr>furtherIdentifier_0n
%type <listPtr>furtherIdentifier_1n
%type <text>furtherIdentifier
%type <listPtr>qIdentifierList
%type <listPtr>furtherQIdentifier_0n
%type <listPtr>furtherQIdentifier_1n
%type <text>furtherQIdentifier
%type <listPtr>qlcIdentifierList
%type <listPtr>furtherQlcIdentifier_0n
%type <listPtr>furtherQlcIdentifier_1n
%type <text>furtherQlcIdentifier
%type <listPtr>bitsValue
%type <listPtr>bitsList
%type <listPtr>furtherLcIdentifier_0n
%type <listPtr>furtherLcIdentifier_1n
%type <text>furtherLcIdentifier
%type <text>identifier
%type <text>qIdentifier
%type <text>qucIdentifier
%type <text>qlcIdentifier
%type <text>text
%type <text>optsep_textSegment_0n
%type <text>optsep_textSegment_1n
%type <text>optsep_textSegment
%type <text>date
%type <text>format
%type <text>units
%type <valuePtr>anyValue
%type <status>status
%type <access>access
%type <nodePtr>objectIdentifier
%type <text>qlcIdentifier_subid
%type <text>dot_subid_0127
%type <text>dot_subid_1n
%type <text>dot_subid
%type <text>subid
%type <valuePtr>number
%type <valuePtr>negativeNumber
%type <valuePtr>signedNumber
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
			    if ($1 != NULL) {
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
			    identifier = util_strdup($6);
			}
			optsep '{' stmtsep
			importStatement_stmtsep_0n
			oidStatement stmtsep
			{
			    if ($12) {
				objectPtr = addObject(identifier,
						      $12->parentPtr,
						      $12->subid,
						      0, thisParserPtr);
			    }
			}
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
			    $$ = thisModulePtr;
			}
	;

typedefStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
        |		typedefStatement_stmtsep_1n
			{
			    /*
			     * Return the number of successfully
			     * parsed typedef statements.
			     */
			    $$ = $1;
			}
	;

typedefStatement_stmtsep_1n: typedefStatement_stmtsep
			{
			    $$ = $1;
			}
        |		typedefStatement_stmtsep_1n
			typedefStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * typedefs or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

typedefStatement_stmtsep: typedefStatement stmtsep
			{
			    /*
			     * If we got a (Type *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
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
			{
			    /*
			     * Return the number of successfully
			     * parsed object declaring statements.
			     */
			    $$ = $1;
			}
	;

anyObjectStatement_stmtsep_1n: anyObjectStatement_stmtsep
			{
			    $$ = $1;
			}
        |		anyObjectStatement_stmtsep_1n
			anyObjectStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * statements or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

anyObjectStatement_stmtsep: anyObjectStatement stmtsep
			{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
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
			{
			    $$ = NULL;
			}
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
			{
			    $$ = NULL;
			}
        ;

tableStatement:		tableKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			rowStatement stmtsep
			'}' optsep ';'
			{
			    $$ = NULL;
			}
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
			{
			    $$ = NULL;
			}
        ;

columnStatement_stmtsep_1n: columnStatement_stmtsep
			{
			    /*
			     * Return the number of successfully
			     * parsed column statements.
			     */
			    $$ = $1;
			}
        |		columnStatement_stmtsep_1n columnStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * columns or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
	;

columnStatement_stmtsep: columnStatement stmtsep
			{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
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
			{
			    $$ = NULL;
			}
        ;

notificationStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
        |		notificationStatement_stmtsep_1n
			{
			    /*
			     * Return the number of successfully
			     * parsed notification statements.
			     */
			    $$ = $1;
			}
	;

notificationStatement_stmtsep_1n: notificationStatement_stmtsep
			{
			    $$ = $1;
			}
        |		notificationStatement_stmtsep_1n
			notificationStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * notifications or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

notificationStatement_stmtsep: notificationStatement stmtsep
			{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
        ;

notificationStatement:	notificationKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			objectsStatement_stmtsep_01
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			'}' optsep ';'
			{
			    $$ = NULL;
			}
        ;

groupStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
        |               groupStatement_stmtsep_1n
			{
			    /*
			     * Return the number of successfully
			     * parsed group statements.
			     */
			    $$ = $1;
			}
        ;

groupStatement_stmtsep_1n: groupStatement_stmtsep
			{
			    $$ = $1;
			}
        |               groupStatement_stmtsep_1n groupStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * groups or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

groupStatement_stmtsep: groupStatement stmtsep
			{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
        ;

groupStatement:		groupKeyword sep lcIdentifier optsep '{' stmtsep
			oidStatement stmtsep
			objectsStatement_notificationsStatement stmtsep
			statusStatement_stmtsep_01
			descriptionStatement stmtsep
			referenceStatement_stmtsep_01
			'}' optsep ';'
			{
			    $$ = NULL;
			}
        ;

complianceStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
        |               complianceStatement_stmtsep_1n
			{
			    /*
			     * Return the number of successfully
			     * parsed compliance statements.
			     */
			    $$ = $1;
			}
	;

complianceStatement_stmtsep_1n: complianceStatement_stmtsep
			{
			    $$ = $1;
			}
        |               complianceStatement_stmtsep_1n
			complianceStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * compliances or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

complianceStatement_stmtsep: complianceStatement stmtsep
			{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
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
			{
			    $$ = NULL;
			}
        ;

importStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
	|		importStatement_stmtsep_1n
			{
			    $$ = $1;
			}
        ;

importStatement_stmtsep_1n: importStatement_stmtsep
			{
			    $$ = $1;
			}
        |		importStatement_stmtsep_1n importStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * imports or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

importStatement_stmtsep: importStatement stmtsep
			{
			    /*
			     * If we got an (Object *) return rc == 1,
			     * otherwise parsing failed (rc == -1).
			     */
			    if ($1) {
				/* TODO: append list $1 to import list. */
				/* TODO: return count instead of 1. */
				$$ = 1;
			    } else {
				$$ = -1;
			    }
			}
        ;

importStatement:	importKeyword sep ucIdentifier optsep '(' optsep
			identifierList optsep ')' optsep ';'
			{
			    $$ = NULL;
			}
        ;

revisionStatement_stmtsep_0n: /* empty */
			{
			    $$ = 0;
			}
        |		revisionStatement_stmtsep_1n
			{
			    $$ = $1;
			}
        ;

revisionStatement_stmtsep_1n: revisionStatement_stmtsep
			{
			    $$ = $1;
			}
        |		revisionStatement_stmtsep_1n revisionStatement_stmtsep
			{
			    /*
			     * Sum up the number of successfully parsed
			     * revisions or return -1, if at least one
			     * module failed.
			     */
			    if (($1 >= 0) && ($2 >= 0)) {
				$$ = $1 + $2;
			    } else {
				$$ = -1;
			    }
			}
        ;

revisionStatement_stmtsep: revisionStatement stmtsep
			   {
			       /*
				* If we got a (Revision *) return rc == 1,
				* otherwise parsing failed (rc == -1).
				*/
			       if ($1) {
				$$ = 1;
			       } else {
				   $$ = -1;
			       }
			   }
        ;

revisionStatement:	revisionKeyword optsep '{' stmtsep
			dateStatement stmtsep
			descriptionStatement stmtsep
			'}' optsep ';'
			{
			    /* TODO: addRevision() */
			    $$ = NULL;
			}
        ;

typedefTypeStatement:	typeKeyword sep refinedBaseType optsep ';'
			{
			    $$ = NULL;
			}
	;

typeStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |		typeStatement stmtsep
			{
			    $$ = NULL;
			}
	;

typeStatement:		typeKeyword sep refinedBaseType_refinedType
			optsep ';'
			{
			    $$ = NULL;
			}
        ;

writetypeStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |	        writetypeStatement stmtsep
			{
			    $$ = NULL;
			}
	;

writetypeStatement:	writetypeKeyword sep refinedBaseType_refinedType
			optsep ';'
			{
			    $$ = NULL;
			}
        ;

anyIndexStatement:	indexStatement
        |		augmentsStatement
	|		reordersStatement
	|		sparseStatement
	|		expandsStatement
	;

indexStatement:		indexKeyword sep_impliedKeyword_01 optsep
			'(' optsep qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = NULL;
			}
        ;

augmentsStatement:	augmentsKeyword sep qlcIdentifier optsep ';'
			{
			    $$ = NULL;
			}
        ;

reordersStatement:	reordersKeyword sep qlcIdentifier
			sep_impliedKeyword_01 optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = NULL;
			}
        ;

sparseStatement:	sparseKeyword sep qlcIdentifier optsep ';'
			{
			    $$ = NULL;
			}
	;

expandsStatement:	expandsKeyword sep qlcIdentifier
			sep_impliedKeyword_01 optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = NULL;
			}
        ;

sep_impliedKeyword_01:	/* empty */
			{
			    $$ = 0;
			}
	|		sep impliedKeyword
			{
			    $$ = 1;
			}
	;

createStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |               createStatement stmtsep
			{
			    $$ = $1;
			}
	;

createStatement:	createKeyword optsep_createColumns_01 optsep ';'
			{
			    $$ = $2;
			}
        ;

optsep_createColumns_01: /* empty */
			{
			    $$ = NULL;
			}
        |		optsep createColumns
			{
			    $$ = $2;
			}
        ;

createColumns:		'(' optsep qlcIdentifierList optsep ')'
			{
			    $$ = $3;
			}
        ;

oidStatement:		oidKeyword sep objectIdentifier optsep ';'
			{
			    Node *nodePtr = NULL;
			    
			    $$ = nodePtr;
			}
        ;

dateStatement:		dateKeyword sep date optsep ';'
			{
			    $$ = $3;
			}
        ;

organizationStatement:	organizationKeyword sep text optsep ';'
			{
			    $$ = $3;
			}
        ;

contactStatement:	contactKeyword sep text optsep ';'
			{
			    $$ = $3;
			}
        ;

formatStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
	|		formatStatement stmtsep
			{
			    $$ = $1;
			}
	;

formatStatement:	formatKeyword sep text optsep ';'
			{
			    $$ = $3;
			}
        ;

unitsStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |		unitsStatement stmtsep
			{
			    $$ = $1;
			}
	;

unitsStatement:		unitsKeyword sep text optsep ';'
			{
			    $$ = $3;
			}
        ;

statusStatement_stmtsep_01: /* empty */
			{
			    $$ = SMI_STATUS_UNKNOWN;
			}
        |               statusStatement stmtsep
			{
			    $$ = $1;
			}
	;

statusStatement:	statusKeyword sep status optsep ';'
			{
			    $$ = $3;
			}
        ;

accessStatement_stmtsep_01: /* empty */
			{
			    $$ = SMI_ACCESS_UNKNOWN;
			}
        |		accessStatement stmtsep
			{
			    $$ = $1;
			}
	;

accessStatement:	accessKeyword sep access optsep ';'
			{
			    $$ = $3;
			}
        ;

defaultStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |		defaultStatement stmtsep
			{
			    $$ = $1;
			}
	;

defaultStatement:	defaultKeyword sep anyValue optsep ';'
			{
			    $$ = $3;
			}
        ;

descriptionStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |               descriptionStatement stmtsep
			{
			    $$ = $1;
			}
	;

descriptionStatement:	descriptionKeyword sep text optsep ';'
			{
			    $$ = $3;
			}
        ;

referenceStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |		referenceStatement stmtsep
			{
			    $$ = $1;
			}
	;

referenceStatement:	referenceKeyword sep text optsep ';'
			{
			    $$ = $3;
			}
        ;

objectsStatement_notificationsStatement: objectsStatement
			{
			    $$ = $1;
			}
        |		notificationsStatement
			{
			    $$ = $1;
			}
	;

objectsStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |		objectsStatement stmtsep
			{
			    $$ = $1;
			}
	;

objectsStatement:	objectsKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = $5;
			}
        ;

notificationsStatement:	notificationsKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = $5;
			}
        ;

mandatoryStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |               mandatoryStatement stmtsep
			{
			    $$ = $1;
			}
	;

mandatoryStatement:	mandatoryKeyword optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = $5;
			}
        ;

optionalStatement_stmtsep_0n: /* empty */
			{
			    $$ = NULL;
			}
        |               optionalStatement_stmtsep_1n
			{
			    $$ = $1;
			}
	;

optionalStatement_stmtsep_1n: optionalStatement_stmtsep
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
        |		optionalStatement_stmtsep_1n
			optionalStatement_stmtsep
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

optionalStatement_stmtsep: optionalStatement stmtsep
			   {
			       $$ = $1;
			   }
        ;

optionalStatement:	optionalKeyword sep qlcIdentifier optsep '{'
			descriptionStatement stmtsep '}' optsep ';'
			{
			    $$ = NULL;
			}
        ;

refineStatement_stmtsep_0n: /* empty */
			{
			    $$ = NULL;
			}
        |               refineStatement_stmtsep_1n
			{
			    $$ = $1;
			}
	;

refineStatement_stmtsep_1n: refineStatement_stmtsep
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
        |		refineStatement_stmtsep_1n refineStatement_stmtsep
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

refineStatement_stmtsep: refineStatement stmtsep
			{
			    $$ = $1;
			}
        ;

refineStatement:	refineKeyword sep qlcIdentifier optsep '{'
			typeStatement_stmtsep_01
			writetypeStatement_stmtsep_01
			accessStatement_stmtsep_01
			descriptionStatement stmtsep '}' optsep ';'
			{
			    $$ = NULL;
			}
        ;

refinedBaseType_refinedType: refinedBaseType
			{
			    $$ = $1;
			}
        |		refinedType
			{
			    $$ = $1;
			}
	;

refinedBaseType:	OctetStringKeyword optsep_numberSpec_01
			{
			    $$ = NULL;
			}
        |		ObjectIdentifierKeyword
			{
			    $$ = NULL;
			}
	|		Integer32Keyword optsep_numberSpec_01
			{
			    $$ = NULL;
			}
	|		Unsigned32Keyword optsep_numberSpec_01
			{
			    $$ = NULL;
			}
	|		Integer64Keyword optsep_numberSpec_01
			{
			    $$ = NULL;
			}
	|		Unsigned64Keyword optsep_numberSpec_01
			{
			    $$ = NULL;
			}
	|		Float32Keyword optsep_floatSpec_01
			{
			    $$ = NULL;
			}
	|		Float64Keyword optsep_floatSpec_01
			{
			    $$ = NULL;
			}
	|		Float128Keyword optsep_floatSpec_01
			{
			    $$ = NULL;
			}
	|		EnumerationKeyword bitsOrEnumerationSpec
			{
			    $$ = NULL;
			}
	|		BitsKeyword bitsOrEnumerationSpec
			{
			    $$ = NULL;
			}
	;

refinedType:		qucIdentifier optsep_anySpec_01
			{
			    $$ = NULL;
			}
	;

optsep_anySpec_01:	/* empty */
			{
			    $$ = NULL;
			}
	|		optsep anySpec
			{
			    $$ = $2;
			}
	;

anySpec:		numberSpec
			{
			    $$ = $1;
			}
	|		floatSpec
			{
			    $$ = $1;
			}
	;

optsep_numberSpec_01:	/* empty */
			{
			    $$ = NULL;
			}
	|		optsep numberSpec
			{
			    $$ = $2;
			}
	;

numberSpec:		'(' optsep numberElement furtherNumberElement_0n
			optsep ')'
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$3;
			    $$->nextPtr = $4;
			}
        ;

furtherNumberElement_0n:	/* empty */
			{
			    $$ = NULL;
			}
        |		furtherNumberElement_1n
			{
			    $$ = $1;
			}
        ;

furtherNumberElement_1n:	furtherNumberElement
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherNumberElement_1n furtherNumberElement
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
        ;

furtherNumberElement:	optsep '|' optsep numberElement
			{
			    $$ = $4;
			}
	;

numberElement:		number numberUpperLimit_01
			{
			    $$ = util_malloc(sizeof(SmiRange));
			    $$->minValuePtr = $1;
			    if ($2) {
				$$->maxValuePtr = $2;
			    } else {
				$$->maxValuePtr = $1;
			    }
			}
	;

numberUpperLimit_01:	/* empty */
			{
			    $$ = NULL;
			}
        |		numberUpperLimit
			{
			    $$ = $1;
			}
	;

numberUpperLimit:	optsep DOT_DOT optsep number
			{
			    $$ = $4;
			}
        ;

optsep_floatSpec_01:	/* empty */
			{
			    $$ = NULL;
			}
        |		optsep floatSpec
			{
			    $$ = $2;
			}
	;

floatSpec:		'(' optsep floatElement furtherFloatElement_0n
			optsep ')'
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$3;
			    $$->nextPtr = $4;
			}
        ;

furtherFloatElement_0n:	/* empty */
			{
			    $$ = NULL;
			}
        |		furtherFloatElement_1n
			{
			    $$ = $1;
			}
        ;

furtherFloatElement_1n:	furtherFloatElement
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherFloatElement_1n furtherFloatElement
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
        ;

furtherFloatElement:	optsep '|' optsep floatElement
			{
			    $$ = $4;
			}
	;

floatElement:		floatValue floatUpperLimit_01
			{
			    $$ = util_malloc(sizeof(SmiRange));
			    $$->minValuePtr = util_malloc(sizeof(SmiValue));
			    $$->minValuePtr->basetype = SMI_BASETYPE_FLOAT64;
			    $$->minValuePtr->value.float64 = strtod($1, NULL);
			    if ($2) {
				$$->maxValuePtr =
				                 util_malloc(sizeof(SmiValue));
				$$->maxValuePtr->basetype =
				                          SMI_BASETYPE_FLOAT64;
				$$->maxValuePtr->value.float64 =
				                              strtod($2, NULL);
			    } else {
				$$->maxValuePtr = $$->minValuePtr;
			    }
			}
	;

floatUpperLimit_01:	/* empty */
			{
			    $$ = NULL;
			}
        |		floatUpperLimit
			{
			    $$ = $1;
			}
	;

floatUpperLimit:	optsep DOT_DOT optsep floatValue
			{
			    $$ = $4;
			}
        ;

bitsOrEnumerationSpec:	'(' optsep bitsOrEnumerationList optsep ')'
			{
			    $$ = $3;
			}
        ;

bitsOrEnumerationList:	bitsOrEnumerationItem furtherBitsOrEnumerationItem_0n
			optsep_comma_01
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = $2;
			}
        ;

furtherBitsOrEnumerationItem_0n: /* empty */
			{
			    $$ = NULL;
			}
        |		furtherBitsOrEnumerationItem_1n
			{
			    $$ = $1;
			}
        ;

furtherBitsOrEnumerationItem_1n: furtherBitsOrEnumerationItem
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherBitsOrEnumerationItem_1n
			furtherBitsOrEnumerationItem
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
        ;

	
furtherBitsOrEnumerationItem: optsep ',' optsep bitsOrEnumerationItem
			{
			    $$ = $4;
			}
        ;

bitsOrEnumerationItem:	lcIdentifier optsep '(' optsep number optsep ')'
			{
			    $$ = util_malloc(sizeof(SmiNamedNumber));
			    $$->name = util_strdup($1);
			    $$->valuePtr = $5;
			}
        ;

identifierList:		identifier furtherIdentifier_0n optsep_comma_01
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = $2;
			}
        ;

furtherIdentifier_0n:	/* empty */
			{
			    $$ = NULL;
			}
        |		furtherIdentifier_1n
			{
			    $$ = $1;
			}
        ;

furtherIdentifier_1n:	furtherIdentifier
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherIdentifier_1n furtherIdentifier
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
        ;

furtherIdentifier:	optsep ',' optsep identifier
			{
			    $$ = $4;
			}
        ;

qIdentifierList:	qIdentifier furtherQIdentifier_0n optsep_comma_01
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = $2;
			}
        ;

furtherQIdentifier_0n:	/* empty */
			{
			    $$ = NULL;
			}
        |		furtherQIdentifier_1n
			{
			    $$ = $1;
			}
        ;

furtherQIdentifier_1n:	furtherQIdentifier
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherQIdentifier_1n furtherQIdentifier
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
        ;

furtherQIdentifier:	optsep ',' optsep qIdentifier
			{
			    $$ = $4;
			}
        ;

qlcIdentifierList:	qlcIdentifier furtherQlcIdentifier_0n optsep_comma_01
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = $2;
			}
        ;

furtherQlcIdentifier_0n: /* empty */
			{
			    $$ = NULL;
			}
        |		furtherQlcIdentifier_1n
			{
			    $$ = $1;
			}
        ;

furtherQlcIdentifier_1n:	furtherQlcIdentifier
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherQlcIdentifier_1n furtherQlcIdentifier
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
        ;

furtherQlcIdentifier:	optsep ',' optsep qlcIdentifier
			{
			    $$ = $4;
			}
        ;

bitsValue:		'(' optsep bitsList optsep ')'
			{
			    $$ = $3;
			}
        ;

bitsList:		optsep_comma_01
			{
			    $$ = NULL;
			}
        |		lcIdentifier furtherLcIdentifier_0n optsep_comma_01
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = util_strdup($1);
			    $$->nextPtr = $2;
			}
	;

furtherLcIdentifier_0n:	/* empty */
			{
			    $$ = NULL;
			}
        |		furtherLcIdentifier_1n
			{
			    $$ = $1;
			}
	;

furtherLcIdentifier_1n:	furtherLcIdentifier
			{
			    $$ = util_malloc(sizeof(List));
			    $$->ptr = (void *)$1;
			    $$->nextPtr = NULL;
			}
        |		furtherLcIdentifier_1n furtherLcIdentifier
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$2;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

furtherLcIdentifier:	optsep ',' optsep lcIdentifier
			{
			    $$ = util_strdup($4);
			}
        ;

identifier:		ucIdentifier
			{
			    $$ = util_strdup($1);
			}
        |		lcIdentifier
			{
			    $$ = util_strdup($1);
			}
	;

qIdentifier:		qucIdentifier
			{
			    $$ = $1;
			}
        |		qlcIdentifier
			{
			    $$ = $1;
			}
	;

qucIdentifier:		ucIdentifier COLON_COLON ucIdentifier
			{
			    char *s;

			    s = util_malloc(strlen($1) +
					    strlen(SMI_NAMESPACE_OPERATOR) +
					    strlen($3) + 1);
			    sprintf(s, "%s%s%s",
				    $1, SMI_NAMESPACE_OPERATOR, $3);
			    $$ = s;
			}
        |		ucIdentifier
			{
			    $$ = util_strdup($1);
			}
	;

qlcIdentifier:		ucIdentifier COLON_COLON lcIdentifier
			{
			    char *s;

			    s = util_malloc(strlen($1) +
					    strlen(SMI_NAMESPACE_OPERATOR) +
					    strlen($3) + 1);
			    sprintf(s, "%s%s%s",
				    $1, SMI_NAMESPACE_OPERATOR, $3);
			    $$ = s;
			}
        |		lcIdentifier
			{
			    $$ = util_strdup($1);
			}
	;

text:			textSegment optsep_textSegment_0n
			{
			    if ($2) {
				$$ = util_malloc(strlen($1) + strlen($2) + 1);
				strcpy($$, $1);
				strcat($$, $2);
				free($1);
				free($2);
			    } else {
				$$ = $1;
			    }
			}
        ;

optsep_textSegment_0n:	/* empty */
			{
			    $$ = NULL;
			}
        |		optsep_textSegment_1n
			{
			    $$ = $1;
			}
	;

optsep_textSegment_1n:	optsep_textSegment
			{
			    $$ = $1;
			}
        |		optsep_textSegment_1n optsep_textSegment
			{
			    $$ = util_malloc(strlen($1) + strlen($2) + 1);
			    strcpy($$, $1);
			    strcat($$, $2);
			    free($1);
			    free($2);
			}
	;

optsep_textSegment:	optsep textSegment
			{
			    $$ = util_strdup($2);
			}
        ;

date:			textSegment
			/* TODO: check "YYYY-MM-DD HH:MM" or "YYYY-MM-DD" */
			{
			    $$ = util_strdup($1);
			}
        ;

format:			textSegment
			{
			    $$ = util_strdup($1);
			}
        ;

units:			textSegment
			{
			    $$ = util_strdup($1);
			}
        ;

anyValue:		bitsValue
			{
			    $$ = NULL;
			}
	|		negativeNumber
			{
			    $$ = NULL;
			}
	|		hexadecimalNumber
			{
			    $$ = NULL;
			}
	|		floatValue
			{
			    $$ = NULL;
			}
	|		text
			{
			    $$ = NULL;
			}
	|		objectIdentifier
			/* also label in case of enum */
			/* also number */
			{
			    $$ = NULL;
			}
	;

status:			currentKeyword
			{
			    $$ = SMI_STATUS_CURRENT;
			}
        |		deprecatedKeyword
			{
			    $$ = SMI_STATUS_DEPRECATED;
			}
	|		obsoleteKeyword
			{
			    $$ = SMI_STATUS_OBSOLETE;
			}
	;

access:			noaccessKeyword
			{
			    $$ = SMI_ACCESS_NOT_ACCESSIBLE;
			}
        |		notifyonlyKeyword
			{
			    $$ = SMI_ACCESS_NOTIFY;
			}
	|		readonlyKeyword
			{
			    $$ = SMI_ACCESS_READ_ONLY;
			}
	|		readwriteKeyword
			{
			    $$ = SMI_ACCESS_READ_WRITE;
			}
	;

objectIdentifier:	qlcIdentifier_subid dot_subid_0127
			{
			    $$ = (Node *)NULL;
			}
	;

qlcIdentifier_subid:	qlcIdentifier
			{
			    SmiNode *smiNodePtr;
			    
			    if (strstr($1, SMI_NAMESPACE_OPERATOR)) {
				smiNodePtr = smiGetNode($1, NULL);
			    } else {
				smiNodePtr = smiGetNode($1,
							thisModulePtr->name);
			    }
			    if (smiNodePtr) {
				$$ = util_strdup(smiNodePtr->oid);
				smiFreeNode(smiNodePtr);
			    } else {
				$$ = NULL;
			    }
			}
        |		subid
			{
			    $$ = NULL;
			}
	;

dot_subid_0127:		/* empty */
			{
			    $$ = NULL;
			}
        |		dot_subid_1n
			{
			    /* TODO: check upper limit of 127 subids */ 
			    $$ = $1;
			}
	;

dot_subid_1n:		dot_subid
			{
			    $$ = $1;
			}
        |		dot_subid_1n dot_subid
			{
			    $$ = util_malloc(strlen($1) + strlen($2) + 1);
			    strcpy($$, $1);
			    strcat($$, $2);
			    free($1);
			    free($2);
			}
	;

dot_subid:		'.' subid
			{
			    $$ = util_malloc(strlen($2) + 1 + 1);
			    strcpy($$, ".");
			    strcat($$, $2);
			    free($2);
			}
        ;

subid:			decimalNumber
			{
			    $$ = util_strdup($1);
			}
        ;

number:			hexadecimalNumber
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO */
			    $$->basetype = SMI_BASETYPE_UNSIGNED64;
			    $$->value.unsigned64 = 42;
			}
        |		decimalNumber
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_UNSIGNED64;
			    $$->value.unsigned64 = strtoull($1, NULL, 10);
			}
	;

negativeNumber:		'-' decimalNumber
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_INTEGER64;
			    $$->value.integer64 = - strtoull($2, NULL, 10);
			}
        ;

signedNumber:		decimalNumber
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_UNSIGNED64;
			    $$->value.unsigned64 = strtoull($1, NULL, 10);
			}
        |		negativeNumber
			{
			    $$ = $1;
			}
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
			
