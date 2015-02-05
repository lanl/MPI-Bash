/***********************************
 * MPI-Bash collective functions   *
 *                                 *
 * By Scott Pakin <pakin@lanl.gov> *
 ***********************************/

#include "mpibash.h"

/* Synchronize all of the MPI ranks. */
static int
mpi_barrier_builtin (WORD_LIST * list)
{
  no_args(list);
  MPI_TRY(MPI_Barrier(MPI_COMM_WORLD));
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_barrier builtin. */
static char *mpi_barrier_doc[] = {
  "Synchronizes all of the processes in the MPI job.",
  "",
  "No process will return from mpi_barrier until all processes have",
  "called mpi_barrier.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given or an error occurs.",
  NULL
};

/* Describe the mpi_barrier builtin. */
DEFINE_BUILTIN(mpi_barrier, "mpi_barrier");

/* Broadcast a message from one rank to all the others. */
static int
mpi_bcast_builtin (WORD_LIST *list)
{
  char *word;                   /* One argument */
  int root;                     /* MPI root rank */
  char *root_message;           /* Message to broadcast */
  int msglen;                   /* Length in bytes of the above (including the NULL byte) */
  char *varname;                /* Name of the variable to bind the results to */
  static int *all_lengths = NULL;       /* List of every rank's msglen */
  static char *message = NULL;  /* Message received from the root */
  static int alloced = 0;       /* Bytes allocated for the above */
  int i;

  /* Parse the optional message and target variable, which must not be
   * read-only. */
  YES_ARGS(list);
  if (list->next == NULL) {
    /* Non-root */
    root_message = NULL;
    msglen = -1;
  }
  else {
    /* Root */
    root_message = list->word->word;
    msglen = (int) strlen(root_message) + 1;
    list = list->next;
  }
  varname = list->word->word;
  REQUIRE_WRITABLE(varname);
  list = list->next;
  no_args(list);

  /* Acquire global agreement on the root and the message size. */
  if (all_lengths == NULL)
    all_lengths = xmalloc(mpibash_num_ranks * sizeof(int));
  MPI_TRY(MPI_Allgather(&msglen, 1, MPI_INT, all_lengths, 1, MPI_INT, MPI_COMM_WORLD));
  root = -1;
  for (i = 0; i < mpibash_num_ranks; i++) {
    if (all_lengths[i] == -1)
      continue;
    if (root != -1) {
      builtin_error(_
                    ("mpi_bcast: more than one process specified a message"));
      return (EXECUTION_FAILURE);
    }
    root = i;
    msglen = all_lengths[i];
  }
  if (root == -1) {
    builtin_error(_("mpi_bcast: no process specified a message"));
    return (EXECUTION_FAILURE);
  }

  /* Broadcast the message. */
  if (mpibash_rank == root) {
    MPI_TRY(MPI_Bcast(root_message, msglen, MPI_BYTE, root, MPI_COMM_WORLD));
    bind_variable(varname, root_message, 0);
  }
  else {
    if (alloced < msglen) {
      message = xrealloc(message, msglen);
      alloced = msglen;
    }
    MPI_TRY(MPI_Bcast(message, msglen, MPI_BYTE, root, MPI_COMM_WORLD));
    bind_variable(varname, message, 0);
  }
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_bcast builtin. */
static char *mpi_bcast_doc[] = {
  "Broadcast a message to all processes in the same MPI job.",
  "",
  "Arguments:",
  "  MESSAGE       String to broadcast from one process to all the others.",
  "",
  "  NAME          Scalar variable in which to receive the broadcast message.",
  "",
  "Exactly one process in the MPI job must specify a message to",
  "broadcast.  No process will return from mpi_bcast until all processes",
  "have called mpi_bcast.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given or an error occurs.",
  NULL
};

/* Describe the mpi_bcast builtin. */
DEFINE_BUILTIN(mpi_bcast, "mpi_bcast [message] name");
