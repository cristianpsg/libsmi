/*
 * rea.h --
 *
 *      Common header-file for dump-cm.c and dump-svg.c.
 *
 * Copyright (c) 2000 A. Mueller, Technical University of Braunschweig.
 * Copyright (c) 2005 K. Sperner, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-cm.c 2078 2005-03-03 15:17:55Z sperner $
 */



#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"
#include "smidump.h"



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
    GRAPH_CON_AGGREGATION   = 1,
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

#define DIA_PRINT_FLAG	0x01

typedef struct DiaNode {
    int   flags;		/* flags for the dia xml output driver */
    float x,y;			/* coordinates (left upper corner) */
    float xDisp,yDisp;		/* displacement vector for springembedder */
    float w,h;			/* width and height of the dia node */
    int relatedScalars;		/* has related scalars -> print them */
    int indexObjects;		/* has index objects -> print them */
} DiaNode;

typedef struct DiaEdge {
    int   flags;		/* flags for the dia xml output driver */
    float startX, startY;	/* Intersection of edge and startNode */
    float endX, endY;		/* Intersection of edge and endNode */
} DiaEdge;

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
    DiaEdge	     dia;
} GraphEdge;

typedef struct Graph {
    GraphNode    *nodes;
    GraphEdge    *edges;
    GraphCluster *clusters;
} Graph;



/*
 * driver output control
 */
extern int CANVASHEIGHT;
extern int CANVASWIDTH;
extern int SHOW_DEPRECATED;
extern int SHOW_DEPR_OBSOLETE;
extern int STATIC_OUTPUT;
extern int XPLAIN;
extern int DEBUG;
extern int SUPPRESS_DEPRECATED;
extern int PRINT_DETAILED_ATTR;
extern int IGNORE_IMPORTED_NODES;


/*
 * global variables
 */
extern Graph *graph;

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

extern int cmpSmiNodes(SmiNode *node1, SmiNode *node2);



/* ------ Graph primitives ------                                            */

extern GraphNode *graphInsertNode(Graph *graph, SmiNode *smiNode);

extern GraphCluster *graphInsertCluster(Graph *graph);

extern void graphExit(Graph *graph);

extern GraphEdge *graphGetFirstEdgeByNode(Graph *graph, GraphNode *node);

extern GraphEdge *graphGetNextEdgeByNode(Graph *graph, 
					 GraphEdge *edge,
					 GraphNode *node);

extern void graphShowNodes(Graph *graph);



/* ------ algorithm primitives ------                                        */

extern int algGetNumberOfGroups();

extern char *algGetTypeDescription(SmiNode *smiNode);

extern char *algGetTypeName(SmiNode *smiNode);

extern int isBaseType(SmiNode *node);

extern int algIsIndexElement(SmiNode *table, SmiNode *node);



/* -------------- main functions ------------------------------------------- */

extern void algLinkTables();

extern void algCheckLinksByName();

extern void algConnectLonelyNodes();

extern void algCheckForDependency();

extern void algCheckForPointerRels();
