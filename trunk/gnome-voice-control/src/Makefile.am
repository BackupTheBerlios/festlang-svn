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
	voice-control.c \
	action.c \
	action.h \
	gstsphinxsink.c \
	gstsphinxsink.h

voice_control_applet_LDADD = \
	$(GNOME_PANEL_LIBS)  \
	-L$(SPHINX2_PREFIX)/lib \
	-lsphinx2 \
	-lsphinxfe



