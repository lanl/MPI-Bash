###################################
# Configure MPI-Bash              #
# By Scott Pakin <pakin@lanl.gov> #
###################################

# Initialize the GNU Autotools.
AC_PREREQ([2.69])
AC_INIT([mpibash], [1.1], [pakin@lanl.gov])
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

# Find an already-configured bash source tree.
AC_ARG_WITH([bashdir],
  [AS_HELP_STRING([--with-bashdir=DIR],
    [directory containing preconfigured bash sources])],
  [bashdir="$withval"],
  [bashdir=no])
AC_CACHE_CHECK([for a bash source tree],
  [ax_cv_dir_bash_sources], [
ax_cv_dir_bash_sources=no
if test "x$bashdir" = xno ; then
  # --with-bashdir was not specified; look in the current directory.
  if test `echo bash*` != "bash*" ; then
    for dname in `ls -d bash* | sort -r` ; do
      if test -d "$dname" ; then
	ax_cv_dir_bash_sources="`readlink -f $dname`"
	break
      fi
    done
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

# We require the dladdr Glibc extension to help a .so file query itself.
AC_SEARCH_LIBS([dladdr], [dl], , [
  AC_MSG_ERROR([Cannot continue without the dladdr Glibc extension.])])

# Generate a Makefile.
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT