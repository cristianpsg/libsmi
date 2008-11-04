/*
 * parser-yang.y --
 *
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

#include "smi.h"
#include "error.h"
#include "parser-yang.h"
#include "scanner-yang.h"
#include "data.h"
#include "check.h"
#include "util.h"
    
#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif



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





static time_t date;

static char *importModulename = NULL;
static Module *currentModule = NULL;
static Parser *currentParser = NULL;
static Import *currentImport = NULL;
static Type *currentType = NULL;
static YangNode *currentNode = NULL;
static Must *currentMust = NULL;
static Macro *macroPtr = NULL;
static SmiBasetype defaultBasetype = SMI_BASETYPE_UNKNOWN;

/* The declaration stack is used to determine what is the parrent
 * statement of the current one. It helps to write generic
 * substatements like the description statement which calls the
 * function setDescription, which in turn checks the current
 * statement type(decl) and uses the a global pointer to the 
 * current parent statement to set the description. 
 */
typedef struct declStack {
	SmiDecl decl;
	struct declStack *down;
} declStack;

static declStack *dStack = NULL;

static void pushDecl(SmiDecl decl)
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

static void popDecl()
{
	declStack *top;	
	if(dStack != NULL)
	{
		top = dStack;
		dStack = dStack->down;
		free(top);
	}
}

static SmiDecl topDecl()
{
	if (dStack == NULL)
	{
		return SMI_BASETYPE_UNKNOWN;
	}
	else
	{
		return dStack->decl;
	}
}


#define SMI_EPOCH	631152000	/* 01 Jan 1990 00:00:00 */ 
 

static Type *findType(char* spec)
{
	Type *typePtr = NULL;
	Module *modulePtr;
	char *prefix, *type;
	YangNode* n = NULL;
	
	if(strstr(spec,":") != NULL)
	{
		prefix = strtok(spec, ":");
		type = strtok(NULL , ":");
		modulePtr = findImportedModuleByPrefix(currentModule, prefix);
		if(modulePtr == NULL)
		{
			//TODO print unknow prefix error
			return NULL;
		}
		n = findYangNodeByScopeAndName(modulePtr, type, NULL);
	}
	else
	{
		n = findYangNodeByScopeAndName(currentModule, spec, currentNode);
	}
	
	if(n == NULL) return NULL;
	if(n->export.nodeKind == SMI_DECL_TYPEDEF)
	{
		typePtr = n->type;
	}
        return typePtr;
}

//static Type * findTypeNode(char *name)
//{
//	return NULL;	
//}

static void
checkTypes(Parser *parserPtr, Module *modulePtr)
{
    Type *typePtr;
    
    for(typePtr = modulePtr->firstTypePtr;
	typePtr; typePtr = typePtr->nextPtr) {

	/*
	 * Complain about empty description clauses.
	 */

	if (! parserPtr->flags & SMI_FLAG_NODESCR
	    && (! typePtr->export.description
		|| ! typePtr->export.description[0])) {
	    smiPrintErrorAtLine(parserPtr, ERR_EMPTY_DESCRIPTION,
				typePtr->line, typePtr->export.name);
	}
	
	smiCheckNamedNumberRedefinition(parserPtr, typePtr);
	smiCheckNamedNumberSubtyping(parserPtr, typePtr);
    }
}



static time_t
checkDate(Parser *parserPtr, char *date)
{
    struct tm	tm;
    time_t	anytime;
    int		i, len;
    char	*p;

    memset(&tm, 0, sizeof(tm));
    anytime = 0;

    len = strlen(date);
    if (len == 10 || len == 16) {
	for (i = 0; i < len; i++) {
	    if (((i < 4 || i == 5 || i == 6 || i == 8 || i == 9 || i == 11
		  || i == 12 || i == 14 || i == 15) && ! isdigit((int)date[i]))
		|| ((i == 4 || i == 7) && date[i] != '-')
		|| (i == 10 && date[i] != ' ')
		|| (i == 13 && date[i] != ':')) {
		smiPrintError(parserPtr, ERR_DATE_CHARACTER, date);
		anytime = (time_t) -1;
		break;
	    }
	}
    } else {
	smiPrintError(parserPtr, ERR_DATE_LENGTH, date);
	anytime = (time_t) -1;
    }
    
    if (anytime == 0) {
	for (i = 0, p = date, tm.tm_year = 0; i < 4; i++, p++) {
	    tm.tm_year = tm.tm_year * 10 + (*p - '0');
	}
	p++;
	tm.tm_mon = (p[0]-'0') * 10 + (p[1]-'0');
	p += 3;
	tm.tm_mday = (p[0]-'0') * 10 + (p[1]-'0');
	p += 2;
	if (len == 16) {
	    p++;
	    tm.tm_hour = (p[0]-'0') * 10 + (p[1]-'0');
	    p += 3;
	    tm.tm_min = (p[0]-'0') * 10 + (p[1]-'0');
	}
	
	if (tm.tm_mon < 1 || tm.tm_mon > 12) {
	    smiPrintError(parserPtr, ERR_DATE_MONTH, date);
	}
	if (tm.tm_mday < 1 || tm.tm_mday > 31) {
	    smiPrintError(parserPtr, ERR_DATE_DAY, date);
	}
	if (tm.tm_hour < 0 || tm.tm_hour > 23) {
	    smiPrintError(parserPtr, ERR_DATE_HOUR, date);
	}
	if (tm.tm_min < 0 || tm.tm_min > 59) {
	    smiPrintError(parserPtr, ERR_DATE_MINUTES, date);
	}
	
	tm.tm_year -= 1900;
	tm.tm_mon -= 1;
	tm.tm_isdst = 0;

	anytime = timegm(&tm);

	if (anytime == (time_t) -1) {
	    smiPrintError(parserPtr, ERR_DATE_VALUE, date);
	} else {
	    if (anytime < SMI_EPOCH) {
		smiPrintError(parserPtr, ERR_DATE_IN_PAST, date);
	    }
	    if (anytime > time(NULL)) {
		smiPrintError(parserPtr, ERR_DATE_IN_FUTURE, date);
	    }
	}
    }
    
    return (anytime == (time_t) -1) ? 0 : anytime;
}

