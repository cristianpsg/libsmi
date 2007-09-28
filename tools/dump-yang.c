/*
 * dump-yang.c --
 *
 *      Operations to dump MIB modules in the YANG output format.
 *
 * Copyright (c) 2007 J. Schoenwaelder, Jacobs University Bremen.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#include <config.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"
#include "fprint.h"

/*
 * TODO:
 * - reproduce the table comment text as a yang comment
 * - move absolute augments to the top-level
 * - peek into format strings to determine whether we use string or
 *   binary as the base type
 * - translate notifications properly
 */

static int sflag = 0;		/* generate smi: extensions */
static int INDENT = 2;		/* indent factor */

#define  INDENTVALUE	20   /* column to start values, except multiline */
#define	 URNBASE	"urn:ietf:params:xml:ns:yang:smiv2:"


/*
 * Structure used to build a list of imported types.
 */

typedef struct Import {
    char          *module;
    char	  *prefix;
    struct Import *nextPtr;
} Import;

static Import *importList = NULL;

static int silent = 0;


static char*
getStringStatus(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "current" :
	(status == SMI_STATUS_OPTIONAL)    ? "current" :
					     "<unknown>";
}



static char*
getStringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<unknown>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "binary" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "yang:object-identifier" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "uint32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "int32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "uint64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "int64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "enumeration" :
        (basetype == SMI_BASETYPE_BITS)              ? "bits" :
                                                   "<unknown>";
}



