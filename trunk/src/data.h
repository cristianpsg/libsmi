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
 * @(#) $Id: data.h,v 1.23 1998/12/14 16:35:51 strauss Exp $
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





/*
 * Simple generic list type. For SEQUENCEs and INDEX lists.
 */
typedef struct List {
    void	    *ptr;
    struct List	    *next;
} List;



/*
 * Quoted String located in a MIB File.
 */
typedef struct String {
#ifdef TEXTS_IN_MEMORY
    char *ptr;	      			/*   the value			    */
#endif
    int fileoffset;			/*   offset in this file	    */
    int length;				/*   full length		    */
} String;



typedef enum Kind {
    KIND_ANY		 = 0 ,  /*					     */
    KIND_MODULE		 = 1 ,  /*					     */
    KIND_MACRO		 = 2 ,  /*					     */
    KIND_TYPE		 = 3 ,  /*					     */
    KIND_OBJECT		 = 4 ,  /*					     */
    KIND_IMPORT		 = 5 ,  /* descriptors to be imported.               */
    KIND_IMPORTED	 = 6    /* imported descriptor. syntax `mod.descr'.  */
} Kind;

#define NUM_KINDS  7



typedef unsigned short Flags;
#define FLAG_PERMANENT		0x0001 /* e.g. Object and Descriptor `iso'.  */
#define FLAG_IMPORTED		0x0002 /*				     */
#define FLAG_PARENTIMPORTED	0x0004 /* On a Type: This Type's parent is   */
				       /* imported instead of local, hence   */
				       /* its parent is a pointer to a       */
				       /* descriptor instead of a type.      */
#define FLAG_MODULE		0x0008 /* Declared in the current module.    */
#define FLAG_REGISTERED		0x0010 /* On an Object: this is registered.  */
#define FLAG_INCOMPLETE		0x0020 /* Just defined by a forward          */
				       /* referenced type.		     */

#define	FLAG_TC                 0x0100 /* On a Type: This type is declared   */
				       /* by a TC instead of a simple ASN.1  */
				       /* type declaration.                  */

#define	FLAG_SMIV2	        0x0100 /* On a Module: This is an SMIv2 MIB. */

#define FLAG_NOSUBID		0x0100 /* On a (pending) Node: This node's   */
				       /* subid value is not yet known.      */
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
    Object	   *firstObjectPtr;
    Object	   *lastObjectPtr;
    Type	   *firstTypePtr;
    Type	   *lastTypePtr;
    Macro	   *firstMacroPtr;
    Macro	   *lastMacroPtr;
    Import	   *firstImportPtr;
    Import	   *lastImportPtr;
    String	   lastUpdated;
    String	   organization;
    String	   contactInfo;
    String	   description;
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
    smi_descriptor *module;
    smi_descriptor *name;
    struct Import  *nextPtr;
    struct Import  *prevPtr;
    Kind	   kind;
}



typedef struct Type {
    Module         *modulePtr;
    smi_descriptor name;
    smi_fullname   parent;
    smi_syntax	   syntax;
    smi_decl	   decl;
    String	   displayHint;
    smi_status	   status;
    String	   description;
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
    struct Node	   *nodePtr;
    Object	   *prevPtr;		/* chain of Objects in this Module */
    Object	   *nextPtr;
    Object	   *prevSameNodePtr;    /* chain of Objects for this Node  */
    Object	   *nextSameNodePtr;
} Object;



typedef struct Node {
    smi_subid	   subid;
    NodeFlags	   flags;
    struct Node	   *parentNodePtr;
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



extern Node	   *rootNodePtr;
extern Node	   *pendingNodePtr;

extern Type	   *typeInteger, *typeOctetString, *typeObjectIdentifier;


extern Module *addModule(const char *modulename,
			 const char *path,
			 off_t fileoffset,
			 ModuleFlags flags,
			 Parser *parserPtr);

extern void setModuleDescription(Module *modulePtr,
				 String *descriptionPtr);

extern void setModuleLastUpdated(Module *modulePtr,
				 String *lastUpdatedPtr);

extern void setModuleOrganization(Module *modulePtr,
				  String *organizationPtr);

extern void setModuleContactInfo(Module *modulePtr,
				 String *contactInfoPtr);

extern Module *findModuleByName(const char *modulename);



extern int addImport(const char *name,
		     Parser *parserPtr);

extern int checkImports(char *modulename,
			Parser *parserPtr);


/*
extern Descriptor *addDescriptor(const char *name,
				 Module *module,
				 DescriptorKind kind,
				 void *ptr,
				 int flags,
				 Parser *parser);

extern void deleteDescriptor(Descriptor *descriptor);

extern Descriptor *findDescriptor(const char *name,
				  Module *module,
				  DescriptorKind kind);

extern Descriptor *findNextDescriptor(const char *name,
				      Module *module,
				      DescriptorKind kind,
				      Descriptor *start);
*/



extern Object *addObject(const char *objectname,
			 Node *parentNodePtr,
			 smi_subid subid,
			 ObjectFlags flags,
			 Parser *parserPtr);

extern Object *duplicateObject(const char *objectname,
			       Object *objectPtr,
			       ObjectFlags flags,
			       Parser *parserPtr);

extern Node *addNode(Node *parentNodePtr,
		     smi_subid subid,
		     NodeFlags flags,
		     Parser *parserPtr);

extern Node *createNodes(const char *oid);

extern Node *getParentNode(Node *nodePtr);

extern smi_subid getLastSubid(const char *oid);

extern void setObjectSyntax(Object *objectPtr,
			    smi_syntax);

extern void setObjectAccess(Object *objectPtr,
			    smi_access access);

extern void setObjectStatus(Object *objectPtr,
			    smi_status status);

extern void setObjectDescription(Object *objectPtr,
				 String *descriptionPtr);

extern void setObjectFileOffset(Object *objectPtr,
				off_t fileoffset);

extern void setObjectDecl(Object *objectPtr,
			   smi_decl decl);

extern void setObjectFlags(Object *objectPtr,
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

/*
extern void deleteMibTree(Node *root);

extern void dumpMibTree(Node *root, const char *prefix);

extern void dumpMosy(Node *root);
*/


extern Type *addType(const char *typename,
		     smi_syntax syntax,
		     TypeFlags flags,
		     Parser *parserPtr);

extern void setTypeStatus(Type *typePtr,
			  smi_status status);

extern void setTypeDescription(Type *typePtr,
			       String *descriptionPtr);

extern void setTypeFileOffset(Type *typePtr,
			      off_t fileoffset);

extern void setTypeDecl(Type *typePtr,
			smi_decl decl);

extern void setTypeFlags(Type *typePtr,
			 Flags flags);

extern void setTypeDisplayHint(Type *typePtr,
			       String *displayHintPtr);



extern Type *findTypeByName(const char *typename);

extern Type *findTypeByModuleAndName(Module *modulePtr,
				     const char *typename);

extern Type *findTypeByModulenameAndName(const char *modulename,
					 const char *typename);

/*
extern void dumpTypes();
*/



extern Macro *addMacro(const char *macroname,
		       /* Module *modulePtr, parserPtr->modulePtr */
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
