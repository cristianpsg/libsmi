/*
 * dump-smi.c --
 *
 *      Operations to dump SMIv1/v2 module information.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-smi.c,v 1.55 2000/04/10 14:20:27 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "smi.h"
#include "smidump.h"


#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */
#define  INDENTSEQUENCE 40   /* column to start SEQUENCE tables' type column */



static char *convertTypev1[] = {
    NULL,		 "Counter32",	       NULL,	   "Counter",
    NULL,		 "Gauge32",	       NULL,	   "Gauge",
    NULL,		 "Integer32",	       NULL,	   "INTEGER",
    NULL,		 "Unsigned32",	       NULL,	   "Gauge",
    NULL,		 "Bits",	       NULL,	   "OCTET STRING",
    NULL,		 "OctetString",	       NULL,	   "OCTET STRING",
    NULL,		 "ObjectIdentifier",   NULL,	   "OBJECT IDENTIFIER",
    NULL, NULL, NULL, NULL };


static char *convertTypev2[] = {
    NULL,		 "INTEGER",	       NULL,	   "Integer32",
    NULL,		 "Enumeration",	       NULL,	   "INTEGER",
    NULL,		 "Bits",	       NULL,	   "BITS",
    NULL,		 "OctetString",	       NULL,	   "OCTET STRING",
    NULL,		 "ObjectIdentifier",   NULL,	   "OBJECT IDENTIFIER",
    NULL,		 "Counter",	       NULL,	   "Counter32",
    NULL,		 "Gauge",	       NULL,	   "Gauge32",
    NULL, NULL, NULL, NULL };


static char *convertImportv2[] = {
    "IRTF-NMRG-SMING-TYPES", "IpAddress",       "SNMPv2-SMI", "IpAddress",
    "IRTF-NMRG-SMING-TYPES", "Counter32",       "SNMPv2-SMI", "Counter32",
    "IRTF-NMRG-SMING-TYPES", "Gauge32",	        "SNMPv2-SMI", "Gauge32",
    "IRTF-NMRG-SMING-TYPES", "TimeTicks",       "SNMPv2-SMI", "TimeTicks",
    "IRTF-NMRG-SMING-TYPES", "Opaque",	        "SNMPv2-SMI", "Opaque",
    "IRTF-NMRG-SMING-TYPES", "Counter64",       "SNMPv2-SMI", "Counter64",
    "IRTF-NMRG-SMING-TYPES", "DisplayString",   "SNMPv2-TC", "DisplayString",
    "IRTF-NMRG-SMING-TYPES", "PhysAddress",     "SNMPv2-TC", "PhysAddress",
    "IRTF-NMRG-SMING-TYPES", "MacAddress",      "SNMPv2-TC", "MacAddress",
    "IRTF-NMRG-SMING-TYPES", "TruthValue",      "SNMPv2-TC", "TruthValue",
    "IRTF-NMRG-SMING-TYPES", "TestAndIncr",     "SNMPv2-TC", "TestAndIncr",
    "IRTF-NMRG-SMING-TYPES", "AutonomousType",  "SNMPv2-TC", "AutonomousType",
    "IRTF-NMRG-SMING-TYPES", "InstancePointer", "SNMPv2-TC", "InstancePointer",
    "IRTF-NMRG-SMING-TYPES", "VariablePointer", "SNMPv2-TC", "VariablePointer",
    "IRTF-NMRG-SMING-TYPES", "RowPointer",      "SNMPv2-TC", "RowPointer",
    "IRTF-NMRG-SMING-TYPES", "RowStatus",       "SNMPv2-TC", "RowStatus",
    "IRTF-NMRG-SMING-TYPES", "TimeStamp",       "SNMPv2-TC", "TimeStamp",
    "IRTF-NMRG-SMING-TYPES", "TimeInterval",    "SNMPv2-TC", "TimeInterval",
    "IRTF-NMRG-SMING-TYPES", "DateAndTime",     "SNMPv2-TC", "DateAndTime",
    "IRTF-NMRG-SMING-TYPES", "StorageType",     "SNMPv2-TC", "StorageType",
    "IRTF-NMRG-SMING-TYPES", "TDomain",         "SNMPv2-TC", "TDomain",
    "IRTF-NMRG-SMING-TYPES", "TAddress",        "SNMPv2-TC", "TAddress",
    "IRTF-NMRG-SMING", NULL,                    "SNMPv2-SMI", NULL,

    "RFC1155-SMI", "internet",	    "SNMPv2-SMI", "internet",
    "RFC1155-SMI", "directory",	    "SNMPv2-SMI", "directory",
    "RFC1155-SMI", "mgmt",	    "SNMPv2-SMI", "mgmt",
    "RFC1155-SMI", "experimental",  "SNMPv2-SMI", "experimental",
    "RFC1155-SMI", "private",	    "SNMPv2-SMI", "private",
    "RFC1155-SMI", "enterprises",   "SNMPv2-SMI", "enterprises",
    "RFC1155-SMI", "IpAddress",     "SNMPv2-SMI", "IpAddress",
    "RFC1155-SMI", "Counter",       "SNMPv2-SMI", "Counter32",
    "RFC1155-SMI", "Gauge",         "SNMPv2-SMI", "Gauge32",
    "RFC1155-SMI", "TimeTicks",     "SNMPv2-SMI", "TimeTicks",
    "RFC1155-SMI", "Opaque",        "SNMPv2-SMI", "Opaque",
    "RFC1213-MIB", "mib-2",         "SNMPv2-SMI", "mib-2",    
    "RFC1213-MIB", "DisplayString", "SNMPv2-TC",  "DisplayString",    
    NULL, NULL, NULL, NULL };

