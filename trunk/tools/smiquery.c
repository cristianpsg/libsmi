/*
 * smiquery.c --
 *
 *      A simple SMI query program, using libsmi.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smiquery.c,v 1.20 1999/06/17 16:57:09 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "smi.h"



char *smiStringStatus(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	(status == SMI_STATUS_OPTIONAL)    ? "optional" :
					     "<unknown>";
}

char *smiStringAccess(SmiAccess access)
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

char *smiStringLanguage(SmiLanguage language)
{
    return
	(language == SMI_LANGUAGE_UNKNOWN)    ? "<UNKNOWN>" :
	(language == SMI_LANGUAGE_SMIV1)      ? "SMIv1" :
	(language == SMI_LANGUAGE_SMIV2)      ? "SMIv2" :
	(language == SMI_LANGUAGE_SMING)      ? "SMIng" :
						"<unknown>";
}

char *smiStringDecl(SmiDecl macro)
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
        (macro == SMI_DECL_MODULE)	      ? "module" :
        (macro == SMI_DECL_TYPEDEF)	      ? "typedef" :
        (macro == SMI_DECL_NODE)	      ? "node" :
        (macro == SMI_DECL_SCALAR)	      ? "scalar" :
        (macro == SMI_DECL_TABLE)	      ? "table" :
        (macro == SMI_DECL_ROW)		      ? "row" :
        (macro == SMI_DECL_COLUMN)	      ? "column" :
        (macro == SMI_DECL_NOTIFICATION)      ? "notification" :
        (macro == SMI_DECL_GROUP)	      ? "group" :
        (macro == SMI_DECL_COMPLIANCE)	      ? "compliance" :
                                                "<unknown>";
}

char *smiStringNodekind(SmiNodekind nodekind)
{
    return
        (nodekind == SMI_NODEKIND_UNKNOWN)      ? "<UNKNOWN>" :
        (nodekind == SMI_NODEKIND_MODULE)       ? "module" :
        (nodekind == SMI_NODEKIND_NODE)         ? "node" :
        (nodekind == SMI_NODEKIND_SCALAR)       ? "scalar" :
        (nodekind == SMI_NODEKIND_TABLE)        ? "table" :
        (nodekind == SMI_NODEKIND_ROW)          ? "row" :
        (nodekind == SMI_NODEKIND_COLUMN)       ? "column" :
        (nodekind == SMI_NODEKIND_NOTIFICATION) ? "notification" :
        (nodekind == SMI_NODEKIND_GROUP)        ? "group" :
        (nodekind == SMI_NODEKIND_COMPLIANCE)   ? "compliance" :
                                                  "<unknown>";
}

char *smiStringBasetype(SmiBasetype basetype)
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
                                                   "<unknown>";
}



char *format(const char *s)
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


char *formatoid(unsigned int oidlen, SmiSubid *oid)
{
    static char ss[20000];
    int         i;
    
    ss[0] = 0;
    for (i=0; i < oidlen; i++) {
	if (i) strcat(ss, ".");
	sprintf(&ss[strlen(ss)], "%u", oid[i]);
    }
    return ss;
}


char *formattype(const char *module, const char *name)
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


void usage()
{
    fprintf(stderr,
	    "Usage: smiquery [-Vh] [-p <module>] <command> <name>\n"
	    "-V                        show version and license information\n"
	    "-h                        show usage information\n"
	    "-p <module>               preload <module>\n"
	    "module <module>           show information on module <module>\n"
	    "imports <module>          show import list of module <module>\n"
	    "revisions <module>        show revision list of module <module>\n"
	    "node <module.name>        show information on node <module.name>\n"
	    "parent <module.name>      show parent of node <module.name>\n"
	    "compliance <module.name>  show information on compliance node <module.name>\n"
	    "index <module.name>       show index information on table row <module.name>\n"
	    "members <module.name>     show member list of group node <module.name>\n"
	    "children <module.name>    show children list of node <module.name>\n"
	    "type <module.name>        show information on type <module.name>\n"
	    "macro <module.name>       show information on macro <module.name>\n");
}



void version()
{
    printf("smiquery " VERSION "\n");
}



int main(int argc, char *argv[])
{
    SmiModule *module;
    SmiNode *node, *child;
    SmiType *type;
    SmiMacro *macro;
    SmiNamedNumber *nn;
    SmiRange *range;
    SmiImport *import;
    SmiRevision *revision;
    SmiOption *option;
    SmiRefinement *refinement;
    SmiIndex *index;
    char *command, *name;
    int flags;
    char c;
    
    smiInit();

    flags = smiGetFlags();
    
    while ((c = getopt(argc, argv, "Vhp:")) != -1) {
	switch (c) {
	case 'p':
	    smiLoadModule(optarg);
	    break;
	case 'V':
	    version();
	    exit(0);
	case 'h':
	    usage();
	    exit(0);
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
	    printf("    Language: %s\n", smiStringLanguage(module->language));
	}
	smiFreeModule(module);
    }

    if (!strcmp(command, "imports")) {
	printf("     Imports:");
	for(import = smiGetFirstImport(name);
	    import ; import = smiGetNextImport(import)) {
	    printf(" %s.%s", import->importmodule, import->importname);
	}
	printf("\n");
    }

    if (!strcmp(command, "revisions")) {
	for(revision = smiGetFirstRevision(name);
	    revision ; revision = smiGetNextRevision(revision)) {
	    printf("    Revision: %s", ctime(&revision->date));
	    printf(" Description: %s\n", format(revision->description));
	}
    }

    if (!strcmp(command, "node")) {
	node = smiGetNode(name, NULL);
	if (node) {
	    printf("     MibNode: %s\n", format(node->name));
	    printf("      Module: %s\n", format(node->module));
	    printf("         OID: %s\n", formatoid(node->oidlen, node->oid));
	    printf("        Type: %s\n",
		   formattype(node->typemodule, node->typename));
	    printf("    Basetype: %s\n", smiStringBasetype(node->basetype));
	    printf(" Declaration: %s\n", smiStringDecl(node->decl));
	    printf("    NodeKind: %s\n", smiStringNodekind(node->nodekind));
	    printf("      Access: %s\n", smiStringAccess(node->access));
	    printf("      Status: %s\n", smiStringStatus(node->status));
	    printf(" Description: %s\n", format(node->description));
	    printf("   Reference: %s\n", format(node->reference));
	}
	smiFreeNode(node);
    }

    if (!strcmp(command, "parent")) {
	child = smiGetNode(name, NULL);
	if (child) node = smiGetParentNode(child);
	if (child && node) {
	    printf("     MibNode: %s\n", format(node->name));
	    printf("      Module: %s\n", format(node->module));
	    printf("         OID: %s\n", formatoid(node->oidlen, node->oid));
	    printf("        Type: %s\n",
		   formattype(node->typemodule, node->typename));
	    printf("    Basetype: %s\n", smiStringBasetype(node->basetype));
	    printf(" Declaration: %s\n", smiStringDecl(node->decl));
	    printf("    NodeKind: %s\n", smiStringNodekind(node->nodekind));
	    printf("      Access: %s\n", smiStringAccess(node->access));
	    printf("      Status: %s\n", smiStringStatus(node->status));
	    printf(" Description: %s\n", format(node->description));
	    printf("   Reference: %s\n", format(node->reference));
	}
	smiFreeNode(child);
	smiFreeNode(node);
    }

    if (!strcmp(command, "compliance")) {
	node = smiGetNode(name, NULL);
	printf("   Mandatory:");
	for(child = smiGetFirstMandatoryNode(node);
	    child ; child = smiGetNextMandatoryNode(node, child)) {
	    printf(" %s.%s", child->module, child->name);
	}
	printf("\n\n");
	for(option = smiGetFirstOption(node);
	    option ; option = smiGetNextOption(option)) {
	    printf("      Option: %s.%s\n", option->module, option->name);
	    printf(" Description: %s\n\n", format(option->description));
	}
	printf("\n");
	for(refinement = smiGetFirstRefinement(node);
	    refinement ; refinement = smiGetNextRefinement(refinement)) {
	    printf("  Refinement: %s.%s\n",
		   refinement->module, refinement->name);
	    if (refinement->typename) {
		printf("        Type: %s.%s\n",
		       refinement->typemodule, refinement->typename);
	    }
	    if (refinement->writetypename) {
		printf("  Write-Type: %s.%s\n",
		       refinement->writetypemodule, refinement->writetypename);
	    }
	    if (refinement->access != SMI_ACCESS_UNKNOWN) {
		printf("      Access: %s\n",
		       smiStringAccess(refinement->access));
	    }
	    printf(" Description: %s\n\n", format(refinement->description));
	}
	printf("\n");
	smiFreeNode(node);
    }

    if (!strcmp(command, "index")) {
	node = smiGetNode(name, NULL);
	printf("       Index:");
	for(index = smiGetFirstIndex(node);
	    index ; index = smiGetNextIndex(index)) {
	    printf(" %s.%s", index->module, index->name);
	}
	printf("\n");
	smiFreeNode(node);
    }

    if (!strcmp(command, "members")) {
	node = smiGetNode(name, NULL);
	printf("     Members:");
	for(child = smiGetFirstMemberNode(node);
	    child ; child = smiGetNextMemberNode(node, child)) {
	    printf(" %s.%s", child->module, child->name);
	}
	printf("\n");
	smiFreeNode(node);
    }

    if (!strcmp(command, "children")) {
	node = smiGetNode(name, NULL);
	printf("    Children:");
	for(child = smiGetFirstChildNode(node);
	    child ; child = smiGetNextChildNode(child)) {
	    printf(" %s.%s", child->module, child->name);
	}
	printf("\n");
	smiFreeNode(node);
    }

    if (!strcmp(command, "type")) {
	type = smiGetType(name, NULL);
	if (type) {
	    printf("        Type: %s\n", format(type->name));
	    printf("      Module: %s\n", format(type->module));
	    printf("    Basetype: %s\n", smiStringBasetype(type->basetype));
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
	    printf("   Reference: %s\n", format(type->reference));
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

    exit(0);
}
