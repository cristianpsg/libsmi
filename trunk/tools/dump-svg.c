/*
 * dump-svg.c --
 *
 *      Operations to extract a SVG diagram from MIB modules.
 *      This driver is based on the cm-driver by A. Mueller.
 *      This is work in progress.
 *      Mail comments and suggestions to sperner@ibr.cs.tu-bs.de
 *
 * Copyright (c) 2004 K. Sperner, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */



#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"
#include "smidump.h"


extern int smiAsprintf(char **strp, const char *format, ...);

#define ABS(a) ((float)((a > 0.0) ? (a) : (-(a))))



typedef enum GraphCardinality {
    GRAPH_CARD_UNKNOWN            = 0,
    GRAPH_CARD_ONE_TO_ONE         = 1,
    GRAPH_CARD_ONE_TO_MANY        = 2,
    GRAPH_CARD_ZERO_TO_ONE        = 3,
    GRAPH_CARD_ZERO_TO_MANY       = 4,
    GRAPH_CARD_ONE_TO_ZERO_OR_ONE = 5
} GraphCardinality;

typedef enum GraphConnection {
    GRAPH_CON_UNKNOWN       = 0,
    GRAPH_CON_AGGREGATION   = 1,	/* never used??? */
    GRAPH_CON_DEPENDENCY    = 2,
    GRAPH_CON_ASSOCIATION   = 3
} GraphConnection;

typedef enum GraphEnhIndex {
    GRAPH_ENHINDEX_UNKNOWN      = 0,
    GRAPH_ENHINDEX_NOTIFICATION = 1,
    GRAPH_ENHINDEX_TYPES        = 2,
    GRAPH_ENHINDEX_NAMES        = 3,
    GRAPH_ENHINDEX_INDEX        = 4,
    GRAPH_ENHINDEX_REROUTE      = 5,
    GRAPH_ENHINDEX_POINTER      = 6
} GraphEnhIndex;

/*
 * Definition used by the dia output driver.
 */

typedef struct DiaNode {
    float x,y;			/* coordinates (left upper corner) */
    float xDisp,yDisp;		/* displacement vector for springembedder */
    float w,h;			/* width and height of the dia node */
} DiaNode;

/*
 * Generic structures for the internal graph representation.
 */

typedef struct GraphCluster {
    struct GraphCluster *nextPtr;
    struct GraphNode    *firstClusterNode;
    float               xMin;
    float               xMax;
    float               yMin;
    float               yMax;
    float               xOffset;
    float               yOffset;
} GraphCluster;

typedef struct GraphNode {
    struct GraphNode *nextPtr;
    SmiNode          *smiNode;
    int              group;		/* group number of this graph node */
    int              use;		/* use node in the layout-algorithm */
    int              degree;		/* quantity of adjacent nodes */
    GraphCluster     *cluster;		/* cluster the node belongs to */
    struct GraphNode *nextClusterNode;
    DiaNode          dia;
} GraphNode;

typedef struct GraphEdge {
    struct GraphEdge *nextPtr;         
    GraphNode        *startNode;
    GraphNode        *endNode;
    SmiIndexkind     indexkind;
    GraphConnection  connection;
    GraphCardinality cardinality;
    GraphEnhIndex    enhancedindex;
    int              use;		/* use edge in the layout-algorithm */
} GraphEdge;

typedef struct Graph {
    GraphNode    *nodes;
    GraphEdge    *edges;
    GraphCluster *clusters;
} Graph;


/*
 * Constants used by the reverse engineering algorithm.
 */

static char *pointer[] = {
    "Index", NULL
};

static char *suffix[] = {
    "OrZero", NULL
};

static char *supportObjs[] = {
    "RowStatus", "StorageType", NULL
};

static char *baseTypes[] = {
    "Integer32", "OctetString", "Unsigned32", "Integer64",
    "Unsigned64", "Float32", "Float64", "Float128",
    "Enumeration", "Counter32", "Counter64","Bits",
    "Gauge", "Gauge32", "Integer", "TimeTicks",
    "IpAddress", "Opaque", "ObjectIdentifier",
    NULL
};

/*
 * Definitions used by the dia output driver (node layout).
 */

//FIXME int or float?
static const float HEADFONTSIZETABLE   = (float)7;
static const float HEADSPACESIZETABLE  = (float)4;
static const float ATTRFONTSIZE        = (float)6;
static const float ATTRSPACESIZE       = (float)2;
static const float TABLEHEIGHT         = (float)20; /*headline of the table*/
static const float TABLEELEMHEIGHT     = (float)15; /*height of one attribute*/
static const float TABLEBOTTOMHEIGHT   = (float)5;  /*bottom of the table*/

static const int MODULE_INFO_WIDTH     =150;
//The description of RowStatus is quite long... :-/
static const int DYN_TEXT              =550;
//TODO make these values configurable by options passed to the driver
static const int CANVASHEIGHT          =700;
static const int CANVASWIDTH           =1100;
static const float STARTSCALE          =(float)0.5;

//used by the springembedder
static const int ITERATIONS            =100;

/*
 * Property String for index objects
 */
static const char* INDEXPROPERTY       = " {index}";

/*
 * driver output control
 */
static int       XPLAIN                = 0; /* false, generates ASCII output */
static int       DEBUG_XPLAIN          = 0; /* false, generates additional
					       output in xplain-mode */
static int       SUPPRESS_DEPRECATED   = 1; /* true, suppresses deprecated
					       objects */
static int       PRINT_DETAILED_ATTR   = 1; /* true, prints all column
					       objects */
static int       IGNORE_IMPORTED_NODES = 1; /* true, ignores nodes which are
					       imported from other MIBs*/

/*
 * global variables
 */
static Graph     *graph  = NULL;            /* the graph */

/*
 * help functions
 */
#ifndef max
#define max(a, b) ((a < b) ? b : a)
#endif
#ifndef min
#define min(a, b) ((a < b) ? a : b)
#endif





/* ------ Misc. -----------------                                            */


static char *getTimeString(time_t t)
{
    static char   *s = NULL;
    struct tm	  *tm;

    if (s) xfree(s);
    
    tm = gmtime(&t);
    smiAsprintf(&s, "%04d%02d%02d%02d%02dZ",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min);
    return s;
}


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
    
    if (!node1 || !node2 || !module1 || !module2) return 0;
    
    return (strcmp(node1->name, node2->name) == 0 &&
	    strcmp(module1->name, module2->name) == 0);
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





/* ------ Graph primitives ------                                            */


/*
 * graphInsertCluster
 *
 *          Inserts a new cluster into an existing graph.
 *
 * Result : pointer to the new cluster
 */
static GraphCluster *graphInsertCluster(Graph *graph)
{
    GraphCluster *newCluster;
    GraphCluster *tCluster;
    GraphCluster *lastCluster;

    newCluster = xmalloc(sizeof(GraphCluster));
    memset(newCluster, 0, sizeof(GraphCluster));

    if (graph->clusters == NULL) {
	graph->clusters = newCluster;
	return newCluster;
    }

    lastCluster = NULL;
    for (tCluster = graph->clusters; tCluster; tCluster = tCluster->nextPtr) {
	lastCluster = tCluster;
    }

    lastCluster->nextPtr = newCluster;

    return newCluster;
}


/*
 * graphInsertNode
 *
 *          Inserts a new node into an existing graph.
 *
 * Result : pointer to the new node
 */
static GraphNode *graphInsertNode(Graph *graph, SmiNode *smiNode)
{
    GraphNode *newNode;
    GraphNode *tNode;
    GraphNode *lastNode;

    newNode = xmalloc(sizeof(GraphNode));
    memset(newNode, 0, sizeof(GraphNode));
    newNode->smiNode = smiNode;

    if (graph->nodes == NULL) {
	graph->nodes = newNode;
	return newNode;
    }

    lastNode = NULL; 
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	lastNode = tNode;
    }

    lastNode->nextPtr = newNode;

    return newNode;
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
 * Result : pointer to the new edge
 */
static GraphEdge *graphInsertEdge(Graph *graph, GraphNode *startNode,
			      GraphNode *endNode,
			      SmiIndexkind indexkind,
			      GraphEnhIndex enhancedindex)
{
    GraphEdge *newEdge;
    GraphEdge *tEdge;
    GraphEdge *lastEdge;

    newEdge = xmalloc(sizeof(GraphEdge));
    memset(newEdge, 0, sizeof(GraphEdge));
    newEdge->startNode = startNode;
    newEdge->endNode = endNode;
    newEdge->indexkind = indexkind;
    newEdge->connection = GRAPH_CON_ASSOCIATION;
    newEdge->enhancedindex = enhancedindex;

    switch (newEdge->indexkind) {
    case SMI_INDEX_AUGMENT : 
	newEdge->cardinality = GRAPH_CARD_ONE_TO_ONE; 
	break;
    case SMI_INDEX_SPARSE  : 
	newEdge->cardinality = GRAPH_CARD_ONE_TO_ZERO_OR_ONE; 
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
	return newEdge;
    }

    lastEdge = NULL; 
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	lastEdge = tEdge;
    }

    lastEdge->nextPtr = newEdge;

    return newEdge;
}

/*
 * graphExit
 *
 * Frees all memory allocated by the graph.
 */
static void graphExit(Graph *graph)
{
    GraphEdge *tEdge, *dummyEdge;
    GraphNode *tNode, *dummyNode;
    GraphCluster *tCluster, *dummyCluster;

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
    
	dummyCluster = NULL;
	tCluster = graph->clusters;
	while (tCluster != NULL) {
	    dummyCluster = tCluster;
	    tCluster = tCluster->nextPtr;
      
	    xfree(dummyCluster);
	}
    
	xfree(graph);
    }
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
    
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	/*if (tEdge->startNode->smiNode->name == node->smiNode->name ||
	  tEdge->endNode->smiNode->name == node->smiNode->name) break;*/
	if (cmpSmiNodes(tEdge->startNode->smiNode, node->smiNode) ||
	    cmpSmiNodes(tEdge->endNode->smiNode, node->smiNode)) break;
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
    
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	/*if (tEdge->startNode->smiNode->name ==
	    edge->startNode->smiNode->name &&
	    tEdge->endNode->smiNode->name ==
	    edge->endNode->smiNode->name) break;*/
	if (cmpSmiNodes(tEdge->startNode->smiNode,edge->startNode->smiNode) &&
	    cmpSmiNodes(tEdge->endNode->smiNode,edge->endNode->smiNode))
	    break;
    }
  
    for (tEdge = tEdge->nextPtr; tEdge; tEdge = tEdge->nextPtr) {
	/*if (tEdge->startNode->smiNode->name == node->smiNode->name ||
	  tEdge->endNode->smiNode->name == node->smiNode->name) break;*/
	if (cmpSmiNodes(tEdge->startNode->smiNode, node->smiNode) ||
	    cmpSmiNodes(tEdge->endNode->smiNode, node->smiNode)) break;
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
    
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	/*if (tEdge->startNode->smiNode->name == startNode->smiNode->name &&
	  tEdge->endNode->smiNode->name == endNode->smiNode->name) {*/
	if (cmpSmiNodes(tEdge->startNode->smiNode, startNode->smiNode) &&
	    cmpSmiNodes(tEdge->endNode->smiNode, endNode->smiNode)) {
	    
	    return 1;
	}
    }
    
    return 0;
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
    
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
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

    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->nodekind == SMI_NODEKIND_TABLE)
	    printf(" [TABLE]");
	else printf("[SCALAR]");
	printf("%40s [%s]\n", tNode->smiNode->name,
	       smiGetNodeModule(tNode->smiNode)->name);
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
 */
static void graphShowEdges(Graph *graph) 
{
    GraphEdge  *tEdge;
    
    if (!graph->edges) {
	printf("No edges!\n");
	return;
    }
  
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {

	if (DEBUG_XPLAIN) {
	    switch (tEdge->enhancedindex) {
	    case GRAPH_ENHINDEX_UNKNOWN :
		printf("[UNKNOWN] ");
		break;
	    case GRAPH_ENHINDEX_NOTIFICATION :
		break;
	    case GRAPH_ENHINDEX_NAMES :
		printf("  [NAMES] ");
		break;
	    case GRAPH_ENHINDEX_TYPES :
		printf("  [TYPES] ");
		break;
	    case GRAPH_ENHINDEX_INDEX :
		printf("  [INDEX] ");
		break;
	    case GRAPH_ENHINDEX_REROUTE :
		printf("[REROUTE] ");
		break;
	    case GRAPH_ENHINDEX_POINTER :
		printf("[POINTER] ");
		break;	    
	    }
	}
	
	switch (tEdge->connection) {
	case GRAPH_CON_AGGREGATION:	/* never used??? */
	    printf("AG.");
	    break;
	case GRAPH_CON_DEPENDENCY:
	    printf("D. ");
	    break;
	case GRAPH_CON_ASSOCIATION:
	    printf("A. ");
	    break;    
	case GRAPH_CON_UNKNOWN:
	    break;
	}

	switch (tEdge->cardinality) {
	case GRAPH_CARD_UNKNOWN      :
	    printf("  (-:-)  ");
	    break;
	case GRAPH_CARD_ONE_TO_ONE   :
	    printf("  (1:1)  ");
	    break;
	case GRAPH_CARD_ONE_TO_MANY  :
	    printf("  (1:n)  ");
	    break;
	case GRAPH_CARD_ZERO_TO_ONE  :
	    printf("  (0:1)  ");
	    break;
	case GRAPH_CARD_ZERO_TO_MANY :
	    printf("  (0:n)  ");
	    break;
	case GRAPH_CARD_ONE_TO_ZERO_OR_ONE :
	    printf("(1:0..1) ");
	    break;	    
	}

	switch (tEdge->indexkind) {
	case SMI_INDEX_UNKNOWN  :
	    printf("GENERIC ");
	    break;
	case SMI_INDEX_INDEX    :
	    printf("  INDEX ");
	    break;
	case SMI_INDEX_AUGMENT  :
	    printf("AUGMENT ");
	    break;
	case SMI_INDEX_SPARSE   :
	    printf(" SPARSE ");
	    break;
	case SMI_INDEX_EXPAND   :
	    printf(" EXPAND ");
	    break;
	case SMI_INDEX_REORDER  :
	    printf("REORDER ");
	    break;
	}
	printf("%29s - ",tEdge->startNode->smiNode->name);
	printf("%s\n",tEdge->endNode->smiNode->name);
    }
}