static char*
getStringDate(time_t t)
{
    static char   s[27];
    struct tm	  *tm;

    tm = gmtime(&t);
    sprintf(s, "%04d-%02d-%02d",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    return s;
}



static char*
getModuleUrn(const char *moduleName)
{
    static const char *prefix = URNBASE;
    static char *buf = NULL;
    static size_t buflen = 0;
    size_t prefix_len;
    size_t name_len;

    prefix_len = strlen(prefix);
    name_len = strlen(moduleName);
    
    if (buflen < prefix_len + name_len + 1) {
	if (buf) xfree(buf);
	buflen = prefix_len + name_len + 1;
	buf = xmalloc(buflen);
    }

    strcpy(buf, prefix);
    strcat(buf, moduleName);

    return buf;
}



static char*
getValueString(SmiValue *valuePtr, SmiType *typePtr)
{
    static char    s[1024];
    char           ss[9];
    int		   n;
    unsigned int   i;
    SmiNamedNumber *nn;
    SmiNode        *nodePtr;
    
    s[0] = 0;
    
    switch (valuePtr->basetype) {
    case SMI_BASETYPE_UNSIGNED32:
	sprintf(s, "%lu", valuePtr->value.unsigned32);
	break;
    case SMI_BASETYPE_INTEGER32:
	sprintf(s, "%ld", valuePtr->value.integer32);
	break;
    case SMI_BASETYPE_UNSIGNED64:
	sprintf(s, UINT64_FORMAT, valuePtr->value.unsigned64);
	break;
    case SMI_BASETYPE_INTEGER64:
	sprintf(s, INT64_FORMAT, valuePtr->value.integer64);
	break;
    case SMI_BASETYPE_FLOAT32:
    case SMI_BASETYPE_FLOAT64:
    case SMI_BASETYPE_FLOAT128:
	break;
    case SMI_BASETYPE_ENUM:
	for (nn = smiGetFirstNamedNumber(typePtr); nn;
	     nn = smiGetNextNamedNumber(nn)) {
	    if (nn->value.value.unsigned32 == valuePtr->value.unsigned32)
		break;
	}
	if (nn) {
	    sprintf(s, "%s", nn->name);
	} else {
	    sprintf(s, "%ld", valuePtr->value.integer32);
	}
	break;
    case SMI_BASETYPE_OCTETSTRING:
	for (i = 0; i < valuePtr->len; i++) {
	    if (!isprint((int)valuePtr->value.ptr[i])) break;
	}
	if (i == valuePtr->len) {
	    sprintf(s, "\"%s\"", valuePtr->value.ptr);
	} else {
            sprintf(s, "0x%*s", 2 * valuePtr->len, "");
            for (i=0; i < valuePtr->len; i++) {
                sprintf(ss, "%02x", valuePtr->value.ptr[i]);
                strncpy(&s[2+2*i], ss, 2);
            }
	}
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	for (i = 0, n = 0; i < valuePtr->len * 8; i++) {
	    if (valuePtr->value.ptr[i/8] & (1 << i%8)) {
		if (n)
		    sprintf(&s[strlen(s)], ", ");
		n++;
		for (nn = smiGetFirstNamedNumber(typePtr); nn;
		     nn = smiGetNextNamedNumber(nn)) {
		    if (nn->value.value.unsigned32 == i)
			break;
		}
		if (nn) {
		    sprintf(&s[strlen(s)], "%s", nn->name);
		} else {
		    sprintf(s, "%d", i);
		}
	    }
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	nodePtr = smiGetNodeByOID(valuePtr->len, valuePtr->value.oid);
	if (nodePtr) {
	    sprintf(s, "%s", nodePtr->name);
	} else {
	    strcpy(s, "");
	    for (i=0; i < valuePtr->len; i++) {
		if (i) strcat(s, ".");
		sprintf(&s[strlen(s)], "%u", valuePtr->value.oid[i]);
	    }
	}
	break;
    }

    return s;
}


static int
isPrefixUnique(const char *prefix)
{
    Import *import;

    for (import = importList; import; import = import->nextPtr) {
         if (strcmp(prefix, import->prefix) == 0) {
             return 0;
	 }
    }

    return 1;
}


static char*
guessNicePrefix(const char *moduleName)
{
    char *prefix;
    int i;

    char *specials[] = {
	    "SNMPv2-TC", "smi",
	    "SNMPv2-MIB", "snmp",
	    NULL, NULL
    };

    for (i = 0; specials[i]; i +=2) {
	if (strcmp(moduleName, specials[i]) == 0) {
            if (isPrefixUnique(specials[i+1])) {
                return xstrdup(specials[i+1]);
	    }
	}
    }

    prefix = xstrdup(moduleName);
    for (i = 0; prefix[i]; i++) {
        prefix[i] = tolower(prefix[i]);
    }

    for (i = 0; prefix[i]; i++) {
        if (prefix[i] == '-') {
            prefix[i] = 0;
	    if (isPrefixUnique(prefix)) {
                return prefix;
	    }
	    prefix[i] = '-';
	}
    }

    return prefix;
}


static const char*
getModulePrefix(const char *moduleName)
{
    Import *import;
    static char *prefix = NULL;

    for (import = importList; import; import = import->nextPtr) {
        if (strcmp(moduleName, import->module) == 0) {
            return import->prefix;
	}
    }

    if (prefix) xfree(prefix);
    prefix = guessNicePrefix(moduleName);
    return prefix;
}



static Import*
addImport(char *module, char *name)
{
    Import **import, *newImport;
    
    if (!module || !name) {
	return NULL;
    }
	    
    for (import = &importList; *import; import = &(*import)->nextPtr) {
	int c = strcmp((*import)->module, module);
	if (c == 0) return *import;
	if (c > 0) break;
    }

    newImport = xmalloc(sizeof(Import));
    newImport->module = module;
    newImport->prefix = guessNicePrefix(module);

    newImport->nextPtr = *import;
    *import = newImport;
	
    return *import;
}



static void
createImportList(SmiModule *smiModule)
{
    SmiImport   *smiImport;

    addImport("yang-types", "");
    addImport("inet-types", "");
    
    for (smiImport = smiGetFirstImport(smiModule); smiImport;
	 smiImport = smiGetNextImport(smiImport)) {
	    if (strcmp(smiImport->module, "SNMPv2-CONF") != 0
		    && strcmp(smiImport->module, "SNMPv2-SMI") != 0) {
		    addImport(smiImport->module, smiImport->name);
	    }
    }
}



static void
freeImportList(void)
{
    Import *import, *freeme;

    for (import = importList; import; ) {
        xfree(import->prefix);
	freeme = import;
	import = import->nextPtr;
	xfree(freeme);
    }
    importList = NULL;
}



static int
isGroup(SmiNode *smiNode)
{
    SmiNode *childNode;
    
    for(childNode = smiGetFirstChildNode(smiNode);
	childNode;
	childNode = smiGetNextChildNode(childNode)) {
	if ((childNode->nodekind == SMI_NODEKIND_SCALAR
	     || childNode->nodekind == SMI_NODEKIND_TABLE)
	    && childNode->status == SMI_STATUS_CURRENT) {
	    return 1;
	}
    }

    return 0;
}


static void
fprintRevisions(FILE *f, int indent, SmiModule *smiModule)
{
    int i;
    SmiRevision *smiRevision;
    
    for(i = 0, smiRevision = smiGetFirstRevision(smiModule);
	smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	fprintSegment(f, indent, "revision ", 0);
	fprint(f, "\"%s\" {\n", getStringDate(smiRevision->date));
	fprintSegment(f, 2 * indent, "description", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, 2 * indent, smiRevision->description);
	fprint(f, ";\n");
        fprintSegment(f, indent, "}\n", 0);
	i++;
    }
    if (i) {
	fprint(f, "\n");
    }
}


static void
fprintImports(FILE *f, SmiModule *smiModule)
{
    Import *import;
    int len = 4;
    
    for (import = importList; import; import = import->nextPtr) {
	if (strlen(import->module) > len) len = strlen(import->module);
    }
    
    for (import = importList; import; import = import->nextPtr) {
	fprintSegment(f, INDENT, "import", 0);
	fprint(f, " %-*s { prefix \"%s\"; }\n", len,
	       import->module, import->prefix);
    }
    fprint(f, "\n");

}




static void
fprintSubtype(FILE *f, int indent, SmiType *smiType)
{
    SmiRange       *range;
    SmiNamedNumber *nn;
    char	   s[1024];
    char	   *tkw, *lkw, *vkw;
    int		   i = 0;
    int		   len = 4;

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for (nn = smiGetFirstNamedNumber(smiType);
	     nn ; nn = smiGetNextNamedNumber(nn)) {
	    if (strlen(nn->name) > len) len = strlen(nn->name);
	}
	
	for(i = 0, nn = smiGetFirstNamedNumber(smiType);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (! i) {
		fprint(f, " {\n");
	    }
	    tkw = (smiType->basetype == SMI_BASETYPE_BITS) ? "bits" : "enumeration";
	    lkw = (smiType->basetype == SMI_BASETYPE_BITS) ? "bit" : "enum";
	    vkw = (smiType->basetype == SMI_BASETYPE_BITS) ? "position" : "value";
	    sprintf(s, "%s %-*s \{ %s %s; }\n",
		    lkw, len, nn->name,
		    vkw, getValueString(&nn->value, smiType));
	    fprintSegment(f, indent + INDENT, s, 0);
	}
	if (i) {
	    fprintSegment(f, indent, "}\n", 0);
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType);
	    range ; i++, range = smiGetNextRange(range)) {
	    if (i) {
		fprint(f, " | ");
	    } else {
		fprint(f, " {\n");
		if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		    fprintSegment(f, indent + INDENT, "length \"", 0);
		} else {
		    fprintSegment(f, indent + INDENT, "range \"", 0);
		}
	    }	    
	    if (memcmp(&range->minValue, &range->maxValue,
		       sizeof(SmiValue))) {
		sprintf(s, "%s", getValueString(&range->minValue, smiType));
		sprintf(&s[strlen(s)], "..%s", 
			getValueString(&range->maxValue, smiType));
	    } else {
		sprintf(s, "%s", getValueString(&range->minValue, smiType));
	    }
	    fprint(f, s);
	}
	if (i) {
	    fprint(f, "\";\n");
	    fprintSegment(f, indent, "}\n", 0);
	}
    }
    if (! i) {
	fprint(f, ";\n");
    }
}



