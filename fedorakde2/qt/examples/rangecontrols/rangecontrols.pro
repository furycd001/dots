TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= rangecontrols.h
SOURCES		= main.cpp \
		  rangecontrols.cpp
TARGET		= rangecontrols
DEPENDPATH=../../include
REQUIRES=large-config
