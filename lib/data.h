/*
 * data.h --
 *
 *      Definitions for the main data structures.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: data.h,v 1.8 1999/03/23 22:55:39 strauss Exp $
 */

#ifndef _DATA_H
#define _DATA_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "smi.h"
#include "defs.h"
#include "parser-smi.h"





typedef struct List {
    void	    *ptr;
    struct List	    *nextPtr;
} List;



typedef struct NamedNumber {
    char	    *name;
    smi_number	    number;
} NamedNumber;



typedef struct Range {
    smi_number	    min;
    smi_number	    max;
} Range;



typedef enum Kind {
    KIND_UNKNOWN	 = 0 ,  /*					     */
    KIND_MODULE		 = 1 ,  /*					     */
    KIND_MACRO		 = 2 ,  /*					     */
    KIND_TYPE		 = 3 ,  /*					     */
    KIND_OBJECT		 = 4 ,  /*					     */
    KIND_IMPORT		 = 5 ,  /* descriptors to be imported.               */
    KIND_IMPORTED	 = 6    /* imported descriptor. syntax `mod.descr'.  */
} Kind;
#define NUM_KINDS  7



typedef unsigned short ParserFlags;
typedef unsigned short ModuleFlags;
typedef unsigned short ObjectFlags;
typedef unsigned short NodeFlags;
typedef unsigned short TypeFlags;
typedef unsigned short MacroFlags;

#define NODE_FLAG_ROOT		0x0001 /* mark node tree's root */

#define FLAG_IMPORTED		0x0002 /*				     */
#define FLAG_REGISTERED		0x0004 /* On an Object: this is registered.  */
#define FLAG_INCOMPLETE		0x0008 /* Just defined by a forward          */
				       /* referenced type or object.         */
#define	FLAG_SMIV2	        0x0010 /* On a Module: This is an SMIv2 MIB. */
#define	FLAG_SMING	        0x0020 /* On a Module: This is an SMIng MIB. */

#define	FLAG_ERRORS	SMI_ERRORS     /* Otherwise be quiet,       */
				       /* useful when IMPORTing.    */
#define FLAG_STATS	SMI_STATS      /* Print module statistics.  */
#define FLAG_RECURSIVE	SMI_RECURSIVE  /* Errors and stats also for */
				       /* imported modules.         */
				       /* useful when IMPORTing.    */
#define	FLAG_ERRORLINES	SMI_ERRORLINES /* When printing errors:     */
				       /* print also the wrong line */
#define	FLAG_VIEWALL	SMI_VIEWALL    /* smiFunctions return all   */
				       /* labels they know, not     */
				       /* only the ones in the      */
				       /* current view.             */


typedef enum LocationType {
#ifdef BACKEND_SMI
    LOCATION_SMIFILE	     = 1,
    LOCATION_SMIDIR	     = 2,
#endif
#ifdef BACKEND_SMING
    LOCATION_SMINGFILE	     = 3,
    LOCATION_SMINGDIR	     = 4,
#endif
#ifdef BACKEND_DBM
    LOCATION_DBM	     = 5,
#endif
#ifdef BACKEND_RPC
    LOCATION_RPC	     = 6,
#endif
    LOCATION_UNKNOWN         = 0
} LocationType;

typedef struct Location {
    char	    *name;
    LocationType    type;
#ifdef BACKEND_RPC
    CLIENT	    *cl;
#endif
    struct Location *nextPtr;
    struct Location *prevPtr;
} Location;


typedef struct View {
    char	    *name;
    struct View	    *nextPtr;
    struct View	    *prevPtr;
} View;