static char *convertImportv1[] = {
    "IRTF-NMRG-SMING-TYPES", "IpAddress",   "RFC1155-SMI", "IpAddress",
    "IRTF-NMRG-SMING-TYPES", "Counter32",   "RFC1155-SMI", "Counter",
    "IRTF-NMRG-SMING-TYPES", "Gauge32",	    "RFC1155-SMI", "Gauge",
    "IRTF-NMRG-SMING-TYPES", "TimeTicks",   "RFC1155-SMI", "TimeTicks",
    "IRTF-NMRG-SMING-TYPES", "Opaque",	    "RFC1155-SMI", "Opaque",
    "IRTF-NMRG-SMING-TYPES", "Counter64",   NULL,          NULL,
    "IRTF-NMRG-SMING",       "mib-2",	    "RFC1213-MIB", "mib-2",
    
    "SNMPv2-SMI", "IpAddress",              "RFC1155-SMI", "IpAddress",
    "SNMPv2-SMI", "Counter32",              "RFC1155-SMI", "Counter",
    "SNMPv2-SMI", "Gauge32",	            "RFC1155-SMI", "Gauge",
    "SNMPv2-SMI", "TimeTicks",              "RFC1155-SMI", "TimeTicks",
    "SNMPv2-SMI", "Opaque",	            "RFC1155-SMI", "Opaque",
    "SNMPv2-SMI", "Integer32",	            NULL,	   NULL,
    "SNMPv2-SMI", "Unsigned32",	            NULL,	   NULL,
    "SNMPv2-SMI", "Counter64",	            NULL,	   NULL,
    "SNMPv2-SMI", "mib-2",                  "RFC1213-MIB", "mib-2",
    "SNMPv2-SMI", "MODULE-IDENTITY",        NULL,	   NULL,
    "SNMPv2-SMI", "OBJECT-IDENTITY",        NULL,	   NULL,
    "SNMPv2-SMI", "ObjectName",		    NULL,	   NULL,
    "SNMPv2-SMI", "NotificationName",       NULL,	   NULL,
    "SNMPv2-SMI", "ObjectSyntax",	    NULL,	   NULL,
    "SNMPv2-SMI", "SimpleSyntax",	    NULL,	   NULL,
    "SNMPv2-SMI", "ApplicationSyntax",      NULL,	   NULL,
    "SNMPv2-SMI", "OBJECT-TYPE",	    NULL,	   NULL,
    "SNMPv2-SMI", "NOTIFICATION-TYPE",      NULL,	   NULL,
    NULL, NULL, NULL, NULL };

static int current_column = 0;
static int smiv1 = 0;
static int silent = 0;



typedef struct Import {
    char          *module;
    char          *name;
    struct Import *nextPtr;
} Import;

static Import *importList = NULL;



static int invalidType(SmiBasetype basetype)
{
    if (smiv1) {
	return (basetype == SMI_BASETYPE_INTEGER64)
	    || (basetype == SMI_BASETYPE_UNSIGNED64)
	    || (basetype == SMI_BASETYPE_FLOAT32)
	    || (basetype == SMI_BASETYPE_FLOAT64)
	    || (basetype == SMI_BASETYPE_FLOAT128);
    } else {
	return (basetype == SMI_BASETYPE_INTEGER64)
	    || (basetype == SMI_BASETYPE_FLOAT32)
	    || (basetype == SMI_BASETYPE_FLOAT64)
	    || (basetype == SMI_BASETYPE_FLOAT128);
    }
}



