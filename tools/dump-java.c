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
 * @(#) $Id$
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



#define  INDENT		2    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	9   /* column to start multiline texts */
#define  INDENTMAX	64   /* max column to fill, break lines otherwise */



static int errors;



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



static void print(char *fmt, ...)
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



static void printSegment(int column, char *string, int length)
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



static void printWrapped(int column, char *string)
{
    if ((current_column + strlen(string)) > INDENTMAX) {
	print("\n");
	printSegment(column, "", 0);
    }
    print("%s", string);
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
    char	   s[100];
    int		   i;

    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType->module, smiType->name);
	    nn ; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (i) {
		print(", ");
	    } else {
		print(" (");
	    }
	    sprintf(s, "%s(%s)", nn->name, getValueString(nn->valuePtr));
	    printWrapped(INDENTVALUE + INDENT, s);
	}
	if (i) {
	    print(")");
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType->module, smiType->name);
	    range ; i++, range = smiGetNextRange(range)) {
	    if (i) {
		print(" | ");
	    } else {
		print(" (");
	    }	    
	    if (bcmp(range->minValuePtr, range->maxValuePtr,
		     sizeof(SmiValue))) {
		sprintf(s, "%s", getValueString(range->minValuePtr));
		sprintf(&s[strlen(s)], "..%s", 
			getValueString(range->maxValuePtr));
	    } else {
		sprintf(s, "%s", getValueString(range->minValuePtr));
	    }
	    printWrapped(INDENTVALUE + INDENT, s);
	}
	if (i) {
	    print(")");
	}
    }
}



static void printMultilineString(const char *s)
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



