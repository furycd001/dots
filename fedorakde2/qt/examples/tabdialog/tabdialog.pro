TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= tabdialog.h
SOURCES		= main.cpp \
		  tabdialog.cpp
TARGET		= tabdialog
DEPENDPATH=../../include
REQUIRES=large-config
