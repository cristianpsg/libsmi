/*
 * dump-corba.c --
 *
 *      Operations to dump CORBA IDL and OID definitions. This is based
 *	on the JIDM Specification Translation developed by the JIDM task
 *	force, published as Open Group <URL:http://www.opengroup.org/>
 *	document C802 (ISBN 1-85912-256-6, January 2000).
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-corba.c,v 1.21 2000/02/24 15:30:01 strauss Exp $
 */

#include <config.h>

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
static IdlEntry *idlVBTypeNameList = NULL;

static IdlEntry **idlNameLists[] = {
    &idlModuleNameList, &idlNodeNameList,
    &idlTypeNameList, &idlVBTypeNameList,
    NULL
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



static char* getIdlVBTypeName(char *module, char *name, int *isnew)
{
    char *s, *vbTypeName;

    vbTypeName = xmalloc(strlen(name) + 10);
    sprintf(vbTypeName, "%sVBType", name);
    vbTypeName[0] = toupper((int) vbTypeName[0]);

    if (isnew) {
	*isnew = 0;
    }
    s = dictFindName(idlVBTypeNameList, module, vbTypeName);
    if (! s) {
	if (isnew) {
	    *isnew = 1;
	}
	s = dictAddName(&idlVBTypeNameList, module, vbTypeName);
    }

    xfree(vbTypeName);
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


static char *getIdlAnyTypeName(SmiNode *smiNode, SmiType *smiType)
{
    SmiModule *smiModule;
    char *typeName;
    
    if (! smiType->name) {
	smiModule = smiGetNodeModule(smiNode);
	if (smiModule && strlen(smiModule->name)) {
	    typeName = getIdlTypeName(smiModule->name, smiNode->name);
	} else {
	    typeName = getBaseTypeString(smiType->basetype);
	}
    } else {
	smiModule = smiGetTypeModule(smiType);
	if (smiModule && strlen(smiModule->name)) {
	    typeName = getIdlTypeName(smiModule->name, smiType->name);
	} else {
	    typeName = getBaseTypeString(smiType->basetype);
	}
    }

    return typeName;
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
	    sprintf(s, "'%*s'H", 2 * valuePtr->len, "");
	    for (i=0; i < valuePtr->len; i++) {
		sprintf(ss, "%02x", valuePtr->value.ptr[i]);
		strncpy(&s[1+2*i], ss, 2);
	    }
	} else if (valuePtr->format == SMI_VALUEFORMAT_BINSTRING) {
	    sprintf(s, "'%*s'B", 8 * valuePtr->len, "");
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
	    for (p = valuePtr->value.bits; *p; p++) {
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



static void createImportList(SmiModule *smiModule)
{
    SmiNode     *smiNode;
    SmiType     *smiType;
    SmiModule   *smiTypeModule;
    SmiNodekind kind = SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN;
    
    for (smiNode = smiGetFirstNode(smiModule, kind);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, kind)) {

	smiType = smiGetNodeType(smiNode);
	smiTypeModule = smiGetTypeModule(smiType);
	if (smiType && smiTypeModule &&
	    strcmp(smiTypeModule->name, smiModule->name)) {
	    if (strlen(smiTypeModule->name)) {
		addImport(smiTypeModule->name, smiType->name);
	    }
	}
	if (smiType->basetype == SMI_BASETYPE_INTEGER32) {
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
    print("%*c%s", column, ' ', string);
    if (length) {
	print("%*c", length - strlen(string) - column, ' ');
    }
}



static void printMultilineString(const char *s)
{
    int i, len;
    
    printSegment(INDENTTEXTS - 1, "\"", 0);
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    putc(s[i], stdout);
	    current_column++;
	    if (s[i] == '\n') {
		current_column = 0;
		printSegment(INDENTTEXTS, "", 0);
	    }
	}
    }
    putc('\"', stdout);
    current_column++;
}



static void printMultiline(const char *s)
{
    int i, len;
    
    printSegment(INDENTTEXTS, "", 0);
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    putc(s[i], stdout);
	    current_column++;
	    if (s[i] == '\n') {
		current_column = 0;
		printSegment(INDENTTEXTS, "", 0);
	    }
	}
    }
    putc('\n', stdout);
    current_column++;
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
    
    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if ((childNode->nodekind == SMI_NODEKIND_SCALAR
	     || childNode->nodekind == SMI_NODEKIND_TABLE)
	    && current(childNode->status)) {
	    return 1;
	}
    }

    return 0;
}



