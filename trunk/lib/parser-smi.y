/*
 * parser-smi.y --
 *
 *      Syntax rules for parsing the SMIv1/v2 MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: parser-smi.y,v 1.31 1999/06/10 15:28:22 strauss Exp $
 */

%{

#ifdef BACKEND_SMI
    
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
    
#include "smi.h"
#include "error.h"
#include "parser-smi.h"
#include "scanner-smi.h"
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



static Node	   *parentNodePtr;
static int	   impliedFlag;
static SmiBasetype defaultBasetype;
static Module      *complianceModulePtr = NULL; 


%}

/*
 * The grammars start symbol.
 */
%start mibFile



/*
 * We call the parser from within the parser when IMPORTing modules,
 * hence we need reentrant parser code. This is a bison feature.
 */
%pure_parser



/*
 * The attributes.
 */
%union {
    char           *text;	  		/* scanned quoted text       */
    char           *id;				/* identifier name           */
    int            err;				/* actually just a dummy     */
    Object         *objectPtr;			/* object identifier         */
    SmiStatus      status;			/* a STATUS value            */
    SmiAccess      access;			/* an ACCESS value           */
    Type           *typePtr;
    Index          *indexPtr;
    List           *listPtr;			/* SEQUENCE and INDEX lists  */
    NamedNumber    *namedNumberPtr;		/* BITS or enum item         */
    Range          *rangePtr;			/* type restricting range    */
    SmiValue	   *valuePtr;
    SmiUnsigned32  unsigned32;			/*                           */
    SmiInteger32   integer32;			/*                           */
    struct Compl   compl;
    Module	   *modulePtr;
}



/*
 * Tokens and their attributes.
 */
%token DOT_DOT
%token COLON_COLON_EQUAL

%token <id>UPPERCASE_IDENTIFIER
%token <id>LOWERCASE_IDENTIFIER
%token <id>NUMBER
%token <text>BIN_STRING
%token <text>HEX_STRING
%token <text>QUOTED_STRING

%token <id>ACCESS
%token <id>AGENT_CAPABILITIES
%token <id>APPLICATION
%token <id>AUGMENTS
%token <id>BEGIN_
%token <id>BITS
%token <id>CHOICE
%token <id>CONTACT_INFO
%token <id>CREATION_REQUIRES
%token <id>COUNTER32
%token <id>COUNTER64
%token <id>DEFINITIONS
%token <id>DEFVAL
%token <id>DESCRIPTION
%token <id>DISPLAY_HINT
%token <id>END
%token <id>ENTERPRISE
%token <id>EXPORTS
%token <id>FROM
%token <id>GROUP
%token <id>GAUGE32
%token <id>IDENTIFIER
%token <id>IMPLICIT
%token <id>IMPLIED
%token <id>IMPORTS
%token <id>INCLUDES
%token <id>INDEX
%token <id>INTEGER
%token <id>INTEGER32
%token <id>IPADDRESS
%token <id>LAST_UPDATED
%token <id>MACRO
%token <id>MANDATORY_GROUPS
%token <id>MAX_ACCESS
%token <id>MIN_ACCESS
%token <id>MODULE
%token <id>MODULE_COMPLIANCE
%token <id>MODULE_IDENTITY
%token <id>NOTIFICATIONS
%token <id>NOTIFICATION_GROUP
%token <id>NOTIFICATION_TYPE
%token <id>OBJECT
%token <id>OBJECT_GROUP
%token <id>OBJECT_IDENTITY
%token <id>OBJECT_TYPE
%token <id>OBJECTS
%token <id>OCTET
%token <id>OF
%token <id>ORGANIZATION
%token <id>OPAQUE
%token <id>PRODUCT_RELEASE
%token <id>REFERENCE
%token <id>REVISION
%token <id>SEQUENCE
%token <id>SIZE
%token <id>STATUS
%token <id>STRING
%token <id>SUPPORTS
%token <id>SYNTAX
%token <id>TEXTUAL_CONVENTION
%token <id>TIMETICKS
%token <id>TRAP_TYPE
%token <id>UNITS
%token <id>UNIVERSAL
%token <id>UNSIGNED32
%token <id>VARIABLES
%token <id>VARIATION
%token <id>WRITE_SYNTAX



/*
 * Types of non-terminal symbols.
 */
%type  <err>mibFile
%type  <err>modules
%type  <err>module
%type  <id>moduleName
%type  <id>importIdentifier
%type  <err>importIdentifiers
%type  <id>importedKeyword
%type  <err>linkagePart
%type  <err>linkageClause
%type  <err>importPart
%type  <err>imports
%type  <err>declarationPart
%type  <err>declarations
%type  <err>declaration
%type  <err>exportsClause
%type  <err>macroClause
%type  <id>macroName
%type  <typePtr>choiceClause
%type  <id>typeName
%type  <id>typeSMI
%type  <err>typeTag
%type  <err>valueDeclaration
%type  <typePtr>conceptualTable
%type  <typePtr>row
%type  <typePtr>entryType
%type  <listPtr>sequenceItems
%type  <objectPtr>sequenceItem
%type  <typePtr>Syntax
%type  <typePtr>sequenceSyntax
%type  <listPtr>NamedBits
%type  <namedNumberPtr>NamedBit
%type  <id>identifier
%type  <err>objectIdentityClause
%type  <err>objectTypeClause
%type  <err>trapTypeClause
%type  <text>descriptionClause
%type  <err>VarPart
%type  <err>VarTypes
%type  <err>VarType
%type  <err>DescrPart
%type  <access>MaxAccessPart
%type  <err>notificationTypeClause
%type  <err>moduleIdentityClause
%type  <err>typeDeclaration
%type  <typePtr>typeDeclarationRHS
%type  <typePtr>ObjectSyntax
%type  <typePtr>sequenceObjectSyntax
%type  <valuePtr>valueofObjectSyntax
%type  <typePtr>SimpleSyntax
%type  <valuePtr>valueofSimpleSyntax
%type  <typePtr>sequenceSimpleSyntax
%type  <typePtr>ApplicationSyntax
%type  <typePtr>sequenceApplicationSyntax
%type  <listPtr>anySubType
%type  <listPtr>integerSubType
%type  <listPtr>octetStringSubType
%type  <listPtr>ranges
%type  <rangePtr>range
%type  <valuePtr>value
%type  <listPtr>enumSpec
%type  <listPtr>enumItems
%type  <namedNumberPtr>enumItem
%type  <valuePtr>enumNumber
%type  <status>Status
%type  <status>Status_Capabilities
%type  <text>DisplayPart
%type  <text>UnitsPart
%type  <access>Access
%type  <indexPtr>IndexPart
%type  <listPtr>IndexTypes
%type  <objectPtr>IndexType
%type  <objectPtr>Index
%type  <objectPtr>Entry
%type  <valuePtr>DefValPart
%type  <valuePtr>Value
%type  <listPtr>BitsValue
%type  <listPtr>BitNames
%type  <text>BitName
%type  <objectPtr>ObjectName
%type  <objectPtr>NotificationName
%type  <text>ReferPart
%type  <err>RevisionPart
%type  <err>Revisions
%type  <err>Revision
%type  <listPtr>ObjectsPart
%type  <listPtr>Objects
%type  <objectPtr>Object
%type  <listPtr>NotificationsPart
%type  <listPtr>Notifications
%type  <objectPtr>Notification
%type  <text>Text
%type  <text>ExtUTCTime
%type  <objectPtr>objectIdentifier
%type  <objectPtr>subidentifiers
%type  <objectPtr>subidentifier
%type  <text>objectIdentifier_defval
%type  <err>subidentifiers_defval
%type  <err>subidentifier_defval
%type  <err>objectGroupClause
%type  <err>notificationGroupClause
%type  <err>moduleComplianceClause
%type  <compl>ComplianceModulePart
%type  <compl>ComplianceModules
%type  <compl>ComplianceModule
%type  <modulePtr>ComplianceModuleName
%type  <listPtr>MandatoryPart
%type  <listPtr>MandatoryGroups
%type  <objectPtr>MandatoryGroup
%type  <compl>CompliancePart
%type  <compl>Compliances
%type  <compl>Compliance
%type  <listPtr>ComplianceGroup
%type  <listPtr>ComplianceObject
%type  <typePtr>SyntaxPart
%type  <typePtr>WriteSyntaxPart
%type  <typePtr>WriteSyntax
%type  <access>AccessPart
%type  <err>agentCapabilitiesClause
%type  <err>ModulePart_Capabilities
%type  <err>Modules_Capabilities
%type  <err>Module_Capabilities
%type  <err>ModuleName_Capabilities
%type  <listPtr>CapabilitiesGroups
%type  <listPtr>CapabilitiesGroup
%type  <err>VariationPart
%type  <err>Variations
%type  <err>Variation
%type  <err>NotificationVariation
%type  <err>ObjectVariation
%type  <err>CreationPart
%type  <err>Cells
%type  <err>Cell
%type  <unsigned32>number

%%

/*
 * Yacc rules.
 *
 */


/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
mibFile:		modules
    			{ $$ = 0; }
	|		/* empty */
    			{ $$ = 0; }
	;

modules:		module
			{ $$ = 0; }
	|		modules module
			{ $$ = 0; }
	;

/*
 * The general structure of a module is described at REF:RFC1902,3. .
 * An example is given at REF:RFC1902,5.7. .
 */
module:			moduleName
			{
			    /*
			     * This module is to be parsed, either if we
			     * want the whole file or if its name matches
			     * the module name we are looking for.
			     * In fact, we always parse it, but we just
			     * remember its contents if needed.
			     */
			    thisParserPtr->modulePtr = findModuleByName($1);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule($1,
					      thisParserPtr->path,
					      thisParserPtr->character,
					      0,
					      thisParserPtr);
			    }
			    thisParserPtr->modulePtr->flags &= ~FLAG_SMIV2;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			    if (!strcmp($1, "SNMPv2-SMI")) {
			        /*
				 * SNMPv2-SMI is an SMIv2 module that cannot
				 * be identified by importing from SNMPv2-SMI.
				 */
			        thisParserPtr->modulePtr->flags |= FLAG_SMIV2;
			    }

			}
			DEFINITIONS COLON_COLON_EQUAL BEGIN_
			exportsClause
			linkagePart
			declarationPart
			END
			{
			    if ((thisModulePtr->flags & FLAG_SMIV2) &&
				(thisModulePtr->numModuleIdentities < 1) &&
				strcmp(thisModulePtr->name, "SNMPv2-SMI") &&
				strcmp(thisModulePtr->name, "SNMPv2-CONF") &&
				strcmp(thisModulePtr->name, "SNMPv2-TC")) {
			        printError(thisParserPtr,
					   ERR_NO_MODULE_IDENTITY);
			    }
			    /* TODO
			    for (p = firstPendingNode; p; p = p->next) {
				printError(parser, ERR_UNKNOWN_OIDLABEL,
					   p->descriptor->name);
			    }
			    */
			    $$ = 0;
			}
	;

