/*
 * dump-corba.c --
 *
 *      Operations to dump CORBA IDL and OID definitions.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-corba.c,v 1.3 1999/11/24 19:02:38 strauss Exp $
 */

/*
 * TODO:
 *	  - notifications (3.5)
 *	  - factory interfaces (3.4.7)
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>

#include "smi.h"
#include "smidump.h"



#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */


static int current_column = 0;

static int silent = 0;


/*
 * The following list of IDL keywords is taken from the CORBA
 * 2.3.1 IDL specification section 3.2.4 (October 1999).
 */


static char *idlKeywords[] = {
    "abstract",
    "any",		"attribute",	"boolean",	"case",
    "char",		"const",	"context",	"custom",
    "default",		"double",	"enum",		"exception",
    "factory",		"FALSE",	"fixed",	"float",
    "in",		"inout",	"interface",	"long",
    "module",		"native",	"object",	"octet",
    "oneway",		"out",		"private",	"public",
    "raises",		"readonly",	"sequence",	"short",
    "string",		"struct",	"supports",	"switch",
    "TRUE",		"truncatable",	"typedef",	"unsigned",
    "union",		"valuebase",	"valuetype",	"void",
    "wchar",		"wstring",	NULL
};


/*
 * Structure used to build a list of imported types.
 */


typedef struct Import {
    char          *module;
    char          *name;
    struct Import *nextPtr;
} Import;

static Import *importList = NULL;


/*
 * Structure used to build dictionaries that translate names
 * into IDL names following the generic JIDM rules.
 */


typedef struct IdlEntry {
    char            *module;
    char            *name;
    char            *idlname;
    struct IdlEntry *nextPtr;
} IdlEntry;

static IdlEntry *idlModuleNameList = NULL;
static IdlEntry *idlNodeNameList = NULL;
static IdlEntry *idlTypeNameList = NULL;

static IdlEntry **idlNameLists[] = {
    &idlModuleNameList, &idlNodeNameList, &idlTypeNameList, NULL
};



static char* dictFindName(IdlEntry *list, char *module, char* name)
{
    IdlEntry *p;

    for (p = list; p; p = p->nextPtr) {
	if (! strcasecmp(p->module, module)
	    && ((! p->name && ! name) || ! strcasecmp(p->name, name))) {
	    return p->idlname;
	}
    }

    return NULL;
}



static char* dictAddName(IdlEntry **listPtr, char *module, char *name)
{
    IdlEntry *p;
    char *s, *idl;
    int i;

    /*
     * Create a new IDL identifier by translating hyphens
     * to underscores.
     */

    s = name ? name : module;
    idl = xmalloc(strlen(s) + 1);
    for (i = 0; s[i]; i++) {
	idl[i] = (s[i] == '-') ? '_' : s[i];
    }
    idl[i] = 0;

    /*
     * Check for any collisions with IDL keywords or previously
     * created IDL identifiers.
     */

    for (i = 0; idlKeywords[i]; i++) {
	if (! strcasecmp(idlKeywords[i], idl)) {
	    fprintf(stderr, "smidump: "
		    "`%s' (%s%s%s) collides with IDL keyword `%s'\n",
		    idl, module, name ? "::" : "", name ? name : "",
		    idlKeywords[i]);
	}
    }

    for (i = 0; idlNameLists[i]; i++) {
	IdlEntry *list = *(idlNameLists[i]);
	for (p = list; p; p = p->nextPtr) {
	    if (! strcasecmp(p->idlname, idl)) {
		fprintf(stderr, "smidump: "
			"`%s' (%s%s%s) collides with `%s' (%s%s%s)\n",
			idl, module,
			name ? "::" : "", name ? name : "",
			p->idlname, p->module,
			p->name ? "::" : "", p->name ? p->name : "");
	    }
	}
    }
    
    /*
     * Safe the translated identifier in the dictionary.
     */
    
    p = xmalloc(sizeof(IdlEntry));
    p->module = xstrdup(module);
    p->name = name ? xstrdup(name) : NULL;
    p->idlname = idl;
    p->nextPtr = *listPtr;
    *listPtr = p;

    return idl;
}



