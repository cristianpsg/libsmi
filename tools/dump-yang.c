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
 * @(#) $Id: dump-mosy.c 6635 2007-03-13 21:53:21Z schoenw $
 */

#include <config.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"


#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	4    /* column to start multiline texts */
#define  INDENTMAX	64   /* max column to fill, break lines otherwise */

/*
 * Structure used to build a list of imported types.
 */

typedef struct Import {
    char          *module;
    struct Import *nextPtr;
} Import;

static Import *importList = NULL;

static int current_column = 0;
static int silent = 0;


static char *getStringStatus(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "current" :
	(status == SMI_STATUS_OPTIONAL)    ? "current" :
					     "<unknown>";
}



static char *getStringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<unknown>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "binary" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectIdentifier" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "uint32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "int32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "uint64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "int64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "enumeration" :
        (basetype == SMI_BASETYPE_BITS)              ? "binary" :
                                                   "<unknown>";
}



static char *getStringTime(time_t t)
{
    static char   s[27];
    struct tm	  *tm;

    tm = gmtime(&t);
    sprintf(s, "%04d-%02d-%02d %02d:%02d",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min);
    return s;
}



static char *getStringDate(time_t t)
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
    static const char *prefix = "urn:ietf:";
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
getModulePrefix(const char *moduleName)
{
    static char *buf = NULL;
    static size_t buflen = 0;
    size_t len;
    int i;

    static char *strip[] = {
	"-MIB", "-SMI", "-TC", NULL
    };

    len = strlen(moduleName) + 1;

    if (buflen < len) {
	if (buf) xfree(buf);
	buflen = len;
	buf = xmalloc(buflen);
    }

    for (i = 0; moduleName[i]; i++) {
	buf[i] = tolower(moduleName[i]);
    }
    buf[i] = 0;

    for (i = 0; strip[i]; i++) {
	len = strlen(strip[i]);
	if (buflen <= len) continue;
	if (strcasecmp(buf+buflen-len-1, strip[i]) == 0) {
	    buf[buflen-len-1] = 0;
	    break;
	}
    }

    return buf;
}



static char*
getTypeString(SmiBasetype basetype, SmiType *smiType)
{
    int         i;
    char        *typeModule, *typeName;

    typeName = smiType ? smiType->name : NULL;
    typeModule = smiType ? smiGetTypeModule(smiType)->name : NULL;
    
    if ((!typeModule) && (typeName) &&
	(basetype != SMI_BASETYPE_ENUM) &&
	(basetype != SMI_BASETYPE_BITS)) {
#if 0
	for(i=0; convertType[i]; i += 2) {
	    if (!strcmp(typeName, convertType[i])) {
		return convertType[i+1];
	    }
	}
#endif
    }

    if ((!typeModule) || (!strlen(typeModule)) || (!typeName)) {
	if (basetype == SMI_BASETYPE_ENUM) {
	    return "Enumeration";
	}
	if (basetype == SMI_BASETYPE_BITS) {
	    return "Bits";
	}
    }
	
    if (!typeName) {
	return getStringBasetype(basetype);
    }
    
    /* TODO: fully qualified if unambigous */

    return typeName;
}



static char *getValueString(SmiValue *valuePtr, SmiType *typePtr)
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



static Import*
addImport(char *module, char *name)
{
    Import **import, *newImport;
    
    if (!module || !name) {
	return NULL;
    }
	    
    for (import = &importList; *import; import = &(*import)->nextPtr) {
	int c = strcmp((*import)->module, module);
	if (c == 0) return;
	if (c > 0) break;
    }

    newImport = xmalloc(sizeof(Import));
    newImport->module = module;
    newImport->nextPtr = *import;
    *import = newImport;
	
    return *import;
}



static void
createImportList(SmiModule *smiModule)
{
    SmiImport   *smiImport;
    
    for (smiImport = smiGetFirstImport(smiModule); smiImport;
	 smiImport = smiGetNextImport(smiImport)) {
	addImport(smiImport->module, smiImport->name);
    }
}



static void
freeImportList(void)
{
    Import *import, *freeme;

    for (import = importList; import; ) {
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
fprint(FILE *f, char *fmt, ...)
{
    va_list ap;
    char    *s;
    char    *p;
    
    va_start(ap, fmt);
    current_column += smiVasprintf(&s, fmt, ap);
    va_end(ap);
    fputs(s, f);
    if ((p = strrchr(s, '\n'))) {
        current_column = strlen(p) - 1;
    }
    free(s);
}



static void
fprintSegment(FILE *f, int column, char *string, int length)
{
    fprint(f, "%*c%s", column, ' ', string);
    if (length) {
	fprint(f, "%*c", length - strlen(string) - column, ' ');
    }
}



static void
fprintWrapped(FILE *f, int column, char *string)
{
    if ((current_column + strlen(string)) > INDENTMAX) {
	putc('\n', f);
	current_column = 0;
	fprintSegment(f, column, "", 0);
    }
    fprint(f, "%s", string);
}



static void
fprintMultilineString(FILE *f, int column, const char *s)
{
    int i, len;
    
    fprintSegment(f, column - 1 + INDENTTEXTS, "\"", 0);
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    putc(s[i], f);
	    current_column++;
	    if (s[i] == '\n') {
		current_column = 0;
		fprintSegment(f, column + INDENTTEXTS, "", 0);
	    }
	}
    }
    putc('\"', f);
    current_column++;
}