static char *getStatusString(SmiStatus status)
{
    if (smiv1) {
	return
	    (status == SMI_STATUS_CURRENT)     ? "mandatory" :
	    (status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	    (status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	    (status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	    (status == SMI_STATUS_OPTIONAL)    ? "optional" :
					         "<unknown>";
    } else {
	return
	    (status == SMI_STATUS_CURRENT)     ? "current" :
	    (status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	    (status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	    (status == SMI_STATUS_MANDATORY)   ? "current" :
	    (status == SMI_STATUS_OPTIONAL)    ? "current" :
					         "<unknown>";
    }
}



static char *getAccessString(SmiAccess access, int create)
{
    if (smiv1) {
	return
	    (access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" :
	    (access == SMI_ACCESS_NOTIFY)	  ? "read-only" :
	    (access == SMI_ACCESS_READ_ONLY)      ? "read-only" :
	    (access == SMI_ACCESS_READ_WRITE)     ? "read-write" :
						    "<unknown>";
    } else {
	if (create && (access == SMI_ACCESS_READ_WRITE)) {
	    return "read-create";
	} else {
	    return
	    (access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" :
	    (access == SMI_ACCESS_NOTIFY)	  ? "accessible-for-notify" :
	    (access == SMI_ACCESS_READ_ONLY)      ? "read-only" :
	    (access == SMI_ACCESS_READ_WRITE)     ? "read-write" :
						    "<unknown>";
	}
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



static char *getTypeString(SmiBasetype basetype, SmiType *smiType)
{
    int         i;
    char	**convertType;
    char        *typeModule, *typeName;

    typeName = smiType ? smiType->name : NULL;
    typeModule = smiType ? smiGetTypeModule(smiType)->name : NULL;

    convertType = smiv1 ? convertTypev1 : convertTypev2;

    if (typeName &&
	(basetype != SMI_BASETYPE_ENUM) &&
	(basetype != SMI_BASETYPE_BITS)) {
	for(i=0; convertType[i+1]; i += 4) {
	    if ((!strcmp(typeName, convertType[i+1])) &&
		((!typeModule) || (!convertType[i]) ||
		 (!strcmp(typeModule, convertType[i])))) {
		return convertType[i+3];
	    }
	}
    }

    if ((!typeModule) || (!strlen(typeModule)) || (!typeName)) {
	if (basetype == SMI_BASETYPE_ENUM) {
	    return "INTEGER";
	}
	if (basetype == SMI_BASETYPE_BITS) {
	    if (smiv1) {
		return "OCTET STRING";
	    } else {
		return "BITS";
	    }
	}
    }
	
    /* TODO: fully qualified if unambigous */

    return typeName;
}



static char *getOidString(SmiNode *smiNode, int importedParent)
{
    SmiNode	 *parentNode;
    SmiModule	 *smiModule;
    static char	 s[200];
    char	 append[200];
    unsigned int i;

    append[0] = 0;

    parentNode = smiNode;
    smiModule = smiGetNodeModule(smiNode);
    
    do {
	
	if (parentNode->oidlen <= 1) {
	    break;
	}
	
	/* prepend the cut-off subidentifier to `append'. */
	strcpy(s, append);
	sprintf(append, " %u%s", parentNode->oid[parentNode->oidlen-1], s);

	/* retrieve the parent SmiNode */
	parentNode = smiGetParentNode(parentNode);

	if (!parentNode) {
	    sprintf(s, "%s", append);
	    return s;
	}
	
	/* found an imported or a local parent node? */
	if ((parentNode->name && strlen(parentNode->name)) &&
	    (smiIsImported(smiModule, NULL, parentNode->name) ||
	     (!importedParent &&
	      (smiGetNodeModule(parentNode) == smiModule)))) {
	    sprintf(s, "%s%s", parentNode->name, append);
	    return s;
	}
	
    } while (parentNode);

    s[0] = 0;
    for (i=0; i < smiNode->oidlen; i++) {
	if (i) strcat(s, " ");
	sprintf(&s[strlen(s)], "%u", smiNode->oid[i]);
    }
    return s;
}



static char *getUppercaseString(char *s)
{
    static char *ss;

    ss = xstrdup(s);
    ss[0] = (char)toupper((int)ss[0]);
    return ss;
}



static int isObjectGroup(SmiNode *groupNode)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    
    for (smiElement = smiGetFirstElement(groupNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {

	smiNode = smiGetElementNode(smiElement);
	
	if (smiNode->nodekind != SMI_NODEKIND_SCALAR
	    && smiNode->nodekind != SMI_NODEKIND_COLUMN) {
	    return 0;
	}
    }

    return 1;
}



static int isNotificationGroup(SmiNode *groupNode)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    
    for (smiElement = smiGetFirstElement(groupNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {

	smiNode = smiGetElementNode(smiElement);
	
	if (smiNode->nodekind != SMI_NODEKIND_NOTIFICATION) {
	    return 0;
	}
    }

    return 1;
}



static Import* addImport(char *module, char *name)
{
    Import **import, *newImport;
    char   **convertImport;
    int    i;

    convertImport = smiv1 ? convertImportv1 : convertImportv2;

    for(i = 0; convertImport[i]; i += 4) {
	if (convertImport[i] && convertImport[i+1]
	    && !strcmp(module, convertImport[i])
	    && !strcmp(name, convertImport[i+1])) {
	    module = convertImport[i+2];
	    name = convertImport[i+3];
	    break;
	} else if (convertImport[i] && !convertImport[i+1]
		   && !strcmp(module, convertImport[i])) {
	    module = convertImport[i+2];
	    break;
	}
    }

    if (!module || !name) {
	return NULL;
    }
	    
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
    if (! newImport) {
	return NULL;
    }
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
    SmiNodekind kind = SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN;
    SmiImport   *smiImport;
    SmiModule	*smiModule2;
    
    for(smiNode = smiGetFirstNode(smiModule, kind); smiNode;
	smiNode = smiGetNextNode(smiNode, kind)) {
	smiType = smiGetNodeType(smiNode);
	if (smiType) {
	    smiModule2 = smiGetTypeModule(smiType);
	    if (smiModule2 && (smiModule2 != smiModule)) {
		if (strlen(smiModule2->name) && smiType->name) {
		    addImport(smiModule2->name, smiType->name);
		}
	    }
	}

	if (! smiv1 &&
	    smiType && smiType->basetype == SMI_BASETYPE_INTEGER32) {
	    addImport("SNMPv2-SMI", "Integer32");
	}

	if ((!smiv1) &&
	    (smiNode->value.basetype == SMI_BASETYPE_OBJECTIDENTIFIER) &&
	    (!strcmp(smiNode->value.value.ptr, "zeroDotZero"))) {
	    addImport("SNMPv2-SMI", "zeroDotZero");
	}
    }

    smiNode = smiGetFirstNode(smiModule,
			      SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN
			      | SMI_NODEKIND_TABLE | SMI_NODEKIND_ROW);
    if (smiNode) {
	addImport(smiv1 ? "RFC-1212" : "SNMPv2-SMI", "OBJECT-TYPE");
    }

    smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
    if (smiNode) {
	addImport(smiv1 ? "RFC-1215" : "SNMPv2-SMI",
		  smiv1 ? "TRAP-TYPE" : "NOTIFICATION-TYPE");
    }

    if (! smiv1) {
	smiNode = smiGetModuleIdentityNode(smiModule);
	if (smiNode) {
	    if (strcmp("SNMPv2-SMI", smiModule->name)) {
		addImport("SNMPv2-SMI", "MODULE-IDENTITY");
	    }
	}
    }
    
    if (! smiv1) {
	for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NODE);
	    smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NODE)) {
	    if (smiNode->status != SMI_STATUS_UNKNOWN &&
		smiNode != smiGetModuleIdentityNode(smiModule)) {
		if (strcmp("SNMPv2-SMI", smiModule->name)) {
		    addImport("SNMPv2-SMI", "OBJECT-IDENTITY");
		}
		break;
	    }
	}
    }

    if (! smiv1) {
	smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_COMPLIANCE);
	if (smiNode) {
	    if (strcmp("SNMPv2-CONF", smiModule->name)) {
		addImport("SNMPv2-CONF", "MODULE-COMPLIANCE");
	    }
	}
    }

    if (! smiv1) {
	for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_GROUP);
	    smiNode;
	    smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
	    if (isObjectGroup(smiNode)) {
		if (strcmp("SNMPv2-CONF", smiModule->name)) {
		    addImport("SNMPv2-CONF", "OBJECT-GROUP");
		}
	    } else if (isNotificationGroup(smiNode)) {
		if (strcmp("SNMPv2-CONF", smiModule->name)) {
		    addImport("SNMPv2-CONF", "NOTIFICATION-GROUP");
		}
	    }
	}
    }

    if (! smiv1) {
	for(smiType = smiGetFirstType(smiModule);
	    smiType; smiType = smiGetNextType(smiType)) {
	    if (smiType->description) {
		if (strcmp("SNMPv2-TC", smiModule->name)) {
		    addImport("SNMPv2-TC", "TEXTUAL-CONVENTION");
		}
		break;
	    }
	}
    }

    for(smiImport = smiGetFirstImport(smiModule); smiImport;
	smiImport = smiGetNextImport(smiImport)) {
	if (islower((int) smiImport->name[0]) ||
	    (!strcmp(smiImport->module, "SNMPv2-SMI")) ||
	    (!smiv1 && !strcmp(smiImport->module, "SNMPv2-TC"))) {
	    addImport(smiImport->module, smiImport->name);
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



static void printSegment(int column, char *string, int length, int comment)
{
    char *f;
    
    if (comment) {
	f = "-- %*c%s";
	/* if (column >= 3) column -= 3; */
    } else {
	f = "%*c%s";
    }
    
    print(f, column, ' ', string);
    if (length) {
	print("%*c", length - strlen(string) - column, ' ');
    }
}



static void printWrapped(int column, char *string, int comment)
{
    if ((current_column + strlen(string)) > INDENTMAX) {
	putc('\n', stdout);
	current_column = 0;
	printSegment(column, "", 0, comment);
    }
    print("%s", string);
}



static void printMultilineString(const char *s, const int comment)
{
    int i, len;
    
    printSegment(INDENTTEXTS - 1, "\"", 0, comment);
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    putc(s[i], stdout);
	    current_column++;
	    if (s[i] == '\n') {
		current_column = 0;
		printSegment(INDENTTEXTS, "", 0, comment);
	    }
	}
    }
    putc('\"', stdout);
    current_column++;
}



static char *getValueString(SmiValue *valuePtr, SmiType *typePtr)
{
    static char    s[100];
    char           ss[9];
    int		   i, n;
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
            sprintf(s, "'%*s'H", 2 * valuePtr->len, " ");
            for (i=0; i < valuePtr->len; i++) {
                sprintf(ss, "%02x", valuePtr->value.ptr[i]);
                strncpy(&s[1+2*i], ss, 2);
            }
	}
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "{");
	for (i = 0, n = 0; i < valuePtr->len * 8; i++) {
	    if (valuePtr->value.ptr[i/8] & (1 << i%8)) {
		for (nn = smiGetFirstNamedNumber(typePtr); nn;
		     nn = smiGetNextNamedNumber(nn)) {
		    if (nn->value.value.unsigned32 == i)
			break;
		}
		if (nn) {
		    if (n)
			sprintf(&s[strlen(s)], ", ");
		    n++;
		    sprintf(&s[strlen(s)], "%s", nn->name);
		}
	    }
	}
	sprintf(&s[strlen(s)], "}");
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	nodePtr = smiGetNodeByOID(valuePtr->len, valuePtr->value.oid);
	if (nodePtr) {
	    sprintf(s, "%s", nodePtr->name);
	} else {
	    strcpy(s, "{");
	    for (i=0; i < valuePtr->len; i++) {
		if (i) strcat(s, " ");
		sprintf(&s[strlen(s)], "%u", valuePtr->value.oid[i]);
	    }
	    strcat(s, "}");
	}
	break;
    }

    return s;
}



static void printSubtype(SmiType *smiType, const int comment)
{
    SmiRange       *range;
    SmiNamedNumber *nn;
    char	   s[100];
    int		   i, c;

    c = comment || (smiv1 && smiType->basetype == SMI_BASETYPE_BITS);
    
    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType);
	    nn; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (i) {
		print(", ");
	    } else {
		if (c) {
		    print("\n");
		    printSegment(INDENT, "", INDENTVALUE, c);
		    print("  { ");
		} else {
		    print(" { ");
		}
	    }
	    sprintf(s, "%s(%s)", nn->name,
		    getValueString(&nn->value, smiType));
	    printWrapped(INDENTVALUE + INDENT, s, c);
	}
	if (i) {
	    print(" }");
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType);
	    range; i++, range = smiGetNextRange(range)) {
	    if (i) {
		print(" | ");
	    } else {
		if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		    print(" (SIZE(");
		} else {
		    print(" (");
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
	    printWrapped(INDENTVALUE + INDENT, s, 0);
	}
	if (i) {
	    if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		print(")");
	    }
	    print(")");
	}
    }
}



