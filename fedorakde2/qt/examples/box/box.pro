TEMPLATE	= app
CONFIG		+= qt opengl warn_on release
HEADERS		= glbox.h \
		  globjwin.h
SOURCES		= glbox.cpp \
		  globjwin.cpp \
		  main.cpp
TARGET		= box
DEPENDPATH	= ../include
REQUIRES=opengl
