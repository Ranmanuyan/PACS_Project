# This file is part of Autoconf.                       -*- Autoconf -*-


# ===========================================================================
#            http://autoconf-archive.cryp.to/ax_prog_doxygen.html
# ===========================================================================
#
# SYNOPSIS
#
#   DX_INIT_DOXYGEN(PROJECT-NAME, DOXYFILE-PATH, [OUTPUT-DIR])
#   DX_DOXYGEN_FEATURE(ON|OFF)
#   DX_DOT_FEATURE(ON|OFF)
#   DX_HTML_FEATURE(ON|OFF)
#   DX_CHM_FEATURE(ON|OFF)
#   DX_CHI_FEATURE(ON|OFF)
#   DX_MAN_FEATURE(ON|OFF)
#   DX_RTF_FEATURE(ON|OFF)
#   DX_XML_FEATURE(ON|OFF)
#   DX_PDF_FEATURE(ON|OFF)
#   DX_PS_FEATURE(ON|OFF)
#
# DESCRIPTION
#
#   The DX_*_FEATURE macros control the default setting for the given
#   Doxygen feature. Supported features are 'DOXYGEN' itself, 'DOT' for
#   generating graphics, 'HTML' for plain HTML, 'CHM' for compressed HTML
#   help (for MS users), 'CHI' for generating a seperate .chi file by the
#   .chm file, and 'MAN', 'RTF', 'XML', 'PDF' and 'PS' for the appropriate
#   output formats. The environment variable DOXYGEN_PAPER_SIZE may be
#   specified to override the default 'a4wide' paper size.
#
#   By default, HTML, PDF and PS documentation is generated as this seems to
#   be the most popular and portable combination. MAN pages created by
#   Doxygen are usually problematic, though by picking an appropriate subset
#   and doing some massaging they might be better than nothing. CHM and RTF
#   are specific for MS (note that you can't generate both HTML and CHM at
#   the same time). The XML is rather useless unless you apply specialized
#   post-processing to it.
#
#   The macros mainly control the default state of the feature. The use can
#   override the default by specifying --enable or --disable. The macros
#   ensure that contradictory flags are not given (e.g.,
#   --enable-doxygen-html and --enable-doxygen-chm,
#   --enable-doxygen-anything with --disable-doxygen, etc.) Finally, each
#   feature will be automatically disabled (with a warning) if the required
#   programs are missing.
#
#   Once all the feature defaults have been specified, call DX_INIT_DOXYGEN
#   with the following parameters: a one-word name for the project for use
#   as a filename base etc., an optional configuration file name (the
#   default is 'Doxyfile', the same as Doxygen's default), and an optional
#   output directory name (the default is 'doxygen-doc').
#
#   Automake Support
#
#   The following is a template aminclude.am file for use with Automake.
#   Make targets and variables values are controlled by the various
#   DX_COND_* conditionals set by autoconf.
#
#   The provided targets are:
#
#     doxygen-doc: Generate all doxygen documentation.
#
#     doxygen-run: Run doxygen, which will generate some of the
#                  documentation (HTML, CHM, CHI, MAN, RTF, XML)
#                  but will not do the post processing required
#                  for the rest of it (PS, PDF, and some MAN).
#
#     doxygen-man: Rename some doxygen generated man pages.
#
#     doxygen-ps:  Generate doxygen PostScript documentation.
#
#     doxygen-pdf: Generate doxygen PDF documentation.
#
#   Note that by default these are not integrated into the automake targets.
#   If doxygen is used to generate man pages, you can achieve this
#   integration by setting man3_MANS to the list of man pages generated and
#   then adding the dependency:
#
#     $(man3_MANS): doxygen-doc
#
#   This will cause make to run doxygen and generate all the documentation.
#
#   The following variable is intended for use in Makefile.am:
#
#     DX_CLEANFILES = everything to clean.
#
#   Then add this variable to MOSTLYCLEANFILES.
#
#     ----- begin aminclude.am -------------------------------------
#
#     ## --------------------------------- ##
#     ## Format-independent Doxygen rules. ##
#     ## --------------------------------- ##
#
#     if DX_COND_doc
#
#     ## ------------------------------- ##
#     ## Rules specific for HTML output. ##
#     ## ------------------------------- ##
#
#     if DX_COND_html
#
#     DX_CLEAN_HTML = @DX_DOCDIR@/html
#
#     endif DX_COND_html
#
#     ## ------------------------------ ##
#     ## Rules specific for CHM output. ##
#     ## ------------------------------ ##
#
#     if DX_COND_chm
#
#     DX_CLEAN_CHM = @DX_DOCDIR@/chm
#
#     if DX_COND_chi
#
#     DX_CLEAN_CHI = @DX_DOCDIR@/@PACKAGE@.chi
#
#     endif DX_COND_chi
#
#     endif DX_COND_chm
#
#     ## ------------------------------ ##
#     ## Rules specific for MAN output. ##
#     ## ------------------------------ ##
#
#     if DX_COND_man
#
#     DX_CLEAN_MAN = @DX_DOCDIR@/man
#
#     endif DX_COND_man
#
#     ## ------------------------------ ##
#     ## Rules specific for RTF output. ##
#     ## ------------------------------ ##
#
#     if DX_COND_rtf
#
#     DX_CLEAN_RTF = @DX_DOCDIR@/rtf
#
#     endif DX_COND_rtf
#
#     ## ------------------------------ ##
#     ## Rules specific for XML output. ##
#     ## ------------------------------ ##
#
#     if DX_COND_xml
#
#     DX_CLEAN_XML = @DX_DOCDIR@/xml
#
#     endif DX_COND_xml
#
#     ## ----------------------------- ##
#     ## Rules specific for PS output. ##
#     ## ----------------------------- ##
#
#     if DX_COND_ps
#
#     DX_CLEAN_PS = @DX_DOCDIR@/@PACKAGE@.ps
#
#     DX_PS_GOAL = doxygen-ps
#
#     doxygen-ps: @DX_DOCDIR@/@PACKAGE@.ps
#
#     @DX_DOCDIR@/@PACKAGE@.ps: @DX_DOCDIR@/@PACKAGE@.tag
#   	  cd @DX_DOCDIR@/latex; \
#   	  rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
#   	  $(DX_LATEX) refman.tex; \
#   	  $(MAKEINDEX_PATH) refman.idx; \
#   	  $(DX_LATEX) refman.tex; \
#   	  countdown=5; \
#   	  while $(DX_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
#   			    refman.log > /dev/null 2>&1 \
#   	     && test $$countdown -gt 0; do \
#   	      $(DX_LATEX) refman.tex; \
#   	      countdown=`expr $$countdown - 1`; \
#   	  done; \
#   	  $(DX_DVIPS) -o ../@PACKAGE@.ps refman.dvi
#
#     endif DX_COND_ps
#
#     ## ------------------------------ ##
#     ## Rules specific for PDF output. ##
#     ## ------------------------------ ##
#
#     if DX_COND_pdf
#
#     DX_CLEAN_PDF = @DX_DOCDIR@/@PACKAGE@.pdf
#
#     DX_PDF_GOAL = doxygen-pdf
#
#     doxygen-pdf: @DX_DOCDIR@/@PACKAGE@.pdf
#
#     @DX_DOCDIR@/@PACKAGE@.pdf: @DX_DOCDIR@/@PACKAGE@.tag
#   	  cd @DX_DOCDIR@/latex; \
#   	  rm -f *.aux *.toc *.idx *.ind *.ilg *.log *.out; \
#   	  $(DX_PDFLATEX) refman.tex; \
#   	  $(DX_MAKEINDEX) refman.idx; \
#   	  $(DX_PDFLATEX) refman.tex; \
#   	  countdown=5; \
#   	  while $(DX_EGREP) 'Rerun (LaTeX|to get cross-references right)' \
#   			    refman.log > /dev/null 2>&1 \
#   	     && test $$countdown -gt 0; do \
#   	      $(DX_PDFLATEX) refman.tex; \
#   	      countdown=`expr $$countdown - 1`; \
#   	  done; \
#   	  mv refman.pdf ../@PACKAGE@.pdf
#
#     endif DX_COND_pdf
#
#     ## ------------------------------------------------- ##
#     ## Rules specific for LaTeX (shared for PS and PDF). ##
#     ## ------------------------------------------------- ##
#
#     if DX_COND_latex
#
#     DX_CLEAN_LATEX = @DX_DOCDIR@/latex
#
#     endif DX_COND_latex
#
#     .PHONY: doxygen-run doxygen-doc $(DX_PS_GOAL) $(DX_PDF_GOAL)
#
#     .INTERMEDIATE: doxygen-run $(DX_PS_GOAL) $(DX_PDF_GOAL)
#
#     doxygen-run: @DX_DOCDIR@/@PACKAGE@.tag
#
#     doxygen-doc: doxygen-run $(DX_PS_GOAL) $(DX_PDF_GOAL)
#
#     @DX_DOCDIR@/@PACKAGE@.tag: $(DX_CONFIG) $(pkginclude_HEADERS)
#   	  rm -rf @DX_DOCDIR@
#   	  $(DX_ENV) $(DX_DOXYGEN) $(srcdir)/$(DX_CONFIG)
#
#     DX_CLEANFILES = \
#         @DX_DOCDIR@/@PACKAGE@.tag \
#         -r \
#         $(DX_CLEAN_HTML) \
#         $(DX_CLEAN_CHM) \
#         $(DX_CLEAN_CHI) \
#         $(DX_CLEAN_MAN) \
#         $(DX_CLEAN_RTF) \
#         $(DX_CLEAN_XML) \
#         $(DX_CLEAN_PS) \
#         $(DX_CLEAN_PDF) \
#         $(DX_CLEAN_LATEX)
#
#     endif DX_COND_doc
#
#     ----- end aminclude.am ---------------------------------------
#
# LAST MODIFICATION
#
#   2009-04-19
#
# COPYLEFT
#
#   Copyright (c) 2009 Oren Ben-Kiki <oren@ben-kiki.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved.