static void printIndex(SmiNode *indexNode, const int comment)
{
    SmiElement *smiElement;
    int        n, j;

    printSegment(INDENT, "INDEX", INDENTVALUE, comment);
    print("{ ");
    for (n = 0, smiElement = smiGetFirstElement(indexNode); smiElement;
	 n++, smiElement = smiGetNextElement(smiElement));
    for (j = 0, smiElement = smiGetFirstElement(indexNode); smiElement;
	 j++, smiElement = smiGetNextElement(smiElement)) {
	if (j) {
	    print(", ");
	}
	if (indexNode->implied && ((j+1) == n)) {
	    printWrapped(INDENTVALUE + 2, "IMPLIED ", 0);
	}
	printWrapped(INDENTVALUE + 2, smiGetElementNode(smiElement)->name, 0);
	/* TODO: non-local name if non-local */
    } /* TODO: empty? -> print error */
    print(" }\n");
}



static void printImports(char *modulename)
{
    Import        *import;
    char	  *lastModulename = NULL;
    char	  *importedModulename, *importedDescriptor;
    int		  i;
    char	  **convertImport;

    convertImport = smiv1 ? convertImportv1 : convertImportv2;

    for(import = importList; import; import = import->nextPtr) {
	importedModulename = import->module;
	importedDescriptor = import->name;

	if (!strlen(importedModulename))
	    continue;
	
	for(i = 0; convertImport[i]; i += 4) {
	    if (convertImport[i] && convertImport[i+1]
		&& !strcmp(importedModulename, convertImport[i])
		&& !strcmp(importedDescriptor, convertImport[i+1])) {
		importedModulename = convertImport[i+2];
		importedDescriptor = convertImport[i+3];
		break;
	    } else if (convertImport[i] && !convertImport[i+1]
		       && !strcmp(importedModulename, convertImport[i])) {
		importedModulename = convertImport[i+2];
		break;
	    }
	}

	if (importedModulename && importedDescriptor &&
	    strlen(importedDescriptor)) {
	    if ((!lastModulename) ||
		strcmp(importedModulename, lastModulename)) {
		if (!lastModulename) {
		    print("IMPORTS");
		} else {
		    print("\n");
		    printSegment(2 * INDENT, "", 0, 0);
		    print("FROM %s", lastModulename);
		}
		print("\n");
		printSegment(INDENT, "", 0, 0);
	    } else {
		print(",");
		print(" ");
	    }
	    printWrapped(INDENT, importedDescriptor, 0);
	    lastModulename = importedModulename;
	}
    }
    if (lastModulename) {
	print("\n");
	printSegment(2 * INDENT, "", 0, 0);
	print("FROM %s;\n\n", lastModulename);
    }
}



