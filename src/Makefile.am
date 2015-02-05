###################################
# Build MPI-Bash                  #
#                                 #
# By Scott Pakin <pakin@lanl.gov> #
###################################

plugindir = $(libexecdir)/mpibash
plugin_LTLIBRARIES = mpibash.la preload-mpi.la

mpibash_la_SOURCES = \
	mpibash.h \
	init.c \
	util.c \
	pt2pt.c \
	coll.c
mpibash_la_CPPFLAGS = $(BASH_CPPFLAGS)
mpibash_la_LDFLAGS = -no-undefined -module -avoid-version

preload_mpi_la_SOURCES = preload-mpi.c
preload_mpi_la_CPPFLAGS = $(BASH_CPPFLAGS)
preload_mpi_la_LDFLAGS = -no-undefined -module -avoid-version

# Because mpibash.so and preload_mpi.so are modules, not ordinary
# shared libraries one explicitly links against, the Libtool .la files
# are, as far as I can tell, useless.  Remove them.
install-data-hook:
	$(RM) "$(DESTDIR)$(plugindir)/mpibash.la"
	$(RM) "$(DESTDIR)$(plugindir)/preload-mpi.la"

uninstall-hook:
	$(RM) "$(DESTDIR)$(plugindir)/mpibash.so"
	$(RM) "$(DESTDIR)$(plugindir)/preload-mpi.so"
	-rmdir "$(DESTDIR)$(plugindir)"
