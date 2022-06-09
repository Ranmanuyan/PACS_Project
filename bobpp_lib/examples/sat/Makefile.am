# ==========================================================================
# Copyright(c)'2005 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for examples/TestDC Bob++ library
#
#	This makefile.am is used to generate the real makefile using the GNU
#	autotools suite.
#	The example is :
#
#	From these source codes, we are able to execute the resolution in the 
#	Sequential, Threaded, Atha and mpi environments.
#
# ==========================================================================

#=======================
# Section to name the generated binaries
#=======================
if USE_MPI
   PROG_MPI=csatmpi osatmpi 
endif

if USE_ATHA
   PROG_ATHA=csatatha osatatha 
endif

PROG_SEQ = csat osat 
PROG_THR = csatthr osatthr

noinst_PROGRAMS= ${PROG_SEQ} ${PROG_THR} ${PROG_ATHA} ${PROG_MPI}

#=======================
# General section
#=======================
CLEANFILES=${noinst_PROGRAMS} *.o

LDADD=-L${prefix}/lib -lbobpp-${GENERIC_API_VERSION}
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

#=======================
# The Queen Section (sequential and thr)
#=======================
csat_SOURCES=sat.cpp
csat_CPPFLAGS=${AM_CPPFLAGS} -DCOUNT

csatthr_SOURCES=sat.cpp
csatthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded -DCOUNT

osat_SOURCES=sat.cpp

osatthr_SOURCES=sat.cpp
osatthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded


#=======================
# The Atha and MPI environment
#=======================
if USE_ATHA
  csatatha_SOURCES=sat.cpp
  csatatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  csatatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DCOUNT -DAtha

  osatatha_SOURCES=sat.cpp
  osatatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  osatatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha
endif

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@

  csatmpi_SOURCES = sat.cpp
  csatmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI -DCOUNT
  csatmpi_LINK = $(MPICXXLINK)
csatmpi-sat.o: sat.cpp
	$(MPICXXCOMPILE) -DCOUNT -DMPxMPI -c -o $@ $< 

  osatmpi_SOURCES = sat.cpp
  osatmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  osatmpi_LINK = $(MPICXXLINK)
osatmpi-sat.o: sat.cpp
	$(MPICXXCOMPILE) -DMPxMPI -c -o $@ $< 
endif

