/*************************************
 * MPI-Bash point-to-point functions *
 *                                   *
 * By Scott Pakin <pakin@lanl.gov>   *
 *************************************/

#include "mpibash.h"

/* Send a message to another MPI rank. */
int
mpi_send_builtin (WORD_LIST * list)
{
  char *word;                   /* One argument */
  intmax_t target_rank;         /* MPI target rank */
  char *message;                /* Message to send to rank target_rank */
  intmax_t tag = 0;             /* Message tag to use */

  /* Parse "-t TAG" (optional), where TAG is a number or "any". */
  YES_ARGS(list);
  word = list->word->word;
  if (ISOPTION(word, 't')) {
    list = list->next;
    if (list == 0) {
      sh_needarg("mpi_send");
      return EX_USAGE;
    }
    word = list->word->word;
    if (!legal_number(word, &tag)) {
      sh_neednumarg("-t");
      return EX_USAGE;
    }
    list = list->next;
  }
  else if (*word == '-') {
    sh_invalidopt(word);
    builtin_usage();
    return EX_USAGE;
  }

  /* Parse the target rank, which must be a number. */
  YES_ARGS(list);
  word = list->word->word;
  if (!legal_number(word, &target_rank)) {
    builtin_error(_("mpi_send: numeric rank required"));
    return EX_USAGE;
  }
  list = list->next;

  /* Parse the message to send. */
  YES_ARGS(list);
  message = list->word->word;
  list = list->next;
  no_args(list);

  /* Send the message. */
  MPI_TRY(MPI_Send(message, strlen(message) + 1, MPI_BYTE, (int)target_rank,
                   (int)tag, MPI_COMM_WORLD));
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_send builtin. */
static char *mpi_send_doc[] = {
  "Send a message to a remote process in the same MPI job.",
  "",
  "Options:",
  "  -t TAG        Send the message using tag TAG (default: 0).  TAG must",
  "                be a nonnegative integer.",
  "",
  "Arguments:",
  "  RANK          Whom to send the message to.  RANK must be an integer in",
  "                the range [0, $(mpi_comm_size)-1].",
  "",
  "  MESSAGE       String to send to rank RANK.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given or an error occurs.",
  NULL
};

/* Describe the mpi_send builtin. */
DEFINE_BUILTIN(mpi_send, "mpi_send [-t tag] rank message");

/* Receive a message from another MPI rank. */
int
mpi_recv_builtin(WORD_LIST *list)
{
  char *word;                   /* One argument */
  intmax_t source_rank;         /* MPI source rank */
  char *endptr;                 /* Used for parsing strings into numbers */
  MPI_Status status;            /* Status of an MPI operation */
  int count;                    /* Message length in bytes */
  intmax_t tag = 0;             /* Message tag to use */
  char *varname;                /* Name of the variable to bind the results to */
  static char *message = NULL;  /* Message received from MPI */
  static size_t alloced = 0;    /* Number of bytes allocated for the above */
  int opt;                      /* Parsed option */

  /* Parse any options provided. */
  reset_internal_getopt();
  while ((opt = internal_getopt(list, "t:")) != -1) {
    switch (opt) {
      case 't':
        if (!strcmp(list_optarg, "any"))
          tag = MPI_ANY_TAG;
        else if (!legal_number(list_optarg, &tag)) {
          builtin_error(_("-t: numeric argument or \"any\" required"));
          return (EX_USAGE);
        }
        break;

      default:
        sh_invalidopt(word);
        builtin_usage();
        return (EX_USAGE);
    }
  }
  list = loptend;

  /* Parse the source rank, which must be a number or "any". */
  YES_ARGS(list);
  word = list->word->word;
  if (!legal_number(word, &source_rank)) {
    if (!strcmp(word, "any"))
      source_rank = MPI_ANY_SOURCE;
    else {
      builtin_error(_("mpi_recv: numeric rank or \"any\" required"));
      return (EX_USAGE);
    }
  }
  list = list->next;

  /* Parse the target variable, which must not be read-only. */
  YES_ARGS(list);
  varname = list->word->word;
  REQUIRE_WRITABLE(varname);
  list = list->next;
  no_args(list);

  /* Receive a message.  Because we don't know long the message will
   * be, we first probe to get the length. */
  MPI_TRY(MPI_Probe((int) source_rank, (int) tag, MPI_COMM_WORLD, &status));
  MPI_TRY(MPI_Get_count(&status, MPI_BYTE, &count));
  if (alloced < count) {
    message = xrealloc(message, count);
    alloced = count;
  }
  MPI_TRY(MPI_Recv(message, count, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG,
                   MPI_COMM_WORLD, &status));
  bind_array_variable(varname, 0, message, 0);
  mpibash_bind_array_variable_number(varname, 1, status.MPI_SOURCE, 0);
  mpibash_bind_array_variable_number(varname, 2, status.MPI_TAG, 0);
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the mpi_recv builtin. */
static char *mpi_recv_doc[] = {
  "Receive a message from a remote process in the same MPI job.",
  "",
  "Options:",
  "  -t TAG        Receive only messages sent using tag TAG (default: 0).",
  "                TAG must be either a nonnegative integer or the string",
  "                \"any\" to receive messages sent using any tag.",
  "",
  "Arguments:",
  "  RANK          Receive only messages sent from sender RANK.  RANK",
  "                must either be in the range [0, $(mpi_comm_size)-1] or",
  "                be the string \"any\" to receive messages from any sender.",
  "",
  "  NAME          Array variable in which to receive the message, sender",
  "                rank, and tag.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given or an error occurs.",
  NULL
};

/* Describe the mpi_recv builtin. */
DEFINE_BUILTIN(mpi_recv, "mpi_recv [-t tag] rank name");
