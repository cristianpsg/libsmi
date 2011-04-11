/*
 * smi2yang.c --
 *
 *      Conversion of SMIv2 modules to YANG modules
 *
 * Copyright (c) 2007-2011 J. Schoenwaelder, Jacobs University Bremen
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Author: Juergen Schoenwaelder
 * @(#) $Id: smi.c 20582 2009-09-03 10:34:06Z schoenw $
 */

/*
 * TODO:
 * - handle opaque in a reasonable way (test case AGGREGATE-MIB)
 * - fix addYangNode to pick up a suitable line number?
 * - should createModuleInfo set the correct path?
 * - reconsider the format to pattern translation
 */

/*
 * TODO: (draft-ietf-netmod-smi-yang-00.txt)
 * - create container representing augmenting tables -- oops illegal!
 * - create container for all nodes (optional flag?)
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "smi2yang.h"
#include "yang-data.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#define  URNBASE        "urn:ietf:params:xml:ns:yang:smiv2:"

static const char *convertType[] = {

    /*
     * Translation of the SMIng built-in types to the YANG
     * equivalents.
     */
    
    "",		  "Integer32",   NULL,	     "int32",
    "",		  "Integer64",   NULL,	     "int64",
    "",		  "Unsigned32",  NULL,	     "uint32",
    "",		  "Unsigned64",  NULL,	     "uint64",
    "",		  "OctetString", NULL,	     "binary",
    "",		  "Enumeration", NULL,	     "enumeration",
    "",		  "Bits",	 NULL,	     "bits",
    "",		  "ObjectIdentifier", "ietf-yang-types", "object-identifier",

    /*
     * We want to do these translations as well in order to retire the
     * SNMPv2-SMI module which is not really an SMIv2 module but part
     * of the definition of SNMPv2-SMI itself.
     */

    "SNMPv2-SMI", "Integer32",  NULL,        "int32",
    "SNMPv2-SMI", "Integer64",  NULL,        "int64",
    "SNMPv2-SMI", "Unsigned32", NULL,        "uint32",
    "SNMPv2-SMI", "Opaque",    NULL,	     "binary",
    "SNMPv2-SMI", "Counter32", "ietf-yang-types", "counter32",
    "SNMPv2-SMI", "Counter64", "ietf-yang-types", "counter64",
    "SNMPv2-SMI", "Gauge32",   "ietf-yang-types", "gauge32",
    "SNMPv2-SMI", "TimeTicks", "ietf-yang-types", "timeticks",
    "SNMPv2-SMI", "IpAddress", "ietf-inet-types", "ipv4-address",

    /*
     * And we like to do the same for RFC1155-SMI definitions...
     */

    "RFC1155-SMI", "Opaque",    NULL,	     "binary",
    "RFC1155-SMI", "Counter",   "ietf-yang-types", "counter32",
    "RFC1155-SMI", "Gauge",     "ietf-yang-types", "gauge32",
    "RFC1155-SMI", "TimeTicks", "ietf-yang-types", "timeticks",
    "RFC1155-SMI", "IpAddress", "ietf-inet-types", "ipv4-address",
    
    /*
     * We also translate frequently used SNMPv2-TCs that have a YANG
     * equivalent. Note that DateAndTime is slightly different from
     * the ISO profile used by date-and-time.
     */

    "SNMPv2-TC",  "PhysAddress", "ietf-yang-types", "phys-address",
    "SNMPv2-TC",  "MacAddress",  "ietf-yang-types", "mac-address",
    "SNMPv2-TC",  "TimeStamp",   "ietf-yang-types", "timestamp",

    NULL, NULL, NULL, NULL
};


static const char *convertImport[] = {

    /*
     * Things that are not types and removed from imports...
     */

    "SNMPv2-SMI",  "MODULE-IDENTITY",    NULL, NULL,
    "SNMPv2-SMI",  "OBJECT-IDENTITY",    NULL, NULL,
    "SNMPv2-SMI",  "OBJECT-TYPE",        NULL, NULL,
    "SNMPv2-SMI",  "NOTIFICATION-TYPE",  NULL, NULL,
    "SNMPv2-SMI",  "mib-2",              NULL, NULL,
    "SNMPv2-TC",   "TEXTUAL-CONVENTION", NULL, NULL,
    "SNMPv2-CONF", "OBJECT-GROUP",       NULL, NULL,
    "SNMPv2-CONF", "NOTIFICATION-GROUP", NULL, NULL,
    "SNMPv2-CONF", "MODULE-COMPLIANCE",  NULL, NULL,
    "SNMPv2-CONF", "AGENT-CAPABILITIES", NULL, NULL,
    "SNMPv2-MIB",  "snmpTraps",	         NULL, NULL,

    NULL, NULL, NULL, NULL
};

