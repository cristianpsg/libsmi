/*
 * dump-jdmk.c --
 *
 *      Operations to dump Java class templates from use with JDMK.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-jdmk.c,v 1.1 1999/10/05 15:52:21 strauss Exp $
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
#include <time.h>

#include "smi.h"
#include "smidump.h"



static int errors;



static void printPrefixed(const char *prefix, const char *s)
{
    int i;

    printf("%s", prefix);
    if (s) {
	for (i=0; i < strlen(s); i++) {
	    if (s[i] != '\n') {
		printf("%c", s[i]);
	    } else {
		printf("\n");
		printf("%s", prefix);
	    }
	}
    }
    printf("\n");
}



static char *getJavaIdentifier(char *s)
{
    static char ss[100];
    int         i;
    
    strcpy(ss, s);

    for(i=0; i < strlen(ss); i++) {
	if (ss[i] == '-') ss[i] = '_';
    }

    return ss;
}


static char *getUpper(char *s)
{
    static char ss[100];
    
    strcpy(ss, s);
    ss[0] = toupper(ss[0]);

    return ss;
}


static char *getOidString(SmiNode *smiNode, int importedParent)
{
    SmiNode	 *parentNode, *node;
    static char	 s[200];
    char	 append[200];
    unsigned int i;
    
    append[0] = 0;

    parentNode = smiNode;

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
	if (node != smiNode) {
	    smiFreeNode(node);
	}

	if (!parentNode) {
	    sprintf(s, "%s", append);
	    return s;
	}
	
	/* found an imported or a local parent node? */
	if ((parentNode->name && strlen(parentNode->name)) &&
	    (smiIsImported(smiNode->module,
			   parentNode->module, parentNode->name) ||
	     (!importedParent &&
	      !strcmp(parentNode->module, smiNode->module)))) {
	    sprintf(s, "%s%s", parentNode->name, append);
	    smiFreeNode(parentNode);
	    return s;
	}
	
    } while (parentNode);

    /* smiFreeNode(parentNode); */
    s[0] = 0;
    for (i=0; i < smiNode->oidlen; i++) {
	if (i) strcat(s, ".");
	sprintf(&s[strlen(s)], "%u", smiNode->oid[i]);
    }
    return s;
}



static char *getValueString(SmiValue *valuePtr)
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
	sprintf(s, "%s", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_OCTETSTRING:
	sprintf(s, "\"%s\"", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	if (valuePtr->value.bits) {
	    for(p = valuePtr->value.bits; *p; p++) {
		if (p != valuePtr->value.bits)
		    sprintf(&s[strlen(s)], ", ");
		sprintf(&s[strlen(s)], "%s", *p);
	    }
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	/* TODO */
	break;
    }

    return s;
}



static void printSubtype(SmiType *smiType)
{
    SmiRange       *range;
    SmiNamedNumber *nn;
    int		   i;

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType->module, smiType->name);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (i) {
		printf(", ");
	    } else {
		printf(" (");
	    }
	    printf("%s(%s)", nn->name, getValueString(nn->valuePtr));
	}
	if (i) {
	    printf(")");
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType->module, smiType->name);
	    range ; i++, range = smiGetNextRange(range)) {
	    if (i) {
		printf(" | ");
	    } else {
		printf(" (");
	    }	    
	    if (bcmp(range->minValuePtr, range->maxValuePtr,
		     sizeof(SmiValue))) {
		printf("%s", getValueString(range->minValuePtr));
		printf("..%s",  getValueString(range->maxValuePtr));
	    } else {
		printf("%s", getValueString(range->minValuePtr));
	    }
	}
	if (i) {
	    printf(")");
	}
    }
}



static void printObjects(char *modulename)
{
    SmiNode	   *smiTable, *smiRow, *smiColumn;
    char	   *type;

    /*
     * Create classes for all tables.
     */
    for(smiTable = smiGetFirstNode(modulename, SMI_NODEKIND_TABLE);
	smiTable; smiTable = smiGetNextNode(smiTable, SMI_NODEKIND_TABLE)) {

	smiRow = smiGetFirstChildNode(smiTable);
	if (!smiRow) {
	    continue;
	}
	
	printf("\n/**\n");
	printf(" * Derived from MIB table %s.%s.\n",
	       modulename, smiTable->name);
	printf(" *\n");
	printPrefixed(" * ", smiTable->description);
	printf(" *\n");
	printPrefixed(" * ", smiRow->description);
	printf(" */\n");
	printf("public class %s {\n", getUpper(smiRow->name));
	printf("\n");

	/* constructor */
	printf("    // constructor \n");
	printf("    public %s() {\n", getUpper(smiRow->name));
	printf("    }\n\n");

	for(smiColumn = smiGetFirstChildNode(smiRow); smiColumn;
	    smiColumn = smiGetNextChildNode(smiColumn)) {

	    if (smiColumn->typename) {
		type = smiColumn->typename;
	    } else {
		switch (smiColumn->basetype) {
		case SMI_BASETYPE_INTEGER32:
		    type = "SnmpInt";
		    break;
		case SMI_BASETYPE_OCTETSTRING:
		    type = "SnmpString";
		    break;
		case SMI_BASETYPE_OBJECTIDENTIFIER:
		    type = "SnmpOid";
		    break;
		case SMI_BASETYPE_UNSIGNED32:
		    type = "SnmpUnsignedInt"; /* TODO: or Counter or Gauge? */
		    break;
		case SMI_BASETYPE_INTEGER64:
		    type = "SnmpCounter64"; /* TODO */
		    break;
		case SMI_BASETYPE_UNSIGNED64:
		    type = "SnmpCounter64"; /* TODO */
		    break;
		case SMI_BASETYPE_FLOAT32:
		case SMI_BASETYPE_FLOAT64:
		case SMI_BASETYPE_FLOAT128:
		    type = "TODO";
		    break;
		case SMI_BASETYPE_ENUM:
		    type = "int";
		    break;
		case SMI_BASETYPE_BITS:
		    type = "int";
		    break;
		default:
		    type = "XXX";
		    break;
		}
	    }
	    
	    printf("    /**\n");
	    printf("     * Derived from MIB table column %s.%s.\n",
		   modulename, smiColumn->name);
	    printf("     *\n");
	    printPrefixed("     * ", smiColumn->description);
	    printf("     */\n");
	    
	    printf("    public SnmpVar %s;\n", getUpper(smiColumn->name));

	    printf("\n");
	}

	printf("}\n\n");
    }
    
}



int dumpJdmk(char *modulename, int flags)
{
    SmiModule	 *smiModule;
    
    errors = 0;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smilint: cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	printf("//\n");
	printf("// This file has been generated by smidump "
	      VERSION ". Do not edit.\n");
	printf("// It is based on the MIB module %s.\n", smiModule->name);
	printf("//\n\n");
	/* XXX: hyphens in module names allowed in package/class names? */
	printf("package %s;\n", getJavaIdentifier(smiModule->name));
	printf("\n");
	printf("import java.io.*;\n");
	printf("import java.net.*;\n");
	printf("import java.util.*;\n");
	printf("import com.sun.jaw.snmp.common.*;\n");
	printf("import com.sun.jaw.snmp.manager.*;\n");
	printf("\n\n");

	printObjects(smiModule->name);
	
    }

    return errors;
}

