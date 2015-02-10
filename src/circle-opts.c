/****************************************
 * Circle-Bash option-setting functions *
 *                                      *
 * By Scott Pakin <pakin@lanl.gov>      *
 ****************************************/

#include "mpibash.h"
#include <libcircle.h>

/* Set Libcircle options. */
int
circle_set_options_builtin(WORD_LIST * list)
{
  char *word;        /* One argument */
  int flags = 0;     /* Flags to pass to CIRCLE_set_options */

  if (list == NULL)
    flags = CIRCLE_DEFAULT_FLAGS;
  else
    while (list != NULL) {
      word = list->word->word;
      if (!strcmp(word, "split_random"))
        flags |= CIRCLE_SPLIT_RANDOM;
      else if (!strcmp(word, "split_equal"))
        flags |= CIRCLE_SPLIT_EQUAL;
      else if (!strcmp(word, "create_global"))
        flags |= CIRCLE_CREATE_GLOBAL;
      else {
        builtin_error(_("invalid flag \"%s\""), word);
        return (EXECUTION_FAILURE);
      }
      list = list->next;
    }
  CIRCLE_set_options(flags);
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_set_options builtin. */
static char *circle_set_options_doc[] = {
  "Change Libcircle's run-time behavior.",
  "",
  "Arguments:",
  "  FLAG          \"split_random\", \"split_equal\", or \"create_global\"",
  "",
  "Multiple flags can be provided (space-separated).  If no flags are",
  "provided, Libcircle reverts to its default options.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given.",
  NULL
};

/* Describe the circle_set_options builtin. */
DEFINE_BUILTIN(circle_set_options, "circle_set_options [flag...]");

/* Alter Libcircle's logging verbosity. */
int
circle_enable_logging_builtin (WORD_LIST *list)
{
  char *word;                   /* One argument */
  CIRCLE_loglevel loglevel;     /* Level to set */

  /* Parse the log level. */
  YES_ARGS(list);
  word = list->word->word;
  if (!strcmp(word, "fatal"))
    loglevel = CIRCLE_LOG_FATAL;
  else if (!strcmp(word, "error"))
    loglevel = CIRCLE_LOG_ERR;
  else if (!strcmp(word, "warning"))
    loglevel = CIRCLE_LOG_WARN;
  else if (!strcmp(word, "info"))
    loglevel = CIRCLE_LOG_INFO;
  else if (!strcmp(word, "debug"))
    loglevel = CIRCLE_LOG_DBG;
  else {
    builtin_error(_("invalid log level \"%s\""), word);
    return (EXECUTION_FAILURE);
  }

  /* Set the log level. */
  CIRCLE_enable_logging(loglevel);
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_enable_logging builtin. */
static char *circle_enable_logging_doc[] = {
  "Change Libcircle's logging verbosity",
  "",
  "Arguments:",
  "  LOG_LEVEL     \"fatal\", \"error\", \"warning\", \"info\", or \"debug\"",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid option is given.",
  NULL
};

/* Describe the circle_enable_logging builtin. */
DEFINE_BUILTIN(circle_enable_logging, "circle_enable_logging log_level");
