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
 * @(#) $Id$
 */


/* 
 * -- TO DO --
 *
 * RMON2-MIB ifTable hat 0 Elements???? (sollte 1 sein)
 *  ---> statt SPARSE liegt eine EXPAND Beziehung vor
 *
 * in algCheckForDependency evtl. noch RowPointer einfuegen
 *
 * Wenn moeglich in 4. Phase den endTable an die neue Kante depTable - startTable
 * haengen. Bislang bleibt die Verbindung startTable - endTable.
 */





#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "smi.h"
#include "smidump.h"





typedef enum GraphImport {
  GRAPH_IMPORT_TRUE  = 1,
  GRAPH_IMPORT_FALSE = 0
} GraphImport;


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
    GRAPH_CON_DEPENDENCY    = 2
} GraphConnection;

typedef struct GraphNode{
    struct GraphNode *nextPtr;
    SmiNode          *smiNode;
    GraphImport      imported;
} GraphNode;


typedef struct GraphEdge {
    struct GraphEdge *nextPtr;         
    GraphNode        *startNode;
    GraphNode        *endNode;
    SmiIndexkind     indexkind;
    GraphConnection  connection;
    GraphCardinality cardinality;
} GraphEdge;


typedef struct Graph {
    GraphNode *nodes;
    GraphEdge *edges;
} Graph;

static SmiModule *module;


#define max(a, b) ((a < b) ? b : a)
#define min(a, b) ((a < b) ? a : b)

#define DEBUG




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
    GraphNode *newNode;
    GraphNode *tNode;
    GraphNode *lastNode;

    newNode           = xmalloc(sizeof(GraphNode));
    newNode->nextPtr  = NULL;
    newNode->smiNode  = smiNode;
    newNode->imported = GRAPH_IMPORT_FALSE;

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
			      GraphNode *endNode, SmiIndexkind indexkind)
{
    GraphEdge *newEdge;
    GraphEdge *tEdge;
    GraphEdge *lastEdge;

    newEdge              = xmalloc(sizeof(GraphEdge));
    newEdge->nextPtr     = NULL;
    newEdge->startNode   = startNode;
    newEdge->endNode     = endNode;
    newEdge->indexkind   = indexkind;
    newEdge->connection  = GRAPH_CON_AGGREGATION;
 
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
 *          Frees all memory allocated by the graph and calls smiExit().
 *
 * Input  : graph = pointer to a Graph structure
 *
 * Result : void
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
 *          Returns the first edge of the graph.
 *
 * Input  : graph = pointer to a graph structure
 *
 * Result : pointer to the first edge
 */
GraphEdge *graphGetFirstEdge(Graph *graph) 
{
    return (graph->edges) ? (GraphEdge *)graph->edges : NULL;  
}

/*
 * graphGetNextEdge
 *
 *          Returns the next edge after the given edge.
 *
 * Input  : graph = pointer to a graph structure
 *          edge  = pointer to an edge
 *
 * Result : pointer to the next edge
 */
GraphEdge *graphGetNextEdge(Graph *graph, GraphEdge *edge) 
{
    GraphEdge *tEdge;

    if (!graph->edges || !edge) return NULL;

    for (tEdge = graphGetFirstEdge(graph); tEdge; tEdge = tEdge->nextPtr) {
	if (tEdge->startNode->smiNode->name == edge->startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name == edge->endNode->smiNode->name) break;
    }
  
    if (tEdge->nextPtr == NULL) return NULL;

    return tEdge->nextPtr;
}

/*
 * graphGetLastEdge
 *
 * Returns the last edge of the given graph.
 */
GraphEdge *graphGetLastEdge(Graph *graph) 
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
GraphEdge *graphGetFirstEdgeByNode(Graph *graph, GraphNode *node)
{
    GraphEdge *tEdge;

    if (!graph || !node) return NULL;

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == node->smiNode->name ||
	    tEdge->endNode->smiNode->name == node->smiNode->name) break;
    }

    if (!tEdge) return NULL;

    return tEdge;
}

/*
 * graphGetNextEdgeByNode
 *
 * Returns the next edge adjacent to the given node (as startNode or EndNode)
 * after the given edge.
 */
