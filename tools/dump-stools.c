/*
 * dump-stools.c --
 *
 *      Operations to generate MIB module stubs for the stools package
 *
 * Copyright (c) 2001 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-netsnmp.c,v 1.9 2001/01/26 15:24:19 schoenw Exp $
 */

/*
 * TODO:
 *	  - assume that we build a dynamic loadable module
 *	  - update to 4.X version of the UCD API
 *	  - generate #defines for deprecated and obsolete objects
 *	  - generate stub codes for the various functions
 *	  - generate type and range checking code
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"
#include "smidump.h"


#if 0
static char *getAccessString(SmiAccess access)
{
    if (access == SMI_ACCESS_READ_WRITE) {
	return "RWRITE";
    } else if (access == SMI_ACCESS_READ_ONLY) {
	return "RONLY";
    } else {
	return "";
    }
}


static char *getBaseTypeString(SmiBasetype basetype)
{
    switch(basetype) {
    case SMI_BASETYPE_UNKNOWN:
	return "ASN_NULL";
    case SMI_BASETYPE_INTEGER32:
    case SMI_BASETYPE_ENUM:
	return "ASN_INTEGER";
    case SMI_BASETYPE_OCTETSTRING:
    case SMI_BASETYPE_BITS:
	return "ASN_OCTET_STR";
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	return "ASN_OBJECT_ID";
    case SMI_BASETYPE_UNSIGNED32:
	return "ASN_INTEGER";
    case SMI_BASETYPE_INTEGER64:
	return "ASN_INTEGER";
    case SMI_BASETYPE_UNSIGNED64:
	return "ASN_INTEGER";
    case SMI_BASETYPE_FLOAT32:
    case SMI_BASETYPE_FLOAT64:
    case SMI_BASETYPE_FLOAT128:
	return "ASN_Real";
    }

    return NULL;
}
#endif


static char* translate(char *m)
{
    char *s;
    int i;

    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
    }
  
    return s;
}



static char* translateUpper(char *m)
{
    char *s;
    int i;

    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
	if (islower((int) s[i])) {
	    s[i] = toupper(s[i]);
	}
    }
  
    return s;
}



static char* translateLower(char *m)
{
    char *s;
    int i;

    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
	if (isupper((int) s[i])) {
	    s[i] = tolower(s[i]);
	}
    }
  
    return s;
}



static char* translateFileName(char *m)
{
    char *s;
    int i;

    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '_') s[i] = '-';
	if (isupper((int) s[i])) {
	    s[i] = tolower(s[i]);
	}
    }
  
    return s;
}



static FILE * createFile(char *name, char *suffix)
{
    char *fullname;
    FILE *f;

    fullname = xmalloc(strlen(name) + (suffix ? strlen(suffix) : 0) + 2);
    strcpy(fullname, name);
    if (suffix) {
        strcat(fullname, suffix);
    }
    if (!access(fullname, R_OK)) {
        fprintf(stderr, "smidump: %s already exists\n", fullname);
        xfree(fullname);
        return NULL;
    }
    f = fopen(fullname, "w");
    if (!f) {
        fprintf(stderr, "smidump: cannot open %s for writing: ", fullname);
        perror(NULL);
        xfree(fullname);
        exit(1);
    }
    xfree(fullname);
    return f;
}



static int isGroup(SmiNode *smiNode)
{
    SmiNode *childNode;

    if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	return 1;
    }
    
    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_SCALAR) {
	    return 1;
	}
    }

    return 0;
}



static int isAccessible(SmiNode *groupNode)
{
    SmiNode *smiNode;
    int num = 0;
    
    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if ((smiNode->nodekind == SMI_NODEKIND_SCALAR
	     || smiNode->nodekind == SMI_NODEKIND_COLUMN)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    num++;
	}
    }

    return num;
}



static unsigned int getMinSize(SmiType *smiType)
{
    SmiRange *smiRange;
    SmiType  *parentType;
    unsigned int min = 65535, size;
    
    switch (smiType->basetype) {
    case SMI_BASETYPE_BITS:
	return 0;
    case SMI_BASETYPE_OCTETSTRING:
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	size = 0;
	break;
    default:
	return -1;
    }

    for (smiRange = smiGetFirstRange(smiType);
	 smiRange ; smiRange = smiGetNextRange(smiRange)) {
	if (smiRange->minValue.value.unsigned32 < min) {
	    min = smiRange->minValue.value.unsigned32;
	}
    }
    if (min < 65535 && min > size) {
	size = min;
    }

    parentType = smiGetParentType(smiType);
    if (parentType) {
	unsigned int psize = getMinSize(parentType);
	if (psize > size) {
	    size = psize;
	}
    }

    return size;
}



static unsigned int getMaxSize(SmiType *smiType)
{
    SmiRange *smiRange;
    SmiType  *parentType;
    SmiNamedNumber *nn;
    unsigned int max = 0, size;
    
    switch (smiType->basetype) {
    case SMI_BASETYPE_BITS:
    case SMI_BASETYPE_OCTETSTRING:
	size = 65535;
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	size = 128;
	break;
    default:
	return -1;
    }

    if (smiType->basetype == SMI_BASETYPE_BITS) {
	for (nn = smiGetFirstNamedNumber(smiType);
	     nn;
	     nn = smiGetNextNamedNumber(nn)) {
	    if (nn->value.value.unsigned32 > max) {
		max = nn->value.value.unsigned32;
	    }
	}
	size = (max / 8) + 1;
	return size;
    }

    for (smiRange = smiGetFirstRange(smiType);
	 smiRange ; smiRange = smiGetNextRange(smiRange)) {
	if (smiRange->maxValue.value.unsigned32 > max) {
	    max = smiRange->maxValue.value.unsigned32;
	}
    }
    if (max > 0 && max < size) {
	size = max;
    }

    parentType = smiGetParentType(smiType);
    if (parentType) {
	unsigned int psize = getMaxSize(parentType);
	if (psize < size) {
	    size = psize;
	}
    }

    return size;
}



static void printHeaderTypedef(FILE *f, SmiModule *smiModule,
			       SmiNode *groupNode)
{
    SmiNode *smiNode;
    SmiType *smiType;
    char    *cModuleName, *cGroupName, *cName;
    unsigned minSize, maxSize;

    cModuleName = translateLower(smiModule->name);
    cGroupName = translate(groupNode->name);

    fprintf(f,
	    "/*\n"
	    " * C type definitions for %s::%s.\n"
	    " */\n\n",
	    smiModule->name, groupNode->name);
    
    fprintf(f, "typedef struct %s {\n", cGroupName);
	    
    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
