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
 * @(#) $Id: dump-java.c,v 1.1 1999/06/15 14:09:46 strauss Exp $
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



#define INDENT  4



static int errors;



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
    int		 j;
    SmiNode	 *smiRow, *smiColumn;
    SmiIndex	 *smiIndex;
    char	 *type;
    
    for(smiRow = smiGetFirstNode(modulename, SMI_NODEKIND_ROW);
	smiRow; smiRow = smiGetNextNode(smiRow, SMI_NODEKIND_ROW)) {

	printf("public class Stub_%s {\n", smiRow->name);
	
	if (smiRow->oid) {
	    printf("// oid %s\n\n", getOidString(smiRow, 0));
	}

	/* constructor */
	printf("%*spublic Stub_%s() {\n", INDENT, "", smiRow->name);
	printf("%*s}\n\n", INDENT, "");
	
#if 0
	smiType = smiGetType(smiRow->typemodule, smiRow->typename);
	printf("%s.%s",
	       smiType->parentmodule ? smiType->parentmodule : "",
	       smiType->parentname);
	printSubtype(smiType);
	printf(";\n");
#endif

	for(smiColumn = smiGetFirstChildNode(smiRow); smiColumn;
	    smiColumn = smiGetNextChildNode(smiColumn)) {

	    type = "XXX";

	    if (smiColumn->access >= SMI_ACCESS_READ_ONLY) {
		printf("%*spublic %s get_%s() ", INDENT, "",
		       type, smiColumn->name);
		printf("throws SMException {\n");
		printf("%*sreturn connection.getRequest(\"%s.\"+index);\n",
		       2*INDENT, "", smiColumn->name);
		printf("%*s}\n\n", INDENT, "");
	    }
	    if (smiColumn->access >= SMI_ACCESS_READ_WRITE) {
		printf("%*spublic void set_%s(%s) ", INDENT, "",
		       smiColumn->name, type);
		printf("throws SMException {\n");
		printf("%*sconnection.setRequest(\"%s.\"+index);\n",
		       2*INDENT, "", smiColumn->name);
		printf("%*s}\n\n", INDENT, "");
	    }
	}




	switch (smiRow->indexkind) {
	case SMI_INDEX_INDEX:
	    if (smiRow->implied) {
		printf("index implied");
	    } else {
		printf("index");
	    }
	    printf("(");
	    for (j = 0, smiIndex = smiGetFirstIndex(smiRow); smiIndex;
		 j++, smiIndex = smiGetNextIndex(smiIndex)) {
		if (j) {
		    printf(", ");
		}
		printf("%s", smiIndex->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    printf(");\n");
	    break;
	case SMI_INDEX_AUGMENT:
	    if (smiRow->relatedname) {
		printf("augments");
		printf("%s;\n", smiRow->relatedname);
		/* TODO: non-local name if non-local */
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_REORDER:
	    if (smiRow->relatedname) {
		printf("reorders %s", smiRow->relatedname);
		/* TODO: non-local name if non-local */
		if (smiRow->implied) {
		    printf(" implied");
		}
		printf(" (");
		for (j = 0, smiIndex = smiGetFirstIndex(smiRow); smiIndex;
		     j++, smiIndex = smiGetNextIndex(smiIndex)) {
		    if (j) {
			printf(", ");
		    }
		    printf("%s", smiIndex->name);
		    /* TODO: non-local name if non-local */
		} /* TODO: empty? -> print error */
		printf(");\n");
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_SPARSE:
	    if (smiRow->relatedname) {
		printf("sparse");
		printf("%s;\n", smiRow->relatedname);
		/* TODO: non-local name if non-local */
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_EXPAND:
	    if (smiRow->relatedname) {
		printf("expands %s", smiRow->relatedname);
		/* TODO: non-local name if non-local */
		if (smiRow->implied) {
		    printf(" implied");
		}
		printf(" (");
		for (j = 0, smiIndex = smiGetFirstIndex(smiRow); smiIndex;
		     j++, smiIndex = smiGetNextIndex(smiIndex)) {
		    if (j) {
			printf(", ");
		    }
		    printf("%s", smiIndex->name);
		    /* TODO: non-local name if non-local */
		} /* TODO: empty? -> print error */
		printf(");\n");
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;
	}

	/* TODO: init columns with DEFVALs */

	printf("};\n\n");
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
	printf("//\n");
	/* XXX: hyphens in module names allowed in package/class names? */
	printf("package %s\n", getJavaIdentifier(smiModule->name));
	printf("\n");
	printf("import jmgmt.snmp.io.*;\n");
	printf("import java.io.IOException;\n");
	printf("\n");

	printObjects(smiModule->name);
	
	printf("// end of file %s.\n", smiModule->name);

    }

    return errors;
}