static int isCreatable(SmiNode *smiNode)
{
    SmiNode *childNode;
    
    if ((isGroup(smiNode) || smiNode->nodekind == SMI_NODEKIND_ROW)
	&& current(smiNode->status)) {

	for (childNode = smiGetFirstChildNode(smiNode);
	     childNode;
	     childNode = smiGetNextChildNode(childNode)) {
	    
	    if ((childNode->nodekind == SMI_NODEKIND_SCALAR
		 || childNode->nodekind == SMI_NODEKIND_COLUMN)
		&& current(childNode->status)
		&& childNode->access == SMI_ACCESS_READ_WRITE) {
		return 1;
	    }
	}
    }

    return 0;
}



static void printDescription(SmiNode *smiNode, int indent)
{
    print("\n");
    printSegment(indent, "/*\n", 0);
    if (smiNode->description) {
	printMultiline(smiNode->description);
    }
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
    SmiElement *smiElement;
    int        j;

    for (j = 0, smiElement = smiGetFirstElement(indexNode);
	 smiElement;
	 j++, smiElement = smiGetNextElement(smiElement)) {
	if (j) {
	    print(" ");
	}
	print(smiGetElementNode(smiElement)->name);
	/* TODO: non-local name if non-local */
    } /* TODO: empty? -> print error */
}



static void printIncludes(SmiModule *smiModule)
{
    Import    *import;
    char      *lastModulename = NULL;
    
    print("#include <ASN1Types.idl>\n");
    print("#include <SNMPMgmt.idl>\n");

    for (import = importList; import; import = import->nextPtr) {
	if (!lastModulename
	    || strcmp(lastModulename, import->module)) {
	    print("#include <%s.idl>\n",
		  getIdlModuleName(import->module));
	    lastModulename = import->module;
	}
    }

    print("\n");
}



static void printImportedTypedefs(SmiModule *smiModule)
{
    Import    *import;
    int	      cnt = 0;
    char      *idlTypeName;

    for (import = importList; import; import = import->nextPtr) {
	cnt++;
	idlTypeName = getIdlTypeName(import->module, import->name);
	printSegment(INDENT, "typedef ", 0);
	print("%s::%s %s;\n",
	      getIdlModuleName(import->module), idlTypeName, idlTypeName);
    }

    if (cnt) {
	print("\n");
    }
}



static void printModule(SmiModule *smiModule)
{
    SmiRevision  *smiRevision;
    SmiNode      *smiNode;
    char         *idlModuleName;

    smiNode = smiGetModuleIdentityNode(smiModule);

    if (smiNode) {

	idlModuleName = getIdlModuleName(smiModule->name);
	printSegment(INDENT, "const ", 0);
	print("string moduleIdentity = \"%s\";\n", smiNode->name);
	printSegment(INDENT, "const ", 0);
	print("ASN1_ObjectIdentifier %s = \"::%s::%s\";\n\n",
	      getIdlModuleName(smiNode->name),
	      idlModuleName, smiNode->name);
	if (! silent) {
	    printSegment(INDENT, "/*\n", 0);
	    if (smiModule->description) {
		printMultiline(smiModule->description);
		print("\n");
	    }
	    smiRevision = smiGetFirstRevision(smiModule);
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
	    for (; smiRevision;
		 smiRevision = smiGetNextRevision(smiRevision)) {
		if (! smiRevision->description ||
		    strcmp(smiRevision->description,
			   "[Revision added by libsmi due to a LAST-UPDATED clause.]")) {
		    printSegment(INDENT, "REVISION:", INDENTVALUE);
		    print("\"%s\"\n", getTimeString(smiRevision->date));
		    printSegment(INDENT, "REVISION-DESCRIPTION:", 0);
		    print("\n");
		    if (smiRevision->description) {
			printMultilineString(smiRevision->description);
		    } else {
			printMultilineString("...");
		    }
		    print("\n\n");
		}
	    }
	    printSegment(INDENT, "*/", 0);
	    print("\n\n");
	}
    }

}



