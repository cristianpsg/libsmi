/*
 * smiquery.c --
 *
 *      A simple SMI query program, using libsmi.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smiquery.c,v 1.10 1999/05/20 17:01:46 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"

#define SMIQUERY_CONFIG_FILE "/usr/local/etc/smiquery.conf"



char * 
smiStringStatus(status)
    SmiStatus status;
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	(status == SMI_STATUS_OPTIONAL)    ? "optional" :
					     "<unknown>";
}

char *
smiStringAccess(access)
    SmiAccess access;
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

char *
smiStringDecl(macro)
    SmiDecl macro;
{
    return
        (macro == SMI_DECL_UNKNOWN)           ? "<UNKNOWN>" :
        (macro == SMI_DECL_TYPEASSIGNMENT)    ? "<TYPE-ASSIGNMENT>" :
        (macro == SMI_DECL_VALUEASSIGNMENT)   ? "<VALUE-ASSIGNMENT>" :
        (macro == SMI_DECL_OBJECTTYPE)        ? "OBJECT-TYPE" :
        (macro == SMI_DECL_OBJECTIDENTITY)    ? "OBJECT-IDENTITY" :
        (macro == SMI_DECL_MODULEIDENTITY)    ? "MODULE-IDENTITY" :
        (macro == SMI_DECL_NOTIFICATIONTYPE)  ? "NOTIFICATIONTYPE" :
        (macro == SMI_DECL_TRAPTYPE)          ? "TRAP-TYPE" :
        (macro == SMI_DECL_OBJECTGROUP)       ? "OBJECT-GROUP" :
        (macro == SMI_DECL_NOTIFICATIONGROUP) ? "NOTIFICATION-GROUP" :
        (macro == SMI_DECL_MODULECOMPLIANCE)  ? "MODULE-COMPLIANCE" :
        (macro == SMI_DECL_AGENTCAPABILITIES) ? "AGENT-CAPABILITIES" :
        (macro == SMI_DECL_TEXTUALCONVENTION) ? "TEXTUAL-CONVENTION" :
                                                "<unknown>";
}

char *
smiStringBasetype(basetype)
    SmiBasetype basetype;
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<UNKNOWN>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "OctetString" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectIdentifier" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "Unsigned32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "Integer32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "Unsigned64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "Integer64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "Float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "Float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "Float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "Enumeration" :
        (basetype == SMI_BASETYPE_BITS)              ? "Bits" :
        (basetype == SMI_BASETYPE_SEQUENCE)          ? "SEQUENCE" :
        (basetype == SMI_BASETYPE_SEQUENCEOF)        ? "SEQUENCE OF" :
        (basetype == SMI_BASETYPE_CHOICE)            ? "CHOICE" :
                                                   "<unknown>";
}



char *
format(const char *s)
{
    static char ss[20000];
    int i, j;

    if (!s) {
	sprintf(ss, "-");
    } else {
	for(i = 0, j = 0; s[i]; i++) {
	    ss[j++] = s[i];
	    if (s[i] == '\n') {
		sprintf(&ss[j], "              ");
		j += 14;
	    }
	}
	ss[j] = 0;
    }
    return ss;
}


char *
formattype(const char *module, const char *name)
{
    static char ss[200];

    if (!name) {
	strcpy(ss, "<unknown>");
    } else if (!module) {
	strcpy(ss, name);
    } else {
	sprintf(ss, "%s.%s", module, name);
    }
    return ss;
}


void
usage()
{
    fprintf(stderr,
	    "Usage: smiquery [-vrs] [-l level] [-p module] command name\n"
	    "known commands: module, node, type, macro, names, children, members, parent\n");
}



int
main(argc, argv)
    int argc;
    char *argv[];
{
    SmiModule *module;
    SmiNode *node;
    SmiType *type;
    SmiMacro *macro;
    SmiNamedNumber *nn;
    SmiRange *range;
    char *command, *name;
    int flags;
    char c;
    
    smiInit();

    flags = smiGetFlags();
    
    while ((c = getopt(argc, argv, "rsvl:p:")) != -1) {
	switch (c) {
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'v':
	    flags |= SMI_ERRORLINES;
	    smiSetFlags(flags);
	    break;
	case 'r':
	    flags |= SMI_RECURSIVE;
	    smiSetFlags(flags);
	    break;
	case 's':
	    flags |= SMI_STATS;
	    smiSetFlags(flags);
	    break;
	case 'p':
	    smiLoadModule(optarg);
	    break;
	default:
	    usage();
	    exit(1);
	}
    }

    if (optind+2 != argc) {
	usage();
	exit(0);
    }
	
    command = argv[optind];
    name = argv[optind+1];

    if (!strcmp(command, "module")) {
	module = smiGetModule(name);
	if (module) {
	    printf("      Module: %s\n", format(module->name));
	    printf("      Object: %s\n", format(module->object));
	    printf(" LastUpdated: %s", ctime(&module->lastupdated));
	    printf("Organization: %s\n", format(module->organization));
	    printf(" ContactInfo: %s\n", format(module->contactinfo));
	    printf(" Description: %s\n", format(module->description));
	    printf("   Reference: %s\n", format(module->reference));
	}
	smiFreeModule(module);
    }

    if (!strcmp(command, "node")) {
	node = smiGetNode(name, NULL);
	if (node) {
	    printf("     MibNode: %s\n", format(node->name));
	    printf("      Module: %s\n", format(node->module));
	    printf("         OID: %s\n", format(node->oid));
	    printf("        Type: %s\n",
		   formattype(node->typemodule, node->typename));
	    printf("      Syntax: %s\n", smiStringBasetype(node->basetype));
	    printf(" Declaration: %s\n", smiStringDecl(node->decl));
	    printf("      Access: %s\n", smiStringAccess(node->access));
	    printf("      Status: %s\n", smiStringStatus(node->status));
	    printf(" Description: %s\n", format(node->description));
	    printf("   Reference: %s\n", format(node->reference));
	}
	smiFreeNode(node);
    }

    if (!strcmp(command, "type")) {
	type = smiGetType(name, NULL);
	if (type) {
	    printf("        Type: %s\n", format(type->name));
	    printf("      Module: %s\n", format(type->module));
	    printf("      Syntax: %s\n", smiStringBasetype(type->basetype));
	    printf(" Parent Type: %s\n",
		   formattype(type->parentmodule, type->parentname));
	    printf("Restrictions:");
	    if ((type->basetype == SMI_BASETYPE_ENUM) ||
		(type->basetype == SMI_BASETYPE_BITS)) {
		for(nn = smiGetFirstNamedNumber(type->module, type->name);
		    nn ; nn = smiGetNextNamedNumber(nn)) {
		    printf(" %s(%ld)",
			   nn->name, nn->valuePtr->value.unsigned32);
		}
	    } else {
		for(range = smiGetFirstRange(type->module, type->name);
		    range ; range = smiGetNextRange(range)) {
		    printf(" %ld..%ld",
			   range->minValuePtr->value.unsigned32,
			   range->maxValuePtr->value.unsigned32);
		}
	    }
	    printf("\n");
	    printf(" Declaration: %s\n", smiStringDecl(type->decl));
	    printf(" DisplayHint: %s\n", format(type->format));
	    printf("      Status: %s\n", smiStringStatus(type->status));
	    printf(" Description: %s\n", format(type->description));
	}
	smiFreeType(type);
    }

    if (!strcmp(command, "macro")) {
	macro = smiGetMacro(name, NULL);
	if (macro) {
	    printf("       Macro: %s\n", format(macro->name));
	    printf("      Module: %s\n", format(macro->module));
	}
	smiFreeMacro(macro);
    }

#if 0
    if (!strcmp(command, "names")) {
	list = smiGetNames(name, NULL);
	if (list) {
	    printf("       Names:");
	    for (p = list; *p; p++) {
		printf(" %s", *p);
	    }
	    printf("\n");
	}
    }
    
    if (!strcmp(command, "children")) {
	list = smiGetChildren(name, NULL);
	if (list) {
	    printf("    Children:");
	    for (p = list; *p; p++) {
		printf(" %s", *p);
	    }
	    printf("\n");
	}
    }

    if (!strcmp(command, "parent")) {
	fullname = smiGetParent(name, NULL);
	if (fullname) {
	    printf("      Parent: %s\n", fullname);
	}
    }
    
    if (!strcmp(command, "imports")) {
	list = smiGetImports(name);
	if (list) {
	    printf("     Imports:");
	    for (p = list; *p; p++) {
		printf(" %s", *p);
	    }
	    printf("\n");
	}
    }
#endif
    
    exit(0);
}