void checkUniqueAndKey()
{
	YangNode *node;
	YangNodeList *list;
	for(list = currentNode->uniqueList; list; list = list->next)
	{
		for(node = currentNode->firstChildPtr; node; node = node->nextSiblingPtr)
		{
			if(!strcmp(list->name,node->export.name))
			{
				list->yangNode = node;
				smiFree(list->name);
				break;
			}
		}
		if(!list->yangNode)
			smiPrintError(currentParser,ERR_NO_SUCH_UNIQUE_LEAF,
					      			list->name,currentNode->export.name);		
	}
	
	
	
	for(list = currentNode->keyList; list; list = list->next)
	{
		for(node = currentNode->firstChildPtr; node; node = node->nextSiblingPtr)
		{
			if(!strcmp(list->name,node->export.name))
			{
				list->yangNode = node;
				smiFree(list->name);
				break;
			}
		}
		if(!list->yangNode)
			smiPrintError(currentParser,ERR_NO_SUCH_KEY_LEAF,
					      			list->name,currentNode->export.name);		
	}
	return;
}

void setDescription(char *description)
{
	switch( topDecl() )
	{
		case SMI_DECL_YANG_MODULE:
			if(!currentModule->export.description)
				setModuleDescription(currentModule, description, currentParser);
			else
				smiPrintError(currentParser,
					      ERR_REDEFINED_DESCRIPTION,
					      NULL);
			break;
		case SMI_DECL_REVISION:
			addRevision(date, description, currentParser);
			break;
		case SMI_DECL_TYPEDEF:
			setTypeDescription(currentType, description, currentParser);
			break;
		case SMI_DECL_LEAF:
		case SMI_DECL_LEAF_LIST:
		case SMI_DECL_LIST:
		case SMI_DECL_CONTAINER:
		case SMI_DECL_CHOICE:
		case SMI_DECL_CASE:
		case SMI_DECL_USES:
		case SMI_DECL_AUGMENT: 
		case SMI_DECL_GROUPING:
			setYangNodeDescription(currentNode, description);
			//printf("DEBUGGG %s\n",currentNode->export.description);
			break;
		case SMI_DECL_MUST_STATEMENT:
			setMustDescription(currentMust, description);
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong description DECL %d, at line %d\n",topDecl(),currentParser->line);
			break;
	}
}

void setPrefix(char *prefix)
{
	switch( topDecl() )
	{
		case SMI_DECL_YANG_MODULE:
			if(!currentModule->export.prefix)
				setModulePrefix(currentModule, prefix, currentParser);
			else
				smiPrintError(currentParser,
					      ERR_REDEFINED_PREFIX,
					      NULL);
			break;
		case SMI_DECL_IMPORT:
				setImportPrefix(currentImport, prefix);
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong prefix DECL %d, at line %d\n",topDecl(),currentParser->line);
			break;
			
	}
}

void setReference(char *reference)
{
	switch( topDecl() )
	{
		case SMI_DECL_YANG_MODULE:
			if(!currentModule->export.reference)
				setModuleReference(currentModule, reference, currentParser);
			else
				smiPrintError(currentParser,
					      ERR_REDEFINED_REFERENCE,
					      NULL);
			break;
		case SMI_DECL_TYPEDEF:
			setTypeReference(currentType, reference, currentParser);
			break;
		case SMI_DECL_LEAF:
		case SMI_DECL_LEAF_LIST:
		case SMI_DECL_LIST:
		case SMI_DECL_CONTAINER:
		case SMI_DECL_CHOICE:
		case SMI_DECL_CASE:
		case SMI_DECL_USES:
		case SMI_DECL_AUGMENT:  
		case SMI_DECL_GROUPING:
			setYangNodeReference(currentNode, reference);
			break;
		case SMI_DECL_MUST_STATEMENT:
			setMustReference(currentMust, reference);
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong reference decl %d, at line %d\n",topDecl(),currentParser->line);
			break;
	}
}

void setStatus(SmiStatus status)
{
	switch( topDecl() )
	{
		case SMI_DECL_LEAF_LIST:
		case SMI_DECL_LEAF:
		case SMI_DECL_LIST:		
		case SMI_DECL_CONTAINER:
			setYangNodeStatus(currentNode, status);
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong status decl %d, at line %d\n",topDecl(),currentParser->line);
			break;
	}
}

