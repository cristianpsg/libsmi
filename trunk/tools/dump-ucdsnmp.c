/*
 * dump-ucdsnmp.c --
 *
 *      Operations to generate UCD SNMP mib module implementation code.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 * Copyright (c) 1999 J. Schoenwaelder, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-ucdsnmp.c,v 1.7 2000/02/08 21:39:23 strauss Exp $
 */

/*
 * TODO:
 *	  - assume that we build a dynamic loadable module
 *	  - update to 4.X version of the UCD API
 *	  - generate #defines for deprecated and obsolete objects
 *	  - generate stub codes for the various functions
 *	  - generate type and range checking code
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


#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */



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
	    smiFreeNode(childNode);
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
    int max = 0, size;
    
    switch (smiType->basetype) {
    case SMI_BASETYPE_OCTETSTRING:
	size = 65535;
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	size = 128;
	break;
    default:
	return -1;
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
	smiFreeType(parentType);
    }

    return size;
}



static void printReadMethodDecls(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Forward declaration of read methods for groups of scalars and tables:\n"
		       " */\n\n");
	    }
	    printf("static unsigned char *\nread_%s_stub(struct variable *,"
		   " oid *, size_t *, int, size_t *, WriteMethod **);\n",
		   smiNode->name);
	}
    }
    
    if (cnt) {
	printf("\n");
    }
}



static void printWriteMethodDecls(SmiModule *smiModule)
{
    SmiNode     *smiNode;
    int         cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->access == SMI_ACCESS_READ_WRITE) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Forward declaration of write methods for writable objects:\n"
		       " */\n\n");
	    }
	    printf("static int\nwrite_%s_stub(int,"
		   " u_char *, u_char, int, u_char *, oid *, int);\n",
		   smiNode->name);
	}
    }
    
    if (cnt) {
	printf("\n");
    }
}



static void printDefinesGroup(SmiNode *groupNode, int cnt)
{
    char      *cName, *cGroupName;
    SmiNode   *smiNode;
    int	      i, num = 0;

    if (cnt == 1) {
	printf("/*\n"
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
	    printf("#define %-32s %d\n", cName,
		   smiNode->oid[smiNode->oidlen-1]);
	    xfree(cName);
	}
    }
    printf("\n");

    if (num) {
	printf("static oid %s_base[] = {", cGroupName);
	for (i = 0; i < groupNode->oidlen; i++) {
	    printf("%s%d", i ? ", " : "", groupNode->oid[i]);
	}
	printf("};\n\n");
	printf("struct variable %s_variables[] = {\n", cGroupName);
	for (smiNode = smiGetFirstChildNode(groupNode);
	     smiNode;
	     smiNode = smiGetNextChildNode(smiNode)) {
	    if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
		&& (smiNode->access == SMI_ACCESS_READ_ONLY
		    || smiNode->access == SMI_ACCESS_READ_WRITE)) {
		cName = translateUpper(smiNode->name);
		printf("    { %s, %s, %s, read_%s_stub, %d, {%d} },\n",
		       cName, getBaseTypeString(smiNode->basetype),
		       getAccessString(smiNode->access),
		       cGroupName, 1, smiNode->oid[smiNode->oidlen-1]);
		xfree(cName);
	    }
	}
	printf("};\n\n");
    }

    xfree(cGroupName);
}



static void printDefines(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printDefinesGroup(smiNode, ++cnt);
	}
    }
    
    if (cnt) {
	printf("\n");
    }
}



static void printRegister(SmiNode *groupNode, int cnt)
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
    printf("\n");

    if (cnt == 1) {
	printf("/*\n"
	       " * Registration functions for the various MIB groups.\n"
	       " */\n\n");
    }
    
    cGroupName = translate(groupNode->name);

    printf("int register_%s()\n{\n", cGroupName);
    printf("    return register_mib(\"%s\",\n"
	   "         %s_variables,\n"
	   "         sizeof(struct variable),\n"
	   "         sizeof(%s_variables)/sizeof(struct variable),\n"
	   "         %s_base,\n"
	   "         sizeof(%s_base)/sizeof(oid));\n",
	   cGroupName, cGroupName, cGroupName, cGroupName, cGroupName);
    printf("};\n\n");

    xfree(cGroupName);
}