#if 0
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)
#endif
	    ) {
	    smiType = smiGetNodeType(smiNode);
	    if (!smiType) {
		continue;
	    }
	    
	    cName = translate(smiNode->name);
	    switch (smiType->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		maxSize = getMaxSize(smiType);
		minSize = getMinSize(smiType);
		fprintf(f,
			"    uint32_t  *%s;\n", cName);
		if (maxSize != minSize) {
		    fprintf(f,
			    "    size_t    _%sLength;\n", cName);
		}
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		maxSize = getMaxSize(smiType);
		minSize = getMinSize(smiType);
		fprintf(f,
			"    u_char    *%s;\n", cName);
		if (maxSize != minSize) {
		    fprintf(f,
			    "    size_t    _%sLength;\n", cName);
		}
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
		fprintf(f,
			"    int32_t   *%s;\n", cName);
		break;
	    case SMI_BASETYPE_UNSIGNED32:
		fprintf(f,
			"    uint32_t  *%s;\n", cName);
		break;
	    case SMI_BASETYPE_INTEGER64:
		fprintf(f,
			"    int64_t   *%s; \n", cName);
		break;
	    case SMI_BASETYPE_UNSIGNED64:
		fprintf(f,
			"    uint64_t  *%s; \n", cName);
		break;
	    default:
		fprintf(f,
			"    /* ?? */  _%s; \n", cName);
		break;
	    }
	    xfree(cName);
	}
    }
    
    fprintf(f,
	    "    void      *_clientData;\t\t"
	    "/* pointer to client data structure */\n");
    if (groupNode->nodekind == SMI_NODEKIND_ROW) {
	fprintf(f, "    struct %s *_nextPtr;\t"
		"/* pointer to next table entry */\n", cGroupName);
    }
    fprintf(f,
	    "\n    /* private space to hold actual values */\n\n");

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
#if 0
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)
#endif
	    ) {
	    smiType = smiGetNodeType(smiNode);
	    if (!smiType) {
		continue;
	    }
	    
	    cName = translate(smiNode->name);
	    switch (smiType->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		maxSize = getMaxSize(smiType);
		fprintf(f,
			"    uint32_t  _%s[%u];\n", cName, maxSize);
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		maxSize = getMaxSize(smiType);
		fprintf(f,
			"    u_char    _%s[%u];\n", cName, maxSize);
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
		fprintf(f,
			"    int32_t   _%s;\n", cName);
		break;
	    case SMI_BASETYPE_UNSIGNED32:
		fprintf(f,
			"    uint32_t  _%s;\n", cName);
		break;
	    case SMI_BASETYPE_INTEGER64:
		fprintf(f,
			"    int64_t   _%s; \n", cName);
		break;
	    case SMI_BASETYPE_UNSIGNED64:
		fprintf(f,
			"    uint64_t  _%s; \n", cName);
		break;
	    default:
		fprintf(f,
			"    /* ?? */  _%s; \n", cName);
		break;
	    }
	    xfree(cName);
	}
    }

    fprintf(f, "} %s_t;\n\n", cGroupName);

    fprintf(f, "extern int\n"
	    "%s_mgr_get_%s(struct snmp_session *s, %s_t **%s);\n",
	    cModuleName, cGroupName, cGroupName, cGroupName);
    fprintf(f, "\n");

    xfree(cGroupName);
    xfree(cModuleName);
}



