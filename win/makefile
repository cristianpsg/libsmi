#
# makefile --
#
#       Makefile for VC++ (tested with version 4.2) for compiling
#	libsmi and associated tools natively under Win32.
#
# Copyright (c) 2000 E. Schoenfelder, Gaertner Datensysteme Braunschweig.
# Copyright (c) 2000 J. Schoenwaelder, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: makefile,v 1.3 2000/06/21 10:33:43 strauss Exp $

##
## Project directories -- these may need to be customized for your site
##
## ROOT --	top of the source tree.
## TMPDIR --	location for .obj files.
## TOOLS32 --	location of VC++ compiler installation.
##
ROOT	= .\..
TMPDIR	= .
#TOOLS32	= C:\MSDEV
TOOLS32	= C:\PROGRA~1\MICROS~6\VC98

##
## Installation paths: 
##
## MIBROOT  -- definition for default search path (unix-style)
## MIBDIR   -- same in windows style
## BINDIR   -- destination for the binaries (smidump.exe, ...)
## LIBDIR   -- destination for the library (smi.lib)
##
MIBROOT = c:/smi/mibs
INSTROOT = c:\smi
MIBDIR = $(INSTROOT)\mibs
BINDIR = $(INSTROOT)\bin
LIBDIR = $(INSTROOT)\lib

##
## Visual C++ tools
##
PATH=$(TOOLS32)\bin;$(PATH)

cc32	= $(TOOLS32)\bin\cl -I$(TOOLS32)\include -D__STDC__
CP      = copy
RM      = del
MKDIR	= md

##
## Definitions:
##
# DEBUG = -W3 -Zi -Od /Yd
INCLUDES = -I$(ROOT)\lib -I$(ROOT)\win -I$(TOOLS32)\include
DEFINES = $(DEBUG)

!include <win32.mak>

## 
## Global makefile settings
##
LIBSMIOBJS = \
	$(TMPDIR)\win.obj \
	$(TMPDIR)\data.obj \
	$(TMPDIR)\check.obj \
	$(TMPDIR)\error.obj \
	$(TMPDIR)\util.obj \
	$(TMPDIR)\smi.obj \
	$(TMPDIR)\parser-smi.tab.obj \
	$(TMPDIR)\scanner-smi.obj \
	$(TMPDIR)\parser-sming.tab.obj \
	$(TMPDIR)\scanner-sming.obj

SMIQUERYOBJS = \
	$(TMPDIR)\smiquery.obj

SMILINTOBJS = \
	$(TMPDIR)\smilint.obj

SMIDUMPOBJS = \
	$(TMPDIR)\smidump.obj \
	$(TMPDIR)\dump-sming.obj \
	$(TMPDIR)\dump-smi.obj \
	$(TMPDIR)\dump-imports.obj \
	$(TMPDIR)\dump-types.obj \
	$(TMPDIR)\dump-tree.obj \
	$(TMPDIR)\dump-mosy.obj \
	$(TMPDIR)\dump-corba.obj \
	$(TMPDIR)\dump-ucdsnmp.obj \
	$(TMPDIR)\dump-jax.obj \
	$(TMPDIR)\dump-xml.obj \
	$(TMPDIR)\dump-cm.obj \
	$(TMPDIR)\dump-metrics.obj

SMILIB = smi.lib

##
## Implicit Targets
##
.c.obj:
	$(cc32) $(cdebug) $(cflags) $(INCLUDES) \
		$(DEFINES) -Fo$(TMPDIR)\ $<
{$(ROOT)\lib}.c{$(TMPDIR)}.obj:
	$(cc32) $(cdebug) $(cflags) $(INCLUDES) \
		$(DEFINES) -Fo$(TMPDIR)\ $<
{$(ROOT)\tools}.c{$(TMPDIR)}.obj:
	$(cc32) $(cdebug) $(cflags) $(INCLUDES) \
		$(DEFINES) -Fo$(TMPDIR)\ $<

##
## Targets
##
all:	libs tools

libs:	$(SMILIB)

$(SMILIB):	$(LIBSMIOBJS)
	$(implib) /out:smi.lib $(LIBSMIOBJS)

tools:	smiquery.exe smilint.exe smidump.exe

smiquery.exe:	$(SMIQUERYOBJS) $(SMILIB)
	$(cc32) $(DEFINES) /osmiquery.exe $(SMIQUERYOBJS) $(SMILIB)

smilint.exe:	$(SMILINTOBJS) $(SMILIB)
	$(cc32) $(DEFINES) /osmilint.exe $(SMILINTOBJS) $(SMILIB)

smidump.exe:	$(SMIDUMPOBJS) $(SMILIB)
	$(cc32) $(DEFINES) \
		/osmidump.exe $(SMIDUMPOBJS) $(SMILIB)

clean:
	del *.lib
	del *.obj
	del *.exe
	del *.ilk
	del *.pdb
	del *.mdp

install:	all install-doc install-bin install-lib install-mibs

install-doc:
	$(MKDIR) $(INSTROOT)
	$(CP) ..\README $(INSTROOT)
	$(CP) ..\COPYING $(INSTROOT)
	$(CP) README.win $(INSTROOT)
	$(MKDIR) $(INSTROOT)\doc
	$(CP) ..\doc\*.txt $(INSTROOT)\doc
	$(CP) ..\doc\*.ps $(INSTROOT)\doc

install-bin:
	$(MKDIR) $(BINDIR)
	$(CP) *.exe $(BINDIR)

install-lib:
	$(MKDIR) $(LIBDIR)
	$(CP) *.lib $(LIBDIR)

install-mibs:
	$(MKDIR) $(MIBDIR)
	$(MKDIR) $(MIBDIR)\iana
	$(CP) ..\mibs\iana\*.* $(MIBDIR)\iana
	$(MKDIR) $(MIBDIR)\ietf
	$(CP) ..\mibs\ietf\*.* $(MIBDIR)\ietf
	$(MKDIR) $(MIBDIR)\irtf
	$(CP) ..\mibs\irtf\*.* $(MIBDIR)\irtf
	$(MKDIR) $(MIBDIR)\site

##