static void dictFree(IdlEntry **list)
{
    IdlEntry *p, *q;

    for (p = *list; p; ) {
	q = p;
	p = p->nextPtr;
	xfree(q->module);
	if (q->name) xfree(q->name);
	xfree(q->idlname);
	xfree(q);
    }

    *list = NULL;
}



static char* getIdlModuleName(char *module)
{
    char *s;

    s = dictFindName(idlModuleNameList, module, NULL);
    if (! s) {
	s = dictAddName(&idlModuleNameList, module, NULL);
    }
    return s;
}



static char *getIdlNodeName(char *module, char *name)
{
    char *s;
    s = dictFindName(idlNodeNameList, module, name);
    if (! s) {
	s = dictAddName(&idlNodeNameList, module, name);
    }
    return s;
}



static char* getIdlTypeName(char *module, char *name)
{
    char *s, *typename;

    typename = xmalloc(strlen(name) + 10);
    sprintf(typename, "%sType", name);
    typename[0] = toupper((int) typename[0]);

    s = dictFindName(idlTypeNameList, module, typename);
    if (! s) {
	s = dictAddName(&idlTypeNameList, module, typename);
    }

    xfree(typename);
    return s;
}



static int current(SmiStatus status)
{
    switch (status) {
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_UNKNOWN:
    case SMI_STATUS_MANDATORY:
    case SMI_STATUS_OPTIONAL:
	return 1;
    default:
	return 0;
    }
}



static char *getTimeString(time_t t)
{
    static char   s[27];
    struct tm	  *tm;

    tm = gmtime(&t);
    sprintf(s, "%04d%02d%02d%02d%02dZ",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min);
    return s;
}



