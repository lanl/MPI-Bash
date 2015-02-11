/************************************
 * Circle-Bash reduction functions  *
 *                                  *
 * By Scott Pakin <pakin@lanl.gov>  *
 ************************************/

#include "circlebash.h"

/* Specify a callback to invoke a reduction. */
static int
circle_cb_reduce_init_builtin (WORD_LIST *list)
{
  return mpibash_find_callback_function(list, &circlebash_reduce_init_func);
}

/* Define the documentation for the circle_cb_reduce_init builtin. */
static char *circle_cb_reduce_init_doc[] = {
  "Register a function that will initiate a reduction operation.",
  "",
  "Arguments:",
  "  FUNC          User-defined callback function that will invoke",
  "                circle_reduce when called",
  "",
  "FUNC will be invoked on all ranks.",
  "",
  "If FUNC is omitted, no function will be associated with reduction",
  "initialization.  This can be used to nullify a previous",
  "circle_cb_reduce_init invocation.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid function is given or an error occurs.",
  NULL
};

/* Describe the circle_cb_reduce_init builtin. */
DEFINE_BUILTIN(circle_cb_reduce_init, "circle_cb_reduce_init [func]");

/* Specify a function that will reduce two values. */
static int
circle_cb_reduce_op_builtin (WORD_LIST *list)
{
  return mpibash_find_callback_function(list, &circlebash_reduce_op_func);
}

/* Define the documentation for the circle_cb_reduce_op builtin. */
static char *circle_cb_reduce_op_doc[] = {
  "Register a function that will complete a reduction operation.",
  "",
  "Arguments:",
  "  FUNC          User-defined callback function that will receive",
  "                two items to reduce and invoke circle_reduce on",
  "                the reduced value",
  "",
  "If FUNC is omitted, no function will be associated with reduction",
  "execution.  This can be used to nullify a previous circle_cb_reduce_op",
  "invocation.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid function is given or an error occurs.",
  NULL
};

/* Describe the circle_cb_reduce_op builtin. */
DEFINE_BUILTIN(circle_cb_reduce_op, "circle_cb_reduce_op [func]");

/* Specify a function to receive the final reduced value. */
static int
circle_cb_reduce_fini_builtin (WORD_LIST *list)
{
  return mpibash_find_callback_function(list, &circlebash_reduce_fini_func);
}

/* Define the documentation for the circle_cb_reduce_fini builtin. */
static char *circle_cb_reduce_fini_doc[] = {
  "Register a function that will complete a reduction operation.",
  "",
  "Arguments:",
  "  FUNC          User-defined callback function that will receive",
  "                the final reduced data",
  "",
  "If FUNC is omitted, no function will be associated with reduction",
  "completion.  This can be used to nullify a previous",
  "circle_cb_reduce_fini invocation.",
  "",
  "Libcircle guarantees that FUNC will be invoked only on rank 0.",
  "",
  "Exit Status:",
  "Returns 0 unless an invalid function is given or an error occurs.",
  NULL
};

/* Describe the circle_cb_reduce_fini builtin. */
DEFINE_BUILTIN(circle_cb_reduce_fini, "circle_cb_reduce_fini [func]");

/* Inject a value to reduce. */
static int
circle_reduce_builtin (WORD_LIST *list)
{
  char *work;                   /* Work to perform */

  /* Extract the work argument. */
  YES_ARGS(list);
  work = list->word->word;
  list = list->next;
  no_args(list);

  /* Complain if we're not within a proper callback function. */
  if (!circlebash_within_reduction) {
    builtin_error(_
                  ("not within a Libcircle \"reduce_init\" or \"reduce_op\" callback function"));
    return EXECUTION_FAILURE;
  }

  /* Reduce the work. */
  CIRCLE_reduce(work, strlen(work));
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_reduce builtin. */
static char *circle_reduce_doc[] = {
  "Seed the next phase of a reduction operation",
  "",
  "Arguments:",
  "  WORK          \"Work\" as represented by an arbitrary string of limited",
  "                size (generally around 4KB)",
  "",
  "This function should be called both by the callback function",
  "registered with circle_cb_reduce_init and the callback function",
  "registered with circle_reduce_op.",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
  NULL
};

/* Describe the circle_reduce builtin. */
DEFINE_BUILTIN(circle_reduce, "circle_reduce work");
