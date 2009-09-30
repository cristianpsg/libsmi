/*
 * parser-yang.y --
 *
 *      Syntax rules for parsing the YANG MIB module language.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *  Authors: Kaloyan Kanev, Siarhei Kuryla
 */

%{

#include <config.h>
    
#ifdef BACKEND_YANG

#define _ISOC99_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <float.h>

#if defined(_MSC_VER)
#include <malloc.h>
#endif

#include "yang.h"
#include "yang-data.h"
#include "parser-yang.h"
#include "scanner-yang.h"
#include "util.h"
#include "error.h"
#include "errormacros.h"
    
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif


/*
 * These arguments are passed to yyparse() and yylex().
 */
#define YYPARSE_PARAM parserPtr
#define YYLEX_PARAM   parserPtr
 
    
#define thisParserPtr      ((Parser *)parserPtr)
#define thisModulePtr      (((Parser *)parserPtr)->yangModulePtr)
#define thisModuleInfoPtr  ((_YangModuleInfo*)((Parser *)parserPtr)->yangModulePtr->info)


#define DEBUG

#ifdef DEBUG 
#define debug(args...) fprintf (stderr, args);
#else
#define debug
#endif

    
/*
 * NOTE: The argument lvalp ist not really a void pointer. Unfortunately,
 * we don't know it better at this point. bison generated C code declares
 * YYSTYPE just a few lines below based on the `%union' declaration.
 */
extern int yylex(void *lvalp, Parser *parserPtr);

/* The declaration stack is used to determine what is the parrent
 * statement of the current one. It helps to write generic
 * substatements like the description statement which calls the
 * function setDescription, which in turn checks the current
 * statement type(decl) and uses the a global pointer to the 
 * current parent statement to set the description. 
 */
typedef struct declStack {
	YangDecl decl;
    _YangNode* node;
	struct declStack *down;
} declStack;

static declStack *dStack = NULL;

static void pushDecl(YangDecl decl)
{
	declStack *top = (declStack*)smiMalloc(sizeof(declStack));
	top->down = NULL;
	top->decl = decl;
	
	if(dStack == NULL) dStack = top;
	else
	{
		top->down = dStack;
		dStack = top;
	}
}

static void pushNode(_YangNode* node)
{
	declStack *top = (declStack*)smiMalloc(sizeof(declStack));
	top->down = NULL;
	top->node = node;
    if (node) {
        top->decl = node->export.nodeKind;
    }
	
	if(dStack == NULL) dStack = top;
	else
	{
		top->down = dStack;
		dStack = top;
	}
}

static void pop()
{
	declStack *top;	
	if(dStack != NULL)
	{
		top = dStack;
		dStack = dStack->down;
		free(top);
	}
}

static YangDecl topDecl()
{
	if (dStack == NULL)
	{
		return YANG_DECL_UNKNOWN;
	}
	else
	{
		return dStack->decl;
	}
}

static _YangNode* topNode()
{
	if (dStack == NULL)
	{
		return NULL;
	}
	else
	{
		return dStack->node;
	}
}

Parser *currentParser = NULL;

static _YangNode *node = NULL;

char* getPrefix(char* identifierRef) {
    char* colonIndex = strchr(identifierRef, ':');
    if (!colonIndex) return NULL;
    return smiStrndup(identifierRef, colonIndex - identifierRef);
}

char* getIdentifier(char* identifierRef) {
    char* colonIndex = strchr(identifierRef, ':');
    if (!colonIndex) return smiStrdup(identifierRef);
    return smiStrdup(colonIndex + 1);
}

%}

/*
 * The grammars start symbol.
 */
%start yangFile

/*
 * We call the parser from within the parser when IMPORTing modules,
 * hence we need reentrant parser code. This is a bison feature.
 */
%pure_parser

/*
 * The attributes.
 */
%union {
    char            *id;				/* identifier name           */
    int             rc;                 /* >=0: ok, <0: error    */
    char            *text;
}

/*
 * Tokens and their attributes.
 */
%token <rc>augmentKeyword
%token <rc>belongs_toKeyword
%token <rc>choiceKeyword
%token <rc>configKeyword
%token <rc>contactKeyword
%token <rc>containerKeyword
%token <rc>defaultKeyword
%token <rc>descriptionKeyword
%token <rc>enumKeyword
%token <rc>error_app_tagKeyword
%token <rc>error_messageKeyword
%token <rc>extensionKeyword
%token <rc>groupingKeyword
%token <rc>importKeyword
%token <rc>includeKeyword
%token <rc>keyKeyword
%token <rc>leafKeyword
%token <rc>leaf_listKeyword
%token <rc>lengthKeyword
%token <rc>listKeyword
%token <rc>mandatoryKeyword
%token <rc>max_elementsKeyword
%token <rc>min_elementsKeyword
%token <rc>moduleKeyword
%token <rc>submoduleKeyword
%token <rc>mustKeyword
%token <rc>namespaceKeyword
%token <rc>ordered_byKeyword
%token <rc>organizationKeyword
%token <rc>prefixKeyword
%token <rc>rangeKeyword
%token <rc>referenceKeyword
%token <rc>patternKeyword
%token <rc>revisionKeyword
%token <rc>statusKeyword
%token <rc>typeKeyword
%token <rc>typedefKeyword
%token <rc>uniqueKeyword
%token <rc>unitsKeyword
%token <rc>usesKeyword
%token <rc>valueKeyword
%token <rc>whenKeyword
%token <rc>bitKeyword
%token <rc>pathKeyword
%token <rc>anyXMLKeyword
%token <rc>deprecatedKeyword
%token <rc>currentKeyword
%token <rc>obsoleteKeyword
%token <rc>trueKeyword
%token <rc>falseKeyword
%token <rc>caseKeyword
%token <rc>inputKeyword
%token <rc>outputKeyword
%token <rc>rpcKeyword
%token <rc>notificationKeyword
%token <rc>argumentKeyword
%token <rc>yangversionKeyword
%token <rc>baseKeyword
%token <rc>deviationKeyword
%token <rc>deviateKeyword
%token <rc>featureKeyword
%token <rc>identityKeyword
%token <rc>ifFeatureKeyword
%token <rc>positionKeyword
%token <rc>presenceKeyword
%token <rc>refineKeyword
%token <rc>requireInstanceKeyword
%token <rc>yinElementKeyword
%token <rc>notSupportedKeyword
%token <rc>addKeyword
%token <rc>deleteKeyword
%token <rc>replaceKeyword

%token <text>identifier
%token <text>identifierRefArg
%token <text>identifierRefArgStr
%token <text>dateString
%token <text>yangVersion
%token <text>qString
%token <text>uqString

%token <text>decimalNumber
%token <text>floatNumber
%token <text>hexNumber

/*
 * Types of non-terminal symbols.
 */

