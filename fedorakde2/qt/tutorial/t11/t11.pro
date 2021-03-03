TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= cannon.h \
		  lcdrange.h
SOURCES		= cannon.cpp \
		  lcdrange.cpp \
		  main.cpp
unix:LIBS		+= -lm
TARGET		= t11
REQUIRES=full-config
