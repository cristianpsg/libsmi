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
 * @(#) $Id: parser-sming.y,v 1.6 1999/04/07 18:21:33 strauss Exp $
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



static char *typeIdentifier, *nodeIdentifier,
	    *scalarIdentifier, *tableIdentifier, *rowIdentifier,
	    *columnIdentifier, *notificationIdentifier,
	    *groupIdentifier, *complianceIdentifier;
static char *importModulename = NULL;
static Object *moduleObjectPtr = NULL;
static Object *nodeObjectPtr = NULL;
static Object *scalarObjectPtr = NULL;
static Object *tableObjectPtr = NULL;
static Object *rowObjectPtr = NULL;
static Object *columnObjectPtr = NULL;
static Object *notificationObjectPtr = NULL;
static Object *groupObjectPtr = NULL;
static Object *complianceObjectPtr = NULL;
static Type *typePtr = NULL;
static SmiBasetype defaultBasetype = SMI_BASETYPE_UNKNOWN;

 

Type *
findType(spec, parserPtr)
    char *spec;
    Parser *parserPtr; 
{
    Type *typePtr;
    Import *importPtr;
    
    if (!strstr(spec, SMI_NAMESPACE_OPERATOR)) {
	typePtr = findTypeByModuleAndName(thisModulePtr, spec);
	if (!typePtr) {
	    importPtr = findImportByName(spec, thisModulePtr);
	    if (importPtr) {
		typePtr = findTypeByModulenameAndName(importPtr->module, spec);
	    }
	}
    } else {
	typePtr = findTypeByModulenameAndName(smiModule(spec),
					      smiDescriptor(spec));
    }
    return typePtr;
}
 

			    
Object *
findObject(spec, parserPtr)
    char *spec;
    Parser *parserPtr; 
{
    Object *objectPtr;
    Import *importPtr;
    
    if (!strstr(spec, SMI_NAMESPACE_OPERATOR)) {
	objectPtr = findObjectByModuleAndName(thisModulePtr, spec);
	if (!objectPtr) {
	    importPtr = findImportByName(spec, thisModulePtr);
	    if (importPtr) {
		objectPtr = findObjectByModulenameAndName(importPtr->module,
							  spec);
	    }
	}
    } else {
	objectPtr = findObjectByModulenameAndName(smiModule(spec),
						  smiDescriptor(spec));
    }
    return objectPtr;
}
 

			    
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
			    free($3);
			}
			sep lcIdentifier
			{
			    moduleObjectPtr = addObject($6,
							pendingNodePtr, 0,
							FLAG_INCOMPLETE,
							thisParserPtr);
			    setObjectDecl(moduleObjectPtr,
					  SMI_DECL_MODULE);
			    setModuleIdentityObject(
				thisParserPtr->modulePtr, moduleObjectPtr);
			    free($6);
			}
			optsep '{' stmtsep
			importStatement_stmtsep_0n
			oidStatement stmtsep
			{
			    /*
			     * Note: Due to parsing imported modules
			     * the contents of moduleObjectPtr got
			     * invalid. Therefore, we created the
			     * module identity object above as
			     * incomplete, and now we add it's
			     * registration information.
			     */

			    if ($12) {
				moduleObjectPtr = addObject("",
							    $12->parentPtr,
							    $12->subid,
							    0, thisParserPtr);
				moduleObjectPtr =
				    setObjectName(moduleObjectPtr,
					       thisModulePtr->objectPtr->name);
				setObjectDecl(moduleObjectPtr,
					      SMI_DECL_MODULE);
				deleteObjectFlags(moduleObjectPtr,
					       FLAG_INCOMPLETE);
				addObjectFlags(moduleObjectPtr,
					       FLAG_REGISTERED);
				setModuleIdentityObject(
				    thisParserPtr->modulePtr, moduleObjectPtr);
			    }
			}
			organizationStatement stmtsep
			{
			    if ($15) {
				setModuleOrganization(thisParserPtr->modulePtr,
						      $15);
			    }
			}
			contactStatement stmtsep
			{
			    if ($18) {
				setModuleContactInfo(thisParserPtr->modulePtr,
						     $18);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (moduleObjectPtr && $21) {
				setObjectDescription(moduleObjectPtr, $21);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (moduleObjectPtr && $24) {
				setObjectReference(moduleObjectPtr, $24);
			    }
			}
			revisionStatement_stmtsep_0n
			typedefStatement_stmtsep_0n
			anyObjectStatement_stmtsep_0n
			notificationStatement_stmtsep_0n
			groupStatement_stmtsep_0n
			complianceStatement_stmtsep_0n
			'}' optsep ';'
			{
			    /*
			     * TODO: walk through rows of this modules with
			     * flag & FLAG_INDEXLABELS and convert their
			     * labelstring lists to (Object *) lists.
			     * Also: create lists.
			     */
			    $$ = thisModulePtr;
			    moduleObjectPtr = NULL;
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

typedefStatement:	typedefKeyword sep ucIdentifier
			{
			    typeIdentifier = $3;
			}
			optsep '{' stmtsep
			typedefTypeStatement stmtsep
			{
			    if ($8) {
				if ($8->name) {
				    /*
				     * If the exact type has been found
				     * and no new Type structure has been
				     * created, we have to create a
				     * duplicate now.
				     */
				    typePtr = duplicateType($8, 0,
							    thisParserPtr);
				} else {
				    typePtr = $8;
				}
				setTypeName(typePtr, typeIdentifier);
				setTypeDecl(typePtr, SMI_DECL_TYPEDEF);
			    }
			}
			defaultStatement_stmtsep_01
			{
			    if (typePtr && $11) {
				setTypeValue(typePtr, $11);
			    }
			}
			formatStatement_stmtsep_01
			{
			    if (typePtr && $13) {
				setTypeFormat(typePtr, $13);
			    }
			}
			unitsStatement_stmtsep_01
			{
			    if (typePtr && $15) {
				setTypeUnits(typePtr, $15);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (typePtr && $17) {
				setTypeStatus(typePtr, $17);
			    }
			}
			descriptionStatement_stmtsep_01
			{
			    if (typePtr && $19) {
				setTypeDescription(typePtr, $19);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (typePtr && $21) {
				setTypeReference(typePtr, $21);
			    }
			}
			'}' optsep ';'
			{
			    $$ = 0;
			    typePtr = NULL;
			    free(typeIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
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

nodeStatement:		nodeKeyword sep lcIdentifier
			{
			    nodeIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				nodeObjectPtr = addObject(nodeIdentifier,
							  $8->parentPtr,
							  $8->subid,
							  0, thisParserPtr);
				setObjectDecl(nodeObjectPtr, SMI_DECL_NODE);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (nodeObjectPtr) {
				setObjectStatus(nodeObjectPtr, $11);
			    }
			}
			descriptionStatement_stmtsep_01
			{
			    if (nodeObjectPtr && $13) {
				setObjectDescription(nodeObjectPtr, $13);
				/*
				 * If the node has a description, it gets
				 * registered. This is used to distinguish
				 * between SMIv2 OBJECT-IDENTITY macros and
				 * non-registering ASN.1 value assignments.
				 */
				addObjectFlags(nodeObjectPtr, FLAG_REGISTERED);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (nodeObjectPtr && $15) {
				setObjectReference(nodeObjectPtr, $15);
			    }
			}
			'}' optsep ';'
			{
			    $$ = nodeObjectPtr;
			    nodeObjectPtr = NULL;
			    free(nodeIdentifier);
			}
        ;

scalarStatement:	scalarKeyword sep lcIdentifier
			{
			    scalarIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				scalarObjectPtr = addObject(scalarIdentifier,
							    $8->parentPtr,
							    $8->subid,
							    0, thisParserPtr);
				setObjectDecl(scalarObjectPtr,
					      SMI_DECL_SCALAR);
			    }
			}
			typeStatement stmtsep
			{
			    if (scalarObjectPtr && $11) {
				setObjectType(scalarObjectPtr, $11);
				defaultBasetype = $11->basetype;
				if (!($11->name)) {
				    /*
				     * An inlined type.
				     */
				    setTypeName($11, scalarIdentifier);
				}
			    }
			}
			accessStatement stmtsep
			{
			    if (scalarObjectPtr) {
				setObjectAccess(scalarObjectPtr, $14);
			    }
			}
			defaultStatement_stmtsep_01
			{
			    if (scalarObjectPtr && $17) {
				setObjectValue(scalarObjectPtr, $17);
			    }
			}
			formatStatement_stmtsep_01
			{
			    if (scalarObjectPtr && $19) {
				setObjectFormat(scalarObjectPtr, $19);
			    }
			}
			unitsStatement_stmtsep_01
			{
			    if (scalarObjectPtr && $21) {
				setObjectUnits(scalarObjectPtr, $21);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (scalarObjectPtr) {
				setObjectStatus(scalarObjectPtr, $23);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (scalarObjectPtr && $25) {
				setObjectDescription(scalarObjectPtr, $25);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (scalarObjectPtr && $28) {
				setObjectReference(scalarObjectPtr, $28);
			    }
			}
			'}' optsep ';'
			{
			    $$ = scalarObjectPtr;
			    scalarObjectPtr = NULL;
			    free(scalarIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
			}
        ;

tableStatement:		tableKeyword sep lcIdentifier
			{
			    tableIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				tableObjectPtr = addObject(tableIdentifier,
							   $8->parentPtr,
							   $8->subid,
							   0, thisParserPtr);
				setObjectDecl(tableObjectPtr,
					      SMI_DECL_TABLE);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (tableObjectPtr) {
				setObjectStatus(tableObjectPtr, $11);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (tableObjectPtr && $13) {
				setObjectDescription(tableObjectPtr, $13);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (tableObjectPtr && $16) {
				setObjectReference(tableObjectPtr, $16);
			    }
			}
			rowStatement stmtsep
			'}' optsep ';'
			{
			    $$ = tableObjectPtr;
			    tableObjectPtr = NULL;
			    free(tableIdentifier);
			}
        ;

rowStatement:		rowKeyword sep lcIdentifier
			{
			    rowIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				rowObjectPtr = addObject(rowIdentifier,
							 $8->parentPtr,
							 $8->subid,
							 0, thisParserPtr);
				setObjectDecl(rowObjectPtr,
					      SMI_DECL_ROW);
			    }
			}
			anyIndexStatement stmtsep
			{
			    if (rowObjectPtr && $11) {
				setObjectIndex(rowObjectPtr, $11);
				addObjectFlags(rowObjectPtr, FLAG_INDEXLABELS);
				/* TODO: add this object to a list of rows
				 * that have to be converted when the whole
				 * module has been parsed */
			    }
			}
			createStatement_stmtsep_01
			{
			    ;
			}
			statusStatement_stmtsep_01
			{
			    if (rowObjectPtr) {
				setObjectStatus(rowObjectPtr, $16);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (rowObjectPtr && $18) {
				setObjectDescription(rowObjectPtr, $18);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (rowObjectPtr && $21) {
				setObjectReference(rowObjectPtr, $21);
			    }
			}
			columnStatement_stmtsep_1n
			'}' optsep ';'
			{
			    $$ = rowObjectPtr;
			    rowObjectPtr = NULL;
			    free(rowIdentifier);
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

columnStatement:	columnKeyword sep lcIdentifier
			{
			    columnIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				columnObjectPtr = addObject(columnIdentifier,
							    $8->parentPtr,
							    $8->subid,
							    0, thisParserPtr);
				setObjectDecl(columnObjectPtr,
					      SMI_DECL_COLUMN);
			    }
			}
			typeStatement stmtsep
			{
			    if (columnObjectPtr && $11) {
				setObjectType(columnObjectPtr, $11);
				defaultBasetype = $11->basetype;
				if (!($11->name)) {
				    /*
				     * An inlined type.
				     */
				    setTypeName($11, columnIdentifier);
				}
			    }
			}
			accessStatement stmtsep
			{
			    if (columnObjectPtr) {
				setObjectAccess(columnObjectPtr, $14);
			    }
			}
			defaultStatement_stmtsep_01
			{
			    if (columnObjectPtr && $17) {
				setObjectValue(columnObjectPtr, $17);
			    }
			}
			formatStatement_stmtsep_01
			{
			    if (columnObjectPtr && $19) {
				setObjectFormat(columnObjectPtr, $19);
			    }
			}
			unitsStatement_stmtsep_01
			{
			    if (columnObjectPtr && $21) {
				setObjectUnits(columnObjectPtr, $21);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (columnObjectPtr) {
				setObjectStatus(columnObjectPtr, $23);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (columnObjectPtr && $25) {
				setObjectDescription(columnObjectPtr, $25);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (columnObjectPtr && $28) {
				setObjectReference(columnObjectPtr, $28);
			    }
			}
			'}' optsep ';'
			{
			    $$ = columnObjectPtr;
			    columnObjectPtr = NULL;
			    free(columnIdentifier);
			    defaultBasetype = SMI_BASETYPE_UNKNOWN;
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

notificationStatement:	notificationKeyword sep lcIdentifier
			{
			    notificationIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				notificationObjectPtr =
				    addObject(notificationIdentifier,
					      $8->parentPtr,
					      $8->subid,
					      0, thisParserPtr);
				setObjectDecl(notificationObjectPtr,
					      SMI_DECL_NOTIFICATION);
			    }
			}
			objectsStatement_stmtsep_01
			{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (notificationObjectPtr && $11) {
				for (listPtr = $11; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(notificationObjectPtr, $11);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (notificationObjectPtr) {
				setObjectStatus(notificationObjectPtr, $13);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (notificationObjectPtr && $15) {
				setObjectDescription(notificationObjectPtr,
						     $15);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (notificationObjectPtr && $18) {
				setObjectReference(notificationObjectPtr, $18);
			    }
			}
			'}' optsep ';'
			{
			    $$ = notificationObjectPtr;
			    notificationObjectPtr = NULL;
			    free(notificationIdentifier);
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

groupStatement:		groupKeyword sep lcIdentifier
			{
			    groupIdentifier = $3;
			}
			optsep '{' stmtsep
			oidStatement stmtsep
			{
			    if ($8) {
				groupObjectPtr = addObject(groupIdentifier,
							   $8->parentPtr,
							   $8->subid,
							   0, thisParserPtr);
				setObjectDecl(groupObjectPtr, SMI_DECL_GROUP);
			    }
			}
			objectsStatement_notificationsStatement stmtsep
			{
			    List *listPtr;
			    Object *objectPtr;
			    
			    if (groupObjectPtr && $11) {
				for (listPtr = $11; listPtr;
				     listPtr = listPtr->nextPtr) {
				    objectPtr = findObject(listPtr->ptr,
							   thisParserPtr);
				    listPtr->ptr = objectPtr;
				}
				setObjectList(groupObjectPtr, $11);
			    }
			}
			statusStatement_stmtsep_01
			{
			    if (groupObjectPtr) {
				setObjectStatus(groupObjectPtr, $14);
			    }
			}
			descriptionStatement stmtsep
			{
			    if (groupObjectPtr && $16) {
				setObjectDescription(groupObjectPtr, $16);
			    }
			}
			referenceStatement_stmtsep_01
			{
			    if (groupObjectPtr && $19) {
				setObjectReference(groupObjectPtr, $19);
			    }
			}
			'}' optsep ';'
			{
			    $$ = groupObjectPtr;
			    groupObjectPtr = NULL;
			    free(groupIdentifier);
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

complianceStatement:	complianceKeyword sep lcIdentifier
			optsep '{' stmtsep
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

importStatement:	importKeyword sep ucIdentifier
			{
			    importModulename = $3;
			}
			optsep '(' optsep
			identifierList
			{
			    List *listPtr, *nextPtr;
			    
			    for (listPtr = $8; listPtr; listPtr = nextPtr) {
				addImport(listPtr->ptr, thisParserPtr);
				thisParserPtr->modulePtr->
				                      numImportedIdentifiers++;
				nextPtr = listPtr->nextPtr;
				free(listPtr);
			    }
			}
			optsep ')' optsep ';'
			{
			    if (!findModuleByName(importModulename)) {
				smiLoadModule(importModulename);
			    }
			    checkImports(importModulename, thisParserPtr);
			    free(importModulename);
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
			    $$ = addRevision(smiMkTime($5), $7, thisParserPtr);
			}
        ;

typedefTypeStatement:	typeKeyword sep refinedBaseType optsep ';'
			{
			    $$ = $3;
			}
	;

typeStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |		typeStatement stmtsep
			{
			    $$ = $1;
			}
	;

typeStatement:		typeKeyword sep refinedBaseType_refinedType
			optsep ';'
			{
			    $$ = $3;
			}
        ;

writetypeStatement_stmtsep_01: /* empty */
			{
			    $$ = NULL;
			}
        |	        writetypeStatement stmtsep
			{
			    $$ = $1;
			}
	;

writetypeStatement:	writetypeKeyword sep refinedBaseType_refinedType
			optsep ';'
			{
			    $$ = $3;
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
			    $$ = util_malloc(sizeof(Index));
			    if ($2) {
				$$->implied = 1;
			    } else {
				$$->implied = 0;
			    }
			    $$->indexkind = SMI_INDEX_INDEX;
			    $$->listPtr = $6;
			    $$->rowPtr = NULL;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			}
        ;

augmentsStatement:	augmentsKeyword sep qlcIdentifier optsep ';'
			{
			    $$ = util_malloc(sizeof(Index));
			    $$->implied = 0;
			    $$->indexkind = SMI_INDEX_AUGMENT;
			    $$->listPtr = NULL;
			    $$->rowPtr = (void *)$3;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			}
        ;

reordersStatement:	reordersKeyword sep qlcIdentifier
			sep_impliedKeyword_01 optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = util_malloc(sizeof(Index));
			    if ($4) {
				$$->implied = 1;
			    } else {
				$$->implied = 0;
			    }
			    $$->indexkind = SMI_INDEX_REORDER;
			    $$->listPtr = $8;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			    $$->rowPtr = (void *)$3;
			}
        ;

sparseStatement:	sparseKeyword sep qlcIdentifier optsep ';'
			{
			    $$ = util_malloc(sizeof(Index));
			    $$->implied = 0;
			    $$->indexkind = SMI_INDEX_SPARSE;
			    $$->listPtr = NULL;
			    $$->rowPtr = (void *)$3;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			}
	;

expandsStatement:	expandsKeyword sep qlcIdentifier
			sep_impliedKeyword_01 optsep '(' optsep
			qlcIdentifierList optsep ')' optsep ';'
			{
			    $$ = util_malloc(sizeof(Index));
			    if ($4) {
				$$->implied = 1;
			    } else {
				$$->implied = 0;
			    }
			    $$->indexkind = SMI_INDEX_EXPAND;
			    $$->listPtr = $8;
			    /*
			     * NOTE: at this point $$->listPtr and $$-rowPtr
			     * contain identifier strings. Converstion to
			     * (Object *)'s must be delayed till the whole
			     * module is parsed, since even in SMIng index
			     * clauses can contain forward references.
			     */
			    $$->rowPtr = (void *)$3;
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
			    $$ = $3;
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
			    $$ = SMI_STATUS_CURRENT;
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
			    if (!$2) {
				$$ = typeSmingOctetStringPtr;
			    } else {
				$$ = duplicateType(typeSmingOctetStringPtr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingOctetStringPtr->name);
				setTypeList($$, $2);
			    }
			}
        |		ObjectIdentifierKeyword
			{
			    $$ = typeSmingObjectIdentifierPtr;
			}
	|		Integer32Keyword optsep_numberSpec_01
			{
			    if (!$2) {
				$$ = typeSmingInteger32Ptr;
			    } else {
				$$ = duplicateType(typeSmingInteger32Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingInteger32Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		Unsigned32Keyword optsep_numberSpec_01
			{
			    if (!$2) {
				$$ = typeSmingUnsigned32Ptr;
			    } else {
				$$ = duplicateType(typeSmingUnsigned32Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingUnsigned32Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		Integer64Keyword optsep_numberSpec_01
			{
			    if (!$2) {
				$$ = typeSmingInteger64Ptr;
			    } else {
				$$ = duplicateType(typeSmingInteger64Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingInteger64Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		Unsigned64Keyword optsep_numberSpec_01
			{
			    if (!$2) {
				$$ = typeSmingUnsigned64Ptr;
			    } else {
				$$ = duplicateType(typeSmingUnsigned64Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingUnsigned64Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		Float32Keyword optsep_floatSpec_01
			{
			    if (!$2) {
				$$ = typeSmingFloat32Ptr;
			    } else {
				$$ = duplicateType(typeSmingFloat32Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingFloat32Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		Float64Keyword optsep_floatSpec_01
			{
			    if (!$2) {
				$$ = typeSmingFloat64Ptr;
			    } else {
				$$ = duplicateType(typeSmingFloat64Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingFloat64Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		Float128Keyword optsep_floatSpec_01
			{
			    if (!$2) {
				$$ = typeSmingFloat128Ptr;
			    } else {
				$$ = duplicateType(typeSmingFloat128Ptr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingFloat128Ptr->name);
				setTypeList($$, $2);
			    }
			}
	|		EnumerationKeyword bitsOrEnumerationSpec
			{
			    if (!$2) {
				$$ = typeSmingEnumPtr;
			    } else {
				$$ = duplicateType(typeSmingEnumPtr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingEnumPtr->name);
				setTypeList($$, $2);
			    }
			}
	|		BitsKeyword bitsOrEnumerationSpec
			{
			    if (!$2) {
				$$ = typeSmingBitsPtr;
			    } else {
				$$ = duplicateType(typeSmingBitsPtr, 0,
						   thisParserPtr);
				setTypeParent($$,
					      typeSmingBitsPtr->name);
				setTypeList($$, $2);
			    }
			}
	;

refinedType:		qucIdentifier optsep_anySpec_01
			{
			    char s[SMI_MAX_FULLNAME];

			    typePtr = findType($1, thisParserPtr);
			    if (typePtr && $2) {
				sprintf(s, "%s%s%s",
					typePtr->modulePtr->name,
					SMI_NAMESPACE_OPERATOR,
					typePtr->name);
				typePtr = duplicateType(typePtr, 0,
							thisParserPtr);
				setTypeParent(typePtr, s);
				setTypeList(typePtr, $2);
			    }

			    $$ = typePtr;
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
			    $$->name = $1;
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
			    $$->ptr = $1;
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
			    $$ = $4;
			}
        ;

identifier:		ucIdentifier
			{
			    $$ = $1;
			}
        |		lcIdentifier
			{
			    $$ = $1;
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
			    free($1);
			    free($3);
			}
        |		ucIdentifier
			{
			    $$ = $1;
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
			    free($1);
			    free($3);
			}
        |		lcIdentifier
			{
			    $$ = $1;
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

/*
 * The type of `anyValue' must be determined from the context. `anyValue'
 * appears only in default value clauses. Hence, we set a global variable
 * XXX in the object type declaring clause to remember the expected type.
 * Here, we use this variable to build an SmiValue with the appropriate
 * base type.
 */
anyValue:		bitsValue
			{
			    int i;
			    List *listPtr, *nextPtr;
			    
			    if (defaultBasetype == SMI_BASETYPE_BITS) {
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_BITS;
				$$->value.bits = NULL;
				for (i = 0, listPtr = $1; listPtr;
				     i++, listPtr = nextPtr) {
				    nextPtr = listPtr->nextPtr;
				    $$->value.bits =
					util_realloc($$->value.bits,
						     sizeof(char *) * (i+2));
				    $$->value.bits[i] = listPtr->ptr;
				    $$->value.bits[i+1] = NULL;
				    free(listPtr);
				}
			    } else {
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				$$ = NULL;
			    }
			}
	|		decimalNumber
			{
			    /* Note: might also be an OID or signed */
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_UNSIGNED32:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 10);
				break;
			    case SMI_BASETYPE_UNSIGNED64:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_UNSIGNED64;
				$$->value.unsigned64 = strtoull($1, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER32:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_INTEGER32;
				$$->value.integer32 = strtol($1, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER64:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_INTEGER64;
				$$->value.integer64 = strtoll($1, NULL, 10);
				break;
			    case SMI_BASETYPE_OBJECTIDENTIFIER:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
				$$->value.oid = util_strdup($1);
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				$$ = NULL;
				break;
			    }
			}
	|		'-' decimalNumber
			{
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_INTEGER32:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_INTEGER32;
				$$->value.integer32 = - strtoul($2, NULL, 10);
				break;
			    case SMI_BASETYPE_INTEGER64:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_INTEGER64;
				$$->value.integer64 = - strtoull($2, NULL, 10);
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				$$ = NULL;
				break;
			    }
			}
	|		hexadecimalNumber
			{
			    /* TODO */
			    $$ = NULL;
			}
	|		floatValue
			{
			    /* TODO */
			    /* Note: might also be an OID */
			    $$ = NULL;
			}
	|		text
			{
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_OCTETSTRING;
				$$->value.ptr = $1;
			    } else {
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				$$ = NULL;
			    }
			}
	|		qlcIdentifier
			{
			    /* Note: might be an Enumeration item or OID */
			    /* TODO: convert if it's an oid? */
			    switch (defaultBasetype) {
			    case SMI_BASETYPE_ENUM:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_ENUM;
				$$->value.ptr = $1;
				break;
			    case SMI_BASETYPE_OBJECTIDENTIFIER:
				$$ = util_malloc(sizeof(SmiValue));
				$$->basetype = SMI_BASETYPE_OBJECTIDENTIFIER;
				$$->value.ptr = $1;
				break;
			    default:
				printError(thisParserPtr,
					   ERR_UNEXPECTED_VALUETYPE);
				$$ = NULL;
				break;
			    }
			}
	|		qlcIdentifier dot_subid_1n
			{
			    /* TODO */
			    $$ = NULL;
			}
	|		subid '.' subid dot_subid_1n
			{
			    /* TODO */
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
			    char *oid = NULL;
			    Node *nodePtr;

			    if ($1 && $2) {
				oid = util_malloc(strlen($1) + strlen($2) + 1);
				strcpy(oid, $1);
				strcat(oid, $2);
				free($1);
				free($2);
			    } else if ($1) {
				oid = util_malloc(strlen($1) + 1);
				strcpy(oid, $1);
				free($1);
			    }
			    
			    if (oid) {
				nodePtr = findNodeByOidString(oid);
				if (!nodePtr) {
				    nodePtr = createNodes(oid);
				}
				$$ = nodePtr;
			    } else {
				$$ = NULL;
			    }
			}
	;

qlcIdentifier_subid:	qlcIdentifier
			{
			    SmiNode *smiNodePtr;
			    Import *importPtr;

			    /* TODO: findObject(), then createOid */
			    if (strstr($1, SMI_NAMESPACE_OPERATOR)) {
				smiNodePtr = smiGetNode($1, "");
			    } else {
				smiNodePtr = smiGetNode($1,
							thisModulePtr->name);
				if (!smiNodePtr) {
				    importPtr =
					findImportByName($1, thisModulePtr);
				    if (importPtr) {
					smiNodePtr =
					    smiGetNode($1, importPtr->module);
				    }
				}
			    }
			    if (smiNodePtr) {
				$$ = util_strdup(smiNodePtr->oid);
				smiFreeNode(smiNodePtr);
			    } else {
				printError(thisParserPtr,
					   ERR_UNKNOWN_OIDLABEL, $1);
				$$ = NULL;
			    }
			}
        |		subid
			{
			    $$ = $1;
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
			