%type <rc>yangFile
%type <rc>moduleStatement
%type <rc>submoduleStatement
%type <rc>commonStatement
%type <rc>dataDefStatement
%type <rc>extensionStatement
%type <rc>extensionStatementBody
%type <rc>extensionSubstatement
%type <rc>extensionSubstatement_0n
%type <rc>argumentStatement
%type <rc>argumentStatementBody
%type <rc>yinElementOptional
%type <rc>linkageStatement
%type <rc>linkageStatement_0n
%type <rc>moduleMetaStatement
%type <rc>moduleMetaStatement_0n
%type <rc>moduleHeaderStatement
%type <rc>moduleHeaderStatement_0n
%type <rc>submoduleHeaderStatement
%type <rc>belongsToStatement
%type <rc>bodyStatement
%type <rc>bodyStatement_0n
%type <rc>containerStatement
%type <rc>containerSubstatement
%type <rc>containerSubstatement_0n
%type <rc>mustStatement
%type <rc>mustSubstatement
%type <rc>mustSubstatement_0n
%type <rc>presenceStatement
%type <rc>organizationStatement
%type <rc>contactStatement
%type <rc>referenceStatement
%type <rc>descriptionStatement
%type <rc>revisionStatement
%type <rc>optionalRevision
%type <rc>revisionStatement_0n
%type <rc>revisionDescriptionStatement_0n
%type <rc>revisionDescriptionStatement
%type <rc>importStatement
%type <rc>includeStatement
%type <rc>includeStatementBody
%type <rc>prefixStatement
%type <rc>typedefStatement
%type <rc>typedefSubstatement
%type <rc>typedefSubstatement_0n
%type <rc>typeStatement
%type <rc>optionalTypeBodyStatements
%type <rc>typeBodyStmts
%type <rc>unitsStatement
%type <rc>featureStatement
%type <rc>featureSpec
%type <rc>featureSubstatement_0n
%type <rc>featureSubstatement
%type <rc>ifFeatureStatement
%type <rc>identityStatement
%type <rc>identitySpec
%type <rc>identitySubstatement_0n
%type <rc>identitySubstatement
%type <rc>booleanValue

%type <text>numRestriction
%type <rc>stringRestriction
%type <rc>stringRestriction_0n
%type <rc>optionalRestrictionSpec
%type <rc>restrictionSpec_0n
%type <rc>restrictionSpec
%type <rc>range
%type <rc>length
%type <text>date
%type <text>string
%type <text>prefix
%type <text>identifierRef
%type <text>identifierStr
%type <rc>enumSpec
%type <rc>enum
%type <rc>enumSubstatementSpec
%type <rc>enumSubstatement_0n
%type <rc>enumSubstatement
%type <rc>valueStatement
%type <rc>leafrefSpec
%type <rc>identityrefSpec
%type <rc>baseStatement
%type <rc>bitsSpec
%type <rc>bitsStatement
%type <rc>bitsSubstatementSpec
%type <rc>bitsSubstatement_0n
%type <rc>bitsSubstatement
%type <rc>positionStatement
%type <rc>unionSpec
%type <rc>stmtEnd
%type <rc>unknownStatement0_n
%type <rc>unknownStatement
%type <rc>errorAppTagStatement
%type <rc>errorMessageStatement
%type <rc>path
%type <rc>requireInstanceStatement
%type <rc>pattern
%type <rc>status
%type <rc>statusStatement
%type <rc>configStatement
%type <rc>defaultStatement
%type <rc>leafStatement
%type <rc>leafSubstatement
%type <rc>leafSubstatement_0n
%type <rc>leaf_listStatement
%type <rc>leaf_listSubstatement
%type <rc>leaf_listSubstatement_0n
%type <rc>listStatement
%type <rc>listSubstatement
%type <rc>listSubstatement_0n
%type <rc>min_elementsStatement
%type <rc>max_elementsStatement
%type <rc>ordered_byStatement
%type <rc>keyStatement
%type <rc>uniqueStatement
%type <rc>choiceStatement
%type <rc>choiceSpec
%type <rc>choiceSubstatement
%type <rc>choiceSubstatement_0n
%type <rc>caseStatement
%type <rc>caseSpec
%type <rc>caseSubstatement
%type <rc>caseSubstatement_0n
%type <rc>caseDataDef
%type <rc>groupingStatement
%type <rc>groupingSubstatement
%type <rc>groupingSubstatement_0n
%type <rc>usesStatement
%type <rc>usesSubstatement
%type <rc>usesSubstatement_0n
%type <rc>refineStatement
%type <rc>refineSpec
%type <rc>refineSubstatement
%type <rc>refine
%type <rc>refine_0n
%type <rc>augmentStatement
%type <rc>augmentSubstatement
%type <rc>augmentSubstatement_0n
%type <rc>whenStatement
%type <rc>rpcStatement
%type <rc>rpcSpec
%type <rc>rpcSubstatement
%type <rc>rpcSubstatement_0n
%type <rc>inputStatement
%type <rc>inputOutputSubstatement
%type <rc>inputOutputSubstatement_0n
%type <rc>notificationStatement
%type <rc>notificationSpec
%type <rc>notificationSubstatement
%type <rc>notificationSubstatement_0n
%type <rc>anyXMLStatement
%type <rc>anyXMLSpec
%type <rc>anyXMLSubstatement
%type <rc>anyXMLSubstatement_0n
%type <rc>deviationStatement
%type <rc>deviationSubstatement_0n
%type <rc>deviationSubstatement
%type <rc>deviateNotSupported
%type <rc>deviateAddStatement
%type <rc>deviateAddSpec
%type <rc>deviateAddSubstatement_0n
%type <rc>deviateAddSubstatement
%type <rc>deviateDeleteStatement
%type <rc>deviateDeleteSpec
%type <rc>deviateDeleteSubstatement_0n
%type <rc>deviateDeleteSubstatement
%type <rc>deviateReplaceStatement
%type <rc>deviateReplaceSpec
%type <rc>deviateReplaceSubstatement_0n
%type <rc>deviateReplaceSubstatement

%%

/*
 * Yacc rules.
 *
 */


/*
 * One mibFile may contain multiple MIB modules.
 * It's also possible that there's no module in a file.
 */
yangFile:		moduleStatement
        |
                submoduleStatement
        ;

moduleStatement:	moduleKeyword identifierStr
			{
                currentParser = thisParserPtr;
                thisParserPtr->yangModulePtr = findYangModuleByName($2, NULL);
			    if (!thisParserPtr->yangModulePtr) {
                    thisParserPtr->yangModulePtr =  addYangNode($2, YANG_DECL_MODULE, NULL);
                    
                    if (smiHandle->firstYangModulePtr) {
                        smiHandle->firstYangModulePtr->nextSiblingPtr = thisModulePtr;
                    } else {
                        smiHandle->firstYangModulePtr = thisModulePtr;
                    }
			    } else {
			        smiPrintError(thisParserPtr, ERR_MODULE_ALREADY_LOADED, $2);
                    free($2);
                    /*
                     * this aborts parsing the whole file,
                     * not only the current module.
                     */
                    YYABORT;
			    }
                thisModulePtr->info = createModuleInfo(thisModulePtr);
                pushNode(thisModulePtr);
			}
			'{'
                stmtSep
				moduleHeaderStatement_0n
				linkageStatement_0n
				moduleMetaStatement_0n
				revisionStatement_0n
				bodyStatement_0n			
			'}'
			{
                thisModuleInfoPtr->parsingState  = YANG_PARSING_DONE;
                pop();
                semanticAnalysis(thisModulePtr);
			}
	;