static char *getAccessString(SmiAccess access, int create)
{
    if (create && (access == SMI_ACCESS_READ_WRITE)) {
	return "read-create";
    } else {
	return
	    (access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" :
	    (access == SMI_ACCESS_NOTIFY)         ? "accessible-for-notify" :
	    (access == SMI_ACCESS_READ_ONLY)      ? "read-only" :
	    (access == SMI_ACCESS_READ_WRITE)     ? "read-write" :
					            "<unknown>";
    }
}



static char *getBaseTypeString(SmiBasetype basetype)
{
    switch(basetype) {
    case SMI_BASETYPE_UNKNOWN:
	return "ASN1_Null";
    case SMI_BASETYPE_INTEGER32:
    case SMI_BASETYPE_ENUM:
	return "ASN1_Integer";
    case SMI_BASETYPE_OCTETSTRING:
    case SMI_BASETYPE_BITS:
	return "ASN1_OctetString";
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	return "ASN1_ObjectIdentifier";
    case SMI_BASETYPE_UNSIGNED32:
	return "ASN1_Unsigned";
    case SMI_BASETYPE_INTEGER64:
	return "ASN1_Integer64";
    case SMI_BASETYPE_UNSIGNED64:
	return "ASN1_Unsigned64";
    case SMI_BASETYPE_FLOAT32:
    case SMI_BASETYPE_FLOAT64:
    case SMI_BASETYPE_FLOAT128:
	return "ASN1_Real";
    }

    return NULL;
}


static char *getValueString(SmiValue *valuePtr)
{
    static char s[100];
    char        ss[9];
    int		i;
    char        **p;
    
    s[0] = 0;
    
    switch (valuePtr->basetype) {
    case SMI_BASETYPE_UNSIGNED32:
	sprintf(s, "%lu", valuePtr->value.unsigned32);
	break;
    case SMI_BASETYPE_INTEGER32:
	sprintf(s, "%ld", valuePtr->value.integer32);
	break;
    case SMI_BASETYPE_UNSIGNED64:
	sprintf(s, "%llu", valuePtr->value.unsigned64);
	break;
    case SMI_BASETYPE_INTEGER64:
	sprintf(s, "%lld", valuePtr->value.integer64);
	break;
    case SMI_BASETYPE_FLOAT32:
    case SMI_BASETYPE_FLOAT64:
    case SMI_BASETYPE_FLOAT128:
	break;
    case SMI_BASETYPE_ENUM:
	sprintf(s, "%s", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_OCTETSTRING:
	if (valuePtr->format == SMI_VALUEFORMAT_TEXT) {
	    sprintf(s, "\"%s\"", valuePtr->value.ptr);
	} else if (valuePtr->format == SMI_VALUEFORMAT_HEXSTRING) {
	    sprintf(s, "'%*s'H", 2 * valuePtr->len, " ");
	    for (i=0; i < valuePtr->len; i++) {
		sprintf(ss, "%02x", valuePtr->value.ptr[i]);
		strncpy(&s[1+2*i], ss, 2);
	    }
	} else if (valuePtr->format == SMI_VALUEFORMAT_BINSTRING) {
	    sprintf(s, "'%*s'B", 8 * valuePtr->len, " ");
	    for (i=0; i < valuePtr->len; i++) {
		/* TODO */
		sprintf(ss, "%02x", valuePtr->value.ptr[i]);
		strncpy(&s[1+8*i], ss, 8);
	    }
	} else {
	    sprintf(s, "\"%s\"", valuePtr->value.ptr);
	}
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	if (valuePtr->value.bits) {
	    for(p = valuePtr->value.bits; *p; p++) {
		if (p != valuePtr->value.bits)
		    sprintf(&s[strlen(s)], ", ");
		sprintf(&s[strlen(s)], "%s", *p);
	    }
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	/* TODO */
	break;
    }

    return s;
}



static Import* addImport(char *module, char *name)
{
    Import **import, *newImport;
    
    for (import = &importList; *import; import = &(*import)->nextPtr) {
	int c = strcmp((*import)->module, module);
	if (c < 0) continue;
	if (c == 0) {
	    int d = strcmp((*import)->name, name);
	    if (d < 0) continue;
	    if (d == 0) return *import;
	    if (d > 0) break;
	}
	if (c > 0) break;
    }

    newImport = xmalloc(sizeof(Import));
    newImport->module = module;
    newImport->name = name;
    newImport->nextPtr = *import;
    *import = newImport;
	
    return *import;
}



static void createImportList(char *modulename)
{
    SmiNode     *smiNode;
    SmiType     *smiType;
    SmiNodekind kind = SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN;
    
    for(smiNode = smiGetFirstNode(modulename, kind);
	smiNode;
	smiNode = smiGetNextNode(smiNode, kind)) {

	smiType = smiGetType(smiNode->typemodule, smiNode->typename);
	if (smiType && strcmp(smiType->module, modulename)) {
	    if (strlen(smiType->module)) {
		addImport(smiType->module, smiType->name);
	    }
	}

	if (smiNode->basetype == SMI_BASETYPE_INTEGER32) {
	    addImport("SNMPv2-SMI", "Integer32");
	}
    }
}



static void freeImportList(void)
{
    Import *import, *freeme;

    for (import = importList; import; ) {
	freeme = import;
	import = import->nextPtr;
	xfree(freeme);
    }
    importList = NULL;
}



static void print(char *fmt, ...)
{
    va_list ap;
    char    s[200];
    char    *p;
    
    va_start(ap, fmt);
    current_column += vsprintf(s, fmt, ap);
    va_end(ap);

    fputs(s, stdout);

    if ((p = strrchr(s, '\n'))) {
	current_column = strlen(p) - 1;
    }
}



static void printSegment(int column, char *string, int length)
{
    char     s[200];

    if (length) {
	sprintf(s, "%*c%s%*c", 1 + column, ' ', string,
		length - strlen(string) - column, ' ');
    } else {
	sprintf(s, "%*c%s", 1 + column, ' ', string);
    }
    print(&s[1]);
}



static void printMultilineString(const char *s)
{
    int i;
    
    printSegment(INDENTTEXTS - 1, "\"", 0);
    if (s) {
	for (i=0; i < strlen(s); i++) {
	    if (s[i] != '\n') {
		print("%c", s[i]);
	    } else {
		print("\n");
		printSegment(INDENTTEXTS, "", 0);
	    }
	}
    }
    print("\"");
}



static void printMultiline(const char *s)
{
    int i;
    
    printSegment(INDENTTEXTS, "", 0);
    if (s) {
	for (i=0; i < strlen(s); i++) {
	    if (s[i] != '\n') {
		print("%c", s[i]);
	    } else {
		print("\n");
		printSegment(INDENTTEXTS, "", 0);
	    }
	}
    }
    print("\n");
}



static char* translate(char *s)
{
    int i;
    
    s = xstrdup(s);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
    }

    return s;
}



static int isGroup(SmiNode *smiNode)
{
    SmiNode *childNode;
    
    for(childNode = smiGetFirstChildNode(smiNode);
	childNode;
	childNode = smiGetNextChildNode(childNode)) {
	if ((childNode->nodekind == SMI_NODEKIND_SCALAR
	     || childNode->nodekind == SMI_NODEKIND_TABLE)
	    && current(childNode->status)) {
	    smiFreeNode(childNode);
	    return 1;
	}
    }

    return 0;
}


static void printDescription(SmiNode *smiNode, int indent)
{
    print("\n");
    printSegment(indent, "/*\n", 0);
    printMultiline(smiNode->description);
    if (smiNode->reference) {
	printSegment(indent, "REFERENCE:", 0);
	print("\n");
	printMultilineString(smiNode->reference);
	print("\n\n");
    }
    if (smiNode->units) {
	printSegment(indent, "UNITS:", 0);
	print("\n");
	printMultilineString(smiNode->units);
	print("\n\n");
    }
    printSegment(indent, "*/\n", 0);
}



static void printIndex(SmiNode *indexNode)
{
    SmiListItem *smiListItem;
    int      j;

    for (j = 0, smiListItem = smiGetFirstListItem(indexNode);
	 smiListItem;
	 j++, smiListItem = smiGetNextListItem(smiListItem)) {
	if (j) {
	    print(" ");
	}
	print(smiListItem->name);
	/* TODO: non-local name if non-local */
    } /* TODO: empty? -> print error */
}



static void printIncludes(char *modulename)
{
    Import    *import;
    char      *lastModulename = NULL;
    
    printf("#include <ASN1Types.idl>\n");
    printf("#include <SNMPMgmt.idl>\n");

    for(import = importList; import; import = import->nextPtr) {
	if (!lastModulename
	    || strcmp(lastModulename, import->module)) {
	    printf("#include <%s.idl>\n",
		   getIdlModuleName(import->module));
	    lastModulename = import->module;
	}
    }

    printf("\n");
}



static void printImportedTypedefs(char *modulename)
{
    Import    *import;
    int	      cnt = 0;
    char      *idlTypeName;

    for(import = importList; import; import = import->nextPtr) {
	cnt++;
	idlTypeName = getIdlTypeName(import->module, import->name);
	printSegment(INDENT, "typedef ", 0);
	print("%s::%s %s;\n",
	      getIdlModuleName(import->module), idlTypeName, idlTypeName);
    }

    if (cnt) {
	printf("\n");
    }
}



static void printModule(char *modulename)
{
    SmiRevision  *smiRevision;
    SmiModule    *smiModule;
    SmiNode      *smiNode;
    char         *idlModuleName;

    smiModule = smiGetModule(modulename);
    
    if (smiModule->object) {

	smiNode = smiGetNode(modulename, smiModule->object);

	if (smiNode) {
	    idlModuleName = getIdlModuleName(modulename);
	    printSegment(INDENT, "const ", 0);
	    print("string moduleIdentity = \"%s\";\n", smiNode->name);
	    printSegment(INDENT, "const ", 0);
	    print("ASN1_ObjectIdentifier %s = \"::%s::%s\";\n\n",
		  getIdlModuleName(smiNode->name),
		  idlModuleName, smiNode->name);
	    if (! silent) {
		printSegment(INDENT, "/*\n", 0);
		printMultiline(smiModule->description);
		print("\n");
		smiRevision = smiGetFirstRevision(modulename);
		printSegment(INDENT, "LAST-UPDATED:", INDENTVALUE);
		print(smiRevision
		      ? getTimeString(smiRevision->date) : "197001010000Z");
		print("\n\n");
		printSegment(INDENT, "ORGANIZATION:", 0);
		print("\n");
		printMultilineString(smiModule->organization);
		print("\n\n");
		printSegment(INDENT, "CONTACT-INFO:", 0);
		print("\n");
		printMultilineString(smiModule->contactinfo);
		print("\n\n");
		for(; smiRevision;
		    smiRevision = smiGetNextRevision(smiRevision)) {
		    if (strcmp(smiRevision->description,
	       "[Revision added by libsmi due to a LAST-UPDATED clause.]")) {
			printSegment(INDENT, "REVISION:", INDENTVALUE);
			print("\"%s\"\n", getTimeString(smiRevision->date));
			printSegment(INDENT, "REVISION-DESCRIPTION:", 0);
			print("\n");
			printMultilineString(smiRevision->description);
			print("\n\n");
		    }
		}
		printSegment(INDENT, "*/", 0);
		print("\n\n");
	    }
	    smiFreeNode(smiNode);
	}
    }
}



static void printType(SmiType *smiType)
{
    SmiNamedNumber *nn;
    char           *idlTypeName;
    char           *nnName;
    int            i;

    if (! silent) {
	if (smiType->description) {
	    printSegment(INDENT, "/*\n", 0);
	    printMultiline(smiType->description);
	    if (smiType->reference) {
		printSegment(INDENT, "REFERENCE:", 0);
		print("\n");
		printMultilineString(smiType->reference);
		print("\n\n");
	    }
	    if (smiType->format) {
		printSegment(INDENT, "DISPLAY-HINT:", 0);
		print(" %s\n", smiType->format);
		print("\n\n");
	    }
	    printSegment(INDENT, "*/\n", 0);
	}
    }
    idlTypeName = getIdlTypeName(smiType->module, smiType->name);
    printSegment(INDENT, "typedef ", 0);
    printf("%s %s; \n",
	   getBaseTypeString(smiType->basetype), idlTypeName);
    
    if (smiType->basetype == SMI_BASETYPE_ENUM) {
	for(nn = smiGetFirstNamedNumber(smiType->module, smiType->name);
	    nn;
	    nn = smiGetNextNamedNumber(nn)) {
	    printSegment(INDENT, "const ", 0);
	    nnName = translate(nn->name);
	    print("%s %s_%s = %s;\n", idlTypeName, idlTypeName, nnName,
		  getValueString(&nn->value));
	    xfree(nnName);
	}
	printSegment(INDENT, "const string ", 0);
	print("%s_NameNumberList = \"", idlTypeName);
	for(i = 0, nn = smiGetFirstNamedNumber(smiType->module, smiType->name);
	    nn;
	    i++, nn = smiGetNextNamedNumber(nn)) {
	    nnName = translate(nn->name);
	    if (i) {
		print(" , ");
	    }
	    print("%s (%s)", nnName, getValueString(&nn->value));
	    xfree(nnName);
	}
	print("\";\n");
    }
    print("\n");
}



static void printTypedefs(char *modulename)
{
    SmiNode        *smiNode;
    SmiType        *smiType;
    SmiNodekind    kind = SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN;
    
    for (smiType = smiGetFirstType(modulename);
	 smiType;
	 smiType = smiGetNextType(smiType)) {
	if (current(smiType->status)) {
	    printType(smiType);
	}
    }

    for (smiNode = smiGetFirstNode(modulename, kind);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, kind)) {
	if (current(smiNode->status)) {
	    smiType = smiGetType(smiNode->typemodule, smiNode->typename);
	    if (smiType) {
		if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
		    printType(smiType);
		}
		smiFreeType(smiType);
	    }
	}
    }
}



