/*
 * smi.h --
 *
 *      Interface Implementation of libsmi.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi.h,v 1.1 1999/03/25 14:15:15 strauss Exp $
 */

#ifndef _SMI_H
#define _SMI_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



#define SMI_CONFIG_FILE         "/usr/local/etc/smi.conf"

#define SMI_NAMESPACE_OPERATOR	"::"

#define SMI_VIEWALL     0x0800  /* all modules are `known', need no views. */
#define SMI_ERRORS      0x1000  /* print parser errors. */
#define SMI_ERRORLINES  0x2000  /* print error line contents of modules. */
#define SMI_STATS       0x4000  /* print statistics after parsing a module. */
#define SMI_RECURSIVE   0x8000  /* recursively parse imported modules. */
#define SMI_FLAGMASK \
                (SMI_VIEWALL|SMI_STATS|SMI_RECURSIVE|SMI_ERRORS|SMI_ERRORLINES)

#define SMI_MAX_DESCRIPTOR	64
#define SMI_MAX_OID		1407		/* 128 * 10 + 127 */
#define SMI_MAX_STRING		65535
#define SMI_MAX_FULLNAME	130		/* 64 + 2 + 64 */



typedef unsigned long long	SmiUnsigned64;

typedef long long		SmiInteger64;

typedef unsigned long		SmiUnsigned32;

typedef long			SmiInteger32;

typedef unsigned int		SmiSubid;

typedef unsigned long long	SmiNumber;



typedef enum SmiSyntax {
    SMI_SYNTAX_UNKNOWN		= 0,
    SMI_SYNTAX_INTEGER32        = 1, /* equal to INTEGER */
    SMI_SYNTAX_OCTETSTRING	= 2,
    SMI_SYNTAX_OBJECTIDENTIFIER = 3,
    SMI_SYNTAX_SEQUENCE	     	= 4,
    SMI_SYNTAX_SEQUENCEOF	= 5,
    SMI_SYNTAX_IPADDRESS	= 6,
    SMI_SYNTAX_COUNTER32	= 7,
    SMI_SYNTAX_GAUGE32	     	= 8,
    SMI_SYNTAX_UNSIGNED32	= 9,
    SMI_SYNTAX_TIMETICKS	= 10,
    SMI_SYNTAX_OPAQUE	     	= 11,
    SMI_SYNTAX_COUNTER64	= 12,
    SMI_SYNTAX_CHOICE	     	= 13, /* only for internal use */
    SMI_SYNTAX_BITS             = 14,

    SMI_SYNTAX_INTEGER64	= 33,
    SMI_SYNTAX_UNSIGNED64       = 34,
    SMI_SYNTAX_GAUGE64          = 35,
    SMI_SYNTAX_FLOAT32          = 36,
    SMI_SYNTAX_FLOAT64          = 37,
    SMI_SYNTAX_FLOAT128         = 38,
    SMI_SYNTAX_ENUM             = 39,

    SMI_SYNTAX_HEX		= 65, /* for internal use */
    SMI_SYNTAX_BIN		= 66, /* for internal use */
    SMI_SYNTAX_STRING		= 67  /* for internal use */
} SmiSyntax;

typedef enum SmiStatus {
    SMI_STATUS_UNKNOWN	 	= 0,
    SMI_STATUS_CURRENT	 	= 1,
    SMI_STATUS_DEPRECATED	= 2,
    SMI_STATUS_MANDATORY	= 3,
    SMI_STATUS_OPTIONAL	 	= 4,    
    SMI_STATUS_OBSOLETE	 	= 5
} SmiStatus;

typedef enum SmiAccess {
    SMI_ACCESS_UNKNOWN	 	= 0,
    SMI_ACCESS_NOT_ACCESSIBLE	= 1,
    SMI_ACCESS_NOTIFY	 	= 2,
    SMI_ACCESS_READ_ONLY	= 3,
    SMI_ACCESS_READ_WRITE	= 4,
    SMI_ACCESS_READ_CREATE	= 5,
    SMI_ACCESS_WRITE_ONLY	= 6
} SmiAccess;