/*
 * SMIv2 modules we never like to import from...
 */

static const char *ignoreImports[] = {
    "RFC1155-SMI", "SNMPv2-SMI", "SNMPv2-CONF", NULL
};


/*
 * Structure used to build a list of imported types.
 */

typedef struct Import {
    char          *module;
    char	  *prefix;
    struct Import *nextPtr;
} Import;

static Import *importList = NULL;

/*
 * The current translation flags. May be different between translation
 * calls.
 */

static int smi2yangFlags;

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

#if 0
static int
isGroup(SmiNode *smiNode)
{
    SmiNode *childNode;
    
    for (childNode = smiGetFirstChildNode(smiNode);
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
#endif

static int
smi2yangPrune(SmiNode *smiNode)
{
    SmiNode *childNode;

    switch (smiNode->nodekind) {
    case SMI_NODEKIND_CAPABILITIES:
    case SMI_NODEKIND_COMPLIANCE:
    case SMI_NODEKIND_GROUP:
	return 1;
    case SMI_NODEKIND_SCALAR:
    case SMI_NODEKIND_COLUMN:
	return 0;
    default:
	for (childNode = smiGetFirstChildNode(smiNode);
	     childNode;
	     childNode = smiGetNextChildNode(childNode)) {
	    if (! smi2yangPrune(childNode)) return 0;
	}
	return 1;
    }
}


static int
isAccessibleLeaf(SmiNode *smiNode)
{
    return (smiNode->access == SMI_ACCESS_NOT_ACCESSIBLE
	    || smiNode->access == SMI_ACCESS_READ_ONLY
	    || smiNode->access == SMI_ACCESS_READ_WRITE);
}


static int
hasObjects(SmiNode *smiNode)
{
    SmiNode *childNode;

    switch (smiNode->nodekind) {
    case SMI_NODEKIND_SCALAR:
    case SMI_NODEKIND_COLUMN:
	return isAccessibleLeaf(smiNode);
    case SMI_NODEKIND_TABLE:
    case SMI_NODEKIND_ROW:
	return 1;
    default:
	break;
    }

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (hasObjects(childNode)) {
	    return 1;
	}
    }

    return 0;
}


#if 0
static int
isIndex(SmiNode *groupNode, SmiNode *smiNode)
{
    SmiElement *smiElement;
    int cnt = 0;
    
    /*
     * We return an indication whether smiNode is part of an index. In
     * fact, we return number of times smiNode is part of an index
     * since we sometimes have to disambiguate names...
     */

    for (smiElement = smiGetFirstElement(groupNode);
	 smiElement; smiElement = smiGetNextElement(smiElement)) {
	if (smiNode == smiGetElementNode(smiElement)) {
	    cnt++;
	}
    }

    return cnt;
}
#endif


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
    int i, d;

    char *specials[] = {
	"ietf-yang-smiv2", "smiv2",
	"ietf-yang-types", "yang",
	"ietf-inet-types", "inet",
	NULL, NULL
    };

    for (i = 0; specials[i]; i +=2) {
	if (strcmp(moduleName, specials[i]) == 0) {
            if (isPrefixUnique(specials[i+1])) {
                return smiStrdup(specials[i+1]);
	    }
	}
    }

    prefix = smiStrdup(moduleName);
    for (i = 0; prefix[i]; i++) {
        prefix[i] = tolower(prefix[i]);
    }

    for (i = 0, d = 0; prefix[i]; i++) {
        if (prefix[i] == '-') {
	    d++;
	    if (d > 1) {
		prefix[i] = 0;
		if (isPrefixUnique(prefix)) {
		    return prefix;
		}
		prefix[i] = '-';
	    }
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
 
    if (prefix) smiFree(prefix);
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

    newImport = smiMalloc(sizeof(Import));
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
    SmiIdentifier impModule, impName;
    SmiType	*smiType;
    SmiNode	*smiNode;
    int i;

    for (smiImport = smiGetFirstImport(smiModule); smiImport;
	 smiImport = smiGetNextImport(smiImport)) {

	impModule = smiImport->module;
	impName = smiImport->name;

	for (i = 0; convertType[i]; i += 4) {
	    if (strcmp(smiImport->module, convertType[i]) == 0
		&& strcmp(smiImport->name, convertType[i+1]) == 0) {
		impModule = (SmiIdentifier) convertType[i+2];
		impName = (SmiIdentifier) convertType[i+3];
		break;
	    }
	}

	if (! impModule || ! impName) continue;

	for (i = 0; convertImport[i]; i += 4) {
	    if (strcmp(smiImport->module, convertImport[i]) == 0
		&& strcmp(smiImport->name, convertImport[i+1]) == 0) {
		impModule = (SmiIdentifier) convertImport[i+2];
		impName = (SmiIdentifier) convertImport[i+3];
		break;
	    }
	}

	if (! impModule || ! impName) continue;
	addImport(impModule, impName);
    }

    /*
     * Add import for the smi:oid extension and friends.
     *
     * XXX: We also need this import if we have an OBJECT-IDENTITY!
     */
    if (smi2yangFlags & SMI_TO_YANG_FLAG_SMI_EXTENSIONS) {
	addImport("ietf-yang-smiv2", "smiv2");
    }

    /*
     * Add import for yang-types that were originally ASN.1
     * builtins...
     */

    for (smiType = smiGetFirstType(smiModule);
	 smiType; smiType = smiGetNextType(smiType)) {
	SmiType *parentType = smiGetParentType(smiType);
	if (parentType && strcmp(parentType->name, "ObjectIdentifier") == 0) {
	    addImport("ietf-yang-types", "object-identifier");
	}
    }

    for (smiNode = smiGetFirstNode(smiModule,
				   SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode,
				  SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN)) {
	smiType = smiGetNodeType(smiNode);
	if (! smiType->name) {
	    smiType = smiGetParentType(smiType);
	}
	if (smiType && strcmp(smiType->name, "ObjectIdentifier") == 0) {
	    addImport("ietf-yang-types", "object-identifier");
	}
    }
}



static void
freeImportList(void)
{
    Import *import, *freeme;

    for (import = importList; import; ) {
        smiFree(import->prefix);
	freeme = import;
	import = import->nextPtr;
	smiFree(freeme);
    }
    importList = NULL;
}


static void
smi2yangNamespace(SmiModule *smiModule, _YangNode *yangModulePtr,
		  _YangModuleInfo *yangModuleInfoPtr)
{
    char *s;
    const char *p;
    _YangNode *node = NULL;

    smiAsprintf(&s, "%s%s", URNBASE, smiModule->name);
    node = addYangNode(s, YANG_DECL_NAMESPACE, yangModulePtr);
    smiFree(s);
    if (yangModuleInfoPtr) {
	yangModuleInfoPtr->namespace = node->export.value;
    }

    p = getModulePrefix(smiModule->name);
    (void) addYangNode(p, YANG_DECL_PREFIX, yangModulePtr);
    if (yangModuleInfoPtr) {
	yangModuleInfoPtr->prefix = smiStrdup(p);
    }
}


static void
smi2yangImports(SmiModule *smiModule, _YangNode *yangModulePtr)
{
    int i;
    Import *import;
    _YangNode *node;
    
    for (import = importList; import; import = import->nextPtr) {
	for (i = 0; ignoreImports[i]; i++) {
	    if (strcmp(ignoreImports[i], import->module) == 0) {
		break;
	    }
	}
	if (ignoreImports[i] == NULL) {
	    node = addYangNode(import->module, YANG_DECL_IMPORT, yangModulePtr);
	    (void) addYangNode(import->prefix, YANG_DECL_PREFIX, node);
	}
    }
}


static char*
smi2yangPath(SmiNode *smiNode)
{
     SmiNode *entryNode = NULL;
     SmiNode *tableNode = NULL;
     SmiNode *contNode = NULL;
     SmiModule *smiModule = NULL;
     char *s = NULL, *o = NULL;

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
         smiAsprintf(&s, "/%s:%s",
		     getModulePrefix(smiModule->name), contNode->name);
     }
     if (contNode == smiNode) {
	 return s;
     }

     if (entryNode) {
	 smiModule = smiGetNodeModule(entryNode);
	 if (smiModule) {
	     o = s;
	     smiAsprintf(&s, "%s/%s:%s", o ? o : "",
			 getModulePrefix(smiModule->name), entryNode->name);
	     smiFree(o);
	 }
	 if (entryNode == smiNode) {
	     return s;
	 }
     }

     smiModule = smiGetNodeModule(smiNode);
     if (smiModule) {
	 o = s;
         smiAsprintf(&s, "%s/%s:%s", o ? o : "",
		     getModulePrefix(smiModule->name), smiNode->name);
     }
     return s;
}


static void
smi2yangMeta(SmiModule *smiModule, _YangNode *yangModulePtr,
	     _YangModuleInfo *yangModuleInfoPtr)
{
    _YangNode *node;

    if (smiModule->organization) {
	node = addYangNode(smiModule->organization,
			   YANG_DECL_ORGANIZATION, yangModulePtr);
	yangModuleInfoPtr->organization = node->export.value;
    }

    if (smiModule->contactinfo) {
	node = addYangNode(smiModule->contactinfo,
			   YANG_DECL_CONTACT, yangModulePtr);
	yangModuleInfoPtr->contact = node->export.value;
    }

    if (smiModule->description) {
	node = addYangNode(smiModule->description,
			   YANG_DECL_DESCRIPTION, yangModulePtr);
	setDescription(yangModulePtr, smiModule->description);
    }
}


static void
smi2yangRevisions(SmiModule *smiModule, _YangNode *yangModulePtr)
{
    SmiRevision *smiRevision;
    _YangNode *node = NULL;
    
    for (smiRevision = smiGetFirstRevision(smiModule);
	smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	node = addYangNode(getStringDate(smiRevision->date),
			   YANG_DECL_REVISION, yangModulePtr);
	if (smiRevision->description) {
	    addYangNode(smiRevision->description,
			YANG_DECL_DESCRIPTION, node);
	}
    }
}


static void
smi2yangUnits(_YangNode *node, char *units)
{
    if (units) {
	(void) addYangNode(units, YANG_DECL_UNITS, node);
    }
}


static void
smi2yangDefault(_YangNode *node, SmiValue *smiValue, SmiType *smiType)
{
    char *s;

    if (! smi2yangFlags & SMI_TO_YANG_FLAG_SMI_EXTENSIONS) {
	return;
    }

    if (smiValue->basetype != SMI_BASETYPE_UNKNOWN) {
	s = smiValueAsString(smiValue, smiType, SMI_LANGUAGE_YANG);
	if (s) {
	    (void) addYangNode(s, YANG_DECL_SMI_DEFAULT, node);
	}
    }
}


static void
smi2yangStatus(_YangNode *node, SmiStatus status)
{
    YangStatus s = YANG_STATUS_DEFAULT_CURRENT;

    switch (status) {
    case SMI_STATUS_UNKNOWN:
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_MANDATORY:
    case SMI_STATUS_OPTIONAL:
	return;
    case SMI_STATUS_DEPRECATED:
	s = YANG_STATUS_DEPRECATED;
	break;
    case SMI_STATUS_OBSOLETE:
	s = YANG_STATUS_OBSOLETE;
	break;
    }
    
    (void) addYangNode(yangStatusAsString(s), YANG_DECL_STATUS, node);
    setStatus(node, s);
}


static void
smi2yangDescription(_YangNode *node, char *description)
{
    if (description) {
	(void) addYangNode(description, YANG_DECL_DESCRIPTION, node);
	setDescription(node, description);
    }
}


static void
smi2yangReference(_YangNode *node, char *reference)
{
    if (reference) {
	(void) addYangNode(reference, YANG_DECL_REFERENCE, node);
	setDescription(node, reference);
    }
}


static void
smi2yangAccess(_YangNode *node, SmiAccess access)
{
    char *s;

    if (! smi2yangFlags & SMI_TO_YANG_FLAG_SMI_EXTENSIONS) {
	return;
    }

    s = smiAccessAsString(access);
    if (s) {
	(void) addYangNode(s, YANG_DECL_SMI_MAX_ACCESS, node);
    }
}


static void
smi2yangOID(_YangNode *node, SmiSubid *oid, unsigned int oidlen)
{
    char *s;
    
    if (! smi2yangFlags & SMI_TO_YANG_FLAG_SMI_EXTENSIONS) {
	return;
    }

    s = smiRenderOID(oidlen, oid, 0);
    (void) addYangNode(s, YANG_DECL_SMI_OID, node);
    smiFree(s);
}


static void
smi2yangFormat(_YangNode *node, const char *format)
{
    if (! smi2yangFlags & SMI_TO_YANG_FLAG_SMI_EXTENSIONS) {
	return;
    }

    if (format) {
	(void) addYangNode(format, YANG_DECL_SMI_DISPLAY_HINT, node);
    }
}


static void
smi2yangSubtype(_YangNode *node, SmiType *smiType)
{
    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	SmiNamedNumber *nn;
	_YangNode *nam_node = NULL;
	YangDecl nam_kind, val_kind;
	nam_kind = (smiType->basetype == SMI_BASETYPE_BITS)
	    ? YANG_DECL_BIT : YANG_DECL_ENUM;
	val_kind = (smiType->basetype == SMI_BASETYPE_BITS)
	    ? YANG_DECL_POSITION : YANG_DECL_VALUE;
	for (nn = smiGetFirstNamedNumber(smiType);
	     nn; nn = smiGetNextNamedNumber(nn)) {
	    nam_node = addYangNode(nn->name, nam_kind, node);
	    if (nam_node) {
		addYangNode(smiValueAsString(&nn->value, smiType,
					     SMI_LANGUAGE_YANG),
			    val_kind, nam_node);
	    }
	}
    } else {
	SmiRange *range;
	char *sn = NULL, *so = NULL;
	for (range = smiGetFirstRange(smiType);
	     range; range = smiGetNextRange(range)) {
	    smiAsprintf(&sn, "%s%s%s",
			so ? so : "",
			so ? "|" : "",
			smiValueAsString(&range->minValue, smiType,
					 SMI_LANGUAGE_YANG));
	    if (sn && memcmp(&range->minValue, &range->maxValue,
			     sizeof(SmiValue))) {
		char *st;
		smiAsprintf(&st, "%s..%s",
			    sn,
			    smiValueAsString(&range->maxValue, smiType,
					     SMI_LANGUAGE_YANG));
		free(sn);
		sn = st;
	    }
	    if (so) free(so);
	    so = sn;
	    sn = NULL;
	}
	if (so) {
	    if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		(void) addYangNode(so, YANG_DECL_LENGTH, node);
	    } else {
		(void) addYangNode(so, YANG_DECL_RANGE, node);
	    }
	    free(so);
	}
    }

