MPI-Bash: Parallel scripting right from the Bourne-Again Shell (Bash)
=====================================================================

Introduction
------------

Parallel, scientific applications running on massively parallel supercomputers commonly produce large numbers of large data files.  While parallel filesystems improve the performance of file generation, postprocessing activities such as archiving and compressing the data are often run sequentially (and therefore slowly), squandering the supercomputer's vast performance.  Consequently, data that take hours to generate may take days to postprocess.

Writing and debugging parallel postprocessing programs is often too time-consuming to be worthwhile, especially for one-off postprocessing operations. What the community needs is a simple mechanism for cobbling together scripts that distribute postprocessing work across a supercomputer's many nodes. MPI-Bash attempts to supply that mechanism by augmenting the GNU project's command shell, [Bash](http://www.gnu.org/software/bash/), with support for the Message Passing Interface ([MPI](http://www.mpi-forum.org/)) and, if available, the [Libcircle](http://hpc.github.io/libcircle/) library for work distribution and load balancing.

MPI-Bash makes it easy to parallelize Bash scripts that run a set of Linux commands independently over a large number of input files. Because MPI-Bash includes various MPI functions for data transfer and synchronization, it is not limited to embarrassingly parallel workloads but can incorporate phased operations (i.e., all workers must finish operation *X* before any worker is allowed to begin operation *Y*).

Installation
------------

1. Download the [Bash source code](http://www.gnu.org/software/bash/).  As of this writing, MPI-Bash is being tested primarily with Bash v4.3.  On a Debian Linux system (or derivative such as Ubuntu), the following is a convenient approach:

        apt-get source bash

2. Build Bash as normal.  There is no need to install it if you already have Bash on your system.

        ./configure
        make

3. Go into the MPI-Bash source directory.  If you're building MPI-Bash from a Git clone rather than a release you'll have to generate the MPI-Bash `configure` script:

        autoreconf -fvi

4. Configure, build, and install MPI-Bash, pointing it to your Bash source tree:

        ./configure --with-bashdir=$HOME/bash-4.3.30 --prefix=$HOME/mpibash CC=mpicc
        make
        make install

Usage
-----

Unlike the initial release of MPI-Bash, which was coded as a patched version of the `bash` executable, newer versions are coded as a Bash ["loadable builtin"](http://www.drdobbs.com/shell-corner-bash-dynamically-loadable-b/199102950) (a.k.a. plugin), which makes it much easier to deploy.

To use MPI-Bash functions, create an executable Bash script that begins with the following few lines:

    #! /usr/bin/env mpibash

    enable -f $MPIBASH_PLUGIN mpi_init
    mpi_init

The `enable` line loads and enables the `mpi_init` builtin, and the subsequent line invokes it.  Besides MPI initialization, `mpi_init` additionally loads and enables all of the other MPI-Bash builtins.

The script can then be run like any other MPI program, such as via a command like the following:

    mpirun -np 16 ./my-script.sh

If MPI-Bash is run on a large number of nodes, a parallel filesystem (e.g., [Lustre](http://lustre.opensfs.org/)) is essential for performance. Otherwise, most of the parallelism that a script exposes will be lost as file operations are serialized during writes to a non-parallel filesystem.

Documentation
-------------

For the time being, see the [old MPI-Bash home page](http://www.ccs3.lanl.gov/~pakin/software/mpibash-4.3.html) for descriptions of each MPI function that MPI-Bash adds to Bash.  For a more scholarly discussion, see

> Scott Pakin, "Parallel Post-Processing with MPI-Bash".  *Proceedings of the First International Workshop on HPC User Support Tools* (HUST-14), New Orleans, Lousiana, pp. 1-12, November 2014.  IEEE Press, Piscataway, New Jersey, USA. ISBN: 978-1-4799-7023-0.

Copyright and license
---------------------

Los Alamos National Security, LLC (LANS) owns the copyright to MPI-Bash, which it identifies internally as CODE-2015-8.  MPI-Bash is licensed under the [GNU General Public License](https://gnu.org/licenses/gpl.html), version 3.

![GPLv3 logo](https://gnu.org/graphics/gplv3-127x51.png "GNU General Public License, version 3")

Author
------

Scott Pakin, [_pakin@lanl.gov_](mailto:pakin@lanl.gov)
