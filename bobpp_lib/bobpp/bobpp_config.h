#ifndef _BOBPP_BOBPP_CONFIG_H
#define _BOBPP_BOBPP_CONFIG_H 1
 
/* bobpp/bobpp_config.h. Generated automatically at end of configure. */
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if debugging is enabled. */
/* #undef ENABLE_DEBUG */

/* Define if fault tolerancy is enabled. */
/* #undef ENABLE_FT */

/* Define if space generation is enabled. */
/* #undef ENABLE_SPACE */

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef BOBPP_HAVE_DLFCN_H 
#define BOBPP_HAVE_DLFCN_H  1 
#endif

/* Define to 1 if you have the `gettimeofday' function. */
#ifndef BOBPP_HAVE_GETTIMEOFDAY 
#define BOBPP_HAVE_GETTIMEOFDAY  1 
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef BOBPP_HAVE_INTTYPES_H 
#define BOBPP_HAVE_INTTYPES_H  1 
#endif

/* Defined if you have Kaapi */
/* #undef HAVE_LIBATHA */

/* Defined if you have libgiw */
/* #undef HAVE_LIBGIW */

/* Defined if you have libglock */
/* #undef HAVE_LIBGLOCK */

/* Defined if you have libglop */
/* #undef HAVE_LIBGLOP */

/* Defined if you have libppglop */
/* #undef HAVE_LIBPPGLOP */

/* Define to 1 if you have the `pthread' library (-lpthread). */
#ifndef BOBPP_HAVE_LIBPTHREAD 
#define BOBPP_HAVE_LIBPTHREAD  1 
#endif

/* Define to 1 if you have the `pthreadGC2' library (-lpthreadGC2). */
/* #undef HAVE_LIBPTHREADGC2 */

/* Define to 1 if you have the <memory.h> header file. */
#ifndef BOBPP_HAVE_MEMORY_H 
#define BOBPP_HAVE_MEMORY_H  1 
#endif

/* Defined if you have libmpi */
#ifndef BOBPP_HAVE_MPI 
#define BOBPP_HAVE_MPI  1 
#endif

/* Define to 1 if you have the `pthread_setaffinity_np' function. */
/* #undef HAVE_PTHREAD_SETAFFINITY_NP */

/* Define to 1 if you have the `socket' function. */
#ifndef BOBPP_HAVE_SOCKET 
#define BOBPP_HAVE_SOCKET  1 
#endif

/* Define to 1 if `stat' has the bug that it succeeds when given the
   zero-length file name argument. */
/* #undef HAVE_STAT_EMPTY_STRING_BUG */

/* Define to 1 if stdbool.h conforms to C99. */
/* #undef HAVE_STDBOOL_H */

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef BOBPP_HAVE_STDINT_H 
#define BOBPP_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef BOBPP_HAVE_STDLIB_H 
#define BOBPP_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef BOBPP_HAVE_STRINGS_H 
#define BOBPP_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef BOBPP_HAVE_STRING_H 
#define BOBPP_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef BOBPP_HAVE_SYS_STAT_H 
#define BOBPP_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef BOBPP_HAVE_SYS_TIME_H 
#define BOBPP_HAVE_SYS_TIME_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef BOBPP_HAVE_SYS_TYPES_H 
#define BOBPP_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef BOBPP_HAVE_UNISTD_H 
#define BOBPP_HAVE_UNISTD_H  1 
#endif

/* Define to 1 if the system has the type `_Bool'. */
/* #undef HAVE__BOOL */

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
/* #undef LSTAT_FOLLOWS_SLASHED_SYMLINK */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef BOBPP_LT_OBJDIR 
#define BOBPP_LT_OBJDIR  ".libs/" 
#endif

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Define to the address where bug reports for this package should be sent. */
#ifndef BOBPP_PACKAGE_BUGREPORT 
#define BOBPP_PACKAGE_BUGREPORT  "Bertrand.Lecun@prism.uvsq.fr" 
#endif

/* Define to the full name of this package. */
#ifndef BOBPP_PACKAGE_NAME 
#define BOBPP_PACKAGE_NAME  "bobpp" 
#endif

/* Define to the full name and version of this package. */
#ifndef BOBPP_PACKAGE_STRING 
#define BOBPP_PACKAGE_STRING  "bobpp 0.3.0" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef BOBPP_PACKAGE_TARNAME 
#define BOBPP_PACKAGE_TARNAME  "bobpp" 
#endif

/* Define to the home page for this package. */
#ifndef BOBPP_PACKAGE_URL 
#define BOBPP_PACKAGE_URL  "" 
#endif

/* Define to the version of this package. */
#ifndef BOBPP_PACKAGE_VERSION 
#define BOBPP_PACKAGE_VERSION  "0.3.0" 
#endif

/* Define as the return type of signal handlers (`int' or `void'). */
#ifndef BOBPP_RETSIGTYPE 
#define BOBPP_RETSIGTYPE  void 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef BOBPP_STDC_HEADERS 
#define BOBPP_STDC_HEADERS  1 
#endif

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#ifndef BOBPP_TIME_WITH_SYS_TIME 
#define BOBPP_TIME_WITH_SYS_TIME  1 
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif
 
/* once: _BOBPP_BOBPP_CONFIG_H */
#endif
