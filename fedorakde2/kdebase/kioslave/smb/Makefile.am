## Makefile.am of kdebase/kioslave/man

INCLUDES=$(all_includes)
LDFLAGS =$(all_libraries) 
####### Files

#bin_PROGRAMS = kiosmb
#kiosmb_SOURCES = kio_smb.cpp \
#             kio_smb_auth.cpp \
#             kio_smb_cache.cpp \
#             kio_smb_browse.cpp \
#             kio_smb_config.cpp \
#             kio_smb_dir.cpp \
#             kio_smb_file.cpp \
#             kio_smb_internal.cpp \
#             main.cpp
#kiosmb_LDADD = -lkio -lsmbclient

kde_module_LTLIBRARIES = kio_smb.la
kio_smb_la_SOURCES = kio_smb.cpp \
             kio_smb_auth.cpp \
             kio_smb_cache.cpp \
             kio_smb_browse.cpp \
             kio_smb_config.cpp \
             kio_smb_dir.cpp \
             kio_smb_file.cpp \
             kio_smb_internal.cpp

kio_smb_la_LIBADD = -lkio -lsmbclient

kio_smb_la_LDFLAGS = -module -avoid-version -no-undefined

noinst_HEADERS = kio_smb.h kio_smb_internal.h

kdelnk_DATA = smb.protocol

kdelnkdir = $(kde_servicesdir)

dirtree_DATA = smb-network.desktop
dirtreedir = $(kde_datadir)/konqueror/dirtree/remote

METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kio_smb.pot
