/*
 * smi.h --
 *
 *      Interface Definition of libsmi.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi.h,v 1.50 1999/12/12 12:51:07 strauss Exp $
 */

#ifndef _SMI_H
#define _SMI_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



#define SMI_FLAG_VIEWALL   0x1000 /* all modules are `known', need no views. */
#define SMI_FLAG_ERRORS    0x2000 /* print parser errors.                    */
#define SMI_FLAG_RECURSIVE 0x4000 /* recursively parse imported modules.     */
#define SMI_FLAG_STATS     0x8000 /* print statistics after parsing module.  */
#define SMI_FLAG_MASK      (SMI_FLAG_VIEWALL|SMI_FLAG_STATS|\
			    SMI_FLAG_RECURSIVE|SMI_FLAG_ERRORS)



/* misc mappings of SMI types to C types                                     */
typedef char                    *SmiIdentifier;
typedef unsigned long           SmiUnsigned32;
typedef long                    SmiInteger32;
typedef unsigned long long      SmiUnsigned64;
typedef long long               SmiInteger64;
typedef unsigned int            SmiSubid;
typedef float                   SmiFloat32;
typedef double                  SmiFloat64;
typedef long double             SmiFloat128;



/* SmiLanguage -- language of an actual MIB module                           */
typedef enum SmiLanguage {
    SMI_LANGUAGE_UNKNOWN                = 0,  /* should not occur            */
    SMI_LANGUAGE_SMIV1                  = 1,
    SMI_LANGUAGE_SMIV2                  = 2,
    SMI_LANGUAGE_SMING                  = 3
} SmiLanguage;

/* SmiBasetype -- base types of all languages                                */
typedef enum SmiBasetype {
    SMI_BASETYPE_UNKNOWN                = 0,  /* should not occur            */
    SMI_BASETYPE_INTEGER32              = 1,  /* also SMIv1/v2 INTEGER       */
    SMI_BASETYPE_OCTETSTRING            = 2,
    SMI_BASETYPE_OBJECTIDENTIFIER       = 3,
    SMI_BASETYPE_UNSIGNED32             = 4,
    SMI_BASETYPE_INTEGER64              = 5,  /* only SMIng                  */
    SMI_BASETYPE_UNSIGNED64             = 6,  /* SMIv2 and SMIng             */
    SMI_BASETYPE_FLOAT32                = 7,  /* only SMIng                  */
    SMI_BASETYPE_FLOAT64                = 8,  /* only SMIng                  */
    SMI_BASETYPE_FLOAT128               = 9,  /* only SMIng                  */
    SMI_BASETYPE_ENUM                   = 10,
    SMI_BASETYPE_BITS                   = 11  /* only SMIv2 and SMIng        */
} SmiBasetype;

/* SmiStatus -- values of status levels                                      */
typedef enum SmiStatus {
    SMI_STATUS_UNKNOWN          = 0, /* should not occur                     */
    SMI_STATUS_CURRENT          = 1, /* only SMIv2 and SMIng                 */
    SMI_STATUS_DEPRECATED       = 2, /* SMIv1, SMIv2 and SMIng               */
    SMI_STATUS_MANDATORY        = 3, /* only SMIv1                           */
    SMI_STATUS_OPTIONAL         = 4, /* only SMIv1                           */
    SMI_STATUS_OBSOLETE         = 5  /* only SMIv2 and SMIng                 */
} SmiStatus;

/* SmiAccess -- values of access levels                                      */
typedef enum SmiAccess {
    SMI_ACCESS_UNKNOWN          = 0, /* should not occur                     */
    SMI_ACCESS_NOT_ACCESSIBLE   = 1, /* the values 1 to 5 are allowed to be  */
    SMI_ACCESS_NOTIFY           = 2, /* compared by relational operators.    */
    SMI_ACCESS_READ_ONLY        = 3,
    SMI_ACCESS_READ_WRITE       = 4
} SmiAccess;

