INCLUDES =							\
	-DGNOMELOCALEDIR=\"$(prefix)/$(DATADIRNAME)/locale\"	\
	-DGNOMEDATADIR=\"$(datadir)\"				\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(GNOME_PANEL_CFLAGS)


libexec_PROGRAMS = voice_control_applet

voice_control_applet_SOURCES = voice-control.c

voice_control_applet_LDADD = \
	$(GNOME_PANEL_LIBS)