GraphEdge *graphGetNextEdgeByNode(Graph *graph, 
				  GraphEdge *edge,
				  GraphNode *node) 
{
    GraphEdge *tEdge;

    if (!graph || !node) return NULL;

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == edge->startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name == edge->endNode->smiNode->name) break;
    }
  
    for (tEdge = graphGetNextEdge(graph,tEdge);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == node->smiNode->name ||
	    tEdge->endNode->smiNode->name == node->smiNode->name) break;
    }

    if (!tEdge) return NULL;

    return tEdge;
}

/*
 * graphCheckForRedundantEdge
 *
 * Finds redundant edges and returns 1 if one is found and 0 otherwise.
 */
int graphCheckForRedundantEdge(Graph *graph, GraphNode *startNode, GraphNode *endNode)
{
    GraphEdge *tEdge;

    if (!graph || !startNode || !endNode) return 0;

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {
	if (tEdge->startNode->smiNode->name == startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name == endNode->smiNode->name) return 1;
    }

    return 0;
}

/*
 * graphGetLastNode
 *
 *          Returns the last node of the list representing the graph nodes.
 *
 * Input  : graph = pointer to a graph structure
 *
 * Result : pointer to the last node of the list
 */
GraphNode *graphGetLastNode(Graph *graph) 
{
    GraphNode *tNode;

    if (!graph->nodes) return NULL;

    for (tNode = graph->nodes; tNode->nextPtr; tNode = tNode->nextPtr) {}

    return tNode;
}

/*
 * graphGetFirstNode
 *
 *          Returns the first node of the graph.
 *
 * Input  : graph   = pointer to a graph structure
 *
 * Result : pointer to the first node of the graph
 */
GraphNode *graphGetFirstNode(Graph *graph)
{
    return (graph->nodes) ? (GraphNode *)graph->nodes : NULL;
}

/*
 * graphGetNextNode
 *
 *          Returns the next graph node after the given node.
 *
 * Input  : graph = pointer to a graph structure
 *          node  = pointer to the previous node
 *
 * Result : pointer to the next node.
 */
GraphNode *graphGetNextNode(Graph *graph, GraphNode *node) 
{
    GraphNode *tNode;

    if (!graph->nodes || !node) return NULL;

    for (tNode = graphGetFirstNode(graph); tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->name == node->smiNode->name) break;
    }
  
    if (tNode->nextPtr == NULL) return NULL;

    return tNode->nextPtr;
}

/*
 * graphGetNode
 *
 *          Returns the graph node containing smiNode.
 *
 * Input  : graph   = pointer to a graph structure
 *          smiNode = pointer to the SmiNode to be found
 *
 * Result : pointer to the GraphNode containing smiNode
 */
GraphNode *graphGetNode(Graph *graph, SmiNode *smiNode)
{
    GraphNode *tNode;

    if (!smiNode || !graph) return NULL;

    for (tNode = graphGetFirstNode(graph); 
	 tNode; 
	 tNode = graphGetNextNode(graph, tNode)) {
	if (tNode->smiNode->name == smiNode->name) break;
    }

    return tNode;
}

/*
 * graphShowNodes
 *
 *          Prints all the node of Graph.
 *
 * Input  : graph   = pointer to a graph structure
 *
 * Result : void
 */
void graphShowNodes(Graph *graph) 
{
    GraphNode *tNode;
  
    if (graph->nodes == NULL) {printf("No nodes!\n"); return;}

    for (tNode = graphGetFirstNode(graph); 
	 tNode; 
	 tNode = graphGetNextNode(graph,tNode)) {
	printf("Node : %40s\n",tNode->smiNode->name);
    }
}

/*
 * graphShowEdges
 *
 *          Prints all edges with their cardinality and type of relationship.
 *
 * Input  : graph   = pointer to an edge structure
 *
 * Result : void
 */
