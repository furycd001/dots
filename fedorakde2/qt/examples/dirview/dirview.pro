TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= dirview.h
SOURCES		= dirview.cpp \
		  main.cpp
TARGET		= dirview
DEPENDPATH=../../include
REQUIRES=full-config
