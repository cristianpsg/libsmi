/*
 * smidiff.c --
 *
 *      ...
 *
 * Copyright (c) 2001 ...
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smidiff.c,v 1.1 2001/08/22 17:51:46 strauss Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <smi.h>

#ifdef _DEBUG
#define SMI_DIFF_DBG_MSG( a, b ) smiDiffDbgMsg( ( a ), ( b ) )
#define SMI_DIFF_DBG_ENABLE_TAG( a ) smiDiffEnableTag( a )

#define SMI_DIFF_MAXTAGS 10
static char *tags[SMI_DIFF_MAXTAGS];
static int tagsize = 0;

#define SMI_DIFF_DBG_TAG_ACTIVE   1
#define SMI_DIFF_DBG_TAG_INACTIVE 0

static int active( const char *tag )
{
  int i;
  for( i = 0; i < tagsize; i++ ) {
    if( !strcmp( tag, tags[i] ) ) 
      return SMI_DIFF_DBG_TAG_ACTIVE;
  }
  return SMI_DIFF_DBG_TAG_INACTIVE;
}

static void smiDiffEnableTag( const char *tag )
{
  if( ! tag )
    return;
  
  if( tagsize < SMI_DIFF_MAXTAGS ) {
    tags[tagsize++] = strdup( tag );
  }
} 

static void smiDiffDbgMsg( const char *tag, const char *msg )
{
  if( active( tag ) == SMI_DIFF_DBG_TAG_ACTIVE )
    printf( "%s: %s\n", tag, msg );
}
#else if /* No debugging, please */
#define SMI_DIFF_DBG_MSG
#define SMI_DIFF_DBG_ENABLE_TAG
#endif /* _DEBUG */ 


char *oidToStr( unsigned int oidlen, SmiSubid *oid )
{
  int i;
  char buf[128];
  char *ret = (char *)malloc( oidlen * 4 );
  
  sprintf( buf, "%u", oid[0] ); 
  strcat( ret, buf );
  for( i = 1; i < oidlen; i++) {
    sprintf( buf, ".%u",oid[i] );
    strcat( ret, buf );
  }
  SMI_DIFF_DBG_MSG( "ots", ret );

  return ret;
}

#ifndef MAX
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif /* ifndef MAX */

#define SMI_DIFF_ERROR       -1
#define SMI_NO_DIFF           0
#define SMI_NAME_CHANGED      1
#define SMI_OID_LEN_CHANGED   2
#define SMI_OID_CHANGED       4
#define SMI_DECL_CHANGED      8
#define SMI_ACCESS_CHANGED   16
#define SMI_STATUS_CHANGED   32
#define SMI_FORMAT_CHANGED   64
#define SMI_VALUE_CHANGED   128
#define SMI_UNITS_CHANGED   256
#define SMI_DESCR_CHANGED   512

int nodecmp( SmiNode *oldNode, SmiNode *newNode )
{
  char dbgstr[100];

  if( ! ( oldNode && newNode ) )
    return SMI_DIFF_ERROR;

  /* compare names */
  if( strcmp( oldNode->name, newNode->name ) ) {
    /* TBD: dump a message */
    sprintf( dbgstr, "names differ: expected %s and found %s",
	     oldNode->name, newNode->name );
    SMI_DIFF_DBG_MSG( "cmp", dbgstr );
    
    /* O.K., names are different. If the oid is differnt as well,
       probably we are in a complete different node. */
    if( oldNode->oidlen != newNode->oidlen || 
	memcmp( oldNode->oid, newNode->oid, MAX( oldNode->oidlen,
						 newNode->oidlen ) ) ) 
	return SMI_OID_CHANGED && SMI_NAME_CHANGED;
    
    return SMI_NAME_CHANGED;
  }
  
  /* compare oids */
  if( oldNode->oidlen != newNode->oidlen || 
	memcmp( oldNode->oid, newNode->oid, MAX( oldNode->oidlen,
						 newNode->oidlen ) ) ) 
    return SMI_OID_CHANGED;
  
  /* compare decl */
  if( oldNode->decl != newNode->decl )
    return SMI_DECL_CHANGED;

  /* compare access */
  if( oldNode->access != newNode->access )
    return SMI_ACCESS_CHANGED;

  /* compare status */
  if( oldNode->status != newNode->status ) {
    switch( oldNode->status ) {
    case SMI_STATUS_CURRENT :
      if( newNode->status == SMI_STATUS_DEPRECATED ) {
	sprintf(dbgstr,
		"Legal Change: Staus changed from current to deprecated in %s",
		oldNode->name);
	SMI_DIFF_DBG_MSG( "cmp", dbgstr );
      }
      else if( newNode->status == SMI_STATUS_OBSOLETE ) {
	sprintf( dbgstr,
		 "Legal Change: Staus changed from current to obsolete in %s",
		 oldNode->name );
	SMI_DIFF_DBG_MSG("cmp", dbgstr );
      }
      break;
    case SMI_STATUS_DEPRECATED :
      if( newNode->status == SMI_STATUS_OBSOLETE ) {
	sprintf(dbgstr,
		"Legal Change: \
Staus changed from deprecated to obsolete in %s",
		oldNode->name);
	SMI_DIFF_DBG_MSG("cmp", dbgstr );
      }
      break;
    default : return SMI_STATUS_CHANGED;
    }
  }
  
  /* TBD: compare format */
  
  /* TBD: compare value */

  /* compare units */
  if( newNode->units ) {
    if( oldNode->units ) {
      if( strcmp( oldNode->units, newNode->units ) )
	return SMI_UNITS_CHANGED;
    }
    else {
      sprintf( dbgstr, "Legal Change: unit %s added to %s", 
	       newNode->units, newNode->name );
      SMI_DIFF_DBG_MSG( "cmp", dbgstr );
    }
  }
  else
    if( oldNode->units )
      return SMI_UNITS_CHANGED;

  /* compare description */
  if( ! newNode->description ) {
    if( oldNode->description ) 
      return SMI_DESCR_CHANGED; 
  }
  else if( strcmp( oldNode->description, newNode->description ) ) {
    SMI_DIFF_DBG_MSG( "cmp", "Legal Change: description altered");
    SMI_DIFF_DBG_MSG( "cmp", "Old description:");
    SMI_DIFF_DBG_MSG( "cmp", oldNode->description );
    SMI_DIFF_DBG_MSG( "cmp", "New description:");
    SMI_DIFF_DBG_MSG( "cmp", newNode->description );
    SMI_DIFF_DBG_MSG( "cmp", "Change is legal if it is a clarification" );
  }
    
  
  /* no differences found */
  return SMI_NO_DIFF;
}