static void printAttribute(SmiNode *smiNode)
{
    char *idlTypeName = NULL, *idlNodeName;
    
    if (smiNode->access < SMI_ACCESS_READ_ONLY) {
	return;
    }

    idlNodeName = getIdlNodeName(smiNode->module, smiNode->name);
    if (! silent) {
	printDescription(smiNode, 2*INDENT);
    }
    printSegment(2*INDENT,
		 smiNode->access == SMI_ACCESS_READ_ONLY
		 ? "readonly attribute" : "attribute", 0);

    if (smiNode->typemodule) {
	idlTypeName = getIdlTypeName(smiNode->typemodule, smiNode->typename);
    } else {
	idlTypeName = getBaseTypeString(smiNode->basetype);
    }
    print(" %s %s;\n", idlTypeName, idlNodeName);
}



static void printGroupInterface(SmiNode *smiNode)
{
    SmiNode *childNode;
    char *idlNodeName;

    idlNodeName = getIdlNodeName(smiNode->module, smiNode->name);
    printSegment(INDENT, "interface", 0);
    print(" %s : SNMPMgmt::SmiEntry {\n", idlNodeName);

    for(childNode = smiGetFirstChildNode(smiNode);
	childNode;
	childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_TABLE
	    && current(childNode->status)) {
	    if (! silent) {
		printDescription(childNode, 2*INDENT);
	    }
	    printSegment(2*INDENT, "SNMPMgmt::SmiTableIterator", 0);
	    print(" get_%s();\n",
		  getIdlNodeName(childNode->module, childNode->name));
	}
	if (childNode->nodekind == SMI_NODEKIND_SCALAR
	    && current(childNode->status)) {
	    printAttribute(childNode);
	}
    }

    printSegment(INDENT, "};\n\n", 0);
}