void setType(SmiBasetype basetype, Type *parent, char *parentName)
{
	
	switch( topDecl() )
	{
		case SMI_DECL_TYPEDEF:
			if(basetype == SMI_BASETYPE_UNKNOWN) //type derived from non-base type
			{
				
				if(parent) //known parent - put reference
				{
					//for typedefs type name is the same as node name
					char *typeName = smiStrdup(currentNode->export.name);
					currentType = createType(typeName); //add type with this name
					setTypeDecl(currentType, SMI_DECL_TYPEDEF);
					setTypeParent(currentType,parent);
					setTypeBasetype(currentType,parent->export.basetype);
				}
				else //forward reference - put empty type with parent's name
				{
					char *typeName = smiStrdup(parentName);
					currentType = createType(typeName); //add type with this name
				//IMPORTANT: After parsing is finished the dummy types are changed with
				// references to their parent. Dummies are distinguished by the fact that
				// they have no parent (parentPtr = NULL)
				}
			}
			else //New type derived from base type
			{
				
				//for typedefs type name is the same as node name
			        currentType = createType(currentNode->export.name);		

				setTypeBasetype(currentType, basetype);
				setTypeParent(currentType, parent);
			}
		        	
			break;
		case SMI_DECL_LEAF_LIST:
		case SMI_DECL_LEAF:
			if(basetype == SMI_BASETYPE_UNKNOWN) //type derived from non-base type
			{
				if(parent) //known parent - put reference
				{
					char *typeName = smiStrdup(parentName);
					currentType = createType(typeName); //add type with this name
					setTypeDecl(currentType, SMI_DECL_TYPEDEF);
					setTypeParent(currentType,parent);
					setTypeBasetype(currentType,parent->export.basetype);
				}
				else //forward reference - put empty type with parent's name
				{
					char *typeName = smiStrdup(parentName);
					currentType = createType(typeName); //add type with this name
				//IMPORTANT: After parsing is finished the dummy types are changed with
				// references to their parent. Dummies are distinguished by the fact that
				// they have no parent (parentPtr = NULL)
				}
			}
			else //New type derived from base type
			{
		
				currentType = createType(parentName);		

				setTypeBasetype(currentType, basetype);
				setTypeParent(currentType, parent);
			}
				
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong err decl %d, at line %d\n",topDecl(),currentParser->line);
			break;
	}
}

void setErrorMessage(char *err)
{
	switch( topDecl() )
	{
		case SMI_DECL_RANGE:
			setTypeErrorMessage(currentType, err);
			break;
		case SMI_DECL_MUST_STATEMENT:
			setMustErrorMessage(currentMust, err);
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong err decl %d, at line %d\n",topDecl(),currentParser->line);
			break;
	}
}

void setErrorAppTag(char *err)
{
	switch( topDecl() )
	{
		case SMI_DECL_RANGE:
			setTypeErrorAppTag(currentType, err);
			break;
		case SMI_DECL_MUST_STATEMENT:
			setMustErrorAppTag(currentMust, err);
			break;
		default:
			//TODO print error
			printf("DEBUGG: OOPS wrong errapp decl %d, at line %d\n",topDecl(),currentParser->line);
			break;
	}
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
    char           *id;				/* identifier name           */
    int            rc;				/* >=0: ok, <0: error        */
    time_t	   date;			/* a date value		     */
    char	   *text;
    Module	   *modulePtr;
    Node	   *nodePtr;
    Object	   *objectPtr;
    Macro	   *macroPtr;
    Type	   *typePtr;
    Index	   index;
    Option	   *optionPtr;
    Refinement	   *refinementPtr;
    SmiStatus	   status;
    SmiAccess	   access;
    NamedNumber    *namedNumberPtr;
    Range	   *rangePtr;
    SmiValue	   *valuePtr;
    List	   *listPtr;
    Revision	   *revisionPtr;
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
%token <rc>int8Keyword
%token <rc>int16Keyword
%token <rc>int32Keyword
%token <rc>int64Keyword
%token <rc>uint8Keyword
%token <rc>uint16Keyword
%token <rc>uint32Keyword
%token <rc>uint64Keyword
%token <rc>float32Keyword
%token <rc>float64Keyword
%token <rc>stringKeyword
%token <rc>booleanKeyword
%token <rc>enumerationKeyword
%token <rc>bitsKeyword
%token <rc>binaryKeyword
%token <rc>keyrefKeyword
%token <rc>emptyKeyword
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

%token <text>identifier
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
%type <rc>extensionSubstatement
%type <rc>extensionSubstatement_0n
%type <rc>linkageStatement
%type <rc>linkageStatement_0n
%type <rc>moduleMetaStatement
%type <rc>moduleMetaStatement_0n
%type <rc>moduleHeaderStatement
%type <rc>moduleHeaderStatement_0n
%type <rc>bodyStatement
%type <rc>bodyStatement_0n
%type <rc>containerStatement
%type <rc>containerSubstatement
%type <rc>containerSubstatement_0n
%type <rc>mustStatement
%type <rc>mustSubstatement
%type <rc>mustSubstatement_0n
%type <rc>organizationStatement
%type <rc>contactStatement
%type <rc>referenceStatement
%type <rc>descriptionStatement
%type <rc>revisionStatement
%type <rc>revisionStatement_0n
%type <rc>importStatement
%type <rc>prefixStatement
%type <rc>typedefStatement
%type <rc>typedefSubstatement
%type <rc>typedefSubstatement_0n
%type <rc>typeStatement
%type <rc>type
%type <rc>refinedType
%type <rc>refinedBasetype
%type <text>numRestriction
%type <rc>stringRestriction
%type <rc>restriction
%type <rc>range
%type <rc>length
%type <date>date
%type <text>string
%type <rc>enumSpec
%type <rc>enum
%type <rc>enum_0n
%type <rc>optExtension
%type <rc>optExtensionSubstatement0_n
%type <rc>optExtensionSubstatement
%type <rc>error_app_tag
%type <rc>error_message
%type <rc>err
%type <rc>optError
%type <rc>path
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
%type <rc>choiceSubstatement
%type <rc>choiceSubstatement_0n
%type <rc>caseStatement
%type <rc>caseSubstatement
%type <rc>caseSubstatement_0n
%type <rc>groupingStatement
%type <rc>groupingSubstatement
%type <rc>groupingSubstatement_0n
%type <rc>usesStatement
%type <rc>usesSubstatement
%type <rc>usesSubstatement_0n
%type <rc>augmentStatement
%type <rc>augmentSubstatement
%type <rc>augmentSubstatement_0n
%type <rc>whenStatement
%type <rc>rpcStatement
%type <rc>rpcSubstatement
%type <rc>rpcSubstatement_0n
%type <rc>inputStatement
%type <rc>inputSubstatement
%type <rc>inputSubstatement_0n
%type <rc>outputStatement
%type <rc>outputSubstatement
%type <rc>outputSubstatement_0n
%type <rc>notificationStatement
%type <rc>notificationSubstatement
%type <rc>notificationSubstatement_0n
%type <rc>anyXMLStatement
%type <rc>anyXMLSubstatement
%type <rc>anyXMLSubstatement_0n

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
			{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    $$ = $1;
		printf("DEBUGGG finished parsing module\n");
			}
	|
			submoduleStatement
			{
			    /*
			     * Return the number of successfully
			     * parsed modules.
			     */
			    $$ = $1;
		printf("DEBUGGG finished parsing submodule\n");
			}
	;

moduleStatement:	moduleKeyword identifier
			{
				thisParserPtr->modulePtr = findModuleByName($2);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule($2,
					      smiStrdup(thisParserPtr->path),
					      0,
					      thisParserPtr);
			    } else {
			        smiPrintError(thisParserPtr,
					      ERR_MODULE_ALREADY_LOADED,
					      $2);
				free($2);
				/*
				 * this aborts parsing the whole file,
				 * not only the current module.
				 */
				YYABORT;
			    }
			    thisModulePtr->export.language = SMI_LANGUAGE_YANG;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			    thisParserPtr->firstIndexlabelPtr = NULL;
			    thisParserPtr->identityObjectName = NULL;
				$$ = 1;

				currentParser = thisParserPtr;
				currentModule = thisModulePtr;

			   pushDecl(SMI_DECL_YANG_MODULE);
			}
			'{'
				moduleHeaderStatement_0n
				linkageStatement_0n
				moduleMetaStatement_0n
				revisionStatement_0n
				bodyStatement_0n			
			'}'
			{
			   popDecl();
			}
	;

