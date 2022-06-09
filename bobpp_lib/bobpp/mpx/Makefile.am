#
# Message Passing part of Bob++
#
INCLUDES = -I$(top_srcdir)
CXX=$(MPICXX)

lib_LTLIBRARIES=libbobpp_mpi.la
h_sources= mpx.hpp mpx_env.hpp mpx_mess.hpp mpx_strategy.hpp
cc_sources= mpx.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)/mpx
library_include_HEADERS = $(h_sources)

libbobpp_mpi_la_SOURCES= $(h_sources) $(cc_sources)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = bobpp_mpi.pc