static void printInit(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;

    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printRegister(smiNode, ++cnt);
	}
    }

    if (cnt) {
	printf("\n");
    }
}



static void printReadMethod(SmiNode *groupNode)
{
    SmiNode   *smiNode;
    char      *cName, *sName, *lName;

    sName = translate(groupNode->name);

    printf("static unsigned char *\nread_%s_stub(struct variable *vp,\n"
	   "    oid     *name,\n"
	   "    size_t  *length,\n"
	   "    int     exact,\n"
	   "    size_t  *var_len,\n"
	   "    WriteMethod **write_method)\n"
	   "{\n", sName);

    printf("    static %s_t %s;\n\n", sName, sName);

    smiNode = smiGetFirstChildNode(groupNode);
    if (smiNode && smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	printf("    /* check whether the instance identifier is valid */\n\n");
	printf("    if (header_generic(vp, name, length, exact, var_len,\n"
	       "                       write_method) == MATCH_FAILED) {\n"
	       "        return NULL;\n"
	       "    }\n\n");
    }
    if (smiNode) {
	smiFreeNode(smiNode);
    }

    printf("    /* call the user supplied function to retrieve values */\n\n");
    printf("    read_%s(&%s);\n\n", sName, sName);

    printf("    /* return the current value of the variable */\n\n");
    printf("    switch (vp->magic) {\n\n");

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    cName = translateUpper(smiNode->name);
	    lName = translate(smiNode->name);
	    printf("    case %s:\n", cName);
	    switch (smiNode->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		printf("        *var_len = %s._%sLength;\n"
		       "        return (unsigned char *) %s.%s;\n",
		       sName, lName, sName, lName);
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		printf("        *var_len = %s._%sLength;\n"
		       "        return (unsigned char *) %s.%s;\n",
		       sName, lName, sName, lName);
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
	    case SMI_BASETYPE_UNSIGNED32:
		printf("        return (unsigned char *) &%s.%s;\n",
		       sName, lName);
		break;
	    default:
		printf("        /* add code to return the value here */\n");
	    }
	    printf("\n");
	    xfree(cName);
	    xfree(lName);
	}
    }

    printf("    default:\n"
	   "         ERROR_MSG(\"\");\n"
	   "    }\n\n");
    
    printf("    return NULL;\n"
	   "}\n\n");

    xfree(sName);
}



static void printReadMethods(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Read methods for groups of scalars and tables:\n"
		       " */\n\n");
	    }
	    printReadMethod(smiNode);
	}
    }
    
    if (cnt) {
	printf("\n");
    }
}



static void printWriteMethods(SmiModule *smiModule)
{
    SmiNode     *smiNode;
    int         cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->access == SMI_ACCESS_READ_WRITE) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Forward declaration of write methods for writable objects:\n"
		       " */\n\n");
	    }
	    printf("static int\nwrite_%s_stub(int action,\n"
		   "    u_char   *var_val,\n"
		   "    u_char   var_val_type,\n"
		   "    int      var_val_len,\n"
		   "    u_char   *statP,\n"
		   "    oid      *name,\n"
		   "    int      name_len)\n"
		   "{\n", smiNode->name);
	    printf("    return SNMP_ERR_NOERROR;\n"
		   "}\n\n");
	}
    }
    
    if (cnt) {
	printf("\n");
    }
}



