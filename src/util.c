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
