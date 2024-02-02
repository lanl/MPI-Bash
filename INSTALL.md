Installing MPI-Bash
===================

MPI-Bash has been packaged for a few Linux distributions so you may be able to install it with your distribution's package manager.  If not, then continue reading.

MPI-Bash is implemented as a Bash plugin.  To date, it has been tested only with Bash versions 4.3.*x*, 4.4.*x*, and 5.1.*x*.  There is indication that MPI-Bash does not work with Bash v4.2 or earlier ([issue #10](https://github.com/lanl/MPI-Bash/issues/10)).

To install MPI-Bash you will need the Bash header files.  On Debian-based distributions these are provided by the [`bash-builtins` package](https://packages.debian.org/search?searchon=names&keywords=bash-builtins), and on RPM-based distributions these are provided by the [`bash-devel` package](https://rpmfind.net/linux/rpm2html/search.php?query=bash-devel).  Ensure that whatever package you install includes `execute-cmd.h`, which sometimes is omitted from distribution packages.

If you have the Bash header files, follow Option 1 below.  If not, the Bash header files can be found in the Bash source code, as discussed in Option 2.

Option 1: Package-provided Bash header files
--------------------------------------------

1. Go into the MPI-Bash source directory.  If you're building MPI-Bash from a Git clone rather than a release you'll have to generate the MPI-Bash `configure` script:

        autoreconf -fvi

2. Configure, build, and install MPI-Bash:

        ./configure --prefix=$HOME/mpibash CC=mpicc
        make
        make install

   `configure` should find the Bash header files on its own.

Option 2: Bash source code provided Bash header files
-----------------------------------------------------

1. Download the [Bash source code](http://www.gnu.org/software/bash/) with the same version number as your running `bash`.  On a Debian Linux system (or derivative such as Ubuntu), the following is a convenient approach:

        apt-get source bash

2. Build Bash as normal.  There is no need to install it (assuming you already have Bash on your system).

        ./configure
        make

3. Go into the MPI-Bash source directory.  If you're building MPI-Bash from a Git clone rather than a release you'll have to generate the MPI-Bash `configure` script:

        autoreconf -fvi

4. Configure, build, and install MPI-Bash, pointing it to your Bash source tree:

        ./configure --with-bashdir=$HOME/bash-5.1.16 --prefix=$HOME/mpibash CC=mpicc
        make
        make install