typedef enum SmiDecl {
    SMI_DECL_UNKNOWN		= 0,
    SMI_DECL_TYPEASSIGNMENT  	= 1,
    SMI_DECL_VALUEASSIGNMENT  	= 2,
    SMI_DECL_OBJECTTYPE	     	= 3,
    SMI_DECL_OBJECTIDENTITY     = 4,
    SMI_DECL_MODULEIDENTITY     = 5,
    SMI_DECL_NOTIFICATIONTYPE   = 6,
    SMI_DECL_TRAPTYPE	     	= 7,
    SMI_DECL_OBJECTGROUP	= 8, 
    SMI_DECL_NOTIFICATIONGROUP  = 9,
    SMI_DECL_MODULECOMPLIANCE   = 10,
    SMI_DECL_AGENTCAPABILITIES  = 11,
    SMI_DECL_TEXTUALCONVENTION	= 12,

    SMI_DECL_MODULE		= 33,
    SMI_DECL_TYPEDEF		= 34,
    SMI_DECL_NODE		= 35,
    SMI_DECL_SCALAR		= 36,
    SMI_DECL_TABLE		= 37,
    SMI_DECL_ROW		= 38,
    SMI_DECL_COLUMN		= 39,
    SMI_DECL_NOTIFICATION	= 40,
    SMI_DECL_GROUP		= 41,
    SMI_DECL_COMPLIANCE		= 42
} SmiDecl;



typedef struct SmiRevision {
    long	        date;
    char		*description;
} SmiRevision;

typedef struct SmiValue {
    SmiSyntax	        syntax;
    union {
	SmiUnsigned64      unsigned64;
	SmiInteger64       integer64;
	SmiUnsigned32      unsigned32;
	SmiInteger32       integer32;
	char		   *ptr;
	char		   **bits;
	/* TODO ... */
    } value;
} SmiValue;
   
typedef struct SmiNamedNumber {
    char	        *name;
    SmiValue	        *valuePtr;
} SmiNamedNumber;

typedef struct SmiRange {
    SmiValue	        *minValuePtr;
    SmiValue	        *maxValuePtr;
} SmiRange;

typedef struct SmiModule {
    char		*name;
    char		*object;
    long		lastupdated;
    char		*organization;
    char 		*contactinfo;
    char		*description;
    char		*reference;
    SmiRevision		**revisions;
} SmiModule;

typedef struct SmiNode {
    char		*name;
    char		*module;
    char		*oid;
    char		*type;
    char		**index;
    SmiDecl		decl;
    SmiSyntax		syntax;
    SmiAccess		access;
    SmiStatus		status;
    char		*format;
    SmiValue	        *value;
    char		*units;
    char		*description;
    char		*reference;
} SmiNode;

typedef struct SmiType {
    char		*name;
    char		*module;
    SmiSyntax		syntax;
    char		*parent;
    void		**list;   /* SmiNamedNumber or SmiRange */
    SmiDecl		decl;
    char		*format;
    SmiValue	        *value;
    char		*units;
    SmiStatus		status;
    char		*description;
    char		*reference;
} SmiType;

typedef struct SmiMacro {
    char		*name;
    char		*module;
} SmiMacro;



/*
 * Functions (macros) to get strings namings libsmi types.
 */

#define smingStringStatus(status) ( \
	(status == SMI_STATUS_CURRENT)     ? "current" : \
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" : \
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" : \
	(status == SMI_STATUS_MANDATORY)   ? "current" : \
	(status == SMI_STATUS_OPTIONAL)    ? "current" : \
					     "<unknown>" )

#define smiStringStatus(status) ( \
	(status == SMI_STATUS_CURRENT)     ? "current" : \
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" : \
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" : \
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" : \
	(status == SMI_STATUS_OPTIONAL)    ? "optional" : \
					     "<unknown>" )

