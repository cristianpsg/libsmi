%
%/*
% * smi.x -- ( smi.h smi_clnt.c smi_svc.c smi_xdr.c )
% *
% *      RPC interface definition for the SMI service.
% *
% * Copyright (c) 1998 Technical University of Braunschweig.
% *
% * See the file "license.terms" for information on usage and redistribution
% * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
% *
% * @(#) $Id: smi.x,v 1.12 1998/11/23 12:56:59 strauss Exp $
% */
%

const SMI_MAX_DESCRIPTOR	= 64;
const SMI_MAX_OID		= 1407;		/* 128 * 10 + 127 */
const SMI_MAX_STRING		= 65535;
const SMI_MAX_FULLNAME		= 129;		/* 64 + 1 + 64 */

typedef string smi_descriptor<SMI_MAX_DESCRIPTOR>;
typedef string smi_oid<SMI_MAX_OID>;
typedef string smi_string<SMI_MAX_STRING>;
typedef string smi_fullname<SMI_MAX_FULLNAME>;

typedef unsigned int smi_subid;

enum smi_syntax {
    SMI_SYNTAX_UNKNOWN		= 0,
    SMI_SYNTAX_INTEGER          = 1, /* equal to INTEGER32 */
    SMI_SYNTAX_OCTET_STRING	= 2,
    SMI_SYNTAX_OBJECT_IDENTIFIER = 3,
    SMI_SYNTAX_SEQUENCE	     	= 4,
    SMI_SYNTAX_SEQUENCE_OF	= 5,
    SMI_SYNTAX_IPADDRESS	= 6,
    SMI_SYNTAX_COUNTER32	= 7,
    SMI_SYNTAX_GAUGE32	     	= 8,
    SMI_SYNTAX_UNSIGNED32	= 9,
    SMI_SYNTAX_TIMETICKS	= 10,
    SMI_SYNTAX_OPAQUE	     	= 11,
    SMI_SYNTAX_COUNTER64	= 12,
    SMI_SYNTAX_CHOICE	     	= 13 /* only for internal use */
    /* TODO: BITS ? */
};

enum smi_status {
    SMI_STATUS_UNKNOWN	 	= 0,
    SMI_STATUS_CURRENT	 	= 1,
    SMI_STATUS_DEPRECATED	= 2,
    SMI_STATUS_MANDATORY	= 3,
    SMI_STATUS_OPTIONAL	 	= 4,    
    SMI_STATUS_OBSOLETE	 	= 5
};

enum smi_access {
    SMI_ACCESS_UNKNOWN	 	= 0,
    SMI_ACCESS_NOT_ACCESSIBLE	= 1,
    SMI_ACCESS_NOTIFY	 	= 2,
    SMI_ACCESS_READ_ONLY	= 3,
    SMI_ACCESS_READ_WRITE	= 4,
    SMI_ACCESS_READ_CREATE	= 5,
    SMI_ACCESS_WRITE_ONLY	= 6
};

enum smi_decl {
    SMI_DECL_UNKNOWN		= 0,
    SMI_DECL_SIMPLEASSIGNMENT  	= 1,
    SMI_DECL_OBJECTTYPE	     	= 2,
    SMI_DECL_OBJECTIDENTITY     = 3,
    SMI_DECL_MODULEIDENTITY     = 4,
    SMI_DECL_NOTIFICATIONTYPE   = 5,
    SMI_DECL_TRAPTYPE	     	= 6,
    SMI_DECL_OBJECTGROUP	= 7, 
    SMI_DECL_NOTIFICATIONGROUP  = 8,
    SMI_DECL_MODULECOMPLIANCE   = 9,
    SMI_DECL_AGENTCAPABILITIES  = 10,
    SMI_DECL_TEXTUALCONVENTION	= 11
};

struct smi_getspec {
    smi_fullname	name;
    int			wantdescr;
};

struct smi_module {
    smi_descriptor	name;
    smi_oid		oid;
    smi_string		lastupdated;
    smi_string		organization;
    smi_string		contactinfo;
    smi_string		description;
    /* TODO: revisions */
};

struct smi_node {
    smi_descriptor	name;
    smi_descriptor	module;
    smi_oid		oid;
    smi_fullname	type;
#if 0
    /* TODO */
    smi_index		index;
#endif
    smi_decl		decl;
    smi_syntax		syntax;
    smi_access		access;
    smi_status		status;
    smi_string		description;
};

struct smi_type {
    smi_descriptor	name;
    smi_descriptor	module;
    smi_syntax		syntax;
    smi_decl		decl;
    smi_string		display;
    smi_status		status;
    smi_string		description;
    /* TODO: parent ? */
    /* TODO: restrictions */
};

struct smi_macro {
    smi_descriptor	name;
    smi_descriptor	module;
};

/*
 * smi_namelist represents a string containing a blank-separated list
 * of elements. These elements are usually fully qualified smi_fullnames
 * representing mib nodes or types.
 *
 * TODO: think about length limitations.
 */
struct smi_namelist {
    smi_string		namelist;
};

program SMIPROG {
    version SMIVERS {
	/*
	 * SMIPROC_MODULE returns the module information. The input's
	 * smi_getspec.smi_fullname value must be a module name.
	 */
	smi_module	SMIPROC_MODULE(smi_getspec)	= 1;
	/*
	 * SMIPROC_NODE returns the node information. The input's
	 * smi_getspec.smi_fullname might also be an object instance
	 * identifiers to retrieve the corresponding object type node.
	 */
	smi_node	SMIPROC_NODE(smi_getspec)	= 2;
	/*
	 * SMIPROC_TYPE
	 */
	smi_type	SMIPROC_TYPE(smi_getspec)	= 3;
	/*
	 * SMIPROC_MACRO
	 */
	smi_macro	SMIPROC_MACRO(smi_fullname)	= 4;
	/*
	 * SMIPROC_NAMES returns a blank-separated list of fully
	 * qualified smi_fullnames that match the given input,
	 * which might be a non-unique descriptor or an OID that
	 * is defined in multiple modules.
	 */
	smi_namelist	SMIPROC_NAMES(smi_fullname) 	= 5;
	/*
	 * SMIPROC_CHILDREN returns a blank-separated list of fully
	 * qualified smi_fullnames representing all child nodes of
	 * the given input node.
	 * TODO: Probably this list can get very large (e.g. mib-2).
	 *	 Possible solution: return childeren as numerical OID
	 *	 ranges. Problem: Then the client cannot filter out the
	 *	 children that do not belong to its view by modules.
	 */
	smi_namelist	SMIPROC_CHILDREN(smi_fullname) 	= 6;
	/*
	 * SMIPROC_MEMBERS returns a blank-separated list of fully
	 * qualified smi_fullnames representing
	 * - all members of the given input node if it represents an
	 *   object group, or (TODO)
	 * - all members of the given input node if it represents a
	 *   notification group, or (TODO)
	 * - all variables that must be present in a notification
	 *   if it represents a notification node, or (TODO)
	 * - all columns if it represents a row type (SEQUENCE), or
	 * - all index elements of a table if it represents a table
	 *   object type (SEQUENCE OF ...).
	 * - [??? all augmentation elements if it represents a table
	 *   augmentation object type. ???]
	 */
	smi_namelist	SMIPROC_MEMBERS(smi_fullname) 	= 7;
	/*
	 * SMIPROC_PARENT returns a fully qualified smi_fullname
	 * representing the parent of the given input node.
	 */
	smi_fullname	SMIPROC_PARENT(smi_fullname) 	= 8;
    } = 1;
} = 0x22315258; /* User-defined range: 0x20000000 - 0x3fffffff */