submoduleStatement:	submoduleKeyword identifierStr
			{
                currentParser = thisParserPtr;
                thisParserPtr->yangModulePtr = findYangModuleByName($2, NULL);
			    if (!thisParserPtr->yangModulePtr) {
                    thisParserPtr->yangModulePtr =  addYangNode($2, YANG_DECL_SUBMODULE, NULL);
                    
                    if (smiHandle->firstYangModulePtr) {
                        smiHandle->firstYangModulePtr->nextSiblingPtr = thisModulePtr;
                    } else {
                        smiHandle->firstYangModulePtr = thisModulePtr;
                    }
			    } else {
			        smiPrintError(thisParserPtr, ERR_MODULE_ALREADY_LOADED, $2);
                    free($2);
                    /*
                     * this aborts parsing the whole file,
                     * not only the current module.
                     */
                    YYABORT;
			    }
                currentParser = thisParserPtr;
                thisModulePtr->info = createModuleInfo(thisModulePtr);
                pushNode(thisModulePtr);
			}
			'{'
                stmtSep
				submoduleHeaderStatement
				linkageStatement_0n
				moduleMetaStatement_0n
				revisionStatement_0n
				bodyStatement_0n			
			'}'
			{
                thisModuleInfoPtr->parsingState  = YANG_PARSING_DONE;
                pop();
                semanticAnalysis(thisModulePtr);
			}
	;

moduleHeaderStatement_0n:	moduleHeaderStatement0_n
         	{
                if (!thisModuleInfoPtr->namespace) {
                    smiPrintError(parserPtr, ERR_NAMESPACE_MISSING, NULL);
                }
                if (!thisModuleInfoPtr->prefix) {
                    smiPrintError(parserPtr, ERR_PREFIX_MISSING, NULL);
                }
			}
		;

moduleHeaderStatement0_n:
                |
                    moduleHeaderStatement moduleHeaderStatement0_n
                ;

moduleHeaderStatement:	yangVersionStatement stmtSep
                |
                        namespaceStatement stmtSep
                |
                        prefixStatement stmtSep
		;

moduleMetaStatement_0n:	
		|
			moduleMetaStatement_0n moduleMetaStatement stmtSep
		;

moduleMetaStatement:	organizationStatement
		|
			contactStatement 
		|
			descriptionStatement
		|
			referenceStatement
		;

submoduleHeaderStatement:	belongsToStatement stmtSep
                            yangVersionStatement stmtSep
                |
                            yangVersionStatement stmtSep
                            belongsToStatement stmtSep
                |
                            belongsToStatement stmtSep
                ;


belongsToStatement: belongs_toKeyword identifierStr 
                    {
                        node = addYangNode($2, YANG_DECL_BELONGS_TO, topNode());
                        pushNode(node);
                    }
                    '{'
                        stmtSep prefixStatement stmtSep 
                    '}'
                    {
                        pop();
                    }
                    ;

linkageStatement_0n:
		|
			linkageStatement_0n linkageStatement
		;

linkageStatement:	includeStatement stmtSep
            |
                    importStatement stmtSep
		;

revisionStatement_0n:
		|
			revisionStatement_0n revisionStatement stmtSep
		;

bodyStatement_0n:	
		|
			bodyStatement_0n bodyStatement stmtSep
		;

bodyStatement:		extensionStatement
                |
                    featureStatement
                |
                    identityStatement
                |
                    typedefStatement
                |
                    groupingStatement
                |
                    dataDefStatement
                |
                    augmentStatement
                |
                    rpcStatement
                |
                    notificationStatement
                |
                    deviationStatement
                ;

dataDefStatement:	containerStatement
                |
                    leafStatement
                |
                    leaf_listStatement
                |
                    listStatement
                |
                    choiceStatement
                |
                    anyXMLStatement
                |
                    usesStatement
                ;

commonStatement:	descriptionStatement 
                |
                    statusStatement
                |
                    referenceStatement
                |
                    configStatement
                ;	       

organizationStatement:	organizationKeyword string stmtEnd
			{
                if (!thisModuleInfoPtr->organization) {
                    node = addYangNode($2, YANG_DECL_ORGANIZATION, topNode());
                    thisModuleInfoPtr->organization = node->export.value;
                } else {
                    smiPrintError(currentParser, ERR_REDEFINED_ORGANIZATION, NULL);
                }				
			}
	;

contactStatement:	contactKeyword string stmtEnd
			{
                if (!thisModuleInfoPtr->contact) {
                    node = addYangNode($2, YANG_DECL_CONTACT, topNode());
                    thisModuleInfoPtr->contact = node->export.value;
                } else {
                    smiPrintError(currentParser, ERR_REDEFINED_CONTACT, NULL);
                }
			}
	;

descriptionStatement:	descriptionKeyword string stmtEnd
			{
                uniqueNodeKind(topNode(), YANG_DECL_DESCRIPTION);
                setDescription(topNode(), $2);
                node = addYangNode($2, YANG_DECL_DESCRIPTION, topNode());
			}
	;

referenceStatement:	referenceKeyword string stmtEnd
			{
                uniqueNodeKind(topNode(), YANG_DECL_REFERENCE);
                setReference(topNode(), $2);
                node = addYangNode($2, YANG_DECL_REFERENCE, topNode());
			}
	;

statusStatement:	statusKeyword status stmtEnd
			{
                uniqueNodeKind(topNode(), YANG_DECL_STATUS);
                setStatus(topNode(), $2);
                node = addYangNode(statusKeywords[$2], YANG_DECL_STATUS, topNode());
			}
	;

namespaceStatement:	namespaceKeyword string stmtEnd
		  	{
                if (!thisModuleInfoPtr->namespace) {
                    node = addYangNode($2, YANG_DECL_NAMESPACE, topNode());
                    thisModuleInfoPtr->namespace = node->export.value;
                } else {
                    smiPrintError(currentParser, ERR_REDEFINED_NAMESPACE, NULL);
                }
			}
	;

yangVersionStatement:  yangversionKeyword yangVersion stmtEnd
		  	{
                if (!thisModuleInfoPtr->version) {
                    node = addYangNode($2, YANG_DECL_YANGVERSION, topNode());
                    thisModuleInfoPtr->version = node->export.value;
                } else {
                    smiPrintError(currentParser, ERR_REDEFINED_YANGVERSION, NULL);
                }
			}
	;

status:		deprecatedKeyword
		{
			$$ = YANG_STATUS_DEPRECATED;
		}
	|
		currentKeyword
		{
			$$ = YANG_STATUS_CURRENT;
		}
	|
		obsoleteKeyword
		{
			$$ = YANG_STATUS_OBSOLETE;
		}
	;

prefixStatement:	prefixKeyword prefix stmtEnd
			{
                node = addYangNode($2, YANG_DECL_PREFIX, topNode());
                switch(topDecl())
                {
                    case YANG_DECL_MODULE:
                        if(!thisModuleInfoPtr->prefix)
                            thisModuleInfoPtr->prefix = node->export.value;
                        else 
                            smiPrintError(currentParser, ERR_REDEFINED_PREFIX, NULL);
                        break;
                    case YANG_DECL_BELONGS_TO:
                        thisModuleInfoPtr->prefix = node->export.value;
                    case YANG_DECL_IMPORT:
                    
                        break;
                    default:
                        //TODO print error
                        debug("DEBUGG: OOPS wrong prefix DECL %d, at line %d\n", topDecl(), currentParser->line);
                        break;			
                }
			}
	;

revisionStatement:	revisionKeyword date ';' 
			{
                node = addYangNode($2, YANG_DECL_REVISION, topNode());
            }
        |
                revisionKeyword date
			{
                node = addYangNode($2, YANG_DECL_REVISION, topNode());
				pushNode(node);
			}
			'{'
                stmtSep
				revisionDescriptionStatement_0n
			'}'
			{
				pop();
			}
    ;