## ----------##
## Defaults. ##
## ----------##

DX_ENV=""
AC_DEFUN([DX_FEATURE_doc],  ON)
AC_DEFUN([DX_FEATURE_dot],  ON)
AC_DEFUN([DX_FEATURE_man],  OFF)
AC_DEFUN([DX_FEATURE_html], ON)
AC_DEFUN([DX_FEATURE_chm],  OFF)
AC_DEFUN([DX_FEATURE_chi],  OFF)
AC_DEFUN([DX_FEATURE_rtf],  OFF)
AC_DEFUN([DX_FEATURE_xml],  OFF)
AC_DEFUN([DX_FEATURE_pdf],  ON)
AC_DEFUN([DX_FEATURE_ps],   ON)

## --------------- ##
## Private macros. ##
## --------------- ##

# DX_ENV_APPEND(VARIABLE, VALUE)
# ------------------------------
# Append VARIABLE="VALUE" to DX_ENV for invoking doxygen.
AC_DEFUN([DX_ENV_APPEND], [AC_SUBST([DX_ENV], ["$DX_ENV $1='$2'"])])

# DX_DIRNAME_EXPR
# ---------------
# Expand into a shell expression prints the directory part of a path.
AC_DEFUN([DX_DIRNAME_EXPR],
         [[expr ".$1" : '\(\.\)[^/]*$' \| "x$1" : 'x\(.*\)/[^/]*$']])

