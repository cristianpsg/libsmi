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
#include "rea.h"



extern int smiAsprintf(char **strp, const char *format, ...);



/*
 * Definitions used by the svg output driver (node layout).
 */

//FIXME int or float?
static const float HEADFONTSIZETABLE   = (float)7;
static const float HEADSPACESIZETABLE  = (float)4;
static const float ATTRFONTSIZE        = (float)6;
static const float ATTRSPACESIZE       = (float)2;
static const float TABLEHEIGHT         = (float)20; /*headline of the table*/
static const float TABLEELEMHEIGHT     = (float)15; /*height of one attribute*/
static const float TABLEBOTTOMHEIGHT   = (float)5;  /*bottom of the table*/
static const float RELATEDSCALARHEIGHT = (float)20; /*related scalars space*/
static const float INDEXOBJECTHEIGHT   = (float)20; /*index objects space*/

static const int MODULE_INFO_WIDTH     =150;
//The description of RowStatus is quite long... :-/
static const int DYN_TEXT              =550;
static const float STARTSCALE          =(float)0.5;

//used by the springembedder
static const int ITERATIONS            =100;



/* ------ Misc. -----------------                                            */



static char *getTimeString(time_t t)
{
    static char   *s = NULL;
    struct tm	  *tm;

    if (s) xfree(s);
    
    tm = gmtime(&t);
    if (tm->tm_hour == 0 && tm->tm_min == 0) {
	smiAsprintf(&s, "%04d-%02d-%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    } else {
	smiAsprintf(&s, "%04d-%02d-%02d %02d:%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min);
    }
    return s;
}



/* -------------- main functions ------------------------------------------- */



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
	if ((node->status == SMI_STATUS_DEPRECATED
	    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
	    || (node->status == SMI_STATUS_OBSOLETE
	    && !SHOW_DEPR_OBSOLETE))
	    continue;
	graphInsertNode(graph, node);
    }
    for (node = smiGetFirstNode(module, SMI_NODEKIND_SCALAR);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_SCALAR)) {
	if ((node->status == SMI_STATUS_DEPRECATED
	    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
	    || (node->status == SMI_STATUS_OBSOLETE
	    && !SHOW_DEPR_OBSOLETE))
	    continue;
	graphInsertNode(graph, node);
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
    if (!STATIC_OUTPUT) {
	printf(" <g transform=\"translate(%.2f,%.2f) scale(%.2f)\">\n",
							xMin, yMin, scale);
	printf(" <g id=\"tooltip\" style=\"visibility: hidden\">\n");
	printf("   <rect id=\"ttr\" x=\"0\" y=\"0\" rx=\"5\" ry=\"5\"");
	printf(" width=\"100\" height=\"16\"/>\n");
	printf("   <text id=\"ttt\" x=\"0\" y=\"0\"");
	printf(" style=\"visibility: hidden\">");
	printf("dyn. Text</text>\n");
	//FIXME: calculate number of lines dynamically.
	for (i = 0; i < DYN_TEXT; i++) {
	    printf("   <text x=\"-10\" y=\"-10\">dyn. Text</text>\n");
	}
	printf(" </g>\n");
	printf(" </g>\n");
    }
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

    printf("    <text ");
    if (!index) {
	printf("id=\"%s\" ", node->name);
    }
    printf("x=\"%.2f\" y=\"%.2f\">\n",
				*textXOffset + ATTRSPACESIZE, *textYOffset);

    *textYOffset += TABLEELEMHEIGHT;

    //FIXME
    //printf(" textLength=\"100\" lengthAdjust=\"spacingAndGlyphs\"");

    if (node->access == SMI_ACCESS_NOT_ACCESSIBLE) {
	printf("         -");
    } else {
	printf("         +");
    }

    printf("<tspan");
    if (!STATIC_OUTPUT && node->description) {
	tooltip = (char *)xmalloc(2*strlen(node->description));
	parseTooltip(node->description, tooltip);
	printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	printf(" onmouseout=\"HideTooltip(evt)\"");
	xfree(tooltip);
    }
    printf(">%s:</tspan>\n", node->name);

    printf("         <tspan");
    if (typeDescription = algGetTypeDescription(node)) {
	if (!STATIC_OUTPUT) {
	    tooltip = (char *)xmalloc(2*strlen(typeDescription));
	    parseTooltip(typeDescription, tooltip);
	    printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	    printf(" onmouseout=\"HideTooltip(evt)\"");
	    xfree(tooltip);
	}
    }
    printf(">%s</tspan></text>\n", algGetTypeName(node));
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
    if (!STATIC_OUTPUT) {
	//the "+"-button
	printf("    <g onclick=\"enlarge('%s',%i)\"\n",
			smiGetFirstChildNode(node->smiNode)->name, *classNr);
	printf("       transform=\"translate(%.2f,%.2f)\">\n",
				xOrigin + node->dia.w - 26, yOrigin + 3);
	printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\"");
	printf(" rx=\"2\"\n");
	printf("            style=\"stroke: black; fill: none\"/>\n");
	printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
	printf("          +</text>\n");
	printf("    </g>\n");
	//the "-"-button
	printf("    <g onclick=\"scaledown('%s',%i)\"\n",
			smiGetFirstChildNode(node->smiNode)->name, *classNr);
	printf("       transform=\"translate(%.2f,%.2f)\">\n",
				xOrigin + node->dia.w - 13, yOrigin + 3);
	printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\"");
	printf(" rx=\"2\"\n");
	printf("            style=\"stroke: black; fill: none\"/>\n");
	printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
	printf("          -</text>\n");
	printf("    </g>\n");
    }

    (*classNr)++;

    if (node->smiNode->nodekind == SMI_NODEKIND_TABLE) {

	if (node->dia.relatedScalars) {
	    printf("    <text x=\"%.2f\" y=\"%.2f\">\n",
				textXOffset + ATTRSPACESIZE, textYOffset);
	    printf("         related scalar objects:</text>\n");
	    textYOffset += TABLEELEMHEIGHT;

	    //A
	    printSVGRelatedScalars(node, &textYOffset, &textXOffset);

	    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
			    xOrigin,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT,
			    xOrigin + node->dia.w,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT);
	    printf("          fill=\"none\" stroke=\"black\"/>\n");
	    textYOffset += RELATEDSCALARHEIGHT - TABLEELEMHEIGHT;
	}

	if (node->dia.indexObjects) {
	    printf("    <text x=\"%.2f\" y=\"%.2f\">\n",
				textXOffset + ATTRSPACESIZE, textYOffset);
	    printf("         index objects:</text>\n");
	    textYOffset += TABLEELEMHEIGHT;

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

	    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
			    xOrigin,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT,
			    xOrigin + node->dia.w,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT);
	    printf("          fill=\"none\" stroke=\"black\"/>\n");
	    textYOffset += INDEXOBJECTHEIGHT - TABLEELEMHEIGHT;
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
    if (!STATIC_OUTPUT) {
	//the "+"-button
	printf("    <g onclick=\"enlarge('%s',%i)\"\n",
			smiGetParentNode(tNode->smiNode)->name, *classNr);
	printf("       transform=\"translate(%.2f,%.2f)\">\n",
				xOrigin + tNode->dia.w - 26, yOrigin + 3);
	printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\"");
	printf(" rx=\"2\"\n");
	printf("            style=\"stroke: black; fill: none\"/>\n");
	printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
	printf("          +</text>\n");
	printf("    </g>\n");
	//the "-"-button
	printf("    <g onclick=\"scaledown('%s',%i)\"\n",
			smiGetParentNode(tNode->smiNode)->name, *classNr);
	printf("       transform=\"translate(%.2f,%.2f)\">\n",
				xOrigin + tNode->dia.w - 13, yOrigin + 3);
	printf("      <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\"");
	printf(" rx=\"2\"\n");
	printf("            style=\"stroke: black; fill: none\"/>\n");
	printf("      <text x=\"5\" y=\"9\" style=\"text-anchor:middle\">\n");
	printf("          -</text>\n");
	printf("    </g>\n");
    }

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

