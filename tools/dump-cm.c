/*
 * dump-cm.c --
 *
 *      Operations to dump conceptual models for MIB modules.
 *
 * Copyright (c) 1999 A. Mueller, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-cm.c,v 1.5 2000/04/11 08:33:33 strauss Exp $
 */


/* 
 * -- TO DO --
 *
 * RMON2-MIB ifTable hat 0 Elements???? (sollte 1 sein)
 *  ---> statt SPARSE liegt eine EXPAND Beziehung vor
 *
 * Rangeueberpruefung noch einfuegen (linkLonelyTables)
 *
 * Kardinalitaeten
 *
 * Aggregationen ??
 *
 * Beschriftung der Links uberarbeiten.
 *
 * mehrere Connection Points benutzen.
 */





#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "smi.h"
#include "smidump.h"





typedef enum GraphCardinality {
  GRAPH_CARD_UNKNOWN      = 0,
  GRAPH_CARD_ONE_TO_ONE   = 1,
  GRAPH_CARD_ONE_TO_MANY  = 2,
  GRAPH_CARD_ZERO_TO_ONE  = 3,
  GRAPH_CARD_ZERO_TO_MANY = 4
} GraphCardinality;

typedef enum GraphConnection {
    GRAPH_CON_UNKNOWN       = 0,
    GRAPH_CON_AGGREGATION   = 1,
    GRAPH_CON_DEPENDENCY    = 2,
    GRAPH_CON_ASSOCIATION   = 3
} GraphConnection;

typedef enum GraphEnhIndex {
    GRAPH_ENHINDEX_UNKNOWN      = 0,
    GRAPH_ENHINDEX_NOTIFICATION = 1,
    GRAPH_ENHINDEX_TYPES        = 2,
    GRAPH_ENHINDEX_NAMES        = 3,
    GRAPH_ENHINDEX_INDEX        = 4
} GraphEnhIndex;

typedef struct GraphNode{
    struct GraphNode *nextPtr;
    SmiNode          *smiNode;
    int              group;
    int              print;
    int              connections;
    int              conPoints[7];
    float            x,y,w,h;
} GraphNode;

typedef struct GraphEdge {
    struct GraphEdge *nextPtr;         
    GraphNode        *startNode;
    GraphNode        *endNode;
    SmiIndexkind     indexkind;
    GraphConnection  connection;
    GraphCardinality cardinality;
    GraphEnhIndex    enhancedindex;
    int              print;
} GraphEdge;

typedef struct Graph {
    GraphNode *nodes;
    GraphEdge *edges;
} Graph;


static const int  SUFFIXCOUNT   = 1;
static const int  SUPPORTCOUNT  = 2;
static char  *suffix[1]         = {"OrZero"};
static char  *support[2]        = {"RowStatus", "StorageType"};

/* layout for the nodes */
static const float HEADFONTSIZETABLE   = 0.51;
static const float HEADSPACESIZETABLE  = 0.6;
static const float HEADFONTSIZESCALAR  = 0.545;
static const float HEADSPACESIZESCALAR = 0.6;
static const float ATTRFONTSIZE        = 0.48;
static const float ATTRSPACESIZE       = 2.4;
static const float RECTCORRECTION      = 0.85;
static const float EDGEYSPACING        = 2;

/* global graph layout */
static const float YSPACING            = 3;
static const float XSPACING            = 3;
static const float NEWLINEDISTANCE     = 40;
static const float XOFFSET             = 3;
static const float YOFFSET             = 3;

static SmiModule *module;
static int       XPLAIN;


#define max(a, b) ((a < b) ? b : a)
#define min(a, b) ((a < b) ? a : b)








/* ------ Graph primitives ------                                            */





/*
 * graphInsertNode
 *
 *          Inserts a new node into an existing graph.
 *
 * Input  : graph   = pointer to a graph structure
 *          smiNode = pointer to a SmiNode
 *
 * Result : pointer to the new graph structure
 */
static Graph *graphInsertNode(Graph *graph, SmiNode *smiNode)
{
    int i;
    
    GraphNode *newNode;
    GraphNode *tNode;
    GraphNode *lastNode;

    newNode              = xmalloc(sizeof(GraphNode));
    newNode->nextPtr     = NULL;
    newNode->smiNode     = smiNode;
    newNode->group       = 0; /* no grouping */
    newNode->print       = 0;
    newNode->connections = 0;
    
    newNode->x           = 0.0; /* rectangle size */
    newNode->y           = 0.0;   
    newNode->w           = 0.0;
    newNode->h           = 0.0;

    for (i = 0; i < 7; i++) {
	newNode->conPoints[i] = 0;
    }
    
    if (graph->nodes == NULL) {
	graph->nodes = newNode;
	return graph;
    }

    lastNode = NULL; 
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	lastNode = tNode;
    }

    lastNode->nextPtr = newNode;

    return graph;
}

/*
 * graphInsertEdge
 *
 *          Inserts a new edge into an existing list of edges.
 *
 * Input  : graph     = pointer to an edge structure
 *          startNode = pointer to the starting node of the edge
 *          endNode   = pointer to the ending node of the edge
 *          indexkind = type of relationship between the two nodes
 *
 * Result : pointer to the new edge list
 */
static Graph *graphInsertEdge(Graph *graph, GraphNode *startNode,
			      GraphNode *endNode,
			      SmiIndexkind indexkind,
			      GraphEnhIndex enhancedindex)
{
    GraphEdge *newEdge;
    GraphEdge *tEdge;
    GraphEdge *lastEdge;

    newEdge                = xmalloc(sizeof(GraphEdge));
    newEdge->nextPtr       = NULL;
    newEdge->startNode     = startNode;
    newEdge->endNode       = endNode;
    newEdge->indexkind     = indexkind;
    newEdge->connection    = GRAPH_CON_AGGREGATION;
    newEdge->enhancedindex = enhancedindex;
    newEdge->print         = 0;

    startNode->connections++;
    endNode->connections++;
    
    switch (newEdge->indexkind) {
    case SMI_INDEX_AUGMENT : 
	newEdge->cardinality = GRAPH_CARD_ONE_TO_ONE; 
	break;
    case SMI_INDEX_SPARSE  : 
	newEdge->cardinality = GRAPH_CARD_ONE_TO_MANY; 
	break;
    case SMI_INDEX_EXPAND  : 
	newEdge->cardinality = GRAPH_CARD_UNKNOWN; 
	break;
    case SMI_INDEX_REORDER : 
	newEdge->cardinality = GRAPH_CARD_ONE_TO_ONE; 
	break;
    case SMI_INDEX_INDEX   : 
	newEdge->cardinality = GRAPH_CARD_UNKNOWN; 
	break;
    case SMI_INDEX_UNKNOWN : 
	newEdge->cardinality = GRAPH_CARD_UNKNOWN; 
	break;
    }

    if (graph->edges == NULL) {
	graph->edges = newEdge;
	return graph;
    }

    lastEdge = NULL; 
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	lastEdge = tEdge;
    }

    lastEdge->nextPtr = newEdge;

    return graph;
}

/*
 * graphExit
 *
 * Frees all memory allocated by the graph and calls smiExit().
 */
static void graphExit(Graph *graph)
{
    GraphEdge *tEdge, *dummyEdge;
    GraphNode *tNode, *dummyNode;

    if (graph) {

	dummyNode = NULL;
	tNode = graph->nodes;
	while (tNode != NULL) {
	    dummyNode = tNode;
	    tNode = tNode->nextPtr;
      
	    xfree(dummyNode);
	}
    
	dummyEdge = NULL;
	tEdge = graph->edges;
	while (tEdge != NULL) {
	    dummyEdge = tEdge;
	    tEdge = tEdge->nextPtr;
      
	    xfree(dummyEdge);
	}
    
	xfree(graph);
    }
  
    smiExit();
}

/*
 * graphGetFirstEdge
 *
 * Returns the first edge of the graph.
 */
static GraphEdge *graphGetFirstEdge(Graph *graph) 
{
    return (graph->edges) ? (GraphEdge *)graph->edges : NULL;  
}

/*
 * graphGetNextEdge
 *
 * Returns the next edge after the given edge.
 */
GraphEdge *graphGetNextEdge(Graph *graph, GraphEdge *edge) 
{
    GraphEdge *tEdge;

    if (!graph->edges || !edge) {
	return NULL;
    }
    
/*    for (tEdge = graphGetFirstEdge(graph); tEdge; tEdge = tEdge->nextPtr) {
	if (tEdge->startNode->smiNode->name ==
	    edge->startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name ==
	    edge->endNode->smiNode->name) break;
	    } */
  
    if (!edge->nextPtr) {
	return NULL;
    }
    
    return edge->nextPtr;
}

/*
 * graphGetLastEdge
 *
 * Returns the last edge of the given graph.
 */
static GraphEdge *graphGetLastEdge(Graph *graph) 
{
    GraphEdge *tEdge;

    if (!graph->edges) return NULL;

    for (tEdge = graph->edges; tEdge->nextPtr; tEdge = tEdge->nextPtr) {}

    return tEdge;
}

/*
 * graphGetFirstEdgeByNode
 *
 * Returns the first edge adjacent to the given node (as startNode or EndNode).
 */
static GraphEdge *graphGetFirstEdgeByNode(Graph *graph, GraphNode *node)
{
    GraphEdge *tEdge;

    if (!graph || !node) {
	return NULL;
    }
    
    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == node->smiNode->name ||
	    tEdge->endNode->smiNode->name == node->smiNode->name) break;
    }

    if (!tEdge) {
	return NULL;
    }
    
    return tEdge;
}

