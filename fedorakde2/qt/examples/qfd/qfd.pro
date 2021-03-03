TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= fontdisplayer.h
SOURCES		= fontdisplayer.cpp \
		  qfd.cpp
TARGET		= qfd
DEPENDPATH=../../include
REQUIRES=large-config
