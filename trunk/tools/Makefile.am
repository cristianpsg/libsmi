#
# Makefile.am --
#
#      Template to generate the tools Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# @(#) $Id: Makefile.am,v 1.1 1999/05/28 14:15:18 strauss Exp $
#

AUTOMAKE_OPTIONS	= no-dependencies foreign

INCLUDES		= -I../lib

EXTRA_DIST		= dump-smi.h dump-sming.h dump-data.h $(man_MANS)

bin_PROGRAMS		= smiquery smilint smidump

man_MANS		= smiquery.1 smilint.1 smidump.1

smiquery_SOURCES	= smiquery.c
smiquery_LDADD		= ../lib/libsmi.la

smilint_SOURCES		= smilint.c
smilint_LDADD		= ../lib/libsmi.la

smidump_SOURCES		= smidump.c dump-sming.c dump-smi.c dump-data.c
smidump_LDADD		= ../lib/libsmi.la