revisionDescriptionStatement_0n:
                            |
                                    revisionDescriptionStatement_0n revisionDescriptionStatement stmtSep
                            ;


revisionDescriptionStatement: 
                    |
                        referenceStatement
                    |
                        descriptionStatement
                    ;

date: 	dateString
	{
        checkDate(currentParser, $1);
	}
	;

importStatement: importKeyword identifierStr
		{
            node = addYangNode($2, YANG_DECL_IMPORT, topNode());
			pushNode(node);
		}
		'{'
            stmtSep
			prefixStatement stmtSep
            optionalRevision
		'}'
		{            
            externalModule(topNode());
			pop();
            while (topNode() != thisModulePtr) {
                pop();
            }
		}
        ;

optionalRevision:
                    |
                      revisionStatement stmtSep 
                    ;

includeStatement: includeKeyword identifierStr
		{
            node = addYangNode($2, YANG_DECL_INCLUDE, topNode());
			pushNode(node);
		}
                includeStatementBody
		{
            _YangNode *includedModule = externalModule(topNode());
            validateInclude(thisModulePtr, includedModule);
			pop();
            while (topNode() != thisModulePtr) {
                pop();
            }
		}
        ;

includeStatementBody:         ';'
                |
                    '{'
                        stmtSep
                        optionalRevision
                    '}'
                ;

featureStatement: featureKeyword identifierStr
                {
                    node = addYangNode($2, YANG_DECL_FEATURE, topNode());
                    pushNode(node);
                }
                  featureSpec
                {
                    pop();
                }
                ;

featureSpec:    ';'
            |   
                '{'
                        stmtSep
                        featureSubstatement_0n
                '}'
                ;

featureSubstatement_0n:
                |
                   featureSubstatement_0n featureSubstatement stmtSep
                ;

featureSubstatement:    ifFeatureStatement
                    |
                        statusStatement
                    |
                        descriptionStatement
                    |   
                        referenceStatement;

ifFeatureStatement: ifFeatureKeyword identifierRef stmtEnd
                    {
                        node = addYangNode($2, YANG_DECL_IF_FEATURE, topNode());
                        createIdentifierRef(node, getPrefix($2), getIdentifier($2));
                    }
                    ;

identityStatement: identityKeyword identifierStr 
                {
                    node = addYangNode($2, YANG_DECL_IDENTITY, topNode());
                    pushNode(node);
                }
                   identitySpec
                {
                    pop();
                };

identitySpec:    ';'
            |   
                '{'
                        stmtSep
                        identitySubstatement_0n
                '}'
                ;

identitySubstatement_0n:
                |
                   identitySubstatement_0n identitySubstatement stmtSep
                ;

identitySubstatement:   baseStatement
                    |
                        statusStatement
                    |
                        descriptionStatement
                    |   
                        referenceStatement;

typedefStatement:   typedefKeyword identifierStr
                {
                    if (getBuiltInType($2) != YANG_TYPE_NONE) {
                        smiPrintError(thisParserPtr, ERR_ILLEGAL_TYPE_NAME, $2);
                    }
                    node = addYangNode($2, YANG_DECL_TYPEDEF, topNode());
                    pushNode(node);
                }
                '{'
                    stmtSep
                    typedefSubstatement_0n
                '}'
                {                                
                    if (getCardinality(topNode(), YANG_DECL_TYPE) != 1) {
                        smiPrintError(currentParser, ERR_WRONG_CARDINALITY, yandDeclKeyword[YANG_DECL_TYPE], "1");
                    }
                    pop();
                }
        ;

typedefSubstatement_0n:	
                |
                       typedefSubstatement_0n typedefSubstatement stmtSep
                ;				

typedefSubstatement:
                        typeStatement
                    |
                        unitsStatement
                    |
                        defaultStatement
                    |
                        statusStatement
                    |
                        referenceStatement
                    |
                        descriptionStatement
                ;

typeStatement: typeKeyword identifierRef 
               {
                    node = addYangNode($2, YANG_DECL_TYPE, topNode());
                    createTypeInfo(node);
                    if (getBuiltInType($2) == YANG_TYPE_NONE) {
                        createIdentifierRef(node, getPrefix($2), getIdentifier($2));
                    }
                    pushNode(node);
               }
               optionalTypeBodyStatements
               {
                    pop();
               }
            ;

optionalTypeBodyStatements: ';'
                        |   
                            '{'
                                    stmtSep
                                    typeBodyStmts
                            '}'
                        ;

typeBodyStmts:  numRestriction
            |
                stringRestriction_0n
            |
                enumSpec
            |
                leafrefSpec
            |
                identityrefSpec
            |
                bitsSpec
            |
                unionSpec
            ;

numRestriction: range stmtSep;

range:	rangeKeyword string
		{
            node = addYangNode($2, YANG_DECL_RANGE, topNode());
            pushNode(node);
		}
		optionalRestrictionSpec
		{
			pop();
		}
	;


stringRestriction_0n:
                |
                    stringRestriction_0n stringRestriction
                ;

stringRestriction: length
                |
                   pattern
                ;

length:	lengthKeyword string
		{
            uniqueNodeKind(topNode(), YANG_DECL_LENGTH);
            node = addYangNode($2, YANG_DECL_LENGTH, topNode());
            pushNode(node);
        }
        optionalRestrictionSpec
        {
			pop();
		}
	;


pattern:	patternKeyword string 
        {
            node = addYangNode($2, YANG_DECL_PATTERN, topNode());
            pushNode(node);
        }
        optionalRestrictionSpec
        {
			pop();
		}
	;

enumSpec:   enum stmtSep
        |
            enumSpec enum stmtSep
        ;

enum:   enumKeyword string 
        {
            if (strlen($2) == 0 || isWSP($2[0]) || isWSP($2[strlen($2)-1])) {
                smiPrintError(currentParser, ERR_WRONG_ENUM);
            }
            node = findChildNodeByTypeAndValue(topNode(), YANG_DECL_ENUM, $2);
            if (node) {
                smiPrintError(currentParser, ERR_DUPLICATED_ENUM_NAME, $2);
            }
            node = addYangNode($2, YANG_DECL_ENUM, topNode());
            pushNode(node);
        }
        enumSubstatementSpec
        {
            pop();
        }
	;

enumSubstatementSpec: ';'
        |
          '{'
                stmtSep
                enumSubstatement_0n
          '}';

enumSubstatement_0n: 
                |
                    enumSubstatement_0n enumSubstatement stmtSep
                ;

enumSubstatement:    valueStatement
                |
                     statusStatement
                |
                     descriptionStatement
                |
                     referenceStatement
        ;

valueStatement: valueKeyword string stmtEnd 
            {                
                node = addYangNode($2, YANG_DECL_VALUE, topNode());
            }
            ;

optionalRestrictionSpec: ';' 
            |
                 '{'
                        stmtSep
                        restrictionSpec_0n
                 '}'
            ;

restrictionSpec_0n: 
                |
                    restrictionSpec_0n restrictionSpec stmtSep
                ;

restrictionSpec: descriptionStatement
            |
                 referenceStatement
            |
                 errorMessageStatement
            |
                 errorAppTagStatement
            ;

