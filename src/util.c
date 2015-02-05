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
