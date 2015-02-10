/****************************************
 * Circle-Bash main queueing functions  *
 *                                      *
 * By Scott Pakin <pakin@lanl.gov>      *
 ****************************************/

#include "circlebash.h"

/* Look up a user-provided callback function. */
static int
find_callback_function (WORD_LIST *list, SHELL_VAR **user_func)
{
  char *funcname;     /* Name of the user-defined function. */

  /* If no argument was provided, nullify the callback function. */
  if (list == NULL) {
    *user_func = NULL;
    return EXECUTION_SUCCESS;
  }

  /* Get the callback function. */
  funcname = list->word->word;
  list = list->next;
  no_args(list);
  *user_func = find_function(funcname);
  if (*user_func == NULL) {
    builtin_error(_("function %s not found"), funcname);
    return EXECUTION_FAILURE;
  }
  return EXECUTION_SUCCESS;
}

/* Register a callback for populating the distributed queue. */
static int
circle_cb_create_builtin (WORD_LIST *list)
{
  return find_callback_function(list, &circlebash_create_func);
}

/* Define the documentation for the circle_cb_create builtin. */
static char *circle_cb_create_doc[] = {
  "Register a function that will create work when asked.",
  "",
  "Arguments:",
  "  FUNC          User-defined callback function that will invoke",
  "                circle_enqueue when called",
  "",
  "If FUNC is omitted, no function will be associated with work creation.",
  "This can be used to nullify a previous circle_cb_create invocation.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid function is given or an error occurs.",
  NULL,
};

/* Describe the circle_cb_create builtin. */
DEFINE_BUILTIN(circle_cb_create, "circle_cb_create [func]");

/* Register a callback for processing a work item from the distributed queue. */
static int
circle_cb_process_builtin (WORD_LIST *list)
{
  return find_callback_function(list, &circlebash_process_func);
}

/* Define the documentation for the circle_cb_process builtin. */
static char *circle_cb_process_doc[] = {
  "Register a function that will process work when asked.",
  "",
  "Arguments:",
  "  FUNC          User-defined callback function that will invoke",
  "                circle_enqueue when called",
  "",
  "If FUNC is omitted, no function will be associated with work processing.",
  "This can be used to nullify a previous circle_cb_process invocation.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid function is given or an error occurs.",
  NULL,
};

/* Describe the circle_cb_process builtin. */
DEFINE_BUILTIN(circle_cb_process, "circle_cb_process [func]");

/* Process work items until the distributed queue is empty. */
static int
circle_begin_builtin (WORD_LIST *list)
{
  no_args(list);
  CIRCLE_begin();
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_begin builtin. */
static char *circle_begin_doc[] = {
  "Begin creation and processing of the distributed work queue.",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
NULL
};

/* Describe the circle_begin builtin. */
DEFINE_BUILTIN(circle_begin, "circle_begin");

/* Enqueue a work item onto the distributed queue. */
static int
circle_enqueue_builtin (WORD_LIST *list)
{
  char *work;                   /* Work to perform */

  /* Extract the work argument. */
  YES_ARGS(list);
  work = list->word->word;
  list = list->next;
  no_args(list);

  /* Complain if we're not within a proper callback function. */
  if (circlebash_current_handle == NULL) {
    builtin_error(_
                  ("not within a Libcircle \"create\" or \"process\" callback function"));
    return EXECUTION_FAILURE;
  }

  /* Enqueue the work. */
  if (circlebash_current_handle->enqueue(work) == -1)
    return EXECUTION_FAILURE;
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_enqueue builtin. */
static char *circle_enqueue_doc[] = {
  "Enqueue work onto the distributed queue.",
  "",
  "Arguments:",
  "  WORK          \"Work\" as represented by an arbitrary string of limited",
  "                size (generally around 4KB)",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
  NULL
};

/* Describe the circle_enqueue builtin. */
DEFINE_BUILTIN(circle_enqueue, "circle_enqueue work");

/* Dequeue a work item from the distributed queue. */
static int
circle_dequeue_builtin (WORD_LIST *list)
{
  char *varname;                /* Variable in which to store the work string */
  char work[CIRCLE_MAX_STRING_LEN + 1]; /* Work to perform */

  /* Extract the variable-name argument. */
  YES_ARGS(list);
  varname = list->word->word;
  list = list->next;
  no_args(list);

  /* Complain if we're not within a callback function. */
  if (circlebash_current_handle == NULL) {
    builtin_error(_("not within a Libcircle callback function"));
    return EXECUTION_FAILURE;
  }

  /* Dequeue the work and bind it to the given variable. */
  if (circlebash_current_handle->dequeue(work) == -1)
    return EXECUTION_FAILURE;
  bind_variable(varname, work, 0);
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_dequeue builtin. */
static char *circle_dequeue_doc[] = {
  "Dequeue work from the distributed queue into a variable.",
  "",
  "Arguments:",
  "  VAR           Variable in which to receive previously enqueued \"work\"",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
  NULL
};

/* Describe the circle_dequeue builtin. */
DEFINE_BUILTIN(circle_dequeue, "circle_dequeue var");