errorMessageStatement: error_messageKeyword string stmtEnd
            {
                uniqueNodeKind(topNode(), YANG_DECL_ERROR_MESSAGE);
                node = addYangNode($2, YANG_DECL_ERROR_MESSAGE, topNode());
            }
            ;

errorAppTagStatement: error_app_tagKeyword string stmtEnd
            {
                uniqueNodeKind(topNode(), YANG_DECL_ERROR_APP_TAG);
                node = addYangNode($2, YANG_DECL_ERROR_APP_TAG, topNode());
            }
        ;

leafrefSpec: path stmtSep
        |
             path stmtSep
             requireInstanceStatement stmtSep
        |
             requireInstanceStatement stmtSep
             path stmtSep
        ;

path:	pathKeyword string stmtEnd
		{
            uniqueNodeKind(topNode(), YANG_DECL_PATH);
            node = addYangNode($2, YANG_DECL_PATH, topNode());
		}
	;

requireInstanceStatement: requireInstanceKeyword booleanValue stmtEnd
        {
            uniqueNodeKind(topNode(), YANG_DECL_REQUIRE_INSTANCE);
            if ($2 == YANG_BOOLEAN_TRUE) {
                node = addYangNode("true", YANG_DECL_REQUIRE_INSTANCE, topNode());
            } else {
                node = addYangNode("false", YANG_DECL_REQUIRE_INSTANCE, topNode());
            }
        }
        ;

identityrefSpec: baseStatement stmtSep
            |
                 baseStatement stmtSep
                 requireInstanceStatement stmtSep
            |
                 requireInstanceStatement stmtSep
                 baseStatement stmtSep
            ;

baseStatement: baseKeyword identifierRef stmtEnd 
            {
                uniqueNodeKind(topNode(), YANG_DECL_BASE);
                node = addYangNode($2, YANG_DECL_BASE, topNode());
                createIdentifierRef(node, getPrefix($2), getIdentifier($2));
            }
            ;

bitsSpec:   bitsStatement stmtSep
        |
            bitsSpec bitsStatement stmtSep
        ;

bitsStatement: bitKeyword identifier 
            {
                node = findChildNodeByTypeAndValue(topNode(), YANG_DECL_BIT, $2);
                if (node) {
                    smiPrintError(currentParser, ERR_DUPLICATED, "bit", $2);
                }

                node = addYangNode($2, YANG_DECL_BIT, topNode());
                pushNode(node);
            }
            bitsSubstatementSpec
            {
                pop();
            }
            ;

bitsSubstatementSpec:   ';'
            |
               '{'
                    stmtSep
                    bitsSubstatement_0n
               '}';

bitsSubstatement_0n:
        |
           bitsSubstatement_0n bitsSubstatement stmtSep
        ;

bitsSubstatement:   positionStatement
                |
                    descriptionStatement
                |
                    referenceStatement
                |
                    statusStatement
                ;

positionStatement: positionKeyword string stmtEnd 
                {
                    if (!isNonNegativeInteger($2)) {                            
                        smiPrintError(currentParser, ERR_ARG_VALUE, $2, "non-negative-integer");
                    }                    
                    uniqueNodeKind(topNode(), YANG_DECL_POSITION);

                    /* position values must be unique within the type */
                    _YangNode* typePtr = topNode()->parentPtr;
                    _YangNode* childPtr = typePtr->firstChildPtr;
                    while (childPtr) {
                        if (childPtr->export.nodeKind == YANG_DECL_BIT) {
                            _YangNode* positionPtr = findChildNodeByTypeAndValue(childPtr, YANG_DECL_POSITION, $2);
                            if (positionPtr) {
                                smiPrintError(currentParser, ERR_DUPLICATED, "position", $2);
                            }
                        }
                        childPtr = childPtr->nextSiblingPtr;
                    }

                    node = addYangNode($2, YANG_DECL_POSITION, topNode());
                }
                ;

unionSpec: typeStatement stmtSep
        |
           unionSpec typeStatement stmtSep
        ;

stmtEnd:    ';'
         |
            '{'
                    unknownStatement0_n
            '}'
        ;

stmtSep:
	|
        unknownStatement0_n
	;

unknownStatement0_n:
	|
        unknownStatement unknownStatement0_n;
	;

unknownStatement:   identifierRefArg 
                    {
                        node = addYangNode($1, YANG_DECL_UNKNOWN_STATEMENT, topNode());
                        createIdentifierRef(node, getPrefix($1), getIdentifier($1));
                        pushNode(node);
                    }
                    stmtEnd 
                    {
                        pop();
                    }
        |                    
                    identifierRefArg string
                    {
                        node = addYangNode($1, YANG_DECL_UNKNOWN_STATEMENT, topNode());
                        createIdentifierRef(node, getPrefix($1), getIdentifier($1));
                        node->export.extra = smiStrdup($2);
                        pushNode(node);
                    }
                    stmtEnd 
                    {
                        pop();
                    }
        ;

containerStatement: containerKeyword identifierStr
			{
                node = addYangNode($2, YANG_DECL_CONTAINER, topNode());				
                pushNode(node);
			}
			'{'
				containerSubstatement_0n
			'}'
			{
				pop();
			}
	;


containerSubstatement_0n:	
		|
		       containerSubstatement containerSubstatement_0n
	;

containerSubstatement:	ifFeatureStatement
                    |
                        commonStatement
                    |
                        dataDefStatement
                    |
                        groupingStatement
                    |
                        mustStatement
                    |
                        whenStatement
                    |
                        presenceStatement
                    |
                        typedefStatement
                    ;

mustStatement: mustKeyword string
		{
            node = addYangNode($2, YANG_DECL_MUST_STATEMENT, topNode());
            pushNode(node);
		}
		'{'
			mustSubstatement_0n
		'}'
		{
			pop();
		}
	|
		mustKeyword string ';'
		{
            node = addYangNode($2, YANG_DECL_MUST_STATEMENT, topNode());
		}
	;

mustSubstatement_0n:
            |
		       mustSubstatement_0n mustSubstatement stmtSep
        ;

mustSubstatement:	errorMessageStatement
                |
                    errorAppTagStatement
                |
                    descriptionStatement
                |
                    referenceStatement
                ;

presenceStatement: presenceKeyword string stmtEnd
                {
                    uniqueNodeKind(topNode(), YANG_DECL_PRESENCE);
                    node = addYangNode($2, YANG_DECL_PRESENCE, topNode());
                }
                ;
			
configStatement: 	configKeyword trueKeyword stmtEnd
                {
                    uniqueNodeKind(topNode(), YANG_CONFIG_TRUE);
                    setConfig(topNode(), YANG_CONFIG_TRUE);
                    node = addYangNode("true", YANG_DECL_CONFIG, topNode());
                }
            |
                    configKeyword falseKeyword stmtEnd
                {
                    uniqueNodeKind(topNode(), YANG_CONFIG_TRUE);
                    setConfig(topNode(), YANG_CONFIG_FALSE);
                    node = addYangNode("false", YANG_DECL_CONFIG, topNode());
                }
            ;

mandatoryStatement: mandatoryKeyword trueKeyword stmtEnd
			{
                uniqueNodeKind(topNode(), YANG_DECL_MANDATORY);
                node = addYangNode("true", YANG_DECL_MANDATORY, topNode());
			}
		|
		    	mandatoryKeyword falseKeyword stmtEnd	
			{
                uniqueNodeKind(topNode(), YANG_DECL_MANDATORY);
				node = addYangNode("false", YANG_DECL_MANDATORY, topNode());
			}
		;
			