/*
 * REF:RFC1902,3.2.
 */
linkagePart:		linkageClause
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

linkageClause:		IMPORTS importPart ';'
			{ $$ = 0; }
        ;

exportsClause:		/* empty */
			{ $$ = 0; }
	|		EXPORTS
			{
			    if (strcmp(thisParserPtr->modulePtr->name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_EXPORTS);
			    }
			}
			/* the scanner skips until... */
			';'
			{ $$ = 0; }
	;

importPart:		imports
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
			/* TODO: ``IMPORTS ;'' allowed? refer ASN.1! */
	;

imports:		import
			{ $$ = 0; }
	|		imports import
			{ $$ = 0; }
	;

import:			importIdentifiers FROM moduleName
			/* TODO: multiple clauses with same moduleName
			 * allowed? I guess so. refer ASN.1! */
			{
			    if (!strcmp($3, "SNMPv2-SMI")) {
			        /*
				 * A module that imports from SNMPv2-SMI
				 * seems to be SMIv2 style.
				 */
			        thisParserPtr->modulePtr->flags |= FLAG_SMIV2;
			    }

			    /*
			     * Recursively call the parser to suffer
			     * the IMPORTS, if the module is not yet
			     * loaded.
			     */
			    if (!findModuleByName($3)) {
				smiLoadModule($3);
			    }
			    checkImports($3, thisParserPtr);
			}
	;

importIdentifiers:	importIdentifier
			{ $$ = 0; }
	|		importIdentifiers ',' importIdentifier
			/* TODO: might this list list be empty? */
			{ $$ = 0; }
	;

/*
 * Note that some named types must not be imported, REF:RFC1902,590 .
 */
importIdentifier:	LOWERCASE_IDENTIFIER
			{
			    addImport($1, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    $$ = util_strdup($1);
			}
	|		UPPERCASE_IDENTIFIER
			{
			    addImport($1, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    $$ = util_strdup($1);
			}
	|		importedKeyword
			/* TODO: what exactly is allowed here?
			 * What should be checked? */
			{
			    addImport($1, thisParserPtr);
			    thisParserPtr->modulePtr->numImportedIdentifiers++;
			    $$ = util_strdup($1);
			}
	;

/*
 * These keywords are no real keywords. They have to be imported
 * from the SMI, TC, CONF MIBs.
 */
/*
 * TODO: Think! Shall we really leave these words as keywords or should
 * we prefer the symbol table appropriately??
 */
importedKeyword:	ACCESS
        |		AGENT_CAPABILITIES
	|		AUGMENTS
	|		BITS
	|		CONTACT_INFO
	|		CREATION_REQUIRES
	|		COUNTER32
	|		COUNTER64
	|		DEFVAL
	|		DESCRIPTION
	|		DISPLAY_HINT
	|		GROUP
	|		GAUGE32
	|		IMPLIED
	|		INDEX
	|		INTEGER32
	|		IPADDRESS
	|		LAST_UPDATED
	|		MANDATORY_GROUPS
	|		MAX_ACCESS
	|		MIN_ACCESS
	|		MODULE
	|		MODULE_COMPLIANCE
	|		MODULE_IDENTITY
	|		NOTIFICATIONS
	|		NOTIFICATION_GROUP
	|		NOTIFICATION_TYPE
	|		OBJECT_GROUP
	|		OBJECT_IDENTITY
	|		OBJECT_TYPE
	|		OBJECTS
	|		ORGANIZATION
	|		OPAQUE
	|		PRODUCT_RELEASE
	|		REFERENCE
	|		REVISION
	|		STATUS
	|		SUPPORTS
	|		SYNTAX
	|		TEXTUAL_CONVENTION
	|		TIMETICKS
	|		TRAP_TYPE
	|		UNITS
	|		UNSIGNED32
	|		VARIATION
	|		WRITE_SYNTAX
			/* TODO: which keywords should really be
			 * allowed in import clauses? */
	;

moduleName:		UPPERCASE_IDENTIFIER
			{
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_MODULENAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_MODULENAME_32, $1);
			    }
			    $$ = util_strdup($1);
			}
	;

/*
 * The paragraph at REF:RFC1902,490 lists roughly what's allowed
 * in the body of an information module.
 */
declarationPart:	declarations
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
			/* TODO: might this list really be emtpy? */
	;

declarations:		declaration
			{ $$ = 0; }
	|		declarations declaration
			{ $$ = 0; }
	;

declaration:		typeDeclaration
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		valueDeclaration
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		objectIdentityClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		objectTypeClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		trapTypeClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		notificationTypeClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		moduleIdentityClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
			/* TODO: check it's first and once */
	|		moduleComplianceClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		objectGroupClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		notificationGroupClause
			{
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		macroClause
			{ 
			    thisParserPtr->modulePtr->numStatements++;
			    $$ = 0;
			}
	|		error '}'
			{
			    printError(thisParserPtr, ERR_FLUSH_DECLARATION);
			    yyerrok;
			    $$ = 1;
			}
	;

/*
 * Macro clauses. Its contents are not really parsed, but skipped by
 * the scanner until 'END' is read. This is just to make the SMI
 * documents readable.
 */
macroClause:		macroName
			MACRO
			{
			    /*
			     * ASN.1 macros are known to be in these
			     * modules.
			     */
			    if (strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC-1215") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_MACRO);
			    }
			}
			/* the scanner skips until... */
			END
			{
			    /*
			     * Some bison magics make the objectIdentifier
			     * to be $7 instead of $6.
			     */
			    addMacro($1, thisParserPtr->character, 0,
				     thisParserPtr);
			    $$ = 0;
                        }
	;

macroName:		MODULE_IDENTITY     { $$ = util_strdup($1); } 
	|		OBJECT_TYPE	    { $$ = util_strdup($1); }
	|		TRAP_TYPE	    { $$ = util_strdup($1); }
	|		NOTIFICATION_TYPE   { $$ = util_strdup($1); }
	|		OBJECT_IDENTITY	    { $$ = util_strdup($1); }
	|		TEXTUAL_CONVENTION  { $$ = util_strdup($1); }
	|		OBJECT_GROUP	    { $$ = util_strdup($1); }
	|		NOTIFICATION_GROUP  { $$ = util_strdup($1); }
	|		MODULE_COMPLIANCE   { $$ = util_strdup($1); }
	|		AGENT_CAPABILITIES  { $$ = util_strdup($1); }
	;

choiceClause:		CHOICE
			{
			    if (strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_CHOICE);
			    }
			}
			/* the scanner skips until... */
			'}'
			{
			    $$ = addType(NULL, SMI_BASETYPE_CHOICE, 0,
					 thisParserPtr);
			}
	;

/*
 * The only ASN.1 value declarations are for OIDs, REF:RFC1902,491 .
 */
valueDeclaration:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			    if (thisParserPtr->modulePtr->flags & FLAG_SMIV2) {
			        if (strchr($1, '-') &&
				    (strcmp($1, "mib-2") ||
				  strcmp(thisModulePtr->name, "SNMPv2-SMI"))) {
				    printError(thisParserPtr,
					       ERR_OIDNAME_INCLUDES_HYPHEN,
					       $1);
				}
			    }
			}
			OBJECT IDENTIFIER
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $7;
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr =
				    duplicateObject(objectPtr,
						    0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectDecl(objectPtr,
					  SMI_DECL_VALUEASSIGNMENT);
			    setObjectNodekind(objectPtr,
					      SMI_NODEKIND_NODE);
			    $$ = 0;
			}
	;

/*
 * This is for simple ASN.1 style type assignments and textual conventions.
 */
