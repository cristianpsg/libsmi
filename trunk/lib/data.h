/*
 * data.h --
 *
 *      Definitions for the main data structures.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: data.h,v 1.78 2002/05/16 23:21:55 bunkus Exp $
 */

#ifndef _DATA_H
#define _DATA_H


#include <stdio.h>

#include "smi.h"



typedef struct List {
    SmiElement      export;
    void	    *ptr;
    struct List	    *nextPtr;
} List;

typedef enum Kind {
    KIND_UNKNOWN	 = 0 ,  /*					     */
    KIND_MODULE		 = 1 ,  /*					     */
    KIND_MACRO		 = 2 ,  /*					     */
    KIND_TYPE		 = 3 ,  /*					     */
    KIND_OBJECT		 = 4 ,  /*					     */
    KIND_IMPORT		 = 5 ,  /* descriptors to be imported.               */
    KIND_IMPORTED	 = 6 ,  /* imported descriptor. syntax `mod.descr'.  */
    KIND_NOTFOUND	 = 7    /* to be imported, but not found             */
} Kind;
#define NUM_KINDS  7



typedef unsigned short ParserFlags;
typedef unsigned short ModuleFlags;
typedef unsigned short ObjectFlags;
typedef unsigned short NodeFlags;
typedef unsigned short TypeFlags;
typedef unsigned short MacroFlags;

#define NODE_FLAG_ROOT		0x0001 /* mark node tree's root */

#define FLAG_SEQTYPE		0x0002 /* type is set from SMIv1/2 SEQUENCE. */
#define FLAG_REGISTERED		0x0004 /* On an Object: this is registered.  */
#define FLAG_INCOMPLETE		0x0008 /* Just defined by a forward          */
				       /* referenced type or object.         */
#define	FLAG_CREATABLE	        0x0040 /* On a Row: New rows can be created. */
#define FLAG_INGROUP		0x0080 /* Node is contained in a group.      */
#define	FLAG_INCOMPLIANCE	0x0100 /* Group is mentioned in a compliance
                                           statement.                        */


typedef struct View {
    char	    *name;
    struct View	    *nextPtr;
    struct View	    *prevPtr;
} View;



typedef struct Module {
    SmiModule	    export;
    time_t	    lastUpdated; /* only for SMIv2 modules */
    struct Object   *objectPtr;
    struct Object   *firstObjectPtr;
    struct Object   *lastObjectPtr;
    struct Type	    *firstTypePtr;
    struct Type	    *lastTypePtr;
    struct Macro    *firstMacroPtr;
    struct Macro    *lastMacroPtr;
    struct Import   *firstImportPtr;
    struct Import   *lastImportPtr;
    struct Revision *firstRevisionPtr;
    struct Revision *lastRevisionPtr;
    ModuleFlags	    flags;
    int		    numImportedIdentifiers;
    int		    numStatements;
    int		    numModuleIdentities;
    struct Module   *nextPtr;
    struct Module   *prevPtr;
    struct Node     *prefixNodePtr;
} Module;



typedef struct Revision {
    SmiRevision	    export;
    Module          *modulePtr;
    struct Revision *nextPtr;
    struct Revision *prevPtr;
    int		    line;
} Revision;



typedef struct Import {
    SmiImport      export;
    Module         *modulePtr;
    struct Import  *nextPtr;
    struct Import  *prevPtr;
    Kind	   kind;
    int		   use;
    int		   line;
} Import;



typedef struct NamedNumber {
    SmiNamedNumber export;
    struct Type    *typePtr;
} NamedNumber;



typedef struct Range {
    SmiRange       export;
    struct Type    *typePtr;
} Range;



typedef struct Type {
    SmiType        export;
    Module         *modulePtr;
    struct Type    *parentPtr;
    struct List    *listPtr;
    TypeFlags	   flags;
    struct Type    *nextPtr;
    struct Type    *prevPtr;
    int		   line;
} Type;



typedef struct Option {
    SmiOption      export;
    struct Object  *compliancePtr;
    struct Object  *objectPtr;
} Option;



