/*
 * interface.c --
 *
 *      Interface of libsmi.
 *
 * Copyright (c) 1998 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * @(#) $Id$
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "defs.h"
#include "config.h"
#include "error.h"
#include "data.h"
#include "interface.h"

extern int yydebug;

Parser toplevelParser;
int flags;
char module[MAX_IDENTIFIER_LENGTH+1];



MibNode *
getMibNode(input)
    char *input;
{
    char *element, *module, *name;
    unsigned int subid;
    MibNode *node;

    if (isdigit(input[0])) {
	/*
	 * input in `1.3.0x6.1...' form.
	 */
	element = strdup(input);
	/* TODO: success? */
	name = strtok(element, ".");
	node = rootMibNode;
	while (name) {
	    subid = (unsigned int)strtoul(name, NULL, 0);
	    node = findMibNodeByParentAndSubid(node, subid);
	    name = strtok(NULL, ".");
	}
    } else if (strchr(input, '!')) {
	/*
	 * input in `Module.name' form.
	 */
	element = strdup(input);
	/* TODO: success? */
	module = strtok(element, "!");
	name = strtok(NULL, " ");
	node = findMibNodeByModulenameAndName(module, name);
	free(element);
    } else if ((element = strrchr(input, '.')) != NULL) {
	/*
	 * input in `name.name.name...' form.
	 */
	node = findMibNodeByName(&element[1]);
	/* TODO: all elements must match */
    } else {
	/*
	 * input in simple `name' form.
	 */
	node = findMibNodeByName(input);
	/* TODO: defines views! */
    }

    return node;
}	



Type *
getType(input)
    char *input;
{
    char *element, *module, *name;
    Type *type;
    
    if (strchr(input, '!')) {
	/*
	 * input in `Module.name' form.
	 */
	element = strdup(input);
	/* TODO: success? */
	module = strtok(element, "!");
	name = strtok(NULL, " ");
	type = findTypeByModulenameAndName(module, name);
	free(element);
    } else {
	/*
	 * input in simple `name' form.
	 */
	type = findTypeByName(input);
	/* TODO: defines views! */
    }

    return type;
}	



char *
getString(string, module)
    String *string;
    Module *module;
{
    char *p;
    int fd;
    
    if (string->ptr) {
	return string->ptr;
    } else {
	p = malloc(string->length+1);
	/* TODO: success? */
	/* TODO: who will free() this? */
	fd = open(module->path, O_RDONLY);
	if (fd > 0) {
	    lseek(fd, string->fileoffset, SEEK_SET);
	    read(fd, p, string->length);
	    /* TODO: loop if read() < length. */
	    close(fd);
	    p[string->length] = 0;
	    return p;
	} else {
	    return "";
	    /* TODO: error handling? */
	}
    }
}






int
smiLoadMibModule(modulename)
    const char *modulename;
{
    int rc = -1;
    char *path;
    
    if (!findModuleByName(modulename)) {
	path = findFileByModulename(modulename);
	if (!path) {
	    rc = -2;
	} else {
	    rc = readMibFile(path, modulename, flags);
	}
    } else {
	/*
	 * Module is already loaded.
	 */
	rc = 0;
    }
    return rc;
}



int
smiInit()
{
    yydebug = 0;
    printErrorLines = 0;
    errorLevel = 3;
    debugLevel = 0;

    flags = FLAG_WHOLEFILE | FLAG_ERRORS | FLAG_WHOLEMOD;

    initData();
    
#ifdef CONFIG_FILE
    readConfig(CONFIG_FILE, &flags);
#endif

    return 0;
}


char *
smiGetMibNodeDescription(input)
    const char *input;
{
    MibNode *node;
    
    printf("smiGetMibNodeDescription(%s)\n", input);

    node = getMibNode(input);
    
    if (node) {
	return getString(&node->description, node->module);
    }
    return NULL;
}


char *
smiGetTypeDescription(input)
    const char *input;
{
    Type *type;
    
    printf("smiGetTypeDescription(%s)\n", input);

    type = getType(input);
    
    if (type) {
	return getString(&type->description, type->module);
    }

    return NULL;
}

