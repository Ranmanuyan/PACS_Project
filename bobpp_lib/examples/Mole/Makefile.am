# ==========================================================================
# Copyright(c)'2005 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for examples/TestDC Bob++ library
#
#	This makefile.am is used to generate the real makefile using the GNU
#	autotools suite.
#	The example is :
#	From thses source codes, we are able to execute the resolution in the 
#	Sequential, Threaded, Atha and mpi environments.
#
# ==========================================================================

#=======================
# Section to name the generated binaries
#=======================
if USE_MPI
   PROG_MPI=cmolempi omolempi ctrapmpi otrapmpi
endif

if USE_ATHA
   PROG_ATHA=cmoleatha omoleatha ctrapatha otrapatha
endif

PROG_SEQ = cmole omole ctrap otrap
PROG_THR = cmolethr omolethr ctrapthr otrapthr

noinst_PROGRAMS= ${PROG_SEQ} ${PROG_THR} ${PROG_ATHA} ${PROG_MPI}

#=======================
# General section
#=======================
CLEANFILES=${noinst_PROGRAMS} *.o

LDADD=-L${prefix}/lib -lbobpp-${GENERIC_API_VERSION}
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

#=======================
# The Mole (sequential and thr)
#=======================
cmole_SOURCES=mole.cpp
cmole_CPPFLAGS=${AM_CPPFLAGS} -DCOUNT

cmolethr_SOURCES=mole.cpp
cmolethr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded -DCOUNT

omole_SOURCES=mole.cpp

omolethr_SOURCES=mole.cpp
omolethr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded

ctrap_SOURCES=trap.cpp
ctrap_CPPFLAGS=${AM_CPPFLAGS} -DCOUNT

ctrapthr_SOURCES=trap.cpp
ctrapthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded -DCOUNT

otrap_SOURCES=trap.cpp

otrapthr_SOURCES=trap.cpp
otrapthr_CPPFLAGS=${AM_CPPFLAGS} -DThreaded

#=======================
# The Atha and MPI environment
#=======================
if USE_ATHA
  cmoleatha_SOURCES=mole.cpp
  cmoleatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  cmoleatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DCOUNT -DAtha

  omoleatha_SOURCES=mole.cpp
  omoleatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  omoleatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha


  ctrapatha_SOURCES=trap.cpp
  ctrapatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  ctrapatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DCOUNT -DAtha

  otrapatha_SOURCES=trap.cpp
  otrapatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  otrapatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA} -DAtha

endif
if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@

  cmolempi_SOURCES = mole.cpp
  cmolempi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI -DCOUNT
  cmolempi_LINK = $(MPICXXLINK)
cmolempi-mole.o: mole.cpp
	$(MPICXXCOMPILE) -DCOUNT -DMPxMPI -c -o $@ $< 

  omolempi_SOURCES = mole.cpp
  omolempi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  omolempi_LINK = $(MPICXXLINK)
omolempi-mole.o: mole.cpp
	$(MPICXXCOMPILE) -DMPxMPI -c -o $@ $< 

  ctrapmpi_SOURCES = trap.cpp
  ctrapmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI -DCOUNT
  ctrapmpi_LINK = $(MPICXXLINK)
ctrapmpi-trap.o: trap.cpp
	$(MPICXXCOMPILE) -DCOUNT -DMPxMPI -c -o $@ $< 

  otrapmpi_SOURCES = trap.cpp
  otrapmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  otrapmpi_LINK = $(MPICXXLINK)
otrapmpi-trap.o: trap.cpp
	$(MPICXXCOMPILE) -DMPxMPI -c -o $@ $< 
endif

