/*******************************************
 * Circle-Bash checkpoin/restart functions *
 *                                         *
 * By Scott Pakin <pakin@lanl.gov>         *
 *******************************************/

#include "circlebash.h"

/* Checkpoint queue state to disk. */
static int
circle_checkpoint_builtin (WORD_LIST *list)
{
  no_args(list);
  CIRCLE_checkpoint();
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_checkpoint builtin. */
static char *circle_checkpoint_doc[] = {
  "Checkpoint a work queue to disk.",
  "",
  "Write a file called circle${circle_rank}.txt containing the current",
  "queue state of rank ${circle_rank}.  On a later run, a worker can",
  "invoke circle_read_restarts to repopulate its queue from such a",
  "checkpoint file.",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
  NULL
};

/* Describe the circle_checkpoint builtin. */
DEFINE_BUILTIN(circle_checkpoint, "circle_checkpoint");

/* Reload queue state from disk. */
static int
circle_read_restarts_builtin (WORD_LIST *list)
{
  no_args(list);
  CIRCLE_read_restarts();
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_read_restarts builtin. */
static char *circle_read_restarts_doc[] = {
  "Repopulate a work queue from a disk checkpoint.",
  "",
  "Read queue contents from a file called circle${circle_rank}.txt, which",
  "was previously produced by circle_checkpoint.",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
  NULL
};

/* Describe the circle_read_restarts builtin. */
DEFINE_BUILTIN(circle_read_restarts, "circle_read_restarts");
