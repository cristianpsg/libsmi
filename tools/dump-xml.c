/*
 * dump-xml.c --
 *
 *      Operations to dump SMIng module information in XML format.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-xml.c,v 1.1 2000/03/21 10:30:40 strauss Exp $
 */

/*
 * TODO:
 *
 * - index statement
 * - compliance statement
 * - quoted strings
 * - named numbers?
 * - range and size restrictions?
 * - node end tags
 * - define a real DTD
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



#define  INDENT		2    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	4    /* column to start multiline texts */
#define  INDENTMAX	64   /* max column to fill, break lines otherwise */

#define  STYLE_IMPORTS  2



static char *excludeType[] = {
    "ObjectSyntax",
    "SimpleSyntax",
    "ApplicationSyntax",
    NULL };
    
static char *convertType[] = {
    "INTEGER",             "Integer32",
    "OCTET STRING",        "OctetString",
    "OBJECT IDENTIFIER",   "ObjectIdentifier",
    
    "Gauge",               "Gauge32",
    "Counter",             "Counter32",
    "NetworkAddress",      "IpAddress", /* ??? */
    
    NULL, NULL };

static char *convertImport[] = {
    "SNMPv2-SMI",   "MODULE-IDENTITY",    NULL, NULL,
    "SNMPv2-SMI",   "OBJECT-IDENTITY",    NULL, NULL,
    "SNMPv2-SMI",   "OBJECT-TYPE",        NULL, NULL,
    "SNMPv2-SMI",   "NOTIFICATION-TYPE",  NULL, NULL,
    "SNMPv2-SMI",   "ObjectName",         NULL, NULL,
    "SNMPv2-SMI",   "NotificationName",   NULL, NULL,
    "SNMPv2-SMI",   "ObjectSyntax",       NULL, NULL,
    "SNMPv2-SMI",   "SimpleSyntax",       NULL, NULL,
    "SNMPv2-SMI",   "Integer32",          NULL, NULL,
    "SNMPv2-SMI",   "Unsigned32",         NULL, NULL,
    "SNMPv2-SMI",   "ApplicationSyntax",  NULL, NULL,
    "SNMPv2-SMI",   "IpAddress",          "IRTF-NMRG-SMING-TYPES", "IpAddress",
    "SNMPv2-SMI",   "Counter32",          "IRTF-NMRG-SMING-TYPES", "Counter32",
    "SNMPv2-SMI",   "Gauge32",            "IRTF-NMRG-SMING-TYPES", "Gauge32",
    "SNMPv2-SMI",   "TimeTicks",          "IRTF-NMRG-SMING-TYPES", "TimeTicks",
    "SNMPv2-SMI",   "Opaque",             "IRTF-NMRG-SMING-TYPES", "Opaque",
    "SNMPv2-SMI",   "Counter64",          "IRTF-NMRG-SMING-TYPES", "Counter64",
    "SNMPv2-SMI",   NULL,                 "IRTF-NMRG-SMING", NULL,
    "SNMPv2-TC",    "TEXTUAL-CONVENTION", NULL, NULL,
    "SNMPv2-TC",    NULL,                 "IRTF-NMRG-SMING-TYPES", NULL,
    "SNMPv2-CONF",  "OBJECT-GROUP",       NULL, NULL,
    "SNMPv2-CONF",  "NOTIFICATION-GROUP", NULL, NULL,
    "SNMPv2-CONF",  "MODULE-COMPLIANCE",  NULL, NULL,
    "SNMPv2-CONF",  "AGENT-CAPABILITIES", NULL, NULL,
 
    "RFC1155-SMI",  "OBJECT-TYPE",        NULL, NULL,
    "RFC1155-SMI",  "ObjectName",         NULL, NULL,
    "RFC1155-SMI",  "ObjectSyntax",       NULL, NULL,
    "RFC1155-SMI",  "SimpleSyntax",       NULL, NULL,
    "RFC1155-SMI",  "ApplicationSyntax",  NULL, NULL,
    "RFC1155-SMI",  "Gauge",              "IRTF-NMRG-SMING-TYPES", "Gauge32",
    "RFC1155-SMI",  "Counter",            "IRTF-NMRG-SMING-TYPES", "Counter32",
    "RFC1155-SMI",  "TimeTicks",          "IRTF-NMRG-SMING-TYPES", "TimeTicks",
    "RFC1155-SMI",  "IpAddress",          "IRTF-NMRG-SMING-TYPES", "IpAddress",
    "RFC1155-SMI",  "NetworkAddress",     NULL, NULL, /* ??? */
    "RFC1155-SMI",  "Opaque",             "IRTF-NMRG-SMING-TYPES", "Opaque",
    "RFC1155-SMI",  NULL,                 "IRTF-NMRG-SMING", NULL,
    "RFC1158-MIB",  "DisplayString",  "IRTF-NMRG-SMING-TYPES", "DisplayString",
    "RFC-1212",     "OBJECT-TYPE",        NULL, NULL,
    "RFC1213-MIB",  "mib-2",              "IRTF-NMRG-SMING", "mib-2",
    "RFC1213-MIB",  "system",             "SNMPv2-MIB", "system",
    "RFC1213-MIB",  "interfaces",         "IF-MIB", "interfaces",
/*  "RFC1213-MIB",  "at",                 "RFC1213-MIB", "at", */
    "RFC1213-MIB",  "ip",                 "IP-MIB", "ip",
    "RFC1213-MIB",  "icmp",               "IP-MIB", "icmp",
    "RFC1213-MIB",  "tcp",                "TCP-MIB", "tcp",
    "RFC1213-MIB",  "udp",                "UDP-MIB", "udp",
/*  "RFC1213-MIB",  "egp",                "RFC1213-MIB", "egp", */
    "RFC1213-MIB",  "transmission",       "SNMPv2-SMI", "transmission",
    "RFC1213-MIB",  "snmp",               "SNMPv2-MIB", "snmp",
    "RFC1213-MIB",  "sysDescr",           "SNMPv2-MIB", "sysDescr",
    "RFC1213-MIB",  "sysObjectID",        "SNMPv2-MIB", "sysObjectID",
    "RFC1213-MIB",  "sysUpTime",          "SNMPv2-MIB", "sysUpTime",
    "RFC1213-MIB",  "ifIndex",            "IF-MIB", "ifIndex",
/* TODO ...many more objects from RFC1213-MIB.. */    
    "RFC1213-MIB",  "DisplayString",  "IRTF-NMRG-SMING-TYPES", "DisplayString",
    "RFC1213-MIB",  "PhysAddress",    "IRTF-NMRG-SMING-TYPES", "PhysAddress",
    "RFC-1215",     "TRAP-TYPE",          NULL, NULL,                          



    
    /* TODO: how to convert more SMIv1 information? */

    NULL, NULL, NULL, NULL };

