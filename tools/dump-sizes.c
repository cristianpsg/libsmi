/*
 * dump-sizes.c --
 *
 *      Operations to compute and dump SNMPv3 PDU sizes.
 *
 * Copyright (c) 2003 J. Schoenwaelder, International University Bremen.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: dump-sizes.c,v 1.1 2003/09/17 12:37:58 schoenw Exp $
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include "smi.h"
#include "smidump.h"

static int silent = 0;


static SmiInteger32
getAbsMinEnum(SmiType *smiType)
{
     SmiNamedNumber *nn;
     SmiInteger32 min = 2147483647;

     for (nn = smiGetFirstNamedNumber(smiType);
	  nn;
	  nn = smiGetNextNamedNumber(nn)) {
	  if (abs(nn->value.value.integer32) < min) {
	       min = abs(nn->value.value.integer32);
	  }
     }
     return min;
}



static SmiInteger32
getAbsMaxEnum(SmiType *smiType)
{
     SmiNamedNumber *nn;
     SmiInteger32 max = 0;

     for (nn = smiGetFirstNamedNumber(smiType);
	  nn;
	  nn = smiGetNextNamedNumber(nn)) {
	  if (abs(nn->value.value.integer32) > max) {
	       max = abs(nn->value.value.integer32);
	  }
     }
     return max;
}



static SmiInteger32
getAbsMinInteger32(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiInteger32 min = 2147483647;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getAbsMinInteger32(parent) : 0;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (abs(range->minValue.value.integer32) < min) {
	       min = abs(range->minValue.value.integer32);
	  }
     }
     return min;
}



static SmiInteger32
getAbsMaxInteger32(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiInteger32 max = 0;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getAbsMaxInteger32(parent) : 2147483647;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (abs(range->maxValue.value.integer32) > max) {
	       max = abs(range->maxValue.value.integer32);
	  }
     }
     return max;
}



static SmiUnsigned32
getMinUnsigned32(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiInteger32 min = 0xffffffff;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getMinUnsigned32(parent) : 0;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (range->minValue.value.unsigned32 < min) {
	       min = range->minValue.value.unsigned32;
	  }
     }
     return min;
}



static SmiUnsigned32
getMaxUnsigned32(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiUnsigned32 max = 0;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getMaxUnsigned32(parent) : 0xffffffff;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (range->maxValue.value.unsigned32 > max) {
	       max = range->maxValue.value.unsigned32;
	  }
     }
     return max;
}



static SmiInteger64
getAbsMinInteger64(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiInteger64 min = 9223372036854775808ULL;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getAbsMinInteger64(parent) : min;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (abs(range->minValue.value.integer64) < min) {
	       min = abs(range->minValue.value.integer64);
	  }
     }
     return min;
}



static SmiInteger64
getAbsMaxInteger64(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiInteger64 max = 0;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getAbsMaxInteger64(parent) : max;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (abs(range->maxValue.value.integer64) > max) {
	       max = abs(range->maxValue.value.integer64);
	  }
     }
     return max;
}



static SmiUnsigned64
getMinUnsigned64(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiInteger64 min = 0xffffffffffffffffLL;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getMinUnsigned64(parent) : min;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (range->minValue.value.unsigned64 < min) {
	       min = range->minValue.value.unsigned64;
	  }
     }
     return min;
}



static SmiUnsigned64
getMaxUnsigned64(SmiType *smiType)
{
     SmiType *parent;
     SmiRange *range;
     SmiUnsigned64 max = 0;

     range = smiGetFirstRange(smiType);
     if (! range) {
	  parent = smiGetParentType(smiType);
	  return parent ? getMaxUnsigned64(parent) : max;
     }

     for (; range; range = smiGetNextRange(range)) {
	  if (range->maxValue.value.unsigned64 > max) {
	       max = range->maxValue.value.unsigned64;
	  }
     }
     return max;
}



static unsigned int
getMinSize(SmiType *smiType)
{
    SmiRange *smiRange;
    SmiType  *parentType;
    unsigned int min = 65535, size;
    
    switch (smiType->basetype) {
    case SMI_BASETYPE_BITS:
	return 0;
    case SMI_BASETYPE_OCTETSTRING:
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	size = 0;
	break;
    default:
	return 0;
    }

    for (smiRange = smiGetFirstRange(smiType);
	 smiRange ; smiRange = smiGetNextRange(smiRange)) {
	if (smiRange->minValue.value.unsigned32 < min) {
	    min = smiRange->minValue.value.unsigned32;
	}
    }
    if (min < 65535 && min > size) {
	size = min;
    }

    parentType = smiGetParentType(smiType);
    if (parentType) {
	unsigned int psize = getMinSize(parentType);
	if (psize > size) {
	    size = psize;
	}
    }

    return size;
}



static unsigned int
getMaxSize(SmiType *smiType)
{
    SmiRange *smiRange;
    SmiType  *parentType;
    SmiNamedNumber *nn;
    unsigned int max = 0, size;
    
    switch (smiType->basetype) {
    case SMI_BASETYPE_BITS:
    case SMI_BASETYPE_OCTETSTRING:
	size = 65535;
	break;
    case SMI_BASETYPE_OBJECTIDENTIFIER:
	size = 128;
	break;
    default:
	return 0xffffffff;
    }

    if (smiType->basetype == SMI_BASETYPE_BITS) {
	for (nn = smiGetFirstNamedNumber(smiType);
	     nn;
	     nn = smiGetNextNamedNumber(nn)) {
	    if (nn->value.value.unsigned32 > max) {
		max = nn->value.value.unsigned32;
	    }
	}
	size = (max / 8) + 1;
	return size;
    }

    for (smiRange = smiGetFirstRange(smiType);
	 smiRange ; smiRange = smiGetNextRange(smiRange)) {
	if (smiRange->maxValue.value.unsigned32 > max) {
	    max = smiRange->maxValue.value.unsigned32;
	}
    }
    if (max > 0 && max < size) {
	size = max;
    }

    parentType = smiGetParentType(smiType);
    if (parentType) {
	unsigned int psize = getMaxSize(parentType);
	if (psize < size) {
	    size = psize;
	}
    }

    return size;
}



static int
ber_len_length(int length)
{
     int len;
     
     if (length < 0x80) {
	  return 1;
     }

     for (len = 0; length > 0; len++) {
	  length >>= 8;
     }
     return len;
}



static int
ber_len_subid(SmiSubid subid)
{
     int len = 0;

     do {
	  subid >>= 7;
	  len++;
     } while (subid > 0);
     return len;
}



static int
ber_len_oid(const SmiSubid *oid, unsigned int oidlen)
{
     int len = 0;
     int i;
     
     len += ber_len_subid(oid[1] + oid[0] * 40);
     for (i = 2; i < oidlen; i++) {
	  len += ber_len_subid(oid[i]);
     }

     len += ber_len_length(len);	/* length */
     len += 1;				/* tag */
     return len;
}



