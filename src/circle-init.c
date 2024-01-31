/*********************************************************
 * Circle-Bash initialization and finalization functions *
 *                                                       *
 * By Scott Pakin <pakin@lanl.gov>                       *
 *********************************************************/

#include "circlebash.h"
#include "execute_cmd.h"

/* Define our library-local variables in an ad hoc namespace. */
SHELL_VAR *circlebash_create_func = NULL;   /* User-defined callback function for CIRCLE_cb_create. */
SHELL_VAR *circlebash_process_func = NULL;  /* User-defined callback function for CIRCLE_cb_process. */
CIRCLE_handle *circlebash_current_handle = NULL;    /* Active handle within a callback or NULL if not within a callback */
SHELL_VAR *circlebash_reduce_init_func = NULL;  /* User-defined callback function for CIRCLE_cb_reduce_init. */
SHELL_VAR *circlebash_reduce_op_func = NULL;   /* User-defined callback function for CIRCLE_cb_reduce_op. */
SHELL_VAR *circlebash_reduce_fini_func = NULL;  /* User-defined callback function for CIRCLE_cb_reduce_fini. */
int circlebash_within_reduction = 0;           /* 1=within a reduction callback; 0=not */

/* Define all of our file-local variables as statics. */
static char *all_circle_builtins[] = {  /* All builtins that Circle-Bash defines except circle_init */
  "circle_abort",
  "circle_begin",
  "circle_cb_create",
  "circle_cb_process",
  "circle_cb_reduce_fini",
  "circle_cb_reduce_init",
  "circle_cb_reduce_op",
  "circle_checkpoint",
  "circle_dequeue",
  "circle_enable_logging",
  "circle_enqueue",
  "circle_finalize",
  "circle_read_restarts",
  "circle_reduce",
  "circle_set_options",
  NULL
};

/* Invoke the user-defined creation-callback function (circlebash_create_func). */
static void
internal_create_func (CIRCLE_handle * handle)
{
  WORD_LIST *funcargs;

  if (circlebash_create_func == NULL)
    return;
  circlebash_current_handle = handle;
  funcargs = make_word_list(make_word("cb_create"), NULL);
  execute_shell_function(circlebash_create_func, funcargs);
  dispose_words(funcargs);
  circlebash_current_handle = NULL;
}

/* Invoke the user-defined process-callback function (circlebash_process_func). */
static void
internal_process_func (CIRCLE_handle * handle)
{
  WORD_LIST *funcargs;

  if (circlebash_process_func == NULL)
    return;
  circlebash_current_handle = handle;
  funcargs = make_word_list(make_word("cb_process"), NULL);
  execute_shell_function(circlebash_process_func, funcargs);
  dispose_words(funcargs);
  circlebash_current_handle = NULL;
}

/* Invoke the user-defined reduction-initiation callback function
 * (circlebash_reduce_init_func). */
static void
internal_reduce_init_func (void)
{
  WORD_LIST *funcargs;

  if (circlebash_reduce_init_func == NULL)
    return;
  circlebash_within_reduction = 1;
  funcargs = make_word_list(make_word("cb_reduce_init"), NULL);
  execute_shell_function(circlebash_reduce_init_func, funcargs);
  dispose_words(funcargs);
  circlebash_within_reduction = 0;
}

/* Invoke the user-defined reduction callback function
 * (circlebash_reduce_op_func). */
static void
internal_reduce_op_func (buf1, size1, buf2, size2)
     const void *buf1;
     size_t size1;
     const void *buf2;
     size_t size2;
{
  WORD_LIST *funcargs;

  if (circlebash_reduce_op_func == NULL)
    return;
  circlebash_within_reduction = 1;
  funcargs = make_word_list(make_word(buf2), NULL);
  funcargs = make_word_list(make_word(buf1), funcargs);
  funcargs = make_word_list(make_word("cb_reduce_op"), funcargs);
  execute_shell_function(circlebash_reduce_op_func, funcargs);
  dispose_words(funcargs);
  circlebash_within_reduction = 0;
}

