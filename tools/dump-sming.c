/*
 * dump-sming.c --
 *
 *      Operations to dump SMIng module information.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-sming.c,v 1.11 1999/03/29 22:34:06 strauss Exp $
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



#define  INDENT		2    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */

#define  STYLE_IMPORTS  2



char *excludeDescriptors[] = {
    "ObjectName", "NotificationName", "ObjectSyntax", "SimpleSyntax",
    "Integer32", "ApplicationSyntax", "Unsigned32",
    NULL };

char *typeDescriptors[] = {
    "IpAddress", "Counter32", "Gauge32", "TimeTicks", "Opaque", "Counter64",
    NULL };

char *smi2smingTypes[] = {
    "INTEGER", "Integer32",
    "OCTET STRING", "OctetString",
    "OBJECT IDENTIFIER", "ObjectIdentifier",
    NULL, NULL };



static int current_column = 0;



static char *
getTypeString(name, syntax)
    char *name;
{
    int i;

    if (syntax == SMI_BASETYPE_ENUM) {
	return "Enumeration";
    }
    
    if (syntax == SMI_BASETYPE_BITS) {
	return "Bits";
    }
    
    for(i=0; smi2smingTypes[i]; i += 2) {
	if (!strcmp(name, smi2smingTypes[i])) {
	    return smi2smingTypes[i+1];
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
	}
	strcpy(child, smiDescriptor(fullname));
    } while (((!importedParent) || (!smiIsImported(modulename, fullname))) &&
	     (strchr(smiDescriptor(fullname), '.')));
    sprintf(s, "%s%s", smiDescriptor(fullname), append);

    return s;
}



