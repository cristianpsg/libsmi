/*
 * dump-dia.c --
 *
 *      Operations to dump UML representation of MIBs in DIA XML format.
 *
 * Copyright (c) 1999 F. Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-dia.c,v 1.4 2000/02/22 17:11:14 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"


#define	 X_OFFSET	2.0
#define	 Y_OFFSET	2.0



static char *getTypeName(SmiNode *smiNode)
{
    SmiType *smiType, *parentType;

    smiType = smiGetNodeType(smiNode);

    if (!smiType || smiNode->nodekind == SMI_NODEKIND_TABLE)
	return NULL;
    
    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
	parentType = smiGetParentType(smiType);
	smiType = parentType;
    }

    return smiType->name;
}



static int isGroup(SmiNode *smiNode)
{
    SmiNode *childNode;
    
    for(childNode = smiGetFirstChildNode(smiNode);
	childNode;
	childNode = smiGetNextChildNode(childNode)) {
	if ((childNode->nodekind == SMI_NODEKIND_SCALAR
	     || childNode->nodekind == SMI_NODEKIND_TABLE)
	    && childNode->status == SMI_STATUS_CURRENT) {
	    return 1;
	}
    }

    return 0;
}



static void printAssiciation(SmiNode *node, SmiNode *relatedNode)
{
    int conn;
    SmiNode *parentNode, *columnNode;


    if (relatedNode->nodekind == SMI_NODEKIND_ROW) {
	parentNode = relatedNode;
	conn = 1;
    } else {
	parentNode = smiGetParentNode(relatedNode);
	for (conn = 8, columnNode = smiGetFirstChildNode(parentNode);
	     columnNode && columnNode != relatedNode;
	     conn += 2, columnNode = smiGetNextChildNode(columnNode));
    }

    fprintf(stderr, "[%s]-[%s:%d]\n", node->name, parentNode->name, conn);
    
    printf("    <dia:object type=\"UML - Association\" "
	   "version=\"0\" id=\"Assoc:%s:%s\">\n",
	   node->name, parentNode->name);
    
    printf("      <dia:attribute name=\"obj_pos\">\n");
    printf("        <dia:point val=\"0,0\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"obj_bb\">\n");
    printf("        <dia:rectangle val=\"0,0;0,0\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"orth_points\">\n");
    printf("        <dia:point val=\"0,0\"/>\n");
    printf("        <dia:point val=\"0,0\"/>\n");
    printf("        <dia:point val=\"0,0\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"orth_orient\">\n");
    printf("        <dia:enum val=\"1\"/>\n");
    printf("        <dia:enum val=\"0\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"name\">\n");
    printf("        <dia:string/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"direction\">\n");
    printf("        <dia:enum val=\"0\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"ends\">\n");
    printf("        <dia:composite>\n");
    printf("          <dia:attribute name=\"role\">\n");
    printf("            <dia:string/>\n");
    printf("          </dia:attribute>\n");
    printf("          <dia:attribute name=\"multiplicity\">\n");
    printf("            <dia:string/>\n");
    printf("          </dia:attribute>\n");
    printf("          <dia:attribute name=\"arrow\">\n");
    printf("            <dia:boolean val=\"false\"/>\n");
    printf("          </dia:attribute>\n");
    printf("          <dia:attribute name=\"aggregate\">\n");
    printf("            <dia:enum val=\"0\"/>\n");
    printf("          </dia:attribute>\n");
    printf("        </dia:composite>\n");
    printf("        <dia:composite>\n");
    printf("          <dia:attribute name=\"role\">\n");
    printf("            <dia:string/>\n");
    printf("          </dia:attribute>\n");
    printf("          <dia:attribute name=\"multiplicity\">\n");
    printf("            <dia:string/>\n");
    printf("          </dia:attribute>\n");
    printf("          <dia:attribute name=\"arrow\">\n");
    printf("            <dia:boolean val=\"false\"/>\n");
    printf("          </dia:attribute>\n");
    printf("          <dia:attribute name=\"aggregate\">\n");
    printf("            <dia:enum val=\"1\"/>\n");
    printf("          </dia:attribute>\n");
    printf("        </dia:composite>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:connections>\n");
    printf("        <dia:connection handle=\"0\" "
	   "to=\"Node:%s\" connection=\"%d\"/>\n",
	   node->name, 1);

    printf("        <dia:connection handle=\"1\" "
	   "to=\"Node:%s\" connection=\"%d\"/>\n",
	   parentNode->name, conn);
    
    printf("      </dia:connections>\n");
    printf("    </dia:object>\n");
}



static void printClass(double *x, double *y, SmiNode *node)
{
    SmiNode *childNode;
    SmiNode *relatedNode;
    SmiElement *element;

    /* printf("    <dia:group>\n"); */

    printf("    <dia:object type=\"UML - Class\" "
	   "version=\"0\" id=\"Node:%s\">\n", node->name);
    printf("      <dia:attribute name=\"obj_pos\">\n");
    printf("        <dia:point val=\"%f,%f\"/>\n", *x, *y);
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"obj_bb\">\n");
    printf("        <dia:rectangle val=\"0.1,0.1;0.1,0.1\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"elem_corner\">\n");
    printf("        <dia:point val=\"%f,%f\"/>\n", *x, *y);
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"elem_width\">\n");
    printf("        <dia:real val=\"%f\"/>\n", 0.1);
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"elem_height\">\n");
    printf("        <dia:real val=\"%f\"/>\n", 0.1);
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"name\">\n");
    printf("        <dia:string>#%s#</dia:string>\n", node->name);
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"stereotype\">\n");
    printf("        <dia:string/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"abstract\">\n");
    printf("        <dia:boolean val=\"false\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"suppress_attributes\">\n");
    printf("        <dia:boolean val=\"false\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"suppress_operations\">\n");
    printf("        <dia:boolean val=\"true\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"visible_attributes\">\n");
    printf("        <dia:boolean val=\"true\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"visible_operations\">\n");
    printf("        <dia:boolean val=\"true\"/>\n");
    printf("      </dia:attribute>\n");

    printf("      <dia:attribute name=\"attributes\">\n");

    for(childNode = smiGetFirstChildNode(node);
	childNode;
	childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_SCALAR
	    || childNode->nodekind == SMI_NODEKIND_COLUMN) {
	    if (childNode->status != SMI_STATUS_OBSOLETE) {
		printf("        <dia:composite type=\"umlattribute\">\n");
		printf("          <dia:attribute name=\"name\">\n");
		printf("            <dia:string>#%s#</dia:string>\n",
		       childNode->name);
		printf("          </dia:attribute>\n");
		printf("          <dia:attribute name=\"type\">\n");
		printf("            <dia:string>#%s#</dia:string>\n",
		       getTypeName(childNode));
		printf("          </dia:attribute>\n");
		printf("          <dia:attribute name=\"value\">\n");
		printf("            <dia:string/>\n");
		printf("          </dia:attribute>\n");
		printf("          <dia:attribute name=\"visibility\">\n");
		printf("            <dia:enum val=\"0\"/>\n");
		printf("          </dia:attribute>\n");
		printf("          <dia:attribute name=\"abstract\">\n");
		printf("            <dia:boolean val=\"false\"/>\n");
		printf("          </dia:attribute>\n");
		printf("          <dia:attribute name=\"class_scope\">\n");
		printf("            <dia:boolean val=\"false\"/>\n");
		printf("          </dia:attribute>\n");
		printf("        </dia:composite>\n");
	    }
	}
    }

    printf("      </dia:attribute>\n");

    printf("      <dia:attribute name=\"operations\"/>\n");
    printf("      <dia:attribute name=\"template\">\n");
    printf("        <dia:boolean val=\"false\"/>\n");
    printf("      </dia:attribute>\n");
    printf("      <dia:attribute name=\"templates\"/>\n");
    printf("    </dia:object>\n");

    if (node->nodekind == SMI_NODEKIND_ROW) {

	relatedNode = NULL;
	switch (node->indexkind) {
	case SMI_INDEX_INDEX:
	case SMI_INDEX_REORDER:
	    break;
	case SMI_INDEX_EXPAND:  /* TODO: we have to do more work here! */
	    break;
	case SMI_INDEX_AUGMENT:
	case SMI_INDEX_SPARSE:
	    relatedNode = smiGetRelatedNode(node);
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;	    
	}

	if (!relatedNode) {
	    for (element = smiGetFirstElement(node);
		 element;
		 element = smiGetNextElement(element)) {
		relatedNode = smiGetElementNode(element);

		printAssiciation(node, relatedNode);
	    }
	} else {
	    printAssiciation(node, relatedNode);
	}
    }

    /* printf("    </dia:group>\n"); */

    *x += X_OFFSET;
    *y += Y_OFFSET;
    
}