static void printRowInterface(SmiNode *smiNode)
{
    SmiNode *childNode;
    char *idlModuleName, *idlNodeName;

    idlNodeName = getIdlNodeName(smiNode->module, smiNode->name);
    if (! silent) {
	printDescription(smiNode, INDENT);
    }
    printSegment(INDENT, "interface", 0);
    if (smiNode->indexkind == SMI_INDEX_AUGMENT
	|| smiNode->indexkind == SMI_INDEX_SPARSE) {
	idlModuleName = getIdlModuleName(smiNode->relatedmodule);
	print(" %s : %s::%s {\n", idlNodeName,
	      idlModuleName, smiNode->relatedname);
    } else {
	print(" %s : SNMPMgmt::SmiEntry {\n", idlNodeName);	
    }

    if (smiNode->indexkind == SMI_INDEX_INDEX
	|| smiNode->indexkind == SMI_INDEX_REORDER) {
	print("\n");
	printSegment(2*INDENT, "const ", 0);
	print("string IndexVarList = \"");
	printIndex(smiNode);
	print("\";\n");
    }

    /* SMI_INDEX_EXPAND ? */

    for(childNode = smiGetFirstChildNode(smiNode);
	childNode;
	childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_COLUMN
	    && current(childNode->status)) {
	    printAttribute(childNode);
	}
    }
    
    printSegment(INDENT, "};\n\n", 0);
}