static void printType(SmiNode *smiNode, SmiType *smiType)
{
    SmiNamedNumber *nn;
    char           *idlTypeName;
    char           *nnName;
    int            i;

    if (! silent) {
	if (smiType->name) {
	    printSegment(INDENT, "/*\n", 0);
	    if (smiType->description) {
		printMultiline(smiType->description);
	    }
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
    if (! smiType->name) {
	idlTypeName = getIdlTypeName(smiGetNodeModule(smiNode)->name,
				     smiNode->name);
    } else {
	idlTypeName = getIdlTypeName(smiGetTypeModule(smiType)->name,
				     smiType->name);
    }
    printSegment(INDENT, "typedef ", 0);
    print("%s %s; \n",
	  getBaseTypeString(smiType->basetype), idlTypeName);
    
    if (smiType->basetype == SMI_BASETYPE_ENUM) {
	for (nn = smiGetFirstNamedNumber(smiType);
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
	for (i = 0, nn = smiGetFirstNamedNumber(smiType);
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



static void printTypedefs(SmiModule *smiModule)
{
    SmiNode        *smiNode;
    SmiType        *smiType;
    SmiNodekind    kind = SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN;
    
    for (smiType = smiGetFirstType(smiModule);
	 smiType;
	 smiType = smiGetNextType(smiType)) {
	if (current(smiType->status)) {
	    printType(NULL, smiType);
	}
    }

    for (smiNode = smiGetFirstNode(smiModule, kind);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, kind)) {
	if (current(smiNode->status)) {
	    smiType = smiGetNodeType(smiNode);
	    if (smiType && ! smiType->name) {
		printType(smiNode, smiType);
	    }
	}
    }
}



static void printAttribute(SmiNode *smiNode)
{
    char *idlTypeName = NULL, *idlNodeName;
    SmiType *smiType;
    SmiModule *smiModule;
    
    if (smiNode->access < SMI_ACCESS_READ_ONLY) {
	return;
    }

    smiModule = smiGetNodeModule(smiNode);
    idlNodeName = getIdlNodeName(smiModule->name, smiNode->name);
    if (! silent) {
	printDescription(smiNode, 2*INDENT);
    }
    printSegment(2*INDENT,
		 smiNode->access == SMI_ACCESS_READ_ONLY
		 ? "readonly attribute" : "attribute", 0);

    smiType = smiGetNodeType(smiNode);
    idlTypeName = getIdlAnyTypeName(smiNode, smiType);
    print(" %s %s;\n", idlTypeName, idlNodeName);
}



static void printGroupInterface(SmiNode *smiNode)
{
    SmiNode *childNode;
    char *idlNodeName;
    SmiModule *smiModule, *childModule;

    smiModule = smiGetNodeModule(smiNode);
    idlNodeName = getIdlNodeName(smiModule->name, smiNode->name);
    printSegment(INDENT, "interface", 0);
    print(" %s : SNMPMgmt::SmiEntry {\n", idlNodeName);

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_TABLE
	    && current(childNode->status)) {
	    if (! silent) {
		printDescription(childNode, 2*INDENT);
	    }
	    printSegment(2*INDENT, "SNMPMgmt::SmiTableIterator", 0);
	    childModule = smiGetNodeModule(childNode);
	    print(" get_%s();\n", getIdlNodeName(childModule->name,
						 childNode->name));
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
    SmiNode *childNode, *relatedNode;
    char *idlModuleName, *idlNodeName;

    idlNodeName = getIdlNodeName(smiGetNodeModule(smiNode)->name,
				 smiNode->name);
    if (! silent) {
	printDescription(smiNode, INDENT);
    }
    printSegment(INDENT, "interface", 0);
    if (smiNode->indexkind == SMI_INDEX_AUGMENT
	|| smiNode->indexkind == SMI_INDEX_SPARSE) {
	relatedNode = smiGetRelatedNode(smiNode);
	idlModuleName = getIdlModuleName(smiGetNodeModule(relatedNode)->name);
	print(" %s : %s::%s {\n", idlNodeName,
	      idlModuleName, relatedNode->name);
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

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_COLUMN
	    && current(childNode->status)) {
	    printAttribute(childNode);
	}
    }
    
    printSegment(INDENT, "};\n\n", 0);
}



static void printInterfaces(SmiModule *smiModule)
{
    SmiNode *smiNode;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
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



static void printConstructor(SmiNode *smiNode)
{
    SmiNode *childNode;
    SmiType *smiType;
    SmiModule *smiModule;
    char    *idlNodeName;
    char    *idlChildNodeName, *idlChildTypeName;
    int	    cnt = 0;

    smiModule = smiGetNodeModule(smiNode);
    idlNodeName = getIdlNodeName(smiModule->name, smiNode->name);

    print("\n");
    printSegment(2*INDENT, "", 0);
    print("%s create_%s (\n", idlNodeName, idlNodeName);

    /* XXX I think we need to include INDEX columns here XXX */
    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if ((childNode->nodekind == SMI_NODEKIND_SCALAR
	     || childNode->nodekind == SMI_NODEKIND_COLUMN)
	    && current(childNode->status)
	    && childNode->access == SMI_ACCESS_READ_WRITE) {

	    cnt++;
	    idlChildNodeName =
		getIdlNodeName(smiGetNodeModule(childNode)->name,
			       childNode->name);
	    smiType = smiGetNodeType(childNode);
	    idlChildTypeName = getIdlAnyTypeName(childNode, smiType);
	    if (cnt > 1) {
		print(",\n");
	    }
	    printSegment(3*INDENT, "in ", 0);
	    print("%s %s", idlChildTypeName, idlChildNodeName);
	}
    }
    print("\n");
    
    printSegment(2*INDENT, ") raises (\n", 0);
    printSegment(3*INDENT, "SNMPMgmt::AlreadyExists,\n", 0);
    printSegment(3*INDENT, "CosLifeCycle::InvalidCriteria,\n", 0);
    printSegment(3*INDENT, "CosLifeCycle::CannotMeetCriteria\n", 0);
    printSegment(2*INDENT, ");\n", 0);
}



static void printFactory(SmiModule *smiModule)
{
    SmiNode *smiNode;
    int	    cnt = 0;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	
	if (isCreatable(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		printSegment(INDENT, "interface SmiEntryFactory : "
			     "SNMPMgmt::GenericFactory {\n", 0);
	    }
	    printConstructor(smiNode);
	}
    }

    if (cnt) {
	printSegment(INDENT, "};\n\n", 0);
    }
}



static void printNotificationVBTypes(SmiModule *smiModule)
{
    SmiNode     *smiNode, *listSmiNode;
    SmiElement  *smiElement;
    SmiType	*smiType;
    char	*idlTypeName;
    char	*idlVBTypeName;
    int		isnew;
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	for (smiElement = smiGetFirstElement(smiNode);
	     smiElement; smiElement = smiGetNextElement(smiElement)) {
	    listSmiNode = smiGetElementNode(smiElement);
	    idlVBTypeName = getIdlVBTypeName(
		                           smiGetNodeModule(listSmiNode)->name,
					   listSmiNode->name, &isnew);
	    if (isnew && listSmiNode) {
		printSegment(INDENT, "struct ", 0);
		print("%s {\n", idlVBTypeName);
		printSegment(2*INDENT, "string var_name;\n", 0);
		printSegment(2*INDENT, "string var_index;\n", 0);
		printSegment(2*INDENT, "", 0);
		smiType = smiGetNodeType(listSmiNode);
		idlTypeName = getIdlAnyTypeName(listSmiNode, smiType);
		print("%s %s;\n", idlTypeName,
		                  smiGetElementNode(smiElement)->name);
		printSegment(INDENT, "};\n\n", 0);
	    }
	}
    }
}



static void printNotificationTypes(SmiModule *smiModule)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    char	*idlTypeName;
    char	*idlVBTypeName;
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	if ((smiElement = smiGetFirstElement(smiNode))) {
	    idlTypeName = getIdlTypeName(smiGetNodeModule(smiNode)->name,
					 smiNode->name);
	    printSegment(INDENT, "struct ", 0);
	    print("%s {\n", idlTypeName);
	    for (; smiElement; smiElement = smiGetNextElement(smiElement)) {
		idlVBTypeName = getIdlVBTypeName(smiGetNodeModule(
		                          smiGetElementNode(smiElement))->name,
				    smiGetElementNode(smiElement)->name, NULL);
		printSegment(2*INDENT, "", 0);
		print("%s %s;\n", idlVBTypeName,
		      smiGetElementNode(smiElement)->name);
	    }
	    printSegment(INDENT, "};\n\n", 0);
	}
    }
}



