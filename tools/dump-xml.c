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
 * @(#) $Id: dump-xml.c,v 1.4 2000/03/30 09:29:02 strauss Exp $
 */

/*
 * TODO:
 *
 * - always print a full index list and in addition signal the table
 *   relationship (if any), e.g. whether it is an augments, sparse, ...
 *   (may affect SMIng?)
 * - should "sparse" not be changed to be a verb?
 *   (may affect SMIng?)
 * - mandatory, conditionally mandatory and optional groups in compliances
 *   are treated differently. Does this really make sense?
 *   (may affect SMIng?)
 * - index statements
 * - create statements
 * - value representations (e.g. Enumeration, Bits, Octetstring)
 * - compliance statements
 * - finish DTD and check against it
 */

#if 0
<smi:compliance>
  <smi:complgroups>
    <smi:mandatory ... />
    <smi:conditional ... />
    <smi:optional ... />
  </smi:complgroups>
  <smi:refinements>
  </smi:refinements
</smi:compliance>
#endif

#if 0
<xxx implied="true">
   <index module="xx" name="xx"/>
   <index module="xx" name="xx"/>

   <augments module="xx" name="xx"/>
   <sparse module="xx" name="xx"/>

   <reorders module="xx" name="xx"/>
     <index module="xx" name="xx"/>
     <index module="xx" name="xx"/>
   </reorders>

   <expands module="xx" name="xx"/>
     <index module="xx" name="xx"/>
     <index module="xx" name="xx"/>
   </expands>
</xxx>
#endif

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
    { '&',	"&amp;" },
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



static char *getTypeString(SmiBasetype basetype, SmiType *smiType)
{
    int         i;
    char        *typeModule, *typeName;

    typeName = smiType ? smiType->name : NULL;
    typeModule = smiType ? smiGetTypeModule(smiType)->name : NULL;
    
    if ((!typeModule) && (typeName) &&
	(basetype != SMI_BASETYPE_ENUM) &&
	(basetype != SMI_BASETYPE_BITS)) {
	for(i=0; convertType[i]; i += 2) {
	    if (!strcmp(typeName, convertType[i])) {
		return convertType[i+1];
	    }
	}
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
	for (i = 0; i < valuePtr->len; i++) {
	    sprintf(&s[strlen(s)], i ? ".%u" : "%u", valuePtr->value.oid[i]);
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
    if (smiNode->status != SMI_STATUS_UNKNOWN) {
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



static void printValue(int indent, SmiValue *smiValue, SmiType *smiType)
{
    if (smiType && smiValue && smiValue->basetype != SMI_BASETYPE_UNKNOWN) {
	printSegment(indent, "<default>", 0);
	print("%s", getValueString(smiValue, smiType));
	print("</default>\n");
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



static void printAccess(int indent, SmiAccess smiAccess)
{
    if (smiAccess != SMI_ACCESS_UNKNOWN) {
	printSegment(indent, "", 0);
	print("<smi:access>%s</smi:access>\n", getAccessString(smiAccess));
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



static void printIndex(int indent, SmiNode *smiNode)
{
    SmiNode   *relatedNode;
    SmiModule *relatedModule = NULL;

    printSegment(indent, "<xxxx", 0);
    if (smiNode->implied) {
	print(" implied=\"true\"");
    }
    print(">\n");

    relatedNode = smiGetRelatedNode(smiNode);
    if (relatedNode) {
	relatedModule = smiGetNodeModule(relatedNode);
    }
    switch (smiNode->indexkind) {
    case SMI_INDEX_INDEX:
	printElementList(indent + INDENT, "index",
			 smiGetFirstElement(smiNode));
	break;
    case SMI_INDEX_AUGMENT:
	if (relatedNode && relatedModule) {
	    printSegment(indent + INDENT, "", 0);
	    print("<augments module=\"%s\" name=\"%s\"/>\n",
		  relatedModule->name, relatedNode->name);
	} /* TODO: else print error */
	break;
    case SMI_INDEX_REORDER:
	if (relatedNode && relatedModule) {
	    printSegment(indent + INDENT, "", 0);
	    print("<reorders module=\"%s\" name=\"%s\"/>\n",
		  relatedModule->name, relatedNode->name);
	    printElementList(indent + INDENT, "index",
			     smiGetFirstElement(smiNode));
	} /* TODO: else print error */
	break;
    case SMI_INDEX_SPARSE:
	if (relatedNode && relatedModule) {
	    printSegment(indent + INDENT, "", 0);
	    print("<sparse module=\"%s\" name=\"%s\"/>\n",
		  relatedModule->name, relatedNode->name);
	} /* TODO: else print error */
	break;
    case SMI_INDEX_EXPAND:
	if (relatedNode && relatedModule) {
	    printSegment(indent + INDENT, "", 0);
	    print("<expands module=\"%s\" name=\"%s\"/>\n",
		  relatedModule->name, relatedNode->name);
	    printElementList(indent + INDENT, "index",
			     smiGetFirstElement(smiNode));
	} /* TODO: else print error */
	break;
    case SMI_INDEX_UNKNOWN:
	break;
    }
    printSegment(indent, "</xxxx>\n", 0);
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



static void printImport(int indent, SmiImport *smiImport)
{
    printSegment(indent, "", 0);
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
	printImport(2 * INDENT, smiImport);
    }

    if (i) {
	printSegment(INDENT, "</smi:imports>\n\n", 0);
    }
}



static void printTypedef(int indent, SmiType *smiType)
{
    printSegment(indent, "<smi:typedef", 0);
    if (smiType->name) {
	print(" name=\"%s\"", smiType->name);
    }
    print(" basetype=\"%s\"", getStringBasetype(smiType->basetype));
    if (smiType->name && smiType->status != SMI_STATUS_UNKNOWN) {
	print(" status=\"%s\"", getStringStatus(smiType->status));
    }
    print(">\n");
    
    printSegment(indent + INDENT, "<smi:parent>", 0);
    print("%s", getTypeString(smiType->basetype, smiGetParentType(smiType)));
    print("</smi:parent>\n");
    printRanges(indent + INDENT, smiType);
    printNamedNumbers(indent + INDENT, smiType);
    printValue(indent + INDENT, &smiType->value, smiType);
    printFormat(indent + INDENT, smiType->format);
    printUnits(indent + INDENT, smiType->units);
    printDescription(indent + INDENT, smiType->description);
    printReference(indent + INDENT, smiType->reference);
    
    printSegment(indent, "</smi:typedef>\n", 0);
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
	printTypedef(2 * INDENT, smiType);
    }

    if (i) {
	printSegment(INDENT, "</smi:typedefs>\n\n", 0);
    }
}



static void printObject(int indent, SmiNode *smiNode)
{
    SmiType *smiType;
    char    *tag = NULL;
    
    if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	tag = "node";
    } else if (smiNode->nodekind == SMI_NODEKIND_CAPABILITIES) {
	tag = "node";
    } else if (smiNode->nodekind == SMI_NODEKIND_TABLE) {
	tag = "table";
    } else if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	tag = "row";
    } else if (smiNode->nodekind == SMI_NODEKIND_COLUMN) {
	tag = "column";
    } else if (smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	tag = "scalar";
    }

    smiType = smiGetNodeType(smiNode);
    
    printNodeStartTag(indent, tag, smiNode);
    if (smiType && (smiType->basetype != SMI_BASETYPE_UNKNOWN)) {
	if (smiType->name) {
	    printSegment(indent + INDENT, "", 0);
	    print("<smi:type>%s</smi:type>\n",
		  getTypeString(smiType->basetype, smiType));
	} else {
	    printTypedef(indent + INDENT, smiType);
	}
    }
    if ((smiNode->nodekind != SMI_NODEKIND_TABLE) &&
	(smiNode->nodekind != SMI_NODEKIND_ROW) &&
	(smiNode->nodekind != SMI_NODEKIND_CAPABILITIES) &&
	(smiNode->nodekind != SMI_NODEKIND_NODE)) {
	printAccess(indent + INDENT, smiNode->access);
    }
    printFormat(indent + INDENT, smiNode->format);
    printUnits(indent + INDENT, smiNode->units);
    if (smiType) {
	printValue(indent + INDENT, &smiNode->value, smiType);
    }
    if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	printIndex(indent + INDENT, smiNode);
    }
    printDescription(indent + INDENT, smiNode->description);
    printReference(indent + INDENT, smiNode->reference);

    printNodeEndTag(indent, tag, smiNode);
}



static void printObjects(SmiModule *smiModule)
{
    int		 i;
    SmiNode	 *smiNode;
    SmiNodekind  nodekinds;

    nodekinds =  SMI_NODEKIND_NODE | SMI_NODEKIND_TABLE |
	SMI_NODEKIND_ROW | SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR |
	SMI_NODEKIND_CAPABILITIES;
    
    for (i = 0, smiNode = smiGetFirstNode(smiModule, nodekinds);
	 smiNode;
	 i++, smiNode = smiGetNextNode(smiNode, nodekinds)) {

	if (i == 0) {
	    printSegment(INDENT, "<smi:objects>\n", 0);
	}

	printObject(2 * INDENT, smiNode);

#if 0
	if (smiNode->create) {
	    printSegment((2 + indent) * INDENT, "<create/>\n", INDENTVALUE);
	    /* TODO: create list */
	}
	
	i++;
#endif
    }
    
    if (i) {
	printSegment(INDENT, "</smi:objects>\n\n", 0);
    }
}



static void printNotification(int indent, SmiNode *smiNode)
{
    printNodeStartTag(indent, "notification", smiNode);

    printSegment(indent + INDENT, "<smi:objects>\n", 0);
    printElementList(indent + 2 * INDENT, "object",
		     smiGetFirstElement(smiNode));
    printSegment(indent + INDENT, "</smi:objects>\n", 0);
    printDescription(indent + INDENT, smiNode->description);
    printReference(indent + INDENT, smiNode->reference);
    
    printNodeEndTag(indent, "notification", smiNode);
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
	printNotification(2 * INDENT, smiNode);
    }

    if (i) {
	printSegment(INDENT, "</smi:notifications>\n\n", 0);
    }
}



static void printGroup(int indent, SmiNode *smiNode)
{
    printNodeStartTag(indent, "group", smiNode);
    
    printSegment(indent + INDENT, "<smi:members>\n", 0);
    printElementList(indent + 2 * INDENT, "member",
		     smiGetFirstElement(smiNode));
    printSegment(indent + INDENT, "</smi:members>\n", 0);
    printDescription(indent + INDENT, smiNode->description);
    printReference(indent + INDENT, smiNode->reference);

    printNodeEndTag(indent, "group", smiNode);
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
	printGroup(2 * INDENT, smiNode);
    }

    if (i) {
	printSegment(INDENT, "</smi:groups>\n\n", 0);
    }
}



static void printComplGroups(int indent, SmiNode *smiNode)
{
    SmiNode   *optSmiNode;
    SmiModule *optSmiModule;
    SmiOption *smiOption;

    if (! smiGetFirstElement(smiNode) && !smiGetFirstOption(smiNode)) {
	return;
    }
    
    printSegment(indent, "<smi:options>\n", 0);
    printElementList(indent + INDENT, "mandatory",
		     smiGetFirstElement(smiNode));

    for(smiOption = smiGetFirstOption(smiNode);
	smiOption;
	smiOption = smiGetNextOption(smiOption)) {
	optSmiNode = smiGetOptionNode(smiOption);
	optSmiModule = smiGetNodeModule(optSmiNode);
	printSegment(indent + INDENT, "", 0);
	print("<smi:option module=\"%s\" name=\"%s\">\n",
	      optSmiModule->name, optSmiNode->name);
	printDescription(indent + 2 * INDENT, smiOption->description);
	printSegment(indent + INDENT, "</smi:option>\n", 0);
    }
    
    printSegment(indent, "</smi:options>\n", 0);
}



static void printRefinement(int indent, SmiRefinement *smiRefinement)
{
#if 0
	    printSegment(2 * INDENT, "", 0);
	    print("refine %s {\n",
		  smiGetRefinementNode(smiRefinement)->name);
	    
	    smiType = smiGetRefinementType(smiRefinement);
	    if (smiType) {
		printSegment(3 * INDENT, "type", INDENTVALUE);
		print("%s",
		      getTypeString(smiType->basetype,
				    smiGetParentType(smiType)));
		printSubtype(smiType);
		print(";\n");
	    }
	    
	    smiType = smiGetRefinementWriteType(smiRefinement);
	    if (smiType) {
		printSegment(3 * INDENT, "writetype", INDENTVALUE);
		print("%s",
		      getTypeString(smiType->basetype,
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
#endif
}



static void printRefinements(int indent, SmiNode *smiNode)
{
    SmiRefinement *smiRefinement;
    int            i;

    for(i = 0, smiRefinement = smiGetFirstRefinement(smiNode);
	smiRefinement;
	i++, smiRefinement = smiGetNextRefinement(smiRefinement)) {

    	if (!i) {
	    printSegment(indent, "<smi:refinements>\n", 0);
	    print("<!--\n");
	}

	printRefinement(indent + INDENT, smiRefinement);
    }
    
    if (i) {
	print("-->\n");
	printSegment(indent, "</smi:refinements>\n\n", 0);
    }
}



static void printCompliance(int indent, SmiNode *smiNode)
{
    printNodeStartTag(indent, "compliance", smiNode);

    printDescription(indent + INDENT, smiNode->description);
    printReference(indent + INDENT, smiNode->reference);
    printComplGroups(indent + INDENT, smiNode);
    printRefinements(indent + INDENT, smiNode);

    printNodeEndTag(indent, "compliance", smiNode);
}



static void printCompliances(SmiModule *smiModule)
{
    SmiNode *smiNode;
    int      i;

    for(i = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_COMPLIANCE);
	smiNode;
	i++, smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	
	if (!i) {
	    printSegment(INDENT, "<smi:compliances>\n", 0);
	}

	printCompliance(2 * INDENT, smiNode);
    }

    if (i) {
	printSegment(INDENT, "</smi:compliances>\n\n", 0);
    }
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