static int
ber_len_int32(const SmiInteger32 value)
{
     SmiInteger32 val = value;
     unsigned char ch, sign;
     int lim;
     int len = 0;
     
     if (val < 0) {
	  lim  = -1;
	  sign = 0x80;
     } else {
	  lim  = 0;
	  sign = 0x00;
     }
     do {
	  ch = (unsigned char) val;
	  val >>= 8;
	  len++;
     } while ((val != lim) || (unsigned char) (ch & 0x80) != sign);

     len += ber_len_length(len);	/* length */
     len += 1;				/* tag */
     return len;
}



static int
ber_len_uint32(const SmiUnsigned32 value)
{
     SmiUnsigned32 val = value;
     unsigned char ch;
     int len = 0;
     
     do {
	  ch = (unsigned char) val;
	  val >>= 8;
	  len++;
     } while ((val != 0) || (ch & 0x80) != 0x00);

     len += ber_len_length(len);	/* length */
     len += 1;				/* tag */
     return len;
}



static int
ber_len_int64(const SmiInteger64 value)
{
     SmiInteger64 val = value;
     unsigned char ch, sign;
     int lim;
     int len = 0;
     
     if (val < 0) {
	  lim  = -1;
	  sign = 0x80;
     } else {
	  lim  = 0;
	  sign = 0x00;
     }
     do {
	  ch = (unsigned char) val;
	  val >>= 8;
	  len++;
     } while ((val != lim) || (unsigned char) (ch & 0x80) != sign);

     len += ber_len_length(len);	/* length */
     len += 1;				/* tag */
     return len;
}



