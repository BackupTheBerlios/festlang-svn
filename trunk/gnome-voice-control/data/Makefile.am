uidir 	= $(datadir)/gnome-2.0/ui
ui_DATA	= GNOME_VoiceControlApplet.xml	

serverdir       = $(libdir)/bonobo/servers
server_in_files = GNOME_VoiceControlApplet_Factory.server.in
server_DATA     = $(server_in_files:.server.in=.server)

APPLET_LOCATION=$(libexecdir)/voice_control_applet
APPLET_TYPE=exe

$(server_in_files): $(server_in_files).in Makefile
	sed -e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
	    -e "s|\@TYPE\@|$(APPLET_TYPE)|" \
	    -e "s|\@VERSION\@|$(PACKAGE_VERSION)|" \
	    $< > $@

@INTLTOOL_SERVER_RULE@

EXTRA_DIST =						\
	GNOME_VoiceControlApplet_Factory.server.in.in	\
	$(ui_DATA)

CLEANFILES = $(server_in_files) $(server_DATA)