/*
 * graphGetNextEdgeByNode
 *
 * Returns the next edge adjacent to the given node (as startNode or EndNode)
 * after the given edge.
 */
static GraphEdge *graphGetNextEdgeByNode(Graph *graph, 
					 GraphEdge *edge,
					 GraphNode *node) 
{
    GraphEdge *tEdge;

    if (!graph || !node) {
	return NULL;
    }
    
    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name ==
	    edge->startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name ==
	    edge->endNode->smiNode->name) break;
    }
  
    for (tEdge = graphGetNextEdge(graph,tEdge);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == node->smiNode->name ||
	    tEdge->endNode->smiNode->name == node->smiNode->name) break;
    }

    if (!tEdge) {
	return NULL;
    }
    
    return tEdge;
}

/*
 * graphCheckForRedundantEdge
 *
 * Finds redundant edges and returns 1 if one is found and 0 otherwise.
 */
static int graphCheckForRedundantEdge(Graph *graph,
			       GraphNode *startNode,
			       GraphNode *endNode)
{
    GraphEdge *tEdge;

    if (!graph || !startNode || !endNode) {
	return 0;
    }
    
    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name == endNode->smiNode->name) {
	    return 1;
	}
    }
    
    return 0;
}

/*
 * graphGetLastNode
 *
 * Returns the last node of the list representing the graph nodes.
 */
static GraphNode *graphGetLastNode(Graph *graph) 
{
    GraphNode *tNode;

    if (!graph->nodes) {
	return NULL;
    }
    
    for (tNode = graph->nodes; tNode->nextPtr; tNode = tNode->nextPtr) {}

    return tNode;
}

/*
 * graphGetFirstNode
 *
 * Returns the first node of the graph.
 */
static GraphNode *graphGetFirstNode(Graph *graph)
{
    return (graph->nodes) ? (GraphNode *)graph->nodes : NULL;
}

/*
 * graphGetNextNode
 *
 * Returns the next graph node after the given node.
 */
static GraphNode *graphGetNextNode(Graph *graph, GraphNode *node) 
{
    GraphNode *tNode;

    if (!graph->nodes || !node) {
	return NULL;
    }
    
    for (tNode = graphGetFirstNode(graph); tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->name == node->smiNode->name) break;
    }
  
    if (tNode->nextPtr == NULL) {
	return NULL;
    }
    
    return tNode->nextPtr;
}

/*
 * graphGetNode
 *
 * Returns the graph node containing smiNode.
 */
static GraphNode *graphGetNode(Graph *graph, SmiNode *smiNode)
{
    GraphNode *tNode;

    if (!smiNode || !graph) {
	return NULL;
    }
    
    for (tNode = graphGetFirstNode(graph); 
	 tNode; 
	 tNode = graphGetNextNode(graph, tNode)) {
	if (tNode->smiNode->name == smiNode->name) {
	    break;
	}
    }

    return tNode;
}

/*
 * graphShowNodes
 *
 * Prints the nodes of the graph.
 */
static void graphShowNodes(Graph *graph) 
{
    GraphNode *tNode;
  
    if (!graph->nodes) {
	printf("No nodes!\n");
	return;
    }

    for (tNode = graphGetFirstNode(graph); 
	 tNode; 
	 tNode = graphGetNextNode(graph,tNode)) {
	printf("Node : %40s\n",tNode->smiNode->name);
    }
}

/*
 * graphShowEdges
 *
 * Prints the edges with their attributes in the following order :
 *    - start node
 *    - reason for the link
 *    - connection type in UML
 *    - cardinality
 *    - index relationship derived from the row-objects of the tables
 *
 * AGGR.   = aggregation
 * DEPEND. = dependency
 */
static void graphShowEdges(Graph *graph) 
{
    GraphEdge  *tEdge;
    
    if (!graph->edges) {
	printf("No edges!\n");
	return;
    }
  
    for (tEdge = graphGetFirstEdge(graph); 
	 tEdge; 
	 tEdge = graphGetNextEdge(graph,tEdge)) {
	printf("Edge : %32s -",tEdge->startNode->smiNode->name);

	switch (tEdge->enhancedindex) {
	case GRAPH_ENHINDEX_UNKNOWN :
	    printf("   [UNKNOWN] ");
	    break;
	case GRAPH_ENHINDEX_NOTIFICATION :
	    printf(" [NOTIFICATION] ");
	    break;
	case GRAPH_ENHINDEX_NAMES :
	    printf("    [NAMES]     ");
	    break;
	case GRAPH_ENHINDEX_TYPES :
	    printf("    [TYPES]     ");
	    break;
	case GRAPH_ENHINDEX_INDEX :
	    printf("    [INDEX]     ");
	    break;
	}
	
	switch (tEdge->connection) {
	case GRAPH_CON_AGGREGATION:
	    printf("   AGGR. ");
	    break;
	case GRAPH_CON_DEPENDENCY:
	    printf(" DEPEND. ");
	    break;
	case GRAPH_CON_ASSOCIATION:
	    printf("  ASSOC. ");
	    break;    
	case GRAPH_CON_UNKNOWN:
	    break;
	}

	switch (tEdge->cardinality) {
	case GRAPH_CARD_UNKNOWN      :
	    printf(" (UNKNOWN) ");
	    break;
	case GRAPH_CARD_ONE_TO_ONE   :
	    printf("     (1:1) ");
	    break;
	case GRAPH_CARD_ONE_TO_MANY  :
	    printf("     (1:n) ");
	    break;
	case GRAPH_CARD_ZERO_TO_ONE  :
	    printf("     (0:1) ");
	    break;
	case GRAPH_CARD_ZERO_TO_MANY :
	    printf("     (0:n) ");
	    break;
	}

	switch (tEdge->indexkind) {
	case SMI_INDEX_UNKNOWN  :
	    printf(" UNKNOWN ");
	    break;
	case SMI_INDEX_INDEX    :
	    printf("   INDEX ");
	    break;
	case SMI_INDEX_AUGMENT  :
	    printf(" AUGMENT ");
	    break;
	case SMI_INDEX_SPARSE   :
	    printf("  SPARSE ");
	    break;
	case SMI_INDEX_EXPAND   :
	    printf("  EXPAND ");
	    break;
	case SMI_INDEX_REORDER  :
	    printf(" REORDER ");
	    break;
	}

	printf("- %s\n",tEdge->endNode->smiNode->name);
    
#if 0
	printf("[");
	for (smiElement = smiGetFirstElement(
	    smiGetFirstChildNode(tEdge->startNode->smiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    printf(" %s ",smiGetElementNode(smiElement)->name);
	}
	
	printf("] - [");    
	for (smiElement = smiGetFirstElement(
	    smiGetFirstChildNode(tEdge->endNode->smiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    printf(" %s ",smiGetElementNode(smiElement)->name);
	}
    
	printf("]\n\n");    
#endif
    
    }
}





/* ------ algorithm primitives ------                                        */




/*
 * cmpSmiNodes
 *
 * Compares two SmiNode and returns 1 if they are equal and 0 otherwise.
 */
static int cmpSmiNodes(SmiNode *node1, SmiNode *node2)
{
    SmiModule *module1, *module2;

    module1 = smiGetNodeModule(node1);
    module2 = smiGetNodeModule(node2);
    
    if (!node1 || !node2) return 0;
    
    if (strcasecmp(node1->name, node2->name) == 0 &&
	strcasecmp(module1->name, module2->name) == 0) {
	return 1;
    }

    return 0;
}

/*
 * algCountElements
 *
 * Returns the number of elements in a given row entry.
 */
static int algCountElements(SmiNode *smiNode) 
{
    int          count;
    SmiElement   *smiElement;

    if (smiNode->nodekind != SMI_NODEKIND_ROW) {
	return 0;
    }
    
    count = 0;
    for (smiElement = smiGetFirstElement(smiNode);
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	count++;
    }

    return count;
}

/*
 * algInsertEdge
 *
 * Inserts an edge in a given graph. The edge is adjacent to snode 
 * and enode.
 * The type of edge is given in indexkind and the enhanced index as
 * an additional information in enhancedindex.
 * Nodes which are not loaded yet into the node list of the graph
 * are added (nodes from imported MIBs).
 */
static Graph *algInsertEdge(Graph *graph, SmiNode *snode, SmiNode *enode, 
			    SmiIndexkind indexkind,
			    GraphEnhIndex enhancedindex) 
{
    GraphNode *startNode;
    GraphNode *endNode;

    if (!graph) return NULL;
    if (snode->status == SMI_STATUS_OBSOLETE ||
	enode->status == SMI_STATUS_OBSOLETE) return graph;

    startNode = graphGetNode(graph, snode);
    endNode   = graphGetNode(graph, enode);

    /* insert imported nodes into graph if needed */
    if (startNode == NULL) {
	graph = graphInsertNode(graph, snode);
	startNode = graphGetNode(graph, snode);
    }
    if (endNode == NULL) {
	graph = graphInsertNode(graph, enode);
	endNode = graphGetNode(graph, enode);
    }  

    if (graphCheckForRedundantEdge(graph, startNode, endNode) == 0) {
	graph = graphInsertEdge(graph, startNode, endNode,
				indexkind,
				enhancedindex); 
    }
    
    return graph;
}

/*
 * strgetpfx
 *
 * Returns the prefix of a given string.
 */
static char *strgetpfx(const char *s1)
{
    char *ss1;
    int  i;

    ss1 = strdup(s1);
    for (i = 0; i < strlen(s1); i++) {
	ss1[i] = 0;
    }
    
    for (i = 0; s1[i]; i++) {
	if (isupper(s1[i])) break;
    }

    strncpy(ss1, s1, i);

    return strdup(ss1);
}

/*
 * algGetGroupByPrefix
 *
 * Returns the group number associated with the prefix of the
 * given node. If there is no group the result is 0 for no
 * grouping.
 */
static int algGetGroupByPrefix(Graph *graph, SmiNode *smiNode)
{
    GraphNode *tNode;
    
    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	if (tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR &&
	    !graphGetFirstEdgeByNode(graph, tNode)) {
	    if (strcmp(strgetpfx(smiNode->name),
		       strgetpfx(tNode->smiNode->name)) == 0) {
		return tNode->group;
	    }
	}
    }

    return 0;
}

/*
 * strpfxlen
 *
 * Returns the number of identical characters at the beginning of s1 and s2.
 */
static int strpfxlen(const char *s1, const char *s2)
{
    int i;
  
    for (i = 0; s1[i] && s2[i]; i++) {
	if (s1[i] != s2[i]) {
	    break;
	}
    }
  
    return i;
}

/*
 * algGetNumberOfGroups
 *
 * Returns the number of groups.
 */
static int algGetNumberOfGroups(Graph *graph)
{
    GraphNode *tNode;
    int       maxGroup;

    maxGroup = 0;
    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	maxGroup = max(maxGroup, tNode->group); 
    }

    return maxGroup;
}

/*
 * algPrintGroup
 *
 * Prints the group with the number group.
 */
static void algPrintGroup(Graph *graph, int group)
{
    GraphNode *tNode;
    
    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {   
	if (tNode->group == group) {
	    printf("%2d - %35s\n",group, tNode->smiNode->name);
	}
    }
}

/*
 * algGetTypeName
 *
 * Returns the name of the data type used in smiNode.
 */
static char *algGetTypeName(SmiNode *smiNode)
{
    char *type;
    SmiType *smiType, *parentType;
  
    smiType = smiGetNodeType(smiNode);
  
    if (!smiType || smiNode->nodekind == SMI_NODEKIND_TABLE)
	return NULL;
  
    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
	parentType = smiGetParentType(smiType);
	smiType = parentType;
    }
  
    type = xstrdup(smiType->name);
    return type;
}

/*
 * algCountElementsFromOtherTables
 *
 * Returns the number of index objects derived from other tables than
 * the given one.
 */
static int algCountElementsFromOtherTables(SmiNode *smiNode)
{
    SmiElement *smiElement;
    SmiNode    *tNode;
    int        elems = 0;
    
    for (smiElement = smiGetFirstElement(smiNode);
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {

	tNode = smiGetElementNode(smiElement);
	
	/*if (smiGetParentNode(smiGetParentNode(tNode))->name !=
	    smiGetParentNode(smiNode)->name) {
	    elems++;
	} */

	if (cmpSmiNodes(smiGetParentNode(smiGetParentNode(tNode)),
			smiGetParentNode(smiNode)) != 1) {
	    elems++;
	}
    }

    return elems;
}

/*
 * algGetNumberOfRows
 *
 * Returns the number of rows =
 *   number of elements in smiNode - number of rows 
 *   + RowStatus-Objects + StorageType-Objects
 *
 * If the number is > 0 it is only a supporting table (like entLPMappingTable) 
 * and if the number is < 0 it is a "full" table (like ifTable).
 *
 * Subroutine for algCheckForDependency.
 */
static int algGetNumberOfRows(SmiNode *smiNode)
{
    SmiNode *tSmiNode;
    SmiNode *table;
    int     i, elemCount;

    elemCount = 0;
    table = smiNode;
    tSmiNode = smiGetFirstChildNode(table);

    elemCount = algCountElements(tSmiNode) -
	algCountElementsFromOtherTables(tSmiNode);

    for (tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_COLUMN);
	 tSmiNode;
	 tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_COLUMN)) {

	/*if (table->name != smiGetParentNode(
	    smiGetParentNode(tSmiNode))->name) {
	    break;
	} */
	if (cmpSmiNodes(table, smiGetParentNode(smiGetParentNode(tSmiNode)))
	    != 1) {
	    break;
	}
	
	for (i = 0;
	     i < SUPPORTCOUNT;
	     i++) {
	    if (strcmp(algGetTypeName(tSmiNode), support[i]) == 0) break;
	}

	if (i == SUPPORTCOUNT) elemCount--;
    }

    return elemCount;
}

