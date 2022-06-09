
/*
 * BOB++
 */

using namespace std;
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<unistd.h>

/** \mainpage Bobpp
    @author Bertrand Le Cun, Fran&ccedil;ois Galea, Pascal Vander-Swalmen, Tarek Menouer
    @version $(VERSION)

\section MainIntro Introduction

The goal of the Bobpp (aka Bobpp, Bobo, prononciate bob plus plus) framework Library is to allow easy
implementation of solver on specific problems using sequential and parallel search algorithms : 
such as Branch and Bound, Branch and Price/Cut, Divide and Conquer.

The official web page of the project is https://forge.prism.uvsq.fr/projects/bobpp
The lastest version of this documentation could be reached at http://www.prism.uvsq.fr/~blec/bobpp/

\section LibComp Ideas on the conception of the library

The idea is to propose a set of classes for each search algorithm that implement a skeleton of algorithm.
The developper may use the classes proposed by by the framework but may also reimplement its own classes 
to modify the behaviour of the resulting solver.

Each of the proposed algorithm performs a search in a dynamicaly constructed space.
The different classes are :
 - \em Nodes are elements of the <em>space search</em>. 
   The basic class is the Bob::base_Node
 - One of the nodes is known as the \em root node.
 - A \em method must be given to generate nodes from another one. This method
   is the functor of the GenChild class (see #Bob::base_GenChild).
 - The Instance is a class that store all the global data. An object of this 
   class could be initialized, but the data are read only during the search. the basic class is #Bob::base_Instance
 - The search could store some nodes in the "Goal".
 - The search is performed by an "Algo" (see #Bob::base_Algo).

This very simple scheme includes algorithms such as \ref DCAlgoGRP,
\ref BBAlgoGRP and its derivate \ref  LPBBAlgoGRP, A*, Dynamic programming, Tabu search, etc...

All of them differ in :
- The \em structure of the space search (tree, graph),
- The <em>scheduling strategy</em> to explore the space search,
- various information available to control the search (evaluation, costs, configuration).

The Bobpp library is based on these basic ideas. The programming of a user
algorithm can be performed by specializing one of the proposed algorithms.

An algo is executed in a specific "Programming environment", (see Bob::BaseAlgoEnvProg or more generaly @ref EnvProgGRP ) that could be sequential or 
parallel using Posix Thread, MPI or kaapi,...

\image html archi_bobpp.png
\image latex archi_bobpp.eps "Overview of Bobpp architecture" width=10cm

\section MainFeature Features

- Object Oriented, The interface is manly based on C++ template. 
- Several Search Algorithms \ref UserAlgoGRP :  @ref BBAlgoGRP with Branch and Price and Branch and Cut, @ref CDCAlgoGRP,  @ref SDCAlgoGRP
- Transparent parallelizations (see @ref EnvProgGRP) the available parallelization are based on Posix Thread (@ref ThrEnvProgGRP), on MPI (@ref MPEnvProgGRP) or Kaapi (@ref AthaEnvProgGRP).
- Automatic Options and Parameters Handling ( see @ref OptGRP Module).
- Counters and timers to get some performances measure (see @ref STATGRP module)
- Save/restore feature, it is possible to stop the search and save it to a file, to resume it after.
- Logging feature : the search log the statistices, in term of activities. Each search algorithm has 
its own set of statistices. but a search could also be logged with the relation of a child node with its parent.
Then we are able to draw the space after the end of the search

\section LibArchi Modules of the library

The Bobpp library is composed of three main parts.
-# The \em core (@ref CoreGRP), includes various features
   - the options (@ref OptGRP),
   - the core static class : Bob::core
   - the data structures that stores objects like nodes and solutions (@ref GDS_GRP and more precisely @ref PQGRP) .
-# The hierarchy of the different possible algorithms (@ref UserAlgoGRP), including 
    - @ref BBAlgoGRP, 
    - @ref CDCAlgoGRP 
    - @ref SDCAlgoGRP.
-# The programming environment (@ref EnvProgGRP), which handles the execution
of the algorithm on different distributed, multithreaded or sequential
environments.

The Bobpp disributions comes with different examples the description of these
example are given in the \ref Examples page.

\section Howto HowTo

To use the library first see the \ref InstMan "Installation page". This
page indicates the procedure to produce the different parts of the library.

\section Thanks Thanks,

This libray has been mainly developped by <a href="http://www.prism.uvsq.fr/~blec">Bertrand Le Cun</a>.
Several persons have contributed to the library.
  - Francois Galea has perform a great work. 
      - the tool to vizualize the file log (bobview), 
      - the first MIP application.
      - he is the main contributor of the Q3AP solver the solver of 3 dimensional Quadratic Assignment Problem (This solver is not included in the distribution).
      - First implementation of the MPX programming environment.
      - He has also contribute to these documentation pages.
  - Pascal Vander-Swalmen
      - Pascal has rewritten the MPX enviroment.
      - He is contributing now by porting the ILP level on top of the 
        <a href="https://projects.coin-or.org/Osi/">OSI</a> 
        (One ofthe project of <a href="http://www.coin-or.org/">COIN-OR</a> group).
  - Tarek Menouer
      - Tarek is porting the Constraint Solver Or-Tools on top of Bobpp.
      - He also contribute to test the different parallelizations : thread, MPX, etc...
  - Christophe Louat has mainly contribute to the cut generation library
    proposed by the Glop library. But he has used and implements several 
    MIP applications. His works has been used as base work of the ILP level 
    of the Bobpp.
*/