static void printTypedef(SmiNode *groupNode)
{
    SmiNode *smiNode;
    SmiType *smiType;
    char    *cGroupName, *cName;
    
    cGroupName = translate(groupNode->name);
    printf("typedef struct %s {\n", cGroupName);

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
	    switch (smiNode->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		printf("    u_int32_t %s[%u];\n", cName, getMaxSize(smiType));
		printf("    size_t    _%sLength;\n", cName);
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		printf("    u_char    %s[%u];\n", cName, getMaxSize(smiType));
		printf("    size_t    _%sLength;\n", cName);
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
		printf("    int32_t   %s;\n", cName);
		break;
	    case SMI_BASETYPE_UNSIGNED32:
		printf("    u_int32_t %s;\n", cName);
		break;
	    default:
		printf("    /* add code to return the value here */\n");
	    }
	    xfree(cName);
	    smiFreeType(smiType);
	}
    }

    printf("    void      *_clientData;\t\t/* pointer to client data structure */\n");
    if (groupNode->nodekind == SMI_NODEKIND_ROW) {
	printf("    struct %s *_nextPtr;\t/* pointer to next table entry */\n", cGroupName);
    }
    
    printf("} %s_t;\n\n", cGroupName);
    printf("extern int\n"
	   "read_%s(%s_t *%s);\n\n",
	   cGroupName, cGroupName, cGroupName);
    printf("extern int\n"
	   "register_%s();\n\n",
	   cGroupName);
    xfree(cGroupName);
}



static void printTypedefs(SmiModule *smiModule)
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
		printf("/*\n"
		       " * Structures for groups of scalars and table entries:\n"
		       " */\n\n");
	    }
	    printTypedef(smiNode);
	}
    }
    
    if (cnt) {
	printf("\n");
    }

    if (cnt) {
	cModuleName = translateLower(smiModule->name);
	printf("typedef struct %s {\n", cModuleName);
	for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	    smiNode;
	    smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	    if (isGroup(smiNode) && isAccessible(smiNode)) {
		cSmiNodeName = translate(smiNode->name);
		if (smiNode->nodekind == SMI_NODEKIND_ROW) {
		    printf("    %s_t\t*%s;\n", cSmiNodeName, cSmiNodeName);
		} else {
		    printf("    %s_t\t%s;\n", cSmiNodeName, cSmiNodeName);
		}
		xfree(cSmiNodeName);
	    }
	}
	printf("} %s_t;\n\n", cModuleName);
	xfree(cModuleName);
    }
}



int dumpUcdH(char *modulename, int flags)
{
    SmiModule    *smiModule;
    char	 *cModuleName;
    char	 *cInitName;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    printf("/*\n"
	   " * This C header file has been generated by smidump " VERSION ".\n"
	   " * It is intended to be used with the UCD SNMP agent.\n"
	   " *\n"
	   " * This header is derived from the %s module.\n"
	   " *\n * $I" "d$\n"
	   " */\n\n", smiModule->name);

    cModuleName = translate(smiModule->name);

    printf("#ifndef _%s_H_\n", cModuleName);
    printf("#define _%s_H_\n\n", cModuleName);

    printf("#include <stdlib.h>\n\n");

    printf("/*\n"
	   " * Initialization function:\n"
	   " */\n\n");
    cInitName = translateLower(smiModule->name);
    printf("void init_%s(void);\n\n", cInitName);
    xfree(cInitName);

    printTypedefs(smiModule);

    printf("#endif /* _%s_H_ */\n", cModuleName);
    xfree(cModuleName);

    smiFreeModule(smiModule);
    
    return 0;
}



int dumpUcdC(char *modulename, int flags)
{
    SmiModule    *smiModule;
    char	 *cModuleName;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    printf("/*\n"
	   " * This C file has been generated by smidump " VERSION ".\n"
	   " * It is intended to be used with the UCD SNMP agent.\n"
	   " *\n"
	   " * This C file is derived from the %s module.\n"
	   " *\n * $I" "d$\n"
	   " */\n\n", smiModule->name );
	
    printf("#include <stdio.h>\n"
	   "#include <string.h>\n"
	   "#include <malloc.h>\n"
	   "\n");

    cModuleName = translateLower(smiModule->name);
    printf("#include \"%s.h\"\n", cModuleName);
    printf("\n");
    xfree(cModuleName);

    printf("#include <asn1.h>\n"
	   "#include <snmp.h>\n"
	   "#include <snmp_api.h>\n"
	   "#include <snmp_impl.h>\n"
	   "#include <snmp_vars.h>\n"
	   "\n");

    printReadMethodDecls(smiModule);
    printWriteMethodDecls(smiModule);
    printDefines(smiModule);
    printInit(smiModule);

    printReadMethods(smiModule);
    printWriteMethods(smiModule);

    smiFreeModule(smiModule);

    return 0;
}