static int current_column = 0;



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



static char *smingStringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<UNKNOWN>" :
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



static char *getTimeString(time_t t)
{
    static char   s[27];
    struct tm	  *tm;

    tm = gmtime(&t);
    sprintf(s, "%04d-%02d-%02d %02d:%02d",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min);
    return s;
}



static char *getTypeString(char *module, SmiBasetype basetype,
			   SmiType *smiType)
{
    int         i;
    char        *typemodule, *type_name;

    type_name = smiType ? smiType->name : NULL;
    typemodule = smiType ? smiGetTypeModule(smiType)->name : NULL;
    
    if ((!typemodule) && (type_name) &&
	(basetype != SMI_BASETYPE_ENUM) &&
	(basetype != SMI_BASETYPE_BITS)) {
	for(i=0; convertType[i]; i += 2) {
	    if (!strcmp(type_name, convertType[i])) {
		return convertType[i+1];
	    }
	}
    }

    if ((!typemodule) || (!strlen(typemodule)) || (!type_name)) {
	if (basetype == SMI_BASETYPE_ENUM) {
	    return "Enumeration";
	}
	if (basetype == SMI_BASETYPE_BITS) {
	    return "Bits";
	}
    }
	
    if (!type_name) {
	return smingStringBasetype(basetype);
    }
    
    /* TODO: fully qualified if unambigous */

    return type_name;
}