submoduleStatement:	submoduleKeyword identifier
			{
				thisParserPtr->modulePtr = findModuleByName($2);
			    if (!thisParserPtr->modulePtr) {
				thisParserPtr->modulePtr =
				    addModule($2,
					      smiStrdup(thisParserPtr->path),
					      0,
					      thisParserPtr);
			    } else {
			        smiPrintError(thisParserPtr,
					      ERR_MODULE_ALREADY_LOADED,
					      $2);
				free($2);
				/*
				 * this aborts parsing the whole file,
				 * not only the current module.
				 */
				YYABORT;
			    }
			    thisModulePtr->export.language = SMI_LANGUAGE_YANG;
			    thisParserPtr->modulePtr->numImportedIdentifiers
				                                           = 0;
			    thisParserPtr->modulePtr->numStatements = 0;
			    thisParserPtr->modulePtr->numModuleIdentities = 0;
			    thisParserPtr->firstIndexlabelPtr = NULL;
			    thisParserPtr->identityObjectName = NULL;
				$$ = 1;

				currentParser = thisParserPtr;
				currentModule = thisModulePtr;

			   pushDecl(SMI_DECL_YANG_MODULE);
			}
			'{'
				moduleHeaderStatement_0n
				linkageStatement_0n
				moduleMetaStatement_0n
				revisionStatement_0n
				bodyStatement_0n			
			'}'
			{
			   popDecl();
			}
	;

moduleHeaderStatement_0n:	moduleHeaderStatement
		     	{
				$$ = 1;
			}
		|
			moduleHeaderStatement moduleHeaderStatement_0n
			{
				$$ = 1 + $2;
			}
		;

moduleHeaderStatement:	namespaceStatement
		|
			prefixStatement
		;
moduleMetaStatement_0n:	
		     	{
				$$ = 1;
			}
		|
			moduleMetaStatement_0n moduleMetaStatement 
			{
				$$ = 1 + $2;
			}
		;

moduleMetaStatement:	organizationStatement
		|
			descriptionStatement
		|
			contactStatement
		|
			referenceStatement
		;
linkageStatement_0n:	{
				$$ = 1;
			}
		|
			linkageStatement_0n linkageStatement
			{
				$$ = 1 + $2;
			}
		;

linkageStatement:	includeStatement
		|
			importStatement
		;

revisionStatement_0n:	revisionStatement
		     	{
				$$ = 1;
			}
		|
			revisionStatement revisionStatement_0n
			{
				$$ = 1 + $2;
			}
		;

bodyStatement_0n:	
		     	{
				$$ = 1;
			}
		|
			bodyStatement_0n bodyStatement
			{
				$$ = 1 + $2;
			}
		;

bodyStatement:		extensionStatement
	     	|
	     		dataDefStatement
		|
			rpcStatement
		|
			notificationStatement
		|
			groupingStatement
		|
			typedefStatement
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
		|
			augmentStatement
		;

commonStatement:	descriptionStatement
       		|
			statusStatement
		|
			referenceStatement
		|
			configStatement
		;	       

descriptionStatement:	descriptionKeyword string optExtension
			{
				setDescription($2);		 	
				$$ = 1;
			}
	;

organizationStatement:	organizationKeyword string optExtension
			{
				setModuleOrganization(currentModule, $2);		 	
				$$ = 1;
			}
	;

contactStatement:	contactKeyword string optExtension
			{
				setModuleContactInfo(currentModule, $2);		 	
				$$ = 1;
			}
	;

referenceStatement:	referenceKeyword string optExtension
			{
				setReference($2);		 	
				$$ = 1;
			}
	;

statusStatement:	statusKeyword status optExtension
			{
				setStatus($2);		 	
				$$ = 1;
			}
	;

namespaceStatement:	namespaceKeyword string optExtension
		  	{
				setModuleXMLNamespace(currentModule, $2);
			}
	;
