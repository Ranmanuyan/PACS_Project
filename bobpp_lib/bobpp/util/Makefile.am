# ==========================================================================
# Copyright(c)'2007 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Integer Linear Programming module of the Bob++ library
#
# ==========================================================================


h_sources=bob_util.hpp bob_string.hpp bob_misc.hpp bob_array.hpp bob_parse.hpp

cc_sources=bob_array.cpp bob_parse.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/util
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir) 

noinst_LTLIBRARIES=libutil.la
libutil_la_SOURCES= $(h_sources) $(cc_sources)

