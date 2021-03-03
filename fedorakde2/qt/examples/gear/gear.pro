TEMPLATE	= app
CONFIG		+= qt opengl warn_on release
HEADERS		= 
SOURCES		= gear.cpp
TARGET		= gear
DEPENDPATH	= ../include
unix:LIBS		+= -lm
REQUIRES=opengl
