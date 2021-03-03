TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= richtext.h
SOURCES		= main.cpp \
		  richtext.cpp
TARGET		= richtext
DEPENDPATH=../../include
REQUIRES=large-config
