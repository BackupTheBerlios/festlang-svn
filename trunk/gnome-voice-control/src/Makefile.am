libexec_PROGRAMS = voice_control_applet

voice_control_applet_SOURCES = \
	action.c \
	action.h \
	gstsphinxsink.c \
	gstsphinxsink.h \
	spi-listener.c \
	spi-listener.h \
	voice-control.c 

INCLUDES =					\
        -I$(top_srcdir) 					\
	-I$(top_builddir)					\
	-DAPPNAME_DATA_DIR=\"$(pkgdatadir)\"			\
	-DGNOMELOCALEDIR=\"$(prefix)/$(DATADIRNAME)/locale\"	\
	-DGNOMEDATADIR=\"$(datadir)\"				\
	-DPOCKETSPHINX_PREFIX=\"$(POCKETSPHINX_PREFIX)\"	\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(DISABLE_DEPRECATED)					\
	$(GNOME_PANEL_CFLAGS)					\
	$(GST_CFLAGS)						\
	$(POCKETSPHINX_CFLAGS)					\
	$(FR_CFLAGS)


voice_control_applet_LDADD = \
	$(GNOME_PANEL_LIBS)  \
	$(GST_LIBS)  \
	$(POCKETSPHINX_LIBS)




