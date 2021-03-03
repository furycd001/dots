TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= application.h 
SOURCES		= application.cpp \
		  main.cpp 
TARGET		= mdi
DEPENDPATH=../../include
REQUIRES=workspace full-config
