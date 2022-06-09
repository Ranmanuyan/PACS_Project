# ==========================================================================
# Copyright(c)'2004 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Bob++ library
#
# ==========================================================================

#
# Cell Broadband Engine part of bobpp
#
lib_LTLIBRARIES=libbobpp_cell.la
h_sources= cell.hpp spe_defs.hpp
cc_sources= cell.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/cell
library_include_HEADERS = *.hpp

INCLUDES = -I$(top_srcdir)

libbobpp_cell_la_SOURCES= $(h_sources) $(cc_sources)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = bobpp_cell.pc