leafStatement: leafKeyword identifierStr
			{
				node = addYangNode($2, YANG_DECL_LEAF, topNode());
                pushNode(node);
			}
			'{'
                stmtSep
				leafSubstatement_0n
			'}'
			{
                if (getCardinality(topNode(), YANG_DECL_TYPE) != 1) {
                    smiPrintError(currentParser, ERR_WRONG_CARDINALITY, yandDeclKeyword[YANG_DECL_TYPE], "1");
                }
				pop();
			}
		;
			
leafSubstatement_0n:
            |
                   leafSubstatement_0n leafSubstatement stmtSep
        ;

leafSubstatement:	ifFeatureStatement
                |
                    unitsStatement
                |
                    mustStatement
                |
                    commonStatement
                |
                    mandatoryStatement
                |
                    typeStatement
                |
                    defaultStatement	
                |
                    whenStatement
                ;

leaf_listStatement: leaf_listKeyword identifierStr
			{
				node = addYangNode($2, YANG_DECL_LEAF_LIST, topNode());
                pushNode(node);
			}
			'{'
                stmtSep
				leaf_listSubstatement_0n
			'}'
			{
                if (getCardinality(topNode(), YANG_DECL_TYPE) != 1) {
                    smiPrintError(currentParser, ERR_WRONG_CARDINALITY, yandDeclKeyword[YANG_DECL_TYPE], "1");
                }
				pop();
			}
		;
			
leaf_listSubstatement_0n:
		|
		       leaf_listSubstatement_0n leaf_listSubstatement stmtSep
        ;

leaf_listSubstatement:	mustStatement
                    |
                        commonStatement
                    |
                        typeStatement
                    |
                        ifFeatureStatement
                    |
                        unitsStatement
                    |	
                        whenStatement
                    |	
                        max_elementsStatement
                    |
                        min_elementsStatement
                    |
                        ordered_byStatement
                    ;
		
listStatement: listKeyword identifierStr
			{
				node = addYangNode($2, YANG_DECL_LIST, topNode());
                pushNode(node);
			}
			'{'
                stmtSep
				listSubstatement_0n
			'}'
			{
				pop();
			}
		;

listSubstatement_0n:
            |
		       listSubstatement_0n listSubstatement stmtSep
        	;

listSubstatement:	mustStatement
                |
                    commonStatement
                |
                    max_elementsStatement
                |
                    min_elementsStatement
                |
                    ordered_byStatement
                |
                    keyStatement
                |
                    uniqueStatement
                |
                    dataDefStatement
                |
                    typedefStatement
                |
                    groupingStatement
                |
                    whenStatement
                |
                    ifFeatureStatement
                ;

max_elementsStatement: 	max_elementsKeyword string stmtEnd
                    {
                        if (!isNonNegativeInteger($2)) {                            
                            smiPrintError(currentParser, ERR_ARG_VALUE, $2, "non-negative-integer");
                        }
                        uniqueNodeKind(topNode(), YANG_DECL_MAX_ELEMENTS);
                        node = addYangNode($2, YANG_DECL_MAX_ELEMENTS, topNode());
                    }
                ;

min_elementsStatement: 	min_elementsKeyword string stmtEnd
                    {
                        if (!isNonNegativeInteger($2)) {
                            smiPrintError(currentParser, ERR_ARG_VALUE, $2, "non-negative-integer");
                        }
                        uniqueNodeKind(topNode(), YANG_DECL_MIN_ELEMENTS);
                        node = addYangNode($2, YANG_DECL_MIN_ELEMENTS, topNode());
                    }
        		;

ordered_byStatement: 	ordered_byKeyword string stmtEnd
                    {
                        if (!strcmp($2, "user") || !strcmp($2, "system")) {
                        } else {
                            smiPrintError(thisParserPtr, ERR_IVALIDE_ORDERED_BY_VALUE, $2);
                        }
                        uniqueNodeKind(topNode(), YANG_DECL_ORDERED_BY);
                        node = addYangNode($2, YANG_DECL_ORDERED_BY, topNode());
                    }
                ;

keyStatement: keyKeyword string stmtEnd
		{
            uniqueNodeKind(topNode(), YANG_DECL_KEY);
            node = addYangNode($2, YANG_DECL_KEY, topNode());
            node->info = getKeyList($2);
		}
	;
	
uniqueStatement: uniqueKeyword string stmtEnd
		{
            node = addYangNode($2, YANG_DECL_UNIQUE, topNode());
            YangList *il = getUniqueList($2);
            node->info = processUniqueList(node, il);
            freeIdentiferList(il);
		}
	;

choiceStatement: choiceKeyword identifierStr
		{
            node = addYangNode($2, YANG_DECL_CHOICE, topNode());
            pushNode(node);
		}
        choiceSpec
		{
			pop();
		}
		;
 
choiceSpec: ';'
        |
            '{'
                stmtSep
                choiceSubstatement_0n			
            '}'
        ;

choiceSubstatement_0n:
		|
		       choiceSubstatement_0n choiceSubstatement stmtSep
        ;

choiceSubstatement:	commonStatement
                  |
                    defaultStatement
                  |
                    whenStatement
                  |
                    ifFeatureStatement
                  |
                    mandatoryStatement
                  |
                    caseStatement
                ;

caseStatement: 	caseKeyword identifierStr
		{
            node = findChildNodeByTypeAndValue(topNode(), YANG_DECL_CASE, $2);
            if (node) {
                smiPrintError(currentParser, ERR_DUPLICATED_CASE_IDENTIFIER, $2);
            }
            node = addYangNode($2, YANG_DECL_CASE, topNode());
            pushNode(node);
		}
        caseSpec
		{
			pop();
		}	
	|
        containerStatement
    |
        leafStatement
    |
        leaf_listStatement
    |
        listStatement
    |
        anyXMLStatement
	;

caseSpec:   ';'
        |
            '{'
                stmtSep
                caseSubstatement_0n
            '}'
        ;

caseSubstatement_0n:
            |
                caseSubstatement_0n caseSubstatement  stmtSep
            ;

caseSubstatement: 	descriptionStatement
                |
                    statusStatement
                |
                    referenceStatement
                |
                    whenStatement
                |
                    ifFeatureStatement
                |
                    caseDataDef
                ;


caseDataDef:    containerStatement
            |
                leafStatement
            |
                leaf_listStatement
            |
                listStatement
            |
                anyXMLStatement
            |
                usesStatement
            ;


groupingStatement: groupingKeyword identifierStr
		{
            node = addYangNode($2, YANG_DECL_GROUPING, topNode());
            pushNode(node);
		}
		'{'
            stmtSep
			groupingSubstatement_0n
		'}'
		{
			pop();
		}
		;

groupingSubstatement_0n:	
		|
                groupingSubstatement_0n groupingSubstatement stmtSep
        ;

groupingSubstatement:	statusStatement
                    |
                        descriptionStatement
                    |
                        referenceStatement
                    |
                        dataDefStatement
                    |
                        groupingStatement
                    |
                        typedefStatement
                    ;