typedef struct Refinement {
    SmiRefinement  export;
    struct Object  *compliancePtr; /* the compl. Object this Ref belongs to  */
    struct Object  *objectPtr;     /* the Object refined by this Refinement  */
    Type	   *typePtr;       /* the refined Type (or NULL)             */
    Type	   *writetypePtr;  /* the refined WriteType (or NULL)        */
} Refinement;



typedef struct Compl {
    List  *mandatorylistPtr;
    List  *optionlistPtr;
    List  *refinementlistPtr;
} Compl;



typedef struct Index {
    int	           implied;
    SmiIndexkind   indexkind;
    struct List    *listPtr; /* list of index columns (Object *) */
    struct Object  *rowPtr;  /* a referenced row (e.g. SMIv2 AUGMENTS) */
} Index;


typedef struct SubjectCategories {
    struct List *categories;
    int         allCategories;
} SubjectCategories;


typedef struct Object {
    SmiNode        export;
    Module         *modulePtr;
    ObjectFlags	   flags;
    Type	   *typePtr;
    struct Object  *relatedPtr;         /* a related Object (augmented row) */
    struct List    *listPtr;	        /* OG, NT, NG, MC */
    struct List    *optionlistPtr;
    struct List    *refinementlistPtr;
    struct Node	   *nodePtr;
    struct Object  *prevPtr;		/* chain of Objects in this Module */
    struct Object  *nextPtr;
    struct Object  *prevSameNodePtr;    /* chain of Objects for this Node  */
    struct Object  *nextSameNodePtr;
    struct Object  *pibReferencesPtr;   /* PIB-REFERENCES */
    struct Object  *pibTagPtr;          /* PIB-TAG */
    int            allSubjectCategories;/* SUBJECT-CATEGORIES all, others are stored in listPtr */
    struct List    *uniquenessPtr;      /* UNIQUENESS */
    struct List    *installErrorsPtr;   /* INSTALL-ERRORS */
    int		   line;
} Object;



typedef struct Node {
    SmiSubid	   subid;
    NodeFlags	   flags;
    int            oidlen;
    SmiSubid       *oid;
    struct Node	   *parentPtr;
    struct Node	   *nextPtr;
    struct Node	   *prevPtr;
    struct Node	   *firstChildPtr;
    struct Node	   *lastChildPtr;
    Object	   *firstObjectPtr;
    Object	   *lastObjectPtr;
} Node;



typedef struct Macro {
    SmiMacro	   export;
    Module	   *modulePtr;
    MacroFlags	   flags;
    struct Macro   *nextPtr;
    struct Macro   *prevPtr;
    int		   line;
} Macro;



typedef struct Parser {
    char	   *path;
    FILE	   *file;
    int		   line;
    int		   lcline;		/* for SMI comment warning */
    Module	   *modulePtr;
    ParserFlags	   flags;
    List	   *firstIndexlabelPtr; /* only for the SMIng parser */
    char	   *identityObjectName; /* only for the SMIng parser */
} Parser;



typedef struct Handle {
    char            *name;
    struct Handle   *prevPtr;
    struct Handle   *nextPtr;
    View     	    *firstViewPtr;
    View     	    *lastViewPtr;
    Module   	    *firstModulePtr;
    Module   	    *lastModulePtr;
    Node     	    *rootNodePtr;
    Node     	    *pendingNodePtr;
    Type     	    *typeOctetStringPtr;
    Type     	    *typeObjectIdentifierPtr;
    Type     	    *typeInteger32Ptr;
    Type     	    *typeUnsigned32Ptr;
    Type     	    *typeInteger64Ptr;
    Type     	    *typeUnsigned64Ptr;
    Type     	    *typeFloat32Ptr;
    Type     	    *typeFloat64Ptr;
    Type     	    *typeFloat128Ptr;
    Type     	    *typeEnumPtr;
    Type     	    *typeBitsPtr;
    int	     	    flags;
    char     	    *path;
    char     	    *cache;
    char     	    *cacheProg;
    int      	    errorLevel;
    SmiErrorHandler *errorHandler;
} Handle;



extern int	 smiDepth;	/* SMI parser recursion depth */