/* ------ algorithm primitives ------                                        */



/*
 * algCountIndexElements
 *
 * Returns the number of index elements in a given row entry.
 */

static int algCountIndexElements(SmiNode *smiNode) 
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
static void algInsertEdge(SmiNode *snode, SmiNode *enode, 
			  SmiIndexkind indexkind,
			  GraphEnhIndex enhancedindex) 
{
    GraphNode *startNode;
    GraphNode *endNode;

    if (!graph) return;

    startNode = graphGetNode(graph, snode);
    endNode   = graphGetNode(graph, enode);

    /* insert imported nodes into graph if needed */
    if (startNode == NULL) {
	if (IGNORE_IMPORTED_NODES) return;
	
	startNode = graphInsertNode(graph, snode);
    }
    if (endNode == NULL) {
	if (IGNORE_IMPORTED_NODES) return;
	
	endNode = graphInsertNode(graph, enode);
    }  

    if (graphCheckForRedundantEdge(graph, startNode, endNode) == 0 &&
	graphCheckForRedundantEdge(graph, endNode, startNode) == 0) {
	graphInsertEdge(graph, startNode, endNode, indexkind, enhancedindex); 
    }
}

/*
 * algGetGroupByFatherNode
 *
 * Returns the group number associated with the father node of the
 * given node. If there is no group the result is 0 for no
 * grouping.
 */
static int algGetGroupByFatherNode(SmiNode *smiNode)
{
    GraphNode *tNode;
    
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR &&
	    !graphGetFirstEdgeByNode(graph, tNode)) {
	    if (cmpSmiNodes(smiGetParentNode(smiNode),
			    smiGetParentNode(tNode->smiNode))) {
		return tNode->group;
	    }
	}
    }

    return 0;
}

/*
 * algGetNumberOfGroups
 *
 * Returns the number of groups.
 */
static int algGetNumberOfGroups()
{
    GraphNode *tNode;
    int       maxGroup;

    maxGroup = 0;

    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	maxGroup = max(maxGroup, tNode->group); 
    }

    return maxGroup;
}

/*
 * algPrintGroup
 *
 * Prints the group with the number group.
 */
static void algPrintGroup(int group)
{
    GraphNode *tNode;
    
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->group == group) {
	    printf("%2d - %35s\n", group, tNode->smiNode->name);
	}
    }
}

/*
 * algGetTypeDescription
 *
 * Returns the description of the data type used in smiNode.
 */
static char *algGetTypeDescription(SmiNode *smiNode)
{
    SmiType *smiType, *parentType;
  
    smiType = smiGetNodeType(smiNode);
  
    if (!smiType || smiNode->nodekind == SMI_NODEKIND_TABLE)
	return NULL;
  
    if (smiType->decl == SMI_DECL_IMPLICIT_TYPE) {
	parentType = smiGetParentType(smiType);
	smiType = parentType;
    }
  
    return smiType->description;
}

/*
 * algGetTypeName
 *
 * Returns the name of the data type used in smiNode.
 */
static char *algGetTypeName(SmiNode *smiNode)
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

    elemCount = algCountIndexElements(tSmiNode) -
	algCountElementsFromOtherTables(tSmiNode);

    for (tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_COLUMN);
	 tSmiNode;
	 tSmiNode = smiGetNextNode(tSmiNode, SMI_NODEKIND_COLUMN)) {

	if (cmpSmiNodes(table, smiGetParentNode(smiGetParentNode(tSmiNode)))
	    != 1) {
	    break;
	}
	
	for (i = 0; supportObjs[i]; i++) {
	    char *typeName = algGetTypeName(tSmiNode);
	    if (typeName && (strcasecmp(typeName, supportObjs[i]) == 0)) {
		break;
	    }
	}

	if (!supportObjs[i]) elemCount--;
    }

    return elemCount;
}

/*
 * isBaseType
 *
 * returns 1 if smiElement is a basetype used in SMIv2/SMIng. Otherwise
 * the result is 0.
 */
static int isBaseType(SmiNode *node)
{
    int i;

    for (i = 0; baseTypes[i]; i++) {
	if (strcasecmp(algGetTypeName(node), baseTypes[i]) == 0) {
	    return 1;
	}
    }

    return 0;
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
    GraphNode  *tNode;
    
    typeName = algGetTypeName(typeNode);
    /* if (isBaseType(typeNode)) return NULL; */

    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {

	    tSmiNode = tNode->smiNode;
	    if (cmpSmiNodes(tSmiNode, startTable) == 1) break;
	    
	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(tSmiNode));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
		/* found type matching ? */
		if (strcmp(typeName,
			       algGetTypeName(smiGetElementNode
					      (smiElement))) == 0) {
		    return tSmiNode;
		}
	    }
	    
	}
    }
    
    return NULL;
}

/*
 * 0 = totaly equal (order and objects)
 * 1 = equal objects but different order
 * 2 = inequal
 * 3 = error;
 */
 static int equalElements(SmiNode *firstRow, SmiNode *secondRow)
{
    int i,j,number1, number2;
    SmiElement *elemFirst, *elemSecond;

    if (!firstRow->nodekind == SMI_NODEKIND_ROW ||
	!secondRow->nodekind == SMI_NODEKIND_ROW) {
	/* printf("no row entries\n"); */
	return 3;
    }

    if (cmpSmiNodes(firstRow, secondRow) == 1) {
	/* printf("same objects\n"); */
	return 3;
    }
    
    number1 = algCountIndexElements(firstRow);
    number2 = algCountIndexElements(secondRow);

    if (number1 != number2) {
	/* printf("Different number of elements\n"); */
	return 2;
    }

    i = 0;
    for (elemFirst = smiGetFirstElement(firstRow);
	 elemFirst;
	 elemFirst = smiGetNextElement(elemFirst)) {
	i++;

	j = 0;
	for (elemSecond = smiGetFirstElement(secondRow);
	     elemSecond;
	     elemSecond = smiGetNextElement(elemSecond)) {
	    j++;
	    if (i == j && cmpSmiNodes(smiGetElementNode(elemFirst),
				      smiGetElementNode(elemSecond)) == 1) {
		break;
	    }
	}

	/* maybe same elements but in different order */
	if (!elemSecond) break;
    }
    if (!elemFirst) {
	/* printf("totaly equal\n"); */
	return 0;
    }

    for (elemFirst = smiGetFirstElement(firstRow);
	 elemFirst;
	 elemFirst = smiGetNextElement(elemFirst)) {

	for (elemSecond = smiGetFirstElement(secondRow);
	     elemSecond;
	     elemSecond = smiGetNextElement(elemSecond)) {

	    if (cmpSmiNodes(smiGetElementNode(elemFirst),
			    smiGetElementNode(elemSecond)) == 1) {
		break;
	    }
	}

	/* maybe same elements but in different order */
	if (!elemSecond) break;
    }

    if (!elemFirst) {
	/* printf("Equal\n"); */
	return 1;
    }
    else {
	/* printf("inequal"); */
	return 2;
    }
}

/*
 * algIsIndexElement
 *
 * Tests whether a given node is part of the index of a particular
 * table. Returns 1 if the node is an index node and 0 otherwise.
 */

