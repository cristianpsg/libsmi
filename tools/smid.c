/*
 * Brainstorming....
 *
 * command name
 * command element name
 * command element name
 *
 * command: node, type
 *
 * element: name, module, oid, type, basetype, declaration, nodekind, access
 *	    status, description, reference
 *
 * name: name, module.name, oid
 *
 *
 * response: XXXYZZZZZZ....
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <syslog.h>


#include <smi.h>



#define SMID_PORT	 2578
#define SMID_MAX_CLIENTS 8



char *smiStringStatus(SmiStatus status)
{
    return
	(status == SMI_STATUS_CURRENT)     ? "current" :
	(status == SMI_STATUS_DEPRECATED)  ? "deprecated" :
	(status == SMI_STATUS_OBSOLETE)    ? "obsolete" :
	(status == SMI_STATUS_MANDATORY)   ? "mandatory" :
	(status == SMI_STATUS_OPTIONAL)    ? "optional" :
					     "<unknown>";
}

char *smiStringAccess(SmiAccess access)
{
    return
	(access == SMI_ACCESS_NOT_ACCESSIBLE) ? "not-accessible" :
	(access == SMI_ACCESS_NOTIFY)	      ? "accessible-for-notify" :
	(access == SMI_ACCESS_READ_ONLY)      ? "read-only" :
	(access == SMI_ACCESS_READ_WRITE)     ? "read-write" :
	(access == SMI_ACCESS_READ_CREATE)    ? "read-create" :
	(access == SMI_ACCESS_WRITE_ONLY)     ? "write-only" :
						"<unknown>";
}

char *smiStringLanguage(SmiLanguage language)
{
    return
	(language == SMI_LANGUAGE_UNKNOWN)    ? "<UNKNOWN>" :
	(language == SMI_LANGUAGE_SMIV1)      ? "SMIv1" :
	(language == SMI_LANGUAGE_SMIV2)      ? "SMIv2" :
	(language == SMI_LANGUAGE_SMING)      ? "SMIng" :
						"<unknown>";
}

char *smiStringDecl(SmiDecl macro)
{
    return
        (macro == SMI_DECL_UNKNOWN)           ? "<UNKNOWN>" :
        (macro == SMI_DECL_TYPEASSIGNMENT)    ? "<TYPE-ASSIGNMENT>" :
        (macro == SMI_DECL_VALUEASSIGNMENT)   ? "<VALUE-ASSIGNMENT>" :
        (macro == SMI_DECL_OBJECTTYPE)        ? "OBJECT-TYPE" :
        (macro == SMI_DECL_OBJECTIDENTITY)    ? "OBJECT-IDENTITY" :
        (macro == SMI_DECL_MODULEIDENTITY)    ? "MODULE-IDENTITY" :
        (macro == SMI_DECL_NOTIFICATIONTYPE)  ? "NOTIFICATIONTYPE" :
        (macro == SMI_DECL_TRAPTYPE)          ? "TRAP-TYPE" :
        (macro == SMI_DECL_OBJECTGROUP)       ? "OBJECT-GROUP" :
        (macro == SMI_DECL_NOTIFICATIONGROUP) ? "NOTIFICATION-GROUP" :
        (macro == SMI_DECL_MODULECOMPLIANCE)  ? "MODULE-COMPLIANCE" :
        (macro == SMI_DECL_AGENTCAPABILITIES) ? "AGENT-CAPABILITIES" :
        (macro == SMI_DECL_TEXTUALCONVENTION) ? "TEXTUAL-CONVENTION" :
        (macro == SMI_DECL_MODULE)	      ? "module" :
        (macro == SMI_DECL_TYPEDEF)	      ? "typedef" :
        (macro == SMI_DECL_NODE)	      ? "node" :
        (macro == SMI_DECL_SCALAR)	      ? "scalar" :
        (macro == SMI_DECL_TABLE)	      ? "table" :
        (macro == SMI_DECL_ROW)		      ? "row" :
        (macro == SMI_DECL_COLUMN)	      ? "column" :
        (macro == SMI_DECL_NOTIFICATION)      ? "notification" :
        (macro == SMI_DECL_GROUP)	      ? "group" :
        (macro == SMI_DECL_COMPLIANCE)	      ? "compliance" :
                                                "<unknown>";
}

char *smiStringNodekind(SmiNodekind nodekind)
{
    return
        (nodekind == SMI_NODEKIND_UNKNOWN)      ? "<UNKNOWN>" :
        (nodekind == SMI_NODEKIND_MODULE)       ? "module" :
        (nodekind == SMI_NODEKIND_NODE)         ? "node" :
        (nodekind == SMI_NODEKIND_SCALAR)       ? "scalar" :
        (nodekind == SMI_NODEKIND_TABLE)        ? "table" :
        (nodekind == SMI_NODEKIND_ROW)          ? "row" :
        (nodekind == SMI_NODEKIND_COLUMN)       ? "column" :
        (nodekind == SMI_NODEKIND_NOTIFICATION) ? "notification" :
        (nodekind == SMI_NODEKIND_GROUP)        ? "group" :
        (nodekind == SMI_NODEKIND_COMPLIANCE)   ? "compliance" :
                                                  "<unknown>";
}

char *smiStringBasetype(SmiBasetype basetype)
{
    return
        (basetype == SMI_BASETYPE_UNKNOWN)           ? "<UNKNOWN>" :
        (basetype == SMI_BASETYPE_OCTETSTRING)       ? "OctetString" :
        (basetype == SMI_BASETYPE_OBJECTIDENTIFIER)  ? "ObjectIdentifier" :
        (basetype == SMI_BASETYPE_UNSIGNED32)        ? "Unsigned32" :
        (basetype == SMI_BASETYPE_INTEGER32)         ? "Integer32" :
        (basetype == SMI_BASETYPE_UNSIGNED64)        ? "Unsigned64" :
        (basetype == SMI_BASETYPE_INTEGER64)         ? "Integer64" :
        (basetype == SMI_BASETYPE_FLOAT32)           ? "Float32" :
        (basetype == SMI_BASETYPE_FLOAT64)           ? "Float64" :
        (basetype == SMI_BASETYPE_FLOAT128)          ? "Float128" :
        (basetype == SMI_BASETYPE_ENUM)              ? "Enumeration" :
        (basetype == SMI_BASETYPE_BITS)              ? "Bits" :
                                                   "<unknown>";
}



int prefix(char *gaga, char *ga)
{
    char *g;
    int i, rc;
    
    g = strdup(ga);
    for (i=0; i < strlen(g); i++) g[i] = (char)tolower((int)g[i]);
    rc = (strstr(gaga, g) == gaga);
    free(g);
    return rc;
}



void print(int fd, int code, char *fmt, ...)
{
    va_list ap;
    char s[200], ss[200];
    char *p;
    
    va_start(ap, fmt);
    vsprintf(s, fmt, ap);
    va_end(ap);

    
    sprintf(ss, "%03d %s\n", code, s);
    
    syslog(LOG_NOTICE, "fd %d -> %s", fd, ss);
    write(fd, ss, strlen(ss));
}



char *formatoid(unsigned int oidlen, SmiSubid *oid)
{
    static char ss[2000];
    int i;
    
    ss[0] = 0;
    for (i=0; i < oidlen; i++) {
        if (i) strcat(ss, ".");
        sprintf(&ss[strlen(ss)], "%u", oid[i]);
    }
    return ss;
}



void work(int fd, char *buf)
{
    char *cmd, *elem, *name;
    SmiNode *smiNode;
    SmiModule *smiModule;
    int all;
    
    syslog(LOG_NOTICE, "fd %d <- %s", fd, buf);

    cmd = strtok(buf, " ");
    if (cmd) name = strtok(NULL, " ");
    if (name) elem = strtok(NULL, " "); else elem = NULL;

    if (prefix("help", cmd)) {
	print(fd, 001, "This is smid - (c) 1999 Frank Strauss, Technical University of Braunschweig.");
    } else if (prefix("module", cmd)) {
	smiModule = smiGetModule(name);
	
    } else if (prefix("node", cmd)) {
	if (name) {
	    smiNode = smiGetNode(name, NULL);
	    if (!smiNode) {
		print(fd, 504, "unknown name");
	    } else {
		if (!elem) {
		    if (smiNode->module) {
			print(fd, 110, "%s.%s",
			      smiNode->module, smiNode->name);
		    } else {
			print(fd, 110, "%s", smiNode->name);
		    }
		    print(fd, 111, "%s",
			  formatoid(smiNode->oidlen, smiNode->oid));
		    if (smiNode->typename) {
			if (smiNode->typemodule) {
			    print(fd, 112, "%s.%s",
				  smiNode->typemodule,
				  smiNode->typename);
			} else {
			    print(fd, 112, "%s",
				  smiNode->typename);
			}
		    }
		    if (smiNode->nodekind != SMI_NODEKIND_UNKNOWN) {
			print(fd, 113, "%s",
			      smiStringNodekind(smiNode->nodekind));
		    }
		    if (smiNode->format) {
			print(fd, 114, "%s", smiNode->format);
		    }
		    if (smiNode->status != SMI_STATUS_UNKNOWN) {
			print(fd, 115, "%s",
			      smiStringStatus(smiNode->status));
		    }
		    if (smiNode->access != SMI_ACCESS_UNKNOWN) {
			print(fd, 116, "%s",
			      smiStringAccess(smiNode->access));
		    }
		} else {
		    while (elem) {
			if (prefix("name", elem)) {
			    if (smiNode->module) {
				print(fd, 110, "%s.%s",
				      smiNode->module, smiNode->name);
			    } else {
				print(fd, 110, "%s", smiNode->name);
			    }
			} else if (prefix("oid", elem)) {
			    print(fd, 111, "%s",
				  formatoid(smiNode->oidlen, smiNode->oid));
			} else if (prefix("type", elem)) {
			    if (smiNode->typename) {
				if (smiNode->typemodule) {
				    print(fd, 112, "%s.%s",
					  smiNode->typemodule,
					  smiNode->typename);
				} else {
				    print(fd, 112, "%s",
					  smiNode->typename);
				}
			    } else {
				print(fd, 412, "no type");
			    }
			} else if (prefix("nodekind", elem)) {
			    if (smiNode->nodekind != SMI_NODEKIND_UNKNOWN) {
				print(fd, 113, "%s",
				      smiStringNodekind(smiNode->nodekind));
			    } else {
				print(fd, 413, "no nodekind");
			    }
			} else if (prefix("format", elem)) {
			    if (smiNode->format) {
				print(fd, 114, "%s", smiNode->format);
			    } else {
				print(fd, 414, "no format");
			    }
			} else if (prefix("status", elem)) {
			    if (smiNode->status != SMI_STATUS_UNKNOWN) {
				print(fd, 115, "%s",
				      smiStringStatus(smiNode->status));
			    } else {
				print(fd, 415, "no status");
			    }
			} else if (prefix("access", elem)) {
			    if (smiNode->access != SMI_ACCESS_UNKNOWN) {
				print(fd, 116, "%s",
				      smiStringAccess(smiNode->access));
			    } else {
				print(fd, 416, "no access");
			    }
			} else {
			    print(fd, 410, "unknown element");
			}
			elem = strtok(NULL, " ");
		    }
		}
	    }
	} else {
	    print(fd, 503, "need a name");
	}
    } else {
	print(fd, 501, "unknown command");
    }
}



main(int argc, char *argv[])
{
    int sock;
    int sopt_val, sopt_len;
    struct sockaddr_in srv_addr;
    int client[FD_SETSIZE];
    int n, i, fd;
    fd_set fds, fdmask;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    char buf[100];
    char *p;

    smiInit();

    for (i=1; i < argc; i++) {
	smiLoadModule(argv[i]);
    }
	
    smiLoadModule("SNMPv2-SMI");
    smiLoadModule("SNMPv2-MIB");
	
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	perror("cannot create socket");
	return -1;
    }

    sopt_val = 1;
    sopt_len = sizeof(sopt_val);
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		   (char *)&sopt_val, sopt_len) < 0) {
	perror("cannot set SO_REUSEADDR");
	close(sock);
	return -1;
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons (SMID_PORT);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
	perror("cannot bind socket");
	close(sock);
	return -1;
    }

    if (listen(sock, SMID_MAX_CLIENTS) < 0) {
	perror("cannot listen to socket");
	close(sock);
	return -1;
    }

    for (i=0; i < SMID_MAX_CLIENTS; i++) client[i] = -1;

    FD_ZERO(&fdmask);
    FD_SET(sock, &fdmask);

    openlog("smid", LOG_PID, LOG_DAEMON);
    
    while (1) {
	fds = fdmask;
	n = select(32, &fds, NULL, NULL, NULL);
	if (FD_ISSET(sock, &fds)) {
	    if ((fd = accept(sock, (struct sockaddr *)&addr, &addr_len)) < 0) {
		perror("accept failed");
		fd = -1;
	    } else {
		client[fd] = fd;
		FD_SET(fd, &fdmask);
		syslog(LOG_NOTICE,
		       "accepted client %s on fd %d",
		       inet_ntoa(addr.sin_addr), fd);
	    }
	} else {
	    for (fd=0; fd < FD_SETSIZE; fd++) {
		if (FD_ISSET(fd, &fds)) {
		    n = read(fd, buf, 99);
		    if (!n) {
			close(fd);
			syslog(LOG_NOTICE,
			       "dropped client %s on fd %d",
			       inet_ntoa(addr.sin_addr), fd);
		    } else {
			strtok(buf, "\015\012");
			work(fd, buf);
		    }
		}
	    }
	}
    }
}