/* Invoke the user-defined reduction-finalization callback function
 * (circlebash_reduce_fini_func). */
static void
internal_reduce_fini_func (const void *buf, size_t size)
{
  WORD_LIST *funcargs;

  if (circlebash_reduce_fini_func == NULL)
    return;
  funcargs = make_word_list(make_word(buf), NULL);
  funcargs = make_word_list(make_word("cb_reduce_fini"), funcargs);
  execute_shell_function(circlebash_reduce_fini_func, funcargs);
  dispose_words(funcargs);
}

/* Load another builtin from our plugin by invoking "enable -f
 * circlebash.so <name>". */
static int
load_circle_builtin (char *name)
{
  static char *circlebash_so = NULL;   /* Path to circlebash.so */

  /* Find our .so file name. */
  if (circlebash_so == NULL) {
    Dl_info self_info;

    if (dladdr(load_circle_builtin, &self_info) == 0 || self_info.dli_fname == NULL) {
      fprintf(stderr, _("circle_init: failed to find the Circle-Bash .so file\n"));
      return EXECUTION_FAILURE;
    }
    circlebash_so = strdup(self_info.dli_fname);
  }

  /* Enable the given Circle-Bash function. */
  return mpibash_invoke_bash_command("enable", "-f", circlebash_so, name, NULL);
}

/* Initialize Circle-Bash. */
static int
circle_init_builtin (WORD_LIST *list)
{
  char *marg = "mpibash";
  char **margv = &marg;
  int margc = 1;
  int circle_rank;              /* Rank in the Libcircle job */
  char **func;

  /* Initialize Libcircle. */
  no_args(list);
  circle_rank = CIRCLE_init(margc, margv, CIRCLE_DEFAULT_FLAGS);
  mpibash_bind_variable_number("circle_rank", circle_rank, 0);
  CIRCLE_enable_logging(CIRCLE_LOG_WARN);

  /* Register internal callbacks with Libcircle.  These will in turn invoke
   * user-specified bash functions. */
  CIRCLE_cb_create(internal_create_func);
  CIRCLE_cb_process(internal_process_func);
  CIRCLE_cb_reduce_init(internal_reduce_init_func);
  CIRCLE_cb_reduce_op(internal_reduce_op_func);
  CIRCLE_cb_reduce_fini(internal_reduce_fini_func);

  /* As a convenience for the user, load all of the other MPI-Bash builtins. */
  for (func = all_circle_builtins; *func; func++)
    if (load_circle_builtin(*func) != EXECUTION_SUCCESS)
      return EXECUTION_FAILURE;

  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_init builtin. */
static char *circle_init_doc[] = {
  "Initialize Circle-Bash.",
  "",
  "Invoke CIRCLE_init() then load all of the other Circle-Bash builtins.",
  "",
  "Exit Status:",
  "Returns success if Libcircle successfully initialized and all Libcircle",
  "builtins were loaded into the shell.",
  NULL
};

/* Describe the circle_init builtin. */
DEFINE_BUILTIN(circle_init, "circle_init");


/* Finalize Circle-Bash. */
static int
circle_finalize_builtin (WORD_LIST *list)
{
  no_args(list);
  CIRCLE_finalize();
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_finalize builtin. */
static char *circle_finalize_doc[] = {
  "Finalize Circle-Bash.",
  "",
  "Invoke CIRCLE_finalize().",
  "",
  "Exit Status:",
  "Always succeeds.",
  NULL
};

/* Describe the circle_finalize builtin. */
DEFINE_BUILTIN(circle_finalize, "circle_finalize");

/* Abort Circle-Bash. */
int
circle_abort_builtin (WORD_LIST *list)
{
  no_args(list);
  CIRCLE_abort();
  return EXECUTION_SUCCESS;
}

/* Define the documentation for the circle_abort builtin. */
static char *circle_abort_doc[] = {
  "Terminate queue processing.",
  "",
  "Exit Status:",
  "Returns 0 unless an error occurs.",
  NULL
};

/* Describe the circle_abort builtin. */
DEFINE_BUILTIN(circle_abort, "circle_abort");
