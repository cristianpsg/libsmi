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
 * @(#) $Id: smi.h,v 1.6 1999/03/30 18:37:23 strauss Exp $
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

typedef double			SmiFloat64; /* currently, no 32/64/128 diffs */



typedef enum SmiBasetype {
    SMI_BASETYPE_UNKNOWN		= 0,
    SMI_BASETYPE_INTEGER32		= 1,
    SMI_BASETYPE_OCTETSTRING		= 2,
    SMI_BASETYPE_OBJECTIDENTIFIER	= 3,
    SMI_BASETYPE_UNSIGNED32		= 4,
    SMI_BASETYPE_INTEGER64		= 5,
    SMI_BASETYPE_UNSIGNED64		= 6,
    SMI_BASETYPE_FLOAT32		= 7,
    SMI_BASETYPE_FLOAT64		= 8,
    SMI_BASETYPE_FLOAT128		= 9,
    SMI_BASETYPE_ENUM			= 10,
    SMI_BASETYPE_BITS			= 11,
    SMI_BASETYPE_HEXSTRING		= 12,
    SMI_BASETYPE_BINSTRING		= 13,
    SMI_BASETYPE_LABEL			= 14,
    SMI_BASETYPE_CHOICE			= 15,
    SMI_BASETYPE_SEQUENCE	     	= 16,
    SMI_BASETYPE_SEQUENCEOF		= 17
} SmiBasetype;

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

typedef enum SmiIndexkind {
    SMI_INDEX_UNKNOWN		= 0,
    SMI_INDEX_INDEX		= 1,
    SMI_INDEX_AUGMENT		= 2,
    SMI_INDEX_REORDER		= 3,
    SMI_INDEX_SPARSE		= 4,
    SMI_INDEX_EXPAND		= 5
} SmiIndexkind;

typedef struct SmiRevision {
    long	        date;
    char		*description;
} SmiRevision;

typedef struct SmiValue {
    SmiBasetype	        basetype;
    union {
	SmiUnsigned64       unsigned64;
	SmiInteger64        integer64;
	SmiUnsigned32       unsigned32;
	SmiInteger32        integer32;
	SmiFloat64	    float64;
	char		    *oid;
	char		    *ptr;
	char		    **bits;
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

typedef struct SmiOption {
    char		*name;
    char		*module;
    char		*description;
} SmiOption;
    
typedef struct SmiRefinement {
    char		*name;
    char		*module;
    char		*description;
    char		*type;
    char		*writetype;
    SmiAccess		access;
} SmiRefinement;
    
typedef struct SmiModule {
    char		*name;
    char		*object;
    long		lastupdated;
    char		*organization;
    char 		*contactinfo;
    char		*description;
    char		*reference;
} SmiModule;

typedef struct SmiNode {
    char		*name;
    char		*module;
    char		*oid;
    char		*type;
    char		**list;
    SmiIndexkind	indexkind;
    int			implied;
    char		**index;
    char		*relatedrow;
    SmiOption		**option;
    SmiRefinement	**refinement;
    SmiDecl		decl;
    SmiBasetype		basetype;
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
    SmiBasetype		basetype;
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

#define smingStringAccess(access) ( \
	(access == SMI_ACCESS_NOT_ACCESSIBLE) ? "noaccess" : \
	(access == SMI_ACCESS_NOTIFY)	      ? "notifyonly" : \
	(access == SMI_ACCESS_READ_ONLY)      ? "readonly" : \
	(access == SMI_ACCESS_READ_WRITE)     ? "readwrite" : \
	(access == SMI_ACCESS_READ_CREATE)    ? "readwrite" : \
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

#define smiStringBasetype(basetype) ( \
	(basetype == SMI_BASETYPE_UNKNOWN)           ? "<UNKNOWN>" : \
	(basetype == SMI_BASETYPE_OCTETSTRING)       ? "OctetString" : \
	(basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectIdentifier" : \
	(basetype == SMI_BASETYPE_UNSIGNED32)        ? "Unsigned32" : \
	(basetype == SMI_BASETYPE_INTEGER32)         ? "Integer32" : \
	(basetype == SMI_BASETYPE_UNSIGNED64)        ? "Unsigned64" : \
	(basetype == SMI_BASETYPE_INTEGER64)         ? "Integer64" : \
	(basetype == SMI_BASETYPE_FLOAT32)           ? "Float32" : \
	(basetype == SMI_BASETYPE_FLOAT64)           ? "Float64" : \
	(basetype == SMI_BASETYPE_FLOAT128)          ? "Float128" : \
	(basetype == SMI_BASETYPE_ENUM)              ? "Enumeration" : \
	(basetype == SMI_BASETYPE_BITS)              ? "Bits" : \
	(basetype == SMI_BASETYPE_SEQUENCE)          ? "SEQUENCE" : \
	(basetype == SMI_BASETYPE_SEQUENCEOF)        ? "SEQUENCE OF" : \
	(basetype == SMI_BASETYPE_CHOICE)            ? "CHOICE" : \
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

extern void smiFreeModule(SmiModule *smiModulePtr);

extern SmiRevision *smiGetFirstRevision(char *modulename);

extern SmiRevision *smiGetNextRevision(char *modulename,
				       time_t date);

extern void smiFreeRevision(SmiRevision *smiRevisionPtr);

extern SmiNode *smiGetNode(char *spec,
			   char *mod);

extern SmiNode *smiGetFirstNode(char *modulename);

extern SmiNode *smiGetNextNode(char *modulename,
			       char *name);

extern void smiFreeNode(SmiNode *smiNodePtr);

extern char **smiGetMandatoryGroups(char *spec,
				    char *mod);

extern SmiType *smiGetType(char *spec,
			   char *mod);

extern SmiType *smiGetFirstType(char *modulename);

extern SmiType *smiGetNextType(char *modulename,
			       char *name);

extern void smiFreeType(SmiType *smiTypePtr);

extern SmiMacro *smiGetMacro(char *spec,
			     char *mod);

extern void smiFreeMacro(SmiMacro *smiMacroPtr);

extern char **smiGetNames(char *spec,
			  char *mod);

extern char **smiGetChildren(char *spec,
			     char *mod);

extern char **smiGetMembers(char *spec,
			    char *mod);

extern char *smiGetParent(char *spec,
			  char *mod);



#endif /* _SMI_H */
