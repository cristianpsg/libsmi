/*
 * dump-jax.c --
 *
 *      Operations to generate UCD SNMP mib module implementation code.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-jax.c,v 1.3 2000/03/13 13:25:54 strauss Exp $
 */

#include <config.h>

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



static struct {
    SmiBasetype basetype;
    char        *smitype;
    char        *javatype;
    char        *agentxtype;
} convertType[] = {
    { SMI_BASETYPE_OCTETSTRING,      "Opaque",    "byte[]",    "OPAQUE" },
    { SMI_BASETYPE_OCTETSTRING,      "TimeTicks", "long",      "TIMETICKS" },
    { SMI_BASETYPE_OCTETSTRING,      "Counter",   "long",      "COUNTER32" },
    { SMI_BASETYPE_OCTETSTRING,      "Counter32", "long",      "COUNTER32" },
    { SMI_BASETYPE_OCTETSTRING,      "IpAddress", "byte[]",    "IPADDRESS" },
    { SMI_BASETYPE_INTEGER32,        NULL,	  "int",       "INTEGER" },
    { SMI_BASETYPE_OCTETSTRING,      NULL,	  "byte[]",    "OCTETSTRING" },
    { SMI_BASETYPE_OBJECTIDENTIFIER, NULL,	  "AgentXOID", "OBJECTIDENTIFIER" },
    { SMI_BASETYPE_UNSIGNED32,       NULL,	  "long",      "GAUGE32" },
    { SMI_BASETYPE_INTEGER64,        NULL,	  "long",      "INTEGER" },
    { SMI_BASETYPE_UNSIGNED64,       NULL,	  "long",      "COUNTER64" },
    { SMI_BASETYPE_ENUM,             NULL,	  "int",       "INTEGER" },
    { SMI_BASETYPE_BITS,             NULL,	  "byte[]",    "OCTETSTRING" },
    { SMI_BASETYPE_UNKNOWN,          NULL,	  NULL,        NULL }
};



static char* translate(char *m)
{
    static char *s = NULL;
    int i;

    if (s) xfree(s);
    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
    }
  
    return s;
}



static char* translateUpper(char *m)
{
    static char *s = NULL;
    int i;

    if (s) xfree(s);
    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
	if (islower((int) s[i])) {
	    s[i] = toupper(s[i]);
	}
    }
  
    return s;
}



static char* translate1Upper(char *m)
{
    static char *s = NULL;
    int i;

    if (s) xfree(s);
    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
    }
    if (islower((int) s[0])) {
	s[0] = toupper(s[0]);
    }
    
    return s;
}



static char* translateLower(char *m)
{
    static char *s = NULL;
    int i;

    if (s) xfree(s);
    s = xstrdup(m);
    for (i = 0; s[i]; i++) {
	if (s[i] == '-') s[i] = '_';
	if (isupper((int) s[i])) {
	    s[i] = tolower(s[i]);
	}
    }
  
    return s;
}



static char *getJavaType(SmiType *smiType)
{
    int i;
    
    for(i=0; convertType[i].basetype != SMI_BASETYPE_UNKNOWN; i++) {
	if (smiType->basetype == convertType[i].basetype)
	    return convertType[i].javatype;
    }

    return "<UNKNOWN>";
}



static char *getAgentXType(SmiType *smiType)
{
    int i;
    SmiType *parentType;
    SmiModule *smiModule;
    
    parentType = smiGetParentType(smiType);
    if (parentType) {
	smiModule = smiGetTypeModule(parentType);
	if (smiModule && strlen(smiModule->name)) {
	    smiType = parentType;
	}
    }
    
    for(i=0; convertType[i].basetype != SMI_BASETYPE_UNKNOWN; i++) {
	if (smiType->basetype == convertType[i].basetype) {
	    if (!convertType[i].smitype)
		return convertType[i].agentxtype;
	    if ((smiType->name) &&
		(!strcmp(convertType[i].smitype, smiType->name)))
		return convertType[i].agentxtype;
	}
	
    }

    return "<UNKNOWN>";
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
    }

    return size;
}



