/*
 * dump-smi.c --
 *
 *      Operations to dump SMIv1/v2 module information.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-smi.c,v 1.1 1999/04/06 17:49:25 strauss Exp $
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

#include "smi.h"
#include "util.h"



#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */
#define  INDENTSEQUENCE 40   /* column to start SEQUENCE tables' type column */



static char *convertType[] = {
    "Enumeration",	   "INTEGER",
    "Bits",		   "BITS",
    "OctetString",	   "OCTET STRING",
    "ObjectIdentifier",	   "OBJECT IDENTIFIER",
    NULL, NULL };

static char *convertImport[] = {
    "IRTF-NMRG-SMING-TYPES", "IpAddress",   "SNMPv2-SMI",  "IpAddress",
    "IRTF-NMRG-SMING-TYPES", "Counter32",   "SNMPv2-SMI",  "Counter32",
    "IRTF-NMRG-SMING-TYPES", "Gauge32",	    "SNMPv2-SMI",  "Gauge32",
    "IRTF-NMRG-SMING-TYPES", "TimeTicks",   "SNMPv2-SMI",  "TimeTicks",
    "IRTF-NMRG-SMING-TYPES", "Opaque",	    "SNMPv2-SMI",  "Opaque",
    "IRTF-NMRG-SMING-TYPES", "Counter64",   "SNMPv2-SMI",  "Counter64",
    "IRTF-NMRG-SMING", NULL,		    "SNMPv2-SMI",  NULL,
    NULL, NULL, NULL, NULL };



static int current_column = 0;
static int version = 2;


static char *
getTypeString(name, syntax)
    char *name;
{
    int i;

    if (syntax == SMI_BASETYPE_ENUM) {
	return "INTEGER";
    }
    
    if (syntax == SMI_BASETYPE_BITS) {
	return "BITS";
    }

    if (!name) {
	return "<unknown>";
    }
    
    for(i=0; convertType[i]; i += 2) {
	if (!strcmp(name, convertType[i])) {
	    return convertType[i+1];
	}
    }
    return name;
}



static char *
getOidString(modulename, object, importedParent)
    char	 *modulename;
    char	 *object;
    int		 importedParent;  /* force use of imported oid label (MI) */
{
    SmiNode      *smiNode;
    static char	 s[SMI_MAX_OID+1];
    char	 child[SMI_MAX_OID+1];
    char	 append[SMI_MAX_OID+1];
    char         *fullname;
    char	 *p;
    
    s[0] = 0;

    strcpy(child, object);
    append[0] = 0;
    do {
	fullname = smiGetParent(child, modulename);
	if (fullname) {
	    smiNode = smiGetNode(child, modulename);
	    if (smiNode) {
		strcpy(s, append);
		if ((p = strrchr(smiNode->oid, '.'))) {
		    sprintf(append, "%s%s", p, s);
		}
	    } else {
		strcpy(s, append);
		sprintf(append, "%s%s", strrchr(child, '.'), s);
	    }
	    strcpy(child, smiDescriptor(fullname));
	} else {
	    strcpy(child, "<unknown>");
	    fullname = child;
	    break;
	}
    } while ((strchr(smiDescriptor(fullname), '.')) ||
	     (importedParent && !smiIsImported(modulename, fullname)));
    sprintf(s, "%s%s", smiDescriptor(fullname), append);

    for (p = s; p[0]; p++)
	if (*p == '.') *p = ' ';
    
    return s;
}



static void
print(char *fmt, ...)
{
    va_list ap;
    char    s[200];
    char    *p;
    
    va_start(ap, fmt);
    current_column += vsprintf(s, fmt, ap);
    va_end(ap);

    printf("%s", s);

    if ((p = strrchr(s, '\n'))) {
	current_column = strlen(p) - 1;
    }
}