void
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
    
    print("(");
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
	    if (bcmp(((SmiRange *)*listPtr)->minValuePtr,
		     ((SmiRange *)*listPtr)->maxValuePtr,
		     sizeof(SmiValue))) {
		sprintf(s, "%s", 
			getValueString(((SmiRange *)*listPtr)->minValuePtr));
		sprintf(&s[strlen(s)], "..%s", 
			getValueString(((SmiRange *)*listPtr)->maxValuePtr));
	    } else {
		sprintf(s, "%s",
			getValueString(((SmiRange *)*listPtr)->minValuePtr));
	    }
	    printWrapped(INDENTVALUE + INDENT, s);
	}
    }
    print(")");
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
    int		  convert, i;
    char	  types[200];
    char	  *t;
    
    list = smiGetMembers(modulename, "");

    types[0] = 0;
    
    if (!list)
	return;
    
    for(p = list; *p; p++) {
	importedModulename = *p;
	importedDescriptor = strstr(*p, SMI_NAMESPACE_OPERATOR);
	importedDescriptor[0] = 0;
	importedDescriptor =
	    &importedDescriptor[strlen(SMI_NAMESPACE_OPERATOR)];

	/*
	 * imported SMI modules have to be handled more carefully:
	 * The module name is mapped to an SMIng module and some definitions
	 * are stripped off (namely macros). Also note, that this may
	 * lead to an empty list of imported descriptors from that SMIv1/v2
	 * module.
	 */
	convert = 0;
	if (!strcmp(importedModulename, "SNMPv2-SMI")) {
	    convert = 1;
	    importedModulename = "IRTF-NMRG-SMING";
	}
	if (!strcmp(importedModulename, "SNMPv2-TC")) {
	    convert = 1;
	    importedModulename = "IRTF-NMRG-SMING-CORE-TYPES";
	}
	if (!strcmp(importedModulename, "SNMPv2-CONF")) {
	    convert = 1;
	    importedModulename = "";
	}
	if (convert) {
	    /*
	     * Exclude all-uppercase descriptors, assuming they are macros.
	     */
	    if (!strpbrk(importedDescriptor,
			 "abcdefghijklmnopqrstuvwxyz")) {
		importedDescriptor = "";
	    }
	    /*
	     * Exclude descriptors from a list of well known SMIv1/v2 types,
	     * which need not to be imported im SMIng.
	     */
	    for(i=0; excludeDescriptors[i]; i++) {
		if (!strcmp(importedDescriptor, excludeDescriptors[i])) {
		    importedDescriptor = "";
		    break;
		}
	    }
	    for(i=0; typeDescriptors[i]; i++) {
		if (!strcmp(importedDescriptor, typeDescriptors[i])) {
		    strcat(types, importedDescriptor);
		    strcat(types, " ");
		    importedDescriptor = "";
		    break;
		}
	    }
	}

	if (strlen(importedDescriptor)) {
	    if ((!lastModulename) ||
		strcmp(importedModulename, lastModulename)) {
		if (lastModulename) {
#if STYLE_IMPORTS == 1
		    print("\n");
		    printSegment(INDENT, ");\n", 0);
#endif
#if STYLE_IMPORTS == 2
		    print(");\n\n");
#endif
		    if (strlen(types)) {
			printSegment(INDENT, "", 0);
			print("import IRTF-NMRG-SMING-TYPES (");
			for (t = strtok(types, " "); t;
			     t = strtok(NULL, " ")) {
			    if (t != types) {
				print(",");
#if STYLE_IMPORTS == 2
				print(" ");
#endif
			    }
#if STYLE_IMPORTS == 1
			    print("\n");
			    printSegment(2 * INDENT, "", 0);
			    print("%s", t);
#endif
#if STYLE_IMPORTS == 2
			    printWrapped(INDENTVALUE + INDENT, t);
#endif
			}
#if STYLE_IMPORTS == 1
			print("\n");
			printSegment(INDENT, ");\n\n", 0);
#endif
#if STYLE_IMPORTS == 2
			print(");\n\n");
#endif
			types[0] = 0;
		    }
		} else {
		    print("//\n// IDENTIFIER IMPORTS\n//\n\n");
		}
		printSegment(INDENT, "", 0);
		print("import %s (", importedModulename);
		lastModulename = importedModulename;
	    } else {
		print(",");
#if STYLE_IMPORTS == 2
		print(" ");
#endif
	    }
#if STYLE_IMPORTS == 1
	    print("\n");
	    printSegment(2 * INDENT, "", 0);
	    print("%s", importedDescriptor);
#endif
#if STYLE_IMPORTS == 2
	    printWrapped(INDENTVALUE + INDENT, importedDescriptor);
#endif
	}
    }
    if (lastModulename) {
#if STYLE_IMPORTS == 1
	print("\n");
	printSegment(INDENT, ");\n\n", 0);
#endif
#if STYLE_IMPORTS == 2
	print(");\n\n");
#endif
    }
}



static void
printRevisions(modulename)
    char	 *modulename;
{
    int i;
    SmiRevision *smiRevision;
    
    for(i = 0, smiRevision = smiGetFirstRevision(modulename);
	smiRevision;
	smiRevision = smiGetNextRevision(modulename,
					 smiRevision->date)) {
	printSegment(INDENT, "revision {\n", 0);
	printSegment(2 * INDENT, "date", INDENTVALUE);
	print("\"%s\";\n", smiCTime(smiRevision->date));
	printSegment(2 * INDENT, "description", INDENTVALUE);
	print("\n");
	printMultilineString(smiRevision->description);
	print(";\n");
        printSegment(INDENT, "};\n", 0);
	i++;
    }
    if (i) {
	print("\n");
    }
}



