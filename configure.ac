# *****************************************************************************
# Versioning
# *****************************************************************************

m4_define([ev_major_version], [1])
m4_define([ev_minor_version], [4])
m4_define([ev_micro_version], [4])
m4_define([ev_extra_version], [])
m4_define([ev_version], [ev_major_version.ev_minor_version.ev_micro_version()ev_extra_version])

# The xreader API version
m4_define([ev_api_version], [1.5.0])

# Libtool versioning. The backend and view libraries have separate versions.
# Before making a release, the libtool version should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A

# Libtool version of the backend library
m4_define([ev_document_lt_current], [3])
m4_define([ev_document_lt_revision], [0])
m4_define([ev_document_lt_age], [0])
m4_define([ev_document_lt_version_info], [ev_document_lt_current:ev_document_lt_revision:ev_document_lt_age])
m4_define([ev_document_lt_current_minus_age], [m4_eval(ev_document_lt_current - ev_document_lt_age)])

# Libtool version of the view library
m4_define([ev_view_lt_current], [3])
m4_define([ev_view_lt_revision], [0])
m4_define([ev_view_lt_age], [0])
m4_define([ev_view_lt_version_info], [ev_view_lt_current:ev_view_lt_revision:ev_view_lt_age])
m4_define([ev_view_lt_current_minus_age], [m4_eval(ev_view_lt_current - ev_view_lt_age)])

# Binary version for the document backends
m4_define([ev_binary_version], [ev_document_lt_current])

# *****************************************************************************

AC_PREREQ([2.57])
AC_INIT([xreader], [ev_version], [https://github.com/linuxmint/xreader/issues], [xreader])
AM_INIT_AUTOMAKE([1.10 foreign tar-ustar dist-xz no-dist-gzip check-news])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

if test -z "$enable_maintainer_mode"; then
    enable_maintainer_mode=yes
fi
AM_MAINTAINER_MODE([enable])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_STDC_HEADERS
AM_PROG_LIBTOOL
AC_PROG_SED
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])

MATE_MAINTAINER_MODE_DEFINES
MATE_COMPILE_WARNINGS
MATE_CXX_WARNINGS

DISABLE_DEPRECATED=
AC_SUBST([DISABLE_DEPRECATED])

IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=xreader
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])
AM_GLIB_GNU_GETTEXT

m4_pattern_allow([AM_V_GEN])dnl Make autoconf not complain about the rule below
EV_INTLTOOL_XREADER_BACKEND_RULE='%.xreader-backend:   %.xreader-backend.in   $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*.po) ; $(AM_V_GEN) LC_ALL=C $(INTLTOOL_MERGE) -d -u -c $(top_builddir)/po/.intltool-merge-cache $(top_srcdir)/po $< [$]@'
AC_SUBST([EV_INTLTOOL_XREADER_BACKEND_RULE])

GLIB_GSETTINGS

dnl Specify required versions of dependencies
CAIRO_REQUIRED=1.14.0
GLIB_REQUIRED=2.36.0
GTK_API_VERSION=3.0
WEBKIT_API_VERSION=4.0
GAIL_API_VERSION=-3.0
GTK_REQUIRED=3.14.0
WEBKIT_REQUIRED=2.4.3
LIBSECRET_REQUIRED=0.5
LIBXML_REQUIRED=2.5.0
LIBEVINCE_REQUIRED=3.18.2

AC_SUBST([GLIB_REQUIRED])
AC_SUBST([GTK_REQUIRED])
AC_SUBST([GTK_API_VERSION])

dnl Check dependencies

# LIB_CFLAGS       for helpers and generic widgets. (libdocument, cut-and-paste)
# BACKEND_CFLAGS   for backend implementations.
# FRONTEND_CFLAGS  for frontend implementations. (properties, thumbnailer)
# FRONTEND_LIBS
# SHELL_CFLAGS     for shell implementation.
# SHELL_LIBS

AC_SEARCH_LIBS([hypot],[m])

