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
 * @(#) $Id: dump-types.c,v 1.23 2000/11/09 22:29:54 strauss Exp $
 */

/*
 * Decide how we want to name the implicitely defined types.
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "smi.h"
#include "smidump.h"


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
    SmiType	*smiType;
    SmiModule	*smiModule;
    SmiBasetype smiBasetype;
    struct TypeNode *nextNodePtr;
    struct TypeNode *childNodePtr;
} TypeNode;


static TypeNode typeRoot = {
    NULL, NULL, SMI_BASETYPE_UNKNOWN, NULL, NULL
};


static int pmodc = 0;
static SmiModule **pmodv = NULL;


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



static char *getFlags(SmiModule *smiModule, SmiType *smiType)
{
    static char flags[4];
    int		i;
    
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

    for (i = 0; i < pmodc; i++) {
	if (strcmp(pmodv[i]->name, smiModule->name) == 0) {
	    break;
	}
    }

    if ((!smiModule) || (strlen(smiModule->name) == 0)) {
	flags[2] = 'b';
#if 0	/* xxx fixme */
    } else if (!moduleList) {
	flags[2] = '-';
    } else if (mPtr) {
	flags[2] = 'l';
#endif
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
	sprintf(s, UINT64_FORMAT, valuePtr->value.unsigned64);
	break;
    case SMI_BASETYPE_INTEGER64:
	sprintf(s, INT64_FORMAT, valuePtr->value.integer64);
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



static void addToTypeTree(TypeNode *root,
			  SmiModule *smiModule, SmiType *smiType)
{
    TypeNode  *newType, **typePtrPtr;
    SmiType   *smiParentType;
    SmiModule *smiParentModule;

    if (! root) {
	return;
    }

    smiParentType = smiGetParentType(smiType);
    smiParentModule = smiParentType ? smiGetTypeModule(smiParentType) : NULL;

    if ((root->smiModule == smiParentModule && root->smiType == smiParentType)
	|| (! root->smiModule && ! root->smiType
	    && root->smiBasetype == smiType->basetype)) {

	 newType = xmalloc(sizeof(TypeNode));
	 newType->smiModule = smiModule;
	 newType->smiType = smiType;
	 newType->smiBasetype = smiType->basetype;
	 newType->childNodePtr = NULL;
	 newType->nextNodePtr = NULL;

	 for (typePtrPtr = &(root->childNodePtr);
	      *typePtrPtr; typePtrPtr = &((*typePtrPtr)->nextNodePtr)) ;
	 *typePtrPtr = newType;
	 return;
     }

     if (root->nextNodePtr) {
	 addToTypeTree(root->nextNodePtr, smiModule, smiType);
     }
     
     if (root->childNodePtr) {
	 addToTypeTree(root->childNodePtr, smiModule, smiType);
     }
}



static void freeTypeTree(TypeNode *root)
{
    if (root->childNodePtr) {
	if (root->childNodePtr->smiModule) {
	    freeTypeTree(root->childNodePtr);
	    root->childNodePtr = NULL;
	} else {
	    freeTypeTree(root->childNodePtr);
	}
    }
    
    if (root->nextNodePtr) {
	if (root->nextNodePtr->smiModule) {
	    freeTypeTree(root->nextNodePtr);
	    root->nextNodePtr = NULL;
	} else {
	    freeTypeTree(root->nextNodePtr);
	}
    }
    
    if (root->smiModule) {
	xfree(root);
    }
}



static TypeNode *findInTypeTree(TypeNode *root,
				SmiModule *smiModule, SmiType *smiType)
{
    TypeNode *result = NULL;
    
    if (root->smiModule && root->smiModule == smiModule
	&& smiType->name
	&& root->smiType && root->smiType == smiType) {
	result = root;
    }
    
    if (!result && root->childNodePtr) {
	result = findInTypeTree(root->childNodePtr, smiModule, smiType);
    }
    
    if (!result && root->nextNodePtr) {
	result = findInTypeTree(root->nextNodePtr, smiModule, smiType);
    }
    
    return result;
}



static char* getTypeName(TypeNode *typeNode)
{
    SmiRefinement *smiRefinement;
    SmiType *smiType;
    char *name = "?";

    if (typeNode->smiType->name) {
	return xstrdup(typeNode->smiType->name);
	
    } else if (typeNode->smiModule) {
	SmiNode *smiNode;

	for (smiNode = smiGetFirstNode(typeNode->smiModule, SMI_NODEKIND_ANY);
	     smiNode;
	     smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	    if (smiGetNodeType(smiNode) == typeNode->smiType) {
		name = xmalloc(strlen(smiNode->name) + 3);
		sprintf(name, "(%s)", smiNode->name);
		return name;
	    }
	}

	for (smiNode = smiGetFirstNode(typeNode->smiModule,
				       SMI_NODEKIND_COMPLIANCE);
	     smiNode;
	     smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	    for(smiRefinement = smiGetFirstRefinement(smiNode);
		smiRefinement;
		smiRefinement = smiGetNextRefinement(smiRefinement)) {
		smiType = smiGetRefinementType(smiRefinement);
		if (smiType == typeNode->smiType) {
		    smiNode = smiGetRefinementNode(smiRefinement);
		    name = xmalloc(strlen(smiNode->name) + 3);
		    sprintf(name, "(%s)", smiNode->name);
		    return name;
		}
		smiType = smiGetRefinementWriteType(smiRefinement);
		if (smiType == typeNode->smiType) {
		    smiNode = smiGetRefinementNode(smiRefinement);
		    name = xmalloc(strlen(smiNode->name) + 3);
		    sprintf(name, "(%s)", smiNode->name);
		    return name;
		}
	    }
	}
    }

    return xstrdup(name);
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
    char *name;
    int namelen = -1;

    if (root->smiModule) {
	printf("%s  |\n", prefix);
    }

    for (typeNode = root; typeNode; typeNode = typeNode->nextNodePtr) {
	if (typeNode->smiType) {
	    int len;
	    name = getTypeName(typeNode);
	    len = strlen(name);
	    if (len > namelen) namelen = len;
	    xfree(name);
	}
    }
    
    for (typeNode = root; typeNode; typeNode = typeNode->nextNodePtr) {
 	if (typeNode != &typeRoot) {
	    char c = '+', *flags;
	    if (typeNode->smiType) {
		name = getTypeName(typeNode);
		c = getStatusChar(typeNode->smiType->status);
		if (getBaseTypeCount(typeNode->smiBasetype)) {
		    flags = getFlags(typeNode->smiModule, typeNode->smiType);
		    if (flags && *flags) {
			printf("%s  %c-- %s %-*s", prefix, c, flags,
			       namelen, name);
		    } else {
			printf("%s  %c--%-*s", prefix, c,
			       namelen, name);
		    }
		    printRestrictions(typeNode->smiType);
		    if (typeNode->smiType->format) {
			printf("\t\"%s\"", typeNode->smiType->format);
		    }
		    printf("\n");
		}
		xfree(name);
	    } else {
		printf("xxxx\n");
	    }
 	}
 	if (typeNode->childNodePtr) {
 	    char *newprefix;
 	    newprefix = xmalloc(strlen(prefix)+10);
 	    strcpy(newprefix, prefix);
 	    if (typeNode != &typeRoot) {
		for (nextNode = typeNode->nextNodePtr;
		     nextNode; nextNode = nextNode->nextNodePtr) {
		    if (getBaseTypeCount(nextNode->smiBasetype)) {
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
		if (getBaseTypeCount(nextNode->smiBasetype)) {
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
    SmiModule *smiModule;
    SmiType   *smiParentType;

    smiModule = smiGetTypeModule(smiType);
    if (! smiModule) {
	return;
    }
    
    if (findInTypeTree(&typeRoot, smiModule, smiType)) {
	return;
    }

    smiParentType = smiGetParentType(smiType);
    if (smiParentType && smiParentType->name) {
 	if (smiParentType) {
 	    addType(smiParentType);
 	}
    }

    addToTypeTree(&typeRoot, smiModule, smiType);
    incrBaseTypeCount(smiType->basetype);
}



void dumpTypes(int modc, SmiModule **modv, int flags, char *output)
{
    SmiNode	  *smiNode;
    SmiType       *smiType;
    SmiRefinement *smiRefinement;
    int		  i;
    const int	  nodekind = SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR;

    pmodc = modc;
    pmodv = modv;
    
    initBaseTypeCount();

    for (i = 0; i < modc; i++) {

	if (! (flags & SMIDUMP_FLAG_UNITE)) {
	    initBaseTypeCount();
	}

	for (smiType = smiGetFirstType(modv[i]);
	     smiType;
	     smiType = smiGetNextType(smiType)) {
	    addType(smiType);
	}

	for (smiNode = smiGetFirstNode(modv[i], nodekind);
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
	
	for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_COMPLIANCE);
	     smiNode;
	     smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
	    for (smiRefinement = smiGetFirstRefinement(smiNode);
		 smiRefinement;
		 smiRefinement = smiGetNextRefinement(smiRefinement)) {
		smiType = smiGetRefinementType(smiRefinement);
		if (smiType) {
		    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
			addType(smiType);
		    }
		    incrBaseTypeCount(smiType->basetype);
		}
		smiType = smiGetRefinementWriteType(smiRefinement);
		if (smiType) {
		    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
			addType(smiType);
		    }
		    incrBaseTypeCount(smiType->basetype);
		}
	    }
	}

	if (! (flags & SMIDUMP_FLAG_UNITE)) {
	    if (! (flags & SMIDUMP_FLAG_SILENT)) {
		printf("# %s type derivation tree (generated by smidump "
		       SMI_VERSION_STRING ")\n\n", modv[i]->name);
	    }
	    printTypeTree(&typeRoot, "");
	    freeTypeTree(&typeRoot);
	}
    }

    if (flags & SMIDUMP_FLAG_UNITE) {
	if (! (flags & SMIDUMP_FLAG_SILENT)) {
	    printf("# united type derivation tree (generated by smidump "
		   SMI_VERSION_STRING ")\n\n");
	}
	printTypeTree(&typeRoot, "");
	freeTypeTree(&typeRoot);
    }
}