static int
ber_len_uint64(const SmiUnsigned64 value)
{
     SmiUnsigned64 val = value;
     unsigned char ch;
     int len = 0;
     
     do {
	  ch = (unsigned char) val;
	  val >>= 8;
	  len++;
     } while ((val != 0) || (ch & 0x80) != 0x00);

     len += ber_len_length(len);	/* length */
     len += 1;				/* tag */
     return len;
}



static int
ber_len_val(SmiType *smiType, int flags)
{
     int len = 0;
     SmiInteger32  int32;
     SmiUnsigned32 uint32;
     SmiInteger64  int64;
     SmiUnsigned64 uint64;
     SmiSubid      oid[128];
     unsigned int  oidlen = sizeof(oid)/sizeof(oid[0]);
     
     switch (smiType->basetype) {
     case SMI_BASETYPE_OBJECTIDENTIFIER:
	  if (flags) {
	       int i;

	       oid[0] = 2;
	       for (i = 1; i < 128; i++) {
		    oid[i] = 4294967295UL;
	       }
	  } else {
	       oid[0] = oid[1] = 0, oidlen = 2;
	  }
	  len = ber_len_oid(oid, oidlen);
	  break;
     case SMI_BASETYPE_OCTETSTRING:
     case SMI_BASETYPE_BITS:
	  if (flags) {
	       len = getMaxSize(smiType);
	  } else {
	       len = getMinSize(smiType);
	  }
	  break;
     case SMI_BASETYPE_ENUM:
	  if (flags) {
	       int32 = getAbsMaxEnum(smiType);
	  } else {
	       int32 = getAbsMinEnum(smiType);
	  }
	  len = ber_len_int32(int32);
	  break;
     case SMI_BASETYPE_INTEGER32:
	  if (flags) {
	       int32 = getAbsMaxInteger32(smiType);
	  } else {
	       int32 = getAbsMinInteger32(smiType);
	  }
	  len = ber_len_int32(int32);
	  break;
     case SMI_BASETYPE_UNSIGNED32:
	  if (flags) {
	       uint32 = getMaxUnsigned32(smiType);
	  } else {
	       uint32 = getMinUnsigned32(smiType);
	  }
	  len = ber_len_uint32(uint32);
	  break;
     case SMI_BASETYPE_INTEGER64:
	  if (flags) {
	       int64 = getAbsMaxInteger64(smiType);
	  } else {
	       int64 = getAbsMinInteger64(smiType);
	  }
	  len = ber_len_int64(int64);
	  break;
     case SMI_BASETYPE_UNSIGNED64:
	  if (flags) {
	       uint64 = getMaxUnsigned64(smiType);
	  } else {
	       uint64 = getMinUnsigned64(smiType);
	  }
	  len = ber_len_uint64(uint64);
	  break;
     default:
	  break;
     }

     return len;
}



static void
append_index(SmiSubid *oid, unsigned int *oidlen,
	     SmiNode *indexNode, int flags)
{
     SmiInteger32  int32;
     SmiUnsigned32 uint32;
     SmiType *indexType;
     int i, len;

     if (! indexNode) return;

     indexType = smiGetNodeType(indexNode);
     if (! indexType) return;

     switch (indexType->basetype) {
     case SMI_BASETYPE_OBJECTIDENTIFIER:
	  fprintf(stderr, "*** xxx object identifiers in the index are not supported ***\n");
#if 0
	  if (flags) {
	       int i;

	       oid[0] = 2;
	       for (i = 1; i < 128; i++) {
		    oid[i] = 4294967295UL;
	       }
	  } else {
	       oid[0] = oid[1] = 0, oidlen = 2;
	  }
	  len = ber_len_oid(oid, oidlen);
#endif
	  break;
     case SMI_BASETYPE_OCTETSTRING:
     case SMI_BASETYPE_BITS:
	  if (flags) {
	       len = getMaxSize(indexType);
	  } else {
	       len = getMinSize(indexType);
	  }
	  if (! indexNode->implied) {
	       oid[(*oidlen)++] = len;
	  }
	  for (i = 0; i < len && *oidlen < 128; i++) {
	       oid[(*oidlen)++] = flags ? 255 : 0;
	  }
	  break;
     case SMI_BASETYPE_ENUM:
	  if (flags) {
	       int32 = getAbsMaxEnum(indexType);
	  } else {
	       int32 = getAbsMinEnum(indexType);
	  }
	  oid[(*oidlen)++] = int32;
	  break;
     case SMI_BASETYPE_INTEGER32:
	  if (flags) {
	       int32 = getAbsMaxInteger32(indexType);
	  } else {
	       int32 = getAbsMinInteger32(indexType);
	  }
	  oid[(*oidlen)++] = int32;
	  break;
     case SMI_BASETYPE_UNSIGNED32:
	  if (flags) {
	       uint32 = getMaxUnsigned32(indexType);
	  } else {
	       uint32 = getMinUnsigned32(indexType);
	  }
	  oid[(*oidlen)++] = uint32;
	  break;
     }
}

