###################################
# Build MPI-Bash                  #
#                                 #
# By Scott Pakin <pakin@lanl.gov> #
###################################

plugin_LTLIBRARIES = mpibash.la
if HAVE_LIBCIRCLE
  plugin_LTLIBRARIES += circlebash.la
endif

mpibash_la_SOURCES = \
	mpibash.h \
	init.c \
	util.c \
	pt2pt.c \
	coll.c
mpibash_la_CPPFLAGS = $(BASH_CPPFLAGS)
mpibash_la_LDFLAGS = -module -avoid-version

circlebash_la_SOURCES = \
	mpibash.h \
	circle-init.c \
	util.c
circlebash_la_CPPFLAGS = $(BASH_CPPFLAGS)
circlebash_la_CFLAGS = $(CIRCLE_CFLAGS)
circlebash_la_LDFLAGS = -module -avoid-version
circlebash_la_LIBADD = $(CIRCLE_LIBS)

# Because mpibash.so is a module, not an ordinary shared library one
# explicitly links against, the Libtool .la file is, as far as I can
# tell, useless.  Remove it.
install-data-hook:
	$(RM) "$(DESTDIR)$(plugindir)/mpibash.la"
	$(RM) "$(DESTDIR)$(plugindir)/circlebash.la"

uninstall-hook:
	$(RM) "$(DESTDIR)$(plugindir)/mpibash.so"
	$(RM) "$(DESTDIR)$(plugindir)/circlebash.so"
	-rmdir "$(DESTDIR)$(plugindir)"
