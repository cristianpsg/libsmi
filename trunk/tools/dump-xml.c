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
 * @(#) $Id: dump-xml.c,v 1.3 2000/03/29 16:09:09 strauss Exp $
 */

/*
 * TODO:
 *
 * - index statements
 * - compliance statement
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



typedef struct XmlEscape {
    char character;
    char *escape;
} XmlEscape;

static XmlEscape xmlEscapes [] = {
    { '<',	"&lt;" },
    { '>',	"&gt;" },
    { '&',	"&amp" },
    { 0,	NULL }
};



static char *convertType[] = {
    "INTEGER",             "Integer32",
    "OCTET STRING",        "OctetString",
    "OBJECT IDENTIFIER",   "ObjectIdentifier",
    
    "Gauge",               "Gauge32",
    "Counter",             "Counter32",
    "NetworkAddress",      "IpAddress", /* ??? */
    
    NULL, NULL };


static int current_column = 0;



static char *getStringLanguage(SmiLanguage lang)
{
    return
	(lang == SMI_LANGUAGE_SMIV1) ? "SMIv1" :
	(lang == SMI_LANGUAGE_SMIV2) ? "SMIv2" :
	(lang == SMI_LANGUAGE_SMING) ? "SMIng" :
                                     	NULL;
}



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
	return getStringBasetype(basetype);
    }
    
    /* TODO: fully qualified if unambigous */

    return type_name;
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
	sprintf(append, ".%u%s", parentNode->oid[parentNode->oidlen-1], s);

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
    int i, j, len;

#ifdef INDENTTEXTS
    printSegment(column + INDENTTEXTS, "", 0);
