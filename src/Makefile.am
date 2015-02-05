###################################
# Build MPI-Bash                  #
#                                 #
# By Scott Pakin <pakin@lanl.gov> #
###################################

plugin_LTLIBRARIES = mpibash.la

mpibash_la_SOURCES = \
	mpibash.h \
	init.c \
	util.c \
	pt2pt.c \
	coll.c
mpibash_la_CPPFLAGS = $(BASH_CPPFLAGS)
mpibash_la_LDFLAGS = -no-undefined -module -avoid-version

# Because mpibash.so is a module, not an ordinary shared library one
# explicitly links against, the Libtool .la file is, as far as I can
# tell, useless.  Remove it.
install-data-hook:
	$(RM) "$(DESTDIR)$(plugindir)/mpibash.la"

uninstall-hook:
	$(RM) "$(DESTDIR)$(plugindir)/mpibash.so"
	-rmdir "$(DESTDIR)$(plugindir)"
