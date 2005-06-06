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
#include "dump-svg-script.h"

#define URL "http://libsmi.dyndns.org/cgi-bin/mib2svg.cgi?"



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

static const int MODULE_INFO_WIDTH     =150;
//The description of RowStatus is quite long... :-/
static const int DYN_TEXT              =470;
static const float STARTSCALE          =(float)0.5;

//used by the springembedder
static const int ITERATIONS            =100;

static char *link;
static const char *linkcolor = "rgb(0%,0%,50%)";



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
    SmiNode   *node;
    GraphNode *newNode;
    
    /* get tables and scalars from the MIB module */
    for (node = smiGetFirstNode(module, SMI_NODEKIND_TABLE);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_TABLE)) {
	if ((node->status == SMI_STATUS_DEPRECATED
	    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
	    || (node->status == SMI_STATUS_OBSOLETE
	    && !SHOW_DEPR_OBSOLETE))
	    continue;
	newNode = graphInsertNode(graph, node);
	newNode->smiModule = module;
    }
    for (node = smiGetFirstNode(module, SMI_NODEKIND_SCALAR);
	 node;
	 node = smiGetNextNode(node, SMI_NODEKIND_SCALAR)) {
	if ((node->status == SMI_STATUS_DEPRECATED
	    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
	    || (node->status == SMI_STATUS_OBSOLETE
	    && !SHOW_DEPR_OBSOLETE))
	    continue;
	newNode = graphInsertNode(graph, node);
	newNode->smiModule = module;
    }
}



/* ------ XML primitives ------                                              */



/*
 * parseTooltip: Parse any input to output to make the text safe for the
 * ShowTooltipMZ-functin in the ecma-script.
 */
static void parseTooltip(char *input, char *output)
{
    int i, j;

    for (i = j = 0; input[i]; i++) {
	switch (input[i]) {
	case '\n':
	    output[j++] = '\\';
	    output[j++] = 'n';
	    break;
	case '\\':
	    output[j++] = '\\';
	    output[j++] = '\\';
	    break;
	case '\"':
	    //quotes are not allowed in strings.
	    //See chapter 3.4.5 in "Understanding SNMP MIBs"
	    break;
	case '&':
	    output[j++] = '&';
	    output[j++] = 'a';
	    output[j++] = 'm';
	    output[j++] = 'p';
	    output[j++] = ';';
	    break;
	case '<':
	    output[j++] = '&';
	    output[j++] = 'l';
	    output[j++] = 't';
	    output[j++] = ';';
	    break;
	case '>':
	    output[j++] = '&';
	    output[j++] = 'g';
	    output[j++] = 't';
	    output[j++] = ';';
	    break;
	case '\'':
	    //It seems, &apos; doesn't work...
	    output[j++] = '\\';
	    output[j++] = '\'';
	    break;
	default:
	    output[j++] = input[i];
	}
    }
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
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	(status == SMI_STATUS_OPTIONAL)    ? "optional" :
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
	printf("   <text class=\"tooltip\" xml:space=\"preserve\"");
	printf(" id=\"ttt\" x=\"0\" y=\"0\" style=\"visibility: hidden\">");
	printf("dyn. Text</text>\n");
	//FIXME: calculate number of lines dynamically.
	for (i = 0; i < DYN_TEXT; i++) {
	    printf("   <text class=\"tooltip\" xml:space=\"preserve\"");
	    printf(" x=\"-10\" y=\"-10\">dyn. Text</text>\n");
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
			      int modc, SmiModule **modv,
			      float *textYOffset, float *textXOffset)
{
    int         i, target_exists = 0;
    size_t      length;
    char        *tooltip, *tooltipDescription, *typeDescription;
    const char  *baseTypeTooltipText = "This is a basetype.";
    const char  *isDefined = " is defined in module ";

    printf("    <text ");
    if (!index) {
	printf("id=\"%s\" ", node->name);
    }
    printf("x=\"%.2f\" y=\"%.2f\">\n",
				*textXOffset + ATTRSPACESIZE, *textYOffset);

    *textYOffset += TABLEELEMHEIGHT;

    //FIXME
    //printf(" textLength=\"100\" lengthAdjust=\"spacingAndGlyphs\"");

    if (!index) {
	if (node->access == SMI_ACCESS_NOT_ACCESSIBLE) {
	    printf("         -");
	} else {
	    printf("         +");
	}
    }

    printf("         <tspan");
    if (!STATIC_OUTPUT && node->description) {
	tooltip = (char *)xmalloc(2*strlen(node->description));
	parseTooltip(node->description, tooltip);
	printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	printf(" onmouseout=\"HideTooltip(evt)\"");
	xfree(tooltip);
    }
    printf(">%s:</tspan>\n", node->name);

    printf("         <tspan");
    if (!STATIC_OUTPUT) {
	//FIXME <tspan><tspan>test</tspan></tspan> isn't allowed :-(
	if ((typeDescription = algGetTypeDescription(node))) {
	    tooltipDescription = (char *)xmalloc(2*strlen(typeDescription));
	    parseTooltip(typeDescription, tooltipDescription);
	    if (algGetTypeModule(node)) {
		length = strlen(tooltipDescription) + 150;
		tooltip = (char *)xmalloc(length);
		strcpy(tooltip, algGetTypeName(node));
		strcat(tooltip, isDefined);
		strcat(tooltip, algGetTypeModule(node)->name);
		strcat(tooltip, ":\\n\\n");
		strcat(tooltip, tooltipDescription);
	    } else {
		length = strlen(tooltipDescription);
		tooltip = (char *)xmalloc(length);
		strcpy(tooltip, tooltipDescription);
	    }
	    xfree(tooltipDescription);
	    printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	    printf(" onmouseout=\"HideTooltip(evt)\"");
	    xfree(tooltip);
	    printf(">");
	    if (algGetTypeModule(node)) {
		for (i=0; i<modc; i++) {
		    if (modv[i] == algGetTypeModule(node)) {
			target_exists = 1;
		    }
		}
		if (!target_exists) {
		    printf("\n");
		    printf("          <a xlink:href=\"%s", link);
		    printf("&amp;mibs=%s\">\n", algGetTypeModule(node)->name);
		    printf("           <tspan fill=\"%s\">", linkcolor);
		    printf("%s</tspan>\n", algGetTypeName(node));
		    printf("          </a>");
		    printf("\n");
		} else {
		    printf("%s", algGetTypeName(node));
		}
	    } else {
		printf("%s", algGetTypeName(node));
	    }
	    printf("        </tspan></text>\n");
	} else if (isBaseType(node)) {
	    length = strlen(baseTypeTooltipText) + 1;
	    tooltip = (char *)xmalloc(length);
	    strcpy(tooltip, baseTypeTooltipText);
	    printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	    printf(" onmouseout=\"HideTooltip(evt)\"");
	    xfree(tooltip);
	    printf(">%s</tspan></text>\n", algGetTypeName(node));
	}
    } else {
	printf(">%s</tspan></text>\n", algGetTypeName(node));
    }
}

/*
 * prints the related scalars for a given table
 */
static void printSVGRelatedScalars(GraphNode *node,
				   int modc, SmiModule **modv,
				   float *textYOffset, float *textXOffset)
{
    GraphEdge *tEdge;
    
    for (tEdge = graphGetFirstEdgeByNode(graph, node);
	 tEdge;
	 tEdge = graphGetNextEdgeByNode(graph, tEdge, node)) {
	if (tEdge->startNode == node  &&
	    tEdge->endNode->smiNode->nodekind == SMI_NODEKIND_SCALAR) {

	    printSVGAttribute(tEdge->endNode->smiNode, 0,
			      modc, modv,
			      textYOffset, textXOffset);
	}
    }
}

/*
 * prints all columns objects of the given node
 */
static void printSVGAllColumns(GraphNode *node,
			       int modc, SmiModule **modv,
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
	    printSVGAttribute(smiNode, 0,
			      modc, modv,
			      textYOffset, textXOffset);
    }
}

/*
 * adds the index to an augmenting table (row-element)
 */
static void printSVGAugmentIndex(GraphNode *tNode,
				 int modc, SmiModule **modv,
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
		    printSVGAttribute(smiGetElementNode(smiElement), 1,
				      modc, modv,
				      textYOffset, textXOffset);
		}
	    }
	}
    }
}

