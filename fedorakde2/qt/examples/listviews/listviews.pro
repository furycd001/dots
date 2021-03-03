TEMPLATE	= app
CONFIG		+= qt warn_on release
HEADERS		= listviews.h
SOURCES		= listviews.cpp \
		  main.cpp
TARGET		= listviews
DEPENDPATH=../../include
REQUIRES=large-config