static void
fprintRevisions(FILE *f, SmiModule *smiModule)
{
    int i;
    SmiRevision *smiRevision;
    
    for(i = 0, smiRevision = smiGetFirstRevision(smiModule);
	smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	fprintSegment(f, INDENT, "revision ", 0);
	fprint(f, "\"%s\" {\n", getStringDate(smiRevision->date));
	fprintSegment(f, 2 * INDENT, "description", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, 2 * INDENT, smiRevision->description);
	fprint(f, ";\n");
        fprintSegment(f, INDENT, "};\n", 0);
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
    
    createImportList(smiModule);
    
    for (import = importList; import; import = import->nextPtr) {
	fprintSegment(f, INDENT, "import", 0);
	fprint(f, " %s {\n", import->module);
	fprintSegment(f, 2*INDENT, "namespace", 0);
	fprint(f, " \"urn:ietf:%s\";\n", import->module);
	fprintSegment(f, 2*INDENT, "prefix   ", 0);
	fprint(f, " \"%s\";\n", getModulePrefix(import->module));
	fprintSegment(f, INDENT, "}\n", 0);
    }
    fprint(f, "\n");

    freeImportList();
}




static void
fprintSubtype(FILE *f, int indent, SmiType *smiType)
{
    SmiRange       *range;
    SmiNamedNumber *nn;
    char	   s[1024];
    int		   i = 0;

    if (smiType->name) {
	fprint(f, ";\n");
	return;
    }

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (! i) {
		fprint(f, " {\n");
	    }
	    sprintf(s, "enum %s { value %s; }\n", nn->name,
		    getValueString(&nn->value, smiType));
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

    fprintSegment(f, indent, "type", 0);

    smiModule = smiGetTypeModule(smiType);
    if (smiType) parentType = smiGetParentType(smiType);
    if (parentType) {
	parentModule = smiGetTypeModule(parentType);
    }

    /* this type is a base type - easy */

    if (parentType == NULL) {
	fprint(f, " %s", getStringBasetype(smiType->basetype));
	goto done;
    }

    /* this type is an imported type - easy */

    if (smiModule != thisModule) {
	fprint(f, " %s:%s",
	       getModulePrefix(smiModule->name),
	       smiType->name);
	goto done;
    }

    /* this is a locally defined named type - easy as well */

    if (smiModule == thisModule && smiType->name) {
	fprint(f, " %s", smiType->name);
	goto done;
    }

    /* this is a locally defined unnamed type - easy as well? */

    if (smiModule == thisModule && smiType->name == NULL) {
	if (parentModule == thisModule) {
	    fprint(f, " %s", parentType->name);
	} else {
	    fprint(f, " %s:%s",
		   getModulePrefix(parentModule->name),
		   parentType->name);
	}
	goto done;
    }

    /* uops - still here ? */
    
#if 0
    if (parentModule && parentModule != smiGetTypeModule(smiType)) {
	fprint(f, " %s:%s;\n",
	       getModulePrefix(parentModule->name),
	       parentType->name);
	return;
    }
    
    fprint(f, " %s", getTypeString(smiType->basetype,
				   smiGetParentType(smiType)));
#endif
 done:

    fprintSubtype(f, indent, smiType);
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
    if (format) {
	fprintSegment(f, 2 * INDENT, "smi:format", 0);
	fprint(f, " \"%s\";\n", format);
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
    if (access != SMI_ACCESS_READ_WRITE) {
	fprintSegment(f, indent, "config", 0);
	fprint(f, " false;\n");
    }
}


static void
fprintTypedefs(FILE *f, SmiModule *smiModule)
{
    int		 i, j;
    SmiType	 *smiType;
    
    for (i = 0, smiType = smiGetFirstType(smiModule);
	 smiType; smiType = smiGetNextType(smiType)) {

#if 0
	if ((!(strcmp(smiModule->name, "SNMPv2-SMI"))) ||
	    (!(strcmp(smiModule->name, "RFC1155-SMI")))) {
	    for(j=0; excludeType[j]; j++) {
		if (!strcmp(smiType->name, excludeType[j])) break;
	    }
	    if (excludeType[j]) break;
	}
#endif
	    
	if (!i && !silent) {
	    fprintSegment(f, INDENT, "/*** TYPE DEFINITIONS ***/\n\n", 0);
	}
	fprintSegment(f, INDENT, "", 0);
	fprint(f, "typedef %s {\n", smiType->name);

	fprintType(f, 2 * INDENT,
		   smiGetTypeModule(smiType),
		   smiGetParentType(smiType));

#if 0
	if (smiType->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    fprintSegment(f, 2 * INDENT, "default", INDENTVALUE);
	    fprint(f, "%s", getValueString(&smiType->value, smiType));
	    fprint(f, ";\n");
	}
#endif
	
	fprintUnits(f, 2 * INDENT, smiType->units);
	fprintFormat(f, 2 * INDENT, smiType->format);
	fprintStatus(f, 2 * INDENT, smiType->status);
	fprintDescription(f, 2 * INDENT, smiType->description);
	fprintReference(f, 2 * INDENT, smiType->reference);

	fprintSegment(f, INDENT, "}\n\n", 0);
	i++;
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
    fprintUnits(f, indent, smiNode->units);
    fprintStatus(f, indent + INDENT, smiNode->status);
    fprintDescription(f, indent + INDENT, smiNode->description);
    fprintReference(f, indent + INDENT, smiNode->reference);
    fprintConfig(f, indent + INDENT, smiNode->access);
    fprintSegment(f, indent, "}\n", 0);
}


static void
fprintContainer(FILE *f, int indent, SmiNode *smiNode)
{
    SmiNode *childNode;
    char *type;

    type = (smiNode->nodekind == SMI_NODEKIND_ROW) ? "list" : "container";
    
    fprintSegment(f, indent, type, 0);
    fprint(f, " %s {\n", smiNode->name);

    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_SCALAR
	    || childNode->nodekind == SMI_NODEKIND_COLUMN) {
	    fprintLeaf(f, indent + INDENT, childNode);
	}
    }
    
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
	if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    fprintContainer(f, INDENT, smiNode);
	}
    }
}