static void printPushNotifications(SmiModule *smiModule)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    char        *idlNodeName;
    char	*idlTypeName;
    int         cnt = 0;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	cnt++;
	if (cnt == 1) {
	    if (! silent) {
		printSegment(INDENT,
			     "/* typed push event communication */\n", 0);
	    }
	    printSegment(INDENT, "interface Notifications : ", 0);
	    print("SNMPMgmt::Notifications {\n");
	}
	idlNodeName = getIdlNodeName(smiModule->name,
				     smiNode->name);
	if (! silent) {
	    printDescription(smiNode, 2*INDENT);
	}
	printSegment(2*INDENT, "void ", 0);
	print("%s (\n", idlNodeName);
	printSegment(3*INDENT, "in CosNaming::Name src_entry_name,\n", 0);
	printSegment(3*INDENT, "in CORBA::ScopedName event_type,\n", 0);
	printSegment(3*INDENT, "in ASN1_GeneralizedTime event_time", 0);
	if ((smiElement = smiGetFirstElement(smiNode))) {
	    idlTypeName = getIdlTypeName(smiModule->name, smiNode->name);
	    print(",\n");
	    printSegment(3*INDENT, "in ", 0);
	    print("%s notification_info", idlTypeName);
	}
	print("\n");
	printSegment(2*INDENT, ");\n", 0);
    }    

    if (cnt) {
	printSegment(INDENT, "};\n\n", 0);
    }
}



