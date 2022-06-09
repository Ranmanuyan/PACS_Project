# ==========================================================================
# Copyright(c)'2007 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for example/Qap Bob++ library
#
# ==========================================================================

if USE_ATHA
  PROG_ATHA=qapatha
endif

if USE_MPI
  PROG_MPI=qapmpi
endif

EXTRA_DIST = Text

noinst_PROGRAMS= qap qap2 qapthr qapvns qapvnsthr ${PROG_ATHA} ${PROG_MPI}

CLEANFILES=${noinst_PROGRAMS} *.o

BOBO_LIB_NAME=lib${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}.la
LDADD=${prefix}/lib/libbobpp-1.0.a
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

qap_SOURCES = qap.cpp 
qap_CPPFLAGS = $(AM_CPPFLAGS) 
qap_LDFLAGS = 

qap2_SOURCES = qap.cpp 
qap2_CPPFLAGS = $(AM_CPPFLAGS) -DV2
qap2_LDFLAGS = 

qapthr_SOURCES = qap.cpp 
qapthr_CPPFLAGS = $(AM_CPPFLAGS) -DThreaded
qapthr_LDFLAGS = 

qapvns_SOURCES = qapvns.cpp
qapvns_CPPFLAGS = $(AM_CPPFLAGS) 
qapvns_LDFLAGS = 

qapvnsthr_SOURCES = qapvns.cpp
qapvnsthr_CPPFLAGS = $(AM_CPPFLAGS) -DThreaded
qapvnsthr_LDFLAGS = 


if USE_ATHA
  qapatha_SOURCES=qap.cpp
  qapatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  qapatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha
endif

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@
  qapmpi_SOURCES = qap.cpp 
  qapmpi_CPPFLAGS = $(AM_CPPFLAGS) -DMPxMPI
  qapmpi_LINK = $(MPICXXLINK)
qapmpi-qap.o: qap.cpp
	$(MPICXXCOMPILE) -DMPxMPI -c -o $@ $<
endif

