/****************************************************
 * Top-level dynamically loadable bash builtin that *
 * dynamically loads all other MPI-Bash builtins    *
 *                                                  *
 * By Scott Pakin <pakin@lanl.gov>                  *
 ****************************************************/

#include "builtins.h"
#include "shell.h"

#include <stdio.h>
#include <mpi.h>

/* Initialize MPI with MPI_Init().  This has never worked for me with
 * Open MPI so we provide a hack in which the user can set
 * LD_PRELOAD=preload-mpi.so in advance of running bash. */
int
mpi_init_builtin (WORD_LIST *list)
{
  int inited;

  no_args(list);
  MPI_Initialized(&inited);
  if (!inited) {
    /* MPI_Init() has not yet been called.  For Open MPI, at least,
     * this is bad news.  However, we optimistically try to initialize
     * MPI anyway, just in case it works. */
    char *marg = "mpibash";
    char **margv = &marg;
    int margc = 1;

    MPI_Init(&margc, &margv);
  }
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_init builtin. */
char *mpi_init_doc[] = {
  "Initialize MPI and MPI-Bash.",
  "",
  "Invoke MPI_Init() then load all of the other MPI-Bash builtins.",
  "",
  "Exit Status:",
  "Returns success if MPI successfully initialized and all MPI-Bash",
  "builtins were loaded into the shell.",
  (char *)NULL
};

/* Describe the mpi_bash builtin. */
struct builtin mpi_init_struct = {
  "mpi_init",                /* Builtin name */
  mpi_init_builtin,          /* Function implementing the builtin */
  BUILTIN_ENABLED,           /* Initial flags for builtin */
  mpi_init_doc,              /* Builtin documentation */
  "mpi_init",                /* Usage synopsis */
  0                          /* Reserved */
};