/*
 * create svg-output for the given node
 */
static void printSVGObject(GraphNode *node, int *classNr,
			   int modc, SmiModule **modv)
{
    SmiElement *smiElement;
    float textXOffset, textYOffset, xOrigin, yOrigin;
    size_t length;
    char *tooltip, *tooltipTable, *tooltipEntry;
    const char *tableHeading = "Table-Description:\\n";
    const char *entryHeading = "Entry-Description:\\n";
    const char *blankLine = "\\n\\n";
    
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
    printf("    <rect id=\"%sRect\"",
           smiGetFirstChildNode(node->smiNode)->name);
    printf(" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"\n",
           xOrigin, yOrigin, node->dia.w, node->dia.h);
    printf("          fill=\"white\" stroke=\"black\"/>\n");
    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
           xOrigin, yOrigin + TABLEHEIGHT,
           xOrigin + node->dia.w, yOrigin + TABLEHEIGHT);
    printf("          fill=\"none\" stroke=\"black\"/>\n");
    printf("    <text x=\"0\" y=\"%.2f\"\n", yOrigin + 15);
    printf("          style=\"text-anchor:middle; font-weight:bold\"");

    //descriptions for the table and the entries
    if (!STATIC_OUTPUT) {
	if (node->smiNode->description) {
	    tooltipTable=(char *)xmalloc(2*strlen(node->smiNode->description));
	    parseTooltip(node->smiNode->description, tooltipTable);
	}
	if (smiGetFirstChildNode(node->smiNode)->description) {
	    tooltipEntry=(char *)xmalloc(2*strlen(smiGetFirstChildNode(
						node->smiNode)->description));
	    parseTooltip(smiGetFirstChildNode(node->smiNode)->description,
								tooltipEntry);
	}

	length = strlen(tableHeading) + strlen(entryHeading)
		 + strlen(blankLine) + 1
		 + strlen(tooltipTable) + strlen(tooltipEntry);
	tooltip = (char *)xmalloc(length);

	strcpy(tooltip, "\0");
	if (node->smiNode->description) {
	    strcat(tooltip, tableHeading);
	    strcat(tooltip, tooltipTable);
	}
	if (node->smiNode->description
		&& smiGetFirstChildNode(node->smiNode)->description) {
	    strcat(tooltip, blankLine);
	}
	if (smiGetFirstChildNode(node->smiNode)->description) {
	    strcat(tooltip, entryHeading);
	    strcat(tooltip, tooltipEntry);
	}

	printf(" onmousemove=\"ShowTooltipMZ(evt,'%s')\"", tooltip);
	printf(" onmouseout=\"HideTooltip(evt)\"");

	if (node->smiNode->description) {
	    xfree(tooltipTable);
	}
	if (smiGetFirstChildNode(node->smiNode)->description) {
	    xfree(tooltipEntry);
	}
	xfree(tooltip);
    }

    printf(">\n");
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
	    //A
	    printSVGRelatedScalars(node,
				   modc, modv,
				   &textYOffset, &textXOffset);

	    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
			    xOrigin,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT,
			    xOrigin + node->dia.w,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT);
	    printf("          fill=\"none\" stroke=\"black\"/>\n");
	    textYOffset += TABLEBOTTOMHEIGHT;
	}

	if (node->dia.indexObjects) {
	    //B
	    printSVGAugmentIndex(node,
				 modc, modv,
				 &textYOffset, &textXOffset);
	    //C
	    for (smiElement = smiGetFirstElement(
		smiGetFirstChildNode(node->smiNode));
		 smiElement;
		 smiElement = smiGetNextElement(smiElement)) {
		printSVGAttribute(smiGetElementNode(smiElement), 1,
				  modc, modv,
				  &textYOffset, &textXOffset);
	    }

	    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
			    xOrigin,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT,
			    xOrigin + node->dia.w,
			    textYOffset - TABLEELEMHEIGHT + TABLEBOTTOMHEIGHT);
	    printf("          fill=\"none\" stroke=\"black\"/>\n");
	    textYOffset += TABLEBOTTOMHEIGHT;
	}

	//D
	if (PRINT_DETAILED_ATTR) {
	    printSVGAllColumns(node,
			       modc, modv,
			       &textYOffset, &textXOffset);
	}
    }

    printf("  </g>\n");
    printf(" </g>\n");
}

