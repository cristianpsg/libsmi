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
 * @(#) $Id: dump-sming.c,v 1.7 1999/03/24 16:25:31 strauss Exp $
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

#include "smi.h"



char *excludeDescriptors[] = {
    "ObjectName", "NotificationName", "ObjectSyntax", "SimpleSyntax",
    "Integer32", "ApplicationSyntax", "IpAddress", "Counter32", "Gauge32",
    "Unsigned32", "TimeTicks", "Opaque", "Counter64",
    NULL };

char *smi2smingTypes[] = {
    "INTEGER", "Integer32",
    "OCTET STRING", "OctetString",
    "OBJECT IDENTIFIER", "ObjectIdentifier",
    NULL, NULL };



static char *
getSmingType(name, syntax)
    char *name;
{
    int i;

    if (syntax == SMI_SYNTAX_ENUM) {
	return "Enumeration";
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
printSmiValue(valuePtr)
    SmiValue  *valuePtr;
{
    switch (valuePtr->syntax) {
    case SMI_SYNTAX_UNSIGNED32:
    case SMI_SYNTAX_OCTETSTRING:
	printf("%lu", valuePtr->value.unsigned32);
	break;
    case SMI_SYNTAX_INTEGER32:
    case SMI_SYNTAX_GAUGE32:
	printf("%ld", valuePtr->value.integer32);
	break;
    case SMI_SYNTAX_UNSIGNED64:
	printf("%llu", valuePtr->value.unsigned64);
	break;
    case SMI_SYNTAX_INTEGER64:
    case SMI_SYNTAX_GAUGE64:
	printf("%lld", valuePtr->value.integer64);
	break;
    case SMI_SYNTAX_FLOAT32:
    case SMI_SYNTAX_FLOAT64:
    case SMI_SYNTAX_FLOAT128:
		
    case SMI_SYNTAX_UNKNOWN:
    case SMI_SYNTAX_IPADDRESS:
    case SMI_SYNTAX_COUNTER32:
    case SMI_SYNTAX_COUNTER64:
    case SMI_SYNTAX_TIMETICKS:
    case SMI_SYNTAX_OPAQUE:
    case SMI_SYNTAX_CHOICE:
    case SMI_SYNTAX_OBJECTIDENTIFIER:
    case SMI_SYNTAX_SEQUENCE:
    case SMI_SYNTAX_SEQUENCEOF:
    case SMI_SYNTAX_ENUM:
    case SMI_SYNTAX_BITS:

    case SMI_SYNTAX_BIN:
    case SMI_SYNTAX_HEX:
    case SMI_SYNTAX_STRING:
	break;
    }
}



static void
printSubtype(smiType)
    SmiType *smiType;
{
    void    **listPtr;

    printf("(");
    for (listPtr = smiType->list; *listPtr; listPtr++) {
	if ((smiType->syntax == SMI_SYNTAX_ENUM) ||
	    (smiType->syntax == SMI_SYNTAX_BITS)) {
	    if (listPtr != smiType->list) printf(", ");
	    printf("%s(", ((SmiNamedNumber *)*listPtr)->name);
	    printSmiValue(((SmiNamedNumber *)*listPtr)->valuePtr);
	    printf(")");
	} else {
	    if (listPtr != smiType->list) printf(" | ");
	    printSmiValue(((SmiRange *)*listPtr)->minValuePtr);
	    if (bcmp(((SmiRange *)*listPtr)->minValuePtr,
		     ((SmiRange *)*listPtr)->maxValuePtr,
		     sizeof(SmiValue))) {
		printf("..");
		printSmiValue(((SmiRange *)*listPtr)->maxValuePtr);
	    }
	}
    }
    printf(")");
}



static void
printMultilineString(s)
    const char *s;
{
    int i;
    
    printf("       \"");
    if (s) {
	for (i=0; i < strlen(s); i++) {
	    if (s[i] != '\n') {
		printf("%c", s[i]);
	    } else {
		printf("\n");
		printf("        ");
	    }
	}
    }
    printf("\"");
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
    
    list = smiGetMembers(modulename, "");

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
	    importedModulename = "IETF-SMING-BASE";
	}
	if (!strcmp(importedModulename, "SNMPv2-TC")) {
	    convert = 1;
	    importedModulename = "IETF-SMING-TYPES";
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
	}

	if (strlen(importedDescriptor)) {
	    if ((!lastModulename) ||
		strcmp(importedModulename, lastModulename)) {
		if (lastModulename) {
		    printf("\n    );\n");
		} else {
		    printf("//\n// IDENTIFIER IMPORTS\n//\n\n");
		}
		printf("    import %s (\n        ", importedModulename);
		lastModulename = importedModulename;
	    } else {
		printf(",\n        ");
	    }
	    printf("%s", importedDescriptor);
	}
    }
    if (lastModulename) {
	printf("\n    );\n\n");
    }
}



void
printRevisions(modulename)
    char	 *modulename;
{
    int i;
    SmiRevision *smiRevision;
    
    for(i = 0, smiRevision = smiGetFirstRevision(modulename);
	smiRevision;
	smiRevision = smiGetNextRevision(modulename,
					 smiRevision->date)) {
	printf("    revision {\n");
	printf("        date            \"%s\";\n",
	       smiCTime(smiRevision->date));
	printf("        description     \n");
	printMultilineString(smiRevision->description);
	printf(";\n    };\n");
	i++;
    }
    if (i) {
	printf("\n");
    }
}



void
printTypedefs(modulename)
    char	 *modulename;
{
    int		 i;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(modulename, smiType->name)) {
	if (!i) {
	    printf("//\n// TYPE DEFINITIONS\n//\n\n");
	}
	printf("    typedef %s {\n", smiType->name);

	if (!smiType->parent) { printf("XXX\n"); exit(1); }
	printf("        type            %s",
	       getSmingType(smiType->parent, smiType->syntax));
	if (smiType->list) {
	    printf(" ");
	    printSubtype(smiType);
	}
	printf(";\n");

	if (smiType->value) {
	    printf("        default         ");
	    printSmiValue(smiType->value);
	    printf(";\n");
	}
	
	if (smiType->format) {
	    printf("        format          \"%s\";\n", smiType->format);
	}
	if (smiType->units) {
	    printf("        units           \"%s\";\n", smiType->units);
	}
	if (smiType->status != SMI_STATUS_CURRENT) {
	    printf("        status          %s;\n",
		   smingStringStatus(smiType->status));
	}
	printf("        description     \n");
	printMultilineString(smiType->description);
	printf(";\n");
	if (smiType->reference) {
	    printf("        reference       \n");
	    printMultilineString(smiType->reference);
	    printf(";\n");
	}
	printf("    };\n\n");
	i++;
    }
}