static void
dumpYang(int modc, SmiModule **modv, int flags, char *output)
{
    SmiModule   *smiModule;
    int		i;
    FILE	*f = stdout;

    silent = (flags & SMIDUMP_FLAG_SILENT);

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

	fprint(f, "/*\n");
	fprint(f, " * This module has been generated by smidump "
	       SMI_VERSION_STRING ":\n");
	fprint(f, " *\n");
	fprint(f, " *      smidump -f yang %s\n", smiModule->name);
	fprint(f, " *\n");
	fprint(f, " * Do not edit. Edit the source file instead!\n");
	fprint(f, " */\n\n");
	fprint(f, "module %s {\n", smiModule->name);
	fprint(f, "\n");

	if (! silent) {
	    fprintSegment(f, INDENT, "/*** META INFORMATION ***/\n\n", 0);
	}
	fprintSegment(f, INDENT, "organization", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, INDENT, smiModule->organization);
	fprint(f, ";\n\n");
	fprintSegment(f, INDENT, "contact", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, INDENT, smiModule->contactinfo);
	fprint(f, ";\n\n");
	fprintSegment(f, INDENT, "description", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, INDENT, smiModule->description);
	fprint(f, ";\n\n");
	if (smiModule->reference) {
	    fprintSegment(f, INDENT, "reference", INDENTVALUE);
	    fprint(f, "\n");
	    fprintMultilineString(f, INDENT, smiModule->reference);
	    fprint(f, ";\n\n");
	}

	fprintRevisions(f, smiModule);

	if (! silent) {
	    fprintSegment(f, INDENT, "/*** NAMESPACE / PREFIX DEFINITION ***/\n\n", 0);
	}

	fprintSegment(f, INDENT, "target-namespace ", 0);
	fprint(f, "\"%s\";\n", getModuleUrn(smiModule->name));
	fprintSegment(f, INDENT, "prefix ", 0);
	fprint(f, "\"%s\";\n\n", getModulePrefix(smiModule->name));

	if (! silent) {
	    fprintSegment(f, INDENT, "/*** LINKAGE (IMPORTS / INCLUDES) ***/\n\n", 0);
	}

	fprintImports(f, modv[i]);
	fprintTypedefs(f, modv[i]);
	fprintContainers(f, modv[i]);

#if 0	
	printAssignements(f, modv[i]);
	printObjects(f, modv[i]);
	printNotifications(f, modv[i]);
	printGroups(f, modv[i]);
	printCompliances(f, modv[i]);
#endif

    	fprint(f, "} /* end of module %s */\n", smiModule->name);
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
    
    static SmidumpDriver driver = {
	"yang",
	dumpYang,
	0,
	SMIDUMP_DRIVER_CANT_UNITE,
	"YANG format",
	NULL,
	NULL
    };
    
    smidumpRegisterDriver(&driver);
}