typeDeclaration:	typeName
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_TYPENAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_TYPENAME_32, $1);
			    }
			}
			COLON_COLON_EQUAL typeDeclarationRHS
			{
			    Type *typePtr;
			    
			    if (strlen($1)) {
				typePtr = $4;
				setTypeName(typePtr, $1);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

typeName:		UPPERCASE_IDENTIFIER
			{
			    $$ = util_strdup($1);
			}
	|		typeSMI
			{
			    /*
			     * well known types (keywords in this grammar)
			     * are known to be defined in these modules.
			     */
			    if (strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_TYPE_SMI, $1);
			    }
			    /*
			     * clear $$, so that the `typeDeclarationRHS'
			     * rule will not add a new Descriptor for this
			     * already known type.
			     */
			}
	;

typeSMI:		INTEGER32   { $$ = util_strdup($1); }
	|		IPADDRESS   { $$ = util_strdup($1); }
	|		COUNTER32   { $$ = util_strdup($1); }
	|		GAUGE32     { $$ = util_strdup($1); }
	|		UNSIGNED32  { $$ = util_strdup($1); }
	|		TIMETICKS   { $$ = util_strdup($1); }
	|		OPAQUE      { $$ = util_strdup($1); }
	|		COUNTER64   { $$ = util_strdup($1); }
	;

typeDeclarationRHS:	Syntax
			{
			    if ($1->name) {
				/*
				 * If we found an already defined type,
				 * we have to inherit a new type structure.
				 * (Otherwise the `Syntax' rule created
				 * a new type for us.)
				 */
				$$ = duplicateType($1, 0, thisParserPtr);
				setTypeDecl($$, SMI_DECL_TYPEASSIGNMENT);
			    } else {
				$$ = $1;
			    }
			}
	|		TEXTUAL_CONVENTION
			DisplayPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			SYNTAX Syntax
			{
			    if (($9) && !($9->name)) {
				/*
				 * If the Type we found has just been
				 * defined, we don't have to allocate
				 * a new one.
				 */
				$$ = $9;
			    } else {
				/*
				 * Otherwise, we have to allocate a
				 * new Type struct, inherited from $9.
				 */
				$$ = duplicateType($9, 0, thisParserPtr);
				deleteTypeFlags($$, FLAG_IMPORTED);
			    }
			    setTypeDescription($$, $6);
			    if ($7) {
				setTypeReference($$, $7);
			    }
			    setTypeStatus($$, $4);
			    if ($2) {
				setTypeFormat($$, $2);
			    }
			    setTypeDecl($$, SMI_DECL_TEXTUALCONVENTION);
			}
	|		choiceClause
			{
			    $$ = $1;
			}
	;

/* REF:RFC1902,7.1.12. */
conceptualTable:	SEQUENCE OF row
			{
			    char s[SMI_MAX_FULLNAME];
			    
			    if ($3) {
				$$ = addType(NULL,
					     SMI_BASETYPE_SEQUENCEOF, 0,
					     thisParserPtr);
				sprintf(s, "%s::%s", $3->modulePtr->name,
					$3->name);
				setTypeParent($$, s);
			    } else {
				$$ = NULL;
			    }
			}
	;

row:			UPPERCASE_IDENTIFIER
			/*
			 * In this case, we do NOT allow `Module.Type'.
			 * The identifier must be defined in the local
			 * module.
			 */
			{
			    Type *typePtr;
			    Import *importPtr;
			    SmiType *stypePtr;

			    $$ = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->name, $1);
			    if (! $$) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    typePtr = addType($1,
						      SMI_BASETYPE_SEQUENCE,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				    $$ = typePtr;
				} else {
				    /*
				     * imported type.
				     * TODO: is this allowed in a SEQUENCE? 
				     */
				    stypePtr = smiGetType(
						  importPtr->importmodule, $1);
				    if (stypePtr) {
					$$ = addType($1, stypePtr->basetype,
						     FLAG_IMPORTED,
						     thisParserPtr);
				    } else {
					$$ = addType($1, SMI_BASETYPE_UNKNOWN,
					       FLAG_INCOMPLETE | FLAG_IMPORTED,
						     thisParserPtr);
				    }
				}
			    }
			}
		        /* TODO: this must be an entryType */
	;

/* REF:RFC1902,7.1.12. */
entryType:		SEQUENCE '{' sequenceItems '}'
			{
			    $$ = addType(NULL, SMI_BASETYPE_SEQUENCE, 0,
					 thisParserPtr);
			    setTypeList($$, $3);
			}
;

sequenceItems:		sequenceItem
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		sequenceItems ',' sequenceItem
			/* TODO: might this list be emtpy? */
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 * NOTE: REF:RFC1902,7.1.12. was less clear, it said:
 * `normally omitting the sub-typing information'
 */
sequenceItem:		LOWERCASE_IDENTIFIER sequenceSyntax
			{
			    Object *objectPtr;
			    SmiNode *snodePtr;
			    Import *importPtr;
			    
			    objectPtr =
			        findObjectByModuleAndName(thisParserPtr->modulePtr,
							  $1);

			    if (objectPtr) {
				$$ = objectPtr;
			    } else {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr) {
				    objectPtr = addObject($1, pendingNodePtr,
					                  0,
					                  FLAG_INCOMPLETE,
						          thisParserPtr);
				    setObjectFileOffset(objectPtr,
						        thisParserPtr->character);
				    $$ = objectPtr;
				} else {
				    /*
				     * imported object.
				     */
				    snodePtr = smiGetNode(
						  importPtr->importmodule, $1);
				    $$ = addObject($1,
					getParentNode(
				             createNodes(snodePtr->oidlen,
							 snodePtr->oid)),
					snodePtr->oid[snodePtr->oidlen-1],
					FLAG_IMPORTED, thisParserPtr);
				}
			    }
			}
	;

Syntax:			ObjectSyntax
			{
			    $$ = $1;
			    /*
			     * Remember the basetype. This could be
			     * needed if a following DEFVAL clause has
			     * to decide whether a HEX or BIN string
			     * is a number or an octet string.
			     */
			    defaultBasetype = $$->basetype;
			}
	|		BITS '{' NamedBits '}'
			/* TODO: standalone `BITS' ok? seen in RMON2-MIB */
			/* -> no, it's only allowed in a SEQUENCE {...} */
			{
			    Type *typePtr;
			    
			    typePtr = addType(NULL, SMI_BASETYPE_BITS,
					      FLAG_INCOMPLETE,
					      thisParserPtr);
			    setTypeList(typePtr, $3);
			    $$ = typePtr;
			}
	;

sequenceSyntax:		/* ObjectSyntax */
			sequenceObjectSyntax
			{
			    $$ = $1;
			}
	|		BITS
			{
			    /* TODO: */
			    $$ = typeSmiOctetStringPtr;
			}
	|		UPPERCASE_IDENTIFIER anySubType
			{
			    Type *typePtr;
			    Import *importPtr;
			    
			    $$ = findTypeByModulenameAndName(
				thisParserPtr->modulePtr->name, $1);
			    if (! $$) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    typePtr = addType($1, SMI_BASETYPE_UNKNOWN,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				    $$ = typePtr;
				} else {
				    /*
				     * imported type.
				     *
				     * We are in a SEQUENCE clause,
				     * where we do not have to create
				     * a new Type struct.
				     */
				}
			    }
			}
	;

NamedBits:		NamedBit
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		NamedBits ',' NamedBit
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

NamedBit:		identifier
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_BITNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_BITNAME_32, $1);
			    }
			}
			'(' number ')'
			{
			    $$ = util_malloc(sizeof(NamedNumber));
			    $$->valuePtr = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->name = util_strdup($1);
			    $$->valuePtr->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->valuePtr->value.unsigned32 = $4;
			}
	;

identifier:		LOWERCASE_IDENTIFIER
			/* TODO */
			{
			    $$ = util_strdup($1);
			}
	;

objectIdentityClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			OBJECT_IDENTITY
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $11;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr,
				                            0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTIDENTITY);
			    setObjectNodekind(objectPtr,
					      SMI_NODEKIND_NODE);
			    setObjectStatus(objectPtr, $5);
			    setObjectDescription(objectPtr, $7);
			    if ($8) {
				setObjectReference(objectPtr, $8);
			    }
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    $$ = 0;
			}
	;

objectTypeClause:	LOWERCASE_IDENTIFIER
			{
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			OBJECT_TYPE
			SYNTAX Syntax
		        UnitsPart
			MaxAccessPart
			STATUS Status
			descriptionClause
			ReferPart
			IndexPart
			DefValPart
			COLON_COLON_EQUAL '{' ObjectName '}'
			{
			    Object *objectPtr, *parentPtr;
			    List *listPtr, *newlistPtr;

			    objectPtr = $16;
			    
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr,
				                            0, thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTTYPE);
			    setObjectNodekind(objectPtr, SMI_NODEKIND_XXX);
			    /* TODO XXX... detect from basetype, then
			                   set nodekind */
			    setObjectType(objectPtr, $5);
			    if (!($5->name)) {
				/*
				 * An inlined type.
				 */
				setTypeName($5, $1);
			    }
			    setObjectAccess(objectPtr, $7);
			    if ($7 == SMI_ACCESS_READ_CREATE) {
				parentPtr =
				  objectPtr->nodePtr->parentPtr->lastObjectPtr;
				if (parentPtr && parentPtr->typePtr &&
				    (parentPtr->typePtr->basetype ==
				       SMI_BASETYPE_SEQUENCEOF)) {
				    /*
				     * add objectPtr to the parent object's
				     * listPtr, which is the list of columns
				     * needed for row creation.
				     *
				     * Note, that this would clash, if the
				     * parent row object-type is not yet
				     * defined.
				     */
				    newlistPtr = util_malloc(sizeof(List));
				    newlistPtr->nextPtr = NULL;
				    newlistPtr->ptr = objectPtr;
				    /*
				     * Look up the parent object-type.
				     */
				    if (parentPtr->listPtr) {
					for(listPtr = parentPtr->listPtr;
					    listPtr->nextPtr;
					    listPtr = listPtr->nextPtr);
					listPtr->nextPtr = newlistPtr;
				    } else {
					parentPtr->listPtr = newlistPtr;
				    }
				    addObjectFlags(parentPtr, FLAG_CREATABLE);
				}
			    }
			    setObjectStatus(objectPtr, $9);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    if ($10) {
				setObjectDescription(objectPtr, $10);
			    }
			    if ($11) {
				setObjectReference(objectPtr, $11);
			    }
			    if ($12) {
				setObjectIndex(objectPtr, $12);
			    }
			    if ($13) {
				setObjectValue(objectPtr, $13);
			    }
			    $$ = 0;
			}
	;

descriptionClause:	/* empty */
			{
			    if (thisParserPtr->modulePtr->flags &
			            FLAG_SMIV2) {
				printError(thisParserPtr,
					   ERR_MISSING_DESCRIPTION);
			    }
			    $$ = NULL;
			}
	|		DESCRIPTION Text
			{
			    $$ = $2;
			}
	;

trapTypeClause:		LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			TRAP_TYPE
			{
			    if (thisParserPtr->modulePtr->flags &
			            FLAG_SMIV2) {
			        printError(thisParserPtr, ERR_TRAP_TYPE);
			    }
			}
			ENTERPRISE objectIdentifier
			VarPart
			DescrPart
			ReferPart
			COLON_COLON_EQUAL number
			/* TODO: range of number? */
			{ $$ = 0; }
	;

VarPart:		VARIABLES '{' VarTypes '}'
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

VarTypes:		VarType
			{ $$ = 0; }
	|		VarTypes ',' VarType
			{ $$ = 0; }
	;

VarType:		ObjectName
			{ $$ = 0; }
	;

DescrPart:		DESCRIPTION Text
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

MaxAccessPart:		MAX_ACCESS
			{
			    if (!(thisParserPtr->modulePtr->flags &
			              FLAG_SMIV2)) {
			        printError(thisParserPtr, ERR_MAX_ACCESS_IN_SMIV1);
			    }
			}
			Access
			{ $$ = $3; }
	|		ACCESS
			{
			    if (thisParserPtr->modulePtr->flags &
			            FLAG_SMIV2) {
			        printError(thisParserPtr, ERR_ACCESS_IN_SMIV2);
			    }
			}
			Access
			/* TODO: limited values in v1 */
			{ $$ = $3; }
	;

notificationTypeClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			NOTIFICATION_TYPE
			ObjectsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL
			'{' NotificationName '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $12;
				
			    if (objectPtr->modulePtr != thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONTYPE);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectList(objectPtr, $4);
			    setObjectStatus(objectPtr, $6);
			    setObjectDescription(objectPtr, $8);
			    if ($9) {
				setObjectReference(objectPtr, $9);
			    }
			    $$ = 0;
			}
	;

moduleIdentityClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			MODULE_IDENTITY
			{
			    if (thisParserPtr->modulePtr->numModuleIdentities > 0)
			    {
			        printError(thisParserPtr,
					   ERR_TOO_MANY_MODULE_IDENTITIES);
			    }
			    if (thisParserPtr->modulePtr->numStatements > 0) {
			        printError(thisParserPtr,
					   ERR_MODULE_IDENTITY_NOT_FIRST);
			    }
			}
			LAST_UPDATED ExtUTCTime
			{
			    setModuleLastUpdated(thisParserPtr->modulePtr,
						 smiMkTime($6));
			}
			ORGANIZATION Text
			CONTACT_INFO Text
			DESCRIPTION Text
			RevisionPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $17;
			    
			    thisParserPtr->modulePtr->numModuleIdentities++;
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_MODULEIDENTITY);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setModuleIdentityObject(thisParserPtr->modulePtr,
						    objectPtr);
			    setModuleOrganization(thisParserPtr->modulePtr,
						  $9);
			    setModuleContactInfo(thisParserPtr->modulePtr,
						 $11);
			    setObjectDescription(objectPtr, $13);
			    $$ = 0;
			}
        ;

ObjectSyntax:		SimpleSyntax
			{
			    $$ = $1;
			}
	|		typeTag SimpleSyntax
			{
			    if (strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-TC") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "SNMPv2-CONF") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC-1212") &&
				strcmp(thisParserPtr->modulePtr->name,
				       "RFC1155-SMI")) {
			        printError(thisParserPtr, ERR_TYPE_TAG, $1);
			    }
			    $$ = $2;
			}
	|		conceptualTable	     /* TODO: possible? row? entry? */
			{
			    /* TODO */
			    $$ = $1;
			}
	|		row		     /* the uppercase name of a row  */
			{
			    /* TODO */
			    $$ = $1;
			}
	|		entryType	     /* SEQUENCE { ... } phrase */
			{
			    /* TODO */
			    $$ = $1;
			}
	|		ApplicationSyntax
			{
			    /* TODO */
			    $$ = $1;
			}
        ;

typeTag:		'[' APPLICATION number ']' IMPLICIT
			{ $$ = 0; }
	|		'[' UNIVERSAL number ']' IMPLICIT
			{ $$ = 0; }
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceObjectSyntax:	sequenceSimpleSyntax
			{ $$ = $1; }
/*	|		conceptualTable	     /* TODO: possible? row? entry? */
/*	|		row		     /* the uppercase name of a row  */
/*	|		entryType	     /* it's SEQUENCE { ... } phrase */
	|		sequenceApplicationSyntax
			{ $$ = $1; }
        ;

/* TODO: specify really according to ObjectSyntax!!! */
valueofObjectSyntax:	valueofSimpleSyntax
			{ $$ = $1; }
			/* conceptualTables and rows do not have DEFVALs
			 */
			/* valueofApplicationSyntax would not introduce any
			 * further syntax of ObjectSyntax values.
			 */
	;

SimpleSyntax:		INTEGER			/* (-2147483648..2147483647) */
			{
			    $$ = typeSmiIntegerPtr;
			}
	|		INTEGER integerSubType
			{
			    $$ = duplicateType(typeSmiIntegerPtr, 0, thisParserPtr);
			    setTypeParent($$, typeSmiIntegerPtr->name);
			    setTypeList($$, $2);
			}
	|		INTEGER enumSpec
			{
			    $$ = duplicateType(typeSmiIntegerPtr, 0, thisParserPtr);
			    setTypeParent($$, typeSmiIntegerPtr->name);
			    setTypeBasetype($$, SMI_BASETYPE_ENUM);
			    setTypeList($$, $2);
			}
	|		INTEGER32		/* (-2147483648..2147483647) */
			{
			    /* TODO: any need to distinguish from INTEGER? */
			    $$ = typeSmiIntegerPtr;
			}
        |		INTEGER32 integerSubType
			{
			    $$ = duplicateType(typeSmiIntegerPtr, 0, thisParserPtr);
			    setTypeParent($$, typeSmiIntegerPtr->name);
			    setTypeList($$, $2);
			}
	|		UPPERCASE_IDENTIFIER enumSpec
			{
			    Type *parentPtr;
			    SmiType *stypePtr;
			    char s[SMI_MAX_FULLNAME];
			    Import *importPtr;
			    
			    parentPtr = findTypeByModuleAndName(
			        thisParserPtr->modulePtr, $1);
			    if (!parentPtr) {
			        importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    parentPtr = addType($1,
							SMI_BASETYPE_UNKNOWN,
						        FLAG_INCOMPLETE,
						        thisParserPtr);
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				    sprintf(s, "%s::%s",
					    thisParserPtr->modulePtr->name,
					    parentPtr->name);
				    setTypeParent($$, s);
				} else {
				    /*
				     * imported type.
				     */
				    stypePtr = smiGetType(
						  importPtr->importmodule, $1);
				    $$ = addType(NULL, stypePtr->basetype, 0,
						 thisParserPtr);
				    sprintf(s, "%s::%s",
					    importPtr->importmodule,
					    importPtr->importname);
				    setTypeParent($$, s);
				}
			    } else {
			        $$ = duplicateType(parentPtr, 0, thisParserPtr);
				sprintf(s, "%s::%s",
				        thisParserPtr->modulePtr->name, $1);
				setTypeParent($$, s);
			    }
			    setTypeBasetype($$, SMI_BASETYPE_ENUM);
			    setTypeList($$, $2);
			}
	|		moduleName '.' UPPERCASE_IDENTIFIER enumSpec
			/* TODO: UPPERCASE_IDENTIFIER must be an INTEGER */
			{
			    Type *parentPtr;
			    SmiType *stypePtr;
			    char s[SMI_MAX_FULLNAME];
			    Import *importPtr;
			    
			    parentPtr = findTypeByModulenameAndName($1, $3);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName($1,
							  $3, thisModulePtr);
				if (!importPtr) {
				    printError(thisParserPtr,
					       ERR_UNKNOWN_TYPE, $3);
				    $$ = NULL;
				} else {
				    /*
				     * imported type.
				     */
				    stypePtr = smiGetType($1, $3);
				    /* TODO: success? */
				    $$ = addType(NULL, stypePtr->basetype, 0,
						 thisParserPtr);
				    sprintf(s, "%s::%s",
					    importPtr->importmodule,
					    importPtr->importname);
				    setTypeParent($$, s);
				}
			    } else {
			        $$ = duplicateType(parentPtr, 0, thisParserPtr);
				sprintf(s, "%s::%s", $1, $3);
				setTypeParent($$, s);
			    }
			    setTypeBasetype($$, SMI_BASETYPE_ENUM);
			    setTypeList($$, $4);
			}
	|		UPPERCASE_IDENTIFIER integerSubType
			{
			    Type *parentPtr;
			    SmiType *stypePtr;
			    char s[SMI_MAX_FULLNAME];
			    Import *importPtr;
			    
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (!parentPtr) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    parentPtr = addType($1,
							SMI_BASETYPE_UNKNOWN,
							FLAG_INCOMPLETE,
							thisParserPtr);
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				    sprintf(s, "%s::%s",
					    thisParserPtr->modulePtr->name,
					    parentPtr->name);
				    setTypeParent($$, s);
				} else {
				    /*
				     * imported type.
				     */
				    stypePtr = smiGetType(
						  importPtr->importmodule, $1);
				    $$ = addType(NULL, stypePtr->basetype, 0,
						 thisParserPtr);
				    sprintf(s, "%s::%s",
					    importPtr->importmodule,
					    importPtr->importname);
				    setTypeParent($$, s);
				}
			    } else {
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
				sprintf(s, "%s::%s",
				        thisParserPtr->modulePtr->name, $1);
				setTypeParent($$, s);
			    }
			    setTypeList($$, $2);
			}
	|		moduleName '.' UPPERCASE_IDENTIFIER integerSubType
			/* TODO: UPPERCASE_IDENTIFIER must be an INT/Int32. */
			{
			    Type *parentPtr;
			    SmiType *stypePtr;
			    char s[SMI_MAX_FULLNAME];
			    Import *importPtr;
			    
			    parentPtr = findTypeByModulenameAndName($1, $3);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName($1,
							  $3, thisModulePtr);
				if (!importPtr) {
				    printError(thisParserPtr,
					       ERR_UNKNOWN_TYPE, $3);
				    $$ = NULL;
				} else {
				    /*
				     * imported type.
				     */
				    stypePtr = smiGetType($1, $3);
				    /* TODO: success? */
				    $$ = addType(NULL, stypePtr->basetype, 0,
						 thisParserPtr);
				    sprintf(s, "%s::%s",
					    importPtr->importmodule,
					    importPtr->importname);
				    setTypeParent($$, s);
				}
			    } else {
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
				sprintf(s, "%s::%s", $1, $3);
				setTypeParent($$, s);
			    }
			    setTypeList($$, $4);
			}
	|		OCTET STRING		/* (SIZE (0..65535))	     */
			{
			    $$ = typeSmiOctetStringPtr;
			}
	|		OCTET STRING octetStringSubType
			{
			    $$ = duplicateType(typeSmiOctetStringPtr, 0,
					       thisParserPtr);
			    setTypeParent($$, typeSmiOctetStringPtr->name);
			    setTypeList($$, $3);
			}
	|		UPPERCASE_IDENTIFIER octetStringSubType
			{
			    Type *parentPtr;
			    SmiType *stypePtr;
			    char s[SMI_MAX_FULLNAME];
			    Import *importPtr;
			    
			    parentPtr = findTypeByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (!parentPtr) {
				importPtr = findImportByName($1,
							     thisModulePtr);
				if (!importPtr) {
				    /* 
				     * forward referenced type. create it,
				     * marked with FLAG_INCOMPLETE.
				     */
				    parentPtr = addType($1,
						     SMI_BASETYPE_UNKNOWN,
						     FLAG_INCOMPLETE,
						     thisParserPtr);
				    $$ = duplicateType(parentPtr, 0,
						       thisParserPtr);
				    sprintf(s, "%s::%s",
					    thisParserPtr->modulePtr->name,
					    parentPtr->name);
				    setTypeParent($$, s);
				} else {
				    /*
				     * imported type.
				     */
				    stypePtr = smiGetType(
						  importPtr->importmodule, $1);
				    $$ = addType(NULL, stypePtr->basetype, 0,
						 thisParserPtr);
				    sprintf(s, "%s::%s",
					    importPtr->importmodule,
					    importPtr->importname);
				    setTypeParent($$, s);
				}
			    } else {
				$$ = duplicateType(parentPtr, 0,
						   thisParserPtr);
				sprintf(s, "%s::%s",
				        thisParserPtr->modulePtr->name, $1);
				setTypeParent($$, s);
			    }
			    setTypeList($$, $2);
			}
	|		moduleName '.' UPPERCASE_IDENTIFIER octetStringSubType
			/* TODO: UPPERCASE_IDENTIFIER must be an OCTET STR. */
			{
			    Type *parentPtr;
			    SmiType *stypePtr;
			    char s[SMI_MAX_FULLNAME];
			    Import *importPtr;
			    
			    parentPtr = findTypeByModulenameAndName($1, $3);
			    if (!parentPtr) {
				importPtr = findImportByModulenameAndName($1,
							  $3, thisModulePtr);
				if (!importPtr) {
				    printError(thisParserPtr,
					       ERR_UNKNOWN_TYPE, $3);
				    $$ = NULL;
				} else {
				    /*
				     * imported type.
				     */
				    stypePtr = smiGetType($1, $3);
				    /* TODO: success? */
				    $$ = addType(NULL, stypePtr->basetype, 0,
						 thisParserPtr);
				    sprintf(s, "%s::%s",
					    importPtr->importmodule,
					    importPtr->importname);
				    setTypeParent($$, s);
				}
			    } else {
			        $$ = duplicateType(parentPtr, 0, thisParserPtr);
				sprintf(s, "%s::%s", $1, $3);
				setTypeParent($$, s);
			    }
			    setTypeList($$, $4);
			}
	|		OBJECT IDENTIFIER
			{
			    $$ = typeSmiObjectIdentifierPtr;
			}
        ;

