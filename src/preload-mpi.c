/****************************************************
 * Initialize MPI in case bash can't.               *
 * (This program should be loaded with LD_PRELOAD.) *
 *                                                  *
 * By Scott Pakin <pakin@lanl.gov>                  *
 ****************************************************/

#include <stdlib.h>
#include <mpi.h>

__attribute__((constructor))
static void prepare_mpi (void)
{
  char *marg = "preload_mpi";
  char **margv = &marg;
  int margc = 1;

  MPI_Init(&margc, &margv);
  unsetenv("LD_PRELOAD");   /* Our children should not invoke MPI_Init(). */
}

__attribute__((destructor))
static void tear_down_mpi (void)
{
    MPI_Finalize();
}