/** \page InstMan Getting started with Bobpp

This section describes what you can install on the different supported systems,
and how.

- \subpage InstManSupported "Supported platforms"
- \subpage InstManDownload "Downloading the source files"
- \subpage InstManUnx "Configuration, compilation and installation on Un*x systems"
- \subpage InstManConfigure "Configuration options"
- \subpage InstManLocal "Installing Bobpp in a user directory"
- \subpage InstManDocu "Generating the documentation"
- \subpage InstManWin "Compilation and installation on Microsoft Windows systems"



\page InstManSupported Supported Platforms

\section supported_unix UN*X systems

Bobpp runs on many Un*x compatible systems. You may want to use a desktop PC
running a GNU system, a workstation with a proprietary system, a cluster of
workstations as well as computation grids.

Installation can be done through the now classic "configure / make / make
install" process.

\section supported_win Microsoft Windows systems

It was possible at a time to used Miscrosoft Windows systems, on a limited extent : no
support for clusters of Windows stations is available. Only the sequential and
multithreaded parallel environments can be used.

There is limited support for execution logging on Windows systems : only log
files can be generated. The \ref Logtools "log viewer and log listener" cannot
currently be compiled on Windows systems.

\page InstManDownload Downloading the sources files

You can download the Bobpp source code using git, through an anonymous
repository access. 

Bobpp is based on the GNU
<a href="http://www.gnu.org/software/autoconf/">autoconf</a>,
<a href="http://www.gnu.org/software/automake/">automake</a> and
<a href="http://www.gnu.org/software/libtool/">libtool</a> tools to simplify the
configuration of the libraries. These tools enable the software designers to
publish source file packages with a \p configure script and a set of \p
Makefile.in files, which handle all the necessary configuration before compiling
the sources and installing the compiled binary files. If you get the source
files from the subversion repository, you need to generate those \p
configure and \p Makefile.in files manually. Thus, your system must have all
these tools installed.

The command for fetching the source code is the following:

\code
      $ git clone  git://forge.prism.uvsq.fr/bobpp.git bobpp_read_only
\endcode

This generates a \p bobpp directory containing the latest revision of Bobpp.

Later on, you can update the source code to the latest revision by issuing the
following shell command:

\code
      $ git pull
\endcode

You can then generate all the necessary configure and make files can be done by
running this command :

\code
      $ ./bootstrap
\endcode

Obviously, your system needs the autoconf, automake and libtool tools to be
installed. If not, better use a tarball of the sources instead.

\page InstManConfigure Configuration options

On Un*x systems, the configuration of the Bobpp library has to be done by
providing arguments to the supplied (or autotools-generated) \c configure
script.

The currently possible options are:

<ul>
<li><tt>--prefix=\<installation dir\></tt> - select the directory where to install
(put) header files, archive, scripts, etc... required to compile and run Bobpp
programs.

If not set, Bobpp will be installed globally on your system, in the
standard <tt>/usr/local</tt> directory. Note that you will need root privileges
for such a global installation.

If set, this allows you to install the Bobpp into a user installation
directory, without the necessity for system privileges. More information about
installation in user directories can be found in \ref InstManLocal this page.
</li>
<li><tt>--with-kaapi=\<arg\></tt> - specify whether the kaapi/athapascan
programming environment needs to be generated or not. The provided argument may
one of the following:
<ul>
<li>\c check (default) - generate the Kaapi/Athapascan
module only if Kaapi is installed</li>
<li>\c no - don't generate the kaapi/athapascan module</li>
<li>\c yes - the Kaapi/Athapascan module must be generated. If Kaapi has not
been found, the configure process stops with an error message</li>
<li><tt>path/to/kaapi</tt> - generate the Kaapi/Athapascan module, assuming
the Kaapi root installation directory is <tt>path/to/kaapi</tt>.
</ul></li>

<li> \p --with-glop=\<solver\> - specify which linear solver
has to be used with the Glop library in Bobpp. At this time, \c glpk, \c clp, \c
lp_solve, \c cplex or \c xpress are the possible options. Of course the
corresponding Glop module must be installed, otherwise the installation process
will fail.

The default behaviour is to check for the presence of any of the possible glop
modules, and select one of them if found. If the selected module is not the one
you want, use the \p --with-glop option to manually choose the module you want.

Notice that this option is almost deprecated. Glop is currently replaced by the OSI interface of Coin-OR.
</li>
<li> <tt>--with-giw=\<yes|no|check\></tt> specifies whether the <a
href="http://giw.sourceforge.net/">GIW</a> library is present on the system or
not. By default, \p configure checks autoamtically for the presence of GIW.

GIW is only necessary for compiling the \ref bobview "bobview" log
viewing tool. If GIW is not available, bobview will neither be compiled nor
installed.</li>
<li> \p --enable-debug : with this option, Bobpp will display many information during the execution.
You may deactivate them using command line options of your Bobpp application.
(see @ref DEBUG "here" for more information)</li>
<li> \p --enable-ft : with this option, Bobpp will be able to save a search on file and restart the search
from the file. 
</ul>

Other non-Bobpp specific options are also available. To get a full list of
available options, issue a <tt>./configure --help</tt> command.


\page InstManUnx Configuration, Compilation and Installation on Un*x systems

The whole setup process (configuration, build and installation) follow a now
standard "configure / make / make install" process.

Bobpp relies on an external command called <a
href="http://pkg-config.freedesktop.org/wiki/"><tt>pkg-config</tt></a>. If this
program is not installed on your system, you will not be able to install Bobpp.
<tt>pkg-config</tt> is used when compiling Bobpp applications, and is also used
by some of the libraries (Glop, Kaapi, GIW) that some parts of Bobpp depend on.

\section _calling Calling the configure script

Configuring Bobpp consists in executing a \p configure script from the Bobpp
source directory, with specific options, like in the example below:

\code
      $ ./configure --prefix=/usr/local/bobpp
\endcode

The above command configures the library of the current host such that the
installation directory would be <tt>/usr/local/bobpp</tt> (which requires root
permission during the last install step).

If you use the following configure command line the installation is quite automatic !
the header files will be put in a bobpp directory in /usr/local/include, the lib in /usr/local/lib
and the binary tools will be in /usr/local/bin.

\code
      $ ./configure --prefix=/usr/local
\endcode

\see \ref InstManConfigure "List of configuration options"
\see \ref InstManLocal "Installing Bobpp into a user directory"

The result of this command should be a quite long listing of the automatic configuration
of differents tools, libraries, functions, ... and the determination of the C++ compiler.
The configure script uses the default compiler which can be redefined by defining the environment
variable CXX before running configure. For instance, if you want to specify an other C++ compiler,
you can defined CXX environment variable.


\section _make Build the binary files

Compilation of the binaries can be done through this command:
\code
      $ make
\endcode

\section _install Install the binaries and headers

If the previous steps successed, then the installation of the library is done
with the command.
\code
      $ make install
\endcode
Keep in mind you may need privileged permission for installation in a system
dicrectory such as <tt>/usr/local</tt>. Ensure you invoke the command above
having the correct permission enabled, for instance by switching users
beforehand.


\page InstManLocal Installing Bobpp into a user directory

On Un*x systems, the default behaviour for installation is to use a standard
system directory (such as <tt>/usr/local</tt>) as the installation directory.
This may be useful for a system-wide installation, but it requires the user to
have root access to the system.

An alternative way consists in installing Bobpp into a local user directory.
This is made possible using the \c --prefix option of the \c configure script.
Nowadays, most open source software programs work in the same way, which is
convenient for installing different programs in a common location. However,
additional setup of the user's account is necessary.

This section describes the necessary setup of environment variables in order to
get Bobpp working from a user directory. The variables are standard, which means
this setup will also work for various <tt>autoconf</tt>-based programs and
libraries.

\section InstManLocal_env Environment variables

The list of the different environment variables to be configured is the
following:

- \c PATH - this variable contains the list of all directories where executable
files may be found. It contains different fields separated by colon characters.
You may need to concatenate the defaut value of this variable, which generally
contains the list of system directories for executable commands and programs.

- \c C_INCLUDE_PATH - this variable contains a list of \e additional directories
where C header files may be found. Its default value is empty, which means
header files can only be found in system directories.

- \c CPLUS_INCLUDE_PATH - this varaible is similar to the \c C_INCLUDE_PATH
variable, excepted it concerns C++ header files. You generally may want to set
it up with the same value as \c C_INCLUDE_PATH.

- \c LD_LIBRARY_PATH - this variable contains a list of \e additional
directories where static and dynamic libraries may be found at \e compile time.
It is necessary when compiling programs or libraries which depend on other
libraries (for instance, some setups of Bobpp may depend on Kaapi or MPI
libraries.

- \c LD_RUN_PATH - this variable contains a list of \e additional directories
where dynamic libraries may be found at \e run time. Indeed, dynamic libraries
are not included in the generated binary files, unlike static libraries. Thus,
the system needs to find all necessary dynamic libraries before being able to
run a specific program.

- \c PKG_CONFIG_PATH - this variable contains a list of \e additional
directories where data files for \c pkg-config may be found. \c pkg-config is
widely used by libraries to simplify the compilation of programs which may
depend on various libraries, and is more and more used as standard.

\section InstManLocal_sh Configuring a sh-based user account

We describe here a straightforward method for configuring user accounts which
are based on \c sh compatible shells (\c sh, \c ksh, \c bash ...).

Suppose you have choosen a local directory where all header, library, binary or
documentation files will be installed. Any sub-directory in your home directory
should fit. In the following example we assume that the chosen directory is
<tt>$HOME/local</tt>.

You need to determine which configuration script is executed at login, or when
running a new instance of the shell (when opening a new terminal window for
instance). Standard setups of \c sh call the \c .profile script at login, while
\c ksh and \c bash run a \c .kshrc or \c .bashrc, even when not in a login
shell. For more information, refer to the documentation of the shell you are
using.

To configure all the necessary variables such that <tt>$HOME/local</tt> is used
as an installation directory, add the following lines to your configuration
script:

\code
for i in $HOME/local
do
  test -d $i || continue
  PATH=$i/bin:$PATH
  LD_LIBRARY_PATH=$i/lib:$LD_LIBRARY_PATH
  LD_RUN_PATH=$i/lib:$LD_RUN_PATH
  PKG_CONFIG_PATH=$i/lib/pkgconfig:$PKG_CONFIG_PATH
  C_INCLUDE_PATH=$i/include:$C_INCLUDE_PATH
done

# remove trailing colons in the different variables
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH%:}
export LD_RUN_PATH=${LD_RUN_PATH%:}
export C_INCLUDE_PATH=${C_INCLUDE_PATH%:}
export CPLUS_INCLUDE_PATH=${C_INCLUDE_PATH}
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH%:}
\endcode

Note that multiple installation directories can easily been set up. For this,
just add the different paths to the list in the \c for statement.


\page InstManDocu Generating the documentation

Bobpp uses
<a href="http://www.doxygen.org/">Doxygen</a> to generate its
documentation from the source code files.

On Un*x systems, you can generate the documentation with the command:
\code
      $ make doxygen-doc
\endcode

This line generates the html documentation and the pdf documentation in the
doxygen-doc directory.

The latest version of the documentation can also be browsed online through
<a href="http://bobpp.prism.uvsq.fr/">this link</a>.


\page InstManWin Compilation and installation on Microsoft Windows systems

<strong> Once upon a time, it has been possible to compile the Bobpp framework on Windows, 
but the feature is no supported any more, Maybe later</strong>

You can install a subset of the Bobpp modules on top of a Windows based
computer. This will allow you to create sequential and multithreaded Bobpp
applications. No support for distributed programming environments is provided.

Windows support is only available for the
<a href="http://www.bloodshed.net/devcpp.html">Dev-C++</a> IDE.

\section _winbuild Build Bobpp library file

Bobpp requires the
<a href="http://devpaks.org/details.php?devpak=194">pthreads-w32 DevPak</a> to
be installed in Dev-C++.

Bobpp is supplied with a \c bobpp.proj project file for Dev-C++. Just open
this project file, click the "build all" button, and the library file will be
generated.

\section _wininstall Install Bobpp in Dev-C++

After having generated the Bobpp library file, you can install it in an easy way
into Dev-C++, through the following steps:

<ol>
<li>Execute the \c build_devpack.bat batch file. This will generate a
<tt>libbobpp-$(VERSION)</tt> directory with \c lib and \c include
subdirectories, among others.</li>
<li>Create a tar archive file containing this directory, with the same name,
plus the \c .tar extension. On Windows, tar archives can be generated using the
<a href="http://www.7-zip.org/">7-zip</a> archiving program.</li>
<li>Compress this tar archive using the bzip2 method (7-zip also supports bzip2
compression). This will result in a \c libbobpp-$(VERSION).tar.bz2 compressed
archive file.</li>
<li>Rename \c libbobpp-$(VERSION).tar.bz2 into \c libbobpp-$(VERSION).DevPak.
</li>
<li>Install the libbobpp-$(VERSION).DevPak file in Dev-C++.</li>.
</ol>

*/