status:		deprecatedKeyword
		{
			$$ = SMI_STATUS_DEPRECATED;
		}
	|
		currentKeyword
		{
			$$ = SMI_STATUS_CURRENT;
		}
	|
		obsoleteKeyword
		{
			$$ = SMI_STATUS_OBSOLETE;
		}
	;

prefixStatement:	prefixKeyword string optExtension
			{
				setPrefix($2);
			}
	;

revisionStatement:	revisionKeyword date
			{
				date = $2;
				pushDecl(SMI_DECL_REVISION)
			}
			'{'
				descriptionStatement
			'}'
			{
				popDecl();
			}
	;

date: 	string
	{
		$$ = checkDate(currentParser, $1);
	}
	;

importStatement: importKeyword identifier
		{
			Module *m = findModuleByName($2);
			
			if(!m)
			{
				m = loadModule($2, currentParser);
			}
			if(m)
			{		    
					Import *im = addImport("",currentParser);
					setImportModulename(im, m->export.name);
					currentImport = im;
			}
			else
			{
				smiPrintError(thisParserPtr,
					      ERR_IMPORT_NOT_FOUND,
					      $2);
			}
			pushDecl(SMI_DECL_IMPORT);
		}
		'{'
			prefixStatement
		'}'
		{
			popDecl();
		}
        ;

includeStatement: includeKeyword identifier
		{
			Module *m = findModuleByName($2);
			
			if(!m)
			{
				m = loadModule($2, currentParser);
			}
			if(m)
			{		    
					Import *im = addImport("",currentParser);
					setImportModulename(im, m->export.name);
					currentImport = im;
			}
			else
			{
				smiPrintError(thisParserPtr,
					      ERR_IMPORT_NOT_FOUND,
					      $2);
			}
			pushDecl(SMI_DECL_IMPORT);
		}
		'{'
			prefixStatement
		'}'
		{
			popDecl();
		}
        ;

typedefStatement:	typedefKeyword identifier
			{
				pushDecl(SMI_DECL_TYPEDEF);
				currentNode = addYangNode($2,SMI_DECL_TYPEDEF, currentNode, currentModule);
			}
			'{'
				typedefSubstatement_0n
			'}'
			{
				popDecl();
				//currentType = NULL;
			}
	;

typedefSubstatement_0n:	typedefSubstatement
			{
				$$ = 1;
			}
		|
		       typedefSubstatement typedefSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;				

typedefSubstatement:	typeStatement
		|
			descriptionStatement
		|
			referenceStatement
		|
			defaultStatement
	;

typeStatement: typeKeyword type
		{
				
		}
	;

type:		refinedBasetype
	|
		refinedType
	;

refinedBasetype:	float32Keyword numRestriction
			{
				setType(SMI_BASETYPE_FLOAT32, smiHandle->typeFloat32Ptr, NULL);
			}
		|	
			float64Keyword numRestriction
			{
				setType(SMI_BASETYPE_FLOAT64, smiHandle->typeFloat64Ptr, NULL);
			}
		|
			int8Keyword numRestriction
			{
				setType(SMI_BASETYPE_INTEGER8, smiHandle->typeInteger8Ptr, NULL);
			}
		|	
			int16Keyword numRestriction
			{
				setType(SMI_BASETYPE_INTEGER16, smiHandle->typeInteger16Ptr, NULL);
			}
		|	
			int32Keyword numRestriction
			{
				setType(SMI_BASETYPE_INTEGER32, smiHandle->typeInteger32Ptr, NULL);
			}
		|
			int64Keyword numRestriction
			{
				setType(SMI_BASETYPE_INTEGER64, smiHandle->typeInteger64Ptr, NULL);
			}
		|
			uint8Keyword numRestriction
			{
				setType(SMI_BASETYPE_UNSIGNED8, smiHandle->typeUnsigned8Ptr, NULL);
			}
		|	
			uint16Keyword numRestriction
			{
				setType(SMI_BASETYPE_UNSIGNED16, smiHandle->typeUnsigned16Ptr, NULL);
			}
		|	
			uint32Keyword numRestriction
			{
				setType(SMI_BASETYPE_UNSIGNED32, smiHandle->typeUnsigned32Ptr, NULL);
			}
		|	
			uint64Keyword numRestriction
			{
				setType(SMI_BASETYPE_UNSIGNED64, smiHandle->typeUnsigned64Ptr, NULL);
			}
		|
			stringKeyword stringRestriction
			{
				
				setType(SMI_BASETYPE_OCTETSTRING, smiHandle->typeOctetStringPtr, NULL);
			}
		|
			float32Keyword ';'
			{
				setType(SMI_BASETYPE_FLOAT32, smiHandle->typeFloat32Ptr, NULL);
			}
		|	
			float64Keyword ';'
			{
				setType(SMI_BASETYPE_FLOAT64, smiHandle->typeFloat64Ptr, NULL);
			}
		|
			int8Keyword ';'
			{
				setType(SMI_BASETYPE_INTEGER8, smiHandle->typeInteger8Ptr, NULL);
			}
		|	
			int16Keyword ';'
			{
				setType(SMI_BASETYPE_INTEGER16, smiHandle->typeInteger16Ptr, NULL);
			}
		|	
			int32Keyword ';'
			{
				setType(SMI_BASETYPE_INTEGER32, smiHandle->typeInteger32Ptr, NULL);
			}
		|
			int64Keyword ';'
			{
				setType(SMI_BASETYPE_INTEGER64, smiHandle->typeInteger64Ptr, NULL);
			}
		|
			uint8Keyword ';'
			{
				setType(SMI_BASETYPE_UNSIGNED8, smiHandle->typeUnsigned8Ptr, NULL);
			}
		|	
			uint16Keyword ';'
			{
				setType(SMI_BASETYPE_UNSIGNED16, smiHandle->typeUnsigned16Ptr, NULL);
			}
		|	
			uint32Keyword ';'
			{
				setType(SMI_BASETYPE_UNSIGNED32, smiHandle->typeUnsigned32Ptr, NULL);
			}
		|	
			uint64Keyword ';'
			{
				setType(SMI_BASETYPE_UNSIGNED64, smiHandle->typeUnsigned64Ptr, NULL);
			}
		|
			stringKeyword ';'
			{
				
				setType(SMI_BASETYPE_OCTETSTRING, smiHandle->typeOctetStringPtr, NULL);
			}
		|
			booleanKeyword ';'
			{
				setType(SMI_BASETYPE_BOOLEAN, smiHandle->typeBooleanPtr, NULL);
			}
		|
			keyrefKeyword path
			{
				setType(SMI_BASETYPE_KEYREF, smiHandle->typeKeyrefPtr, NULL);
			}
		|
			binaryKeyword ';'
			{
				setType(SMI_BASETYPE_BINARY, smiHandle->typeBinaryPtr, NULL);
			}
		|
			enumerationKeyword enumSpec
	;

