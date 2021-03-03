TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= aclock.h
SOURCES		= aclock.cpp \
		  main.cpp
TARGET		= aclock
DEPENDPATH=../../include
REQUIRES=full-config