static int algIsIndexElement(SmiNode *table, SmiNode *node)
{
    SmiElement *smiElement;
    SmiNode    *row;

    if (node->nodekind != SMI_NODEKIND_TABLE) {
	return 0;
    }
    
    row = smiGetFirstChildNode(table);
    if (!row || row->nodekind != SMI_NODEKIND_ROW) {
	return 0;
    }
    
    for (smiElement = smiGetFirstElement(row);
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	SmiNode *indexNode = smiGetElementNode(smiElement);
	if (cmpSmiNodes(indexNode, node)) {
	    return 1;
	}
    }

    return 0;
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
static void algCheckForExpandRel(SmiNode *smiNode) 
{
    SmiNode      *tNode;
    SmiNode      *baseTable;
    SmiNode      *expTable;
    SmiElement   *smiElement;
    SmiElement   *findElement;
    unsigned int refcounter;
    unsigned int basecounter;

    if (!smiNode) return;

    expTable = smiGetParentNode(smiNode);  

    /* count the elements in the given table <- father of smiNode */
    refcounter = algCountIndexElements(smiNode);

    /* find the base table <- via the last element which does not refer to 
       the expTable */
    baseTable = NULL;
    for (smiElement = smiGetFirstElement(smiNode); 
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {  
	tNode = smiGetElementNode(smiElement);
	tNode = smiGetParentNode(tNode);
	tNode = smiGetParentNode(tNode);

	if (cmpSmiNodes(tNode, expTable) == 1) break;
	
	baseTable = tNode;
    }  

    if (!baseTable) return;

    /* count the elements in the basetable */
    basecounter = algCountIndexElements(smiGetFirstChildNode(baseTable));

    /* are baseTable and expTable identical ? */
    if (basecounter >= refcounter) {

	/* searching for new base table candidate in order to handle multiple
	   indices */
	for (baseTable = smiGetNextNode(baseTable, SMI_NODEKIND_TABLE);
	     baseTable;
	     baseTable = smiGetNextNode(baseTable, SMI_NODEKIND_TABLE)) {
      
	    basecounter = algCountIndexElements(smiGetFirstChildNode(baseTable));
      
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
			if (cmpSmiNodes(tNode, smiGetElementNode(findElement))
			    == 1) break;
		    }
	  
		    if (!findElement) {
			return;
		    }
		}	
		break;
	    }
	}
    
	if (!baseTable) {
	    return;
	}
    }

    for (smiElement = smiGetFirstElement(smiGetFirstChildNode(baseTable)); 
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {
	tNode = smiGetElementNode(smiElement);

	for (findElement = smiGetFirstElement(smiGetFirstChildNode(expTable)); 
	     findElement; 
	     findElement = smiGetNextElement(findElement)) {
	    if (cmpSmiNodes(tNode, smiGetElementNode(findElement)) == 1)
		break;
	}
    
	if (!findElement) {
	    return;
	}
    }

    algInsertEdge(baseTable, expTable, SMI_INDEX_EXPAND,
		  GRAPH_ENHINDEX_INDEX);  
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
static void algCheckForSparseRel(SmiNode *smiNode) 
{
    SmiNode      *tNode = NULL;
    SmiNode      *table;
    SmiElement   *smiElement;
    unsigned int basecounter;

    if (!smiNode) return;

    table = smiGetParentNode(smiNode);

    basecounter = algCountIndexElements(smiNode);

    /* getting the basetable via the father node of the last element
       to handle multiple instanceing */
    for (smiElement = smiGetFirstElement(smiNode);
	 smiElement; 
	 smiElement = smiGetNextElement(smiElement)) {
	tNode = smiGetElementNode(smiElement);    
    }

    if (! tNode) {
	return;
    }
    
    tNode = smiGetParentNode(tNode);
    if (equalElements(smiNode, tNode) == 0) {
	algInsertEdge(smiGetParentNode(tNode), smiGetParentNode(smiNode), 
		      SMI_INDEX_SPARSE, GRAPH_ENHINDEX_INDEX);
    }
}

/*
 * algCheckForReOrderRel
 *
 * Checks the given table for a reorder relationship to an other table.
 *
 * Criterias for reoder relationships :
 *    - same number of elements
 *    - same elements must occur in a different order
 */
static void algCheckForReorderRel(SmiNode *smiNode)
{
    SmiNode   *tNode;
    GraphNode *graphNode;
    
    if (!smiNode) {
	return;
    }

    for (graphNode = graph->nodes;
	 graphNode;
	 graphNode = graphNode->nextPtr) {	
	tNode = graphNode->smiNode;
	if (tNode->nodekind == SMI_NODEKIND_TABLE) {
	    if (equalElements(smiNode, smiGetFirstChildNode(tNode)) == 1) {
		algInsertEdge(smiGetParentNode(smiNode), tNode,
			      SMI_INDEX_REORDER, GRAPH_ENHINDEX_INDEX);
		break;
	    }
	}
    }
}

/*
 * algGetIndexkind
 *
 * Gets the indexkind of the given table. The row object of the table is
 * passed to this function.
 * Therefore three subfunctions are called to get the indexkind. 
 *   - algChechForExpandRel
 *   - algCheckForSparseRel
 *   - algCheckForReOrderRel
 * Look there for further information.
 */
static void algGetIndexkind(SmiNode *row) 
{
    algCheckForExpandRel(row);
    algCheckForSparseRel(row);
    algCheckForReorderRel(row);
}

/*
 * algLinkTables
 *
 * Links the tables of the given module.
 *
 *  1. Getting the tables and the scalars from the given module.
 *  2. Linking the tables :
 *     - AUGMENT no work to do just adding the corresponding edge
 *     - for other relationships the subfunction algGetIndexkind is called
 *       look there for further information  
 */
static void algLinkTables() 
{
    GraphNode  *tGraphNode;
    SmiNode    *node; 
    SmiNode    *tSmiNode;

    /* linking the tables */
    for (tGraphNode = graph->nodes;
	 tGraphNode;
	 tGraphNode = tGraphNode->nextPtr) {
	node = tGraphNode->smiNode;

	if (node->nodekind == SMI_NODEKIND_TABLE) {
	    node = smiGetFirstChildNode(node);

	    if (node->nodekind == SMI_NODEKIND_ROW) {

		/* get the relationship between the tables and insert
		   the edges */
		if (node->indexkind == SMI_INDEX_INDEX) {
		    algGetIndexkind(node);
		} else {
		    tSmiNode = node;
		    node = smiGetRelatedNode(node);
		    node = smiGetParentNode(node);
		    algInsertEdge(node,
				  smiGetParentNode(tSmiNode), 
				  tSmiNode->indexkind,
				  GRAPH_ENHINDEX_INDEX);	    
		}
	    }
	}
    }

   if (XPLAIN) {
       printf("--- Second Phase - linking the tables\n\n");
       graphShowEdges(graph);
   } 
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
 * Only legal overlappings lead to a new edge. A illegal overlap is :
 * - when the next character in both strings is a lower case character
 *   because if something like this is detected the two strings are
 *   corresponding in the middle of their names and not at defined points
 *   like suffix or prefix (-> the next character must be upper case or NULL)
 */
static void algCheckLinksByName() 
{
    GraphEdge *tEdge, *tEdge2, *newEdge;
    char      *start, *end, *end2;
    int       overlap, longestOverlap, i;

    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {

	if (tEdge->indexkind == SMI_INDEX_EXPAND) {

	    start = tEdge->startNode->smiNode->name;
	    end = tEdge->endNode->smiNode->name;

	    overlap = strpfxlen(start,end);
      
	    /*
	     * looking for better connection with longer overlap
	     * maybe better to traverse the edges with graphGetNextEdgeByNode
	     * using tEdge->startNode
	     */  
	    newEdge = NULL;
	    longestOverlap = overlap;
	    for (tEdge2 = graphGetFirstEdgeByNode(graph,tEdge->startNode);
		 tEdge2;
		 tEdge2 = graphGetNextEdgeByNode(graph, tEdge2,
						 tEdge->startNode)) {
	
		/*
		 * must be a sparse relationship to get a correct new edge
		 */
		if (tEdge2->indexkind == SMI_INDEX_SPARSE) {
		    end2 = tEdge2->endNode->smiNode->name;
	  
		    /*
		     * new overlap longer and different tables ? 
		     */
		    i = strpfxlen(end,end2);
		    if (overlap < i &&
			!cmpSmiNodes(tEdge->endNode->smiNode,
				     tEdge2->endNode->smiNode)) {
			/*
			 * legal overlap ?
			 */
			if (islower((int)end[i]) && islower((int)end2[i])) {
			    break;
			}

			longestOverlap=i;
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

    if (XPLAIN) {
	printf("\n--- Third Phase - reordering the connections\n\n");
	graphShowEdges(graph);
    }
}

/*
 * algLinkObjectsByNames
 *
 * Links Scalars to Tables using the prefix
 * Links Tables to Tables using the prefix
 */
static void algLinkObjectsByNames()
{
    GraphNode *tNode, *tNode2;
    int       overlap,minoverlap,new;

    /* getting the minimum overlap for all nodes */
    minoverlap = 10000;
    tNode2 = graph->nodes;
    for (tNode = tNode2->nextPtr; tNode; tNode = tNode->nextPtr) {	
	minoverlap = min(minoverlap, strpfxlen(tNode->smiNode->name,
					       tNode2->smiNode->name));
    }

    /*
     * prefix overlap of one is too short to create any usefull edges
     */
    if (minoverlap == 1) return;
    
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {    
	if (!graphGetFirstEdgeByNode(graph, tNode)) {
	    overlap = minoverlap;

	    for (tNode2 = graph->nodes; tNode2; tNode2 = tNode2->nextPtr) {
		if (cmpSmiNodes(tNode->smiNode, tNode2->smiNode)) continue;
		
		new = strpfxlen(tNode->smiNode->name, tNode2->smiNode->name);
		
		if (new >= overlap) {

		    /*
		     * no scalar - scalar edges
		     */
		    if (tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR &&
			tNode2->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
			continue;
		    }

		    /*
		     * is it a legal prefix
		     * (if the next char is NULL || uppercase)
		     */
		    if (tNode->smiNode->name[new] &&
			tNode2->smiNode->name[new]) {
			if (!isupper((int)tNode->smiNode->name[new]) ||
			    !isupper((int)tNode2->smiNode->name[new])) continue;
		    }
		    
		    overlap = new;
		}
	    }

	    if (overlap == minoverlap) continue;
	    
	    new = 0;
	    for (tNode2 = graph->nodes; tNode2; tNode2 = tNode2->nextPtr) {
		if (cmpSmiNodes(tNode->smiNode, tNode2->smiNode)) continue;

		new = strpfxlen(tNode->smiNode->name, tNode2->smiNode->name);
		
		if (new == overlap && new > minoverlap) {

		    /*
		     * a scalar should only be adjacent to one node
		     */
		    if (tNode2->smiNode->nodekind == SMI_NODEKIND_SCALAR &&
			graphGetFirstEdgeByNode(graph,tNode2)) continue;
		    if (tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR &&
			graphGetFirstEdgeByNode(graph,tNode)) continue;    

		    /*
		     * adding only table -> scalar edges
		     */
		    if (tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR &&
			tNode2->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
			continue;
		    }
		    
		    if (tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
			algInsertEdge(tNode2->smiNode, tNode->smiNode,
				      SMI_INDEX_UNKNOWN,
				      GRAPH_ENHINDEX_NAMES);
		    } else {
			algInsertEdge(tNode->smiNode, tNode2->smiNode,
				      SMI_INDEX_UNKNOWN,
				      GRAPH_ENHINDEX_NAMES);
		    }
		}
	    }
	}
    }
}


/*
 * algGroupScalars
 *
 * Grouping the scalars using a common fathernode.
 */
static void algGroupScalars()
{
    GraphNode *tNode;
    int       actGroup, fGroup;
    
    actGroup = 0;
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {	
	if (!graphGetFirstEdgeByNode(graph, tNode) &&
	    tNode->smiNode->nodekind == SMI_NODEKIND_SCALAR) {
	    fGroup = algGetGroupByFatherNode(tNode->smiNode);
	    if (fGroup == 0) {
		tNode->group = ++actGroup;
	    }
	    else {
		tNode->group = fGroup; 
	    }
	}
    }

    if (XPLAIN) {
	printf("Scalar Groups : \n");

	if (algGetNumberOfGroups() != 0) {
	    for (actGroup = 1;
		 actGroup <= algGetNumberOfGroups();
		 actGroup++) {
		algPrintGroup(actGroup);
		printf("\n");
	    }
	} else printf("No groups!\n");
	printf("\n");
    }
}

/*
 * algLinkLonelyTables
 *
 * Links isolated tables with other tables via the types of the
 * index objects.
 * If no type is found the types are checked via the suffix if they differ
 * only in the range (-OrZero). Therefore the suffix-vector is checked.
 * Basetypes used in SMIv1/v2/ng are sorted out.
 */
static void algLinkLonelyTables()
{
    SmiElement *smiElement, *smiElement2 = NULL;
    GraphNode  *tNode, *tNode2;
    int        i;

    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (!graphGetFirstEdgeByNode(graph, tNode) &&
	    tNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {

	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(tNode->smiNode));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {

		for (tNode2 = graph->nodes;
		     tNode2;
		     tNode2 = tNode2->nextPtr) {
		    if (tNode->smiNode->nodekind == SMI_NODEKIND_TABLE &&
			tNode != tNode2) {

			for (smiElement2 = smiGetFirstElement(
			    smiGetFirstChildNode(tNode2->smiNode));
			     smiElement2;
			     smiElement2 = smiGetNextElement(smiElement2)) {

			    if (strcasecmp(algGetTypeName(
				           smiGetElementNode(smiElement2)),
				       algGetTypeName(
					   smiGetElementNode(smiElement)))
				== 0) {

				if (isBaseType(smiGetElementNode(smiElement))
				    == 1){
				    continue;
				}
				
				graphInsertEdge(graph,tNode2, tNode,
						SMI_INDEX_UNKNOWN,
						GRAPH_ENHINDEX_TYPES);
				break;
			    }
			    else {
				for (i = 0; suffix[i]; i++) {
				    if (strstr(
					algGetTypeName(
					    smiGetElementNode(smiElement)),
					suffix[i])) {
					graphInsertEdge(graph,tNode2,
							tNode,
							SMI_INDEX_UNKNOWN,
							GRAPH_ENHINDEX_TYPES);
					break;
				    }
				}
				
				if (suffix[i]) break;
			    }
			}
		    }
		    if (smiElement2) break;
		}
		if (tNode2) break;
	    }
		
	}
    }
}

/*
 * algConnectLonelyNodes
 *
 * Connect the isolated nodes (scalars and tables)
 *
 * 1. Trying to link tables via the type of the index objects
 * 2. Trying to link scalars to tables using the names
 * 3. Trying to group scalars which have not been adjacent to any edge.
 */
static void algConnectLonelyNodes() 
{
    if (XPLAIN) {
	printf("\n--- Fourth Phase -  connecting isolated nodes\n\n");
    }

    algLinkLonelyTables();

    algLinkObjectsByNames();

    algGroupScalars();

    if (XPLAIN) {
	graphShowEdges(graph);
    }
}

/*
 * Subfunction of algCheckForDependencies
 *
 * change UML-edges to dependecy if possible
 */
static void createDependencies()
{
    GraphEdge *tEdge;
    int       elemCount;
    
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	
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
}

/*
 * returns 1 if a new rerouting edge creates a loop and 0 otherwise
 */
static int isloop(GraphEdge *tEdge, SmiNode *depTable)
{
    GraphEdge *loopEdge;

    for (loopEdge = graphGetFirstEdgeByNode(graph, tEdge->endNode);
	 loopEdge;
	 loopEdge = graphGetNextEdgeByNode(graph,loopEdge,tEdge->endNode)) {

	if ((cmpSmiNodes(loopEdge->startNode->smiNode, depTable)) ||
	    (cmpSmiNodes(loopEdge->endNode->smiNode, depTable) &&
	     (loopEdge != tEdge))) {
	    return 1;
	}
    }

    return 0;
}

/*
 * subfunction of algCheckForDependency
 */
static void rerouteDependencyEdge(GraphEdge *tEdge)
{
    SmiNode    *startTable, *endTable, *depTable;
    SmiElement *smiElement;
    
    startTable = tEdge->startNode->smiNode;
    endTable = tEdge->endNode->smiNode;
    
    for (smiElement = smiGetFirstElement(
	smiGetFirstChildNode(endTable));
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	
	/* look only at expanded indices (only present in endTable) */
	if (cmpSmiNodes(endTable, smiGetParentNode(
	    smiGetParentNode(smiGetElementNode(smiElement)))) == 1) {
	    depTable = algFindEqualType(startTable,
					smiGetElementNode(smiElement));
	    
	    /* depTable different to endTable? */
	    if (depTable && !cmpSmiNodes(depTable, endTable)) {

		/* prevent loops */
		if (isloop(tEdge, depTable)) continue;
		
		algInsertEdge(depTable,
			      startTable, SMI_INDEX_UNKNOWN,
			      GRAPH_ENHINDEX_REROUTE);
		break;
	    }
	}
    }
}

/*
 * algCheckForDependency
 *
 * Tries to change connection types from association to dependency by
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
static void algCheckForDependency() 
{
    GraphEdge  *tEdge;

    createDependencies();
    
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {

	if (tEdge->connection == GRAPH_CON_DEPENDENCY) {

	    rerouteDependencyEdge(tEdge);
	}
    }

    if (XPLAIN) {
	printf("\n--- Fifth Phase - checking for dependency relationships\n\n");
	graphShowEdges(graph);
    }
}

/*
 *
 */
static SmiNode *algFindTable(SmiNode *node)
{
    GraphNode  *tNode;
    SmiNode    *smiNode;
    SmiElement *smiElement;
    char       *toFind;
    
    if (!node) return NULL;

    toFind = xstrdup(node->name
		     + strpfxlen(node->name, smiGetParentNode(node)->name));
		     
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {
	    if (cmpSmiNodes(node, tNode->smiNode)) continue;
	    smiNode = smiGetFirstChildNode(tNode->smiNode);
	    
	    for (smiElement = smiGetFirstElement(smiNode);
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
		smiNode = smiGetElementNode(smiElement);
		if (strstr(smiNode->name, toFind)) {
		    xfree(toFind);
		    return smiGetParentNode(smiGetParentNode(smiNode));
		}
	    }
	}
    }

    xfree(toFind);
    
    return NULL;
}

/*
 * Creates edges based on column-objects pointing to other tables.
 * Column-objects with a substring "Index" are examined.
 */
static void algCheckForPointerRels()
{
    GraphNode *tNode;
    SmiModule *module;
    SmiNode   *smiNode = NULL;
    SmiNode   *ppNode, *table;
    int       i;
    
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->smiNode->nodekind == SMI_NODEKIND_TABLE) {

	    module  = smiGetNodeModule(tNode->smiNode);

	    for (smiNode = smiGetFirstNode(module, SMI_NODEKIND_COLUMN);
		 smiNode;
		 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COLUMN)) {
		ppNode = smiGetParentNode(smiNode);
		ppNode = smiGetParentNode(ppNode);
	
		if (!algIsIndexElement(tNode->smiNode, smiNode) &&
		    cmpSmiNodes(tNode->smiNode, ppNode)) {

		    for (i = 0; pointer[i]; i++) {
			if (strstr(smiNode->name, pointer[i])) {
			    /* printf("%s \n",smiNode->name); */
			    table = algFindTable(smiNode);
			    if (table) {
				algInsertEdge(table, tNode->smiNode,
					      SMI_INDEX_UNKNOWN,
					      GRAPH_ENHINDEX_POINTER);
			    }
			}
		    }
		}
	    }
	}
    }

    if (XPLAIN) {
	printf("\n--- Sixth Phase - checking for pointer relationships\n\n");
	graphShowEdges(graph);	
    }
}

/*
 * Creates the graph nodes of the given module
 */
static void algCreateNodes(SmiModule *module)
{
    SmiNode *node;
    
    /* get tables and scalars from the MIB module */
    for (node = smiGetFirstNode(module, SMI_NODEKIND_TABLE);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_TABLE)) {
	if (node->status != SMI_STATUS_OBSOLETE) {
	    if (SUPPRESS_DEPRECATED && node->status != SMI_STATUS_DEPRECATED)
		graphInsertNode(graph, node);
	}
    }
    for (node = smiGetFirstNode(module, SMI_NODEKIND_SCALAR);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_SCALAR)) {
	if (node->status != SMI_STATUS_OBSOLETE) {
	    if (SUPPRESS_DEPRECATED && node->status != SMI_STATUS_DEPRECATED)
		graphInsertNode(graph, node);
	}
    }
}



/* ------ XML primitives ------                                              */




/*
 * parseTooltip: Parse any input to output to make the text safe for the
 * ShowTooltipMZ-functin in the ecma-script.
 * FIXME: Linebreaks in the input are not necessarily linebreaks in the
 *        tooltip, but they should be :-/
 *        This will need changes in the ecma-script.
 */
static void parseTooltip(char *input, char *output)
{
    int i, j;

    for (i = j = 0; input[i]; i++) {
	switch (input[i]) {
	case '\n':
	case ' ':
	    if (i != 0 && input[i+1] != '\n' && input[i+1] != ' ')
		output[j++] = ' ';
	    break;
	case '\'':
	    output[j++] = '\\';
	    output[j++] = '\'';
	    break;
	case '\\':
	    output[j++] = '\\';
	    output[j++] = '\\';
	    break;
	//FIXME &quot;
	case '\"':
	    output[j++] = '\\';
	    output[j++] = '\'';
	    output[j++] = '\\';
	    output[j++] = '\'';
	    break;
	// '&' should not appear in a tag in a xml-document...
	//FIXME
	case '&':
	    output[j++] = '+';
	    break;
	// '<' and '>' should not appear in a tag in a xml-document...
	//FIXME
	case '<':
	    output[j++] = '(';
	    break;
	case '>':
	    output[j++] = ')';
	    break;
	default:
	    output[j++] = input[i];
	}
    }
    output[j] = '\0';
    if (output[--j] == ' ')
	output[j] = '\0';
}