/*
 * algFindEqualType
 *
 * Looks in all tables indices for an equal type to the type used in typeNode.
 * It returns the first table found which is different to startTable.
 *
 * Subroutine for algCheckForDependency. 
 */
static SmiNode *algFindEqualType(SmiNode *startTable, SmiNode *typeNode)
{
    SmiElement *smiElement;
    SmiNode    *tSmiNode;
    char       *typeName;

    typeName = algGetTypeName(typeNode);

    for (tSmiNode = smiGetFirstNode(module, SMI_NODEKIND_TABLE);
	 tSmiNode;
	 tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_TABLE)) {
	if (cmpSmiNodes(tSmiNode, startTable) == 1) break;
	/* if (tSmiNode->name == startTable->name) break; */
    
	for (smiElement = smiGetFirstElement(smiGetFirstChildNode(tSmiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    /* found type matching ? */
	    if (strcmp(typeName,
		       algGetTypeName(smiGetElementNode(smiElement)))== 0) {
		return tSmiNode;
	    }
	}
    }
  
    return NULL;
}



/* -------------- main functions ------------------------------------------- */



/*
 * algCheckForExpandRel
 *
 * Checks the given table for an expand relationship to an other table.
 *
 * 1. gets the expanded table (father node of smiNode)
 * 2. gets the base table by walking through the elements of the smiNode
 *    starting at the end. The first table found which is different from the
 *    expanded table is the first candidate for the base table.
 * 3. comparing the number of elements in both tables
 *    if the number in the expanded table is greater -> 5.
 *    if the number in the base table is greater     -> 4.
 * 4. getting a second base table candidate :
 *    It is possible that a table expands an other table which is expanded
 *    by this.
 *    Therefore it is hard to track the base table.
 *    - scanning all tables which are different from the expanded table
 *    - the base table must have :
 *         - least elements
 *         - the same elementes must occur as in the expanded table
 * 5. the elements in both tables are checked for equality 
 */
static Graph *algCheckForExpandRel(Graph *graph, SmiNode *smiNode) 
{
    SmiNode      *tNode;
    SmiNode      *baseTable;
    SmiNode      *expTable;
    SmiElement   *smiElement;
    SmiElement   *findElement;
    unsigned int refcounter;
    unsigned int basecounter;

    if (!smiNode) return graph;

    expTable = smiGetParentNode(smiNode);  

    /* count the elements in the given table <- father of smiNode */
    refcounter = algCountElements(smiNode);

    /* find the base table <- via the last element which does not refer to 
       the expTable */
    baseTable = NULL;
    for (smiElement = smiGetFirstElement(smiNode); 
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {  
	tNode = smiGetElementNode(smiElement);
	tNode = smiGetParentNode(tNode);
	tNode = smiGetParentNode(tNode);

	/*if (tNode->name == expTable->name) {
	    break;
	    }*/
	if (cmpSmiNodes(tNode, expTable) == 1) break;
	
	baseTable = tNode;
    }  

    if (!baseTable) return graph;

    /* count the elements in the basetable */
    basecounter = algCountElements(smiGetFirstChildNode(baseTable));

    /* are baseTable and expTable identical ? */
    if (basecounter >= refcounter) {

	/* searching for new base table candidate in order to handle multiple
	   indices */
	for (baseTable = smiGetNextNode(baseTable, SMI_NODEKIND_TABLE);
	     baseTable;
	     baseTable = smiGetNextNode(baseTable, SMI_NODEKIND_TABLE)) {
      
	    basecounter = algCountElements(smiGetFirstChildNode(baseTable));
      
	    if (basecounter < refcounter) {

		for (smiElement = smiGetFirstElement(
		    smiGetFirstChildNode(expTable)); 
		     smiElement; 
		     smiElement = smiGetNextElement(smiElement)) {
		    tNode = smiGetElementNode(smiElement);

		    /*if (smiGetParentNode(smiGetParentNode(tNode))->name == 
		      expTable->name) break; */
		    if (cmpSmiNodes(smiGetParentNode(smiGetParentNode(tNode)),
				    expTable) == 1) break;
	  
		    for (findElement = smiGetFirstElement(
			smiGetFirstChildNode(baseTable)); 
			 findElement; 
			 findElement = smiGetNextElement(findElement)) {
			/*if (tNode->name ==
			    smiGetElementNode(findElement)->name) {
			    break;
			    }*/
			if (cmpSmiNodes(tNode, smiGetElementNode(findElement))
			    == 1) break;
		    }
	  
		    if (!findElement) {
			return graph;
		    }
		}	
		break;
	    }
	}
    
	if (!baseTable) {
	    return graph;
	}
    }

    for (smiElement = smiGetFirstElement(smiGetFirstChildNode(baseTable)); 
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {
	tNode = smiGetElementNode(smiElement);

	for (findElement = smiGetFirstElement(smiGetFirstChildNode(expTable)); 
	     findElement; 
	     findElement = smiGetNextElement(findElement)) {
	    /*if (tNode->name == smiGetElementNode(findElement)->name) {
		break;
	    } */
	    if (cmpSmiNodes(tNode, smiGetElementNode(findElement)) == 1)
		break;
	}
    
	if (!findElement) {
	    return graph;
	}
    }
  
    graph = algInsertEdge(graph, baseTable, expTable, SMI_INDEX_EXPAND,
			  GRAPH_ENHINDEX_INDEX);  

    return graph;
}

/*
 * algCheckForSparseRel
 *
 * Checks the given table for a sparse relationship to an other table. 
 *
 * Criterias for a sparse relationship :
 *    - same number of index elements in both tables
 *    - the same order of this elements
 * 1. Getting the basetable via the last element in the index of smiNode.
 * 2. comparing the number of elements
 */
static Graph *algCheckForSparseRel(Graph *graph, SmiNode *smiNode) 
{
    SmiNode      *tNode = NULL;
    SmiNode      *table;
    SmiElement   *smiElement;
    SmiElement   *findElement;
    unsigned int refcounter;
    unsigned int basecounter;

    if (!smiNode) return graph;

    table = smiGetParentNode(smiNode);

    basecounter = algCountElements(smiNode);

    /* zuerst wird die richtige Tabelle gesucht. Dazu wird der 
       Vaterknoten des letzten Elementes herangezogen, 
       um Mehrfachindizierungen zu finden */
    for (smiElement = smiGetFirstElement(smiNode);
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {
	tNode = smiGetElementNode(smiElement);    
    }

    if (! tNode) {
	return graph;
    }

    tNode = smiGetParentNode(tNode);  

    /* Ueberpruefung auf Tabellengleichheit */
    /*if (table->name == smiGetParentNode(tNode)->name) return graph;*/
    if (cmpSmiNodes(table, smiGetParentNode(tNode)) == 1) return graph;
  
    refcounter = algCountElements(tNode);

    /* same number of elements ? */
    if (refcounter != basecounter) return graph;

    table = smiGetParentNode(tNode);

    refcounter = 0;
    for (smiElement = smiGetFirstElement(smiNode); 
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {

	/* Tabelle holen */
	tNode = smiGetElementNode(smiElement);
	tNode = smiGetParentNode(tNode);
	tNode = smiGetParentNode(tNode);

	if (tNode->name == table->name) refcounter++;
	else {
	    /* suche, ob das Element nicht doch im Knoten vorkommt, da evtl. 
	       im falschen Vaterknoten gesucht wurde 
	       (kann bei Mehrfachindizierungen vorkommen */
	    tNode = smiGetElementNode(smiElement);
	    for (findElement = smiGetFirstElement(
		smiGetFirstChildNode(table)); 
		 findElement; 
		 findElement = smiGetNextElement(findElement)) {
		/*if (smiGetElementNode(findElement)->name == tNode->name) {
		    refcounter++;
		} */
		if (cmpSmiNodes(smiGetElementNode(findElement), tNode) == 1)
		    refcounter++;
	    }
	}
    }  

    /* Stimmen alle Elemente ueberein ? */
    if (refcounter != basecounter) return graph;

    graph = algInsertEdge(graph, table, smiGetParentNode(smiNode), 
			  SMI_INDEX_SPARSE, GRAPH_ENHINDEX_INDEX); 
    return graph;
}

/*
 * algCheckForReOrderRel
 *
 * Checks the given table for a reorder relationship to an other table.
 *
 * Criterias for reoder relationships :
 *    - same number of elements
 *    - same elements must occur in a different order
 *
 * 1.
 */
static Graph *algCheckForReOrderRel(Graph *graph, SmiNode *smiNode) 
{
    SmiNode      *baseTable;
    SmiNode      *reorderTable;
    SmiNode      *tNode;
    GraphNode    *tGrNode;  /* nicht auf NULL initialisieren ! */
    SmiElement   *smiElement;
    SmiElement   *findElement;
    unsigned int c1,c2;

    if (!smiNode) return graph;

    reorderTable = smiGetParentNode(smiNode);

    smiElement = smiGetFirstElement(smiNode);
    tNode = smiGetElementNode(smiElement);
    baseTable = smiGetParentNode(smiGetParentNode(tNode));

    /* identical tables ? */
    /*if (baseTable->name == reorderTable->name) return graph;*/
    if (cmpSmiNodes(baseTable, reorderTable) == 1) return graph;
    
    /* same number of elements ? */
    if (algCountElements(smiGetFirstChildNode(baseTable)) !=
	algCountElements(smiGetFirstChildNode(reorderTable))) 
	return graph;

    /* are the same elements present in each table ? */
    for (smiElement = smiGetFirstElement(smiNode);
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {

	tNode = smiGetElementNode(smiElement);

	for (findElement = smiGetFirstElement(smiGetFirstChildNode(baseTable));
	     findElement;
	     findElement = smiGetNextElement(findElement)) {
	    /*if (tNode->name == smiGetElementNode(findElement)->name) break;*/
	    if (cmpSmiNodes(tNode, smiGetElementNode(findElement)) == 1) break;
	}
    
	/* no, they are not */
	if (!findElement) {
	    break;
	}
    }

    /* looking for other baseTable */
    if (smiElement) {

	for (tGrNode = graphGetFirstNode(graph);
	     tGrNode;
	     tGrNode = graphGetNextNode(graph, tGrNode)) {
      
	    baseTable = NULL;
	    if (tGrNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {
		
		baseTable = tGrNode->smiNode;

		if (algCountElements(smiGetFirstChildNode(baseTable)) !=
		    algCountElements(smiGetFirstChildNode(reorderTable))) 
		    continue;	
	
		for (smiElement = smiGetFirstElement(smiNode);
		     smiElement;
		     smiElement = smiGetNextElement(smiElement)) {
		    tNode = smiGetElementNode(smiElement);
		    for (findElement = smiGetFirstElement(
			smiGetFirstChildNode(baseTable));
			 findElement;
			 findElement = smiGetNextElement(findElement)) {
			/*if (tNode->name ==
			    smiGetElementNode(findElement)->name) break;*/
			if (cmpSmiNodes(tNode,
					smiGetElementNode(findElement)) == 1)
			    break;
		    }	  
		    /* not the same elements */
		    if (!findElement) {
			break;
		    }
		}
	    }
	}
    }

    /* no new baseTable found */
    if (!tGrNode) {
	return graph;
    }

    /* do the elemnts appear in the same order ? */
    c1 = 0; /* pos. of element in the reorder table */
    for (smiElement = smiGetFirstElement(smiNode);
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	c1++;
	tNode = smiGetElementNode(smiElement);

	c2 = 0; /*pos. of the element in the basetable */
	for (findElement = smiGetFirstElement(smiGetFirstChildNode(baseTable));
	     findElement;
	     findElement = smiGetNextElement(findElement)) {
	    c2++;
      
	    if (c2 == c1) {
		if (cmpSmiNodes(tNode, smiGetElementNode(findElement)) == 0)
		  break; else return graph;
	   /*if (tNode->name != smiGetElementNode(findElement)->name) break;
	     else return graph; *//* yes, they do */
	    }
	}
    }

    /* Table passed all tests -> inserting new reorder edge */
    graph = algInsertEdge(graph, baseTable, reorderTable,
			  SMI_INDEX_REORDER, GRAPH_ENHINDEX_INDEX);
  
    return graph;
}

/*
 * algGetIndexkind
 *
 * Gets the indexkind of the given table. The row object of the table is
 * passed to this function.
 * Therefore three subfunctions are called to get. 
 *   - algChechForExpandRel
 *   - algCheckForSparseRel
 *   - algCheckForReOrderRel
 * Look there for further information.
 */
static Graph *algGetIndexkind(Graph *graph, SmiNode *table) 
{
    graph = algCheckForExpandRel(graph, table);
    graph = algCheckForSparseRel(graph, table);
    graph = algCheckForReOrderRel(graph, table);

    return graph;
}

/*
 * algLinkTables
 *
 * Links the tables of the given module.
 *
 *  1. Getting the tables and the scalars from the given module.
 *  2. Linking the tables :
 *     - AUGMENT no work to do just adding the correspondend edge
 *     - for other relationships the subfunction algGetIndexkind is called
 *       look there for further information  
 */
static Graph *algLinkTables(Graph *graph) 
{
    GraphNode  *tGraphNode;
    SmiNode    *node; 
    SmiNode    *tSmiNode;

    /* get tables and scalars from the MIB module */
    for (node = smiGetFirstNode(module, SMI_NODEKIND_TABLE);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_TABLE)) {
	if (node->status != SMI_STATUS_OBSOLETE)
	    graph = graphInsertNode(graph, node);
    }
    for (node = smiGetFirstNode(module, SMI_NODEKIND_SCALAR);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_SCALAR)) {
	if (node->status != SMI_STATUS_OBSOLETE)
	    graph = graphInsertNode(graph, node);
    }

    /* linking the tables */
    for (tGraphNode = graphGetFirstNode(graph); 
	 tGraphNode; 
	 tGraphNode = graphGetNextNode(graph, tGraphNode)) {

	node = tGraphNode->smiNode;

	if (node->nodekind == SMI_NODEKIND_TABLE) {
	    node = smiGetFirstChildNode(node);

	    if (node->nodekind == SMI_NODEKIND_ROW) {

		/* get the relationship between the tables and insert
		   the edges */
		if (node->indexkind == SMI_INDEX_INDEX) 
		    graph = algGetIndexkind(graph, node);

		if (node->indexkind == SMI_INDEX_AUGMENT) {
		    tSmiNode = node;
		    node = smiGetRelatedNode(node);
		    node = smiGetParentNode(node);

		    graph = algInsertEdge(graph,
					  node,
					  smiGetParentNode(tSmiNode), 
					  SMI_INDEX_AUGMENT,
					  GRAPH_ENHINDEX_INDEX);
		}
	    }
	}
    }

    if (XPLAIN==1) {
	printf("--- First phase - getting the nodes and linking the tables \n\n");
	graphShowNodes(graph);
	printf("\n");
	graphShowEdges(graph);
    }

    return graph;
}

/*
 * algCheckLinksByName
 *
 * Reordering the connections by looking at the names.
 * Related objects are linked (if their names are related).
 * Every expand relationship is examined. Therefore number of
 * identical characters at the beginning of the both table names is
 * counted.
 * Then every sparse relationship (only the ending node) is checked if 
 * there is better connection with more identical characters at the beginning.
 * The starting node must be same as in the expand relationship to make sure
 * that there are no unrelated tables are linked.
 */
static Graph *algCheckLinksByName(Graph *graph) 
{
    GraphEdge *tEdge, *tEdge2, *newEdge;
    char      *start, *end, *end2;
    int       overlap, longestOverlap;

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {

	/* only EXPAND-relatiobships are examined */
	if (tEdge->indexkind == SMI_INDEX_EXPAND) {

	    start = tEdge->startNode->smiNode->name;
	    end = tEdge->endNode->smiNode->name;

	    /* getting the start overlap */
	    overlap = strpfxlen(start,end);
      
	    /* looking for better connection with longer overlap */
	    /* maybe better to traverse the edges with graphGetNextEdgeByNode
	       using tEdge->startNode */
	    newEdge = NULL;
	    longestOverlap = overlap;
	    for (tEdge2 = graphGetFirstEdgeByNode(graph,tEdge->startNode);
		 tEdge2;
		 tEdge2 = graphGetNextEdgeByNode(graph, tEdge2,
						 tEdge->startNode)) {
	
		/* must be a sparse relationship to get a correct new edge */
		if (tEdge2->indexkind == SMI_INDEX_SPARSE) {
		    end2 = tEdge2->endNode->smiNode->name;
	  
		    /* new overlap longer and different tables ? */
		    if (overlap < strpfxlen(end,end2) && end != end2) {
			longestOverlap=strpfxlen(end,end2);
			newEdge = tEdge2;
		    }
		}
	    }
      
	    /* better connection found -> changing the edge */
	    if (newEdge) {
		tEdge->startNode = newEdge->endNode;
	    }
	}
    }

    if (XPLAIN == 1) {
	printf("\n--- Second Phase - reordering the connections\n\n");
	graphShowEdges(graph);
    }

    return graph;
}

/*
 * algLinkScalarsToTables
 *
 *
 */
static Graph *algLinkScalarsToTables(Graph *graph)
{
    GraphNode *tNode, *tNode2, *newNode;
    int       overlap,minoverlap;

    /* getting the min. overlap for all nodes */
    minoverlap = 10000;
    tNode2 = graphGetFirstNode(graph);
    for (tNode = graphGetNextNode(graph,tNode2);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	minoverlap = min(minoverlap, strpfxlen(tNode->smiNode->name,
					       tNode2->smiNode->name));
    }

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
    
	if (!graphGetFirstEdgeByNode(graph, tNode)) {
 
	    overlap = 0;
	    newNode = NULL;
	    for (tNode2 = graphGetFirstNode(graph);
		 tNode2;
		 tNode2 = graphGetNextNode(graph, tNode2)) {
		
		/* Variante, in der ein gemeinsamer Prefix aussortiert wird */
#if 1	
		if (strpfxlen(tNode->smiNode->name,tNode2->smiNode->name) >
		    overlap &&
		    strpfxlen(tNode->smiNode->name,tNode2->smiNode->name) >
		    minoverlap+1 &&  
		    tNode->smiNode->name != tNode2->smiNode->name) {
		    overlap = strpfxlen(tNode->smiNode->name,
					tNode2->smiNode->name);
		    newNode = tNode2;
		}
		
		/* Original-Code */
#else
		if (strpfxlen(tNode->smiNode->name,tNode2->smiNode->name) >
		    overlap &&
		    tNode->smiNode->name != tNode2->smiNode->name) {
		    overlap = strpfxlen(tNode->smiNode->name,
					tNode2->smiNode->name);
		    newNode = tNode2;
		}
#endif
	    }

	    if (newNode) {
		/* no scalar - scalar edge */
		if (newNode->smiNode->nodekind != SMI_NODEKIND_SCALAR ||
		    tNode->smiNode->nodekind != SMI_NODEKIND_SCALAR) {

		    /* getting the direction of the edge : table-scalar */
		    if (newNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {
			graph = algInsertEdge(graph, 
					      newNode->smiNode,
					      tNode->smiNode,
					      SMI_INDEX_UNKNOWN,
					      GRAPH_ENHINDEX_NAMES);
		    }
		    else {
			graph = algInsertEdge(graph, 
					      tNode->smiNode, 
					      newNode->smiNode,
					      SMI_INDEX_UNKNOWN,
					      GRAPH_ENHINDEX_NAMES);
		    }
		}
	    }
	}
    }

    return graph;
}

/*
 * algGroupScalars
 *
 */
static Graph *algGroupScalars(Graph *graph)
{
    GraphNode *tNode;
    int       actGroup, fGroup;
    
    actGroup = 0;
    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	
	if (!graphGetFirstEdgeByNode(graph, tNode) &&
	    tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	    fGroup = algGetGroupByPrefix(graph, tNode->smiNode);
	    if (fGroup == 0) {
		tNode->group = ++actGroup;
	    }
	    else {
		tNode->group = fGroup; 
	    }
	}
    }

    if (XPLAIN == 1) {
	printf("Scalar Groups : \n");
	for (actGroup = 1;
	     actGroup <= algGetNumberOfGroups(graph);
	     actGroup++) {
	    algPrintGroup(graph, actGroup);
	    printf("\n");
	}
	printf("\n");
    }
    
    return graph; 
}

/*
 * algLinkLonelyTables
 *
 * Links isolated tables with other tables via the types of the
 * index objects.
 * If no type is found the types are checked via the suffix if they differ
 * only in the range (-OrZero). Therefore the suffix-vector is checked.
 */
static Graph *algLinkLonelyTables(Graph *graph)
{
    SmiElement *smiElement, *smiElement2 = NULL;
    GraphNode  *tNode, *tNode2;
    int        i;

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	if (!graphGetFirstEdgeByNode(graph, tNode) &&
	    tNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {

	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(tNode->smiNode));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {

		for (tNode2 = graphGetFirstNode(graph);
		     tNode2;
		     tNode2 = graphGetNextNode(graph, tNode2)) {
		    if (tNode->smiNode->nodekind == SMI_NODEKIND_TABLE &&
			tNode != tNode2) {
			for (smiElement2 = smiGetFirstElement(
			    smiGetFirstChildNode(tNode2->smiNode));
			     smiElement2;
			     smiElement2 = smiGetNextElement(smiElement2)) {
			    if (strcmp(algGetTypeName(
				           smiGetElementNode(smiElement2)),
				       algGetTypeName(
					   smiGetElementNode(smiElement)))
				== 0) {
				graph = graphInsertEdge(graph,tNode2, tNode,
							SMI_INDEX_UNKNOWN,
							GRAPH_ENHINDEX_TYPES);
				break;
			    }
			    else {
				for (i = 0;
				     i < SUFFIXCOUNT;
				     i++) {
				    if (strstr(
					algGetTypeName(
					    smiGetElementNode(smiElement)),
					suffix[i])) {
					graph =
					    graphInsertEdge(graph,tNode2,
							tNode,
							SMI_INDEX_UNKNOWN,
							GRAPH_ENHINDEX_TYPES);
					break;
				    }
				}
				
				if (i < SUFFIXCOUNT) break;
			    }
			}
		    }
		    if (smiElement2) break;
		}
		if (tNode2) break;
	    }
		
	}
    }
	 
    return graph;
}

/*
 * algConnectLonelyNodes
 *
 * Connect the isolated nodes (scalars and tables)
 *
 * 1. Trying to link tables via the type of the index objects
 * 2. Trying to link scalars to tables via the names
 * 3. Trying to group scalars which have not been adjacent to any edge.
 */
static Graph *algConnectLonelyNodes(Graph *graph) 
{
    if (XPLAIN == 1) {
	printf("\n--- Fourth Phase -  connecting isolated nodes\n\n");
    }

    graph = algLinkLonelyTables(graph);

    graph = algLinkScalarsToTables(graph);

    graph = algGroupScalars(graph);

    if (XPLAIN == 1) {
	graphShowEdges(graph);
    }

    return graph;
}

/*
 * algCheckForDependency
 *
 * Tries to change connection types from aggregation to dependency by
 * looking at the types.
 *
 * 1. Supporting tables are revealed. Supporting tables consist only
 *    index objects and RowStatus-Objects and/or Storage-Type/Objects.
 * 2. If a supporting table is found the connection type is changed to
 *    dependency.
 * 3. Now the types of the index objects are checked (only in the end-table).
 *    If the same type is found in an other table the start-table is going to
 *    be connected with it (inserting a new edge). 
 *    The end-table is still connected to the start-table.
 */
static Graph *algCheckForDependency(Graph *graph) 
{
    GraphEdge  *tEdge, *tEdge2;
    SmiNode    *endTable, *startTable, *depTable;
    SmiElement *smiElement;
    int        elemCount;

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {

	if (tEdge->indexkind == SMI_INDEX_EXPAND ||
	    tEdge->indexkind == SMI_INDEX_UNKNOWN) {

	    if (tEdge->startNode->smiNode->nodekind == SMI_NODEKIND_TABLE &&
		tEdge->endNode->smiNode->nodekind   == SMI_NODEKIND_TABLE) {
		
		elemCount = algGetNumberOfRows(tEdge->startNode->smiNode);
		if (elemCount >= 0) tEdge->connection = GRAPH_CON_DEPENDENCY;

		elemCount = algGetNumberOfRows(tEdge->endNode->smiNode);
		if (elemCount >= 0) tEdge->connection = GRAPH_CON_DEPENDENCY;
	    }
	}
    }

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {

	if (tEdge->connection == GRAPH_CON_DEPENDENCY) {
      
	    startTable = tEdge->startNode->smiNode;
	    endTable = tEdge->endNode->smiNode;
      
	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(endTable));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
	
		/* look only at expanded indices (only present in endTable) */
		if (smiGetParentNode(smiGetParentNode(
		    smiGetElementNode(smiElement)))->name ==
		    endTable->name) {

		    depTable = algFindEqualType(startTable,
						smiGetElementNode(smiElement));

		    /* depTable different to endTable? */
		    if (depTable && (strcmp(depTable->name, endTable->name)
				     != 0)) {

			graph = algInsertEdge(graph,
					      depTable,
					      startTable, SMI_INDEX_UNKNOWN,
					      GRAPH_ENHINDEX_TYPES);
			tEdge2 = graphGetLastEdge(graph);
			tEdge2->connection = GRAPH_CON_ASSOCIATION;
			break;
		    }
		}
	    }
	}
    }

    if (XPLAIN == 1) {
	printf("\n--- Fifth Phase - checking for dependency connections\n\n");
	graphShowEdges(graph);
    }

    return graph;
}

/*
 * algCheckNotifications
 *
 * Checks the notifications for edges not yet present.
 * Every notification objects is checked whether its elements are
 * connected or not.
 */
static Graph *algCheckNotifications(Graph *graph)
{
    SmiNode    *smiNode, *smiNode2, *smiNode3;
    SmiElement *smiElement;

    for (smiNode = smiGetFirstNode(module, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {

	smiElement = smiGetFirstElement(smiNode);
	if (smiElement) {
	    smiNode2 = smiGetParentNode(smiGetElementNode(smiElement));
	    smiNode2 = smiGetParentNode(smiNode2);

	    for (smiElement = smiGetNextElement(smiElement);
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {

		smiNode3 = smiGetParentNode(smiGetParentNode(
		    smiGetElementNode(smiElement)));

		if (cmpSmiNodes(smiNode2, smiNode3) == 0) { 
		    graph = algInsertEdge(graph, smiNode2, smiNode3,
					  SMI_INDEX_UNKNOWN,
					  GRAPH_ENHINDEX_NOTIFICATION); 
		}
	    }
	}

    }

    if (XPLAIN == 1) {
	printf("\n--- Third Phase - notifications \n\n");
	graphShowEdges(graph);
    }

    return graph;
}





/* ------ XML primitives ------                                              */





static void printHeader()
{
    printf("<?xml version=\"1.0\"?>\n");
    printf("<diagram xmlns:dia=\"http://www.lysator.liu.se/~alla/dia/\">\n");
    printf("  <diagramdata>\n");
    printf("    <attribute name=\"background\">\n");
    printf("      <color val=\"#ffffff\"/>\n");
    printf("    </attribute>\n");
    printf("    <attribute name=\"paper\">\n");
    printf("      <composite type=\"paper\">\n");
    printf("        <attribute name=\"name\">\n");
    printf("          <string>#A4#</string>\n");
    printf("        </attribute>\n");
    printf("        <attribute name=\"tmargin\">\n");
    printf("         <real val=\"2.82\"/>\n");
    printf("       </attribute>\n");
    printf("       <attribute name=\"bmargin\">\n");
    printf("         <real val=\"2.82\"/>\n");
    printf("        </attribute>\n");
    printf("       <attribute name=\"lmargin\">\n");
    printf("         <real val=\"2.82\"/>\n");
    printf("       </attribute>\n");
    printf("       <attribute name=\"rmargin\">\n");
    printf("         <real val=\"2.82\"/>\n");
    printf("       </attribute>\n");
    printf("       <attribute name=\"is_portrait\">\n");
    printf("         <boolean val=\"true\"/>\n");
    printf("       </attribute>\n");
    printf("      <attribute name=\"scaling\">\n");
    printf("         <real val=\"1\"/>\n");
    printf("      </attribute>\n");
    printf("      <attribute name=\"fitto\">\n");
    printf("        <boolean val=\"false\"/>\n");
    printf("      </attribute>\n");
    printf("    </composite>\n");
    printf("   </attribute>\n");
    printf("  </diagramdata>\n");
    printf("  <layer name=\"Background\" visible=\"true\">\n");   
}

static void printCloseXML()
{
    printf("  </layer>\n");
    printf("</diagram>\n");
}

static void printXMLObject(GraphNode *node, float x, float y)
{
    SmiElement *smiElement;

    node->x = x;
    node->y = y;
    node->print = 1;
    
    printf("    <object type=\"UML - Class\" version=\"0\" id=\"%s\">\n",
	   node->smiNode->name);
    printf("      <attribute name=\"obj_pos\">\n");
    printf("       <point val=\"%f,%f\"/>\n",x,y);
    printf("      </attribute>\n");
    printf("     <attribute name=\"obj_bb\">\n");
    printf("       <rectangle val=\"0.0,0.0;0.0,0.0\"/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"elem_corner\">\n");
    printf("       <point val=\"%f,%f\"/>\n",x,y);
    printf("     </attribute>\n");
    printf("     <attribute name=\"elem_width\">\n");
    printf("       <real val=\"%f\"/>\n",node->w);
    printf("     </attribute>\n");
    printf("     <attribute name=\"elem_height\">\n");
    printf("       <real val=\"%f\"/>\n",node->h);
    printf("     </attribute>\n");
    printf("     <attribute name=\"name\">\n");
    printf("       <string>#&lt;&lt;%s&gt;&gt;#</string>\n",
	   node->smiNode->name);
    printf("     </attribute>\n");
    printf("     <attribute name=\"stereotype\">\n");
    printf("       <string/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"abstract\">\n");
    printf("       <boolean val=\"false\"/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"suppress_attributes\">\n");
    printf("        <boolean val=\"false\"/>\n");    
    printf("      </attribute>\n");   
    printf("      <attribute name=\"suppress_operations\">\n");
    printf("        <boolean val=\"true\"/>\n");
    printf("      </attribute>\n");
    printf("     <attribute name=\"visible_attributes\">\n");

    if (node->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	printf("       <boolean val=\"false\"/>\n");
    } else {
	printf("       <boolean val=\"true\"/>\n");
    }    

    printf("     </attribute>\n");
    printf("     <attribute name=\"visible_operations\">\n");
    printf("        <boolean val=\"false\"/>\n");
    printf("      </attribute>\n");

    printf("     <attribute name=\"attributes\">\n");

    for (smiElement = smiGetFirstElement(smiGetFirstChildNode(node->smiNode));
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	printf("        <composite type=\"umlattribute\">\n");
	printf("          <attribute name=\"name\">\n");
	printf("            <string>#%s#</string>\n",
	       smiGetElementNode(smiElement)->name);
	printf("          </attribute>\n");
	printf("          <attribute name=\"type\">\n");
	printf("            <string>#%s#</string>\n",
	       algGetTypeName(smiGetElementNode(smiElement)));
	printf("          </attribute>\n");
	printf("          <attribute name=\"value\">\n");
	printf("            <string/>\n");
	printf("          </attribute>\n");
	printf("          <attribute name=\"visibility\">\n");
	printf("            <enum val=\"0\"/>\n");
	printf("          </attribute>\n");
	printf("          <attribute name=\"abstract\">\n");
	printf("            <boolean val=\"false\"/>\n");
	printf("          </attribute>\n");
	printf("          <attribute name=\"class_scope\">\n");
	printf("            <boolean val=\"false\"/>\n");
	printf("          </attribute>\n");
	printf("        </composite>\n");	
    }
    
    printf("      </attribute>\n");
    
    printf("     <attribute name=\"operations\"/>\n");
    printf("    <attribute name=\"template\">\n");
    printf("      <boolean val=\"false\"/>\n");
    printf("    </attribute>\n");
    printf("     <attribute name=\"templates\"/>\n");
    printf("   </object>\n");
}

static void printXMLGroup(Graph *graph, int group, float x, float y)
{
    GraphNode *tNode;

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	if (tNode->group == group) break;
    }

    if (!tNode) return;
    
    printf("    <object type=\"UML - Class\" version=\"0\" id=\"%s\">\n",
	   strgetpfx(tNode->smiNode->name));
    printf("      <attribute name=\"obj_pos\">\n");
    printf("       <point val=\"%f,%f\"/>\n",x,y);
    printf("      </attribute>\n");
    printf("     <attribute name=\"obj_bb\">\n");
    printf("       <rectangle val=\"0.0,0.0;0.0,0.0\"/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"elem_corner\">\n");
    printf("       <point val=\"%f,%f\"/>\n",x,y);
    printf("     </attribute>\n");
    printf("     <attribute name=\"elem_width\">\n");
    printf("       <real val=\"%f\"/>\n",0.0);
    printf("     </attribute>\n");
    printf("     <attribute name=\"elem_height\">\n");
    printf("       <real val=\"%f\"/>\n",0.0);
    printf("     </attribute>\n");
    printf("     <attribute name=\"name\">\n");
    printf("       <string>#&lt;&lt;%s - Group&gt;&gt;#</string>\n",
	   strgetpfx(tNode->smiNode->name));
    printf("     </attribute>\n");
    printf("     <attribute name=\"stereotype\">\n");
    printf("       <string/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"abstract\">\n");
    printf("       <boolean val=\"false\"/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"suppress_attributes\">\n");
    printf("        <boolean val=\"false\"/>\n");    
    printf("      </attribute>\n");   
    printf("      <attribute name=\"suppress_operations\">\n");
    printf("        <boolean val=\"true\"/>\n");
    printf("      </attribute>\n");
    printf("     <attribute name=\"visible_attributes\">\n");
    printf("       <boolean val=\"true\"/>\n");
    printf("     </attribute>\n");
    printf("     <attribute name=\"visible_operations\">\n");
    printf("        <boolean val=\"false\"/>\n");
    printf("      </attribute>\n");

    printf("     <attribute name=\"attributes\">\n");

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph,tNode)){

	if (tNode->group == group) {
	    printf("        <composite type=\"umlattribute\">\n");
	    printf("          <attribute name=\"name\">\n");
	    printf("            <string>#%s#</string>\n",
		   tNode->smiNode->name);
	    printf("          </attribute>\n");
	    printf("          <attribute name=\"type\">\n");
	    printf("            <string>#%s#</string>\n",
		   algGetTypeName(tNode->smiNode));
	    printf("          </attribute>\n");
	    printf("          <attribute name=\"value\">\n");
	    printf("            <string/>\n");
	    printf("          </attribute>\n");
	    printf("          <attribute name=\"visibility\">\n");
	    printf("            <enum val=\"0\"/>\n");
	    printf("          </attribute>\n");
	    printf("          <attribute name=\"abstract\">\n");
	    printf("            <boolean val=\"false\"/>\n");
	    printf("          </attribute>\n");
	    printf("          <attribute name=\"class_scope\">\n");
	    printf("            <boolean val=\"false\"/>\n");
	    printf("          </attribute>\n");
	    printf("        </composite>\n");
	}
    }
    
    printf("      </attribute>\n");
    
    printf("     <attribute name=\"operations\"/>\n");
    printf("    <attribute name=\"template\">\n");
    printf("      <boolean val=\"false\"/>\n");
    printf("    </attribute>\n");
    printf("     <attribute name=\"templates\"/>\n");
    printf("   </object>\n");
}

static float getObjX(GraphNode *tNode)
{
    return (float) tNode->w / 2 + tNode->x;   
}

static float getObjY(GraphNode *tNode)
{
    return (float) tNode->y;
}

static float getObjYRel(GraphNode *tNode, int con)
{
    return (float) tNode->y-EDGEYSPACING-con;
}

static float getRectSX(GraphNode *tNode)
{
    return (float) tNode->w / 2 + tNode->x - RECTCORRECTION;
}

static float getRectEX(GraphNode *tNode)
{
    return (float) tNode->w / 2 + tNode->x + RECTCORRECTION;
}

static float getRectSY(GraphNode *tNode)
{
    return (float) tNode->y - 2 - RECTCORRECTION;
}

static float getRectEY(GraphNode *tNode)
{
    return (float) tNode->y - 2 + RECTCORRECTION;
}

static void printXMLDependency(GraphEdge *tEdge, int con)
{
    if (tEdge->print == 1) return;
    tEdge->print = 1;

    printf("    <object type=\"UML - Dependency\" "
	   "version=\"0\" id=\"Depend:%s:%s\">\n",
	   tEdge->startNode->smiNode->name,
	   tEdge->endNode->smiNode->name);    

    printf("      <attribute name=\"obj_pos\">\n");
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjY(tEdge->startNode));	   
    printf("     </attribute>\n");
    printf("      <attribute name=\"obj_bb\">\n");
    printf("       <rectangle val=\"%f,%f;%f,%f\"/>\n"
	   ,getRectSX(tEdge->startNode)
	   ,getRectSY(tEdge->startNode)
	   ,getRectEX(tEdge->startNode)
	   ,getRectEY(tEdge->startNode));
    printf("     </attribute>\n");
    printf("     <attribute name=\"orth_points\">\n");
    printf("       <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjY(tEdge->startNode));	
    printf("       <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjYRel(tEdge->startNode,con));
    printf("       <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->endNode)
	   ,getObjYRel(tEdge->startNode,con));
    printf("       <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->endNode)
	   ,getObjY(tEdge->endNode));	
    printf("     </attribute>\n");
    printf("     <attribute name=\"orth_orient\">\n");
    printf("       <enum val=\"1\"/>\n");
    printf("       <enum val=\"0\"/>\n");
    printf("       <enum val=\"1\"/>\n");
    printf("     </attribute>\n");
    printf("    <attribute name=\"draw_arrow\">\n");
    printf("       <boolean val=\"true\"/>\n");
    printf("      </attribute>\n");
    printf("     <attribute name=\"name\">\n");
    printf("        <string/>\n");
    printf("   </attribute>\n");
    printf("     <attribute name=\"stereotype\">\n");
    printf("      <string/>\n");
    printf("    </attribute>\n");
    printf("    <connections>\n");

    printf("      <connection handle=\"0\" to=\"%s\" connection=\"%d\"/>\n",
	   tEdge->startNode->smiNode->name,1);
    printf("      <connection handle=\"1\" to=\"%s\" connection=\"%d\"/>\n",
	   tEdge->endNode->smiNode->name,1);
    
    printf("    </connections>\n");
    printf("    </object>\n");
}

static void printXMLAggregation(GraphEdge *tEdge, int con)
{
    if (tEdge->print == 1) return;
    tEdge->print = 1;
    
    printf("    <object type=\"UML - Association\" "
	   "version=\"0\" id=\"Assoc:%s:%s\">\n",
	   tEdge->startNode->smiNode->name,
	   tEdge->endNode->smiNode->name);
    
    printf("      <attribute name=\"obj_pos\">\n");
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjY(tEdge->startNode));
    printf("      </attribute>\n");
    printf("      <attribute name=\"obj_bb\">\n");
    printf("        <rectangle val=\"%f,%f;%f,%f\"/>\n"
	   ,getRectSX(tEdge->startNode)
	   ,getRectSY(tEdge->startNode)
	   ,getRectEX(tEdge->startNode)
	   ,getRectEY(tEdge->startNode));
    printf("      </attribute>\n");
    printf("      <attribute name=\"orth_points\">\n");
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjY(tEdge->startNode));
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjYRel(tEdge->startNode,con));
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->endNode)
	   ,getObjYRel(tEdge->startNode,con));
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->endNode)
	   ,getObjY(tEdge->endNode));
    printf("      </attribute>\n");
    printf("      <attribute name=\"orth_orient\">\n");
    printf("        <enum val=\"1\"/>\n");
    printf("        <enum val=\"0\"/>\n");
    printf("        <enum val=\"1\"/>\n");   
    printf("      </attribute>\n");
    printf("      <attribute name=\"name\">\n");

    switch(tEdge->indexkind) {
    case SMI_INDEX_UNKNOWN :
	switch (tEdge->enhancedindex) {
	case GRAPH_ENHINDEX_UNKNOWN :
	    break;
	case GRAPH_ENHINDEX_TYPES :
	    printf("       <string>#%s#</string>\n","UNKNOWN (TYPES)");
	    break;
	case GRAPH_ENHINDEX_NAMES :
	    printf("       <string>#%s#</string>\n","UNKNOWN (NAMES)");
	    break;
	case GRAPH_ENHINDEX_NOTIFICATION :
	    printf("       <string>#%s#</string>\n","UNKNOWN (NOTIFICATION)");
	    break;
	case GRAPH_ENHINDEX_INDEX :
	    printf("       <string>#%s#</string>\n","UNKNOWN (INDEX)");
	    break;	    
	}
	break;
    case SMI_INDEX_INDEX :
	printf("       <string>#%s#</string>\n","INDEX");
	break;
    case SMI_INDEX_AUGMENT :
	printf("       <string>#%s#</string>\n","AUGMENT");
	break;
    case SMI_INDEX_SPARSE :
	printf("       <string>#%s#</string>\n","SPARSE");
	break;
    case SMI_INDEX_REORDER :
	printf("       <string>#%s#</string>\n","REORDER");
	break;
    case SMI_INDEX_EXPAND :
	printf("       <string>#%s#</string>\n","EXPAND");
	break;
    }
    
    printf("      </attribute>\n");
    printf("      <attribute name=\"direction\">\n");
    printf("        <enum val=\"0\"/>\n");
    printf("      </attribute>\n");
    printf("      <attribute name=\"ends\">\n");
    printf("        <composite>\n");
    printf("          <attribute name=\"role\">\n");
    printf("            <string/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"multiplicity\">\n");
    
    switch (tEdge->cardinality) {
    case GRAPH_CARD_UNKNOWN :
	printf("       <string>#-#</string>\n");
	break;
    case GRAPH_CARD_ONE_TO_ONE :
	printf("       <string>#1#</string>\n");
	break;
    case GRAPH_CARD_ONE_TO_MANY :
	printf("       <string>#1#</string>\n");
	break;
    case GRAPH_CARD_ZERO_TO_ONE :
	printf("       <string>#0#</string>\n");
	break;
    case GRAPH_CARD_ZERO_TO_MANY :
	printf("       <string>#0#</string>\n");
	break;    
    }
   
    printf("          </attribute>\n");
    printf("          <attribute name=\"arrow\">\n");
    printf("            <boolean val=\"false\"/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"aggregate\">\n");
    printf("            <enum val=\"0\"/>\n");
    printf("          </attribute>\n");
    printf("        </composite>\n");
    printf("        <composite>\n");
    printf("          <attribute name=\"role\">\n");
    printf("            <string/>\n");
    printf("          </attribute>\n");

    printf("          <attribute name=\"multiplicity\">\n");

    switch (tEdge->cardinality) {
    case GRAPH_CARD_UNKNOWN :
	printf("       <string>#-#</string>\n");
	break;
    case GRAPH_CARD_ONE_TO_ONE :
	printf("       <string>#1#</string>\n");
	break;
    case GRAPH_CARD_ONE_TO_MANY :
	printf("       <string>#n#</string>\n");
	break;
    case GRAPH_CARD_ZERO_TO_ONE :
	printf("       <string>#1#</string>\n");
	break;
    case GRAPH_CARD_ZERO_TO_MANY :
	printf("       <string>#n#</string>\n");
	break;    
    }
    
    printf("          </attribute>\n");
    printf("          <attribute name=\"arrow\">\n");
    printf("            <boolean val=\"false\"/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"aggregate\">\n");
    printf("            <enum val=\"1\"/>\n");
    printf("          </attribute>\n");
    printf("        </composite>\n");
    printf("      </attribute>\n");
    printf("      <connections>\n");
    printf("        <connection handle=\"0\" "
	   "to=\"%s\" connection=\"%d\"/>\n",
	   tEdge->startNode->smiNode->name, 1);
    
    printf("        <connection handle=\"1\" "
	   "to=\"%s\" connection=\"%d\"/>\n",
	   tEdge->endNode->smiNode->name, 1);
    
    printf("      </connections>\n");
    printf("    </object>\n");
}

