/*******************************************
 * Definitions used throughout Circle-Bash *
 *                                         *
 * By Scott Pakin <pakin@lanl.gov>         *
 *******************************************/

#ifndef _CIRCLEBASH_H_
#define _CIRCLEBASH_H_

#include "mpibash.h"
#include <libcircle.h>

extern SHELL_VAR *circlebash_create_func;   /* User-defined callback function for CIRCLE_cb_create. */
extern SHELL_VAR *circlebash_process_func;  /* User-defined callback function for CIRCLE_cb_process. */
extern CIRCLE_handle *circlebash_current_handle;    /* Active handle within a callback or NULL if not within a callback */

#endif
