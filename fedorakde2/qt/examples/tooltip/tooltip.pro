TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= tooltip.h
SOURCES		= main.cpp \
		  tooltip.cpp
TARGET		= tooltip
DEPENDPATH=../../include
REQUIRES=large-config