int main(int argc, char *argv[])
{
    SmiModule *oldModule, *newModule;
    SmiNode *node;
    int oldCount, newCount;

    oldCount = newCount = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: smidiff oldmodule newmodule\n");
        exit(1);
    }

    /* enable some debugging tags */
    /*SMI_DIFF_DBG_ENABLE_TAG( "nodes" );*/
      SMI_DIFF_DBG_ENABLE_TAG( "cmp" ); 
    SMI_DIFF_DBG_ENABLE_TAG( "ots" );
    SMI_DIFF_DBG_ENABLE_TAG( "fmt" );
    /*    SMI_DIFF_DBG_ENABLE_TAG( "val" ); */
 
    /* the `:' separates the view identifier */
    smiInit("smidiff:old");
    oldModule = smiGetModule(smiLoadModule(argv[1]));
 
    smiInit("smidiff:new");
    newModule = smiGetModule(smiLoadModule(argv[2]));
 
    /* to switch a specific view, simply call smiInit() again */
    smiInit("smidiff:old");
    for(node = smiGetFirstNode(oldModule, SMI_NODEKIND_ANY);
	node;
        node = smiGetNextNode(node, SMI_NODEKIND_ANY)) {
      SmiNode *newnode;

      char dbgstr[100] ;
      
      /* try to find node with same name in the new module */

 
      sprintf( dbgstr, "Searching for node number %d (%s)...", 
	       oldCount, node->name );
      SMI_DIFF_DBG_MSG( "nodes", dbgstr );
      if( node->format ) {
	sprintf( dbgstr, "node %s has format %s", node->name, node->format );
	SMI_DIFF_DBG_MSG( "fmt", dbgstr );
      }
      sprintf( dbgstr, "%s has value (basetype is %d)", 
	       node->name, node->value.basetype );
      SMI_DIFF_DBG_MSG( "val", dbgstr );

      /* switch to the new module */
      smiInit( "smidiff:new" );
      
      newnode = smiGetNodeByOID( node->oidlen, node->oid );

      if( ! newnode ) 
	fprintf( stderr, 
		 "node %s not found in new module. Perhaps oid changed.\n",  
		 node->name );
      else {
	  int cmpres;
	sprintf( dbgstr, "... found %s", newnode->name );
	SMI_DIFF_DBG_MSG( "nodes", dbgstr );

	/* compare nodes */
	cmpres = nodecmp( node, newnode );
	switch( cmpres ) {
	case SMI_NO_DIFF : break;
	case ( SMI_NAME_CHANGED && SMI_OID_CHANGED ) :
	    /* in this case probably a node as been moved, so
	       let us try to find a node with a same name */
	    newnode = smiGetNode( newModule, node->name );
	    if( ! newnode )
		fprintf( stderr, 
			 "ERROR: Node %s has been deleted\n", node->name );
	    else
	      
	      fprintf( stderr,
			 "ERROR: Node %s has been moved from %s to %s\n",
		       node->name, oidToStr( node->oidlen, node->oid ), 
		       oidToStr( newnode->oidlen, newnode->oid ) );
	    
	    break;
	default: fprintf( stderr,
			  "ERROR: Node %s changed\n", node->name );
	}
      }
      oldCount++;
      
      /* switch back to old module */
      smiInit( "smidiff:old" );
    };

    smiInit("smidiff:new");
    for(node = smiGetFirstNode(newModule, SMI_NODEKIND_ANY);
	node;
        node = smiGetNextNode(node, SMI_NODEKIND_ANY)) {
	newCount++;
    };

    printf("The old module has %d nodes.\n", oldCount);
    printf("The new module has %d nodes.\n", newCount);

    smiInit("smidiff:old");
    smiExit();

    smiInit("smidiff:new");
    smiExit();

    return 0;
}