static void
printSegment(column, string, length)
    int      column;
    char     *string;
    int	     length;
{
    char     s[200];

    if (length) {
	sprintf(s, "%*c%s%*c",
		1 + column, ' ', string,
		length - strlen(string) - column, ' ');
    } else {
	sprintf(s, "%*c%s",
		1 + column, ' ', string);
    }
    print(&s[1]);
}



static void
printWrapped(column, string)
    int  column;
    char *string;
{
    if ((current_column + strlen(string)) > INDENTMAX) {
	print("\n");
	printSegment(column, "", 0);
    }
    print("%s", string);
}



static char *
getValueString(valuePtr)
    SmiValue  *valuePtr;
{
    static char s[100];
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
    case SMI_BASETYPE_LABEL:
	sprintf(s, "%s", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_OCTETSTRING:
	sprintf(s, "\"%s\"", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	for(p = valuePtr->value.bits; *p; p++) {
	    if (p != valuePtr->value.bits)
		sprintf(&s[strlen(s)], ", ");
	    sprintf(&s[strlen(s)], "%s", *p);
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
    case SMI_BASETYPE_CHOICE:
    case SMI_BASETYPE_OBJECTIDENTIFIER:
    case SMI_BASETYPE_SEQUENCE:
    case SMI_BASETYPE_SEQUENCEOF:
	break;
    case SMI_BASETYPE_BINSTRING:
	break;
    case SMI_BASETYPE_HEXSTRING:
	break;
    }

    return s;
}



static void
printSubtype(smiType)
    SmiType *smiType;
{
    void    **listPtr;
    char    s[100];

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	    (smiType->basetype == SMI_BASETYPE_BITS)) {
	print("{ ");
    } else {
	print("(");
    }
    for (listPtr = smiType->list; *listPtr; listPtr++) {
	if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	    (smiType->basetype == SMI_BASETYPE_BITS)) {
	    if (listPtr != smiType->list)
		print(", ");
	    sprintf(s, "%s(%s)", ((SmiNamedNumber *)*listPtr)->name,
		   getValueString(((SmiNamedNumber *)*listPtr)->valuePtr));
	    printWrapped(INDENTVALUE + INDENT, s);
	} else {
	    if (listPtr != smiType->list)
		print(" | ");
	    s[0] = 0;
	    if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		sprintf(&s[strlen(s)], "SIZE(");
	    }
	    if (bcmp(((SmiRange *)*listPtr)->minValuePtr,
		     ((SmiRange *)*listPtr)->maxValuePtr,
		     sizeof(SmiValue))) {
		sprintf(&s[strlen(s)], "%s", 
			getValueString(((SmiRange *)*listPtr)->minValuePtr));
		sprintf(&s[strlen(s)], "..%s", 
			getValueString(((SmiRange *)*listPtr)->maxValuePtr));
	    } else {
		sprintf(&s[strlen(s)], "%s",
			getValueString(((SmiRange *)*listPtr)->minValuePtr));
	    }
	    if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		sprintf(&s[strlen(s)], ")");
	    }
	    printWrapped(INDENTVALUE + INDENT, s);
	}
    }
    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	    (smiType->basetype == SMI_BASETYPE_BITS)) {
	print(" }");
    } else {
	print(")");
    }
}



static void
printMultilineString(s)
    const char *s;
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



