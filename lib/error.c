/*
 * error.c --
 *
 *      Error handling routines.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: error.c,v 1.107 2003/02/27 12:10:15 schoenw Exp $
 */

#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(HAVE_WIN_H)
#include "win.h"
#endif

#include "smi.h"
#include "error.h"
#include "util.h"

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif



/*
 * Structure to hold error messages with their associated error level.
 * Note that it is possible to modify the error levels at run time.
 */

typedef struct Error {
    int level;			/* 0: fatal, no way to continue		     */
                 		/* 1: severe, changing semantics to continue */
				/*    must be corrected                      */
				/* 2: error, but able to continue,           */
				/*    should be corrected                    */
				/* 3: minor error, but should be corrected   */
				/* 4: change is recommended if possible      */
				/* 5: warning, but might totally correct     */
				/*    under some circumstances               */
				/* 6: just a notice                          */
    int id;			/* error id, used in error() invocation	     */
    char *tag;			/* tag for error identification on cmd line  */
    char *fmt;			/* the complete error format string	     */
    char *description;		/* text describing the error in more detail  */
} Error;



/*
 * Note: The Makefile produces a list of error macros for every `ERR...'
 * pattern in this file (error.c). This list is written to errormacros.h.
 */

static Error errors[] = {
    { 0, ERR_INTERNAL, "internal", 
      "internal error!!!", NULL},
    { 0, ERR_MAX_LEX_DEPTH, "", 
      "maximum IMPORTS nesting, probably a loop?", NULL},
    { 1, ERR_LEX_UNEXPECTED_CHAR, "lexical", 
      "lexically unexpected character, skipping to end of line", NULL},
    { 0, ERR_OUT_OF_MEMORY, "memory", 
      "out of memory (internal error!)", NULL},
    { 1, ERR_OTHER_ERROR, "other", 
      "%s", NULL},
    { 1, ERR_ILLEGAL_KEYWORD, "keyword-illegal", 
      "illegal keyword `%s'", NULL},
    { 2, ERR_ID_ENDS_IN_HYPHEN, "hyphen-end",
      "identifier `%s' illegally ends in a hyphen", NULL},
    { 3, ERR_LEADING_ZEROS, "number-leading-zero",
      "leading zero(s) on a number", NULL},
    { 2, ERR_NUMBER_TOO_LARGE, "number-range",
      "number `%s' too large", NULL},
    { 2, ERR_HEX_ENDS_IN_B, "string-delimiter",
      "hexadecimal string terminated by binary string delimiter, assume hex value", NULL},
    { 2, ERR_MODULENAME_64, "namelength-64-module",
      "module name `%s' must not be longer that 64 characters", NULL},
    { 4, ERR_MODULENAME_32, "namelength-32-module",
      "module name `%s' longer than 32 characters", NULL},
    { 2, ERR_TYPENAME_64, "namelength-64-type",
      "type name `%s' must not be longer that 64 characters", NULL},
    { 4, ERR_TYPENAME_32, "namelength-32-type",
      "type name `%s' longer than 32 characters", NULL},
    { 2, ERR_OIDNAME_64, "namelength-64-object",
      "object identifier name `%s' must not be longer that 64 characters", NULL},
    { 4, ERR_OIDNAME_32, "namelength-32-object",
      "object identifier name `%s' longer than 32 characters", NULL},
    { 2, ERR_ENUMNAME_64, "namelength-64-enumeration",
      "enumeration name `%s' must not be longer that 64 characters", NULL},
    { 4, ERR_ENUMNAME_32, "namelength-32-enumeration",
      "enumeration name `%s' longer than 32 characters", NULL},
    { 2, ERR_BITNAME_64, "namelength-64-bit",
      "bit name `%s' must not be longer than 64 characters", NULL},
    { 4, ERR_BITNAME_32, "namelength-32-bit",
      "bit name `%s' longer than 32 characters", NULL},
    { 2, ERR_UCIDENTIFIER_64, "namelength-64-uc-identifier",
      "uppercase identifier `%s' must not be longer than 64 characters", NULL},
    { 2, ERR_LCIDENTIFIER_64, "namelength-64-lc-identifier",
      "lowercase identifier `%s' must not be longer than 64 characters", NULL},
    { 2, ERR_TRAP_TYPE, "trap",
      "TRAP-TYPE macro is not allowed in SMIv2", NULL},
    { 2, ERR_TOO_MANY_MODULE_IDENTITIES, "module-identity-multiple",
      "more than one MODULE-IDENTITY clause in SMIv2 MIB", NULL},
    { 2, ERR_NO_MODULE_IDENTITY, "module-identity-missing",
      "missing MODULE-IDENTITY clause in SMIv2 MIB", NULL},
    { 2, ERR_OID_DEFVAL_TOO_LONG_SMIV2, "default-too-long-smiv2",
      "object identifier default values must be expressed as a single identifier", NULL},
    { 4, ERR_OID_DEFVAL_TOO_LONG_SMIV1, "default-too-long-smiv1",
      "object identifier default values in form of sub-identifier sequences are not implemented", NULL},
    { 2, ERR_INVALID_SMIV1_ACCESS, "access-invalid-smiv1", 
      "invalid access `%s' in SMIv1 MIB", NULL},
    { 2, ERR_INVALID_SMIV2_ACCESS, "access-invalid-smiv2",
      "invalid access `%s' in SMIv2 MIB", NULL},
    { 2, ERR_SMIV2_WRITE_ONLY, "access-write-only-smiv2", 
      "access `write-only' is no longer allowed in SMIv2", NULL},
    { 4, ERR_SMIV1_WRITE_ONLY, "access-write-only-smiv1", 
      "access `write-only' is not a good idea", NULL},
    { 2, ERR_INVALID_NOTIFICATION_VARIATION_ACCESS, "variation-access-notification", 
      "invalid access `%s' in a notification variation", NULL},
    { 2, ERR_INVALID_OBJECT_VARIATION_ACCESS, "variation-access-object", 
      "invalid access `%s' in an object variation", NULL},
    { 2, ERR_INVALID_VARIATION_ACCESS, "variation-access", 
      "invalid access `%s' in a variation", NULL},
    { 2, ERR_NOTIFICATION_VARIATION_SYNTAX, "variation-syntax", 
      "SYNTAX is not allowed in a notification variation", NULL},
    { 2, ERR_NOTIFICATION_VARIATION_WRITESYNTAX, "variation-writesyntax", 
      "WRITE-SYNTAX is not allowed in a notification variation", NULL},
    { 2, ERR_DEFVAL_SYNTAX, "defval-syntax", 
      "default value syntax does not match object syntax", NULL},
    { 2, ERR_NOTIFICATION_VARIATION_DEFVAL, "variation-defval", 
      "default value is not allowed in a notification variation", NULL},
    { 2, ERR_NOTIFICATION_VARIATION_CREATION, "variation-creation", 
      "CREATION-REQUIRES is not allowed in a notification variation", NULL},
    { 3, ERR_MODULE_IDENTITY_NOT_FIRST, "module-identity-not-first", 
      "MODULE-IDENTITY clause must be the first declaration in a module", NULL},
    { 2, ERR_INVALID_SMIV1_STATUS, "status-invalid-smiv1", 
      "invalid status `%s' in SMIv1 MIB", NULL},
    { 2, ERR_INVALID_SMIV2_STATUS, "status-invalid-smiv2", 
      "invalid status `%s' in SMIv2 MIB", NULL},
    { 2, ERR_INVALID_CAPABILITIES_STATUS, "status-invalid-capabilities", 
      "invalid status `%s' in AGENT-CAPABILITIES macro", NULL},
    { 5, ERR_OIDNAME_INCLUDES_HYPHEN, "hyphen-in-object",
      "object identifier name `%s' should not include hyphens in SMIv2 MIB", NULL},
    { 2, ERR_ILLEGAL_CHAR_IN_STRING, "char-illegal-string", 
      "illegal character `%c' (0x%2x) in quoted string", NULL},
    { 2, ERR_BIN_STRING_MUL8, "", 
      "length of binary string `%s' is not a multiple of 8", NULL},
    { 2, ERR_HEX_STRING_MUL2, "", 
      "length of hexadecimal string `%s' is not a multiple of 2", NULL},
    { 5, ERR_FLUSH_DECLARATION, "", 
      "flushing recent incorrect declaration, see previous error(s)", NULL},
    { 2, ERR_MAX_ACCESS_IN_SMIV1, "", 
      "MAX-ACCESS is SMIv2 style, use ACCESS in SMIv1 MIBs instead", NULL},
    { 2, ERR_ACCESS_IN_SMIV2, "", 
      "ACCESS is SMIv1 style, use MAX-ACCESS in SMIv2 MIBs instead", NULL},
    { 5, ERR_UNWANTED_MODULE, "", 
      "ignoring unwanted module `%s'", NULL},
    { 1, ERR_MODULE_NOT_FOUND, "", 
      "failed to locate MIB module `%s'", NULL},
    { 2, ERR_OBJECT_IDENTIFIER_REGISTERED, "", 
      "Object identifier label `%s.%s' already registered at `%s'", NULL},
    { 1, ERR_OPENING_INPUTFILE, "", 
      "%s: %s", NULL},
    { 1, ERR_ILLEGAL_INPUTFILE, "", 
      "%s: unable to determine SMI version", NULL},
    { 1, ERR_UNKNOWN_OIDLABEL, "", 
      "unknown object identifier label `%s'", NULL},
    { 2, ERR_SINGLE_SUBIDENTIFIER, "", 
      "single number `%s' is not a valid object identifier", NULL},
    { 2, ERR_SUBIDENTIFIER_VS_OIDLABEL, "", 
      "subidentifier `%s' does not match object identifier label `%s'", NULL},
    { 2, ERR_EXISTENT_OBJECT, "", 
      "an object named `%s' already exists", NULL},
    { 2, ERR_IDENTIFIER_NOT_IN_MODULE, "", 
      "identifier `%s' cannot be imported from module `%s'", NULL},
    { 1, ERR_MACRO, "", 
      "MACRO definitions are only allowed in SMI base modules", NULL},
    { 1, ERR_CHOICE, "", 
      "CHOICE type definitions are only allowed in SMI base modules", NULL},
    { 1, ERR_TYPE_SMI, "", 
      "type `%s' may only be defined in SMI/SPPI base modules", NULL},
    { 1, ERR_TYPE_TAG, "", 
      "tagged or IMPLICIT types may only be defined in SMI base modules", NULL},
    { 1, ERR_MACRO_ALREADY_EXISTS, "", 
      "module `%s' already declared a macro `%s'", NULL},
    { 1, ERR_EXPORTS, "", 
      "EXPORTS are only allowed in SMIv1 base modules", NULL},
    { 1, ERR_ILLEGALLY_QUALIFIED, "", 
      "illegally qualified object identifier label `%s'", NULL},
    { 2, ERR_MISSING_DESCRIPTION, "", 
      "missing DESCRIPTION on SMIv2 OBJECT-TYPE", NULL},
    { 2, ERR_OIDLABEL_NOT_FIRST, "", 
      "Object identifier element `%s' name only allowed as first element", NULL},
    { 2, ERR_UNKNOWN_TYPE, "", 
      "unknown type `%s'", NULL},
    { 1, ERR_LOCATION, "", 
      "opening MIB directory or file `%s': %s", NULL},
    { 1, ERR_UNKNOWN_LOCATION_TYPE, "", 
      "unknown MIB location type `%s'", NULL},
    { 2, ERR_ILLEGAL_RANGE_FOR_COUNTER, "counter-range-illegal", 
      "illegal range restriction for counter type `%s'", NULL},
    { 2, ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE, "range-illegal", 
      "illegal range restriction for non-numerical parent type `%s'", NULL},
    { 2, ERR_ILLEGAL_SIZE_FOR_PARENT_TYPE, "size-illegal", 
      "illegal size restriction for non-octet-string parent type `%s'", NULL},
    { 2, ERR_ILLEGAL_ENUM_FOR_PARENT_TYPE, "enum-illegal", 
      "illegal enumeration or bits restriction for non-enumeration-or-bits parent type `%s'", NULL},
    { 1, ERR_SMIV2_SIGNED_NUMBER_RANGE, "", 
      "number `%s' is out of SMIv1/SMIv2 signed number range", NULL},
    { 1, ERR_SMIV2_UNSIGNED_NUMBER_RANGE, "", 
      "number `%s' is out of SMIv1/SMIv2 unsigned number range", NULL},
    { 1, ERR_INTEGER32_TOO_LARGE, "", 
      "Integer32 value `%u' is too large", NULL},
    { 1, ERR_UNEXPECTED_VALUETYPE, "", 
      "type of value does not match declaration", NULL},
    { 1, ERR_SMI_NOT_SUPPORTED, "", 
      "file `%s' seems to be SMIv1 or SMIv2 which is not supported", NULL},
    { 1, ERR_SMING_NOT_SUPPORTED, "", 
      "file `%s' seems to be SMIng which is not supported", NULL},
    { 5, ERR_UNUSED_IMPORT, "import-unused", 
      "identifier `%s' imported from module `%s' is never used", NULL},
    { 2, ERR_MACRO_NOT_IMPORTED, "", 
      "macro `%s' has not been imported from module `%s'", NULL},
    { 5, ERR_IMPLICIT_NODE, "", 
      "implicit node definition", NULL},
    { 3, ERR_SCALAR_READCREATE, "", 
      "scalar object must not have a `read-create' access value", NULL},
    { 4, ERR_NAMEDNUMBER_INCLUDES_HYPHEN, "hyphen-in-label", 
      "named number `%s' must not include a hyphen in SMIv2", NULL},
    { 4, ERR_NAMEDBIT_INCLUDES_HYPHEN, "hyphen-in-label", 
      "named bit `%s' must not include a hyphen in SMIv2", NULL},
    { 2, ERR_REDEFINITION, "", 
      "redefinition of identifier `%s'", NULL},
    { 5, ERR_EXT_REDEFINITION, "", 
      "redefinition of identifier `%s::%s'", NULL},
    { 5, ERR_CASE_REDEFINITION, "", 
      "identifier `%s' differs from `%s' only in case", NULL},
    { 5, ERR_EXT_CASE_REDEFINITION, "", 
      "identifier `%s' differs from `%s::%s' only in case", NULL},
    { 6, ERR_PREVIOUS_DEFINITION, "",
      "previous definition of `%s'", NULL},
    { 2, ERR_INVALID_FORMAT, "invalid-format", 
      "invalid format specification `%s'", NULL},
    { 3, ERR_REFINEMENT_ALREADY_EXISTS, "", 
      "refinement for `%s' already exists in this compliance statement", NULL},
    { 3, ERR_OPTIONALGROUP_ALREADY_EXISTS, "", 
      "optional group definition for `%s' already exists in this compliance statement", NULL},
    { 2, ERR_ILLEGAL_OID_DEFVAL, "", 
      "cannot handle other default values than 0.0 for `%s'", NULL},
    { 2, ERR_UNEXPECTED_TYPE_RESTRICTION, "subtype-in-sequence", 
      "subtyping not allowed in SEQUENCE", NULL},
    { 1, ERR_UNKNOWN_CONFIG_CMD, "", 
      "unknown configuration command `%s' in file `%s'", NULL},
    { 6, ERR_CACHE_CONFIG_NOT_SUPPORTED, "", 
      "module caching is not supported though configured in file `%s'", NULL},
    { 4, ERR_SMIV2_OPAQUE_OBSOLETE, "opaque-smiv2", 
      "SMIv2 provides Opaque solely for backward-compatibility", NULL},
    { 4, ERR_SPPI_OPAQUE_OBSOLETE, "opaque-sppi", 
      "SPPI provides Opaque solely for backward-compatibility", NULL},
    { 2, ERR_DATE_CHARACTER, "date-character", 
      "date specification `%s' contains an illegal character", NULL},
    { 2, ERR_DATE_LENGTH, "date-length", 
      "date specification `%s' has an illegal length", NULL},
    { 2, ERR_DATE_VALUE, "date-value", 
      "date specification `%s' contains an illegal value", NULL},
    { 5, ERR_DATE_YEAR_2DIGITS, "date-year-2digits", 
      "date specification `%s' contains a two-digit year representing `%d'", NULL},
    { 2, ERR_DATE_YEAR, "date-year", 
      "date specification `%s' contains an illegal year", NULL},
    { 2, ERR_DATE_MONTH, "date-month", 
      "date specification `%s' contains an illegal month", NULL},
    { 2, ERR_DATE_DAY, "date-day", 
      "date specification `%s' contains an illegal day", NULL},
    { 2, ERR_DATE_HOUR, "date-hour", 
      "date specification `%s' contains an illegal hour", NULL},
    { 2, ERR_DATE_MINUTES, "date-minutes", 
      "date specification `%s' contains illegal minutes", NULL},
    { 4, ERR_DATE_IN_FUTURE, "date-in-future", 
      "date specification `%s' is in the future", NULL},
    { 4, ERR_DATE_IN_PAST, "date-in-past", 
      "date specification `%s' predates the SMI standard", NULL},
    { 5, ERR_INTEGER_IN_SMIV2, "integer-misuse", 
      "use Integer32 instead of INTEGER in SMIv2", NULL},
    { 5, ERR_MODULE_ALREADY_LOADED, "", 
      "module `%s' is already loaded, aborting parser on this file", NULL},
    { 2, ERR_SMIV2_BASETYPE_NOT_IMPORTED, "basetype-not-imported", 
      "SMIv2 base type `%s' must be imported from SNMPv2-SMI", NULL},
    { 2, ERR_BASETYPE_UNKNOWN, "basetype-unknown", 
      "type `%s' of node `%s' does not resolve to a known base type", NULL},
    { 2, ERR_ROW_SUBID_ONE, "row-node-subidentifier-one", 
      "subidentifier of row node `%s' must be 1", NULL},
    { 5, ERR_ROWNAME_ENTRY, "row-name-entry", 
      "row identifier `%s' should match `*Entry'", NULL},
    { 5, ERR_TABLENAME_TABLE, "table-name-table", 
      "table identifier `%s' should match `*Table'", NULL},
    { 5, ERR_ROWNAME_TABLENAME, "row-name-table-name",
      "row identifier `%s' should have the same prefix as table identifier `%s'", NULL},
    { 5, ERR_SMIV2_TYPE_ASSIGNEMENT, "type-assignement-smiv2", 
      "type assignement `%s' should be a textual convention in SMIv2", NULL},
    { 5, ERR_ILLEGAL_IMPORT, "import-illegal", 
      "identifier `%s' may not be imported from module `%s'", NULL},
    { 3, ERR_SMIV2_NESTED_TEXTUAL_CONVENTION, "textual-convention-nested", 
      "textual convention `%s' can not be derived from the textual convention `%s'", NULL},
    { 3, ERR_TABLE_ACCESS, "access-table-illegal", 
      "table node `%s' must be `not-accessible'", NULL},
    { 3, ERR_ROW_ACCESS, "access-row-illegal", 
      "row node `%s' must be `not-accessible'", NULL},
    { 4, ERR_COUNTER_ACCESS, "access-counter-illegal", 
      "counter object `%s' must be `read-only' or `accessible-for-notify'", NULL},
    { 2, ERR_ENUM_SUBTYPE, "subtype-enumeration-illegal", 
      "named number `%s(%d)' illegal in sub-type of `%s'", NULL},
    { 2, ERR_BITS_SUBTYPE, "subtype-bits-illegal", 
      "named number `%s' illegal in sub-type of `%s'", NULL},
    { 2, ERR_ENUM_NAME_REDEFINITION, "enum-name-redefinition", 
      "redefinition of name `%s' in number enumeration", NULL},
    { 2, ERR_ENUM_NUMBER_REDEFINITION, "enum-number-redefinition", 
      "redefinition of number `%d' in number enumeration", NULL},
    { 2, ERR_ENUM_ZERO, "enum-zero", 
      "number enumeration contains zero value in SMIv1 MIB", NULL},
    { 2, ERR_BITS_NAME_REDEFINITION, "bits-name-redefinition", 
      "redefinition of name `%s' in named bits list", NULL},
    { 2, ERR_BITS_NUMBER_REDEFINITION, "bits-number-redefinition", 
      "redefinition of number `%u' in named bits list", NULL},
    { 2, ERR_BITS_NUMBER_TOO_LARGE, "bits-number-too-large", 
      "named bit `%s(%u)' exceeds maximum bit position", NULL},
    { 4, ERR_BITS_NUMBER_LARGE, "bits-number-large", 
      "named bit `%s(%u)' may cause interoperability or implementation problems", NULL},
    { 4, ERR_BITS_ZERO_NOT_NAMED, "bits-zero-not-used",
      "first bit (bit zero) has no name assigned",
      "RFC 2578 section 7.1.4 requires that named bits start at bit\n"
      "position zero." },
    { 2, ERR_RANGE_OUT_OF_BASETYPE, "range-bounds",
      "range limit exceeds underlying basetype", NULL},
    { 2, ERR_RANGE_OVERLAP, "range-overlap",
      "overlapping range limits", NULL},
    { 5, ERR_RANGES_NOT_ASCENDING, "range-ascending",
      "ranges not in ascending order", NULL},
    { 5, ERR_NAMED_NUMBERS_NOT_ASCENDING, "named-numbers-ascending",
      "named numbers not in ascending order", NULL},
    { 2, ERR_EXCHANGED_RANGE_LIMITS, "range-exchanged",
      "range limits must be `lower-bound .. upper-bound'", NULL},
    { 1, ERR_INDEX_BASETYPE, "index-illegal-basetype", 
      "illegal base type `%s' in index element `%s' of row %s", NULL},
    { 5, ERR_INDEX_TOO_LARGE, "index-exceeds-too-large", 
      "index of row `%s' can exceed OID size limit by %d subidentifier(s)", NULL},
    { 2, ERR_INDEX_NO_RANGE, "index-element-no-range",
      "index element `%s' of row `%s' must have a range restriction", NULL},
    { 2, ERR_INDEX_NO_RANGE_MOD, "index-element-no-range",
      "index element `%s::%s' of row `%s' must have a range restriction", NULL},
    { 1, ERR_INDEX_STRING_NO_SIZE, "index-element-no-size",
      "index element `%s' of row `%s' must have a size restriction",
      "RFC 2578 section 3.5 restricts object identifier to have at\n"
      "most 128 sub-identifier. This implies that index elements used\n"
      "to form instance identifiers must have a size contraint which\n"
      "ensures that the 128 sub-identifier constraint is kept intact." },
    { 1, ERR_INDEX_STRING_NO_SIZE_MOD, "index-element-no-size",
      "index element `%s::%s' of row `%s' must have a size restriction", NULL},
    { 5, ERR_INDEX_OID_NO_SIZE, "index-element-no-size",
      "index element `%s' of row `%s' should but cannot have a size restriction", NULL},
    { 5, ERR_INDEX_OID_NO_SIZE_MOD, "index-element-no-size",
      "index element `%s::%s' of row `%s' should but cannot have a size restriction", NULL},
    { 1, ERR_INDEX_RANGE_NEGATIVE, "index-element-range-negative",
      "range restriction of index element `%s' of row `%s' must be non-negative", NULL},
    { 1, ERR_INDEX_ENUM_NEGATIVE, "index-element-enum-negative",
      "enumerations of index element `%s' of row `%s' must be non-negative", NULL},
    { 5, ERR_INDEX_NOT_COLUMN, "index-element-not-column",
      "index element `%s' of row `%s' must be a column", NULL},
    { 2, ERR_AUGMENT_NESTED, "augment-nested",
      "row `%s' augments `%s' which is not a base table row", NULL},
    { 2, ERR_AUGMENT_NO_ROW, "augment-no-row",
      "row `%s' augments or extends `%s' which is not a row", NULL},
    { 4, ERR_NODE_NOT_IN_GROUP, "group-membership",
      "node `%s' must be contained in at least one conformance group", NULL},
    { 4, ERR_NOTIFICATION_NOT_IN_GROUP, "group-membership",
      "notification `%s' must be contained in at least one conformance group", NULL},
    { 3, ERR_INVALID_GROUP_MEMBER, "group-member-invalid",
      "node `%s' is an invalid member of group `%s'", NULL},
    { 3, ERR_MIXED_GROUP_MEMBERS, "group-member-mixed",
      "group `%s' contains scalars/columns and notifications", NULL},
    { 2, ERR_BAD_LOWER_IDENTIFIER_CASE, "bad-identifier-case",
      "`%s' should start with a lower case letter", NULL},
    { 2, ERR_UNDERSCORE_IN_IDENTIFIER, "underscore-in-identifier",
      "identifier `%s' must not contain an underscore", NULL},
    { 5, ERR_OID_REUSE, "oid-reuse",
      "identifier `%s' reuses object identifier assigned to identifier `%s'", NULL},
    { 1, ERR_OID_REGISTERED, "oid-registered",
      "identifier `%s' registers object identifier already registered by `%s'", NULL},
    { 1, ERR_OID_RECURSIVE, "oid-recursive",
      "object identifier `%s' defined recursively or too long", NULL},
    { 5, ERR_COMMENT_TERMINATES, "comment-terminates",
      "note, -- terminates a comment", NULL},
    { 5, ERR_OBSOLETE_IMPORT, "obsolete-import",
      "identifier `%s' should be imported from `%s' instead of `%s'", NULL},
    { 4, ERR_GROUP_OBJECT_STATUS, "group-object-status",
      "%s group `%s' includes %s object `%s'", NULL},
    { 4, ERR_COMPLIANCE_GROUP_STATUS, "compliance-group-status",
      "%s compliance statement `%s' includes %s group `%s'", NULL},
    { 4, ERR_COMPLIANCE_OBJECT_STATUS, "compliance-object-status",
      "%s compliance statement `%s' includes %s object `%s'", NULL},
    { 5, ERR_GROUP_UNREF, "group-unref",
      "%s group `%s' is not referenced in this module", NULL},
    { 5, ERR_TYPE_UNREF, "type-unref",
      "%s type `%s' is not referenced in this module", NULL},
    { 5, ERR_SEQUENCE_ORDER, "sequence-order", 
      "SEQUENCE element #%d `%s' does not match order of columnar objects under `%s'", NULL},
    { 3, ERR_SEQUENCE_NO_COLUMN, "sequence-no-column", 
      "SEQUENCE element #%d `%s' is not a child node under `%s'", NULL},
    { 3, ERR_SEQUENCE_MISSING_COLUMN, "sequence-missing-column", 
      "SEQUENCE of `%s' is missing columnar object `%s'", NULL},
    { 4, ERR_ILLEGAL_ROWSTATUS_DEFAULT, "rowstatus-default", 
      "illegal `RowStatus' default value `%s'", NULL},
    { 4, ERR_ILLEGAL_STORAGETYPE_DEFAULT, "storagetype-default", 
      "illegal `StorageType' default value `%s'", NULL},
    { 2, ERR_DEFVAL_OUT_OF_BASETYPE, "defval-basetype", 
      "default value exceeds range of underlying basetype", NULL},
    { 2, ERR_DEFVAL_OUT_OF_RANGE, "defval-range", 
      "default value does not match range restriction of underlying type", NULL},
    { 2, ERR_DEFVAL_OUT_OF_ENUM, "defval-enum", 
      "default value does not match underlying enumeration type", NULL},
    { 5, ERR_TADDRESS_WITHOUT_TDOMAIN, "taddress-tdomain", 
      "`TAddress' object should have an accompanied `TDomain' object", NULL},
    { 5, ERR_INETADDRESS_WITHOUT_TYPE, "inetaddress-inetaddresstype", 
      "`InetAddress' object should have an accompanied preceding `InetAdressType' object", NULL},
    { 5, ERR_INETADDRESSTYPE_SUBTYPED, "inetaddresstype-subtyped", 
      "`InetAddressType' should not be subtyped", NULL},
    { 5, ERR_INETADDRESS_SPECIFIC, "inetaddress-specific", 
      "`InetAddress' should be used instead of `%s'", NULL},
    { 5, ERR_TRANSPORTADDRESS_WITHOUT_TYPE, "transportaddress-transportaddresstype", 
      "`TransportAddress' object should have an accompanied preceding `TransportAdressType' or `TransportDomain' object", NULL},
    { 5, ERR_TRANSPORTADDRESSTYPE_SUBTYPED, "transportaddresstype-subtyped", 
      "`TransportAddressType' should not be subtyped", NULL},
    { 5, ERR_TRANSPORTADDRESS_SPECIFIC, "transportaddress-specific", 
      "`TransportAddress' should be used instead of `%s'", NULL},
    { 3, ERR_NOTIFICATION_OBJECT_TYPE, "notification-object-type", 
      "object `%s' of notification `%s' must be a scalar or column", NULL},
    { 3, ERR_NOTIFICATION_OBJECT_ACCESS, "notification-object-access", 
      "object `%s' of notification `%s' must not be `not-accessible'", NULL},
    { 5, ERR_NOTIFICATION_OBJECT_MIX, "notification-object-mix", 
      "notification `%s' contains objects from multiple tables/groups", NULL},
    { 5, ERR_EMPTY_DESCRIPTION, "empty-description",
      "zero-length description string", NULL},
    { 5, ERR_EMPTY_REFERENCE, "empty-reference",
      "zero-length reference string", NULL},
    { 5, ERR_EMPTY_ORGANIZATION, "empty-organization",
      "zero-length organization string", NULL},
    { 5, ERR_EMPTY_CONTACT, "empty-contact",
      "zero-length contact string", NULL},
    { 5, ERR_EMPTY_FORMAT, "empty-format",
      "zero-length format string", NULL},
    { 5, ERR_EMPTY_UNITS, "empty-units",
      "zero-length units string", NULL},
    { 5, ERR_INDEX_DEFVAL, "index-element-has-default",
      "index element `%s' of row `%s' has a default value", NULL},
    { 2, ERR_SEQUENCE_TYPE_MISMATCH, "sequence-type-mismatch",
      "type of `%s' in sequence and object type definition do not match", NULL},
    { 5, ERR_INDEX_ACCESSIBLE, "index-element-accessible",
      "index element `%s' of row `%s' should be not-accessible in SMIv2 MIB", NULL},
    { 5, ERR_INDEX_NON_ACCESSIBLE, "index-element-not-accessible",
      "exactly one index element of row `%s' must be accessible in SMIv2 MIB", NULL},
    { 3, ERR_REFINEMENT_NOT_LISTED, "refinement-not-listed",
      "refined object `%s' not listed in a mandatory or optional group", NULL},
    { 5, ERR_NOTIFICATION_NOT_REVERSIBLE, "notification-not-reversible",
      "notification `%s' is not reverse mappable", NULL},
    { 5, ERR_NOTIFICATION_ID_TOO_LARGE, "notification-id-too-large",
      "last sub-identifier of notification `%s' too large", NULL},
    { 2, ERR_NODE_PARENT_TYPE, "parent-node",
      "node's parent node must be simple node", NULL},
    { 2, ERR_SCALAR_PARENT_TYPE, "parent-scalar",
      "scalar's parent node must be simple node", NULL},
    { 2, ERR_TABLE_PARENT_TYPE, "parent-table",
      "table's parent node must be simple node", NULL},
    { 2, ERR_TABLE_ROW_TYPE_MISMATCH, "table-type",
      "table's SEQUENCE OF type does not match row type", NULL},
    { 2, ERR_ROW_PARENT_TYPE, "parent-row",
      "row's parent node must be a table node", NULL},
    { 2, ERR_COLUMN_PARENT_TYPE, "parent-column",
      "columnar object's parent node must be a row", NULL},
    { 2, ERR_NOTIFICATION_PARENT_TYPE, "parent-notification",
      "notifications's parent node must be a simple node", NULL},
    { 2, ERR_GROUP_PARENT_TYPE, "parent-group",
      "group's parent node must be a simple node", NULL},
    { 2, ERR_COMPLIANCE_PARENT_TYPE, "parent-compliance",
      "compliance's parent node must be a simple node", NULL},
    { 2, ERR_CAPABILITIES_PARENT_TYPE, "parent-capabilities",
      "capabilities' parent node must be a simple node", NULL},
    { 1, ERR_SPPI_SIGNED64_NUMBER_RANGE, "", 
      "number `%s' is out of range for SPPI 64bit signed numbers", NULL},
    { 1, ERR_SPPI_UNSIGNED64_NUMBER_RANGE, "", 
      "number `%s' is out of range for SPPI 64bit unsigned numbers", NULL},
    { 0, ERR_SMI_CONSTRUCT_IN_PIB, "", 
      "the SMI construct/keyword `%s' may not be used in a PIB", NULL},
    { 0, ERR_SPPI_CONSTRUCT_IN_MIB, "", 
      "the SPPI construct/keyword `%s' may not be used in a MIB", NULL},
    { 2, ERR_POLICY_ACCESS_IN_PIB, "",
      "the PIB uses POLICY-ACCESS where PIB-ACCESS is required", NULL},
    { 2, ERR_INVALID_SPPI_ACCESS, "access-invalid-sppi", 
      "invalid access `%s' in SPPI PIB", NULL},
    { 2, ERR_INVALID_SPPI_STATUS, "status-invalid-sppi", 
      "invalid status `%s' in SPPI PIB", NULL},
    { 2, ERR_INDEX_SHOULD_BE_PIB_INDEX, "",
      "the PIB uses ACCESS where PIB-INDEX is required", NULL},
    { 0, ERR_INDEX_USED_TWICE, "",
      "INDEX was used twice inside an OBJECT-TYPE declaration", NULL},
    { 1, ERR_SUBJECT_CATEGORIES_MISSING, "subject-categories-missing",
      "a MODULE-IDENTITY clause lacks SUBJECT-CATEGORIES", NULL},
    { 1, ERR_UNKNOWN_SUBJECT_CATEGORY, "unknown-subject-category",
      "the subject category `%s' is invalid", NULL},
    { 1, ERR_OBJECTS_MISSING_IN_OBJECT_GROUP, "objects-missing-in-group",
      "an OBJECT-GROUP is missing the OBJECTS part", NULL},
    { 2, ERR_NOT_ACCESSIBLE_IN_PIB_ACCESS, "not-accessible-in-pib-access",
      "PIB-ACCESS must not be set to `not-accessible'", NULL},
    { 2, ERR_REPORT_ONLY_IN_PIB_MIN_ACCESS, "report-only-in-pib-min-access",
      "PIB-MIN-ACCESS must not be set to `report-only'", NULL},
    { 1, ERR_INDEX_AND_AUGMENTS_USED, "index-and-augments",
      "either INDEX or AUGMENTS may be used, but not both", NULL},
    { 1, ERR_INDEX_WITHOUT_PIB_INDEX, "index-without-pib-index",
      "INDEX may not be used without PIB-INDEX", NULL},
    { 3, ERR_ERROR_NUMBER_RANGE, "install-error-range",
      "the named-number for an INSTALL-ERROR is out of range: allowed 1..65535, current %d", NULL},
    { 3, ERR_CATEGORY_ID_RANGE, "subject-categories-range",
      "the named-number for a SUBJECT-CATEGORIES entry must be greater than 0, current: %d", NULL},
    { 2, ERR_SPPI_BASETYPE_NOT_IMPORTED, "sppi-basetype-not-imported",
      "SPPI basetype `%s' must be imported from COPS-PR-SPPI", NULL},
    { 2, ERR_ROW_LACKS_PIB_INDEX, "row-lacks-pib-index",
      "row definition lacks a PIB-INDEX/AUGMENTS/EXTENDS clause", NULL},
    { 2, ERR_PIB_INDEX_FOR_NON_ROW_TYPE, "pib-index-with-non-row-type",
      "non row type contains a PIB-INDEX/AUGMENTS/EXTENDS clause", NULL},
    { 2, ERR_OPAQUE_IN_SYNTAX, "opaque-in-syntax",
      "`Opaque' must not be used in SYNTAX clauses in PIB modules", NULL},
    { 2, ERR_IPADDRESS_IN_SYNTAX, "ipaddress-in-syntax",
      "`IpAddress' must not be used in SYNTAX clauses in PIB modules", NULL},
    { 2, ERR_TABLE_LACKS_PIB_ACCESS, "table-lacks-pib-access",
      "the table definition must contain a PIB-ACCESS clause", NULL},
    { 2, ERR_PIB_ACCESS_FOR_NON_TABLE, "pib-access-for-non-table",
      "type is not a table but contains a PIB-ACCESS clause", NULL},
    { 2, ERR_INSTALL_ERRORS_FOR_NON_TABLE, "install-errors-for-non-table",
      "type is not a table but contains a INSTALL-ERRORS clause", NULL},
    { 2, ERR_UNIQUENESS_FOR_NON_ROW, "uniqueness-for-non-row",
      "type is not a row but contains a UNIQUENESS clause", NULL},
    { 2, ERR_NOT_A_COLUMN, "not-a-column",
      "`%s', which is used in a UNIQUENESS clause, is not a column in this table", NULL},
    { 2, ERR_EXTENDS_WRONG_ROW_TYPE, "extend-row-type",
      "row `%s' extends `%s' which is neither a base table row nor a sparsely-augmented table row", NULL},
    { 2, ERR_PIB_INDEX_NOT_INSTANCEID, "pib-index-not-instanceid",
      "the PIB-INDEX `%s' must be of type `InstanceId'", NULL},
    { 2, ERR_LACKING_PIB_REFERENCES, "type-lacking-pib-references",
      "objects with SYNTAX `ReferenceId' must contain a PIB-REFERENCES clause", NULL},
    { 2, ERR_PIB_REFERENCES_WRONG_TYPE, "pib-references-with-wrong-type",
      "object contains a PIB-REFERENCES clause but is not of type `ReferenceId'", NULL},
    { 2, ERR_PIB_REFERENCES_NOT_ROW, "pib-references-not-row",
      "the PIB-REFERENCES does not point to a row", NULL},
    { 2, ERR_LACKING_PIB_TAG, "type-lacking-pib-tag",
      "objects with SYNTAX `TagReferenceId' must contain a PIB-TAG clause", NULL},
    { 2, ERR_PIB_TAG_WRONG_TYPE, "pib-tag-with-wrong-type",
      "object contains a PIB-TAG clause but is not of type `TagReferenceId'", NULL},
    { 2, ERR_SUBJECT_CATEGORIES_MISSING_SUBID, "subject-categories-missing-subid",
      "the SUBJECT-CATEGORIES are missing a sub-identifier", NULL},
    { 2, ERR_SUBJECT_CATEGORIES_ALL_WITH_SUBID, "subject-categories-all-with-subid",
      "the SUBJECT-CATEGORIES `all' must not have a sub-identifier", NULL},
    { 3, ERR_SUBJECT_CATEGORIES_ALL, "subject-categories-all",
      "SUBJECT-CATEGORIES contains several categories although `all' is used aswell", NULL},
    { 2, ERR_PIB_TAG_TYPE, "pib-tag-type",
      "a PIB-TAG must point to an object with a SYNTAX of `TagId'", NULL},
    { 3, ERR_ATTRIBUTE_NOT_IN_GROUP, "attribute-not-in-group",
      "attribute `%s' must be contained in at least one conformance group", NULL},
    { 0, ERR_OBJECTPTR_ELEMENT_IN_USE, "objectptr-element-in-use",
      "objectPtr->%s is already in use (%s)", NULL},
    { 1, ERR_OID_ADMIN_ZERO, "",
      "last subidentifier assigned to `%s' may not be zero", NULL},
    { 5, ERR_TYPE_STATUS_DEPRECATED, "type-status-deprecated",
      "type `%s' used by `%s' is deprecated", NULL},
    { 5, ERR_TYPE_STATUS_OBSOLETE, "type-status-obsolete",
      "type `%s' used by `%s' is obsolete",
      "This warning is generated in cases where something with status\n"
      "`current' uses a type whose status has been changed to `obsolete'.\n"
      "Note that the status of imported types can change without the\n"
      "control of the modules using these types." },
    { 0, 0, NULL, NULL, NULL }
};



