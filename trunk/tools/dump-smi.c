/*
 * dump-smi.c --
 *
 *      Operations to dump SMIv1/v2 module information.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-smi.c,v 1.16 1999/06/16 15:04:22 strauss Exp $
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



#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */
#define  INDENTSEQUENCE 40   /* column to start SEQUENCE tables' type column */



static int errors;

static char *convertType[] = {
    NULL,		 "INTEGER",	       NULL,	   "Integer32",
    NULL,		 "Enumeration",	       NULL,	   "INTEGER",
    NULL,		 "Bits",	       NULL,	   "BITS",
    NULL,		 "OctetString",	       NULL,	   "OCTET STRING",
    NULL,		 "ObjectIdentifier",   NULL,	   "OBJECT IDENTIFIER",
    NULL, NULL, NULL, NULL };

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



char *smiStringStatus(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	(status == SMI_STATUS_OPTIONAL)    ? "optional" :
					     "<unknown>";
}



char *smiStringAccess(SmiAccess access)
{
    return
	(access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" :
	(access == SMI_ACCESS_NOTIFY)	      ? "accessible-for-notify" :
	(access == SMI_ACCESS_READ_ONLY)      ? "read-only" :
	(access == SMI_ACCESS_READ_WRITE)     ? "read-write" :
	(access == SMI_ACCESS_READ_CREATE)    ? "read-create" :
	(access == SMI_ACCESS_WRITE_ONLY)     ? "write-only" :
						"<unknown>";
}



char *smiCTime(time_t t)
{
    static char   s[27];
    struct tm	  *tm;

    tm = gmtime(&t);
    sprintf(s, "%04d%02d%02d%02d%02dZ",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min);
    return s;
}



static char *getTypeString(char *module, SmiBasetype basetype,
			   char *typemodule, char *typename)
{
    int         i;

    if (typename &&
	(basetype != SMI_BASETYPE_ENUM) &&
	(basetype != SMI_BASETYPE_BITS)) {
	for(i=0; convertType[i+1]; i += 4) {
	    if ((!strcmp(typename, convertType[i+1])) &&
		((!typemodule) || (!convertType[i]) ||
		 (!strcmp(typemodule, convertType[i])))) {
		return convertType[i+3];
	    }
	}
    }

    if ((!typemodule) || (!typename) || islower((int)typename[0])) {
	if (basetype == SMI_BASETYPE_ENUM) {
	    return "INTEGER";
	}
	if (basetype == SMI_BASETYPE_BITS) {
	    return "BITS";
	}
    }
	
    /* TODO: fully qualified if unambigous */

    return typename;
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
	sprintf(append, " %u%s", parentNode->oid[parentNode->oidlen-1], s);

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
	sprintf(s, "'%s'B", valuePtr->value.ptr);
	break;
    case SMI_BASETYPE_HEXSTRING:
	sprintf(s, "'%s'H", valuePtr->value.ptr);
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
	    nn; i++, nn = smiGetNextNamedNumber(nn)) {
	    if (i) {
		print(", ");
	    } else {
		print(" { ");
	    }
	    sprintf(s, "%s(%s)", nn->name, getValueString(nn->valuePtr));
	    printWrapped(INDENTVALUE + INDENT, s);
	}
	if (i) {
	    print(" }");
	}
    } else {
	for(i = 0, range = smiGetFirstRange(smiType->module, smiType->name);
	    range; i++, range = smiGetNextRange(range)) {
	    if (i) {
		print(" | ");
	    } else {
		if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		    print(" (SIZE(");
		} else {
		    print(" (");
		}
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
	    if (smiType->basetype == SMI_BASETYPE_OCTETSTRING) {
		print(")");
	    }
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
    
    types[0] = 0;

    /* TODO: add needed macros; add types that are base types in SMIng */
    
    for(smiImport = smiGetFirstImport(modulename); smiImport;
	smiImport = smiGetNextImport(smiImport)) {
	importedModulename = smiImport->importmodule;
	importedDescriptor = smiImport->importname;

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



static void printModuleIdentity(char *modulename)
{
    SmiRevision  *smiRevision;
    SmiModule	 *smiModule;
    SmiNode      *smiNode;
    
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
	
	for(; smiRevision; smiRevision = smiGetNextRevision(smiRevision)) {
	    if (strcmp(smiRevision->description,
	         "[Revision added by libsmi due to a LAST-UPDATED clause.]")) {
		printSegment(INDENT, "REVISION", INDENTVALUE);
		print("\"%s\"\n", smiCTime(smiRevision->date));
		printSegment(INDENT, "DESCRIPTION", INDENTVALUE);
		print("\n");
		printMultilineString(smiRevision->description);
		print("\n");
	    }
	}

	smiNode = smiGetNode(modulename, smiModule->object);
	if (smiNode) {
	    printSegment(INDENT, "::= ", 0);
	    print("{ %s }\n\n", getOidString(smiNode, 0));
	}
        /* TODO: else error */
	
	print("\n");
    }
}



static void printTypeDefinitions(char *modulename)
{
    SmiType	 *smiType;

    for(smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(smiType)) {
#if 0
	if ((smiType->decl == SMI_DECL_TYPEASSIGNMENT) ||
	    ((smiType->decl == SMI_DECL_TYPEDEF) &&
	     (!smiType->description))) {
#else
	if (!smiType->description) {
#endif
	    print("%s ::=\n", smiType->name);
	    printSegment(INDENT, "", 0);
	    print("%s", getTypeString(modulename, smiType->basetype,
				      smiType->parentmodule,
				      smiType->parentname));
	    printSubtype(smiType);
	    print("\n\n");
	}
    }
    smiType = smiGetFirstType(modulename);
}



static void printTextualConventions(char *modulename)
{
    SmiType	 *smiType;
    
    for(smiType = smiGetFirstType(modulename);
	smiType; smiType = smiGetNextType(smiType)) {
#if 0
	if ((smiType->decl == SMI_DECL_TEXTUALCONVENTION) ||
	    ((smiType->decl == SMI_DECL_TYPEDEF) &&
	     (smiType->description))) {
#else
	if (smiType->description) {
#endif
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
	    print("%s", getTypeString(modulename, smiType->basetype,
				      smiType->parentmodule,
				      smiType->parentname));
	    printSubtype(smiType);
	    print("\n\n");
	}
    }
}



static void printObjects(char *modulename)
{
    SmiNode	 *smiNode, *rowNode, *colNode, *smiParentNode;
    SmiType	 *smiType;
    SmiIndex	 *smiIndex;
    SmiNodekind  nodekinds;
    int		 i, j, n;
    
    nodekinds =  SMI_NODEKIND_NODE | SMI_NODEKIND_TABLE |
	SMI_NODEKIND_ROW | SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR;
    
    for(smiNode = smiGetFirstNode(modulename, nodekinds);
	smiNode; smiNode = smiGetNextNode(smiNode, nodekinds)) {

	smiParentNode = smiGetParentNode(smiNode);

	if ((smiNode->nodekind == SMI_NODEKIND_NODE) &&
	    (!smiNode->description)) {
	    print("%s OBJECT IDENTIFIER\n", smiNode->name);
	} else if (smiNode->nodekind == SMI_NODEKIND_NODE) {
	    print("%s OBJECT-IDENTITY\n", smiNode->name);
	} else {
	    print("%s OBJECT-TYPE\n", smiNode->name);
	}

	if ((smiNode->basetype != SMI_BASETYPE_UNKNOWN) ||
	    (smiNode->typename)) {
	    printSegment(INDENT, "SYNTAX", INDENTVALUE);
	    if (smiNode->nodekind == SMI_NODEKIND_TABLE) {
		print("SEQUENCE OF ");
		rowNode = smiGetFirstChildNode(smiNode);
		print("%s\n", rowNode->typename);
		    /* TODO: print non-local name qualified */
	    } else if (smiNode->nodekind == SMI_NODEKIND_ROW) {
		print("%s\n", smiNode->typename);
		/* TODO: print non-local name qualified */
	    } else if (smiNode->typename) {
		if (islower((int)smiNode->typename[0])) {
		    /*
		     * an implicitly restricted type.
		     */
		    smiType = smiGetType(smiNode->typemodule,
					 smiNode->typename);
		    print("%s", getTypeString(modulename,
					      smiType->basetype,
					      smiType->parentmodule,
					      smiType->parentname));
		    printSubtype(smiType);
		    print("\n");
		} else {
		    print("%s\n", getTypeString(modulename,
						smiNode->basetype,
						smiNode->typemodule,
						smiNode->typename));
		}
	    }
	}
	
	if (smiNode->units) {
	    printSegment(INDENT, "UNITS", INDENTVALUE);
	    print("\"%s\"\n", smiNode->units);
	}
	
	if (smiNode->access != SMI_ACCESS_UNKNOWN) {
	    printSegment(INDENT, "MAX-ACCESS", INDENTVALUE);
	    print("%s\n", smiStringAccess(smiNode->access));
	    /* TODO: read-create */
	}

	if (smiNode->status != SMI_STATUS_UNKNOWN) {
	    printSegment(INDENT, "STATUS", INDENTVALUE);
	    print("%s\n", smiStringStatus(smiNode->status));
	}
	
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
	    print("{ ");
	    for (n = 0, smiIndex = smiGetFirstIndex(smiNode); smiIndex;
		 n++, smiIndex = smiGetNextIndex(smiIndex));
	    for (j = 0, smiIndex = smiGetFirstIndex(smiNode); smiIndex;
		 j++, smiIndex = smiGetNextIndex(smiIndex)) {
		if (j) {
		    print(", ");
		}
		if (smiNode->implied && ((j+1) == n)) {
		    printWrapped(INDENTVALUE + 2, "IMPLIED ");
		}
		printWrapped(INDENTVALUE + 2, smiIndex->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print(" }\n");
	    break;
	case SMI_INDEX_AUGMENT:
	case SMI_INDEX_SPARSE:
	    printSegment(INDENT, "AUGMENTS", INDENTVALUE);
	    print("{ %s }\n", smiNode->relatedname);
	    /* TODO: non-local name if non-local */
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;
	}
	
	if (smiNode->valuePtr) {
	    printSegment(INDENT, "DEFVAL", INDENTVALUE);
	    print("{ %s }", getValueString(smiNode->valuePtr));
	    print("\n");
	}

	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(smiNode, 0));

	if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    print("%s ::=\n", smiNode->typename);
	    /* TODO: non-local name? */
	    printSegment(INDENT, "SEQUENCE {", 0);
	    for(i = 0, colNode = smiGetFirstChildNode(smiNode); colNode;
		colNode = smiGetNextChildNode(colNode)) {
		if (i) {
		    print(",");
		}
		print("\n");
		printSegment(2 * INDENT, colNode->name, INDENTSEQUENCE);
		if (islower((int)colNode->typename[0])) {
		    /*
		     * an implicitly restricted type.
		     */
		    smiType = smiGetType(colNode->typemodule,
					 colNode->typename);
		    print("%s", getTypeString(modulename, smiType->basetype,
					      smiType->parentmodule,
					      smiType->parentname));
		} else {
		    print("%s", getTypeString(modulename, colNode->basetype,
					      colNode->typemodule,
					      colNode->typename));
		}
		i++;
	    }
	    print("\n");
	    printSegment(INDENT, "}\n", 0);
	    print("\n");
	}
    }
}



static void printNotifications(char *modulename)
{
    SmiNode	 *smiNode, *objectNode;
    int          j;
    
    for(smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_NOTIFICATION);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	print("%s NOTIFICATION-TYPE\n", smiNode->name);

	if ((objectNode = smiGetFirstObjectNode(smiNode))) {
	    printSegment(INDENT, "OBJECTS", INDENTVALUE);
	    print("{ ");
	    for (j = 0; objectNode;
		 j++, objectNode = smiGetNextObjectNode(smiNode, objectNode)) {
		if (j) {
		    print(", ");
		}
		printWrapped(INDENTVALUE + 2, objectNode->name);
		/* TODO: non-local name if non-local */
	    } /* TODO: empty? -> print error */
	    print("}\n");
	}

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
	print("{ %s }\n\n", getOidString(smiNode, 0));
    }
}



static void printObjectGroups(char *modulename)
{
    SmiNode	 *smiNode, *objectNode;
    int          j;
    
    for (smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_GROUP);
	 smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
	    
	objectNode = smiGetFirstMemberNode(smiNode);
	if (objectNode &&
	    (objectNode->nodekind == SMI_NODEKIND_NOTIFICATION)) {
	    continue;
	}
	    
	print("%s OBJECT-GROUP\n", smiNode->name);
	    
	printSegment(INDENT, "OBJECTS", INDENTVALUE);
	print("{ ");
	for (j = 0; objectNode;
	     j++, objectNode = smiGetNextMemberNode(smiNode, objectNode)) {
	    if (j) {
		print(", ");
	    }
	    printWrapped(INDENTVALUE + 2, objectNode->name);
	    /* TODO: non-local name if non-local */
	} /* TODO: empty? -> print error */
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
	print("{ %s }\n\n", getOidString(smiNode, 0));
    }
}



static void printNotificationGroups(char *modulename)
{
    SmiNode	 *smiNode, *objectNode;
    int          j;
    
    for (smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_GROUP);
	 smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
	    
	objectNode = smiGetFirstMemberNode(smiNode);
	if (objectNode &&
	    (objectNode->nodekind != SMI_NODEKIND_NOTIFICATION)) {
	    continue;
	}

	print("%s NOTIFICATION-GROUP\n", smiNode->name);
	    
	printSegment(INDENT, "NOTIFICATIONS", INDENTVALUE);
	print("{ ");
	for (j = 0; objectNode;
	     j++, objectNode = smiGetNextMemberNode(smiNode, objectNode)) {
	    if (j) {
		print(", ");
	    }
	    printWrapped(INDENTVALUE + 2, objectNode->name);
	    /* TODO: non-local name if non-local */
	} /* TODO: empty? -> print error */
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
	print("{ %s }\n\n", getOidString(smiNode, 0));
    }
}



