/*
 * smi_svc_proc.c --
 *
 *      SMI RPC Service routines.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi_svc_proc.c,v 1.1 1998/11/19 19:44:04 strauss Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#ifdef linux
#include <getopt.h>
#endif

#include "smi.h"
#include "defs.h"
#include "config.h"
#include "error.h"
#include "data.h"



smi_module *smiproc_module_1(getspec)
    smi_getspec *getspec;
{
    static smi_module dummy = { NULL, NULL, NULL, NULL, NULL, NULL };
    smi_module *m;

    printDebug(4, "smiproc_module_1(...)\n");

    m = smiGetModule(getspec->name, getspec->wantdescr);

    if (m) {
	return m;
    } else {
	return &dummy;
    }
}



smi_node *smiproc_node_1(getspec)
    smi_getspec *getspec;
{
    static smi_node dummy = { NULL, NULL, NULL, NULL,
			      SMI_DECL_UNKNOWN, SMI_ACCESS_UNKNOWN,
			      SMI_STATUS_UNKNOWN, NULL };
    smi_node *n;
    
    printDebug(4, "smiproc_node_1(...)\n");

    n = smiGetNode(getspec->name, NULL, getspec->wantdescr);

    if (n) {
	return n;
    } else {
	return &dummy;
    }
}



smi_type *smiproc_type_1(getspec)
    smi_getspec *getspec;
{
    static smi_type dummy = { NULL, NULL, SMI_SYNTAX_UNKNOWN,
			      SMI_DECL_UNKNOWN, NULL, SMI_STATUS_UNKNOWN,
			      NULL };
    smi_type *t;
    
    printDebug(4, "smiproc_type_1(...)\n");

    t = smiGetType(getspec->name, NULL, getspec->wantdescr);

    if (t) {
	return t;
    } else {
	return &dummy;
    }
}



smi_macro *smiproc_macro_1(fullname)
    smi_fullname fullname;
{
    static smi_macro dummy = { NULL, NULL };
    smi_macro *m;
    
    printDebug(4, "smiproc_macro_1(...)\n");

    m = smiGetMacro(fullname, NULL);

    if (m) {
	return m;
    } else {
	return &dummy;
    }	    
}



smi_namelist *smiproc_names_1(fullname)
    smi_fullname fullname;
{
    return smiGetNames(fullname, NULL);
}



smi_namelist *smiproc_children_1(fullname)
    smi_fullname fullname;
{
    return smiGetChildren(fullname, NULL);
}



smi_namelist *smiproc_members_1(fullname)
    smi_fullname fullname;
{
    return smiGetMembers(fullname, NULL);
}



smi_fullname *smiproc_parent_1(fullname)
    smi_fullname fullname;
{
    return smiGetParent(fullname, NULL);
}
