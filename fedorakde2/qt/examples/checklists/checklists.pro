TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= checklists.h
SOURCES		= checklists.cpp \
		  main.cpp
TARGET		= checklists
DEPENDPATH=../../include
REQUIRES=large-config
