/*
 * smisubstree.c --
 *
 *      A simple SMI example program, printing an OID's subtree.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @(#) $Id: smisubtree.c,v 1.3 1999/05/21 22:51:05 strauss Exp $
 */

#include <stdio.h>
#include <string.h>

#include "smi.h"

int
main(argc, argv)
    int argc;
    char *argv[];
{
    char *modulename, *fullname;
    SmiNode *smiNode, *smiStartNode;
    
    if (argc != 2) {
	fprintf(stderr, "Usage: smisubtree oid\n");
	exit(1);
    }

    fullname   = argv[1];
    modulename = smiModule(fullname);
    
    smiInit();

    smiLoadModule(modulename);

    for(smiNode = smiStartNode = smiGetNode(fullname, "");
	smiNode && strstr(smiNode->oid, smiStartNode->oid) == smiNode->oid;
	smiNode = smiGetNextNode(modulename, smiNode->name)) {

	printf("%s::%-16s   [%s]\n",
	       smiNode->module, smiNode->name,
	       smiStringBasetype(smiNode->basetype));
	
    };
    
    exit(0);
}