static int isObjectGroup(SmiNode *groupNode)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    
    for (smiElement = smiGetFirstElement(groupNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {

	smiNode = smiGetElementNode(smiElement);
	
	if (smiNode->nodekind != SMI_NODEKIND_SCALAR
	    && smiNode->nodekind != SMI_NODEKIND_COLUMN) {
	    return 0;
	}
    }

    return 1;
}

static int isNotificationGroup(SmiNode *groupNode)
{
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    
    for (smiElement = smiGetFirstElement(groupNode); smiElement;
	 smiElement = smiGetNextElement(smiElement)) {

	smiNode = smiGetElementNode(smiElement);
	
	if (smiNode->nodekind != SMI_NODEKIND_NOTIFICATION) {
	    return 0;
	}
    }

    return 1;
}

static char *getStatusString(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "current" :
	(status == SMI_STATUS_OPTIONAL)    ? "current" :
					     "<unknown>";
}

/*
 * Prints the footer of the SVG output file.
 */
static void printSVGClose(float xMin, float yMin, float xMax, float yMax)
{
    float scale;
    int i;

    scale = max((xMax-xMin)/CANVASWIDTH,(yMax-yMin)/CANVASHEIGHT);
    printf(" <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"\n",
           xMin, yMin, xMax-xMin-1, yMax-yMin-1);
    printf("       fill=\"none\" stroke=\"blue\" stroke-width=\"1\"/>\n");
    printf(" <g transform=\"translate(%.2f,%.2f) scale(%.2f)\">\n",
           xMin, yMin, scale);
    printf(" <g id=\"tooltip\" style=\"visibility: hidden\">\n");
    printf("   <rect id=\"ttr\" x=\"0\" y=\"0\" rx=\"5\" ry=\"5\"");
				    printf(" width=\"100\" height=\"16\"/>\n");
    printf("   <text id=\"ttt\" x=\"0\" y=\"0\" style=\"visibility: hidden\">");
						printf("dyn. Text</text>\n");
    //FIXME: calculate number of lines dynamically.
    for (i = 0; i < DYN_TEXT; i++) {
	printf("   <text x=\"-10\" y=\"-10\">dyn. Text</text>\n");
    }
    printf(" </g>\n");
    printf(" </g>\n");
    printf("</svg>\n");
}

/*
 * FIXME stimmt das?
 * index = 0 -> no index element
 * index = 1 -> index element -> printed with "+"
 */
static void printSVGAttribute(SmiNode *node, int index,
			      float *textYOffset, float *textXOffset)
{
    char        *tooltip;
    char        *typeDescription;

    printf("    <text id=\"%s\" x=\"%.2f\" y=\"%.2f\"",
			node->name, *textXOffset + ATTRSPACESIZE, *textYOffset);

    *textYOffset += TABLEELEMHEIGHT;

    //FIXME
    //printf(" textLength=\"100\" lengthAdjust=\"spacingAndGlyphs\"");

    if (node->nodekind == SMI_NODEKIND_SCALAR) {
	printf(" style=\"text-decoration:underline\">\n");
    } else {
	printf(">\n");
    }

    if (node->access == SMI_ACCESS_NOT_ACCESSIBLE) {
	printf("         -");
    } else {
	printf("         +");
    }

    printf("<tspan");
    if (node->description) {
	tooltip = (char *)xmalloc(2*strlen(node->description));
	parseTooltip(node->description, tooltip);
	printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	printf(" onmouseout=\"HideTooltip(evt)\"");
	xfree(tooltip);
    }
    printf(">%s:</tspan> \n", node->name);

    printf("         <tspan");
    if (typeDescription = algGetTypeDescription(node)) {
	tooltip = (char *)xmalloc(2*strlen(typeDescription));
	parseTooltip(typeDescription, tooltip);
	printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	printf(" onmouseout=\"HideTooltip(evt)\"");
    }
    if (index) {
	printf(">%s%s</tspan></text>\n", algGetTypeName(node), INDEXPROPERTY);
    } else {
	printf(">%s</tspan></text>\n", algGetTypeName(node));
    }
}

/*
 * prints the related scalars for a given table
 */
static void printSVGRelatedScalars(GraphNode *node,
				   float *textYOffset, float *textXOffset)
{
    GraphEdge *tEdge;
    
    for (tEdge = graphGetFirstEdgeByNode(graph, node);
	 tEdge;
	 tEdge = graphGetNextEdgeByNode(graph, tEdge, node)) {
	if (tEdge->startNode == node  &&
	    tEdge->endNode->smiNode->nodekind == SMI_NODEKIND_SCALAR) {

	    printSVGAttribute(tEdge->endNode->smiNode,
			      0, textYOffset, textXOffset);
	}
    }
}

/*
 * prints all columns objects of the given node
 */
static void printSVGAllColumns(GraphNode *node,
			       float *textYOffset, float *textXOffset)
{
    SmiModule *module  = NULL;
    SmiNode   *smiNode = NULL;
    SmiNode   *ppNode;

    module  = smiGetNodeModule(node->smiNode);

    for (smiNode = smiGetFirstNode(module, SMI_NODEKIND_COLUMN);
	 smiNode;
	 smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COLUMN)) {
	ppNode = smiGetParentNode(smiNode);
	ppNode = smiGetParentNode(ppNode);
	
	if (!algIsIndexElement(node->smiNode, smiNode) &&
	    cmpSmiNodes(node->smiNode, ppNode))
	    printSVGAttribute(smiNode, 0, textYOffset, textXOffset);
    }
}

/*
 * adds the index to an augmenting table (row-element)
 */
static void printSVGAugmentIndex(GraphNode *tNode,
				 float *textYOffset, float *textXOffset)
{
    GraphEdge  *tEdge;
    SmiElement *smiElement;

    for (tEdge = graphGetFirstEdgeByNode(graph, tNode);
	 tEdge;
	 tEdge = graphGetNextEdgeByNode(graph, tEdge, tNode)) {
	if (tEdge->indexkind == SMI_INDEX_AUGMENT) {
	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(tEdge->startNode->smiNode));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
		if (!cmpSmiNodes(tNode->smiNode, tEdge->startNode->smiNode)) {
		    printSVGAttribute(smiGetElementNode(smiElement),
				    1, textYOffset, textXOffset);
		}
	    }
	}
    }
}

/*
 * create svg-output for the given node
 */
static void printSVGObject(GraphNode *node, int *classNr)
{
    SmiElement *smiElement;
    float textXOffset, textYOffset, xOrigin, yOrigin;
    
    if (!node) return;

    xOrigin = node->dia.w/-2;
    yOrigin = node->dia.h/-2;
    textYOffset = yOrigin + TABLEHEIGHT + TABLEELEMHEIGHT;
    textXOffset = xOrigin;

    printf(" <g transform=\"translate(%.2f,%.2f)\">\n",
           node->dia.x + node->cluster->xOffset,
           node->dia.y + node->cluster->yOffset);
    printf("  <g id=\"%s\" transform=\"scale(%.1f)\">\n",
           smiGetFirstChildNode(node->smiNode)->name, STARTSCALE);
    printf("    <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"\n",
           xOrigin, yOrigin, node->dia.w, node->dia.h);
    printf("          fill=\"white\" stroke=\"black\"/>\n");
    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
           xOrigin, yOrigin + TABLEHEIGHT,
           xOrigin + node->dia.w, yOrigin + TABLEHEIGHT);
    printf("          fill=\"none\" stroke=\"black\"/>\n");
    printf("    <text x=\"0\" y=\"%.2f\"\n", yOrigin + 15);
    printf("          style=\"text-anchor:middle; font-weight:bold\">\n");
    printf("         %s</text>\n",smiGetFirstChildNode(node->smiNode)->name);
    //the "+"-button
    printf("    <g onclick=\"enlarge('%s',%i)\"\n",
           smiGetFirstChildNode(node->smiNode)->name, *classNr);
    printf("       transform=\"translate(%.2f,%.2f)\">\n",
           xOrigin + node->dia.w - 26, yOrigin + 3);
    printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" rx=\"2\"\n");
    printf("            style=\"stroke: black; fill: none\"/>\n");
    printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
    printf("          +</text>\n");
    printf("    </g>\n");
    //the "-"-button
    printf("    <g onclick=\"scaledown('%s',%i)\"\n",
           smiGetFirstChildNode(node->smiNode)->name, *classNr);
    printf("       transform=\"translate(%.2f,%.2f)\">\n",
           xOrigin + node->dia.w - 13, yOrigin + 3);
    printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" rx=\"2\"\n");
    printf("            style=\"stroke: black; fill: none\"/>\n");
    printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
    printf("          -</text>\n");
    printf("    </g>\n");

    (*classNr)++;

    if (node->smiNode->nodekind == SMI_NODEKIND_TABLE) {

	//A
	printSVGRelatedScalars(node, &textYOffset, &textXOffset);

	//B
	printSVGAugmentIndex(node, &textYOffset, &textXOffset);
	
	//C
	for (smiElement = smiGetFirstElement(
	    smiGetFirstChildNode(node->smiNode));
	     smiElement;
	     smiElement = smiGetNextElement(smiElement)) {
	    printSVGAttribute(smiGetElementNode(smiElement),
			    1, &textYOffset, &textXOffset);
	}

	//D
	if (PRINT_DETAILED_ATTR) {
	    printSVGAllColumns(node, &textYOffset, &textXOffset);
	}
    }

    printf("  </g>\n");
    printf(" </g>\n");
}

/*
 * prints a group of scalars denoted by group
 */
static void printSVGGroup(int group, int *classNr)
{
    GraphNode *tNode;
    float textXOffset, textYOffset, xOrigin, yOrigin;

    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->group == group) break;
    }

    if (!tNode) return;

    xOrigin = tNode->dia.w/-2;
    yOrigin = tNode->dia.h/-2;
    textYOffset = yOrigin + TABLEHEIGHT + TABLEELEMHEIGHT;
    textXOffset = xOrigin;

    printf(" <g transform=\"translate(%.2f,%.2f)\">\n",
           tNode->dia.x + tNode->cluster->xOffset,
           tNode->dia.y + tNode->cluster->yOffset);
    printf("  <g id=\"%s\" transform=\"scale(%.1f)\">\n",
           smiGetParentNode(tNode->smiNode)->name, STARTSCALE);
    printf("    <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"\n",
           xOrigin, yOrigin, tNode->dia.w, tNode->dia.h);
    printf("          fill=\"white\" stroke=\"black\"/>\n");
    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
           xOrigin, yOrigin + TABLEHEIGHT,
           xOrigin + tNode->dia.w, yOrigin + TABLEHEIGHT);
    printf("          fill=\"none\" stroke=\"black\"/>\n");
    printf("    <text x=\"0\" y=\"%.2f\"\n", yOrigin + 15);
    printf("          style=\"text-anchor:middle; font-weight:bold\">\n");
    printf("         %s</text>\n", smiGetParentNode(tNode->smiNode)->name);
    //the "+"-button
    printf("    <g onclick=\"enlarge('%s',%i)\"\n",
           smiGetParentNode(tNode->smiNode)->name, *classNr);
    printf("       transform=\"translate(%.2f,%.2f)\">\n",
           xOrigin + tNode->dia.w - 26, yOrigin + 3);
    printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" rx=\"2\"\n");
    printf("            style=\"stroke: black; fill: none\"/>\n");
    printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
    printf("          +</text>\n");
    printf("    </g>\n");
    //the "-"-button
    printf("    <g onclick=\"scaledown('%s',%i)\"\n",
           smiGetParentNode(tNode->smiNode)->name, *classNr);
    printf("       transform=\"translate(%.2f,%.2f)\">\n",
           xOrigin + tNode->dia.w - 13, yOrigin + 3);
    printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" rx=\"2\"\n");
    printf("            style=\"stroke: black; fill: none\"/>\n");
    printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
    printf("          -</text>\n");
    printf("    </g>\n");

    (*classNr)++;

    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (tNode->group == group) {
	    printSVGAttribute(tNode->smiNode,
			    0, &textYOffset, &textXOffset);
	}
    }
    
    printf("  </g>\n");
    printf(" </g>\n");
}

static void printSVGDependency(GraphEdge *tEdge)
{
    float alpha, beta, endPointX, endPointY;
    const float PI = acos(-1);

    //calculate intersection of edge and endNode for end-point of arrow
    alpha = atan2(tEdge->startNode->dia.y-tEdge->endNode->dia.y,
		  tEdge->startNode->dia.x-tEdge->endNode->dia.x);
    beta = atan2(tEdge->endNode->dia.h, tEdge->endNode->dia.w);
    if (alpha < 0)
	alpha += PI;
    if (alpha < beta
	|| (alpha > PI-beta && alpha < PI+beta)
	|| alpha > 2*PI-beta) {
	//intersection at left or right border
	if (tEdge->startNode->dia.x > tEdge->endNode->dia.x) {
	    endPointX = tEdge->endNode->dia.x +
			tEdge->endNode->dia.w*STARTSCALE/2;
	} else {
	    endPointX = tEdge->endNode->dia.x -
			tEdge->endNode->dia.w*STARTSCALE/2;
	}
	if (tEdge->startNode->dia.y > tEdge->endNode->dia.y) {
	    endPointY = tEdge->endNode->dia.y +
			fabsf(tEdge->endNode->dia.w*STARTSCALE*tan(alpha)/2);
	} else {
	    endPointY = tEdge->endNode->dia.y -
			fabsf(tEdge->endNode->dia.w*STARTSCALE*tan(alpha)/2);
	}
    } else {
	//intersection at top or bottom border
	if (tEdge->startNode->dia.y > tEdge->endNode->dia.y) {
	    endPointY = tEdge->endNode->dia.y +
			tEdge->endNode->dia.h*STARTSCALE/2;
	} else {
	    endPointY = tEdge->endNode->dia.y -
			tEdge->endNode->dia.h*STARTSCALE/2;
	}
	if (tEdge->startNode->dia.x > tEdge->endNode->dia.x) {
	    endPointX = tEdge->endNode->dia.x +
			fabsf(tEdge->endNode->dia.h*STARTSCALE/(2*tan(alpha)));
	} else {
	    endPointX = tEdge->endNode->dia.x -
			fabsf(tEdge->endNode->dia.h*STARTSCALE/(2*tan(alpha)));
	}
    }

    printf(" <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\"\n",
	tEdge->startNode->dia.x + tEdge->startNode->cluster->xOffset,
	tEdge->startNode->dia.y + tEdge->startNode->cluster->yOffset,
	endPointX + tEdge->endNode->cluster->xOffset,
	endPointY + tEdge->endNode->cluster->yOffset);
    printf("       stroke-dasharray=\"10, 10\" stroke=\"black\"");
    printf(" marker-end=\"url(#arrowhead)\"/>\n");
}

