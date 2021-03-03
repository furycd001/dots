TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS	= mydialogimpl.h
SOURCES	= main.cpp mydialogimpl.cpp
INTERFACES	= mydialog.ui
unix:TMAKE_UIC	= $(QTDIR)/bin/uic
win32:TMAKE_UIC	= $(QTDIR)\bin\uic.exe
TARGET		= mydialog