refinedType:	identifier restriction
		{
			Type *t = findType($1);			
			if(t) //parent type already in types list
			{
				setType(SMI_BASETYPE_UNKNOWN,t,$1);
			}
			else //forward reference
			{
				setType(SMI_BASETYPE_UNKNOWN,NULL,$1);
			}
		}
	;

restriction: 	numRestriction
	|
		stringRestriction
	|
		enumSpec
	|
		path
	|
		';'
	;
	
numRestriction: '{'
			range
		'}'
		{
			$$=NULL;
		}
	;

stringRestriction:	'{'
			length
			'}'
	|
		'{'
			pattern
		'}'
	|
		'{'
			pattern
			length
		'}'
	|
		'{'
			length
			pattern
		'}'
	;

enumSpec: 
	'{'
		enum_0n
	'}'


enum_0n:
       |
	enum_0n enum
	;

enum: enumKeyword string ';'
    |
	enumKeyword string
	'{'
		valueKeyword string optExtension
	'}'
	;

range:		rangeKeyword string
		{
			$$=NULL;
			pushDecl(SMI_DECL_RANGE);
		}
		 optError
		{
			popDecl();
		}
	;

length:		lengthKeyword string
		{
			$$=NULL;
			pushDecl(SMI_DECL_RANGE);
		}
		 optError
		{
			popDecl();
		}
	;


path:		pathKeyword string optError
		{
			$$=NULL;
		}
	;

pattern:	patternKeyword string{
			$$=NULL;
			pushDecl(SMI_DECL_PATTERN);
		}
		 optError
		{
			popDecl();
		}
	;

optError:	';'
	| 
		'{'
			err
		'}'
	;

err: 		error_app_tag error_message
	|
		error_message error_app_tag
	|
		error_message
	|
		error_app_tag
	;

error_message: error_messageKeyword string optExtension
		{
			setErrorMessage($2);
		}
	;

error_app_tag: error_app_tagKeyword string optExtension
		{
			setErrorAppTag($2);
		}
	;

optExtension:   ';'
	|
		'{'
			optExtensionSubstatement0_n
		'}'
	;

optExtensionSubstatement0_n:
		|
			optExtensionSubstatement optExtensionSubstatement0_n;
	;

optExtensionSubstatement: identifier string optExtension;

containerStatement: containerKeyword identifier
			{
				pushDecl(SMI_DECL_CONTAINER);
				currentNode = addYangNode($2,SMI_DECL_CONTAINER, currentNode, currentModule);
			}
			'{'
				containerSubstatement_0n
			'}'
			{
				popDecl();
				currentNode = currentNode->parentPtr;
			}
	;


containerSubstatement_0n:	containerSubstatement
			{
				$$ = 1;
			}
		|
		       containerSubstatement containerSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

containerSubstatement:	commonStatement
		|
			dataDefStatement
		|
			mustStatement
		|
			typedefStatement
		;

mustStatement: mustKeyword string
		{
			pushDecl(SMI_DECL_MUST_STATEMENT);
			currentMust = addMustStatement(currentNode, $2);
		}
		'{'
			mustSubstatement_0n
		'}'
		{
			popDecl();
			currentMust = NULL;
		}
	|
		mustKeyword string ';'
		{
		 	addMustStatement(currentNode, $2);
			currentMust = NULL;
		}
	;

mustSubstatement_0n:	mustSubstatement
			{
				$$ = 1;
			}
		|
		       mustSubstatement mustSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

mustSubstatement:	error_message
		|
			error_app_tag
		|
			descriptionStatement
		|
			referenceStatement
		;
			
configStatement: 	configKeyword trueKeyword optExtension
			{
				if(currentNode->parentPtr &&   currentNode->parentPtr->export.config != SMI_CONFIG_FALSE)
						currentNode->export.config = SMI_CONFIG_TRUE;
				else
				{
					currentNode->export.config = SMI_CONFIG_FALSE;
				}
				//TODO print error
					
			
			}
	|
			configKeyword falseKeyword optExtension
			{
				currentNode->export.config = SMI_CONFIG_FALSE;
			}
		;

mandatoryStatement: mandatoryKeyword trueKeyword optExtension
			{
				currentNode->export.mandatory = SMI_MANDATORY_TRUE;
			}
		|
		    	mandatoryKeyword falseKeyword optExtension	
			{
				currentNode->export.mandatory = SMI_MANDATORY_FALSE;
			}
		;
			
leafStatement: leafKeyword identifier
			{
				pushDecl(SMI_DECL_LEAF);
				currentNode = addYangNode($2,SMI_DECL_LEAF, currentNode, currentModule);
			}
			'{'
				leafSubstatement_0n
			'}'
			{
				popDecl();
				currentNode = currentNode->parentPtr;
			}
		;
			