/*
 * Aggregation is a special case of the association.
 * If aggregate = 1 it is an aggregation if 0 it is an association.
 */
static void printSVGAssociation(GraphEdge *tEdge, int aggregate)
{
    int revert = 0;
    float alpha, beta, offset;
    const float PI = acos(-1);

    if (aggregate > 1) aggregate = 1;
    if (aggregate < 0) aggregate = 0;

    //print text upside down, if angle is between 180� and 360�
    if (tEdge->startNode->dia.x > tEdge->endNode->dia.x)
	revert = 1;

    printf(" <path id=\"%s%s\"\n",
	tEdge->startNode->smiNode->name,
	tEdge->endNode->smiNode->name);
    if (!revert) {
	printf("       d=\"M %.2f %.2f %.2f %.2f\"\n",
	    tEdge->startNode->dia.x + tEdge->startNode->cluster->xOffset,
	    tEdge->startNode->dia.y + tEdge->startNode->cluster->yOffset,
	    tEdge->endNode->dia.x + tEdge->endNode->cluster->xOffset,
	    tEdge->endNode->dia.y + tEdge->endNode->cluster->yOffset);
    } else {
	printf("       d=\"M %.2f %.2f %.2f %.2f\"\n",
	    tEdge->endNode->dia.x + tEdge->endNode->cluster->xOffset,
	    tEdge->endNode->dia.y + tEdge->endNode->cluster->yOffset,
	    tEdge->startNode->dia.x + tEdge->startNode->cluster->xOffset,
	    tEdge->startNode->dia.y + tEdge->startNode->cluster->yOffset);
    }
    printf("       stroke=\"black\"/>\n");

    if (!tEdge->cardinality==GRAPH_CARD_UNKNOWN) {
	alpha = atan2(tEdge->endNode->dia.y-tEdge->startNode->dia.y,
		      tEdge->endNode->dia.x-tEdge->startNode->dia.x);
	beta = atan2(tEdge->startNode->dia.h, tEdge->startNode->dia.w);
	if (alpha < 0)
	    alpha += PI;
	if (alpha < beta
	    || (alpha > PI-beta && alpha < PI+beta)
	    || alpha > 2*PI-beta) {
	    //intersection at left or right border
	    offset = 50*tEdge->startNode->dia.w*STARTSCALE
		     / (abs(tEdge->endNode->dia.x-tEdge->startNode->dia.x));
	} else {
	    //intersection at top or bottom border
	    offset = 50*tEdge->startNode->dia.h*STARTSCALE
		     / (abs(tEdge->endNode->dia.y-tEdge->startNode->dia.y));
	}
	printf(" <text text-anchor=\"middle\">\n");
	if (!revert) {
	    printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"%f%\">\n",
		tEdge->startNode->smiNode->name,
		tEdge->endNode->smiNode->name,
		//5% more gap than calculated for tilted text
		5+offset);
	} else {
	    printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"%f%\">\n",
		tEdge->startNode->smiNode->name,
		tEdge->endNode->smiNode->name,
		95-offset);
	}
    }
    switch (tEdge->cardinality) {
    case GRAPH_CARD_ZERO_TO_ONE:
    case GRAPH_CARD_ZERO_TO_MANY:
	printf("       0\n");
	break;
    case GRAPH_CARD_ONE_TO_ONE:
    case GRAPH_CARD_ONE_TO_MANY:
    case GRAPH_CARD_ONE_TO_ZERO_OR_ONE:
	printf("       1\n");
	break;
    case GRAPH_CARD_UNKNOWN:
    }
	if (!tEdge->cardinality==GRAPH_CARD_UNKNOWN) {
	printf("    </textPath>\n");
	printf(" </text>\n");
    }

    if (tEdge->indexkind==SMI_INDEX_AUGMENT ||
	tEdge->indexkind==SMI_INDEX_SPARSE ||
	tEdge->indexkind==SMI_INDEX_REORDER ||
	tEdge->indexkind==SMI_INDEX_EXPAND) {
	printf(" <text text-anchor=\"middle\">\n");
	printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"50%\">\n",
	    tEdge->startNode->smiNode->name,
	    tEdge->endNode->smiNode->name);
    }
    switch(tEdge->indexkind) {
    case SMI_INDEX_AUGMENT:
	printf("       augments\n");
	break;
    case SMI_INDEX_SPARSE:
	printf("       sparses\n");
	break;
    case SMI_INDEX_REORDER:
	printf("       reorders\n");
	break;
    case SMI_INDEX_EXPAND:
	printf("       expands\n");
	break;
    case SMI_INDEX_UNKNOWN:
    case SMI_INDEX_INDEX:
    }
    if (tEdge->indexkind==SMI_INDEX_AUGMENT ||
	tEdge->indexkind==SMI_INDEX_SPARSE ||
	tEdge->indexkind==SMI_INDEX_REORDER ||
	tEdge->indexkind==SMI_INDEX_EXPAND) {
	printf("    </textPath>\n");
	printf(" </text>\n");
    }

    if (!tEdge->cardinality==GRAPH_CARD_UNKNOWN) {
	alpha = atan2(tEdge->startNode->dia.y-tEdge->endNode->dia.y,
		      tEdge->startNode->dia.x-tEdge->endNode->dia.x);
	beta = atan2(tEdge->endNode->dia.h, tEdge->endNode->dia.w);
	if (alpha < 0)
	    alpha += PI;
	if (alpha < beta
	    || (alpha > PI-beta && alpha < PI+beta)
	    || alpha > 2*PI-beta) {
	    //intersection at left or right border
	    offset = 50*tEdge->endNode->dia.w*STARTSCALE
		     / (abs(tEdge->startNode->dia.x-tEdge->endNode->dia.x));
	} else {
	    //intersection at top or bottom border
	    offset = 50*tEdge->endNode->dia.h*STARTSCALE
		     / (abs(tEdge->startNode->dia.y-tEdge->endNode->dia.y));
	}
	printf(" <text text-anchor=\"middle\">\n");
	if (!revert) {
	    printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"%f%\">\n",
		tEdge->startNode->smiNode->name,
		tEdge->endNode->smiNode->name,
		95-offset);
	} else {
	    printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"%f%\">\n",
		tEdge->startNode->smiNode->name,
		tEdge->endNode->smiNode->name,
		5+offset);
	}
    }
    switch (tEdge->cardinality) {
    case GRAPH_CARD_ONE_TO_ONE:
    case GRAPH_CARD_ZERO_TO_ONE:
	printf("       1\n");
	break;
    case GRAPH_CARD_ONE_TO_MANY:
    case GRAPH_CARD_ZERO_TO_MANY:
	printf("       *\n");
	break;
    case GRAPH_CARD_ONE_TO_ZERO_OR_ONE:
	printf("       0..1\n");
	break;
    case GRAPH_CARD_UNKNOWN:
    }
    if (!tEdge->cardinality==GRAPH_CARD_UNKNOWN) {
	printf("    </textPath>\n");
	printf(" </text>\n");
    }
}

static void printSVGConnection(GraphEdge *tEdge)
{
    switch (tEdge->connection) {
    case GRAPH_CON_UNKNOWN:
	break;
    case GRAPH_CON_AGGREGATION :	/* never used??? */
	printSVGAssociation(tEdge,1);
	break;
    case GRAPH_CON_DEPENDENCY :
	printSVGDependency(tEdge);
	break;
    case GRAPH_CON_ASSOCIATION :
	printSVGAssociation(tEdge,0);
	break;	    
    }
}

/*
 * Prints the title of the SVG output file (Modulename and smidump version).
 * TODO
 * Print title somewhere into the SVG.
 * Make size of SVG configurable.
 */
