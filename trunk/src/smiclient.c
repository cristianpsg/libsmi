/*
 * smiclient.c --
 *
 *      A simple SMI RPC client for testing.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smiclient.c,v 1.7 1998/11/26 00:50:09 strauss Exp $
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
    smi_getspec getspec;
    smi_fullname fullname;
    smi_fullname *fullname2;
    smi_module *module;
    smi_node *node;
    smi_type *type;
    smi_macro *macro;
    smi_namelist *namelist;
    CLIENT *cl;
    
    if (argc != 4) {
	fprintf(stderr, "smiclient $Revision$\n");
	fprintf(stderr, "Usage: smiclient <server> <command> <name>\n");
	fprintf(stderr, "known commands: module, node, type, macro, names, children, members, parent\n");
	exit(1);
    }

    /*
     * We must use TCP, since some messages may exceed the UDP limitation
     * of messages larger than UDPMSGSIZE==8800 (at least on Linux).
     */
    if (!(cl = clnt_create(argv[1], SMIPROG, SMIVERS, "tcp"))) {
	clnt_pcreateerror(argv[1]);
	exit(1);
    }

    if (!strcmp(argv[2], "module")) {
	getspec.name = argv[3];
	getspec.wantdescr = 1;
	module = smiproc_module_1(&getspec, cl);
	if (module) {
	    printf("      Module: %s\n", module->name);
	    printf("         OID: %s\n", module->oid);
	    printf(" LastUpdated: %s\n", module->lastupdated);
	    printf("Organization: %s\n", module->organization);
	    printf(" ContactInfo: %s\n", module->contactinfo);
	    printf(" Description: %s\n", module->description);
	} else {
	    clnt_perror(cl, "smiproc_module_1");
	}
    }

    if (!strcmp(argv[2], "node")) {
	getspec.name = argv[3];
	getspec.wantdescr = 1;
	node = smiproc_node_1(&getspec, cl);
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
	} else {
	    clnt_perror(cl, "smiproc_node_1");
	}
    }

    if (!strcmp(argv[2], "type")) {
	getspec.name = argv[3];
	getspec.wantdescr = 1;
	type = smiproc_type_1(&getspec, cl);
	if (type) {
	    printf("        Type: %s\n", type->name);
	    printf("      Module: %s\n", type->module);
	    printf("      Syntax: %s\n", smiStringSyntax(type->syntax));
	    printf(" Declaration: %s\n", smiStringDecl(type->decl));
	    printf(" DisplayHint: %s\n", type->display);
	    printf("      Status: %s\n", smiStringStatus(type->status));
	    printf(" Description: %s\n", type->description);
	} else {
	    clnt_perror(cl, "smiproc_type_1");
	}
    }

    if (!strcmp(argv[2], "macro")) {
	fullname = argv[3];
	macro = smiproc_macro_1(&fullname, cl);
	if (macro) {
	    printf("       Macro: %s\n", macro->name);
	    printf("      Module: %s\n", macro->module);
	} else {
	    clnt_perror(cl, "smiproc_macro_1");
	}
    }
    
    if (!strcmp(argv[2], "names")) {
	fullname = argv[3];
	namelist = smiproc_names_1(&fullname, cl);
	if (namelist) {
	    printf("       Names: %s\n", namelist->namelist);
	} else {
	    clnt_perror(cl, "smiproc_names_1");
	}
    }
    
    if (!strcmp(argv[2], "children")) {
	fullname = argv[3];
	namelist = smiproc_children_1(&fullname, cl);
	if (namelist) {
	    printf("    Children: %s\n", namelist->namelist);
	} else {
	    clnt_perror(cl, "smiproc_children_1");
	}
    }
    
    if (!strcmp(argv[2], "members")) {
	fullname = argv[3];
	namelist = smiproc_members_1(&fullname, cl);
	if (namelist) {
	    printf("     Members: %s\n", namelist->namelist);
	} else {
	    clnt_perror(cl, "smiproc_members_1");
	}
    }
    
    if (!strcmp(argv[2], "parent")) {
	fullname = argv[3];
	fullname2 = smiproc_parent_1(&fullname, cl);
	if (fullname2) {
	    printf("      Parent: %s\n", *fullname2);
	} else {
	    clnt_perror(cl, "smiproc_parent_1");
	}
    }
    
    exit(0);
}
