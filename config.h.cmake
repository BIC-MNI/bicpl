

#cmakedefine01 HAVE_LIBIMAGE

#cmakedefine01 HAVE_LIBNETPBM

#cmakedefine01 HAVE_LIBPPM


/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define to 1 if you have the `cbrt' function. */
#cmakedefine01 HAVE_CBRT

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine01 HAVE_DLFCN_H

/* Define to 1 if you have the `fork' function. */
#cmakedefine01 HAVE_FORK

/* Define to 1 if you have the `gamma' function. */
#cmakedefine01 HAVE_GAMMA

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine01 HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine01 HAVE_INTTYPES_H

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine01 HAVE_MEMORY_H

/* Define to 1 if you have the `random' function. */
#cmakedefine01 HAVE_RANDOM

/* Define to 1 if you have the `srandom' function. */
#cmakedefine01 HAVE_SRANDOM

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine01 HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine01 HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine01 HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine01 HAVE_STRING_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine01 HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine01 HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine01 HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine01 HAVE_UNISTD_H

/* Define to 1 if you have the `vfork' function. */
#cmakedefine01 HAVE_VFORK

/* Define to 1 if you have the <vfork.h> header file. */
#cmakedefine01 HAVE_VFORK_H

/* Define to 1 if `fork' works. */
#cmakedefine01 HAVE_WORKING_FORK

/* Define to 1 if `vfork' works. */
#cmakedefine01 HAVE_WORKING_VFORK

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