static void
printTypedefs(modulename)
    char	 *modulename;
{
    int		 i;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(modulename, smiType->name)) {
	if (!i) {
	    print("//\n// TYPE DEFINITIONS\n//\n\n");
	}
	printSegment(INDENT, "", 0);
	print("typedef %s {\n", smiType->name);

	printSegment(2 * INDENT, "type", INDENTVALUE);
	print("%s", getTypeString(smiType->parent, smiType->basetype));
	if (smiType->list) {
	    print(" ");
	    printSubtype(smiType);
	}
	print(";\n");

	if (smiType->value) {
	    printSegment(2 * INDENT, "default", INDENTVALUE);
	    print("%s", getValueString(smiType->value));
	    print(";\n");
	}
	
	if (smiType->format) {
	    printSegment(2 * INDENT, "format", INDENTVALUE);
	    print ("\"%s\";\n", smiType->format);
	}
	if (smiType->units) {
	    printSegment(2 * INDENT, "units", INDENTVALUE);
	    print ("\"%s\";\n", smiType->units);
	}
	if (smiType->status != SMI_STATUS_CURRENT) {
	    printSegment(2 * INDENT, "status", INDENTVALUE);
	    print ("%s;\n", smingStringStatus(smiType->status));
	}
	printSegment(2 * INDENT, "description", INDENTVALUE);
	print ("\n");
	printMultilineString(smiType->description);
	print(";\n");
	if (smiType->reference) {
	    printSegment(2 * INDENT, "reference", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiType->reference);
	    print(";\n");
	}
	printSegment(INDENT, "};\n\n", 0);
	i++;
    }
}



