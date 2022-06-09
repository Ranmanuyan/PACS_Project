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
   PROG_MPI=cksmpi bksmpi 
endif

if USE_ATHA
   PROG_ATHA=cksatha bksatha 
endif

PROG_SEQ = cks bks 
PROG_THR = cksthr bksthr

noinst_PROGRAMS= ${PROG_SEQ} ${PROG_THR} ${PROG_ATHA} ${PROG_MPI}

#=======================
# General section
#=======================
CLEANFILES=${noinst_PROGRAMS} *.o

LDADD=-L${prefix}/lib -lbobpp-${GENERIC_API_VERSION}
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

#=======================
# The Knapsack Section (sequential and thr)
#=======================
cks_SOURCES=ks.cpp
cks_CPPFLAGS=${AM_CPPFLAGS} -DCOUNT

cksthr_SOURCES=ks.cpp
cksthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded -DCOUNT

bks_SOURCES=ks.cpp

bksthr_SOURCES=ks.cpp
bksthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded


#=======================
# The Atha and MPI environment
#=======================
if USE_ATHA
  cksatha_SOURCES=ks.cpp
  cksatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  cksatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DCOUNT -DAtha

  bksatha_SOURCES=ks.cpp
  bksatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  bksatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha
endif

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@

  cksmpi_SOURCES = ks.cpp
  cksmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI -DCOUNT
  cksmpi_LINK = $(MPICXXLINK)
cksmpi-ks.o: ks.cpp
	$(MPICXXCOMPILE) -DCOUNT -DMPxMPI -c -o $@ $< 

  bksmpi_SOURCES = ks.cpp
  bksmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  bksmpi_LINK = $(MPICXXLINK)
bksmpi-ks.o: ks.cpp
	$(MPICXXCOMPILE) -DMPxMPI -c -o $@ $< 
endif