static void printXMLAssociation(GraphEdge *tEdge, int con)
{
    if (tEdge->print == 1) return;
    tEdge->print = 1;
    
    printf("    <object type=\"UML - Association\" "
	   "version=\"0\" id=\"Assoc:%s:%s\">\n",
	   tEdge->startNode->smiNode->name,
	   tEdge->endNode->smiNode->name);
    
    printf("      <attribute name=\"obj_pos\">\n");
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjY(tEdge->startNode));
    printf("      </attribute>\n");
    printf("      <attribute name=\"obj_bb\">\n");
    printf("        <rectangle val=\"%f,%f;%f,%f\"/>\n"
	   ,getRectSX(tEdge->startNode)
	   ,getRectSY(tEdge->startNode)
	   ,getRectEX(tEdge->startNode)
	   ,getRectEY(tEdge->startNode));
    printf("      </attribute>\n");
    printf("      <attribute name=\"orth_points\">\n");
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjY(tEdge->startNode));
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->startNode)
	   ,getObjYRel(tEdge->startNode,con));
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->endNode)
	   ,getObjYRel(tEdge->startNode,con));
    printf("        <point val=\"%f,%f\"/>\n"
	   ,getObjX(tEdge->endNode)
	   ,getObjY(tEdge->endNode));
    printf("      </attribute>\n");
    printf("      <attribute name=\"orth_orient\">\n");
    printf("        <enum val=\"1\"/>\n");
    printf("        <enum val=\"1\"/>\n");
    printf("        <enum val=\"1\"/>\n");   
    printf("      </attribute>\n");
    printf("      <attribute name=\"name\">\n");
    printf("        <string/>\n");    
    printf("      </attribute>\n");
    printf("      <attribute name=\"direction\">\n");
    printf("        <enum val=\"0\"/>\n");
    printf("      </attribute>\n");
    printf("      <attribute name=\"ends\">\n");
    printf("        <composite>\n");
    printf("          <attribute name=\"role\">\n");
    printf("            <string/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"multiplicity\">\n");
    printf("            <string/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"arrow\">\n");
    printf("            <boolean val=\"false\"/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"aggregate\">\n");
    printf("            <enum val=\"0\"/>\n");
    printf("          </attribute>\n");
    printf("        </composite>\n");
    printf("        <composite>\n");
    printf("          <attribute name=\"role\">\n");
    printf("            <string/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"multiplicity\">\n");
    printf("            <string/>\n");    
    printf("          </attribute>\n");
    printf("          <attribute name=\"arrow\">\n");
    printf("            <boolean val=\"false\"/>\n");
    printf("          </attribute>\n");
    printf("          <attribute name=\"aggregate\">\n");
    printf("            <enum val=\"0\"/>\n");
    printf("          </attribute>\n");
    printf("        </composite>\n");
    printf("      </attribute>\n");
    printf("      <connections>\n");
    printf("        <connection handle=\"0\" "
	   "to=\"%s\" connection=\"%d\"/>\n",
	   tEdge->startNode->smiNode->name, 1);
    
    printf("        <connection handle=\"1\" "
	   "to=\"%s\" connection=\"%d\"/>\n",
	   tEdge->endNode->smiNode->name, 1);
    
    printf("      </connections>\n");
    printf("    </object>\n");
}

