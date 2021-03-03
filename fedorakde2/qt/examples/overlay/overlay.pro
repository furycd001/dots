TEMPLATE	= app
CONFIG		+= qt opengl warn_on release
HEADERS		= glteapots.h \
		  globjwin.h
SOURCES		= glteapots.cpp \
		  globjwin.cpp \
		  main.cpp
TARGET		= overlay
DEPENDPATH	= ../include
REQUIRES=opengl