#if 0
    if (smiType->format
	&& smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
	char *pattern;
	pattern = smiFormatToPattern(smiType->format,
				     smiGetFirstRange(smiType));
	if (pattern) {
	    (void) addYangNode(pattern, YANG_DECL_PATTERN, node);
	}
    }
#endif
}


static _YangNode*
smi2yangType(_YangNode *node, SmiType *smiType, int hasFormat)
{
    const char *typeModule = NULL, *typeName = NULL;
    SmiModule *smiModule;
    int i;
    _YangNode *typeNode = NULL;

    if (! smiType) return NULL;

    smiModule = smiGetTypeModule(smiType);

    if (smiType && ! smiType->name) {
	return smi2yangType(node, smiGetParentType(smiType), hasFormat);
    }

    for (i = 0; convertType[i]; i += 4) {
	if (strcmp(smiModule->name, convertType[i]) == 0
	    && strcmp(smiType->name, convertType[i+1]) == 0) {
	    typeModule = convertType[i+2];
	    typeName = convertType[i+3];
	    break;
	}
    }

    if (! typeName) {
	typeModule = smiModule->name;
	typeName = smiType->name;
    }

    if (typeModule) {
	typeModule = getModulePrefix(typeModule);
    }

    /*
     * We handle a special case here. If we have a format string and
     * the type is binary, we turn it into string.
     */
    
    if (! typeModule && typeName && strcmp(typeName, "binary") == 0) {
	if (hasFormat) {
	    typeName = "string";
	}
    }
	
    if (typeModule && typeName) {
	char *s;
	smiAsprintf(&s, "%s:%s", typeModule, typeName);
	typeNode = addYangNode(s, YANG_DECL_TYPE, node);
	smiFree(s);
    } else {
	typeNode = addYangNode(typeName, YANG_DECL_TYPE, node);
	createTypeInfo(typeNode);
    }

    return typeNode;
}