static void
fprintType(FILE *f, int indent, SmiModule *thisModule, SmiType *smiType)
{
    SmiModule *smiModule = NULL;
    SmiType *parentType = NULL;
    SmiModule *parentModule = NULL;
    int i;

    const char *special[] = {
	    "SNMPv2-SMI", "Counter32", "yang:counter32",
	    "SNMPv2-SMI", "Counter64", "yang:counter64",
	    "SNMPv2-SMI", "Gauge32",   "yang:gauge32",
	    "SNMPv2-SMI", "TimeTicks", "yang:timeticks",
	    "SNMPv2-SMI", "IpAddress", "inet:ipv4-address",
	    NULL, NULL, NULL
    };

    fprintSegment(f, indent, "type", 0);

    smiModule = smiGetTypeModule(smiType);
    if (smiType) parentType = smiGetParentType(smiType);
    if (parentType) {
	parentModule = smiGetTypeModule(parentType);
    }

    /* first lets handle some special cases */

    if (smiModule != thisModule) {
	    for (i = 0; special[i]; i += 3) {
		    if ((strcmp(special[i], smiModule->name) == 0)
			&& (strcmp(special[i+1], smiType->name) == 0)) {
			    fprint(f, " %s", special[i+2]);
			    fprint(f, ";\n");
			    return;
		    }
	    }
    }

    /* this type is a base type - easy */

    if (parentType == NULL) {
	fprint(f, " %s", getStringBasetype(smiType->basetype));
	fprint(f, ";\n");
	return;
    }

    /* this type is an imported type - easy */

    if (smiModule != thisModule) {
	fprint(f, " %s:%s",
	       getModulePrefix(smiModule->name),
	       smiType->name);
	fprint(f, ";\n");
	return;
    }

    /* this is a locally defined named type - easy as well */

    if (smiModule == thisModule && smiType->name) {
	fprint(f, " %s:%s",
	       getModulePrefix(thisModule->name),
	       smiType->name);
	fprintSubtype(f, indent, smiType);
	return;
    }

    /* this is a locally defined unnamed type - easy as well? */

    if (smiModule == thisModule && smiType->name == NULL) {
        if (smiGetParentType(parentType) == NULL) {
	    fprint(f, " %s", getStringBasetype(smiType->basetype));
	} else {
   	    fprint(f, " %s:%s",
		   getModulePrefix(parentModule->name),
		   parentType->name);
	}
	fprintSubtype(f, indent, smiType);
	return;
    }

    /* uops - still here ? */
}


