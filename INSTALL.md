Installing MPI-Bash
===================

MPI-Bash has been packaged for a few Linux distributions (cf. [issue #9](https://github.com/lanl/MPI-Bash/issues/9)) so you may be able to install it with your distribution's package manager.  If not, then continue reading.

MPI-Bash is implemented as a Bash plugin.  To date, it has been tested only with Bash versions 4.3.*x* and 4.4.*x*, and there is indication that MPI-Bash does not work with Bash v4.2 or earlier ([issue #10](https://github.com/lanl/MPI-Bash/issues/10)).

To install MPI-Bash you will need the Bash header files.  On Debian/Ubuntu these are provided by the [`bash-builtins` package](https://packages.debian.org/search?searchon=names&keywords=bash-builtins).  If you have `bash-builtins`, follow Option 1 below.  Alternatively, the Bash header files can be found in the Bash source code, as discussed in Option 2.

Option 1: Package-provided Bash header files
--------------------------------------------

1. If not already installed, install the Bash header files.

        apt install bash-builtins

2. Go into the MPI-Bash source directory.  If you're building MPI-Bash from a Git clone rather than a release you'll have to generate the MPI-Bash `configure` script:

        autoreconf -fvi

3. Configure, build, and install MPI-Bash:

        ./configure --prefix=$HOME/mpibash CC=mpicc
        make
        make install

   `configure` should find the Bash header files on its own.

Option 2: Bash source code provided Bash header files
-----------------------------------------------------

1. Download the [Bash source code](http://www.gnu.org/software/bash/).  On a Debian Linux system (or derivative such as Ubuntu), the following is a convenient approach:

        apt-get source bash

2. Build Bash as normal.  There is no need to install it if you already have Bash on your system.

        ./configure
        make

3. Go into the MPI-Bash source directory.  If you're building MPI-Bash from a Git clone rather than a release you'll have to generate the MPI-Bash `configure` script:

        autoreconf -fvi

4. Configure, build, and install MPI-Bash, pointing it to your Bash source tree:

        ./configure --with-bashdir=$HOME/bash-4.4.12 --prefix=$HOME/mpibash CC=mpicc
        make
        make install
