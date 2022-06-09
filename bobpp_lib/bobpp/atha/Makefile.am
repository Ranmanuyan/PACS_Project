# ==========================================================================
# Copyright(c)'2006 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Bob++ library
#
# ==========================================================================

#
# Athapascan part of bobpp
#
lib_LTLIBRARIES=libbobpp_atha.la
h_sources= atha.h
cc_sources= atha.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/atha
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir) ${INCATHA}

libbobpp_atha_la_SOURCES= $(h_sources) $(cc_sources)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = bobpp_atha.pc
