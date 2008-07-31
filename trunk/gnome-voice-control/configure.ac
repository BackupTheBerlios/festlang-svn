AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(gnome-voice-control, 0.3)
AM_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(GNOME_PANEL, libpanelapplet-2.0 gtk+-2.0 libgnomeui-2.0 gstreamer-0.10 gstreamer-base-0.10 gdk-2.0 libwnck-1.0 cspi-1.0 libnotify)
AC_SUBST(GNOME_PANEL_CFLAGS)
AC_SUBST(GNOME_PANEL_LIBS)

GST_MAJORMINOR=0.10
GST_REQUIRED=0.10.0
PKG_CHECK_MODULES(GST, gstreamer-$GST_MAJORMINOR >= $GST_REQUIRED)
AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_LIBS)

POCKETSPHINX_REQUIRED=0.5
SPHINXBASE_REQUIRED=0.4
PKG_CHECK_MODULES(POCKETSPHINX, sphinxbase >= $SPHINXBASE_REQUIRED pocketsphinx >= $POCKETSPHINX_REQUIRED)
AC_SUBST(POCKETSPHINX_CFLAGS)
AC_SUBST(POCKETSPHINX_LIBS)

IT_PROG_INTLTOOL([0.35])

GNOME_DOC_INIT

GETTEXT_PACKAGE=gnome-voice-control
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

POCKETSPHINX_PREFIX=`pkg-config --variable=prefix pocketsphinx`
AC_SUBST(POCKETSPHINX_PREFIX)

AC_ARG_ENABLE(debug,
              [AC_HELP_STRING([--enable-debug-dump], [Enable input debugging])],enable_debug="$enableval",enable_debug=no)
if test "x$enable_debug" = "xyes"; then
	    AC_DEFINE([ENABLE_DEBUG_DUMP], [1], [Enable debug dump.])
fi

AC_OUTPUT([
src/Makefile
data/Makefile
lang/Makefile
help/Makefile
po/Makefile.in
Makefile
])