static void printImports(char *modulename)
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
    
    for(smiImport = smiGetFirstImport(modulename); smiImport;
	smiImport = smiGetNextImport(smiImport)) {
	importedModulename = smiImport->importmodule;
	importedDescriptor = smiImport->importname;

	/*
	 * imported SMI modules have to be handled more carefully:
	 * The module name is mapped to an SMIng module and some definitions
	 * are stripped off (namely macros). Also note, that this may
	 * lead to an empty list of imported descriptors from that SMIv1/v2
	 * module.
	 */
	for(i = 0; convertImport[i]; i += 4) {
	    if ((!util_strcmp(importedModulename, convertImport[i])) &&
		(!util_strcmp(importedDescriptor, convertImport[i+1]))) {
		importedModulename = convertImport[i+2];
		importedDescriptor = convertImport[i+3];
		/* TODO: hide duplicates */
		break;
	    } else if ((!util_strcmp(importedModulename, convertImport[i])) &&
		       (!convertImport[i+1])) {
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



static void printRevisions(char *modulename)
{
    int i;
    SmiRevision *smiRevision;
    
    for(i = 0, smiRevision = smiGetFirstRevision(modulename);
	smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	printSegment(INDENT, "revision {\n", 0);
	printSegment(2 * INDENT, "date", INDENTVALUE);
	print("\"%s\";\n", smingCTime(smiRevision->date));
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



static void printTypedefs(char *modulename)
{
    int		 i, j;
    SmiType	 *smiType;
    
    for(i = 0, smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(smiType)) {
	
	if ((!(strcmp(modulename, "SNMPv2-SMI"))) ||
	    (!(strcmp(modulename, "RFC1155-SMI")))) {
	    for(j=0; excludeType[j]; j++) {
		if (!strcmp(smiType->name, excludeType[j])) break;
	    }
	    if (excludeType[j]) break;
	}
	    
	if (!i) {
	    print("//\n// TYPE DEFINITIONS\n//\n\n");
	}
	printSegment(INDENT, "", 0);
	print("typedef %s {\n", smiType->name);

	printSegment(2 * INDENT, "type", INDENTVALUE);
	print("%s", getTypeString(modulename, smiType->basetype,
				  smiType->parentmodule, smiType->parentname));
	printSubtype(smiType);
	print(";\n");

	if (smiType->valuePtr) {
	    printSegment(2 * INDENT, "default", INDENTVALUE);
	    print("%s", getValueString(smiType->valuePtr));
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
	if ((smiType->status != SMI_STATUS_CURRENT) &&
	    (smiType->status != SMI_STATUS_UNKNOWN) &&
	    (smiType->status != SMI_STATUS_MANDATORY) &&
	    (smiType->status != SMI_STATUS_OPTIONAL)) {
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



static void printObjects(char *modulename)
{
    int		 i, j;
    SmiNode	 *smiNode, *smiParentNode;
    SmiIndex     *smiIndex;
    SmiType	 *smiType;
    int		 indent = 0;
    int		 lastindent = -1;
    char	 *s;
    
    for(i = 0, smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	smiParentNode = smiGetParentNode(smiNode);
	
	if (smiNode->nodekind == SMI_NODEKIND_NODE) {
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
	} else {
	    continue;
	}

	if (!i) {
	    print("//\n// OBJECT DEFINITIONS\n//\n\n");
	}

	for (j = lastindent; j >= indent; j--) {
	    printSegment((1 + j) * INDENT, "", 0);
	    print("};\n");
	}
	print("\n");
	lastindent = indent;
	
	printSegment((1 + indent) * INDENT, "", 0);
	print("%s %s {\n", s, smiNode->name);
	
	if (smiNode->oid) {
	    printSegment((2 + indent) * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(smiNode, 0));
	}

	if ((smiNode->basetype != SMI_BASETYPE_SEQUENCE) &&
	    (smiNode->basetype != SMI_BASETYPE_SEQUENCEOF)) {
	    if (smiNode->typename) {
		printSegment((2 + indent) * INDENT, "type", INDENTVALUE);
		if (islower((int)smiNode->typename[0])) {
		    /*
		     * an implicitly restricted type.
		     */
		    smiType = smiGetType(smiNode->typemodule,
					 smiNode->typename);
		    print("%s", getTypeString(modulename, smiType->basetype,
					      smiType->parentmodule,
					      smiType->parentname));
		    printSubtype(smiType);
		    print(";\n");
		} else {
		    print("%s;\n", getTypeString(modulename, smiNode->basetype,
						 smiNode->typemodule,
						 smiNode->typename));
		}
	    }
	}

	if ((smiNode->nodekind != SMI_NODEKIND_TABLE) &&
	    (smiNode->nodekind != SMI_NODEKIND_ROW) &&
	    (smiNode->nodekind != SMI_NODEKIND_NODE)) {
	    if (smiNode->access != SMI_ACCESS_UNKNOWN) {
		printSegment((2 + indent) * INDENT, "access", INDENTVALUE);
		print("%s;\n", smingStringAccess(smiNode->access));
	    }
	}

	switch (smiNode->indexkind) {
	case SMI_INDEX_INDEX:
	    if (smiNode->implied) {
		printSegment((2 + indent) * INDENT, "index implied",
			     INDENTVALUE);
	    } else {
		printSegment((2 + indent) * INDENT, "index", INDENTVALUE);
	    }
	    print("(");
	    for (j = 0, smiIndex = smiGetFirstIndex(smiNode); smiIndex;
		 j++, smiIndex = smiGetNextIndex(smiIndex)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, smiIndex->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(");\n");
	    break;
	case SMI_INDEX_AUGMENT:
	    if (smiNode->relatedname) {
		printSegment((2 + indent) * INDENT, "augments", INDENTVALUE);
		print("%s;\n", smiNode->relatedname);
		/* TODO: non-local name if non-local */
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_REORDER:
	    if (smiNode->relatedname) {
		printSegment((2 + indent) * INDENT, "", 0);
		print("reorders %s", smiNode->relatedname);
		/* TODO: non-local name if non-local */
		if (smiNode->implied) {
		    print(" implied");
		}
		print(" (");
		for (j = 0, smiIndex = smiGetFirstIndex(smiNode); smiIndex;
		     j++, smiIndex = smiGetNextIndex(smiIndex)) {
		    if (j) {
			print(", ");
		    }
		    printWrapped(INDENTVALUE + 1, smiIndex->name);
		    /* TODO: non-local name if non-local */
		} /* TODO: empty? -> print error */
		print(");\n");
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_SPARSE:
	    if (smiNode->relatedname) {
		printSegment((2 + indent) * INDENT, "sparse", INDENTVALUE);
		print("%s;\n", smiNode->relatedname);
		/* TODO: non-local name if non-local */
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_EXPAND:
	    if (smiNode->relatedname) {
		printSegment((2 + indent) * INDENT, "", 0);
		print("expands %s", smiNode->relatedname);
		/* TODO: non-local name if non-local */
		if (smiNode->implied) {
		    print(" implied");
		}
		print(" (");
		for (j = 0, smiIndex = smiGetFirstIndex(smiNode); smiIndex;
		     j++, smiIndex = smiGetNextIndex(smiIndex)) {
		    if (j) {
			print(", ");
		    }
		    printWrapped(INDENTVALUE + 1, smiIndex->name);
		    /* TODO: non-local name if non-local */
		} /* TODO: empty? -> print error */
		print(");\n");
	    } /* TODO: else print error */
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;
	}
	
	if (smiNode->create) {
	    printSegment((2 + indent) * INDENT, "create", INDENTVALUE);
	    /* TODO: create list */
	    print(";\n");
	}
	
	if (smiNode->valuePtr) {
	    printSegment((2 + indent) * INDENT, "default", INDENTVALUE);
	    print("%s", getValueString(smiNode->valuePtr));
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
	    (smiNode->status != SMI_STATUS_UNKNOWN) &&
	    (smiNode->status != SMI_STATUS_MANDATORY) &&
	    (smiNode->status != SMI_STATUS_OPTIONAL)) {
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



static void printNotifications(char *modulename)
{
    int		 i, j;
    SmiNode	 *smiNode, *object;
    
    for(i = 0, smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	if (smiNode->nodekind != SMI_NODEKIND_NOTIFICATION) {
	    continue;
	}

	if (!i) {
	    print("//\n// NOTIFICATION DEFINITIONS\n//\n\n");
	}

	printSegment(INDENT, "", 0);
	print("notification %s {\n", smiNode->name);

	if (smiNode->oid) {
	    printSegment(2 * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(smiNode, 0));
	}

	if ((object = smiGetFirstMemberNode(smiNode))) {
	    printSegment(2 * INDENT, "objects", INDENTVALUE);
	    print("(");
	    for (j = 0; object;
		 j++, object = smiGetNextMemberNode(smiNode, object)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, object->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(");\n");
	}
	
	if ((smiNode->status != SMI_STATUS_CURRENT) &&
	    (smiNode->status != SMI_STATUS_UNKNOWN) &&
	    (smiNode->status != SMI_STATUS_MANDATORY) &&
	    (smiNode->status != SMI_STATUS_OPTIONAL)) {
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



static void printGroups(char *modulename)
{
    int		 i, d, j;
    SmiNode	 *smiNode, *member;
    
    for (i = 0, d = 0; d < 3; d++) {
	
	for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	    smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {

	    if (smiNode->nodekind != SMI_NODEKIND_GROUP) {
		continue;
	    }
	    
	    if (!i) {
		print("//\n// GROUP DEFINITIONS\n//\n\n");
	    }

	    printSegment(INDENT, "", 0);
	    print("group %s {\n", smiNode->name);
	    
	    if (smiNode->oid) {
		printSegment(2 * INDENT, "oid", INDENTVALUE);
		print("%s;\n", getOidString(smiNode, 0));
	    }
	    
	    printSegment(2 * INDENT, "members", INDENTVALUE);
	    print("(");
	    for (j = 0, member = smiGetFirstMemberNode(smiNode); member;
		 j++, member = smiGetNextMemberNode(smiNode, member)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, member->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(");\n");
	    
	    if ((smiNode->status != SMI_STATUS_CURRENT) &&
		(smiNode->status != SMI_STATUS_UNKNOWN) &&
		(smiNode->status != SMI_STATUS_MANDATORY) &&
		(smiNode->status != SMI_STATUS_OPTIONAL)) {
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



static void printCompliances(char *modulename)
{
    int		  i, j;
    SmiNode	  *smiNode, *mandatory;
    SmiType	  *smiType;
    SmiOption	  *option;
    SmiRefinement *refinement;
    
    for(i = 0, smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_ANY);
	smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	
	if (smiNode->nodekind != SMI_NODEKIND_COMPLIANCE) {
	    continue;
	}
	    
	if (!i) {
	    print("//\n// COMPLIANCE DEFINITIONS\n//\n\n");
	}

	printSegment(INDENT, "", 0);
	print("compliance %s {\n", smiNode->name);
	    
	if (smiNode->oid) {
	    printSegment(2 * INDENT, "oid", INDENTVALUE);
	    print("%s;\n", getOidString(smiNode, 0));
	}
	    
	if ((smiNode->status != SMI_STATUS_CURRENT) &&
	    (smiNode->status != SMI_STATUS_UNKNOWN) &&
	    (smiNode->status != SMI_STATUS_MANDATORY) &&
	    (smiNode->status != SMI_STATUS_OPTIONAL)) {
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

	if ((mandatory = smiGetFirstMandatoryNode(smiNode))) {
	    print("\n");
	    printSegment(2 * INDENT, "mandatory", INDENTVALUE);
	    print("(");
	    for (j = 0; mandatory;
		 j++,mandatory = smiGetNextMandatoryNode(smiNode, mandatory)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 1, mandatory->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(");\n");
	}
	
	if ((option = smiGetFirstOption(smiNode))) {
	    print("\n");
	    for(; option; option = smiGetNextOption(option)) {
		printSegment(2 * INDENT, "", 0);
		print("optional %s {\n", option->name);
		printSegment(3 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString(option->description);
		print(";\n");
		printSegment(2 * INDENT, "};\n", 0);
	    }
	}
	
	if ((refinement = smiGetFirstRefinement(smiNode))) {
	    print("\n");
	    for(; refinement; refinement = smiGetNextRefinement(refinement)) {
		printSegment(2 * INDENT, "", 0);
		print("refine %s {\n", refinement->name);

		if (refinement->typename) {
		    smiType = smiGetType(refinement->typemodule,
					 refinement->typename);
		    if (smiType) {
			printSegment(3 * INDENT, "type", INDENTVALUE);
			print("%s",
			      getTypeString(modulename, smiType->basetype,
					    smiType->parentmodule,
					    smiType->parentname));
			printSubtype(smiType);
			print(";\n");
		    }
		}

		if (refinement->writetypename) {
		    smiType = smiGetType(refinement->writetypemodule,
					 refinement->writetypename);
		    if (smiType) {
			printSegment(3 * INDENT, "writetype", INDENTVALUE);
			print("%s",
			      getTypeString(modulename, smiType->basetype,
					    smiType->parentmodule,
					    smiType->parentname));
			printSubtype(smiType);
			print(";\n");
		    }
		}

		if (refinement->access != SMI_ACCESS_UNKNOWN) {
		    printSegment(3 * INDENT, "access", INDENTVALUE);
		    print("%s;\n",
			  smingStringAccess(refinement->access));
		}
		printSegment(3 * INDENT, "description", INDENTVALUE);
		print("\n");
		printMultilineString(refinement->description);
		print(";\n");
		printSegment(2 * INDENT, "};\n", 0);
	    }
	}
	
	printSegment(INDENT, "", 0);
	print("};\n\n");
	i++;
    }
}



int dumpJava(char *modulename)
{
    SmiModule	 *smiModule;
    SmiNode	 *smiNode;
    
    errors = 0;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	print("//\n");
	print("// This module has been generated by smidump "
	      VERSION ". Do not edit.\n");
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
	    smiNode = smiGetNode(smiModule->name, smiModule->object);
	    print("%s;\n\n", getOidString(smiNode, 1));
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
	
	printGroups(modulename);
	
	printCompliances(modulename);
	
	print("}; // end of module %s.\n", modulename);

    }

    return errors;
}