static void
fprintStatus(FILE *f, int indent, SmiStatus status)
{
    if ((status != SMI_STATUS_CURRENT) &&
	(status != SMI_STATUS_UNKNOWN) &&
	(status != SMI_STATUS_MANDATORY) &&
	(status != SMI_STATUS_OPTIONAL)) {
	fprintSegment(f, indent, "status", 0);
	fprint(f, " %s;\n", getStringStatus(status));
    }
}


static void
fprintUnits(FILE *f, int indent, const char *units)
{
    if (units) {
	fprintSegment(f, indent, "units", 0);
	fprint(f, " \"%s\";\n", units);
    }
}


static void
fprintFormat(FILE *f, int indent, const char *format)
{
    if (sflag && format) {
	fprintSegment(f, 2 * INDENT, "smi:format", 0);
	fprint(f, " \"%s\";\n", format);
    }
}


static void
fprintObjectIdentifier(FILE *f, int indent, SmiSubid *oid, int oidlen)
{
    int i;

    if (sflag && oid && oidlen) {
	fprintSegment(f, indent, "smi:oid", 0);
	fprint(f, " \"");
	for (i=0; i < oidlen; i++) {
	    fprint(f, "%s%d", i ? "." : "", oid[i]);
	}
	fprint(f, "\";\n");
    }
}


