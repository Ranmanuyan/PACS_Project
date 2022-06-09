#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([bobpp], [0.3.0], [Bertrand.Lecun@prism.uvsq.fr])
AC_CONFIG_SRCDIR([bobpp])
AC_CONFIG_HEADER([config.h])
AX_PREFIX_CONFIG_H(bobpp/bobpp_config.h)
AC_CONFIG_MACRO_DIR([m4])

dnl -----------------------------------------------
dnl Package name and version number (user defined)
dnl -----------------------------------------------

GENERIC_LIBRARY_NAME=bobpp

#release versioning
GENERIC_MAJOR_VERSION=0
GENERIC_MINOR_VERSION=3
GENERIC_MICRO_VERSION=0

#API version (often = GENERIC_MAJOR_VERSION.GENERIC_MINOR_VERSION)
GENERIC_API_VERSION=1.0
AC_SUBST(GENERIC_API_VERSION)

#shared library versioning
GENERIC_LIBRARY_VERSION=1:0:0
#                       | | |
#                +------+ | +---+
#                |        |     |
#             current:revision:age
#                |        |     |
#                |        |     +- increment if interfaces have been added
#                |        |        set to zero if interfaces have been removed
#                                  or changed
#                |        +- increment if source code has changed
#                |           set to zero if current is incremented
#                +- increment if interfaces have been added, removed or changed


dnl --------------------------------
dnl Package name and version number
dnl --------------------------------

dnl -------- the core of the library
AC_SUBST(GENERIC_LIBRARY_VERSION)

PACKAGE=$GENERIC_LIBRARY_NAME
AC_SUBST(GENERIC_LIBRARY_NAME)

GENERIC_VERSION=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION.$GENERIC_MICRO_VERSION
GENERIC_RELEASE=$GENERIC_MAJOR_VERSION.$GENERIC_MINOR_VERSION
AC_SUBST(GENERIC_RELEASE)
AC_SUBST(GENERIC_VERSION)

VERSION=$GENERIC_VERSION

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)


dnl -------- configure options

AC_ARG_ENABLE(debug,
AC_HELP_STRING([--enable-debug],[use it to obtain the debugging informations in Bob++]),
[enable_debug=$enableval],
[enable_debug=no]
)

AC_ARG_ENABLE(space,
AC_HELP_STRING([--enable-space],[use it to obtain the space generation of a Bob++ application]),
[enable_space=$enableval],
[enable_space=no]
)

AC_ARG_ENABLE(ft,
AC_HELP_STRING([--enable-ft],[use it to obtain the fault tolerancy in the Bob++ application]),
[enable_ft=$enableval],
[enable_ft=no]
)

AC_ARG_WITH([glop],
  [AS_HELP_STRING([--with-glop=@<:@ARG@:>@],
    [Use Glop (possible values : no, check, glpk, lpsolve, clp, cplex, xpress) @<:@default=check@:>@])],
  [],
  [with_glop=check])

AC_ARG_WITH([glock],
  [AS_HELP_STRING([--with-glock=@<:@ARG@:>@],
    [Use Glock (possible values : no, yes, check) @<:@default=check@:>@])],
  [],
  [with_glock=check])

AC_ARG_WITH([kaapi],
  [AS_HELP_STRING([--with-kaapi=@<:@ARG@:>@],
    [Use kaapi (possible values : no, yes, check, or the path to Kaapi) @<:@default=check@:>@])],
  [],
  [with_kaapi=check])

AC_ARG_WITH([mpi],
  [AS_HELP_STRING([--with-mpi=@<:@ARG@:>@],
    [Use MPI (possible values : no, yes, check) @<:@default=check@:>@])],
  [],
  [with_mpi=check])

#AC_ARG_WITH([mpc],
#  [AS_HELP_STRING([--with-mpc=@<:@ARG@:>@],
#    [Use MPC (possible values : no or the path to the installation) @<:@default=no@:>@])],
#  [],
#  [with_mpc=no])

