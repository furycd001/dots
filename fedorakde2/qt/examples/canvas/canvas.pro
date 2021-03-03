TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= canvas.h
SOURCES		= canvas.cpp main.cpp
TARGET		= canvas
DEPENDPATH	= ../../include
REQUIRES=canvas large-config
