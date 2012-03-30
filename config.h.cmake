

#cmakedefine HAVE_LIBIMAGE 1

#cmakedefine HAVE_LIBNETPBM 1

#cmakedefine HAVE_LIBPPM 1


/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define to 1 if you have the `cbrt' function. */
#cmakedefine HAVE_CBRT 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H 1

/* Define to 1 if you have the `fork' function. */
#cmakedefine HAVE_FORK 1

/* Define to 1 if you have the `gamma' function. */
#cmakedefine HAVE_GAMMA 1

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H 1

/* Define to 1 if you have the `random' function. */
#cmakedefine HAVE_RANDOM 1

/* Define to 1 if you have the `srandom' function. */
#cmakedefine HAVE_SRANDOM 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to 1 if you have the `vfork' function. */
#cmakedefine HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
#cmakedefine HAVE_VFORK_H 1

/* Define to 1 if `fork' works. */
#cmakedefine HAVE_WORKING_FORK 1

/* Define to 1 if `vfork' works. */
#cmakedefine HAVE_WORKING_VFORK 1

#define PACKAGE "@PACKAGE@"
#define PACKAGE_NAME "@PACKAGE_NAME@"
#define PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"
#define PACKAGE_VERSION "@PACKAGE_VERSION@"
#define PACKAGE_STRING "@PACKAGE_STRING@"
#define VERSION PACKAGE_VERSION

/* File to include in Images/rgb_io.c */
#cmakedefine imageio_GLUE_CODE

/* Define to `int' if <sys/types.h> does not define. */
#cmakedefine pid_t

/* Define as `fork' if `vfork' does not work. */
#ifndef HAVE_WORKING_VFORK
#define vfork fork
#endif //HAVE_WORKING_VFORK