static void
smi2yangTypedefs(SmiModule *smiModule, _YangNode *yangModulePtr)
{
    SmiType *smiType, *baseType;
    _YangNode *node, *typeNode;

    for (smiType = smiGetFirstType(smiModule);
	 smiType; smiType = smiGetNextType(smiType)) {

	baseType = smiGetParentType(smiType);

	node = addYangNode(smiType->name, YANG_DECL_TYPEDEF, yangModulePtr);
	typeNode = smi2yangType(node, baseType, smiType->format != NULL);
	if (typeNode) {
	    smi2yangSubtype(typeNode, smiType);
	}
	smi2yangUnits(node, smiType->units);
	smi2yangStatus(node, smiType->status);
	smi2yangDescription(node, smiType->description);
	smi2yangReference(node, smiType->reference);
	smi2yangFormat(node, smiType->format);
	smi2yangDefault(node, &smiType->value, smiType);
    }
}


static void
smi2yangIdentities(SmiModule *smiModule, _YangNode *yangModulePtr)
{
    SmiNode *smiNode;
    _YangNode *node;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NODE);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NODE)) {
	
	if (smiNode == smiGetModuleIdentityNode(smiModule)) {
	    continue;
	}
	
	if (smiNode->status != SMI_STATUS_UNKNOWN) {
	    node = addYangNode(smiNode->name, YANG_DECL_IDENTITY,
			       yangModulePtr);
	    (void) addYangNode("smiv2:object-identity", YANG_DECL_BASE, node);
	    smi2yangStatus(node, smiNode->status);
	    smi2yangDescription(node, smiNode->description);
	    smi2yangReference(node, smiNode->reference);
	}
    }
}