static void printPullNotifications(SmiModule *smiModule)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    int         cnt = 0;
    char        *idlNodeName;
    char	*idlTypeName;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	cnt++;
	if (cnt == 1) {
	    if (! silent) {
		printSegment(INDENT,
			     "/* typed pull event communication */\n", 0);
	    }
	    printSegment(INDENT, "interface PullNotifications : ", 0);
	    print("SNMPMgmt::PullNotifications {\n");
	}
	idlNodeName = getIdlNodeName(smiModule->name, smiNode->name);
	
	if (! silent) {
	    printDescription(smiNode, 2*INDENT);
	}
	printSegment(2*INDENT, "void ", 0);
	print("pull_%s (\n", idlNodeName);
	printSegment(3*INDENT, "out CosNaming::Name src_entry_name,\n", 0);
	printSegment(3*INDENT, "out CORBA::ScopedName event_type,\n", 0);
	printSegment(3*INDENT, "out ASN1_GeneralizedTime event_time", 0);
	if ((smiElement = smiGetFirstElement(smiNode))) {
	    idlTypeName = getIdlTypeName(smiModule->name, smiNode->name);
	    print(",\n");
	    printSegment(3*INDENT, "out ", 0);
	    print("%s notification_info", idlTypeName);
	}
	print("\n");
	printSegment(2*INDENT, ");\n", 0);
	printSegment(2*INDENT, "boolean ", 0);
	print("try_%s (\n", idlNodeName);
	printSegment(3*INDENT, "out CosNaming::Name src_entry_name,\n", 0);
	printSegment(3*INDENT, "out CORBA::ScopedName event_type,\n", 0);
	printSegment(3*INDENT, "out ASN1_GeneralizedTime event_time", 0);
	if ((smiElement = smiGetFirstElement(smiNode))) {
	    char *idlTypeName;
	    idlTypeName = getIdlTypeName(smiModule->name, smiNode->name);
	    print(",\n");
	    printSegment(3*INDENT, "out ", 0);
	    print("%s notification_info", idlTypeName);
	}
	print("\n");
	printSegment(2*INDENT, ");\n", 0);
    }    

    if (cnt) {
	printSegment(INDENT, "};\n\n", 0);
    }
}



static void printDefVals(SmiModule *smiModule)
{
    SmiNode *smiNode;
    SmiType *smiType;
    int     cnt = 0;
    char    *idlTypeName;
    
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
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
	    smiType = smiGetNodeType(smiNode);
	    idlTypeName = getIdlAnyTypeName(smiNode, smiType);
	    print("%s %s();\n\n", idlTypeName, smiNode->name);
	}
    }

    if (cnt) {
	printSegment(INDENT, "};\n\n", 0);
    }
}



