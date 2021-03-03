TEMPLATE	= app
CONFIG		+= qt opengl warn_on release
HEADERS		= gltexobj.h \
		  globjwin.h
SOURCES		= gltexobj.cpp \
		  globjwin.cpp \
		  main.cpp
TARGET		= texture
DEPENDPATH	= ../include
REQUIRES=opengl