static void printHeaderTypedefs(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    printHeaderTypedef(f, smiModule, smiNode);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void dumpHeader(SmiModule *smiModule, char *baseName)
{
    char *pModuleName;
    FILE *f;

    pModuleName = translateUpper(smiModule->name);

    f = createFile(baseName, ".h");
    if (! f) {
	return;
    }
    
    fprintf(f,
	    "/*\n"
	    " * This C header file has been generated by smidump "
	    SMI_VERSION_STRING ".\n"
	    " * It is intended to be used with the stools package.\n"
	    " *\n"
	    " * This header is derived from the %s module.\n"
	    " *\n * $I" "d$\n"
	    " */\n"
	    "\n", smiModule->name);

    fprintf(f,
	    "#ifndef _%s_H_\n"
	    "#define _%s_H_\n"
	    "\n"
	    "#include \"stools.h\"\n"
	    "\n",
	    pModuleName, pModuleName);

    printHeaderTypedefs(f, smiModule);

    fprintf(f,
	    "#endif /* _%s_H_ */\n",
	    pModuleName);

    fclose(f);
    xfree(pModuleName);
}



static void printOidDefinitions(FILE *f, SmiModule *smiModule)
{
    SmiNode *smiNode;
    char    *cName;
    int     i;
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && smiNode->access != SMI_ACCESS_NOTIFY) {
	    cName = translate(smiNode->name);
  	    fprintf(f, "static oid %s[] = {", cName);
	    for (i = 0; i < smiNode->oidlen; i++) {
		fprintf(f, "%s%u", i ? ", " : "", smiNode->oid[i]);
	    }
	    fprintf(f, "};\n");
	    xfree(cName);
	}
    }
    fprintf(f, "\n");
}



static void printGetScalarAssignement(FILE *f, SmiNode *groupNode)
{
    SmiNode *smiNode;
    SmiType *smiType;
    char    *cGroupName, *cName;
    unsigned maxSize, minSize;

    cGroupName = translate(groupNode->name);

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {

	    smiType = smiGetNodeType(smiNode);
	    if (!smiType) {
		continue;
	    }
	    
	    cName = translate(smiNode->name);
	    fprintf(f,
		    "        if (vars->name_length > sizeof(%s)/sizeof(oid)\n"
		    "            && memcmp(vars->name, %s, sizeof(%s)) == 0) {\n",
		    cName, cName, cName);
	    switch (smiType->basetype) {
	    case SMI_BASETYPE_INTEGER32:
	    case SMI_BASETYPE_UNSIGNED32:
	    case SMI_BASETYPE_ENUM:
		fprintf(f,
			"            (*%s)->_%s = *vars->val.integer;\n"
			"            (*%s)->%s = &((*%s)->_%s);\n",
			cGroupName, cName,
			cGroupName, cName, cGroupName, cName);
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		maxSize = getMaxSize(smiType);
		minSize = getMinSize(smiType);
		fprintf(f,
			"            memcpy((*%s)->_%s, vars->val.string, vars->val_len);\n",
			cGroupName, cName);
		if (minSize != maxSize) {
		    fprintf(f,
			    "            (*%s)->_%sLength = vars->val_len;\n",
			    cGroupName, cName);
		}
		fprintf(f,
			"            (*%s)->%s = (*%s)->_%s;\n",
			cGroupName, cName, cGroupName, cName);
		break;
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		fprintf(f,
			"            memcpy((*%s)->_%s, vars->val.string, vars->val_len);\n",
			cGroupName, cName);
		fprintf(f,
			"            (*%s)->_%sLength = vars->val_len / sizeof(oid);\n",
			cGroupName, cName);
		fprintf(f,
			"            (*%s)->%s = (*%s)->_%s;\n",
			cGroupName, cName, cGroupName, cName);
		break;
	    default:
		break;
	    }
	    fprintf(f,
		    "        }\n");
	    xfree(cName);
	}
    }

    xfree(cGroupName);
}