#define smiStringAccess(access) ( \
	(access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" : \
	(access == SMI_ACCESS_NOTIFY)	      ? "accessible-for-notify" : \
	(access == SMI_ACCESS_READ_ONLY)      ? "read-only" : \
	(access == SMI_ACCESS_READ_WRITE)     ? "read-write" : \
	(access == SMI_ACCESS_READ_CREATE)    ? "read-create" : \
	(access == SMI_ACCESS_WRITE_ONLY)     ? "write-only" : \
						"<unknown>" )

#define smiStringDecl(macro) ( \
	(macro == SMI_DECL_UNKNOWN)           ? "<UNKNOWN>" : \
	(macro == SMI_DECL_TYPEASSIGNMENT)    ? "<TYPE-ASSIGNMENT>" : \
	(macro == SMI_DECL_VALUEASSIGNMENT)   ? "<VALUE-ASSIGNMENT>" : \
	(macro == SMI_DECL_OBJECTTYPE)        ? "OBJECT-TYPE" : \
	(macro == SMI_DECL_OBJECTIDENTITY)    ? "OBJECT-IDENTITY" : \
	(macro == SMI_DECL_MODULEIDENTITY)    ? "MODULE-IDENTITY" : \
	(macro == SMI_DECL_NOTIFICATIONTYPE)  ? "NOTIFICATIONTYPE" : \
	(macro == SMI_DECL_TRAPTYPE)          ? "TRAP-TYPE" : \
	(macro == SMI_DECL_OBJECTGROUP)       ? "OBJECT-GROUP" : \
	(macro == SMI_DECL_NOTIFICATIONGROUP) ? "NOTIFICATION-GROUP" : \
	(macro == SMI_DECL_MODULECOMPLIANCE)  ? "MODULE-COMPLIANCE" : \
	(macro == SMI_DECL_AGENTCAPABILITIES) ? "AGENT-CAPABILITIES" : \
	(macro == SMI_DECL_TEXTUALCONVENTION) ? "TEXTUAL-CONVENTION" : \
					        "<unknown>" )

#define smiStringSyntax(syntax) ( \
	(syntax == SMI_SYNTAX_UNKNOWN)           ? "<UNKNOWN>" : \
	(syntax == SMI_SYNTAX_INTEGER32)         ? "INTEGER" : \
	(syntax == SMI_SYNTAX_OCTETSTRING)       ? "OCTET STRING" : \
	(syntax == SMI_SYNTAX_OBJECTIDENTIFIER)  ? "OBJECT IDENTIFIER" : \
	(syntax == SMI_SYNTAX_SEQUENCE)          ? "SEQUENCE" : \
	(syntax == SMI_SYNTAX_SEQUENCEOF)        ? "SEQUENCE OF" : \
	(syntax == SMI_SYNTAX_IPADDRESS)         ? "IpAddress" : \
	(syntax == SMI_SYNTAX_COUNTER32)         ? "Counter32" : \
	(syntax == SMI_SYNTAX_GAUGE32)           ? "Gauge32" : \
	(syntax == SMI_SYNTAX_UNSIGNED32)        ? "Unsigned32" : \
	(syntax == SMI_SYNTAX_TIMETICKS)         ? "TimeTicks" : \
	(syntax == SMI_SYNTAX_OPAQUE)            ? "Opaque" : \
	(syntax == SMI_SYNTAX_COUNTER64)         ? "Counter64" : \
	(syntax == SMI_SYNTAX_CHOICE)            ? "CHOICE" : \
					           "<unknown>" )



/*
 * libsmi Function Definitions.
 */

extern time_t smiMkTime(const char *s);

extern char *smiCTime(time_t t);

extern char *smiModule(char *fullname);

extern char *smiDescriptor(char *fullname);

extern int smiIsImported(char *modulename,
			 char *fullname);

extern void smiInit();

extern void smiSetDebugLevel(int level);

extern void smiSetErrorLevel(int level);

extern void smiSetFlags(int userflags);

extern int smiGetFlags();

extern int smiReadConfig(const char *file);

extern int smiAddLocation(const char *location);

extern int smiLoadModule(char *modulename);

extern SmiModule *smiGetModule(char *spec);

extern SmiRevision *smiGetFirstRevision(char *modulename);

extern SmiRevision *smiGetNextRevision(char *modulename,
				       time_t date);

extern SmiNode *smiGetNode(char *spec,
			   char *mod);

extern SmiNode *smiGetFirstNode(char *modulename);

extern SmiNode *smiGetNextNode(char *modulename,
			       char *name);

extern SmiType *smiGetType(char *spec,
			   char *mod);

extern SmiType *smiGetFirstType(char *modulename);

extern SmiType *smiGetNextType(char *modulename,
			       char *name);

extern SmiMacro *smiGetMacro(char *spec,
			     char *mod);

extern char **smiGetNames(char *spec,
			  char *mod);

extern char **smiGetChildren(char *spec,
			     char *mod);

extern char **smiGetMembers(char *spec,
			    char *mod);

extern char *smiGetParent(char *spec,
			  char *mod);



#endif /* _SMI_H */