leafSubstatement_0n:	leafSubstatement
			{
				$$ = 1;
			}
		|
		       leafSubstatement leafSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

leafSubstatement:	mustStatement
		|
			commonStatement
		|
			mandatoryStatement
		|
			typeStatement
		|
			defaultStatement	
		;

leaf_listStatement: leaf_listKeyword identifier
			{
				pushDecl(SMI_DECL_LEAF_LIST);
				currentNode = addYangNode($2,SMI_DECL_LEAF_LIST, currentNode, currentModule);
			}
			'{'
				leaf_listSubstatement_0n
			'}'
			{
				popDecl();
				currentNode = currentNode->parentPtr;
			}
		;
			
leaf_listSubstatement_0n:	leaf_listSubstatement
			{
				$$ = 1;
			}
		|
		       leaf_listSubstatement leaf_listSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

leaf_listSubstatement:	mustStatement
		|
			commonStatement
		|
			mandatoryStatement
		|
			typeStatement
		|	
			max_elementsStatement
		|
			min_elementsStatement
		|
			ordered_byStatement
		;
		
listStatement: listKeyword identifier
			{
				pushDecl(SMI_DECL_LIST);
				currentNode = addYangNode($2,SMI_DECL_LIST, currentNode, currentModule);
			}
			'{'
				listSubstatement_0n
			'}'
			{
				popDecl();
				checkUniqueAndKey();
				currentNode = currentNode->parentPtr;
			}
		;

listSubstatement_0n:	listSubstatement
			{
				$$ = 1;
			}
		|
		       listSubstatement listSubstatement_0n
			{
				$$ = 1 + $2;
			}
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
		;

max_elementsStatement: 	max_elementsKeyword string optExtension
			{
				if(strcmp($2,"unbounded")) //if true means string is different from "unbounded"
				{
					//TODO print error
				}
				//else do nothing because unbounded is default value anyway
			}
		|	max_elementsKeyword decimalNumber optExtension
			{
				SmiUnsigned32 value = strtoul($2, NULL, 10);
				setYangNodeMaxElements(currentNode, value);
			}
		;

min_elementsStatement: 	min_elementsKeyword decimalNumber optExtension
			{
				SmiUnsigned32 value = strtoul($2, NULL, 10);
				setYangNodeMinElements(currentNode, value);				
			}
		;

ordered_byStatement: 	ordered_byKeyword string optExtension
			{
				if(!strcmp($2,"system"))
				{
					setYangNodeOrder(currentNode, SMI_ORDER_SYSTEM);
				}
				else if(!strcmp($2,"user"))
				{
					setYangNodeOrder(currentNode, SMI_ORDER_USER);
				}
				else
				{
					//TODO print error
				}
			}
		;

keyStatement: keyKeyword string optExtension
		{
			currentNode->keyList = (YangNodeList*)smiMalloc(sizeof(YangNodeList));
			YangNodeList *list  = currentNode->keyList;
			
			char *beginWord = $2; //beginning or current word
			int i = 0;
			for(i; beginWord[i]; i++) //substitude tabs with spaces
			{
				if(beginWord[i]=='\t') beginWord[i]=' ';
			}
			
			char *nextWord = strstr(beginWord," "); //location of next space
 
			while(nextWord)
			{
				nextWord[0] = '\0'; //change space for null so that duplication is to this position
				list->name = smiStrdup(beginWord);
				list->next = (YangNodeList*)smiMalloc(sizeof(YangNodeList));
				list = list->next;
				nextWord++; //next iteration starts from char after space
				beginWord = nextWord;
				nextWord = strstr(beginWord," ");
			}
			list->name = smiStrdup(beginWord);
			list->next = NULL;//terminate list
		}
	;
	
uniqueStatement: uniqueKeyword string optExtension
		{
			currentNode->uniqueList = (YangNodeList*)smiMalloc(sizeof(YangNodeList));
			YangNodeList *list  = currentNode->uniqueList;
			
			char *beginWord = $2; //beginning or current word
			int i = 0;
			for(i; beginWord[i]; i++) //substitute tabs with spaces
			{
				if(beginWord[i]=='\t') beginWord[i]=' ';
			}
			
			char *nextWord = strstr(beginWord," "); //location of next space
 
			while(nextWord)
			{
				nextWord[0] = '\0'; //change space for null so that duplication is to this position
				list->name = smiStrdup(beginWord);
				list->next = (YangNodeList*)smiMalloc(sizeof(YangNodeList));
				list = list->next;
				nextWord++; //next iteration starts from char after space
				beginWord = nextWord;
				nextWord = strstr(beginWord," ");
			}
			list->name = smiStrdup(beginWord);
			list->next = NULL;//terminate list
		}
	;

choiceStatement: choiceKeyword identifier
		{
			pushDecl(SMI_DECL_CHOICE);
			currentNode = addYangNode($2,SMI_DECL_CHOICE, currentNode, currentModule);
		}
		'{'
			choiceSubstatement_0n			
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;
 
choiceSubstatement_0n:	choiceSubstatement
			{
				$$ = 1;
			}
		|
		       choiceSubstatement choiceSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

choiceSubstatement:	commonStatement
		  |
			caseStatement
		  |
			defaultStatement
		;

caseStatement: 	caseKeyword identifier
		{
			pushDecl(SMI_DECL_CASE);
			currentNode = addYangNode($2,SMI_DECL_CASE, currentNode, currentModule);
		}
		'{'
			caseSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}	
	|
		{
			pushDecl(SMI_DECL_CASE);
			currentNode = addYangNode("",SMI_DECL_CASE, currentNode, currentModule);	
		}
			dataDefStatement
		{
			//TODO fix name of che case node to be the same as caseSubstatement name
			popDecl();
			currentNode = currentNode->parentPtr;
		}	
	;