extern Handle    *smiHandle;    /* The current handle */



extern Handle *addHandle(const char *name);

extern void removeHandle(Handle *handlePtr);

extern Handle *findHandleByName(const char *name);



extern View *addView(const char *modulename);

extern int isInView(const char *modulename);



extern Module *addModule(char *modulename,
			 char *path,
			 ModuleFlags flags,
			 Parser *parserPtr);

extern void setModuleIdentityObject(Module *modulePtr,
				    Object *objectPtr);

extern void setModuleLastUpdated(Module *modulePtr,
				 time_t lastUpdated);

extern void setModuleOrganization(Module *modulePtr,
				  char *organization);

extern void setModuleContactInfo(Module *modulePtr,
				 char *contactinfo);

extern void setModuleDescription(Module *modulePtr,
				 char *description,
				 Parser *parserPtr);

extern void setModuleReference(Module *modulePtr,
			       char *reference,
			       Parser *parserPtr);

extern Module *findModuleByName(const char *modulename);



extern Revision *addRevision(time_t date,
			     char *description,
			     Parser *parserPtr);

extern void setRevisionLine(Revision *revisionPtr,
			    int line,
			    Parser *parserPtr);



extern Import *addImport(char *name,
			 Parser *parserPtr);

extern void setImportModulename(Import *importPtr,
				char *modulename);

extern int checkImports(Module *modulePtr,
			Parser *parserPtr);

extern Import *findImportByName(const char *name,
				Module *modulePtr);

extern Import *findImportByModulenameAndName(const char *modulename,
					     const char *name,
					     Module *modulePtr);

extern Object *addObject(char *objectname,
			 Node *parentNodePtr,
			 SmiSubid subid,
			 ObjectFlags flags,
			 Parser *parserPtr);

extern Object *duplicateObject(Object *templatePtr,
			       ObjectFlags flags,
			       Parser *parserPtr);

extern Node *addNode(Node *parentNodePtr,
		     SmiSubid subid,
		     NodeFlags flags,
		     Parser *parserPtr);

extern Node *createNodes(unsigned int oidlen, SmiSubid *oid);

extern Node *createNodesByOidString(const char *oid);

extern Node *getParentNode(Node *nodePtr);

/*
 * setObjectName() might relink MIB tree object structures. If the
 * current objectPtr is to be used after the call, it should look like
 *   objectPtr = setObjectName(objectPtr, name);
 */
extern Object *setObjectName(Object *objectPtr,
			     char *name);

extern void setObjectType(Object *objectPtr,
			  Type *typePtr);

extern void setObjectAccess(Object *objectPtr,
			    SmiAccess access);

extern void setObjectStatus(Object *objectPtr,
			    SmiStatus status);

extern void setObjectDescription(Object *objectPtr,
				 char *description,
				 Parser *parserPtr);

extern void setObjectReference(Object *objectPtr,
			       char *reference,
			       Parser *parserPtr);

extern void setObjectPibReferences(Object *objectPtr,
			           Object *pibReferencesPtr);

extern void setObjectPibTag(Object *objectPtr,
			    Object *pibTagPtr);

extern void setObjectDecl(Object *objectPtr,
			   SmiDecl decl);

extern void setObjectUniqueness(Object *objectPtr, List *listPtr);

extern void setObjectInstallErrors(Object *objectPtr, List *listPtr);

extern void setObjectLine(Object *objectPtr,
			  int line,
			  Parser *parserPtr);

extern void setObjectNodekind(Object *objectPtr,
			      SmiNodekind nodekind);

extern void addObjectFlags(Object *objectPtr,
			   ObjectFlags flags);

extern void deleteObjectFlags(Object *objectPtr,
			      ObjectFlags flags);

extern int checkObjectFlags(Object *objectPtr, ObjectFlags flags);

extern void setObjectList(Object *objectPtr,
			  struct List *listPtr);

extern void setObjectRelated(Object *objectPtr,
			     Object *relatedPtr);

extern void setObjectImplied(Object *objectPtr,
			     int implied);

extern void setObjectCreate(Object *objectPtr,
			    int create);

