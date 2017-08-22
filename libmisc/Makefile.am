noinst_LTLIBRARIES = libevmisc.la

libevmisc_la_SOURCES = \
	ev-page-action.c	\
	ev-page-action.h	\
	ev-page-action-widget.c	\
	ev-page-action-widget.h

libevmisc_la_CFLAGS =	\
	-DXREADERDATADIR=\"$(pkgdatadir)\"	\
	-I$(top_srcdir)			\
	$(SHELL_CORE_CFLAGS)    	\
	$(WARNING_CFLAGS)       	\
	$(DISABLE_DEPRECATED)

-include $(top_srcdir)/git.mk
