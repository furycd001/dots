kde_module_LTLIBRARIES = libkcm_info.la

libkcm_info_la_SOURCES = main.cpp memory.cpp

libkcm_info_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkcm_info_la_LIBADD = $(LIBALIB) $(LIB_KDEUI) $(LIB_TRU64_MACH) $(LIBCFG) $(LIBODM) $(LIBKSTAT) $(LIBDEVINFO)

INCLUDES= $(all_includes)
 
libkcm_info_la_METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kcminfo.pot


data_DATA = memory.desktop processor.desktop dma.desktop \
	interrupts.desktop ioports.desktop pci.desktop sound.desktop \
	devices.desktop scsi.desktop partitions.desktop xserver.desktop

datadir = $(kde_appsdir)/Settings/Information