#AC_ARG_WITH([cell],
  #[AS_HELP_STRING([--with-cell=@<:@ARG@:>@],
    #[Use Cell B.E. (possible values : no, yes, check) @<:@default=check@:>@])],
  #[],
  #[with_cell=check])

AC_ARG_WITH([giw],
  [AS_HELP_STRING([--with-giw=@<:@ARG@:>@],
    [Use giw to generate bobview (value=installation path) @<:@default=check@:>@])],
  [],
  [with_giw=check])

with_ppglop=no

dnl -------- the doxygen configuration
DX_INIT_DOXYGEN($GENERIC_LIBRARY_NAME, "doxygen.cfg")
DX_HTML_FEATURE(ON) 
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_LANG([C++])
AC_CHECK_PROG(HAVE_PKG_CONFIG,pkg-config,yes)
AS_IF([test "x$HAVE_PKG_CONFIG" != "xyes"],
      [AC_MSG_FAILURE([The pkg-config program must be installed.])])

# Checks for libraries.
AC_CHECK_LIB(pthread, pthread_create)
AC_CHECK_LIB(pthreadGC2, pthread_create)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

# Add warnings if using gcc/g++
if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall"
  CXXFLAGS="$CXXFLAGS -Wall"
fi

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([gettimeofday socket pthread_setaffinity_np])

# Checks for the debugging Bob++
AC_MSG_CHECKING(for the Bob++ debug feature)
if test "$enable_debug" != "no"; then
 AC_DEFINE(ENABLE_DEBUG,1, [Define if debugging is enabled.])
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi

# Checks for the space generation in Bob++
AC_MSG_CHECKING(for the Bob++ space generation)
if test "$enable_space" != "no"; then
 AC_DEFINE(ENABLE_SPACE,1, [Define if space generation is enabled.])
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi

# Checks for the fault tolerancy in Bob++
AC_MSG_CHECKING(for the Bob++ fault tolerancy)
if test "$enable_ft" != "no"; then
 AC_DEFINE(ENABLE_FT,1, [Define if fault tolerancy is enabled.])
 AC_MSG_RESULT(yes)
else
 AC_MSG_RESULT(no)
fi

# Checks for the Glop library.
AC_MSG_CHECKING(for glop library module)
INCGLOP=
LIBGLOP=
AS_IF([test "x$with_glop" != xno],
 [if test "x$with_glop" = xcheck; then
    for i in glpk lpsolve clp cplex xpress; do
      if pkg-config --exists glop_$i ; then with_glop=$i ; break ; fi
    done
  else
    pkg-config --exists glop_$with_glop || AC_MSG_FAILURE(
      [--with-glop=$with_glop was given, but test for $with_glop failed])
  fi
  if test "x$with_glop" != xcheck; then
    AC_SUBST([INCGLOP], ["`pkg-config --cflags glop_$with_glop`"])
    AC_SUBST([LIBGLOP], ["`pkg-config --libs glop_$with_glop`"])
    AC_DEFINE_UNQUOTED([HAVE_LIBGLOP], [$with_glop],
              [Defined if you have libglop])
  else
    with_glop=no
  fi
 ])
AC_MSG_RESULT($with_glop)

# Checks for the PPGlop library.
AS_IF([test "x$with_glop" != xno],
 [AC_MSG_CHECKING(for ppglop)
  INCPPGLOP=
  LIBPPGLOP=
  if pkg-config --exists ppglop ; then with_ppglop=yes ; fi
  if test "x$with_ppglop" != xno; then
    AC_SUBST([INCPPGLOP], ["`pkg-config --cflags glop_$with_glop ppglop`"])
    AC_SUBST([LIBPPGLOP], ["`pkg-config --libs glop_$with_glop ppglop`"])
    AC_DEFINE([HAVE_LIBPPGLOP], [1], [Defined if you have libppglop])
  fi
  AC_MSG_RESULT($with_ppglop)
 ])