static void printSVGHeaderAndTitle(int modc, SmiModule **modv, int nodecount,
				   float xMin, float yMin,
				   float xMax, float yMax)
{
    size_t  length1;
    char   *note1;
    int	    i;
    const char *s11 = "Conceptual model of ";
    const char *s12 = "- generated by smidump " SMI_VERSION_STRING;

    /*
     * Calculate the length of the string...
     */

    length1 = strlen(s11) + strlen(s12) + 1;
    for (i = 0; i < modc; i++) {
	length1 += strlen(modv[i]->name) + 1;
    }

    /*
     * ... before allocating a buffer and putting the string together.
     */

    note1 = xmalloc(length1);
    strcpy(note1, s11);
    for (i = 0; i < modc; i++) {
	strcat(note1, modv[i]->name);
	strcat(note1, " ");
    }
    strcat(note1, s12);

    printf("<?xml version=\"1.0\"?>\n");
    printf("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n");
    printf("  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
    printf("<svg preserveAspectRatio=\"xMinYMin meet\"\n");
    printf("     width=\"%i\" height=\"%i\" viewBox=\"%.2f %.2f %.2f %.2f\"\n",
           CANVASWIDTH, CANVASHEIGHT, xMin, yMin, xMax-xMin, yMax-yMin);
    printf("     version=\"1.1\"\n");
    printf("     xmlns=\"http://www.w3.org/2000/svg\"\n");
    printf("     onload=\"init(evt)\" onzoom=\"ZoomControl()\">\n");

    //the ecma-script for handling the "+"- and "-"-buttons
    //and the tooltip
    printf("\n<script type=\"text/ecmascript\">\n<![CDATA[\n");
    printf("//The script for the tooltip was copied from:\n");
    printf("//SVG - Learning By Coding - ");
			    printf("http://www.datenverdrahten.de/svglbc/\n");
    printf("//Author: Dr. Thomas Meinike 11/03 - thomas@handmadecode.de\n");
    printf("var svgdoc,svgroot;\n");
    printf("var scalFac = new Array(%i);\n\n",nodecount);
    printf("function getSVGDoc(load_evt) {\n");
    printf("    svgdoc=load_evt.target.ownerDocument;\n");
    printf("    svgroot=svgdoc.documentElement;\n");
    printf("    texte=svgdoc.getElementById(\"tooltip\")");
				printf(".getElementsByTagName(\"text\");\n");
    printf("}\n\n");
    printf("function ShowTooltipMZ(mousemove_event,txt) {\n");
    printf("    var ttrelem,tttelem,posx,posy,curtrans,ctx,cty,txt;\n");
    printf("    var sollbreite,maxbreite,ges,anz,tmp,txl,neu,i,k,l\n");
    printf("    ttrelem=svgdoc.getElementById(\"ttr\");\n");
    printf("    tttelem=svgdoc.getElementById(\"ttt\");\n");
    printf("    posx=mousemove_event.clientX;\n");
    printf("    posy=mousemove_event.clientY;\n");
    //FIXME: calculate number of lines dynamically.
    printf("    for(i=1;i<=%i;i++)", DYN_TEXT);
				printf("texte.item(i).firstChild.data=\"\";\n");
    printf("    sollbreite=200;\n");
    printf("    tttelem.childNodes.item(0).data=txt;\n");
    printf("    ges=tttelem.getComputedTextLength();\n");
    printf("    tttelem.childNodes.item(0).data=\"\";\n");
    printf("    anz=Math.ceil(ges/sollbreite);\n");
    printf("    tmp=txt.split(\" \");\n");
    printf("    txl=new Array(tmp.length);\n");
    printf("    neu=new Array(anz);\n");
    printf("    for(i=0;i<tmp.length;i++) {\n");
    printf("        tttelem.childNodes.item(0).data=tmp[i];\n");
    printf("        txl[i]=tttelem.getComputedTextLength();\n");
    printf("    }\n");
    printf("    k=0;\n");
    printf("    maxbreite=0;\n");
    printf("    for(i=0;i<anz;i++) {\n");
    printf("        l=0,neu[i]=\"\";\n");
    printf("        while(l+txl[k]<1.1*sollbreite && k<tmp.length) {\n");
    printf("            l+=txl[k];\n");
    printf("            neu[i]+=tmp[k]+\" \";\n");
    printf("            k++;\n");
    printf("            if(maxbreite<l)maxbreite=l;\n");
    printf("        }\n");
    printf("    }\n");
    printf("    curtrans=svgroot.currentTranslate;\n");
    printf("    ctx=curtrans.x;\n");
    printf("    cty=curtrans.y;\n");
    printf("    ttrelem.setAttribute(\"x\",posx-ctx+10);\n");
    printf("    ttrelem.setAttribute(\"y\",posy-cty-20+10);\n");
    printf("    if(maxbreite>sollbreite) {\n");
    printf("        ttrelem.setAttribute(\"width\",");
			    printf("maxbreite+2*(maxbreite-sollbreite)+3);\n");
    printf("    } else {\n");
    printf("        ttrelem.setAttribute(\"width\",");
			    printf("maxbreite+2*(sollbreite-maxbreite)+3);\n");
    printf("    }\n");
    printf("    ttrelem.setAttribute(\"height\",anz*15+3);\n");
    printf("    ttrelem.setAttribute(\"style\",");
		printf("\"fill: #FFC; stroke: #000; stroke-width: 0.5px\");\n");
    printf("    for(i=1;i<=anz;i++) {\n");
    printf("        texte.item(i).firstChild.data=neu[i-1];\n");
    printf("        texte.item(i).setAttribute(\"x\",posx-ctx+15);\n");
    printf("        texte.item(i).setAttribute(\"y\",");
				    printf("parseInt(i-1)*15+posy-cty+3);\n");
    printf("        texte.item(i).setAttribute(\"style\",");
				printf("\"fill: #00C; font-size: 11px\");\n");
    printf("    }\n");
    printf("    svgdoc.getElementById(\"tooltip\").style");
			printf(".setProperty(\"visibility\",\"visible\");\n");
    printf("}\n\n");
    printf("function HideTooltip() {\n");
    printf("    svgdoc.getElementById(\"tooltip\").style");
			printf(".setProperty(\"visibility\",\"hidden\");\n");
    printf("}\n\n");
    printf("function ZoomControl() {\n");
    printf("    var curzoom;\n");
    printf("    curzoom=svgroot.currentScale;\n");
    printf("    svgdoc.getElementById(\"tooltip\")");
	printf(".setAttribute(\"transform\",\"scale(\"+1/curzoom+\")\");\n");
    printf("}\n\n");
    printf("function init(evt) {\n");
    printf("    for (i=0; i<%i; i++) {\n",nodecount);
    printf("        scalFac[i] = %.1f;\n", STARTSCALE);
    printf("    }\n");
    printf("    getSVGDoc(evt);\n");
    printf("}\n\n");
    printf("function colorText(object, color) {\n");
    printf("    var obj = svgDocument.getElementById(object);\n");
    printf("    obj.setAttribute(\"style\",\"fill: \"+color);");
    printf("}\n\n");
    printf("function enlarge(name, number) {\n");
    printf("    var obj = svgDocument.getElementById(name);\n");
    printf("    scalFac[number] = scalFac[number] * 1.1;\n");
    printf("    obj.setAttribute(\"transform\",");
    printf("\"scale(\"+scalFac[number]+\")\");\n");
    printf("}\n\n");
    printf("function scaledown(name, number) {\n");
    printf("    var obj = svgDocument.getElementById(name);\n");
    printf("    scalFac[number] = scalFac[number] / 1.1;\n");
    printf("    obj.setAttribute(\"transform\",");
    printf("\"scale(\"+scalFac[number]+\")\");\n");
    printf("}\n");
    printf("// ]]>\n</script>\n\n");

    printf(" <title>%s</title>\n", note1);

    //definition for the arrowheads
    printf(" <defs>\n");
    printf("     <marker id=\"arrowhead\" markerWidth=\"12\"");
    printf(" markerHeight=\"8\" refX=\"12\" refY=\"4\" orient=\"auto\">\n");
    printf("     <path d=\"M 0 0 12 4 0 8\" fill=\"none\" stroke=\"black\"/>");
    printf("     </marker>");
    printf(" </defs>\n");

    xfree(note1);
}

/*
 * Calculates the size of a given node for the UML representation.
 *
 * FIXME this algorithm may work good for a monospace-font. we have some
 * problems with the proportional-font. :-(
 */
static GraphNode *diaCalcSize(GraphNode *node)
{
    GraphEdge  *tEdge;
    SmiNode    *tNode,*ppNode;
    SmiElement *smiElement;
    SmiModule  *module;

    if (node->smiNode->nodekind == SMI_NODEKIND_SCALAR) return node;

    node->use = 1;
    /*
    node->dia.x = (float) (rand()/RAND_MAX*CANVASWIDTH);
    node->dia.y = (float) (rand()/RAND_MAX*CANVASHEIGHT);
    */
    node->dia.x = (float) rand();
    node->dia.y = (float) rand();
    node->dia.x /= (float) RAND_MAX;
    node->dia.y /= (float) RAND_MAX;
    //node->dia.x *= (float) CANVASWIDTH;
    //node->dia.y *= (float) CANVASHEIGHT;
    //fprintf(stderr, "(%.2f,%.2f)\n", node->dia.x, node->dia.y);
    node->dia.w = strlen(node->smiNode->name) * HEADFONTSIZETABLE
	+ HEADSPACESIZETABLE;
    node->dia.h = TABLEHEIGHT + TABLEBOTTOMHEIGHT;

    //C
    for (smiElement = smiGetFirstElement(
	smiGetFirstChildNode(node->smiNode));
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	
	tNode = smiGetElementNode(smiElement);
	
	node->dia.w = max(node->dia.w, (strlen(tNode->name) +
					strlen(algGetTypeName(tNode)) +
					strlen(INDEXPROPERTY) + 3)
		      * ATTRFONTSIZE
		      + ATTRSPACESIZE);
	node->dia.h += TABLEELEMHEIGHT;
    }
    
    //A
    for (tEdge = graphGetFirstEdgeByNode(graph,node);
	 tEdge;
	 tEdge = graphGetNextEdgeByNode(graph, tEdge, node)) {
	if (tEdge->startNode == node &&
	    tEdge->endNode->smiNode->nodekind == SMI_NODEKIND_SCALAR) {

	    tNode = tEdge->endNode->smiNode;
	    
	    node->dia.w = max(node->dia.w, (strlen(tNode->name) +
				    strlen(algGetTypeName(tNode)) + 2)
			  * ATTRFONTSIZE
			  + ATTRSPACESIZE);		
	    node->dia.h += TABLEELEMHEIGHT;
	}
    }

    //B
    for (tEdge = graphGetFirstEdgeByNode(graph,node);
	 tEdge;
	 tEdge = graphGetNextEdgeByNode(graph, tEdge, node)) {
	if (tEdge->indexkind == SMI_INDEX_AUGMENT) {
	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(tEdge->startNode->smiNode));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
		if (!cmpSmiNodes(node->smiNode, tEdge->startNode->smiNode)) {

		    tNode = smiGetElementNode(smiElement);

		    node->dia.w = max(node->dia.w, (strlen(tNode->name) +
					    strlen(algGetTypeName(tNode)) +
					    strlen(INDEXPROPERTY) + 3)
						    * ATTRFONTSIZE
						    + ATTRSPACESIZE);
		    node->dia.h += TABLEELEMHEIGHT;
		}
	    }
	}
    }

    //D
    if (PRINT_DETAILED_ATTR && node->smiNode->nodekind == SMI_NODEKIND_TABLE) {
	module  = smiGetNodeModule(node->smiNode);

	for (tNode = smiGetFirstNode(module, SMI_NODEKIND_COLUMN);
	     tNode;
	     tNode = smiGetNextNode(tNode, SMI_NODEKIND_COLUMN)) {
	    ppNode = smiGetParentNode(tNode);
	    ppNode = smiGetParentNode(ppNode);

	    if (cmpSmiNodes(node->smiNode, ppNode)) {
		int len;
		char *typeName;

		typeName = algGetTypeName(tNode);
		len = strlen(tNode->name) + (typeName ? strlen(typeName)+2 : 1);
		node->dia.w = max(node->dia.w, len
		    * ATTRFONTSIZE
		    + ATTRSPACESIZE);
		node->dia.h += TABLEELEMHEIGHT;
	    }
	}
    }

    return node;
}

/*
 * Calculates the size of a group-node for the UML representation.
 */
static GraphNode *calcGroupSize(int group)
{
    GraphNode *calcNode, *node;
    SmiNode   *tNode;

    for (calcNode = graph->nodes; calcNode; calcNode = calcNode->nextPtr) {
	if (calcNode->group == group) break;
    }

    if (!calcNode) return NULL;

    calcNode->use = 1;
    /*
    calcNode->dia.x = (float) (rand()/RAND_MAX*CANVASWIDTH);
    calcNode->dia.y = (float) (rand()/RAND_MAX*CANVASHEIGHT);
    */
    /*
    calcNode->dia.x = (float) rand();
    calcNode->dia.y = (float) rand();
    calcNode->dia.x /= (float) RAND_MAX;
    calcNode->dia.y /= (float) RAND_MAX;
    */
    //calcNode->dia.x *= (float) CANVASWIDTH;
    //calcNode->dia.y *= (float) CANVASHEIGHT;
    //fprintf(stderr, "(%.2f,%.2f)\n", calcNode->dia.x, calcNode->dia.y);
    calcNode->dia.w = strlen(calcNode->smiNode->name) * HEADFONTSIZETABLE
	+ HEADSPACESIZETABLE;
    calcNode->dia.h = TABLEHEIGHT + TABLEBOTTOMHEIGHT;

    for (node = graph->nodes; node; node = node->nextPtr) {
	if (node->group == group) {
	    tNode = node->smiNode;
	    calcNode->dia.w = max(calcNode->dia.w, (strlen(tNode->name) +
				    strlen(algGetTypeName(tNode)) + 2)
			    * ATTRFONTSIZE
			    + ATTRSPACESIZE);
	    calcNode->dia.h += TABLEELEMHEIGHT;
	}
    }

    return calcNode;
}


/* ------------------------------------------------------------------------- */


