# ==========================================================================
# Copyright(c)'2006 by PRiSM Laboratory Opale Team
# see the copyright file for more information
# Author: F. Galea
# Makefile.am for Bob++ tools
# ==========================================================================

if USE_GIW
  PROG_GIW = bobview bobloggui 
endif
if COND_SOCKET
  PROG_SOCKET = boblistener
endif

bin_PROGRAMS = ${PROG_GIW} ${PROG_SOCKET} bobplot bob2csv

bobview_CFLAGS = `pkg-config --cflags gtk+-2.0 giw`
bobview_LDFLAGS = `pkg-config --libs gtk+-2.0 giw`

bobloggui_SOURCES = bobloggui.cpp boblogview.hpp boblog.cpp boblog.hpp boblogview.cpp
bobloggui_CXXFLAGS = `pkg-config --cflags gtk+-2.0 giw`
bobloggui_LDADD=-L../bobpp -lbobpp-${GENERIC_API_VERSION}
bobloggui_LDFLAGS = `pkg-config --libs gtk+-2.0 giw`

bobplot_SOURCES = bobplot.cpp
bobplot_LDADD=-L../bobpp -lbobpp-${GENERIC_API_VERSION}

bob2csv_SOURCES = bob2csv.cpp
bob2csv_LDADD=-L../bobpp -lbobpp-${GENERIC_API_VERSION}