/** \page Examples Examples
 *
 * <ul>
 * <li> Branch and Bound
 * <ul>
 * <li> \subpage BBSimul : the goal is to generate nodes with a random evaluation from a given value to a maximum value. An invariant is that the evaluation of one node is greater thant the evaluation of its parent node.</li>
 * <li> \subpage MIP : The goal is to solve a Mixed integer program, using Branch and Bound</li>
 * </ul>
 * </li>
 * <li> Divide and Conquer
 * <ul>
 * <li>\subpage NQueens : solve the n-queens problem using a simple divide and conquer</li>
 * <li>\subpage Knapsack : solve a knapsack problem using a simple divide and conquer</li>
 * </ul>
 * </li>
 * </ul>
 *
 */

/** \page BBSimul Simulation of Branch and Bound
 * Here is the simple sequential version of the code. In this code,
 * the same problem is solve two times, to test the two Bob::EnvProg methods.
 * \include examples/SimBB/seq.cpp
 * But there also exists the Multithreaded version
 * \include examples/SimBB/thr.cpp
 * and the Athapascan version
 * \include examples/SimBB/atha.cpp
 */

/** \page MIP Mixed integer programming
 * You could find in the example/TestBB directory the mip.cpp file.
 * This example use the Bobpp Branch and Bound algorithm to solve integer
 * linear program. It uses to evaluate each node a linear solver using
 * the <a href="http://glop.prism.uvsq.fr/">Glop library</a>. At this time
 * Glop is not distributed with Bobpp.
 *
 * Another directory example/Mip also stores such an example. But in this one we
 * have added the cuts generation. We have implemented the
 * <a href="http://glock.prism.uvsq.fr/">Glock library</a> to generate cuts
 * on top of the Glop library.
 *
 * These two examples can solve a linear program in lp format
 * \include Mip_Cuts/roy.lp
 *
 */

