/*
 * scanner.c --
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
 * @(#) $Id: scanner.c,v 1.2 1999/04/01 21:57:13 strauss Exp $
 */

#if defined(BACKEND_SMI) || defined(BACKEND_SMING)

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "defs.h"    
#include "error.h"



#define thisParser      (*(Parser *)parser)



char *currentString = NULL;
int currentStringLength = 0;
int currentStringMaxLength = 0;
int currentStringColumn = 0;
int currentStringFetchLine = 0; 

int lexDepth = 0;



#endif