#endif
    if (s) {
	len = strlen(s);
	for (i=0; i < len; i++) {
	    for (j = 0; xmlEscapes[j].character; j++) {
		if (xmlEscapes[j].character == s[i]) break;
	    }
	    if (xmlEscapes[j].character) {
		fputs(xmlEscapes[j].escape, stdout);
		current_column += strlen(xmlEscapes[j].escape);
	    } else {
		putc(s[i], stdout);
		current_column++;
	    }
	    if (s[i] == '\n') {
		current_column = 0;
#ifdef INDENTTEXTS
		printSegment(column + INDENTTEXTS, "", 0);
#endif
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



static void printNodeStartTag(int indent, const char *tag, SmiNode *smiNode)
{
    int i;
    
    printSegment(indent, "", 0);
    print("<smi:%s name=\"%s\"", tag, smiNode->name);
    print(" oid=\"");
    for (i = 0; i < smiNode->oidlen; i++) {
	print(i ? ".%u" : "%u", smiNode->oid[i]);
    }
    print("\"");
    if (smiNode->status != SMI_STATUS_CURRENT) {
	print(" status=\"%s\"", getStringStatus(smiNode->status));
    }
    print(">\n");
}



static void printNodeEndTag(int indent, const char *tag, SmiNode *smiNode)
{
    printSegment(2 * INDENT, "", 0);
    print("</smi:%s>\n", tag);
}



static void printRanges(int indent, SmiType *smiType)
{
    SmiRange       *range;

    for(range = smiGetFirstRange(smiType);
	range;
	range = smiGetNextRange(range)) {
	printSegment(indent, "<smi:range", 0);
	print(" min=\"%s\"", getValueString(&range->minValue, smiType));
	print(" max=\"%s\"", getValueString(&range->maxValue, smiType));
	print("/>\n");
    }
}



static void printNamedNumbers(int indent, SmiType *smiType)
{
    SmiNamedNumber *nn;

    if ((smiType->basetype != SMI_BASETYPE_ENUM) &&
	(smiType->basetype != SMI_BASETYPE_BITS)) {
	return;
    }

    for (nn = smiGetFirstNamedNumber(smiType);
	 nn;
	 nn = smiGetNextNamedNumber(nn)) {
	printSegment(indent, "<smi:namednumber", 0);
	print(" name=\"%s\"", nn->name);
	print(" number=\"%s\"", getValueString(&nn->value, smiType));
	print("/>\n");
    }
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
	printSegment(indent, "<smi:description>\n", 0);
	printMultilineString(indent, description);
	print("\n");
	printSegment(indent, "</smi:description>\n", 0);
    }
}



static void printReference(int indent, const char *reference)
{
    if (reference) {
	printSegment(indent, "<smi:reference>\n", 0);
	printMultilineString(indent, reference);
	print("\n");
	printSegment(indent, "</smi:reference>\n", 0);
    }
}



static void printFormat(int indent, const char *format)
{
    if (format) {
	printSegment(indent, "", 0);
	print ("<smi:format>%s</smi:format>\n", format);
    }
}



static void printUnits(int indent, const char *units)
{
    if (units) {
	printSegment(indent, "", 0);
	print ("<smi:units>%s</smi:units>\n", units);
    }
}



static void printElementList(int indent, const char *tag,
			     SmiElement *smiElement)
{
    SmiModule *smiModule;
    SmiNode   *smiNode;

    for (; smiElement; smiElement = smiGetNextElement(smiElement)) {
	smiNode = smiGetElementNode(smiElement);
	smiModule = smiGetNodeModule(smiNode);
	printSegment(indent, "", 0);
	print("<smi:%s module=\"%s\" name=\"%s\"/>\n",
	      tag, smiModule->name, smiNode->name);
    }
}



static void printOid(int indent, SmiNode *smiNode)
{
    if (smiNode->oid) {
	printSegment(indent, "", 0);
	print("<smi:oid>%s</smi:oid>\n", getOidString(smiNode, 0));
    }
}



static void printModule(SmiModule *smiModule)
{
    SmiRevision *smiRevision;
    SmiNode     *smiNode;
    int         i;
    char	*lang;

    lang = getStringLanguage(smiModule->language);

    printSegment(INDENT, "", 0);
    if (lang) {
	print("<smi:module name=\"%s\" language=\"%s\">\n",
	      smiModule->name, lang);
    } else {
	print("<smi:module name=\"%s\">\n", smiModule->name);
    }

    if (smiModule->organization) {
	printSegment(2 * INDENT, "<smi:organization>", INDENTVALUE);
	print("\n");
	printMultilineString(2 * INDENT, smiModule->organization);
	print("\n");
	printSegment(2 * INDENT, "</smi:organization>\n", 0);
    }

    if (smiModule->contactinfo) {
	printSegment(2 * INDENT, "<smi:contact>", INDENTVALUE);
	print("\n");
	printMultilineString(2 * INDENT, smiModule->contactinfo);
	print("\n");
	printSegment(2 * INDENT, "</smi:contact>\n", 0);
    }
    printDescription(2 * INDENT, smiModule->description);
    printReference(2 * INDENT, smiModule->reference);

    for(i = 0, smiRevision = smiGetFirstRevision(smiModule);
	smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	printSegment(2 * INDENT, "", 0);
	print("<smi:revision date=\"%s\">\n",
	      getTimeString(smiRevision->date));
	printDescription(3 * INDENT, smiRevision->description);
        printSegment(2 * INDENT, "</smi:revision>\n", 0);
	i++;
    }

    smiNode = smiGetModuleIdentityNode(smiModule);
    if (smiNode) {
	printSegment(2 * INDENT, "", 0);
	print("<smi:identity node=\"%s\"/>\n", smiNode->name);
    }

    printSegment(INDENT, "</smi:module>\n\n", 0);
}



static void printImport(SmiImport *smiImport)
{
    printSegment(2 * INDENT, "", 0);
    print("<smi:import module=\"%s\" name=\"%s\"/>\n",
	  smiImport->module, smiImport->name);
}


    
static void printImports(SmiModule *smiModule)
{
    SmiImport *smiImport;
    int        i;

    for (i = 0, smiImport = smiGetFirstImport(smiModule);
	 smiImport;
	 i++, smiImport = smiGetNextImport(smiImport)) {
	if (i == 0) {
	    printSegment(INDENT, "<smi:imports>\n", 0);
	}
	printImport(smiImport);
    }

    if (i) {
	printSegment(INDENT, "</smi:imports>\n\n", 0);
    }
}



static void printTypedef(SmiModule *smiModule, SmiType *smiType)
{
    printSegment(2 * INDENT, "", 0);
    print("<smi:typedef name=\"%s\" basetype=\"%s\"", smiType->name,
	  getStringBasetype(smiType->basetype));
    if (smiType->status != SMI_STATUS_CURRENT) {
	print(" status=\"%s\"", getStringStatus(smiType->status));
    }
    print(">\n");
    
    printSegment(3 * INDENT, "<smi:parenttype>", 0);
    print("%s", getTypeString(smiModule->name, smiType->basetype,
			      smiGetParentType(smiType)));
    print("</smi:parenttype>\n");
    printRanges(3 * INDENT, smiType);
    printNamedNumbers(3 * INDENT, smiType);
    
    if (smiType->value.basetype != SMI_BASETYPE_UNKNOWN) {
	printSegment(3 * INDENT, "<default>", 0);
	print("%s", getValueString(&smiType->value, smiType));
	print("</default>\n");
    }
    
    printFormat(3 * INDENT, smiType->format);
    printUnits(3 * INDENT, smiType->units);
    printDescription(3 * INDENT, smiType->description);
    printReference(3 * INDENT, smiType->reference);
    
    printSegment(2 * INDENT, "</smi:typedef>\n", 0);
}



static void printTypedefs(SmiModule *smiModule)
{
    int		 i;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(smiModule);
	smiType;
	i++, smiType = smiGetNextType(smiType)) {

	if (i == 0) {
	    printSegment(INDENT, "<smi:typedefs>\n", 0);
	}
	printTypedef(smiModule, smiType);
    }

    if (i) {
	printSegment(INDENT, "</smi:typedefs>\n\n", 0);
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
    char	 *s[3] = { NULL, NULL, NULL };
    SmiNodekind  nodekinds;

    nodekinds =  SMI_NODEKIND_NODE | SMI_NODEKIND_TABLE |
	SMI_NODEKIND_ROW | SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR |
	SMI_NODEKIND_CAPABILITIES;
    
    for(i = 0, smiNode = smiGetFirstNode(smiModule, nodekinds);
	smiNode; smiNode = smiGetNextNode(smiNode, nodekinds)) {

	if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	    indent = 0;
	} else if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	    indent = 0;
	} else if (smiNode->nodekind == SMI_NODEKIND_TABLE) {
	    indent = 0;
	} else if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    indent = 1;
	} else if (smiNode->nodekind == SMI_NODEKIND_COLUMN) {
	    indent = 2;
	} else if (smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	    indent = 0;
	}

	for (j = lastindent; j >= indent; j--) {
	    printSegment((1 + j) * INDENT, "", 0);
	    print("</smi:%s>\n", s[j]);
	}
	print("\n");
	lastindent = indent;
	
	if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	    s[indent] = "node";
	} else if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	    s[indent] = "node";
	} else if (smiNode->nodekind == SMI_NODEKIND_TABLE) {
	    s[indent] = "table";
	} else if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    s[indent] = "row";
	} else if (smiNode->nodekind == SMI_NODEKIND_COLUMN) {
	    s[indent] = "column";
	} else if (smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	    s[indent] = "scalar";
	}

	if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	    printSegment((1 + indent) * INDENT, "", 0);
	    print("<!-- This has been an SMIv2 AGENT-CAPABILITIES node: -->\n");
	}
	
	printSegment((1 + indent) * INDENT, "", 0);
	print("<smi:%s name=\"%s\">\n", s[indent], smiNode->name);

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
		print("</type>\n");
		printRanges((2 + indent) * INDENT, smiType);
		printNamedNumbers((2 + indent) * INDENT, smiType);
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
	print("</smi:%s>\n\n", s[indent]);
    }
}