static void
fprintDescription(FILE *f, int indent, const char *description)
{
    fprintSegment(f, indent, "description", INDENTVALUE);
    fprint(f, "\n");
    fprintMultilineString(f, indent, description);
    fprint(f, ";\n");
}


static void
fprintReference(FILE *f, int indent, const char *reference)
{
    if (reference) {
	fprintSegment(f, indent, "reference", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, indent, reference);
	fprint(f, ";\n");
    }
}


static void
fprintConfig(FILE *f, int indent, SmiAccess access)
{
    if (access == SMI_ACCESS_READ_WRITE) {
	fprintSegment(f, indent, "config true;\n", 0);
    } else {
	fprintSegment(f, indent, "config false;\n", 0);
    }
}


static void
fprintTypedefs(FILE *f, SmiModule *smiModule)
{
    int		 i;
    SmiType	 *smiType;
    
    for (i = 0, smiType = smiGetFirstType(smiModule);
	 smiType; smiType = smiGetNextType(smiType)) {

	if (!i && !silent) {
	    fprintSegment(f, INDENT, "/*** TYPE DEFINITIONS ***/\n\n", 0);
	}
	fprintSegment(f, INDENT, "", 0);
	fprint(f, "typedef %s {\n", smiType->name);

	fprintType(f, 2 * INDENT, smiGetTypeModule(smiType), smiType);

	if (smiType->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    fprintSegment(f, 2 * INDENT, "default", INDENTVALUE);
	    fprint(f, "%s", getValueString(&smiType->value, smiType));
	    fprint(f, ";\n");
	}
	
	fprintUnits(f, 2 * INDENT, smiType->units);
	fprintStatus(f, 2 * INDENT, smiType->status);
	fprintDescription(f, 2 * INDENT, smiType->description);
	fprintReference(f, 2 * INDENT, smiType->reference);
	fprintFormat(f, 2 * INDENT, smiType->format);

	fprintSegment(f, INDENT, "}\n\n", 0);
	i++;
    }
}


static void
fprintPath(FILE *f, SmiNode *smiNode)
{
     SmiNode *entryNode = NULL;
     SmiNode *tableNode = NULL;
     SmiNode *contNode = NULL;
     SmiModule *smiModule = NULL;

     switch (smiNode->nodekind) {
     case SMI_NODEKIND_SCALAR:
	     contNode = smiGetParentNode(smiNode);
	     break;
     case SMI_NODEKIND_COLUMN:
	     entryNode = smiGetParentNode(smiNode);
	     tableNode = smiGetParentNode(entryNode);
	     contNode = smiGetParentNode(tableNode);
	     break;
     case SMI_NODEKIND_ROW:
	     entryNode = smiNode;
	     tableNode = smiGetParentNode(entryNode);
	     contNode = smiGetParentNode(tableNode);
	     break;
     case SMI_NODEKIND_TABLE:
	     contNode = smiGetParentNode(tableNode);
	     break;
     default:
	     break;
     }

     smiModule = smiGetNodeModule(contNode);
     if (smiModule) {
         fprint(f, "/%s:%s", getModulePrefix(smiModule->name), contNode->name);
     }
     if (contNode == smiNode) return;

     smiModule = smiGetNodeModule(entryNode);
     if (smiModule) {
         fprint(f, "/%s:%s", getModulePrefix(smiModule->name), entryNode->name);
     }
     if (entryNode == smiNode) return;

     smiModule = smiGetNodeModule(smiNode);
     if (smiModule) {
         fprint(f, "/%s:%s", getModulePrefix(smiModule->name), smiNode->name);
     }
}