static void
printImports(modulename)
    char	  *modulename;
{
    char	  **list;
    char	  *lastModulename = NULL;
    char	  *importedModulename, *importedDescriptor;
    char	  **p;
    int		  i;
    char	  types[200];
    
    list = smiGetMembers(modulename, "");

    types[0] = 0;

    /* TODO: add needed macros; add types that are base types in SMIng */
    
    if (!list)
	return;

    for(p = list; *p; p++) {
	importedModulename = *p;
	importedDescriptor = strstr(*p, SMI_NAMESPACE_OPERATOR);
	importedDescriptor[0] = 0;
	importedDescriptor =
	    &importedDescriptor[strlen(SMI_NAMESPACE_OPERATOR)];

	for(i = 0; convertImport[i]; i += 4) {
	    if ((!util_strcmp(importedModulename, convertImport[i])) &&
		(!util_strcmp(importedDescriptor, convertImport[i+1]))) {
		importedModulename = convertImport[i+2];
		importedDescriptor = convertImport[i+3];
		break;
	    } else if ((!util_strcmp(importedModulename, convertImport[i])) &&
		       (!convertImport[i+1])) {
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
		    printSegment(2 * INDENT, "", 0);
		    print("FROM %s", lastModulename);
		}
		print("\n");
		printSegment(INDENT, "", 0);
	    } else {
		print(",");
		print(" ");
	    }
	    printWrapped(INDENT, importedDescriptor);
	    lastModulename = importedModulename;
	}
    }
    if (lastModulename) {
	print("\n");
	printSegment(2 * INDENT, "", 0);
	print("FROM %s;\n\n", lastModulename);
    }
}



static void
printModuleIdentity(modulename)
    char	 *modulename;
{
    int		 i;
    SmiRevision  *smiRevision;
    SmiModule	 *smiModule;
    
    smiModule = smiGetModule(modulename);
    
    if (smiModule->object) {
	print("%s MODULE-IDENTITY\n", smiModule->object);
	printSegment(INDENT, "LAST-UPDATED", INDENTVALUE);
	smiRevision = smiGetFirstRevision(modulename);
	if (smiRevision)
	    print("\"%s\"\n", smiCTime(smiRevision->date));
	else
	    print("\"197001010000Z\"\n");
	printSegment(INDENT, "ORGANIZATION", INDENTVALUE);
	print("\n");
	printMultilineString(smiModule->organization);
	print("\n");
	printSegment(INDENT, "CONTACT-INFO", INDENTVALUE);
	print("\n");
	printMultilineString(smiModule->contactinfo);
	print("\n");
	printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
	print("\n");
	printMultilineString(smiModule->description);
	print("\n");
	
	for(i = 0; smiRevision;
	    smiRevision = smiGetNextRevision(modulename, smiRevision->date)) {
	    if (strcmp(smiRevision->description,
	         "[Revision added by libsmi due to a LAST-UPDATED clause.]")) {
		printSegment(INDENT, "REVISION", INDENTVALUE);
		print("\"%s\";\n", smiCTime(smiRevision->date));
		printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
		print("\n");
		printMultilineString(smiRevision->description);
		print("\n");
	    }
	    i++;
	}

	print("\n");
    }
}



static void
printTypeDefinitions(modulename)
    char	 *modulename;
{
    SmiType	 *smiType;
    
    for(smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(modulename, smiType->name)) {
	if ((smiType->decl == SMI_DECL_TYPEASSIGNMENT) ||
	    ((smiType->decl == SMI_DECL_TYPEDEF) &&
	     (!smiType->description))) {
	    print("%s ::=\n", smiType->name);
	    printSegment(INDENT, "", 0);
	    print("%s", getTypeString(smiType->parent, smiType->basetype));
	    if (smiType->list) {
		print(" ");
		printSubtype(smiType);
	    }
	    print("\n\n");
	}
    }
}



static void
printTextualConventions(modulename)
    char	 *modulename;
{
    SmiType	 *smiType;
    
    for(smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(modulename, smiType->name)) {
	if ((smiType->decl == SMI_DECL_TEXTUALCONVENTION) ||
	    ((smiType->decl == SMI_DECL_TYPEDEF) &&
	     (smiType->description))) {
	    print("%s ::= TEXTUAL-CONVENTION\n", smiType->name);

	    if (smiType->format) {
		printSegment(INDENT, "DISPLAY-HINT", INDENTVALUE);
		print ("\"%s\"\n", smiType->format);
	    }
	    
	    printSegment(INDENT, "STATUS", INDENTVALUE);
	    print ("%s\n", smiStringStatus(smiType->status));
	    
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiType->description);
	    print("\n");

	    if (smiType->reference) {
		printSegment(INDENT, "REFERENCE", INDENTVALUE);
		print("\n");
		printMultilineString(smiType->description);
		print("\n");
	    }
	    
	    printSegment(INDENT, "SYNTAX", INDENTVALUE);
	    print("%s", getTypeString(smiType->parent, smiType->basetype));
	    if (smiType->list) {
		print(" ");
		printSubtype(smiType);
	    }
	    print("\n\n");
	}
    }
}