static void printClasses(double *x, double *y, SmiModule *smiModule)
{
    SmiNode *smiNode;

    for(smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	smiNode;
	smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY)) {
	if (isGroup(smiNode)) {
	    printClass(x, y, smiNode);
	}
	if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	    printClass(x, y, smiNode);
	}
    }
}



static void printIndexAssociations(SmiModule *module)
{
    SmiNode *node, *relatedNode;
    SmiElement *element;
    
    for(node = smiGetFirstNode(module, SMI_NODEKIND_ROW);
	node;
	node = smiGetNextNode(node, SMI_NODEKIND_ROW)) {

	relatedNode = NULL;
	switch (node->indexkind) {
	case SMI_INDEX_INDEX:
	case SMI_INDEX_REORDER:
	    break;
	case SMI_INDEX_EXPAND:  /* TODO: we have to do more work here! */
	    break;
	case SMI_INDEX_AUGMENT:
	case SMI_INDEX_SPARSE:
	    relatedNode = smiGetRelatedNode(node);
	    break;
	case SMI_INDEX_UNKNOWN:
	    break;	    
	}

	if (!relatedNode) {
	    for (element = smiGetFirstElement(node);
		 element;
		 element = smiGetNextElement(element)) {
		relatedNode = smiGetElementNode(element);

		printAssiciation(node, relatedNode);
		
	    }
	} else {

	    printAssiciation(node, relatedNode);
	}
    }
}



int dumpDia(char *modulename, int flags)
{
    SmiModule    *smiModule;
    double	 x, y;

    if (!modulename) {
	fprintf(stderr,
		"smidump: united output not supported for dia format\n");
	exit(1);
    }

    smiModule = smiGetModule(modulename);
    if (!smiModule) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    printf("<?xml version=\"1.0\"?>\n");
    printf("<!-- This file has been generated by smidump " VERSION
	   ". Do not edit. -->\n");
    printf("<dia:diagram xmlns:dia=\"http://www.lysator.liu.se/~alla/dia/\">\n");
    printf("  <dia:diagramdata>\n");
    printf("    <dia:attribute name=\"background\">\n");
    printf("      <dia:color val=\"#ffffff\"/>\n");
    printf("    </dia:attribute>\n");
    printf("  </dia:diagramdata>\n");
    printf("  <dia:layer name=\"Background\" visible=\"true\">\n");
    
    x = X_OFFSET, y = Y_OFFSET;

    printClasses(&x, &y, smiModule);

    printf("  </dia:layer>\n");
    printf("</dia:diagram>\n");
    
    return 0;
}
