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
 * @(#) $Id$
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"



int
main(argc, argv)
    int argc;
    char *argv[];
{
    smi_fullname *fullname2;
    smi_module *module;
    smi_node *node;
    smi_type *type;
    smi_macro *macro;
    smi_namelist *namelist;
    
    if (argc != 3) {
	fprintf(stderr, "smiquery $Revision$\n");
	fprintf(stderr, "Usage: smiquery <command> <name>\n");
	fprintf(stderr, "known commands: module, node, type, macro, names, children, members, parent\n");
	exit(1);
    }

    smiInit();
    smiReadConfig("/usr/local/etc/smiquery.conf");

    if (!strcmp(argv[1], "module")) {
	module = smiGetModule(argv[2], 1);
	if (module) {
	    printf("      Module: %s\n", module->name);
	    printf("         OID: %s\n", module->oid);
	    printf(" LastUpdated: %s\n", module->lastupdated);
	    printf("Organization: %s\n", module->organization);
	    printf(" ContactInfo: %s\n", module->contactinfo);
	    printf(" Description: %s\n", module->description);
	}
    }

    if (!strcmp(argv[1], "node")) {
	node = smiGetNode(argv[2], "", 1);
	if (node) {
	    printf("     MibNode: %s\n", node->name);
	    printf("      Module: %s\n", node->module);
	    printf("         OID: %s\n", node->oid);
	    printf("        Type: %s\n", node->type);
	    printf("      Syntax: %s\n", smiStringSyntax(node->syntax));
	    printf(" Declaration: %s\n", smiStringDecl(node->decl));
	    printf("      Access: %s\n", smiStringAccess(node->access));
	    printf("      Status: %s\n", smiStringStatus(node->status));
	    printf(" Description: %s\n", node->description);
	}
    }

    if (!strcmp(argv[1], "type")) {
	type = smiGetType(argv[2], "", 1);
	if (type) {
	    printf("        Type: %s\n", type->name);
	    printf("      Module: %s\n", type->module);
	    printf("      Syntax: %s\n", smiStringSyntax(type->syntax));
	    printf(" Declaration: %s\n", smiStringDecl(type->decl));
	    printf(" DisplayHint: %s\n", type->display);
	    printf("      Status: %s\n", smiStringStatus(type->status));
	    printf(" Description: %s\n", type->description);
	}
    }

    if (!strcmp(argv[1], "macro")) {
	macro = smiGetMacro(argv[2], "");
	if (macro) {
	    printf("       Macro: %s\n", macro->name);
	    printf("      Module: %s\n", macro->module);
	}
    }
    
    if (!strcmp(argv[1], "names")) {
	namelist = smiGetNames(argv[2], "");
	if (namelist) {
	    printf("       Names: %s\n", namelist->namelist);
	}
    }
    
    if (!strcmp(argv[1], "children")) {
	namelist = smiGetChildren(argv[2], "");
	if (namelist) {
	    printf("    Children: %s\n", namelist->namelist);
	}
    }
    
    if (!strcmp(argv[1], "members")) {
	namelist = smiGetMembers(argv[2], "");
	if (namelist) {
	    printf("     Members: %s\n", namelist->namelist);
	}
    }
    
    if (!strcmp(argv[1], "parent")) {
	fullname2 = smiGetParent(argv[2], "");
	if (fullname2) {
	    printf("      Parent: %s\n", *fullname2);
	}
    }
    
    exit(0);
}
