/*
 * dump-ucdsnmp.c --
 *
 *      Operations to generate NET SNMP mib module implementation code.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-ucdsnmp.c,v 1.19 2000/10/18 07:47:32 strauss Exp $
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
    
    for(childNode = smiGetFirstChildNode(smiNode);
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



static int getMaxSize(SmiType *smiType)
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

    for(smiRange = smiGetFirstRange(smiType);
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
	int psize = getMaxSize(parentType);
	if (psize < size) {
	    size = psize;
	}
    }

    return size;
}



static void printReadMethodDecls(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		fprintf(f,
			"/*\n"
			" * Forward declaration of read methods for groups of scalars and tables:\n"
			" */\n\n");
	    }
	    fprintf(f,
		    "static unsigned char *\nread_%s_stub(struct variable *,"
		    " oid *, size_t *, int, size_t *, WriteMethod **);\n",
		    smiNode->name);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void printWriteMethodDecls(FILE *f, SmiModule *smiModule)
{
    SmiNode     *smiNode;
    int         cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->access == SMI_ACCESS_READ_WRITE) {
	    cnt++;
	    if (cnt == 1) {
		fprintf(f,
			"/*\n"
			" * Forward declaration of write methods for writable objects:\n"
			" */\n\n");
	    }
	    fprintf(f,
		    "static int\nwrite_%s_stub(int,"
		    " u_char *, u_char, int, u_char *, oid *, int);\n",
		    smiNode->name);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void printDefinesGroup(FILE *f, SmiNode *groupNode, int cnt)
{
    char      *cName, *cGroupName;
    SmiNode   *smiNode;
    SmiType   *smiType;
    int	      i, num = 0;

    if (cnt == 1) {
	fprintf(f,
	"/*\n"
	" * Definitions of tags that are used internally to read/write\n"
	" * the selected object type. These tags should be unique.\n"
	" */\n\n");
    }

    cGroupName = translate(groupNode->name);

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    num++;
	    cName = translateUpper(smiNode->name);
	    fprintf(f, "#define %-32s %d\n", cName,
		    smiNode->oid[smiNode->oidlen-1]);
	    xfree(cName);
	}
    }
    fprintf(f, "\n");

    if (num) {
	fprintf(f, "static oid %s_base[] = {", cGroupName);
	for (i = 0; i < groupNode->oidlen; i++) {
	    fprintf(f, "%s%d", i ? ", " : "", groupNode->oid[i]);
	}
	fprintf(f, "};\n\n");
	fprintf(f, "struct variable %s_variables[] = {\n", cGroupName);
	for (smiNode = smiGetFirstChildNode(groupNode);
	     smiNode;
	     smiNode = smiGetNextChildNode(smiNode)) {
	    if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
		&& (smiNode->access == SMI_ACCESS_READ_ONLY
		    || smiNode->access == SMI_ACCESS_READ_WRITE)) {
		smiType = smiGetNodeType(smiNode);
		cName = translateUpper(smiNode->name);
		fprintf(f, "    { %s, %s, %s, read_%s_stub, %d, {%d} },\n",
			cName, getBaseTypeString(smiType->basetype),
			getAccessString(smiNode->access),
			cGroupName, 1, smiNode->oid[smiNode->oidlen-1]);
		xfree(cName);
	    }
	}
	fprintf(f, "};\n\n");
    }

    xfree(cGroupName);
}



static void printDefines(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printDefinesGroup(f, smiNode, ++cnt);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void printRegister(FILE *f, SmiNode *groupNode, int cnt)
{
    SmiNode *smiNode;
    char    *cGroupName;
    int     num = 0;

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    num++;
	}
    }
    fprintf(f, "\n");

    if (cnt == 1) {
	fprintf(f,
		"/*\n"
		" * Registration functions for the various MIB groups.\n"
		" */\n\n");
    }
    
    cGroupName = translate(groupNode->name);

    fprintf(f, "int register_%s()\n{\n", cGroupName);
    fprintf(f,
	    "    return register_mib(\"%s\",\n"
	    "         %s_variables,\n"
	    "         sizeof(struct variable),\n"
	    "         sizeof(%s_variables)/sizeof(struct variable),\n"
	    "         %s_base,\n"
	    "         sizeof(%s_base)/sizeof(oid));\n",
	    cGroupName, cGroupName, cGroupName, cGroupName, cGroupName);
    fprintf(f, "};\n\n");

    xfree(cGroupName);
}