/*
 *----------------------------------------------------------------------
 *
 * smiSetErrorSeverity --
 *
 *      Sets the severity of errors with tags matching pattern.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Changes the severity in the module internal error table.
 *
 *----------------------------------------------------------------------
 */

void
smiSetErrorSeverity(char *pattern, int severity)
{
    int i;
    
    for (i = 0; errors[i].fmt; i++) {
	if (strstr(errors[i].tag, pattern) == errors[i].tag) {
	    errors[i].level = severity;
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiSetErrorHandler --
 *
 *      Set the function that is called to handle error messages.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Changes the error handler used for subsequent error messages.
 *
 *----------------------------------------------------------------------
 */

void
smiSetErrorHandler(SmiErrorHandler smiErrorHandler)
{
    smiHandle->errorHandler = smiErrorHandler;
}



/*
 *----------------------------------------------------------------------
 *
 * smiGetErrorSeverity --
 *
 *      Return the severity of the error identified by id.
 *
 * Results:
 *      The error severity.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

int
smiGetErrorSeverity(int id)
{
    if (id < 0 || id >= (sizeof(errors) / sizeof(Error)) - 1) {
	return -1;
    }
    return errors[id].level;
}


/*
 *----------------------------------------------------------------------
 *
 * smiGetErrorTag --
 *
 *      Return the tag of the error identified by id.
 *
 * Results:
 *      The error tag or NULL.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

char*
smiGetErrorTag(int id)
{
    if (id < 0 || id >= sizeof(errors) / sizeof(Error)) {
	return NULL;
    }
    return errors[id].tag;
}



/*
 *----------------------------------------------------------------------
 *
 * smiGetErrorMsg --
 *
 *      Return the message of the error identified by id.
 *
 * Results:
 *      The error message or NULL.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

char*
smiGetErrorMsg(int id)
{
    if (id < 0 || id >= sizeof(errors) / sizeof(Error)) {
	return NULL;
    }
    return errors[id].fmt;
}



/*
 *----------------------------------------------------------------------
 *
 * smiGetErrorDescription --
 *
 *      Return the description of the error identified by id.
 *
 * Results:
 *      The error description or NULL.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

char*
smiGetErrorDescription(int id)
{
    if (id < 0 || id >= sizeof(errors) / sizeof(Error)) {
	return NULL;
    }
    return errors[id].description;
}



/*
 *----------------------------------------------------------------------
 *
 * smiErrorHandler --
 *
 *      This is the default error printing function. This is the
 *	only place in the libsmi where any output is generated.
 *	This function may be replaced by an application specific
 *	error message handler.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Prints error messages to the stderr output channel.
 *
 *----------------------------------------------------------------------
 */

void
smiErrorHandler(char *path, int line, int severity, char *msg, char *tag)
{
    if (path) {
	fprintf(stderr, "%s:%d: ", path, line);
    }
    switch (severity) {
    case 4:
    case 5:
	fprintf(stderr, "warning: ");
	break;
    case 6:	
	fprintf(stderr, "info: ");
	break;
    }
    fprintf(stderr, "%s\n", msg);

    /*
     * A severe error, no way to continue :-(
     */
    if (severity <= 0) {
	exit(1);
    }
}



/*
 *----------------------------------------------------------------------
 *
 * printError --
 *
 *      Internal error printer which is called by the varargs
 *	entry points (see below). If formats the error message
 *	and calls the error handling function that is currently
 *	registered.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Terminates the program if the error is severe and there
 *	is no way to continue.
 *
 *----------------------------------------------------------------------
 */

static void
printError(Parser *parser, int id, int line, va_list ap)
{
    char *buffer;
    int i;
    
    if (! smiHandle->errorHandler) {
	return;
    }

    /*
     * Search for the tag instead of just using the id as an index so
     * that we do not run into trouble if the id is bogus.
     */

    for (i = 0; errors[i].fmt; i++) {
	if (errors[i].id == id) break;
    }
    if (! errors[i].fmt) {
	i = 0;		/* assumes that 0 is the internal error */
    }

    if (parser) {

	if (parser->modulePtr) {
	    if ((parser->modulePtr->export.conformance > errors[i].level) ||
		(parser->modulePtr->export.conformance == 0)) {
		parser->modulePtr->export.conformance = errors[i].level;
	    }
	}
	
	if ((errors[i].level <= smiHandle->errorLevel) &&
	    (parser->flags & SMI_FLAG_ERRORS) &&
	    ((smiDepth == 1) || (parser->flags & SMI_FLAG_RECURSIVE))) {
	    smiVasprintf(&buffer, errors[i].fmt, ap);
	    (smiHandle->errorHandler) (parser->path, line,
				       errors[i].level, buffer, errors[i].tag);
	}
    } else {
	if (errors[i].level <= smiHandle->errorLevel) {
	    smiVasprintf(&buffer, errors[i].fmt, ap);
	    (smiHandle->errorHandler) (NULL, 0, errors[i].level,
				       buffer, errors[i].tag);
	}
    }
}



/*
 *----------------------------------------------------------------------
 *
 * smiPrintErrorAtLine --
 *
 *      Like smiPrintError() but shows a specfic line no.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiPrintErrorAtLine(Parser *parser, int id, int line, ...)
{
    va_list ap;

    va_start(ap, line);
    printError(parser, id, line, ap);
    va_end(ap);
}



/*
 *----------------------------------------------------------------------
 *
 * smiPrintError --
 *
 *      Prints an error message. The line number is taken from
 *	the current parser position.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

void
smiPrintError(Parser *parser, int id, ...)
{
    va_list ap;

    va_start(ap, id);
    printError(parser, id, parser ? parser->line : 0, ap);
    va_end(ap);
}
