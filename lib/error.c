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
 * @(#) $Id: error.c,v 1.47 2000/06/16 13:53:57 strauss Exp $
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

#include "smi.h"
#include "error.h"


int smiErrorLevel;		/* Higher levels produce more warnings */



/*
 * The following function pointer marks the current error printing
 * function. This may be overwritten by a language specific function
 * in case the library is embedded into other programs.
 */

static void smiErrorHandler(char *path, int line, int severity, char *msg);
static SmiErrorHandler *handler = smiErrorHandler;



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
} Error;



/*
 * Note: The Makefile produces a list of error macros for every `ERR...'
 * pattern in this file (error.c). This list is written to errormacros.h.
 */

static Error errors[] = {
    { 0, ERR_INTERNAL, "internal", 
      "internal error!!!" },
    { 0, ERR_MAX_LEX_DEPTH, "", 
      "maximum IMPORTS nesting, probably a loop?" },
    { 0, ERR_LEX_UNEXPECTED_CHAR, "lexical", 
      "lexically unexpected character (internal error!)" },
    { 0, ERR_OUT_OF_MEMORY, "memory", 
      "out of memory (internal error!)" },
    { 1, ERR_OTHER_ERROR, "other", 
      "%s" },
    { 1, ERR_ILLEGAL_KEYWORD, "keyword-illegal", 
      "illegal keyword `%s'" },
    { 2, ERR_ID_ENDS_IN_HYPHEN, "hyphen-end",
      "identifier `%s' illegally ends in a hyphen" },
    { 3, ERR_LEADING_ZEROS, "number-leading-zero",
      "leading zero(s) on a number" },
    { 2, ERR_NUMBER_TOO_LARGE, "number-range",
      "number `%s' too large" },
    { 2, ERR_HEX_ENDS_IN_B, "string-delimiter",
      "hexadecimal string terminated by binary string delimiter, assume hex value" },
    { 2, ERR_MODULENAME_64, "namelength-64-module",
      "module name `%s' must not be longer that 64 characters" },
    { 4, ERR_MODULENAME_32, "namelength-32-module",
      "module name `%s' longer than 32 characters" },
    { 2, ERR_TYPENAME_64, "namelength-64-type",
      "type name `%s' must not be longer that 64 characters" },
    { 4, ERR_TYPENAME_32, "namelength-32-type",
      "type name `%s' longer than 32 characters" },
    { 2, ERR_OIDNAME_64, "namelength-64-object",
      "object identifier name `%s' must not be longer that 64 characters" },
    { 4, ERR_OIDNAME_32, "namelength-32-object",
      "object identifier name `%s' longer than 32 characters" },
    { 2, ERR_ENUMNAME_64, "namelength-64-enumeration",
      "enumeration name `%s' must not be longer that 64 characters" },
    { 4, ERR_ENUMNAME_32, "namelength-32-enumeration",
      "enumeration name `%s' longer than 32 characters" },
    { 2, ERR_BITNAME_64, "namelength-64-bit",
      "bit name `%s' must not be longer than 64 characters" },
    { 4, ERR_BITNAME_32, "namelength-32-bit",
      "bit name `%s' longer than 32 characters" },
    { 2, ERR_UCIDENTIFIER_64, "namelength-64-uc-identifier",
      "uppercase identifier `%s' must not be longer than 64 characters" },
    { 2, ERR_LCIDENTIFIER_64, "namelength-64-lc-identifier",
      "lowercase identifier `%s' must not be longer than 64 characters" },
    { 2, ERR_TRAP_TYPE, "trap",
      "TRAP-TYPE macro is not allowed in SMIv2 style MIB" },
    { 2, ERR_TOO_MANY_MODULE_IDENTITIES, "module-identity-multiple",
      "more than one MODULE-IDENTITY clause in SMIv2 MIB" },
    { 2, ERR_NO_MODULE_IDENTITY, "module-identity-missing",
      "missing MODULE-IDENTITY clause in SMIv2 MIB" },
    { 4, ERR_OID_DEFVAL_TOO_LONG, "default-too-long",
      "OBJECT IDENTIFIER DEFVALs must be expressed as a single identifier" },
    { 2, ERR_INVALID_SMIV1_ACCESS, "access-invalid-smiv1", 
      "invalid access `%s' in SMIv1 MIB" },
    { 2, ERR_INVALID_SMIV2_ACCESS, "access-invalid-smiv2",
      "invalid access `%s' in SMIv2 MIB" },
    { 2, ERR_SMIV2_WRITE_ONLY, "access-write-only-smiv2", 
      "access `write-only' is no longer allowed in SMIv2" },
    { 4, ERR_SMIV1_WRITE_ONLY, "access-write-only-smiv1", 
      "access `write-only' is not a good idea" },
    { 2, ERR_INVALID_NOTIFICATION_VARIATION_ACCESS, "variation-access-notification", 
      "invalid access `%s' in a notification variation" },
    { 2, ERR_INVALID_OBJECT_VARIATION_ACCESS, "variation-access-object", 
      "invalid access `%s' in an object variation" },
    { 2, ERR_INVALID_VARIATION_ACCESS, "variation-access", 
      "invalid access `%s' in a variation" },
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
      "MODULE-IDENTITY clause must be the first declaration in a module" },
    { 2, ERR_INVALID_SMIV1_STATUS, "status-invalid-smiv1", 
      "invalid STATUS `%s' in SMIv1 MIB" },
    { 2, ERR_INVALID_SMIV2_STATUS, "status-invalid-smiv2", 
      "invalid STATUS `%s' in SMIv2 MIB" },
    { 2, ERR_INVALID_CAPABILITIES_STATUS, "status-invalid-capabilities", 
      "invalid STATUS `%s' in AGENT-CAPABILITIES macro" },
    { 5, ERR_OIDNAME_INCLUDES_HYPHEN, "hyphen-in-object",
      "Object Identifier name `%s' may only include hyphens in SMIv1 converted SMIv2 MIBs" },
    { 2, ERR_ILLEGAL_CHAR_IN_STRING, "char-illegal-string", 
      "illegal character `%c' (0x%2x) in quoted string" },
    { 2, ERR_BIN_STRING_MUL8, "", 
      "length of binary string `%s' is not a multiple of 8" },
    { 2, ERR_HEX_STRING_MUL2, "", 
      "length of hexadecimal string `%s' is not a multiple of 2" },
    { 5, ERR_FLUSH_DECLARATION, "", 
      "flushing recent incorrect declaration, see previous error(s)" },
    { 2, ERR_MAX_ACCESS_IN_SMIV1, "", 
      "MAX-ACCESS is SMIv2 style, use ACCESS in SMIv1 MIBs instead" },
    { 2, ERR_ACCESS_IN_SMIV2, "", 
      "ACCESS is SMIv1 style, use MAX-ACCESS in SMIv2 MIBs instead" },
    { 5, ERR_UNWANTED_MODULE, "", 
      "ignoring unwanted module `%s'" },
    { 1, ERR_MODULE_NOT_FOUND, "", 
      "failed to locate MIB module `%s'" },
    { 2, ERR_OBJECT_IDENTIFIER_REGISTERED, "", 
      "Object identifier label `%s.%s' already registered at `%s'" },
    { 1, ERR_OPENING_INPUTFILE, "", 
      "%s: %s" },
    { 1, ERR_ILLEGAL_INPUTFILE, "", 
      "%s: unable to determine SMI version" },
    { 1, ERR_UNKNOWN_OIDLABEL, "", 
      "unknown object identifier label `%s'" },
    { 2, ERR_SINGLE_SUBIDENTIFIER, "", 
      "single number `%s' is not a valid object identifier" },
    { 2, ERR_SUBIDENTIFIER_VS_OIDLABEL, "", 
      "subidentifier `%s' does not match object identifier label `%s'" },
    { 2, ERR_OIDLABEL_DOESNOT_EXTEND, "", 
      "Object identifier label `%s' (%s) doesn't extend `%s'" },
    { 2, ERR_EXISTENT_OBJECT, "", 
      "an object named `%s' already exists" },
    { 2, ERR_IDENTIFIER_NOT_IN_MODULE, "", 
      "identifier `%s' cannot be imported from module `%s'" },
    { 1, ERR_MACRO, "", 
      "MACRO definitions are only allowed in SMI base modules" },
    { 1, ERR_CHOICE, "", 
      "CHOICE type definitions are only allowed in SMI base modules" },
    { 1, ERR_TYPE_SMI, "", 
      "type `%s' may only be defined in SMI base modules" },
    { 1, ERR_TYPE_TAG, "", 
      "tagged or IMPLICIT types may only be defined in SMI base modules" },
    { 1, ERR_MACRO_ALREADY_EXISTS, "", 
      "module `%s' already declared a macro `%s'" },
    { 1, ERR_EXPORTS, "", 
      "EXPORTS are only allowed in SMIv1 base modules" },
    { 1, ERR_ILLEGALLY_QUALIFIED, "", 
      "illegally qualified object identifier label `%s'" },
    { 2, ERR_MISSING_DESCRIPTION, "", 
      "missing DESCRIPTION on SMIv2 OBJECT-TYPE" },
    { 2, ERR_OIDLABEL_NOT_FIRST, "", 
      "Object identifier element `%s' name only allowed as first element" },
    { 2, ERR_UNKNOWN_TYPE, "", 
      "unknown type `%s'" },
    { 1, ERR_LOCATION, "", 
      "opening MIB directory or file `%s': %s" },
    { 1, ERR_UNKNOWN_LOCATION_TYPE, "", 
      "unknown MIB location type `%s'" },
    { 2, ERR_ILLEGAL_RANGE_FOR_PARENT_TYPE, "", 
      "illegal range restriction for non-numerical parent type `%s'" },
    { 1, ERR_SMIV2_SIGNED_NUMBER_RANGE, "", 
      "number `%s' is out of SMIv1/SMIv2 signed number range" },
    { 1, ERR_SMIV2_UNSIGNED_NUMBER_RANGE, "", 
      "number `%s' is out of SMIv1/SMIv2 unsigned number range" },
    { 1, ERR_INTEGER32_TOO_LARGE, "", 
      "Integer32 value `%u' is too large" },
    { 1, ERR_UNEXPECTED_VALUETYPE, "", 
      "type of value does not match declaration" },
    { 1, ERR_SMI_NOT_SUPPORTED, "", 
      "file `%s' seems to be SMIv1 or SMIv2 which is not supported" },
    { 1, ERR_SMING_NOT_SUPPORTED, "", 
      "file `%s' seems to be SMIng which is not supported" },
    { 6, ERR_UNUSED_IMPORT, "import-unused", 
      "identifier `%s' imported from module `%s' is never used" },
    { 2, ERR_MACRO_NOT_IMPORTED, "", 
      "macro `%s' has not been imported from module `%s'" },
    { 6, ERR_IMPLICIT_NODE, "", 
      "implicit node definition" },
    { 3, ERR_SCALAR_READCREATE, "", 
      "scalar object must not have a `read-create' access value" },
    { 4, ERR_NAMEDNUMBER_INCLUDES_HYPHEN, "", 
      "named number `%s' must not include a hyphen in SMIv2" },
    { 4, ERR_NAMEDBIT_INCLUDES_HYPHEN, "", 
      "named bit `%s' must not include a hyphen in SMIv2" },
    { 2, ERR_REDEFINITION, "", 
      "redefinition of identifier `%s'" },
    { 6, ERR_EXT_REDEFINITION, "", 
      "redefinition of identifier `%s' (%s)" },
    { 5, ERR_CASE_REDEFINITION, "", 
      "identifier `%s' differs from `%s' only in case" },
    { 6, ERR_EXT_CASE_REDEFINITION, "", 
      "identifier `%s' differs from `%s' only in case (%s)" },
    { 6, ERR_PREVIOUS_DEFINITION, "",
      "previous definition of `%s'" },
    { 2, ERR_INVALID_FORMAT, "", 
      "invalid format specification `%s'" },
    { 3, ERR_REFINEMENT_ALREADY_EXISTS, "", 
      "refinement for `%s' already exists in this compliance statement" },
    { 3, ERR_OPTIONALGROUP_ALREADY_EXISTS, "", 
      "optional group definition for `%s' already exists in this compliance statement" },
    { 2, ERR_ILLEGAL_OID_DEFVAL, "", 
      "cannot handle other default values than 0.0 for `%s'" },
    { 2, ERR_UNEXPECTED_TYPE_RESTRICTION, "", 
      "unexpected type restriction" },
    { 1, ERR_UNKNOWN_CONFIG_CMD, "", 
      "unknown configuration command `%s' in file `%s'" },
    { 4, ERR_OPAQUE_OBSOLETE, "opaque", 
      "SMIv2 provides Opaque solely for backward-compatibility" },
    { 2, ERR_DATE_CHARACTER, "date-character", 
      "date specification `%s' contains an illegal character" },
    { 2, ERR_DATE_LENGTH, "date-length", 
      "date specification `%s' has an illegal length" },
    { 2, ERR_DATE_VALUE, "date-value", 
      "date specification `%s' contains an illegal value" },
    { 6, ERR_DATE_YEAR_2DIGITS, "date-year-2digits", 
      "date specification `%s' contains a two-digit year representing `%d'" },
    { 2, ERR_DATE_YEAR, "date-year", 
      "date specification `%s' contains an illegal year" },
    { 2, ERR_DATE_MONTH, "date-month", 
      "date specification `%s' contains an illegal month" },
    { 2, ERR_DATE_DAY, "date-day", 
      "date specification `%s' contains an illegal day" },
    { 2, ERR_DATE_HOUR, "date-hour", 
      "date specification `%s' contains an illegal hour" },
    { 2, ERR_DATE_MINUTES, "date-minutes", 
      "date specification `%s' contains illegal minutes" },
    { 4, ERR_DATE_IN_FUTURE, "date-in-future", 
      "date specification `%s' is in the future" },
    { 4, ERR_DATE_IN_PAST, "date-in-past", 
      "date specification `%s' predates the SMI standard" },
    { 6, ERR_INTEGER_IN_SMIV2, "integer-misuse", 
      "use Integer32 instead of INTEGER in SMIv2" },
    { 5, ERR_MODULE_ALREADY_LOADED, "", 
      "module `%s' is already loaded, aborting parser on this file" },
    { 2, ERR_BASETYPE_NOT_IMPORTED, "basetype-not-imported", 
      "SMIv2 base type `%s' must be imported from SNMPv2-SMI" },
    { 2, ERR_BASETYPE_UNKNOWN, "basetype-unknown", 
      "type `%s' of node `%s' does not resolve to a known base type" },
    { 2, ERR_ROW_SUBID_ONE, "row-node-subidentifier-one", 
      "subidentifier of row node `%s' must be 1" },
    { 6, ERR_ROWNAME_ENTRY, "row-name-entry", 
      "row identifier `%s' should match `*Entry'" },
    { 6, ERR_TABLENAME_TABLE, "table-name-table", 
      "table identifier `%s' should match `*Table'" },
    { 6, ERR_SMIV2_TYPE_ASSIGNEMENT, "type-assignement-smiv2", 
      "type assignement `%s' should be a textual convention in SMIv2" },
    { 6, ERR_ILLEGAL_IMPORT, "import-illegal", 
      "identifier `%s' may not be imported from module `%s'" },
    { 4, ERR_SMIV2_NESTED_TEXTUAL_CONVENTION, "textual-convention-nested", 
      "textual convention `%s' can not be derived from the textual convention `%s'" },
    { 3, ERR_TABLE_ACCESS, "access-table-illegal", 
      "table node `%s' must by `not-accessible'" },
    { 3, ERR_ROW_ACCESS, "access-row-illegal", 
      "row node `%s' must by `not-accessible'" },
    { 2, ERR_ENUM_SUBTYPE, "subtype-enumeration-illegal", 
      "named number `%s' illegal in sub-type of `%s'" },
    { 2, ERR_BITS_SUBTYPE, "subtype-bits-illegal", 
      "named number `%s' illegal in sub-type of `%s'" },
    { 2, ERR_ENUM_NAME_REDEFINITION, "enum-name-redefinition", 
      "redefinition of name `%s' in number enumeration" },
    { 2, ERR_ENUM_NUMBER_REDEFINITION, "enum-number-redefinition", 
      "redefinition of number `%d' in number enumeration" },
    { 2, ERR_BITS_NAME_REDEFINITION, "bits-name-redefinition", 
      "redefinition of name `%s' in named bits list" },
    { 2, ERR_BITS_NUMBER_REDEFINITION, "bits-number-redefinition", 
      "redefinition of number `%u' in named bits list" },
    { 2, ERR_BITS_NUMBER_TOO_LARGE, "bits-number-too-large", 
      "named bit `%s(%u)' exceeds maximum bit position" },
    { 4, ERR_BITS_NUMBER_LARGE, "bits-number-large", 
      "named bit `%s(%u)' may cause interoperability or implementation problems" },
    { 2, ERR_RANGE_OUT_OF_BASETYPE, "range-bounds",
      "range limit exceeds underlying basetype" },
    { 2, ERR_RANGE_OVERLAP, "range-overlap",
      "overlapping range limits" },
    { 6, ERR_RANGES_NOT_ASCENDING, "range-ascending",
      "ranges not in ascending order" },
    { 2, ERR_EXCHANGED_RANGE_LIMITS, "range-exchanged",
      "range limits must be `lower-bound .. upper-bound'" },
    { 1, ERR_INDEX_BASETYPE, "index-illegal-basetype", 
      "illegal base type `%s' in index element `%s' of row %s" },
    { 5, ERR_INDEX_TOO_LARGE, "index-exceeds-too-large", 
      "index of row `%s' can exceed OID size limit" },
    { 1, ERR_INDEX_NO_RANGE, "index-element-no-range",
      "index element `%s' of row `%s' must have a range restriction" },
    { 1, ERR_INDEX_NO_SIZE, "index-element-no-size",
      "index element `%s' of row `%s' must have a size restriction" },
    { 1, ERR_INDEX_NEGATIVE, "index-element-negative",
      "range restriction of index element `%s' of row `%s' must be non-negative" },
    { 6, ERR_EMPTY_DESCRIPTION, "empty-description",
      "zero length description of `%s'" },
    { 6, ERR_INDEX_NOT_COLUMN, "index-element-not-column",
      "index element `%s' of row `%s' must be a column" },

    { 0, 0, NULL, NULL }
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
    handler = smiErrorHandler;
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
    return errors[id].tag ? errors[id].tag : "";
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
    return errors[id].fmt ? errors[id].fmt : "";
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