#undef DUMP_OID

static int
ber_len_varbind(SmiNode *smiNode, int flags)
{
     SmiNode *row;
     SmiSubid oid[128];
     unsigned int oidlen = sizeof(oid)/sizeof(oid[0]);
     int len = 0;
#ifdef DUMP_OID
     int x;
#endif

     switch (smiNode->nodekind) {
     case SMI_NODEKIND_SCALAR:
	  for (oidlen = 0; oidlen < smiNode->oidlen; oidlen++) {
	       oid[oidlen] = smiNode->oid[oidlen];
	  }
	  oid[oidlen++] = 0;
	  break;
     case SMI_NODEKIND_COLUMN:
	  for (oidlen = 0; oidlen < smiNode->oidlen; oidlen++) {
	       oid[oidlen] = smiNode->oid[oidlen];
	  }
	  row = smiGetParentNode(smiNode);
	  if (row) {
	       SmiNode *indexNode = NULL, *iNode;
	       SmiElement *smiElement;
	       
	       switch (row->indexkind) {
	       case SMI_INDEX_INDEX:
	       case SMI_INDEX_REORDER:
		    indexNode = row;
		    break;
	       case SMI_INDEX_EXPAND: /* TODO: we have to do more work here! */
		    break;
	       case SMI_INDEX_AUGMENT:
	       case SMI_INDEX_SPARSE:
		    indexNode = smiGetRelatedNode(row);
		    break;
	       case SMI_INDEX_UNKNOWN:
		    break;
	       }
	       if (indexNode) {
		    for (smiElement = smiGetFirstElement(indexNode);
			 smiElement;
			 smiElement = smiGetNextElement(smiElement)) {
			 iNode = smiGetElementNode(smiElement);
			 append_index(oid, &oidlen, iNode, flags);
		    }
	       }
	  }
	  break;
     default:
	  return 0;
     }

#ifdef DUMP_OID
     fprintf(stderr, "%-32s\t", smiNode->name);
     for (x = 0; x < oidlen; x++) {
	  fprintf(stderr, ".%u", oid[x]);
     }
     fprintf(stderr, "\n");
#endif


     len += ber_len_oid(oid, oidlen);
     len += ber_len_val(smiGetNodeType(smiNode), flags);
     len += ber_len_length(len) + 1;

     return len;
}



static int
isGroup(SmiNode *smiNode)
{
    SmiNode *childNode;

    if (smiNode->nodekind == SMI_NODEKIND_ROW) {
	return 1;
    }
    
    for (childNode = smiGetFirstChildNode(smiNode);
	 childNode;
	 childNode = smiGetNextChildNode(childNode)) {
	if (childNode->nodekind == SMI_NODEKIND_SCALAR) {
	    return 1;
	}
    }

    return 0;
}



