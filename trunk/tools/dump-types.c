/*
 * dump-types.c --
 *
 *      Operations to dump the type hierarchy in a human readable format.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-types.c,v 1.5 2000/02/06 23:30:59 strauss Exp $
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "smi.h"
#include "smidump.h"


static char *currentModule;


typedef struct BaseTypeCount {
    SmiBasetype basetype;
    int         counter;
} BaseTypeCount;


static BaseTypeCount basetypes[] = {
    { SMI_BASETYPE_INTEGER32, 0 },
    { SMI_BASETYPE_OCTETSTRING, 0 },
    { SMI_BASETYPE_OBJECTIDENTIFIER, 0 },
    { SMI_BASETYPE_UNSIGNED32, 0 },
    { SMI_BASETYPE_INTEGER64, 0 },
    { SMI_BASETYPE_UNSIGNED64, 0 },
    { SMI_BASETYPE_FLOAT32, 0 },
    { SMI_BASETYPE_FLOAT64, 0 },
    { SMI_BASETYPE_FLOAT128, 0 },
    { SMI_BASETYPE_ENUM, 0 },
    { SMI_BASETYPE_BITS, 0 },
    { SMI_BASETYPE_UNKNOWN, 0 }
};


typedef struct TypeNode {
    char        *typemodule;
    char        *typename;
    SmiBasetype basetype;
    struct TypeNode *nextNodePtr;
    struct TypeNode *childNodePtr;
} TypeNode;


static TypeNode _Bits = {
    NULL, "Bits", SMI_BASETYPE_BITS, NULL, NULL
};
static TypeNode _Enumeration = {
    NULL, "Enumeration", SMI_BASETYPE_ENUM, &_Bits, NULL
};
static TypeNode _Float128 = {
    NULL, "Float128", SMI_BASETYPE_FLOAT128, &_Enumeration, NULL
};
static TypeNode _Float64 = {
    NULL, "Float64", SMI_BASETYPE_FLOAT64, &_Float128, NULL
};
static TypeNode _Float32 = {
    NULL, "Float32", SMI_BASETYPE_FLOAT32, &_Float64, NULL
};
static TypeNode _Unsigned64 = {
    NULL, "Unsigned64", SMI_BASETYPE_UNSIGNED64, &_Float32, NULL
};
static TypeNode _Unsigned32 = {
    NULL, "Unsigned32", SMI_BASETYPE_UNSIGNED32, &_Unsigned64, NULL
};
static TypeNode _Integer64 = {
    NULL, "Integer64", SMI_BASETYPE_INTEGER64, &_Unsigned32, NULL
};
static TypeNode _Integer32 = {
    NULL, "Integer32", SMI_BASETYPE_INTEGER32, &_Integer64, NULL
};
static TypeNode _ObjectIdentifier = {
    NULL, "ObjectIdentifier", SMI_BASETYPE_OBJECTIDENTIFIER, &_Integer32, NULL
};
static TypeNode _OctetString = {
    NULL, "OctetString", SMI_BASETYPE_OCTETSTRING, &_ObjectIdentifier, NULL
};
static TypeNode typeRoot = {
    NULL, "", SMI_BASETYPE_UNKNOWN, NULL, &_OctetString
};



static char getStatusChar(SmiStatus status)
{
    switch (status) {
    case SMI_STATUS_UNKNOWN:
	return '+';
    case SMI_STATUS_CURRENT:
	return '+';
    case SMI_STATUS_DEPRECATED:
	return 'x';
    case SMI_STATUS_MANDATORY:
        return '+';
    case SMI_STATUS_OPTIONAL:
	return '+';
    case SMI_STATUS_OBSOLETE:
	return 'o';
    }

    return ' ';
}



static char *getFlags(SmiType *smiType)
{
    static char flags[4];
    SmiModule *smiModule;
    
    memset(flags, 0, sizeof(flags));
    strcpy(flags, "---");

    switch (smiType->decl) {
    case SMI_DECL_IMPLICIT_TYPE:
	flags[0] = 'i';
	break;
    case SMI_DECL_TYPEASSIGNMENT:
	flags[0] = 'a';
	break;
    case SMI_DECL_TEXTUALCONVENTION:
    case SMI_DECL_TYPEDEF:
	flags[0] = 't';
	break;
    default:
	break;
    }

    smiModule = smiGetTypeModule(smiType);
    if (! smiModule || strlen(smiModule->name) == 0) {
	flags[2] = 'b';
    } else if (strcmp(smiModule->name, currentModule) == 0) {
	flags[2] = 'l';
    } else {
	flags[2] = 'i';
    }

    return flags;
}



static void initBaseTypeCount()
{
    int i;

    for (i = 0; i < sizeof(basetypes)/sizeof(BaseTypeCount); i++) {
	basetypes[i].counter = 0;
    }
}



static void incrBaseTypeCount(SmiBasetype basetype)
{
    int i;

    for (i = 0; i < sizeof(basetypes)/sizeof(BaseTypeCount); i++) {
	if (basetypes[i].basetype == basetype) {
	    basetypes[i].counter++;
	}
    }
}



static int getBaseTypeCount(SmiBasetype basetype)
{
    int i;

    for (i = 0; i < sizeof(basetypes)/sizeof(BaseTypeCount); i++) {
	if (basetypes[i].basetype == basetype) {
	    return basetypes[i].counter;
	}
    }

    return -1;
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
        sprintf(s, "%s", valuePtr->value.ptr);
	break;
    }

    return s;
}



static TypeNode *createTypeNode(SmiType *smiType)
{
    TypeNode *newType;
    
    newType = xmalloc(sizeof(TypeNode));
    newType->typemodule = smiGetTypeModule(smiType)->name;
    newType->typename = smiType->name;
    newType->basetype = smiType->basetype;
    newType->childNodePtr = NULL;
    newType->nextNodePtr = NULL;

    return newType;
}



static void addToTypeTree(TypeNode *root, SmiType *smiType)
{
    TypeNode *newType, **typePtrPtr;
    SmiType *smiParentType;
    
    if (! root) {
	return;
    }

    smiParentType = smiGetParentType(smiType);
    
    if ((!root->typemodule
	 || !smiParentType
	 || strcmp(smiGetTypeModule(smiParentType)->name,
		   root->typemodule) == 0) &&
	(!smiParentType || strcmp(smiParentType->name, root->typename) == 0)) {
	newType = createTypeNode(smiType);
	newType->typemodule = smiGetTypeModule(smiType)->name;

	for (typePtrPtr = &(root->childNodePtr);
	     *typePtrPtr; typePtrPtr = &((*typePtrPtr)->nextNodePtr)) ;
	*typePtrPtr = newType;
	return;
    }

    if (root->nextNodePtr) {
	addToTypeTree(root->nextNodePtr, smiType);
    }

    if (root->childNodePtr) {
	addToTypeTree(root->childNodePtr, smiType);
    }
}


static void freeTypeTree(TypeNode *root)
{
    if (root->childNodePtr) {
	if (root->childNodePtr->typemodule) {
	    freeTypeTree(root->childNodePtr);
	    root->childNodePtr = NULL;
	} else {
	    freeTypeTree(root->childNodePtr);
	}
    }
    
    if (root->nextNodePtr) {
	if (root->nextNodePtr->typemodule) {
	    freeTypeTree(root->nextNodePtr);
	    root->nextNodePtr = NULL;
	} else {
	    freeTypeTree(root->nextNodePtr);
	}
    }

    if (root->typemodule) {
	xfree(root);
    }
}
 


static TypeNode *findInTypeTree(TypeNode *root, SmiType *smiType)
{
    TypeNode *result = NULL;
    
    if (root->typemodule
 	&& strcmp(root->typemodule, smiGetTypeModule(smiType)->name) == 0
 	&& strcmp(root->typename, smiType->name) == 0) {
	result = root;
    }
    
    if (!result && root->childNodePtr) {
 	result = findInTypeTree(root->childNodePtr, smiType);
    }
    
    if (! result && root->nextNodePtr) {
 	result = findInTypeTree(root->nextNodePtr, smiType);
    }
    
    return result;
}



static void printRestrictions(SmiType *smiType)
{
    SmiNamedNumber *nn;
    SmiRange       *range;
    char           s1[40], s2[40];
    int            i;
    
    if ((smiType->basetype == SMI_BASETYPE_ENUM) ||
	(smiType->basetype == SMI_BASETYPE_BITS)) {
	for(i = 0, nn = smiGetFirstNamedNumber(smiType);
	    nn ; nn = smiGetNextNamedNumber(nn), i++) {
	    printf("%s%s(%ld)", (i == 0) ? " {" : ", ",
		   nn->name, nn->value.value.integer32);
	}
	if (i) printf("}");
    } else {
	for(i = 0, range = smiGetFirstRange(smiType);
	    range ; range = smiGetNextRange(range), i++) {
	    strcpy(s1, getValueString(&range->minValue));
	    strcpy(s2, getValueString(&range->maxValue));
	    printf("%s%s", (i == 0) ? " [" : ", ", s1);
	    if (strcmp(s1, s2)) printf("..%s", s2);
	}
	if (i) printf("]");
    }
}



static void printTypeTree(TypeNode *root, char *prefix)
{
    TypeNode *typeNode, *nextNode;
    int namelen = -1;
    SmiModule *smiModule;
    
    if (root->typemodule) {
	printf("%s  |\n", prefix);
    }

    for (typeNode = root; typeNode; typeNode = typeNode->nextNodePtr) {
	int len = strlen(typeNode->typename);
	if (len > namelen) namelen = len;
    }
    
    for (typeNode = root; typeNode; typeNode = typeNode->nextNodePtr) {
 	if (typeNode != &typeRoot) {
	    SmiType *smiType;
	    char c = '+', *flags;
	    smiModule = smiGetModule(typeNode->typemodule);
	    smiType = smiGetType(smiModule, typeNode->typename);
	    if (smiType) {
		c = getStatusChar(smiType->status);
		if (getBaseTypeCount(typeNode->basetype)) {
		    flags = getFlags(smiType);
		    if (flags && *flags) {
			printf("%s  %c-- %s %-*s", prefix, c, flags,
			       namelen, typeNode->typename);
		    } else {
			printf("%s  %c--%-*s", prefix, c,
			       namelen, typeNode->typename);
		    }
		    printRestrictions(smiType);
		    if (smiType->format) {
			printf("\t\"%s\"", smiType->format);
		    }
		    printf("\n");
		}
		smiFreeType(smiType);
	    }
 	}
 	if (typeNode->childNodePtr) {
 	    char *newprefix;
 	    newprefix = xmalloc(strlen(prefix)+10);
 	    strcpy(newprefix, prefix);
 	    if (typeNode != &typeRoot) {
		for (nextNode = typeNode->nextNodePtr;
		     nextNode; nextNode = nextNode->nextNodePtr) {
		    if (getBaseTypeCount(nextNode->basetype)) {
			break;
		    }
		}
 		if (nextNode) {
  		    strcat(newprefix, "  |");
 		} else {
 		    strcat(newprefix, "   ");
 		}
 	    }
 	    printTypeTree(typeNode->childNodePtr, newprefix);
 	    xfree(newprefix);
	    for (nextNode = typeNode->nextNodePtr;
		 nextNode; nextNode = nextNode->nextNodePtr) {
		if (getBaseTypeCount(nextNode->basetype)) {
		    break;
		}
	    }
	    if (nextNode) {
		printf("%s  |\n", prefix);
	    }
 	}
    }
}



static void addType(SmiType *smiType)
{
    SmiType *smiParentType;
    
    if (strlen(smiGetTypeModule(smiType)->name) == 0) {
	return;
    }
    
    if (findInTypeTree(&typeRoot, smiType)) {
	return;
    }

    smiParentType = smiGetParentType(smiType);
    if (smiParentType && smiParentType->name) {
 	if (smiParentType) {
 	    addType(smiParentType);
	    smiFreeType(smiParentType);
 	}
    }

    addToTypeTree(&typeRoot, smiType);
}



int dumpTypes(char *modulename, int flags)
{
    SmiModule   *smiModule;
    SmiType     *smiType;
    SmiNode     *smiNode;
    SmiNodekind nodekind = SMI_NODEKIND_SCALAR | SMI_NODEKIND_COLUMN;
    
    currentModule = modulename;
    smiModule = smiGetModule(modulename);
    
    initBaseTypeCount();

    if (! (flags & SMIDUMP_FLAG_SILENT)) {
	printf("# %s type derivation tree (generated by smidump "
	       VERSION ")\n\n", modulename);
    }

    for (smiType = smiGetFirstType(smiModule);
 	 smiType;
 	 smiType = smiGetNextType(smiType)) {
 	addType(smiType);
	incrBaseTypeCount(smiType->basetype);
    }
    
    for (smiNode = smiGetFirstNode(modulename, nodekind);
 	 smiNode;
 	 smiNode = smiGetNextNode(smiNode, nodekind)) {
 	smiType = smiGetType(smiGetModule(smiNode->typemodule),
			     smiNode->typename);
 	if (smiType) {
 	    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
 		addType(smiType);
 	    }
	    smiFreeType(smiType);
 	}
	incrBaseTypeCount(smiNode->basetype);
    }
    
    printTypeTree(&typeRoot, "");
    
    freeTypeTree(&typeRoot);
    return 0;
}
