/*
 * scanner.h --
 *
 *      Variables and functions needed by all scanner backends.
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
 * @(#) $Id: scanner.h,v 1.1 1999/03/31 17:24:29 strauss Exp $
 */

#include "defs.h"    
#include "error.h"



#define MAX_LEX_DEPTH		 20



extern char            *currentString;
extern int	       currentStringLength;
extern int	       currentStringMaxLength;
extern int	       currentStringColumn;
extern int	       currentStringFetchLine; 
extern int	       lexDepth;
extern YY_BUFFER_STATE yybuffer[MAX_LEX_DEPTH];
 


#define thisParser (*(Parser *)parser)



/* extern void yyerror(const char *msg); */



extern void trackInput(int nl, Module *parser);
			        /* XXX ??? */