typedef struct Module {
    smi_descriptor name;
    char	    *path;
    Location	    *locationPtr;
    off_t	    fileoffset;
    struct Object   *firstObjectPtr;
    struct Object   *lastObjectPtr;
    struct Type	    *firstTypePtr;
    struct Type	    *lastTypePtr;
    struct Macro    *firstMacroPtr;
    struct Macro    *lastMacroPtr;
    struct Import   *firstImportPtr;
    struct Import   *lastImportPtr;
    time_t	    lastUpdated;
    char	    *organization;
    char	    *contactInfo;
    struct Object   *objectPtr;
    struct Revision *firstRevisionPtr;
    struct Revision *lastRevisionPtr;
    ModuleFlags	    flags;
    int		    numImportedIdentifiers;
    int		    numStatements;
    int		    numModuleIdentities;
    struct Module   *nextPtr;
    struct Module   *prevPtr;
} Module;



typedef struct Import {
    smi_descriptor module;
    smi_descriptor name;
    struct Import  *nextPtr;
    struct Import  *prevPtr;
    Kind	   kind;
} Import;



typedef struct Revision {
    time_t          date;
    char	    *description;
    struct Revision *nextPtr;
    struct Revision *prevPtr;
} Revision;



typedef struct Type {
    Module         *modulePtr;
    smi_descriptor name;
    smi_fullname   parentType;
    smi_syntax	   syntax;
    smi_decl	   decl;
    char	   *format;
    char	   *units;
    smi_status	   status;
    struct List	   *itemlistPtr;
    char	   *description;
    char	   *reference;
    off_t          fileoffset;
    TypeFlags	   flags;
    struct Type    *nextPtr;
    struct Type    *prevPtr;
} Type;



typedef struct Object {
    Module         *modulePtr;
    smi_descriptor name;
    off_t	   fileoffset;
    smi_decl	   decl;
    ObjectFlags	   flags;
    Type	   *typePtr;
    smi_access	   access;
    smi_status	   status;
    struct List	   *indexPtr;
    char	   *description;
    char	   *reference;
    char	   *units;
    struct Node	   *nodePtr;
    struct Object  *prevPtr;		/* chain of Objects in this Module */
    struct Object  *nextPtr;
    struct Object  *prevSameNodePtr;    /* chain of Objects for this Node  */
    struct Object  *nextSameNodePtr;
} Object;



typedef struct Node {
    smi_subid	   subid;
    NodeFlags	   flags;
    struct Node	   *parentPtr;
    struct Node	   *nextPtr;
    struct Node	   *prevPtr;
    struct Node	   *firstChildPtr;
    struct Node	   *lastChildPtr;
    Object	   *firstObjectPtr;
    Object	   *lastObjectPtr;
} Node;



typedef struct Macro {
    Module	   *modulePtr;
    smi_descriptor name;
    off_t	   fileoffset;
    MacroFlags	   flags;
    struct Macro   *nextPtr;
    struct Macro   *prevPtr;
} Macro;



typedef struct Parser {
    char	   *path;
    Location	   *locationPtr;
    FILE	   *file;
    int		   line;
    int		   column;
    int		   character;
    char	   linebuf[MAX_LINEBUF_LENGTH+1];
    Module	   *modulePtr;
    ParserFlags	   flags;
} Parser;



extern int	smiFlags;

extern Node	*rootNodePtr;
extern Node	*pendingNodePtr;

extern Type	*typeIntegerPtr, *typeOctetStringPtr, *typeObjectIdentifierPtr;

extern Location	*firstLocationPtr, *lastLocationPtr;

extern View	*firstViewPtr, *lastViewPtr;



extern View *addView(const char *modulename);



extern Location *addLocation(const char *location,
			     ModuleFlags flags);



extern Module *addModule(const char *modulename,
			 const char *path,
			 Location *locationPtr,
			 off_t fileoffset,
			 ModuleFlags flags,
			 Parser *parserPtr);

extern void setModuleIdentityObject(Module *modulePtr,
				    Object *objectPtr);

extern void setModuleLastUpdated(Module *modulePtr,
				 time_t lastUpdated);

extern void setModuleOrganization(Module *modulePtr,
				  char *organization);

extern void setModuleContactInfo(Module *modulePtr,
				 char *contactInfo);

extern Module *findModuleByName(const char *modulename);



extern Revision *addRevision(time_t date,
			     const char *description,
			     Parser *parserPtr);



