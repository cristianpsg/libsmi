/*
 * table.h --
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

#ifndef _MIBDUMP_TABLE_H
#define _MIBDUMP_TABLE_H

#include <ucd-snmp/ucd-snmp-config.h>
#include <ucd-snmp/ucd-snmp-includes.h>

#ifndef MIN
#define MIN(a,b) ((a)) < ((b)) ? ((a)) : ((b))
#endif /* #ifndef MIN */


/* column list */
typedef struct MDCol {
    oid col;
    char *value;
    struct MDCol *next;
} MDCol;

/* row list */
typedef struct MDRow {
    oid *index;
    int index_len;
    MDCol *columns;
    struct MDRow *next;
} MDRow;

/* table */
typedef struct MDTable {
    oid *prefix;
    int prefix_len;
    char *name;
    MDRow *rows;
} MDTable;

/* table operations */
struct MDTable *md_createTable( char *name, oid *prefix, int prefix_len );
MDRow *md_addRow( MDTable *table, oid *index, int index_len );
MDRow *md_findRow( MDTable *table, oid *index, int index_len );
void md_addColumn( MDRow *row, oid col, char *value );
MDCol *md_findColumn( MDRow *row, oid col );
void md_dropTable( MDTable *table );

#endif /* #infdef _MIBDUMP_TABLE_H */
