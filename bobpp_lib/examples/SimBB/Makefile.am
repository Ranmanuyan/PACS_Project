# ==========================================================================
# Copyright(c)'2005 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for example/test Bob++ library
#
# ==========================================================================

if USE_ATHA
  PROG_ATHA=bbatha
endif

if USE_MPI
  PROG_MPI=bbmpi
endif

#if USE_CELL
#  PROG_CELL=bbcell
#endif

noinst_PROGRAMS= bbseq bbthr ${PROG_ATHA} ${PROG_MPI} ${PROG_CELL}

CLEANFILES=${noinst_PROGRAMS} *.o

LDADD=${prefix}/lib/libbobpp-1.0.a

#AM_LDFLAGS=-L${prefix}/lib -l${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g
bbseq_SOURCES=seq.cpp
bbthr_SOURCES=thr.cpp

if USE_ATHA
  bbatha_SOURCES=atha.cpp
  bbatha_LDFLAGS = ${LIBATHA} -lbobpp_atha
  bbatha_CPPFLAGS = $(AM_CPPFLAGS) ${INCATHA}
endif

if USE_MPI
MPICXXCOMPILE = $(MPICXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
MPICXXLINK = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
       --mode=link $(MPICXX) $(AM_CXXFLAGS) $(CXXFLAGS) \
       -lbobpp_mpi -o $@
  bbmpi_SOURCES = mp.cpp
  bbmpi_LINK = $(MPICXXLINK)
mp.o: mp.cpp
	$(MPICXXCOMPILE) -c -o $@ $<

endif

#if USE_CELL
# MOSTLYCLEANFILES = cellbe_spe
# SPUCOMPILE = $(SPUCXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
#       $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
#
# bbcell_SOURCES = cellbe.cpp
# bbcell_LDADD = cellbe_spe_csf.o
# bbcell_LDFLAGS = $(AM_LDFLAGS) $(LIBCELL)
# #bbcell_LINK = $(PPULINK)
#
# cellbe_spe_csf.o: cellbe_spe
#	${EMBEDSPU} bobpp_spu_handle $< $@
#
# cellbe_spe: cellbe_spe.cpp
#	$(SPUCOMPILE) -o  $@ $<
#endif
