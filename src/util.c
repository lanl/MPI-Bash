/***********************************
 * Utility functions called	   *
 * throughout MPI-Bash		   *
 *				   *
 * By Scott Pakin <pakin@lanl.gov> *
 ***********************************/

#include "mpibash.h"

/* Perform the same operation as bind_variable, but with VALUE being a
 * number, not a string. */
SHELL_VAR *
mpibash_bind_variable_number (const char *name, long value, int flags)
{
  char numstr[25];    /* String version of VALUE */

  sprintf (numstr, "%ld", value);
  return bind_variable (name, numstr, flags);
}

/* Report an error to the user and return EXECUTION_FAILURE. */
int
mpibash_report_mpi_error (int mpierr)
{
  char errstr[MPI_MAX_ERROR_STRING];
  int errstrlen;

  MPI_Error_string(mpierr, errstr, &errstrlen);
  builtin_error("%s", errstr);
  return EXECUTION_FAILURE;
}

/* Perform the same operation as bind_array_variable, but with VALUE
 * being a number, not a string. */
SHELL_VAR *
mpibash_bind_array_variable_number (char *name, arrayind_t ind, long value, int flags)
{
  char numstr[25];    /* String version of VALUE */

  sprintf (numstr, "%ld", value);
  return bind_array_variable (name, ind, numstr, flags);
}

/* Invoke a bash builtin command (list of tokens ending in NULL).
 * Return EXECUTION_SUCCESS or EXECUTION_FAILURE. */
int
mpibash_invoke_bash_command(char *funcname, ...)
{
  static char *mpibash_so = NULL;       /* Path to mpibash.so */
  sh_builtin_func_t *func;      /* Pointer to the function corresponding to funcname */
  WORD_LIST *arg_list = NULL;   /* Function and arguments. */
  char *one_arg;                /* A single argument to invoke_bash_command */
  va_list args;                 /* All arguments to invoke_bash_command */

  /* Find our .so file name. */
  if (mpibash_so == NULL) {
    Dl_info self_info;

    if (dladdr(mpibash_invoke_bash_command, &self_info) == 0 ||
        self_info.dli_fname == NULL) {
      fprintf(stderr, _("mpi_init: failed to find the MPI-Bash .so file\n"));
      return EXECUTION_FAILURE;
    }
    mpibash_so = strdup(self_info.dli_fname);
  }

  /* Find the given command, even if it's disabled. */
  func = builtin_address(funcname);
  if (func == NULL) {
    fprintf(stderr, _("mpi_init: failed to find the %s builtin\n"), funcname);
    return EXECUTION_FAILURE;
  }

  /* Construct a list of arguments. */
  va_start(args, funcname);
  for (one_arg = va_arg(args, char *); one_arg != NULL;
       one_arg = va_arg(args, char *))
      arg_list = make_word_list(make_bare_word(one_arg), arg_list);
  va_end(args);
  arg_list = REVERSE_LIST(arg_list, WORD_LIST *);

  /* Invoke the specified command with its arguments. */
  if (func(arg_list) == EXECUTION_FAILURE) {
    fprintf(stderr, _("mpi_init: failed to get execute bash function %s\n"),
            funcname);
    dispose_words(arg_list);
    return EXECUTION_FAILURE;
  }
  dispose_words(arg_list);
  return EXECUTION_SUCCESS;
}

/* Look up a user-provided callback function. */
int
mpibash_find_callback_function (WORD_LIST *list, SHELL_VAR **user_func)
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
