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
 * @(#) $Id: smi_svc_proc.c,v 1.5 1998/11/23 12:57:00 strauss Exp $
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


smi_module *
#ifdef linux
smiproc_module_1_svc(getspec, req)
    smi_getspec *getspec;
    struct svc_req *req;
#else
smiproc_module_1(getspec)
    smi_getspec *getspec;
#endif
{
    static smi_module dummy = { "", "", "", "", "", "" };
    smi_module *m;

    printDebug(4, "smiproc_module_1(<%s,%d>)\n",
	       getspec->name, getspec->wantdescr);

    m = smiGetModule(getspec->name, getspec->wantdescr);

    if (m) {
	return m;
    } else {
	return &dummy;
    }
}



smi_node *
#ifdef linux
smiproc_node_1_svc(getspec, req)
    smi_getspec *getspec;
    struct svc_req *req;
#else
smiproc_node_1(getspec)
    smi_getspec *getspec;
#endif
{
    static smi_node dummy = { "", "", "", "",
			      SMI_DECL_UNKNOWN, SMI_ACCESS_UNKNOWN,
			      SMI_STATUS_UNKNOWN, "" };
    smi_node *n;
    
    printDebug(4, "smiproc_node_1(<%s,%d>)\n",
	       getspec->name, getspec->wantdescr);

    n = smiGetNode(getspec->name, NULL, getspec->wantdescr);

    if (n) {
	return n;
    } else {
	return &dummy;
    }
}



smi_type *
#ifdef linux
smiproc_type_1_svc(getspec, req)
    smi_getspec *getspec;
    struct svc_req *req;
#else
smiproc_type_1(getspec)
    smi_getspec *getspec;
#endif
{
    static smi_type dummy = { "", "", SMI_SYNTAX_UNKNOWN,
			      SMI_DECL_UNKNOWN, "", SMI_STATUS_UNKNOWN,
			      "" };
    smi_type *t;
    
    printDebug(4, "smiproc_type_1(<%s,%d>)\n",
	       getspec->name, getspec->wantdescr);

    t = smiGetType(getspec->name, NULL, getspec->wantdescr);

    if (t) {
	return t;
    } else {
	return &dummy;
    }
}



smi_macro *
#ifdef linux
smiproc_macro_1_svc(fullname, req)
    smi_fullname *fullname;
    struct svc_req *req;
#else
smiproc_macro_1(fullname)
    smi_fullname *fullname;
#endif
{
    static smi_macro dummy = { "", "" };
    smi_macro *m;
    
    printDebug(4, "smiproc_macro_1(%s)\n", fullname);

    m = smiGetMacro(*fullname, NULL);

    if (m) {
	return m;
    } else {
	return &dummy;
    }	    
}



smi_namelist *
#ifdef linux
smiproc_names_1_svc(fullname, req)
    smi_fullname *fullname;
    struct svc_req *req;
#else
smiproc_names_1(fullname)
    smi_fullname *fullname;
#endif
{
    static smi_namelist dummy = { "" };
    smi_namelist *n;
    
    printDebug(4, "smiproc_names_1(%s)\n", fullname);

    n = smiGetNames(*fullname, NULL);

    if (n) {
	return n;
    } else {
	return &dummy;
    }	    
}



smi_namelist *
#ifdef linux
smiproc_children_1_svc(fullname, req)
    smi_fullname *fullname;
    struct svc_req *req;
#else
smiproc_children_1(fullname)
    smi_fullname *fullname;
#endif
{
    static smi_namelist dummy = { "" };
    smi_namelist *n;
    
    printDebug(4, "smiproc_children_1(%s)\n", fullname);

    n = smiGetChildren(*fullname, NULL);

    if (n) {
	return n;
    } else {
	return &dummy;
    }	    
}



smi_namelist *
#ifdef linux
smiproc_members_1_svc(fullname, req)
    smi_fullname *fullname;
    struct svc_req *req;
#else
smiproc_members_1(fullname)
    smi_fullname *fullname;
#endif
{
    static smi_namelist dummy = { "" };
    smi_namelist *n;
    
    printDebug(4, "smiproc_children_1(%s)\n", fullname);

    n = smiGetChildren(*fullname, NULL);

    if (n) {
	return n;
    } else {
	return &dummy;
    }	    
}



smi_fullname *
#ifdef linux
smiproc_parent_1_svc(fullname, req)
    smi_fullname *fullname;
    struct svc_req *req;
#else
smiproc_parent_1(fullname)
    smi_fullname *fullname;
#endif
{
    return smiGetParent(*fullname, NULL);
}
