# ==========================================================================
# Copyright(c)'2004,2005,2006,2007,2008 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: B. Le Cun
# Makefile.am for Bob++ library
#
# ==========================================================================

BOBO_LIB_NAME=lib${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION}.la

noinst_PROGRAMS= opt1 opt2 bbnode pri pqskew bv
CLEANFILES=${noinst_PROGRAMS} *.o
LDADD=-L${prefix}/lib -lbobpp-${GENERIC_API_VERSION} 
AM_CPPFLAGS=-I${prefix}/include/${GENERIC_LIBRARY_NAME}-${GENERIC_API_VERSION} -g

bv_SOURCES=bv.cpp
opt1_SOURCES=opt1.cpp
opt2_SOURCES=opt2.cpp
bbnode_SOURCES=bbnode.cpp
pri_SOURCES=pri.cpp
pqskew_SOURCES=pq.cpp