static void
smi2yangLeaf(_YangNode *container, SmiNode *smiNode)
{
    _YangNode *node, *typeNode;
    SmiType *smiType;
    
    smiType = smiGetNodeType(smiNode);

    node = addYangNode(smiNode->name, YANG_DECL_LEAF, container);

    typeNode = smi2yangType(node, smiType, smiType->format != NULL);
    if (! smiType->name) {
	smi2yangSubtype(typeNode, smiType);
    }
    smi2yangUnits(node, smiNode->units);
    smi2yangStatus(node, smiNode->status);
    smi2yangDescription(node, smiNode->description);
    smi2yangReference(node, smiNode->reference);
    smi2yangFormat(node, smiNode->format);
    smi2yangDefault(node, &smiNode->value, smiType);
    smi2yangAccess(node, smiNode->access);
    smi2yangOID(node, smiNode->oid, smiNode->oidlen);
}


static void
smi2yangLeafrefLeaf(_YangNode *node, SmiNode *smiNode)
{
    SmiNode *entryNode;
    _YangNode *leafNode, *typeNode;
    char *s;

    entryNode = smiGetParentNode(smiNode);

    leafNode = addYangNode(smiNode->name, YANG_DECL_LEAF, node);

    typeNode = addYangNode("leafref", YANG_DECL_TYPE, leafNode);
    s = smi2yangPath(smiNode);
    (void) addYangNode(s, YANG_DECL_PATH, typeNode);
    smiFree(s);
    smi2yangStatus(leafNode, smiNode->status);
    smi2yangDescription(leafNode, "[Automagically generated leafref leaf.]");
}


