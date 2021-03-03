TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= qwerty.h
SOURCES		= main.cpp \
		  qwerty.cpp
TARGET		= qwerty
DEPENDPATH=../../include
REQUIRES=large-config
