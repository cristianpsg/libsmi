/*
 * smi.x --
 *
 *      RPC interface definition for the SMI service.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#define SMI_MAX_DESCRIPTOR	64
#define SMI_MAX_OID		1407		/* 128 * 10 + 127 */
#define SMI_MAX_STRING		65535
#define SMI_MAX_FULLNAME	129		/* 64 + 1 + 64 */

typedef string smi_descriptor<SMI_MAX_DESCRIPTOR>;
typedef string smi_oid<SMI_MAX_OID>;
typedef string smi_string<SMI_MAX_STRING>;
typedef string smi_fullname<SMI_MAX_FULLNAME>;

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
};

enum smi_status {
    SMI_STATUS_UNKNOWN	 	= 0 ,
    SMI_STATUS_CURRENT	 	= 1 ,
    SMI_STATUS_DEPRECATED	= 2 ,
    SMI_STATUS_MANDATORY	= 3 ,
    SMI_STATUS_OPTIONAL	 	= 4 ,    
    SMI_STATUS_OBSOLETE	 	= 5
};

enum smi_access {
    SMI_ACCESS_UNKNOWN	 	= 0 ,
    SMI_ACCESS_NOT		= 1 ,
    SMI_ACCESS_NOTIFY	 	= 2 ,
    SMI_ACCESS_READ_ONLY	= 3 ,
    SMI_ACCESS_READ_WRITE	= 4 ,
    SMI_ACCESS_READ_CREATE	= 5 ,
    SMI_ACCESS_WRITE_ONLY	= 6
};

enum smi_decl {
    SMI_DECL_UNKNOWN		= 0 ,
    SMI_DECL_SIMPLEASSIGNMENT  	= 1 ,
    SMI_DECL_OBJECTTYPE	     	= 2 ,
    SMI_DECL_OBJECTIDENTITY     = 3 ,
    SMI_DECL_MODULEIDENTITY     = 4 ,
    SMI_DECL_NOTIFICATIONTYPE   = 5 ,
    SMI_DECL_TRAPTYPE	     	= 6 ,
    SMI_DECL_OBJECTGROUP	= 7 ,
    SMI_DECL_NOTIFICATIONGROUP  = 8 ,
    SMI_DECL_MODULECOMPLIANCE   = 9 ,
    SMI_DECL_AGENTCAPABILITIES  = 10,
    SMI_DECL_TEXTUALCONVENTION	= 11
};



struct smi_getspec {
    smi_namespec	name;
    boolean		wantdescr;
};

struct smi_module {
    smi_descriptor	name;
    smi_oid		oid;
    smi_string		description;
#if 0
    smi_string		lastupdated;
    smi_string		organization;
    smi_string		contact;
#endif;
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
    smi_access		access;
    smi_status		status;
    smi_description	description;
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

struct smi_namelist {
    smi_string		namelist; /* containing blank-separated elements. */
};

program SMIPROG {
    version SMIVERS {
	smi_module	SMIPROC_MODULE(smi_getspec)	= 1;
	smi_node	SMIPROC_NODE(smi_getspec)	= 2;
	smi_type	SMIPROC_TYPE(smi_getspec)	= 3;
	smi_namelist	SMIPROC_NAMES(smi_namespec) 	= 4;
	smi_namelist	SMIPROC_CHILDREN(smi_namespec) 	= 5;
	smi_namelist	SMIPROC_MEMBERS(smi_namespec) 	= 6;
    } = 1;
} = 0x22315258; /* User-defined range: 0x20000000 - 0x3fffffff */
