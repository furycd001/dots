TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= cannon.h \
		  lcdrange.h
SOURCES		= cannon.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t8
REQUIRES=large-config