/* SmiNodekind -- type or statement that leads to a definition               */
typedef unsigned int SmiNodekind;
#define SMI_NODEKIND_UNKNOWN      0x0000     /* should not occur             */
#define SMI_NODEKIND_MODULE       0x0001
#define SMI_NODEKIND_NODE         0x0002
#define SMI_NODEKIND_SCALAR       0x0004
#define SMI_NODEKIND_TABLE        0x0008
#define SMI_NODEKIND_ROW          0x0010
#define SMI_NODEKIND_COLUMN       0x0020
#define SMI_NODEKIND_NOTIFICATION 0x0040
#define SMI_NODEKIND_GROUP        0x0080
#define SMI_NODEKIND_COMPLIANCE   0x0100
#define SMI_NODEKIND_ANY          0xffff

/* SmiDecl -- type or statement that leads to a definition                   */
typedef enum SmiDecl {
    SMI_DECL_UNKNOWN            = 0,  /* should not occur                    */
    /* SMIv1/v2 ASN.1 statements and macros */
    SMI_DECL_IMPLICIT_TYPE      = 1,
    SMI_DECL_TYPEASSIGNMENT     = 2,
    SMI_DECL_IMPL_SEQUENCE      = 3,
    SMI_DECL_IMPL_SEQUENCEOF    = 4,
    SMI_DECL_VALUEASSIGNMENT    = 5,
    SMI_DECL_OBJECTTYPE         = 6,
    SMI_DECL_OBJECTIDENTITY     = 7,
    SMI_DECL_MODULEIDENTITY     = 8,
    SMI_DECL_NOTIFICATIONTYPE   = 9,
    SMI_DECL_TRAPTYPE           = 10,
    SMI_DECL_OBJECTGROUP        = 11, 
    SMI_DECL_NOTIFICATIONGROUP  = 12,
    SMI_DECL_MODULECOMPLIANCE   = 13,
    SMI_DECL_AGENTCAPABILITIES  = 14,
    SMI_DECL_TEXTUALCONVENTION  = 15,
    /* SMIng statements */
    SMI_DECL_MODULE             = 33,
    SMI_DECL_TYPEDEF            = 34,
    SMI_DECL_NODE               = 35,
    SMI_DECL_SCALAR             = 36,
    SMI_DECL_TABLE              = 37,
    SMI_DECL_ROW                = 38,
    SMI_DECL_COLUMN             = 39,
    SMI_DECL_NOTIFICATION       = 40,
    SMI_DECL_GROUP              = 41,
    SMI_DECL_COMPLIANCE         = 42
} SmiDecl;

/* SmiIndexkind -- actual kind of a table row's index method                 */
typedef enum SmiIndexkind {
    SMI_INDEX_UNKNOWN           = 0, 
    SMI_INDEX_INDEX             = 1,
    SMI_INDEX_AUGMENT           = 2,
    SMI_INDEX_REORDER           = 3,
    SMI_INDEX_SPARSE            = 4,
    SMI_INDEX_EXPAND            = 5
} SmiIndexkind;

/* SmiValueformat -- how a value is formatted in the module                  */
typedef enum SmiValueformat {
    SMI_VALUEFORMAT_NATIVE      = 0, 
    SMI_VALUEFORMAT_BINSTRING   = 1, /* OctetString or ObjectIdentifier      */
    SMI_VALUEFORMAT_HEXSTRING   = 2, /* OctetString or ObjectIdentifier      */
    SMI_VALUEFORMAT_TEXT        = 3, /* OctetString                          */
    SMI_VALUEFORMAT_NAME        = 4, /* Enum or named ObjectIdentifier       */
    SMI_VALUEFORMAT_OID         = 5  /* ObjectIdentifier (illegal in SMIv2)  */
} SmiValueformat;

/* SmiValue -- any single value; for use in default values and subtyping     */
typedef struct SmiValue {
    SmiBasetype             basetype;
    SmiValueformat	    format;
    unsigned int	    len;         /* only for OIDs and OctetStrings   */
    union {
        SmiUnsigned64       unsigned64;
        SmiInteger64        integer64;
        SmiUnsigned32       unsigned32;
        SmiInteger32        integer32;
        SmiFloat32          float32;
        SmiFloat64          float64;
        SmiFloat128         float128;
        SmiSubid	    *oid;
        char                *ptr;	 /* OctetString, Enum, or named OID  */
        char                **bits;      /* array of BitNames                */
    } value;
} SmiValue;

