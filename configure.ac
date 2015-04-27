dnl Process this file with autoconf to produce a configure script.

AC_INIT(bicpl, [1.5.0], [robert.d.vincent@mcgill.ca])
AC_CONFIG_SRCDIR([Include/bicpl.h])
AC_CONFIG_AUX_DIR(ac_config_aux)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Build only static libs by default
AM_DISABLE_SHARED
AM_PROG_LIBTOOL

AC_CHECK_LIB(netcdf, ncopen, [], [AC_MSG_ERROR([-lnetcdf not found])])
AC_CHECK_LIB(hdf5, H5open, [], [AC_MSG_ERROR([-lhdf5 not found])])
AC_CHECK_LIB(minc2, miopen, [], [AC_MSG_ERROR([-lminc2 not found])])

AC_FUNC_FORK
AC_CHECK_FUNCS(srandom random cbrt gamma gettimeofday)
AC_CHECK_HEADERS([sys/time.h])

AC_DEFINE([HAVE_MINC1],[1],[Define if MINC 1.0 is enabled.])
AC_DEFINE([HAVE_MINC2],[1],[Define if MINC 2.0 is enabled.])
AC_DEFINE([MINC2],[1],[Define if MINC 2.0 is enabled.])

imageio_GLUE_CODE=Images/rgb_io_none.c

AC_DEFINE_UNQUOTED(imageio_GLUE_CODE,
    ["$imageio_GLUE_CODE"],[File to include in Images/rgb_io.c])


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
