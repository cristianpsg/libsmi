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
 * @(#) $Id: smi.h,v 1.39 1999/06/16 15:04:41 strauss Exp $
 */

#ifndef _SMI_H
#define _SMI_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



#define SMI_VIEWALL     0x0800  /* all modules are `known', need no views.   */
#define SMI_ERRORS      0x1000  /* print parser errors.                      */
#define SMI_STATS       0x4000  /* print statistics after parsing a module.  */
#define SMI_RECURSIVE   0x8000  /* recursively parse imported modules.       */
#define SMI_FLAGMASK    (SMI_VIEWALL|SMI_STATS|SMI_RECURSIVE|SMI_ERRORS)



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
    SMI_BASETYPE_BITS                   = 11, /* only SMIv2 and SMIng        */
    SMI_BASETYPE_HEXSTRING              = 12, /* only for values             */
    SMI_BASETYPE_BINSTRING              = 13, /* only for values, SMIv1/v2   */
    SMI_BASETYPE_LABEL                  = 14, /* labels as values            */
    SMI_BASETYPE_CHOICE                 = 15, /* only for parsing SMI specs  */
    SMI_BASETYPE_SEQUENCE               = 16, /* only for parsing SMI specs  */
    SMI_BASETYPE_SEQUENCEOF             = 17  /* only for parsing SMI specs  */
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
    SMI_ACCESS_NOT_ACCESSIBLE   = 1, /* the values 1 to 5 are allow to be    */
    SMI_ACCESS_NOTIFY           = 2, /* compared by relational operators.    */
    SMI_ACCESS_READ_ONLY        = 3,
    SMI_ACCESS_READ_WRITE       = 4,
    SMI_ACCESS_READ_CREATE      = 5,
    SMI_ACCESS_WRITE_ONLY       = 6
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
    SMI_DECL_TYPEASSIGNMENT     = 1,
    SMI_DECL_VALUEASSIGNMENT    = 2,
    SMI_DECL_OBJECTTYPE         = 3,
    SMI_DECL_OBJECTIDENTITY     = 4,
    SMI_DECL_MODULEIDENTITY     = 5,
    SMI_DECL_NOTIFICATIONTYPE   = 6,
    SMI_DECL_TRAPTYPE           = 7,
    SMI_DECL_OBJECTGROUP        = 8, 
    SMI_DECL_NOTIFICATIONGROUP  = 9,
    SMI_DECL_MODULECOMPLIANCE   = 10,
    SMI_DECL_AGENTCAPABILITIES  = 11,
    SMI_DECL_TEXTUALCONVENTION  = 12,
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

/* SmiValue -- any single value; for use in default values and subtyping     */
typedef struct SmiValue {
    SmiBasetype         basetype;
    union {
        SmiUnsigned64       unsigned64;
        SmiInteger64        integer64;
        SmiUnsigned32       unsigned32;
        SmiInteger32        integer32;
        SmiFloat32          float32;
        SmiFloat64          float64;
        SmiFloat128         float128;
	unsigned int	    oidlen;
        SmiSubid	    *oid;
        char                *ptr;
        char                **bits;      /* array of SmiNamedNumber pointers */
    } value;
} SmiValue;

/* SmiNamedNumber -- a named number; for enumeration and bitset types        */
typedef struct SmiNamedNumber {
    SmiIdentifier	module;
    SmiIdentifier	type;
    SmiIdentifier       name;
    SmiValue            *valuePtr;
} SmiNamedNumber;

/* SmiRange -- a min-max value range; for subtyping of sizes or numbers      */
typedef struct SmiRange {
    SmiIdentifier	module;
    SmiIdentifier	type;
    SmiValue            *minValuePtr;
    SmiValue            *maxValuePtr;
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
    SmiIdentifier       module;
    SmiIdentifier       name;
    SmiBasetype         basetype;
    SmiIdentifier	parentmodule;
    SmiIdentifier	parentname;
    SmiDecl             decl;
    char                *format;
    SmiValue            *valuePtr;
    char                *units;
    SmiStatus           status;
    char                *description;
    char                *reference;
} SmiType;

/* SmiNode -- the main structure of any clause that defines a node           */
typedef struct SmiNode {
    SmiIdentifier       module;
    SmiIdentifier       name;
    unsigned int	oidlen;
    SmiSubid		*oid;
    SmiIdentifier       typemodule;
    SmiIdentifier       typename;
    SmiIndexkind        indexkind;
    int                 implied;
    int                 create;
    SmiIdentifier       relatedmodule;    
    SmiIdentifier       relatedname;    
    SmiNodekind         nodekind;
    SmiDecl             decl;
    SmiBasetype         basetype;
    SmiAccess           access;
    SmiStatus           status;
    char                *format;
    SmiValue            *valuePtr;
    char                *units;
    char                *description;
    char                *reference;
} SmiNode;

/* SmiIndex -- a table row index column				             */
typedef struct SmiIndex {
    SmiIdentifier       module;
    SmiIdentifier       name;
    SmiIdentifier       rowmodule;
    SmiIdentifier       rowname;
    int			number;
} SmiIndex;

/* SmiMember -- a member of a group                                          */
typedef struct SmiMember {
    SmiIdentifier       module;
    SmiIdentifier       name;
    SmiIdentifier       groupmodule;
    SmiIdentifier       groupname;
} SmiMember;

/* SmiOption -- an optional group in a compliance statement                  */
typedef struct SmiOption {
    SmiIdentifier       module;
    SmiIdentifier       name;
    SmiIdentifier       compliancemodule;
    SmiIdentifier       compliancename;
    char                *description;
} SmiOption;

/* SmiRefinement -- a refined object in a compliance statement               */
typedef struct SmiRefinement {
    SmiIdentifier       module;
    SmiIdentifier       name;
    SmiIdentifier       compliancemodule;
    SmiIdentifier       compliancename;
    SmiIdentifier       typemodule;
    SmiIdentifier       typename;
    SmiIdentifier       writetypemodule;
    SmiIdentifier       writetypename;
    SmiAccess           access;
    char                *description;
} SmiRefinement;



extern char *smiModule(char *fullname);

extern char *smiDescriptor(char *fullname);



extern int smiInit();

extern void smiSetErrorLevel(int level);

extern int smiGetFlags();

extern void smiSetFlags(int userflags);

extern char *smiGetPath();

extern int smiSetPath(const char *path);

extern char *smiLoadModule(char *module);



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

extern SmiNode *smiGetFirstMemberNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetNextMemberNode(SmiNode *smiGroupNodePtr,
				     SmiNode *smiMemberNodePtr);

extern SmiNode *smiGetFirstObjectNode(SmiNode *smiNotificationNodePtr);

extern SmiNode *smiGetNextObjectNode(SmiNode *smiNotificationNodePtr,
				     SmiNode *smiObjectNodePtr);

extern SmiNode *smiGetFirstMandatoryNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetNextMandatoryNode(SmiNode *smiComplianceNodePtr,
					SmiNode *smiMandatoryNodePtr);

extern void smiFreeNode(SmiNode *smiNodePtr);




extern SmiIndex *smiGetFirstIndex(SmiNode *smiRowNodePtr);

extern SmiIndex *smiGetNextIndex(SmiIndex *smiIndexPtr);

extern void smiFreeIndex(SmiIndex *smiIndexPtr);




extern SmiOption *smiGetFirstOption(SmiNode *smiComplianceNodePtr);

extern SmiOption *smiGetNextOption(SmiOption *smiOptionPtr);

extern void smiFreeOption(SmiOption *smiOptionPtr);



extern SmiRefinement *smiGetFirstRefinement(SmiNode *smiComplianceNodePtr);

extern SmiRefinement *smiGetNextRefinement(SmiRefinement *smiRefinementPtr);

extern void smiFreeRefinement(SmiRefinement *smiRefinementPtr);



/* extern char **smiGetNames(char *spec, char *mod); */



#endif /* _SMI_H */