/* SmiNamedNumber -- a named number; for enumeration and bitset types        */
typedef struct SmiNamedNumber {
    SmiIdentifier	typemodule;
    SmiIdentifier	typename;
    SmiIdentifier       name;
    SmiValue            value;
} SmiNamedNumber;

/* SmiRange -- a min-max value range; for subtyping of sizes or numbers      */
typedef struct SmiRange {
    SmiIdentifier	typemodule;
    SmiIdentifier	typename;
    SmiValue            minValue;
    SmiValue            maxValue;
} SmiRange;

/* SmiModule -- the main structure of a module                               */
typedef struct SmiModule {
    SmiIdentifier       name;
    SmiIdentifier       object;
    time_t              lastupdated;   /* for apps with SMIv2 semantics */
    char                *organization;
    char                *contactinfo;
    char                *description;
    char                *reference;
    SmiLanguage		language;
} SmiModule;

/* SmiRevision -- content of a single module's revision clause               */
typedef struct SmiRevision {
    SmiIdentifier       module;
    time_t              date;
    char                *description;
} SmiRevision;

/* SmiImport -- an imported descriptor                                       */
typedef struct SmiImport {
    SmiIdentifier       module;
    SmiIdentifier       importmodule;
    SmiIdentifier       importname;
} SmiImport;

/* SmiMacro -- the main structure of a SMIv1/v2 macro or SMIng extension     */
typedef struct SmiMacro {
    SmiIdentifier       module;
    SmiIdentifier       name;
} SmiMacro;

/* SmiType -- the main structure of a type definition (also base types)      */
typedef struct SmiType {
    SmiIdentifier       name;
    SmiIdentifier       module;
    SmiBasetype         basetype;
    SmiIdentifier	parentname;
    SmiIdentifier	parentmodule;
    SmiDecl             decl;
    char                *format;
    SmiValue            value;
    char                *units;
    SmiStatus           status;
    char                *description;
    char                *reference;
} SmiType;

/* SmiNode -- the main structure of any clause that defines a node           */
typedef struct SmiNode {
    SmiIdentifier       name;
    SmiIdentifier       module;
    unsigned int	oidlen;
    SmiSubid		*oid;
    SmiIdentifier       typename;
    SmiIdentifier       typemodule;
    SmiDecl             decl;
    SmiBasetype         basetype;
    SmiAccess           access;
    SmiStatus           status;
    char                *format;
    SmiValue            value;
    char                *units;
    char                *description;
    char                *reference;
    SmiIndexkind        indexkind;
    int                 implied;
    int                 create;
    SmiIdentifier       relatedname;    
    SmiIdentifier       relatedmodule;    
    SmiNodekind         nodekind;
} SmiNode;

/* SmiListItem -- an item in a list (row index column, notification object)  */
typedef struct SmiListItem {
    SmiIdentifier       name;
    SmiIdentifier       module;
    SmiIdentifier       listname;
    SmiIdentifier       listmodule;
    int			number;
} SmiListItem;

/* SmiMember -- a member of a group                                          */
typedef struct SmiMember {
    SmiIdentifier       name;
    SmiIdentifier       module;
    SmiIdentifier       groupname;
    SmiIdentifier       groupmodule;
} SmiMember;

/* SmiOption -- an optional group in a compliance statement                  */
typedef struct SmiOption {
    SmiIdentifier       name;
    SmiIdentifier       module;
    SmiIdentifier       compliancename;
    SmiIdentifier       compliancemodule;
    char                *description;
} SmiOption;

