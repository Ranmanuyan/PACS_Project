# ==========================================================================
# Copyright(c)'2005 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for examples/TestDC Bob++ library
#
#	This makefile.am is used to generate the real makefile using the GNU
#	autotools suite.
#	The example is :
#	KS : the knapsack source is a very simple resolution of the knapsack 
#	problem. The aim of this source code is not to have the best method to 
#	solve the knapsack, but it is to illustrate how to use the Bob++ Costed 
#	divide and Conquer method Bob::CDCAlgo.
#
#	From thses source codes, we are able to execute the resolution in the 
#	Sequential, Threaded, Atha and mpi environments.
#
# ==========================================================================

#=======================
# Section to name the generated binaries
#=======================
if USE_MPI
   PROG_MPI=cqueenmpi oqueenmpi 
endif

if USE_ATHA
   PROG_ATHA=cqueenatha oqueenatha 
endif

PROG_SEQ = cqueen oqueen 
PROG_THR = cqueenthr oqueenthr

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
cqueen_SOURCES=queen.cpp
cqueen_CPPFLAGS=${AM_CPPFLAGS} -DCOUNT

cqueenthr_SOURCES=queen.cpp
cqueenthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded -DCOUNT

oqueen_SOURCES=queen.cpp

oqueenthr_SOURCES=queen.cpp
oqueenthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded


#=======================
# The Atha and MPI environment
#=======================
if USE_ATHA
  cqueenatha_SOURCES=queen.cpp
  cqueenatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  cqueenatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DCOUNT -DAtha

  oqueenatha_SOURCES=queen.cpp
  oqueenatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  oqueenatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha
endif

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@

  cqueenmpi_SOURCES = queen.cpp
  cqueenmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI -DCOUNT
  cqueenmpi_LINK = $(MPICXXLINK)
cqueenmpi-queen.o: queen.cpp
	$(MPICXXCOMPILE) -DCOUNT -DMPxMPI -c -o $@ $< 

  oqueenmpi_SOURCES = queen.cpp
  oqueenmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  oqueenmpi_LINK = $(MPICXXLINK)
oqueenmpi-queen.o: queen.cpp
	$(MPICXXCOMPILE) -DMPxMPI -c -o $@ $< 
endif

