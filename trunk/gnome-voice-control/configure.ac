AC_INIT(configure.ac)
AM_INIT_AUTOMAKE(gnome-voice-control, 0.6)
AM_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE

AC_PROG_CC
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(GNOME_PANEL, libpanelapplet-2.0 gtk+-2.0 libgnomeui-2.0 gdk-2.0 libwnck-1.0 cspi-1.0 x11)
AC_SUBST(GNOME_PANEL_CFLAGS)
AC_SUBST(GNOME_PANEL_LIBS)

# Select what recognition engine you want to use.
# You can use "julius" or "sphinx".
RECOGNIZER="julius"
AC_SUBST(RECOGNIZER)

AM_CONDITIONAL(SPHINX, [test "$RECOGNIZER" = "sphinx"])

if test $RECOGNIZER = "sphinx" ; then
    GST_MAJORMINOR=0.10
    GST_REQUIRED=0.10.0
    PKG_CHECK_MODULES(EXTRA, gstreamer-base-$GST_MAJORMINOR gstreamer-$GST_MAJORMINOR >= $GST_REQUIRED)
    AC_SUBST(EXTRA_CFLAGS)
    AC_SUBST(EXTRA_LIBS)

    POCKETSPHINX_REQUIRED=0.5
    SPHINXBASE_REQUIRED=0.4
    PKG_CHECK_MODULES(ASR, sphinxbase >= $SPHINXBASE_REQUIRED pocketsphinx >= $POCKETSPHINX_REQUIRED)
    AC_SUBST(ASR_CFLAGS)
    AC_SUBST(ASR_LIBS)

    EXTRA_PREFIX="-DPOCKETSPHINX_PREFIX=\\\"`pkg-config --variable=prefix pocketsphinx`\\\""
    AC_SUBST(EXTRA_PREFIX)

    ASR_DATA="sphinx/desktop-control.fsg sphinx/desktop-control-en.dict sphinx/desktop-control-en.lm"
    AC_SUBST(ASR_DATA)
else #JULIUS
    CONF_THRESHOLD="0.35"
    AC_DEFINE_UNQUOTED([CONF_THRESHOLD],[$CONF_THRESHOLD],[Confidence threshold applied to execute each recognized action])

    AC_CHECK_LIB(dl, dlopen, [], [
	echo "dl library is required for this program"
        exit -1])
    AC_CHECK_LIB(pthread, pthread_exit, [], [
	echo "pthread library is required for this program"
        exit -1])
    AC_CHECK_LIB(esd, esd_open_sound, [], [
	echo "esd library is required for this program"
        exit -1])
    AC_CHECK_LIB(z, gzdopen, [], [
	echo "z library is required for this program"
        exit -1])
    AC_CHECK_LIB(m, pow10, [], [
	echo "m library is required for this program"
        exit -1])
    
    EXTRA_LIBS="-ldl -lpthread -lesd -lz -lm"
    AC_SUBST(EXTRA_LIBS)

    ASR_LIBS="-L../lib -ljulius -lsent "
    AC_SUBST(ASR_LIBS)

    ASR_CFLAGS="-I../include"
    AC_SUBST(ASR_CFLAGS)

    ASR_DATA="julius/desktop-control.jconf"
    AC_SUBST(ASR_DATA)
    
    ASRHMM_DATA="julius/hmm/hmm-es \
		 julius/hmm/hmm-en \
		 julius/hmm/tiedlist-en"
    AC_SUBST(ASRHMM_DATA)

    ASRLM_DATA="julius/lm/desktop-control-iw-es.dict \
		julius/lm/desktop-control-en.dict \
		julius/lm/desktop-control-en.dfa"
    AC_SUBST(ASRLM_DATA)
fi

WINDOWS_MODE="FALSE"
AC_DEFINE_UNQUOTED([WINDOWS_MODE],[$WINDOWS_MODE],[Enable windows mode (Debugger team)])
    
IT_PROG_INTLTOOL([0.35])

GNOME_DOC_INIT

GETTEXT_PACKAGE=gnome-voice-control
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT

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