static void
printObjects(modulename)
    char	 *modulename;
{
    SmiNode	 *smiNode, *rowNode, *colNode;
    SmiType	 *smiType;
    char	 rowoid[SMI_MAX_OID+1];
    char	 *typename;
    char	 **p;
    int		 i;
    
    strcpy(rowoid, "-");
    
    for(smiNode = smiGetFirstNode(modulename);
	smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {

	if ((smiNode->decl == SMI_DECL_NODE) ||
	    (smiNode->decl == SMI_DECL_VALUEASSIGNMENT)) {
	    print("%s OBJECT IDENTIFIER\n", smiNode->name);
	} else if ((smiNode->decl == SMI_DECL_TABLE) ||
		   (smiNode->basetype == SMI_BASETYPE_SEQUENCEOF)) {
	    print("%s OBJECT-TYPE\n", smiNode->name);
	} else if ((smiNode->decl == SMI_DECL_ROW) ||
		   (smiNode->basetype == SMI_BASETYPE_SEQUENCE)) {
	    print("%s OBJECT-TYPE\n", smiNode->name);
	    /*
	     * Remember the row's oid to check for subsequent objects
	     * being columns instead of scalars more efficiently.
	     */
	    strcpy(rowoid, smiNode->oid);
	} else if ((smiNode->decl == SMI_DECL_COLUMN) ||
		   (strstr(smiNode->oid, rowoid))) {
	    print("%s OBJECT-TYPE\n", smiNode->name);
	} else if ((smiNode->decl == SMI_DECL_SCALAR) ||
		   (smiNode->decl == SMI_DECL_OBJECTTYPE)) {
	    print("%s OBJECT-TYPE\n", smiNode->name);
	} else {
	    continue;
	}

	if ((smiNode->decl != SMI_DECL_NODE) &&
	    (smiNode->decl != SMI_DECL_VALUEASSIGNMENT)) {
	    
	    printSegment(INDENT, "SYNTAX", INDENTVALUE);
	    if ((smiNode->decl == SMI_DECL_TABLE) ||
		   (smiNode->basetype == SMI_BASETYPE_SEQUENCEOF)) {
		print("SEQUENCE OF ");
		rowNode = smiGetNextNode(modulename, smiNode->name);
		print("%s\n", getTypeString(smiDescriptor(rowNode->type),
					  SMI_BASETYPE_UNKNOWN));
	    } else if ((smiNode->decl == SMI_DECL_ROW) ||
		   (smiNode->basetype == SMI_BASETYPE_SEQUENCE)) {
		print("%s\n", getTypeString(smiDescriptor(smiNode->type),
					    SMI_BASETYPE_UNKNOWN));
	    } else if (smiNode->type) {
		typename = smiDescriptor(smiNode->type);
		if (islower((int)typename[0])) {
		    /*
		     * an implicitly restricted type.
		     */
		    smiType = smiGetType(smiNode->type, "");
		    print("%s", getTypeString(smiDescriptor(smiType->parent),
					      smiType->basetype));
		    if (smiType->list) {
			print(" ");
			printSubtype(smiType);
		    }
		    print("\n");
		} else {
		    print("%s\n", getTypeString(typename,
						SMI_BASETYPE_UNKNOWN));
		}
	    }
	    
	    if (smiNode->units) {
		printSegment(INDENT, "UNITS", INDENTVALUE);
		print("\"%s\"\n", smiNode->units);
	    }
	    
	    printSegment(INDENT, "MAX-ACCESS", INDENTVALUE);
	    print("%s\n", smiStringAccess(smiNode->access));
	    /* TODO: read-create */
		
	    printSegment(INDENT, "STATUS", INDENTVALUE);
	    print("%s\n", smiStringStatus(smiNode->status));
	    
	    if (smiNode->description) {
		printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
		print("\n");
		printMultilineString(smiNode->description);
		print("\n");
	    }
	    
	    if (smiNode->reference) {
		printSegment(INDENT, "REFERENCE", INDENTVALUE);
		print("\n");
		printMultilineString(smiNode->reference);
		print("\n");
	    }
	    
	    switch (smiNode->indexkind) {
	    case SMI_INDEX_INDEX:
	    case SMI_INDEX_REORDER:
	    case SMI_INDEX_EXPAND:
		printSegment(INDENT, "INDEX", INDENTVALUE);
		/* TODO: if (smiNode->implied) ... */
		print("{ ");
		for(p = smiNode->index; *p; p++) {
		    if (p != smiNode->index) {
			print(", ");
		    }
		    printWrapped(INDENTVALUE + 1, smiDescriptor(*p));
		} /* TODO: empty? -> print error */
		print(" }\n");
		break;
	    case SMI_INDEX_AUGMENT:
	    case SMI_INDEX_SPARSE:
		printSegment(INDENT, "AUGMENTS", INDENTVALUE);
		print("{ %s }\n", smiNode->relatedrow);
		break;
	    case SMI_INDEX_UNKNOWN:
		break;
	    }
	    
	    if (smiNode->value) {
		printSegment(INDENT, "DEFVAL", INDENTVALUE);
		print("%s", getValueString(smiNode->value));
		print("\n");
	    }

	}
	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(modulename, smiNode->oid, 0));

	if ((smiNode->decl == SMI_DECL_ROW) ||
	    (smiNode->basetype == SMI_BASETYPE_SEQUENCE)) {
	    print("%s ::=\n", getTypeString(smiDescriptor(smiNode->type),
					    SMI_BASETYPE_UNKNOWN));
	    printSegment(INDENT, "SEQUENCE {", 0);
	    for(i = 0, colNode = smiGetNextNode(modulename, smiNode->name);
		colNode && strstr(colNode->oid, rowoid);
		colNode = smiGetNextNode(modulename, colNode->name)) {
		if (i) {
		    print(",");
		}
		print("\n");
		printSegment(2 * INDENT, colNode->name, INDENTSEQUENCE);
		typename = smiDescriptor(colNode->type);
		if (islower((int)typename[0])) {
		    /*
		     * an implicitly restricted type.
		     */
		    smiType = smiGetType(colNode->type, "");
		    print("%s", getTypeString(smiDescriptor(smiType->parent),
					      smiType->basetype));
		} else {
		    print("%s", getTypeString(typename,
					      SMI_BASETYPE_UNKNOWN));
		}
		i++;
	    }
	    print("\n");
	    printSegment(INDENT, "}\n", 0);
	    print("\n");
	}
    }
}