valueofSimpleSyntax:	number			/* 0..2147483647 */
			/* NOTE: Counter64 must not have a DEFVAL */
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->value.unsigned32 = $1;
			}
	|		'-' number		/* -2147483648..0 */
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    $$->value.integer32 = - $2;
			}
	|		BIN_STRING		/* number or OCTET STRING */
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				if (strlen($1)) {
				    $$->basetype = SMI_BASETYPE_BINSTRING;
				    $$->value.ptr = util_strdup($1);
				} else {
				    $$->basetype = SMI_BASETYPE_OCTETSTRING;
				    $$->value.ptr = "";
				}
			    } else {
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 2);
			    }
			}
	|		HEX_STRING		/* number or OCTET STRING */
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    if (defaultBasetype == SMI_BASETYPE_OCTETSTRING) {
				if (strlen($1)) {
				    $$->basetype = SMI_BASETYPE_HEXSTRING;
				    $$->value.ptr = util_strdup($1);
				} else {
				    $$->basetype = SMI_BASETYPE_OCTETSTRING;
				    $$->value.ptr = "";
				}
			    } else {
				$$->basetype = SMI_BASETYPE_UNSIGNED32;
				$$->value.unsigned32 = strtoul($1, NULL, 16);
			    }
			}
	|		LOWERCASE_IDENTIFIER	/* enumeration or named oid */
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_LABEL;
			    $$->value.ptr = util_strdup($1);
			}
	|		QUOTED_STRING		/* an OCTET STRING */
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_OCTETSTRING;
			    $$->value.ptr = util_strdup($1);
			}
			/* NOTE: If the value is an OBJECT IDENTIFIER, then
			 *       it must be expressed as a single ASN.1
			 *	 identifier, and not as a collection of
			 *	 of sub-identifiers.
			 *	 REF: draft,p.34
			 *	 Anyway, we try to accept it. But it's only
			 *	 possible for numbered sub-identifiers, since
			 *	 other identifiers would make something like
			 *	 { gaga } indistiguishable from a BitsValue.
			 */
	|		'{' objectIdentifier_defval '}'
			/*
			 * This is only for some MIBs with invalid numerical
			 * OID notation for DEFVALs. We DO NOT parse them
			 * correctly. We just don't want to produce a
			 * parser error.
			 */
			{
			    /* TODO: SMIv1 allows something like { 0 0 } !
			     * SMIv2 does not! Check more carefully!
			     */
			    printError(thisParserPtr, ERR_OID_DEFVAL_TOO_LONG);
			    $$ = util_malloc(sizeof(SmiValue));
			    $$->basetype = SMI_BASETYPE_LABEL;
			    if (thisModulePtr->flags & FLAG_SMIV2) {
				$$->value.oidlen = 2;
				$$->value.oid[0] = 0;
				$$->value.oid[1] = 0;
				/* TODO */
#if 0
				for (importPtr = thisModulePtr->firstImportPtr;
				     importPtr;
				     importPtr = importPtr->nextPtr) {
				    if ((!importPtr->nextPtr) ||
					(!strcmp(importPtr->nextPtr->importmodule,
						 "SNMPv2-SMI"))) {
					break;
				    }
				    if (importPtr) {
					newPtr = util_malloc(sizeof(Import));
					newPtr->importmodule =
					    util_strdup("SNMPv2-SMI");
					newPtr->importname =
					    util_strdup($$->value.oid);
					newPtr->kind = KIND_OBJECT;
					newPtr->nextPtr = importPtr->nextPtr;
					newPtr->prevPtr = importPtr;
					importPtr->nextPtr->prevPtr = newPtr;
					importPtr->nextPtr = newPtr;
				    }
				}
			    } else {
				$$->value.oid = "zeroDotZero";
				objectPtr = addObject("zeroDotZero",
				      findNodeByParentAndSubid(rootNodePtr, 0),
					  0, 0, thisParserPtr);
				
#endif
			    }
			}
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceSimpleSyntax:	INTEGER	anySubType	/* (-2147483648..2147483647) */
			{
			    $$ = typeSmiIntegerPtr;
			}
        |		INTEGER32 anySubType	/* (-2147483648..2147483647) */
			{
			    /* TODO: any need to distinguish from INTEGER? */
			    $$ = typeSmiIntegerPtr;
			}
	|		OCTET STRING anySubType
			{
			    /* TODO: warning: ignoring subtype in SEQUENCE */
			    $$ = typeSmiOctetStringPtr;
			}
	|		OBJECT IDENTIFIER
			{
			    $$ = typeSmiObjectIdentifierPtr;
			}
	;

ApplicationSyntax:	IPADDRESS
			{
			    $$ = findTypeByName("IpAddress");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "IpAddress");
			    }
			}
	|		COUNTER32		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Counter32");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter32");
			    }
			}
	|		GAUGE32			/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Gauge32");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			}
	|		GAUGE32 integerSubType
			{
			    Type *parentPtr;
			    
			    parentPtr = findTypeByName("Gauge32");
			    if (! parentPtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			    $$ = duplicateType(parentPtr, 0, thisParserPtr);
			    setTypeParent($$, parentPtr->name);
			    setTypeList($$, $2);
			}
	|		UNSIGNED32		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Unsigned32");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Unsigned32");
			    }
			}
	|		UNSIGNED32 integerSubType
			{
			    Type *parentPtr;
			    
			    parentPtr = findTypeByName("Unsigned32");
			    if (! parentPtr) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Unsigned32");
			    }
			    $$ = duplicateType(parentPtr, 0, thisParserPtr);
			    setTypeParent($$, parentPtr->name);
			    setTypeList($$, $2);
			}
	|		TIMETICKS		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("TimeTicks");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "TimeTicks");
			    }
			}
	|		OPAQUE			/* IMPLICIT OCTET STRING     */
			{
			    $$ = findTypeByName("Opaque");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Opaque");
			    }
			}
	|		COUNTER64	        /* (0..18446744073709551615) */
			{
			    $$ = findTypeByName("Counter64");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter64");
			    }
			}
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceApplicationSyntax: IPADDRESS
			{
			    $$ = findTypeByName("IpAddress");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "IpAddress");
			    }
			}
	|		COUNTER32		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Counter32");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter32");
			    }
			}
	|		GAUGE32	anySubType	/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Gauge32");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Gauge32");
			    }
			}
	|		UNSIGNED32 anySubType /* (0..4294967295)	     */
			{
			    $$ = findTypeByName("Unsigned32");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Unsigned32");
			    }
			}
	|		TIMETICKS		/* (0..4294967295)	     */
			{
			    $$ = findTypeByName("TimeTicks");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "TimeTicks");
			    }
			}
	|		OPAQUE			/* IMPLICIT OCTET STRING     */
			{
			    $$ = findTypeByName("Opaque");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Opaque");
			    }
			}
	|		COUNTER64	        /* (0..18446744073709551615) */
			{
			    $$ = findTypeByName("Counter64");
			    if (! $$) {
				printError(thisParserPtr, ERR_UNKNOWN_TYPE,
					   "Counter64");
			    }
			}
	;