static void printModuleIdentity(SmiModule *smiModule)
{
    SmiRevision  *smiRevision;
    SmiNode      *smiNode;

    smiNode = smiGetModuleIdentityNode(smiModule);
    
    if (smiNode) {

	if (smiv1 && smiNode) {
	    print("%s OBJECT IDENTIFIER\n", smiNode->name);
	    printSegment(INDENT, "::= ", 0, 0);
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}

	if (! smiv1 || ! silent) {

	    if (smiv1) {
		print("-- %s MODULE-IDENTITY\n", smiNode->name);
	    } else {
		print("%s MODULE-IDENTITY\n", smiNode->name);
	    }
	    printSegment(INDENT, "LAST-UPDATED", INDENTVALUE, smiv1);
	    smiRevision = smiGetFirstRevision(smiModule);
	    if (smiRevision)
		print("\"%s\"\n", getTimeString(smiRevision->date));
	    else
		print("\"197001010000Z\"\n");
	    printSegment(INDENT, "ORGANIZATION", INDENTVALUE, smiv1);
	    print("\n");
	    printMultilineString(smiModule->organization, smiv1);
	    print("\n");
	    printSegment(INDENT, "CONTACT-INFO", INDENTVALUE, smiv1);
	    print("\n");
	    printMultilineString(smiModule->contactinfo, smiv1);
	    print("\n");
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE, smiv1);
	    print("\n");
	    if (smiModule->description) {
		printMultilineString(smiModule->description, smiv1);
	    } else {
		printMultilineString("...", smiv1);
	    }
	    print("\n");
	    
	    for(; smiRevision;
		smiRevision = smiGetNextRevision(smiRevision)) {
		if (!smiRevision->description
		    || strcmp(smiRevision->description,
	   "[Revision added by libsmi due to a LAST-UPDATED clause.]")) {
		    printSegment(INDENT, "REVISION", INDENTVALUE, smiv1);
		    print("\"%s\"\n", getTimeString(smiRevision->date));
		    printSegment(INDENT, "DESCRIPTION", INDENTVALUE, smiv1);
		    print("\n");
		    if (smiRevision->description) {
			printMultilineString(smiRevision->description, smiv1);
		    } else {
			printMultilineString("...", smiv1);
		    }
		    print("\n");
		}
	    }
	    
	    if (smiNode) {
		printSegment(INDENT, "::= ", 0, smiv1);
		print("{ %s }\n\n", getOidString(smiNode, 0));
	    }
	    /* TODO: else error */
	    
	    print("\n");
	}
    }
}



static void printTypeDefinitions(SmiModule *smiModule)
{
    SmiType	 *smiType;
    int		 invalid;

    for(smiType = smiGetFirstType(smiModule);
	smiType; smiType = smiGetNextType(smiType)) {
	if (smiType->status == SMI_STATUS_UNKNOWN) {
	    invalid = invalidType(smiType->basetype);
	    if (invalid) {
		print("-- %s ::=\n", smiType->name);
	    } else {
		print("%s ::=\n", smiType->name);
	    }
	    printSegment(INDENT, "", 0, invalid);
	    print("%s", getTypeString(smiType->basetype,
				      smiGetParentType(smiType)));
	    printSubtype(smiType, invalid);
	    print("\n\n");
	}
    }
}



static void printTextualConventions(SmiModule *smiModule)
{
    SmiType	 *smiType;
    int		 invalid;
    
    for(smiType = smiGetFirstType(smiModule);
	smiType; smiType = smiGetNextType(smiType)) {
	if (smiType->status != SMI_STATUS_UNKNOWN) {
	    invalid = invalidType(smiType->basetype);
	    if (smiv1 && !invalid) {
		print("%s ::=\n", smiType->name);
		printSegment(INDENT, "", 0, invalid);
		print("%s", getTypeString(smiType->basetype,
					  smiGetParentType(smiType)));
		printSubtype(smiType, invalid);
		print("\n\n");
	    }

	    if (! smiv1 || ! silent) {

		if (smiv1 || invalid) {
		    print("-- %s ::= TEXTUAL-CONVENTION\n", smiType->name);
		} else {
		    print("%s ::= TEXTUAL-CONVENTION\n", smiType->name);
		}

		if (smiType->format) {
		    printSegment(INDENT, "DISPLAY-HINT", INDENTVALUE,
				 smiv1 || invalid);
		    print ("\"%s\"\n", smiType->format);
		}
	    
		printSegment(INDENT, "STATUS", INDENTVALUE, smiv1 || invalid);
		print ("%s\n", getStatusString(smiType->status));
		
		printSegment(INDENT, "DESCRIPTION", INDENTVALUE, smiv1 || invalid);
		print("\n");
		if (smiType->description) {
		    printMultilineString(smiType->description, smiv1 || invalid);
		} else {
		    printMultilineString("...", smiv1 || invalid);
		}
		print("\n");
		
		if (smiType->reference) {
		    printSegment(INDENT, "REFERENCE", INDENTVALUE,
				 smiv1 || invalid);
		    print("\n");
		    printMultilineString(smiType->description, smiv1 || invalid);
		    print("\n");
		}
		printSegment(INDENT, "SYNTAX", INDENTVALUE, smiv1 || invalid);
		print("%s",
		      getTypeString(smiType->basetype,
				    smiGetParentType(smiType)));
		printSubtype(smiType, smiv1 || invalid);
		print("\n\n");
	    }
	}
    }
}



