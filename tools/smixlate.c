/*
 * smixlate.c --
 *
 *      Translate OIDs located in the input stream.
 *
 * Copyright (c) 2006 Juergen Schoenwaelder, International University Bremen.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: smilint.c 1867 2004-10-06 13:45:31Z strauss $
 */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_WIN_H
#include "win.h"
#endif

#include "smi.h"
#include "shhopt.h"

static int flags;


static char buffer[256];
static int buffer_index = 0;

static inline void buffer_add(char c)
{
    if (buffer_index >= sizeof(buffer)-2) {
	/* we should perhaps return an error and let the app flush
	   the buffer */
	exit(1);
    }
    buffer[buffer_index] = c;
    buffer_index++;
}


static inline void buffer_zap()
{
    buffer_index = 0;
}


static inline void buffer_flush()
{
    if (buffer_index) {
	buffer_add(0);
	fputs(buffer, stdout);
    }
    buffer_zap();
}


static void translate()
{
    SmiNode *smiNode;

    buffer_add(0);
    smiNode = smiGetNode(NULL, buffer);
    if (smiNode) {
	fputs(smiNode->name, stdout);
	buffer_zap();
    } else {
	buffer_flush();
    }
}


static void process(FILE *stream)
{
    int c;
    enum { TXT, NUM, NUMDOT, NUMDOTNUM, OID, OIDDOT } state = TXT;

    /*
     * Shall we require iswhite() or ispunct() before and after the
     * OID?
     *
     * TODO: - preserve and print the suffix
     *       - optionally eat/generate white space to preserve layouts
     *       - translate instance identifier to something meaningful
     *         (e.g. foobar["name",32]) where possible
     *       - provide a reverse translation service (-x) (but this is
     *         more complex since it is unclear how to identify names
     *       - parse the number into an SmiSubid vector
     *       - make sure the first sub-identifier of the OID is in the
     *         range [0:2] to reduce false positives.
     */

    while ((c = fgetc(stream)) != EOF) {
	switch (state) {
	case TXT:
	    buffer_flush();
	    if (isdigit(c)) {
		buffer_add(c);
		state = NUM;
	    } else {
		fputc(c, stdout);
		fflush(stdout);
	    }
	    break;
	case NUM:
	    if (isdigit(c)) {
		buffer_add(c);
	    } else if (c == '.') {
		buffer_add(c);
		state = NUMDOT;
	    } else {
		buffer_add(c);
		state = TXT;
	    }
	    break;
	case NUMDOT:
	    if (isdigit(c)) {
		buffer_add(c);
		state = NUMDOTNUM;
	    } else {
		buffer_add(c);
		state = TXT;
	    }
	    break;
	case NUMDOTNUM:
	    if (isdigit(c)) {
		buffer_add(c);
	    } if (c == '.') {
		buffer_add(c);
		state = OID;
	    } else {
		buffer_add(c);
		state = TXT;
	    }
	    break;
	case OID:
	    if (isdigit(c)) {
		buffer_add(c);
	    } else if (c == '.') {
		buffer_add(c);
		state = OIDDOT;
	    } else {
		translate();
		fputc(c, stdout);
		state = TXT;
	    }
	    break;
	case OIDDOT:
	    if (isdigit(c)) {
		buffer_add(c);
		state = OID;
	    } else {
		translate();
		fputc(c, stdout);
		state = TXT;
	    }
	    break;
	}
    }
}



static void usage()
{
    fprintf(stderr,
	    "Usage: smilint [options] [module or path ...]\n"
	    "  -V, --version         show version and license information\n"
	    "  -h, --help            show usage information\n"
	    "  -c, --config=file     load a specific configuration file\n"
	    "  -p, --preload=module  preload <module>\n"
	    "  -r, --recursive       print errors also for imported modules\n"
	    "  -l, --level=level     set maximum level of errors and warnings\n"
	    "  -i, --ignore=prefix   ignore errors matching prefix pattern\n"
	    "  -I, --noignore=prefix do not ignore errors matching prefix pattern\n");
}



static void help() { usage(); exit(0); }
static void version() { printf("smilint " SMI_VERSION_STRING "\n"); exit(0); }
static void config(char *filename) { smiReadConfig(filename, "smilint"); }
static void preload(char *module) { smiLoadModule(module); }
static void recursive() { flags |= SMI_FLAG_RECURSIVE; smiSetFlags(flags); }
static void level(int lev) { smiSetErrorLevel(lev); }
static void ignore(char *ign) { smiSetSeverity(ign, 128); }
static void noignore(char *ign) { smiSetSeverity(ign, -1); }



int main(int argc, char *argv[])
{
    int i;

    static optStruct opt[] = {
	/* short long              type        var/func       special       */
	{ 'h', "help",           OPT_FLAG,   help,          OPT_CALLFUNC },
	{ 'V', "version",        OPT_FLAG,   version,       OPT_CALLFUNC },
	{ 'c', "config",         OPT_STRING, config,        OPT_CALLFUNC },
	{ 'p', "preload",        OPT_STRING, preload,       OPT_CALLFUNC },
	{ 'r', "recursive",      OPT_FLAG,   recursive,     OPT_CALLFUNC },
	{ 'l', "level",          OPT_INT,    level,         OPT_CALLFUNC },
	{ 'i', "ignore",         OPT_STRING, ignore,        OPT_CALLFUNC },
	{ 'I', "noignore",       OPT_STRING, noignore,      OPT_CALLFUNC },
	{ 0, 0, OPT_END, 0, 0 }  /* no more options */
    };
    
    for (i = 1; i < argc; i++)
	if ((strstr(argv[i], "-c") == argv[i]) ||
	    (strstr(argv[i], "--config") == argv[i])) break;
    if (i == argc) 
	smiInit("smixlate");
    else
	smiInit(NULL);

    flags = smiGetFlags();
    flags |= SMI_FLAG_ERRORS;
    flags |= SMI_FLAG_NODESCR;
    smiSetFlags(flags);

    optParseOptions(&argc, argv, opt, 0);

    for (i = 1; i < argc; i++) {
	if (smiLoadModule(argv[i]) == NULL) {
	    fprintf(stderr, "smilint: cannot locate module `%s'\n",
		    argv[i]);
	    smiExit();
	    exit(1);
	}
    }


    process(stdin);

    smiExit();

    return 0;
}
