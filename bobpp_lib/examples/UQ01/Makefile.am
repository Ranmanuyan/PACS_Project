# ==========================================================================
# Copyright(c)'2011 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for example/uq01 Bob++ library
#
# ==========================================================================

if USE_ATHA
  PROG_ATHA=uq01atha
endif

if USE_MPI
  PROG_MPI=uq01mpi
endif

EXTRA_DIST = pb

noinst_PROGRAMS= uq01 uq01thr ${PROG_ATHA} ${PROG_MPI}

CLEANFILES=${noinst_PROGRAMS} *.o

BOBO_LIB_NAME=lib${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}.la
LDADD=${prefix}/lib/libbobpp-1.0.a
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

uq01_SOURCES = uq01.cpp 
uq01_CPPFLAGS = $(AM_CPPFLAGS) 
uq01_LDFLAGS = 

uq01thr_SOURCES = uq01.cpp 
uq01thr_CPPFLAGS = $(AM_CPPFLAGS) -DThreaded
uq01thr_LDFLAGS = 


if USE_ATHA
  uq01atha_SOURCES=uq01.cpp
  uq01atha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  uq01atha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha
endif

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@
  uq01mpi_SOURCES = uq01.cpp 
  uq01mpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  uq01mpi_LINK = $(MPICXXLINK) ${LIBGLOP}
uq01mpi-uq01.o: uq01.cpp
	$(MPICXXCOMPILE) ${INCGLOP} -DMPxMPI -c -o $@ $<
endif