static void calculateIntersectionPoints(GraphEdge *tEdge)
{
    float alpha, beta;
    const float PI = acos(-1);

    //calculate intersection of edge and startNode
    alpha = atan2(tEdge->startNode->dia.y-tEdge->endNode->dia.y,
		  tEdge->startNode->dia.x-tEdge->endNode->dia.x);
    beta = atan2(tEdge->startNode->dia.h, tEdge->startNode->dia.w);
    if (alpha < 0)
	alpha += PI;
    if (alpha < beta
	|| (alpha > PI-beta && alpha < PI+beta)
	|| alpha > 2*PI-beta) {
	//intersection at left or right border
	if (tEdge->startNode->dia.x < tEdge->endNode->dia.x) {
	    tEdge->dia.startX = tEdge->startNode->dia.x +
			tEdge->startNode->dia.w*STARTSCALE/2;
	} else {
	    tEdge->dia.startX = tEdge->startNode->dia.x -
			tEdge->startNode->dia.w*STARTSCALE/2;
	}
	if (tEdge->startNode->dia.y < tEdge->endNode->dia.y) {
	    tEdge->dia.startY = tEdge->startNode->dia.y +
			fabsf(tEdge->startNode->dia.w*STARTSCALE*tan(alpha)/2);
	} else {
	    tEdge->dia.startY = tEdge->startNode->dia.y -
			fabsf(tEdge->startNode->dia.w*STARTSCALE*tan(alpha)/2);
	}
    } else {
	//intersection at top or bottom border
	if (tEdge->startNode->dia.y < tEdge->endNode->dia.y) {
	    tEdge->dia.startY = tEdge->startNode->dia.y +
			tEdge->startNode->dia.h*STARTSCALE/2;
	} else {
	    tEdge->dia.startY = tEdge->startNode->dia.y -
			tEdge->startNode->dia.h*STARTSCALE/2;
	}
	if (tEdge->startNode->dia.x < tEdge->endNode->dia.x) {
	    tEdge->dia.startX = tEdge->startNode->dia.x +
		fabsf(tEdge->startNode->dia.h*STARTSCALE/(2*tan(alpha)));
	} else {
	    tEdge->dia.startX = tEdge->startNode->dia.x -
		fabsf(tEdge->startNode->dia.h*STARTSCALE/(2*tan(alpha)));
	}
    }

    //calculate intersection of edge and endNode
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
	    tEdge->dia.endX = tEdge->endNode->dia.x +
			tEdge->endNode->dia.w*STARTSCALE/2;
	} else {
	    tEdge->dia.endX = tEdge->endNode->dia.x -
			tEdge->endNode->dia.w*STARTSCALE/2;
	}
	if (tEdge->startNode->dia.y > tEdge->endNode->dia.y) {
	    tEdge->dia.endY = tEdge->endNode->dia.y +
			fabsf(tEdge->endNode->dia.w*STARTSCALE*tan(alpha)/2);
	} else {
	    tEdge->dia.endY = tEdge->endNode->dia.y -
			fabsf(tEdge->endNode->dia.w*STARTSCALE*tan(alpha)/2);
	}
    } else {
	//intersection at top or bottom border
	if (tEdge->startNode->dia.y > tEdge->endNode->dia.y) {
	    tEdge->dia.endY = tEdge->endNode->dia.y +
			tEdge->endNode->dia.h*STARTSCALE/2;
	} else {
	    tEdge->dia.endY = tEdge->endNode->dia.y -
			tEdge->endNode->dia.h*STARTSCALE/2;
	}
	if (tEdge->startNode->dia.x > tEdge->endNode->dia.x) {
	    tEdge->dia.endX = tEdge->endNode->dia.x +
			fabsf(tEdge->endNode->dia.h*STARTSCALE/(2*tan(alpha)));
	} else {
	    tEdge->dia.endX = tEdge->endNode->dia.x -
			fabsf(tEdge->endNode->dia.h*STARTSCALE/(2*tan(alpha)));
	}
    }
}

