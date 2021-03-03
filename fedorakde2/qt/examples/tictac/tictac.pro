TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= tictac.h
SOURCES		= main.cpp \
		  tictac.cpp
TARGET		= tictac
DEPENDPATH=../../include
REQUIRES=medium-config