static void
fprintLeaf(FILE *f, int indent, SmiNode *smiNode)
{
    SmiType *smiType;
    
    smiType = smiGetNodeType(smiNode);

    fprintSegment(f, indent, "leaf ", 0);
    fprint(f, "%s {\n", smiNode->name);
    fprintType(f, indent + INDENT, smiGetNodeModule(smiNode), smiType);
    fprintUnits(f, indent + INDENT, smiNode->units);
    fprintStatus(f, indent + INDENT, smiNode->status);
    fprintDescription(f, indent + INDENT, smiNode->description);
    fprintReference(f, indent + INDENT, smiNode->reference);
    fprintConfig(f, indent + INDENT, smiNode->access);
    fprintFormat(f, indent + INDENT, smiNode->format);
    fprintObjectIdentifier(f, indent + INDENT, smiNode->oid, smiNode->oidlen);
    fprintSegment(f, indent, "}\n", 0);
}


static void
fprintKeyrefLeaf(FILE *f, int indent, SmiNode *smiNode)
{
    SmiNode *entryNode;

    entryNode = smiGetParentNode(smiNode);
    fprintSegment(f, indent, "leaf ", 0);
    fprint(f, "%s {\n", smiNode->name);
    fprintSegment(f, indent + INDENT, "type keyref {\n", 0);
    fprintSegment(f, indent + 2 * INDENT, "path \"", 0);
    fprintPath(f, smiNode);
    fprint(f, "\";\n");
    fprintSegment(f, indent + INDENT, "}\n", 0);
    fprintStatus(f, indent + INDENT, smiNode->status);
    fprintDescription(f, indent + INDENT,
		      "Automagically generated keyref leaf.");
    fprintConfig(f, indent + INDENT,
	 entryNode->create ? SMI_ACCESS_READ_WRITE : SMI_ACCESS_READ_ONLY);
    fprintSegment(f, indent, "}\n\n", 0);
}


static void
fprintKey(FILE *f, int indent, SmiNode *smiNode)
{
    SmiElement *smiElement;
    int j;

    fprintSegment(f, indent, "key \"", 0);

    for (j = 0, smiElement = smiGetFirstElement(smiNode); smiElement;
	 j++, smiElement = smiGetNextElement(smiElement)) {
        if (j) {
            fprint(f, ", ");
	}
	fprintWrapped(f, indent + 5,
		      smiGetElementNode(smiElement)->name);
    }
    fprint(f, "\";\n");
}


static void
fprintLeafs(FILE *f, int indent, SmiNode *smiNode)
{
    SmiNode *childNode;
    int c;
    
    for (c = 0, childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_COLUMN) {
	    if (c) {
		fprint(f, "\n");
	    }
	    fprintLeaf(f, indent, childNode);
	    c++;
	}
    }
}