static void printObjects(SmiModule *smiModule)
{
    SmiNode	 *smiNode, *rowNode, *colNode, *smiParentNode, *relatedNode;
    SmiType	 *smiType;
    SmiNodekind  nodekinds;
    int		 i, invalid, create, assignement;
    
    nodekinds =  SMI_NODEKIND_NODE | SMI_NODEKIND_TABLE |
	SMI_NODEKIND_ROW | SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR |
	SMI_NODEKIND_CAPABILITIES;
    
    for(smiNode = smiGetFirstNode(smiModule, nodekinds);
	smiNode; smiNode = smiGetNextNode(smiNode, nodekinds)) {

	smiType = smiGetNodeType(smiNode);
	smiParentNode = smiGetParentNode(smiNode);
	
	create = smiParentNode ? smiParentNode->create : 0;
	
	invalid = !smiType ? 0 : invalidType(smiType->basetype);
	assignement = 0;

	if (invalid && silent
	    && (smiNode->nodekind == SMI_NODEKIND_SCALAR
		|| smiNode->nodekind == SMI_NODEKIND_COLUMN)) {
	    continue;
	}

	if (smiNode == smiGetModuleIdentityNode(smiModule)) {
	    continue;
	}
	
	if ((smiNode->nodekind == SMI_NODEKIND_NODE) &&
	    (smiNode->status == SMI_STATUS_UNKNOWN)) {
	    assignement = 1;
	    print("%s OBJECT IDENTIFIER\n", smiNode->name);
	} else if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	    if (smiv1) {
		assignement = 1;
		print("%s OBJECT IDENTIFIER\n", smiNode->name);
	    } else {
		print("%s OBJECT-IDENTITY\n", smiNode->name);
	    }
	} else if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	    /* TODO: real agent capabilities */
	    print("-- This has been an SMIv2 AGENT-CAPABILITIES node:\n");
	    if (smiv1) {
		assignement = 1;
		print("%s OBJECT IDENTIFIER\n", smiNode->name);
	    } else {
		print("%s OBJECT-IDENTITY\n", smiNode->name);
	    }
	} else {
	    if (invalid) {
		print("-- %s OBJECT-TYPE\n", smiNode->name);
	    } else {
		print("%s OBJECT-TYPE\n", smiNode->name);
	    }
	}

	if ((smiNode->nodekind == SMI_NODEKIND_TABLE) ||
	    (smiNode->nodekind == SMI_NODEKIND_ROW) ||
	    (smiType)) {
	    printSegment(INDENT, "SYNTAX", INDENTVALUE, invalid);
	    if (smiNode->nodekind == SMI_NODEKIND_TABLE) {
		print("SEQUENCE OF ");
		rowNode = smiGetFirstChildNode(smiNode);
		smiType = smiGetNodeType(rowNode);
		if (smiType) {
		    print("%s\n", smiType->name);
		} else {
		    /* guess type name is uppercase row name */
		    char *s = getUppercaseString(rowNode->name);
		    print("%s\n", s);
		    xfree(s);
		}
		/* TODO: print non-local name qualified */
	    } else if (smiNode->nodekind == SMI_NODEKIND_ROW) {
		if (smiType) {
		    print("%s\n", smiType->name);
		} else {
		    char *s = getUppercaseString(smiNode->name);
		    /* guess type name is uppercase row name */
		    print("%s\n", s);
		    xfree(s);
		}
		/* TODO: print non-local name qualified */
	    } else if (smiType) {
		if (!smiType->name) {
		    /*
		     * an implicitly restricted type.
		     */
		    print("%s", getTypeString(smiType->basetype,
					      smiGetParentType(smiType)));
		    printSubtype(smiType, invalid);
		    print("\n");
		} else {
		    print("%s\n",
			  getTypeString(smiType->basetype, smiType));
		}
	    }
	}

	if (! assignement && smiNode->units) {
	    printSegment(INDENT, "UNITS", INDENTVALUE, smiv1 || invalid);
	    print("\"%s\"\n", smiNode->units);
	}
	
	if (! assignement && smiNode->access != SMI_ACCESS_UNKNOWN &&
	    smiNode->nodekind != SMI_NODEKIND_CAPABILITIES) {
	    if (smiv1) {
		printSegment(INDENT, "ACCESS", INDENTVALUE, invalid);
	    } else {
		printSegment(INDENT, "MAX-ACCESS", INDENTVALUE, 0);
	    }
	    print("%s\n", getAccessString(smiNode->access, create));
	}

	if (! assignement && smiNode->status != SMI_STATUS_UNKNOWN) {
	    printSegment(INDENT, "STATUS", INDENTVALUE, invalid);
	    print("%s\n", getStatusString(smiNode->status));
	}
	
	if (! assignement) {
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE, invalid);
	    print("\n");
	    if (smiNode->description) {
		printMultilineString(smiNode->description, invalid);
	    } else {
		printMultilineString("...", invalid);
	    }
	    print("\n");
	}
	    
	if (! assignement && smiNode->reference) {
	    printSegment(INDENT, "REFERENCE", INDENTVALUE, smiv1 || invalid);
	    print("\n");
	    printMultilineString(smiNode->reference, smiv1 || invalid);
	    print("\n");
	}

	relatedNode = smiGetRelatedNode(smiNode);
	switch (smiNode->indexkind) {
	case SMI_INDEX_INDEX:
	case SMI_INDEX_REORDER:
	    printIndex(smiNode, invalid);
	    break;
	case SMI_INDEX_EXPAND:  /* TODO: we have to do more work here! */
	    printIndex(smiNode, invalid);
	    break;
	case SMI_INDEX_AUGMENT:
	    if (smiv1 && ! invalid) {
		if (relatedNode) {
		    printIndex(relatedNode, invalid);
		}
	    }
	    if ((! smiv1 || ! silent) && relatedNode) {
		printSegment(INDENT, "AUGMENTS", INDENTVALUE, smiv1 || invalid);
		print("{ %s }\n", relatedNode->name);
	    }
	    break;
	case SMI_INDEX_SPARSE:
	    if (! invalid) {
		if (relatedNode) {
		    printIndex(relatedNode, invalid);
		}
	    }
	    /* TODO: non-local name if non-local */
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;
	}
	
	if (smiNode->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    printSegment(INDENT, "DEFVAL", INDENTVALUE, invalid);
	    print("{ %s }", getValueString(&smiNode->value, smiType));
	    print("\n");
	}

	printSegment(INDENT, "::= ", 0, invalid);
	print("{ %s }\n\n", getOidString(smiNode, 0));

	smiType = smiGetNodeType(smiNode);
	if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    if (smiType) {
		print("%s ::=\n", smiType->name);
	    } else {
		/* guess type name is uppercase row name */
		char *s = getUppercaseString(smiNode->name);
		print("%s ::=\n", s);
		xfree(s);
	    }
	    /* Find the last valid node in this sequence. Wen need it
	     * to suppress its trailing camma. */
	    for(i = 0, invalid = 0, colNode = smiGetFirstChildNode(smiNode);
		colNode;
		colNode = smiGetNextChildNode(colNode)) {
		smiType = smiGetNodeType(colNode);
		if (!invalidType(smiType->basetype)) {
		    relatedNode = colNode;
		}
	    }
	    if (relatedNode) relatedNode = smiGetNextChildNode(relatedNode);
	    /* TODO: non-local name? */
	    printSegment(INDENT, "SEQUENCE {", 0, 0);
	    for(i = 0, invalid = 0, colNode = smiGetFirstChildNode(smiNode);
		colNode;
		colNode = smiGetNextChildNode(colNode)) {
		if (! invalid || ! silent) {
		    if (i && (relatedNode != colNode)) {
			print(",");
		    }
		    print("\n");
		}
		
		smiType = smiGetNodeType(colNode);
		invalid = invalidType(smiType->basetype);

		if (! invalid || ! silent) {
		    printSegment(2 * INDENT, colNode->name, INDENTSEQUENCE,
				 invalid);
		    if (smiType && smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
			print("%s", getTypeString(smiType->basetype,
						  smiGetParentType(smiType)));
		    } else {
			print("%s",
			      getTypeString(smiType->basetype,
					    smiGetNodeType(colNode)));
		    }
		}
		i++;
	    }
	    print("\n");
	    printSegment(INDENT, "}\n", 0, 0);
	    print("\n");
	}
    }
}



