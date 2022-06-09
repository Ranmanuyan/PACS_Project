# ==========================================================================
# Copyright(c)'2004 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Bob++ library
#
# ==========================================================================

if USE_ATHA
  SUBDIR_ATHA=atha
endif

if USE_MPI
  SUBDIR_MPX=mpx
endif

#if USE_CELL
#  SUBDIR_CELL=cell
#endif

if USE_GLOP
if USE_PPGLOP
  SUBDIR_ILP=ilp
endif
endif

SUBDIRS = thr util ${SUBDIR_ATHA} ${SUBDIR_MPX} ${SUBDIR_ILP} ${SUBDIR_CELL}
#
# bobpp
#
h_sources= bobpp bobpp.h\
bob_balgo.h bob_conf.h  bob_envprog.h  bob_gdsbase.h   bob_opt.h   bob_schedalgo.h\
bob_simspalgo.h bob_core.h  bob_dspq.h    bob_error.h    bob_pri.h   bob_sens.h\
bob_bbalgo.h  bob_cost.h  bob_eval.h     bob_io.h    bob_prob.h bob_keystate.hpp\
bob_stat.h bob_dcalgo.h bob_log.h bob_id.h bob_cdcalgo.h bob_sdcalgo.h\
bob_vnsalgo.h bob_pqnode.h\
bobpp_config.h

cc_sources=\
bob_error.cpp  \
bob_io.cpp  \
bob_core.cpp  \
bob_log.cpp  \
bob_stat.cpp  \
bob_opt.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)


lib_LTLIBRARIES=libbobpp-1.0.la
libbobpp_1_0_la_SOURCES= $(h_sources) $(cc_sources)
libbobpp_1_0_la_LIBADD= thr/libthr.la util/libutil.la
libbobpp_1_0_la_LDFLAGS= -no-undefined -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)

