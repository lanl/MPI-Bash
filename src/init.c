/****************************************************
 * Top-level dynamically loadable bash builtin that *
 * dynamically loads all other MPI-Bash builtins    *
 *                                                  *
 * By Scott Pakin <pakin@lanl.gov>                  *
 ****************************************************/

#include "builtins.h"
#include "shell.h"
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <mpi.h>

static int we_called_init = 0;  /* 1=we called MPI_Init(); 0=it was called for us */

/* Load another builtin from our plugin by invoking "enable -f
 * mpibash.so <name>". */
static int load_mpi_builtin (char *name)
{
  static char *mpibash_so = NULL;   /* Path to mpibash.so */
  static sh_builtin_func_t *enable_func = NULL;   /* Pointer to the enable function */
  char *enable_args[4];
  WORD_LIST *enable_list;

  /* Find our .so file name. */
  if (mpibash_so == NULL) {
    Dl_info self_info;

    if (dladdr(load_mpi_builtin, &self_info) == 0 || self_info.dli_fname == NULL) {
      fprintf(stderr, "mpi_init: failed to find the MPI-Bash .so file\n");
      return EXECUTION_FAILURE;
    }
    mpibash_so = strdup(self_info.dli_fname);
  }

  /* Find bash's enable function, even if it's disabled. */
  enable_func = builtin_address("enable");
  if (enable_func == NULL) {
    fprintf(stderr, "mpi_init: failed to find the enable builtin\n");
    return EXECUTION_FAILURE;
  }

  /* Enable the specified function. */
  enable_args[0] = "-f";
  enable_args[1] = mpibash_so;
  enable_args[2] = name;
  enable_args[3] = NULL;
  enable_list = strvec_to_word_list(enable_args, 1, 0);
  if (enable_func(enable_list) == EXECUTION_FAILURE) {
    fprintf(stderr, "mpi_init: failed to load builtin %s from %s\n", name, mpibash_so);
    dispose_words(enable_list);
    return EXECUTION_FAILURE;
  }
  dispose_words(enable_list);
  return EXECUTION_SUCCESS;
}

/* Initialize MPI with MPI_Init().  This has never worked for me with
 * Open MPI so we provide a hack in which the user can set
 * LD_PRELOAD=preload-mpi.so in advance of running bash. */
int
mpi_init_builtin (WORD_LIST *list)
{
  int inited;

  /* Initialize MPI. */
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
    we_called_init = 1;
  }

  /* As a convenience for the user, load all of the other MPI-Bash builtins. */
  if (load_mpi_builtin("mpi_finalize") != EXECUTION_SUCCESS)
    return EXECUTION_FAILURE;

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

/* Describe the mpi_init builtin. */
struct builtin mpi_init_struct = {
  "mpi_init",                /* Builtin name */
  mpi_init_builtin,          /* Function implementing the builtin */
  BUILTIN_ENABLED,           /* Initial flags for builtin */
  mpi_init_doc,              /* Builtin documentation */
  "mpi_init",                /* Usage synopsis */
  0                          /* Reserved */
};

/* Finalize MPI with MPI_Finalize(), but only if we previously invoked
 * MPI_Init() explicitly. */
int
mpi_finalize_builtin (WORD_LIST *list)
{
  no_args(list);
  if (we_called_init)
    MPI_Finalize();
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_finalize builtin. */
char *mpi_finalize_doc[] = {
  "Finalize MPI and MPI-Bash.",
  "",
  "Invoke MPI_Finalize().",
  "",
  "Exit Status:",
  "Always succeeds.  However, the MPI standard does not define what",
  "happens after MPI_Finalize() is called.  Consequently, the shell or",
  "shell script should exit immediately after mpi_finalize returns.",
  (char *)NULL
};

/* Describe the mpi_finalize builtin. */
struct builtin mpi_finalize_struct = {
  "mpi_finalize",                /* Builtin name */
  mpi_finalize_builtin,          /* Function implementing the builtin */
  BUILTIN_ENABLED,               /* Initial flags for builtin */
  mpi_finalize_doc,              /* Builtin documentation */
  "mpi_finalize",                /* Usage synopsis */
  0                              /* Reserved */
};
