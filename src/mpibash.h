/****************************************
 * Definitions used throughout MPI-Bash *
 *                                      *
 * By Scott Pakin <pakin@lanl.gov>      *
 ****************************************/

#ifndef _MPIBASH_H_
#define _MPIBASH_H_

#include "builtins.h"
#include "shell.h"
#include "common.h"
#include "bashintl.h"
#include "bashgetopt.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <mpi.h>

/* Try an MPI operation.  Return with an error message on failure. */
#define MPI_TRY(STMT)                                   \
  do                                                    \
    {                                                   \
      int mpierr;                                       \
      mpierr = STMT;                                    \
      if (mpierr != MPI_SUCCESS)                        \
        return mpibash_report_mpi_error (mpierr);       \
    }                                                   \
  while (0)

/* Return with a usage message if no arguments remain. */
#define YES_ARGS(LIST)                          \
  if ((LIST) == 0) {                            \
    builtin_usage();                            \
    return EX_USAGE;                            \
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

/* Simplify defining an MPI-Bash builtin. */
#define DEFINE_BUILTIN(NAME, SYNOPSIS)                          \
  struct builtin NAME##_struct = {                              \
    #NAME,             /* Builtin name */                       \
    NAME##_builtin,    /* Function implementing the builtin */  \
    BUILTIN_ENABLED,   /* Initial flags for builtin */          \
    NAME##_doc,        /* Builtin documentation */              \
    SYNOPSIS,          /* Usage synopsis */                     \
    0                  /* Reserved */                           \
  }

/* Declare all of the external variables and functions we need. */
extern int mpibash_rank;
extern int mpibash_num_ranks;
extern SHELL_VAR *mpibash_bind_variable_number (const char *name, long value, int flags);
extern int mpibash_report_mpi_error (int mpierr);
extern SHELL_VAR *mpibash_bind_array_variable_number (char *name, arrayind_t ind, long value, int flags);
extern int mpibash_invoke_bash_command(char *funcname, ...);
extern int mpibash_find_callback_function (WORD_LIST *list, SHELL_VAR **user_func);

#endif
