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
 * @(#) $Id$
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
#include "scanner.h"
#include "parser-bison.h"
#include "data.h"



smi_module *smiproc_module_1(getspec)
    smi_getspec *getspec;
{
    static smi_module res;
    
    return &res;
}



smi_node *smiproc_node_1(getspec)
    smi_getspec *getspec;
{
    static smi_node res;
    
    return &res;
}



smi_type *smiproc_type_1(getspec)
    smi_getspec *getspec;
{
    static smi_type res;
    
    return &res;
}



smi_macro *smiproc_macro_1(getspec)
    smi_getspec *getspec;
{
    static smi_macro res;
    
    return &res;
}



smi_namelist *smiproc_names_1(getspec)
    smi_getspec *getspec;
{
    static smi_namelist res;
    
    return &res;
}



smi_namelist *smiproc_children_1(getspec)
    smi_getspec *getspec;
{
    static smi_namelist res;
    
    return &res;
}



smi_namelist *smiproc_members_1(getspec)
    smi_getspec *getspec;
{
    static smi_namelist res;
    
    return &res;
}



smi_fullname *smiproc_parent_1(getspec)
    smi_getspec *getspec;
{
    static smi_fullname res;
    
    return &res;
}



