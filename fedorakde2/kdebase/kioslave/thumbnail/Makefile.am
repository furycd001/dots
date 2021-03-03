## $Id: Makefile.am,v 1.3 2001/03/31 12:14:19 malte Exp $
## Makefile.am of kdebase/kioslave/thumbnail

INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
METASOURCES = AUTO

kde_module_LTLIBRARIES = kio_thumbnail.la imagethumbnail.la \
	textthumbnail.la htmlthumbnail.la gsthumbnail.la

kio_thumbnail_la_SOURCES = thumbnail.cpp
kio_thumbnail_la_LIBADD = $(LIB_KSYCOCA)
kio_thumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

imagethumbnail_la_SOURCES = imagecreator.cpp
imagethumbnail_la_LIBADD = $(LIB_KSYCOCA) # for kimageio
imagethumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

textthumbnail_la_SOURCES = textcreator.cpp
textthumbnail_la_LIBADD = $(LIB_KDECORE)
textthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

htmlthumbnail_la_SOURCES = htmlcreator.cpp
htmlthumbnail_la_LIBADD = $(LIB_KHTML)
htmlthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

gsthumbnail_la_SOURCES = gscreator.cpp
gsthumbnail_la_LIBADD = $(LIB_KDECORE)
gsthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

noinst_HEADERS = thumbnail.h imagecreator.h textcreator.h htmlcreator.h \
	gscreator.h
include_HEADERS = thumbcreator.h

kdelnk_DATA = thumbnail.protocol
kdelnkdir = $(kde_servicesdir)

servicetypes_DATA = thumbcreator.desktop
servicetypesdir = $(kde_servicetypesdir)

services_DATA = imagethumbnail.desktop textthumbnail.desktop \
                htmlthumbnail.desktop gsthumbnail.desktop
servicesdir = $(kde_servicesdir)