static void printInterfaces(char *modulename)
{
    SmiNode *smiNode;

    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printGroupInterface(smiNode);
	}
	if (smiNode->nodekind == SMI_NODEKIND_ROW
	    && current(smiNode->status)) {
	    printRowInterface(smiNode);
	}
    }
}



static void printDefVals(char *modulename)
{
    SmiNode *smiNode;
    int     cnt = 0;
    char    *idlTypeName;
    
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	
	if (smiNode->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    cnt++;
	    if (cnt == 1) {
		printSegment(INDENT, "/* pseudo */\n", 0);
		printSegment(INDENT, "interface DefaultValues {\n", 0);
	    }
	    if (! silent) {
		printSegment(2*INDENT, "/* DEFVAL: ", 0);
		print(" %s */\n", getValueString(&smiNode->value));
	    }
	    printSegment(2*INDENT, "", 0);
	    if (smiNode->typemodule) {
		idlTypeName = getIdlTypeName(smiNode->typemodule,
					     smiNode->typename);
	    } else {
		idlTypeName = getBaseTypeString(smiNode->basetype);
	    }
	    print("%s %s();\n\n", idlTypeName, smiNode->name);
	}
    }

    if (cnt) {
	printSegment(INDENT, "};\n\n", 0);
    }
}



static void printDisplayHints(char *modulename)
{
    SmiType *smiType;
    int     cnt = 0;

    for (smiType = smiGetFirstType(modulename);
	 smiType;
	 smiType = smiGetNextType(smiType)) {
	if (current(smiType->status) && smiType->format) {
	    cnt++;
	    if (cnt == 1) {
		printSegment(INDENT, "/* pseudo */\n", 0);
		printSegment(INDENT, "interface TextualConventions {\n", 0);
	    }
	    print("\n");
	    if (! silent) {
		printSegment(2*INDENT, "/*\n", 0);
		printMultiline(smiType->description);
		if (smiType->reference) {
		    printSegment(2*INDENT, "REFERENCE:", 0);
		    print("\n");
		    printMultilineString(smiType->reference);
		}
		if (smiType->format) {
		    printSegment(2*INDENT, "DISPLAY-HINT:", 0);
		    print(" %s\n", smiType->format);
		}
		printSegment(2*INDENT, "*/\n", 0);
	    }
	    printSegment(2*INDENT, "", 0);
	    print("string %sToString (in %s Value);\n", smiType->name,
		  getIdlTypeName(smiType->module, smiType->name));
	    printSegment(2*INDENT, "", 0);
	    print("%s %sFromString (in string str);\n",
		  getIdlTypeName(smiType->module, smiType->name),
		  smiType->name);
	}
    }

    if (cnt) {
	printSegment(INDENT, "};\n\n", 0);
    }
}