void graphShowEdges(Graph *graph) 
{
    GraphEdge  *tEdge;

    if (graph->edges == NULL) {printf("No edges!\n"); return;};
  
    for (tEdge = graphGetFirstEdge(graph); 
	 tEdge; 
	 tEdge = graphGetNextEdge(graph,tEdge)) {
	printf("Edge : %32s -",tEdge->startNode->smiNode->name);

	switch (tEdge->connection) {
	case GRAPH_CON_AGGREGATION:
	    printf("   AGGR. ");
	    break;
	case GRAPH_CON_DEPENDENCY:
	    printf(" DEPEND. ");
	    break;
	case GRAPH_CON_UNKNOWN:
	    break;
	}

	switch (tEdge->cardinality) {
	case GRAPH_CARD_UNKNOWN      : printf(" (UNKNOWN) "); break;
	case GRAPH_CARD_ONE_TO_ONE   : printf("     (1:1) "); break;
	case GRAPH_CARD_ONE_TO_MANY  : printf("     (1:n) "); break;
	case GRAPH_CARD_ZERO_TO_ONE  : printf("     (0:1) "); break;
	case GRAPH_CARD_ZERO_TO_MANY : printf("     (0:n) "); break;
	}

	switch (tEdge->indexkind) {
	case SMI_INDEX_UNKNOWN  : printf(" UNKNOWN "); break;
	case SMI_INDEX_INDEX    : printf("   INDEX "); break;
	case SMI_INDEX_AUGMENT  : printf(" AUGMENT "); break;
	case SMI_INDEX_SPARSE   : printf("  SPARSE "); break;
	case SMI_INDEX_EXPAND   : printf("  EXPAND "); break;
	case SMI_INDEX_REORDER  : printf(" REORDER "); break;
	}

	printf("- %s\n",tEdge->endNode->smiNode->name);
    
#if 0
	printf("[");
	for (smiElement = smiGetFirstElement(smiGetFirstChildNode(tEdge->startNode->smiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    printf(" %s ",smiGetElementNode(smiElement)->name);
	}
	printf("] - [");    
	for (smiElement = smiGetFirstElement(smiGetFirstChildNode(tEdge->endNode->smiNode));
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
 * algCountElements
 *
 *          Returns the number of elements in a given row entry.
 *
 * Input  : smiNode = pointer to a row node
 *
 * Result : number of elements in the row node
 */
int algCountElements(SmiNode *smiNode) 
{
    int          count;
    SmiElement   *smiElement;

    if (smiNode->nodekind != SMI_NODEKIND_ROW) return 0;

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
 * The type of edge is given in indexkind.
 * Nodes which are not loaded yet into the node list of the graph
 * are added (nodes from imported MIBs).
 */
Graph *algInsertEdge(Graph *graph, SmiNode *snode, SmiNode *enode, 
		     SmiIndexkind indexkind) 
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
	startNode->imported = GRAPH_IMPORT_TRUE;    
    }
    if (endNode == NULL) {
	graph = graphInsertNode(graph, enode);
	endNode = graphGetNode(graph, enode);
	endNode->imported = GRAPH_IMPORT_TRUE;
    }  

    if (graphCheckForRedundantEdge(graph, startNode, endNode) == 0)
	graph = graphInsertEdge(graph, startNode, endNode, indexkind); 

    return graph;
}

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
 *    It is possible that a table expands an other table which is expanded by this.
 *    Therefore it is hard to track the base table.
 *    - scanning all tables which are different from the expanded table
 *    - the base table must have :
 *         - least elements
 *         - the same elementes must occur as in the expanded table
 * 5. the elements in both tables are checked for equality 
 */
Graph *algCheckForExpandRel(Graph *graph, SmiNode *smiNode) 
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
	if (tNode->name == expTable->name) break;

	baseTable = tNode;
    }  

    if (!baseTable) return graph;

    /* count the elements in the basetable */
    basecounter = algCountElements(smiGetFirstChildNode(baseTable));

    /* are baseTable and expTable identical ? */
    if (basecounter >= refcounter) {

	/* searching for new base table candidate in order to handle multiple indices */
	for (baseTable = smiGetNextNode(baseTable, SMI_NODEKIND_TABLE);
	     baseTable;
	     baseTable = smiGetNextNode(baseTable, SMI_NODEKIND_TABLE)) {
      
	    basecounter = algCountElements(smiGetFirstChildNode(baseTable));
      
	    if (basecounter < refcounter) {

		for (smiElement = smiGetFirstElement(smiGetFirstChildNode(expTable)); 
		     smiElement; 
		     smiElement = smiGetNextElement(smiElement)) {
		    tNode = smiGetElementNode(smiElement);

		    if (smiGetParentNode(smiGetParentNode(tNode))->name == 
			expTable->name) break;
	  
		    for (findElement = smiGetFirstElement(smiGetFirstChildNode(baseTable)); 
			 findElement; 
			 findElement = smiGetNextElement(findElement)) {
			if (tNode->name == smiGetElementNode(findElement)->name) break;
		    }
	  
		    if (!findElement) return graph;
		}	
		break;
	    }
	}
    
	if (!baseTable) return graph; 
    }

    for (smiElement = smiGetFirstElement(smiGetFirstChildNode(baseTable)); 
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {
	tNode = smiGetElementNode(smiElement);

	for (findElement = smiGetFirstElement(smiGetFirstChildNode(expTable)); 
	     findElement; 
	     findElement = smiGetNextElement(findElement)) {
	    if (tNode->name == smiGetElementNode(findElement)->name) break;
	}
    
	if (!findElement) return graph;
    }
  
    graph = algInsertEdge(graph, baseTable, expTable, SMI_INDEX_EXPAND);  

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
Graph *algCheckForSparseRel(Graph *graph, SmiNode *smiNode) 
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

    /* Ueberpruefung auf Tabellengleichheit -> Vermeidung von Loops im Graphen */
    if (table->name == smiGetParentNode(tNode)->name) return graph;
  
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
	    for (findElement = smiGetFirstElement(smiGetFirstChildNode(table)); 
		 findElement; 
		 findElement = smiGetNextElement(findElement)) {
		if (smiGetElementNode(findElement)->name == tNode->name) refcounter++;
	    }
	}
    }  

    /* Stimmen alle Elemente ueberein ? */
    if (refcounter != basecounter) return graph;

    graph = algInsertEdge(graph, table, smiGetParentNode(smiNode), 
			  SMI_INDEX_SPARSE); 
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
Graph *algCheckForReOrderRel(Graph *graph, SmiNode *smiNode) 
{
    SmiNode      *baseTable;
    SmiNode      *reorderTable;
    SmiNode      *tNode;
    GraphNode    *tGrNode = NULL;
    SmiElement   *smiElement;
    SmiElement   *findElement;
    unsigned int c1,c2;

    if (!smiNode) return graph;

    reorderTable = smiGetParentNode(smiNode);

    smiElement = smiGetFirstElement(smiNode);
    tNode = smiGetElementNode(smiElement);
    baseTable = smiGetParentNode(smiGetParentNode(tNode));

    /* identical tables ? */
    if (baseTable->name == reorderTable->name) return graph;
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
	    if (tNode->name == smiGetElementNode(findElement)->name) break;
	}
    
	/* no, they are not */
	if (!findElement) break;
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
		    for (findElement = smiGetFirstElement(smiGetFirstChildNode(baseTable));
			 findElement;
			 findElement = smiGetNextElement(findElement)) {
			if (tNode->name == smiGetElementNode(findElement)->name) break;
		    }	  
		    /* not the same elements */
		    if (!findElement) break;
		}
	    }
	}
    }

    /* no new baseTable found */
    if (!tGrNode) return graph;

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
		if (tNode->name != smiGetElementNode(findElement)->name) break;
		else return graph; /* yes, they do */
	    }
	}
    }

    /* Table passed all tests -> inserting new reorder edge */
    graph = algInsertEdge(graph, baseTable, reorderTable, SMI_INDEX_REORDER);
  
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
Graph *algGetIndexkind(Graph *graph, SmiNode *table) 
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
Graph *algLinkTables(Graph *graph) 
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

		/* get the relationship between the tables and insert the edges */
		if (node->indexkind == SMI_INDEX_INDEX) 
		    graph = algGetIndexkind(graph, node);

		if (node->indexkind == SMI_INDEX_AUGMENT) {
		    tSmiNode = node;
		    node = smiGetRelatedNode(node);
		    node = smiGetParentNode(node);

		    graph = algInsertEdge(graph, node, smiGetParentNode(tSmiNode), 
					  SMI_INDEX_AUGMENT );
		}
	    }
	}
    }