static void
smi2yangKey(_YangNode *node, SmiNode *smiNode)
{
    SmiElement *smiElement;
    char *s = NULL, *o = NULL;
    int c;

    for (c = 0, smiElement = smiGetFirstElement(smiNode); smiElement;
	 smiElement = smiGetNextElement(smiElement), c++) {
	o = s;
	smiAsprintf(&s, "%s%s%s", o ? o : "", c ? " " : "",
		    smiGetElementNode(smiElement)->name);
	if (o) smiFree(o);
    }

    if (s) {
	(void) addYangNode(s, YANG_DECL_KEY, node);
	smiFree(s);
    }
}


static void
smi2yangLeafs(_YangNode *node, SmiNode *smiNode)
{
    SmiNode *childNode;
    
    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_COLUMN
	    && isAccessibleLeaf(childNode)) {
	    smi2yangLeaf(node, childNode);
	}
    }
}


static void
smi2yangList(_YangNode *node, SmiNode *smiNode)
{
    SmiNode *entryNode;
    SmiNode *childNode;
    SmiNode *parentNode;
    SmiElement *smiElement;
    _YangNode *listNode, *tableNode;

    entryNode = smiGetFirstChildNode(smiNode);
    if (!entryNode) return;

    tableNode = addYangNode(smiNode->name, YANG_DECL_CONTAINER, node);

    smi2yangStatus(tableNode, smiNode->status);
    smi2yangDescription(tableNode, smiNode->description);
    smi2yangReference(tableNode, smiNode->reference);
    smi2yangOID(tableNode, smiNode->oid, smiNode->oidlen);

    listNode = addYangNode(entryNode->name, YANG_DECL_LIST, tableNode);

    smi2yangKey(listNode, entryNode);
    smi2yangStatus(listNode, entryNode->status);
    smi2yangDescription(listNode, entryNode->description);
    smi2yangReference(listNode, entryNode->reference);
    smi2yangOID(listNode, entryNode->oid, entryNode->oidlen);

    /*
     * Add leafref nodes for foreign key elements.
     */

    for (smiElement = smiGetFirstElement(entryNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	childNode = smiGetElementNode(smiElement);
	parentNode = smiGetParentNode(childNode);
        if (childNode->nodekind == SMI_NODEKIND_COLUMN
            && parentNode != entryNode) {
	    smi2yangLeafrefLeaf(listNode, childNode);
	}
    }
    
    smi2yangLeafs(listNode, entryNode);
}


static void
smi2yangAugment(_YangNode *node, SmiNode *smiNode)
{
    SmiNode *entryNode, *baseEntryNode = NULL;
    _YangNode *augmentNode, *tableNode;
    char *s;

    entryNode = smiGetFirstChildNode(smiNode);
    if (entryNode) {
        baseEntryNode = smiGetRelatedNode(entryNode);
    }
    if (! smiNode || ! entryNode || ! baseEntryNode) {
        return;
    }

    tableNode = addYangNode(smiNode->name, YANG_DECL_CONTAINER, node);

    smi2yangStatus(tableNode, smiNode->status);
    smi2yangDescription(tableNode, smiNode->description);
    smi2yangReference(tableNode, smiNode->reference);
    smi2yangOID(tableNode, smiNode->oid, smiNode->oidlen);

    s = smi2yangPath(baseEntryNode);
    augmentNode = addYangNode(s, YANG_DECL_AUGMENT, tableNode);
    smiFree(s);

    smi2yangStatus(augmentNode, entryNode->status);
    smi2yangDescription(augmentNode, entryNode->description);
    smi2yangReference(augmentNode, entryNode->reference);
    smi2yangOID(augmentNode, entryNode->oid, entryNode->oidlen);

    smi2yangLeafs(augmentNode, entryNode);
}


static void
smi2yangContainer(_YangNode *yangModulePtr, SmiNode *smiNode)
{
    _YangNode *node;
    SmiNode *childNode;

    if (smi2yangPrune(smiNode)) {
	return;
    }

    node = addYangNode(smiNode->name, YANG_DECL_CONTAINER, yangModulePtr);
    (void) addYangNode("false", YANG_DECL_CONFIG, node);
    smi2yangOID(node, smiNode->oid, smiNode->oidlen);

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {

	if (childNode->nodekind == SMI_NODEKIND_SCALAR
	    && isAccessibleLeaf(childNode)) {
	    smi2yangLeaf(node, childNode);
	    continue;
	}

	if (childNode->nodekind == SMI_NODEKIND_TABLE) {
	    SmiNode *entryNode = smiGetFirstChildNode(childNode);
	    if (entryNode) {
	        switch (entryNode->indexkind) {
		case SMI_INDEX_INDEX:
		case SMI_INDEX_REORDER:
		case SMI_INDEX_SPARSE:
		case SMI_INDEX_EXPAND:
			smi2yangList(node, childNode);
			break;
		case SMI_INDEX_AUGMENT:
   		        smi2yangAugment(node, childNode);
			break;
		default:
			break;
		}
	    }
	    continue;
	}

	if (childNode->nodekind == SMI_NODEKIND_NODE) {
	    smi2yangContainer(node, childNode);
	}
    }
}


static void
smi2yangContainers(SmiModule *smiModule, _YangNode *yangModulePtr)
{
    SmiNode *smiNode;

#if 0
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	fprintf(stderr, "** considering node %s\n", smiNode->name);
	if (smiGetNodeModule(smiNode) != smiModule) {
	    continue;
	}
	fprintf(stderr, "** considering top-level %s\n", smiNode->name);
	if (hasObjects(smiNode)) {
	    smi2yangContainer(yangModulePtr, smiNode);
	}
    }
#else
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	SmiNode *parentNode = smiGetParentNode(smiNode);
	if (smiGetNodeModule(smiNode) != smiModule) {
	    continue;
	}
	if (smiGetNodeModule(parentNode) != smiModule
	    && hasObjects(smiNode)) {
	    smi2yangContainer(yangModulePtr, smiNode);
	}
    }