static void printInit(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;

    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printRegister(f, smiNode, ++cnt);
	}
    }

    if (cnt) {
	fprintf(f, "\n");
    }
}



static void printReadMethod(FILE *f, SmiNode *groupNode)
{
    SmiNode   *smiNode;
    SmiType   *smiType;
    char      *cName, *sName, *lName;

    sName = translate(groupNode->name);

    fprintf(f,
	    "static unsigned char *\nread_%s_stub(struct variable *vp,\n"
	    "    oid     *name,\n"
	    "    size_t  *length,\n"
	    "    int     exact,\n"
	    "    size_t  *var_len,\n"
	    "    WriteMethod **write_method)\n"
	    "{\n", sName);

    fprintf(f, "    static %s_t %s;\n\n", sName, sName);
    
    smiNode = smiGetFirstChildNode(groupNode);
    if (smiNode && smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	fprintf(f,
		"    /* check whether the instance identifier is valid */\n"
		"\n"
		"    if (header_generic(vp, name, length, exact, var_len,\n"
		"                       write_method) == MATCH_FAILED) {\n"
		"        return NULL;\n"
		"    }\n"
		"\n");
    }

    fprintf(f,
	    "    /* call the user supplied function to retrieve values */\n"
	    "\n"
	    "    read_%s(&%s);\n"
	    "\n", sName, sName);

    fprintf(f,
	    "    /* return the current value of the variable */\n"
	    "\n"
	    "    switch (vp->magic) {\n"
	    "\n");

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    cName = translateUpper(smiNode->name);
	    lName = translate(smiNode->name);
	    smiType = smiGetNodeType(smiNode);
	    fprintf(f, "    case %s:\n", cName);
	    switch (smiType->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		fprintf(f,
			"        *var_len = %s._%sLength;\n"
			"        return (unsigned char *) %s.%s;\n",
			sName, lName, sName, lName);
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		fprintf(f,
			"        *var_len = %s._%sLength;\n"
			"        return (unsigned char *) %s.%s;\n",
			sName, lName, sName, lName);
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
	    case SMI_BASETYPE_UNSIGNED32:
		fprintf(f,
			"        return (unsigned char *) &%s.%s;\n",
			sName, lName);
		break;
	    default:
		fprintf(f,
			"        /* add code to return the value here */\n");
	    }
	    fprintf(f, "\n");
	    xfree(cName);
	    xfree(lName);
	}
    }

    fprintf(f,
	    "    default:\n"
	    "         ERROR_MSG(\"\");\n"
	    "    }\n"
	    "\n"
	    "    return NULL;\n"
	    "}\n"
	    "\n");

    xfree(sName);
}



static void printReadMethods(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		fprintf(f,
			"/*\n"
			" * Read methods for groups of scalars and tables:\n"
			" */\n\n");
	    }
	    printReadMethod(f, smiNode);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void printWriteMethods(FILE *f, SmiModule *smiModule)
{
    SmiNode     *smiNode;
    int         cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->access == SMI_ACCESS_READ_WRITE) {
	    cnt++;
	    if (cnt == 1) {
		fprintf(f,
			"/*\n"
			" * Forward declaration of write methods for writable objects:\n"
			" */\n\n");
	    }
	    fprintf(f,
		    "static int\nwrite_%s_stub(int action,\n"
		    "    u_char   *var_val,\n"
		    "    u_char   var_val_type,\n"
		    "    int      var_val_len,\n"
		    "    u_char   *statP,\n"
		    "    oid      *name,\n"
		    "    int      name_len)\n"
		    "{\n", smiNode->name);
	    fprintf(f,
		    "    return SNMP_ERR_NOERROR;\n"
		    "}\n\n");
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }
}



