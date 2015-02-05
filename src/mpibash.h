/****************************************
 * Definitions used throughout MPI-Bash *
 *				        *
 * By Scott Pakin <pakin@lanl.gov>      *
 ****************************************/

#ifndef _MPIBASH_H_
#define _MPIBASH_H_

#include "builtins.h"
#include "shell.h"
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <mpi.h>

extern int mpibash_rank;
extern int mpibash_num_ranks;

/* Try an MPI operation.  Return with an error message on failure. */
#define MPI_TRY(STMT)                           \
  do                                            \
    {                                           \
      int mpierr;                               \
      mpierr = STMT;                            \
      if (mpierr != MPI_SUCCESS)                \
        return report_mpi_error (mpierr);       \
    }                                           \
  while (0)

/* Return with a usage message if no arguments remain. */
#define YES_ARGS(LIST)                          \
  if ((LIST) == 0)                              \
    {                                           \
      builtin_usage ();                         \
      return (EX_USAGE);                        \
    }

/* Return with an error message if a given variable is read-only or if
 * we can't write to it for any other reason (e.g., it's defined as a
 * function). */
#define REQUIRE_WRITABLE(NAME)                                          \
  do                                                                    \
    {                                                                   \
      SHELL_VAR *bindvar = find_shell_variable (NAME);                  \
      if (bindvar)                                                      \
        {                                                               \
          if (readonly_p (bindvar))                                     \
            {                                                           \
              err_readonly (NAME);                                      \
              return (EXECUTION_FAILURE);                               \
            }                                                           \
          if (unbind_variable (NAME) == -1)                             \
            {                                                           \
              builtin_error ("Failed to write to variable %s", NAME);   \
              return (EXECUTION_FAILURE);                               \
            }                                                           \
        }                                                               \
    }                                                                   \
  while (0)

#endif