static void printNotifications(SmiModule *smiModule)
{
    SmiNode	 *smiNode, *parentNode;
    SmiElement   *smiElement;
    int          j;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	if (smiv1) {
	    print("%s TRAP-TYPE\n", smiNode->name);
	    parentNode = smiGetParentNode(smiNode);
	    while (!parentNode->name) {
		parentNode = smiGetParentNode(parentNode);
	    }
	    printSegment(INDENT, "ENTERPRISE", INDENTVALUE, 0);
	    print("%s\n", parentNode->name);
	} else {
	    print("%s NOTIFICATION-TYPE\n", smiNode->name);
	}

	if ((smiElement = smiGetFirstElement(smiNode))) {
	    if (smiv1) {
		printSegment(INDENT, "VARIABLES", INDENTVALUE, 0);
	    } else {
		printSegment(INDENT, "OBJECTS", INDENTVALUE, 0);
	    }
	    print("{ ");
	    for (j = 0; smiElement;
		 j++, smiElement = smiGetNextElement(smiElement)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2,
			     smiGetElementNode(smiElement)->name, 0);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(" }\n");
	}

	if (! smiv1 || ! silent) {
	    printSegment(INDENT, "STATUS", INDENTVALUE, smiv1);
	    print("%s\n", getStatusString(smiNode->status));
	}

	printSegment(INDENT, "DESCRIPTION", INDENTVALUE, 0);
	print("\n");
	if (smiNode->description) {
	    printMultilineString(smiNode->description, 0);
	} else {
	    printMultilineString("...", 0);
	}
	print("\n");
	    
	if (smiNode->reference) {
	    printSegment(INDENT, "REFERENCE", INDENTVALUE, smiv1);
	    print("\n");
	    printMultilineString(smiNode->reference, smiv1);
	    print("\n");
	}

	printSegment(INDENT, "::= ", 0, 0);
	if (smiv1) {
	    print("%d\n\n", smiNode->oid[smiNode->oidlen-1]);
	} else {
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}
    }
}



static void printGroups(SmiModule *smiModule)
{
    SmiNode	 *smiNode;
    SmiElement   *smiElement;
    int          j, objectGroup = 0, notificationGroup = 0;
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_GROUP);
	 smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {

	objectGroup = isObjectGroup(smiNode);
	if (! objectGroup) {
	    notificationGroup = isNotificationGroup(smiNode);
	}

	if (smiv1 || (!objectGroup && !notificationGroup)) {
	    print("%s OBJECT IDENTIFIER\n", smiNode->name);
	    printSegment(INDENT, "::= ", 0, 0);
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}

	if (!objectGroup && !notificationGroup) {
	    continue;
	}

	if (! smiv1 || ! silent) {

	    if (smiv1) {
		print("-- %s %s\n", smiNode->name,
		      objectGroup ? "OBJECT-GROUP" : "NOTIFICATION-GROUP");
	    } else {
		print("%s %s\n", smiNode->name,
		      objectGroup ? "OBJECT-GROUP" : "NOTIFICATION-GROUP");
	    }

	    if (objectGroup) {
		printSegment(INDENT, "OBJECTS", INDENTVALUE, smiv1);
	    } else {
		printSegment(INDENT, "NOTIFICATIONS", INDENTVALUE, smiv1);
	    }
	    print("{ ");
	    for (j = 0, smiElement = smiGetFirstElement(smiNode);
		 smiElement;
		 j++, smiElement = smiGetNextElement(smiElement)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2,
			     smiGetElementNode(smiElement)->name, smiv1);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(" }\n");
	    
	    printSegment(INDENT, "STATUS", INDENTVALUE, smiv1);
	    print("%s\n", getStatusString(smiNode->status));
	    
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE, smiv1);
	    print("\n");
	    if (smiNode->description) {
		printMultilineString(smiNode->description, smiv1);
	    } else {
		printMultilineString("...", smiv1);
	    }
	    print("\n");
	    
	    if (smiNode->reference) {
		printSegment(INDENT, "REFERENCE", INDENTVALUE, smiv1);
		print("\n");
		printMultilineString(smiNode->reference, smiv1);
		print("\n");
	    }
	    
	    printSegment(INDENT, "::= ", 0, smiv1);
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}
    }
}