anySubType:		integerSubType
			{
			    /* TODO: warning: ignoring SubType */
			    $$ = NULL;
			}
	|	        octetStringSubType
			{
			    /* TODO: warning: ignoring SubType */
			    $$ = NULL;
			}
	|		enumSpec
			{
			    /* TODO: warning: ignoring SubType */
			    $$ = NULL;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
        ;		      


/* REF: draft,p.46 */
integerSubType:		'(' ranges ')'		/* at least one range        */
			/*
			 * the specification mentions an alternative of an
			 * empty RHS here. this would lead to reduce/reduce
			 * conflicts. instead, we differentiate the parent
			 * rule(s) (SimpleSyntax).
			 */
			{ $$ = $2; }
	;

octetStringSubType:	'(' SIZE '(' ranges ')' ')'
			/*
			 * the specification mentions an alternative of an
			 * empty RHS here. this would lead to reduce/reduce
			 * conflicts. instead, we differentiate the parent
			 * rule(s) (SimpleSyntax).
			 */
			{ $$ = $4; }
	;

ranges:			range
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		ranges '|' range
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

range:			value
			{
			    $$ = util_malloc(sizeof(Range));
			    /* TODO: success? */
			    $$->minValuePtr = $1;
			    $$->maxValuePtr = $1;
			}
	|		value DOT_DOT value
			{
			    $$ = util_malloc(sizeof(Range));
			    /* TODO: success? */
			    $$->minValuePtr = $1;
			    $$->maxValuePtr = $3;
			}
	;

value:			'-' number
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    /* TODO: range check */
			    $$->value.integer32 = - $2;
			}
	|		number
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->value.unsigned32 = $1;
			}
	|		HEX_STRING
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->value.unsigned32 = strtoul($1, NULL, 16);
			}
	|		BIN_STRING
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_UNSIGNED32;
			    $$->value.unsigned32 = strtoul($1, NULL, 2);
			}
	;

enumSpec:		'{' enumItems '}'
			{
			    $$ = $2;
			}
	;

enumItems:		enumItem
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		enumItems ',' enumItem
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = (void *)$3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

enumItem:		LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_ENUMNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_ENUMNAME_32, $1);
			    }
			}
			'(' enumNumber ')'
			{
			    $$ = util_malloc(sizeof(NamedNumber));
			    /* TODO: success? */
			    $$->name = util_strdup($1);
			    $$->valuePtr = $4;
			}
	;

enumNumber:		number
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    /* TODO: range check */
			    $$->value.integer32 = $1;
			}
	|		'-' number
			{
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_INTEGER32;
			    /* TODO: range check */
			    $$->value.integer32 = - $2;
			    /* TODO: non-negative is suggested */
			}
	;

Status:			LOWERCASE_IDENTIFIER
			{
			    if (thisParserPtr->modulePtr->flags & FLAG_SMIV2) {
				if (!strcmp($1, "current")) {
				    $$ = SMI_STATUS_CURRENT;
				} else if (!strcmp($1, "deprecated")) {
				    $$ = SMI_STATUS_DEPRECATED;
				} else if (!strcmp($1, "obsolete")) {
				    $$ = SMI_STATUS_OBSOLETE;
				} else {
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV2_STATUS, $1);
				    $$ = SMI_STATUS_UNKNOWN;
				}
			    } else {
				if (!strcmp($1, "mandatory")) {
				    $$ = SMI_STATUS_MANDATORY;
				} else if (!strcmp($1, "optional")) {
				    $$ = SMI_STATUS_OPTIONAL;
				} else if (!strcmp($1, "obsolete")) {
				    $$ = SMI_STATUS_OBSOLETE;
				} else if (!strcmp($1, "deprecated")) {
				    $$ = SMI_STATUS_OBSOLETE;
				} else {
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV1_STATUS, $1);
				    $$ = SMI_STATUS_UNKNOWN;
				}
			    }
			}
        ;		

Status_Capabilities:	LOWERCASE_IDENTIFIER
			{
			    if (!strcmp($1, "current")) {
				$$ = SMI_STATUS_CURRENT;
			    } else if (!strcmp($1, "obsolete")) {
				$$ = SMI_STATUS_OBSOLETE;
			    } else {
				printError(thisParserPtr,
					   ERR_INVALID_CAPABILITIES_STATUS,
					   $1);
				$$ = SMI_STATUS_UNKNOWN;
			    }
			}
        ;

DisplayPart:		DISPLAY_HINT Text
			{
			    $$ = $2;
			}
        |		/* empty */
			{
			    $$ = NULL;
			}
        ;

UnitsPart:		UNITS Text
			{
			    $$ = $2;
			}
        |		/* empty */
			{
			    $$ = NULL;
			}
        ;

Access:			LOWERCASE_IDENTIFIER
			{
			    if (thisParserPtr->modulePtr->flags & FLAG_SMIV2) {
				if (!strcmp($1, "not-accessible")) {
				    $$ = SMI_ACCESS_NOT_ACCESSIBLE;
				} else if (!strcmp($1,
						   "accessible-for-notify")) {
				    $$ = SMI_ACCESS_NOTIFY;
				} else if (!strcmp($1, "read-only")) {
				    $$ = SMI_ACCESS_READ_ONLY;
				} else if (!strcmp($1, "read-write")) {
				    $$ = SMI_ACCESS_READ_WRITE;
				} else if (!strcmp($1, "read-create")) {
				    $$ = SMI_ACCESS_READ_CREATE;
				} else {
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV2_ACCESS,
					       $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				}
			    } else {
				if (!strcmp($1, "not-accessible")) {
				    $$ = SMI_ACCESS_NOT_ACCESSIBLE;
				} else if (!strcmp($1, "read-only")) {
				    $$ = SMI_ACCESS_READ_ONLY;
				} else if (!strcmp($1, "read-write")) {
				    $$ = SMI_ACCESS_READ_WRITE;
				} else if (!strcmp($1, "write-only")) {
				    $$ = SMI_ACCESS_WRITE_ONLY;
				} else {
				    printError(thisParserPtr,
					       ERR_INVALID_SMIV1_ACCESS, $1);
				    $$ = SMI_ACCESS_UNKNOWN;
				}
			    }
			}
        ;

IndexPart:		INDEX
			{
			    /*
			     * Use a global variable to fetch and remember
			     * whether we have seen an IMPLIED keyword.
			     */
			    impliedFlag = 0;
			}
			'{' IndexTypes '}'
			{
			    Index *p;
			    
			    p = util_malloc(sizeof(Index));
			    /* TODO: success? */
			    p->indexkind = SMI_INDEX_INDEX;
			    p->implied   = impliedFlag;
			    p->listPtr   = $4;
			    p->rowPtr    = NULL;
			    $$ = p;
			}
        |		AUGMENTS '{' Entry '}'
			/* TODO: no AUGMENTS clause in v1 */
			/* TODO: how to differ INDEX and AUGMENTS ? */
			{
			    Index *p;
			    
			    p = util_malloc(sizeof(Index));
			    /* TODO: success? */
			    p->indexkind    = SMI_INDEX_AUGMENT;
			    p->implied      = 0;
			    p->listPtr      = NULL;
			    p->rowPtr       = $3;
			    $$ = p;
			}
        |		/* empty */
			{
			    $$ = NULL;
			}
	;

IndexTypes:		IndexType
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
        |		IndexTypes ',' IndexType
			/* TODO: might this list be emtpy? */
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

IndexType:		IMPLIED Index
			{
			    impliedFlag = 1;
			    $$ = $2;
			}
	|		Index
			{
			    $$ = $1;
			}
	;

Index:			ObjectName
			/*
			 * TODO: use the SYNTAX value of the correspondent
			 *       OBJECT-TYPE invocation
			 */
			{
			    $$ = $1;
			}
        ;

Entry:			ObjectName
			{
			    $$ = $1;
			}
        ;

DefValPart:		DEFVAL '{' Value '}'
			{ $$ = $3; }
	|		/* empty */
			{ $$ = NULL; }
			/* TODO: different for DefValPart in AgentCaps ? */
	;

Value:			valueofObjectSyntax
			{ $$ = $1; }
	|		'{' BitsValue '}'
			{
			    int i = 0;
			    List *listPtr;
			    
			    $$ = util_malloc(sizeof(SmiValue));
			    /* TODO: success? */
			    $$->basetype = SMI_BASETYPE_BITS;
			    $$->value.bits = NULL;
			    for (i = 0, listPtr = $2; listPtr;
				 i++, listPtr = listPtr->nextPtr) {
				$$->value.bits = util_realloc($$->value.bits,
						       sizeof(char *) * (i+2));
				$$->value.bits[i] = listPtr->ptr;
				$$->value.bits[i+1] = NULL;
			    }
			}
	;

BitsValue:		BitNames
			{ $$ = $1; }
	|		/* empty */
			{ $$ = NULL; }
	;

BitNames:		BitName
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		BitNames ',' BitName
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

BitName:		identifier
			{ $$ = $1; }
	;

ObjectName:		objectIdentifier
			{
			    $$ = $1;
			}
	;

NotificationName:	objectIdentifier
			{
			    $$ = $1;
			}
	;

ReferPart:		REFERENCE Text
			{ $$ = $2; }
	|		/* empty */
			{ $$ = NULL; }
	;

RevisionPart:		Revisions
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Revisions:		Revision
			{ $$ = 0; }
	|		Revisions Revision
			{ $$ = 0; }
	;

Revision:		REVISION ExtUTCTime
			DESCRIPTION Text
			{
			    time_t date;

			    date = smiMkTime($2);

			    /*
			     * If the first REVISION (which is the newest)
			     * has another date than the LAST-UPDATED clause,
			     * we add an implicit Revision structure.
			     */
			    if ((!thisModulePtr->firstRevisionPtr) &&
				(date != thisModulePtr->lastUpdated)) {
				addRevision(thisModulePtr->lastUpdated,
	            "[Revision added by libsmi due to a LAST-UPDATED clause.]",
					    thisParserPtr);
			    }
			    
			    if (addRevision(date, $4, thisParserPtr))
				$$ = 0;
			    else
				$$ = -1;
			}
	;

