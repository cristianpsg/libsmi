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
 * @(#) $Id: dump-sming.c,v 1.5 1999/03/17 19:09:11 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "smi.h"



char *excludeDescriptors[] = {
    "ObjectName", "NotificationName", "ObjectSyntax", "SimpleSyntax",
    "Integer32", "ApplicationSyntax", "IpAddress", "Counter32", "Gauge32",
    "Unsigned32", "TimeTicks", "Opaque", "Counter64",
    NULL };


static char *
getOidString(modulename, object, importedParent)
    char	 *modulename;
    char	 *object;
    int		 importedParent;  /* force use of imported oid label (MI) */
{
    smi_node     *smiNode;
    static char	 s[SMI_MAX_OID+1];
    char	 child[SMI_MAX_OID+1];
    char	 append[SMI_MAX_OID+1];
    smi_fullname smiFullname;
    char	 *p;
    
    s[0] = 0;

    if (importedParent) {
	strcpy(child, object);
	append[0] = 0;
	do {
	    smiFullname = smiGetParent(child, modulename);
	    if (smiFullname) {
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
	    strcpy(child, smiDescriptor(smiFullname));
	} while ((!smiIsImported(modulename, smiFullname)) &&
		 (strchr(smiDescriptor(smiFullname), '.')));
	sprintf(s, "%s%s", smiDescriptor(smiFullname), append);
    } else {
	smiNode = smiGetNode(object, modulename);
	if (smiNode) {
	    strcpy(s, smiNode->oid);
	}
    }
    return s;
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
    smi_namelist  *smiNamelist;
    char	  *lastModulename = NULL;
    char	  *importedModulename, *importedDescriptor, *p;
    int		  convert, i;
    
    smiNamelist = smiGetMembers(modulename, "");

    for(p = strtok(smiNamelist->namelist, " "); p; p = strtok(NULL, " ")) {
	importedModulename = p;
	importedDescriptor = strstr(p, SMI_NAMESPACE_OPERATOR);
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
    smi_revision *smiRevision;
    
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
    int i;
    smi_type *smiType;
    
    for(i = 0, smiType = smiGetFirstType(modulename);
	smiType;
	smiType = smiGetNextType(modulename, smiType->name)) {
	if (!i) {
	    printf("//\n// TYPE DEFINITIONS\n//\n\n");
	}
	printf("    typedef %s {\n", smiType->name);
	/* TODO: */
	printf("     // type            ...;\n");
	printf("     // syntax          %s;\n",
	       smiStringSyntax(smiType->syntax));
	/* TODO: */
	printf("     // default         ...;\n");
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
	if (smiType->reference) {
	    printf("        reference       \n");
	    printMultilineString(smiType->reference);
	}
	printf(";\n    };\n\n");
	i++;
    }
}



void
dumpSming(modulename)
    char	 *modulename;
{
    smi_module	 *smiModule;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	printf("//\n");
	printf("// This module has been generated by smidump. Do not edit.\n");
	printf("//\n");
	printf("module %s ", smiModule->name);
	if (strlen(smiModule->object)) {
	    printf("%s ", smiModule->object);
	}
	printf("{\n");
	printf("\n");

	printImports(modulename);

	/*
	 * Module Header
	 */
	if (strlen(smiModule->object)) {
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
	    if (strlen(smiModule->reference)) {
		printf("    reference           \n");
		printMultilineString(smiModule->reference);
		printf(";\n");
		printf("\n");
	    }

	    printRevisions(modulename);
	    
	}
	
	printTypedefs(modulename);
	
	/* printObjects(); */
	/* printNotifications(); */
	/* printGroups(); */
	/* printCompliances(); */	
	printf("};\n");
    }
}
