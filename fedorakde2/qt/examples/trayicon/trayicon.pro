TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= trayicon.h
SOURCES		= main.cpp \
		  trayicon.cpp \
		  trayicon_win.cpp
INTERFACES	=
TARGET		= trayicon