usesStatement:  usesKeyword identifierRef
            {
                node = addYangNode($2, YANG_DECL_USES, topNode());
                createIdentifierRef(node, getPrefix($2), getIdentifier($2));
                pushNode(node);
            }
            '{'
            usesSubstatement_0n
            '}'
            {
                pop();
            }
        |
                usesKeyword identifierRef
            {
                node = addYangNode($2, YANG_DECL_USES, topNode());
                createIdentifierRef(node, getPrefix($2), getIdentifier($2));
            }
            ';'
            ;

usesSubstatement_0n:
		|
		       usesSubstatement_0n usesSubstatement stmtSep
        ;

usesSubstatement:	descriptionStatement
                |
                    referenceStatement
                |
                    statusStatement
                |
                    whenStatement
                |   
                    ifFeatureStatement
                | 
                    refineStatement
                |
                    augmentStatement
                    
		;

refineStatement:    refineKeyword string
            {
                if (!isDescendantSchemaNodeid($2)) {
                    smiPrintError(thisParserPtr, ERR_DESCEDANT_FORM, $2);
                }
                node = addYangNode($2, YANG_DECL_REFINE, topNode());
                pushNode(node);
            }
                    refineSpec
            {
                pop();
            }
            ;

refineSpec: ';'
        |   
            '{'
                stmtSep
                refineSubstatement
            '}'
        ;

refineSubstatement: refine_0n;

refine_0n: 
                |
                    refine_0n refine stmtSep;

refine: mustStatement
    |
        presenceStatement
    |
        configStatement
    |
        descriptionStatement
    |
        referenceStatement
    |
        defaultStatement
    |
        mandatoryStatement
    |
        min_elementsStatement
    |
        max_elementsStatement
    |
        defaultStatement
    ;

augmentStatement: augmentKeyword string 
		{
            if (topDecl() == YANG_DECL_USES) {
                if (!isDescendantSchemaNodeid($2)) {
                    smiPrintError(thisParserPtr, ERR_DESCEDANT_FORM, $2);
                }
            } else {
                if (!isAbsoluteSchemaNodeid($2)) {
                    smiPrintError(thisParserPtr, ERR_ABSOLUTE_FORM, $2);
                }
            }
            node = addYangNode($2, YANG_DECL_AUGMENT, topNode());
            pushNode(node);
		}
		'{'
            stmtSep
			augmentSubstatement_0n
    	'}'
		{
            node = topNode()->firstChildPtr;
            int count = 0;
            while (node) {
                if (node->export.nodeKind == YANG_DECL_CASE ||
                    isDataDefNode(node)) {
                        count++;
                }
                node = node->nextSiblingPtr;
            }
            if (count == 0) {
                smiPrintError(thisParserPtr, ERR_DATADEF_NODE_REQUIRED, $2);
            }

			pop();
		}
		;

augmentSubstatement_0n:	augmentSubstatement stmtSep
                |
                        augmentSubstatement_0n augmentSubstatement stmtSep
                ;

augmentSubstatement:	whenStatement
                    |
                        ifFeatureStatement
                    |
                        descriptionStatement
                    |
                        referenceStatement
                    |
                        statusStatement
                    |
                        dataDefStatement
                    |
                        caseStatement
                    ;


whenStatement:	whenKeyword string stmtEnd
	    {
            uniqueNodeKind(topNode(), YANG_DECL_WHEN);
            node = addYangNode($2, YANG_DECL_WHEN, topNode());
        }
		;

rpcStatement: rpcKeyword identifierStr
		{
            node = addYangNode($2, YANG_DECL_RPC, topNode());
            pushNode(node);
		}
              rpcSpec
		{
			pop();
		}
		;

rpcSpec:    ';'
        |
            '{'
                stmtSep
                rpcSubstatement_0n
            '}'
        ;

rpcSubstatement_0n:	
		|
		       rpcSubstatement_0n rpcSubstatement  stmtSep
	;

rpcSubstatement:	ifFeatureStatement
                |
                    descriptionStatement
                |
                    referenceStatement
                |
                    statusStatement
                |
                    typedefStatement
                |
                    groupingStatement
                |
                    inputStatement
                |
                    outputStatement
            ;

inputStatement: inputKeyword
		{
            node = addYangNode(NULL, YANG_DECL_INPUT, topNode());
            pushNode(node);
		}
		'{'
            stmtSep
			inputOutputSubstatement_0n
		'}'
		{
            int numberDataDefStmts = 0;
            _YangNode *childPtr = topNode()->firstChildPtr;
            while (childPtr) {
                if (!(childPtr->export.nodeKind == YANG_DECL_TYPEDEF ||
                    childPtr->export.nodeKind == YANG_DECL_GROUPING)) {
                        numberDataDefStmts++;
                    }
                childPtr = childPtr->nextSiblingPtr;
            }
            if (!numberDataDefStmts) {
                smiPrintError(thisParserPtr, ERR_DATA_DEF_REQUIRED, "input");            
            }
			pop();
		}
		;

inputOutputSubstatement_0n:	inputOutputSubstatement stmtSep
                |
                        inputOutputSubstatement_0n inputOutputSubstatement stmtSep
            	;

inputOutputSubstatement:	dataDefStatement
                        |
                            groupingStatement
                        |
                            typedefStatement
                        ;

outputStatement: outputKeyword
		{
            node = addYangNode(NULL, YANG_DECL_OUTPUT, topNode());
            pushNode(node);
		}
		'{'
            stmtSep
			inputOutputSubstatement_0n
		'}'
		{
            int numberDataDefStmts = 0;
            _YangNode *childPtr = topNode()->firstChildPtr;
            while (childPtr) {
                if (!(childPtr->export.nodeKind == YANG_DECL_TYPEDEF ||
                    childPtr->export.nodeKind == YANG_DECL_GROUPING)) {
                        numberDataDefStmts++;
                    }
                childPtr = childPtr->nextSiblingPtr;
            }
            if (!numberDataDefStmts) {
                smiPrintError(thisParserPtr, ERR_DATA_DEF_REQUIRED, "input");            
            }
			pop();
		}
		;


notificationStatement: notificationKeyword identifierStr
                {
                    node = addYangNode($2, YANG_DECL_NOTIFICATION, topNode());
                    pushNode(node);
                }
                        notificationSpec
                {
                    pop();
                }
                ;

notificationSpec:   ';'
                |
                    '{'
                        stmtSep
                        notificationSubstatement_0n
                    '}'
                    ;


notificationSubstatement_0n:
		|
		       notificationSubstatement_0n notificationSubstatement stmtSep
	;

notificationSubstatement:	ifFeatureStatement
                        |
                            descriptionStatement
                        |
                            referenceStatement
                        |
                            statusStatement
                        |	
                            dataDefStatement
                        |
                            groupingStatement
                        |
                            typedefStatement
                        ;

deviationStatement: deviationKeyword string 
                {
                        node = addYangNode($2, YANG_DECL_DEVIATION, topNode());
                        pushNode(node);
                }
                '{'
                    stmtSep
                    deviationSubstatement_0n
                '}'
                {
                    pop();
                }
                ;

deviationSubstatement_0n:   deviationSubstatement stmtSep
                        |
                            deviationSubstatement_0n deviationSubstatement stmtSep;

deviationSubstatement:  descriptionStatement
                    |
                        referenceStatement
                    |
                        deviateNotSupported
                    |
                        deviateAddStatement
                    |
                        deviateDeleteStatement
                    |
                        deviateReplaceStatement
                    ;