/*
 * prints a group of scalars denoted by group
 */
static void printSVGGroup(int group, int *classNr,
			  int modc, SmiModule **modv)
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
    printf("    <rect id=\"%sRect\"",
           smiGetParentNode(tNode->smiNode)->name);
    printf(" x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\"\n",
           xOrigin, yOrigin, tNode->dia.w, tNode->dia.h);
    printf("          fill=\"white\" stroke=\"black\"/>\n");
    printf("    <polygon points=\"%.2f %.2f %.2f %.2f\"\n",
           xOrigin, yOrigin + TABLEHEIGHT,
           xOrigin + tNode->dia.w, yOrigin + TABLEHEIGHT);
    printf("          fill=\"none\" stroke=\"black\"/>\n");
    printf("    <text x=\"0\" y=\"%.2f\"\n", yOrigin + 15);
    printf("          style=\"text-anchor:middle; font-weight:bold\">\n");
    //groups don't seem to have a description.
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
	    printSVGAttribute(tNode->smiNode, 0,
			      modc, modv,
			      &textYOffset, &textXOffset);
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

    //print text upside down, if angle is between 180� and 360�
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
	printf(" startOffset=\"10%%\">\n");
    } else {
	printf(" startOffset=\"90%%\">\n");
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
	;
    }
    printf("</textPath>\n");
    printf(" </text>\n");

    if (tEdge->indexkind==SMI_INDEX_AUGMENT ||
	tEdge->indexkind==SMI_INDEX_SPARSE ||
	tEdge->indexkind==SMI_INDEX_REORDER ||
	tEdge->indexkind==SMI_INDEX_EXPAND) {
	printf(" <text text-anchor=\"middle\">\n");
	printf("    <textPath xlink:href=\"#%s%s\" startOffset=\"50%%\">\n",
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
	;
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
	printf(" startOffset=\"90%%\">\n");
    } else {
	printf(" startOffset=\"10%%\">\n");
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
	;
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
static void printSVGHeaderAndTitle(int modc, SmiModule **modv,
				   int nodecount, int miCount,
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
	//css-stylesheet for the tooltip-text
	printf("<style type=\"text/css\">\n<![CDATA[\ntext.tooltip {\n");
	printf("    font-family: \"Courier New\", Courier, monospace;\n}\n");
	printf("]]>\n</style>\n\n");

	//the ecma-script for handling the "+"- and "-"-buttons
	//and the tooltip
	//and the folding of the module information
	//and the colorizing of the text
	printf("<script type=\"text/ecmascript\">\n<![CDATA[\n");
	//print the script from the included file
	//FIXME calculate things dynamically:
	//      * maximal number of lines for the tooltip.
	printf(code, nodecount, DYN_TEXT, nodecount, STARTSCALE,
						    miCount, miCount, miCount);
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
    node->dia.x = (float) rand();
    node->dia.y = (float) rand();
    node->dia.x /= (float) RAND_MAX;
    node->dia.y /= (float) RAND_MAX;
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
	node->dia.h += TABLEBOTTOMHEIGHT;
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
	node->dia.h += TABLEBOTTOMHEIGHT;
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


static void calcModuleIdentityCount(int modc, SmiModule **modv, int *miCount)
{
    int         i;
    SmiNode     *smiNode;
    SmiRevision *smiRevision;

    //MODULE-IDENTITY
    (*miCount)++;
    for (i = 0; i < modc; i++) {
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {
	    //name of the module
	    (*miCount)++;
	    //revision history of the module
	    smiRevision = smiGetFirstRevision(modv[i]);
	    if (!smiRevision) {
		(*miCount)++;
	    } else {
		for(; smiRevision;
				smiRevision = smiGetNextRevision(smiRevision)) {
		    (*miCount)++;
		}
	    }
	}
    }
}

static void calcNotificationTypeCount(int modc, SmiModule **modv,
				      int *miCount, int nType[])
{
    int     i;
    SmiNode *smiNode;

    //NOTIFICATION-TYPE
    (*miCount)++;
    for (i = 0; i < modc; i++) {
	nType[i] = 0;
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {
	    //name of the module
	    (*miCount)++;
	    //name of the notification
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_NOTIFICATION);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION)) {
		if ((smiNode->status == SMI_STATUS_DEPRECATED
		    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
		    || (smiNode->status == SMI_STATUS_OBSOLETE
		    && !SHOW_DEPR_OBSOLETE))
		    continue;
		(*miCount)++;
		nType[i] = 1;
	    }
	}
    }
}

static void calcObjectGroupCount(int modc, SmiModule **modv,
				 int *miCount, int oGroup[])
{
    int     i;
    SmiNode *smiNode;

    //OBJECT-GROUP
    (*miCount)++;
    for (i = 0; i < modc; i++) {
	oGroup[i] = 0;
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {
	    //name of the module
	    (*miCount)++;
	    //name of the group
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
		(*miCount)++;
		oGroup[i] = 1;
	    }
	}
    }
}

