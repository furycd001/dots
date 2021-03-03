TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= motor.h \
		  vw.h
SOURCES		= main.cpp \
		  motor.cpp \
		  vw.cpp
TARGET		= validator
DEPENDPATH=../../include
REQUIRES=medium-config