static void
fprintList(FILE *f, int indent, SmiNode *smiNode)
{
    SmiNode *entryNode;
    SmiNode *childNode;
    SmiNode *parentNode;
    SmiElement *smiElement;

    entryNode = smiGetFirstChildNode(smiNode);

    fprint(f, "\n");
    fprintSegment(f, indent, "/* XXX table comments here XXX */\n", 0);
    fprint(f, "\n");

    fprintSegment(f, indent, "list", 0);
    fprint(f, " %s {\n\n", entryNode->name);

    fprintKey(f, indent + INDENT, entryNode);
    fprintStatus(f, indent + INDENT, entryNode->status);
    fprintDescription(f, indent + INDENT, entryNode->description);
    fprintReference(f, indent + INDENT, entryNode->reference);
    fprintObjectIdentifier(f, indent + INDENT,
			   entryNode->oid, entryNode->oidlen);
    fprint(f, "\n");

    for (smiElement = smiGetFirstElement(entryNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	childNode = smiGetElementNode(smiElement);
	parentNode = smiGetParentNode(childNode);
        if (childNode->nodekind == SMI_NODEKIND_COLUMN
            && parentNode != entryNode) {
	    fprintKeyrefLeaf(f, indent + INDENT, childNode);
	}
    }

    fprintLeafs(f, indent + INDENT, entryNode);

    fprintObjectIdentifier(f, indent + INDENT,
			   entryNode->oid, entryNode->oidlen);
    fprintSegment(f, indent, "}\n", 0);
}


static void
fprintAugment(FILE *f, int indent, SmiNode *smiNode)
{
    SmiNode *entryNode = NULL;
    SmiNode *baseEntryNode = NULL;

    entryNode = smiGetFirstChildNode(smiNode);
    if (entryNode) {
        baseEntryNode = smiGetRelatedNode(entryNode);
    }
    if (! entryNode || ! baseEntryNode) {
        return;
    }

    fprint(f, "\n");
    fprintSegment(f, indent, "/* XXX table comments here XXX */\n", 0);
    fprint(f, "\n");

    fprintSegment(f, indent, "augment", 0);
    fprint(f, " \"");
    fprintPath(f, baseEntryNode);
    fprint(f, "\" {\n\n");

    fprintLeafs(f, indent + INDENT, entryNode);
    fprintObjectIdentifier(f, indent + INDENT,
			   smiNode->oid, smiNode->oidlen);
    fprintSegment(f, indent, "}\n", 0);
}


static void
fprintContainer(FILE *f, int indent, SmiNode *smiNode)
{
    SmiNode *childNode;
    int c;

    fprintSegment(f, indent, "container", 0);
    fprint(f, " %s {\n\n", smiNode->name);

    for (c = 0, childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (c) {
	    fprint(f, "\n");
	}
	if (childNode->nodekind == SMI_NODEKIND_SCALAR) {
	    fprintLeaf(f, indent + INDENT, childNode);
	    c++;
	}
	if (childNode->nodekind == SMI_NODEKIND_TABLE) {
	    SmiNode *entryNode = smiGetFirstChildNode(childNode);
	    if (entryNode) {
	        switch (entryNode->indexkind) {
		case SMI_INDEX_INDEX:
		case SMI_INDEX_REORDER:
		case SMI_INDEX_SPARSE:
		case SMI_INDEX_EXPAND:
			fprintList(f, indent + INDENT, childNode);
			c++;
			break;
		case SMI_INDEX_AUGMENT:
			fprintAugment(f, indent + INDENT, childNode);
			c++;
			break;
		default:
			break;
		}
	    }
	} 
    }

    fprintObjectIdentifier(f, indent + INDENT,
			   smiNode->oid, smiNode->oidlen);
    
    fprintSegment(f, indent, "}\n\n", 0);
}


static void
fprintContainers(FILE *f, SmiModule *smiModule)
{
    SmiNode *smiNode;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    fprintContainer(f, INDENT, smiNode);
	}
    }
}


static void
fprintNamespace(FILE *f, int indent, SmiModule *smiModule)
{
     if (! silent) {
	  fprintSegment(f, indent, "/*** NAMESPACE / PREFIX DEFINITION ***/\n\n", 0);
     }

     fprintSegment(f, indent, "namespace ", 0);
     fprint(f, "\"%s\";\n", getModuleUrn(smiModule->name));
     fprintSegment(f, indent, "prefix ", 0);
     fprint(f, "\"%s\";\n\n", getModulePrefix(smiModule->name));
}


static void
fprintLinkage(FILE *f, int indent, SmiModule *smiModule)
{
     if (! silent) {
	  fprintSegment(f, indent, "/*** LINKAGE (IMPORTS / INCLUDES) ***/\n\n", 0);
     }
     fprintImports(f, smiModule);
}


static void
fprintMeta(FILE *f, int indent, SmiModule *smiModule)
{
     if (! silent) {
	  fprintSegment(f, indent, "/*** META INFORMATION ***/\n\n", 0);
     }
     fprintSegment(f, indent, "organization", INDENTVALUE);
     fprint(f, "\n");
     fprintMultilineString(f, indent, smiModule->organization);
     fprint(f, ";\n\n");
     fprintSegment(f, indent, "contact", INDENTVALUE);
     fprint(f, "\n");
     fprintMultilineString(f, indent, smiModule->contactinfo);
     fprint(f, ";\n\n");
     fprintSegment(f, indent, "description", INDENTVALUE);
     fprint(f, "\n");
     fprintMultilineString(f, indent, smiModule->description);
     fprint(f, ";\n\n");
     if (smiModule->reference) {
	  fprintSegment(f, indent, "reference", INDENTVALUE);
	  fprint(f, "\n");
	  fprintMultilineString(f, indent, smiModule->reference);
	  fprint(f, ";\n\n");
     }
}