ObjectsPart:		OBJECTS '{' Objects '}'
			{
			    $$ = $3;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

Objects:		Object
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		Objects ',' Object
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

Object:			ObjectName
			{
			    $$ = $1;
			}
	;

NotificationsPart:	NOTIFICATIONS '{' Notifications '}'
			{
			    $$ = $3;
			}
	;

Notifications:		Notification
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		Notifications ',' Notification
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

Notification:		NotificationName
			{
			    $$ = $1;
			}
	;

Text:			QUOTED_STRING
			{
			    $$ = util_strdup($1);
			}
	;

/*
 * TODO: REF: 
 */
ExtUTCTime:		QUOTED_STRING
			{
			    /* TODO: check length and format */
			    $$ = util_strdup($1);
			}
	;

objectIdentifier:	{
			    parentNodePtr = rootNodePtr;
			}
			subidentifiers
			{
			    $$ = $2;
			    parentNodePtr = $2->nodePtr;
			}
	;

subidentifiers:
			subidentifier
			{
			    $$ = $1;
			}
	|		subidentifiers
			subidentifier
			{
			    $$ = $2;
			}
        ;

subidentifier:
			LOWERCASE_IDENTIFIER
			{
			    Object *objectPtr;
			    SmiNode *snodePtr;
			    Import *importPtr;
			    
			    if (parentNodePtr != rootNodePtr) {
				printError(thisParserPtr,
					   ERR_OIDLABEL_NOT_FIRST, $1);
			    } else {
				objectPtr = findObjectByModuleAndName(
				    thisParserPtr->modulePtr, $1);
				if (objectPtr) {
				    $$ = objectPtr;
				} else {
				    importPtr = findImportByName($1,
							       thisModulePtr);
				    if (!importPtr) {
					/*
					 * If we are in a MODULE-COMPLIANCE
					 * statement with a given MODULE...
					 */
					if (complianceModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    complianceModulePtr, $1);
					    if (objectPtr) {
						addImport($1, thisParserPtr);
					    }
					} else {
					    /* 
					     * forward referenced node.
					     * create it,
					     * marked with FLAG_INCOMPLETE.
					     */
					    objectPtr = addObject($1,
							      pendingNodePtr,
							      0,
							      FLAG_INCOMPLETE,
							      thisParserPtr);
					    setObjectFileOffset(objectPtr,
						     thisParserPtr->character);
					}
					$$ = objectPtr;
				    } else {
					/*
					 * imported object.
					 */
					snodePtr = smiGetNode(
						  importPtr->importmodule, $1);
					if (snodePtr) {
					    $$ = addObject($1, 
							   getParentNode(
					          createNodes(snodePtr->oidlen,
						 	      snodePtr->oid)),
					    snodePtr->oid[snodePtr->oidlen-1],
							   FLAG_IMPORTED,
							   thisParserPtr);
					} else {
					    $$ = addObject($1, pendingNodePtr,
							   0,
					       FLAG_IMPORTED | FLAG_INCOMPLETE,
							   thisParserPtr);
					}
				    }
				}
				parentNodePtr = $$->nodePtr;
			    }
			}
	|		moduleName '.' LOWERCASE_IDENTIFIER
			{
			    Object *objectPtr;
			    SmiNode *snodePtr;
			    Import *importPtr;
			    
			    if (parentNodePtr != rootNodePtr) {
				printError(thisParserPtr,
					   ERR_OIDLABEL_NOT_FIRST, $1);
			    } else {
				objectPtr = findObjectByModulenameAndName(
				    $1, $3);
				if (objectPtr) {
				    $$ = objectPtr;
				} else {
				    importPtr = findImportByModulenameAndName(
					$1, $3, thisModulePtr);
				    if (!importPtr) {
					/* TODO: check: $1 == thisModule ? */
					/*
					 * If we are in a MODULE-COMPLIANCE
					 * statement with a given MODULE...
					 */
					if (complianceModulePtr) {
					    objectPtr =
						findObjectByModuleAndName(
						    complianceModulePtr, $1);
					    if (objectPtr) {
						addImport($1, thisParserPtr);
					    }
					} else {
					    /* 
					     * forward referenced node.
					     * create it,
					     * marked with FLAG_INCOMPLETE.
					     */
					    objectPtr = addObject($1,
							    pendingNodePtr,
							      0,
							      FLAG_INCOMPLETE,
							      thisParserPtr);
					    setObjectFileOffset(objectPtr,
						     thisParserPtr->character);
					}
					$$ = objectPtr;
				    } else {
					/*
					 * imported object.
					 */
					snodePtr = smiGetNode($1, $3);
					$$ = addObject($3, 
					  getParentNode(
					      createNodes(snodePtr->oidlen,
						          snodePtr->oid)),
					  snodePtr->oid[snodePtr->oidlen-1],
					  FLAG_IMPORTED,
					  thisParserPtr);
				    }
				}
				parentNodePtr = $$->nodePtr;
			    }
			}
	|		number
			{
			    Node *nodePtr;
			    Object *objectPtr;

			    nodePtr = findNodeByParentAndSubid(parentNodePtr,
							       $1);
			    if (nodePtr && nodePtr->lastObjectPtr) {
				/*
				 * hopefully, the last defined Object for
				 * this Node is the one we expect.
				 */
				$$ = nodePtr->lastObjectPtr;
			    } else {
				objectPtr = addObject("",
						      parentNodePtr,
						      $1,
						      FLAG_INCOMPLETE,
						      thisParserPtr);
				$$ = objectPtr;
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
			    }
			    parentNodePtr = $$->nodePtr;
			}
	|		LOWERCASE_IDENTIFIER '(' number ')'
			{
			    Object *objectPtr;
			    
			    /* TODO: search in local module and
			     *       in imported modules
			     */
			    objectPtr = findObjectByModuleAndName(
				thisParserPtr->modulePtr, $1);
			    if (objectPtr) {
				printError(thisParserPtr,
					   ERR_EXISTENT_OBJECT, $1);
				$$ = objectPtr;
				if ($$->nodePtr->subid != $3) {
				    printError(thisParserPtr,
					       ERR_SUBIDENTIFIER_VS_OIDLABEL,
					       $3, $1);
				}
			    } else {
				objectPtr = addObject($1, parentNodePtr,
						      $3, 0,
						      thisParserPtr);
				setObjectDecl(objectPtr,
					      SMI_DECL_VALUEASSIGNMENT);
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
				$$ = objectPtr;
			    }
			    
			    parentNodePtr = $$->nodePtr;
			}
	|		moduleName '.' LOWERCASE_IDENTIFIER '(' number ')'
			{
			    Object *objectPtr;
			    char *md;

			    md = util_malloc(sizeof(char) *
					     (strlen($1) + strlen($3) + 2));
			    sprintf(md, "%s.%s", $1, $3);
			    objectPtr = findObjectByModulenameAndName($1, $3);
			    if (objectPtr) {
				printError(thisParserPtr, ERR_EXISTENT_OBJECT,
					   $1);
				$$ = objectPtr;
				if ($$->nodePtr->subid != $5) {
				    printError(thisParserPtr,
					       ERR_SUBIDENTIFIER_VS_OIDLABEL,
					       $5, md);
				}
			    } else {
				printError(thisParserPtr,
					   ERR_ILLEGALLY_QUALIFIED, md);
				objectPtr = addObject($3, parentNodePtr,
						   $5, 0,
						   thisParserPtr);
				setObjectFileOffset(objectPtr,
						    thisParserPtr->character);
				$$ = objectPtr;
			    }
			    util_free(md);
			    parentNodePtr = $$->nodePtr;
			}
	;

objectIdentifier_defval: subidentifiers_defval
			{ $$ = NULL; }
        ;		/* TODO: right? */

subidentifiers_defval:	subidentifier_defval
			{ $$ = 0; }
	|		subidentifiers_defval subidentifier_defval
			{ $$ = 0; }
        ;		/* TODO: right? */

subidentifier_defval:	LOWERCASE_IDENTIFIER '(' number ')'
			{ $$ = 0; }
	|		number
			{ $$ = 0; }
	;		/* TODO: right? range? */

objectGroupClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			OBJECT_GROUP
			ObjectsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $12;

			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr, SMI_DECL_OBJECTGROUP);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $6);
			    setObjectDescription(objectPtr, $8);
			    if ($9) {
				setObjectReference(objectPtr, $9);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, $4);
			    $$ = 0;
			}
	;

notificationGroupClause: LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			NOTIFICATION_GROUP
			NotificationsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $12;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    objectPtr = setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_NOTIFICATIONGROUP);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $6);
			    setObjectDescription(objectPtr, $8);
			    if ($9) {
				setObjectReference(objectPtr, $9);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, $4);
			    $$ = 0;
			}
	;

moduleComplianceClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			MODULE_COMPLIANCE
			STATUS Status
			DESCRIPTION Text
			ReferPart
			ComplianceModulePart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    Refinement *refinementPtr;
			    List *listPtr;
			    char s[SMI_MAX_DESCRIPTOR * 2 + 15];
			    
			    objectPtr = $12;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    setObjectName(objectPtr, $1);
			    setObjectDecl(objectPtr,
					  SMI_DECL_MODULECOMPLIANCE);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $5);
			    setObjectDescription(objectPtr, $7);
			    if ($8) {
				setObjectReference(objectPtr, $8);
			    }
			    setObjectAccess(objectPtr,
					    SMI_ACCESS_NOT_ACCESSIBLE);
			    setObjectList(objectPtr, $9.mandatorylistPtr);
			    objectPtr->optionlistPtr = $9.optionlistPtr;
			    objectPtr->refinementlistPtr =
				                          $9.refinementlistPtr;

			    /*
			     * Dirty: Fake the types' names in the
			     * refinement list:
			     * ``<compliancename>+<objecttypename>+type''
			     * ``<compliancename>+<objecttypename>+writetype''
			     */
			    if ($9.refinementlistPtr) {
				for (listPtr = $9.refinementlistPtr;
				     listPtr;
				     listPtr = listPtr->nextPtr) {
				    refinementPtr =
					((Refinement *)(listPtr->ptr));
				    if (refinementPtr->typePtr) {
					sprintf(s, "%s+%s+type", $1,
					       refinementPtr->objectPtr->name);
					setTypeName(refinementPtr->typePtr, s);
				    }
				    if (refinementPtr->writetypePtr) {
					sprintf(s, "%s+%s+writetype", $1,
					       refinementPtr->objectPtr->name);
				   setTypeName(refinementPtr->writetypePtr, s);
				    }
				}
			    }
			    $$ = 0;
			}
	;

ComplianceModulePart:	ComplianceModules
			{
			    $$ = $1;
			}
	;