extern void setObjectIndexkind(Object *objectPtr,
			       SmiIndexkind indexkind);

extern void setObjectIndex(Object *objectPtr,
			   Index *indexPtr);

extern void setObjectFormat(Object *objectPtr,
			    char *format);

extern void setObjectUnits(Object *objectPtr,
			   char *units);

extern void setObjectValue(Object *objectPtr,
			   SmiValue *valuePtr);

extern void setObjectSubjectCategories(Object *objectPtr, int allCategories);

extern Node *findNodeByParentAndSubid(Node *parentNodePtr,
				      SmiSubid subid);

extern Node *findNodeByOid(unsigned int oidlen, SmiSubid *oid);

extern Node *findNodeByOidString(char *oid);

extern Object *findObjectByNode(Node *nodePtr);

extern Object *findObjectByModuleAndNode(Module *modulePtr,
					 Node *nodePtr);

extern Object *findObjectByModulenameAndNode(const char *modulename,
					     Node *nodePtr);

extern Object *findObjectByName(const char *objectname);

extern Object *findNextObjectByName(const char *objectname,
				    Object *prevObjectPtr);

extern Object *findObjectByModuleAndName(Module *modulePtr,
					 const char *objectname);

extern Object *findObjectByModulenameAndName(const char *modulename,
					     const char *objectname);



extern Type *addType(char *type_name,
		     SmiBasetype basetype,
		     TypeFlags flags,
		     Parser *parserPtr);

extern Type *duplicateType(Type *templatePtr,
			   TypeFlags flags,
			   Parser *parserPtr);

extern Type *setTypeName(Type *typePtr,
			 char *name);

extern void setTypeStatus(Type *typePtr,
			  SmiStatus status);

extern void setTypeBasetype(Type *typePtr,
			  SmiBasetype basetype);

extern void setTypeParent(Type *typePtr,
			  Type *parentPtr);

extern void setTypeList(Type *typePtr,
			struct List *listPtr);

extern void setTypeDescription(Type *typePtr,
			       char *description,
			       Parser *parserPtr);

extern void setTypeReference(Type *typePtr,
			     char *reference,
			     Parser *parserPtr);

extern void setTypeDecl(Type *typePtr,
			SmiDecl decl);

extern void setTypeLine(Type *typePtr,
			int line,
			Parser *parserPtr);

extern void addTypeFlags(Type *typePtr,
			 TypeFlags flags);

extern void deleteTypeFlags(Type *typePtr,
			    TypeFlags flags);

extern void setTypeFormat(Type *typePtr,
			  char *format);

extern void setTypeUnits(Type *typePtr,
			 char *units);

extern void setTypeValue(Type *typePtr,
			 SmiValue *valuePtr);



extern Type *findTypeByName(const char *type_name);

extern Type *findNextTypeByName(const char *type_name,
				Type *prevTypePtr);

extern Type *findTypeByModuleAndName(Module *modulePtr,
				     const char *type_name);

extern Type *findTypeByModulenameAndName(const char *modulename,
					 const char *type_name);

extern NamedNumber *findTypeNamedNumber(Type *typePtr,
					SmiInteger32 number);

extern Macro *addMacro(char *macroname,
		       MacroFlags flags,
		       Parser *parserPtr);

extern void setMacroStatus(Macro *macroPtr,
			   SmiStatus status);

extern void setMacroDescription(Macro *macroPtr,
				char *description,
				Parser *parserPtr);

extern void setMacroReference(Macro *macroPtr,
			      char *reference,
			      Parser *parserPtr);

extern void setMacroDecl(Macro *macroPtr,
			 SmiDecl decl);

extern void setMacroLine(Macro *macroPtr,
			 int line,
			 Parser *parserPtr);

extern Macro *findMacroByName(const char *macroname);

extern Macro *findMacroByModuleAndName(Module *modulePtr,
				       const char *macroname);

extern Macro *findMacroByModulenameAndName(const char *modulename,
					   const char *macroname);



extern int smiInitData(void);

extern void smiFreeData(void);

extern Module *loadModule(const char *modulename, Parser *parserPtr);


#endif /* _DATA_H */
