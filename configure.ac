###################################
# Configure MPI-Bash              #
# By Scott Pakin <pakin@lanl.gov> #
###################################

# Initialize the GNU Autotools.
AC_PREREQ([2.69])
AC_INIT([mpibash], [1.3], [pakin@lanl.gov])
AC_CONFIG_SRCDIR([src/init.c])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_AR
LT_INIT([disable-static])

# Check for programs we require.
AC_PROG_CC
AC_CHECK_PROG([READLINK], [readlink], [yes], [no])
if test "x$READLINK" != xyes ; then
  AC_MSG_ERROR([Cannot continue without a readlink program.])
fi
PKG_PROG_PKG_CONFIG

# Find an already-configured bash source tree.
AC_ARG_WITH([bashdir],
  [AS_HELP_STRING([--with-bashdir=DIR],
    [directory containing preconfigured bash sources])],
  [bashdir="$withval"],
  [bashdir=no])
AC_CACHE_CHECK([for a directory containing bash header files],
  [ax_cv_dir_bash_sources], [
ax_cv_dir_bash_sources=no
if test "x$bashdir" = xno ; then
  # --with-bashdir was not specified; look in the current directory for a
  # bash source directory.
  if test `echo bash*` != "bash*" ; then
    for dname in `ls -d bash* | sort -r` ; do
      if test -d "$dname" ; then
        ax_cv_dir_bash_sources="`readlink -f $dname`"
        break
      fi
    done
  # bash was not found in the current directory.  See if pkg-config can
  # find the bash headers.
  elif test "x$PKG_CONFIG" != xno ; then
    PKG_CHECK_EXISTS([bash], [
      ax_cv_dir_bash_sources=`$PKG_CONFIG --variable=headersdir bash`
    ])
  fi
else
  # --with-bashdir was specified; ensure it's valid.
  if test -d "$bashdir" ; then
    ax_cv_dir_bash_sources="`readlink -f $bashdir`"
  fi
fi
  ])
if test "x$ax_cv_dir_bash_sources" = xno ; then
  AC_MSG_ERROR([Cannot continue without a preconfigured bash source directory.  Please specify one with --with-bashdir.])
fi
if test "x$bashdir" = xno ; then
  bashdir="$ax_cv_dir_bash_sources"
