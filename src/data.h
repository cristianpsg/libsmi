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
 * @(#) $Id: data.h,v 1.28 1998/10/08 15:58:19 strauss Exp $
 */

#ifndef _DATA_H
#define _DATA_H

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "defs.h"
#include "error.h"
#include "parser.h"






/*
 * Reference to the location of a quoted string in a MIB module file.
 * NOTE: Module name and path are taken from parent Module structure.
 */
typedef struct TextRef {
    off_t      fileoffset;
    int	       length;
} TextRef;



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
#define	           NODE_IMPLICIT	  0
#define	           NODE_VALUEASSIGNMENT   10
#define		   NODE_INOIDVALUE	  11
#define		   NODE_FORWARD		  12
typedef enum DeclMacro {
    MACRO_NONE		     = 0 ,
    MACRO_OBJECTTYPE	     = 1 ,
    MACRO_OBJECTIDENTITY     = 2 ,
    MACRO_MODULEIDENTITY     = 3 ,
    MACRO_NOTIFICATIONTYPE   = 4 ,
    MACRO_TRAPTYPE	     = 5 ,
    MACRO_OBJECTGROUP	     = 6 ,
    MACRO_NOTIFICATIONGROUP  = 7 ,
    MACRO_MODULECOMPLIANCE   = 8 ,
    MACRO_AGENTCAPABILITIES  = 9
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
#if 0
#define FLAG_PENDING		0x0020 /* Not yet linked into the main tree. */
#endif
#if 0
#define FLAG_INPROGRESS		0x0040 /* On a (pending) MibNode: This marks */
				       /* that the integration into the main */
				       /* tree is in progress to supress     */
				       /* recursion problems.                */
#endif

#define	FLAG_TC                 0x0100 /* On a Type: This type is declared   */
				       /* by a TC instead of a simple ASN.1  */
				       /* ASN.1 type declaration.            */

#define	FLAG_SMIV2	        0x0100 /* On a Module: This is an SMIv2 MIB. */

#define FLAG_NOVALUE		0x0100 /* On a (pending) MibNode: This node's*/
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
 * Mib Node.
 */
typedef struct MibNode {
    struct Module  *module;
#if 0
    char	   oid[MAX_OID_STRING_LENGTH+1];
#endif
    unsigned int   subid;
    Descriptor	   *descriptor;
    off_t	   fileoffset;
    DeclMacro	   macro;
    Flags	   flags;
    struct MibNode *parent;
    struct MibNode *next;
    struct MibNode *prev;
    struct MibNode *firstChild;
    struct MibNode *lastChild;
} MibNode;



/*
 * Mib Module.
 */
typedef struct Module {
    Descriptor    *descriptor;
    Descriptor	  *firstDescriptor[NUM_KINDS];
    Descriptor	  *lastDescriptor[NUM_KINDS];
    char	  path[MAX_PATH_LENGTH+1];
    off_t	  fileoffset;
    TextRef	  lastUpdated;
    TextRef	  organization;
    TextRef	  contactInfo;
    TextRef	  description;
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
    Module     *module;
    char       syntax[MAX_OID_STRING_LENGTH+1];
    Descriptor *descriptor;
    off_t      fileoffset;
    Flags      flags;
} Type;



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



/*
 *
 */
typedef struct PendingMibNode {
    Descriptor		  *descriptor;
    MibNode		  *node;
    struct PendingMibNode *next;
} PendingMibNode;







/*
 * Quoted String.
 */
typedef struct String {
    char content[201];			/*   first 200 chars		    */
    int fileoffset;			/*   offset in this file	    */
    int length;				/*   full length		    */
} String;



/*
 *
 */
typedef struct Node {
    MibNode *parent;
    MibNode *mibnode;
} Node;



/*
 * Object Identifier in numerical string format (e.g. "1 3 6 1").
 */
typedef struct Oid {
    char content[768];			/*   128 * (1blank + 5digits)	    */
} Oid;





extern Directory	*firstDirectory;
extern Directory	*lastDirectory;
extern Descriptor	*firstDescriptor[NUM_KINDS];
extern Descriptor	*lastDescriptor[NUM_KINDS];

extern PendingMibNode	*firstPendingMibNode;

extern MibNode		*rootMibNode;
extern MibNode		*pendingRootMibNode;



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




extern MibNode *addMibNode(const char *name,
			   Module *module,
			   MibNode *parent,
			   unsigned int subid,
			   off_t fileoffset,
			   DeclMacro macro,
			   Flags flags,
			   Parser *parser);

extern void changeMibNode(MibNode *mibnode,
			  Descriptor *descriptor,
			  DeclMacro macro,
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



extern Type *addType(const char *name,
		     Module *module,
		     const char *syntax,
		     off_t fileoffset,
		     int flags,
		     Parser *parser);

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
