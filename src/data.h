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
 * @(#) $Id: data.h,v 1.4 1998/10/14 16:07:30 strauss Exp $
 */

#ifndef _DATA_H
#define _DATA_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

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
 * RevisionsPart of Module.
 */
typedef struct Revision {
    char       TODO;
    struct Revision *next;
} Revision;



/*
 * Directories to search for MIB module files.
 */
typedef struct Directory {
    char	     dir[MAX_PATH_LENGTH+1];
    struct Directory *next;
    struct Directory *prev;
} Directory;



/*
 * SYNTAX.
 */
typedef enum Syntax {
    SYNTAX_UNKNOWN           = 0,
    SYNTAX_INTEGER           = 1, /* equal to INTEGER32 */
    SYNTAX_OCTET_STRING	     = 2,
    SYNTAX_OBJECT_IDENTIFIER = 3,
    SYNTAX_SEQUENCE	     = 4,
    SYNTAX_SEQUENCE_OF	     = 5,
    SYNTAX_IPADDRESS	     = 6,
    SYNTAX_COUNTER32	     = 7,
    SYNTAX_GAUGE32	     = 8,
    SYNTAX_UNSIGNED32	     = 9,
    SYNTAX_TIMETICKS	     = 10,
    SYNTAX_OPAQUE	     = 11,
    SYNTAX_COUNTER64	     = 12
} Syntax;


	
/*
 * STATUS values.
 */
typedef enum Status {
    STATUS_UNKNOWN	 = 0 ,
    STATUS_CURRENT	 = 1 ,
    STATUS_DEPRECATED	 = 2 ,
    STATUS_OBSOLETE	 = 3
} Status;



/*
 * ACCESS values.
 */
typedef enum Access {
    ACCESS_UNKNOWN	 = 0 ,
    ACCESS_NOT		 = 1 ,
    ACCESS_NOTIFY	 = 2 ,
    ACCESS_READ_ONLY	 = 3 ,
    ACCESS_READ_WRITE	 = 4 ,
    ACCESS_READ_CREATE	 = 5 ,
    ACCESS_WRITE_ONLY	 = 6
} Access;



/*
 * Kinds of Descriptors.
 */
typedef enum DescriptorKind {
    KIND_ANY	 = 0 ,
    KIND_MODULE	 = 1 ,
    KIND_MACRO	 = 2 ,
    KIND_TYPE	 = 3 ,
    KIND_MIBNODE = 4 ,
    KIND_IMPORT	 = 5
} DescriptorKind;

#define NUM_KINDS  6



/*
 * Macros that may declare MibNodes.
 */
typedef enum DeclMacro {
    MACRO_UNKNOWN	     = 0 ,
    MACRO_NONE		     = 1 ,
    MACRO_OBJECTTYPE	     = 2 ,
    MACRO_OBJECTIDENTITY     = 3 ,
    MACRO_MODULEIDENTITY     = 4 ,
    MACRO_NOTIFICATIONTYPE   = 5 ,
    MACRO_TRAPTYPE	     = 6 ,
    MACRO_OBJECTGROUP	     = 7 ,
    MACRO_NOTIFICATIONGROUP  = 8 ,
    MACRO_MODULECOMPLIANCE   = 9 ,
    MACRO_AGENTCAPABILITIES  = 10
} DeclMacro;



/*
 * Flags (general and structure-specific ones).
 */
typedef unsigned short Flags;
#define FLAG_PERMANENT		0x0001 /* e.g. MibNode and Descriptor `iso'. */
#define FLAG_IMPORTED		0x0002 /*				     */
#define FLAG_REPOSITORY		0x0004 /*				     */
#define FLAG_MODULE		0x0008 /* Declared in the current module.    */
#define FLAG_REGISTERED		0x0010 /* On a MibNode: this is registered.  */

#define	FLAG_TC                 0x0100 /* On a Type: This type is declared   */
				       /* by a TC instead of a simple ASN.1  */
				       /* type declaration.                  */

#define	FLAG_SMIV2	        0x0100 /* On a Module: This is an SMIv2 MIB. */

#define FLAG_NOSUBID		0x0100 /* On a (pending) MibNode: This node's*/
				       /* subid value is not yet known.      */
#define FLAG_ROOT	        0x0200 /* Marks the single root MibNode.     */

#define	FLAG_WHOLEFILE		0x0100 /* We want to read the whole */
				       /* file, not just a single   */
				       /* module.		    */
#define FLAG_ACTIVE		0x0200 /* We want to read something */
				       /* from the current module.  */
#define	FLAG_ERRORS		0x0400 /* Otherwise be quiet,       */
				       /* useful when IMPORTing.    */
#define FLAG_WHOLEMOD		0x0800 /* Fetch all items instead   */
				       /* of just idlist (IMPORTS). */
#define FLAG_STATS		0x1000 /* Print module statistics.  */
#define FLAG_RECURSIVE		0x2000 /* Errors and stats also for */
				       /* imported modules.         */

#define FLAGS_GENERAL		0x00ff
#define FLAGS_SPECIFIC		0xff00



/*
 * Known descriptors.
 */
typedef struct Descriptor {
    char       name[MAX_IDENTIFIER_LENGTH+1];
    struct Module     *module;
    void       *ptr;
    DescriptorKind kind;
    int	       flags;
    struct Descriptor *next;
    struct Descriptor *prev;
    struct Descriptor *nextSameModule;
    struct Descriptor *prevSameModule;
    struct Descriptor *nextSameKind;
    struct Descriptor *prevSameKind;
    struct Descriptor *nextSameModuleAndKind;
    struct Descriptor *prevSameModuleAndKind;
} Descriptor;



/*
 * Mib Module.
 */
typedef struct Module {
    Descriptor    *descriptor;
    Descriptor	  *firstDescriptor[NUM_KINDS];
    Descriptor	  *lastDescriptor[NUM_KINDS];
    char	  path[MAX_PATH_LENGTH+1];
    off_t	  fileoffset;
    String	  lastUpdated;
    String	  organization;
    String	  contactInfo;
    String	  description;
    Revision      firstRevision;
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
    Syntax      syntax;
    DeclMacro	macro;
    String      displayHint;
    Status      status;
    String	description;
#if 0
    Restriction *firstRestriction;
#endif
    off_t       fileoffset;
    Flags       flags;
} Type;



/*
 * Mib Node.
 */
typedef struct MibNode {
    struct Module  *module;
    unsigned int   subid;
    Descriptor	   *descriptor;
    off_t	   fileoffset;
    DeclMacro	   macro;
    Flags	   flags;
    Type	   *type;
    Access	   access;
    Status	   status;
    String	   description;
    struct MibNode *parent;
    struct MibNode *next;
    struct MibNode *prev;
    struct MibNode *firstChild;
    struct MibNode *lastChild;
} MibNode;



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
    char             module[MAX_IDENTIFIER_LENGTH+1];
    char	     path[MAX_PATH_LENGTH+1];
    FILE	     *file;
    int		     line;
    int		     column;
    int		     character;
    char	     linebuf[MAX_LINEBUF_LENGTH+1];
    Module	     *thisModule;
    Descriptor	     *firstImportDescriptor;
    Flags	     flags;
} Parser;



