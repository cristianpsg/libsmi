/*
 * parser.y --
 *
 *      Syntax rules for parsing the MIB module language.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: parser.y,v 1.58 1998/10/08 18:18:29 strauss Exp $
 */

%{

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
    
#ifdef linux
#include <getopt.h>
#endif

#include "defs.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"
#include "data.h"



/*
 * These arguments are passed to yyparse() and yylex().
 */
#define YYPARSE_PARAM parser
#define YYLEX_PARAM   parser

    
    
#define thisParser      ((Parser *)parser)
#define thisModule     (((Parser *)parser)->thisModule)


    
/*
 * NOTE: The argument lvalp ist not really a void pointer. Unfortunately,
 * we don't know it better at this point. bison generated C code declares
 * YYSTYPE just a few lines below from the `%union' declaration.
 */
extern int yylex(void *lvalp, Parser *parser);



Module dummyModule;
MibNode *parent;

 
 
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
    String text;				/* scanned quoted text       */
    char id[201];				/* identifier name           */
    int err;					/* actually just a dummy     */
    MibNode *mibnode;
    Status status;
    Access access;
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
%type  <err>choiceClause
%type  <id>typeName
%type  <id>typeSMI
%type  <err>typeTag
%type  <err>valueDeclaration
%type  <err>conceptualTable
%type  <err>row
%type  <err>entryType
%type  <err>sequenceItems
%type  <id>sequenceItem
%type  <err>Syntax
%type  <err>sequenceSyntax
%type  <err>NamedBits
%type  <err>NamedBit
%type  <id>identifier
%type  <err>objectIdentityClause
%type  <err>objectTypeClause
%type  <err>trapTypeClause
%type  <text>descriptionClause
%type  <err>VarPart
%type  <err>VarTypes
%type  <err>VarType
%type  <err>DescrPart
%type  <err>MaxAccessPart
%type  <err>notificationTypeClause
%type  <err>moduleIdentityClause
%type  <err>typeDeclaration
%type  <err>typeDeclarationRHS
%type  <err>ObjectSyntax
%type  <err>sequenceObjectSyntax
%type  <err>valueofObjectSyntax
%type  <err>SimpleSyntax
%type  <err>valueofSimpleSyntax
%type  <err>sequenceSimpleSyntax
%type  <err>ApplicationSyntax
%type  <err>sequenceApplicationSyntax
%type  <err>integerSubType
%type  <err>octetStringSubType
%type  <err>ranges
%type  <err>range
%type  <err>value
%type  <err>enumSpec
%type  <err>enumItems
%type  <err>enumItem
%type  <status>Status
%type  <status>Status_Capabilities
%type  <err>DisplayPart
%type  <err>UnitsPart
%type  <access>Access
%type  <err>IndexPart
%type  <err>IndexTypes
%type  <err>IndexType
%type  <err>Index
%type  <err>Entry
%type  <err>DefValPart
%type  <err>Value
%type  <err>BitsValue
%type  <err>BitNames
%type  <err>BitName
%type  <mibnode>ObjectName
%type  <mibnode>NotificationName
%type  <err>ReferPart
%type  <err>RevisionPart
%type  <err>Revisions
%type  <err>Revision
%type  <err>ObjectsPart
%type  <err>Objects
%type  <err>Object
%type  <err>NotificationsPart
%type  <err>Notifications
%type  <err>Notification
%type  <text>Text
%type  <err>ExtUTCTime
%type  <mibnode>objectIdentifier
%type  <mibnode>subidentifiers
%type  <mibnode>subidentifier
%type  <err>objectIdentifier_defval
%type  <err>subidentifiers_defval
%type  <err>subidentifier_defval
%type  <err>objectGroupClause
%type  <err>notificationGroupClause
%type  <err>moduleComplianceClause
%type  <err>ModulePart_Compliance
%type  <err>Modules_Compliance
%type  <err>Module_Compliance
%type  <err>ModuleName_Compliance
%type  <err>MandatoryPart
%type  <err>Groups
%type  <err>Group
%type  <err>CompliancePart
%type  <err>Compliances
%type  <err>Compliance
%type  <err>ComplianceGroup
%type  <err>Object_Compliance
%type  <err>SyntaxPart
%type  <err>WriteSyntaxPart
%type  <err>WriteSyntax
%type  <err>AccessPart
%type  <err>agentCapabilitiesClause
%type  <err>ModulePart_Capabilities
%type  <err>Modules_Capabilities
%type  <err>Module_Capabilities
%type  <err>ModuleName_Capabilities
%type  <err>VariationPart
%type  <err>Variations
%type  <err>Variation
%type  <err>NotificationVariation
%type  <err>ObjectVariation
%type  <err>CreationPart
%type  <err>Cells
%type  <err>Cell
%type  <text>number

%%

/*
 * Yacc rules.
 *
 * Conventions:
 *
 * All uppercase symbols are terminals as given by the lexer.
 * 
 * Symbols with mixed characters and the first letter uppercase
 * are taken from the specifications in RFCs (and probably the
 * Simple Book). In some cases conflicts were resolved by appending
 * a `_' character and a context specification like in
 * ModuleName_Compliance.
 * 
 * Symbols starting with a lowercase letter are defined by the author
 * based on prosa definitions in documents or for other obvious use.
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
			    if ((thisParser->flags & FLAG_WHOLEFILE) ||
				(!strcmp(thisParser->module, $1))) {
				thisParser->flags |= FLAG_ACTIVE;
				thisModule = addModule($1,
						       thisParser->path,
						       thisParser->character,
						       thisParser->flags &
						       FLAGS_GENERAL,
						       thisParser);
			    } else {
				printError(parser, ERR_UNWANTED_MODULE, $1);
				thisParser->flags &= ~FLAG_ACTIVE;
				thisModule = &dummyModule;
			    }
			    thisModule->flags &= ~FLAG_SMIV2;
			    thisModule->numImportedIdentifiers = 0;
			    thisModule->numStatements = 0;
			    thisModule->numModuleIdentities = 0;
			    if (!strcmp($1, "SNMPv2-SMI")) {
			        /*
				 * SNMPv2-SMI is an SMIv2 module that cannot
				 * be identified by importing from SNMPv2-SMI.
				 */
			        thisModule->flags |= FLAG_SMIV2;
			    }

			}
			DEFINITIONS COLON_COLON_EQUAL BEGIN_
			exportsClause
			linkagePart
			declarationPart
			END
			{
			    PendingMibNode *p;
			    
			    if ((thisModule->flags & FLAG_SMIV2) &&
				(thisModule->numModuleIdentities < 1)) {
			        printError(parser, ERR_NO_MODULE_IDENTITY);
			    }
			    for (p = firstPendingMibNode; p; p = p->next) {
				printError(parser, ERR_UNKNOWN_OIDLABEL,
					   p->descriptor->name);
			    }
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
			    if (strcmp(thisModule->descriptor->name,
				       "RFC1155-SMI")) {
			        printError(parser, ERR_EXPORTS);
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
			    char *path;
			    int flags;
			    
			    if (!strcmp($3, "SNMPv2-SMI")) {
			        /*
				 * A module that imports from SNMPv2-SMI
				 * seems to be SMIv2 style.
				 */
			        thisModule->flags |= FLAG_SMIV2;
			    }

			    /*
			     * Recursively call the parser to suffer
			     * the IMPORTS, if the module is not yet
			     * loaded.
			     */
			    if (!findModuleByName($3)) {
				flags = thisParser->flags;
				flags &= ~(FLAG_WHOLEFILE | FLAG_WHOLEMOD);
				if (!(flags & FLAG_RECURSIVE)) {
				    flags &= ~(FLAG_ERRORS | FLAG_STATS);
				}
				path = findFileByModulename($3);
				if (!path) {
				    printError(parser, ERR_MODULE_NOT_FOUND,
					       $3);
				} else {
				    printError(parser, ERR_INCLUDE, $3, path);
				    readMibFile(path, $3, flags);
				}
			    }
			    checkImportDescriptors(findModuleByName($3),
						   parser);
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
			    addImportDescriptor($1, parser);
			    thisModule->numImportedIdentifiers++;
			    strncpy($$, $1, sizeof($1)-1);
			}
	|		UPPERCASE_IDENTIFIER
			{
			    addImportDescriptor($1, parser);
			    thisModule->numImportedIdentifiers++;
			    strncpy($$, $1, sizeof($1)-1);
			}
	|		importedKeyword
			/* TODO: what exactly is allowed here?
			 * What should be checked? */
			{
			    addImportDescriptor($1, parser);
			    thisModule->numImportedIdentifiers++;
			    strncpy($$, $1, sizeof($1)-1);
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
importedKeyword:	ACCESS		   { strncpy($$, $1, sizeof($$)-1); }
	|		AGENT_CAPABILITIES { strncpy($$, $1, sizeof($$)-1); }
	|		AUGMENTS	   { strncpy($$, $1, sizeof($$)-1); }
	|		BITS		   { strncpy($$, $1, sizeof($$)-1); }
	|		CONTACT_INFO	   { strncpy($$, $1, sizeof($$)-1); }
	|		CREATION_REQUIRES  { strncpy($$, $1, sizeof($$)-1); }
	|		COUNTER32	   { strncpy($$, $1, sizeof($$)-1); }
	|		COUNTER64	   { strncpy($$, $1, sizeof($$)-1); }
	|		DEFVAL		   { strncpy($$, $1, sizeof($$)-1); }
	|		DESCRIPTION	   { strncpy($$, $1, sizeof($$)-1); }
	|		DISPLAY_HINT	   { strncpy($$, $1, sizeof($$)-1); }
	|		GROUP		   { strncpy($$, $1, sizeof($$)-1); }
	|		GAUGE32		   { strncpy($$, $1, sizeof($$)-1); }
	|		IMPLIED		   { strncpy($$, $1, sizeof($$)-1); }
	|		INDEX		   { strncpy($$, $1, sizeof($$)-1); }
	|		INTEGER32	   { strncpy($$, $1, sizeof($$)-1); }
	|		IPADDRESS	   { strncpy($$, $1, sizeof($$)-1); }
	|		LAST_UPDATED	   { strncpy($$, $1, sizeof($$)-1); }
	|		MANDATORY_GROUPS   { strncpy($$, $1, sizeof($$)-1); }
	|		MAX_ACCESS	   { strncpy($$, $1, sizeof($$)-1); }
	|		MIN_ACCESS	   { strncpy($$, $1, sizeof($$)-1); }
	|		MODULE		   { strncpy($$, $1, sizeof($$)-1); }
	|		MODULE_COMPLIANCE  { strncpy($$, $1, sizeof($$)-1); }
	|		MODULE_IDENTITY	   { strncpy($$, $1, sizeof($$)-1); }
	|		NOTIFICATIONS	   { strncpy($$, $1, sizeof($$)-1); }
	|		NOTIFICATION_GROUP { strncpy($$, $1, sizeof($$)-1); }
	|		NOTIFICATION_TYPE  { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECT_GROUP	   { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECT_IDENTITY	   { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECT_TYPE	   { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECTS		   { strncpy($$, $1, sizeof($$)-1); }
	|		ORGANIZATION	   { strncpy($$, $1, sizeof($$)-1); }
	|		OPAQUE		   { strncpy($$, $1, sizeof($$)-1); }
	|		PRODUCT_RELEASE	   { strncpy($$, $1, sizeof($$)-1); }
	|		REFERENCE	   { strncpy($$, $1, sizeof($$)-1); }
	|		REVISION	   { strncpy($$, $1, sizeof($$)-1); }
	|		STATUS		   { strncpy($$, $1, sizeof($$)-1); }
	|		SUPPORTS	   { strncpy($$, $1, sizeof($$)-1); }
	|		SYNTAX		   { strncpy($$, $1, sizeof($$)-1); }
	|		TEXTUAL_CONVENTION { strncpy($$, $1, sizeof($$)-1); }
	|		TIMETICKS	   { strncpy($$, $1, sizeof($$)-1); }
	|		TRAP_TYPE	   { strncpy($$, $1, sizeof($$)-1); }
	|		UNITS		   { strncpy($$, $1, sizeof($$)-1); }
	|		UNSIGNED32	   { strncpy($$, $1, sizeof($$)-1); }
	|		VARIATION	   { strncpy($$, $1, sizeof($$)-1); }
	|		WRITE_SYNTAX	   { strncpy($$, $1, sizeof($$)-1); }
			/* TODO: which keywords should really be
			 * allowed in import clauses? */
	;

moduleName:		UPPERCASE_IDENTIFIER
			{
			    if (strlen($1) > 64) {
			        printError(parser, ERR_MODULENAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_MODULENAME_32, $1);
			    }
			    strncpy($$, $1, sizeof($1)-1);
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
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		valueDeclaration
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		objectIdentityClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		objectTypeClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		trapTypeClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		notificationTypeClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		moduleIdentityClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
			/* TODO: check it's first and once */
	|		moduleComplianceClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		objectGroupClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		notificationGroupClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		macroClause
			{ 
			    thisModule->numStatements++;
			    $$ = 0;
			}
	|		error '}'
			{
			    printError(parser, ERR_FLUSH_DECLARATION);
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
			    if (strcmp(thisModule->descriptor->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisModule->descriptor->name,
				       "SNMPv2-TC") &&
				strcmp(thisModule->descriptor->name,
				       "SNMPv2-CONF") &&
				strcmp(thisModule->descriptor->name,
				       "RFC-1212") &&
				strcmp(thisModule->descriptor->name,
				       "RFC1155-SMI")) {
			        printError(parser, ERR_MACRO);
			    }
			}
			/* the scanner skips until... */
			END
			{
			    if (thisParser->flags & FLAG_ACTIVE) {
				/*
				 * Some bison magics make the objectIdentifier
				 * to be $7 instead of $6.
				 */
				addMacro($1, thisModule,
					thisParser->character,
					(thisParser->flags &
					 (FLAG_WHOLEMOD |
					  FLAG_WHOLEFILE))
					? FLAG_MODULE : 0,
					thisParser);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
                        }
	;

macroName:		MODULE_IDENTITY	    { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECT_TYPE	    { strncpy($$, $1, sizeof($$)-1); }
	|		NOTIFICATION_TYPE   { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECT_IDENTITY	    { strncpy($$, $1, sizeof($$)-1); }
	|		TEXTUAL_CONVENTION  { strncpy($$, $1, sizeof($$)-1); }
	|		OBJECT_GROUP	    { strncpy($$, $1, sizeof($$)-1); }
	|		NOTIFICATION_GROUP  { strncpy($$, $1, sizeof($$)-1); }
	|		MODULE_COMPLIANCE   { strncpy($$, $1, sizeof($$)-1); }
	|		AGENT_CAPABILITIES  { strncpy($$, $1, sizeof($$)-1); }
	;

choiceClause:		CHOICE
			{
			    if (strcmp(thisModule->descriptor->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisModule->descriptor->name,
				       "SNMPv2-TC") &&
				strcmp(thisModule->descriptor->name,
				       "SNMPv2-CONF") &&
				strcmp(thisModule->descriptor->name,
				       "RFC-1212") &&
				strcmp(thisModule->descriptor->name,
				       "RFC1155-SMI")) {
			        printError(parser, ERR_CHOICE);
			    }
			}
			/* the scanner skips until... */
			'}'
			{ $$ = 0; }
	;

/*
 * The only ASN.1 value declarations are for OIDs, REF:RFC1902,491 .
 */
valueDeclaration:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			    if (thisModule->flags & FLAG_SMIV2) {
			        if (strchr($1, '-')) {
				    printError(parser,
					       ERR_OIDNAME_INCLUDES_HYPHEN,
					       $1);
				}
			    }
			}
			OBJECT IDENTIFIER
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $7,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($7, descriptor, MACRO_NONE,
					      0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

/*
 * This is for simple ASN.1 style type assignments and textual conventions.
 */
typeDeclaration:	typeName
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_TYPENAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_TYPENAME_32, $1);
			    }
			}
			COLON_COLON_EQUAL typeDeclarationRHS
			{
			    if (thisParser->flags & FLAG_ACTIVE) {
				addType($1, thisModule,
					"$4.type", thisParser->character,
					/* TODO: due to TC or SYNTAX ?!? */
					(thisParser->flags &
					 (FLAG_WHOLEMOD |
					  FLAG_WHOLEFILE))
					? FLAG_MODULE : 0,
					thisParser);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

typeName:		UPPERCASE_IDENTIFIER
			{
			    strncpy($$, $1, sizeof($$)-1);
			}
	|		typeSMI
			{
			    /*
			     * well known types (keywords in this grammar)
			     * are known to be defined in these modules.
			     */
			    if (strcmp(thisModule->descriptor->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisModule->descriptor->name,
				       "SNMPv2-TC") &&
				strcmp(thisModule->descriptor->name,
				       "SNMPv2-CONF") &&
				strcmp(thisModule->descriptor->name,
				       "RFC-1212") &&
				strcmp(thisModule->descriptor->name,
				       "RFC1155-SMI")) {
			        printError(parser, ERR_TYPE_SMI, $1);
			    }
			    strncpy($$, $1, sizeof($$)-1);
			}
	;

typeSMI:		INTEGER32	{ strncpy($$, $1, sizeof($$)-1); }
	|		IPADDRESS	{ strncpy($$, $1, sizeof($$)-1); }
	|		COUNTER32	{ strncpy($$, $1, sizeof($$)-1); }
	|		GAUGE32		{ strncpy($$, $1, sizeof($$)-1); }
	|		UNSIGNED32	{ strncpy($$, $1, sizeof($$)-1); }
	|		TIMETICKS	{ strncpy($$, $1, sizeof($$)-1); }
	|		OPAQUE		{ strncpy($$, $1, sizeof($$)-1); }
	|		COUNTER64	{ strncpy($$, $1, sizeof($$)-1); }
	;

typeDeclarationRHS:	Syntax
			{ $$ = 0; }
	|		TEXTUAL_CONVENTION
			DisplayPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			SYNTAX Syntax
			{ $$ = 0; }
	|		choiceClause
			{ $$ = 0; }
	;

/* REF:RFC1902,7.1.12. */
conceptualTable:	SEQUENCE OF row
			{ $$ = 0; }
	;

row:			UPPERCASE_IDENTIFIER
			/* in this case, we do NOT allow `Module.Type' */
			{ $$ = 0; }
			/* TODO: this must be an entryType */
	;

/* REF:RFC1902,7.1.12. */
entryType:		SEQUENCE '{' sequenceItems '}'
			{ $$ = 0; }
	;

sequenceItems:		sequenceItem
			{ $$ = 0; }
	|		sequenceItems ',' sequenceItem
			/* TODO: might this list be emtpy? */
			{ $$ = 0; }
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 * NOTE: REF:RFC1902,7.1.12. was less clear, it said:
 * `normally omitting the sub-typing information'
 */
sequenceItem:		LOWERCASE_IDENTIFIER sequenceSyntax
			{
			    strncpy($$, $1, sizeof($1)-1);
			}
	;

Syntax:			ObjectSyntax
			{ $$ = 0; }
	|		BITS '{' NamedBits '}'
			/* TODO: standalone `BITS' ok? seen in RMON2-MIB */
			/* -> no, it's only allowed in a SEQUENCE {...} */
			{ $$ = 0; }
	;

sequenceSyntax:		ObjectSyntax
			{ $$ = 0; }
	|		BITS
			{ $$ = 0; }
	;

NamedBits:		NamedBit
			{ $$ = 0; }
	|		NamedBits ',' NamedBit
			{ $$ = 0; }
	;

NamedBit:		identifier
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_BITNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_BITNAME_32, $1);
			    }
			}
			'(' number ')'
			{ $$ = 0; }
	;

identifier:		LOWERCASE_IDENTIFIER
			/* TODO */
			{
			    strncpy($$, $1, sizeof($1)-1);
			}
	;

objectIdentityClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			}
			OBJECT_IDENTITY
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $11,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($11, descriptor,
					      MACRO_OBJECTIDENTITY,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

objectTypeClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
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
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $16,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($16, descriptor,
					      MACRO_OBJECTTYPE,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

descriptionClause:	/* empty */
			{
			    if (thisModule->flags & FLAG_SMIV2) {
				printError(parser,
					   ERR_MISSING_DESCRIPTION);
			    }
			    strcpy($$.content, "");
			    $$.fileoffset = thisParser->character;
			    $$.length = 0;
			}
	|		DESCRIPTION Text
			{
			    strncpy($$.content, $2.content,
				    sizeof($$.content)-1);
			    $$.fileoffset = $2.fileoffset;
			    $$.length = $2.length;
			}
	;

trapTypeClause:		LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			}
			TRAP_TYPE
			{
			    if (thisModule->flags & FLAG_SMIV2) {
			        printError(parser, ERR_TRAP_TYPE);
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
			    if (!(thisModule->flags & FLAG_SMIV2)) {
			        printError(parser, ERR_MAX_ACCESS_IN_SMIV1);
			    }
			}
			Access
			{ $$ = 0; }
	|		ACCESS
			{
			    if (thisModule->flags & FLAG_SMIV2) {
			        printError(parser, ERR_ACCESS_IN_SMIV2);
			    }
			}
			Access
			/* TODO: limited values in v1 */
			{ $$ = 0; }
	;

notificationTypeClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
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
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $12,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($12, descriptor, 
					      MACRO_NOTIFICATIONTYPE,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

moduleIdentityClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			}
			MODULE_IDENTITY
			{
			    if (thisModule->numModuleIdentities > 0) {
			        printError(parser,
					   ERR_TOO_MANY_MODULE_IDENTITIES);
			    }
			    if (thisModule->numStatements > 0) {
			        printError(parser,
					   ERR_MODULE_IDENTITY_NOT_FIRST);
			    }
			}
			LAST_UPDATED ExtUTCTime
			ORGANIZATION Text
			CONTACT_INFO Text
			DESCRIPTION Text
			RevisionPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Descriptor *descriptor;
			    
			    thisModule->numModuleIdentities++;
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $16,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($16, descriptor,
					      MACRO_MODULEIDENTITY,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
        ;

ObjectSyntax:		SimpleSyntax
			{ $$ = 0; }
	|		typeTag SimpleSyntax
			{
			    if (strcmp(thisModule->descriptor->name,
				       "SNMPv2-SMI") &&
			        strcmp(thisModule->descriptor->name,
				       "SNMPv2-TC") &&
				strcmp(thisModule->descriptor->name,
				       "SNMPv2-CONF") &&
				strcmp(thisModule->descriptor->name,
				       "RFC-1212") &&
				strcmp(thisModule->descriptor->name,
				       "RFC1155-SMI")) {
			        printError(parser, ERR_TYPE_TAG, $1);
			    }
			}
	|		conceptualTable	     /* TODO: possible? row? entry? */
			{ $$ = 0; }
	|		row		     /* the uppercase name of a row  */
			{ $$ = 0; }
	|		entryType	     /* it's SEQUENCE { ... } phrase */
			{ $$ = 0; }
	|		ApplicationSyntax
			{ $$ = 0; }
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
			{ $$ = 0; }
/*	|		conceptualTable	     /* TODO: possible? row? entry? */
/*	|		row		     /* the uppercase name of a row  */
/*	|		entryType	     /* it's SEQUENCE { ... } phrase */
	|		sequenceApplicationSyntax
			{ $$ = 0; }
        ;

/* TODO: specify really according to ObjectSyntax!!! */
valueofObjectSyntax:	valueofSimpleSyntax
			{ $$ = 0; }
			/* conceptualTables and rows do not have DEFVALs
			 */
			/* valueofApplicationSyntax would not introduce any
			 * further syntax of ObjectSyntax values.
			 */
	;

SimpleSyntax:		INTEGER			/* (-2147483648..2147483647) */
			{ $$ = 0; }
	|		INTEGER integerSubType
			{ $$ = 0; }
	|		INTEGER enumSpec
			{ $$ = 0; }
	|		INTEGER32		/* (-2147483648..2147483647) */
			{ $$ = 0; }
        |		INTEGER32 integerSubType
			{ $$ = 0; }
	|		UPPERCASE_IDENTIFIER enumSpec
			{ $$ = 0; }
	|		moduleName '.' UPPERCASE_IDENTIFIER enumSpec
			/* TODO: UPPERCASE_IDENTIFIER must be an INTEGER */
			{ $$ = 0; }
	|		UPPERCASE_IDENTIFIER integerSubType
			{ $$ = 0; }
	|		moduleName '.' UPPERCASE_IDENTIFIER integerSubType
			/* TODO: UPPERCASE_IDENTIFIER must be an INT/Int32. */
			{ $$ = 0; }
	|		OCTET STRING		/* (SIZE (0..65535))	     */
			{ $$ = 0; }
	|		OCTET STRING octetStringSubType
			{ $$ = 0; }
	|		UPPERCASE_IDENTIFIER octetStringSubType
			{ $$ = 0; }
	|		moduleName '.' UPPERCASE_IDENTIFIER octetStringSubType
			/* TODO: UPPERCASE_IDENTIFIER must be an OCTET STR. */
			{ $$ = 0; }
	|		OBJECT IDENTIFIER
			{ $$ = 0; }
	;

valueofSimpleSyntax:	number			/* 0..2147483647 */
			/* NOTE: Counter64 must not have a DEFVAL */
			{ $$ = 0; }
	|		'-' number		/* -2147483648..0 */
			{ $$ = 0; }
	|		BIN_STRING		/* number or OCTET STRING */
			{ $$ = 0; }
	|		HEX_STRING		/* number or OCTET STRING */
			{ $$ = 0; }
	|		LOWERCASE_IDENTIFIER	/* enumeration label */
			{ $$ = 0; }
	|		QUOTED_STRING		/* an OCTET STRING */
			{ $$ = 0; }
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
			{
			    printError(parser, ERR_OID_DEFVAL_TOO_LONG);
			}
			/* TODO: need context knowledge about SYNTAX
			 *       to decide what is allowed
			 */
			{ $$ = 0; }
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceSimpleSyntax:	INTEGER			/* (-2147483648..2147483647) */
			{ $$ = 0; }
        |		INTEGER32		/* (-2147483648..2147483647) */
			{ $$ = 0; }
	|		OCTET STRING		/* (SIZE (0..65535))	     */
			{ $$ = 0; }
	|		OBJECT IDENTIFIER
			{ $$ = 0; }
	;

ApplicationSyntax:	IPADDRESS
			{ $$ = 0; }
	|		COUNTER32		/* (0..4294967295)	     */
			{ $$ = 0; }
	|		GAUGE32			/* (0..4294967295)	     */
			{ $$ = 0; }
	|		GAUGE32 integerSubType
			{ $$ = 0; }
	|		UNSIGNED32		/* (0..4294967295)	     */
			{ $$ = 0; }
	|		UNSIGNED32 integerSubType
			{ $$ = 0; }
	|		TIMETICKS		/* (0..4294967295)	     */
			{ $$ = 0; }
	|		OPAQUE			/* IMPLICIT OCTET STRING     */
			{ $$ = 0; }
	|		COUNTER64	        /* (0..18446744073709551615) */
			{ $$ = 0; }
	;

/*
 * In a SEQUENCE { ... } there are no sub-types, enumerations or
 * named bits. REF: draft, p.29
 */
sequenceApplicationSyntax: IPADDRESS
			{ $$ = 0; }
	|		COUNTER32		/* (0..4294967295)	     */
			{ $$ = 0; }
	|		GAUGE32			/* (0..4294967295)	     */
			{ $$ = 0; }
	|		UNSIGNED32		/* (0..4294967295)	     */
			{ $$ = 0; }
	|		TIMETICKS		/* (0..4294967295)	     */
			{ $$ = 0; }
	|		OPAQUE			/* IMPLICIT OCTET STRING     */
			{ $$ = 0; }
	|		COUNTER64	        /* (0..18446744073709551615) */
			{ $$ = 0; }
	;

/* REF: draft,p.46 */
integerSubType:		'(' ranges ')'		/* at least one range        */
			/*
			 * the specification mentions an alternative of an
			 * empty RHS here. this would lead to reduce/reduce
			 * conflicts. instead, we differentiate the parent
			 * rule(s) (SimpleSyntax).
			 */
			{ $$ = 0; }
	;

octetStringSubType:	'(' SIZE '(' ranges ')' ')'
			/*
			 * the specification mentions an alternative of an
			 * empty RHS here. this would lead to reduce/reduce
			 * conflicts. instead, we differentiate the parent
			 * rule(s) (SimpleSyntax).
			 */
			{ $$ = 0; }
	;

ranges:			range
			{ $$ = 0; }
	|		ranges '|' range
			{ $$ = 0; }
	;

range:			value
			{ $$ = 0; }
	|		value DOT_DOT value
			{ $$ = 0; }
	;

value:			'-' number
			{ $$ = 0; }
	|		number
			{ $$ = 0; }
	|		HEX_STRING
			{ $$ = 0; }
	|		BIN_STRING
			{ $$ = 0; }
	;

enumSpec:		'{' enumItems '}'
			{ $$ = 0; }
	;

enumItems:		enumItem
			{ $$ = 0; }
	|		enumItems ',' enumItem
			{ $$ = 0; }
	;

enumItem:		LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_ENUMNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_ENUMNAME_32, $1);
			    }
			}
			'(' number ')'
			/* TODO: these numbers might be negative, right? */
			{ $$ = 0; }
	;

Status:			LOWERCASE_IDENTIFIER
			{
			    if (thisModule->flags & FLAG_SMIV2) {
				if (!strcmp($1, "current")) {
				    $$ = STATUS_CURRENT;
				} else if (!strcmp($1, "deprecated")) {
				    $$ = STATUS_DEPRECATED;
				} else if (!strcmp($1, "obsolete")) {
				    $$ = STATUS_OBSOLETE;
				} else {
				    printError(parser,
					       ERR_INVALID_SMIV1_STATUS, $1);
				    $$ = STATUS_UNKNOWN;
				}
			    }
			}
        ;		

Status_Capabilities:	LOWERCASE_IDENTIFIER
			{
			    if (!strcmp($1, "current")) {
				$$ = STATUS_CURRENT;
			    } else if (!strcmp($1, "obsolete")) {
				$$ = STATUS_OBSOLETE;
			    } else {
				printError(parser,
					   ERR_INVALID_CAPABILITIES_STATUS,
					   $1);
				$$ = STATUS_UNKNOWN;
			    }
			}
        ;

DisplayPart:		DISPLAY_HINT QUOTED_STRING
			{ $$ = 0; }
        |		/* empty */
			{ $$ = 0; }
        ;

UnitsPart:		UNITS QUOTED_STRING
			{ $$ = 0; }
        |		/* empty */
			{ $$ = 0; }
        ;

Access:			LOWERCASE_IDENTIFIER
			{
			    if (thisModule->flags & FLAG_SMIV2) {
				if (!strcmp($1, "not-accessible")) {
				    $$ = ACCESS_NOT;
				} else if (!strcmp($1,
						   "accessible-for-notify")) {
				    $$ = ACCESS_NOTIFY;
				} else if (!strcmp($1, "read-only")) {
				    $$ = ACCESS_READ_ONLY;
				} else if (!strcmp($1, "read-write")) {
				    $$ = ACCESS_READ_WRITE;
				} else if (!strcmp($1, "read-create")) {
				    $$ = ACCESS_READ_CREATE;
				} else {
				    printError(parser,
					       ERR_INVALID_SMIV2_ACCESS,
					       $1);
				    $$ = ACCESS_UNKNOWN;
				}
			    } else {
				if (!strcmp($1, "not-accessible")) {
				    $$ = ACCESS_NOT;
				} else if (!strcmp($1, "read-only")) {
				    $$ = ACCESS_READ_ONLY;
				} else if (!strcmp($1, "read-write")) {
				    $$ = ACCESS_READ_WRITE;
				} else if (!strcmp($1, "write-only")) {
				    $$ = ACCESS_WRITE_ONLY;
				} else {
				    printError(parser,
					       ERR_INVALID_SMIV1_ACCESS, $1);
				    $$ = ACCESS_UNKNOWN;
				}
			    }
			}
        ;

IndexPart:		INDEX '{' IndexTypes '}'
			{ $$ = 0; }
        |		AUGMENTS '{' Entry '}'
			/* TODO: no AUGMENTS clause in v1 */
			{ $$ = 0; }
        |		/* empty */
			{ $$ = 0; }
	;

IndexTypes:		IndexType
			{ $$ = 0; }
        |		IndexTypes ',' IndexType
			{ $$ = 0; }
	;

IndexType:		IMPLIED Index
			{ $$ = 0; }
	|		Index
			{ $$ = 0; }
	;

Index:			ObjectName
			/* TODO: use the SYNTAX value of the correspondent
			 *       OBJECT-TYPE invocation
			 */
			{ $$ = 0; }
        ;

Entry:			ObjectName
			/* TODO: use the SYNTAX value of the correspondent
			 *       OBJECT-TYPE invocation
			 */
			{ $$ = 0; }
        ;

DefValPart:		DEFVAL '{' Value '}'
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
			/* TODO: different for DefValPart in AgentCaps ? */
	;

Value:			valueofObjectSyntax
			{ $$ = 0; }
	|		'{' BitsValue '}'
			{ $$ = 0; }
	;

BitsValue:		BitNames
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

BitNames:		BitName
			{ $$ = 0; }
	|		BitNames ',' BitName
			{ $$ = 0; }
	;

BitName:		identifier
			{ $$ = 0; }
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
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
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
			{ $$ = 0; }
	;

ObjectsPart:		OBJECTS '{' Objects '}'
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Objects:		Object
			{ $$ = 0; }
	|		Objects ',' Object
			{ $$ = 0; }
	;

Object:			ObjectName
			{ $$ = 0; }
	;

NotificationsPart:	NOTIFICATIONS '{' Notifications '}'
			{ $$ = 0; }
	;

Notifications:		Notification
			{ $$ = 0; }
	|		Notifications ',' Notification
			{ $$ = 0; }
	;

Notification:		NotificationName
			{ $$ = 0; }
	;

Text:			QUOTED_STRING
			{
			    strncpy($$.content, $1.content,
				    sizeof($$.content)-1);
			    $$.fileoffset = $1.fileoffset;
			    $$.length = $1.length;
			}
	;

/*
 * TODO: REF: 
 */
ExtUTCTime:		QUOTED_STRING
			/* TODO: check length and format */
			{ $$ = 0; }
	;

objectIdentifier:	{
			    parent = rootMibNode;
			}
			subidentifiers
			{
			    $$ = $2;
			    parent = $2;
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
			    MibNode *node;

			    if (parent != rootMibNode) {
				printError(parser, ERR_OIDLABEL_NOT_FIRST, $1);
			    } else {

				/* TODO: replace by ...ByModuleAndName !! */
				node = findMibNodeByName($1);
				if (node) {
				    $$ = node;
				} else {
				    $$ = addMibNode($1, thisModule,
						    pendingRootMibNode,
						    0 /*subid not yet known*/,
						    thisParser->character,
						    MACRO_NONE,
						    FLAG_MODULE | FLAG_NOVALUE,
						    parser);
				}
				parent = $$;
			    }
			}
	|		moduleName '.' LOWERCASE_IDENTIFIER
			{
			    MibNode *node;
			    char s[2*MAX_IDENTIFIER_LENGTH+2];
			    
			    sprintf(s, "%s.%s", $1, $3);

			    if (parent != rootMibNode) {
				printError(parser, ERR_OIDLABEL_NOT_FIRST, s);
			    } else {

				node = findMibNodeByModulenameAndName($1, $3);
				if (node) {
				    $$ = node;
				} else {
				    /*
				     * oid label is qualified by module name
				     * but not known, hence it seems to be
				     * an error. nevertheless, we define it
				     * like a forward referenced oid.
				     */
				    printError(parser,
					       ERR_UNKNOWN_OIDLABEL, s);
				    $$ = addMibNode($1, thisModule,
						    pendingRootMibNode,
						    0 /*subid not yet known*/,
						    thisParser->character,
						    MACRO_NONE,
						    FLAG_MODULE | FLAG_NOVALUE,
						    parser);
				}
				parent = $$;
			    }
			}
	|		number
			{
			    MibNode *node;

			    node = findMibNodeByParentAndSubid(parent,
				       atoi($1.content));
			    if (node) {
				$$ = node;
			    } else {
				$$ = addMibNode(NULL, thisModule,
						parent, atoi($1.content),
						thisParser->character,
						MACRO_NONE, FLAG_MODULE,
						parser);
			    }
			}
	|		LOWERCASE_IDENTIFIER '(' number ')'
			{
			    MibNode *node;
			    
			    node = findMibNodeByName($1); /* TODO AndModule */
			    if (node) {
				printError(parser, ERR_EXISTENT_DESCRIPTOR,
					   $1);
				$$ = node;
				if (node->subid != atoi($3.content)) {
				    printError(parser,
					       ERR_SUBIDENTIFIER_VS_OIDLABEL,
					       $3.content, $1);
				}
			    } else {
				$$ = addMibNode($1, thisModule, parent,
						atoi($3.content),
						thisParser->character,
						MACRO_NONE, FLAG_MODULE,
						parser);
			    }
			}
	|		moduleName '.' LOWERCASE_IDENTIFIER '(' number ')'
			{
			    MibNode *node;
			    char md[2*MAX_IDENTIFIER_LENGTH+2];
			    
			    sprintf(md, "%s.%s", $1, $3);
			    node = findMibNodeByModulenameAndName($1, $3);
			    if (node) {
				printError(parser, ERR_EXISTENT_DESCRIPTOR,
					   $1);
				$$ = node;
				if (node->subid != atoi($5.content)) {
				    printError(parser,
					       ERR_SUBIDENTIFIER_VS_OIDLABEL,
					       $5.content, md);
				}
			    } else {
				printError(parser, ERR_ILLEGALLY_QUALIFIED,
					   md);
				$$ = addMibNode($3, thisModule,
						parent, atoi($5.content),
						thisParser->character,
						MACRO_NONE, FLAG_MODULE,
						parser);
			    }
			}
	;

objectIdentifier_defval: subidentifiers_defval
			{ $$ = 0; }
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
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			}
			OBJECT_GROUP
			ObjectsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $12,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($12, descriptor,
					      MACRO_OBJECTGROUP,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

notificationGroupClause: LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			}
			NOTIFICATION_GROUP
			NotificationsPart
			STATUS Status
			DESCRIPTION Text
			ReferPart
			COLON_COLON_EQUAL
			'{' objectIdentifier '}'
			{
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $12,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($12, descriptor,
					      MACRO_NOTIFICATIONGROUP,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

moduleComplianceClause:	LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
			    }
			}
			MODULE_COMPLIANCE
			STATUS Status
			DESCRIPTION Text
			ReferPart
			ModulePart_Compliance
			COLON_COLON_EQUAL '{' objectIdentifier '}'
			{
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $12,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($12, descriptor,
					      MACRO_MODULECOMPLIANCE,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
			}
	;

ModulePart_Compliance:	Modules_Compliance
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Modules_Compliance:	Module_Compliance
			{ $$ = 0; }
	|		Modules_Compliance Module_Compliance
			{ $$ = 0; }
	;

Module_Compliance:	MODULE ModuleName_Compliance
			MandatoryPart
			CompliancePart
			{ $$ = 0; }
	;

ModuleName_Compliance:	UPPERCASE_IDENTIFIER objectIdentifier
			{ $$ = 0; }
	|		UPPERCASE_IDENTIFIER
			{ $$ = 0; }
	|		/* empty, only if contained in MIB module */
			/* TODO: RFC 1904 looks a bit different, is this ok? */
			{ $$ = 0; }
	;

MandatoryPart:		MANDATORY_GROUPS '{' Groups '}'
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Groups:			Group
			{ $$ = 0; }
	|		Groups ',' Group
			{ $$ = 0; }
	;

Group:			objectIdentifier
			{ $$ = 0; }
	;

CompliancePart:		Compliances
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

Compliances:		Compliance
			{ $$ = 0; }
	|		Compliances Compliance
			{ $$ = 0; }
	;

Compliance:		ComplianceGroup
			{ $$ = 0; }
	|		Object_Compliance
			{ $$ = 0; }
	;

ComplianceGroup:	GROUP objectIdentifier
			DESCRIPTION Text
			{ $$ = 0; }
	;

Object_Compliance:	OBJECT ObjectName
			SyntaxPart
			WriteSyntaxPart
			AccessPart
			DESCRIPTION Text
			{ $$ = 0; }
	;

SyntaxPart:		SYNTAX Syntax
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

WriteSyntaxPart:	WRITE_SYNTAX WriteSyntax
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

WriteSyntax:		Syntax
			/* TODO: right? */
			{ $$ = 0; }
	;

AccessPart:		MIN_ACCESS Access
			{ $$ = 0; }
	|		/* empty */
			{ $$ = 0; }
	;

agentCapabilitiesClause: LOWERCASE_IDENTIFIER
			{ 
			    if (strlen($1) > 64) {
			        printError(parser, ERR_OIDNAME_64, $1);
			    } else if (strlen($1) > 32) {
			        printError(parser, ERR_OIDNAME_32, $1);
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
			    Descriptor *descriptor;
			    
			    if (thisParser->flags & FLAG_ACTIVE) {
				descriptor = addDescriptor($1, thisModule,
					      KIND_MIBNODE,
					      $14,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? FLAG_MODULE : 0,
					      thisParser);
				changeMibNode($14, descriptor,
					      MACRO_AGENTCAPABILITIES,
					      (thisParser->flags &
					       (FLAG_WHOLEMOD |
						FLAG_WHOLEFILE))
					      ? (FLAG_MODULE | FLAG_REGISTERED)
					      : 0);
				$$ = 0;
			    } else {
				$$ = 0;
			    }
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
			INCLUDES '{' Groups '}'
			VariationPart
			{ $$ = 0; }
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
			    strncpy($$.content, $1,
				    sizeof($$.content)-1);
			}
	;

%%
