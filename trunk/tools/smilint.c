/*
 * smilint.c --
 *
 *      MIB module checker main program.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smilint.c,v 1.34 2000/10/21 09:35:03 strauss Exp $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"

/*
 * These are functions that are not officially exported by the libsmi.
 * See the original prototype definitions in lib/error.h.
 */

extern int smiGetErrorSeverity(int id);
extern char* smiGetErrorTag(int id);
extern char* smiGetErrorMsg(int id);



static int mFlag = 0;	/* show the name for error messages */
static int sFlag = 0;	/* show the severity for error messages */
static int eFlag = 0;	/* print the list of possible error messages */



typedef struct Error {
    int id;
    int severity;
    char *tag;
    char *msg;
} Error;



static int compare(const void *v1, const void *v2)
{
    Error *err1 = (Error *) v1;
    Error *err2 = (Error *) v2;

    if (err1->severity < err2->severity) {
	return -1;
    }
    if (err1->severity > err2->severity) {
	return 1;
    }
    return strcmp(err1->msg, err2->msg);
}



static void errors()
{
    int i;
    size_t cnt;
    Error *errors;
    
    for (cnt = 0, i = 0; smiGetErrorSeverity(i) >= 0; i++) {
	cnt++;
    }
    
    errors = malloc(cnt * sizeof(Error));
    if (! errors) {
	fprintf(stderr, "smilint: malloc failed - running out of memory\n");
	exit(1);
    }
    memset(errors, 0, cnt * sizeof(Error));
    for (i = 0; i < (int)cnt; i++) {
	errors[i].id = i;
	errors[i].severity = smiGetErrorSeverity(i);
	errors[i].tag = smiGetErrorTag(i);
	errors[i].msg = smiGetErrorMsg(i);
    }

    qsort(errors, cnt, sizeof(Error), compare);
    
    for (i = 0; i < (int)cnt; i++) {
	if (sFlag) {
	    printf("[%d] %s (%s)\n",
		   errors[i].severity, errors[i].msg, errors[i].tag);
	} else {
	    printf("%s (%s)\n", errors[i].msg, errors[i].tag);
	}
    }
    
    free(errors);
}



static void usage()
{
    fprintf(stderr,
	    "Usage: smilint [-Vehrs] [-c <configfile>] [-p <module>] [-l <level>]\n"
	    "               [-i <error-pattern>] <module_or_path>\n"
	    "-V                    show version and license information\n"
	    "-e                    print list of known error messages\n"
	    "-h                    show usage information\n"
	    "-m                    print the name of errors in braces\n"
	    "-r                    print errors also for imported modules\n"
	    "-s                    print the severity of errors in brackets\n"
	    "-c <configfile>       load a specific configuration file\n"
	    "-p <module>           preload <module>\n"
	    "-l <level>            set maximum level of errors and warnings\n"
	    "-i <error-pattern>    ignore errors matching prefix pattern\n"
	    "<module_or_path>      plain name of MIB module or file path\n");
}



static void version()
{
    printf("smilint " SMI_VERSION_STRING "\n");
}



static void
errorHandler(char *path, int line, int severity, char *msg, char *tag)
{
    if (path) {
	fprintf(stderr, "%s:%d: ", path, line);
    }
    if (sFlag) {
	fprintf(stderr, "[%d] ", severity);
    }
    if (mFlag) {
	fprintf(stderr, "{%s} ", tag);
    }
    fprintf(stderr, "%s\n", msg);
}



int main(int argc, char *argv[])
{
    char c;
    int flags, i;

    for (i = 1; i < argc; i++) if (strstr(argv[i], "-c") == argv[i]) break;
    if (i == argc) 
	smiInit("smilint");
    else
	smiInit(NULL);

    flags = smiGetFlags();
    flags |= SMI_FLAG_ERRORS;
    flags |= SMI_FLAG_NODESCR;
    smiSetFlags(flags);

    while ((c = getopt(argc, argv, "Vehmrsp:l:i:c:")) != -1) {
	switch (c) {
	case 'c':
	    smiReadConfig(optarg, "smiquery");
	    break;
	case 'V':
	    version();
	    return 0;
	case 'e':
	    eFlag++;
	    break;
	case 'h':
	    usage();
	    return 0;
	case 'm':
	    mFlag++;
	    break;
	case 'r':
	    flags |= SMI_FLAG_RECURSIVE;
	    smiSetFlags(flags);
	    break;
	case 's':
	    sFlag++;
	    break;
	case 'p':
	    smiLoadModule(optarg);
	    break;
	case 'i':
	    smiSetSeverity(optarg, 9);
	    break;
	case 'l':
	    smiSetErrorLevel(atoi(optarg));
	    break;
	default:
	    usage();
	    exit(1);
	}
    }

    if (sFlag || mFlag) {
	smiSetErrorHandler(errorHandler);
    }

    if (eFlag) {
	errors();
	return 0;
    }
    
    while (optind < argc) {
	if (smiLoadModule(argv[optind]) == NULL) {
	    fprintf(stderr, "smilint: cannot locate module `%s'\n",
		    argv[optind]);
	    exit(1);
	}
	optind++;
    }

    smiExit();

    return 0;
}
