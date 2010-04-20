/*
 * yang.c --
 *
 *      Interface Implementation of YANG.
 *
 * Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Author: Kaloyan Kanev, Siarhei Kuryla
 * @(#) $Id: data.c 12198 2009-01-05 08:37:07Z schoenw $
 */

#include <config.h>

#ifdef BACKEND_YANG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <limits.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#define ONLY_ORIGINAL 1
        
#include "yang.h"
#include "error.h"
#include "util.h"
#include "snprintf.h"

const char* yandDeclKeyword[] = {   "unknown",
                                    "module",
                                    "submodule",
                                    "revision",
                                    "import",
                                    "range",
                                    "pattern",
                                    "container",
                                    "must",
                                    "leaf",
                                    "leaf-list",
                                    "list",
                                    "case",
                                    "uses",
                                    "augument",
                                    "grouping",
                                    "choice",
                                    "argument",
                                    "rpc",
                                    "input",
                                    "output",
                                    "anyxml",
                                    "include",
                                    "organization",
                                    "contact",
                                    "namespace",
                                    "yang-version",
                                    "prefix",
                                    "typedef",
                                    "path",
                                    "length",
                                    "type",
                                    "error-message",
                                    "error-app-tag",
                                    "mandatory",
                                    "notification",
                                    "extension",
                                    "belongs-to",
                                    "yin-element",
                                    "unknown-statement",
                                    "description",
                                    "reference",
                                    "status",
                                    "config",
                                    "enum",
                                    "value",
                                    "require-instance",
                                    "base",
                                    "bit",
                                    "position",
                                    "units",
                                    "default",
                                    "feature",
                                    "if-feature",
                                    "identity",
                                    "presence",
                                    "when",
                                    "min-elements",
                                    "max-elements",
                                    "ordered-by",
                                    "key",
                                    "unique",
                                    "refine",
                                    "deviation",
                                    "deviate",
                                    "complex-type",
                                    "abstract",
                                    "extends",
                                    "instance",
                                    "instance-list",
                                    "instance-type"
                               };

const char* statusKeywords[] = {  "default",
                                  "current",
                                  "deprecated",
                                  "",
                                  "",
                                  "obsolete"
                                };
/*
 * YANG API
 */

/*
 * Determines whether a module with a certain name is a YANG module
 */
int yangIsModule(const char* modulename) {
    _YangNode   *modulePtr;
    modulePtr = findYangModuleByName(modulename, NULL);
    
    if (modulePtr) return 0;

    return 1;    
}

YangNode* yangGetModule(char *modulename) {
    if (ONLY_ORIGINAL) {
        return &((getModuleInfo(findYangModuleByName(modulename, NULL))->originalModule)->export);
    } else {
        return &(findYangModuleByName(modulename, NULL)->export);
    }
}

YangNode *yangGetFirstChildNode(YangNode *yangNodePtr) {
    _YangNode *nodePtr = (_YangNode *)yangNodePtr;
    if (!nodePtr) return NULL;
    nodePtr = nodePtr->firstChildPtr;

    if (!nodePtr) {
        return NULL;
    } else {
        return &(nodePtr->export);
    }
}

YangNode *yangGetNextSibling(YangNode *yangNodePtr) {
    _YangNode *nodePtr = (_YangNode *)yangNodePtr;
    if (!nodePtr) return NULL;
    nodePtr = nodePtr->nextSiblingPtr;
    if (!nodePtr) {
        return NULL;
    } else {
        return &(nodePtr->export);
    }
}

YangNode *yangGetFirstModule(void) {
    if (ONLY_ORIGINAL) {
        return &((getModuleInfo(smiHandle->firstYangModulePtr)->originalModule)->export);
    } else {
        return &(smiHandle->firstYangModulePtr->export);
    }
}

YangNode *yangGetNextModule(YangNode *yangModulePtr) {
    _YangNode *modulePtr = (_YangNode *)yangModulePtr;
    if (ONLY_ORIGINAL) {
        _YangNode* cur = smiHandle->firstYangModulePtr;
        while (cur) {
            if (getModuleInfo(cur)->originalModule == modulePtr) break;
            cur = cur->nextSiblingPtr;
        }    

        if (cur && cur->nextSiblingPtr) {
            return &(getModuleInfo(cur->nextSiblingPtr)->originalModule->export);    
        } else {
            return NULL;
        }
    } else {
        if (modulePtr && modulePtr->nextSiblingPtr) {
            return &(modulePtr->nextSiblingPtr->export);    
        } else {
            return NULL;
        }        
    }
}

int yangIsTrueConf(YangConfig conf) {
    return conf == YANG_CONFIG_DEFAULT_TRUE || conf == YANG_CONFIG_TRUE;
}

#endif