# DX_IF_FEATURE(FEATURE, IF-ON, IF-OFF)
# -------------------------------------
# Expands according to the M4 (static) status of the feature.
AC_DEFUN([DX_IF_FEATURE], [ifelse(DX_FEATURE_$1, ON, [$2], [$3])])

# DX_REQUIRE_PROG(VARIABLE, PROGRAM)
# ----------------------------------
# Require the specified program to be found for the DX_CURRENT_FEATURE to work.
AC_DEFUN([DX_REQUIRE_PROG], [
AC_PATH_TOOL([$1], [$2])
if test "$DX_FLAG_[]DX_CURRENT_FEATURE$$1" = 1; then
    AC_MSG_WARN([$2 not found - will not DX_CURRENT_DESCRIPTION])
    AC_SUBST(DX_FLAG_[]DX_CURRENT_FEATURE, 0)
fi
])

# DX_TEST_FEATURE(FEATURE)
# ------------------------
# Expand to a shell expression testing whether the feature is active.
AC_DEFUN([DX_TEST_FEATURE], [test "$DX_FLAG_$1" = 1])

# DX_CHECK_DEPEND(REQUIRED_FEATURE, REQUIRED_STATE)
# -------------------------------------------------
# Verify that a required features has the right state before trying to turn on
# the DX_CURRENT_FEATURE.
AC_DEFUN([DX_CHECK_DEPEND], [
test "$DX_FLAG_$1" = "$2" \
|| AC_MSG_ERROR([doxygen-DX_CURRENT_FEATURE ifelse([$2], 1,
                            requires, contradicts) doxygen-DX_CURRENT_FEATURE])
])

# DX_CLEAR_DEPEND(FEATURE, REQUIRED_FEATURE, REQUIRED_STATE)
# ----------------------------------------------------------
# Turn off the DX_CURRENT_FEATURE if the required feature is off.
AC_DEFUN([DX_CLEAR_DEPEND], [
test "$DX_FLAG_$1" = "$2" || AC_SUBST(DX_FLAG_[]DX_CURRENT_FEATURE, 0)
])

