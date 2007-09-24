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



static char *getAccessString(SmiAccess access)
{
    return
	(access == SMI_ACCESS_NOT_ACCESSIBLE) ? "noaccess" :
	(access == SMI_ACCESS_NOTIFY)	      ? "notifyonly" :
	(access == SMI_ACCESS_READ_ONLY)      ? "readonly" :
	(access == SMI_ACCESS_READ_WRITE)     ? "readwrite" :
						"<unknown>";
}



static char *getStringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<unknown>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "OctetString" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectIdentifier" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "Unsigned32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "Integer32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "Unsigned64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "Integer64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "Float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "Float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "Float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "Enumeration" :
        (basetype == SMI_BASETYPE_BITS)              ? "Bits" :
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



static char *getTypeString(SmiBasetype basetype, SmiType *smiType)
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
	fprint(f, " \"http://www.ietf.org/smi/%s\";\n", import->module);
	fprintSegment(f, INDENT, "}\n", 0);
    }

    freeImportList();
}




static void
fprintSubtype(FILE *f, SmiType *smiType)
{
    SmiRange       *range;
    SmiNamedNumber *nn;
    char	   s[1024];
    int		   i = 0;

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (! i) {
		fprint(f, " {\n");
	    }
	    sprintf(s, "enum %s { value %s; }\n", nn->name,
		    getValueString(&nn->value, smiType));
	    fprintSegment(f, 3*INDENT, s, 0);
	}
	if (i) {
	    fprintSegment(f, 2*INDENT, "}\n", 0);
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType);
	    range ; i++, range = smiGetNextRange(range)) {
	    if (i) {
		fprint(f, " | ");
	    } else {
		fprint(f, " {\n");
		if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		    fprintSegment(f, 3 * INDENT, "length \"", 0);
		} else {
		    fprintSegment(f, 3 * INDENT, "range \"", 0);
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
	    fprintSegment(f, 2*INDENT, "}\n", 0);
	}
    }
    if (! i) {
	fprint(f, ";\n");
    }
}



static void
fprintTypedefs(FILE *f, SmiModule *smiModule)
{
    int		 i, j;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(smiModule);
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

	fprintSegment(f, 2 * INDENT, "type", 0);
	fprint(f, " %s", getTypeString(smiType->basetype,
				       smiGetParentType(smiType)));
	fprintSubtype(f, smiType);
#if 0
	if (smiType->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    fprintSegment(f, 2 * INDENT, "default", INDENTVALUE);
	    fprint(f, "%s", getValueString(&smiType->value, smiType));
	    fprint(f, ";\n");
	}
#endif
	
	if (smiType->format) {
	    fprintSegment(f, 2 * INDENT, "smi:format", 0);
	    fprint(f, " \"%s\";\n", smiType->format);
	}
	if (smiType->units) {
	    fprintSegment(f, 2 * INDENT, "units", INDENTVALUE);
	    fprint(f, "\"%s\";\n", smiType->units);
	}
#if 0
	if ((smiType->status != SMI_STATUS_CURRENT) &&
	    (smiType->status != SMI_STATUS_UNKNOWN) &&
	    (smiType->status != SMI_STATUS_MANDATORY) &&
	    (smiType->status != SMI_STATUS_OPTIONAL)) {
	    fprintSegment(f, 2 * INDENT, "status", INDENTVALUE);
	    fprint(f, "%s;\n", getStringStatus(smiType->status));
	}
#else
	fprintSegment(f, 2 * INDENT, "status", 0);
	fprint(f, " %s;\n", getStringStatus(smiType->status));
#endif
	fprintSegment(f, 2 * INDENT, "description", INDENTVALUE);
	fprint(f, "\n");
	fprintMultilineString(f, 2 * INDENT, smiType->description);
	fprint(f, ";\n");
	if (smiType->reference) {
	    fprintSegment(f, 2 * INDENT, "reference", INDENTVALUE);
	    fprint(f, "\n");
	    fprintMultilineString(f, 2 * INDENT, smiType->reference);
	    fprint(f, ";\n");
	}
	fprintSegment(f, INDENT, "}\n\n", 0);
	i++;
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
	    fprintSegment(f, INDENT, "/*** NAMESPACE INFORMATION ***/\n\n", 0);
	}
	fprintImports(f, modv[i]);
	fprintSegment(f, INDENT, "target", 0);
	fprint(f, " %s {\n", smiModule->name);
	fprintSegment(f, 2 * INDENT, "\"", 0);
        fprint(f, "http://www.ietf.org/smi/%s\";\n", smiModule->name);
	fprintSegment(f, INDENT, "}\n\n", 0);
	
	fprintTypedefs(f, modv[i]);

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
