/*
 * smi.h --
 *
 *      Interface Definition of libsmi (version 2:1:0).
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi.h.in,v 1.8 2000/02/11 16:42:32 strauss Exp $
 */

#ifndef _SMI_H
#define _SMI_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



#define SMI_LIBRARY_VERSION "2:1:0"
extern const char *smi_library_version;



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
    SMI_STATUS_OBSOLETE         = 5  /* SMIv1, SMIv2 and SMIng               */
} SmiStatus;

/* SmiAccess -- values of access levels                                      */
typedef enum SmiAccess {
    SMI_ACCESS_UNKNOWN          = 0, /* should not occur                     */
    SMI_ACCESS_NOT_IMPLEMENTED  = 1, /* only for agent capability variations */
    SMI_ACCESS_NOT_ACCESSIBLE   = 2, /* the values 2 to 5 are allowed to be  */
    SMI_ACCESS_NOTIFY           = 3, /* compared by relational operators.    */
    SMI_ACCESS_READ_ONLY        = 4,
    SMI_ACCESS_READ_WRITE       = 5
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
    SMI_DECL_IMPL_SEQUENCEOF    = 4,	/* this will go away */
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
    SMI_DECL_EXTENSION          = 34,
    SMI_DECL_TYPEDEF            = 35,
    SMI_DECL_NODE               = 36,
    SMI_DECL_SCALAR             = 37,
    SMI_DECL_TABLE              = 38,
    SMI_DECL_ROW                = 39,
    SMI_DECL_COLUMN             = 40,
    SMI_DECL_NOTIFICATION       = 41,
    SMI_DECL_GROUP              = 42,
    SMI_DECL_COMPLIANCE         = 43
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
    SmiIdentifier       name;
    SmiValue            value;
} SmiNamedNumber;

/* SmiRange -- a min-max value range; for subtyping of sizes or numbers      */
typedef struct SmiRange {
    SmiValue            minValue;
    SmiValue            maxValue;
} SmiRange;

/* SmiModule -- the main structure of a module                               */
typedef struct SmiModule {
    SmiIdentifier       name;
    char                *path;
    char                *organization;
    char                *contactinfo;
    char                *description;
    char                *reference;
    SmiLanguage		language;
} SmiModule;

/* SmiRevision -- content of a single module's revision clause               */
typedef struct SmiRevision {
    time_t              date;
    char                *description;
} SmiRevision;

/* SmiImport -- an imported descriptor                                       */
typedef struct SmiImport {
    SmiIdentifier       module;
    SmiIdentifier       name;
} SmiImport;

/* SmiMacro -- the main structure of a SMIv1/v2 macro or SMIng extension     */
typedef struct SmiMacro {
    SmiIdentifier       name;
    SmiDecl             decl;
    SmiStatus           status;
    char                *description;
    char                *reference;
} SmiMacro;

/* SmiType -- the main structure of a type definition (also base types)      */
typedef struct SmiType {
    SmiIdentifier       name;
    SmiBasetype         basetype;
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
    int			oidlen;
    SmiSubid		*oid;         /* array of length oidlen */
    SmiDecl             decl;
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
    SmiNodekind         nodekind;
} SmiNode;

/* SmiElement -- an item in a list (row index column, notification object)   */
typedef struct SmiElement {
    /* no visible attributes */
} SmiElement;

/* SmiOption -- an optional group in a compliance statement                  */
typedef struct SmiOption {
    char                *description;
} SmiOption;

/* SmiRefinement -- a refined object in a compliance statement               */
typedef struct SmiRefinement {
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

extern void smiSetSeverity(char *pattern, int severity);

extern int smiReadConfig(const char *filename, const char *tag);

extern char *smiLoadModule(char *module);

extern int smiIsLoaded(char *module);



extern SmiModule *smiGetModule(char *module);

extern SmiModule *smiGetFirstModule();

extern SmiModule *smiGetNextModule(SmiModule *smiModulePtr);
      
extern SmiNode *smiGetModuleIdentityNode(SmiModule *smiModulePtr);

extern SmiImport *smiGetFirstImport(SmiModule *smiModulePtr);

extern SmiImport *smiGetNextImport(SmiImport *smiImportPtr);

extern int smiIsImported(SmiModule *smiModulePtr,
			 SmiModule *importedModulePtr, char *importedName);

extern SmiRevision *smiGetFirstRevision(SmiModule *smiModulePtr);

extern SmiRevision *smiGetNextRevision(SmiRevision *smiRevisionPtr);



extern SmiType *smiGetType(SmiModule *smiModulePtr, char *type);

extern SmiType *smiGetFirstType(SmiModule *smiModulePtr);

extern SmiType *smiGetNextType(SmiType *smiTypePtr);

extern SmiType *smiGetParentType(SmiType *smiTypePtr);

extern SmiModule *smiGetTypeModule(SmiType *smiTypePtr);

extern SmiRange *smiGetFirstRange(SmiType *smiTypePtr);

extern SmiRange *smiGetNextRange(SmiRange *smiRangePtr);

extern SmiNamedNumber *smiGetFirstNamedNumber(SmiType *smiTypePtr);

extern SmiNamedNumber *smiGetNextNamedNumber(SmiNamedNumber
					         *smiNamedNumberPtr);


extern SmiMacro *smiGetMacro(SmiModule *smiModulePtr, char *macro);

extern SmiMacro *smiGetFirstMacro(SmiModule *smiModulePtr);

extern SmiMacro *smiGetNextMacro(SmiMacro *smiMacroPtr);

extern SmiModule *smiGetMacroModule(SmiMacro *smiMacroPtr);



extern SmiNode *smiGetNode(SmiModule *smiModulePtr, char *name);

extern SmiNode *smiGetNodeByOID(unsigned int oidlen, SmiSubid oid[]);

extern SmiNode *smiGetFirstNode(SmiModule *smiModulePtr, SmiNodekind nodekind);

extern SmiNode *smiGetNextNode(SmiNode *smiNodePtr, SmiNodekind nodekind);

extern SmiNode *smiGetParentNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetRelatedNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetFirstChildNode(SmiNode *smiNodePtr);

extern SmiNode *smiGetNextChildNode(SmiNode *smiNodePtr);

extern SmiModule *smiGetNodeModule(SmiNode *smiNodePtr);

extern SmiType *smiGetNodeType(SmiNode *smiNodePtr);




extern SmiElement *smiGetFirstElement(SmiNode *smiNodePtr);

extern SmiElement *smiGetNextElement(SmiElement *smiElementPtr);

extern SmiNode *smiGetElementNode(SmiElement *smiElementPtr);



extern SmiOption *smiGetFirstOption(SmiNode *smiComplianceNodePtr);

extern SmiOption *smiGetNextOption(SmiOption *smiOptionPtr);

extern SmiNode *smiGetOptionNode(SmiOption *smiOptionPtr);



extern SmiRefinement *smiGetFirstRefinement(SmiNode *smiComplianceNodePtr);

extern SmiRefinement *smiGetNextRefinement(SmiRefinement *smiRefinementPtr);

extern SmiModule *smiGetRefinementModule(SmiRefinement *smiRefinementPtr);

extern SmiNode *smiGetRefinementNode(SmiRefinement *smiRefinementPtr);

extern SmiType *smiGetRefinementType(SmiRefinement *smiRefinementPtr);

extern SmiType *smiGetRefinementWriteType(SmiRefinement *smiRefinementPtr);



#endif /* _SMI_H */