static void calcNotificationGroupCount(int modc, SmiModule **modv,
				       int *miCount, int nGroup[])
{
    int     i;
    SmiNode *smiNode;

    //NOTIFICATION-GROUP
    (*miCount)++;
    for (i = 0; i < modc; i++) {
	nGroup[i] = 0;
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {
	    //name of the module
	    (*miCount)++;
	    //name of the group
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
		(*miCount)++;
		nGroup[i] = 1;
	    }
	}
    }
}

static void calcModuleComplianceCount(int modc, SmiModule **modv,
				      int *miCount, int mCompl[])
{
    int           i;
    char          *done = NULL;
    char          s[100];
    char          *module;
    SmiNode       *smiNode, *smiNode2;
    SmiModule     *smiModule2;
    SmiElement    *smiElement;
    SmiOption     *smiOption;
    SmiRefinement *smiRefinement;

    //MODULE-COMPLIANCE
    (*miCount)++;
    for (i = 0; i < modc; i++) {
	mCompl[i] = 0;
	smiNode = smiGetModuleIdentityNode(modv[i]);
	if (smiNode) {
	    //name of the module
	    (*miCount)++;
	    //name of the compliance
	    for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_COMPLIANCE);
		smiNode;
		smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_COMPLIANCE)) {
		if ((smiNode->status == SMI_STATUS_DEPRECATED
		    && !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
		    || (smiNode->status == SMI_STATUS_OBSOLETE
		    && !SHOW_DEPR_OBSOLETE))
		    continue;
		(*miCount)++;
		mCompl[i] = 1;
		//modules for the compliance
		done = xstrdup("+");
		for (module = modv[i]->name; module; ) {
		    //name of the module
		    (*miCount)++;
		    //mandatory groups
		    (*miCount)++;
		    //groups
		    for (smiOption = smiGetFirstOption(smiNode); smiOption;
				    smiOption = smiGetNextOption(smiOption)) {
			smiNode2 = smiGetOptionNode(smiOption);
			smiModule2 = smiGetNodeModule(smiNode2);
			if (!strcmp(smiModule2->name, module)) {
			    (*miCount)++;
			}
		    }
		    //objects
		    for (smiRefinement = smiGetFirstRefinement(smiNode);
			smiRefinement;
			smiRefinement = smiGetNextRefinement(smiRefinement)) {
			smiNode2 = smiGetRefinementNode(smiRefinement);
			smiModule2 = smiGetNodeModule(smiNode2);
			if (!strcmp(smiModule2->name, module)) {
			    (*miCount)++;
			}
		    }
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
	    }
	}
    }
}

/*
 * calculate the number of entries in the module-information-section.
 * headings for empty sections are counted here, but they are omitted
 * in the svg, so the calculated number is an upper bound. the maximal
 * size of this gap is 4*(modc+1). this may be considered as a bug.
 */
static void calcMiCount(int modc, SmiModule **modv, int *miCount,
			int nType[], int oGroup[], int nGroup[], int mCompl[])
{
    calcModuleIdentityCount(modc, modv, miCount);
    calcNotificationTypeCount(modc, modv, miCount, nType);
    calcObjectGroupCount(modc, modv, miCount, oGroup);
    calcNotificationGroupCount(modc, modv, miCount, nGroup);
    calcModuleComplianceCount(modc, modv, miCount, mCompl);
}


/* ------------------------------------------------------------------------- */