static void printDisplayHints(SmiModule *smiModule)
{
    SmiType *smiType;
    int     cnt = 0;

    for (smiType = smiGetFirstType(smiModule);
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
		if (smiType->description) {
		    printMultiline(smiType->description);
		}
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
		  getIdlTypeName(smiGetTypeModule(smiType)->name,
				 smiType->name));
	    printSegment(2*INDENT, "", 0);
	    print("%s %sFromString (in string str);\n",
		  getIdlTypeName(smiGetTypeModule(smiType)->name,
				 smiType->name),
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

    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for CORBA-IDL format\n");
	exit(1);
    }
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    silent = (flags & SMIDUMP_FLAG_SILENT);

    print("/*\n * This CORBA IDL file has been generated by smidump "
	  VERSION ". Do not edit.\n */\n\n");
    
    idlModuleName = getIdlModuleName(smiModule->name);
    createImportList(smiModule);

    print("#ifndef _%s_IDL_\n", idlModuleName);
    print("#define _%s_IDL_\n\n", idlModuleName);

    printIncludes(smiModule);

    print("module %s {\n\n", idlModuleName);

    printImportedTypedefs(smiModule);
    printModule(smiModule);
    printTypedefs(smiModule);
    printInterfaces(smiModule);
    printNotificationVBTypes(smiModule);
    printNotificationTypes(smiModule);
    printPushNotifications(smiModule);
    printPullNotifications(smiModule);
    printFactory(smiModule);
    printDefVals(smiModule);
    printDisplayHints(smiModule);
    
    print("};\n\n");
    print("#endif /* !_%s_IDL_ */\n", idlModuleName);

    freeImportList();
    dictFree(&idlModuleNameList);
    dictFree(&idlNodeNameList);
    dictFree(&idlTypeNameList);
    dictFree(&idlVBTypeNameList);

    return 0;
}



static void printNameAndOid(SmiNode *smiNode, SmiNode *smiParentNode)
{
    int  i;
    char *idlModuleName;

    idlModuleName = getIdlModuleName(smiGetNodeModule(smiNode)->name);

    if (smiParentNode) {
	print("::%s::%s::%s ",
	       idlModuleName, smiParentNode->name, smiNode->name);
    } else {
	print("::%s::%s ", idlModuleName, smiNode->name);
    }
    for (i = 0; i < smiNode->oidlen; i++) {
	print("%s%u", i ? "." : "", smiNode->oid[i]);
    }
    print(" ");
}



int dumpCorbaOid(char *modulename, int flags)
{
    SmiModule *smiModule;
    SmiNode   *smiNode;
    SmiType   *smiType;

    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for CORBA-OID format\n");
	exit(1);
    }
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	if (isGroup(smiNode)) {
            printNameAndOid(smiNode, NULL);
	    print("Group not-accessible\n");
	    /* XXX what if the node is also of SMI_NODEKIND_MODULE ?? */
	    continue;
	}

	smiType = smiGetNodeType(smiNode);
	
	switch (smiNode->nodekind) {
	case SMI_NODEKIND_NODE:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, NULL);
		print("ASN1_ObjectIdentifier not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_SCALAR:
	    if (current(smiNode->status)) {
		SmiNode *smiParentNode = smiGetParentNode(smiNode);
		printNameAndOid(smiNode, smiParentNode);
		print("%s %s\n",
		      getBaseTypeString(smiType->basetype),
		      getAccessString(smiNode->access, 0));
	    }
	    break;
	case SMI_NODEKIND_TABLE:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, NULL);
		print("Table not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_ROW:
	    if (current(smiNode->status)) {
		printNameAndOid(smiNode, NULL);
		print("TableEntry not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_COLUMN:
	    if (current(smiNode->status)) {
		SmiNode *smiParentNode = smiGetParentNode(smiNode);
		int create = smiParentNode ? smiParentNode->create : 0;
		printNameAndOid(smiNode, smiParentNode);
		print("%s %s\n",
		      getBaseTypeString(smiType->basetype),
		      getAccessString(smiNode->access, create));
	    }
	    break;
	case SMI_NODEKIND_NOTIFICATION:
	    if (current(smiNode->status)) {
		SmiNode *smiParentNode = smiGetParentNode(smiNode);
		printNameAndOid(smiNode, smiParentNode);
		print("Notification not-accessible\n");
	    }
	    break;
	case SMI_NODEKIND_GROUP:
	    break;
	case SMI_NODEKIND_COMPLIANCE:
	    break;
	}
    }

    return 0;
}