static void dumpTable(SmiNode *smiNode)
{
    FILE *f;
    char s[100];
    SmiNode *parentNode, *columnNode;
    int i;
    
    parentNode = smiGetParentNode(smiNode);
    
    sprintf(s, "%s.java", translate1Upper(parentNode->name));
    f = fopen(s, "w");
    if (!f) {
	fprintf(stderr, "smidump: cannot open %s for writing: ", s);
	perror(NULL);
	exit(1);
    }

    fprintf(f,
	    "/*\n"
	    " * This Java file has been generated by smidump " VERSION "."
                " Do not edit!\n"
	    " * It is intended to be used within a Java AgentX sub-agent"
	        " environment.\n"
	    " *\n"
	    " * $I" "d$\n"
	    " */\n\n");

    fprintf(f,
	    "/**\n"
	    "    This class represents a Java AgentX (JAX) implementation of\n"
	    "    the table %s defined in %s.\n"
	    "\n"
	    "    @version 1\n"
	    "    @author  smidump\n"
	    "    @see     AgentXTable\n"
	    " */\n\n", parentNode->name, smiGetNodeModule(smiNode)->name);
    
    fprintf(f,
	    "import java.util.Vector;\n"
	    "\n"
	    "import jax.AgentXOID;\n"
	    "import jax.AgentXVarBind;\n"
	    "import jax.AgentXTable;\n"
	    "import jax.AgentXEntry;\n"
	    "\n");

    fprintf(f, "public class %s extends AgentXTable\n{\n\n",
	    translate1Upper(parentNode->name));

    fprintf(f,
	    "    // entry OID\n"
	    "    private final static long[] OID = {");
    for (i = 0; i < smiNode->oidlen; i++) {
	fprintf(f, "%s%d", i ? ", " : "", smiNode->oid[i]);
    }
    fprintf(f, "};\n\n");

    fprintf(f,
	    "    // constructor\n"
	    "    public %s()\n", translate1Upper(parentNode->name));
    fprintf(f,
	    "    {\n"
	    "        oid = new AgentXOID(OID);\n"
	    "\n"
	    "        // register implemented columns\n");
    for (columnNode = smiGetFirstChildNode(smiNode);
	 columnNode;
	 columnNode = smiGetNextChildNode(columnNode)) {
	if (columnNode->access >= SMI_ACCESS_READ_ONLY) {
	    fprintf(f,
		    "        columns.addElement(new Long(%ld));\n",
		    columnNode->oid[columnNode->oidlen-1]);
	}
    }
    fprintf(f,
	    "    }\n\n");
    
    fprintf(f,
	    "    public AgentXVarBind getVarBind(AgentXEntry entry,"
	         " long column)\n");
    fprintf(f,
	    "    {\n"
	    "        AgentXOID oid = new AgentXOID(getOID(), column, entry.getInstance());\n"
	    "\n"
	    "        switch ((int)column) {\n");

    for (columnNode = smiGetFirstChildNode(smiNode);
	 columnNode;
	 columnNode = smiGetNextChildNode(columnNode)) {
	if (columnNode->access >= SMI_ACCESS_NOTIFY) {
	    fprintf(f,
		    "        case %ld: // %s\n",
		    columnNode->oid[columnNode->oidlen-1],
		    columnNode->name);
	    fprintf(f,
		    "        {\n");
	    fprintf(f,
		    "            %s value = ((%s)entry).get_%s();\n",
		    getJavaType(smiGetNodeType(columnNode)),
		    translate1Upper(smiNode->name),
		    columnNode->name);
	    fprintf(f,
		    "            return new AgentXVarBind(oid, "
		    "AgentXVarBind.%s, value);\n",
		    getAgentXType(smiGetNodeType(columnNode)));
	    
	    fprintf(f,
		    "        }\n");
	}
    }

    fprintf(f,
	    "        }\n"
	    "\n"
	    "        return null;\n"
	    "    }\n\n");

    fprintf(f,
	    "}\n\n");

    fclose(f);
}