static void printModuleCompliances(char *modulename)
{
    SmiNode	  *smiNode, *mandatory;
    SmiType	  *smiType;
    SmiOption	  *smiOption;
    SmiRefinement *smiRefinement;
    char	  *module;
    char	  *done = NULL; /* "+" separated list of module names */
    char	  s[100];
    int		  j;

    for (smiNode = smiGetFirstNode(modulename, SMI_NODEKIND_COMPLIANCE);
	 smiNode; smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	
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

	/* `this module' always first */
	done = util_strdup("+");
	for (module = smiNode->module; module; ) {

	    print("\n");
	    printSegment(INDENT, "MODULE", INDENTVALUE);
	    if (strlen(module) && strcmp(modulename, module)) {
		print("%s\n", module);
	    } else {
		print("-- this module\n");
	    }
	    
	    for (j = 0, mandatory = smiGetFirstMandatoryNode(smiNode);
		 mandatory;
		 mandatory = smiGetNextMandatoryNode(smiNode, mandatory)) {
		if (!strcmp(mandatory->module, module)) {
		    if (j) {
			print(", ");
		    } else {
			print("\n");
			printSegment(2 * INDENT,
				     "MANDATORY-GROUPS", INDENTVALUE);
			print("{ ");
		    }
		    j++;
		    printWrapped(INDENTVALUE + 2, mandatory->name);
		}
	    }
	    if (j) {
		print(" }\n");
	    }
		
	    for(smiOption = smiGetFirstOption(smiNode); smiOption;
		smiOption = smiGetNextOption(smiOption)) {
		if (!strcmp(smiOption->module, module)) {
		    print("\n");
		    printSegment(2 * INDENT, "GROUP", INDENTVALUE);
		    print("%s\n", smiOption->name);
		    printSegment(2 * INDENT, "DESCRIPTION", INDENTVALUE);
		    print("\n");
		    printMultilineString(smiOption->description);
		    print("\n");
		}
	    }

	    for(smiRefinement = smiGetFirstRefinement(smiNode); smiRefinement;
		smiRefinement = smiGetNextRefinement(smiRefinement)) {
		if (!strcmp(smiRefinement->module, module)) {
		    print("\n");
		    printSegment(2 * INDENT, "OBJECT", INDENTVALUE);
		    print("%s\n", smiRefinement->name);

		    if (smiRefinement->typename) {
			smiType = smiGetType(smiRefinement->typemodule,
					     smiRefinement->typename);
			if (smiType) {
			    printSegment(2 * INDENT, "SYNTAX", INDENTVALUE);
			    print("%s",
				  getTypeString(module, smiType->basetype,
						smiType->parentmodule,
						smiType->parentname));
			    printSubtype(smiType);
			    print("\n");
			}
		    }
		    
		    if (smiRefinement->writetypename) {
			smiType = smiGetType(smiRefinement->writetypemodule,
					     smiRefinement->writetypename);
			if (smiType) {
			    printSegment(2 * INDENT, "WRITE-SYNTAX",
					 INDENTVALUE);
			    print("%s",
				  getTypeString(module, smiType->basetype,
						smiType->parentmodule,
						smiType->parentname));
			    printSubtype(smiType);
			    print("\n");
			}
		    }
		    
		    if (smiRefinement->access != SMI_ACCESS_UNKNOWN) {
			printSegment(2 * INDENT, "MIN-ACCESS", INDENTVALUE);
			print("%s\n",
			      smiStringAccess(smiRefinement->access));
		    }
		    printSegment(2 * INDENT, "DESCRIPTION", INDENTVALUE);
		    print("\n");
		    printMultilineString(smiRefinement->description);
		    print("\n");
		}
	    }

	    /*
	     * search the next module name in the list of mandatory
	     * groups, optional groups and refinements.
	     */
	    util_strcat(&done, module);
	    util_strcat(&done, "+");
	    module = NULL;
	    for (mandatory = smiGetFirstMandatoryNode(smiNode);
		 mandatory;
		 mandatory = smiGetNextMandatoryNode(smiNode, mandatory)) {
		sprintf(s, "+%s+", mandatory->module);
		if ((!strstr(done, s))) {
		    module = mandatory->module;
		    break;
		}
	    }
	    if (!module) {
		; /* TODO: search in options list */
	    }
	    if (!module) {
		; /* TODO: search in refinements list */
	    }
	}

	print("\n");
	printSegment(INDENT, "::= ", 0);
	print("{ %s }\n\n", getOidString(smiNode, 0));
    }
    util_free(done);
}



int dumpSmi(char *modulename)
{
    SmiModule	 *smiModule;

    errors = 0;
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "Cannot locate module `%s'\n", modulename);
	exit(1);
    } else {
	print("--\n");
	print("-- This module has been generated by smidump "
	      VERSION ". Do not edit.\n");
	print("--\n");
	print("%s DEFINITIONS ::= BEGIN\n\n", smiModule->name);

	printImports(modulename);

	printModuleIdentity(modulename);

	printTypeDefinitions(modulename);
	
	printTextualConventions(modulename);

	printObjects(modulename);
	
	printNotifications(modulename);
	
	printObjectGroups(modulename);
	
	printNotificationGroups(modulename);
	
	printModuleCompliances(modulename);
	
	print("END -- end of module %s.\n", modulename);

    }

    return errors;
}



int dumpSmiV1(modulename)
    char *modulename;
{
    version = 1;
    return dumpSmi(modulename);
}



int dumpSmiV2(modulename)
    char *modulename;
{
    version = 2;
    return dumpSmi(modulename);
}