static void
printNotifications(modulename)
    char	 *modulename;
{
    SmiNode	 *smiNode;
    char	 **p;
    
    for(smiNode = smiGetFirstNode(modulename);
	smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {

	if ((smiNode->decl != SMI_DECL_NOTIFICATIONTYPE) &&
	    (smiNode->decl != SMI_DECL_NOTIFICATION)) {
	    continue;
	}

	print("%s NOTIFICATION-TYPE\n", smiNode->name);

	printSegment(INDENT, "OBJECTS", INDENTVALUE);
	print("{ ");
	if (smiNode->list) {
	    for(p = smiNode->list; *p; p++) {
		if (p != smiNode->list) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2, *p);
	    }
	}
	print(" }\n");

	printSegment(INDENT, "STATUS", INDENTVALUE);
	print("%s\n", smiStringStatus(smiNode->status));

	if (smiNode->description) {
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print("\n");
	}
	    
	if (smiNode->reference) {
	    printSegment(INDENT, "REFERENCE", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print("\n");
	}

	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(modulename, smiNode->oid, 0));
    }
}



static void
printObjectGroups(modulename)
    char	 *modulename;
{
    SmiNode	 *smiNode, *objectNode;
    char	 **p;
    
    for (smiNode = smiGetFirstNode(modulename);
	 smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {
	    
	if ((smiNode->decl != SMI_DECL_OBJECTGROUP) &&
	    (smiNode->decl != SMI_DECL_GROUP)) {
	    continue;
	}
	if (smiNode->list) {
	    objectNode = smiGetNode(*(smiNode->list), modulename);
	    if (objectNode &&
		((objectNode->decl == SMI_DECL_NOTIFICATIONTYPE) ||
		 (objectNode->decl == SMI_DECL_NOTIFICATION))) {
		continue;
	    }
	}
	    
	print("%s OBJECT-GROUP\n", smiNode->name);
	    
	printSegment(INDENT, "OBJECTS", INDENTVALUE);
	print("{ ");
	if (smiNode->list) {
	    for(p = smiNode->list; *p; p++) {
		if (p != smiNode->list) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2, *p);
	    }
	}
	print(" }\n");

	printSegment(INDENT, "STATUS", INDENTVALUE);
	print("%s\n", smiStringStatus(smiNode->status));

	if (smiNode->description) {
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print("\n");
	}
	    
	if (smiNode->reference) {
	    printSegment(INDENT, "REFERENCE", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print("\n");
	}

	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(modulename, smiNode->oid, 0));
    }
}



