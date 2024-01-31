MPI-Bash: Parallel scripting right from the Bourne-Again Shell (Bash)
=====================================================================

Introduction
------------

Parallel, scientific applications running on massively parallel supercomputers commonly produce large numbers of large data files.  While parallel filesystems improve the performance of file generation, postprocessing activities such as archiving and compressing the data are often run sequentially (and therefore slowly), squandering the supercomputer's vast performance.  Consequently, data that take hours to generate may take days to postprocess.

Writing and debugging parallel postprocessing programs is often too time-consuming to be worthwhile, especially for one-off postprocessing operations. What the community needs is a simple mechanism for cobbling together scripts that distribute postprocessing work across a supercomputer's many nodes. MPI-Bash attempts to supply that mechanism by augmenting the GNU project's command shell, [Bash](http://www.gnu.org/software/bash/), with support for the Message Passing Interface ([MPI](http://www.mpi-forum.org/)) and, if available, the [Libcircle](http://hpc.github.io/libcircle/) library for work distribution and load balancing.

MPI-Bash makes it easy to parallelize Bash scripts that run a set of Linux commands independently over a large number of input files. Because MPI-Bash includes various MPI functions for data transfer and synchronization, it is not limited to embarrassingly parallel workloads but can incorporate phased operations (i.e., all workers must finish operation *X* before any worker is allowed to begin operation *Y*).

Installation
------------

[Installation instructions](https://github.com/lanl/MPI-Bash/blob/master/INSTALL.md) are provided in a separate file.

Usage
-----

Unlike the initial release of MPI-Bash, which was coded as a patched version of the `bash` executable, newer versions are coded as a Bash ["loadable builtin"](http://www.drdobbs.com/shell-corner-bash-dynamically-loadable-b/199102950) (a.k.a. plugin), which makes it much easier to deploy.

To use MPI-Bash functions, create an executable Bash script that begins with the following few lines:

    #! /usr/bin/env mpibash

    enable -f mpibash.so mpi_init
    mpi_init
    mpi_finalize

The `enable` line loads and enables the `mpi_init` builtin, and the subsequent line invokes it.  Besides MPI initialization, `mpi_init` additionally loads and enables all of the other MPI-Bash builtins.

The script can then be run like any other MPI program, such as via a command like the following:

    mpiexec -n 16 ./my-script.sh

If `MPI_Init` fails, it may because your system is unable to `mpiexec` one script (`mpibash`) that itself runs another script (your program) that dynamically loads the MPI libraries.  See if the workaround discussed in [issue #6](https://github.com/lanl/MPI-Bash/issues/6) applies in your situation.

When running MPI-Bash on a large number of nodes, a parallel filesystem (e.g., [Lustre](http://lustre.opensfs.org/)) is essential for performance. Otherwise, most of the parallelism that a script exposes will be lost as file operations are serialized during writes to a non-parallel filesystem.

Documentation
-------------

MPI-Bash documentation is available on the [MPI-Bash wiki](https://github.com/lanl/MPI-Bash/wiki).

Copyright and license
---------------------

Triad National Security, LLC (Triad) owns the copyright to MPI-Bash, which it identifies internally as LA-CC-14-101.  MPI-Bash is licensed under the [GNU General Public License](http://www.gnu.org/licenses/gpl-3.0.html), version 3.

![GPLv3 logo](https://gnu.org/graphics/gplv3-127x51.png "GNU General Public License, version 3")

Author
------

Scott Pakin, [_pakin@lanl.gov_](mailto:pakin@lanl.gov)