static void
dumpSizeOfPDU(FILE *f, SmiNode *smiNode)
{
     SmiNode *child;
     int worst = 0;
     int best = 0;
     int b, w, n = 0;
     
     fprintf(f, "%s {\n", smiNode->name);
     
     for (child = smiGetFirstChildNode(smiNode);
	  child;
	  child = smiGetNextChildNode(child)) {
	  if (child->access == SMI_ACCESS_READ_WRITE
	      || child->access == SMI_ACCESS_READ_ONLY) {

	       b = ber_len_varbind(child, 0);
	       w = ber_len_varbind(child, 1);

	       fprintf(f, "  %-32s\t[%d..%d]\n", child->name, b, w);
	       
	       best += b, worst += w, n++;
	  }
     }

     /* varbind list sequence length and tag */
     best  += ber_len_length(best)  + 1;
     worst += ber_len_length(worst) + 1;

     /* request-id as defined in RFC 3416 */
     best += ber_len_int32(0);
     worst += ber_len_int32(-214783648);
     
     /* error-status as defined in RFC 3416 */
     best += ber_len_int32(0);
     worst += ber_len_int32(18);
     
     /* error-index as defined in RFC 3416 */
     best += ber_len_int32(0);
     worst += ber_len_int32(n-1);

     /* PDU sequence length and tag */
     best  += ber_len_length(best)  + 1;
     worst += ber_len_length(worst) + 1;
     
     fprintf(f, "} [%d..%d]\n\n", best, worst);
}



static void
dumpSizeOfCreatePDU(FILE *f, SmiNode *smiNode, int ignoreDefaultColumns)
{
     SmiNode *child;
     SmiType *childType;
     SmiModule *childTypeModule;
     
     int worst = 0;
     int best = 0;
     int b, w, n = 0;
     int isRowStatus;
     
     fprintf(f, "%s {\n", smiNode->name);
     
     for (child = smiGetFirstChildNode(smiNode);
	  child;
	  child = smiGetNextChildNode(child)) {
	  if (child->access == SMI_ACCESS_READ_WRITE) {

	       /* Ensure RowStatus columns are present even if they
		* have a default value. */

	       childType = smiGetNodeType(child);
	       childTypeModule = childType
		    ? smiGetTypeModule(childType) : NULL;
	       
	       isRowStatus
		    = (childType && childType->name
		       && childTypeModule && childTypeModule->name)
		    ? (strcmp(childType->name, "RowStatus") == 0
		       && strcmp(childTypeModule->name, "SNMPv2-TC") == 0)
		    : 0;

	       /* xxx at least one PDU must be present xxx */

	       if (ignoreDefaultColumns
		   && child->value.basetype == SMI_BASETYPE_UNKNOWN
		   && !isRowStatus) {
		    continue;
	       }

	       b = ber_len_varbind(child, 0);
	       w = ber_len_varbind(child, 1);

	       fprintf(f, "  %-32s\t[%d..%d]\n", child->name, b, w);
	       
	       best += b, worst += w, n++;
	  }
     }

     /* varbind list sequence length and tag */
     best  += ber_len_length(best)  + 1;
     worst += ber_len_length(worst) + 1;

     /* request-id as defined in RFC 3416 */
     best += ber_len_int32(0);
     worst += ber_len_int32(-214783648);
     
     /* error-status as defined in RFC 3416 */
     best += ber_len_int32(0);
     worst += ber_len_int32(18);
     
     /* error-index as defined in RFC 3416 */
     best += ber_len_int32(0);
     worst += ber_len_int32(n-1);

     /* PDU sequence length and tag */
     best  += ber_len_length(best)  + 1;
     worst += ber_len_length(worst) + 1;
     
     fprintf(f, "} [%d..%d]\n\n", best, worst);
}



static void
dumpSizeOfNotificationPDU(FILE *f, SmiNode *smiNode)
{
     SmiElement *smiElement;
     SmiNode *varNode;
     int worst = 0;
     int best = 0;
     int w, b;
     int len = 0;
     static const SmiSubid snmpTrapOid0[]
	  = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };
     static const int snmpTrapOid0Len
	  = sizeof(snmpTrapOid0) / sizeof(SmiSubid);
     
     fprintf(f, "%s {\n", smiNode->name);

     b = 15, w = 19;
     fprintf(f, "  %-32s\t[%d..%d]\n", "sysUpTime", b, w);
     best += b, worst += w;
     
     len += ber_len_oid(smiNode->oid, smiNode->oidlen);
     len += ber_len_oid(snmpTrapOid0, snmpTrapOid0Len);
     len += ber_len_length(len) + 1;
     b = len, w = len;
     fprintf(f, "  %-32s\t[%d..%d]\n", "snmpTrapOID", b, w);
     best += b, worst += w;
     
     for (smiElement = smiGetFirstElement(smiNode);
	  smiElement;
	  smiElement = smiGetNextElement(smiElement)) {
	  varNode = smiGetElementNode(smiElement);
	  if (! varNode) continue;

	  b = ber_len_varbind(varNode, 0);
	  w = ber_len_varbind(varNode, 1);

	  fprintf(f, "  %-32s\t[%d..%d]\n", varNode->name, b, w);
	  
	  best += b, worst += w;
     }

     best  += ber_len_length(best)  + 1;
     worst += ber_len_length(worst) + 1;
     
     fprintf(f, "} [%d..%d]\n\n", best, worst);
}