static void printModuleIdentity(int modc, SmiModule **modv, float *x, float *y)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <text x=\"%.2f\" y=\"%.2f\">MODULE-IDENTITY</text>\n", *x, *y);
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name and description of the module.
	    *x += TABLEELEMHEIGHT;
	    if (modv[i]->description) {
		tooltip = (char *)xmalloc(2*strlen(modv[i]->description));
		parseTooltip(modv[i]->description, tooltip);
		printf(" <text x=\"%.2f\" y=\"%.2f\"", *x, *y);
		printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
		printf(" onmouseout=\"HideTooltip(evt)\">%s</text>\n",
								smiNode->name);
		xfree(tooltip);
	    } else {
		printf(" <text x=\"%.2f\" y=\"%.2f\">%s</text>\n",
							*x, *y, smiNode->name);
	    }
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //revision history of the module.
	    *x += 2*TABLEELEMHEIGHT;
	    smiRevision = smiGetFirstRevision(modv[i]);
	    if (!smiRevision) {
		printf(" <text x=\"%.2f\" y=\"%.2f\">197001010000Z</text>\n",
									*x, *y);
		*y += TABLEELEMHEIGHT;
	    } else {
		for(; smiRevision;
				smiRevision = smiGetNextRevision(smiRevision)) {
		    printf(" <text x=\"%.2f\" y=\"%.2f\"", *x, *y);
		    if (smiRevision->description && strcmp(
		smiRevision->description,
		"[Revision added by libsmi due to a LAST-UPDATED clause.]")) {
			tooltip = (char *)xmalloc(2*
					strlen(smiRevision->description));
			parseTooltip(smiRevision->description, tooltip);
			printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"",
								tooltip);
			printf(" onmouseout=\"HideTooltip(evt)\"");
			xfree(tooltip);
		    }
		    printf(">%s</text>\n", getTimeString(smiRevision->date));
		    *y += TABLEELEMHEIGHT;
		}
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printNotificationType(int modc, SmiModule **modv,
				  float *x, float *y)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <text x=\"%.2f\" y=\"%.2f\">NOTIFICATION-TYPE</text>\n", *x, *y);
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <text x=\"%.2f\" y=\"%.2f\">%s</text>\n",
							*x, *y, smiNode->name);
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the notification
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_NOTIFICATION);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {
		printf(" <text id=\"%s\" x=\"%.2f\" y=\"%.2f\"><tspan",
							smiNode->name, *x, *y);

		smiElement = smiGetFirstElement(smiNode);
		if (smiElement || smiNode->description) {
		    printf(" onmousemove=\"");
		}
		if (smiNode->description) {
		    tooltip = (char *)xmalloc(2*strlen(smiNode->description));
		    parseTooltip(smiNode->description, tooltip);
		    printf("ShowTooltipMZ(evt,'%s')", tooltip);
		    xfree(tooltip);
		}
		if (smiElement && smiNode->description) {
		    printf(";");
		}
		for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			printf(";");
		    }
		    printf("colorText('%s','red')",
					smiGetElementNode(smiElement)->name);
		}
		if (j || smiNode->description) {
		    printf("\"");
		}

		smiElement = smiGetFirstElement(smiNode);
		if (smiElement || smiNode->description) {
		    printf(" onmouseout=\"");
		}
		if (smiNode->description) {
		    printf("HideTooltip(evt)");
		}
		if (smiElement && smiNode->description) {
		    printf(";");
		}
		for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			printf(";");
		    }
		    printf("colorText('%s','black')",
					smiGetElementNode(smiElement)->name);
		}
		if (j || smiNode->description) {
		    printf("\"");
		}

		printf(">%s</tspan>", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		*y += TABLEELEMHEIGHT;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printObjectGroup(int modc, SmiModule **modv, float *x, float *y)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <text x=\"%.2f\" y=\"%.2f\">OBJECT-GROUP</text>\n", *x, *y);
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <text x=\"%.2f\" y=\"%.2f\">%s</text>\n",
							*x, *y, smiNode->name);
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the group
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_GROUP);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
		if (!isObjectGroup(smiNode))
		    continue;
		printf(" <text id=\"%s\" x=\"%.2f\" y=\"%.2f\"><tspan",
							smiNode->name, *x, *y);

		smiElement = smiGetFirstElement(smiNode);
		if (smiElement || smiNode->description) {
		    printf(" onmousemove=\"");
		}
		if (smiNode->description) {
		    tooltip = (char *)xmalloc(2*strlen(smiNode->description));
		    parseTooltip(smiNode->description, tooltip);
		    printf("ShowTooltipMZ(evt,'%s')", tooltip);
		    xfree(tooltip);
		}
		if (smiElement && smiNode->description) {
		    printf(";");
		}
		for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			printf(";");
		    }
		    printf("colorText('%s','red')",
					smiGetElementNode(smiElement)->name);
		}
		if (j || smiNode->description) {
		    printf("\"");
		}

		smiElement = smiGetFirstElement(smiNode);
		if (smiElement || smiNode->description) {
		    printf(" onmouseout=\"");
		}
		if (smiNode->description) {
		    printf("HideTooltip(evt)");
		}
		if (smiElement && smiNode->description) {
		    printf(";");
		}
		for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			printf(";");
		    }
		    printf("colorText('%s','black')",
					smiGetElementNode(smiElement)->name);
		}
		if (j || smiNode->description) {
		    printf("\"");
		}

		printf(">%s</tspan>", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		*y += TABLEELEMHEIGHT;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printNotificationGroup(int modc, SmiModule **modv,
				   float *x, float *y)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <text x=\"%.2f\" y=\"%.2f\">NOTIFICATION-GROUP</text>\n", *x, *y);
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <text x=\"%.2f\" y=\"%.2f\">%s</text>\n",
							*x, *y, smiNode->name);
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the group
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_GROUP);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
		if (!isNotificationGroup(smiNode))
		    continue;
		printf(" <text id=\"%s\" x=\"%.2f\" y=\"%.2f\"><tspan",
							smiNode->name, *x, *y);

		smiElement = smiGetFirstElement(smiNode);
		if (smiElement || smiNode->description) {
		    printf(" onmousemove=\"");
		}
		if (smiNode->description) {
		    tooltip = (char *)xmalloc(2*strlen(smiNode->description));
		    parseTooltip(smiNode->description, tooltip);
		    printf("ShowTooltipMZ(evt,'%s')", tooltip);
		    xfree(tooltip);
		}
		if (smiElement && smiNode->description) {
		    printf(";");
		}
		for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			printf(";");
		    }
		    printf("colorText('%s','red')",
					smiGetElementNode(smiElement)->name);
		}
		if (j || smiNode->description) {
		    printf("\"");
		}

		smiElement = smiGetFirstElement(smiNode);
		if (smiElement || smiNode->description) {
		    printf(" onmouseout=\"");
		}
		if (smiNode->description) {
		    printf("HideTooltip(evt)");
		}
		if (smiElement && smiNode->description) {
		    printf(";");
		}
		for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
		    if (j) {
			printf(";");
		    }
		    printf("colorText('%s','black')",
					smiGetElementNode(smiElement)->name);
		}
		if (j || smiNode->description) {
		    printf("\"");
		}

		printf(">%s</tspan>", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		*y += TABLEELEMHEIGHT;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printModuleCompliance(int modc, SmiModule **modv,
				  float *x, float *y)
{
    int           i, j;
    char          *tooltip;
    char          *done = NULL;
    char          s[100];
    char          *module;
    SmiNode       *smiNode, *smiNode2;
    SmiModule     *smiModule2;
    SmiElement    *smiElement;
    SmiRevision   *smiRevision;
    SmiOption     *smiOption;
    SmiRefinement *smiRefinement;

    printf(" <text x=\"%.2f\" y=\"%.2f\">MODULE-COMPLIANCE</text>\n", *x, *y);
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <text x=\"%.2f\" y=\"%.2f\">%s</text>\n",
							*x, *y, smiNode->name);
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the compliance
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_COMPLIANCE);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
		printf(" <text x=\"%.2f\" y=\"%.2f\"><tspan", *x, *y);

		if (smiNode->description) {
		    tooltip = (char *)xmalloc(2*strlen(smiNode->description));
		    parseTooltip(smiNode->description, tooltip);
		    printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')", tooltip);
		    xfree(tooltip);
		    printf("\" onmouseout=\"HideTooltip(evt)\"");
		}
		printf(">%s</tspan>", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		*y += TABLEELEMHEIGHT;

		//modules for the compliance
		*x += TABLEELEMHEIGHT;
		done = xstrdup("+");
		for (module = modv[i]->name; module; ) {
		    printf(" <text x=\"%.2f\" y=\"%.2f\">", *x, *y);
		    printf("%s</text>\n", module);
		    *y += TABLEELEMHEIGHT;

		    //mandatory groups
		    *x += TABLEELEMHEIGHT;
		    printf(" <text x=\"%.2f\" y=\"%.2f\"", *x, *y);
		    smiElement = smiGetFirstElement(smiNode);
		    if (smiElement) {
			printf(" onmousemove=\"");
		    }
		    for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
			if (!strcmp(smiGetNodeModule(smiGetElementNode(
						smiElement))->name, module)) {
			    if (j) {
				printf(";");
			    }
			    printf("colorText('%s','red')",
					smiGetElementNode(smiElement)->name);
			}
		    }
		    if (j) {
			printf("\"");
		    }
		    smiElement = smiGetFirstElement(smiNode);
		    if (smiElement) {
			printf(" onmouseout=\"");
		    }
		    for (j = 0; smiElement;
			j++, smiElement = smiGetNextElement(smiElement)) {
			if (!strcmp(smiGetNodeModule(smiGetElementNode(
						smiElement))->name, module)) {
			    if (j) {
				printf(";");
			    }
			    printf("colorText('%s','black')",
					smiGetElementNode(smiElement)->name);
			}
		    }
		    if (j) {
			printf("\"");
		    }
		    printf(">MANDATORY-GROUPS</text>");
		    *y += TABLEELEMHEIGHT;
		    *x -= TABLEELEMHEIGHT;

		    //groups
		    *x += TABLEELEMHEIGHT;
		    for (smiOption = smiGetFirstOption(smiNode); smiOption;
				    smiOption = smiGetNextOption(smiOption)) {
			smiNode2 = smiGetOptionNode(smiOption);
			smiModule2 = smiGetNodeModule(smiNode2);
			if (!strcmp(smiModule2->name, module)) {
			    printf(" <text x=\"%.2f\" y=\"%.2f\">", *x, *y);
			    printf("GROUP <tspan onmousemove=\"");
			    if (smiOption->description) {
				tooltip = (char *)xmalloc(2*strlen(
						smiOption->description));
				parseTooltip(smiOption->description, tooltip);
				printf("ShowTooltipMZ(evt,'%s');", tooltip);
				xfree(tooltip);
			    }
			    printf("colorText('%s','red')", smiNode2->name);
			    printf("\" onmouseout=\"");
			    if (smiOption->description) {
				printf("HideTooltip(evt);");
			    }
			    printf("colorText('%s','black')", smiNode2->name);
			    printf("\">%s</tspan></text>\n", smiNode2->name);
			    *y += TABLEELEMHEIGHT;
			}
		    }
		    *x -= TABLEELEMHEIGHT;

		    //objects
		    *x += TABLEELEMHEIGHT;
		    for (smiRefinement = smiGetFirstRefinement(smiNode);
			smiRefinement;
			smiRefinement = smiGetNextRefinement(smiRefinement)) {
			smiNode2 = smiGetRefinementNode(smiRefinement);
			smiModule2 = smiGetNodeModule(smiNode2);
			if (!strcmp(smiModule2->name, module)) {
			    printf(" <text x=\"%.2f\" y=\"%.2f\">", *x, *y);
			    printf("OBJECT <tspan onmousemove=\"");
			    if (smiRefinement->description) {
				tooltip = (char *)xmalloc(2*strlen(
						smiRefinement->description));
				parseTooltip(smiRefinement->description,
								tooltip);
				printf("ShowTooltipMZ(evt,'%s');", tooltip);
				xfree(tooltip);
			    }
			    printf("colorText('%s','red')", smiNode2->name);
			    printf("\" onmouseout=\"");
			    if (smiRefinement->description) {
				printf("HideTooltip(evt);");
			    }
			    printf("colorText('%s','black')", smiNode2->name);
			    printf("\">%s</tspan></text>\n", smiNode2->name);
			    *y += TABLEELEMHEIGHT;
			}
		    }
		    *x -= TABLEELEMHEIGHT;

		    //find next module
		    done = xrealloc(done,
				strlen(done)+strlen(module)+2*sizeof(char));
		    strcat(done, module);
		    strcat(done, "+");
		    module = NULL;
		    for (smiElement = smiGetFirstElement(smiNode);
			 smiElement;
			 smiElement = smiGetNextElement(smiElement)) {
			sprintf(s, "+%s+", smiGetNodeModule(smiGetElementNode(
							smiElement))->name);
			if ((!strstr(done, s))) {
			    module = smiGetNodeModule(smiGetElementNode(
							smiElement))->name;
			    break;
			}
		    }
		}
		*x -= TABLEELEMHEIGHT;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printModuleInformation(int modc, SmiModule **modv, float x, float y)
{
    printf(" <g transform=\"translate(%.2f,%.2f) scale(%.2f)\">\n",
							x, y, STARTSCALE);
    //now use x and y as relative coordinates.
    x = 0;
    y = 10;

    printModuleIdentity(modc, modv, &x, &y);
    printNotificationType(modc, modv, &x, &y);
    printObjectGroup(modc, modv, &x, &y);
    printNotificationGroup(modc, modv, &x, &y);
    printModuleCompliance(modc, modv, &x, &y);

    printf(" </g>\n");
}


/* ------------------------------------------------------------------------- */


static float fr(float d, float k)
{
    return (float) (k*k/d);
}

static float fa(float d, float k)
{
    return (float) (d*d/k);
}

static int overlap(GraphNode *vNode, GraphNode *uNode)
{
    if (vNode->dia.x+vNode->dia.w/2>=uNode->dia.x-uNode->dia.w/2 &&
	vNode->dia.x-vNode->dia.w/2<=uNode->dia.x+uNode->dia.w/2 &&
	vNode->dia.y+vNode->dia.h/2>=uNode->dia.y-uNode->dia.h/2 &&
	vNode->dia.y-vNode->dia.h/2<=uNode->dia.y+uNode->dia.h/2) {
	return 1;
    } else {
	return 0;
    }
}

/*
 * test if node and edge intersect
 */
static float intersect(GraphNode *node, GraphEdge *edge)
{
    float a, b, intersect = 0;

    //handle case in which edge is parallel to y-axis
    if (edge->endNode->dia.x == edge->startNode->dia.x) {
	if ((node->dia.x-node->dia.w/2*STARTSCALE < edge->endNode->dia.x &&
	    node->dia.x+node->dia.w/2*STARTSCALE < edge->endNode->dia.x) ||
	    (node->dia.x-node->dia.w/2*STARTSCALE > edge->endNode->dia.x &&
	    node->dia.x+node->dia.w/2*STARTSCALE > edge->endNode->dia.x))
	    return intersect;
	intersect = node->dia.x - edge->startNode->dia.x;
    } else {
	//calculate a and b for y=ax+b
	a = (edge->endNode->dia.y - edge->startNode->dia.y) /
	    (edge->endNode->dia.x - edge->startNode->dia.x);
	b = edge->startNode->dia.y - (a * edge->startNode->dia.x);
	//test if entire node is above or under edge
	if ((node->dia.y-node->dia.h/2*STARTSCALE -
			(a * node->dia.x-node->dia.w/2*STARTSCALE) > b &&
	    node->dia.y+node->dia.h/2*STARTSCALE -
			(a * node->dia.x-node->dia.w/2*STARTSCALE) > b &&
	    node->dia.y-node->dia.h/2*STARTSCALE -
			(a * node->dia.x+node->dia.w/2*STARTSCALE) > b &&
	    node->dia.y+node->dia.h/2*STARTSCALE -
			(a * node->dia.x+node->dia.w/2*STARTSCALE) > b) ||
	    (node->dia.y-node->dia.h/2*STARTSCALE -
			(a * node->dia.x-node->dia.w/2*STARTSCALE) < b &&
	    node->dia.y+node->dia.h/2*STARTSCALE -
			(a * node->dia.x-node->dia.w/2*STARTSCALE) < b &&
	    node->dia.y-node->dia.h/2*STARTSCALE -
			(a * node->dia.x+node->dia.w/2*STARTSCALE) < b &&
	    node->dia.y+node->dia.h/2*STARTSCALE -
			(a * node->dia.x+node->dia.w/2*STARTSCALE) < b))
	    return intersect;
	intersect = (a * node->dia.x - node->dia.y + b) /
		    (float)(sqrt(a*a+1));
    }
    //test if node is over upper end of edge or under lower end of edge
    if (node->dia.y+node->dia.h/2*STARTSCALE <
		min(edge->startNode->dia.y,edge->endNode->dia.y) ||
	node->dia.y-node->dia.h/2*STARTSCALE >
		max(edge->startNode->dia.y,edge->endNode->dia.y)) {
	intersect = 0;
	return intersect;
    }
    //node and edge intersect
    return intersect;
}

/*
 * Implements the springembedder. Look at LNCS 2025, pp. 71-86.
 * and: http://citeseer.ist.psu.edu/fruchterman91graph.html
 * Input: Graph with known width and height of nodes.
 * Output: Coordinates (x,y) for the nodes.
 * Only the nodes and edges with use==1 are considered.
 *
 * TODO erst Springembedder, dann Flaeche rausfinden
 * Kraefte in x- bzw. y-Richtung verschieden gewichten: aspect-ratio?
 * Zusammenhangskomponenten einzeln betrachten
 *
 * FIXME * nodecount counts all nodes. we need something like clusternodecount!
 */