static void printTypedef(FILE *f, SmiNode *groupNode)
{
    SmiNode *smiNode;
    SmiType *smiType;
    char    *cGroupName, *cName;
    
    cGroupName = translate(groupNode->name);
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
	    cName = translate(smiNode->name);
	    switch (smiType->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		fprintf(f,
			"    uint32_t  %s[%u];\n", cName, getMaxSize(smiType));
		fprintf(f,
			"    size_t    _%sLength;\n", cName);
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		fprintf(f,
			"    u_char    %s[%u];\n", cName, getMaxSize(smiType));
		fprintf(f,
			"    size_t    _%sLength;\n", cName);
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
		fprintf(f,
			"    int32_t   %s;\n", cName);
		break;
	    case SMI_BASETYPE_UNSIGNED32:
		fprintf(f,
			"    uint32_t  %s;\n", cName);
		break;
	    default:
		fprintf(f,
			"    /* add code to return the value here */\n");
	    }
	    xfree(cName);
	}
    }

    fprintf(f, "    void      *_clientData;\t\t/* pointer to client data structure */\n");
    if (groupNode->nodekind == SMI_NODEKIND_ROW) {
	fprintf(f, "    struct %s *_nextPtr;\t/* pointer to next table entry */\n", cGroupName);
    }
    
    fprintf(f, "} %s_t;\n\n", cGroupName);
    fprintf(f, "extern int\n"
	    "read_%s(%s_t *%s);\n\n",
	    cGroupName, cGroupName, cGroupName);
    fprintf(f, "extern int\n"
	    "register_%s();\n\n",
	    cGroupName);
    xfree(cGroupName);
}



static void printTypedefs(FILE *f, SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    char      *cModuleName;
    char      *cSmiNodeName;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		fprintf(f,
			"/*\n"
			" * Structures for groups of scalars and table entries:\n"
			" */\n\n");
	    }
	    printTypedef(f, smiNode);
	}
    }
    
    if (cnt) {
	fprintf(f, "\n");
    }

    if (cnt) {
	cModuleName = translateLower(smiModule->name);
	fprintf(f, "typedef struct %s {\n", cModuleName);
	for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	    smiNode;
	    smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	    if (isGroup(smiNode) && isAccessible(smiNode)) {
		cSmiNodeName = translate(smiNode->name);
		if (smiNode->nodekind == SMI_NODEKIND_ROW) {
		    fprintf(f, "    %s_t\t*%s;\n", cSmiNodeName, cSmiNodeName);
		} else {
		    fprintf(f, "    %s_t\t%s;\n", cSmiNodeName, cSmiNodeName);
		}
		xfree(cSmiNodeName);
	    }
	}
	fprintf(f, "} %s_t;\n\n", cModuleName);
	xfree(cModuleName);
    }
}



static void dumpHeader(SmiModule *smiModule, char *cModuleName)
{
    char	*pModuleName;
    char	*cInitName;
    FILE	*f;

    pModuleName = translateUpper(smiModule->name);

    f = createFile(cModuleName, ".h");
    if (! f) {
	return;
    }
    
    fprintf(f,
	    "/*\n"
	    " * This C header file has been generated by smidump "
	    SMI_VERSION_STRING ".\n"
	    " * It is intended to be used with the NET SNMP agent.\n"
	    " *\n"
	    " * This header is derived from the %s module.\n"
	    " *\n * $I" "d$\n"
	    " */\n\n", smiModule->name);

    fprintf(f, "#ifndef _%s_H_\n", pModuleName);
    fprintf(f, "#define _%s_H_\n\n", pModuleName);

    fprintf(f, "#include <stdlib.h>\n\n");

    fprintf(f,
	    "#ifdef HAVE_STDINT_H\n"
	    "#include <stdint.h>\n"
	    "#endif\n\n");

    fprintf(f,
	    "/*\n"
	    " * Initialization function:\n"
	    " */\n\n");
    cInitName = translateLower(smiModule->name);
    fprintf(f, "void init_%s(void);\n\n", cInitName);
    xfree(cInitName);

    printTypedefs(f, smiModule);

    fprintf(f, "#endif /* _%s_H_ */\n", pModuleName);

    fclose(f);
    xfree(pModuleName);
}



