SUBDIRS = $(KCM_AUDIOCD)
INCLUDES = $(all_includes) $(CDPARANOIA_INCS) $(LAME_INCS) $(VORBIS_INCS)

kde_module_LTLIBRARIES = kio_audiocd.la

kio_audiocd_la_SOURCES = cddb.cpp audiocd.cpp

kio_audiocd_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(CDPARANOIA_RPATH) \
 $(LAME_RPATH) $(VORBIS_RPATH) -module $(KDE_PLUGIN)

kio_audiocd_la_LIBADD = $(LIB_KIO) $(CDPARANOIA_LIBS) $(LAME_LIBS) $(VORBISENC_LIBS) $(VORBIS_LIBS)

noinst_HEADERS = audiocd.h cddb.h

protocoldir = $(kde_servicesdir)
protocol_DATA = audiocd.protocol

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_audiocd.pot
