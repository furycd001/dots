TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= mywidget.h
SOURCES		= main.cpp \
		  mywidget.cpp
TARGET		= i18n
DEPENDPATH=../../include
REQUIRES=full-config
