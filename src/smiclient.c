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
 * @(#) $Id: smiclient.c,v 1.3 1998/11/23 12:57:00 strauss Exp $
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
    smi_module *module;
    smi_node *node;
    smi_type *type;
    smi_macro *macro;
    smi_namelist *namelist;
    CLIENT *cl;
    
    if (argc != 4) {
	fprintf(stderr, "Usage: smiclient <server> module <name>\n");
	fprintf(stderr, "                          node <name>\n");
	fprintf(stderr, "                          type <name>\n");
	fprintf(stderr, "                          macro <name>\n");
	fprintf(stderr, "                          names <name>\n");
	exit(1);
    }

    if (!(cl = clnt_create(argv[1], SMIPROG, SMIVERS, "udp"))) {
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
    
    exit(0);
}