# DX_FEATURE_ARG(FEATURE, DESCRIPTION,
#                CHECK_DEPEND, CLEAR_DEPEND,
#                REQUIRE, DO-IF-ON, DO-IF-OFF)
# --------------------------------------------
# Parse the command-line option controlling a feature. CHECK_DEPEND is called
# if the user explicitly turns the feature on (and invokes DX_CHECK_DEPEND),
# otherwise CLEAR_DEPEND is called to turn off the default state if a required
# feature is disabled (using DX_CLEAR_DEPEND). REQUIRE performs additional
# requirement tests (DX_REQUIRE_PROG). Finally, an automake flag is set and
# DO-IF-ON or DO-IF-OFF are called according to the final state of the feature.
AC_DEFUN([DX_ARG_ABLE], [
    AC_DEFUN([DX_CURRENT_FEATURE], [$1])
    AC_DEFUN([DX_CURRENT_DESCRIPTION], [$2])
    AC_ARG_ENABLE(doxygen-$1,
                  [AS_HELP_STRING(DX_IF_FEATURE([$1], [--disable-doxygen-$1],
                                                      [--enable-doxygen-$1]),
                                  DX_IF_FEATURE([$1], [don't $2], [$2]))],
                  [
case "$enableval" in
#(
y|Y|yes|Yes|YES)
    AC_SUBST([DX_FLAG_$1], 1)
    $3
;; #(
n|N|no|No|NO)
    AC_SUBST([DX_FLAG_$1], 0)
;; #(
*)
    AC_MSG_ERROR([invalid value '$enableval' given to doxygen-$1])
;;
esac
], [
AC_SUBST([DX_FLAG_$1], [DX_IF_FEATURE([$1], 1, 0)])
$4
])
if DX_TEST_FEATURE([$1]); then
    $5
    :
fi
if DX_TEST_FEATURE([$1]); then
    AM_CONDITIONAL(DX_COND_$1, :)
    $6
    :
else
    AM_CONDITIONAL(DX_COND_$1, false)
    $7
    :
fi
])

## -------------- ##
## Public macros. ##
## -------------- ##

# DX_XXX_FEATURE(DEFAULT_STATE)
# -----------------------------
AC_DEFUN([DX_DOXYGEN_FEATURE], [AC_DEFUN([DX_FEATURE_doc],  [$1])])
AC_DEFUN([DX_MAN_FEATURE],     [AC_DEFUN([DX_FEATURE_man],  [$1])])
AC_DEFUN([DX_HTML_FEATURE],    [AC_DEFUN([DX_FEATURE_html], [$1])])
AC_DEFUN([DX_CHM_FEATURE],     [AC_DEFUN([DX_FEATURE_chm],  [$1])])
AC_DEFUN([DX_CHI_FEATURE],     [AC_DEFUN([DX_FEATURE_chi],  [$1])])
AC_DEFUN([DX_RTF_FEATURE],     [AC_DEFUN([DX_FEATURE_rtf],  [$1])])
AC_DEFUN([DX_XML_FEATURE],     [AC_DEFUN([DX_FEATURE_xml],  [$1])])
AC_DEFUN([DX_XML_FEATURE],     [AC_DEFUN([DX_FEATURE_xml],  [$1])])
AC_DEFUN([DX_PDF_FEATURE],     [AC_DEFUN([DX_FEATURE_pdf],  [$1])])
AC_DEFUN([DX_PS_FEATURE],      [AC_DEFUN([DX_FEATURE_ps],   [$1])])

# DX_INIT_DOXYGEN(PROJECT, [CONFIG-FILE], [OUTPUT-DOC-DIR])
# ---------------------------------------------------------
# PROJECT also serves as the base name for the documentation files.
# The default CONFIG-FILE is "Doxyfile" and OUTPUT-DOC-DIR is "doxygen-doc".
AC_DEFUN([DX_INIT_DOXYGEN], [

# Files:
AC_SUBST([DX_PROJECT], [$1])
AC_SUBST([DX_CONFIG], [ifelse([$2], [], Doxyfile, [$2])])
AC_SUBST([DX_DOCDIR], [ifelse([$3], [], doxygen-doc, [$3])])

# Environment variables used inside doxygen.cfg:
DX_ENV_APPEND(SRCDIR, $srcdir)
DX_ENV_APPEND(PROJECT, $DX_PROJECT)
DX_ENV_APPEND(DOCDIR, $DX_DOCDIR)
DX_ENV_APPEND(VERSION, $PACKAGE_VERSION)

# Doxygen itself:
DX_ARG_ABLE(doc, [generate any doxygen documentation],
            [],
            [],
            [DX_REQUIRE_PROG([DX_DOXYGEN], doxygen)
             DX_REQUIRE_PROG([DX_PERL], perl)],
            [DX_ENV_APPEND(PERL_PATH, $DX_PERL)])

# Dot for graphics:
DX_ARG_ABLE(dot, [generate graphics for doxygen documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_DOT], dot)],
            [DX_ENV_APPEND(HAVE_DOT, YES)
             DX_ENV_APPEND(DOT_PATH, [`DX_DIRNAME_EXPR($DX_DOT)`])],
            [DX_ENV_APPEND(HAVE_DOT, NO)])

# Man pages generation:
DX_ARG_ABLE(man, [generate doxygen manual pages],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_MAN, YES)],
            [DX_ENV_APPEND(GENERATE_MAN, NO)])

# RTF file generation:
DX_ARG_ABLE(rtf, [generate doxygen RTF documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_RTF, YES)],
            [DX_ENV_APPEND(GENERATE_RTF, NO)])

# XML file generation:
DX_ARG_ABLE(xml, [generate doxygen XML documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_XML, YES)],
            [DX_ENV_APPEND(GENERATE_XML, NO)])

# (Compressed) HTML help generation:
DX_ARG_ABLE(chm, [generate doxygen compressed HTML help documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_HHC], hhc)],
            [DX_ENV_APPEND(HHC_PATH, $DX_HHC)
             DX_ENV_APPEND(GENERATE_HTML, YES)
             DX_ENV_APPEND(GENERATE_HTMLHELP, YES)],
            [DX_ENV_APPEND(GENERATE_HTMLHELP, NO)])

# Seperate CHI file generation.
DX_ARG_ABLE(chi, [generate doxygen seperate compressed HTML help index file],
            [DX_CHECK_DEPEND(chm, 1)],
            [DX_CLEAR_DEPEND(chm, 1)],
            [],
            [DX_ENV_APPEND(GENERATE_CHI, YES)],
            [DX_ENV_APPEND(GENERATE_CHI, NO)])

# Plain HTML pages generation:
DX_ARG_ABLE(html, [generate doxygen plain HTML documentation],
            [DX_CHECK_DEPEND(doc, 1) DX_CHECK_DEPEND(chm, 0)],
            [DX_CLEAR_DEPEND(doc, 1) DX_CLEAR_DEPEND(chm, 0)],
            [],
            [DX_ENV_APPEND(GENERATE_HTML, YES)],
            [DX_TEST_FEATURE(chm) || DX_ENV_APPEND(GENERATE_HTML, NO)])

