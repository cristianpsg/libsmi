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
 * @(#) $Id$
 */

/*
 * TODO:
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



#define  INDENT		4    /* indent factor */
#define  INDENTVALUE	20   /* column to start values, except multiline */
#define  INDENTTEXTS	13   /* column to start multiline texts */
#define  INDENTMAX	72   /* max column to fill, break lines otherwise */



static char *safeStrdup(const char *s)
{
    char *m = strdup(s);
    if (! m) {
	fprintf(stderr, "smidump: strdup failed - running out of memory\n");
	exit(1);
    }
    return m;
}



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

    s = safeStrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
    }
  
    return s;
}



static char* translateUpper(char *m)
{
    char *s;
    int i;

    s = safeStrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
	if (islower((int) s[i])) {
	    s[i] = toupper(s[i]);
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



static void printReadMethodDecls(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Forward declaration of read methods for groups of scalars and tables:\n"
		       " */\n\n");
	    }
	    printf("static unsigned char *\nread_%s(struct variable *,"
		   " oid *, int *, int, int *, WriteMethod **);\n",
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
    
    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->access == SMI_ACCESS_READ_WRITE) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Forward declaration of write methods for writable objects:\n"
		       " */\n\n");
	    }
	    printf("static int\nwrite_%s(int,"
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
	    free(cName);
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
		printf("    { %s, %s, %s, read_%s, %d, {%d} }\n",
		       cName, getBaseTypeString(smiNode->basetype),
		       getAccessString(smiNode->access),
		       cGroupName, 1, smiNode->oid[smiNode->oidlen-1]);
		free(cName);
	    }
	}
	printf("};\n\n");
    }

    free(cGroupName);
}



static void printDefines(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
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
	printf("    /*\n"
	       "     * Register scalars groups and rows with the agent.\n"
	       "     */\n\n");
    }
    
    cGroupName = translate(groupNode->name);

    printf("#ifdef UCD_SNMP\n");
    printf("    REGISTER_MIB(\"%s\", %s_variables, variable, %s_base);\n",
	   cGroupName, cGroupName, cGroupName);
    printf("#endif\n");
    printf("#ifdef CMU_LINUX_SNMP\n");
    printf("    mib_register(%s_base,\n",
	   cGroupName);
    printf("                sizeof(%s_base)/sizeof(oid),\n",
	   cGroupName);
    printf("                %s_variables,\n", cGroupName);
    printf("                sizeof(%s_variables)/sizeof(*%s_variables),\n",
	   cGroupName, cGroupName);
    printf("                sizeof(*%s_variables));\n", cGroupName);
    printf("#endif\n");

    free(cGroupName);
}



static void printInit(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    char      *cModuleName;
    int       cnt = 0;

    cModuleName = translate(smiModule->name);
	    
    printf("void init_%s()\n{\n", cModuleName);

    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printRegister(smiNode, ++cnt);
	}
    }

    if (cnt) {
	printf("\n");
    }

    printf("    /*\n"
	   "     * Place any other initialization code you need here.\n"
	   "     */\n\n");
    printf("};\n\n");

    free(cModuleName);
}



static void printReadMethod(SmiNode *groupNode)
{
    SmiNode   *smiNode;
    char      *cName;

    printf("static unsigned char *\nread_%s(struct variable *vp,\n"
	   "         oid     *name,\n"
	   "         int     *length,\n"
	   "         int     exact,\n"
	   "         int     *var_len,\n"
	   "         WriteMethod **write_method)\n"
	   "{\n", groupNode->name);

    printf("    /* variables we may use later */\n\n"
	   "    static long long_ret;\n"
	   "    static unsigned char string[SPRINT_MAX_LEN];\n"
	   "    static oid objid[MAX_OID_LEN];\n"
	   "    static struct counter64 c64;\n\n");

    smiNode = smiGetFirstChildNode(groupNode);
    if (smiNode && smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	printf("    /* check whether the instance identifier is valid */\n\n");
	printf("    if (header_generic(vp, name, length, exact, var_len,\n"
	       "                       write_method) == MATCH_FAILED) {\n"
	       "        return NULL;\n"
	       "    }\n\n");
    }

    printf("    /* return the current value of the variable */\n\n");
    printf("    switch (vp->magic) {\n\n");

    for (smiNode = smiGetFirstChildNode(groupNode);
	 smiNode;
	 smiNode = smiGetNextChildNode(smiNode)) {
	if (smiNode->nodekind & (SMI_NODEKIND_COLUMN | SMI_NODEKIND_SCALAR)
	    && (smiNode->access == SMI_ACCESS_READ_ONLY
		|| smiNode->access == SMI_ACCESS_READ_WRITE)) {
	    cName = translateUpper(smiNode->name);
	    printf("    case %s:\n", cName);
	    switch (smiNode->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		printf("        objid[0] = 0;\n"
		       "        objid[1] = 0;\n"
		       "        *var_len = 2*sizeof(oid);\n"
		       "        return (unsigned char *) objid;\n");
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		printf("        *string = 0;\n"
		       "        *var_len = strlen(string);\n"
		       "        return (unsigned char *) string;\n");
		break;
	    case SMI_BASETYPE_ENUM:
	    case SMI_BASETYPE_INTEGER32:
	    case SMI_BASETYPE_UNSIGNED32:
		printf("        long_ret = 0;\n"
		       "        return (unsigned char *) &long_ret;\n");
		break;
	    default:
		printf("        /* add code to return the value here */\n");
	    }
	    printf("\n");
	    free(cName);
	}
    }

    printf("    default:\n"
	   "         ERROR_MSG(\"\");\n"
	   "    }\n\n");
    
    printf("    return NULL;\n"
	   "}\n\n");
}