static void
printNotificationGroups(modulename)
    char	 *modulename;
{
    SmiNode	 *smiNode, *objectNode;
    char	 **p;
    
    for (smiNode = smiGetFirstNode(modulename);
	 smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {
	    
	if ((smiNode->decl != SMI_DECL_NOTIFICATIONGROUP) &&
	    (smiNode->decl != SMI_DECL_GROUP)) {
	    continue;
	}
	if (smiNode->list) {
	    objectNode = smiGetNode(*(smiNode->list), modulename);
	    if (objectNode &&
		(objectNode->decl != SMI_DECL_NOTIFICATIONTYPE) &&
		(objectNode->decl != SMI_DECL_NOTIFICATION)) {
		continue;
	    }
	}
	    
	print("%s NOTIFICATION-GROUP\n", smiNode->name);
	    
	printSegment(INDENT, "NOTIFICATIONS", INDENTVALUE);
	print("{ ");
	if (smiNode->list) {
	    for(p = smiNode->list; *p; p++) {
		if (p != smiNode->list) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2, *p);
	    }
	}
	print(" }\n");

	printSegment(INDENT, "STATUS", INDENTVALUE);
	print("%s\n", smiStringStatus(smiNode->status));

	if (smiNode->description) {
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print("\n");
	}
	    
	if (smiNode->reference) {
	    printSegment(INDENT, "REFERENCE", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print("\n");
	}

	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(modulename, smiNode->oid, 0));
    }
}