static void printModuleCompliances(SmiModule *smiModule)
{
    SmiNode	  *smiNode, *smiNode2;
    SmiModule     *smiModule2;
    SmiType	  *smiType;
    SmiOption	  *smiOption;
    SmiRefinement *smiRefinement;
    SmiElement    *smiElement;
    char	  *module;
    char	  *done = NULL; /* "+" separated list of module names */
    char	  s[100];
    int		  j;

    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_COMPLIANCE);
	 smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	
	if (smiv1) {
	    print("%s OBJECT IDENTIFIER\n", smiNode->name);
	    printSegment(INDENT, "::= ", 0, 0);
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}

	if (! smiv1 || ! silent) {

	    if (smiv1) {
		print("-- %s MODULE-COMPLIANCE\n", smiNode->name);
	    } else {
		print("%s MODULE-COMPLIANCE\n", smiNode->name);
	    }
	    
	    printSegment(INDENT, "STATUS", INDENTVALUE, smiv1);
	    print("%s\n", getStatusString(smiNode->status));
	    
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE, smiv1);
	    print("\n");
	    if (smiNode->description) {
		printMultilineString(smiNode->description, smiv1);
	    } else {
		printMultilineString("...", smiv1);
	    }
	    print("\n");
	    
	    if (smiNode->reference) {
		printSegment(INDENT, "REFERENCE", INDENTVALUE, smiv1);
		print("\n");
		printMultilineString(smiNode->reference, smiv1);
		print("\n");
	    }
	    
	    /* `this module' always first */
	    done = xstrdup("+");
	    for (module = smiModule->name; module; ) {
		
		print("\n");
		printSegment(INDENT, "MODULE", INDENTVALUE, smiv1);
		if (strlen(module) && strcmp(smiModule->name, module)) {
		    print("%s\n", module);
		} else {
		    if (smiv1) {
			print("-- -- this module\n");
		    } else {
			print("-- this module\n");
		    }
		}
		
		for (j = 0, smiElement = smiGetFirstElement(smiNode);
		     smiElement;
		     smiElement = smiGetNextElement(smiElement)) {
		    if (!strcmp(smiGetNodeModule(smiGetElementNode(smiElement))->name, module)) {
			if (j) {
			    print(", ");
			} else {
			    print("\n");
			    printSegment(2 * INDENT,
					 "MANDATORY-GROUPS", INDENTVALUE, smiv1);
			    print("{ ");
			}
			j++;
			printWrapped(INDENTVALUE + 2,
				     smiGetElementNode(smiElement)->name,
				     smiv1);
		    }
		}
		if (j) {
		    print(" }\n");
		}
		
		for(smiOption = smiGetFirstOption(smiNode); smiOption;
		    smiOption = smiGetNextOption(smiOption)) {
		    smiNode2 = smiGetOptionNode(smiOption);
		    smiModule2 = smiGetNodeModule(smiNode2);
		    if (!strcmp(smiModule2->name, module)) {
			print("\n");
			printSegment(2 * INDENT, "GROUP", INDENTVALUE, smiv1);
			print("%s\n", smiNode2->name);
			printSegment(2 * INDENT, "DESCRIPTION", INDENTVALUE, smiv1);
			print("\n");
			if (smiOption->description) {
			    printMultilineString(smiOption->description, smiv1);
			} else {
			    printMultilineString("...", smiv1);
			}
			print("\n");
		    }
		}
		
		for(smiRefinement = smiGetFirstRefinement(smiNode);
		    smiRefinement;
		    smiRefinement = smiGetNextRefinement(smiRefinement)) {
		    smiNode2 = smiGetRefinementNode(smiRefinement);
		    smiModule2 = smiGetNodeModule(smiNode2);
		    if (!strcmp(smiModule2->name, module)) {
			print("\n");
			printSegment(2 * INDENT, "OBJECT", INDENTVALUE, smiv1);
			print("%s\n", smiNode2->name);

			smiType = smiGetRefinementType(smiRefinement);
			if (smiType) {
			    printSegment(2 * INDENT, "SYNTAX", INDENTVALUE,
					 smiv1);
			    print("%s",
				  getTypeString(smiType->basetype,
						smiGetParentType(smiType)));
			    printSubtype(smiType, smiv1);
			    print("\n");
			}
			
			smiType = smiGetRefinementWriteType(smiRefinement);
			if (smiType) {
			    printSegment(2 * INDENT, "WRITE-SYNTAX",
					 INDENTVALUE, smiv1);
			    print("%s",
				  getTypeString(smiType->basetype,
						smiGetParentType(smiType)));
			    printSubtype(smiType, smiv1);
			    print("\n");
			}
			
			if (smiRefinement->access != SMI_ACCESS_UNKNOWN) {
			    printSegment(2 * INDENT, "MIN-ACCESS", INDENTVALUE,
					 smiv1);
			    print("%s\n",
				  getAccessString(smiRefinement->access, 0));
			    /* we assume, that read-create does not appear in
			     * an OT refinement.
			     */
			}
			printSegment(2 * INDENT, "DESCRIPTION", INDENTVALUE,
				     smiv1);
			print("\n");
			if (smiRefinement->description) {
			    printMultilineString(smiRefinement->description, smiv1);
			} else {
			    printMultilineString("...", smiv1);
			}
			print("\n");
		    }
		}
		
		/*
		 * search the next module name in the list of mandatory
		 * groups, optional groups and refinements.
		 */
		done = xrealloc(done,
				strlen(done)+strlen(module)+2*sizeof(char));
		strcat(done, module);
		strcat(done, "+");
		module = NULL;
		for (smiElement = smiGetFirstElement(smiNode);
		     smiElement;
		     smiElement = smiGetNextElement(smiElement)) {
		    sprintf(s, "+%s+",
			smiGetNodeModule(smiGetElementNode(smiElement))->name);
		    if ((!strstr(done, s))) {
			module =
			 smiGetNodeModule(smiGetElementNode(smiElement))->name;
			break;
		    }
		}
		if (!module) {
		    ; /* TODO: search in options list */
		}
		if (!module) {
		    ; /* TODO: search in refinements list */
		}
	    }

	    print("\n");
	    printSegment(INDENT, "::= ", 0, smiv1);
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}
    }
    xfree(done);
}



static int dumpSmi(char *modulename)
{
    SmiModule	 *smiModule;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    createImportList(smiModule);
    
    print("--\n");
    print("-- This %s module has been generated by smidump "
	  VERSION ". Do not edit.\n", smiv1 ? "SMIv1" : "SMIv2");
    print("--\n\n");
    print("%s DEFINITIONS ::= BEGIN\n\n", smiModule->name);
	
    printImports(modulename);
    printModuleIdentity(smiModule);
    printTypeDefinitions(smiModule);
    printTextualConventions(smiModule);
    printObjects(smiModule);
    printNotifications(smiModule);
    printGroups(smiModule);
    printModuleCompliances(smiModule);
    
    print("END -- end of module %s.\n", modulename);

    freeImportList();

    return 0;
}


int dumpSmiV1(char *modulename, int flags)
{
    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for SMIv1 format\n");
	exit(1);
    }
    
    smiv1 = 1;
    silent = (flags & SMIDUMP_FLAG_SILENT);
    return dumpSmi(modulename);
}


int dumpSmiV2(char *modulename, int flags)
{
    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for SMIv2 format\n");
	exit(1);
    }
    
    smiv1 = 0;
    silent = (flags & SMIDUMP_FLAG_SILENT);
    return dumpSmi(modulename);
}
