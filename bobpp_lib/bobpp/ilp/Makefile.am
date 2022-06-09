# ==========================================================================
# Copyright(c)'2007 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Integer Linear Programming module of the Bob++ library
#
# ==========================================================================

if USE_GLOP 
if USE_PPGLOP

h_sources=bob_branch.h  bob_cut.h  bob_var.h\
bob_lpbbalgo.h    bob_lp.h   

cc_sources=bob_branch.cpp  bob_lp.cpp bob_cut.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/ilp
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir) ${INCGLOP} ${INCPPGLOP}

lib_LTLIBRARIES=liblpbobpp-1.0.la
liblpbobpp_1_0_la_SOURCES= $(h_sources) $(cc_sources)
liblpbobpp_1_0_la_LIBADD= 
liblpbobpp_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
#liblpbobpp_1_0_la_CPPFLAGS= ${INCGLOP} 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = bobpp_lp.pc

endif
endif