static char *getOidString(SmiNode *smiNode, int importedParent)
{
    SmiNode	 *parentNode, *node;
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
	sprintf(append, ".%u%s", parentNode->oid[parentNode->oidlen-1], s);

	/* retrieve the parent SmiNode */
	node = parentNode;
	parentNode = smiGetParentNode(node);

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
	if (i) strcat(s, ".");
	sprintf(&s[strlen(s)], "%u", smiNode->oid[i]);
    }
    return s;
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



static void printWrapped(int column, char *string)
{
    if ((current_column + strlen(string)) > INDENTMAX) {
	putc('\n', stdout);
	current_column = 0;
	printSegment(column, "", 0);
    }
    print("%s", string);
}



static void printMultilineString(int column, const char *s)
{
    int i, len;
    
    printSegment(column + INDENTTEXTS, "", 0);
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    putc(s[i], stdout);
	    current_column++;
	    if (s[i] == '\n') {
		current_column = 0;
		printSegment(column + INDENTTEXTS, "", 0);
	    }
	}
    }
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



static void printSubtype(SmiType *smiType)
{
    SmiRange       *range;
    SmiNamedNumber *nn;
    char	   s[100];
    int		   i;

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (i) {
		print(", ");
	    } else {
		print(" (");
	    }
	    sprintf(s, "%s(%s)", nn->name,
		    getValueString(&nn->value, smiType));
	    printWrapped(INDENTVALUE + INDENT, s);
	}
	if (i) {
	    print(")");
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType);
	    range ; i++, range = smiGetNextRange(range)) {
	    if (i) {
		print(" | ");
	    } else {
		print(" (");
	    }	    
	    if (memcmp(&range->minValue, &range->maxValue,
		       sizeof(SmiValue))) {
		sprintf(s, "%s", getValueString(&range->minValue, smiType));
		sprintf(&s[strlen(s)], "..%s", 
			getValueString(&range->maxValue, smiType));
	    } else {
		sprintf(s, "%s", getValueString(&range->minValue, smiType));
	    }
	    printWrapped(INDENTVALUE + INDENT, s);
	}
	if (i) {
	    print(")");
	}
    }
}



static void printStatus(int indent, SmiStatus status)
{
    if ((status != SMI_STATUS_CURRENT) &&
	(status != SMI_STATUS_UNKNOWN) &&
	(status != SMI_STATUS_MANDATORY) &&
	(status != SMI_STATUS_OPTIONAL)) {
	printSegment(indent, "", 0);
	print ("<status>%s</status>\n", getStringStatus(status));
    }
}



static void printDescription(int indent, const char *description)
{
    if (description) {
	printSegment(indent, "<description>\n", 0);
	printMultilineString(indent, description);
	print("\n");
	printSegment(indent, "</description>\n", 0);
    }
}



static void printReference(int indent, const char *reference)
{
    if (reference) {
	printSegment(indent, "<reference>\n", 0);
	printMultilineString(indent, reference);
	print("\n");
	printSegment(indent, "</reference>\n", 0);
    }
}



static void printFormat(int indent, const char *format)
{
    if (format) {
	printSegment(indent, "", 0);
	print ("<format>%s</format>\n", format);
    }
}



static void printUnits(int indent, const char *units)
{
    if (units) {
	printSegment(indent, "", 0);
	print ("<units>%s</units>\n", units);
    }
}



static void printElementList(int indent, const char *tag,
			     SmiElement *smiElement)
{
    if (smiElement) {
	printSegment(indent, "", 0);
	print("<%s>\n", tag);
	while (smiElement) {
	    printSegment(indent, "", 0);
	    print("<identifier>%s</identifier>\n",
		  smiGetElementNode(smiElement)->name);
	    /* TODO: non-local name if non-local */
	    smiElement = smiGetNextElement(smiElement);
	} /* TODO: empty? -> print error */
	printSegment(2 * INDENT, "", 0);
	print("<%s>\n", tag);
    }
}



static void printOid(int indent, SmiNode *smiNode)
{
    if (smiNode->oid) {
	printSegment(indent, "", 0);
	print("<oid>%s</oid>\n", getOidString(smiNode, 0));
    }
}