# PostScript file generation:
DX_ARG_ABLE(ps, [generate doxygen PostScript documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_LATEX], latex)
             DX_REQUIRE_PROG([DX_MAKEINDEX], makeindex)
             DX_REQUIRE_PROG([DX_DVIPS], dvips)
             DX_REQUIRE_PROG([DX_EGREP], egrep)])

# PDF file generation:
DX_ARG_ABLE(pdf, [generate doxygen PDF documentation],
            [DX_CHECK_DEPEND(doc, 1)],
            [DX_CLEAR_DEPEND(doc, 1)],
            [DX_REQUIRE_PROG([DX_PDFLATEX], pdflatex)
             DX_REQUIRE_PROG([DX_MAKEINDEX], makeindex)
             DX_REQUIRE_PROG([DX_EGREP], egrep)])

# LaTeX generation for PS and/or PDF:
if DX_TEST_FEATURE(ps) || DX_TEST_FEATURE(pdf); then
    AM_CONDITIONAL(DX_COND_latex, :)
    DX_ENV_APPEND(GENERATE_LATEX, YES)
else
    AM_CONDITIONAL(DX_COND_latex, false)
    DX_ENV_APPEND(GENERATE_LATEX, NO)
fi

# Paper size for PS and/or PDF:
AC_ARG_VAR(DOXYGEN_PAPER_SIZE,
           [a4wide (default), a4, letter, legal or executive])
case "$DOXYGEN_PAPER_SIZE" in
#(
"")
    AC_SUBST(DOXYGEN_PAPER_SIZE, "")
;; #(
a4wide|a4|letter|legal|executive)
    DX_ENV_APPEND(PAPER_SIZE, $DOXYGEN_PAPER_SIZE)
;; #(
*)
    AC_MSG_ERROR([unknown DOXYGEN_PAPER_SIZE='$DOXYGEN_PAPER_SIZE'])
;;
esac

#For debugging:
#echo DX_FLAG_doc=$DX_FLAG_doc
#echo DX_FLAG_dot=$DX_FLAG_dot
#echo DX_FLAG_man=$DX_FLAG_man
#echo DX_FLAG_html=$DX_FLAG_html
#echo DX_FLAG_chm=$DX_FLAG_chm
#echo DX_FLAG_chi=$DX_FLAG_chi
#echo DX_FLAG_rtf=$DX_FLAG_rtf
#echo DX_FLAG_xml=$DX_FLAG_xml
#echo DX_FLAG_pdf=$DX_FLAG_pdf
#echo DX_FLAG_ps=$DX_FLAG_ps
#echo DX_ENV=$DX_ENV
])


##### http://autoconf-archive.cryp.to/acx_mpi.html
#
# SYNOPSIS
#
#   ACX_MPI([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   This macro tries to find out how to compile programs that use MPI
#   (Message Passing Interface), a standard API for parallel process
#   communication (see http://www-unix.mcs.anl.gov/mpi/)
#
#   On success, it sets the MPICC, MPICXX, MPIF77, or MPIFC output
#   variable to the name of the MPI compiler, depending upon the
#   current language. (This may just be $CC/$CXX/$F77/$FC, but is more
#   often something like mpicc/mpiCC/mpif77/mpif90.) It also sets
#   MPILIBS to any libraries that are needed for linking MPI (e.g.
#   -lmpi or -lfmpi, if a special MPICC/MPICXX/MPIF77/MPIFC was not
#   found).
#
#   If you want to compile everything with MPI, you should set:
#
#       CC="MPICC" #OR# CXX="MPICXX" #OR# F77="MPIF77" #OR# FC="MPIFC"
#       LIBS="$MPILIBS $LIBS"
#
#   NOTE: The above assumes that you will use $CC (or whatever) for
#   linking as well as for compiling. (This is the default for automake
#   and most Makefiles.)
#
#   The user can force a particular library/compiler by setting the
#   MPICC/MPICXX/MPIF77/MPIFC and/or MPILIBS environment variables.
#
#   ACTION-IF-FOUND is a list of shell commands to run if an MPI
#   library is found, and ACTION-IF-NOT-FOUND is a list of commands to
#   run if it is not found. If ACTION-IF-FOUND is not specified, the
#   default action will define HAVE_MPI.
#
# LAST MODIFICATION
#
#   2006-10-22
#
# COPYLEFT
#
#   Copyright (c) 2006 Steven G. Johnson <stevenj@alum.mit.edu>
#   Copyright (c) 2006 Julian C. Cummings <cummings@cacr.caltech.edu>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
#   02111-1307, USA.
#
#   As a special exception, the respective Autoconf Macro's copyright
#   owner gives unlimited permission to copy, distribute and modify the
#   configure scripts that are the output of Autoconf when processing
#   the Macro. You need not follow the terms of the GNU General Public
#   License when using or distributing such scripts, even though
#   portions of the text of the Macro appear in them. The GNU General
#   Public License (GPL) does govern all other use of the material that
#   constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the
#   Autoconf Macro released by the Autoconf Macro Archive. When you
#   make and distribute a modified version of the Autoconf Macro, you
#   may extend this special exception to the GPL to apply to your
#   modified version as well.

