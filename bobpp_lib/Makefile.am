# ==========================================================================
# Copyright(c)'2006 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Bob++ library
# ==========================================================================

ACLOCAL_AMFLAGS= -I m4

## top directory
include aminclude.am

#Build in these directories:
SUBDIRS= bobpp tools

#Distribute these directories:
DIST_SUBDIRS = bobpp examples tools

EXTRA_DIST = win32 bootstrap

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = bobpp.pc

nobase_include_HEADERS=config.h
