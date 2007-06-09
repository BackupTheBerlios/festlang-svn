INCLUDES =							\
	-DGNOMELOCALEDIR=\"$(prefix)/$(DATADIRNAME)/locale\"	\
	-DGNOMEDATADIR=\"$(datadir)\"				\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	-I$(SPHINX2_PREFIX)/include/sphinx2			\
	-DSPHINX2_PREFIX=\"$(SPHINX2_PREFIX)\"			\
	$(GNOME_PANEL_CFLAGS)


libexec_PROGRAMS = voice_control_applet

voice_control_applet_SOURCES = \
	action.c \
	action.h \
	gstsphinxsink.c \
	gstsphinxsink.h \
	spi-listener.c \
	spi-listener.h \
	voice-control.c 

voice_control_applet_LDADD = \
	$(GNOME_PANEL_LIBS)  \
	-L$(SPHINX2_PREFIX)/lib \
	-lsphinx2 \
	-lsphinx2fe \
	-lsphinx2ad