AC_DEFUN([ACX_MPI], [
AC_PREREQ(2.50) dnl for AC_LANG_CASE

AC_LANG_CASE([C], [
	AC_REQUIRE([AC_PROG_CC])
	AC_ARG_VAR(MPICC,[MPI C compiler command])
	AC_CHECK_PROGS(MPICC, mpicc hcc mpxlc_r mpxlc mpcc cmpicc, $CC)
	acx_mpi_save_CC="$CC"
	CC="$MPICC"
	AC_SUBST(MPICC)
],
[C++], [
	AC_REQUIRE([AC_PROG_CXX])
	AC_ARG_VAR(MPICXX,[MPI C++ compiler command])
	AC_CHECK_PROGS(MPICXX, mpic++ mpicxx mpiCC hcp mpxlC_r mpxlC mpCC cmpic++, $CXX)
	acx_mpi_save_CXX="$CXX"
	CXX="$MPICXX"
	AC_SUBST(MPICXX)
],
[Fortran 77], [
	AC_REQUIRE([AC_PROG_F77])
	AC_ARG_VAR(MPIF77,[MPI Fortran 77 compiler command])
	AC_CHECK_PROGS(MPIF77, mpif77 hf77 mpxlf_r mpxlf mpf77 cmpifc, $F77)
	acx_mpi_save_F77="$F77"
	F77="$MPIF77"
	AC_SUBST(MPIF77)
],
[Fortran], [
	AC_REQUIRE([AC_PROG_FC])
	AC_ARG_VAR(MPIFC,[MPI Fortran compiler command])
	AC_CHECK_PROGS(MPIFC, mpif90 mpxlf95_r mpxlf90_r mpxlf95 mpxlf90 mpf90 cmpif90c, $FC)
	acx_mpi_save_FC="$FC"
	FC="$MPIFC"
	AC_SUBST(MPIFC)
])

if test x = x"$MPILIBS"; then
	AC_LANG_CASE([C], [AC_CHECK_FUNC(MPI_Init, [MPILIBS=" "])],
		[C++], [AC_CHECK_FUNC(MPI_Init, [MPILIBS=" "])],
		[Fortran 77], [AC_MSG_CHECKING([for MPI_Init])
			AC_LINK_IFELSE([AC_LANG_PROGRAM([],[      call MPI_Init])],[MPILIBS=" "
				AC_MSG_RESULT(yes)], [AC_MSG_RESULT(no)])],
		[Fortran], [AC_MSG_CHECKING([for MPI_Init])
			AC_LINK_IFELSE([AC_LANG_PROGRAM([],[      call MPI_Init])],[MPILIBS=" "
				AC_MSG_RESULT(yes)], [AC_MSG_RESULT(no)])])
fi
AC_LANG_CASE([Fortran 77], [
	if test x = x"$MPILIBS"; then
		AC_CHECK_LIB(fmpi, MPI_Init, [MPILIBS="-lfmpi"])
	fi
	if test x = x"$MPILIBS"; then
		AC_CHECK_LIB(fmpich, MPI_Init, [MPILIBS="-lfmpich"])
	fi
],
[Fortran], [
	if test x = x"$MPILIBS"; then
		AC_CHECK_LIB(fmpi, MPI_Init, [MPILIBS="-lfmpi"])
	fi
	if test x = x"$MPILIBS"; then
		AC_CHECK_LIB(mpichf90, MPI_Init, [MPILIBS="-lmpichf90"])
	fi
])
if test x = x"$MPILIBS"; then
	AC_CHECK_LIB(mpi, MPI_Init, [MPILIBS="-lmpi"])
fi
if test x = x"$MPILIBS"; then
	AC_CHECK_LIB(mpich, MPI_Init, [MPILIBS="-lmpich"])
fi

dnl We have to use AC_TRY_COMPILE and not AC_CHECK_HEADER because the
dnl latter uses $CPP, not $CC (which may be mpicc).
AC_LANG_CASE([C], [if test x != x"$MPILIBS"; then
	AC_MSG_CHECKING([for mpi.h])
	AC_TRY_COMPILE([#include <mpi.h>],[],[AC_MSG_RESULT(yes)], [MPILIBS=""
		AC_MSG_RESULT(no)])
fi],
[C++], [if test x != x"$MPILIBS"; then
	AC_MSG_CHECKING([for mpi.h])
	AC_TRY_COMPILE([#include <mpi.h>],[],[AC_MSG_RESULT(yes)], [MPILIBS=""
		AC_MSG_RESULT(no)])
fi],
[Fortran 77], [if test x != x"$MPILIBS"; then
	AC_MSG_CHECKING([for mpif.h])
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[      include 'mpif.h'])],[AC_MSG_RESULT(yes)], [MPILIBS=""
		AC_MSG_RESULT(no)])
fi],
[Fortran], [if test x != x"$MPILIBS"; then
	AC_MSG_CHECKING([for mpif.h])
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([],[      include 'mpif.h'])],[AC_MSG_RESULT(yes)], [MPILIBS=""
		AC_MSG_RESULT(no)])
fi])

AC_LANG_CASE([C], [CC="$acx_mpi_save_CC"],
	[C++], [CXX="$acx_mpi_save_CXX"],
	[Fortran 77], [F77="$acx_mpi_save_F77"],
	[Fortran], [FC="$acx_mpi_save_FC"])