# Checks for the Glock library.
AS_IF([test "x$with_glop" != xno],
 [AC_MSG_CHECKING(for glock)
  INCGLOCK=
  LIBGLOCK=
  AS_IF([test "x$with_glock" != xno],
    [if test "x$with_glock" = xcheck; then
       pkg-config --exists glock && with_glock=yes
     else
       pkg-config --exists glock || AC_MSG_FAILURE(
         [--with-glock=yes was given, but test for glock failed])
     fi
     if test "x$with_glock" != xcheck ; then
       AC_SUBST([INCGLOCK], ["`pkg-config --cflags glock`"])
       AC_SUBST([LIBGLOCK], ["`pkg-config --libs glock`"])
       AC_DEFINE([HAVE_LIBGLOCK], [1],
	         [Defined if you have libglock])
     else
       with_glock=no
     fi
    ])
  AC_MSG_RESULT($with_glock)
 ], [with_glock=no])

# Checks for the Kaapi library.
AC_MSG_CHECKING(for Kaapi library)
INCATHA=
LIBATHA=
AS_IF([test "x$with_kaapi" != xno],
  [if test "x$with_kaapi" = xcheck; then
     pkg-config --exists kaapi && with_kaapi=yes
   elif test "x$with_kaapi" = xyes; then
     pkg-config --exists kaapi || AC_MSG_FAILURE(
       [--with-kaapi=yes was given, but test for Kaapi failed])
   fi
   if test "x$with_kaapi" = xyes ; then
     AC_SUBST([INCATHA], ["`pkg-config --cflags kaapi`"])
     AC_SUBST([LIBATHA], ["`pkg-config --libs kaapi`"])
     AC_DEFINE([HAVE_LIBATHA], [1],
	       [Defined if you have Kaapi])
   else
     with_kaapi=no
   fi
  ])
AC_MSG_RESULT($with_kaapi)

# Checks for a MPI library.
AS_IF([test "x$with_mpi" != xno],
  [if test "x$with_mpi" = xcheck; then
     ACX_MPI([AC_DEFINE([HAVE_MPI], 1, [Defined if you have libmpi])
              with_mpi=yes], with_mpi=no)
   elif test "x$with_mpi" = xyes; then
     ACX_MPI(AC_DEFINE([HAVE_MPI], 1, [Defined if you have libmpi]),
             with_mpi=no)
     test "x$with_mpi" = xyes || AC_MSG_FAILURE(
       [--with-mpi=$with_mpi was given, but test for $with_mpi failed])
   fi
  ])

# Checks for the MPC library.
#AC_MSG_CHECKING(for mpc library)
#AS_IF([test "x$with_mpc" != xno],
#  [ if test -d "$with_mpc"; then
#     AC_DEFINE([HAVE_LIBMPC], [1],
#	       [Defined if you have libmpc])
#   else
#     with_mpc=no
#   fi
#  ])
#AC_MSG_RESULT($with_mpc)

# Checks for a Cell Broadband Engine programming environment
#SPUCC=
#SPUCXX=
#EMBEDSPU=
#AS_IF([test "x$with_cell" != xno],
  #[if test "$host_cpu" != powerpc -a "$host_cpu" != powerpc64; then
     #with_cell=no
   #fi
   #if test "x$with_cell" = xcheck; then
     #AC_CHECK_PROGS(SPUCC,spu-gcc)
     #if test "x$SPUCC" != "x"; then
       #AC_CHECK_PROGS(SPUCXX,spu-g++)
       #AC_CHECK_PROGS(EMBEDSPU,["$host_alias"-embedspu embedspu])
       #AC_CHECK_LIB(spe2, spe_context_create, have_libspe2=ok)
       #test "x$SPUCC" != "x" -a "x$SPUCXX" != "x" \
         #-a "x$have_libspe2" != "x" \
         #-a "x$EMBEDSPU" != "x" && with_cell=yes
     #fi
   #fi
   #if test "x$with_cell" = xyes ; then
     #AC_MSG_CHECKING(whether $SPUCXX requires -ffreestanding option)
     #AC_LANG_CONFTEST(
       #int main(unsigned long long spe_id, unsigned long long pdata) {
         #return 0;
       #}
     #)
     #with_freestanding=
