libexec_PROGRAMS = voice_control_applet

if SPHINX
voice_control_applet_SOURCES =  \
	action.c		\
	action.h		\
	asrsphinx.c		\
	gstsphinxsink.c		\
	gstsphinxsink.h		\
	spi-listener.c		\
	spi-listener.h		\
	voice-control.c		\
	voice-control.h		\
	asr.h
else
voice_control_applet_SOURCES =  \
	action.c		\
	action.h		\
	asrjulius.c		\
	spi-listener.c		\
	spi-listener.h		\
	voice-control.c		\
	voice-control.h		\
	asr.h
endif

INCLUDES =							\
        -I$(top_srcdir) 					\
	-I$(top_builddir)					\
	-DAPPNAME_DATA_DIR=\"$(pkgdatadir)\"			\
	-DGNOMELOCALEDIR=\"$(prefix)/$(DATADIRNAME)/locale\"	\
	-DGNOMEDATADIR=\"$(datadir)\"				\
	$(EXTRA_PREFIX)						\
	$(WARN_CFLAGS)						\
	$(DISABLE_DEPRECATED_CFLAGS)				\
	$(DISABLE_DEPRECATED)					\
	$(GNOME_PANEL_CFLAGS)					\
	$(EXTRA_CFLAGS)						\
	$(ASR_CFLAGS)						
	


voice_control_applet_LDADD =	\
	$(GNOME_PANEL_LIBS)	\
	$(ASR_LIBS)		\
	$(EXTRA_LIBS)



