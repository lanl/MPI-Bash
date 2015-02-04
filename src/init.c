/****************************************************
 * Top-level dynamically loadable bash builtin that *
 * dynamically loads all other MPI-Bash builtins    *
 *                                                  *
 * By Scott Pakin <pakin@lanl.gov>                  *
 ****************************************************/

#include "builtins.h"
#include "shell.h"

int
mpi_init_builtin (WORD_LIST *list)
{
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_init builtin. */
char *mpi_init_doc[] = {
  "Initialize MPI and MPI-Bash.",
  ""
  "Invoke MPI_Init() then load all other MPI-Bash builtins."
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
