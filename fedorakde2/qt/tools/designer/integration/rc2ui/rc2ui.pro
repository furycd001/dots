TEMPLATE	= app
CONFIG		+= qt warn_on release console
HEADERS		= rc2ui.h
SOURCES		= rc2ui.cpp \
		  main.cpp
TARGET		= rc2ui
DESTDIR = $(QTDIR)/bin
