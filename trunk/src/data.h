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
 * @(#) $Id: data.h,v 1.15 1998/11/19 19:43:57 strauss Exp $
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





typedef unsigned int SubId;



/*
 * Quoted String.
 */
typedef struct String {
#ifdef TEXTS_IN_MEMORY
    char *ptr;	      			/*   the value			    */
#endif
    int fileoffset;			/*   offset in this file	    */
    int length;				/*   full length		    */
} String;



/*
 * Kinds of Descriptors.
 */
typedef enum DescriptorKind {
    KIND_ANY		 = 0 ,  /*					     */
    KIND_MODULE		 = 1 ,  /*					     */
    KIND_MACRO		 = 2 ,  /*					     */
    KIND_TYPE		 = 3 ,  /*					     */
    KIND_OBJECT		 = 4 ,  /*					     */
    KIND_IMPORT		 = 5 ,  /* descriptors to be imported.               */
    KIND_IMPORTED	 = 6    /* imported descriptor. syntax `mod.descr'.  */
} DescriptorKind;

#define NUM_KINDS  7



/*
 * Flags (general and structure-specific ones).
 */
typedef unsigned short Flags;
#define FLAG_PERMANENT		0x0001 /* e.g. Object and Descriptor `iso'.  */
#define FLAG_IMPORTED		0x0002 /*				     */
#define FLAG_REPOSITORY		0x0004 /*				     */
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

#define FLAGS_GENERAL		0x00ff
#define FLAGS_SPECIFIC		0xff00



/*
 * Known descriptors.
 */
typedef struct Descriptor {
    char		  *name;
    struct Module	  *module;
    void		  *ptr;
    DescriptorKind	  kind;
    int			  flags;
    struct Descriptor	  *next;
    struct Descriptor	  *prev;
    struct Descriptor	  *nextSameModule;
    struct Descriptor	  *prevSameModule;
    struct Descriptor	  *nextSameKind;
    struct Descriptor	  *prevSameKind;
    struct Descriptor	  *nextSameModuleAndKind;
    struct Descriptor	  *prevSameModuleAndKind;
} Descriptor;



/*
 * Mib Module.
 */
typedef struct Module {
    Descriptor    *descriptor;
    Descriptor	  *firstDescriptor[NUM_KINDS];
    Descriptor	  *lastDescriptor[NUM_KINDS];
    char	  *path;
    off_t	  fileoffset;
    String	  lastUpdated;
    String	  organization;
    String	  contactInfo;
    String	  description;
#if 0
    Revision      firstRevision;
#endif
    Flags	  flags;
    int		  numImportedIdentifiers;
    int		  numStatements;
    int		  numModuleIdentities;
} Module;



/*
 * Type.
 */
typedef struct Type {
    Module      *module;
    Descriptor  *descriptor;
    struct Type	*parent;
    smi_syntax  syntax;
    smi_decl	decl;
    String      displayHint;
    smi_status  status;
    String	description;
#if 0
    Restriction *firstRestriction;
#endif
    off_t       fileoffset;
    Flags       flags;
} Type;



/*
 * Object.
 */
typedef struct Object {
    struct Module  *module;
    Descriptor	   *descriptor;
    off_t	   fileoffset;
    smi_decl	   decl;
    Flags	   flags;
    Type	   *type;
    smi_access	   access;
    smi_status	   status;
    String	   description;
    struct Node	   *node;
    struct Object  *prev;
    struct Object  *next;
} Object;

/*
 * Node.
 */
typedef struct Node {
    unsigned int   subid;
    Flags	   flags;
    struct Node	   *parent;
    struct Node	   *next;
    struct Node	   *prev;
    struct Node	   *firstChild;
    struct Node	   *lastChild;
    Object	   *firstObject;
    Object	   *lastObject;
} Node;



/*
 * Macro.
 */
typedef struct Macro {
    Module     *module;
    Descriptor *descriptor;
    off_t      fileoffset;
    Flags      flags;
} Macro;



typedef struct Parser {
    char             *module;
    char	     *path;
    FILE	     *file;
    int		     line;
    int		     column;
    int		     character;
    char	     linebuf[MAX_LINEBUF_LENGTH+1];
    Module	     *thisModule;
    Descriptor	     *firstImportDescriptor;
    Flags	     flags;
} Parser;



extern Descriptor	*firstDescriptor[NUM_KINDS];
extern Descriptor	*lastDescriptor[NUM_KINDS];

extern Node		*rootNode;
extern Node		*pendingRootNode;

extern Type		*typeInteger, *typeOctetString, *typeObjectIdentifier;


extern Module *addModule(const char *name,
			 const char *path,
			 off_t fileoffset,
			 Flags flags,
			 Parser *parser);

extern void setModuleDescription(Module *node,
				 String *description);

extern void setModuleLastUpdated(Module *node,
				 String *lastUpdated);

extern void setModuleOrganization(Module *node,
				  String *organization);

extern void setModuleContactInfo(Module *node,
				 String *contactInfo);

extern Module *findModuleByName(const char *name);



extern int addImportDescriptor(const char *name,
			       Parser *parser);

extern int checkImportDescriptors(char *modulename,
				  Parser *parser);



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




extern Object *addObject(Node *parent,
			 SubId subid,
			 Module *module,
			 Flags flags,
			 Parser *parser);

extern void setObjectSyntax(Object *object,
			    Type *type);

extern void setObjectAccess(Object *object,
			    smi_access access);

extern void setObjectStatus(Object *object,
			    smi_status status);

extern void setObjectDescription(Object *object,
				 String *description);

extern void setObjectFileOffset(Object *object,
				off_t fileoffset);

extern void setObjectDecl(Object *object,
			   smi_decl decl);

extern void setObjectFlags(Object *object,
			   Flags flags);

extern Node *findNodeByParentAndSubid(Node *parent,
				      unsigned int subid);

extern Object *findObjectByName(const char *name);

extern Object *findObjectByModuleAndName(Module *module,
					 const char *name);

extern Object *findObjectByModulenameAndName(const char *modulename,
					      const char *name);

extern void deleteMibTree(Node *root);

extern void dumpMibTree(Node *root, const char *prefix);

extern void dumpMosy(Node *root);



extern Type *addType(Type *parent,
		     smi_syntax syntax,
		     Module *module,
		     Flags flags,
		     Parser *parser);

extern void setTypeStatus(Type *type,
			  smi_status status);

extern void setTypeDescription(Type *type,
			       String *description);

extern void setTypeFileOffset(Type *type,
			      off_t fileoffset);

extern void setTypeDecl(Type *type,
			smi_decl decl);

extern void setTypeFlags(Type *type,
			 Flags flags);

extern void setTypeDisplayHint(Type *type,
			       String *displayHint);



extern Type *findTypeByName(const char *name);

extern Type *findTypeByModuleAndName(Module *module,
				     const char *name);

extern Type *findTypeByModulenameAndName(const char *modulename,
					 const char *name);

extern void dumpTypes();



extern Macro *addMacro(const char *name,
		       Module *module,
		       off_t fileoffset,
		       int flags,
		       Parser *parser);

extern Macro *findMacroByModuleAndName(Module *module,
				       const char *name);

extern Macro *findMacroByModulenameAndName(const char *modulename,
					   const char *name);



extern int initData();

extern int readMibFile(const char *path, const char *modulename, int flags);

#endif /* _DATA_H */
