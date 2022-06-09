# ==========================================================================
# Copyright(c)'2005 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for example/Mip Bob++ library
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

EXTRA_DIST=roy.lp

noinst_PROGRAMS=${PROG_GLOP}

CLEANFILES=${noinst_PROGRAMS} *.o

LDADD=-lbobpp-${GENERIC_API_VERSION}
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@

if USE_GLOP
  mipmpi_SOURCES = mip.cpp
  mipmpi_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} ${INCGLOCK} -DMPxMPI
  mipmpi_LINK = $(MPICXXLINK) ${LIBGLOP} -lglop_serialize ${LIBGLOCK}
mipmpi-mip.o: mip.cpp
	$(MPICXXCOMPILE) ${INCGLOP} -DMPxMPI -c -o $@ $<
endif
endif

if USE_GLOP
  mipseq_SOURCES = mip.cpp
  mipseq_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} ${INCGLOCK}
  mipseq_LDFLAGS = ${LIBGLOP} -lglop_serialize ${LIBGLOCK}

  mipthr_SOURCES = mip.cpp
  mipthr_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} ${INCGLOCK} -DThreaded
  mipthr_LDFLAGS = ${LIBGLOP} -lglop_serialize ${LIBGLOCK}
if USE_ATHA
    mipatha_SOURCES = mip.cpp
    mipatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCGLOP} ${INCATHA} ${INCGLOCK} -DAtha
    mipatha_LDFLAGS = ${LIBGLOP} -lbobpp_atha -lglop_serialize ${LIBGLOCK} ${LIBATHA}
endif
endif

