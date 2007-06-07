AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(gnome-voice-control, 0.1)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(GNOME_PANEL, libpanelapplet-2.0 gtk+-2.0 libgnomeui-2.0 gstreamer-0.10 gstreamer-base-0.10 gdk-2.0 libwnck-1.0)
AC_SUBST(GNOME_PANEL_CFLAGS)
AC_SUBST(GNOME_PANEL_LIBS)

IT_PROG_INTLTOOL([0.35])

GNOME_DOC_INIT

GETTEXT_PACKAGE=gnome-voice-control
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

AC_ARG_WITH(sphinx2,
	AS_HELP_STRING([--with-sphinx2=DIRECTORY],
			[look for Sphinx2 source files in DIRECTORY]),
			SPHINX2_PREFIX=$withval, SPHINX2_PREFIX="/usr/local")
AC_SUBST(SPHINX2_PREFIX)

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
lang/Makefile
help/Makefile
po/Makefile.in
])

