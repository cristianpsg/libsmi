/*
 * table.c --
 *
 *      Datatype to store contents of a mib table- 
 *
 * Copyright (c) 2002 T. Klie, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#include <malloc.h>
#include "table.h"

struct MDTable
*md_createTable( char *name, oid *prefix, int prefix_len )
{
    struct MDTable *ret = (struct MDTable *)malloc( sizeof( struct MDTable ) );
    if( ret ) {
	ret->name = name;
	ret->prefix = prefix;
	ret->prefix_len = prefix_len;
	ret->rows = NULL;
    }
    return ret;
}

MDRow
*md_addRow( MDTable *table, oid *index, int index_len )
{
    MDRow *iterRow = NULL, *oldIterRow = NULL;

    /* find last row in list */
    for( iterRow = table->rows; iterRow; iterRow = iterRow->next ) {
	if(! memcmp( iterRow->index, index,
		      MIN( index_len, iterRow->index_len ) * sizeof( oid ) ) ){
	    /* row already exists */
	    return iterRow; /* xxx should we give a warning? */
	}
	oldIterRow = iterRow;
    }

    /* create new row */
    iterRow = (struct MDRow *)malloc( sizeof( struct MDRow ) );
    if( !iterRow ) {
	fprintf( stderr, "Warning! Malloc failed in table.c\n" );
	return NULL;
    }

    /* fill in values */
    iterRow->index = index;
    iterRow->index_len = index_len;
    iterRow->columns = NULL;
    iterRow->next = NULL;

    /* connect row to list */
    if( oldIterRow ) {
	/* table already has rows */
	oldIterRow->next = iterRow;
    }
    else {
	/* table has no rows yet */
	table->rows = iterRow;
    }

    return iterRow;
}


MDRow
*md_findRow( MDTable *table, oid *index, int index_len )
{
    MDRow *iterRow = NULL;

    if( ! table ) {
	return NULL;
    }

    for( iterRow = table->rows; iterRow; iterRow = iterRow->next ) {
	if( ! memcmp( iterRow->index, index,
		    MIN( iterRow->index_len, index_len ) * sizeof( oid ) ) ) {
	    return iterRow;
	}
    }
    
    return NULL;
}


void
md_addColumn( MDRow *row, oid col, char *value )
{
    MDCol *iterCol, *oldIterCol = NULL;

    /* find las column in list */
    for( iterCol = row->columns; iterCol; iterCol = iterCol->next ) {
	if( col == iterCol->col ) {
	    /* column already exists */
	    return; /* xxx a warning? */
	}
	oldIterCol = iterCol;
    }
    
    /* create new column */
    iterCol = (struct MDCol *)malloc( sizeof( struct MDCol ) );
    if( ! iterCol ) {
	fprintf( stderr, "Warning! Malloc failed in table.c!\n" );
	return;
    }
    
    /* fill in values */
    iterCol->col = col;
    iterCol->value = value;
    iterCol->next = NULL;

    /* connect column to list */
    if( oldIterCol ) {
	/* row already has columns */
	oldIterCol->next = iterCol;
    }
    else {
	/* row has no columns yet */
	row->columns = iterCol;
    }
}

MDCol
*md_findColumn( MDRow *row, oid col )
{
    MDCol *iterCol;

    for( iterCol = row->columns; iterCol; iterCol = iterCol->next ) {
	if( iterCol->col == col ) {
	    return iterCol;
	}
    }
    return NULL;
}

void
md_dropTable( MDTable *table )
{
    MDRow *iterRow, *oldIterRow = NULL;
    MDCol *iterCol, *oldIterCol = NULL;

    for( iterRow = table->rows; iterRow; iterRow = iterRow->next ) {
	for( iterCol = iterRow->columns; iterCol; iterCol = iterCol->next ) {
	    if( oldIterCol ) {
		free( oldIterCol );
	    }
	    oldIterCol = iterCol;
	}
	if( oldIterRow ) {
	    free( oldIterRow );
	}
	oldIterRow = iterRow;
    }
    free( table );
}  
