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
 * @(#) $Id: error.c,v 1.43 2000/06/07 10:19:49 strauss Exp $
 */

#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "error.h"


int errorLevel;                          /* Higher level for more warnings   */



typedef struct Error {
    int level;			/* -1: fatal, no way to continue	     */
                 		/* 0: other output (e.g. statistics)	     */
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
} Error;


/*
 * Note: The Makefile produces a list of error macros for every `ERR...'
 * pattern in this file (error.c). This list is written to errormacros.h.
 */

Error errors[] = {
    { -1, ERR_INTERNAL, "internal", 
      "Internal error!!!" },
    { -1, ERR_MAX_LEX_DEPTH, "", 
      "Maximum IMPORTS nesting, probably a loop?" },
    { -1, ERR_LEX_UNEXPECTED_CHAR, "lexical", 
      "Lexically unexpected character (internal error!)" },
    { -1, ERR_OUT_OF_MEMORY, "memory", 
      "Out of memory (internal error!)" },
    { 0, ERR_STATISTICS, "", 
      "completed %s" },
    { 1, ERR_OTHER_ERROR, "other", 
      "%s" },
    { 1, ERR_ILLEGAL_KEYWORD, "keyword-illegal", 
      "Illegal keyword `%s'" },
    { 2, ERR_ID_ENDS_IN_HYPHEN, "hyphen-end",
      "Identifier `%s' illegally ends in a hyphen" },
    { 3, ERR_LEADING_ZEROS, "number-leading-zero",
      "Leading zero(s) on a number" },
    { 2, ERR_NUMBER_TOO_LARGE, "number-range",
      "Number `%s' too large" },
    { 2, ERR_HEX_ENDS_IN_B, "string-delimiter",
      "Hexadecimal string terminated by binary string delimiter, assume hex value" },
    { 2, ERR_MODULENAME_64, "namelength-64-module",
      "Module name `%s' must not be longer that 64 characters" },
    { 4, ERR_MODULENAME_32, "namelength-32-module",
      "Module name `%s' longer than 32 characters" },
    { 2, ERR_TYPENAME_64, "namelength-64-type",
      "Type name `%s' must not be longer that 64 characters" },
    { 4, ERR_TYPENAME_32, "namelength-32-type",
      "Type name `%s' longer than 32 characters" },
    { 2, ERR_OIDNAME_64, "namelength-64-object",
      "Object Identifier name `%s' must not be longer that 64 characters" },
    { 4, ERR_OIDNAME_32, "namelength-32-object",
      "Object Identifier name `%s' longer than 32 characters" },
    { 2, ERR_ENUMNAME_64, "namelength-64-enumeration",
      "Enumeration item name `%s' must not be longer that 64 characters" },
    { 4, ERR_ENUMNAME_32, "namelength-32-enumeration",
      "Enumeration item name `%s' longer than 32 characters" },
    { 2, ERR_BITNAME_64, "namelength-64-bit",
      "Bit name `%s' must not be longer than 64 characters" },
    { 4, ERR_BITNAME_32, "namelength-32-bit",
      "Bit name `%s' longer than 32 characters" },
    { 2, ERR_UCIDENTIFIER_64, "namelength-64-uc-identifier",
      "Uppercase identifier `%s' must not be longer than 64 characters" },
    { 2, ERR_LCIDENTIFIER_64, "namelength-64-lc-identifier",
      "Lowercase identifier `%s' must not be longer than 64 characters" },
    { 2, ERR_TRAP_TYPE, "trap",
      "The TRAP-TYPE macro is not allowed in SMIv2 style MIB" },
    { 2, ERR_TOO_MANY_MODULE_IDENTITIES, "module-identity-multiple",
      "There must be no more than exactly one MODULE-IDENTITY clause" },
    { 2, ERR_NO_MODULE_IDENTITY, "module-identity-missing",
      "Missing MODULE-IDENTITY clause in SMIv2 style MIB" },
    { 4, ERR_OID_DEFVAL_TOO_LONG, "default-too-long",
      "OBJECT IDENTIFIER DEFVALs must be expressed as a single identifier" },
    { 2, ERR_INVALID_SMIV1_ACCESS, "access-invalid-smiv1", 
      "Invalid access `%s' in SMIV1 style MIB" },
    { 2, ERR_INVALID_SMIV2_ACCESS, "access-invalid-smiv2",
      "Invalid access `%s' in SMIV2 style MIB" },
    { 2, ERR_SMIV2_WRITE_ONLY, "access-write-only-smiv2", 
      "Access `write-only' is no longer allowed in SMIv2" },
    { 4, ERR_SMIV1_WRITE_ONLY, "access-write-only-smiv1", 
      "Access `write-only' is not a good idea" },
    { 2, ERR_INVALID_NOTIFICATION_VARIATION_ACCESS, "variation-access-notification", 
      "Invalid access `%s' in a notification variation" },
    { 2, ERR_INVALID_OBJECT_VARIATION_ACCESS, "variation-access-object", 
      "Invalid access `%s' in an object variation" },
    { 2, ERR_INVALID_VARIATION_ACCESS, "variation-access", 
      "Invalid access `%s' in a variation" },
    { 2, ERR_NOTIFICATION_VARIATION_SYNTAX, "variation-syntax", 
      "SYNTAX is not allowed in a notification variation" },
    { 2, ERR_NOTIFICATION_VARIATION_WRITESYNTAX, "variation-writesyntax", 
      "WRITE-SYNTAX is not allowed in a notification variation" },
    { 2, ERR_DEFVAL_SYNTAX, "defval-syntax", 
      "DEFVAL syntax does not match object syntax" },
    { 2, ERR_NOTIFICATION_VARIATION_DEFVAL, "variation-defval", 
      "DEFVAL is not allowed in a notification variation" },
    { 2, ERR_NOTIFICATION_VARIATION_CREATION, "variation-creation", 
      "CREATION-REQUIRES is not allowed in a notification variation" },
    { 3, ERR_MODULE_IDENTITY_NOT_FIRST, "module-identity-not-first", 
      "The MODULE-IDENTITY clause must be the first declaration in a module" },
    { 2, ERR_INVALID_SMIV1_STATUS, "status-invalid-smiv1", 
      "Invalid STATUS `%s' in SMIv1 style MIB" },
    { 2, ERR_INVALID_SMIV2_STATUS, "status-invalid-smiv2", 
      "Invalid STATUS `%s' in SMIv2 style MIB" },
    { 2, ERR_INVALID_CAPABILITIES_STATUS, "status-invalid-capabilities", 
      "Invalid STATUS `%s' in AGENT-CAPABILITIES macro" },
    { 5, ERR_OIDNAME_INCLUDES_HYPHEN, "hyphen-in-object",
      "Object Identifier name `%s' may only include hyphens in SMIv1 converted SMIv2 MIBs" },
    { 2, ERR_ILLEGAL_CHAR_IN_STRING, "char-illegal-string", 
      "Illegal character `%c' (0x%2x) in quoted string" },
    { 2, ERR_BIN_STRING_MUL8, "", 
      "Binary string `%s' length is not a multiple of 8" },
    { 2, ERR_HEX_STRING_MUL2, "", 
      "Hexadecimal string `%s' length is not a multiple of 2" },
    { 5, ERR_FLUSH_DECLARATION, "", 
      "Flushing recent incorrect declaration, see previous error(s)" },
    { 2, ERR_MAX_ACCESS_IN_SMIV1, "", 
      "MAX-ACCESS is SMIv2 style, use ACCESS in SMIv1 style MIBs instead" },
    { 2, ERR_ACCESS_IN_SMIV2, "", 
      "ACCESS is SMIv1 style, use MAX-ACCESS in SMIv2 style MIBs instead" },
    { 5, ERR_UNWANTED_MODULE, "", 
      "Ignoring unwanted module `%s'" },
    { 1, ERR_MODULE_NOT_FOUND, "", 
      "Don't know where to find module `%s'" },
    { 2, ERR_OBJECT_IDENTIFIER_REGISTERED, "", 
      "Object identifier label `%s.%s' already registered at `%s'" },
    { 1, ERR_OPENING_INPUTFILE, "", 
      "Opening input file `%s': %s" },
    { 1, ERR_ILLEGAL_INPUTFILE, "", 
      "Opening input file `%s': unable to determine SMI version" },
    { 1, ERR_UNKNOWN_OIDLABEL, "", 
      "Unknown object identifier label `%s'" },
    { 2, ERR_SINGLE_SUBIDENTIFIER, "", 
      "A single number `%s' is no valid object identifier" },
    { 2, ERR_SUBIDENTIFIER_VS_OIDLABEL, "", 
      "Subidentifier `%s' doesn't match object identifier label `%s'" },
    { 2, ERR_OIDLABEL_DOESNOT_EXTEND, "", 
      "Object identifier label `%s' (%s) doesn't extend `%s'" },
    { 2, ERR_OIDLABEL_DOESNOT_EXTEND_1, "", 
      "Object identifier label `%s' (%s) doesn't extend `%s' by one subidentifier" },
    { 2, ERR_EXISTENT_OBJECT, "", 
      "An object named `%s' already exists" },
    { 2, ERR_IDENTIFIER_NOT_IN_MODULE, "", 
      "Identifier `%s' cannot be imported from module `%s'" },
    { 2, ERR_TYPE_ALREADY_EXISTS, "", 
      "Module `%s' already declared a type `%s'" },
    { 1, ERR_MACRO, "", 
      "MACRO definitions are just allowed in SMI base modules" },
    { 1, ERR_CHOICE, "", 
      "CHOICE type definitions are just allowed in SMI base modules" },
    { 1, ERR_TYPE_SMI, "", 
      "Type `%s' may only be defined in SMI base modules" },
    { 1, ERR_TYPE_TAG, "", 
      "Tagged or IMPLICIT types may only be defined in SMI base modules" },
    { 1, ERR_MACRO_ALREADY_EXISTS, "", 
      "Module `%s' already declared a macro `%s'" },
    { 1, ERR_EXPORTS, "", 
      "EXPORTS are just allowed in SMIv1 base modules" },
    { 1, ERR_ILLEGALLY_QUALIFIED, "", 
      "Illeagally qualified object identifier label `%s'" },
    { 2, ERR_MISSING_DESCRIPTION, "", 
      "Missing DESCRIPTION on SMIv2 OBJECT-TYPE" },
    { 2, ERR_OIDLABEL_NOT_FIRST, "", 
      "Object identifier element `%s' name only allowed as first element" },
    { 2, ERR_UNKNOWN_TYPE, "", 
      "Unknown type `%s'" },
    { 1, ERR_LOCATION, "", 
      "Opening MIB directory or file `%s': %s" },
    { 1, ERR_UNKNOWN_LOCATION_TYPE, "", 
      "Unknown MIB location type `%s'" },
    { 2, ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE, "", 
      "Illegal range restriction for non-numerical parent type `%s'" },
    { 1, ERR_SMIV2_SIGNED_NUMBER_RANGE, "", 
      "Number `%s' is out of SMIv1/SMIv2 signed number range" },
    { 1, ERR_SMIV2_UNSIGNED_NUMBER_RANGE, "", 
      "Number `%s' is out of SMIv1/SMIv2 unsigned number range" },
    { 1, ERR_INTEGER32_TOO_LARGE, "", 
      "Integer32 value `%u' is too large" },
    { 1, ERR_UNEXPECTED_VALUETYPE, "", 
      "type of value does not match declaration" },
    { 1, ERR_SMI_NOT_SUPPORTED, "", 
      "File `%s' seems to be SMIv1/v2 which is not supported" },
    { 1, ERR_SMING_NOT_SUPPORTED, "", 
      "File `%s' seems to be SMIng which is not supported" },
    { 6, ERR_UNUSED_IMPORT, "import-unused", 
      "Identifier `%s' imported from module `%s' is never used" },
    { 2, ERR_MACRO_NOT_IMPORTED, "", 
      "Macro `%s' has not been imported from module `%s'" },
    { 6, ERR_IMPLICIT_NODE, "", 
      "Implicit node definition" },
    { 3, ERR_SCALAR_READCREATE, "", 
      "Scalar object must not have a `read-create' access value" },
    { 4, ERR_NAMEDNUMBER_INCLUDES_HYPHEN, "", 
      "Named number `%s' must not include a hyphen in SMIv2" },
    { 4, ERR_NAMEDBIT_INCLUDES_HYPHEN, "", 
      "Named bit `%s' must not include a hyphen in SMIv2" },
    { 2, ERR_REDEFINITION, "", 
      "Redefinition of identifier `%s'" },
    { 5, ERR_CASE_REDEFINITION, "", 
      "Identifier `%s' differs from `%s' only in case" },
    { 2, ERR_INVALID_FORMAT, "", 
      "Invalid format specification `%s'" },
    { 3, ERR_REFINEMENT_ALREADY_EXISTS, "", 
      "A refinement for `%s' already exists in this compliance statement" },
    { 3, ERR_OPTIONALGROUP_ALREADY_EXISTS, "", 
      "An optional group definition for `%s' already exists in this compliance statement" },
    { 2, ERR_ILLEGAL_OID_DEFVAL, "", 
      "Cannot handle other default values than 0.0 for `%s'" },
    { 2, ERR_UNEXPECTED_TYPE_RESTRICTION, "", 
      "Unexpected type restriction" },
    { 1, ERR_UNKNOWN_CONFIG_CMD, "", 
      "Unknown configuration command `%s' in file `%s'" },
    { 4, ERR_OPAQUE_OBSOLETE, "opaque", 
      "SMIv2 provides Opaque solely for backward-compatibility" },
    { 2, ERR_DATE_CHARACTER, "date-character", 
      "Date specification `%s' contains an illegal character" },
    { 2, ERR_DATE_LENGTH, "date-length", 
      "Date specification `%s' has an illegal length" },
    { 2, ERR_DATE_VALUE, "date-value", 
      "Date specification `%s' contains an illegal value" },
    { 6, ERR_DATE_YEAR_2DIGITS, "date-year-2digits", 
      "Date specification `%s' contains a two-digit year representing `%d'" },
    { 2, ERR_DATE_YEAR, "date-year", 
      "Date specification `%s' contains an illegal year" },
    { 2, ERR_DATE_MONTH, "date-month", 
      "Date specification `%s' contains an illegal month" },
    { 2, ERR_DATE_DAY, "date-day", 
      "Date specification `%s' contains an illegal day" },
    { 2, ERR_DATE_HOUR, "date-hour", 
      "Date specification `%s' contains an illegal hour" },
    { 2, ERR_DATE_MINUTES, "date-minutes", 
      "Date specification `%s' contains illegal minutes" },
    { 4, ERR_DATE_IN_FUTURE, "date-in-future", 
      "Date specification `%s' is in the future" },
    { 4, ERR_DATE_IN_PAST, "date-in-past", 
      "Date specification `%s' predates the SMI standard" },
    { 6, ERR_INTEGER_IN_SMIV2, "integer-misuse", 
      "Use Integer32 instead of INTEGER in SMIv2" },
    { 5, ERR_MODULE_ALREADY_LOADED, "", 
      "The module `%s' is already loaded, aborting parser on this file" },
    { 2, ERR_BASETYPE_NOT_IMPORTED, "basetype-not-imported", 
      "The SMIv2 base type `%s' must be imported from SNMPv2-SMI" },
    { 2, ERR_BASETYPE_UNKNOWN, "basetype-unknown", 
      "The type `%s' of node `%s' does not resolve to a known base type" },
    { 2, ERR_ROW_SUBID_ONE, "row-node-subidentifier-one", 
      "The subidentifier of row node `%s' must be 1" },
    { 6, ERR_ROWNAME_ENTRY, "row-name-entry", 
      "The identifier of the row `%s' should match `*Entry'" },
    { 6, ERR_TABLENAME_TABLE, "table-name-table", 
      "The identifier of the table `%s' should match `*Table'" },
    { 6, ERR_SMIV2_TYPE_ASSIGNEMENT, "type-assignement-smiv2", 
      "Type assignement `%s' should be a textual convention in SMIv2" },
    { 6, ERR_ILLEGAL_IMPORT, "import-illegal", 
      "Identifier `%s' may not be imported from module `%s'" },
    { 4, ERR_SMIV2_NESTED_TEXTUAL_CONVENTION, "textual-convention-nested", 
      "Textual convention `%s' can not be derived from the textual convention `%s'" },
    { 3, ERR_TABLE_ACCESS, "access-table-illegal", 
      "Table node `%s' must by `not-accessible'" },
    { 3, ERR_ROW_ACCESS, "access-row-illegal", 
      "Row node `%s' must by `not-accessible'" },
    { 2, ERR_ENUM_SUBTYPE, "subtype-enumeration-illegal", 
      "Named number `%s' illegal in sub-type of `%s'" },
    { 2, ERR_BITS_SUBTYPE, "subtype-bits-illegal", 
      "Named number `%s' illegal in sub-type of `%s'" },
    { 2, ERR_ENUM_NAME_REDEFINITION, "enum-name-redefinition", 
      "Redefinition of name `%s' in number enumeration" },
    { 2, ERR_ENUM_NUMBER_REDEFINITION, "enum-number-redefinition", 
      "Redefinition of number `%d' in number enumeration" },
    { 2, ERR_BITS_NAME_REDEFINITION, "bits-name-redefinition", 
      "Redefinition of name `%s' in named bits list" },
    { 2, ERR_BITS_NUMBER_REDEFINITION, "bits-number-redefinition", 
      "Redefinition of number `%u' in named bits list" },
    { 2, ERR_BITS_NUMBER_TOO_LARGE, "bits-number-too-large", 
      "Named bit `%s(%u)' exceeds maximum bit position" },
    { 4, ERR_BITS_NUMBER_LARGE, "bits-number-large", 
      "Named bit `%s(%u)' may cause interoperability or implementation problems" },
 
    { 0, 0, NULL, NULL }
};



/*
 *----------------------------------------------------------------------
 *
 * errorSeverity --
 *
 *      Sets the severity of errors with tags matching pattern.
 *
 * Results:
 *      TODO.
 *
 * Side effects:
 *      TODO.
 *
 *----------------------------------------------------------------------
 */

void
errorSeverity(char *pattern, int severity)
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
 * printError --
 *
 *      A more verbose wrapper for the yyerror() function.
 *
 * Results:
 *      TODO.
 *
 * Side effects:
 *      TODO.
 *
 *----------------------------------------------------------------------
 */

void
printError(Parser *parser, int id, ...)
{
    va_list ap;
    char *fmt;
	
    if (parser) {
	if ((errors[id].level <= errorLevel) &&
	    (parser->flags & SMI_FLAG_ERRORS) &&
	    ((smiDepth == 1) || (parser->flags & SMI_FLAG_RECURSIVE))) {
	    fprintf(stderr, "%s:%d: ", parser->path, parser->line);
	    fmt = errors[id].fmt;
	    va_start(ap, id);
	    vfprintf(stderr, fmt, ap);
	    fprintf(stderr, "\n");
	    va_end(ap);
	}
    } else {
	if (errors[id].level <= errorLevel) {
	    fmt = errors[id].fmt;
	    va_start(ap, id);
	    vfprintf(stderr, fmt, ap);
	    fprintf(stderr, "\n");
	    va_end(ap);
	}
    }

    if (errors[id].level < 0) {
	exit(-1);
	/* severe error, no way to continue :-( */
    }
}



/*
 *----------------------------------------------------------------------
 *
 * printErrorAtLine --
 *
 *      Like printError() but shows a specfic line no.
 *
 * Results:
 *      TODO.
 *
 * Side effects:
 *      TODO.
 *
 *----------------------------------------------------------------------
 */

void
printErrorAtLine(Parser *parser, int id, int line, ...)
{
    va_list ap;
    char *fmt;
	
    if (parser) {
	if ((errors[id].level <= errorLevel) &&
	    (parser->flags & SMI_FLAG_ERRORS) &&
	    ((smiDepth == 1) || (parser->flags & SMI_FLAG_RECURSIVE))) {
	    fprintf(stderr, "%s:%d: ", parser->path, line);
	    fmt = errors[id].fmt;
	    va_start(ap, line);
	    vfprintf(stderr, fmt, ap);
	    fprintf(stderr, "\n");
	    va_end(ap);
	}
    } else {
	if (errors[id].level <= errorLevel) {
	    fmt = errors[id].fmt;
	    va_start(ap, line);
	    vfprintf(stderr, fmt, ap);
	    fprintf(stderr, "\n");
	    va_end(ap);
	}
    }

    if (errors[id].level < 0) {
	exit(-1);
	/* severe error, no way to continue :-( */
    }
}