static void
smiErrorHandler(char *path, int line, int severity, char *msg)
{
    if (path) {
	fprintf(stderr, "%s:%d: ", path, line);
    }
    fprintf(stderr, "%s\n", msg);
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
    char buffer[1024];
    
    if (! handler) {
	return;
    }

    if (parser) {
	if ((errors[id].level <= smiErrorLevel) &&
	    (parser->flags & SMI_FLAG_ERRORS) &&
	    ((smiDepth == 1) || (parser->flags & SMI_FLAG_RECURSIVE))) {
#ifdef HAVE_VSNPRINTF
	    vsnprintf(buffer, sizeof(buffer), errors[id].fmt, ap);
#else
	    vsprintf(buffer, errors[id].fmt, ap);	/* buffer overwrite */
#endif
	    (handler) (parser->path, line, errors[id].level, buffer);
	}
    } else {
	if (errors[id].level <= smiErrorLevel) {
#ifdef HAVE_VSNPRINTF
	    vsnprintf(buffer, sizeof(buffer), errors[id].fmt, ap);
#else
	    vsprintf(buffer, errors[id].fmt, ap);	/* buffer overwrite */
#endif
	    (handler) (NULL, 0, errors[id].level, buffer);
	}
    }

    /*
     * A severe error, no way to continue :-(
     *
     * TODO: Give the application a chance to overwrite the exit()
     * call so that it can at least do some cleanup.
     */

    if (errors[id].level <= 0) {
	exit(-1);
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
} /*  */