AC_SUBST(MPILIBS)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x = x"$MPILIBS"; then
        $2
        :
else
        ifelse([$1],,[AC_DEFINE(HAVE_MPI,1,[Define if you have the MPI library.])],[$1])
        :
fi
])dnl ACX_MPI


# ===========================================================================
#           http://autoconf-archive.cryp.to/ax_prefix_config_h.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PREFIX_CONFIG_H [(OUTPUT-HEADER [,PREFIX [,ORIG-HEADER]])]
#
# DESCRIPTION
#
#   This is a new variant from ac_prefix_config_ this one will use a
#   lowercase-prefix if the config-define was starting with a
#   lowercase-char, e.g. "#define const", "#define restrict", or "#define
#   off_t", (and this one can live in another directory, e.g.
#   testpkg/config.h therefore I decided to move the output-header to be the
#   first arg)
#
#   takes the usual config.h generated header file; looks for each of the
#   generated "#define SOMEDEF" lines, and prefixes the defined name (ie.
#   makes it "#define PREFIX_SOMEDEF". The result is written to the output
#   config.header file. The PREFIX is converted to uppercase for the
#   conversions.
#
#   Defaults:
#
#     OUTPUT-HEADER = $PACKAGE-config.h
#     PREFIX = $PACKAGE
#     ORIG-HEADER, from AM_CONFIG_HEADER(config.h)
#
#   Your configure.ac script should contain both macros in this order, and
#   unlike the earlier variations of this prefix-macro it is okay to place
#   the AX_PREFIX_CONFIG_H call before the AC_OUTPUT invokation.
#
#   Example:
#
#     AC_INIT(config.h.in)        # config.h.in as created by "autoheader"
#     AM_INIT_AUTOMAKE(testpkg, 0.1.1)    # makes #undef VERSION and PACKAGE
#     AM_CONFIG_HEADER(config.h)          # prep config.h from config.h.in
#     AX_PREFIX_CONFIG_H(mylib/_config.h) # prep mylib/_config.h from it..
#     AC_MEMORY_H                         # makes "#undef NEED_MEMORY_H"
#     AC_C_CONST_H                        # makes "#undef const"
#     AC_OUTPUT(Makefile)                 # creates the "config.h" now
#                                         # and also mylib/_config.h
#
#   if the argument to AX_PREFIX_CONFIG_H would have been omitted then the
#   default outputfile would have been called simply "testpkg-config.h", but
#   even under the name "mylib/_config.h" it contains prefix-defines like
#
#     #ifndef TESTPKG_VERSION
#     #define TESTPKG_VERSION "0.1.1"
#     #endif
#     #ifndef TESTPKG_NEED_MEMORY_H
#     #define TESTPKG_NEED_MEMORY_H 1
#     #endif
#     #ifndef _testpkg_const
#     #define _testpkg_const _const
#     #endif
#
#   and this "mylib/_config.h" can be installed along with other
#   header-files, which is most convenient when creating a shared library
#   (that has some headers) where some functionality is dependent on the
#   OS-features detected at compile-time. No need to invent some
#   "mylib-confdefs.h.in" manually. :-)
#
#   Note that some AC_DEFINEs that end up in the config.h file are actually
#   self-referential - e.g. AC_C_INLINE, AC_C_CONST, and the AC_TYPE_OFF_T
#   say that they "will define inline|const|off_t if the system does not do
#   it by itself". You might want to clean up about these - consider an
#   extra mylib/conf.h that reads something like:
#
#      #include <mylib/_config.h>
#      #ifndef _testpkg_const
#      #define _testpkg_const const
#      #endif
#
#   and then start using _testpkg_const in the header files. That is also a
#   good thing to differentiate whether some library-user has starting to
#   take up with a different compiler, so perhaps it could read something
#   like this:
#
#     #ifdef _MSC_VER
#     #include <mylib/_msvc.h>
#     #else
#     #include <mylib/_config.h>
#     #endif
#     #ifndef _testpkg_const
#     #define _testpkg_const const
#     #endif
#
# LAST MODIFICATION
#
#   2008-04-12
#
# COPYLEFT
#
#   Copyright (c) 2008 Guido U. Draheim <guidod@gmx.de>
#   Copyright (c) 2008 Marten Svantesson
#   Copyright (c) 2008 Gerald Point <Gerald.Point@labri.fr>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Macro Archive. When you make and
#   distribute a modified version of the Autoconf Macro, you may extend this
#   special exception to the GPL to apply to your modified version as well.

