## Makefile.am of kdebase/kioslave/smb

INCLUDES= $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)

####### Files

kde_module_LTLIBRARIES = kio_smb.la

kio_smb_la_SOURCES = kio_smb.cpp my_process.cpp
kio_smb_la_LIBADD = -lkio -lkdesu
kio_smb_la_LDFLAGS = -module $(KDE_PLUGIN)
noinst_HEADERS = kio_smb.h my_process.h

kdelnk_DATA = smb.protocol
kdelnkdir = $(kde_servicesdir)

data_DATA = smb.html
datadir = $(kde_datadir)/kioslave

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_smbro.pot

