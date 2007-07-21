libexec_PROGRAMS = voice_control_applet

voice_control_applet_SOURCES = \
	action.c \
	action.h \
	gstsphinxsink.c \
	gstsphinxsink.h \
	spi-listener.c \
	spi-listener.h \
	voice-control.c 

voice_control_applet_CFLAGS =					\
	-DGNOMELOCALEDIR=\"$(prefix)/$(DATADIRNAME)/locale\"	\
	-DGNOMEDATADIR=\"$(datadir)\"				\
	-DPOCKETSPHINX_PREFIX=\"$(POCKETSPHINX_PREFIX)\"	\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(GNOME_PANEL_CFLAGS)					\
	$(GST_CFLAGS)						\
	$(POCKETSPHINX_CFLAGS)

voice_control_applet_LDADD = \
	$(GNOME_PANEL_LIBS)  \
	$(GST_LIBS)  \
	$(POCKETSPHINX_LIBS)




