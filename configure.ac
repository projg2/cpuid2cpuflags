AC_PREREQ([2.60])
AC_INIT([cpuid2cpuflags], [11])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.9 foreign dist-bzip2 parallel-tests subdir-objects -Wall])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])

AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
AC_TYPE_UINT32_T

# TODO: skip this on x86?
AC_CHECK_HEADERS([sys/auxv.h], [
	AC_CHECK_FUNCS([getauxval])
])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
