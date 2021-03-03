TEMPLATE	= app
CONFIG		+= qt opengl warn_on release
HEADERS		= gearwidget.h \
		  rubberbandwidget.h
SOURCES		= gearwidget.cpp \
		  main.cpp \
		  rubberbandwidget.cpp
TARGET		= overlayrubber
DEPENDPATH	= ../include
REQUIRES=opengl full-config