static void printImports(SmiModule *smiModule)
{
    SmiImport     *smiImport;
    char	  *lastModulename = NULL;
    char	  *importedModulename, *importedDescriptor;
    int		  i;
    char	  types[200];
    char	  *t;
    
    types[0] = 0;

    /*
     * TODO:
     * - add imports for
     *   - external manadatory groups read from a SMIv2 module
     *   - external optional groups ...
     *   - external refined objects ...
     */
    
    for(smiImport = smiGetFirstImport(smiModule); smiImport;
	smiImport = smiGetNextImport(smiImport)) {
	importedModulename = smiImport->module;
	importedDescriptor = smiImport->name;

	if (!strlen(importedModulename))
	    continue;
	
	/*
	 * imported SMI modules have to be handled more carefully:
	 * The module name is mapped to an SMIng module and some definitions
	 * are stripped off (namely macros). Also note, that this may
	 * lead to an empty list of imported descriptors from that SMIv1/v2
	 * module.
	 */
	for(i = 0; convertImport[i]; i += 4) {
	    if (convertImport[i] && convertImport[i+1]
		&& !strcmp(importedModulename, convertImport[i])
		&& !strcmp(importedDescriptor, convertImport[i+1])) {
		importedModulename = convertImport[i+2];
		importedDescriptor = convertImport[i+3];
		/* TODO: hide duplicates */
		break;
	    } else if (convertImport[i] && !convertImport[i+1]
		       && !strcmp(importedModulename, convertImport[i])) {
		importedModulename = convertImport[i+2];
		/* TODO: hide duplicates */
		break;
	    }
	}

	if (importedModulename && importedDescriptor &&
	    strlen(importedDescriptor)) {
	    if ((!lastModulename) ||
		strcmp(importedModulename, lastModulename)) {
		if (lastModulename) {
		    printSegment(INDENT, "</import>\n\n", 0);
		    if (strlen(types)) {
			printSegment(INDENT, "", 0);
			print("import IRTF-NMRG-SMING-TYPES (");
			for (t = strtok(types, " "); t;
			     t = strtok(NULL, " ")) {
			    printWrapped(INDENTVALUE + INDENT, t);
			}
			printSegment(INDENT, "</import>\n\n", 0);
			types[0] = 0;
		    }
		}
		printSegment(INDENT, "", 0);
		print("<import from=\"%s\">\n", importedModulename);
		lastModulename = importedModulename;
	    }
	    printSegment(2 * INDENT, "", 0);
	    print("<identifier>%s</identifier>\n", importedDescriptor);
	}
    }
    if (lastModulename) {
	printSegment(INDENT, "</import>\n\n", 0);
    }
}



static void printRevisions(SmiModule *smiModule)
{
    int i;
    SmiRevision *smiRevision;
    
    for(i = 0, smiRevision = smiGetFirstRevision(smiModule);
	smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	printSegment(INDENT, "<revision>\n", 0);
	printSegment(2 * INDENT, "", 0);
	print("<date>%s</date>\n", getTimeString(smiRevision->date));
	printDescription(2 * INDENT, smiRevision->description);
        printSegment(INDENT, "</revision>\n\n", 0);
	i++;
    }
    if (i) {
	print("\n");
    }
}



static void printTypedefs(SmiModule *smiModule)
{
    int		 j;
    SmiType	 *smiType;
    
    for(smiType = smiGetFirstType(smiModule);
	smiType; smiType = smiGetNextType(smiType)) {
	
	if ((!(strcmp(smiModule->name, "SNMPv2-SMI"))) ||
	    (!(strcmp(smiModule->name, "RFC1155-SMI")))) {
	    for(j = 0; excludeType[j]; j++) {
		if (!strcmp(smiType->name, excludeType[j])) break;
	    }
	    if (excludeType[j]) break;
	}
	    
	printSegment(INDENT, "", 0);
	print("<typedef name=\"%s\">\n", smiType->name);

	printSegment(2 * INDENT, "<type>", 0);
	print("%s", getTypeString(smiModule->name, smiType->basetype,
				  smiGetParentType(smiType)));
	printSubtype(smiType);
	print("</type>\n");

	if (smiType->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    printSegment(2 * INDENT, "<default>", 0);
	    print("%s", getValueString(&smiType->value, smiType));
	    print("</default>\n");
	}
	
	printFormat(2 * INDENT, smiType->format);
	printUnits(2 * INDENT, smiType->units);
	printStatus(2 * INDENT, smiType->status);
	printDescription(2 * INDENT, smiType->description);
	printReference(2 * INDENT, smiType->reference);
	
	printSegment(INDENT, "</typedef>\n\n", 0);
    }
}