static void printReadMethods(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
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
    
    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (smiNode->access == SMI_ACCESS_READ_WRITE) {
	    cnt++;
	    if (cnt == 1) {
		printf("/*\n"
		       " * Forward declaration of write methods for writable objects:\n"
		       " */\n\n");
	    }
	    printf("static int\nwrite_%s(int action,\n"
		   "        u_char   *var_val,\n"
		   "        u_char   var_val_type,\n"
		   "        int      var_val_len,\n"
		   "        u_char   *statP,\n"
		   "        oid      *name,\n"
		   "        int      name_len)\n"
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
	    cName = translate(smiNode->name);
	    switch (smiNode->basetype) {
	    case SMI_BASETYPE_OBJECTIDENTIFIER:
		printf("    oid       %s[%s];\n", cName, "MAX_OID_LEN");
		break;
	    case SMI_BASETYPE_OCTETSTRING:
	    case SMI_BASETYPE_BITS:
		printf("    u_char    %s[%s];\n", cName, "SPRINT_MAX_LEN");
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
	    free(cName);
	}
    }
    
    printf("} %s;\n\n", cGroupName);
    printf("extern int read_%s(%s *return);\n\n", cGroupName, cGroupName);
    free(cGroupName);
}



static void printTypedefs(SmiModule *smiModule)
{
    SmiNode   *smiNode;
    int       cnt = 0;
    
    for(smiNode = smiGetFirstNode(smiModule->name, SMI_NODEKIND_ANY);
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
}



int dumpUcdH(char *modulename)
{
    SmiModule    *smiModule;
    char	 *cModuleName;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    printf("/*\n");
    printf(" * This C header file has been generated by smidump "
	   VERSION ".\n");
    printf(" * It is intended to be used with the UCD/CMU SNMP agent.\n");
    printf(" *\n");
    printf(" * This header is derived from the %s module.\n", smiModule->name);
    printf(" *\n * $Id$\n");
    printf(" */\n\n");

    cModuleName = translate(smiModule->name);

    printf("#ifndef _%s_H_\n", cModuleName);
    printf("#define _%s_H_\n\n", cModuleName);

    printf("config_require(util_funcs)\n\n");

    printf("/* Initialization function: */\n\n");
    printf("void init_%s(void);\n\n", cModuleName);

    printTypedefs(smiModule);

    printf("#endif /* _%s_H_ */\n", cModuleName);
    free(cModuleName);
    
    return 0;
}



int dumpUcdC(char *modulename)
{
    SmiModule    *smiModule;
    char	 *cModuleName;

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    printf("/*\n");
    printf(" * This C file has been generated by smidump "
	   VERSION ".\n");
    printf(" * It is intended to be used with the UCD/CMU SNMP agent.\n");
    printf(" *\n");
    printf(" * This C file is derived from the %s module.\n", smiModule->name);
    printf(" *\n * $Id$\n");
    printf(" */\n\n");

    printf("#ifdef REGISTER_MIB\n");
    printf("#define UCD_SNMP\n");
    printf("#endif\n\n");

    printf("#ifdef CMU_LINUX_SNMP\n");
    printf("#include \"mib_module.h\"\n");
    printf("#endif\n\n");

    printf("#ifdef UCD_SNMP\n");
    printf("#include <config.h>\n");
    printf("#include \"mibincl.h\"\n");
    printf("#include \"util_funcs.h\"\n");
    printf("#include \"system.h\"\n");
    printf("#endif\n\n");
    
    printf("#include <stdio.h>\n");
    printf("#include <string.h>\n");
    printf("#include <malloc.h>\n");
    printf("\n");

    printReadMethodDecls(smiModule);
    printWriteMethodDecls(smiModule);
    printDefines(smiModule);
    printInit(smiModule);

    printReadMethods(smiModule);
    printWriteMethods(smiModule);

    cModuleName = translate(smiModule->name);

    free(cModuleName);

    return 0;
}
