# ==========================================================================
# Copyright(c)'2008 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for examples/LP/example Bob++ library
#
# ==========================================================================

if USE_GLOP
if USE_PPGLOP
if USE_ATHA
  PROG_ATHA=mipatha mipcutatha
endif

noinst_PROGRAMS = mip mipthr mipcut mipcutthr $(PROG_ATHA)

AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}

mip_SOURCES = mip.cpp
mip_CPPFLAGS= $(AM_CPPFLAGS) ${INCGLOP} 
mip_LDFLAGS= ${LIBGLOP} ${LIBPPGLOP} -lglop_serialize
mip_LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a

if USE_ATHA
mipatha_SOURCES = mip.cpp
mipatha_CPPFLAGS= $(AM_CPPFLAGS) ${INCGLOP} ${INCATHA} -DAtha -I../src
mipatha_LDFLAGS= ${LIBGLOP} ${LIBPPGLOP} -lglop_serialize ${LIBATHA} -lbobpp_atha
mipatha_LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a
endif

mipthr_SOURCES = mip.cpp
mipthr_CPPFLAGS= $(AM_CPPFLAGS) ${INCGLOP} -I../src -DThreaded
mipthr_LDFLAGS= ${LIBGLOP} ${LIBPPGLOP} -lglop_serialize
mipthr_LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a

mipcut_SOURCES = mipcut.cpp
mipcut_CPPFLAGS= $(AM_CPPFLAGS) ${INCGLOP} -I../src
mipcut_LDFLAGS= ${LIBGLOP} ${LIBPPGLOP} ${LIBGLOCK} -lglop_serialize
mipcut_LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a

mipcutthr_SOURCES = mipcut.cpp
mipcutthr_CPPFLAGS= $(AM_CPPFLAGS) ${INCGLOP} -I../src -DThreaded
mipcutthr_LDFLAGS= ${LIBGLOP} ${LIBPPGLOP} ${LIBGLOCK} -lglop_serialize
mipcutthr_LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a

if USE_ATHA
mipcutatha_SOURCES = mipcut.cpp
mipcutatha_CPPFLAGS= $(AM_CPPFLAGS) ${INCGLOP} ${INCATHA} -DAtha -I../src
mipcutatha_LDFLAGS= ${LIBGLOP} ${LIBPPGLOP}  ${LIBGLOCK} -lglop_serialize ${LIBATHA} -lbobpp_atha
mipcutatha_LDADD=${prefix}/lib/libbobpp-1.0.a ${prefix}/lib/liblpbobpp-1.0.a
endif

endif
endif