static void
printObjects(modulename)
    char	 *modulename;
{
    int		 i, j;
    SmiNode	 *smiNode;
    SmiType	 *smiType;
    char	 rowoid[SMI_MAX_OID+1];
    int		 indent = 0;
    int		 lastindent = -1;
    char	 *typename;
    char	 *s;
    
    strcpy(rowoid, "-");
    
    for(i = 0, smiNode = smiGetFirstNode(modulename);
	smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {

	if (smiNode->decl == SMI_DECL_VALUEASSIGNMENT) {
	    indent = 0;
	    s = "node";
	} else if (smiNode->decl == SMI_DECL_OBJECTTYPE) {
	    if (smiNode->basetype ==SMI_BASETYPE_SEQUENCEOF) {
		indent = 0;
		s = "table";
	    } else if (smiNode->basetype ==SMI_BASETYPE_SEQUENCE) {
		indent = 1;
		s = "row";
		/*
		 * Remember the row's oid to check for subsequent objects
		 * being columns instead of scalars more efficiently.
		 */
		strcpy(rowoid, smiNode->oid);
	    } else if (strstr(smiNode->oid, rowoid)) {
		indent = 2;
		s = "column";
	    } else {
		indent = 0;
		s = "scalar";
	    }
	} else {
	    continue;
	}

	if (!i) {
	    print("//\n// OBJECT DEFINITIONS\n//\n\n");
	}

#if 1
	for (j = lastindent; j >= indent; j--) {
	    printSegment((1 + j) * INDENT, "", 0);
	    print("};\n");
	}
#endif
	print("\n");
	lastindent = indent;
	
	printSegment((1 + indent) * INDENT, "", 0);
	print("%s %s {\n", s, smiNode->name);
	
	if (smiNode->oid) {
	    printSegment((2 + indent) * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(modulename, smiNode->oid, 0));
	}

	if ((smiNode->basetype != SMI_BASETYPE_SEQUENCE) &&
	    (smiNode->basetype != SMI_BASETYPE_SEQUENCEOF)) {
	    if (smiNode->type) {
		printSegment((2 + indent) * INDENT, "type", INDENTVALUE);
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
		    print(";\n");
		} else {
		    print("%s;\n", getTypeString(typename,
						 SMI_BASETYPE_UNKNOWN));
		}
	    }
	}

	if ((smiNode->basetype != SMI_BASETYPE_SEQUENCE) &&
	    (smiNode->basetype != SMI_BASETYPE_SEQUENCEOF)) {
	    if (smiNode->access != SMI_ACCESS_UNKNOWN) {
		printSegment((2 + indent) * INDENT, "access", INDENTVALUE);
		print("%s;\n", smingStringAccess(smiNode->access));
	    }
	}
	
	/* XXX index */
	
	/* XXX create */
	
	if (smiNode->value) {
	    printSegment((2 + indent) * INDENT, "default", INDENTVALUE);
	    print("%s", getValueString(smiNode->value));
	    print(";\n");
	}
	
	if (smiNode->format) {
	    printSegment((2 + indent) * INDENT, "format", INDENTVALUE);
	    print("\"%s\";\n",smiNode->format);
	}
	if (smiNode->units) {
	    printSegment((2 + indent) * INDENT, "units", INDENTVALUE);
	    print("\"%s\";\n", smiNode->units);
	}
	if ((smiNode->status != SMI_STATUS_CURRENT) &&
	    (smiNode->status != SMI_STATUS_UNKNOWN)) {
	    printSegment((2 + indent) * INDENT, "status", INDENTVALUE);
	    print("%s;\n", smingStringStatus(smiNode->status));
	}
	if (smiNode->description) {
	    printSegment((2 + indent) * INDENT, "description", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print(";\n");
	}
	if (smiNode->reference) {
	    printSegment((2 + indent) * INDENT, "reference", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print(";\n");
	}
	i++;
    }
    
    if (i) {
	printSegment((1 + indent) * INDENT, "", 0);
	print("};\n\n");
    }
}



static void
printNotifications(modulename)
    char	 *modulename;
{
    int		 i;
    SmiNode	 *smiNode;
    char	 **p;
    
    for(i = 0, smiNode = smiGetFirstNode(modulename);
	smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {

	if (smiNode->decl != SMI_DECL_NOTIFICATIONTYPE) {
	    continue;
	}

	if (!i) {
	    print("//\n// NOTIFICATION DEFINITIONS\n//\n\n");
	}

	printSegment(INDENT, "", 0);
	print("notification %s {\n", smiNode->name);

	if (smiNode->oid) {
	    printSegment(2 * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(modulename, smiNode->oid, 0));
	}

	if (smiNode->list) {
	    printSegment(2 * INDENT, "objects", INDENTVALUE);
	    print("(");
	    for(p = smiNode->list; *p; p++) {
		if (p != smiNode->list) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, *p);
	    }
	    print(");\n");
	}
	
	if ((smiNode->status != SMI_STATUS_CURRENT) &&
	    (smiNode->status != SMI_STATUS_UNKNOWN)) {
	    printSegment(2 * INDENT, "status", INDENTVALUE);
	    print("%s;\n", smingStringStatus(smiNode->status));
	}
	
	if (smiNode->description) {
	    printSegment(2 * INDENT, "description", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print(";\n");
	}

	if (smiNode->reference) {
	    printSegment(2 * INDENT, "reference", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print(";\n");
	}

	printSegment(INDENT, "", 0);
	print("};\n\n");
	i++;
    }
}



static void
printGroups(modulename)
    char	 *modulename;
{
    int		 i, d;
    SmiNode	 *smiNode;
    char	 **p;
    
    for (i = 0, d = 0; d < 2; d++) {
	
	for (smiNode = smiGetFirstNode(modulename);
	     smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {
	    
	    if (((d == 0) && (smiNode->decl != SMI_DECL_OBJECTGROUP)) ||
		((d == 1) && (smiNode->decl != SMI_DECL_NOTIFICATIONGROUP))) {
		continue;
	    }
	    
	    if (!i) {
		print("//\n// GROUP DEFINITIONS\n//\n\n");
	    }

	    printSegment(INDENT, "", 0);
	    print("group %s {\n", smiNode->name);
	    
	    if (smiNode->oid) {
		printSegment(2 * INDENT, "oid", INDENTVALUE);
		print("%s;\n", getOidString(modulename, smiNode->oid, 0));
	    }
	    
	    if (smiNode->decl == SMI_DECL_OBJECTGROUP) {
		printSegment(2 * INDENT, "objects", INDENTVALUE);
	    } else {
		printSegment(2 * INDENT, "notifications", INDENTVALUE);
	    }
	    print("(");
	    for(p = smiNode->list; *p; p++) {
		if (p != smiNode->list) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, *p);
	    }
	    print(");\n");
	    
	    if ((smiNode->status != SMI_STATUS_CURRENT) &&
		(smiNode->status != SMI_STATUS_UNKNOWN)) {
		printSegment(2 * INDENT, "status", INDENTVALUE);
		print("%s;\n", smingStringStatus(smiNode->status));
	    }
	    
	    if (smiNode->description) {
		printSegment(2 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString(smiNode->description);
		print(";\n");
	    }
	    
	    if (smiNode->reference) {
		printSegment(2 * INDENT, "reference", INDENTVALUE);
		print("\n");
		printMultilineString(smiNode->reference);
		print(";\n");
	    }
	    
	    printSegment(INDENT, "", 0);
	    print("};\n\n");
	    i++;
	}
    }
}



static void
printCompliances(modulename)
    char	  *modulename;
{
    int		  i;
    SmiNode	  *smiNode;
    SmiType	  *smiType;
    SmiOption	  **smiOption;
    SmiRefinement **smiRefinement;
    char	  **p;
    
    for (i = 0, smiNode = smiGetFirstNode(modulename);
	 smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {
	
	if (smiNode->decl != SMI_DECL_MODULECOMPLIANCE) {
	    continue;
	}
	    
	if (!i) {
	    print("//\n// COMPLIANCE DEFINITIONS\n//\n\n");
	}

	printSegment(INDENT, "", 0);
	print("compliance %s {\n", smiNode->name);
	    
	if (smiNode->oid) {
	    printSegment(2 * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(modulename, smiNode->oid, 0));
	}
	    
	if ((smiNode->status != SMI_STATUS_CURRENT) &&
	    (smiNode->status != SMI_STATUS_UNKNOWN)) {
	    printSegment(2 * INDENT, "status", INDENTVALUE);
	    print("%s;\n", smingStringStatus(smiNode->status));
	}
	    
	if (smiNode->description) {
	    printSegment(2 * INDENT, "description", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->description);
	    print(";\n");
	}
	    
	if (smiNode->reference) {
	    printSegment(2 * INDENT, "reference", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiNode->reference);
	    print(";\n");
	}

	if (smiNode->list) {
	    print("\n");
	    printSegment(2 * INDENT, "mandatory", INDENTVALUE);
	    print("(");
	    for(p = smiNode->list; *p; p++) {
		if (p != smiNode->list) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, *p);
	    }
	    print(");\n");
	}
	
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
	
	printSegment(INDENT, "", 0);
	print("};\n\n");
	i++;
    }
}



void
dumpSming(modulename)
    char	 *modulename;
{
    SmiModule	 *smiModule;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	print("//\n");
	print("// This module has been generated by smidump. Do not edit.\n");
	print("//\n");
	print("module %s ", smiModule->name);
	if (smiModule->object) {
	    print("%s ", smiModule->object);
	}
	print("{\n");
	print("\n");

	printImports(modulename);

	/*
	 * Module Header
	 */
	if (smiModule->object) {
	    print("//\n// MODULE META INFORMATION\n//\n\n");
	    printSegment(INDENT, "oid", INDENTVALUE);
	    print("%s;\n\n",
			 getOidString(modulename, smiModule->object, 1));
	    printSegment(INDENT, "organization", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiModule->organization);
	    print(";\n\n");
	    printSegment(INDENT, "contact", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiModule->contactinfo);
	    print(";\n\n");
	    printSegment(INDENT, "description", INDENTVALUE);
	    print("\n");
	    printMultilineString(smiModule->description);
	    print(";\n\n");
	    if (smiModule->reference) {
		printSegment(INDENT, "reference", INDENTVALUE);
		print("\n");
		printMultilineString(smiModule->reference);
		print(";\n\n");
	    }

	    printRevisions(modulename);
	    
	}
	
	printTypedefs(modulename);
	
	printObjects(modulename);
	
	printNotifications(modulename);
	
	printGroups();
	
	printCompliances();
	
	print("};\n");

    }
}