int dumpCorbaIdl(char *modulename, int flags)
{
    SmiModule    *smiModule;
    char	 *idlModuleName;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    silent = (flags & SMIDUMP_FLAG_SILENT);

    printf("/*\n * This CORBA IDL file has been generated by smidump "
	   VERSION ". Do not edit.\n */\n\n");
    
    idlModuleName = getIdlModuleName(smiModule->name);
    createImportList(modulename);

    printf("#ifndef _%s_IDL_\n", idlModuleName);
    printf("#define _%s_IDL_\n\n", idlModuleName);

    printIncludes(modulename);

    printf("module %s {\n\n", idlModuleName);

    printImportedTypedefs(modulename);
    printModule(modulename);
    printTypedefs(modulename);
    printInterfaces(modulename);
    printDefVals(modulename);
    printDisplayHints(modulename);
    
    printf("};\n\n");
    printf("#endif /* !_%s_IDL_ */\n", idlModuleName);

    freeImportList();
    dictFree(&idlModuleNameList);
    dictFree(&idlNodeNameList);
    dictFree(&idlTypeNameList);
    smiFreeModule(smiModule);

    return 0;
}



static void printNameAndOid(SmiNode *smiNode, SmiNode *smiParentNode)
{
    int  i;
    char *idlModuleName;

    idlModuleName = getIdlModuleName(smiNode->module);

    if (smiParentNode) {
	printf("::%s::%s::%s ",
	       idlModuleName, smiParentNode->name, smiNode->name);
    } else {
	printf("::%s::%s ", idlModuleName, smiNode->name);
    }
    for (i = 0; i < smiNode->oidlen; i++) {
	printf("%s%u", i ? "." : "", smiNode->oid[i]);
    }
    printf(" ");
}



int dumpCorbaOid(char *modulename, int flags)
{
    SmiModule *smiModule;
    SmiNode   *smiNode;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	if (isGroup(smiNode)) {
            printNameAndOid(smiNode, NULL);
	    printf("Group not-accessible\n");
	    /* XXX what if the node is also of SMI_NODEKIND_MODULE ?? */
	    continue;
	}
	
	switch (smiNode->nodekind) {
	case SMI_NODEKIND_MODULE:
	    printNameAndOid(smiNode, NULL);
	    printf("ASN1_ObjectIdentifier not-accessible\n");
	    break;
	case SMI_NODEKIND_NODE:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, NULL);
		printf("ASN1_ObjectIdentifier not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_SCALAR:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, smiGetParentNode(smiNode));
		printf("%s %s\n",
		       getBaseTypeString(smiNode->basetype),
		       getAccessString(smiNode->access, 0));
	    }
	    break;
	case SMI_NODEKIND_TABLE:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, NULL);
		printf("Table not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_ROW:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, NULL);
		printf("TableEntry not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_COLUMN:
	    if (current(smiNode->status)) {
		SmiNode *smiParentNode = smiGetParentNode(smiNode);
		int create = smiParentNode ? smiParentNode->create : 0;
		printNameAndOid(smiNode, smiGetParentNode(smiNode));
		printf("%s %s\n",
		       getBaseTypeString(smiNode->basetype),
		       getAccessString(smiNode->access, create));
	    }
	    break;
	case SMI_NODEKIND_NOTIFICATION:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, smiGetParentNode(smiNode));
		printf("Notification not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_GROUP:
	    break;
	case SMI_NODEKIND_COMPLIANCE:
	    break;
	}
    }

    smiFreeModule(smiModule);
    
    return 0;
}