deviateNotSupported: deviateKeyword notSupportedKeyword ';'
                    {
                        uniqueNodeKind(topNode(), YANG_DECL_DEVIATE);
                        node = addYangNode("not-supported", YANG_DECL_DEVIATE, topNode());
                    }
                |
                     deviateKeyword notSupportedKeyword '{' stmtSep '}'
                    {
                        uniqueNodeKind(topNode(), YANG_DECL_DEVIATE);
                        node = addYangNode("not-supported", YANG_DECL_DEVIATE, topNode());
                    }
                ;

deviateAddStatement:    deviateKeyword addKeyword 
                    {
                        node = addYangNode("add", YANG_DECL_DEVIATE, topNode());
                        pushNode(node);
                    }
                        deviateAddSpec
                    {
                        pop();
                    }
                    ;

deviateAddSpec: ';'
            |
                '{'
                    stmtSep
                    deviateAddSubstatement_0n
                '}'
            ;

deviateAddSubstatement_0n:
                    |
                        deviateAddSubstatement_0n deviateAddSubstatement stmtSep;

deviateAddSubstatement:     unitsStatement
                    |
                            mustStatement
                    |
                            uniqueStatement
                    |
                            defaultStatement
                    |
                            configStatement
                    |
                            mandatoryStatement
                    |
                            min_elementsStatement
                    |
                            max_elementsStatement
                    ;

deviateDeleteStatement:    deviateKeyword deleteKeyword 
                    {
                        node = addYangNode("delete", YANG_DECL_DEVIATE, topNode());
                        pushNode(node);
                    }
                        deviateDeleteSpec
                    {
                        pop();
                    }
                    ;

deviateDeleteSpec: ';'
            |
                '{'
                    stmtSep
                    deviateDeleteSubstatement_0n
                '}'
            ;

deviateDeleteSubstatement_0n:
                    |
                        deviateDeleteSubstatement_0n deviateDeleteSubstatement stmtSep;

deviateDeleteSubstatement:      unitsStatement
                        |
                                mustStatement
                        |
                                uniqueStatement
                        |
                                defaultStatement;

deviateReplaceStatement: deviateKeyword replaceKeyword 
                    {
                        node = addYangNode("replace", YANG_DECL_DEVIATE, topNode());
                        pushNode(node);
                    }
                        deviateReplaceSpec
                    {
                        pop();
                    }
                    ;

deviateReplaceSpec: ';'
            |
                '{'
                    stmtSep
                    deviateReplaceSubstatement_0n
                '}'
            ;

deviateReplaceSubstatement_0n:
                    |
                        deviateReplaceSubstatement_0n deviateReplaceSubstatement stmtSep;

deviateReplaceSubstatement: typeStatement
                        |
                            unitsStatement
                        |
                            defaultStatement
                        |
                            configStatement
                        |
                            mandatoryStatement
                        |
                            min_elementsStatement
                        |
                            max_elementsStatement
                        ;


anyXMLStatement: anyXMLKeyword identifierStr
		{
            node = addYangNode($2, YANG_DECL_ANYXML, topNode());
            pushNode(node);
		}
        anyXMLSpec
		{
			pop();
		}
		;

anyXMLSpec: ';'
        |
            '{'
                stmtSep
                anyXMLSubstatement_0n
            '}'
        ;

anyXMLSubstatement_0n:
    		|
		       anyXMLSubstatement_0n anyXMLSubstatement stmtSep
            ;

anyXMLSubstatement:	commonStatement
                |
                    whenStatement
                |
                    ifFeatureStatement
                |   
                    mustStatement
                |
                    mandatoryStatement
                ;

extensionStatement: extensionKeyword identifierStr
		{
			node = addYangNode($2, YANG_DECL_EXTENSION, topNode());
            pushNode(node);
		}
                extensionStatementBody
		{
			pop();
		}
		
extensionStatementBody:  '{' stmtSep extensionSubstatement_0n '}'
                    |
                         ';'
                    ;                       

extensionSubstatement_0n:	
		|
            extensionSubstatement_0n extensionSubstatement stmtSep
		;

extensionSubstatement:	argumentStatement
                |
                        statusStatement
                |
                        descriptionStatement
                |
                        referenceStatement
                ;

argumentStatement:  argumentKeyword identifierStr
                    {
                        uniqueNodeKind(topNode(), YANG_DECL_ARGUMENT);
                        node = addYangNode($2, YANG_DECL_ARGUMENT, topNode());
                        pushNode(node);
                    }
                    argumentStatementBody
                    {
                        pop();
                    }
                ;

argumentStatementBody:  '{' stmtSep yinElementOptional '}'
                    |
                         ';'
                    ;                       

yinElementOptional: 
                |
                    yinElementKeyword trueKeyword stmtEnd stmtSep
                    {
                        addYangNode("true", YANG_DECL_YIN_ELEMENT, topNode());
                    }
                |
                    yinElementKeyword falseKeyword stmtEnd stmtSep
                    {
                        addYangNode("false", YANG_DECL_YIN_ELEMENT, topNode());
                    }
                ;

unitsStatement: unitsKeyword string stmtEnd
                {
                    uniqueNodeKind(topNode(), YANG_DECL_UNITS);
                    addYangNode($2, YANG_DECL_UNITS, topNode());
                }
                ;

defaultStatement: defaultKeyword string stmtEnd
                {
                    uniqueNodeKind(topNode(), YANG_DECL_DEFAULT);
                    addYangNode($2, YANG_DECL_DEFAULT, topNode());
                }
                ;

prefix:		identifierStr
        ;

identifierRef:  identifierRefArg
            |
                identifierRefArgStr 
            |
                identifierStr 
            ;
identifierStr:    identifier
                | augmentKeyword | belongs_toKeyword | choiceKeyword | configKeyword | contactKeyword | containerKeyword | defaultKeyword | descriptionKeyword
                | enumKeyword | error_app_tagKeyword | error_messageKeyword | extensionKeyword | groupingKeyword | importKeyword | includeKeyword | keyKeyword
                | leafKeyword | leaf_listKeyword | lengthKeyword | listKeyword | mandatoryKeyword | max_elementsKeyword | min_elementsKeyword | moduleKeyword
                | submoduleKeyword | mustKeyword | namespaceKeyword | ordered_byKeyword | organizationKeyword | prefixKeyword | rangeKeyword | referenceKeyword
                | patternKeyword | revisionKeyword | statusKeyword | typeKeyword | typedefKeyword | uniqueKeyword | unitsKeyword | usesKeyword | valueKeyword
                | whenKeyword | bitKeyword | pathKeyword | anyXMLKeyword | deprecatedKeyword | currentKeyword | obsoleteKeyword | trueKeyword | falseKeyword
                | caseKeyword | inputKeyword | outputKeyword | rpcKeyword | notificationKeyword | argumentKeyword | yangversionKeyword | baseKeyword
                | deviationKeyword | deviateKeyword | featureKeyword | identityKeyword | ifFeatureKeyword | positionKeyword | presenceKeyword | refineKeyword
                | requireInstanceKeyword | yinElementKeyword | notSupportedKeyword | addKeyword | deleteKeyword | replaceKeyword
                ;


booleanValue:   trueKeyword
                {
                    $$ = YANG_BOOLEAN_TRUE;
                }
             |
                falseKeyword
                {
                    $$ = YANG_BOOLEAN_FALSE;
                }
             ;

string:		qString
	|
    		uqString
	|
            identifierRef
    |
            dateString
    |
            yangVersion

	;
%%

#endif			