void
printObjects(modulename)
    char	 *modulename;
{
    int		 i;
    SmiNode	 *smiNode;
    
    for(i = 0, smiNode = smiGetFirstNode(modulename);
	smiNode; smiNode = smiGetNextNode(modulename, smiNode->name)) {
	if (!i) {
	    printf("//\n// OBJECT DEFINITIONS\n//\n\n");
	}

	switch (smiNode->decl) {
	case SMI_DECL_VALUEASSIGNMENT:
	    printf("    node %s {\n", smiNode->name);
	    break;
	case SMI_DECL_OBJECTTYPE:
	    printf("    scalar %s {\n", smiNode->name);
	    break;
	default:
	    printf("    any-gaga %s {\n", smiNode->name);
	    break;
	}

	if (smiNode->oid) {
	    printf("        oid             %s;\n",
		   getOidString(modulename, smiNode->oid, 0));
	}

	if (smiNode->type) {
	    printf("        type            %s",
		   getSmingType(smiNode->type, smiNode->syntax));
	    printf(";\n");
	}

	if (smiNode->value) {
	    printf("        default         ");
	    printSmiValue(smiNode->value);
	    printf(";\n");
	}
	
	if (smiNode->format) {
	    printf("        format          \"%s\";\n", smiNode->format);
	}
	if (smiNode->units) {
	    printf("        units           \"%s\";\n", smiNode->units);
	}
	if ((smiNode->status != SMI_STATUS_CURRENT) &&
	    (smiNode->status != SMI_STATUS_UNKNOWN)) {
	    printf("        status          %s;\n",
		   smingStringStatus(smiNode->status));
	}
	if (smiNode->description) {
	    printf("        description     \n");
	    printMultilineString(smiNode->description);
	    printf(";\n");
	}
	if (smiNode->reference) {
	    printf("        reference       \n");
	    printMultilineString(smiNode->reference);
	    printf(";\n");
	}
	printf("    };\n\n");
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
	printf("//\n");
	printf("// This module has been generated by smidump. Do not edit.\n");
	printf("//\n");
	printf("module %s ", smiModule->name);
	if (smiModule->object) {
	    printf("%s ", smiModule->object);
	}
	printf("{\n");
	printf("\n");

	printImports(modulename);

	/*
	 * Module Header
	 */
	if (smiModule->object) {
	    printf("//\n// MODULE META INFORMATION\n//\n\n");
	    printf("    oid                 %s;\n",
		   getOidString(modulename, smiModule->object, 1));
	    printf("\n");
	    printf("    organization        \n");
	    printMultilineString(smiModule->organization);
	    printf(";\n");
	    printf("\n");
	    printf("    contact             \n");
	    printMultilineString(smiModule->contactinfo);
	    printf(";\n");
	    printf("\n");
	    printf("    description         \n");
	    printMultilineString(smiModule->description);
	    printf(";\n");
	    printf("\n");
	    if (smiModule->reference) {
		printf("    reference           \n");
		printMultilineString(smiModule->reference);
		printf(";\n");
		printf("\n");
	    }

	    printRevisions(modulename);
	    
	}
	
	printTypedefs(modulename);
	
	printObjects(modulename);
	
	/* printNotifications(); */
	/* printGroups(); */
	/* printCompliances(); */	
	printf("};\n");
    }
}