static void printInformationNode(SmiNode *smiNode,
				 float *x, float *y, int *miNr)
{
    int         j;
    char        *tooltip;
    SmiElement  *smiElement;

    printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
    printf("(%.2f,%.2f)\">\n", *x, *y);
    printf("  <text id=\"%s\"", smiNode->name);
    switch (smiNode->status) {
    case SMI_STATUS_DEPRECATED:
	printf(" fill=\"rgb(40%%,40%%,40%%)\"");
	break;
    case SMI_STATUS_OBSOLETE:
	printf(" fill=\"rgb(60%%,60%%,60%%)\"");
	break;
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_MANDATORY:
	printf(" fill=\"rgb(0%%,0%%,0%%)\"");
	break;
    case SMI_STATUS_OPTIONAL:
	printf(" fill=\"rgb(20%%,20%%,20%%)\"");
	break;
    case SMI_STATUS_UNKNOWN:
	;
    }

    if (!STATIC_OUTPUT) {
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
    }

    printf(">%s", smiNode->name);
    switch (smiNode->status) {
    case SMI_STATUS_DEPRECATED:
    case SMI_STATUS_OBSOLETE:
    case SMI_STATUS_MANDATORY:
    case SMI_STATUS_OPTIONAL:
	printf(" (%s)", getStatusString(smiNode->status));
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_UNKNOWN:
	;
    }
    printf("</text>\n");
    printf(" </g>\n");
    *y += TABLEELEMHEIGHT;
    (*miNr)++;
}

static void printComplianceNode(SmiNode *smiNode, int modc, SmiModule **modv,
				 float *x, float *y, int *miNr, int i)
{
    int           j, k, foreign_exists, textColor = 0;
    char          *tooltip;
    char          *done = NULL;
    char          s[100];
    char          *module;
    SmiNode       *smiNode2;
    SmiModule     *smiModule2;
    SmiElement    *smiElement;
    //SmiRevision   *smiRevision;
    SmiOption     *smiOption;
    SmiRefinement *smiRefinement;

    printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
    printf("(%.2f,%.2f)\">\n", *x, *y);
    printf("  <text");
    switch (smiNode->status) {
    case SMI_STATUS_DEPRECATED:
	printf(" fill=\"rgb(40%%,40%%,40%%)\"");
	textColor = 40;
	break;
    case SMI_STATUS_OBSOLETE:
	printf(" fill=\"rgb(60%%,60%%,60%%)\"");
	textColor = 60;
	break;
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_MANDATORY:
	printf(" fill=\"rgb(0%%,0%%,0%%)\"");
	textColor = 0;
	break;
    case SMI_STATUS_OPTIONAL:
	printf(" fill=\"rgb(20%%,20%%,20%%)\"");
	textColor = 20;
	break;
    case SMI_STATUS_UNKNOWN:
	;
    }
    printf(">\n");

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
    switch (smiNode->status) {
    case SMI_STATUS_DEPRECATED:
    case SMI_STATUS_OBSOLETE:
    case SMI_STATUS_MANDATORY:
    case SMI_STATUS_OPTIONAL:
	printf(" (%s)", getStatusString(smiNode->status));
    case SMI_STATUS_CURRENT:
    case SMI_STATUS_UNKNOWN:
	;
    }
    printf("</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;

    //modules for the compliance
    *x += TABLEELEMHEIGHT;
    done = xstrdup("+");
    for (module = modv[i]->name; module; ) {
	foreign_exists = 0;
	if (module == modv[i]->name) {
	    foreign_exists = 1;
	} else {
	    for (j = 0; j < modc; j++) {
		if (module == modv[j]->name) {
		    foreign_exists = 1;
		    break;
		}
	    }
	}
	printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
	printf("(%.2f,%.2f)\">\n", *x, *y);
	printf("  <text fill=\"rgb(%i%%,%i%%,%i%%)\">\n",
					    textColor, textColor, textColor);
	if (!STATIC_OUTPUT) {
	    printf("   <tspan style=\"text-anchor:middle\"");
	    printf(" onclick=\"collapse(evt)\">--</tspan>\n");
	}
	if (!foreign_exists && !STATIC_OUTPUT) {
	    printf("   <a xlink:href=\"%s", link);
	    for (k=0; k<modc; k++) {
		printf("&amp;mibs=%s", modv[k]->name);
	    }
	    printf("&amp;mibs=%s\">\n", module);
	    printf("    <tspan fill=\"%s\"", linkcolor);
	    printf(" x=\"5\">%s</tspan>\n", module);
	    printf("   </a>\n");
	} else {
	    printf("    <tspan x=\"5\">%s</tspan>\n", module);
	}
	printf("  </text>\n");
	printf(" </g>\n");
	(*miNr)++;
	*y += TABLEELEMHEIGHT;

	//mandatory groups
	*x += TABLEELEMHEIGHT;
	*x += TABLEBOTTOMHEIGHT;
	printf(" <g id=\"MI%i\" transform=\"translate", *miNr);
	printf("(%.2f,%.2f)\">\n", *x, *y);
	printf("  <text fill=\"rgb(%i%%,%i%%,%i%%)\"",
					    textColor, textColor, textColor);
	if (!STATIC_OUTPUT && foreign_exists) {
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
		    printf("colorText('%s',",
					smiGetElementNode(smiElement)->name);
		    switch (smiGetElementNode(smiElement)->status) {
		    case SMI_STATUS_DEPRECATED:
			printf("'rgb(40%%,40%%,40%%)')");
			break;
		    case SMI_STATUS_OBSOLETE:
			printf("'rgb(60%%,60%%,60%%)')");
			break;
		    case SMI_STATUS_CURRENT:
		    case SMI_STATUS_MANDATORY:
			printf("'rgb(0%%,0%%,0%%)')");
			break;
		    case SMI_STATUS_OPTIONAL:
			printf("'rgb(20%%,20%%,20%%)')");
			break;
		    case SMI_STATUS_UNKNOWN:
			;
		    }
		}
	    }
	    if (j) {
		printf("\"");
	    }
	}
	printf(">Mandatory Groups</text>\n");
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
		printf("  <text fill=\"rgb(%i%%,%i%%,%i%%)\"",
					    textColor, textColor, textColor);
		if (!STATIC_OUTPUT) {
		    printf(" onmousemove=\"");
		    if (smiOption->description) {
			tooltip = (char *)xmalloc(2*strlen(
						    smiOption->description));
			parseTooltip(smiOption->description, tooltip);
			printf("ShowTooltipMZ(evt,'%s')", tooltip);
			xfree(tooltip);
		    }
		    if (smiOption->description && foreign_exists)
			printf(";");
		    if (foreign_exists)
			printf("colorText('%s','salmon')", smiNode2->name);
		    printf("\" onmouseout=\"");
		    if (smiOption->description) {
			printf("HideTooltip(evt)");
		    }
		    if (smiOption->description && foreign_exists)
			printf(";");
		    if (foreign_exists) {
			printf("colorText('%s',", smiNode2->name);
			switch (smiNode2->status) {
			case SMI_STATUS_DEPRECATED:
			    printf("'rgb(40%%,40%%,40%%)')");
			    break;
			case SMI_STATUS_OBSOLETE:
			    printf("'rgb(60%%,60%%,60%%)')");
			    break;
			case SMI_STATUS_CURRENT:
			case SMI_STATUS_MANDATORY:
			    printf("'rgb(0%%,0%%,0%%)')");
			    break;
			case SMI_STATUS_OPTIONAL:
			    printf("'rgb(20%%,20%%,20%%)')");
			    break;
			case SMI_STATUS_UNKNOWN:
			    ;
			}
		    }
		    printf("\"");
		}
		printf(">Group %s</text>\n", smiNode2->name);
		printf(" </g>\n");
		*y += TABLEELEMHEIGHT;
		(*miNr)++;
	    }
	}

	//objects
	for (smiRefinement = smiGetFirstRefinement(smiNode); smiRefinement;
			smiRefinement = smiGetNextRefinement(smiRefinement)) {
	    smiNode2 = smiGetRefinementNode(smiRefinement);
	    smiModule2 = smiGetNodeModule(smiNode2);
	    if (!strcmp(smiModule2->name, module)) {
		printf(" <g id=\"MI%i\" transform=", *miNr);
		printf("\"translate(%.2f,%.2f)\">\n", *x, *y);
		printf("  <text fill=\"rgb(%i%%,%i%%,%i%%)\"",
					    textColor, textColor, textColor);
		if (!STATIC_OUTPUT) {
		    printf(" onmousemove=\"");
		    if (smiRefinement->description) {
			tooltip = (char *)xmalloc(2*strlen(
						smiRefinement->description));
			parseTooltip(smiRefinement->description, tooltip);
			printf("ShowTooltipMZ(evt,'%s')", tooltip);
			xfree(tooltip);
		    }
		    if (smiRefinement->description && foreign_exists)
			printf(";");
		    if (foreign_exists)
			printf("colorText('%s','salmon')", smiNode2->name);
		    printf("\" onmouseout=\"");
		    if (smiRefinement->description) {
			printf("HideTooltip(evt)");
		    }
		    if (smiRefinement->description && foreign_exists)
			printf(";");
		    if (foreign_exists)
			printf("colorText('%s','black')", smiNode2->name);
		    printf("\"");
		}
		printf(">Object %s</text>\n", smiNode2->name);
		printf(" </g>\n");
		*y += TABLEELEMHEIGHT;
		(*miNr)++;
	    }
	}
	*x -= TABLEELEMHEIGHT;
	*x -= TABLEBOTTOMHEIGHT;

	//find next module
	done = xrealloc(done, strlen(done)+strlen(module)+2*sizeof(char));
	strcat(done, module);
	strcat(done, "+");
	module = NULL;
	for (smiElement = smiGetFirstElement(smiNode); smiElement;
				smiElement = smiGetNextElement(smiElement)) {
	    sprintf(s, "+%s+", smiGetNodeModule(smiGetElementNode(
							smiElement))->name);
	    if ((!strstr(done, s))) {
		module = smiGetNodeModule(smiGetElementNode(smiElement))->name;
		break;
	    }
	}
    }
    *x -= TABLEELEMHEIGHT;
}