static void dumpEntry(SmiNode *smiNode)
{
    FILE *f;
    char s[100];
    SmiNode *columnNode, *indexNode;
    SmiType *smiType;
    SmiRange *smiRange;
    SmiElement *element;
    int i, cnt;
    char *p;
    char init[20];
    
    sprintf(s, "%s.java", translate1Upper(smiNode->name));
    f = fopen(s, "w");
    if (!f) {
	fprintf(stderr, "smidump: cannot open %s for writing: ", s);
	perror(NULL);
	exit(1);
    }

    fprintf(f,
	    "/*\n"
	    " * This Java file has been generated by smidump " VERSION "."
                " Do not edit!\n"
	    " * It is intended to be used within a Java AgentX sub-agent"
	        " environment.\n"
	    " *\n"
	    " * $I" "d$\n"
	    " */\n\n");

    fprintf(f,
	    "/**\n"
	    "    This class represents a Java AgentX (JAX) implementation of\n"
	    "    the table row %s defined in %s.\n"
	    "\n"
	    "    @version 1\n"
	    "    @author  smidump\n"
	    "    @see     AgentXTable, AgentXEntry\n"
	    " */\n\n", smiNode->name, smiGetNodeModule(smiNode)->name);
    
    fprintf(f,
	    "import jax.AgentXOID;\n"
	    "import jax.AgentXEntry;\n"
	    "\n");

    fprintf(f, "public class %s extends AgentXEntry\n{\n\n",
	    translate1Upper(smiNode->name));

    for (columnNode = smiGetFirstChildNode(smiNode);
	 columnNode;
	 columnNode = smiGetNextChildNode(columnNode)) {
	
	smiType = smiGetNodeType(columnNode);
	p = getJavaType(smiType);
	if (!strcmp(p, "long")) {
	    strcpy(init, "0");
	} else if (!strcmp(p, "int")) {
	    strcpy(init, "0");
	} else if (!strcmp(p, "byte[]")) {
	    smiRange = smiGetFirstRange(smiType);
	    if ((smiRange && (!smiGetNextRange(smiRange)) &&
		 (!memcmp(&smiRange->minValue, &smiRange->maxValue,
			  sizeof(SmiValue))))) {
		sprintf(init, "new byte[%d]",
			smiRange->maxValue.value.integer32);
	    } else {
		sprintf(init, "new byte[0]");
	    }
	} else if (!strcmp(p, "AgentXOID")) {
	    strcpy(init, "new AgentXOID()");
	} else {
	    strcpy(init, "null");
	}
	fprintf(f,
		"    protected %s %s = %s;\n",
		getJavaType(smiGetNodeType(columnNode)),
		columnNode->name,
		init);
    }
    for (element = smiGetFirstElement(smiNode), cnt = 0;
	 element;
	 element = smiGetNextElement(element)) {
	indexNode = smiGetElementNode(element);
	for (columnNode = smiGetFirstChildNode(smiNode);
	     columnNode;
	     columnNode = smiGetNextChildNode(columnNode)) {
	    if (!strcmp(columnNode->name, indexNode->name))
		break;
	}
	if (!columnNode) {
	    if (!cnt) {
		fprintf(f, "    // foreign indices\n");
	    }
	    cnt++;
	    fprintf(f, "    protected %s %s;\n",
		    getJavaType(smiGetNodeType(indexNode)),
		    indexNode->name);
	}
    }
    fprintf(f, "\n");
    
    fprintf(f,
	    "    // constructor\n"
	    "    public %s(", translate1Upper(smiNode->name));
    for (element = smiGetFirstElement(smiNode), cnt = 0;
	 element;
	 element = smiGetNextElement(element)) {
	if (cnt) {
	    fprintf(f, ",\n%*s", 4 + 7 + 1 + strlen(smiNode->name), " ");
	}
	cnt++;
	indexNode = smiGetElementNode(element);
	fprintf(f, "%s %s",
		getJavaType(smiGetNodeType(indexNode)),
		indexNode->name);
    }
    fprintf(f, ")\n"
	    "    {\n");
    for (element = smiGetFirstElement(smiNode);
	 element;
	 element = smiGetNextElement(element)) {
	indexNode = smiGetElementNode(element);
	fprintf(f, "        this.%s = %s;\n",
		indexNode->name, indexNode->name);
    }
    fprintf(f, "\n");
    for (element = smiGetFirstElement(smiNode);
	 element;
	 element = smiGetNextElement(element)) {
	indexNode = smiGetElementNode(element);

	p = getJavaType(smiGetNodeType(indexNode));
	if (!strcmp(p, "long")) {
	    fprintf(f, "        instance.append(%s);\n",
		    indexNode->name);
	} else if (!strcmp(p, "int")) {
	    fprintf(f, "        instance.append(%s);\n",
		    indexNode->name);
	} else if (!strcmp(p, "byte[]")) {
	    smiType = smiGetNodeType(indexNode);
	    smiRange = smiGetFirstRange(smiType);
	    if ((smiRange && (!smiGetNextRange(smiRange)) &&
		 (!memcmp(&smiRange->minValue, &smiRange->maxValue,
			  sizeof(SmiValue)))) ||
		(smiNode->implied && (!smiGetNextElement(element)))) {
		fprintf(f, "        instance.appendImplied(%s);\n",
			indexNode->name);
	    } else {
		fprintf(f, "        instance.append(%s);\n",
			indexNode->name);
	    }
	} else if (!strcmp(p, "AgentXOID")) {
	    if (smiNode->implied && (!smiGetNextElement(element))) {
		fprintf(f, "        instance.appendImplied(%s);\n",
			indexNode->name);
	    } else {
		fprintf(f, "        instance.append(%s);\n",
			indexNode->name);
	    }
	} else {
	    fprintf(f, "        XXX // [smidump: type of %s not supported]\n",
		    indexNode->name);
	}
    }
    
    fprintf(f,
	    "    }\n"
	    "\n");

    for (columnNode = smiGetFirstChildNode(smiNode);
	 columnNode;
	 columnNode = smiGetNextChildNode(columnNode)) {
	smiType = smiGetNodeType(columnNode);
	if (columnNode->access >= SMI_ACCESS_NOTIFY) {
	    fprintf(f,
		    "    public %s get_%s()\n"
		    "    {\n"
		    "        return %s;\n"
		    "    }\n"
		    "\n",
		    getJavaType(smiType),
		    columnNode->name, columnNode->name);
	}
    }

    fprintf(f,
	    "}\n\n");

    fclose(f);
}



int dumpJax(char *modulename, int flags)
{
    SmiModule    *smiModule;
    SmiNode      *smiNode;
    
    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for Jax format\n");
	exit(1);
    }
    
    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ROW);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ROW)) {
	if (isGroup(smiNode) && isAccessible(smiNode)) {
	    dumpTable(smiNode);
	    dumpEntry(smiNode);
	}
    }
    
    
    return 0;
}
