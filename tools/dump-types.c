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
 * @(#) $Id: dump-types.c,v 1.16 2000/03/22 09:46:14 strauss Exp $
 */

#include <config.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

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
    char        *module;
    char        *name;
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
    if ((!smiModule) || (strlen(smiModule->name) == 0)) {
	flags[2] = 'b';
    } else if (!currentModule) {
	flags[2] = '-';
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



static char *getValueString(SmiValue *valuePtr, SmiType *typePtr)
{
    static char    s[100];
    char           ss[9];
    int		   i, n;
    SmiNamedNumber *nn;
    SmiNode        *nodePtr;
    
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
	for (nn = smiGetFirstNamedNumber(typePtr); nn;
	     nn = smiGetNextNamedNumber(nn)) {
	    if (nn->value.value.unsigned32 == valuePtr->value.unsigned32)
		break;
	}
	if (nn) {
	    sprintf(s, "%s", nn->name);
	} else {
	    sprintf(s, "%ld", valuePtr->value.integer32);
	}
	break;
    case SMI_BASETYPE_OCTETSTRING:
	for (i = 0; i < valuePtr->len; i++) {
	    if (!isprint((int)valuePtr->value.ptr[i])) break;
	}
	if (i == valuePtr->len) {
	    sprintf(s, "\"%s\"", valuePtr->value.ptr);
	} else {
            sprintf(s, "0x%*s", 2 * valuePtr->len, "");
            for (i=0; i < valuePtr->len; i++) {
                sprintf(ss, "%02x", valuePtr->value.ptr[i]);
                strncpy(&s[2+2*i], ss, 2);
            }
	}
	break;
    case SMI_BASETYPE_BITS:
	sprintf(s, "(");
	for (i = 0, n = 0; i < valuePtr->len * 8; i++) {
	    if (valuePtr->value.ptr[i/8] & (1 << i%8)) {
		if (n)
		    sprintf(&s[strlen(s)], ", ");
		n++;
		for (nn = smiGetFirstNamedNumber(typePtr); nn;
		     nn = smiGetNextNamedNumber(nn)) {
		    if (nn->value.value.unsigned32 == i)
			break;
		}
		if (nn) {
		    sprintf(&s[strlen(s)], "%s", nn->name);
		} else {
		    sprintf(s, "%d", i);
		}
	    }
	}
	sprintf(&s[strlen(s)], ")");
	break;
    case SMI_BASETYPE_UNKNOWN:
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	nodePtr = smiGetNodeByOID(valuePtr->len, valuePtr->value.oid);
	if (!nodePtr) {
	    sprintf(s, "%s", nodePtr->name);
	} else {
	    strcpy(s, "");
	    for (i=0; i < valuePtr->len; i++) {
		if (i) strcat(s, ".");
		sprintf(&s[strlen(s)], "%u", valuePtr->value.oid[i]);
	    }
	}
	break;
    }

    return s;
}



static TypeNode *createTypeNode(SmiType *smiType)
{
    TypeNode *newType;
    
    newType = xmalloc(sizeof(TypeNode));
    newType->module = smiGetTypeModule(smiType)->name;
    newType->name = smiType->name;
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
    
    if ((!root->module
	 || !smiParentType
	 || strcmp(smiGetTypeModule(smiParentType)->name,
		   root->module) == 0) &&
	(!smiParentType || strcmp(smiParentType->name, root->name) == 0)) {
	newType = createTypeNode(smiType);
	newType->module = smiGetTypeModule(smiType)->name;

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
	if (root->childNodePtr->module) {
	    freeTypeTree(root->childNodePtr);
	    root->childNodePtr = NULL;
	} else {
	    freeTypeTree(root->childNodePtr);
	}
    }
    
    if (root->nextNodePtr) {
	if (root->nextNodePtr->module) {
	    freeTypeTree(root->nextNodePtr);
	    root->nextNodePtr = NULL;
	} else {
	    freeTypeTree(root->nextNodePtr);
	}
    }

    if (root->module) {
	xfree(root);
    }
}
 


static TypeNode *findInTypeTree(TypeNode *root, SmiType *smiType)
{
    TypeNode *result = NULL;
    
    if (root->module
 	&& strcmp(root->module, smiGetTypeModule(smiType)->name) == 0
	&& smiType->name
 	&& strcmp(root->name, smiType->name) == 0) {
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
	    strcpy(s1, getValueString(&range->minValue, smiType));
	    strcpy(s2, getValueString(&range->maxValue, smiType));
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
    
    if (root->module) {
	printf("%s  |\n", prefix);
    }

    for (typeNode = root; typeNode; typeNode = typeNode->nextNodePtr) {
	int len = strlen(typeNode->name);
	if (len > namelen) namelen = len;
    }
    
    for (typeNode = root; typeNode; typeNode = typeNode->nextNodePtr) {
 	if (typeNode != &typeRoot) {
	    SmiType *smiType;
	    char c = '+', *flags;
	    smiModule = smiGetModule(typeNode->module);
	    smiType = smiGetType(smiModule, typeNode->name);
	    if (smiType) {
		c = getStatusChar(smiType->status);
		if (getBaseTypeCount(typeNode->basetype)) {
		    flags = getFlags(smiType);
		    if (flags && *flags) {
			printf("%s  %c-- %s %-*s", prefix, c, flags,
			       namelen, typeNode->name);
		    } else {
			printf("%s  %c--%-*s", prefix, c,
			       namelen, typeNode->name);
		    }
		    printRestrictions(smiType);
		    if (smiType->format) {
			printf("\t\"%s\"", smiType->format);
		    }
		    printf("\n");
		}
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

    if (!smiType->name) {
	return;
    }
	    
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
 	}
    }

    addToTypeTree(&typeRoot, smiType);
    incrBaseTypeCount(smiType->basetype);
}



int dumpTypes(char *modulename, int flags)
{
    SmiModule   *smiModule;
    SmiType     *smiType;
    
    currentModule = modulename;
    smiModule = modulename ? smiGetModule(modulename) : NULL;
    
    initBaseTypeCount();

    if (! (flags & SMIDUMP_FLAG_SILENT)) {
	printf("# %s type derivation tree (generated by smidump "
	       VERSION ")\n\n", modulename ? modulename : "united");
    }

    if (smiModule) {
	for (smiType = smiGetFirstType(smiModule);
	     smiType;
	     smiType = smiGetNextType(smiType)) {
	    addType(smiType);
	}
    } else {
	for (smiModule = smiGetFirstModule(); smiModule;
	     smiModule = smiGetNextModule(smiModule)) {
	    for (smiType = smiGetFirstType(smiModule);
		 smiType;
		 smiType = smiGetNextType(smiType)) {
		addType(smiType);
	    }
	}
    }
    
#if 0 /* no implicit types */
    for (smiNode = smiGetFirstNode(smiModule, nodekind);
 	 smiNode;
 	 smiNode = smiGetNextNode(smiNode, nodekind)) {
 	smiType = smiGetNodeType(smiNode);
 	if (smiType) {
 	    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
 		addType(smiType);
 	    }
	    incrBaseTypeCount(smiType->basetype);
 	}
    }
#endif
    
    printTypeTree(&typeRoot, "");
    
    freeTypeTree(&typeRoot);
    return 0;
}