AC_DEFUN([AX_PREFIX_CONFIG_H],[dnl
AC_BEFORE([AC_CONFIG_HEADERS],[$0])dnl
AC_CONFIG_COMMANDS([ifelse($1,,$PACKAGE-config.h,$1)],[dnl
AS_VAR_PUSHDEF([_OUT],[ac_prefix_conf_OUT])dnl
AS_VAR_PUSHDEF([_DEF],[ac_prefix_conf_DEF])dnl
AS_VAR_PUSHDEF([_PKG],[ac_prefix_conf_PKG])dnl
AS_VAR_PUSHDEF([_LOW],[ac_prefix_conf_LOW])dnl
AS_VAR_PUSHDEF([_UPP],[ac_prefix_conf_UPP])dnl
AS_VAR_PUSHDEF([_INP],[ac_prefix_conf_INP])dnl
m4_pushdef([_script],[conftest.prefix])dnl
m4_pushdef([_symbol],[m4_cr_Letters[]m4_cr_digits[]_])dnl
_OUT=`echo ifelse($1, , $PACKAGE-config.h, $1)`
_DEF=`echo _$_OUT | sed -e "y:m4_cr_letters:m4_cr_LETTERS[]:" -e "s/@<:@^m4_cr_Letters@:>@/_/g"`
_PKG=`echo ifelse($2, , $PACKAGE, $2)`
_LOW=`echo _$_PKG | sed -e "y:m4_cr_LETTERS-:m4_cr_letters[]_:"`
_UPP=`echo $_PKG | sed -e "y:m4_cr_letters-:m4_cr_LETTERS[]_:"  -e "/^@<:@m4_cr_digits@:>@/s/^/_/"`
_INP=`echo "ifelse($3,,,$3)" | sed -e 's/ *//'`
if test ".$_INP" = "."; then
   for ac_file in : $CONFIG_HEADERS; do test "_$ac_file" = _: && continue
     case "$ac_file" in
        *.h) _INP=$ac_file ;;
        *)
     esac
     test ".$_INP" != "." && break
   done
fi
if test ".$_INP" = "."; then
   case "$_OUT" in
      */*) _INP=`basename "$_OUT"`
      ;;
      *-*) _INP=`echo "$_OUT" | sed -e "s/@<:@_symbol@:>@*-//"`
      ;;
      *) _INP=config.h
      ;;
   esac
fi
if test -z "$_PKG" ; then
   AC_MSG_ERROR([no prefix for _PREFIX_PKG_CONFIG_H])
else
  if test ! -f "$_INP" ; then if test -f "$srcdir/$_INP" ; then
     _INP="$srcdir/$_INP"
  fi fi
  AC_MSG_NOTICE(creating $_OUT - prefix $_UPP for $_INP defines)
  if test -f $_INP ; then
    echo "s/^@%:@undef  *\\(@<:@m4_cr_LETTERS[]_@:>@\\)/@%:@undef $_UPP""_\\1/" > _script
    echo "s/^@%:@undef  *\\(@<:@m4_cr_letters@:>@\\)/@%:@undef $_LOW""_\\1/" >> _script
    echo "s/^@%:@def[]ine  *\\(@<:@m4_cr_LETTERS[]_@:>@@<:@_symbol@:>@*\\)\\(.*\\)/@%:@ifndef $_UPP""_\\1 \\" >> _script
    echo "@%:@def[]ine $_UPP""_\\1 \\2 \\" >> _script
    echo "@%:@endif/" >>_script
    echo "s/^@%:@def[]ine  *\\(@<:@m4_cr_letters@:>@@<:@_symbol@:>@*\\)\\(.*\\)/@%:@ifndef $_LOW""_\\1 \\" >> _script
    echo "@%:@define $_LOW""_\\1 \\2 \\" >> _script
    echo "@%:@endif/" >> _script
    # now executing _script on _DEF input to create _OUT output file
    echo "@%:@ifndef $_DEF"      >$tmp/pconfig.h
    echo "@%:@def[]ine $_DEF 1" >>$tmp/pconfig.h
    echo ' ' >>$tmp/pconfig.h
    echo /'*' $_OUT. Generated automatically at end of configure. '*'/ >>$tmp/pconfig.h

    sed -f _script $_INP >>$tmp/pconfig.h
    echo ' ' >>$tmp/pconfig.h
    echo '/* once:' $_DEF '*/' >>$tmp/pconfig.h
    echo "@%:@endif" >>$tmp/pconfig.h
    if cmp -s $_OUT $tmp/pconfig.h 2>/dev/null; then
      AC_MSG_NOTICE([$_OUT is unchanged])
    else
      ac_dir=`AS_DIRNAME(["$_OUT"])`
      AS_MKDIR_P(["$ac_dir"])
      rm -f "$_OUT"
      mv $tmp/pconfig.h "$_OUT"
    fi
    cp _script _configs.sed
  else
    AC_MSG_ERROR([input file $_INP does not exist - skip generating $_OUT])
  fi
  rm -f conftest.*
fi
m4_popdef([_symbol])dnl
m4_popdef([_script])dnl
AS_VAR_POPDEF([_INP])dnl
AS_VAR_POPDEF([_UPP])dnl
AS_VAR_POPDEF([_LOW])dnl
AS_VAR_POPDEF([_PKG])dnl
AS_VAR_POPDEF([_DEF])dnl
AS_VAR_POPDEF([_OUT])dnl
],[PACKAGE="$PACKAGE"])])

dnl implementation note: a bug report (31.5.2005) from Marten Svantesson points
dnl out a problem where `echo "\1"` results in a Control-A. The unix standard
dnl    http://www.opengroup.org/onlinepubs/000095399/utilities/echo.html
dnl defines all backslash-sequences to be inherently non-portable asking
dnl for replacement mit printf. Some old systems had problems with that
dnl one either. However, the latest libtool (!) release does export an $ECHO
dnl (and $echo) that does the right thing - just one question is left: what
dnl was the first version to have it? Is it greater 2.58 ?


