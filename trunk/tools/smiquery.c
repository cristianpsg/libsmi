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
 * @(#) $Id: smiquery.c,v 1.2 1999/03/16 17:24:14 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"

#define SMIQUERY_CONFIG_FILE "/usr/local/etc/smiquery.conf"



char *
format(const char *s)
{
    static char ss[20000];
    int i, j;

    for(i = 0, j = 0; s[i]; i++) {
	ss[j++] = s[i];
	if (s[i] == '\n') {
	    sprintf(&ss[j], "              ");
	    j += 14;
	}
    }
    ss[j] = 0;
    return ss;
}


void
usage()
{
    fprintf(stderr,
	    "Usage: smiquery [-vVrRsS] [-d level] [-l level] [-c configfile]\n"
	    "       [-L location] [-p module] command name\n"
	    "known commands: module, node, type, macro, names, children, members, parent\n");
}



int
main(argc, argv)
    int argc;
    char *argv[];
{
    smi_fullname fullname;
    smi_module *module;
    smi_node *node;
    smi_type *type;
    smi_macro *macro;
    smi_namelist *namelist;
    char *command, *name;
    int flags;
    char c;
	
    smiInit();

#ifdef SMIQUERY_CONFIG_FILE
    smiReadConfig(SMIQUERY_CONFIG_FILE);
#endif
        
    flags = smiGetFlags();
    
    while ((c = getopt(argc, argv, "rRsSvVd:l:c:L:p:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg);
	    break;
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	case 'd':
	    smiSetDebugLevel(atoi(optarg));
	    break;
	case 'v':
	    flags |= SMI_ERRORLINES;
	    smiSetFlags(flags);
	    break;
	case 'V':
	    flags &= ~SMI_ERRORLINES;
	    smiSetFlags(flags);
	    break;
	case 'r':
	    /* errors and statistics (if -s present) for imported modules */
	    flags |= SMI_RECURSIVE;
	    smiSetFlags(flags);
	    break;
	case 'R':
	    flags &= ~SMI_RECURSIVE;
	    smiSetFlags(flags);
	    break;
	case 's':
	    /* print some module statistics */
	    flags |= SMI_STATS;
	    smiSetFlags(flags);
	    break;
	case 'S':
	    flags &= ~SMI_STATS;
	    break;
	case 'L':
	    smiAddLocation(optarg);
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
	    printf("      Module: %s\n", module->name);
	    printf("      Object: %s\n", module->object);
	    printf(" LastUpdated: %s", ctime(&module->lastupdated));
	    printf("Organization: %s\n", format(module->organization));
	    printf(" ContactInfo: %s\n", format(module->contactinfo));
	    printf(" Description: %s\n", format(module->description));
	    printf("   Reference: %s\n", format(module->reference));
	}
    }

    if (!strcmp(command, "node")) {
	node = smiGetNode(name, "");
	if (node) {
	    printf("     MibNode: %s\n", node->name);
	    printf("      Module: %s\n", node->module);
	    printf("         OID: %s\n", node->oid);
	    printf("        Type: %s\n", node->type);
	    printf("      Syntax: %s\n", smiStringSyntax(node->syntax));
	    printf(" Declaration: %s\n", smiStringDecl(node->decl));
	    printf("      Access: %s\n", smiStringAccess(node->access));
	    printf("      Status: %s\n", smiStringStatus(node->status));
	    printf(" Description: %s\n", format(node->description));
	    printf("   Reference: %s\n", format(node->reference));
	}
    }

    if (!strcmp(command, "type")) {
	type = smiGetType(name, "");
	if (type) {
	    printf("        Type: %s\n", type->name);
	    printf("      Module: %s\n", type->module);
	    printf("      Syntax: %s\n", smiStringSyntax(type->syntax));
	    printf(" Declaration: %s\n", smiStringDecl(type->decl));
	    printf(" DisplayHint: %s\n", type->format);
	    printf("      Status: %s\n", smiStringStatus(type->status));
	    printf(" Description: %s\n", type->description);
	}
    }

    if (!strcmp(command, "macro")) {
	macro = smiGetMacro(name, "");
	if (macro) {
	    printf("       Macro: %s\n", macro->name);
	    printf("      Module: %s\n", macro->module);
	}
    }
    
    if (!strcmp(command, "names")) {
	namelist = smiGetNames(name, "");
	if (namelist) {
	    printf("       Names: %s\n", namelist->namelist);
	}
    }
    
    if (!strcmp(command, "children")) {
	namelist = smiGetChildren(name, "");
	if (namelist) {
	    printf("    Children: %s\n", namelist->namelist);
	}
    }
    
    if (!strcmp(command, "members")) {
	namelist = smiGetMembers(name, "");
	if (namelist) {
	    printf("     Members: %s\n", namelist->namelist);
	}
    }
    
    if (!strcmp(command, "parent")) {
	fullname = smiGetParent(name, "");
	if (fullname) {
	    printf("      Parent: %s\n", fullname);
	}
    }
    
    exit(0);
}