#ifdef DEBUG
    printf("--- First phase - getting the nodes and linking the tables ...\n\n");
    graphShowNodes(graph);
    printf("\n");
    graphShowEdges(graph);
#endif

    return graph;
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
Graph *algCheckLinksByName(Graph *graph) 
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
		 tEdge2 = graphGetNextEdgeByNode(graph, tEdge2, tEdge->startNode)) {
	
		/* must be a sparse relationship to get a correct edge change */
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

#ifdef DEBUG
    printf("\n--- Second Phase - reordering the connections ... \n\n");
    graphShowEdges(graph);
#endif

    return graph;
}

/*
 * algConnectLonelyNodes
 *
 */
Graph *algConnectLonelyNodes(Graph *graph) 
{
    GraphNode *tNode, *tNode2, *newNode;
    GraphEdge *tEdge;
    int       found;
    int       overlap,minoverlap;

    /* getting the min. overlap for all nodes */
    minoverlap = 10000;
    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
    
	for (tNode2 = graphGetFirstNode(graph);
	     tNode2;
	     tNode2 = graphGetNextNode(graph, tNode2)) {    
	    minoverlap = min(minoverlap, strpfxlen(tNode->smiNode->name,
						   tNode2->smiNode->name));
	}
    }

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
    
	found = 0;
	for (tEdge = graphGetFirstEdgeByNode(graph, tNode);
	     tEdge;
	     tEdge = graphGetNextEdgeByNode(graph, tEdge, tNode)) {
	    found = 1;
	}

	if (found == 0) {      
 
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
		    minoverlap+1 &&  /* +1 (ggf. zusaetzliche Verbesserung ????? */
		    tNode->smiNode->name != tNode2->smiNode->name) {
		    overlap = strpfxlen(tNode->smiNode->name,tNode2->smiNode->name);
		    newNode = tNode2;
		}
		/* Original-Code */
