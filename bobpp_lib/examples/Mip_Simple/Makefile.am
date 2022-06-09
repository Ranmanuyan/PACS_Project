# ==========================================================================
# Copyright(c)'2005 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for example/test Bob++ library
#
# ==========================================================================

if USE_GLOP
if USE_ATHA
    PROG_GLOP_ATHA=mipatha
endif
if USE_MPI
    PROG_GLOP_MPI=mipmpi
endif
  PROG_GLOP=mipseq mipthr ${PROG_GLOP_ATHA} ${PROG_GLOP_MPI}
endif

noinst_PROGRAMS= ${PROG_GLOP} 

CLEANFILES=${noinst_PROGRAMS} *.o

LDADD=-L${prefix}/lib -lbobpp-${GENERIC_API_VERSION} 
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g


if USE_GLOP
  mipseq_SOURCES = mip.cpp
  mipseq_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP}
  mipseq_LDFLAGS = ${LIBGLOP} -lglop_serialize

  mipthr_SOURCES = mip.cpp
  mipthr_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DThreaded
  mipthr_LDFLAGS = ${LIBGLOP} -lglop_serialize

if USE_MPI
  mipmpi_SOURCES = mip.cpp
  mipmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} -DMPxMPI
  mipmpi_LINK = $(MPICXXLINK) ${LIBGLOP} -lglop_serialize
mipmpi-mip.o: mip.cpp
	$(MPICXXCOMPILE) ${INCGLOP} -DMPxMPI -c -o $@ $<
endif

if USE_ATHA
    mipatha_SOURCES = mip.cpp
    mipatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} ${INCATHA} -DAtha
    mipatha_LDFLAGS = ${LIBGLOP} -lglop_serialize ${LIBATHA} -lbobpp_atha
endif
endif