/** \page NQueens N-Queens problem
 * \include queen/queen.cpp
 */

/** \page Knapsack Knapsack problem
 * \include ks/ks.cpp
 */


/* TutHowToApp Introduction Tutorial on Howto write an application

  This tutorial tries to drive you to write your own application.
  We first explain all things that are algorithm independant.
   We do not want here to focus on the Branch and Bound or Price, all the information
  here are for all Bobpp algorithms But we would considere that the algo you choose has
  the generic name of GLA.
  For a specific algorithms go to @ref UserAlgoGRP.

  section TutHowToStart Getting Started

  subsection TutDir The miscellaneous files and directory

  subsection TutSource The source files

*/

#include <bobpp/bobpp_config.h>

namespace Bob {

/** @defgroup CoreGRP The core of the library
 *  @brief This module stores various groups of class representing
 *         the core of the library.
 *  @{
 *
 *  This module stores number of sub-modules of the core of the libray.
 */

};
#include<bobpp/bob_conf.h>
#include<bobpp/bob_error.h>
#include<bobpp/bob_io.h>
#include<bobpp/util/bob_util.hpp>
#include<bobpp/bob_log.h>
#include<bobpp/bob_opt.h>
#include<bobpp/bob_core.h>
#include<bobpp/bob_id.h>
#include<bobpp/bob_stat.h>
/**
 * @}
 */

#include<bobpp/bob_sens.h>
#include<bobpp/bob_balgo.h>
#include<bobpp/bob_gdsbase.h>
#include<bobpp/bob_pqnode.h>
#include<bobpp/bob_dspq.h>
#include<bobpp/bob_schedalgo.h>
#include<bobpp/bob_keystate.hpp>
#include<bobpp/bob_simspalgo.h>
#include<bobpp/bob_eval.h>
#include<bobpp/bob_cost.h>
#include<bobpp/bob_pri.h>
#include<bobpp/bob_envprog.h>
#include<bobpp/bob_bbalgo.h>
#include<bobpp/bob_dcalgo.h>
#include<bobpp/bob_cdcalgo.h>
#include<bobpp/bob_sdcalgo.h>
#include<bobpp/bob_vnsalgo.h>


#include<bobpp/bob_prob.h>