# PKG_CHECK_MODULES(LIBDOCUMENT, gtk+-3.0 >= $GTK_REQUIRED gio-2.0 >= $GLIB_REQUIRED)
# PKG_CHECK_MODULES(LIBVIEW, gtk+-3.0 >= $GTK_REQUIRED gail$GAIL_API_VERSION >= $GTK_REQUIRED gthread-2.0 gio-2.0 >= $GLIB_REQUIRED)
PKG_CHECK_MODULES(EVINCE, evince-view-3.0 >= $LIBEVINCE_REQUIRED)
# PKG_CHECK_MODULES(BACKEND, cairo >= $CAIRO_REQUIRED gtk+-3.0 >= $GTK_REQUIRED)
PKG_CHECK_MODULES(FRONTEND_CORE, gtk+-3.0 >= $GTK_REQUIRED gthread-2.0 gio-2.0 >= $GLIB_REQUIRED)
PKG_CHECK_MODULES(GMODULE, gmodule-2.0 >= $GLIB_REQUIRED)

PKG_CHECK_MODULES([SHELL_CORE],[libxml-2.0 >= $LIBXML_REQUIRED gtk+-3.0 >= $GTK_REQUIRED gio-2.0 >= $GLIB_REQUIRED gthread-2.0 x11 evince-view-3.0 >= $LIBEVINCE_REQUIRED evince-document-3.0 >= $LIBEVINCE_REQUIRED])


EVINCE_CFLAGS="$EVINCE_CFLAGS $DEBUG_FLAGS"
EVINCE_LIBS="$EVINCE_LIBS"
AC_SUBST(EVINCE_CFLAGS)
AC_SUBST(EVINCE_LIBS)

dnl
dnl zlib support
dnl

PKG_CHECK_MODULES([ZLIB], [zlib], [found_zlib=yes], [found_zlib=no])
AS_IF([test "x$found_zlib" = "xno"], [
  AC_CHECK_LIB(z, inflate, [AC_CHECK_HEADER(zlib.h, found_zlib=yes)])
  if test "x$found_zlib" = "xno" ; then
    AC_MSG_ERROR([*** Working zlib library and headers not found ***])
  fi
  ZLIB_LIBS='-lz'
  AC_SUBST(ZLIB_LIBS)
])

dnl
dnl SM client
dnl

PKG_CHECK_MODULES([SMCLIENT],[gtk+-3.0 gthread-2.0 sm >= 1.0.0])
AC_SUBST([SMCLIENT_CFLAGS])
AC_SUBST([SMCLIENT_LIBS])

# ***

# BACKEND_LIBTOOL_FLAGS="-module -avoid-version -no-undefined -export-symbols \$(top_srcdir)/backend/backend.symbols"
#AC_SUBST(BACKEND_LIBTOOL_FLAGS)

# ******************
# GKT+ Unix Printing
# ******************

AC_MSG_CHECKING([whether gtk+-unix-print support is requested])
AC_ARG_WITH([gtk-unix-print],
  [AS_HELP_STRING([--without-gtk-unix-print],
                  [Disable the use of gtk-unix-print])],
  [],[with_gtk_unix_print=yes])

AC_MSG_RESULT([$with_gtk_unix_print])

if test "$with_gtk_unix_print" = "yes"; then
   PKG_CHECK_MODULES(GTKUNIXPRINT, [gtk+-unix-print-3.0 >= $GTK_REQUIRED])
   AC_DEFINE([GTKUNIXPRINT_ENABLED], [1], [Define if gtk+-unix-print is enabled.])
fi

# *********************
# Keyring support
# *********************

AC_ARG_WITH(keyring,
        [AS_HELP_STRING([--without-keyring],
                        [Disable the use of libsecret and keyring])],
        [],
        [with_keyring=yes])

AM_CONDITIONAL([WITH_KEYRING],[test "$with_keyring" = "yes"])

if test "$with_keyring" = "yes"; then
        PKG_CHECK_MODULES(LIBSECRET, libsecret-1 >= $LIBSECRET_REQUIRED)
        AC_DEFINE([WITH_KEYRING],[1],[Define if KEYRING support is enabled])
fi

# ****
# DBUS
# ****

AC_ARG_ENABLE([dbus],
  [AS_HELP_STRING([--disable-dbus], [Disable support for dbus])],
  [],
  [enable_dbus=yes])

if test "$enable_dbus" = "yes"; then
  AC_DEFINE([ENABLE_DBUS],[1],[Define if DBUS support is enabled])

   PKG_CHECK_MODULES([EV_DAEMON], [gio-2.0 >= $GLIB_REQUIRED])
fi

AM_CONDITIONAL([ENABLE_DBUS], [test "$enable_dbus" = "yes"])

