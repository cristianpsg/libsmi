/*
 * dump-java.c --
 *
 *      Operations to dump Java class templates from MIB module information.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-java.c,v 1.2 1999/06/15 18:57:52 strauss Exp $
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
#include "util.h"



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
    static char	 s[SMI_MAX_OID+1];
    char	 append[SMI_MAX_OID+1];
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
    case SMI_BASETYPE_LABEL:
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
    case SMI_BASETYPE_CHOICE:
    case SMI_BASETYPE_SEQUENCE:
    case SMI_BASETYPE_SEQUENCEOF:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	/* TODO */
	break;
    case SMI_BASETYPE_BINSTRING:
	/* TODO */
	break;
    case SMI_BASETYPE_HEXSTRING:
	sprintf(s, "0x%s", valuePtr->value.ptr);
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
    SmiNode	   *smiNode, *smiTable, *smiRow, *smiColumn;
    SmiType	   *smiType;
    SmiNamedNumber *smiNamedNumber;
    char	   *type, *prefix, *postfix;

    /*
     * Create classes for all inlined enumerations.
     */
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	if ((smiNode->basetype == SMI_BASETYPE_ENUM) &&
	    smiNode->typename &&
	    islower((int)smiNode->typename[0]) &&
	    (smiType = smiGetType(smiNode->typemodule, smiNode->typename))) {

	    printf("\n/**\n");
	    printf(" * Derived from type of MIB node %s.%s.\n",
		   modulename, smiNode->name);
	    printf(" *\n");
	    printPrefixed(" * ", smiNode->description);
	    printf(" */\n");
	    printf("public class %s {\n", smiNode->name);
	    printf("    private long value;\n\n");
	    for(smiNamedNumber = smiGetFirstNamedNumber(smiType->module,
							smiType->name);
		smiNamedNumber;
		smiNamedNumber = smiGetNextNamedNumber(smiNamedNumber)) {
		printf("    public static final long %s = %lu;\n",
		       smiNamedNumber->name,
		       smiNamedNumber->valuePtr->value.unsigned32);
	    }
	    printf("\n");
	    printf("    /**\n");
	    printf("     * Sets the value.\n");
	    printf("     */\n");
	    printf("    public void set(long newValue) {\n");
	    printf("        value = newValue;\n");
	    printf("    }\n\n");
	    printf("    /**\n");
	    printf("     * Returns the numerical value.\n");
	    printf("     */\n");
	    printf("    public long toLong() {\n");
	    printf("        return value;\n");
	    printf("    }\n\n");
	    printf("    /**\n");
	    printf("     * Returns the named value.\n");
	    printf("     */\n");
	    printf("    public String toString() {\n");
	    printf("        switch (value) {\n");
	    for(smiNamedNumber = smiGetFirstNamedNumber(smiType->module,
							smiType->name);
		smiNamedNumber;
		smiNamedNumber = smiGetNextNamedNumber(smiNamedNumber)) {
		printf("            case %s: return \"%s\";\n",
		       smiNamedNumber->name, smiNamedNumber->name);
	    }
	    printf("        }\n");
	    printf("    }\n\n");
	    
	    printf("}\n\n");
	}
    }

    
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
	printf("public class Stub_%s {\n", smiRow->name);
	printf("\n");

	/* constructor */
	printf("    public Stub_%s() {\n", smiRow->name);
	printf("    }\n\n");
	
	for(smiColumn = smiGetFirstChildNode(smiRow); smiColumn;
	    smiColumn = smiGetNextChildNode(smiColumn)) {

	    prefix = "";
	    postfix = "";
	    
	    switch (smiColumn->basetype) {
	    case SMI_BASETYPE_INTEGER32:
		type = "int";
		break;
	    case SMI_BASETYPE_OCTETSTRING:
		type = "String";
		break;
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		type = "int[]";
		prefix = "SMUtil.dottedToIntArray(";
		postfix = ")";
		break;
	    case SMI_BASETYPE_UNSIGNED32:
	    case SMI_BASETYPE_INTEGER64:
	    case SMI_BASETYPE_UNSIGNED64:
		type = "long";
		break;
	    case SMI_BASETYPE_FLOAT32:
		type = "float";
		break;
	    case SMI_BASETYPE_FLOAT64:
	    case SMI_BASETYPE_FLOAT128:
		type = "double";
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
	    
	    if (smiColumn->access >= SMI_ACCESS_READ_ONLY) {
		printf("    /**\n");
		printf("     * Derived from MIB table column %s.%s.\n",
		       modulename, smiColumn->name);
		printf("     *\n");
		printPrefixed("     * ", smiColumn->description);
		printf("     */\n");
		
		printf("    public %s get%s() ", 
		       type, getUpper(smiColumn->name));
		printf("throws SMException {\n");
		printf("        return %sconnection.getRequest(\"%s.\"+index)%s;\n",
		       prefix, smiColumn->name, postfix);
		printf("    }\n");
	    }
	    if (smiColumn->access >= SMI_ACCESS_READ_WRITE) {
		printf("    public void set%s(%s) ",
		       getUpper(smiColumn->name), type);
		printf("throws SMException {\n");
		printf("        connection.setRequest(\"%s.\"+index);\n",
		       smiColumn->name);
		printf("    }\n");
	    }
	    printf("\n");
	}

	/* TODO: init columns with DEFVALs */

	printf("}\n\n");
    }
    
}



int dumpJava(char *modulename)
{
    SmiModule	 *smiModule;
    
    errors = 0;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
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
	printf("import jmgmt.snmp.io.*;\n");
	printf("import java.io.IOException;\n");
	printf("\n\n");

	printObjects(smiModule->name);
	
    }

    return errors;
}