ComplianceModules:	ComplianceModule
			{
			    $$ = $1;
			}
	|		ComplianceModules ComplianceModule
			{
			    List *listPtr;
			    
			    /* concatenate lists in $1 and $2 */
			    if ($1.mandatorylistPtr) {
				for (listPtr = $1.mandatorylistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.mandatorylistPtr;
				$$.mandatorylistPtr = $1.mandatorylistPtr;
			    } else {
				$$.mandatorylistPtr = $2.mandatorylistPtr;
			    }
			    if ($1.optionlistPtr) {
				for (listPtr = $1.optionlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.optionlistPtr;
				$$.optionlistPtr = $1.optionlistPtr;
			    } else {
				$$.optionlistPtr = $2.optionlistPtr;
			    }
			    if ($1.refinementlistPtr) {
				for (listPtr = $1.refinementlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.refinementlistPtr;
				$$.refinementlistPtr = $1.refinementlistPtr;
			    } else {
				$$.refinementlistPtr = $2.refinementlistPtr;
			    }
			}
	;

ComplianceModule:	MODULE ComplianceModuleName
			{
			    /*
			     * Remember the module. SMIv2 is broken by
			     * design to allow subsequent clauses to
			     * refer identifiers that are not
			     * imported.  Although, SMIv2 does not
			     * require, we will fake it by inserting
			     * appropriate imports.
			     */
			    if ($2 == thisModulePtr)
				complianceModulePtr = NULL;
			    else
				complianceModulePtr = $2;
			}
			MandatoryPart
			CompliancePart
			{
			    $$.mandatorylistPtr = $4;
			    $$.optionlistPtr = $5.optionlistPtr;
			    $$.refinementlistPtr = $5.refinementlistPtr;
			    if (complianceModulePtr) {
				checkImports(complianceModulePtr->name,
					     thisParserPtr);
				complianceModulePtr = NULL;
			    }
			}
	;

ComplianceModuleName:	UPPERCASE_IDENTIFIER objectIdentifier
			{
			    $$ = findModuleByName($1);
			    /* TODO: handle objectIdentifier */
			    if (!$$) {
				smiLoadModule($1);
				$$ = findModuleByName($1);
			    }
			}
	|		UPPERCASE_IDENTIFIER
			{
			    $$ = findModuleByName($1);
			    if (!$$) {
				smiLoadModule($1);
				$$ = findModuleByName($1);
			    }
			}
	|		/* empty, only if contained in MIB module */
			/* TODO: RFC 1904 looks a bit different, is this ok? */
			{
			    $$ = thisModulePtr;
			}
	;

MandatoryPart:		MANDATORY_GROUPS '{' MandatoryGroups '}'
			{
			    $$ = $3;
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

MandatoryGroups:	MandatoryGroup
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		MandatoryGroups ',' MandatoryGroup
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

MandatoryGroup:		objectIdentifier
			{
			    /*
			     * The object found may be an implicitly
			     * created object with flags & FLAG_IMPORTED.
			     * We want to get the original definition:
			     */
			    $$ = $1;
			    if (complianceModulePtr) {
				$$ = findObjectByModuleAndName(
				                           complianceModulePtr,
							   $1->name);
			    }
			}
	;

CompliancePart:		Compliances
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = $1.optionlistPtr;
			    $$.refinementlistPtr = $1.refinementlistPtr;
			}
	|		/* empty */
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = NULL;
			    $$.refinementlistPtr = NULL;
			}
	;

Compliances:		Compliance
			{
			    $$ = $1;
			}
	|		Compliances Compliance
			{
			    List *listPtr;
			    
			    $1.mandatorylistPtr = NULL;

			    /* concatenate lists in $1 and $2 */
			    if ($1.optionlistPtr) {
				for (listPtr = $1.optionlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.optionlistPtr;
				$$.optionlistPtr = $1.optionlistPtr;
			    } else {
				$$.optionlistPtr = $2.optionlistPtr;
			    }
			    if ($1.refinementlistPtr) {
				for (listPtr = $1.refinementlistPtr;
				     listPtr->nextPtr;
				     listPtr = listPtr->nextPtr);
				listPtr->nextPtr = $2.refinementlistPtr;
				$$.refinementlistPtr = $1.refinementlistPtr;
			    } else {
				$$.refinementlistPtr = $2.refinementlistPtr;
			    }
			}
	;

Compliance:		ComplianceGroup
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = $1;
			    $$.refinementlistPtr = NULL;
			}
	|		ComplianceObject
			{
			    $$.mandatorylistPtr = NULL;
			    $$.optionlistPtr = NULL;
			    $$.refinementlistPtr = $1;
			}
	;

ComplianceGroup:	GROUP objectIdentifier
			DESCRIPTION Text
			{
			    $$ = util_malloc(sizeof(List));
			    $$->nextPtr = NULL;
			    $$->ptr = util_malloc(sizeof(Option));
			    ((Option *)($$->ptr))->objectPtr = $2;
			    ((Option *)($$->ptr))->description =
				                               util_strdup($4);
			}
	;

ComplianceObject:	OBJECT ObjectName
			SyntaxPart
			WriteSyntaxPart
			AccessPart
			DESCRIPTION Text
			{
			    $$ = util_malloc(sizeof(List));
			    $$->nextPtr = NULL;
			    $$->ptr = util_malloc(sizeof(Refinement));
			    ((Refinement *)($$->ptr))->objectPtr = $2;
			    ((Refinement *)($$->ptr))->typePtr = $3;
			    ((Refinement *)($$->ptr))->writetypePtr = $4;
			    ((Refinement *)($$->ptr))->access = $5;
			    ((Refinement *)($$->ptr))->description =
				                               util_strdup($7);
			}
	;

SyntaxPart:		SYNTAX Syntax
			{
			    if ($2->name) {
				$$ = duplicateType($2, 0, thisParserPtr);
			    } else {
				$$ = $2;
			    }
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

WriteSyntaxPart:	WRITE_SYNTAX WriteSyntax
			{
			    if ($2->name) {
				$$ = duplicateType($2, 0, thisParserPtr);
			    } else {
				$$ = $2;
			    }
			}
	|		/* empty */
			{
			    $$ = NULL;
			}
	;

WriteSyntax:		Syntax
			{
			    $$ = $1;
			}
	;

AccessPart:		MIN_ACCESS Access
			{
			    $$ = $2;
			}
	|		/* empty */
			{
			    $$ = SMI_ACCESS_UNKNOWN;
			}
	;

agentCapabilitiesClause: LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(thisParserPtr, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(thisParserPtr, ERR_OIDNAME_32, $1);
			    }
			}
			AGENT_CAPABILITIES
			PRODUCT_RELEASE Text
			STATUS Status_Capabilities
			DESCRIPTION Text
			ReferPart
			ModulePart_Capabilities
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Object *objectPtr;
			    
			    objectPtr = $14;
			    
			    if (objectPtr->modulePtr !=
				thisParserPtr->modulePtr) {
				objectPtr = duplicateObject(objectPtr, 0,
							    thisParserPtr);
			    }
			    setObjectDecl(objectPtr,
					  SMI_DECL_AGENTCAPABILITIES);
			    addObjectFlags(objectPtr, FLAG_REGISTERED);
			    deleteObjectFlags(objectPtr, FLAG_INCOMPLETE);
			    setObjectStatus(objectPtr, $7);
			    setObjectDescription(objectPtr, $9);
			    if ($10) {
				setObjectReference(objectPtr, $10);
			    }
				/*
				 * TODO: PRODUCT_RELEASE Text ($5)
				 * TODO: ModulePart_Capabilities ($11)
				 */
			    $$ = 0;
			}
	;

ModulePart_Capabilities: Modules_Capabilities
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Modules_Capabilities:	Module_Capabilities
			{ $$ = 0; }
	|		Modules_Capabilities Module_Capabilities
			{ $$ = 0; }
	;

Module_Capabilities:	SUPPORTS ModuleName_Capabilities
			/* TODO: example in the SimpleBook names the module
			 * while the given syntax provides oids ?! */
			INCLUDES '{' CapabilitiesGroups '}'
			VariationPart
			{ $$ = 0; }
	;

CapabilitiesGroups:	CapabilitiesGroup
			{
			    $$ = util_malloc(sizeof(List));
			    /* TODO: success? */
			    $$->ptr = $1;
			    $$->nextPtr = NULL;
			}
	|		CapabilitiesGroups ',' CapabilitiesGroup
			{
			    List *p, *pp;
			    
			    p = util_malloc(sizeof(List));
			    /* TODO: success? */
			    p->ptr = $3;
			    p->nextPtr = NULL;
			    for (pp = $1; pp->nextPtr; pp = pp->nextPtr);
			    pp->nextPtr = p;
			    $$ = $1;
			}
	;

CapabilitiesGroup:	objectIdentifier
			{
			    $$ = NULL;
			}
	;

ModuleName_Capabilities: objectIdentifier
			{ $$ = 0; }
	|		UPPERCASE_IDENTIFIER
			{ $$ = 0; }
	|		/* empty */
			/* empty, only if contained in MIB module */
			/* TODO: ?? */
			{ $$ = 0; }
	;

VariationPart:		Variations
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Variations:		Variation
			{ $$ = 0; }
	|		Variations Variation
			{ $$ = 0; }
	;

Variation:		ObjectVariation
			{ $$ = 0; }
	|		NotificationVariation
			{ $$ = 0; }
	;

NotificationVariation:	VARIATION ObjectName
			AccessPart
			DESCRIPTION Text
			{ $$ = 0; }
	;

ObjectVariation:	VARIATION ObjectName
			SyntaxPart
			WriteSyntaxPart
			AccessPart
			CreationPart
			DefValPart
			DESCRIPTION Text
			{ $$ = 0; }
	;

CreationPart:		CREATION_REQUIRES '{' Cells '}'
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Cells:			Cell
			{ $$ = 0; }
	|		Cells ',' Cell
			{ $$ = 0; }
	;

Cell:			ObjectName
			{ $$ = 0; }
	;

number:			NUMBER
			{
			    unsigned long ul;
			    
			    ul = strtoul($1, NULL, 10);
			    /* TODO: success? */
			    
			    $$ = ul;
			    
			    /* TODO */
			}
	;

%%

#endif