static void layoutCluster(int nodecount, GraphCluster *cluster,
			int nodeoverlap, int edgeoverlap, int limit_frame)
{
    int i;
    float area, aspectratio, k, c = 0.8, xDelta, yDelta, absDelta, absDisp, t;
    float x2, y2 = 1, dist;
    GraphNode *vNode, *uNode;
    GraphEdge *eEdge;

    //area = CANVASHEIGHT * CANVASWIDTH;
    //k = (float) (c*sqrt(area/nodecount));
    //t = CANVASWIDTH/10;
    k = 200;
    t = 100;
    aspectratio = (float)CANVASWIDTH/(float)CANVASHEIGHT;
    aspectratio = 1;

    for (i=0; i<ITERATIONS; i++) {
	//calculate repulsive forces
	for (vNode = cluster->firstClusterNode; vNode;
					vNode = vNode->nextClusterNode) {
	    vNode->dia.xDisp = 0;
	    vNode->dia.yDisp = 0;
	    for (uNode = cluster->firstClusterNode; uNode;
					uNode = uNode->nextClusterNode) {
		if (vNode==uNode)
		    continue;
		xDelta = vNode->dia.x - uNode->dia.x;
		yDelta = vNode->dia.y - uNode->dia.y;
		absDelta = (float) (sqrt(xDelta*xDelta + yDelta*yDelta));
		vNode->dia.xDisp += aspectratio*
					(xDelta/absDelta)*fr(absDelta, k);
		vNode->dia.yDisp += (yDelta/absDelta)*fr(absDelta, k);
		//add another repulsive force if the nodes overlap
		if (nodeoverlap && overlap(vNode, uNode)) {
		    vNode->dia.xDisp += aspectratio*
					4*(xDelta/absDelta)*fr(1/absDelta, k);
		    vNode->dia.yDisp += 4*(yDelta/absDelta)*fr(1/absDelta, k);
		}
	    }
	}
	for (eEdge = graph->edges; eEdge; eEdge = eEdge->nextPtr) {
	    if (!eEdge->use || eEdge->startNode->cluster != cluster)
		continue;
	    //add another repulsive force if edge and any node overlap
	    if (edgeoverlap) {
		for (vNode = cluster->firstClusterNode; vNode;
					vNode = vNode->nextClusterNode) {
		    if (eEdge->startNode == vNode ||
			eEdge->endNode == vNode ||
			overlap(eEdge->startNode, vNode) ||
			overlap(eEdge->endNode, vNode))
			continue;
		    if (dist = intersect(vNode, eEdge)) {
			if (eEdge->startNode->dia.x == eEdge->endNode->dia.x) {
			    eEdge->startNode->dia.xDisp -=
				8*(dist/fabsf(dist))*fr(1/dist, k);
			    eEdge->endNode->dia.xDisp -=
				8*(dist/fabsf(dist))*fr(1/dist, k);
			    vNode->dia.xDisp +=
				8*(dist/fabsf(dist))*fr(1/dist, k);
			} else {
			    xDelta = -1*(eEdge->endNode->dia.y
					 -eEdge->startNode->dia.y)
				       /(eEdge->endNode->dia.x
					 -eEdge->startNode->dia.x);
			    yDelta = 1;
			    absDelta = (float) (sqrt(xDelta*xDelta
						     + yDelta*yDelta));
			    eEdge->startNode->dia.xDisp +=
				8*(xDelta/absDelta)*fr(1/dist, k);
			    eEdge->startNode->dia.yDisp +=
				8*(yDelta/absDelta)*fr(1/dist, k);
			    eEdge->endNode->dia.xDisp +=
				8*(xDelta/absDelta)*fr(1/dist, k);
			    eEdge->endNode->dia.yDisp +=
				8*(yDelta/absDelta)*fr(1/dist, k);
			    vNode->dia.xDisp -=
				8*(xDelta/absDelta)*fr(1/dist, k);
			    vNode->dia.yDisp -=
				8*(yDelta/absDelta)*fr(1/dist, k);
			}
		    }
		}
	    }
	    //calculate attractive forces
	    xDelta = eEdge->startNode->dia.x - eEdge->endNode->dia.x;
	    yDelta = eEdge->startNode->dia.y - eEdge->endNode->dia.y;
	    absDelta = (float) (sqrt(xDelta*xDelta + yDelta*yDelta));
	    //FIXME aspectratio? divide xDisps by ar?
	    eEdge->startNode->dia.xDisp -= (xDelta/absDelta)*fa(absDelta, k);
	    eEdge->startNode->dia.yDisp -= (yDelta/absDelta)*fa(absDelta, k);
	    eEdge->endNode->dia.xDisp += (xDelta/absDelta)*fa(absDelta, k);
	    eEdge->endNode->dia.yDisp += (yDelta/absDelta)*fa(absDelta, k);
	}
	//limit the maximum displacement to the temperature t
	//and prevent from being displaced outside the frame
	for (vNode = cluster->firstClusterNode; vNode;
					vNode = vNode->nextClusterNode) {
	    absDisp = (float) (sqrt(vNode->dia.xDisp*vNode->dia.xDisp
					+ vNode->dia.yDisp*vNode->dia.yDisp));
	    vNode->dia.x += (vNode->dia.xDisp/absDisp)*min(absDisp, t);
	    vNode->dia.y += (vNode->dia.yDisp/absDisp)*min(absDisp, t);
	    /*
	    if (limit_frame) {
		vNode->dia.x = min(CANVASWIDTH - STARTSCALE*vNode->dia.w/2,
				  max(STARTSCALE*vNode->dia.w/2, vNode->dia.x));
		vNode->dia.y = min(CANVASHEIGHT - STARTSCALE*vNode->dia.h/2,
				  max(STARTSCALE*vNode->dia.h/2, vNode->dia.y));
	    }
	    */
	}
	//reduce the temperature as the layout approaches a better configuration
	t *= 0.9;
    }
}


/* ------------------------------------------------------------------------- */


static void addNodeToCluster(GraphNode *tNode, GraphCluster *tCluster)
{
    GraphEdge *tEdge;

    tNode->cluster = tCluster;
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	if (!tEdge->use)
	    continue;
	if (tEdge->startNode == tNode && tEdge->endNode->cluster == NULL) {
	    tEdge->endNode->nextClusterNode = tNode->nextClusterNode;
	    tNode->nextClusterNode = tEdge->endNode;
	    addNodeToCluster(tEdge->endNode, tCluster);
	}
	if (tEdge->endNode == tNode && tEdge->startNode->cluster == NULL) {
	    tEdge->startNode->nextClusterNode = tNode->nextClusterNode;
	    tNode->nextClusterNode = tEdge->startNode;
	    addNodeToCluster(tEdge->startNode, tCluster);
	}
    }
}

//TODO calculate maximal x- and y-sizes and print them into the header
static void diaPrintXML(int modc, SmiModule **modv)
{
    GraphNode    *tNode, *lastNode;
    GraphEdge    *tEdge;
    GraphCluster *tCluster;
    int          group, nodecount = 0, classNr = 0, singleNodes = 1;
    float        x = 10, xMin = 0, yMin = 0, xMax = 0, yMax = 0, maxHeight = 0;

    //find edges which are supposed to be drawn
    for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	if (tEdge->connection != GRAPH_CON_UNKNOWN
	    && tEdge->startNode->smiNode->nodekind != SMI_NODEKIND_SCALAR
	    && tEdge->endNode->smiNode->nodekind != SMI_NODEKIND_SCALAR
	    && tEdge->startNode != tEdge->endNode) {
	    tEdge->use = 1;
	    tEdge->startNode->degree++;
	    tEdge->endNode->degree++;
	}
    }

    tCluster = graphInsertCluster(graph);

    //prepare nodes which are supposed to be drawn
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	tNode = diaCalcSize(tNode);
	if (tNode->smiNode->nodekind != SMI_NODEKIND_SCALAR) {
	    nodecount++;
	    if (tNode->degree == 0) {
		//single nodes are members of the first cluster.
		if (tCluster->firstClusterNode == NULL) {
		    tCluster->firstClusterNode = tNode;
		} else {
		    lastNode->nextClusterNode = tNode;
		}
		lastNode = tNode;
		tNode->cluster = tCluster;
		tNode->dia.x = x + tNode->dia.w/2*STARTSCALE;
		x += 10 + tNode->dia.w*STARTSCALE;
		tNode->dia.y = 0;
		if (tNode->dia.h*STARTSCALE > maxHeight)
		    maxHeight = tNode->dia.h*STARTSCALE;
	    }
	}
    }
    for (group = 1; group <= algGetNumberOfGroups(); group++) {
	tNode = calcGroupSize(group);
	nodecount++;
	//groupnodes are members of the first cluster.
	if (tCluster->firstClusterNode == NULL) {
	    tCluster->firstClusterNode = tNode;
	} else {
	    lastNode->nextClusterNode = tNode;
	}
	lastNode = tNode;
	tNode->cluster = tCluster;
	tNode->dia.x = x + tNode->dia.w/2*STARTSCALE;
	x += 10 + tNode->dia.w*STARTSCALE;
	tNode->dia.y = 0;
	if (tNode->dia.h*STARTSCALE > maxHeight)
	    maxHeight = tNode->dia.h*STARTSCALE;
    }
    xMax = x;
    if (tCluster->firstClusterNode == NULL)
	singleNodes = 0;

    //cluster the graph
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (!tNode->use)
	    continue;
	if (tNode->cluster == NULL) {
	    tCluster = graphInsertCluster(graph);
	    tCluster->firstClusterNode = tNode;
	    addNodeToCluster(tNode, tCluster);
	}
    }

    //layout cluster (except first) and calculate bounding boxes and offsets
    x = 10;
    for (tCluster = graph->clusters->nextPtr; tCluster;
						tCluster = tCluster->nextPtr) {
	layoutCluster(nodecount, tCluster, 0, 0, 0);
	//FIXME do we need a stage with nodeoverlap and without edgeoverlap?
	layoutCluster(nodecount, tCluster, 1, 0, 0);
	layoutCluster(nodecount, tCluster, 1, 1, 0);
	//layoutCluster(nodecount, tCluster, 1, 1, 1);

	for (tNode = tCluster->firstClusterNode; tNode;
					tNode = tNode->nextClusterNode) {
	    if (tNode->dia.x - STARTSCALE*tNode->dia.w/2 < tCluster->xMin)
		tCluster->xMin = tNode->dia.x - STARTSCALE*tNode->dia.w/2;
	    if (tNode->dia.x + STARTSCALE*tNode->dia.w/2 > tCluster->xMax)
		tCluster->xMax = tNode->dia.x + STARTSCALE*tNode->dia.w/2;
	    if (tNode->dia.y - STARTSCALE*tNode->dia.h/2 < tCluster->yMin)
		tCluster->yMin = tNode->dia.y - STARTSCALE*tNode->dia.h/2;
	    if (tNode->dia.y + STARTSCALE*tNode->dia.h/2 > tCluster->yMax)
		tCluster->yMax = tNode->dia.y + STARTSCALE*tNode->dia.h/2;
	}

	tCluster->xOffset = x - tCluster->xMin;
	x += 10 + tCluster->xMax - tCluster->xMin;
	tCluster->yOffset = -0.5*(tCluster->yMin+tCluster->yMax);
	if (tCluster->yMin + tCluster->yOffset < yMin)
	    yMin = tCluster->yMin + tCluster->yOffset;
	if (tCluster->yMax + tCluster->yOffset > yMax)
	    yMax = tCluster->yMax + tCluster->yOffset;
    }
    yMin -= 10;
    yMax += 10;
    if (x > xMax)
	xMax = x;

    //adjust values for the first cluster (cluster of single nodes)
    graph->clusters->yOffset = yMax + maxHeight/2;
    if (singleNodes)
	yMax += maxHeight + 10;

    //write some debug-information to stderr
    /*
    fprintf(stderr, "xMin\tyMin\txMax\tyMax\txOffset\tyOffset\n");
    for (tCluster = graph->clusters->nextPtr; tCluster;
						tCluster = tCluster->nextPtr) {
	fprintf(stderr, "%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", tCluster->xMin, tCluster->yMin, tCluster->xMax, tCluster->yMax, tCluster->xOffset, tCluster->yOffset);
    }
    fprintf(stderr, "xMin\tyMin\txMax\tyMax\n");
    fprintf(stderr, "%.2f\t%.2f\t%.2f\t%.2f\n", xMin, yMin, xMax, yMax);
    fprintf(stderr, "group\tdegree\tposition\n");
    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
	if (!tNode->use)
	    continue;
	fprintf(stderr, "%i\t%i\t(%.2f,%.2f)\n", tNode->group, tNode->degree, tNode->dia.x, tNode->dia.y);
    }
    */

    //enlarge canvas for ModuleInformation
    xMax += MODULE_INFO_WIDTH;
    //output of svg to stdout begins here
    printSVGHeaderAndTitle(modc, modv, nodecount, xMin, yMin, xMax, yMax);

    //loop through cluster (except first) to print edges and nodes
    for (tCluster = graph->clusters->nextPtr; tCluster;
						tCluster = tCluster->nextPtr) {
	for (tEdge = graph->edges; tEdge; tEdge = tEdge->nextPtr) {
	    if (!tEdge->use || tEdge->startNode->cluster != tCluster)
		continue;
	    printSVGConnection(tEdge);
	}
	for (tNode = tCluster->firstClusterNode; tNode;
					tNode = tNode->nextClusterNode) {
	    printSVGObject(tNode, &classNr);
	}
	//enclose cluster in its bounding box
	printf(" <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"\n",
		tCluster->xMin + tCluster->xOffset,
		tCluster->yMin + tCluster->yOffset,
		tCluster->xMax - tCluster->xMin,
		tCluster->yMax - tCluster->yMin);
	printf("       fill=\"none\" stroke=\"green\" stroke-width=\"1\"/>\n");
    }

    //print single nodes
    for (tNode = graph->clusters->firstClusterNode; tNode;
		    			tNode = tNode->nextClusterNode) {
	if (tNode->group == 0) {
	    printSVGObject(tNode, &classNr);
	} else {
	    printSVGGroup(tNode->group, &classNr);
	}
    }

    //print MODULE-IDENTITY
    printModuleInformation(modc, modv, xMax-MODULE_INFO_WIDTH, yMin+10);

    //output of svg to stdout ends here
    printSVGClose(xMin, yMin, xMax, yMax);
}



/* ------------------------------------------------------------------------- */

static void printModuleNames(int modc, SmiModule **modv)
{
    int i;
    
    printf("Conceptual model of: ");

    for (i = 0; i < modc; i++) {
	printf("%s ", modv[i]->name);
    }

    printf("(generated by smidump " SMI_VERSION_STRING ")\n\n");
}



static void dumpSvg(int modc, SmiModule **modv, int flags, char *output)
{
    int       i;

    if (flags & SMIDUMP_FLAG_UNITE) {
	if (! graph) {
	    graph = xmalloc(sizeof(Graph));
	    graph->nodes = NULL;
	    graph->edges = NULL;
	    graph->clusters = NULL;
	}
	
	for (i = 0; i < modc; i++) {
	    algCreateNodes(modv[i]);
	}
	
	if (XPLAIN) {
	    printModuleNames(modc, modv);
	    printf("\n--- First Phase - loading tables and scalars\n\n");
	    graphShowNodes(graph);
	    printf("\n");
	}
	
	algLinkTables();
	algCheckLinksByName();
	algConnectLonelyNodes();
	algCheckForDependency();
	algCheckForPointerRels();
	
	if (!XPLAIN) {
	    diaPrintXML(modc, modv);
	}
	graphExit(graph);
	graph = NULL;
    } else {
	for (i = 0; i < modc; i++) {
	    if (! graph) {
		graph = xmalloc(sizeof(Graph));
		graph->nodes = NULL;
		graph->edges = NULL;
		graph->clusters = NULL;
	    }
	    
	    algCreateNodes(modv[i]);
	    
	    if (XPLAIN) {
		printModuleNames(1, &(modv[i]));
		printf("\n--- First Phase - loading tables and scalars\n\n");
		graphShowNodes(graph);
		printf("\n");
	    }
	
	    algLinkTables();
	    algCheckLinksByName();
	    algConnectLonelyNodes();
	    algCheckForDependency();
	    algCheckForPointerRels();
	    
	    if (!XPLAIN) {
		diaPrintXML(1, &(modv[i]));
	    }
	
	    graphExit(graph);
	    graph = NULL;
	}
    }

    if (fflush(stdout) || ferror(stdout)) {
	perror("smidump: write error");
	exit(1);
    }
}



void initSvg()
{
    static SmidumpDriverOption opt[] = {
	{ "explain", OPT_FLAG, &XPLAIN, 0,
	  "explain what the algorithm does"},
        { 0, OPT_END, 0, 0 }
    };

    static SmidumpDriver driver = {
	"svg",
	dumpSvg,
	0,
	SMIDUMP_DRIVER_CANT_OUTPUT,
	"svg diagram (work in progress --- don't use)",
	opt,
	NULL
    };

    smidumpRegisterDriver(&driver);
}