static void printObjects(SmiModule *smiModule)
{
    int		 i, j;
    SmiNode	 *smiNode, *relatedNode;
    SmiElement   *smiElement;
    SmiType	 *smiType;
    int		 indent = 0;
    int		 lastindent = -1;
    char	 *s = NULL;
    SmiNodekind  nodekinds;

    nodekinds =  SMI_NODEKIND_NODE | SMI_NODEKIND_TABLE |
	SMI_NODEKIND_ROW | SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR |
	SMI_NODEKIND_CAPABILITIES;
    
    for(i = 0, smiNode = smiGetFirstNode(smiModule, nodekinds);
	smiNode; smiNode = smiGetNextNode(smiNode, nodekinds)) {

	if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	    indent = 0;
	    s = "node";
	} else if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	    indent = 0;
	    s = "node";
	} else if (smiNode->nodekind == SMI_NODEKIND_TABLE) {
	    indent = 0;
	    s = "table";
	} else if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    indent = 1;
	    s = "row";
	} else if (smiNode->nodekind == SMI_NODEKIND_COLUMN) {
	    indent = 2;
	    s = "column";
	} else if (smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	    indent = 0;
	    s = "scalar";
	}

	for (j = lastindent; j >= indent; j--) {
	    printSegment((1 + j) * INDENT, "", 0);
	    print("};\n");
	}
	print("\n");
	lastindent = indent;
	
	if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	    printSegment((1 + indent) * INDENT, "", 0);
	    print("<!-- This has been an SMIv2 AGENT-CAPABILITIES node: -->\n");
	}
	
	printSegment((1 + indent) * INDENT, "", 0);
	print("<%s name=\"%s\">\n", s, smiNode->name);

	printOid((2 + indent) * INDENT, smiNode);

	smiType = smiGetNodeType(smiNode);
	if (smiType && (smiType->basetype != SMI_BASETYPE_UNKNOWN)) {
	    printSegment((2 + indent) * INDENT, "<type>", 0);
	    if (!smiType->name) {
		/*
		 * an implicitly restricted type.
		 */
		print("%s", getTypeString(smiModule->name, smiType->basetype,
					  smiGetParentType(smiType)));
		printSubtype(smiType);
		print("</type>\n");
	    } else {
		print("%s</type>\n",
		      getTypeString(smiModule->name, smiType->basetype,
				    smiType));
	    }
	}

	if ((smiNode->nodekind != SMI_NODEKIND_TABLE) &&
	    (smiNode->nodekind != SMI_NODEKIND_ROW) &&
	    (smiNode->nodekind != SMI_NODEKIND_CAPABILITIES) &&
	    (smiNode->nodekind != SMI_NODEKIND_NODE)) {
	    if (smiNode->access != SMI_ACCESS_UNKNOWN) {
		printSegment((2 + indent) * INDENT, "", 0);
		print("<access>%s</access>\n",
		      getAccessString(smiNode->access));
	    }
	}

	relatedNode = smiGetRelatedNode(smiNode);
	switch (smiNode->indexkind) {
	case SMI_INDEX_INDEX:
	    if (smiNode->implied) {
		printSegment((2 + indent) * INDENT, "index implied",
			     INDENTVALUE);
	    } else {
		printSegment((2 + indent) * INDENT, "index", INDENTVALUE);
	    }
	    print("(");
	    for (j = 0, smiElement = smiGetFirstElement(smiNode); smiElement;
		 j++, smiElement = smiGetNextElement(smiElement)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1,
			     smiGetElementNode(smiElement)->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(");\n");
	    break;
	case SMI_INDEX_AUGMENT:
	    if (relatedNode) {
		printSegment((2 + indent) * INDENT, "augments", INDENTVALUE);
		print("%s;\n", relatedNode->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_REORDER:
	    if (relatedNode) {
		printSegment((2 + indent) * INDENT, "", 0);
		print("reorders %s", relatedNode->name);
		/* TODO: non-local name if non-local */
		if (smiNode->implied) {
		    print(" implied");
		}
		print(" (");
		for (j = 0, smiElement = smiGetFirstElement(smiNode);
		     smiElement;
		     j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			print(", ");
		    }
		    printWrapped(INDENTVALUE + 1,
				 smiGetElementNode(smiElement)->name);
		    /* TODO: non-local name if non-local */
		} /* TODO: empty? -> print error */
		print(");\n");
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_SPARSE:
	    if (relatedNode) {
		printSegment((2 + indent) * INDENT, "sparse", INDENTVALUE);
		print("%s;\n", relatedNode->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_EXPAND:
	    if (relatedNode) {
		printSegment((2 + indent) * INDENT, "", 0);
		print("expands %s", relatedNode->name);
		/* TODO: non-local name if non-local */
		if (smiNode->implied) {
		    print(" implied");
		}
		print(" (");
		for (j = 0, smiElement = smiGetFirstElement(smiNode);
		     smiElement;
		     j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			print(", ");
		    }
		    printWrapped(INDENTVALUE + 1,
				 smiGetElementNode(smiElement)->name);
		    /* TODO: non-local name if non-local */
		} /* TODO: empty? -> print error */
		print(");\n");
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;
	}
	
	if (smiNode->create) {
	    printSegment((2 + indent) * INDENT, "<create/>\n", INDENTVALUE);
	    /* TODO: create list */
	}
	
	if (smiNode->value.basetype != SMI_BASETYPE_UNKNOWN) {
	    printSegment((2 + indent) * INDENT, "default", INDENTVALUE);
	    print("%s", getValueString(&smiNode->value, smiType));
	    print(";\n");
	}
	
	printFormat((2 + indent) * INDENT, smiNode->format);
	printUnits((2 + indent) * INDENT, smiNode->units);
	printStatus((2 + indent) * INDENT, smiNode->status);
	printDescription((2 + indent) * INDENT, smiNode->description);
	printReference((2 + indent) * INDENT, smiNode->reference);
	i++;
    }
    
    if (i) {
	printSegment((1 + indent) * INDENT, "", 0);
	print("};\n\n");
    }
}



static void printNotifications(SmiModule *smiModule)
{
    SmiNode	 *smiNode;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	printSegment(INDENT, "", 0);
	print("<notification name=\"%s\">\n", smiNode->name);

	printOid(2 * INDENT, smiNode);
	printElementList(2 * INDENT, "objects", smiGetFirstElement(smiNode));
	printStatus(2 * INDENT, smiNode->status);
	printDescription(2 * INDENT, smiNode->description);
	printReference(2 * INDENT, smiNode->reference);

	printSegment(INDENT, "</notification>\n\n", 0);
    }
}



static void printGroups(SmiModule *smiModule)
{
    SmiNode	 *smiNode;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_GROUP);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
	
	printSegment(INDENT, "", 0);
	print("<group name=\"%s\">\n", smiNode->name);
	
	printOid(2 * INDENT, smiNode);
	printElementList(2 * INDENT, "members", smiGetFirstElement(smiNode));
	printStatus(2 * INDENT, smiNode->status);
	printDescription(2 * INDENT, smiNode->description);
	printReference(2 * INDENT, smiNode->reference);
	
	printSegment(INDENT, "</group>\n\n", 0);
    }
}