#else
		if (strpfxlen(tNode->smiNode->name,tNode2->smiNode->name) >
		    overlap &&
		    tNode->smiNode->name != tNode2->smiNode->name) {
		    overlap = strpfxlen(tNode->smiNode->name,tNode2->smiNode->name);
		    newNode = tNode2;
		}
#endif
	    }

	    if (newNode) graph = algInsertEdge(graph, 
					       tNode->smiNode, 
					       newNode->smiNode, SMI_INDEX_UNKNOWN);
	}
    }

#ifdef DEBUG
    printf("\n--- Third Phase -  connecting isolated nodes ... \n\n");
    graphShowEdges(graph);
#endif

    return graph;
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
int algGetNumberOfRows(SmiNode *smiNode)
{
    SmiNode *tSmiNode;
    SmiNode *table;
    int     elemCount;

    elemCount = 0;
    table = smiNode;
    tSmiNode = smiGetFirstChildNode(table);
    elemCount = algCountElements(tSmiNode);
    for (tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_COLUMN);
	 tSmiNode;
	 tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_COLUMN)) {
    
	if (table->name != smiGetParentNode(smiGetParentNode(tSmiNode))->name)
	    break;
  
	/* evtl. noch RowPointer einfuegen */
	if (strcmp(algGetTypeName(tSmiNode), "RowStatus") != 0 &&
	    strcmp(algGetTypeName(tSmiNode), "StorageType") != 0)  
	    elemCount--;
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
SmiNode *algFindEqualType(SmiNode *startTable, SmiNode *typeNode)
{
    SmiElement *smiElement;
    SmiNode    *tSmiNode;
    char       *typeName;

    typeName = algGetTypeName(typeNode);

    for (tSmiNode = smiGetFirstNode(module, SMI_NODEKIND_TABLE);
	 tSmiNode;
	 tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_TABLE)) {
	if (tSmiNode->name == startTable->name) break;
    
	for (smiElement = smiGetFirstElement(smiGetFirstChildNode(tSmiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    /* found type matching ? */
	    if (strcmp(typeName, algGetTypeName(smiGetElementNode(smiElement)))== 0) {
		return tSmiNode;
	    }
	}
    }
  
    return NULL;
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
Graph *algCheckForDependency(Graph *graph) 
{
    GraphEdge  *tEdge;
    SmiNode    *endTable, *startTable, *depTable;
    SmiElement *smiElement;
    int        elemCount;

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {

	if (tEdge->indexkind == SMI_INDEX_EXPAND) {
     
	    elemCount = algGetNumberOfRows(tEdge->startNode->smiNode);
	    if (elemCount > 0) tEdge->connection = GRAPH_CON_DEPENDENCY;

	    elemCount = algGetNumberOfRows(tEdge->endNode->smiNode);
	    if (elemCount > 0) tEdge->connection = GRAPH_CON_DEPENDENCY;
	}
    }

    for (tEdge = graphGetFirstEdge(graph);
	 tEdge;
	 tEdge = graphGetNextEdge(graph, tEdge)) {

	if (tEdge->connection == GRAPH_CON_DEPENDENCY) {
      
	    startTable = tEdge->startNode->smiNode;
	    endTable = tEdge->endNode->smiNode;
      
	    for (smiElement = smiGetFirstElement(smiGetFirstChildNode(endTable));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
	
		/* look only at expanded indices (only present in endTable) */
		if (smiGetParentNode(smiGetParentNode(smiGetElementNode(smiElement)))->name ==
		    endTable->name) {
		    depTable = algFindEqualType(startTable, smiGetElementNode(smiElement));
		    /* depTable found and different to endTable ? */
		    if (depTable && (strcmp(depTable->name, endTable->name) != 0)) {
			/* insert only non-redundant edges -> example RMON2*/
			/*if (graphCheckForRedundantEdge(graph,
			  graphGetNode(graph,depTable), 
			  graphGetNode(graph,startTable),
			  SMI_INDEX_UNKNOWN) == 0) */
			graph = algInsertEdge(graph, depTable, startTable, SMI_INDEX_UNKNOWN);
			break;
		    }
		}
	    }
	}
    }

#ifdef DEBUG
    printf("\n--- Fourth Phase - checking for dependency connections ... \n\n");
    graphShowEdges(graph);
#endif

    return graph;
}

Graph *algCheckNotifications(Graph *graph)
{
    SmiNode    *smiNode;
    SmiElement *smiElement;

#ifdef DEBUG
    printf("\n--- Fifth Phase - notifications ... \n\n");

    for (smiNode = smiGetFirstNode(module, SMI_NODEKIND_NOTIFICATION);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {
	printf("Node : %s\n",smiNode->name);
    
	for (smiElement = smiGetFirstElement(smiNode);
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    printf("Elem : %30s -- Father : %30s\n",smiGetElementNode(smiElement)->name,
		   smiGetParentNode(smiGetParentNode(smiGetElementNode(smiElement)))->name);
	}

    }

#endif

    return graph;
}





/* ------ XML primitives ------                                                    */





void printHeader()
{
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
}

void printCloseXML()
{
    printf("  </dia:layer>\n");
    printf("</dia:diagram>\n");
}

void printXML(Graph *graph)
{
    GraphNode *tNode;

    printHeader();

    for (tNode = graphGetFirstNode(graph);
	 tNode;
	 tNode = graphGetNextNode(graph, tNode)) {
    }

    printCloseXML();
}



/* --------------------------- */



/*
 * dumpCM
 *
 * Interfacefuntion to smidump.
 */
int dumpCM(char *modulename, int flags)
{
    Graph     *graph;

    if (!modulename) {
	fprintf(stderr, "smidump: united output not supported for cm format\n");
	exit(1);
    }
  
    module = smiGetModule(modulename);
    if (!module) {
	fprintf(stderr, "smidump: cannot locate module `%s'\n", modulename);
	exit(1);
    }

    graph        = xmalloc(sizeof(Graph));
    graph->nodes = NULL;
    graph->edges = NULL;

#ifdef DEBUG
    printf("\nConceptual Model of %s - generated by smidump " VERSION "\n\n",modulename);
#endif

    graph = algLinkTables(graph);
    graph = algCheckLinksByName(graph);
#if 1
    graph = algConnectLonelyNodes(graph);
#endif
    graph = algCheckForDependency(graph);
#if 1
    graph = algCheckNotifications(graph);
#endif

#ifndef DEBUG
    printXML(graph);
#endif

    graphExit(graph);

    return 0;
}