extern Import *addImport(const char *name,
			 Parser *parserPtr);

extern int checkImports(char *modulename,
			Parser *parserPtr);

extern Import *findImportByName(const char *importname,
				Module *modulePtr);

extern Import *findImportByModulenameAndName(const char *modulename,
					     const char *importname,
					     Module *modulePtr);

extern Object *addObject(const char *objectname,
			 Node *parentNodePtr,
			 smi_subid subid,
			 ObjectFlags flags,
			 Parser *parserPtr);

extern Object *duplicateObject(Object *templatePtr,
			       ObjectFlags flags,
			       Parser *parserPtr);

extern Node *addNode(Node *parentNodePtr,
		     smi_subid subid,
		     NodeFlags flags,
		     Parser *parserPtr);

extern Node *createNodes(const char *oid);

extern Node *getParentNode(Node *nodePtr);

extern smi_subid getLastSubid(const char *oid);

/*
 * setObjectName() might relink MIB tree object structures. If the
 * current objectPtr is to be used after the call, it should look like
 *   objectPtr = setObjectName(objectPtr, name);
 */
extern Object *setObjectName(Object *objectPtr,
			     smi_descriptor name);

extern void setObjectType(Object *objectPtr,
			  Type *typePtr);

extern void setObjectAccess(Object *objectPtr,
			    smi_access access);

extern void setObjectStatus(Object *objectPtr,
			    smi_status status);

extern void setObjectDescription(Object *objectPtr,
				 char *description);

extern void setObjectReference(Object *objectPtr,
				 char *reference);

extern void setObjectFileOffset(Object *objectPtr,
				off_t fileoffset);

extern void setObjectDecl(Object *objectPtr,
			   smi_decl decl);

extern void addObjectFlags(Object *objectPtr,
			   ObjectFlags flags);

extern void setObjectIndex(Object *objectPtr,
			   List *listPtr);

extern Node *findNodeByParentAndSubid(Node *parentNodePtr,
				      smi_subid subid);

extern Object *findObjectByNode(Node *nodePtr);

extern Object *findObjectByModuleAndNode(Module *modulePtr,
					 Node *nodePtr);

extern Object *findObjectByModulenameAndNode(const char *modulename,
					     Node *nodePtr);

extern Object *findObjectByName(const char *objectname);

extern Object *findObjectByModuleAndName(Module *modulePtr,
					 const char *objectname);

extern Object *findObjectByModulenameAndName(const char *modulename,
					     const char *objectname);



extern Type *addType(const char *typename,
		     smi_syntax syntax,
		     TypeFlags flags,
		     Parser *parserPtr);

extern Type *duplicateType(Type *templatePtr,
			   TypeFlags flags,
			   Parser *parserPtr);

extern void setTypeName(Type *typePtr,
			smi_descriptor name);

extern void setTypeStatus(Type *typePtr,
			  smi_status status);

extern void setTypeParent(Type *typePtr,
			  const char *parent);

extern void setTypeItemlistPtr(Type *typePtr,
			       struct List *itemlistPtr);

extern void setTypeDescription(Type *typePtr,
			       char *description);

extern void setTypeFileOffset(Type *typePtr,
			      off_t fileoffset);

extern void setTypeDecl(Type *typePtr,
			smi_decl decl);

extern void setTypeFlags(Type *typePtr,
			 TypeFlags flags);

extern void setTypeFormat(Type *typePtr,
			  char *format);



extern Type *findTypeByName(const char *typename);

extern Type *findTypeByModuleAndName(Module *modulePtr,
				     const char *typename);

extern Type *findTypeByModulenameAndName(const char *modulename,
					 const char *typename);



extern Macro *addMacro(const char *macroname,
		       off_t fileoffset,
		       int flags,
		       Parser *parserPtr);

extern Macro *findMacroByModuleAndName(Module *modulePtr,
				       const char *macroname);

extern Macro *findMacroByModulenameAndName(const char *modulename,
					   const char *macroname);



extern int initData();

extern Module *loadModule(char *modulename);



#endif /* _DATA_H */