static void printSVGDependency(GraphEdge *tEdge)
{
    calculateIntersectionPoints(tEdge);

    printf(" <line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\"\n",
	tEdge->dia.startX + tEdge->startNode->cluster->xOffset,
	tEdge->dia.startY + tEdge->startNode->cluster->yOffset,
	tEdge->dia.endX + tEdge->endNode->cluster->xOffset,
	tEdge->dia.endY + tEdge->endNode->cluster->yOffset);
    printf("       stroke-dasharray=\"10, 10\" stroke=\"black\"");
    printf(" marker-end=\"url(#arrowend)\"/>\n");
}

/*
 * Aggregation is a special case of the association.
 * If aggregate = 1 it is an aggregation if 0 it is an association.
 */
static void printSVGAssociation(GraphEdge *tEdge, int aggregate)
{
    int revert = 0;

    if (aggregate > 1) aggregate = 1;
    if (aggregate < 0) aggregate = 0;

    calculateIntersectionPoints(tEdge);

    //expands should have cardinalities 1 *
    if (tEdge->indexkind==SMI_INDEX_EXPAND)
	tEdge->cardinality = GRAPH_CARD_ONE_TO_MANY;

    //print text upside down, if angle is between 180° and 360°
    if (tEdge->startNode->dia.x > tEdge->endNode->dia.x)
	revert = 1;

    //print edge
    printf(" <path id=\"%s%s\"\n",
	tEdge->startNode->smiNode->name,
	tEdge->endNode->smiNode->name);
    if (!revert) {
	printf("       d=\"M %.2f %.2f %.2f %.2f\"\n",
	    tEdge->dia.startX + tEdge->startNode->cluster->xOffset,
	    tEdge->dia.startY + tEdge->startNode->cluster->yOffset,
	    tEdge->dia.endX + tEdge->endNode->cluster->xOffset,
	    tEdge->dia.endY + tEdge->endNode->cluster->yOffset);
    } else {
	printf("       d=\"M %.2f %.2f %.2f %.2f\"\n",
	    tEdge->dia.endX + tEdge->endNode->cluster->xOffset,
	    tEdge->dia.endY + tEdge->endNode->cluster->yOffset,
	    tEdge->dia.startX + tEdge->startNode->cluster->xOffset,
	    tEdge->dia.startY + tEdge->startNode->cluster->yOffset);
    }
    printf("       stroke=\"black\"");
    if (tEdge->indexkind==SMI_INDEX_AUGMENT ||
	tEdge->indexkind==SMI_INDEX_SPARSE ||
	tEdge->indexkind==SMI_INDEX_EXPAND) {
	if (!revert) {
	    printf(" marker-start=\"url(#arrowstart)\"");
	} else {
	    printf(" marker-end=\"url(#arrowend)\"");
	}
    } else if (tEdge->indexkind==SMI_INDEX_REORDER) {
	printf(" marker-start=\"url(#arrowstart)\"");
	printf(" marker-end=\"url(#arrowend)\"");
    }
    printf("/>\n");

    //edges without labels are finished here
    if (tEdge->cardinality==GRAPH_CARD_UNKNOWN)
	return;

    //print labels
    printf(" <text text-anchor=\"middle\">\n");
    printf("    <textPath xlink:href=\"#%s%s\"",
		tEdge->startNode->smiNode->name, tEdge->endNode->smiNode->name);
    if (!revert) {
	printf(" startOffset=\"10%\">\n");
    } else {
	printf(" startOffset=\"90%\">\n");
    }
    switch (tEdge->cardinality) {
    case GRAPH_CARD_ZERO_TO_ONE:
    case GRAPH_CARD_ZERO_TO_MANY:
	printf("       0");
	break;
    case GRAPH_CARD_ONE_TO_ONE:
    case GRAPH_CARD_ONE_TO_MANY:
    case GRAPH_CARD_ONE_TO_ZERO_OR_ONE:
	printf("       1");
	break;
    case GRAPH_CARD_UNKNOWN:
    }
    printf("</textPath>\n");
    printf(" </text>\n");

    if (tEdge->indexkind==SMI_INDEX_AUGMENT ||
	tEdge->indexkind==SMI_INDEX_SPARSE ||
	tEdge->indexkind==SMI_INDEX_REORDER ||
	tEdge->indexkind==SMI_INDEX_EXPAND) {
	printf(" <text text-anchor=\"middle\">\n");
	printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"50%\">\n",
		tEdge->startNode->smiNode->name, tEdge->endNode->smiNode->name);
    }
    switch(tEdge->indexkind) {
    case SMI_INDEX_AUGMENT:
	printf("       augments");
	break;
    case SMI_INDEX_SPARSE:
	printf("       sparsly augments");
	break;
    case SMI_INDEX_REORDER:
	printf("       reorders");
	break;
    case SMI_INDEX_EXPAND:
	printf("       expands");
	break;
    case SMI_INDEX_UNKNOWN:
    case SMI_INDEX_INDEX:
    }
    if (tEdge->indexkind==SMI_INDEX_AUGMENT ||
	tEdge->indexkind==SMI_INDEX_SPARSE ||
	tEdge->indexkind==SMI_INDEX_REORDER ||
	tEdge->indexkind==SMI_INDEX_EXPAND) {
	printf("</textPath>\n");
	printf(" </text>\n");
    }

    printf(" <text text-anchor=\"middle\">\n");
    printf("    <textPath xlink:href=\"#%s%s\"",
		tEdge->startNode->smiNode->name, tEdge->endNode->smiNode->name);
    if (!revert) {
	printf(" startOffset=\"90%\">\n");
    } else {
	printf(" startOffset=\"10%\">\n");
    }
    switch (tEdge->cardinality) {
    case GRAPH_CARD_ONE_TO_ONE:
    case GRAPH_CARD_ZERO_TO_ONE:
	printf("       1");
	break;
    case GRAPH_CARD_ONE_TO_MANY:
    case GRAPH_CARD_ZERO_TO_MANY:
	printf("       *");
	break;
    case GRAPH_CARD_ONE_TO_ZERO_OR_ONE:
	printf("       0..1");
	break;
    case GRAPH_CARD_UNKNOWN:
    }
    printf("</textPath>\n");
    printf(" </text>\n");
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
    printf("     xmlns=\"http://www.w3.org/2000/svg\"");
    if (!STATIC_OUTPUT)
	printf("\n     onload=\"init(evt)\" onzoom=\"ZoomControl()\"");
    printf(">\n\n");

    if (!STATIC_OUTPUT) {
	//the ecma-script for handling the "+"- and "-"-buttons
	//and the tooltip
	//and the folding of the module information
	printf("<script type=\"text/ecmascript\">\n<![CDATA[\n");
	printf("//The script for the tooltip was copied from:\n");
	printf("//SVG - Learning By Coding - ");
			    printf("http://www.datenverdrahten.de/svglbc/\n");
	printf("//Author: Dr. Thomas Meinike 11/03 - thomas@handmadecode.de\n");
	printf("var svgdoc,svgroot;\n");
	printf("var collapsed = new Array(2);");
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
	printf("function collapse(evt) {\n");
	printf("    var i, k, l, m, svgdoc, obj, targetID, targetX, targetY, ");
							printf("attr;\n");
	printf("    obj = evt.getTarget();\n");
	printf("    svgdoc = obj.ownerDocument;\n");
	printf("    //extract coordinates and id of the clicked text\n");
	printf("    attr = obj.parentNode.parentNode.attributes;\n");
	printf("    for (i=0;i<attr.length;i++) {\n");
	printf("        if (attr.item(i).nodeName == \"transform\") {\n");
	printf("            k = attr.item(i).nodeValue.indexOf(\"(\");\n");
	printf("            l = attr.item(i).nodeValue.indexOf(\",\");\n");
	printf("            m = attr.item(i).nodeValue.indexOf(\")\");\n");
	printf("            targetX = attr.item(i).nodeValue");
						printf(".substring(k+1,l);\n");
	printf("            targetY = attr.item(i).nodeValue");
						printf(".substring(l+1,m);\n");
	printf("        }\n");
	printf("        if (attr.item(i).nodeName == \"id\") {\n");
	printf("            targetID = attr.item(i).nodeValue.substr(2);\n");
	printf("        }\n");
	printf("    }\n");
	printf("    //decide if we are collapsing or uncollapsing\n");
	printf("    if (collapsed[0][targetID] == 0) {\n");
	printf("        hideInfos(evt, obj, svgdoc, targetX, targetY, ");
						printf("targetID, attr);\n");
	printf("        collapsed[0][targetID] = 1;\n");
	printf("    } else {\n");
	printf("        showHiddenInfos(evt, obj, svgdoc, targetX, targetY, ");
						printf("targetID, attr);\n");
	printf("        collapsed[0][targetID] = 0;\n");
	printf("    }\n");
	printf("}\n\n");
	printf("function showHiddenInfos(evt, obj, svgdoc, targetX, targetY, ");
						printf("targetID, attr) {\n");
	printf("    var i, k, l, m, nextObj, nextX, nextY, gapY=0, ");
							printf("clickedID;\n");
	printf("    //change clicked text\n");
	printf("    obj.firstChild.data=\"--\";\n");
	printf("    targetID++;\n");
	printf("    nextObj = svgdoc.getElementById(\"MI\"+targetID);\n");
	printf("    if (nextObj == null)\n");
	printf("        return;\n");
	printf("    //show child texts again\n");
	printf("    for (;;) {\n");
	printf("        attr = nextObj.attributes;\n");
	printf("        for (i=0;i<attr.length;i++) {\n");
	printf("            if (attr.item(i).nodeName == \"transform\") {\n");
	printf("                k = attr.item(i).nodeValue.indexOf(\"(\");\n");
	printf("                l = attr.item(i).nodeValue.indexOf(\",\");\n");
	printf("                m = attr.item(i).nodeValue.indexOf(\")\");\n");
	printf("                nextX = attr.item(i).nodeValue");
						printf(".substring(k+1,l);\n");
	printf("                nextY = attr.item(i).nodeValue");
						printf(".substring(l+1,m);\n");
	printf("            }\n");
	printf("        }\n");
	printf("        if (nextX > targetX) {\n");
	printf("            nextObj.style.setProperty(\"visibility\",");
						printf("\"visible\");\n");
	printf("            gapY += 15;\n");
	printf("            if (collapsed[0][targetID] == 1) {\n");
	printf("                targetID = collapsed[1][targetID];\n");
	printf("            } else {\n");
	printf("                targetID++;\n");
	printf("            }\n");
	printf("            nextObj = svgdoc.getElementById(\"MI\"+");
							printf("targetID);\n");
	printf("            if (nextObj == null)\n");
	printf("                break;\n");
	printf("        } else {\n");
	printf("            break;\n");
	printf("        }\n");
	printf("    }\n");
	printf("    if (nextObj == null)\n");
	printf("        return;\n");
	printf("    //move following texts downwards\n");
	printf("    while (nextObj != null) {\n");
	printf("        attr = nextObj.attributes;\n");
	printf("        for (i=0;i<attr.length;i++) {\n");
	printf("            if (attr.item(i).nodeName == \"transform\") {\n");
	printf("                k = attr.item(i).nodeValue.indexOf(\"(\");\n");
	printf("                l = attr.item(i).nodeValue.indexOf(\",\");\n");
	printf("                m = attr.item(i).nodeValue.indexOf(\")\");\n");
	printf("                nextX = attr.item(i).nodeValue");
						printf(".substring(k+1,l);\n");
	printf("                nextY = attr.item(i).nodeValue");
						printf(".substring(l+1,m);\n");
	printf("            }\n");
	printf("        }\n");
	printf("        nextY = nextY - 1 + gapY + 1;\n");
	printf("        nextObj.setAttribute(\"transform\",\"translate(\"+");
					printf("nextX+\",\"+nextY+\")\");\n");
	printf("        targetID++;\n");
	printf("        nextObj = svgdoc.getElementById(\"MI\"+targetID);\n");
	printf("    }\n");
	printf("}\n\n");
	printf("function hideInfos(evt, obj, svgdoc, targetX, targetY, ");
						printf("targetID, attr) {\n");
	printf("    var i, k, l, m, nextObj, nextX, nextY, gapY=0;\n");
	printf("    clickedID = targetID;\n");
	printf("    //change clicked text\n");
	printf("    obj.firstChild.data=\"+\";\n");
	printf("    targetID++;\n");
	printf("    nextObj = svgdoc.getElementById(\"MI\"+targetID);\n");
	printf("    if (nextObj == null)\n");
	printf("        return;\n");
	printf("    //wipe out child texts\n");
	printf("    for (;;) {\n");
	printf("        attr = nextObj.attributes;\n");
	printf("        for (i=0;i<attr.length;i++) {\n");
	printf("            if (attr.item(i).nodeName == \"transform\") {\n");
	printf("                k = attr.item(i).nodeValue.indexOf(\"(\");\n");
	printf("                l = attr.item(i).nodeValue.indexOf(\",\");\n");
	printf("                m = attr.item(i).nodeValue.indexOf(\")\");\n");
	printf("                nextX = attr.item(i).nodeValue");
						printf(".substring(k+1,l);\n");
	printf("                nextY = attr.item(i).nodeValue");
						printf(".substring(l+1,m);\n");
	printf("            }\n");
	printf("        }\n");
	printf("        if (nextX > targetX) {\n");
	printf("            nextObj.style.setProperty(\"visibility\",");
						printf("\"hidden\");\n");
	printf("            gapY += 15;\n");
	printf("            if (collapsed[0][targetID] == 1) {\n");
	printf("                targetID = collapsed[1][targetID];\n");
	printf("            } else {\n");
	printf("                targetID++;\n");
	printf("            }\n");
	printf("            nextObj = svgdoc.getElementById(\"MI\"+");
							printf("targetID);\n");
	printf("            if (nextObj == null)\n");
	printf("                break;\n");
	printf("        } else {\n");
	printf("            break;\n");
	printf("        }\n");
	printf("    }\n");
	printf("    //save next uncollapsed element in array\n");
	printf("    collapsed[1][clickedID] = targetID;\n");
	printf("    if (nextObj == null)\n");
	printf("        return;\n");
	printf("    //move following texts upwards\n");
	printf("    while (nextObj != null) {\n");
	printf("        attr = nextObj.attributes;\n");
	printf("        for (i=0;i<attr.length;i++) {\n");
	printf("            if (attr.item(i).nodeName == \"transform\") {\n");
	printf("                k = attr.item(i).nodeValue.indexOf(\"(\");\n");
	printf("                l = attr.item(i).nodeValue.indexOf(\",\");\n");
	printf("                m = attr.item(i).nodeValue.indexOf(\")\");\n");
	printf("                nextX = attr.item(i).nodeValue");
						printf(".substring(k+1,l);\n");
	printf("                nextY = attr.item(i).nodeValue");
						printf(".substring(l+1,m);\n");
	printf("            }\n");
	printf("        }\n");
	printf("        nextY -= gapY;\n");
	printf("        nextObj.setAttribute(\"transform\",\"translate(\"+");
					printf("nextX+\",\"+nextY+\")\");\n");
	printf("        targetID++;\n");
	printf("        nextObj = svgdoc.getElementById(\"MI\"+targetID);\n");
	printf("    }\n");
	printf("}\n\n");
	printf("function init(evt) {\n");
	printf("    for (i=0; i<%i; i++) {\n",nodecount);
	printf("        scalFac[i] = %.1f;\n", STARTSCALE);
	printf("    }\n");
	//FIXME calculate arraysize dynamically
	printf("    collapsed[0] = new Array(1000);\n");
	printf("    collapsed[1] = new Array(1000);\n");
	printf("    for (i=0; i<1000; i++) {\n");
	printf("        collapsed[0][i] = 0;\n");
	printf("        collapsed[1][i] = 0;\n");
	printf("    }\n");
	printf("    getSVGDoc(evt);\n");
	printf("}\n\n");
	printf("function colorText(object, color) {\n");
	printf("    var obj = svgDocument.getElementById(object);\n");
	printf("    obj.setAttribute(\"style\",\"fill: \"+color);\n");
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
    }

    printf(" <title>%s</title>\n", note1);

    //definitions for the arrowheads
    printf(" <defs>\n");
    printf("   <marker id=\"arrowstart\" markerWidth=\"12\"");
    printf(" markerHeight=\"8\" refX=\"0\" refY=\"4\" orient=\"auto\">\n");
    printf("     <path d=\"M 12 0 0 4 12 8\"");
    printf(" fill=\"none\" stroke=\"black\"/>\n");
    printf("   </marker>\n");
    printf("   <marker id=\"arrowend\" markerWidth=\"12\"");
    printf(" markerHeight=\"8\" refX=\"12\" refY=\"4\" orient=\"auto\">\n");
    printf("     <path d=\"M 0 0 12 4 0 8\"");
    printf(" fill=\"none\" stroke=\"black\"/>\n");
    printf("   </marker>\n");
    printf(" </defs>\n\n");

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
    float      lastHeight;

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

    lastHeight = node->dia.h;
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
    if (node->dia.h > lastHeight) {
	node->dia.relatedScalars = 1;
	node->dia.h += RELATEDSCALARHEIGHT;
    }

    lastHeight = node->dia.h;
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
					    strlen(algGetTypeName(tNode)) + 3)
						    * ATTRFONTSIZE
						    + ATTRSPACESIZE);
		    node->dia.h += TABLEELEMHEIGHT;
		}
	    }
	}
    }

    //C
    for (smiElement = smiGetFirstElement(
	smiGetFirstChildNode(node->smiNode));
	 smiElement;
	 smiElement = smiGetNextElement(smiElement)) {
	
	tNode = smiGetElementNode(smiElement);
	
	node->dia.w = max(node->dia.w, (strlen(tNode->name) +
					strlen(algGetTypeName(tNode)) + 3)
		      * ATTRFONTSIZE
		      + ATTRSPACESIZE);
	node->dia.h += TABLEELEMHEIGHT;
    }
    if (node->dia.h > lastHeight) {
	node->dia.indexObjects = 1;
	node->dia.h += INDEXOBJECTHEIGHT;
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


static void printModuleIdentity(int modc, SmiModule **modv,
				float *x, float *y, int *miNr)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">MODULE-IDENTITY</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name and description of the module.
	    *x += TABLEELEMHEIGHT;
	    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
	    printf("  <text>\n");
	    if (!STATIC_OUTPUT) {
		printf("   <tspan style=\"text-anchor:middle\"");
		printf(" onclick=\"collapse(evt)\">--</tspan>\n");
	    }
	    printf("   <tspan x=\"5\"");
	    if (!STATIC_OUTPUT && modv[i]->description) {
		tooltip = (char *)xmalloc(2*strlen(modv[i]->description));
		parseTooltip(modv[i]->description, tooltip);
		printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
		printf(" onmouseout=\"HideTooltip(evt)\"");
		xfree(tooltip);
	    }
	    printf(">%s</tspan>\n", smiNode->name);
	    printf("  </text>\n");
	    printf(" </g>\n");
	    (*miNr)++;
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //revision history of the module.
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    smiRevision = smiGetFirstRevision(modv[i]);
	    if (!smiRevision) {
		printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
		printf("  <text>1970-01-01</text>\n");
		printf(" </g>\n");
		(*miNr)++;
		*y += TABLEELEMHEIGHT;
	    } else {
		for(; smiRevision;
				smiRevision = smiGetNextRevision(smiRevision)) {
		    printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		    printf("(%.2f,%.2f)\">\n", *x, *y);
		    printf("  <text");
		    if (!STATIC_OUTPUT && smiRevision->description && strcmp(
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
		    printf(" </g>\n");
		    *y += TABLEELEMHEIGHT;
		    (*miNr)++;
		}
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printNotificationType(int modc, SmiModule **modv,
				  float *x, float *y, int *miNr)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">NOTIFICATION-TYPE</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
	    printf("  <text>\n");
	    if (!STATIC_OUTPUT) {
		printf("   <tspan style=\"text-anchor:middle\"");
		printf(" onclick=\"collapse(evt)\">--</tspan>\n");
	    }
	    printf("   <tspan x=\"5\">%s</tspan>\n", smiNode->name);
	    printf("  </text>\n");
	    printf(" </g>\n");
	    (*miNr)++;
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the notification
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_NOTIFICATION);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {
		if ((smiNode->status == SMI_STATUS_DEPRECATED
		    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
		    || (smiNode->status == SMI_STATUS_OBSOLETE
		    && !SHOW_DEPR_OBSOLETE))
		    continue;
		printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		printf("(%.2f,%.2f)\">\n", *x, *y);
		printf("  <text id=\"%s\"", smiNode->name);

		if (!STATIC_OUTPUT) {
		    smiElement = smiGetFirstElement(smiNode);
		    if (smiElement || smiNode->description) {
			printf(" onmousemove=\"");
		    }
		    if (smiNode->description) {
			tooltip =
				(char *)xmalloc(2*strlen(smiNode->description));
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
		}

		printf(">%s", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		printf(" </g>\n");
		*y += TABLEELEMHEIGHT;
		(*miNr)++;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printObjectGroup(int modc, SmiModule **modv,
			     float *x, float *y, int *miNr)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">OBJECT-GROUP</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
	    printf("  <text>\n");
	    if (!STATIC_OUTPUT) {
		printf("   <tspan style=\"text-anchor:middle\"");
		printf(" onclick=\"collapse(evt)\">--</tspan>\n");
	    }
	    printf("   <tspan x=\"5\">%s</tspan>\n", smiNode->name);
	    printf("  </text>\n");
	    printf(" </g>\n");
	    (*miNr)++;
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the group
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_GROUP);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
		if (!isObjectGroup(smiNode))
		    continue;
		if ((smiNode->status == SMI_STATUS_DEPRECATED
		    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
		    || (smiNode->status == SMI_STATUS_OBSOLETE
		    && !SHOW_DEPR_OBSOLETE))
		    continue;
		printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		printf("(%.2f,%.2f)\">\n", *x, *y);
		printf("  <text id=\"%s\"", smiNode->name);

		if (!STATIC_OUTPUT) {
		    smiElement = smiGetFirstElement(smiNode);
		    if (smiElement || smiNode->description) {
			printf(" onmousemove=\"");
		    }
		    if (smiNode->description) {
			tooltip =
				(char *)xmalloc(2*strlen(smiNode->description));
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
		}

		printf(">%s", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		printf(" </g>\n");
		*y += TABLEELEMHEIGHT;
		(*miNr)++;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printNotificationGroup(int modc, SmiModule **modv,
				   float *x, float *y, int *miNr)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    SmiElement  *smiElement;
    SmiRevision *smiRevision;

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">NOTIFICATION-GROUP</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
	    printf("  <text>\n");
	    if (!STATIC_OUTPUT) {
		printf("   <tspan style=\"text-anchor:middle\"");
		printf(" onclick=\"collapse(evt)\">--</tspan>\n");
	    }
	    printf("   <tspan x=\"5\">%s</tspan>\n", smiNode->name);
	    printf("  </text>\n");
	    printf(" </g>\n");
	    (*miNr)++;
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the group
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_GROUP);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
		if (!isNotificationGroup(smiNode))
		    continue;
		if ((smiNode->status == SMI_STATUS_DEPRECATED
		    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
		    || (smiNode->status == SMI_STATUS_OBSOLETE
		    && !SHOW_DEPR_OBSOLETE))
		    continue;
		printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		printf("(%.2f,%.2f)\">\n", *x, *y);
		printf("  <text id=\"%s\"", smiNode->name);

		if (!STATIC_OUTPUT) {
		    smiElement = smiGetFirstElement(smiNode);
		    if (smiElement || smiNode->description) {
			printf(" onmousemove=\"");
		    }
		    if (smiNode->description) {
			tooltip =
				(char *)xmalloc(2*strlen(smiNode->description));
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
		}

		printf(">%s", smiNode->name);
		printf(" (%s)</text>\n", getStatusString(smiNode->status));
		printf(" </g>\n");
		*y += TABLEELEMHEIGHT;
		(*miNr)++;
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printModuleCompliance(int modc, SmiModule **modv,
				  float *x, float *y, int *miNr)
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

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">MODULE-COMPLIANCE</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {

	    //name of the module
	    *x += TABLEELEMHEIGHT;
	    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
	    printf("  <text>\n");
	    if (!STATIC_OUTPUT) {
		printf("   <tspan style=\"text-anchor:middle\"");
		printf(" onclick=\"collapse(evt)\">--</tspan>\n");
	    }
	    printf("   <tspan x=\"5\">%s</tspan>\n", smiNode->name);
	    printf("  </text>\n");
	    printf(" </g>\n");
	    (*miNr)++;
	    *y += TABLEELEMHEIGHT;
	    *x -= TABLEELEMHEIGHT;

	    //name, status and description of the compliance
	    //TODO print text in different grey colors for different statuses.
	    *x += 2*TABLEELEMHEIGHT;
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_COMPLIANCE);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
		if ((smiNode->status == SMI_STATUS_DEPRECATED
		    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
		    || (smiNode->status == SMI_STATUS_OBSOLETE
		    && !SHOW_DEPR_OBSOLETE))
		    continue;
		printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		printf("(%.2f,%.2f)\">\n", *x, *y);
		printf("  <text>\n");
		if (!STATIC_OUTPUT) {
		    printf("   <tspan style=\"text-anchor:middle\"");
		    printf(" onclick=\"collapse(evt)\">--</tspan>\n");
		}
		printf("   <tspan x=\"5\"");

		if (!STATIC_OUTPUT && smiNode->description) {
		    tooltip = (char *)xmalloc(2*strlen(smiNode->description));
		    parseTooltip(smiNode->description, tooltip);
		    printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')", tooltip);
		    xfree(tooltip);
		    printf("\" onmouseout=\"HideTooltip(evt)\"");
		}
		printf(">%s", smiNode->name);
		printf(" (%s)</tspan>\n", getStatusString(smiNode->status));
		printf("  </text>\n");
		printf(" </g>\n");
		(*miNr)++;
		*y += TABLEELEMHEIGHT;

		//modules for the compliance
		*x += TABLEELEMHEIGHT;
		done = xstrdup("+");
		for (module = modv[i]->name; module; ) {
		    printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		    printf("(%.2f,%.2f)\">\n", *x, *y);
		    printf("  <text>\n");
		    if (!STATIC_OUTPUT) {
			printf("   <tspan style=\"text-anchor:middle\"");
			printf(" onclick=\"collapse(evt)\">--</tspan>\n");
		    }
		    printf("   <tspan x=\"5\">%s</tspan>\n", module);
		    printf("  </text>\n");
		    printf(" </g>\n");
		    (*miNr)++;
		    *y += TABLEELEMHEIGHT;

		    //mandatory groups
		    *x += TABLEELEMHEIGHT;
		    *x += TABLEBOTTOMHEIGHT;
		    printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
		    printf("(%.2f,%.2f)\">\n", *x, *y);
		    printf("  <text");
		    //printf(" <text x=\"%.2f\" y=\"%.2f\"", *x, *y);
		    if (!STATIC_OUTPUT) {
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
		    }
		    printf(">MANDATORY-GROUPS</text>\n");
		    printf(" </g>\n");
		    *y += TABLEELEMHEIGHT;
		    (*miNr)++;

		    //groups
		    for (smiOption = smiGetFirstOption(smiNode); smiOption;
				    smiOption = smiGetNextOption(smiOption)) {
			smiNode2 = smiGetOptionNode(smiOption);
			smiModule2 = smiGetNodeModule(smiNode2);
			if (!strcmp(smiModule2->name, module)) {
			    printf(" <g id=\"MI%i\" transform=", *miNr);
			    printf("\"translate(%.2f,%.2f)\">\n", *x, *y);
			    printf("  <text");
			    if (!STATIC_OUTPUT) {
				printf(" onmousemove=\"");
				if (smiOption->description) {
				    tooltip = (char *)xmalloc(2*strlen(
						smiOption->description));
				    parseTooltip(smiOption->description,
								tooltip);
				    printf("ShowTooltipMZ(evt,'%s');", tooltip);
				    xfree(tooltip);
				}
				printf("colorText('%s','red')", smiNode2->name);
				printf("\" onmouseout=\"");
				if (smiOption->description) {
				    printf("HideTooltip(evt);");
				}
				printf("colorText('%s','black')\"",
								smiNode2->name);
			    }
			    printf(">GROUP %s</text>\n", smiNode2->name);
			    printf(" </g>\n");
			    *y += TABLEELEMHEIGHT;
			    (*miNr)++;
			}
		    }

		    //objects
		    for (smiRefinement = smiGetFirstRefinement(smiNode);
			smiRefinement;
			smiRefinement = smiGetNextRefinement(smiRefinement)) {
			smiNode2 = smiGetRefinementNode(smiRefinement);
			smiModule2 = smiGetNodeModule(smiNode2);
			if (!strcmp(smiModule2->name, module)) {
			    printf(" <g id=\"MI%i\" transform=", *miNr);
			    printf("\"translate(%.2f,%.2f)\">\n", *x, *y);
			    printf("  <text");
			    if (!STATIC_OUTPUT) {
				printf(" onmousemove=\"");
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
				printf("colorText('%s','black')\"",
								smiNode2->name);
			    }
			    printf(">OBJECT %s</text>\n", smiNode2->name);
			    printf(" </g>\n");
			    *y += TABLEELEMHEIGHT;
			    (*miNr)++;
			}
		    }
		    *x -= TABLEELEMHEIGHT;
		    *x -= TABLEBOTTOMHEIGHT;

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
    int miNr = 0;

    printf(" <g transform=\"translate(%.2f,%.2f) scale(%.2f)\">\n",
							x, y, STARTSCALE);
    //now use x and y as relative coordinates.
    x = 0;
    y = 10;

    printModuleIdentity(modc, modv, &x, &y, &miNr);
    printNotificationType(modc, modv, &x, &y, &miNr);
    printObjectGroup(modc, modv, &x, &y, &miNr);
    printNotificationGroup(modc, modv, &x, &y, &miNr);
    printModuleCompliance(modc, modv, &x, &y, &miNr);

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
    if (graph->clusters->nextPtr)
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
	/* This option is also provided by the cm-driver, so we omit it here.
	{ "explain", OPT_FLAG, &XPLAIN, 0,
	  "explain what the algorithm does"},
	*/
	{ "width", OPT_INT, &CANVASWIDTH, 0,
	  "width of the svg output"},
	{ "height", OPT_INT, &CANVASHEIGHT, 0,
	  "height of the svg output"},
	{ "show-deprecated", OPT_FLAG, &SHOW_DEPRECATED, 0,
	  "show deprecated objects"},
	{ "show-depr-obsolete", OPT_FLAG, &SHOW_DEPR_OBSOLETE, 0,
	  "show deprecated and obsolete objects"},
	{ "static-output", OPT_FLAG, &STATIC_OUTPUT, 0,
	  "disable all interactivity (e.g. for printing)"},
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