static void dumpStub(SmiModule *smiModule, char *cModuleName)
{
    char	*stubModuleName;
    FILE	*f;

    stubModuleName = xmalloc(strlen(cModuleName) + 10);
    strcpy(stubModuleName, cModuleName);
    strcat(stubModuleName, "_stub");
    
    f = createFile(stubModuleName, ".c");
    if (! f) {
        return;
    }

    fprintf(f,
	    "/*\n"
	    " * This C file has been generated by smidump "
	    SMI_VERSION_STRING ".\n"
	    " * It is intended to be used with the NET SNMP agent.\n"
	    " *\n"
	    " * This C file is derived from the %s module.\n"
	    " *\n * $I" "d$\n"
	    " */\n\n", smiModule->name );
	
    fprintf(f,
	    "#include <stdio.h>\n"
	    "#include <string.h>\n"
	    "#include <malloc.h>\n"
	    "\n"
	    "#include \"%s.h\"\n"
	    "\n"
	    "#include <asn1.h>\n"
	    "#include <snmp.h>\n"
	    "#include <snmp_api.h>\n"
	    "#include <snmp_impl.h>\n"
	    "#include <snmp_vars.h>\n"
	    "\n",
	    cModuleName);

    printReadMethodDecls(f, smiModule);
    printWriteMethodDecls(f, smiModule);
    printDefines(f, smiModule);
    printInit(f, smiModule);

    printReadMethods(f, smiModule);
    printWriteMethods(f, smiModule);

    fclose(f);
    xfree(stubModuleName);
}



static void dumpImplementation(SmiModule *smiModule, char *cModuleName)
{
    FILE	*f;

    f = createFile(cModuleName, ".c");
    if (! f) {
        return;
    }

    fprintf(f,
	    "/*\n"
	    " * This C file has been generated by smidump "
	    SMI_VERSION_STRING ".\n"
	    " * It is intended to be used with the NET SNMP agent.\n"
	    " *\n"
	    " * This C file is derived from the %s module.\n"
	    " *\n * $I" "d$\n"
	    " */\n\n", smiModule->name );
	
    fprintf(f,
	    "#include <stdio.h>\n"
	    "#include <string.h>\n"
	    "#include <malloc.h>\n"
	    "\n"
	    "#include \"%s.h\"\n"
	    "\n"
	    "#include <asn1.h>\n"
	    "#include <snmp.h>\n"
	    "#include <snmp_api.h>\n"
	    "#include <snmp_impl.h>\n"
	    "#include <snmp_vars.h>\n"
	    "\n",
	    cModuleName);

    fprintf(f,
	    "static oid %s_caps[] = {0,0};\n"
	    "\n",
	    cModuleName);

    fprintf(f,
	    "void init_%s(void)\n"
	    "{\n"
#if 0
	    /* create an entry in the sysORTable */
	    
	    register_sysORTable(if_mib_caps, sizeof(if_mib_caps),
				"IF-MIB implementation version 0.0.");
	    
	    /* register the various parts of the MIB */
	    
	    register_interfaces();
	    register_ifEntry();
	    
	    /* register essential callbacks */
	    
	    snmp_register_callback(SNMP_CALLBACK_LIBRARY,
				   SNMP_CALLBACK_SHUTDOWN,
				   term_if_mib, NULL);
#endif
	    "}\n"
	    "\n",
	    cModuleName);


    fprintf(f,
	    "void deinit_%s()\n"
	    "{\n"
	    "    unregister_sysORTable(%s_caps, sizeof(%s_caps));\n"
	    "}\n"
	    "\n",
	    cModuleName, cModuleName, cModuleName);

    fprintf(f,
	    "int term_%s()\n"
	    "{\n"
	    "    deinit_%s();\n"
	    "    return 0;\n"
	    "}\n"
	    "\n",
	    cModuleName, cModuleName);
	    

    fclose(f);
}



void dumpNetSnmp(Module *module)
{
    SmiModule   *smiModule;
    char	*cModuleName;

    smiModule = module->smiModule;
    cModuleName = translateLower(smiModule->name);

    dumpHeader(smiModule, cModuleName);
    dumpStub(smiModule, cModuleName);
    dumpImplementation(smiModule, cModuleName);

    xfree(cModuleName);
}
