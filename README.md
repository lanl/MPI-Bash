MPI-Bash: Parallel scripting right from the Bourne-Again Shell (Bash)
=====================================================================

Introduction
------------

Parallel, scientific applications running on massively parallel supercomputers commonly produce large numbers of large data files.  While parallel filesystems improve the performance of file generation, postprocessing activities such as archiving and compressing the data are often run sequentially (and therefore slowly), squandering the supercomputer's vast performance.  Consequently, data that take hours to generate may take days to postprocess.

Writing and debugging parallel postprocessing programs is often too time-consuming to be worthwhile, especially for one-off postprocessing operations. What the community needs is a simple mechanism for cobbling together scripts that distribute postprocessing work across a supercomputer's many nodes. MPI-Bash attempts to supply that mechanism by augmenting the GNU project's command shell, [Bash](http://www.gnu.org/software/bash/), with support for the Message Passing Interface ([MPI](http://www.mpi-forum.org/)) and, if available, the [Libcircle](http://hpc.github.io/libcircle/) library for work distribution and load balancing.

MPI-Bash makes it easy to parallelize Bash scripts that run a set of Linux commands independently over a large number of input files. Because MPI-Bash includes various MPI functions for data transfer and synchronization, it is not limited to embarrassingly parallel workloads but can incorporate phased operations (i.e., all workers must finish operation *X* before any worker is allowed to begin operation *Y*).

Installation
------------

1. Download the [Bash source code](http://www.gnu.org/software/bash/).  MPI-Bash has been tested only with Bash v4.3 so that version is recommended.

2. Copy MPI-Bash's `builtins/circle.def`, `builtins/Makefile.in`, `builtins/mpi.def`, `config.h.in`, `configure.ac`, `Makefile.in`, and `shell.c` to the corresponding location in the Bash source tree.

3. Build Bash as normal:

        ./configure
        make
        make install

Usage
-----

The MPI-Bash executable is normally called mpibash. Assuming that mpibash is in the invoking shell's search path, an MPI-Bash script can be written as an ordinary Bash script but with

    #! /usr/bin/env mpibash

as the first line of the script. The script can then be run like any other MPI program, such as via a command like the following:

    mpirun -np 100 ./my-mpibash-script

For testing purposes, mpibash can even be run interactively on systems that support such an execution model:

    mpirun -np 4 xterm -e mpibash

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