#     ac_compile="$SPUCXX -c conftest.cpp"
#     if eval "$ac_compile" 2>conftest.err ; then
#       with_freestanding=no
#     else
#       with_freestanding=yes
#       SPUCXX="$SPUCXX -ffreestanding"
#     fi
#     AC_MSG_RESULT($with_freestanding)
#     AC_DEFINE([HAVE_LIBCELL], [1],
#               [Defined if you have the Cell Broadband Engine environment])
#     AC_SUBST(SPUCC)
#     AC_SUBST(SPUCXX)
#     AC_SUBST(EMBEDSPU)
#     AC_SUBST([INCCELL], [""])
#     AC_SUBST([LIBCELL], ["-lbobpp_cell -lspe2 -lpthread"])
#   else
#     with_cell=no
#   fi
#  ])
#AC_MSG_CHECKING(for Cell Broadband Engine environment)
#AC_MSG_RESULT($with_cell)

#echo $host $host_alias $ac_cv_host $host_cpu

# Eventually add compile options for Intel 32 bit architecture
opt=
AC_MSG_CHECKING(for host-specific tuning options)
AS_IF([test x"$host_cpu" = xi686],
  [opt=-march=i686], [opt=none])
AS_IF([test x"$opt" != xnone], [CXX="$CXX $opt"])
AC_MSG_RESULT($opt)

# Checks for the giw library.
AC_MSG_CHECKING(for giw library)
INCGIW=
LIBGIW=
AS_IF([test "x$with_giw" != xno],
  [if test "x$with_giw" = xcheck; then
     if pkg-config --exists giw ; then with_giw=yes ; fi
   fi
   if test "x$with_giw" != xcheck; then
     AC_SUBST([INCGIW], ["`pkg-config --cflags giw`"])
     AC_SUBST([LIBGIW], ["`pkg-config --libs giw`"])
     AC_DEFINE([HAVE_LIBGIW], [1],
	       [Defined if you have libgiw])
   else
     with_giw=no
   fi
  ])
AC_MSG_RESULT($with_giw)


# Conditional variables
AM_CONDITIONAL(USE_GLOP, test "x$with_glop" != "xno" )
AM_CONDITIONAL(USE_PPGLOP, test "x$with_ppglop" != "xno" )
AM_CONDITIONAL(USE_GLOCK, test "x$with_glock" != "xno" )
AM_CONDITIONAL(USE_ATHA, test "x$with_kaapi" != "xno" )
AM_CONDITIONAL(USE_MPI, test "x$with_mpi" != "xno" )
#AM_CONDITIONAL(USE_CELL, test "x$with_cell" != "xno" )
#AM_CONDITIONAL(USE_MPC, test "x$with_mpc" != "xno" )
AM_CONDITIONAL(USE_GIW, test "x$with_giw" != "xno" )
AM_CONDITIONAL([COND_SOCKET], [test "$ac_cv_func_socket" = "yes"])

AC_OUTPUT(Makefile \
                 bobpp/Makefile \
                 bobpp/thr/Makefile \
                 bobpp/util/Makefile \
                 bobpp/atha/Makefile \
                 bobpp/atha/bobpp_atha.pc \
                 bobpp/ilp/Makefile \
                 bobpp/ilp/bobpp_lp.pc \
                 bobpp/mpx/Makefile \
                 bobpp/mpx/bobpp_mpi.pc \
                 examples/Makefile \
                 examples/Mole/Makefile \
                 examples/flowshop/Makefile \
                 examples/flowshop/DC/Makefile \
                 examples/flowshop/BB/Makefile \
                 examples/ks/Makefile \
                 examples/Mip_Bobo/Makefile \
                 examples/Mip_Cuts/Makefile \
                 examples/Mip_Simple/Makefile \
                 examples/Qap/Makefile \
                 examples/UQ01/Makefile \
                 examples/queen/Makefile \
                 examples/SimBB/Makefile \
                 examples/vrp/Makefile \
                 examples/Tests/Makefile \
                 examples/sat/Makefile \
                 tools/Makefile \
		 bobpp.pc)