static void
printModuleCompliances(modulename)
    char	  *modulename;
{
    SmiNode	  *smiNode;
    SmiType	  *smiType;
    SmiOption	  **smiOption;
    SmiRefinement **smiRefinement;
    char	  **p;
    char	  lastModule[100];
    
    for (smiNode = smiGetFirstNode(modulename);
	 smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {
	
	if ((smiNode->decl != SMI_DECL_MODULECOMPLIANCE) &&
	    (smiNode->decl != SMI_DECL_COMPLIANCE)) {
	    continue;
	}
	    
	print("%s MODULE-COMPLIANCE\n", smiNode->name);

	printSegment(INDENT, "STATUS", INDENTVALUE);
	print("%s\n", smiStringStatus(smiNode->status));

	if (smiNode->description) {
	    printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print("\n");
	}
	    
	if (smiNode->reference) {
	    printSegment(INDENT, "REFERENCE", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print("\n");
	}



	
	if (smiNode->list) {
	    lastModule[0] = '-';
	    for(p = smiNode->list; *p; p++) {
		if (strcmp(lastModule, smiModule(*p))) {
		    if (p != smiNode->list) {
			print(" }\n");
		    }
		    strcpy(lastModule, smiModule(*p));
		    printSegment(INDENT, "MODULE", 0);
		    if (strlen(smiModule(*p)) &&
			strcmp(modulename, smiModule(*p))) {
			print(" %s\n", smiModule(*p));
		    } else {
			print(" -- this module\n");
		    }
		    printSegment(2 * INDENT, "MANDATORY-GROUPS", INDENTVALUE);
		    print("{ ");
		} else {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2, smiDescriptor(*p));
	    }
	    print(" }\n");
	}

#if 0	
	if (smiNode->option) {
	    print("\n");
	    for(smiOption = smiNode->option; *smiOption; smiOption++) {
		printSegment(2 * INDENT, "", 0);
		print("optional %s {\n", (*smiOption)->name);
		printSegment(3 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString((*smiOption)->description);
		print(";\n");
		printSegment(2 * INDENT, "};\n", 0);
	    }
	}
	
	if (smiNode->refinement) {
	    print("\n");
	    for(smiRefinement = smiNode->refinement;
		*smiRefinement; smiRefinement++) {
		printSegment(2 * INDENT, "", 0);
		print("refine %s {\n", (*smiRefinement)->name);

		if ((*smiRefinement)->type) {
		    smiType = smiGetType((*smiRefinement)->type, modulename);
		    if (smiType) {
			printSegment(3 * INDENT, "type", INDENTVALUE);
			print("%s",
			      getTypeString(smiDescriptor(smiType->parent),
					    smiType->basetype));
			if (smiType->list) {
			    print(" ");
			    printSubtype(smiType);
			}
			print(";\n");
		    }
		}

		if ((*smiRefinement)->writetype) {
		    smiType = smiGetType((*smiRefinement)->writetype,
					 modulename);
		    if (smiType) {
			printSegment(3 * INDENT, "writetype", INDENTVALUE);
			print("%s",
			      getTypeString(smiDescriptor(smiType->parent),
					    smiType->basetype));
			if (smiType->list) {
			    print(" ");
			    printSubtype(smiType);
			}
			print(";\n");
		    }
		}

		if ((*smiRefinement)->access != SMI_ACCESS_UNKNOWN) {
		    printSegment(3 * INDENT, "access", INDENTVALUE);
		    print("%s;\n",
			  smingStringAccess((*smiRefinement)->access));
		}
		printSegment(3 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString((*smiRefinement)->description);
		print(";\n");
		printSegment(2 * INDENT, "};\n", 0);
	    }
	}
#endif

	
	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(modulename, smiNode->oid, 0));
    }
}




void
dumpSmi(modulename)
    char	 *modulename;
{
    SmiModule	 *smiModule;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	print("--\n");
	print("-- This module has been generated by smidump. Do not edit.\n");
	print("--\n");
	print("%s DEFINITIONS ::= BEGIN\n\n", smiModule->name);

	printImports(modulename);

	printModuleIdentity(modulename);

	printTypeDefinitions(modulename);

	printTextualConventions(modulename);

	printObjects(modulename);
	
	printNotifications(modulename);
	
	printObjectGroups();
	
	printNotificationGroups();
	
	printModuleCompliances();
	
	print("END -- end of module %s.\n", modulename);

    }
}



void dumpSmiV1(modulename)
    char *modulename;
{
    version = 1;
    dumpSmi(modulename);
}



void dumpSmiV2(modulename)
    char *modulename;
{
    version = 2;
    dumpSmi(modulename);
}

