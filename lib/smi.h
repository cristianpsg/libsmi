/*
 * smi.h --
 *
 *      Interface Implementation of libsmi.
 *
 * Copyright (c) 1999 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smi.h,v 1.13 1999/05/04 17:04:59 strauss Exp $
 */

#ifndef _SMI_H
#define _SMI_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



#define SMI_CONFIG_FILE         "/usr/local/etc/smi.conf"

#define SMI_NAMESPACE_OPERATOR  "::"

#define SMI_VIEWALL     0x0800  /* all modules are `known', need no views.   */
#define SMI_ERRORS      0x1000  /* print parser errors.                      */
#define SMI_ERRORLINES  0x2000  /* print error line contents of modules.     */
#define SMI_STATS       0x4000  /* print statistics after parsing a module.  */
#define SMI_RECURSIVE   0x8000  /* recursively parse imported modules.       */
#define SMI_FLAGMASK \
                (SMI_VIEWALL|SMI_STATS|SMI_RECURSIVE|SMI_ERRORS|SMI_ERRORLINES)

/* limits of string lengths                                                  */
#define SMI_MAX_DESCRIPTOR      64             /*                            */
#define SMI_MAX_OID             1407           /* 128 * 10 digits + 127 dots */
#define SMI_MAX_STRING          65535          /* limit only valid in v1/v2  */
#define SMI_MAX_FULLNAME        130            /* 64 + 2 separator + 64      */



/* misc mappings of SMI types to C types                                     */
typedef char                    *SmiQIdentifier;
typedef char                    *SmiIdentifier;
typedef unsigned long           SmiUnsigned32;
typedef long                    SmiInteger32;
typedef unsigned long long      SmiUnsigned64;
typedef long long               SmiInteger64;
typedef char                    *SmiObjectIdentifier;
typedef unsigned int            SmiSubid;
typedef float                   SmiFloat32;
typedef double                  SmiFloat64;
typedef double                  SmiFloat128;   /* currently, no 64/128 diffs */



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
    SMI_ACCESS_NOT_ACCESSIBLE   = 1,
    SMI_ACCESS_NOTIFY           = 2,
    SMI_ACCESS_READ_ONLY        = 3,
    SMI_ACCESS_READ_WRITE       = 4,
    SMI_ACCESS_READ_CREATE      = 5,
    SMI_ACCESS_WRITE_ONLY       = 6
} SmiAccess;

/* SmiDecl -- type or statement that leads to a definition                   */
typedef enum SmiDecl {
    SMI_DECL_UNKNOWN            = 0,  /* shout not occur                     */
    /* SMIv1/V2 ASN.1 statements and macros */
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

/* SmiRevision -- content of a single module's revision clause               */
typedef struct SmiRevision {
    time_t              date;
    char                *description;
} SmiRevision;

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
        SmiObjectIdentifier oid;
        char                *ptr;
        char                **bits;      /* array of SmiNamedNumber pointers */
    } value;
} SmiValue;

/* SmiNamedNumber -- a named number; for enumeration and bitset types        */
typedef struct SmiNamedNumber {
    SmiIdentifier       name;
    SmiValue            *valuePtr;
} SmiNamedNumber;

/* SmiRange -- a min-max value range; for subtyping of sizes or numbers      */
typedef struct SmiRange {
    SmiValue            *minValuePtr;
    SmiValue            *maxValuePtr;
} SmiRange;

/* SmiOption -- an optional group in a compliance statement                  */
typedef struct SmiOption {
    SmiIdentifier       name;
    SmiIdentifier       module;
    char                *description;
} SmiOption;

/* SmiRefinement -- a refined object in a compliance statement               */
typedef struct SmiRefinement {
    SmiIdentifier       name;
    SmiIdentifier       module;
    char                *description;
    SmiQIdentifier      type;
    SmiQIdentifier      writetype;
    SmiAccess           access;
} SmiRefinement;

/* SmiModule -- the main structure of a module                               */
typedef struct SmiModule {
    SmiIdentifier       name;
    SmiQIdentifier      object;
    time_t              lastupdated;   /* for apps with SMIv2 semantics */
    char                *organization;
    char                *contactinfo;
    char                *description;
    char                *reference;
} SmiModule;

/* SmiNode -- the main structure of any clause that defines a node           */
typedef struct SmiNode {
    char                *name;
    char                *module;
    SmiObjectIdentifier oid;
    char                *type;
    char                **list;
    SmiIndexkind        indexkind;
    int                 implied;
    char                **index;
    char                *relatedrow;
    SmiOption           **option;
    SmiRefinement       **refinement;
    SmiDecl             decl;
    SmiBasetype         basetype;
    SmiAccess           access;
    SmiStatus           status;
    char                *format;
    SmiValue            *value;
    char                *units;
    char                *description;
    char                *reference;
} SmiNode;

/* SmiType -- the main structure of a type definition (also base types)      */
typedef struct SmiType {
    char                *name;
    char                *module;
    SmiBasetype         basetype;
    char                *parent;
    void                **list;   /* SmiNamedNumber or SmiRange */
    SmiDecl             decl;
    char                *format;
    SmiValue            *value;
    char                *units;
    SmiStatus           status;
    char                *description;
    char                *reference;
} SmiType;

/* SmiMacro -- the main structure of a SMIv1/v2 macro or SMIng extension     */
typedef struct SmiMacro {
    char                *name;
    char                *module;
} SmiMacro;



extern char *smingStringStatus(SmiStatus status);

extern char *smiStringStatus(SmiStatus status);

extern char *smingStringAccess(SmiAccess access);

extern char *smiStringAccess(SmiAccess access);

extern char *smiStringDecl(SmiDecl macro);

extern char *smiStringBasetype(SmiBasetype basetype);

extern char *smiModule(char *fullname);

extern char *smiDescriptor(char *fullname);

extern time_t smiMkTime(const char *s);

extern char *smiCTime(time_t t);

extern char *smingCTime(time_t t);



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

extern char **smiGetImports(char *spec,
                            char *mod);

extern int smiIsImported(char *modulename,
                         char *fullname);

extern SmiRevision *smiGetFirstRevision(char *modulename);

extern SmiRevision *smiGetNextRevision(char *modulename,
                                       time_t date);

extern void smiFreeRevision(SmiRevision *smiRevisionPtr);



extern SmiType *smiGetType(char *spec,
                           char *mod);

extern SmiType *smiGetFirstType(char *modulename);

extern SmiType *smiGetNextType(char *modulename,
                               char *name);

extern void smiFreeType(SmiType *smiTypePtr);



extern SmiMacro *smiGetMacro(char *spec,
                             char *mod);

extern void smiFreeMacro(SmiMacro *smiMacroPtr);



extern SmiNode *smiGetNode(char *spec,
                           char *mod);

extern SmiNode *smiGetFirstNode(char *modulename);

extern SmiNode *smiGetNextNode(char *modulename,
                               char *name);

extern void smiFreeNode(SmiNode *smiNodePtr);



extern char **smiGetNames(char *spec,
                          char *mod);

extern char *smiGetParent(char *spec,
                          char *mod);

extern char **smiGetChildren(char *spec,
                             char *mod);



#endif /* _SMI_H */