static void printCompliances(SmiModule *smiModule)
{
    int		  i, j;
    SmiNode	  *smiNode, *smiNode2;
    SmiType	  *smiType;
    SmiOption	  *smiOption;
    SmiRefinement *smiRefinement;
    SmiElement    *smiElement;

    print("<!--\n\n");
    
    for(i = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_COMPLIANCE);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	
	if (!i) {
	    print("//\n// COMPLIANCE DEFINITIONS\n//\n\n");
	}

	printSegment(INDENT, "", 0);
	print("compliance %s {\n", smiNode->name);
	    
	if (smiNode->oid) {
	    printSegment(2 * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(smiNode, 0));
	}

	printStatus(2 * INDENT, smiNode->status);
	printDescription(2 * INDENT, smiNode->description);
	printReference(2 * INDENT, smiNode->reference);

	if ((smiElement = smiGetFirstElement(smiNode))) {
	    print("\n");
	    printSegment(2 * INDENT, "mandatory", INDENTVALUE);
	    print("(");
	    for (j = 0; smiElement;
		 j++, smiElement = smiGetNextElement(smiElement)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1,
			     smiGetElementNode(smiElement)->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(");\n");
	}
	
	if ((smiOption = smiGetFirstOption(smiNode))) {
	    print("\n");
	    for(; smiOption; smiOption = smiGetNextOption(smiOption)) {
		smiNode2 = smiGetOptionNode(smiOption);
		printSegment(2 * INDENT, "", 0);
		print("optional %s {\n", smiNode2->name);
		printSegment(3 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString(3 * INDENT, smiOption->description);
		print(";\n");
		printSegment(2 * INDENT, "};\n", 0);
	    }
	}
	
	if ((smiRefinement = smiGetFirstRefinement(smiNode))) {
	    print("\n");
	    for(; smiRefinement;
		smiRefinement = smiGetNextRefinement(smiRefinement)) {
		printSegment(2 * INDENT, "", 0);
		print("refine %s {\n",
		      smiGetRefinementNode(smiRefinement)->name);

		smiType = smiGetRefinementType(smiRefinement);
		if (smiType) {
		    printSegment(3 * INDENT, "type", INDENTVALUE);
		    print("%s",
			  getTypeString(smiModule->name, smiType->basetype,
					smiGetParentType(smiType)));
		    printSubtype(smiType);
		    print(";\n");
		}

		smiType = smiGetRefinementWriteType(smiRefinement);
		if (smiType) {
		    printSegment(3 * INDENT, "writetype", INDENTVALUE);
		    print("%s",
			  getTypeString(smiModule->name, smiType->basetype,
					smiGetParentType(smiType)));
		    printSubtype(smiType);
		    print(";\n");
		}

		if (smiRefinement->access != SMI_ACCESS_UNKNOWN) {
		    printSegment(3 * INDENT, "access", INDENTVALUE);
		    print("%s;\n", getAccessString(smiRefinement->access));
		}
		printSegment(3 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString(3 * INDENT, smiRefinement->description);
		print(";\n");
		printSegment(2 * INDENT, "};\n", 0);
	    }
	}
	
	printSegment(INDENT, "", 0);
	print("};\n\n");
	i++;
    }

    print("-->\n\n");
}



int dumpXml(char *modulename, int flags)
{
    SmiModule	 *smiModule;
    SmiNode	 *smiNode;
    
    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for SMIng format\n");
	exit(1);
    }

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    print("<?xml version=\"1.0\"?>\n");
    print("<!DOCTYPE sming SYSTEM \"sming.dtd\">\n");
    print("\n");
    print("<!-- This module has been generated by smidump "
	  VERSION ". Do not edit. -->\n");
    print("\n");
    print("<module name=\"%s\">", smiModule->name);
    print("\n\n");
    
    printImports(smiModule);
    
    /*
     * Module Header
     */
    
    printSegment(INDENT, "<organization>", INDENTVALUE);
    print("\n");
    printMultilineString(INDENT, smiModule->organization);
    print("\n");
    printSegment(INDENT, "</organization>\n\n", 0);
    printSegment(INDENT, "<contact>", INDENTVALUE);
    print("\n");
    printMultilineString(INDENT, smiModule->contactinfo);
    print("\n");
    printSegment(INDENT, "</contact>\n\n", 0);
    printDescription(INDENT, smiModule->description);
    printReference(INDENT, smiModule->reference);
    printRevisions(smiModule);

    smiNode = smiGetModuleIdentityNode(smiModule);
    if (smiNode) {
	printSegment(INDENT, "", 0);
	print("<identity node=\"%s\"/>\n\n", smiNode->name);
    }
    
    printTypedefs(smiModule);
    
    printObjects(smiModule);
    
    printNotifications(smiModule);
    
    printGroups(smiModule);
    
    printCompliances(smiModule);

    print("</module>\n");
    
    return 0;
}