dnl Debug mode

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug],
                   [Turn on xreader debug mode]),
    [enable_debug=$enableval],
    [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
   DEBUG_FLAGS="-DEV_ENABLE_DEBUG"
fi

AC_ARG_ENABLE([tests],
    AS_HELP_STRING([--enable-tests], [Enable the tests]),
    [enable_tests=$enableval],
    [enable_tests=yes])

AM_CONDITIONAL(ENABLE_TESTS, test x$enable_tests = xyes)

SHELL_CFLAGS="$SHELL_CORE_CFLAGS $LIBSECRET_CFLAGS -DGDK_MULTIHEAD_SAFE -DGTK_MULTIHEAD_SAFE $DEBUG_FLAGS"
SHELL_LIBS="$SHELL_CORE_LIBS $LIBSECRET_LIBS -lz -lm"
AC_SUBST(SHELL_CFLAGS)
AC_SUBST(SHELL_LIBS)

FRONTEND_CFLAGS="$FRONTEND_CORE_CFLAGS $DEBUG_FLAGS"
FRONTEND_LIBS="$FRONTEND_CORE_LIBS -lz"
AC_SUBST(FRONTEND_CFLAGS)
AC_SUBST(FRONTEND_LIBS)

EV_DAEMON_CFLAGS="$EV_DAEMON_CFLAGS $DEBUG_FLAGS"
AC_SUBST([EV_DAEMON_CFLAGS])
AC_SUBST([EV_DAEMON_LIBS])

# Check for Caja property page build
AC_ARG_ENABLE([caja],
  [AS_HELP_STRING([--disable-caja],
                  [Disable build of caja extensions])],
  [],
  [enable_caja=yes])

if test "$enable_caja" = "yes" ; then
  PKG_CHECK_MODULES([CAJA],[$MM gthread-2.0 libcaja-extension >= 1.17.1],
                    [],[AC_MSG_ERROR([libcaja-extension not found; use --disable-caja to disable the caja extensions])])
  CAJA_EXTENSION_DIR=`$PKG_CONFIG --variable=extensiondir libcaja-extension`
  AC_SUBST([cajaextensiondir],[$CAJA_EXTENSION_DIR])
  AC_SUBST(CAJA_CFLAGS)
  AC_SUBST(CAJA_LIBS)

  AC_DEFINE([HAVE_CAJA],[1], [defined if you build the caja plugin])
fi

AM_CONDITIONAL([ENABLE_CAJA],[test "$enable_caja" = "yes"])

# Check for Nemo property page build
AC_ARG_ENABLE([nemo],
  [AS_HELP_STRING([--disable-nemo],
                  [Disable build of nemo extensions])],
  [],
  [enable_nemo=yes])

if test "$enable_nemo" = "yes" ; then
  PKG_CHECK_MODULES([NEMO],[$MM gthread-2.0 libnemo-extension],
                    [],[AC_MSG_ERROR([libnemo-extension not found; use --disable-nemo to disable the nemo extensions])])
  NEMO_EXTENSION_DIR=`$PKG_CONFIG --variable=extensiondir libnemo-extension`
  AC_SUBST([nemoextensiondir],[$NEMO_EXTENSION_DIR])
  AC_SUBST(NEMO_CFLAGS)
  AC_SUBST(NEMO_LIBS)

  AC_DEFINE([HAVE_NEMO],[1], [defined if you build the nemo plugin])
fi

AM_CONDITIONAL([ENABLE_NEMO],[test "$enable_nemo" = "yes"])

# Check for thumbnailer build

AC_ARG_ENABLE([thumbnailer],
  [AS_HELP_STRING([--disable-thumbnailer],
                  [Disable MATE thumbnailer])],
  [],
  [enable_thumbnailer=yes])

AM_CONDITIONAL([ENABLE_THUMBNAILER],[test "$enable_thumbnailer" = "yes"])

# ***************
# Print Previewer
# ***************

AC_ARG_ENABLE([previewer],
  [AS_HELP_STRING([--disable-previewer],
                  [Disable the MATE Document Previewer])],
  [],
  [enable_previewer=yes])

if test x$enable_previewer = "xyes" ; then
  PKG_CHECK_MODULES([PREVIEWER],[gtk+-3.0 >= $GTK_REQUIRED gthread-2.0 gio-2.0 >= $GLIB_REQUIRED])
fi

AM_CONDITIONAL([ENABLE_PREVIEWER],[test "$enable_previewer" = "yes"])
PREVIEWER_CFLAGS="$PREVIEWER_CFLAGS $GTKUNIXPRINT_CFLAGS $DEBUG_FLAGS"
PREVIEWER_LIBS="$PREVIEWER_LIBS $GTKUNIXPRINT_LIBS -lz"
AC_SUBST(PREVIEWER_CFLAGS)
AC_SUBST(PREVIEWER_LIBS)

# ***
# GIR
# ***

AC_MSG_CHECKING([whether GObject introspection is requested])
AC_ARG_ENABLE([introspection],
    AS_HELP_STRING([--enable-introspection],
                   [Enable GObject introspection]),
    [enable_introspection=$enableval],
    [enable_introspection=no])
AC_MSG_RESULT([$enable_introspection])

G_IR_SCANNER=
G_IR_COMPILER=
G_IR_GENERATE=
GIRDIR=
GIRTYPELIBDIR=

if test "$enable_introspection" = "yes"; then
  GOBJECT_INTROSPECTION_REQUIRED=0.6
  PKG_CHECK_MODULES([GOBJECT_INTROSPECTION],[gobject-introspection-1.0 >= $GOBJECT_INTROSPECTION_REQUIRED])

  G_IR_SCANNER="$($PKG_CONFIG --variable=g_ir_scanner gobject-introspection-1.0)"
  G_IR_COMPILER="$($PKG_CONFIG --variable=g_ir_compiler gobject-introspection-1.0)"
  G_IR_GENERATE="$($PKG_CONFIG --variable=g_ir_generate gobject-introspection-1.0)"
  GIRDIR="$($PKG_CONFIG --variable=girdir gobject-introspection-1.0)"
  GIRTYPELIBDIR="$($PKG_CONFIG --variable=typelibdir gobject-introspection-1.0)"
fi

AC_SUBST([G_IR_SCANNER])
AC_SUBST([G_IR_COMPILER])
AC_SUBST([G_IR_GENERATE])
AC_SUBST([GIRDIR])
AC_SUBST([GIRTYPELIBDIR])

AM_CONDITIONAL([ENABLE_INTROSPECTION],[test "$enable_introspection" = "yes"])

dnl ================== portability checks ===========================================

dnl for backtrace()
AC_CHECK_HEADERS([execinfo.h])

AC_CHECK_DECL([_NL_MEASUREMENT_MEASUREMENT],[
  AC_DEFINE([HAVE__NL_MEASUREMENT_MEASUREMENT],[1],[Define if _NL_MEASUREMENT_MEASUREMENT is available])
  ],[],[#include <langinfo.h>])

dnl =================== Mime types list ====================================================

if test "x$enable_pdf" = "xyes" ; then
    XREADER_MIME_TYPES="application/pdf;application/x-bzpdf;application/x-gzpdf;application/x-xzpdf;"
fi
if test "x$enable_ps" = "xyes" ; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}application/postscript;application/x-bzpostscript;application/x-gzpostscript;image/x-eps;image/x-bzeps;image/x-gzeps;"
fi
if test "x$enable_dvi" = "xyes"; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}application/x-dvi;application/x-bzdvi;application/x-gzdvi;"
fi
if test "x$enable_djvu" = "xyes"; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}image/vnd.djvu;image/vnd.djvu+multipage;"
fi
if test "x$enable_tiff" = "xyes"; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}image/tiff;"
fi
if test "x$enable_comics" = "xyes"; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}application/x-cbr;application/x-cbz;application/x-cb7;application/x-cbt;application/vnd.comicbook+zip;"
fi
if test "x$enable_pixbuf" = "xyes"; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}image/*;"
fi
if test "x$enable_xps" = "xyes"; then
    XREADER_MIME_TYPES="${XREADER_MIME_TYPES}application/oxps;application/vnd.ms-xpsdocument;"
fi
if test "x$have_webkit" = "xyes"; then
	XREADER_MIME_TYPES="${XREADER_MIME_TYPES}application/epub+zip;"
fi

AC_SUBST(XREADER_MIME_TYPES)

AC_CHECK_FUNC(localtime_r, AC_DEFINE(HAVE_LOCALTIME_R, 1, [Defines if localtime_r is available on your system]))

# *****************
# Help files
# *****************

YELP_HELP_INIT

# *****************
# API documentation
# *****************

GTK_DOC_CHECK([1.13], [--flavour no-tmpl])

AC_SUBST([GLIB_PREFIX], [$($PKG_CONFIG --variable=prefix glib-2.0)])
AC_SUBST([GTK_PREFIX], [$($PKG_CONFIG --variable=prefix gtk+-3.0)])

# ******************
# Backends directory
# ******************


# **********
# Versioning
# **********

AC_SUBST([EV_MAJOR_VERSION], [ev_major_version])
AC_SUBST([EV_MINOR_VERSION], [ev_minor_version])
AC_SUBST([EV_MICRO_VERSION], [ev_micro_version])

AC_SUBST([EV_API_VERSION], [ev_api_version])
AC_SUBST([EV_API_VERSION_U], [AS_TR_SH([ev_api_version])])
AC_SUBST([EV_BINARY_VERSION], [ev_binary_version])

#AC_SUBST([EV_DOCUMENT_LT_VERSION_INFO],[ev_document_lt_version_info])
#AC_SUBST([EV_DOCUMENT_LT_CURRENT_MINUS_AGE],[ev_document_lt_current_minus_age])
#AC_SUBST([EV_VIEW_LT_VERSION_INFO], [ev_view_lt_version_info])
#AC_SUBST([EV_VIEW_LT_CURRENT_MINUS_AGE], [ev_view_lt_current_minus_age])

# *****************************************************************************
# *****************************************************************************

AC_CONFIG_FILES([
cut-n-paste/Makefile
cut-n-paste/smclient/Makefile
cut-n-paste/zoom-control/Makefile
cut-n-paste/totem-screensaver/Makefile
cut-n-paste/synctex/Makefile
data/xreader.desktop.in
data/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/16x16/apps/Makefile
data/icons/16x16/actions/Makefile
data/icons/16x16/mimetypes/Makefile
data/icons/22x22/Makefile
data/icons/22x22/apps/Makefile
data/icons/22x22/actions/Makefile
data/icons/22x22/mimetypes/Makefile
data/icons/24x24/Makefile
data/icons/24x24/apps/Makefile
data/icons/24x24/actions/Makefile
data/icons/24x24/mimetypes/Makefile
data/icons/32x32/Makefile
data/icons/32x32/actions/Makefile
data/icons/32x32/mimetypes/Makefile
data/icons/48x48/Makefile
data/icons/48x48/apps/Makefile
data/icons/48x48/actions/Makefile
data/icons/scalable/Makefile
data/icons/scalable/apps/Makefile
data/icons/scalable/actions/Makefile
data/icons/scalable/mimetypes/Makefile
help/Makefile
help/reference/Makefile
help/reference/shell/Makefile
help/reference/shell/version.xml
libmisc/Makefile
Makefile
po/Makefile.in
previewer/Makefile
properties-caja/libxreader-properties-page.caja-extension.in
properties-caja/Makefile
properties-nemo/libxreader-properties-page.nemo-extension.in
properties-nemo/Makefile
shell/Makefile
test/Makefile
thumbnailer/Makefile
])

#AC_CONFIG_FILES(xreader-document-[]ev_api_version[].pc:xreader-document.pc.in)
#AC_CONFIG_FILES(xreader-view-[]ev_api_version[].pc:xreader-view.pc.in)

AC_OUTPUT

echo "
Configure summary:

    GTK+ Unix Print.....:    $with_gtk_unix_print
    Keyring Support.....:    $with_keyring
    DBUS Support........:    $enable_dbus
    Caja Plugin.........:    $enable_caja
    Nemo Plugin.........:    $enable_nemo
    Thumbnailer.........:    $enable_thumbnailer
    Previewer...........:    $enable_previewer
    Gtk-Doc Support.....:    $enable_gtk_doc
    Debug mode..........:    $enable_debug
    GObj. Introspection.:    $enable_introspection
    Tests...............:    $enable_tests

    PDF Backend.........:    $enable_pdf
    PostScript Backend..:    $enable_ps
    TIFF Backend........:    $enable_tiff
    DJVU Backend........:    $enable_djvu
    DVI Backend.........:    $enable_dvi
    Pixbuf Backend......:    $enable_pixbuf
    Comics Backend......:    $enable_comics
    XPS Backend.........:    $enable_xps
    ePub Backend........:    $have_webkit
"
