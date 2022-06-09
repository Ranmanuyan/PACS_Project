# ==========================================================================
# Copyright(c)'2007 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for example/Vrp Bob++ library
#
# ==========================================================================

if USE_GLOP
if USE_PPGLOP
if USE_ATHA
  PROG_ATHA=vrpatha
endif

if USE_MPI
#PROG_MPI=vrpmpi
endif

EXTRA_DIST = Data

noinst_PROGRAMS= vrp vrpthr ${PROG_ATHA} ${PROG_MPI}

CLEANFILES=${noinst_PROGRAMS} *.o

BOBO_LIB_NAME=lib${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}.la
LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a 
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

vrp_SOURCES = vrp.cpp reader.cpp graph.cpp node.cpp
vrp_CPPFLAGS = $(AM_CPPFLAGS) ${INCPPGLOP}
vrp_LDFLAGS = ${LIBPPGLOP} -lglop_serialize

vrpthr_SOURCES = vrp.cpp reader.cpp graph.cpp node.cpp
vrpthr_CPPFLAGS = $(AM_CPPFLAGS) -DThreaded ${INCPPGLOP}
vrpthr_LDFLAGS = ${LIBPPGLOP} -lglop_serialize


if USE_ATHA
  vrpatha_SOURCES=vrp.cpp reader.cpp graph.cpp node.cpp
  vrpatha_LDFLAGS = ${LIBATHA} ${LIBPPGLOP}  -lbobpp_atha -lglop_serialize
  vrpatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} ${INCATHA} -DAtha
endif

if USE_MPI
#MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
#$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
#MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
#--mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
#-lbobpp_mpi -o $@
#vrpmpi_SOURCES = vrp.cpp 
#vrpmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
#vrpmpi_LINK = $(MPICXXLINK) ${LIBGLOP}
#vrpmpi-vrp.o: vrp.cpp
#$(MPICXXCOMPILE) ${INCGLOP} -DMPxMPI -c -o $@ $<
endif
endif
endif