static void printNotification(SmiNode *smiNode)
{
    printNodeStartTag(2 * INDENT, "notification", smiNode);

    printSegment(3 * INDENT, "<smi:objects>\n", 0);
    printElementList(4 * INDENT, "object", smiGetFirstElement(smiNode));
    printSegment(3 * INDENT, "</smi:objects>\n", 0);
    printDescription(3 * INDENT, smiNode->description);
    printReference(3 * INDENT, smiNode->reference);
    
    printNodeEndTag(2 * INDENT, "notification", smiNode);
}



static void printNotifications(SmiModule *smiModule)
{
    SmiNode *smiNode;
    int	     i;
    
    for(i = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	smiNode;
	i++, smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	if (i == 0) {
	    printSegment(INDENT, "<smi:notifications>\n", 0);
	}
	printNotification(smiNode);
    }

    if (i) {
	printSegment(INDENT, "</smi:notifications>\n\n", 0);
    }
}



static void printGroup(SmiNode *smiNode)
{
    printNodeStartTag(2 * INDENT, "group", smiNode);
    
    printSegment(3 * INDENT, "<smi:members>\n", 0);
    printElementList(4 * INDENT, "member", smiGetFirstElement(smiNode));
    printSegment(3 * INDENT, "</smi:members>\n", 0);
    printDescription(3 * INDENT, smiNode->description);
    printReference(3 * INDENT, smiNode->reference);

    printNodeEndTag(2 * INDENT, "group", smiNode);
}



static void printGroups(SmiModule *smiModule)
{
    SmiNode *smiNode;
    int	     i;
    
    for(i = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_GROUP);
	smiNode;
	i++, smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {

	if (i == 0) {
	    printSegment(INDENT, "<smi:groups>\n", 0);
	}
	printGroup(smiNode);
    }

    if (i) {
	printSegment(INDENT, "</smi:groups>\n\n", 0);
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
    
    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for XML format\n");
	exit(1);
    }

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    print("<?xml version=\"1.0\"?>\n");
    print("<!DOCTYPE smi:smi SYSTEM \"smi.dtd\">\n");
    print("\n");
    print("<!-- This module has been generated by smidump "
	  VERSION ". Do not edit. -->\n");
    print("\n");

    print("<smi:smi>\n\n");
    
    printModule(smiModule);
    printImports(smiModule);
    printTypedefs(smiModule);
    printObjects(smiModule);
    printNotifications(smiModule);
    printGroups(smiModule);
    printCompliances(smiModule);

    print("</smi:smi>\n");
    return 0;
}
