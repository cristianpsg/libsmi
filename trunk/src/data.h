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
 * @(#) $Id: data.h,v 1.27 1999/02/18 17:12:59 strauss Exp $
 */

#ifndef _DATA_H
#define _DATA_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "smi.h"
#include "defs.h"
#include "error.h"
#include "parser.h"





typedef struct List {
    void	    *ptr;
    struct List	    *nextPtr;
} List;



typedef struct String {
#ifdef TEXTS_IN_MEMORY
    char *ptr;	      			/*   the value			    */
#endif
    int fileoffset;			/*   offset in this file	    */
    int length;				/*   full length		    */
} String;



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

#define NODE_FLAG_ROOT		0x0001

#define PARSER_FLAG_STATS	SMI_STATS

#define FLAG_PERMANENT		0x0001 /* e.g. Object and Descriptor `iso'.  */
#define FLAG_IMPORTED		0x0002 /*				     */
#define FLAG_PARENTIMPORTED	0x0004 /* On a Type: This Type's parent is   */
				       /* imported instead of local, hence   */
				       /* its parent is a pointer to a       */
				       /* descriptor instead of a type.      */
#define FLAG_MODULE		0x0008 /* Declared in the current module.    */
#define FLAG_REGISTERED		0x0010 /* On an Object: this is registered.  */
#define FLAG_INCOMPLETE		0x0020 /* Just defined by a forward          */
				       /* referenced type or object.         */

#define	FLAG_TC                 0x0100 /* On a Type: This type is declared   */
				       /* by a TC instead of a simple ASN.1  */
				       /* type declaration.                  */

#define	FLAG_SMIV2	        0x0100 /* On a Module: This is an SMIv2 MIB. */

#define FLAG_ROOT	        0x0200 /* Marks the single root Node.        */

#define	FLAG_WHOLEFILE		0x0100 /* We want to read the whole */
				       /* file, not just a single   */
				       /* module.		    */
#define FLAG_ACTIVE		0x0200 /* We want to read something */
				       /* from the current module.  */
#define FLAG_WHOLEMOD		0x0400 /* Fetch all items instead   */
				       /* of just idlist (IMPORTS). */
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

#define FLAGS_GENERAL		0x00ff
#define FLAGS_SPECIFIC		0xff00


typedef struct Module {
    smi_descriptor name;
    char	   *path;
    off_t	   fileoffset;
    struct Object  *firstObjectPtr;
    struct Object  *lastObjectPtr;
    struct Type	   *firstTypePtr;
    struct Type	   *lastTypePtr;
    struct Macro   *firstMacroPtr;
    struct Macro   *lastMacroPtr;
    struct Import  *firstImportPtr;
    struct Import  *lastImportPtr;
    String	   lastUpdated;
    String	   organization;
    String	   contactInfo;
    struct Object  *objectPtr;
#if 0
    Revision       *firstRevisionPtr;
    Revision       *lastRevisionPtr;
#endif
    ModuleFlags	   flags;
    int		   numImportedIdentifiers;
    int		   numStatements;
    int		   numModuleIdentities;
    struct Module  *nextPtr;
    struct Module  *prevPtr;
} Module;



typedef struct Import {
    smi_descriptor module;
    smi_descriptor name;
    struct Import  *nextPtr;
    struct Import  *prevPtr;
    Kind	   kind;
} Import;



typedef struct Type {
    Module         *modulePtr;
    smi_descriptor name;
    smi_fullname   parentType;
    smi_syntax	   syntax;
    smi_decl	   decl;
    String	   format;
    smi_status	   status;
    struct List	   *sequencePtr;
    String	   description;
    String	   reference;
#if 0
    Restriction	   *firstRestriction;
    Restriction	   *lastRestriction;
#endif
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
    String	   description;
    String	   reference;
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
    char           *module;
    char	   *path;
    FILE	   *file;
    int		   line;
    int		   column;
    int		   character;
    char	   linebuf[MAX_LINEBUF_LENGTH+1];
    Module	   *modulePtr;
    ParserFlags	   flags;
} Parser;



extern Node	*rootNodePtr;
extern Node	*pendingNodePtr;

extern Type	*typeIntegerPtr, *typeOctetStringPtr, *typeObjectIdentifierPtr;


extern Module *addModule(const char *modulename,
			 const char *path,
			 off_t fileoffset,
			 ModuleFlags flags,
			 Parser *parserPtr);

extern void setModuleLastUpdated(Module *modulePtr,
				 String *lastUpdatedPtr);

extern void setModuleOrganization(Module *modulePtr,
				  String *organizationPtr);

extern void setModuleContactInfo(Module *modulePtr,
				 String *contactInfoPtr);

extern Module *findModuleByName(const char *modulename);



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

extern void setObjectName(Object *objectPtr,
			   smi_descriptor name);

extern void setObjectType(Object *objectPtr,
			  Type *typePtr);

extern void setObjectAccess(Object *objectPtr,
			    smi_access access);

extern void setObjectStatus(Object *objectPtr,
			    smi_status status);

extern void setObjectDescription(Object *objectPtr,
				 String *descriptionPtr);

extern void setObjectReference(Object *objectPtr,
				 String *referencePtr);

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

extern void setTypeSequencePtr(Type *typePtr,
			       struct List *sequencePtr);

extern void setTypeDescription(Type *typePtr,
			       String *descriptionPtr);

extern void setTypeFileOffset(Type *typePtr,
			      off_t fileoffset);

extern void setTypeDecl(Type *typePtr,
			smi_decl decl);

extern void setTypeFlags(Type *typePtr,
			 TypeFlags flags);

extern void setTypeFormat(Type *typePtr,
			  String *formatPtr);



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

extern int readMibFile(const char *path,
		       const char *modulename,
		       ParserFlags flags);

#endif /* _DATA_H */
