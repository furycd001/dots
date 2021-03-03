TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= listbox.h
SOURCES		= listbox.cpp \
		  main.cpp
TARGET		= listbox
DEPENDPATH	= ../../include 
REQUIRES=medium-config