static void
fprintNotification(FILE *f, SmiNode *smiNode)
{
    SmiElement *smiElement;
    int c;
    
    fprintSegment(f, INDENT, "notification", 0);
    fprint(f, " %s {\n", smiNode->name);
    fprintStatus(f, INDENT + INDENT, smiNode->status);
    fprintDescription(f, INDENT + INDENT, smiNode->description);
    fprintReference(f, INDENT + INDENT, smiNode->reference);
    fprintObjectIdentifier(f, INDENT + INDENT, smiNode->oid, smiNode->oidlen);
    fprint(f, "\n");

    for (c = 0, smiElement = smiGetFirstElement(smiNode); smiElement;
	 c++, smiElement = smiGetNextElement(smiElement)) {
	fprintKeyrefLeaf(f, INDENT + INDENT, smiGetElementNode(smiElement));
    }

    fprintSegment(f, INDENT, "}\n", 0);
}


static void
fprintNotifications(FILE *f, SmiModule *smiModule)
{
    SmiNode *smiNode;
    int c;

    for (c = 0, smiNode = smiGetFirstNode(smiModule,
					  SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 c++, smiNode = smiGetNextNode(smiNode,
				       SMI_NODEKIND_NOTIFICATION)) {
	if (c) {
	    fprint(f, "\n");
	}
	fprintNotification(f, smiNode);
    }
}


static void
dumpYang(int modc, SmiModule **modv, int flags, char *output)
{
    SmiModule   *smiModule;
    int		i;
    FILE	*f = stdout;

    silent = (flags & SMIDUMP_FLAG_SILENT);

    fprint_indent_texts = INDENT;
    
    if (output) {
	f = fopen(output, "w");
	if (!f) {
	    fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	    perror(NULL);
	    exit(1);
	}
    }

    for (i = 0; i < modc; i++) {

	smiModule = modv[i];

        createImportList(smiModule);

	fprint(f, "/*\n");
	fprint(f, " * This module has been generated by smidump "
	       SMI_VERSION_STRING ":\n");
	fprint(f, " *\n");
	fprint(f, " *      smidump -f yang");
	if (sflag) {
	    fprint(f, " --yang-smi");
	}
	fprint(f, " %s\n", smiModule->name);
	fprint(f, " *\n");
	fprint(f, " * Do not edit. Edit the source file instead!\n");
	fprint(f, " */\n\n");
	fprint(f, "module %s {\n", smiModule->name);
	fprint(f, "\n");

	fprintMeta(f, INDENT, smiModule);
	fprintRevisions(f, INDENT, smiModule);
	fprintNamespace(f, INDENT, smiModule);
	fprintLinkage(f, INDENT, smiModule);

	fprintTypedefs(f, modv[i]);
	fprintContainers(f, modv[i]);

	fprintNotifications(f, modv[i]);

    	fprint(f, "} /* end of module %s */\n", smiModule->name);

        freeImportList();
    }

    if (fflush(f) || ferror(f)) {
	perror("smidump: write error");
	exit(1);
    }

    if (output) {
	fclose(f);
    }
}



void initYang()
{
    static SmidumpDriverOption opt[] = {
	{ "smi", OPT_FLAG, &sflag, 0,
	  "generate smi extensions" },
	{ "indent", OPT_INT, &INDENT, 0,
	  "indentation (default 2)" },
        { 0, OPT_END, 0, 0 }
    };

    static SmidumpDriver driver = {
	"yang",
	dumpYang,
	0,
	SMIDUMP_DRIVER_CANT_UNITE,
	"YANG format",
	opt,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}
