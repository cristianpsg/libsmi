/*
 * acconfig.h --
 *
 *      autoheader configuration macros.
 *
 * Copyright (c) 2000 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id: acconfig.h,v 1.4 2000/11/12 17:46:37 strauss Exp $
 */

/* Define if the SMIv1/v2 parser shall be active. */
#define BACKEND_SMI

/* Define if the SMIng parser shall be active. */
#define BACKEND_SMING

/* The default error level at libsmi initialization. */
#define DEFAULT_ERRORLEVEL 3

/* The maximum module import recursion depth. */
#define MAX_LEX_DEPTH 20

/* The full pathname of the global configuration file. */
#define DEFAULT_GLOBALCONFIG "/usr/local/etc/smi.conf"

/* The basename of the per-user configuration file searched in $HOME. */
#define DEFAULT_USERCONFIG ".smirc"

/* The default search path to lookup SMI module files. */
#define DEFAULT_SMIPATH "/usr/local/share/mibs/ietf"

/* The default path separator character. */
#define PATH_SEPARATOR ':'

/* The 64 bit integer types, their formats, and their min/max values. */
#define UINT64_TYPE uint64_t
#define INT64_TYPE int64_t
#define UINT64_FORMAT "%llu"
#define INT64_FORMAT "%lld"
#define LIBSMI_UINT64_MAX 18446744073709551615ULL
#define LIBSMI_INT64_MIN -9223372036854775808LL
#define LIBSMI_INT64_MAX 9223372036854775807LL

/* Define if dmalloc.h is present and shall be used. */
#undef HAVE_DMALLOC_H

/* Define if getopt.h (argv[] option parser) is present. */
#undef HAVE_GETOPT_H

/* Define if pwd.h (/etc/passwd database) is present. */
#undef HAVE_PWD_H

/* Define if the string-to-long-long function is present in libc. */
#undef HAVE_STRTOLL

/* Define if the string-to-quad function (BSD) is present in libc. */
#undef HAVE_STRTOQ

/* Define if the string-to-unsigned-long-long function is present in libc. */
#undef HAVE_STRTOULL

/* Define if the string-to-unsigned-quad function (BSD) is present in libc. */
#undef HAVE_STRTOUQ

/* Define if the timegm() function is present in libc. */
#undef HAVE_TIMEGM

/* Name of package */
#undef PACKAGE

/* Version number of package */
#undef VERSION

