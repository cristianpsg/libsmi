#
# Makefile.am --
#
#      Template to generate the doc Makefile.in using automake.
#
# Copyright (c) 1999 Frank Strauss, Technical University of Braunschweig.
#
# See the file "COPYING" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: Makefile.am,v 1.3 1999/12/14 15:34:44 strauss Exp $
#

EXTRA_DIST		= draft-irtf-nmrg-sming-02.txt

SMI.gif: SMI.eps
	epsffit -sc 0 0 600 600 SMI.eps SMI.eps.tmp
	ps2gif SMI.eps.tmp SMI.gif.tmp
	giftopnm SMI.gif.tmp | pnmcrop | ppmtogif > SMI.gif
	rm SMI.eps.tmp SMI.gif.tmp