static void
gaga(FILE *f, SmiModule *smiModule)
{
     SmiNode *smiNode;
     int c;

     /* objects */
     
     for (c = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ANY);
	  smiNode;
	  smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ANY), c++) {
	  if (isGroup(smiNode)) {
	       dumpSizeOfPDU(f, smiNode);
	  }
     }

     for (c = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ROW);
	  smiNode;
	  smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ROW)) {
	  if (smiNode->create) {
	       if (c == 0) {
		    fprintf(f, "# size of full one-shot row creation PDUs:\n\n");
	       }
	       dumpSizeOfCreatePDU(f, smiNode, 0);
	       c++;
	  }
     }

     for (c = 0, smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_ROW);
	  smiNode;
	  smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_ROW)) {
	  if (smiNode->create) {
	       if (c == 0) {
		    fprintf(f, "# size of one-shot row creation PDUs excluding columns with default values:\n\n");
	       }
	       dumpSizeOfCreatePDU(f, smiNode, 1);
	       c++;
	  }
     }

     for (c = 0,smiNode = smiGetFirstNode(smiModule, SMI_NODEKIND_NOTIFICATION);
	  smiNode;
	  smiNode = smiGetNextNode(smiNode, SMI_NODEKIND_NOTIFICATION), c++) {
          if (c == 0) {
	       fprintf(f, "# size of notification PDUs:\n\n");
	  }
	  dumpSizeOfNotificationPDU(f, smiNode);
     }
}



static void
dumpSizes(int modc, SmiModule **modv, int flags, char *output)
{
     int       i;
     FILE      *f = stdout;
     
     silent = (flags & SMIDUMP_FLAG_SILENT);
     
     if (output) {
	  f = fopen(output, "w");
	  if (!f) {
	       fprintf(stderr, "smidump: cannot open %s for writing: ", output);
	       perror(NULL);
	       exit(1);
	  }
     }
     
     if (flags & SMIDUMP_FLAG_UNITE) {
	  if (! silent) {
	       int pos = 8888;
	       fprintf(f, "# united module PDU sizes (generated by smidump "
		       SMI_VERSION_STRING ")\n");
	       fprintf(f, "#\n# smidump -u -f sizes");
	       for (i = 0; i < modc; i++) {
		    int len = strlen(modv[i]->name);
		    if (pos + len > 70) {
			 fprintf(f, " \\\n#\t"), pos = 8;
		    }
		    fprintf(f, "%s ", modv[i]->name);
		    pos += len + 1;
	       }
	       fprintf(f, "%s\n", (pos == 8) ? "" : "\n");
	  }
	  for (i = 0; i < modc; i++) {
	       gaga(f, modv[i]);
	  }
     } else {
	  for (i = 0; i < modc; i++) {
	       if (! silent) {
		    fprintf(f, "# %s module PDU sizes (generated by smidump "
			    SMI_VERSION_STRING ")\n\n", modv[i]->name);
	       }
	       gaga(f, modv[i]);
	  }
     }
     
     if (fflush(f) || ferror(f)) {
	  perror("smidump: write error");
	  exit(1);
     }
     
     if (output) {
	  fclose(f);
     }
}

void
initSizes()
{
     
     static SmidumpDriver driver = {
	  "sizes",
	  dumpSizes,
	  0,
	  SMIDUMP_DRIVER_CANT_UNITE,
	  "SNMPv3 PDU sizes (RFC 3416)",
	  NULL,
	  NULL
     };
     
     smidumpRegisterDriver(&driver);
}