/* SmiRefinement -- a refined object in a compliance statement               */
typedef struct SmiRefinement {
    SmiIdentifier       name;
    SmiIdentifier       module;
    SmiIdentifier       compliancename;
    SmiIdentifier       compliancemodule;
    SmiIdentifier       typename;
    SmiIdentifier       typemodule;
    SmiIdentifier       writetypename;
    SmiIdentifier       writetypemodule;
    SmiAccess           access;
    char                *description;
} SmiRefinement;



extern char *smiModule(char *fullname);

extern char *smiDescriptor(char *fullname);



extern int smiInit(const char *tag);

extern void smiExit();

extern void smiSetErrorLevel(int level);

extern int smiGetFlags();

extern void smiSetFlags(int userflags);

extern char *smiGetPath();

extern int smiSetPath(const char *path);

extern void smiSetSeverity(const char *pattern, int severity);

extern int smiReadConfig(const char *filename, const char *tag);

extern char *smiLoadModule(char *module);

extern int smiModuleLoaded(char *module);



extern SmiModule *smiGetModule(char *module);

extern SmiModule *smiGetFirstModule();

extern SmiModule *smiGetNextModule(SmiModule *smiModulePtr);
      
extern void smiFreeModule(SmiModule *smiModulePtr);

extern SmiImport *smiGetFirstImport(char *module);

extern SmiImport *smiGetNextImport(SmiImport *smiImportPtr);

extern void smiFreeImport(SmiImport *smiImportPtr);

extern int smiIsImported(char *module, char *importmodule, char *importname);

extern SmiRevision *smiGetFirstRevision(char *module);

extern SmiRevision *smiGetNextRevision(SmiRevision *smiRevisionPtr);

extern void smiFreeRevision(SmiRevision *smiRevisionPtr);



extern SmiType *smiGetType(char *module, char *type);

extern SmiType *smiGetFirstType(char *module);

extern SmiType *smiGetNextType(SmiType *smiTypePtr);

extern void smiFreeType(SmiType *smiTypePtr);

extern SmiRange *smiGetFirstRange(char *module, char *type);

extern SmiRange *smiGetNextRange(SmiRange *smiRangePtr);

extern void smiFreeRange(SmiRange *smiRangePtr);

extern SmiNamedNumber *smiGetFirstNamedNumber(char *module, char *type);

extern SmiNamedNumber *smiGetNextNamedNumber(SmiNamedNumber *smiNamedNumberPtr);

extern void smiFreeNamedNumber(SmiNamedNumber *smiNamedNumberPtr);



extern SmiMacro *smiGetMacro(char *module, char *macro);

extern SmiMacro *smiGetFirstMacro(char *module);

extern SmiMacro *smiGetNextMacro(SmiMacro *smiMacroPtr);

extern void smiFreeMacro(SmiMacro *smiMacroPtr);



extern SmiNode *smiGetNode(char *module, char *name);

extern SmiNode *smiGetNodeByOID(unsigned int oidlen, SmiSubid oid[]);

extern SmiNode *smiGetFirstNode(char *module, SmiNodekind nodekind);

extern SmiNode *smiGetNextNode(SmiNode *smiNodePtr, SmiNodekind nodekind);

extern SmiNode *smiGetParentNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetFirstChildNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetNextChildNode(SmiNode *smiNodePtr);

extern void smiFreeNode(SmiNode *smiNodePtr);




extern SmiListItem *smiGetFirstListItem(SmiNode *smiListNodePtr);

extern SmiListItem *smiGetNextListItem(SmiListItem *smiListItemPtr);

extern void smiFreeListItem(SmiListItem *smiListItemPtr);




extern SmiOption *smiGetFirstOption(SmiNode *smiComplianceNodePtr);

extern SmiOption *smiGetNextOption(SmiOption *smiOptionPtr);

extern void smiFreeOption(SmiOption *smiOptionPtr);



extern SmiRefinement *smiGetFirstRefinement(SmiNode *smiComplianceNodePtr);

extern SmiRefinement *smiGetNextRefinement(SmiRefinement *smiRefinementPtr);

extern void smiFreeRefinement(SmiRefinement *smiRefinementPtr);



/* extern char **smiGetNames(char *spec, char *mod); */



#endif /* _SMI_H */