#endif
}


static void
smi2yangNotificationIndex(_YangNode *node,
			  SmiNode *entryNode, SmiNode *ignoreNode)
{
    SmiElement *smiElement;
    SmiNode *childNode;
    SmiNode *parentNode;

    for (smiElement = smiGetFirstElement(entryNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	childNode = smiGetElementNode(smiElement);
	parentNode = smiGetParentNode(childNode);
	if (childNode != ignoreNode) {
	    smi2yangLeafrefLeaf(node, childNode);
	}
    }
}


static void
smi2yangNotification(_YangNode *container, SmiNode *smiNode)
{
    SmiElement *smiElement;
    SmiNode *vbNode, *entryNode;
    _YangNode *node, *conti;
    int cnt;
    char *s;

    node = addYangNode(smiNode->name, YANG_DECL_NOTIFICATION, container);
    smi2yangStatus(node, smiNode->status);
    smi2yangDescription(node, smiNode->description);
    smi2yangReference(node, smiNode->reference);
    smi2yangOID(node, smiNode->oid, smiNode->oidlen);

    for (smiElement = smiGetFirstElement(smiNode), cnt = 1; smiElement;
	 smiElement = smiGetNextElement(smiElement), cnt++) {
	vbNode = smiGetElementNode(smiElement);
	if (! vbNode) continue;

	entryNode = (vbNode->nodekind == SMI_NODEKIND_COLUMN)
	    ? smiGetParentNode(vbNode) : NULL;

	smiAsprintf(&s, "object-%d", cnt);
	conti = addYangNode(s, YANG_DECL_CONTAINER, node);
	smiFree(s);

	if (entryNode) {
	    switch (entryNode->indexkind) {
	    case SMI_INDEX_INDEX:
		smi2yangNotificationIndex(conti, entryNode, vbNode);
		break;
	    case SMI_INDEX_AUGMENT:
		smi2yangNotificationIndex(conti,
				  smiGetRelatedNode(entryNode), vbNode);
		break;
	    default:
		break;
	    }
	}

	if (isAccessibleLeaf(vbNode)) {
	    smi2yangLeafrefLeaf(conti, vbNode);
	} else {
	    smi2yangLeaf(conti, vbNode);
	}
    }
}


static void
smi2yangNotifications(SmiModule *smiModule, _YangNode *yangModulePtr)
{
    SmiNode *smiNode;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {
	smi2yangNotification(yangModulePtr, smiNode);
    }
}


YangNode*
yangGetModuleFromSmiModule(SmiModule *smiModule, int flags)
{
    _YangNode       *yangModulePtr = NULL;
    _YangModuleInfo *yangModuleInfoPtr = NULL;

    smi2yangFlags = flags;

    createImportList(smiModule);

    yangModulePtr = addYangNode(smiModule->name, YANG_DECL_MODULE, NULL);
    yangModuleInfoPtr = createModuleInfo(yangModulePtr);

    smi2yangNamespace(smiModule, yangModulePtr, yangModuleInfoPtr);
    smi2yangImports(smiModule, yangModulePtr);
    smi2yangMeta(smiModule, yangModulePtr, yangModuleInfoPtr);
    smi2yangRevisions(smiModule, yangModulePtr);
    
    /* body statements */

    smi2yangTypedefs(smiModule, yangModulePtr);
    smi2yangIdentities(smiModule, yangModulePtr);
    smi2yangContainers(smiModule, yangModulePtr);
    smi2yangNotifications(smiModule, yangModulePtr);
    
    yangModuleInfoPtr->parsingState  = YANG_PARSING_DONE;
    freeImportList();

    semanticAnalysis(yangModulePtr);

    return &(yangModulePtr->export);
}