static GraphNode *calcSize(GraphNode *node)
{
    SmiNode    *tNode;
    SmiElement *smiElement;

    if (node->smiNode->nodekind == SMI_NODEKIND_TABLE) {
	node->w = (strlen(node->smiNode->name)+4) * HEADFONTSIZETABLE
	    + HEADSPACESIZETABLE;
    } else {
	node->w = (strlen(node->smiNode->name)+4) * HEADFONTSIZESCALAR
	    + HEADSPACESIZESCALAR;
    }
    
    if (node->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	node->h = 2;
    } else {
	node->h = 2;
	for (smiElement = smiGetFirstElement(
	    smiGetFirstChildNode(node->smiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {

	    tNode = smiGetElementNode(smiElement);
	    node->w = max(node->w, (strlen(tNode->name) +
			  strlen(algGetTypeName(tNode))) * ATTRFONTSIZE
			  + ATTRSPACESIZE);
	    node->h++;
	}
    }
    return node;
}

static void printXML(Graph *graph)
{
    GraphNode *tNode;
    GraphEdge *tEdge;
    int       x,y,ydiff;
    int       group;
    int       conCounter;
    
    printHeader();

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	tNode = calcSize(tNode);
    }

    x = XOFFSET;
    y = YOFFSET;
    ydiff = 1;

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
	if (tNode->group == 0) {

	    if (tNode->print == 0) {
		printXMLObject(tNode,x,y+tNode->connections);
		x += XSPACING + tNode->w;
		ydiff = max(ydiff, tNode->h+tNode->connections);
		if (x > NEWLINEDISTANCE) {
		    x = XOFFSET;
		    y += ydiff + YSPACING;
		    ydiff = 0;
		}
	    }
	    
	    conCounter = 1;
	    for (tEdge = graphGetFirstEdgeByNode(graph, tNode);
		 tEdge;
		 tEdge = graphGetNextEdgeByNode(graph, tEdge, tNode)) {
		if (tEdge->startNode == tNode) {
		    if (tEdge->endNode->print == 0) {
			printXMLObject(tEdge->endNode,x,
				       y+tEdge->endNode->connections);
			x += XSPACING + tEdge->endNode->w;
			ydiff = max(ydiff,
				    tNode->h+tEdge->endNode->connections);
			if (x > NEWLINEDISTANCE) {
			    x = XOFFSET;
			    y += ydiff + YSPACING;
			    ydiff = 0;
			}
		    }
		} else {
		    if (tEdge->startNode->print == 0) {
			printXMLObject(tEdge->startNode,x,
				       y+tEdge->startNode->connections);
			x += XSPACING + tEdge->startNode->w;
			ydiff = max(ydiff,
				    tNode->h+tEdge->startNode->connections);
			if (x > NEWLINEDISTANCE) {
			    x = XOFFSET;
			    y += ydiff + YSPACING;
			    ydiff = 0;
			}
		    }	
		}

		conCounter++;
		
		switch (tEdge->connection) {
		case GRAPH_CON_AGGREGATION :
		    printXMLAggregation(tEdge, conCounter);
		    break;
		case GRAPH_CON_DEPENDENCY :
		    printXMLDependency(tEdge, conCounter);
		    break;
		case GRAPH_CON_ASSOCIATION :
		    printXMLAssociation(tEdge, conCounter);
		    break;	    
		}
	    }
	}
    }

    x = XOFFSET;
    y += ydiff;

    for (group = 1;
	 group <= algGetNumberOfGroups(graph);
	 group++) {
	printXMLGroup(graph,group,x,y);
	x += 2;
	y += 2;
    }
    
    printCloseXML();
}



/* ------------------------------------------------------------------------- */



/*
 * dumpCM
 *
 * Main function called by the interface functions dumpCMDia and
 * dumpCMXplain.
 */
dumpCM(char *modulename, int flags)
{
    Graph     *graph;

    if (!modulename) {
	fprintf(stderr,"smidump: united output not supported for cm format\n");
	exit(1);
    }
  
    module = smiGetModule(modulename);
    if (!module) {
	fprintf(stderr,"smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    graph        = xmalloc(sizeof(Graph));
    graph->nodes = NULL;
    graph->edges = NULL;

    if (XPLAIN == 1) {
       printf("\nConceptual Model of %s - generated by smidump " VERSION "\n\n"
	    , modulename);
}

    graph = algLinkTables(graph);
    graph = algCheckLinksByName(graph);
    graph = algCheckNotifications(graph);
    graph = algConnectLonelyNodes(graph);
    graph = algCheckForDependency(graph);

    if (XPLAIN == 0) {
	printXML(graph);
    }

    graphExit(graph);
}

/*
 * called interface function for UML output in dia format
 */
int dumpCMDia(char *modulename, int flags)
{
    XPLAIN = 0;
    
    dumpCM(modulename, flags);

    return 0;
}

/*
 * called interface function for ASCII output with explanations
 */
int dumpCMXplain(char *modulename, int flags)
{

    XPLAIN = 1;
    
    dumpCM(modulename, flags);

    return 0;
}
