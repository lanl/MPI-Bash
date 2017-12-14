######################################
# Install MPI-Bash parallel commands #
#                                    #
# By Scott Pakin <pakin@lanl.gov>    #
######################################

bin_SCRIPTS = mbxz
if HAVE_LIBCIRCLE
  bin_SCRIPTS += mbcp mbtar
endif

dist_man1_MANS = mbxz.1
if HAVE_LIBCIRCLE
  dist_man1_MANS += mbcp.1 mbtar.1
endif