static void printGetMethod(FILE *f, SmiModule *smiModule,
			      SmiNode *groupNode)
{
    SmiNode *smiNode;
    char    *cModuleName, *cGroupName;

    cModuleName = translateLower(smiModule->name);
    cGroupName = translate(groupNode->name);

    fprintf(f,
	    "int %s_mgr_get_%s(struct snmp_session *s, %s_t **%s)\n"
	    "{\n"
	    "    struct snmp_pdu *request, *response;\n"
	    "    struct variable_list *vars;\n"
	    "    int status;\n"
	    "\n",
	    cModuleName, cGroupName, cGroupName, cGroupName);

    fprintf(f,
	    "    request = snmp_pdu_create(SNMP_MSG_GETNEXT);\n");
	    
    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    fprintf(f,
	    "    snmp_add_null_var(request, %s, sizeof(%s)/sizeof(oid));\n",
		    smiNode->name, smiNode->name);
	}
    }

    fprintf(f,
#if 0
	    "\n"
	    "    peer = snmp_open(s);\n"
	    "    if (!peer) {\n"
	    "        return -1;\n"
	    "    }\n"
#endif
	    "\n"
	    "    status = snmp_synch_response(s, request, &response);\n"
	    "    if (status != STAT_SUCCESS) {\n"
	    "        return -2;\n"
	    "    }\n"
	    "\n");

    /* generate code for error checking and handling */

    fprintf(f,
	    "    *%s = (%s_t *) calloc(1, sizeof(%s_t));\n"
	    "    if (! *%s) {\n"
	    "        return -4;\n"
	    "    }\n"
	    "\n",
	    cGroupName, cGroupName, cGroupName, cGroupName);

    fprintf(f,
	    "    for (vars = response->variables; vars; vars = vars->next_variable) {\n"
	    "        if (vars->type == SNMP_ENDOFMIBVIEW\n"
            "            || (vars->type == SNMP_NOSUCHOBJECT)\n"
            "            || (vars->type == SNMP_NOSUCHINSTANCE)) {\n"
            "            continue;\n"
	    "        }\n");
    printGetScalarAssignement(f, groupNode);
    fprintf(f,
	    "    }\n"
	    "\n");


#if 0
    if (response->errstat != SNMP_ERR_NOERROR) {
	return -3;
    }

    /* copy to data structures */

    /* cleanup */

#endif

    fprintf(f,
	    "    if (response) snmp_free_pdu(response);\n"
	    "\n"
#if 0
	    "    if (snmp_close(peer) == 0) {\n"
	    "        return -5;\n"
	    "    }\n"
	    "\n"
#endif
	    "    return 0;\n"
	    "}\n\n");

    xfree(cGroupName);
    xfree(cModuleName);
}
 



static void printGetMethods(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for (smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    printGetMethod(f, smiModule, smiNode);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void dumpMgrStub(SmiModule *smiModule, char *baseName)
{
    FILE *f;

    f = createFile(baseName, ".c");
    if (! f) {
        return;
    }

    fprintf(f,
	    "/*\n"
	    " * This C file has been generated by smidump "
	    SMI_VERSION_STRING ".\n"
	    " * It is intended to be used with the stools library.\n"
	    " *\n"
	    " * This C file is derived from the %s module.\n"
	    " *\n * $I" "d$\n"
	    " */\n"
	    "\n"
	    "#include \"%s.h\"\n"
	    "\n",
	    smiModule->name, baseName);
	
    printOidDefinitions(f, smiModule);
    
    printGetMethods(f, smiModule);
    
    fclose(f);
}



static void dumpStools(int modc, SmiModule **modv, int flags, char *output)
{
    char	*baseName;
    int		i;

    if (flags & SMIDUMP_FLAG_UNITE) {
	/* not implemented yet */
    } else {
	for (i = 0; i < modc; i++) {
	    baseName = output ? output : translateFileName(modv[i]->name);
	    dumpHeader(modv[i], baseName);
	    dumpMgrStub(modv[i], baseName);
	    if (! output) xfree(baseName);
	}
    }

}



void initStools()
{
    static SmidumpDriver driver = {
	"stools",
	dumpStools,
	SMI_FLAG_NODESCR,
	SMIDUMP_DRIVER_CANT_UNITE,
	"ANSI C manager stubs for the stools package",
	NULL,
	NULL
    };

    smidumpRegisterDriver(&driver);
}