fi
BASH_CPPFLAGS="-DSHELL -I$bashdir -I$bashdir/include -I$bashdir/builtins"
AC_SUBST([BASH_CPPFLAGS])
orig_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$BASH_CPPFLAGS $CPPFLAGS"
AC_CHECK_HEADERS([config.h builtins.h], ,
  [AC_MSG_ERROR([Cannot find bash header $ac_header under $bashdir.  Did you run bash's configure script there?])])
AC_CHECK_HEADERS([pathnames.h], ,
  [AC_MSG_ERROR([Cannot find bash header $ac_header under $bashdir.  Did you run make there?])])
AC_CHECK_HEADERS([shell.h bashgetopt.h], ,
  [AC_MSG_ERROR([Cannot find bash header $ac_header under $bashdir.])],
  [
#include <config.h>
#include <builtins.h>
  ])
CPPFLAGS="$orig_CPPFLAGS"

# Ensure that we have MPI support.
AC_SEARCH_LIBS([MPI_Init], [mpi], , [
  AC_MSG_ERROR([Cannot continue without MPI.  Consider specifying CC=mpicc.])])
AC_CHECK_HEADER([mpi.h], , [
  AC_MSG_ERROR([Cannot continue without MPI.  Consider specifying CC=mpicc.])])
AC_CACHE_CHECK([if we can compile and link an MPI program],
  [ax_cv_link_mpi],
  [AC_LINK_IFELSE(
    [AC_LANG_SOURCE([[
#include <mpi.h>

int main (int argc, char *argv[])
{
  MPI_Init(&argc, &argv);
  MPI_Finalize();
  return 0;
}
    ]])],
    [ax_cv_link_mpi=yes],
    [ax_cv_link_mpi=no])])
if test "x$ax_cv_link_mpi" != xyes ; then
  AC_MSG_ERROR([Cannot continue without MPI.  Consider specifying CC=mpicc.])
fi

# Ensure that the C compiler supports __attribute__((constructor)).
AC_CACHE_CHECK([if $CC supports __attribute__((constructor))],
  [ax_cv_prog_cc_attr_cons],
  [AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE([[
#include <stdio.h>

__attribute__((constructor))
static void hello (void)
{
  fprintf(stderr, "Hello, world!\n");
}

__attribute__((destructor))
static void goodbye (void)
{
  fprintf(stderr, "Goodbye, world!\n");
}
    ]])],
    [ax_cv_prog_cc_attr_cons=yes],
    [ax_cv_prog_cc_attr_cons=no])])
if test "x$ax_cv_prog_cc_attr_cons" != xyes ; then
  AC_MSG_ERROR([Cannot continue without compiler support for constructor functions])
fi

# We require the dladdr Glibc extension to help a .so file query itself.
AC_SEARCH_LIBS([dladdr], [dl], , [
  AC_MSG_ERROR([Cannot continue without the dladdr Glibc extension.])])

# Determine if we have Libcircle installed.
AC_CACHE_CHECK([if we can compile and link a Libcircle program],
  [ax_cv_link_libcircle],
  [ax_cv_link_libcircle=no
   AC_LINK_IFELSE(
    [AC_LANG_SOURCE([[
#include <sys/types.h>
#include <libcircle.h>

int main (int argc, char *argv[])
{
  (void) CIRCLE_init(argc, argv, 0);
  return 0;
}
    ]])],
    [ax_cv_link_libcircle=yes],
    [
# Try to find the Libcircle headers and libraries using pkg-config.
if test "x$PKG_CONFIG" != xno ; then
  PKG_CHECK_EXISTS([libcircle], [
    orig_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS `$PKG_CONFIG libcircle --cflags`"
    orig_LIBS="$LIBS"
    LIBS="$LIBS `$PKG_CONFIG libcircle --libs`"
    AC_LINK_IFELSE(
      [AC_LANG_SOURCE([[
#include <sys/types.h>
#include <libcircle.h>

int main (int argc, char *argv[])
{
  (void) CIRCLE_init(argc, argv, 0);
  return 0;
}
      ]])],
      [ax_cv_link_libcircle=yes
       CIRCLE_CFLAGS="`$PKG_CONFIG libcircle --cflags`"
       CIRCLE_LIBS="`$PKG_CONFIG libcircle --libs`"])
    CFLAGS="$orig_CFLAGS"
    LIBS="$orig_LIBS"
  ])
fi
])])
AC_SUBST([CIRCLE_CFLAGS])
AC_SUBST([CIRCLE_LIBS])
AM_CONDITIONAL([HAVE_LIBCIRCLE], [test "x$ax_cv_link_libcircle" = xyes])

# Tell the Makefiles where plugins should be installed.
AC_ARG_WITH([plugindir],
  [AS_HELP_STRING([--with-plugindir=DIR],
    [install MPI-Bash plugins into DIR [default=$libexecdir/mpibash]])],
  [plugindir=$withval],
  [plugindir=$libexecdir/$PACKAGE_NAME])
AC_SUBST([plugindir], [$plugindir])

# Warn the user if Libcircle isn't available.
if test "x$ax_cv_link_libcircle" != xyes ; then
  AC_MSG_WARN([Building MPI-Bash without Libcircle support])
fi

# Honor the program suffix when generating commands/mb*.
if test "x$program_suffix" = xNONE ; then
  program_suffix=""
fi
AC_SUBST([program_suffix])

# Generate Makefiles and other files.
AC_CONFIG_FILES([Makefile src/Makefile commands/Makefile examples/Makefile])
AC_CONFIG_FILES([mpibash commands/mbcp commands/mbtar commands/mbxz])
AC_OUTPUT