extern Directory	*firstDirectory;
extern Directory	*lastDirectory;
extern Descriptor	*firstDescriptor[NUM_KINDS];
extern Descriptor	*lastDescriptor[NUM_KINDS];

extern MibNode		*rootMibNode;
extern MibNode		*pendingRootMibNode;

extern Type		*typeInteger, *typeInteger32, *typeCounter32,
			*typeGauge32, *typeIpAddress, *typeTimeTicks,
			*typeOpaque, *typeOctetString, *typeUnsigned32,
			*typeObjectIdentifier, *typeCounter64;


extern char *findFileByModulename(const char *module);

extern Directory *addDirectory(const char *dir);



extern Module *addModule(const char *name,
			 const char *path,
			 off_t fileoffset,
			 Flags flags,
			 Parser *parser);

extern Module *findModuleByName(const char *name);



extern int addImportDescriptor(const char *name,
			       Parser *parser);

extern int checkImportDescriptors(Module *module,
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




extern MibNode *addMibNode(MibNode *parent,
			   SubId subid,
			   Module *module,
			   Flags flags,
			   Parser *parser);

extern void setMibNodeAccess(MibNode *node,
			     Access access);

extern void setMibNodeStatus(MibNode *node,
			     Status status);

extern void setMibNodeDescription(MibNode *node,
				  String *description);

extern void setMibNodeFileOffset(MibNode *node,
				 off_t fileoffset);

extern void setMibNodeMacro(MibNode *node,
			    DeclMacro macro);

extern void setMibNodeFlags(MibNode *node,
			    Flags flags);

extern MibNode *findMibNodeByOID(const char *oid);

extern MibNode *findMibNodeByParentAndSubid(MibNode *parent,
					    unsigned int subid);

extern MibNode *findMibNodeByName(const char *name);

extern MibNode *findMibNodeByModuleAndName(Module *module,
					   const char *name);

extern MibNode *findMibNodeByModulenameAndName(const char *modulename,
					       const char *name);

extern void deleteMibTree(MibNode *root);

extern void dumpMibTree(MibNode *root, const char *prefix);

extern void dumpMosy(MibNode *root);



extern Type *addType(Type *parent,
		     Syntax syntax,
		     Module *module,
		     Flags flags,
		     Parser *parser);

extern void setTypeStatus(Type *type,
			  Status status);

extern void setTypeDescription(Type *type,
			       String *description);

extern void setTypeFileOffset(Type *type,
			      off_t fileoffset);

extern void setTypeMacro(Type *type,
			 DeclMacro macro);

extern void setTypeFlags(Type *type,
			 Flags flags);

extern void setTypeDisplayHint(Type *type,
			       String *displayHint);



extern Type *findTypeByModuleAndName(Module *module,
				     const char *name);



extern Macro *addMacro(const char *name,
		       Module *module,
		       off_t fileoffset,
		       int flags,
		       Parser *parser);

extern Macro *findMacroByModuleAndName(Module *module,
				       const char *name);



extern int initData();

extern int readMibFile(const char *path, const char *modulename, int flags);

#endif /* _DATA_H */
