/******************************************************
 * MPI-Bash initialization and finalization functions *
 *                                                    *
 * By Scott Pakin <pakin@lanl.gov>                    *
 ******************************************************/

#include "mpibash.h"

static int we_called_init = 0;  /* 1=we called MPI_Init(); 0=it was called for us */
static char *all_mpibash_builtins[] = {  /* All builtins MPI-Bash defines except mpi_init */
  "mpi_abort",
  "mpi_allreduce",
  "mpi_barrier",
  "mpi_bcast",
  "mpi_comm_rank",
  "mpi_comm_size",
  "mpi_exscan",
  "mpi_finalize",
  "mpi_recv",
  "mpi_scan",
  "mpi_send",
  NULL
};

/* Keep track of who we are within MPI_COMM_WORLD. */
int mpibash_rank;
int mpibash_num_ranks;

extern int running_trap, trap_saved_exit_value;

/* Load another builtin from our plugin by invoking "enable -f
 * mpibash.so <name>". */
static int
load_mpi_builtin (char *name)
{
  static char *mpibash_so = NULL;   /* Path to mpibash.so */
  static sh_builtin_func_t *enable_func = NULL;   /* Pointer to the enable function */
  char *enable_args[4];
  WORD_LIST *enable_list;

  /* Find our .so file name. */
  if (mpibash_so == NULL) {
    Dl_info self_info;

    if (dladdr(load_mpi_builtin, &self_info) == 0 || self_info.dli_fname == NULL) {
      fprintf(stderr, _("mpi_init: failed to find the MPI-Bash .so file\n"));
      return EXECUTION_FAILURE;
    }
    mpibash_so = strdup(self_info.dli_fname);
  }

  /* Enable the given MPI-Bash function. */
  return mpibash_invoke_bash_command("enable", "-f", mpibash_so, name, NULL);
}

/* Initialize MPI with MPI_Init().  This has never worked for me with
 * Open MPI so we provide a hack in which the user can set
 * LD_PRELOAD=mpibash.so in advance of running bash. */
static int
mpi_init_builtin (WORD_LIST *list)
{
  int inited;
  char **func;

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

  /* Make MPI errors return instead of crash.  Also, store our rank
   * and number of ranks. */
  MPI_Errhandler_set (MPI_COMM_WORLD, MPI_ERRORS_RETURN);
  MPI_Comm_rank (MPI_COMM_WORLD, &mpibash_rank);
  MPI_Comm_size (MPI_COMM_WORLD, &mpibash_num_ranks);

  /* As a convenience for the user, load all of the other MPI-Bash builtins. */
  for (func = all_mpibash_builtins; *func; func++)
    if (load_mpi_builtin(*func) != EXECUTION_SUCCESS)
      return EXECUTION_FAILURE;

  /* The LD_PRELOAD of mpibash.so seems to mess up ordinary commands we try to
   * run.  Remove that variable from the environment. */
  if (mpibash_invoke_bash_command("unset", "LD_PRELOAD", NULL) != EXECUTION_SUCCESS)
    return EXECUTION_FAILURE;
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_init builtin. */
static char *mpi_init_doc[] = {
  "Initialize MPI and MPI-Bash.",
  "",
  "Invoke MPI_Init() then load all of the other MPI-Bash builtins.",
  "",
  "Exit Status:",
  "Returns success if MPI successfully initialized and all MPI-Bash",
  "builtins were loaded into the shell.",
  NULL
};

/* Describe the mpi_init builtin. */
DEFINE_BUILTIN(mpi_init, "mpi_init");

/* Finalize MPI with MPI_Finalize(), but only if we previously invoked
 * MPI_Init() explicitly. */
static int
mpi_finalize_builtin (WORD_LIST *list)
{
  no_args(list);
  if (we_called_init)
    if (MPI_Finalize() != MPI_SUCCESS)
      return EXECUTION_FAILURE;
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_finalize builtin. */
static char *mpi_finalize_doc[] = {
  "Finalize MPI and MPI-Bash.",
  "",
  "Invoke MPI_Finalize().",
  "",
  "Exit Status:",
  "Always succeeds.  However, the MPI standard does not define what",
  "happens after MPI_Finalize() is called.  Consequently, the shell or",
  "shell script should exit immediately after mpi_finalize returns.",
  NULL
};

/* Describe the mpi_finalize builtin. */
DEFINE_BUILTIN(mpi_finalize, "mpi_finalize");

/* Return the caller's MPI rank. */
static int
mpi_comm_rank_builtin (WORD_LIST *list)
{
  char *varname;         /* Name of the variable to bind the results to */

  YES_ARGS(list);
  varname = list->word->word;
  REQUIRE_WRITABLE(varname);
  list = list->next;
  no_args(list);
  mpibash_bind_variable_number(varname, mpibash_rank, 0);
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_comm_rank builtin. */
static char *mpi_comm_rank_doc[] = {
  "Return the process's rank in the MPI job.",
  "",
  "Arguments:",
  "  NAME          Scalar variable in which to receive the rank",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given.",
  NULL
};

/* Describe the mpi_comm_rank builtin. */
DEFINE_BUILTIN(mpi_comm_rank, "mpi_comm_rank name");

/* Return the number of MPI ranks available. */
int
mpi_comm_size_builtin (WORD_LIST *list)
{
  char *varname;         /* Name of the variable to bind the results to */

  YES_ARGS (list);
  varname = list->word->word;
  REQUIRE_WRITABLE(varname);
  list = list->next;
  no_args(list);
  mpibash_bind_variable_number(varname, mpibash_num_ranks, 0);
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_comm_size builtin. */
static char *mpi_comm_size_doc[] = {
  "Return the total number of ranks in the MPI job.",
  "",
  "Arguments:",
  "  NAME          Scalar variable in which to receive the number of ranks",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given.",
  NULL
};

/* Describe the mpi_comm_size builtin. */
DEFINE_BUILTIN(mpi_comm_size, "mpi_comm_size name");

/* Abort the program, typically bringing down all ranks. */
int
mpi_abort_builtin (WORD_LIST *list)
{
  int exit_value;

  exit_value = (running_trap == 1 && list == 0) ? trap_saved_exit_value : get_exitstat(list);  /* Copied from exit.def */
  MPI_TRY(MPI_Abort(MPI_COMM_WORLD, exit_value));
  return EXECUTION_FAILURE;
}

/* Define the documentation for the mpi_abort builtin. */
static char *mpi_abort_doc[] = {
  "Abort all processes in the MPI job and exit the shell.",
  "",
  "Exits not only the caller's shell (with a status of N) but also all",
  "remote shells that are part of the same MPI job.  If N is omitted, the",
  "exit status is that of the last command executed.",
  "",
  "This command should be used only in extreme circumstances.  It is",
  "better for each process to exit normally on its own.",
  NULL
};

/* Describe the mpi_abort builtin. */
DEFINE_BUILTIN(mpi_abort, "mpi_abort [n]");