caseSubstatement_0n:	caseSubstatement
			{
				$$ = 1;
			}
		|
		       caseSubstatement caseSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

caseSubstatement: 	descriptionStatement
		|
			statusStatement
		|
			referenceStatement
		|
			mandatoryStatement
		|
			dataDefStatement
		;

groupingStatement: groupingKeyword identifier
		{
			pushDecl(SMI_DECL_GROUPING);
			currentNode = addYangNode($2,SMI_DECL_GROUPING, currentNode, currentModule);
		}
		'{'
			groupingSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

groupingSubstatement_0n:	groupingSubstatement
			{
				$$ = 1;
			}
		|
		       groupingSubstatement groupingSubstatement_0n
			{
				$$ = 1 + $2;
			}
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

usesStatement: usesKeyword identifier
		{
			pushDecl(SMI_DECL_USES);
			currentNode = addYangNode($2,SMI_DECL_USES, currentNode, currentModule);
		}
	     	'{'
			usesSubstatement_0n
	     	'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
	|
		usesKeyword identifier
		{
			pushDecl(SMI_DECL_USES);
			currentNode = addYangNode($2,SMI_DECL_USES, currentNode, currentModule);
		}
		';'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

usesSubstatement_0n:	usesSubstatement
			{
				$$ = 1;
			}
		|
		       usesSubstatement usesSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

usesSubstatement:	descriptionStatement
		|
			referenceStatement
		|
			statusStatement
		|
			containerStatement
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

augmentStatement: augmentKeyword string 
		{
			pushDecl(SMI_DECL_AUGMENT);
			currentNode = addYangNode($2,SMI_DECL_AUGMENT, currentNode, currentModule);
		}
		'{'
			augmentSubstatement_0n
	     	'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

augmentSubstatement_0n:	augmentSubstatement
			{
				$$ = 1;
			}
		|
		       augmentSubstatement augmentSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

augmentSubstatement:	whenStatement
		|
		   	descriptionStatement
		|
			referenceStatement
		|
			statusStatement
		|
			containerStatement
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

whenStatement:	whenKeyword string optExtension
	        {
			currentNode->when = smiStrdup($2)
		}
		;

rpcStatement: rpcKeyword identifier
		{
			pushDecl(SMI_DECL_RPC);
			currentNode = addYangNode($2,SMI_DECL_RPC, currentNode, currentModule);
		}
		'{'
			rpcSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

rpcSubstatement_0n:	rpcSubstatement
			{
				$$ = 1;
			}
		|
		       rpcSubstatement rpcSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

rpcSubstatement:	descriptionStatement
		|
			referenceStatement
		|
			statusStatement
		|
			inputStatement
		|
			outputStatement
	;

inputStatement: inputKeyword
		{
			pushDecl(SMI_DECL_INPUT);
			currentNode = addYangNode("input",SMI_DECL_INPUT, currentNode, currentModule);
		}
		'{'
			inputSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

inputSubstatement_0n:	inputSubstatement
			{
				$$ = 1;
			}
		|
		       inputSubstatement inputSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

inputSubstatement:	dataDefStatement
	  	|
			groupingStatement
		|
			typedefStatement
		;

outputStatement: outputKeyword
		{
			pushDecl(SMI_DECL_OUTPUT);
			currentNode = addYangNode("output",SMI_DECL_OUTPUT, currentNode, currentModule);
		}
		'{'
			outputSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

outputSubstatement_0n:	outputSubstatement
			{
				$$ = 1;
			}
		|
		       outputSubstatement outputSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

outputSubstatement:	dataDefStatement
	  	|
			groupingStatement
		|
			typedefStatement
		;

notificationStatement: notificationKeyword identifier
		{
			pushDecl(SMI_DECL_NOTIFICATION);
			currentNode = addYangNode($2,SMI_DECL_NOTIFICATION, currentNode, currentModule);
		}
		'{'
			notificationSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

notificationSubstatement_0n:	notificationSubstatement
			{
				$$ = 1;
			}
		|
		       notificationSubstatement notificationSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

notificationSubstatement:	descriptionStatement
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

anyXMLStatement: anyXMLKeyword identifier
		{
			pushDecl(SMI_DECL_ANYXML);
			currentNode = addYangNode($2,SMI_DECL_ANYXML, currentNode, currentModule);
		}
		'{'
			anyXMLSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

anyXMLSubstatement_0n:	anyXMLSubstatement
			{
				$$ = 1;
			}
		|
		       anyXMLSubstatement anyXMLSubstatement_0n
			{
				$$ = 1 + $2;
			}
	;

anyXMLSubstatement:	descriptionStatement
		|
			referenceStatement
		|
			statusStatement
		;

extensionStatement: extensionKeyword identifier
		{
			pushDecl(SMI_DECL_EXTENSION);
			currentNode = addYangNode($2,SMI_DECL_EXTENSION, currentNode, currentModule);
		}
		'{'
			extensionSubstatement_0n
		'}'
		{
			popDecl();
			currentNode = currentNode->parentPtr;
		}
		;

extensionSubstatement_0n:	extensionSubstatement
			{
				$$ = 1;
			}
		|
		       extensionSubstatement extensionSubstatement_0n
			{
				$$ = 1 + $2;
			}
		;

extensionSubstatement:	descriptionStatement
		|
			referenceStatement
		|
			statusStatement
		|
			argumentStatement
		;

argumentStatement: argumentKeyword string
		{
			addYangNode($2,SMI_DECL_ARGUMENT, currentNode, currentModule);
		}
		;


defaultStatement: defaultKeyword string optExtension
		;

string:		qString
		{
			$$ = $1;
		}
	|
		uqString
		{
			$$ = $1;
		}
	|
		identifier
		{
			$$ = $1;
		}
	;
%%

#endif
			