static void printModuleIdentity(int modc, SmiModule **modv,
				float *x, float *y, int *miNr)
{
    int         i, j;
    char        *tooltip;
    SmiNode     *smiNode;
    //SmiElement  *smiElement;
    SmiRevision *smiRevision;
    GraphNode   *tNode;

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">Modules</tspan>\n");
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
	    if (!STATIC_OUTPUT) {
		if (modv[i]->description || modc > 1) {
		    printf(" onmousemove=\"");
		}
		if (modv[i]->description) {
		    tooltip = (char *)xmalloc(2*strlen(modv[i]->description));
		    parseTooltip(modv[i]->description, tooltip);
		    printf("ShowTooltipMZ(evt,'%s')", tooltip);
		    xfree(tooltip);
		}
		if (modv[i]->description && modc > 1) {
		    printf(";");
		}
		if (modc > 1) {
		    j = 0;
		    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
			if (tNode->smiModule == modv[i] && tNode->use) {
			    if (j) {
				printf(";");
			    }
			    if (tNode->group == 0) {
				printf("colorText('%sRect','blanchedalmond')",
				    smiGetFirstChildNode(tNode->smiNode)->name);
			    } else {
				printf("colorText('%sRect','blanchedalmond')",
				    smiGetParentNode(tNode->smiNode)->name);
			    }
			    j++;
			}
		    }
		}
		if (modv[i]->description || modc > 1) {
		    printf("\" onmouseout=\"");
		}
		if (modv[i]->description) {
		    printf("HideTooltip(evt)");
		}
		if (modv[i]->description && modc > 1) {
		    printf(";");
		}
		if (modc > 1) {
		    j = 0;
		    for (tNode = graph->nodes; tNode; tNode = tNode->nextPtr) {
			if (tNode->smiModule == modv[i] && tNode->use) {
			    if (j) {
				printf(";");
			    }
			    if (tNode->group == 0) {
				printf("colorText('%sRect','white')",
				    smiGetFirstChildNode(tNode->smiNode)->name);
			    } else {
				printf("colorText('%sRect','white')",
				    smiGetParentNode(tNode->smiNode)->name);
			    }
			    j++;
			}
		    }
		}
		if (modv[i]->description || modc > 1) {
		    printf("\"");
		}
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
				  float *x, float *y, int *miNr, int nType[])
{
    int         i, j;
    SmiNode     *smiNode;
    int         statusOrder[5] = {
		    SMI_STATUS_CURRENT,
		    SMI_STATUS_MANDATORY,
		    SMI_STATUS_OPTIONAL,
		    SMI_STATUS_DEPRECATED,
		    SMI_STATUS_OBSOLETE
		};

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">Notifications</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	if (!nType[i])
	    continue;
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
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    for (j=0; j<5; j++) {
		for (smiNode = smiGetFirstNode(modv[i],
						    SMI_NODEKIND_NOTIFICATION);
		    smiNode;
		    smiNode = smiGetNextNode(smiNode,
						SMI_NODEKIND_NOTIFICATION)) {
		    if (smiNode->status != statusOrder[j])
			continue;
		    if ((smiNode->status == SMI_STATUS_DEPRECATED
			&& !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
			|| (smiNode->status == SMI_STATUS_OBSOLETE
			&& !SHOW_DEPR_OBSOLETE))
			continue;
		    printInformationNode(smiNode, x, y, miNr);
		}
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printObjectGroup(int modc, SmiModule **modv,
			     float *x, float *y, int *miNr, int oGroup[])
{
    int         i, j;
    SmiNode     *smiNode;
    int         statusOrder[5] = {
		    SMI_STATUS_CURRENT,
		    SMI_STATUS_MANDATORY,
		    SMI_STATUS_OPTIONAL,
		    SMI_STATUS_DEPRECATED,
		    SMI_STATUS_OBSOLETE
		};

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">Object Groups</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	if (!oGroup[i])
	    continue;
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
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    for (j=0; j<5; j++) {
		for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_GROUP);
		    smiNode;
		    smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
		    if (!isObjectGroup(smiNode))
			continue;
		    if (smiNode->status != statusOrder[j])
			continue;
		    if ((smiNode->status == SMI_STATUS_DEPRECATED
			&& !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
			|| (smiNode->status == SMI_STATUS_OBSOLETE
			&& !SHOW_DEPR_OBSOLETE))
			continue;
		    printInformationNode(smiNode, x, y, miNr);
		}
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printNotificationGroup(int modc, SmiModule **modv,
				   float *x, float *y, int *miNr, int nGroup[])
{
    int         i, j;
    SmiNode     *smiNode;
    int         statusOrder[5] = {
		    SMI_STATUS_CURRENT,
		    SMI_STATUS_MANDATORY,
		    SMI_STATUS_OPTIONAL,
		    SMI_STATUS_DEPRECATED,
		    SMI_STATUS_OBSOLETE
		};

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">Notification Groups</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	if (!nGroup[i])
	    continue;
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
	    *x += 2*TABLEELEMHEIGHT;
	    *x += TABLEBOTTOMHEIGHT;
	    for (j=0; j<5; j++) {
		for (smiNode = smiGetFirstNode(modv[i], SMI_NODEKIND_GROUP);
		    smiNode;
		    smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_GROUP)) {
		    if (!isNotificationGroup(smiNode))
			continue;
		    if (smiNode->status != statusOrder[j])
			continue;
		    if ((smiNode->status == SMI_STATUS_DEPRECATED
			&& !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
			|| (smiNode->status == SMI_STATUS_OBSOLETE
			&& !SHOW_DEPR_OBSOLETE))
			continue;
		    printInformationNode(smiNode, x, y, miNr);
		}
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	    *x -= TABLEBOTTOMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printModuleCompliance(int modc, SmiModule **modv,
				  float *x, float *y, int *miNr, int mCompl[])
{
    int         i, j;
    SmiNode     *smiNode;
    int         statusOrder[5] = {
		    SMI_STATUS_CURRENT,
		    SMI_STATUS_MANDATORY,
		    SMI_STATUS_OPTIONAL,
		    SMI_STATUS_DEPRECATED,
		    SMI_STATUS_OBSOLETE
		};

    printf(" <g id=\"MI%i\" transform=\"translate(%.2f,%.2f)\">\n",
								*miNr, *x, *y);
    printf("  <text>\n");
    if (!STATIC_OUTPUT) {
	printf("   <tspan style=\"text-anchor:middle\"");
	printf(" onclick=\"collapse(evt)\">--</tspan>\n");
    }
    printf("   <tspan x=\"5\">Compliance Statements</tspan>\n");
    printf("  </text>\n");
    printf(" </g>\n");
    (*miNr)++;
    *y += TABLEELEMHEIGHT;
    for (i = 0; i < modc; i++) {
	if (!mCompl[i])
	    continue;
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
	    *x += 2*TABLEELEMHEIGHT;
	    for (j=0; j<5; j++) {
		for (smiNode = smiGetFirstNode(modv[i],
						    SMI_NODEKIND_COMPLIANCE);
		    smiNode;
		    smiNode = smiGetNextNode(smiNode,
						    SMI_NODEKIND_COMPLIANCE)) {
		    if (smiNode->status != statusOrder[j])
			continue;
		    if ((smiNode->status == SMI_STATUS_DEPRECATED
			&& !SHOW_DEPRECATED && !SHOW_DEPR_OBSOLETE)
			|| (smiNode->status == SMI_STATUS_OBSOLETE
			&& !SHOW_DEPR_OBSOLETE))
			continue;
		    printComplianceNode(smiNode, modc, modv, x, y, miNr, i);
		}
	    }
	    *x -= 2*TABLEELEMHEIGHT;
	}
    }
    *y += TABLEELEMHEIGHT;
}

static void printModuleInformation(int modc, SmiModule **modv,
				   float x, float y,
				   int nType[], int oGroup[],
				   int nGroup[], int mCompl[])
{
    int i, miNr = 0;
    int nTypePrint = 0, oGroupPrint = 0, nGroupPrint = 0, mComplPrint = 0;

    printf(" <g transform=\"translate(%.2f,%.2f) scale(%.2f)\">\n",
							x, y, STARTSCALE);
    //now use x and y as relative coordinates.
    x = 0;
    y = 10;

    //only print sections containig information
    for (i = 0; i < modc; i++) {
	nTypePrint |= nType[i];
	oGroupPrint |= oGroup[i];
	nGroupPrint |= nGroup[i];
	mComplPrint |= mCompl[i];
    }

    printModuleIdentity(modc, modv, &x, &y, &miNr);
    if (nTypePrint)
	printNotificationType(modc, modv, &x, &y, &miNr, nType);
    if (oGroupPrint)
	printObjectGroup(modc, modv, &x, &y, &miNr, oGroup);
    if (nGroupPrint)
	printNotificationGroup(modc, modv, &x, &y, &miNr, nGroup);
    if (mComplPrint)
	printModuleCompliance(modc, modv, &x, &y, &miNr, mCompl);

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
    //float area, aspectratio, k, c = 0.8, xDelta, yDelta, absDelta, absDisp, t;
    //float x2, y2 = 1, dist;
    float aspectratio, k, xDelta, yDelta, absDelta, absDisp, t, dist;
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
		    if ((dist = intersect(vNode, eEdge))) {
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
    GraphNode    *tNode, *lastNode = NULL;
    GraphEdge    *tEdge;
    GraphCluster *tCluster;
    int          group, nodecount=0, classNr=0, singleNodes=1, miCount=0;
    float        x=10, xMin=0, yMin=0, xMax=0, yMax=0, maxHeight=0;
    int          nType[modc], oGroup[modc], nGroup[modc], mCompl[modc];

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
    //enlarge canvas for ModuleInformation
    xMax += MODULE_INFO_WIDTH;

    //count entries in the ModuleInformation-Section
    calcMiCount(modc, modv, &miCount, nType, oGroup, nGroup, mCompl);

    //output of svg to stdout begins here
    printSVGHeaderAndTitle(modc, modv, nodecount, miCount,
							xMin, yMin, xMax, yMax);

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
	    printSVGObject(tNode, &classNr, modc, modv);
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
	    printSVGObject(tNode, &classNr, modc, modv);
	} else {
	    printSVGGroup(tNode->group, &classNr, modc, modv);
	}
    }

    //print MODULE-IDENTITY
    printModuleInformation(modc, modv, xMax-MODULE_INFO_WIDTH, yMin+10,
						nType, oGroup, nGroup, mCompl);

    //output of svg to stdout ends here
    printSVGClose(xMin, yMin, xMax, yMax);
}



static void buildLink(int modc, SmiModule **modv)
{
    size_t length;
    const char *url = URL;
    //note: first string, so no &amp; required
    const char *widthstr = "width=";
    const char *heightstr = "&amp;height=";
    const char *deprstr = "&amp;deprobs=deprecated";
    const char *deprobsstr = "&amp;deprobs=obsolete";
    char width[15];
    char height[15];

    length = strlen(url);
    sprintf(width, "%i", CANVASWIDTH);
    sprintf(height, "%i", CANVASHEIGHT);
    length += strlen(widthstr) + strlen(width);
    length += strlen(heightstr) + strlen(height);
    if (SHOW_DEPRECATED) {
	length += strlen(deprstr);
    }
    if (SHOW_DEPR_OBSOLETE) {
	length += strlen(deprobsstr);
    }
    link = xmalloc(length);
    strcpy(link, url);
    strcat(link, widthstr);
    strcat(link, width);
    strcat(link, heightstr);
    strcat(link, height);
    if (SHOW_DEPRECATED) {
	strcat(link, deprstr);
    }
    if (SHOW_DEPR_OBSOLETE) {
	strcat(link, deprobsstr);
    }
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

    buildLink(modc, modv);

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

    xfree(link);
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
