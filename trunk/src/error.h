/*
 * error.h --
 *
 *      Definitions for error handling.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: error.h,v 1.1.1.1 1998/10/09 10:16:33 strauss Exp $
 */

#ifndef _ERROR_H
#define _ERROR_H

#include "scanner.h"
#include "parser.h"

#define ERR_INTERNAL                             0
#define ERR_ILLEGAL_KEYWORD                      1
#define ERR_ID_ENDS_IN_HYPHEN                    2
#define ERR_LEADING_ZEROS                        3
#define ERR_NUMBER_TO_LARGE                      4
#define ERR_HEX_ENDS_IN_B                        5
#define ERR_LEX_UNEXPECTED_CHAR                  6
#define ERR_MODULENAME_64                        7
#define ERR_MODULENAME_32                        8
#define ERR_TYPENAME_64                          9
#define ERR_TYPENAME_32                          10
#define ERR_OIDNAME_64                           11
#define ERR_OIDNAME_32                           12
#define ERR_ENUMNAME_64                          13
#define ERR_ENUMNAME_32                          14
#define ERR_BITNAME_64                           15
#define ERR_BITNAME_32                           16
#define ERR_TRAP_TYPE                            17
#define ERR_TOO_MANY_MODULE_IDENTITIES           18
#define ERR_NO_MODULE_IDENTITY                   19
#define ERR_OID_DEFVAL_TOO_LONG                  20
#define ERR_INVALID_SMIV1_ACCESS                 21
#define ERR_INVALID_SMIV2_ACCESS                 22
#define ERR_WRITE_ONLY_ACCESS                    23
#define ERR_MODULE_IDENTITY_NOT_FIRST            24
#define ERR_INVALID_SMIV1_STATUS                 25
#define ERR_INVALID_SMIV2_STATUS                 26
#define ERR_INVALID_CAPABILITIES_STATUS          27
#define ERR_OIDNAME_INCLUDES_HYPHEN              28
#define ERR_ILLEGAL_CHAR_IN_STRING               29
#define ERR_BIN_STRING_MUL8			 30
#define ERR_HEX_STRING_MUL8			 31
#define ERR_FLUSH_DECLARATION			 32
#define ERR_OTHER_ERROR				 33
#define ERR_MAX_ACCESS_IN_SMIV1			 34
#define ERR_ACCESS_IN_SMIV2			 35
#define ERR_UNWANTED_MODULE			 36
#define ERR_MAX_LEX_DEPTH			 37
#define ERR_MODULE_NOT_FOUND			 38
#define ERR_INCLUDE				 39
#define ERR_STATISTICS				 40
#define ERR_OBJECT_IDENTIFIER_REGISTERED	 41
#define ERR_ALLOCATING_MIBNODE			 42
#define ERR_OPENING_CONFIGFILE			 43
#define ERR_ALLOCATING_DIRECTORY		 44
#define ERR_ALLOCATING_DESCRIPTOR		 45
#define ERR_OPENING_INPUTFILE			 46
#define ERR_OPENING_IMPORTFILE			 47
#define ERR_ALLOCATING_MIBMODULE		 48
#define ERR_UNKNOWN_OIDLABEL			 49
#define ERR_SINGLE_SUBIDENTIFIER		 50
#define ERR_SUBIDENTIFIER_VS_OIDLABEL		 51
#define ERR_OIDLABEL_DOESNOT_EXTEND		 52
#define ERR_OIDLABEL_DOESNOT_EXTEND_1		 53
#define ERR_EXISTENT_DESCRIPTOR			 54
#define ERR_UNKNOWN_CONFIG_DIRECTIVE		 55
#define ERR_ALLOCATING_PENDING_MIBNODE		 56
#define ERR_ALLOCATING_IMPORTDESCRIPTOR		 57
#define ERR_IDENTIFIER_NOT_IN_MODULE		 58
#define ERR_TYPE_ALREADY_EXISTS			 59
#define ERR_ALLOCATING_TYPE			 60
#define ERR_MACRO				 61
#define ERR_CHOICE				 62
#define ERR_TYPE_SMI				 63
#define ERR_TYPE_TAG				 64
#define ERR_MACRO_ALREADY_EXISTS		 65
#define ERR_ALLOCATING_MACRO			 66
#define ERR_EXPORTS				 67
#define ERR_ILLEGALLY_QUALIFIED			 68
#define ERR_MISSING_DESCRIPTION			 69
#define ERR_OIDLABEL_NOT_FIRST			 70
#define ERR_UNKNOWN_TYPE			 71


#define yyerror(msg) printError(parser, ERR_OTHER_ERROR, msg)


extern int line;			 /* Current line in source file.     */
extern int column;			 /* Current column in current line.  */
extern int character;			 /* Current absolute byte position.  */
extern char linebuf[MAX_LINEBUF_LENGTH]; /* Current line up to current pos.  */

extern int printErrorLines;		 /* Verbose error lines		     */
extern int errorLevel;                   /* Higher level for more warnings   */
extern int debugLevel;                   /* Higher level for more messages   */

extern void printError(Parser *parser, int id, ...);
extern void printDebug(int level, char *fmt, ...);

#endif /* _ERROR_H */
