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
 * @(#) $Id: error.c,v 1.11 1999/06/06 07:40:38 strauss Exp $
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#include "error.h"
#include "scanner-smi.h"
#include "parser-smi.h"


#define thisParser      ((Parser *)parser)



int errorLevel;                          /* Higher level for more warnings   */

typedef struct Error {
    int level;			/* 0: fatal, no way to continie		     */
				/* 1: severe, changing semantics to continue */
				/*    must be corrected                      */
				/* 2: error, but able to continue,           */
				/*    should be corrected                    */
				/* 3: minor error, but should be corrected   */
				/* 4: change is recommended if possible      */
				/* 5: warning, but might totally correct     */
				/*    under some circumstances               */
    int id;			/* error id, used in error() invocation	     */
    char *fmt;			/* a format string			     */
} Error;


/*
 * Note: The Makefile produced a list of error macros for every `ERR...'
 * pattern in this file (error.c). This list is written to error.i.
 */

Error errors[] = {
    { 0, ERR_INTERNAL,
      "Internal error!!!" },
    { 1, ERR_ILLEGAL_KEYWORD,
      "Illegal keyword `%s'" },
    { 2, ERR_ID_ENDS_IN_HYPHEN,
      "Identifier `%s' illegally ends in a hyphen" },
    { 3, ERR_LEADING_ZEROS,
      "Leading zero(s) on a number" },
    { 1, ERR_NUMBER_TO_LARGE,
      "Number `%s' too large" },
    { 1, ERR_HEX_ENDS_IN_B,
      "Hexadecimal string terminated by binary string delimiter, assume hex value" },
    { 1, ERR_LEX_UNEXPECTED_CHAR,
      "Lexically unexpected character (internal error!)" },
    { 1, ERR_MODULENAME_64,
      "Module name `%s' must not be longer that 64 characters" },
    { 4, ERR_MODULENAME_32,
      "Module name `%s' longer than 32 characters, which is not recommended" },
    { 1, ERR_TYPENAME_64,
      "Type name `%s' must not be longer that 64 characters" },
    { 4, ERR_TYPENAME_32,
      "Type name `%s' longer than 32 characters, which is not recommended" },
    { 1, ERR_OIDNAME_64,
      "Object Identifier name `%s' must not be longer that 64 characters" },
    { 4, ERR_OIDNAME_32,
      "Object Identifier name `%s' longer than 32 characters, which is not recommended" },
    { 1, ERR_ENUMNAME_64,
      "Enumeration item name `%s' must not be longer that 64 characters" },
    { 4, ERR_ENUMNAME_32,
      "Enumeration item name `%s' longer than 32 characters, which is not recommended" },
    { 1, ERR_BITNAME_64,
      "Bit name `%s' must not be longer than 64 characters" },
    { 4, ERR_BITNAME_32,
      "Bit name `%s' longer than 32 characters, which is not recommended" },
    { 2, ERR_TRAP_TYPE,
      "The TRAP-TYPE macro is not allowed in SMIv2 style MIB" },
    { 2, ERR_TOO_MANY_MODULE_IDENTITIES,
      "There must be no more than exactly one MODULE-IDENTITY clause" },
    { 2, ERR_NO_MODULE_IDENTITY,
      "Missing MODULE-IDENTITY clause in SMIv2 style MIB" },
    { 4, ERR_OID_DEFVAL_TOO_LONG,
      "OBJECT IDENTIFIER DEFVALs must be expressed as a single identifier" },
    { 2, ERR_INVALID_SMIV1_ACCESS,
      "Invalid ACCESS `%s' in SMIV1 style MIB" },
    { 2, ERR_INVALID_SMIV2_ACCESS,
      "Invalid ACCESS `%s' in SMIV2 style MIB" },
    { 3, ERR_WRITE_ONLY_ACCESS,
      "ACCESS `write-only' is only allowed for backward compatibility in SMIv2" },
    { 3, ERR_MODULE_IDENTITY_NOT_FIRST,
      "The MODULE-IDENTITY clause must be the first declaration in a module" },
    { 2, ERR_INVALID_SMIV1_STATUS,
      "Invalid STATUS `%s' in SMIv1 style MIB" },
    { 2, ERR_INVALID_SMIV2_STATUS,
      "Invalid STATUS `%s' in SMIv2 style MIB" },
    { 2, ERR_INVALID_CAPABILITIES_STATUS,
      "Invalid STATUS `%s' in AGENT-CAPABILITIES macro" },
    { 5, ERR_OIDNAME_INCLUDES_HYPHEN,
      "Object Identifier name `%s' may only include hyphens in SMIv1 converted SMIv2 MIBs" },
    { 2, ERR_ILLEGAL_CHAR_IN_STRING,
      "Illegal character in quoted string (%s)" },
    { 2, ERR_BIN_STRING_MUL8,
      "Binary string `%s' length is not a multiple of 8" },
    { 2, ERR_HEX_STRING_MUL2,
      "Haxadecimal string `%s' length is not a multiple of 2" },
    { 5, ERR_FLUSH_DECLARATION,
      "Flushing recent incorrect declaration, see previous error(s)" },
    { 1, ERR_OTHER_ERROR,
      "%s" },
    { 2, ERR_MAX_ACCESS_IN_SMIV1,
      "MAX-ACCESS is SMIv2 style, use ACCESS in SMIv1 style MIBs instead" },
    { 2, ERR_ACCESS_IN_SMIV2,
      "ACCESS is SMIv1 style, use MAX-ACCESS in SMIv2 style MIBs instead" },
    { 5, ERR_UNWANTED_MODULE,
      "Ignoring unwanted module `%s'" },
    { 0, ERR_MAX_LEX_DEPTH,
      "Maximum IMPORTS nesting, probably a loop?" },
    { 1, ERR_MODULE_NOT_FOUND,
      "Don't know where to find module `%s'" },
    { 9, ERR_INCLUDE,
      "including module `%s' from `%s'..." },
    { 9, ERR_STATISTICS,
      "completed%s" },
    { 1, ERR_OBJECT_IDENTIFIER_REGISTERED,
      "Object identifier label `%s.%s' already registered at `%s'" },
    { 1, ERR_ALLOCATING_OBJECT,
      "Allocating MIB object: %s" },
    { 1, ERR_OPENING_CONFIGFILE,
      "Opening configuration file `%s': %s" },
    { 1, ERR_ALLOCATING_LOCATION,
      "Allocating location list entry: %s" },
    { 1, ERR_ALLOCATING_DESCRIPTOR,
      "Allocating descriptor: %s" },
    { 1, ERR_OPENING_INPUTFILE,
      "Opening input file `%s': %s" },
    { 1, ERR_OPENING_IMPORTFILE,
      "Opening imported file `%s': %s" },
    { 1, ERR_ALLOCATING_MIBMODULE,
      "Allocating MIB module: %s" },
    { 1, ERR_UNKNOWN_OIDLABEL,
      "Unknown object identifier label `%s'" },
    { 1, ERR_SINGLE_SUBIDENTIFIER,
      "A single number `%s' is no valid object identifier" },
    { 1, ERR_SUBIDENTIFIER_VS_OIDLABEL,
      "Subidentifier `%s' doesn't match object identifier label `%s'" },
    { 1, ERR_OIDLABEL_DOESNOT_EXTEND,
      "Object identifier label `%s' (%s) doesn't extend `%s'" },
    { 1, ERR_OIDLABEL_DOESNOT_EXTEND_1,
      "Object identifier label `%s' (%s) doesn't extend `%s' by one subidentifier" },
    { 1, ERR_EXISTENT_OBJECT,
      "An object named `%s' already exists" },
    { 1, ERR_UNKNOWN_CONFIG_DIRECTIVE,
      "%s: Unknown directive `%s'" },
    { 1, ERR_ALLOCATING_PENDING_MIBNODE,
      "Allocating pending MIB node: %s" },
    { 1, ERR_ALLOCATING_IMPORT,
      "Allocating import identifier: %s" },
    { 1, ERR_IDENTIFIER_NOT_IN_MODULE,
      "Identifier `%s' cannot be imported from module `%s'" },
    { 1, ERR_TYPE_ALREADY_EXISTS,
      "Module `%s' already declared a type `%s'" },
    { 1, ERR_ALLOCATING_TYPE,
      "Allocating Type: %s" },
    { 1, ERR_MACRO,
      "MACRO definitions are just allowed in SMI base modules" },
    { 1, ERR_CHOICE,
      "CHOICE type definitions are just allowed in SMI base modules" },
    { 1, ERR_TYPE_SMI,
      "Type `%s' may only be defined in SMI base modules" },
    { 1, ERR_TYPE_TAG,
      "Tagged or IMPLICIT types may only be defined in SMI base modules" },
    { 1, ERR_MACRO_ALREADY_EXISTS,
      "Module `%s' already declared a macro `%s'" },
    { 1, ERR_ALLOCATING_MACRO,
      "Allocating Macro: %s" },
    { 1, ERR_EXPORTS,
      "EXPORTS are just allowed in SMIv1 base modules" },
    { 1, ERR_ILLEGALLY_QUALIFIED,
      "Illeagally qualified object identifier label `%s'" },
    { 2, ERR_MISSING_DESCRIPTION,
      "Missing DESCRIPTION on SMIv2 OBJECT-TYPE" },
    { 2, ERR_OIDLABEL_NOT_FIRST,
      "Object identifier element `%s' name only allowed as first element" },
    { 2, ERR_UNKNOWN_TYPE,
      "Unknown type `%s'" },
    { 1, ERR_LOCATION,
      "Opening MIB directory or file `%s': %s" },
    { 1, ERR_UNKNOWN_LOCATION_TYPE,
      "Unknown MIB location type `%s'" },
    { 1, ERR_ALLOCATING_NODE,
      "Allocating MIB object node: %s" },
    { 1, ERR_ALLOCATING_VIEW,
      "Allocating MIB module view: %s" },
    { 1, ERR_ALLOCATING_REVISION,
      "Allocating MIB module revision: %s" },
    { 1, ERR_OUT_OF_NUMBER_RANGE,
      "number `%s' is out of SMIv2 value range" },
    { 1, ERR_UNEXPECTED_VALUETYPE,
      "type of value does not match declaration" },
    { 1, ERR_SMI_NOT_SUPPORTED,
      "File `%s' seems to be SMIv1/v2 which is not supported" },
    { 1, ERR_SMING_NOT_SUPPORTED,
      "File `%s' seems to be SMIng which is not supported" },
 
    { 0, 0, NULL }
};



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
	    (thisParser->flags & FLAG_ERRORS)) {
#if 0
	    /*
	     * TODO: This would indent output for imported files, but
	     * inhibits Emacs compilation-next-error from working. :-)
	     */
	    { int i; for (i=1 ; i < lexDepth ; i++) fprintf(stderr, ">"); }
#endif
	    fprintf(stderr, "%s:%d: ", thisParser->path, thisParser->line);
	    
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

    if (errors[id].level <= 0) {
	exit(-1);
	/* severe error, no way to continue :-( */
    }
}

