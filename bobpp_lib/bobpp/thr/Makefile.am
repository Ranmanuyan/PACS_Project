# ==========================================================================
# Copyright(c)'2004 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Bob++ library
#
# ==========================================================================

noinst_LTLIBRARIES=libthr.la
#
# Multi threaded part of bobpp
#
h_sources= thr.h
cc_sources= thr.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/thr
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)

libthr_la_SOURCES= $(h_sources) $(cc_sources)

